#include "mainwindow.h"
#include <QApplication>
#include "settings.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.hide();
    
    return a.exec();
    Settings::shutdown();
}
