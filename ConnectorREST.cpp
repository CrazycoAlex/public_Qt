//----------------------------------------------------------------------------//
//                                                                            //
// Файл ConnectorREST.cpp                                                     //
//                                                                            //
//                *** TConnectorREST КЛАСС ПОЛУЧЕНИЯ ДАННЫХ ***               //
//                     *** ЧЕРЕЗ REST ИНТЕРФЕЙС СЕРВЕРА ***                   //
//                                                                            //
//                                                                            //
//                                                           Москва, 2022 год //
//----------------------------------------------------------------------------//


#include "ConnectorREST.h"


TConnectorREST::TConnectorREST(QObject *parent) : QObject(parent)
{
    codeError = 0;
    gettingScenarios = false;
    codeStatus = -1;
    codeStatusGIS = -1;
    timeoutServerREST = 60;
    intervalServerREST = 600;
    ConnectionNameDB = QString("");

    Requester = nullptr;
}


TConnectorREST::~TConnectorREST()
{
    stop();

    if(Requester != nullptr)
    {
        Requester->disconnect();

        delete Requester;
        Requester = nullptr;
    }

    scenariosList.clear();
}


bool TConnectorREST::start(void)
{
    bool rez = false;

    if(testConnectionREST()) setStatus(cStatusIsStarted);
    else setStatus(cStatusIsOffline);

    return rez;
}


bool TConnectorREST::stop(void)
{
    bool rez = false;

    if(Requester != nullptr) setStatus(cStatusIsStopped);

    return rez;
}


QString TConnectorREST::getErrorText(int value)
{
    QString errorText;

    switch (value)
    {
      case cErrorRunGetOnOffline: errorText = QString("Попытка выполнения GET при статусе сервера моделирования оффлайн. Код: ") + QString::number(value);

      case cErrorGetSyntaxError: errorText = QString("Неверный синтаксис запроса GET. Код: ") + QString::number(value);

      case cErrorGetLogicError: errorText = QString("Имеется какая-то логическая ошибка, из-за которой невозможно произвести операцию GET. Код: ") + QString::number(value);

      case cErrorScenarioNotFound: errorText = QString("Сценарий не найден GET. Код: ") + QString::number(value);

      default: errorText = QString("Критическая ошибка в процессе приема данных через REST интерфейс сервера. Код: ") + QString::number(value);
    }

    return errorText;
}


void TConnectorREST::takeScenariosList()
{
    if(!gettingScenarios)
    {
        if((codeStatus == cStatusIsOnline)||(codeStatus == cStatusIsBusy)
           ||(codeStatus == cStatusIsStarted)||(codeStatus == cStatusIsInitialized)) getScenariosListREST();
    }
}


void TConnectorREST::takeScenario(int id)
{
    if((codeStatus == cStatusIsOnline)||(codeStatus == cStatusIsBusy)) getScenarioByIdREST(id);
}

void TConnectorREST::setStatus(int value)
{
    if(codeStatus != value)
    {
        codeStatus = value;

        emit getStatus(value);
    }
}


void TConnectorREST::setStatusGIS(int value)
{
    if(codeStatusGIS != value)
    {
        codeStatusGIS = value;

        emit getStatusGIS(value);
    }
}


void TConnectorREST::setError(int value)
{
    codeError = value;

    if(value < 1) emit getError(value);
}


void TConnectorREST::getDataBaseOptions(QString ConnectionName)
{
    ConnectionNameDB = ConnectionName;
}


void TConnectorREST::getRESTOptions(bool useAuth, QString UserName, QString Password, QString Server, QString Port, QString HTTPMainString,
                    QString ScenariosSect, QString BlastwaleSect, QString ExplosionsSect, QString CommonSect)
{
    stop();

    useAuthREST = useAuth;

    if(useAuthREST)
    {
        UserNameREST = UserName;
        PasswordREST = Password;
    }

    ServerREST = Server;
    PortREST = Port;

    if(HTTPMainString.size() > 0) HTTPMainStringREST = HTTPMainString;
    else HTTPMainStringREST = QString("/");

    ScenariosSectREST = ScenariosSect;
    BlastwaleSectREST = BlastwaleSect;
    ExplosionsSectREST = ExplosionsSect;
    CommonSectREST = CommonSect;

    CommonTextGetApiREST = QString("http://") + ServerREST + QString(":") + PortREST + HTTPMainStringREST;

    Requester = new TRequester(this);

    connect(Requester, SIGNAL(toLog(QString)), this, SLOT(getLog(QString)));
    connect(Requester, SIGNAL(networkError(int)), this, SLOT(getNetworkError(int)));
    connect(Requester, SIGNAL(callbackError(const int, const int, const QString, const QString, const QJsonObject &)), this, SLOT(errorGetREST(const int, const int, const QString, const QString, const QJsonObject &)));
    connect(Requester, SIGNAL(callbackCh0Success(const int, const QJsonObject &)), this, SLOT(okTestConnectionREST(const int, const QJsonObject &)));
    connect(Requester, SIGNAL(callbackCh1Success(const int, const QJsonObject &)), this, SLOT(okTestConnectionGIS(const int, const QJsonObject &)));
    connect(Requester, SIGNAL(callbackCh2Success(const int, const QJsonObject &)), this, SLOT(okScenariosListREST(const int, const QJsonObject &)));
    connect(Requester, SIGNAL(callbackCh3Success(const int, const QJsonObject &)), this, SLOT(okScenarioByIdREST(const int, const QJsonObject &)));
    connect(Requester, SIGNAL(callbackCh4Success(const int, const QJsonObject &)), this, SLOT(okBlastWaleByIdREST(const int, const QJsonObject &)));

    Requester->init(ServerREST, PortREST, HTTPMainStringREST);

    setStatus(cStatusIsInitialized);
}


bool TConnectorREST::testConnectionGIS(QString HTTPMainStringGIS, QString ServerGIS, QString PortGIS)
{
    bool rez = false;

    if((codeStatusGIS == cStatusIsOnline)||(codeStatusGIS == cStatusIsStarted)||(codeStatusGIS == cStatusIsInitialized)||(codeStatusGIS == cStatusIsBusy))
    {
        setStatusGIS(cStatusIsBusy);

        if(HTTPMainStringGIS.contains(QString("http://"), Qt::CaseInsensitive))
        {
            Requester->sendRequest(HTTPMainStringGIS, 1, false);
        }
        else
        {
            QString TextGetApiREST;

            if(ServerGIS != nullptr)
            {
                if(PortGIS != nullptr)
                {
                    if(HTTPMainStringGIS.indexOf(QChar('/')) == 0) TextGetApiREST = QString("http://") + ServerGIS + QString(":") + PortGIS + HTTPMainStringGIS;
                    else TextGetApiREST = QString("http://") + ServerGIS + QString(":") + PortGIS + QString("/") + HTTPMainStringGIS;
                }
                else
                {
                    if(HTTPMainStringGIS.indexOf(QChar('/') == 0)) TextGetApiREST = QString("http://") + ServerGIS + HTTPMainStringGIS;
                    else TextGetApiREST = QString("http://") + ServerGIS + QString("/") + HTTPMainStringGIS;
                }
            }
            else
            {
                if(HTTPMainStringGIS.indexOf(QChar('/') == 0)) TextGetApiREST = QString("http:/") + HTTPMainStringGIS;
                else TextGetApiREST = QString("http://") + HTTPMainStringGIS;
            }

            Requester->sendRequest(TextGetApiREST, 1, false);
        }

        setStatusGIS(cStatusIsInitialized);
        rez = true;
    }
    else setError(cErrorRunGetOnOffline);

    return rez;
}


bool TConnectorREST::testConnectionREST()
{
    bool rez = false;

    if((codeStatus == cStatusIsOnline)||(codeStatus == cStatusIsStarted)||(codeStatus == cStatusIsInitialized))
    {
        setStatus(cStatusIsBusy);

        QString TextGetApiREST = CommonTextGetApiREST + CommonSectREST;

        Requester->sendRequest(TextGetApiREST, 0);

        if(codeStatus == cStatusIsBusy) setStatus(cStatusIsOnline);
        rez = true;
    }
    else setError(cErrorRunGetOnOffline);

    return rez;
}


bool TConnectorREST::getScenariosListREST()
{
    bool rez = false;

    if((codeStatus == cStatusIsOnline)||(codeStatus == cStatusIsBusy)
       ||(codeStatus == cStatusIsStarted)||(codeStatus == cStatusIsInitialized))
    {
        setStatus(cStatusIsBusy);

        gettingScenarios = true;

        QString TextGetApiREST = CommonTextGetApiREST + ScenariosSectREST;

        Requester->sendRequest(TextGetApiREST, 2);

        rez = true;
    }
    else setError(cErrorRunGetOnOffline);

    return rez;
}


bool TConnectorREST::getBlastWaleByIdREST(int value)
{
    bool rez = false;

    if((codeStatus == cStatusIsOnline)||(codeStatus == cStatusIsBusy))
    {
        setStatus(cStatusIsBusy);

        QString TextGetApiREST = CommonTextGetApiREST + BlastwaleSectREST;

        TextGetApiREST = TextGetApiREST.replace(QString("{scenarioId}"), QString("{") + QString::number(value) + QString("}"), Qt::CaseInsensitive);

        Requester->sendRequest(TextGetApiREST, 4);

        rez = true;
    }
    else setError(cErrorRunGetOnOffline);

    return rez;
}


bool TConnectorREST::getScenarioByIdREST(int value)
{
    bool rez = false;

    if((codeStatus == cStatusIsOnline)||(codeStatus == cStatusIsBusy))
    {
        setStatus(cStatusIsBusy);

        QString TextGetApiREST = CommonTextGetApiREST + ExplosionsSectREST;

        TextGetApiREST = TextGetApiREST.replace(QString("{scenarioId}"), QString::number(value), Qt::CaseInsensitive);

        Requester->sendRequest(TextGetApiREST, 3);

        rez = true;
    }
    else setError(cErrorRunGetOnOffline);

    return rez;
}


bool TConnectorREST::getScenariosListDB()
{
    bool rez = false;

    int scenariosCount = 0;

    if(ConnectionNameDB.size() > 0)
    {
        QSqlDatabase visualizerDB = QSqlDatabase::database(ConnectionNameDB);

        if(visualizerDB.isOpen())
        {
            QSqlQuery getScenariosListQuery(visualizerDB);

            QString getScenariosListSQL("SELECT COUNT(scenariocodeid) AS scenariosCount FROM \"classifiersmodel\".\"classifierscenarios\"");

            if(getScenariosListQuery.prepare(getScenariosListSQL))
            {
                getScenariosListQuery.setForwardOnly(true);

                if(getScenariosListQuery.exec(getScenariosListSQL))
                {
                    while(getScenariosListQuery.next())
                    {
                        scenariosCount = getScenariosListQuery.value("scenariosCount").toInt();
                    }
                }
                else
                {
                    setError(cErrorQueryNotExec);
                }
            }
            else
            {
                setError(cErrorQuerySyntax);
            }

            getScenariosListSQL = QString("SELECT scenariocodeid FROM \"classifiersmodel\".\"classifierscenarios\" ORDER BY scenariocodeid");

            if(getScenariosListQuery.prepare(getScenariosListSQL))
            {
                getScenariosListQuery.setForwardOnly(true);

                if(getScenariosListQuery.exec(getScenariosListSQL))
                {
                    while(getScenariosListQuery.next())
                    {
                        rez = getScenariosListQuery.value("scenariocodeid").toInt();
                    }

                    rez = true;
                }
                else
                {
                    setError(cErrorQueryNotExec);
                }
            }
            else
            {
                setError(cErrorQuerySyntax);
            }
        }
        else setError(cErrorConnectionNotOpen);
    }
    else setError(cErrorConnectionNameIsNull);

    return rez;
}


bool TConnectorREST::StatusConnect()
{
    if((codeStatus == cStatusIsStarted)||(codeStatus == cStatusIsOnline)||(codeStatus == cStatusIsBusy)) return true;
    else return false;
}


bool TConnectorREST::StatusConnectGIS()
{
    if((codeStatusGIS == cStatusIsStarted)||(codeStatusGIS == cStatusIsOnline)||(codeStatusGIS == cStatusIsBusy)) return true;
    else return false;
}


void TConnectorREST::getNetworkError(int value)
{
    if(codeError < 1) codeError = value;
}


void TConnectorREST::getLog(QString Value)
{
    emit toLog(Value);
}


void TConnectorREST::errorGetREST(const int numberCh, const int code, const QString urlText, const QString errorText, const QJsonObject &obj)
{
    if((numberCh == 0)&&(urlText.contains(CommonSectREST)))
    {
        if(obj.value("status").toString().contains(QString("success"), Qt::CaseInsensitive)) setStatus(cStatusIsOnline);
        else
        {
            setStatus(cStatusIsOffline);
            toLog(QString("Запрос по адресу:\n") + urlText + QString("\nвернул код:\n") + QString::number(code) + QString("\nвызвал ошибку:\n") + errorText);
        }
    }
    else if(numberCh == 1)
    {
        if(code == 200) setStatusGIS(cStatusIsOnline);
        else setStatusGIS(cStatusIsOffline);
    }
    else
    {
        if(code == 200) setStatus(cStatusIsOnline);
        toLog(QString("Запрос по адресу:\n") + urlText + QString("\nвернул код:\n") + QString::number(code) + QString("\nвызвал ошибку:\n") + errorText);
        testConnectionREST();
    }
}


void TConnectorREST::okTestConnectionGIS(const int code, const QJsonObject & obj)
{
    if(code == 200)
    {
        if(obj.value("status").toString().contains(QString("success"), Qt::CaseInsensitive)) setStatusGIS(cStatusIsOnline);
        else setStatusGIS(cStatusIsOnline);
    }
    else
    {
        setStatusGIS(cStatusIsErrorOnGet);
    }
}


void TConnectorREST::okTestConnectionREST(const int code, const QJsonObject & obj)
{
    if(code == 200)
    {
        if(obj.value("status").toString().contains(QString("success"), Qt::CaseInsensitive)) setStatus(cStatusIsOnline);
        else
        {
            setStatus(cStatusIsOnline);
        }
    }
    else
    {
        if(obj.value("status").toString().contains(QString("success"), Qt::CaseInsensitive)) setStatus(cStatusIsOnline);
        else setStatus(cStatusIsOffline);
    }
}


void TConnectorREST::okScenariosListREST(const int code, const QJsonObject & obj)
{
    if(obj.value("status").toString().contains(QString("success"), Qt::CaseInsensitive))
    {
        setStatus(cStatusIsOnline);

        scenariosList.clear();

        if((!obj.value("data").isNull())&&(!obj.value("data").isUndefined()))
        {
            QJsonObject objScenarios = obj["data"].toObject();

            if(objScenarios.value("scenarios").isArray())
            {
                QJsonArray jsonArray = objScenarios["scenarios"].toArray();

                foreach (const QJsonValue & value, jsonArray)
                {
                    QJsonObject objScenario = value.toObject();
                    TScenario tempScenario;
                    tempScenario.id = objScenario["id"].toInt();
                    tempScenario.name = objScenario["name"].toString().toStdString();
                    tempScenario.description = objScenario["description"].toString().toStdString();
                    tempScenario.created = objScenario["dateCreated"].toString().toStdString();
                    QString tempStr = objScenario["resultsAvailabilityStatus"].toString();
                    if(tempStr.contains(QString("NoResults"), Qt::CaseInsensitive)) tempScenario.status = 1;
                    else if(tempStr.contains(QString("InProcess"), Qt::CaseInsensitive)) tempScenario.status = 2;
                    else if(tempStr.contains(QString("ResultsAvailable"), Qt::CaseInsensitive)) tempScenario.status = 3;
                    else if(tempStr.contains(QString("Unknown"), Qt::CaseInsensitive)) tempScenario.status = 0;
                    else tempScenario.status = 0;
                    scenariosList.push_back(tempScenario);
                }

                emit getCurrentScenariosList(scenariosList);
            }
            else
            {
                setError(cErrorJSONSyntaxRESTAPI);
            }
        }
    }
    else if(code == 200) setStatus(cStatusIsOnline);
    else setStatus(cStatusIsOffline);

    if(codeError < 1)
    {
        toLog(QString("Обновлены данные сервера, всего сценариев: ") + QString::number(scenariosList.size()));
    }
    else
    {
        toLog(QString("При обновлении данных сервера произошла ошибка: код ") + QString::number(codeError) + QString(", всего сценариев: ") + QString::number(scenariosList.size()));
    }

    gettingScenarios = false;
}


void TConnectorREST::okScenarioByIdREST(const int code, const QJsonObject & obj)
{
    if(code == 200)
    {
        setStatus(cStatusIsOnline);
        emit getScenario(obj);
    }
    else
    {
        if(obj.count() > 0) setStatus(cStatusIsOnline);
        else setStatus(cStatusIsOffline);
    }

}


void TConnectorREST::okBlastWaleByIdREST(const int code, const QJsonObject & obj)
{
    if(code == 200)
    {
        setStatus(cStatusIsOnline);
    }
    else
    {
        setStatus(cStatusIsOffline);
    }

    emit getBlastWale(obj);
}
