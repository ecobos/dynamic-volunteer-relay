#include "mainwindow.h"
#include <QApplication>
#include <QLoggingCategory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //QLoggingCategory::setFilterRules("qt.network.ssl.warning=false");

    // Create a Window Object and show it
    MainWindow w;
    w.show();

    return a.exec();
}
