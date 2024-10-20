//----------------------------------------------------------------------------//
//                                                                            //
// Файл DataBaseViewer.cpp                                                    //
//                                                                            //
//        *** TDataBaseViewer КЛАСС ОКНА "ПРОСМОТР ИСХОДНЫХ ДАННЫХ" ***       //
//                                                                            //
//                                                                            //
//                                                           Москва, 2022 год //
//----------------------------------------------------------------------------//


#include "DataBaseViewer.h"


TDataBaseViewer::TDataBaseViewer(QWidget *parent) : QDialog(parent)
{
    codeError = 0;

    setupUi(this);

    setWindowTitle(cWindowName);

    setWindowIcon(QIcon(cWindowIconFilePath));
}


void TDataBaseViewer::setConnectionName(QString Value)
{
    getConnectionName = Value;
}


void TDataBaseViewer::scenarioToDB(int id, QString created, QString name, QString description, const QJsonObject &obj)
{
    int indexDB = -1, code = 0, scenariosCount = 0;

    if(getConnectionName.size() > 0)
    {
        QSqlDatabase visualizerDB = QSqlDatabase::database(getConnectionName);

        if(visualizerDB.isOpen())
        {
            QSqlQuery tempQuery(visualizerDB);

            QString getScenariosListSQL("COUNT(scenariocodeid) AS scenariosCount, id FROM \"classifiersmodel\".\"classifierscenarios\" WHERE scenariocodeid =");
            getScenariosListSQL += QString::number(id);

            if(tempQuery.prepare(getScenariosListSQL))
            {
                tempQuery.setForwardOnly(true);

                if(tempQuery.exec(getScenariosListSQL))
                {
                    while(tempQuery.next())
                    {
                        scenariosCount = tempQuery.value("scenariosCount").toInt();
                        indexDB = tempQuery.value("id").toInt();
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

            if(scenariosCount < 1)
            {
                getScenariosListSQL = QString("INSERT INTO \"classifiersmodel\".\"classifierscenarios\" (scenariocodeid, name, description, datecreated, status)");
                getScenariosListSQL += QString("VALUES (:id, :name, : description, :date, 0)");

                if(tempQuery.prepare(getScenariosListSQL))
                {
                    tempQuery.bindValue(QString(":id"), id);
                    tempQuery.bindValue(QString(":name"), name);
                    tempQuery.bindValue(QString(":description"), description);
                    tempQuery.bindValue(QString(":datecreated"), created);

                    if(tempQuery.exec(getScenariosListSQL))
                    {
                        getScenariosListSQL = QString("SELECT id, scenariocodeid FROM \"classifiersmodel\".\"classifierscenarios\" WHERE scenariocodeid=") + QString::number(id);

                        if(tempQuery.prepare(getScenariosListSQL))
                        {
                            tempQuery.setForwardOnly(true);

                            if(tempQuery.exec(getScenariosListSQL))
                            {
                                while(tempQuery.next())
                                {
                                    indexDB = tempQuery.value("id").toInt();
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
        }
        else setError(cErrorConnectionNotOpen);
    }
    else setError(cErrorConnectionNameIsNull);

    emit updatedScenario(id, indexDB, code);
}


void TDataBaseViewer::on_pbClose_clicked()
{
    close();
}


void TDataBaseViewer::on_pbViewerREST_clicked()
{
    emit showViewerREST();
}


void TDataBaseViewer::setError(int value)
{
    codeError = value;

    if(value > 0) emit getError(value);
}


bool TDataBaseViewer::getScenariosList()
{
    int i = 0, scenariosCount = 0;
    bool rez = false;

    if(getConnectionName.size() > 0)
    {
        QSqlDatabase visualizerDB = QSqlDatabase::database(getConnectionName);

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

            twScenariosList->setRowCount(scenariosCount);

            if(scenariosCount > 0)
            {
                QTableWidgetItem* tempItem;
                getScenariosListSQL = QString("SELECT id, scenariocodeid, name, dateCreated, description, status, countEnginery, countStructures, ambientTemperature, ambientPressure, equipment_id FROM \"classifiersmodel\".\"classifierscenarios\" ORDER BY scenariocodeid");

                if(getScenariosListQuery.prepare(getScenariosListSQL))
                {
                    getScenariosListQuery.setForwardOnly(true);

                    if(getScenariosListQuery.exec(getScenariosListSQL))
                    {
                        twScenariosList->clearContents();

                        while(getScenariosListQuery.next())
                        {
                            for(int j = 0; j < 11; j++)
                            {
                                tempItem = new QTableWidgetItem(getScenariosListQuery.value(j).toString());
                                tempItem->setTextAlignment(Qt::AlignCenter);
                                twScenariosList->setItem(i, j, tempItem);
                            }

                            i++;
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
        }
        else setError(cErrorConnectionNotOpen);
    }
    else setError(cErrorConnectionNameIsNull);

    return rez;
}
