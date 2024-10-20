//----------------------------------------------------------------------------//
//                                                                            //
// Файл DataBaseViewer.h                                                      //
//                                                                            //
//        *** TDataBaseViewer КЛАСС ОКНА "ПРОСМОТР ИСХОДНЫХ ДАННЫХ" ***       //
//                                                                            //
//                                                                            //
//                                                           Москва, 2022 год //
//----------------------------------------------------------------------------//


#ifndef DATABASEVIEWER_H
#define DATABASEVIEWER_H


#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>


#include "ViewerREST.h"


#include "ui_DataBaseViewer.h"


class TDataBaseViewer : public QDialog, private Ui::TDataBaseViewer
{
    Q_OBJECT

  public:
    //Ошибка инициализации подключения к базе данных СПО
    static const int cErrorConnectionNameIsNull = 5;
    //Ошибка подключения к базе данных СПО
    static const int cErrorConnectionNotOpen = 6;
    //Ошибка построения запроса к базе данных СПО
    static const int cErrorQueryNotExec = 7;
    //Синтаксическая ошибка в запроса к базе данных СПО
    static const int cErrorQuerySyntax = 8;

    explicit TDataBaseViewer(QWidget *parent = 0);

    void setConnectionName(QString Value);
    //
    bool getScenariosList();

  signals:
    //Запись в лог
    void toLog(QString text);

    void getError(int value);

    void showViewerREST();

    void updatedScenario(int id, int indexDB, int code);

  public slots:
    void scenarioToDB(int id, QString created, QString name, QString description, const QJsonObject &obj);

  private slots:
    void on_pbClose_clicked();

    void on_pbViewerREST_clicked();

  private:
    //Наименование окна "Просмотр исходных данных"
    const QString cWindowName = QString("Просмотр исходных данных");

    //Иконка окна "Просмотр исходных данных"
    const QString cWindowIconFilePath= QString(":/databaseviewericon");

    //Код ошибки
    int codeError;

    //
    QString getConnectionName;

    //
    void setError(int value);

};

#endif // DATABASEVIEWER_H
