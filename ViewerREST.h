//----------------------------------------------------------------------------//
//                                                                            //
// Файл ViewerREST.h                                                          //
//                                                                            //
//                *** TViewerREST КЛАСС ОКНА ПРОСМОТРА ДАННЫХ ***             //
//                *** ПОЛУЧЕННЫХ ЧЕРЕЗ REST ИНТЕРФЕЙС СЕРВЕРА ***             //
//                                                                            //
//                                                                            //
//                                                           Москва, 2022 год //
//----------------------------------------------------------------------------//


#ifndef VIEWERREST_H
#define VIEWERREST_H


#include <QJsonDocument>
#include <QJsonObject>


#include "src/ConnectorREST.h"


#include "ui_ViewerREST.h"


class TViewerREST : public QDialog, public Ui::TViewerREST
{
    Q_OBJECT

  public:
    explicit TViewerREST(QWidget *parent = 0);
    ~TViewerREST();

  signals:
    //Запись в лог
    void toLog(QString text);

    //
    void getScenario(int id);

    //
    void getScenarios();

    //
    void scenarioToDB(int id, QString created, QString name, QString description, const QJsonObject &obj);

  public slots:
    void refreshScenariosList(const std::vector<TConnectorREST::TScenario> &scenariosList);

    void getScenario(const QJsonObject &obj);

    void updatedScenario(int id, int indexDB, int code);

  private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pbRefresh_clicked();

private:
    //Наименование окна "Просмотр  данных, предоставляемых сервером "
    const QString cWindowName = QString("Просмотр данных, предоставляемых сервером");

    //Иконка окна "Просмотр  данных, предоставляемых сервером"
    const QString cWindowIconFilePath= QString(":/rest");

    //Код ошибки
    int codeError;
};

#endif // VIEWERREST_H
