/*
 * Edgar A. Cobos
 * CSULB Fall 2015
 * CECS 476 - Computer Security
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dynamicvolunteer.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    /*
     * This class has to include the Q_OBJECT macro as defined
     *  by the QT Documentation:http://doc.qt.io/qt-5/qobject.html#Q_OBJECT
     *
     * "Notice that the Q_OBJECT macro is mandatory for any object that implements
     *  signals, slots or properties. You also need to run the Meta Object Compiler
     *  on the source file. We strongly recommend the use of this macro in all
     *  subclasses of QObject regardless of whether or not they actually use signals,
     *  slots and properties, since failure to do so may lead certain functions to
     *  exhibit strange behavior."
     *
     *  In case you were wondering about the inheritance of this class:
     *   QObject -> QWidget -> QMainWindow -> MainWindow (This class)
     */
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void stop();

public slots:
    void toggle();

private:
    Ui::MainWindow *ui;
    DynamicVolunteer *mVolunteer;

    void startProxy();
    void stopProxy();
};

#endif // MAINWINDOW_H
