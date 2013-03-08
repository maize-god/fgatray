#ifndef FGADVICE_H
#define FGADVICE_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>

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

private:
    QNetworkAccessManager* m_netAccMgr;
    QNetworkRequest m_netRequest;
    QNetworkReply* m_activeReply;

    QByteArray m_buffer;

    bool m_getAudio;
    State m_state;

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

    const QString& text() {
        return m_respData.text;
    }

    const QString& errorText() {
        return m_respData.error;
    }
    
signals:
    void got(bool success);

private slots:
    void onDataReady();
    void onRequestFinished();
    
public slots:
    
};

#endif // FGADVICE_H
