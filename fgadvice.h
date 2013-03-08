#ifndef FGADVICE_H
#define FGADVICE_H

#include <QObject>
#include <QtNetwork/QHttp>
#include <QtNetwork/QHttpResponseHeader>

class FGAdvice : public QObject
{
    Q_OBJECT
public:
    enum State {
        Initial = -1,
        Idle = 0,
        GetText = 1,
        GetSound = 2
    };

private:
    QHttp m_http;
    QHttpRequestHeader m_reqHeader;
    QByteArray m_buffer;

    bool m_getAudio;
    State m_state;

public:
    explicit FGAdvice(QObject *parent = 0);

    bool get(bool withAudio = false);
    
signals:
    void got(bool success);

private slots:
    void onResponseHeader(const QHttpResponseHeader& resp);
    
public slots:
    
};

#endif // FGADVICE_H
