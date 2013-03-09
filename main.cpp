#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include "settings.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString locName = QLocale::system().name();
    QString locNameNoDialect = locName;
    {
        int p = locName.indexOf(QChar('_'));
        if(p > 0) {
            locNameNoDialect = locName.left(p);
        }
    }
    QTranslator transQt;
    if(!transQt.load("qt_" + locName,
                 QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
    {
        transQt.load("qt_" + locNameNoDialect,
                     QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    }
    a.installTranslator(&transQt);

    QTranslator transApp;
    if(!transApp.load("fgatray." + locName, ":/l")) {
        transApp.load("fgatray." + locNameNoDialect, ":/l");
    }
    a.installTranslator(&transApp);

    MainWindow w;
    w.hide();
    
    return a.exec();
    Settings::shutdown();
}
