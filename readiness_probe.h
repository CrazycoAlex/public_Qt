//Интерфейс взаимодействия "пробы готовности"
#ifndef READINESS_PROBE_H
#define READINESS_PROBE_H


#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QString>


class TReadinessProbe : public QObject
{
    Q_OBJECT

  public:
    explicit TReadinessProbe(QObject *parent = 0);
    ~TReadinessProbe();
    //
    bool getStatus();

  Q_SIGNALS:
    //Вывод в лог работы
    void toLog(const QString &text);
    //Ошибка в процессе
    void setErrorCode(int codeError);
    //Смена состояния
    void changedStatus(bool statusIsReady);

  public Q_SLOTS:
    //Состояние - сервис запущен
    void setReady();
    //Состояние - сервис остановлен
    void setNotReady();
    //Установка параметров слота
    void setParamsTCP(const QString &serverAddr, int serverPort);
    //Установка параметров "пробы готовности"
    void setProbeParams(int probeType, QString probeName, QString probeAddr, QString probePort, int interval, int timeout);

  private:
    const int cDefaultWebSocketPort = 8088;
    const QByteArray cDefaultServerName = "RRNS_KS";
    const QByteArray cDefaultTCPSocketAddr = "localhost";
    const int cDefaultTCPSocketPort = 8089;
    //Состояние:
    // true  - сервис запущен
    // false - сервис остановлен
    bool isReady;
    int TCPSocketPort;
    QString TCPSocketAddr;

    //Сокет TCP
    QTcpSocket* probeTCP;

    void openSocketTCP();

    void closeSocketTCP();
};
#endif // READINESS_PROBE_H
