//Интерфейс взаимодействия "пробы готовности"
#include "readiness_probe.h"


TReadinessProbe::TReadinessProbe(QObject *parent) : QObject(parent)
{
    isReady = false;

    probeTCP = nullptr;

    TCPSocketAddr = QString(cDefaultTCPSocketAddr);
    TCPSocketPort = cDefaultTCPSocketPort;
}


TReadinessProbe::~TReadinessProbe()
{
    if(probeTCP != nullptr)
    {
        closeSocketTCP();
        delete probeTCP;
        probeTCP = nullptr;
    }

    if(isReady)
    {
        isReady = false;
        emit changedStatus(isReady);
    }
}


bool TReadinessProbe::getStatus()
{
    return isReady;
}

//
void TReadinessProbe::setReady()
{
    if(!isReady) openSocketTCP();
    else if(isReady&&(!probeTCP->isOpen())) openSocketTCP();
}

//
void TReadinessProbe::setNotReady()
{
    if(isReady) closeSocketTCP();
    else if(probeTCP != nullptr)
    {
        if((probeTCP->state() != QAbstractSocket::SocketState::UnconnectedState)&&(probeTCP->state() != QAbstractSocket::SocketState::ClosingState))
        {
            closeSocketTCP();
        }
    }
    else isReady = false;
}


void TReadinessProbe::setParamsTCP(const QString &serverAddr, int serverPort)
{
    TCPSocketAddr = serverAddr;
    TCPSocketPort = serverPort;

    setReady();
}

//
void TReadinessProbe::setProbeParams(int probeType, QString probeName, QString probeAddr, QString probePort, int interval, int timeout)
{
    Q_UNUSED(probeType);
    Q_UNUSED(probeName);

    TCPSocketAddr = probeAddr;
    TCPSocketPort = probePort.toInt();

    Q_UNUSED(interval);
    Q_UNUSED(timeout);

    setReady();
}

//
void TReadinessProbe::openSocketTCP()
{
    if(probeChannel == nullptr) probeChannel = new QWebChannel(this);
    if(probeTCP == nullptr) probeTCP = new QTcpSocket(this);

    if(probeTCP->bind(QHostAddress(TCPSocketAddr), TCPSocketPort, QAbstractSocket::ShareAddress))
    {
        if(!isReady)
        {
            isReady = true;
            emit changedStatus(isReady);
        }

        emit toLog(QString("Open TCP-socket on %1 and port %2").arg(probeTCP->localAddress().toString()).arg(probeTCP->localPort()));
    }
    else
    {
        if(isReady)
        {
            isReady = false;
            emit changedStatus(isReady);
        }
        emit setErrorCode(1000*probeTCP->error());
        emit toLog(QString("NOT open TCP-socket on %1 and port %2").arg(probeTCP->localAddress().toString()).arg(probeTCP->localPort()));
    }
}

//
void TReadinessProbe::closeSocketTCP()
{
    if(probeTCP != nullptr)
    {
        int tempPort = probeTCP->localPort();
        QString tempAddr = probeTCP->localAddress().toString();

        probeTCP->disconnectFromHost();
        probeTCP->close();

        if(isReady)
        {
            isReady = false;
            emit changedStatus(isReady);
        }

        emit toLog(QString("closed TCP-socket on %1 and port %2").arg(tempAddr).arg(tempPort));
    }
}
