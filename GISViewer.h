//----------------------------------------------------------------------------//
//                                                                            //
// Файл GISViewer.h                                                           //
//                                                                            //
//                      *** TGISViewer КЛАСС ОКНА "ГИС" ***                   //
//                                                                            //
//                                                                            //
//                                                           Москва, 2022 год //
//----------------------------------------------------------------------------//


#ifndef GISVIEWER_H
#define GISVIEWER_H


#include <QDialog>
#include <QIcon>
#ifdef WEBENGINE_USED
  #include <QWebEngineView>
  #include <QWebEnginePage>
  #include <QWebEngineSettings>
#endif

#include "ui_GISViewer.h"


class TGISViewer : public QDialog, private Ui::TGISViewer
{
    Q_OBJECT

  public:
    explicit TGISViewer(QWidget *parent = 0);

    ~TGISViewer();

    QString getAddressString();

    bool StatusConnect();

    bool setConnectionGIS(QString HTTPMainStringGIS, QString ServerGIS = nullptr, QString PortGIS = nullptr);

  signals:
    //Запись в лог
    void toLog(QString text);

  private:
    //Наименование окна "Геоинформационная система (ГИС "Cesium")"
    const QString cWindowName = QString("Геоинформационная система (ГИС \"Cesium\")");

    //Иконка окна "Геоинформационная система (ГИС "Cesium")"
    const QString cWindowIconFilePath= QString(":/gisicon");

    //Код ошибки
    int codeError;

    //
    bool getStatusConnect;

    QString CesiumAddressString;

#ifdef WEBENGINE_USED
    QWebEngineView* CesiumWebView;
#else
    QWidget* CesiumWebView;
#endif

};

#endif // GISVIEWER_H
