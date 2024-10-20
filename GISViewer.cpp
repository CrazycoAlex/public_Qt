//----------------------------------------------------------------------------//
//                                                                            //
// Файл GISViewer.cpp                                                         //
//                                                                            //
//                      *** TGISViewer КЛАСС ОКНА "ГИС" ***                   //
//                                                                            //
//                                                                            //
//                                                           Москва, 2022 год //
//----------------------------------------------------------------------------//


#include "GISViewer.h"


TGISViewer::TGISViewer(QWidget *parent) : QDialog(parent)
{
    codeError = 0;

    setupUi(this);

    setWindowTitle(cWindowName);

    setWindowIcon(QIcon(cWindowIconFilePath));

    getStatusConnect = false;

    #ifdef WEBENGINE_USED
      CesiumWebView = new QWebEngineView();
      horizontalLayout->removeWidget(label);
      delete label;
      label = nullptr;
      horizontalLayout->removeItem(horizontalSpacer);
      delete horizontalSpacer;
      horizontalSpacer = nullptr;
      horizontalLayout->removeItem(horizontalSpacer_2);
      delete horizontalSpacer_2;
      horizontalSpacer_2 = nullptr;
      horizontalLayout->setSpacing(0);
    #else
      CesiumWebView = new QWidget(this);
    #endif

    horizontalLayout->addWidget(CesiumWebView);

    #ifdef WEBENGINE_USED
      CesiumWebView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      //CesiumWebView->page()->profile()->setCachePath("C:/cash.txt");
      //CesiumWebView->page()->profile()->setCachePath("/var/share/www/html/cesium/cash.txt");
      //CesiumWebView->history()->clear();
      //CesiumWebView->page()->history()->clear();

      CesiumWebView->settings()->setAttribute(QWebEngineSettings::WebGLEnabled, true);
    #endif

    CesiumWebView->show();
}


TGISViewer::~TGISViewer()
{
    if(CesiumWebView != nullptr)
    {
        CesiumWebView->close();

        delete CesiumWebView;

        CesiumWebView = nullptr;
    }
}


bool TGISViewer::StatusConnect()
{
  return getStatusConnect;
}


bool TGISViewer::setConnectionGIS(QString HTTPMainStringGIS, QString ServerGIS, QString PortGIS)
{
    bool rez = false;

    if(HTTPMainStringGIS.contains(QString("http://"), Qt::CaseInsensitive))
    {
        CesiumAddressString = HTTPMainStringGIS;
    }
    else
    {
        if(ServerGIS != nullptr)
        {
            if(PortGIS != nullptr)
            {
                if(HTTPMainStringGIS.indexOf(QChar('/')) == 0) CesiumAddressString = QString("http://") + ServerGIS + QString(":") + PortGIS + HTTPMainStringGIS;
                else CesiumAddressString = QString("http://") + ServerGIS + QString(":") + PortGIS + QString("/") + HTTPMainStringGIS;
            }
            else
            {
                if(HTTPMainStringGIS.indexOf(QChar('/') == 0)) CesiumAddressString = QString("http://") + ServerGIS + HTTPMainStringGIS;
                else CesiumAddressString = QString("http://") + ServerGIS + QString("/") + HTTPMainStringGIS;
            }
        }
        else
        {
            if(HTTPMainStringGIS.indexOf(QChar('/') == 0)) CesiumAddressString = QString("http:/") + HTTPMainStringGIS;
            else CesiumAddressString = QString("http://") + HTTPMainStringGIS;
        }
    }


    #ifdef WEBENGINE_USED
        CesiumWebView->load(QUrl(getAddressString()));
    #endif

    rez = true;

    return rez;
}


QString TGISViewer::getAddressString()
{
    return CesiumAddressString;
}
