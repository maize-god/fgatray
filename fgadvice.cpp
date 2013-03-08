#include "fgadvice.h"
#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTextDocument>
#include <QStandardPaths>
#include "defaults.h"

FGAdvice::FGAdvice(QObject *parent) :
    QObject(parent),
    m_netRequest(QUrl(FGA_URI)),
    m_getAudio(false),
    m_state(Initial)
{
    m_netAccMgr = new QNetworkAccessManager(this);
    m_respData.clear();
    m_cacheDir.setPath(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    if(!m_cacheDir.exists()) {
        m_cacheDir.mkpath(m_cacheDir.absolutePath());
    }
}

bool FGAdvice::get(bool withAudio)
{
    if(m_state > Idle)
        return false;

    m_getAudio = withAudio;

    m_state = GetText;
    m_stateData.restart(m_netAccMgr->get(m_netRequest));

    connect(m_stateData.activeReply, SIGNAL(readyRead()), SLOT(onDataReady()));
    connect(m_stateData.activeReply, SIGNAL(finished()), SLOT(onRequestFinished()));

    return true;
}

bool FGAdvice::setProxy(
        const QString &host, int port,
        const QString &user, const QString &password)
{
    if(m_state > Idle)
        return false;

    QNetworkProxy proxy(
                QNetworkProxy::HttpProxy, host, port, user, password);
    m_netAccMgr->setProxy(proxy);

    return true;
}

void FGAdvice::onDataReady()
{
    switch(m_state) {
    case StartGetText:
        m_stateData.buffer.reserve(
                    m_stateData.activeReply->header(
                        QNetworkRequest::ContentLengthHeader).toInt());
        m_state = GetText;
        // break through
    case GetText:
    {
        int i = m_stateData.buffer.size();
        int sz = (int) m_stateData.activeReply->bytesAvailable();
        m_stateData.buffer.resize(i + sz);
        m_stateData.activeReply->read(m_stateData.buffer.data() + i, sz);
    }
        break;
    case StartGetSound:
    {
        QString fp = soundFilePath();
        m_stateData.cacheFile = new QFile(fp);
    }
        if(!m_stateData.cacheFile->open(QIODevice::WriteOnly)) {
            m_respData.error = m_stateData.cacheFile->errorString();
            m_stateData.clear();
            break;
        } else {
            m_state = GetSound;
        }
    case GetSound:
    {
        int sz = (int) m_stateData.activeReply->bytesAvailable();
        QByteArray d = m_stateData.activeReply->read(sz);
        if(!d.isEmpty()) {
            m_stateData.cacheFile->write(d);
        }
    }
        break;
    default:
        break;
    }
}

void FGAdvice::onRequestFinished()
{
    switch(m_state) {
    case GetText:
    {
        bool clearState = true;
        m_respData.clear();
        if(!m_getAudio) {
            m_state = Idle;
        }

        if(m_stateData.activeReply->error() == QNetworkReply::NoError) {
            _interpretResponse();
        } else {
            m_state = Error;
            m_respData.error = m_stateData.activeReply->errorString();
        }

        if(m_state != GetText) {
            emit got(m_state == Error ? ReplyError : ReplyText);
        } else {
            QFile cfl(soundFilePath());
            if(cfl.isReadable()) {
                m_state = Idle;
                emit got(ReplySound);
            } else {
                m_state = StartGetSound;
                QString u = QString(FGA_SOUND_URI).arg(m_respData.soundFile);
                QUrl url = QString(u);
                QNetworkRequest nrAudio(url);
                m_stateData.restart(m_netAccMgr->get(nrAudio));

                connect(m_stateData.activeReply, SIGNAL(readyRead()), SLOT(onDataReady()));
                connect(m_stateData.activeReply, SIGNAL(finished()), SLOT(onRequestFinished()));

                clearState = false;
            }
        }

        if(clearState)
            m_stateData.clear();
    }
        break;
    case GetSound:
    {
        if(m_stateData.activeReply->error() != QNetworkReply::NoError) {
            m_state = Error;
            m_respData.error = m_stateData.activeReply->errorString();
            emit got(ReplyError);
        } else {
            m_state = Idle;
            emit got(ReplySound);
        }

        m_stateData.clear();
    }
        break;
    case Error:
        emit got(ReplyError);
        break;
    default:
        m_state = Error;
        m_stateData.clear();
        break;
    }
}

void FGAdvice::_interpretResponse()
{
    m_respData.clear();

    if(!m_stateData.buffer.isEmpty()) {
        QJsonParseError pe;
        QJsonDocument doc = QJsonDocument::fromJson(m_stateData.buffer, &pe);

        if(doc.isObject()) {
            QJsonObject o = doc.object();
            {
                QJsonValue _id = o.value("id");
                if(_id.isDouble())
                    m_respData.id = (int) _id.toDouble();
                else if(_id.isString())
                    m_respData.id = _id.toString().toInt();
            }
            {
                QString _text = o.value("text").toString();
                if(!_text.isEmpty()) {
                    QTextDocument td;
                    td.setHtml(_text);
                    m_respData.text = td.toPlainText();
                }
            }
            m_respData.soundFile = o.value("sound").toString();
        } else if(pe.error != QJsonParseError::NoError) {
            m_respData.error = pe.errorString();
        }
    }

    if(m_respData.id == 0 || m_respData.text.isEmpty()) {
        m_state = Error;
        if(m_respData.error.isEmpty())
            m_respData.error = "Invalid response";
    } else if(m_respData.soundFile.isEmpty()) {
        m_state = Idle;
    }
}
