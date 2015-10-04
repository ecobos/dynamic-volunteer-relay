#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Create a Window Object and show it
    MainWindow w;
    w.show();

    return a.exec();
}
