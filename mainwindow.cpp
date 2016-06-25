/*
 * Edgar A. Cobos
 * CSULB Fall 2015
 * CECS 476 - Computer Security
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"

/**
 * Class Object Constructor. Bind the UI elements with local objects
 *
 * @brief MainWindow::MainWindow
 * @param parent
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Bind the the UI Label with the a local object
    ui->setupUi(this);

    // Initialize the UI Label's content
    ui->statusLabel->setText(tr("Status: Stopped"));

    // Bind the a button's actions with a handling method
    connect(ui->proxyButton, SIGNAL(clicked()), this, SLOT(toggle()));
}

/**
 * Class object destructor
 *
 * @brief MainWindow::~MainWindow
 */
MainWindow::~MainWindow()
{
    delete mController;
    delete ui;
}

/**
 * Starts the proxy in response to a user's explicit request
 *
 * @brief MainWindow::StartProxy
 */
void MainWindow::startProxy(){
    ui->statusLabel->setText(tr("Status: Running"));

    /* Only create a new object if one doesn't exist.
    *  This is checked because a user is able to click the
    *  "start" button at any point of the proxy's state. Having
    *  this check will ensure that a previously created object is
    *  not overwritten by a new one.
    */

   mController = new Controller(this);
   qDebug() << "Created a new ClientConnection object";

   mController->startUp();
   // Update the toggle button with the next state
   ui->proxyButton->setText(tr("Stop"));
}

/**
 * Stops the proxy in response to a user's explicit request
 *
 * @brief MainWindow::StopProxy
 */
void MainWindow::stopProxy(){
    ui->statusLabel->setText(tr("Status: Stopped"));
    emit stop();
    mController->deleteLater(); // Deletes the object being pointed at
    mController = NULL; // Also remove pointer address
    qDebug() << "stop listening fired";

    // Update the toggle button with the next state
    ui->proxyButton->setText(tr("Start Volunteering"));
}

void MainWindow::toggle(){
    if(!mController){
        this->startProxy();
    }else {
        this->stopProxy();
    }
}
