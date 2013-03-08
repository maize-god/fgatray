#include "fgadvice.h"
#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QNetworkReply>
#include "defaults.h"

FGAdvice::FGAdvice(QObject *parent) :
    QObject(parent), m_netRequest(QUrl(FGA_URI)),
    m_getAudio(false), m_state(Initial),
    m_activeReply(0)
{
    m_netAccMgr = new QNetworkAccessManager(this);
}

bool FGAdvice::get(bool withAudio)
{
    if(m_state > Idle)
        return false;

    m_getAudio = withAudio;

    m_state = GetText;
    m_activeReply = m_netAccMgr->get(m_netRequest);

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
        m_buffer.reserve(m_activeReply->header(QNetworkRequest::ContentLengthHeader));
        m_buffer.resize(0);
        m_state = GetText;
        // break through
    case GetText:
    {
        int i = m_buffer.size();
        int sz = (int) m_activeReply->bytesAvailable();
        m_buffer.resize(i + sz);
        m_activeReply->read(m_buffer.data() + i, sz);
    }
        break;
    }
}
