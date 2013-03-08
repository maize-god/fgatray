#include "fgadvice.h"
#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTextDocument>
#include "defaults.h"

FGAdvice::FGAdvice(QObject *parent) :
    QObject(parent),
    m_netRequest(QUrl(FGA_URI)),
    m_activeReply(0),
    m_getAudio(false),
    m_state(Initial)
{
    m_netAccMgr = new QNetworkAccessManager(this);
    m_respData.clear();
}

bool FGAdvice::get(bool withAudio)
{
    if(m_state > Idle)
        return false;

    m_getAudio = withAudio;

    m_buffer.clear();

    m_state = GetText;
    m_activeReply = m_netAccMgr->get(m_netRequest);

    connect(m_activeReply, SIGNAL(readyRead()), SLOT(onDataReady()));
    connect(m_activeReply, SIGNAL(finished()), SLOT(onRequestFinished()));

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
        m_buffer.reserve(m_activeReply->header(QNetworkRequest::ContentLengthHeader).toInt());
        m_state = GetText;
        // break through
    case GetText:
    {
        int i = m_buffer.size();
        int sz = (int) m_activeReply->bytesAvailable();
        m_buffer.resize(i + sz);
        m_activeReply->read(m_buffer.data() + i, sz);
    }
    case StartGetSound:
        // TODO get sound
        break;
    case GetSound:
        // TODO get sound
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
        m_respData.clear();
        if(!m_getAudio) {
            m_state = Idle;
        }

        if(m_activeReply->error() == QNetworkReply::NoError) {
            _interpretResponse();
        } else {
            m_state = Error;
            m_respData.error = m_activeReply->errorString();
        }

        if(m_state != GetText) {
            emit got(m_state != Error);
        } else {
            m_state = StartGetSound;
            // TODO get audio
        }

        m_activeReply->close();
        m_activeReply->deleteLater();
        m_activeReply = 0;
    }
        break;
    case GetSound:
        // TODO get audio finished
        break;
    default:
        // TODO something more sensible?
        break;
    }
}

void FGAdvice::_interpretResponse()
{
    m_respData.clear();

    if(!m_buffer.isEmpty()) {
        QJsonParseError pe;
        QJsonDocument doc = QJsonDocument::fromJson(m_buffer, &pe);

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
