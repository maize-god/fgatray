#ifndef FGADVICE_H
#define FGADVICE_H

#include <QObject>
#include <QDir>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class QNetworkReply;

class FGAdvice : public QObject
{
    Q_OBJECT
public:
    enum State {
        Error = -100,
        Initial = -1,
        Idle = 0,
        StartGetText = 1,
        GetText = 2,
        StartGetSound = 3,
        GetSound = 4
    };

    enum ReplyState {
        ReplyError = 0,
        ReplyText = 1,
        ReplySound = 2
    };

private:
    QNetworkAccessManager* m_netAccMgr;
    QNetworkRequest m_netRequest;

    bool m_getAudio;
    State m_state;

    QDir m_cacheDir;

    struct _StateData {
        QByteArray buffer;
        QFile* cacheFile;
        QNetworkReply* activeReply;

        void clearCacheFile() {
            if(cacheFile != 0) {
                cacheFile->close();
                cacheFile->deleteLater();
                cacheFile = 0;
            }
        }

        void clearActiveReply() {
            if(activeReply != 0) {
                activeReply->close();
                activeReply->deleteLater();
                activeReply = 0;
            }
        }

        void clear() {
            clearCacheFile();
            clearActiveReply();
            buffer.clear();
        }

        void restart(QNetworkReply* newReply) {
            clear();
            activeReply = newReply;
        }

        _StateData() : cacheFile(0), activeReply(0) {}
    } m_stateData;

    struct _RespData {
        QString text;
        int id;
        QString soundFile;
        QString error;

        void clear() {
            text = QString();
            id = 0;
            soundFile = QString();
            error = QString();
        }

        _RespData() {
            clear();
        }
    } m_respData;

    void _interpretResponse();

public:
    explicit FGAdvice(QObject *parent = 0);

    bool get(bool withAudio = false);
    bool setProxy(
            const QString& host,
            int port,
            const QString& user = QString(),
            const QString& password = QString());

    const QString& text() const {
        return m_respData.text;
    }

    const QString& errorText() const {
        return m_respData.error;
    }

    QString soundFilePath() const {
        if(m_respData.soundFile.isEmpty())
            return QString();
        else
            return m_cacheDir.absoluteFilePath(m_respData.soundFile);
    }
    
signals:
    void got(int replyState);

private slots:
    void onDataReady();
    void onRequestFinished();
    
public slots:
    
};

#endif // FGADVICE_H
