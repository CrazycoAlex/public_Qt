//----------------------------------------------------------------------------//
//                                                                            //
// Файл ViewerREST.cpp                                                        //
//                                                                            //
//                *** TViewerREST КЛАСС ОКНА ПРОСМОТРА ДАННЫХ ***             //
//                *** ПОЛУЧЕННЫХ ЧЕРЕЗ REST ИНТЕРФЕЙС СЕРВЕРА ***             //
//                                                                            //
//                                                                            //
//                                                           Москва, 2022 год //
//----------------------------------------------------------------------------//


#include "ViewerREST.h"


TViewerREST::TViewerREST(QWidget *parent) : QDialog(parent)
{
    codeError = 0;

    setupUi(this);

    setWindowTitle(cWindowName);

    setWindowIcon(QIcon(cWindowIconFilePath));
}


TViewerREST::~TViewerREST()
{

}


void TViewerREST::refreshScenariosList(const std::vector<TConnectorREST::TScenario> &scenariosList)
{
    twScenarios->clearContents();
    twScenarios->setRowCount(scenariosList.size());

    QTableWidgetItem* tempItem;

    for (unsigned int i = 0; i < scenariosList.size(); i++)
    {
        int j = 0;
        QString tempStr;

        tempItem = new QTableWidgetItem(QString::number(scenariosList.at(i).id));
        tempItem->setTextAlignment(Qt::AlignCenter);
        twScenarios->setItem(i, j, tempItem);
        j++;

        switch (scenariosList.at(i).status)
        {//0 -  Unknown, 1 - NoResults, 2 - InProcess, 3 - ResultsAvailable;
          case 0: tempStr = QString("Неизвестное состояние"); break;
          case 1: tempStr = QString("Отсутствуют"); break;
          case 2: tempStr = QString("В процессе расчета"); break;
          case 3: tempStr = QString("Доступны"); break;
          default: tempStr = QString("Неизвестное состояние");
        }
        tempItem = new QTableWidgetItem(tempStr);
        tempItem->setTextAlignment(Qt::AlignCenter);
        twScenarios->setItem(i, j, tempItem);
        j++;

        switch (scenariosList.at(i).statusDB)
        {//0 - Неизвестное состояние, 1 - Отсутствует в БД, 2 - Присутствует в БД с другими параметрами, 3 - Синхронизировано с БД, 4 - В процессе получения
          case 0: tempStr = QString("Неизвестное состояние"); break;
          case 1: tempStr = QString("Отсутствует в БД"); break;
          case 2: tempStr = QString("Присутствует в БД с другими параметрами"); break;
          case 3: tempStr = QString("Синхронизировано с БД"); break;
          case 4: tempStr = QString("В процессе получения"); break;
          default: tempStr = QString("Неизвестное состояние");
        }
        tempItem = new QTableWidgetItem(tempStr);
        tempItem->setTextAlignment(Qt::AlignCenter);
        twScenarios->setItem(i, j, tempItem);
        j++;

        tempItem = new QTableWidgetItem(QString::fromStdString(scenariosList.at(i).name));
        tempItem->setTextAlignment(Qt::AlignCenter);
        twScenarios->setItem(i, j, tempItem);
        j++;

        tempItem = new QTableWidgetItem(QString::fromStdString(scenariosList.at(i).created));
        tempItem->setTextAlignment(Qt::AlignCenter);
        twScenarios->setItem(i, j, tempItem);
        j++;

        tempItem = new QTableWidgetItem(QString::fromStdString(scenariosList.at(i).description));
        tempItem->setTextAlignment(Qt::AlignCenter);
        twScenarios->setItem(i, j, tempItem);
        j++;

        tempItem = new QTableWidgetItem(QString::number(scenariosList.at(i).code));
        tempItem->setTextAlignment(Qt::AlignCenter);
        twScenarios->setItem(i, j, tempItem);
        j++;

        tempItem = new QTableWidgetItem(QString::number(scenariosList.at(i).indexDB));
        tempItem->setTextAlignment(Qt::AlignCenter);
        twScenarios->setItem(i, j, tempItem);
        j++;
    }
}


void TViewerREST::getScenario(const QJsonObject &obj)
{
    lwScenario->clear();
    QJsonDocument docScenario(obj);
    lwScenario->addItem(docScenario.toJson(QJsonDocument::Indented));
    int id = obj.value(QString("id")).toInt();
    QString name = obj.value(QString("name")).toString();
    QString dateCreated = obj.value(QString("dateCreated")).toString();
    QString description = obj.value(QString("description")).toString();

    emit scenarioToDB(id, dateCreated, name, description, obj);
}


void TViewerREST::updatedScenario(int id, int indexDB, int code)
{
    QTableWidgetItem* tempItem;

    for(int i = 0; i < twScenarios->rowCount(); i++)
    {
        if(twScenarios->item(i, 0)->text().toInt() == id)
        {
            tempItem = new QTableWidgetItem(QString::number(code));
            tempItem->setTextAlignment(Qt::AlignCenter);
            twScenarios->setItem(i, 6, tempItem);

            tempItem = new QTableWidgetItem(QString::number(indexDB));
            tempItem->setTextAlignment(Qt::AlignCenter);
            twScenarios->setItem(i, 7, tempItem);
        }
    }
}


void TViewerREST::on_pushButton_2_clicked()
{
    if(twScenarios->currentRow() > -1)
    {
        if(twScenarios->item(twScenarios->currentRow(), 1)->text().contains(QString("Доступны"), Qt::CaseInsensitive))
        {
            emit getScenario(twScenarios->item(twScenarios->currentRow(), 0)->text().toInt());
        }
    }
}


void TViewerREST::on_pushButton_clicked()
{
    for(int i = 0; i < twScenarios->rowCount(); i++)
    {
        if(twScenarios->item(i, 1)->text().contains(QString("Доступны"), Qt::CaseInsensitive))
        {
            emit getScenario(twScenarios->item(i, 0)->text().toInt());
        }
    }
}


void TViewerREST::on_pushButton_3_clicked()
{
    close();
}


void TViewerREST::on_pbRefresh_clicked()
{
    emit getScenarios();
}
