//----------------------------------------------------------------------------//
//                                                                            //
// Файл ConnectorREST.h                                                       //
//                                                                            //
//                *** TConnectorREST КЛАСС ПОЛУЧЕНИЯ ДАННЫХ ***               //
//                     *** ЧЕРЕЗ REST ИНТЕРФЕЙС СЕРВЕРА ***                   //
//                                                                            //
//                                                           Москва, 2022 год //
//----------------------------------------------------------------------------//


#ifndef CONNECTORREST_H
#define CONNECTORREST_H


#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>


#include "Requester.h"


class TConnectorREST : public QObject
{
    Q_OBJECT
  public:
    struct TScenario
    {
        TScenario() : index(-1), id(-1), name(""), created(""), description(""), status(0), statusDB(0), indexDB(-1), code(0) {}

        //индекс в векторе
        int index;
        //Идентификатор сценария
        int id;
        //Наименование
        std::string name;
        //Дата создания
        std::string created;
        //Описание
        std::string description;
        //Статус
        //0 -  Unknown, 1 - NoResults, 2 - InProcess, 3 - ResultsAvailable;
        int status;
        //Статус в БД
        //0 - Неизвестное состояние, 1 - Отсутствует в БД, 2 - Присутствует в БД с другими параметрами, 3 - синхронизировано с БД, 4 - В процессе получения
        int statusDB;
        //индекс в БД
        int indexDB;
        //код ошибки
        int code;
    };

    static const int cStatusIsInitialized = 0;
    static const int cStatusIsStarted = 1;
    static const int cStatusIsOnline = 2;
    static const int cStatusIsBusy = 3;
    static const int cStatusIsStopped = 4;
    static const int cStatusIsOffline = 5;
    static const int cStatusIsErrorOnChannel = 6;
    static const int cStatusIsErrorOnGet = 7;
    static const int cStatusIsErrorOnJSON = 8;
    static const int cStatusIsErrorOnDB = 9;
    static const int cStatusIsError = 10;
    static const int cStatusIsClosed = 11;

    //Попытка выполнения GET при статусе сервера оффлайн
    static const int cErrorRunGetOnOffline = 1;
    //Неверный синтаксис запроса GET
    static const int cErrorGetSyntaxError = 2;
    //Имеется какая-то логическая ошибка, из-за которой невозможно произвести операцию GET
    static const int cErrorGetLogicError = 3;
    //Сценарий не найден GET
    static const int cErrorScenarioNotFound = 4;
    //Ошибка инициализации подключения к базе данных СПО
    static const int cErrorConnectionNameIsNull = 5;
    //Ошибка подключения к базе данных СПО
    static const int cErrorConnectionNotOpen = 6;
    //Ошибка построения запроса к базе данных СПО
    static const int cErrorQueryNotExec = 7;
    //Синтаксическая ошибка в запроса к базе данных СПО
    static const int cErrorQuerySyntax = 8;
    //Синтаксическая ошибка в JSON
    static const int cErrorJSONSyntax = 9;
    //Синтаксическая ошибка в JSON (REST API)
    static const int cErrorJSONSyntaxRESTAPI = 10;

    explicit TConnectorREST(QObject *parent = nullptr);
    ~TConnectorREST();

    //Запуск канала взаимодействия с REST интерфейсом сервера моделирования
    bool start(void);

    //Остановка работы канала взаимодействия с REST интерфейсом сервера моделирования
    bool stop(void);

    QString getErrorText(int value);

    bool StatusConnect();

    bool StatusConnectGIS();

  signals:
    void getStatus(int value);

    void getStatusGIS(int value);

    void getScenario(const QJsonObject &obj);

    void getBlastWale(const QJsonObject &obj);

    void getError(int value);

    void toLog(QString Value);

    void getCurrentScenariosList(const std::vector<TConnectorREST::TScenario> &scenariosList);

  public slots:
    //Обновление информации о перечне сценариев на сервере
    void takeScenariosList();

    void takeScenario(int id);

    //Установка флага статуса
    void setStatus(int value);

    //Установка флага статуса сервера ГИС
    void setStatusGIS(int value);

    //Установка флага ошибки
    void setError(int value);

    //Получение параметров подключения к серверу БД
    void getDataBaseOptions(QString ConnectionName);

    //Получение параметров REST
    void getRESTOptions(bool useAuth, QString UserName, QString Password, QString Server, QString Port, QString HTTPMainString,
                        QString ScenariosSect, QString BlastwaleSect, QString ExplosionsSect, QString CommonSect);

    void getNetworkError(int value);

    void getLog(QString Value);

    void errorGetREST(const int numberCh, const int code, const QString urlText, const QString errorText, const QJsonObject &obj);

    void okTestConnectionGIS(const int code, const QJsonObject & obj);

    void okTestConnectionREST(const int code, const QJsonObject & obj);

    void okScenariosListREST(const int code, const QJsonObject & obj);

    void okScenarioByIdREST(const int code, const QJsonObject & obj);

    void okBlastWaleByIdREST(const int code, const QJsonObject & obj);

    //Проверка соединения GIS
    bool testConnectionGIS(QString HTTPMainStringGIS, QString ServerGIS = nullptr, QString PortGIS = nullptr);

    //Проверка соединения REST
    //Получение информации о версии API через GET getVersion (секция общая)
    bool testConnectionREST();

  private:
    const QByteArray cGetCommonSectionGetVersion = "getVersion";
    const QByteArray cGetScenariosSectionGetScenarios = "getScenarios";
    const QByteArray cGetExplosionsSectionGetExplosionById = "getExplosionById";
    const QByteArray cGetBlastwaleSectionGetBlastWaleById = "getBlastWaleById";

    //
    bool gettingScenarios;

    //Код ошибки
    int codeError;

    //Код статуса
    int codeStatus;

    //Код статуса сервера ГИС
    int codeStatusGIS;

    //Период опроса сервера (секунды)
    int intervalServerREST;

    //Время таймаута сервера (секунды)
    int timeoutServerREST;

    //Параметры работы с базой данных
    QString ConnectionNameDB;

    //Параметры работы с REST API
    //признак использования аутентификации
    bool useAuthREST;

    //пользователь REST
    QString UserNameREST;

    //пароль пользователя REST
    QString PasswordREST;

    //сервер REST
    QString ServerREST;

    //порт сервера REST
    QString PortREST;

    //адресная строка
    QString HTTPMainStringREST;

    //секция перечня сценариев
    QString ScenariosSectREST;

    //секция параметров ударной волны
    QString BlastwaleSectREST;

    //секция результатов моделирования
    QString ExplosionsSectREST;

    //секция общая
    QString CommonSectREST;

    //
    QString CommonTextGetApiREST;

    //
    TRequester* Requester;

    //Перечень сценариев на сервере моделирования
    std::vector<TScenario> scenariosList;

    //Получение списка сценариев применения через GET getScenarios (секция перечня сценариев)
    bool getScenariosListREST();

    //Получение параметров по результатам моделирования применения по сценарию с идентификатором
    // через GET getBlastWaleById (секция параметров)
    bool getBlastWaleByIdREST(int value);

    //Получение информации по результатам моделирования применения по сценарию с идентификатором
    // через GET getExplosionById (секция результатов моделирования)
    bool getScenarioByIdREST(int value);

    //Получение списка сценариев применения из базы данных
    bool getScenariosListDB();
};

#endif // CONNECTORREST_H
