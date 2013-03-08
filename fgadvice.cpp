#include "fgadvice.h"
#include <QtNetwork/QHttpRequestHeader>
#include "defaults.h"

FGAdvice::FGAdvice(QObject *parent) :
    QObject(parent), m_reqHeader("GET", FGA_API_PATH),
    m_getAudio(false), m_state(Initial)
{
    m_http.setHost(FGA_HOST);
}

bool FGAdvice::get(bool withAudio)
{
    if(m_state > Idle)
        return false;

    m_getAudio = withAudio;

    m_state = GetText;
    m_http.request(m_reqHeader);

    return true;
}


void FGAdvice::onResponseHeader(const QHttpResponseHeader &resp)
{
    // TODO HERE
}
