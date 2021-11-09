#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    checkSettings();
    createTrayIcon();

    server = new MyTcpServer(this);

    connect(server, SIGNAL(text(QString)), this, SLOT(textReceived(QString)));
    connect(server, SIGNAL(log(QString)), this, SLOT(log(QString)));
    server->startServer();

}

void MainWindow::closeEvent(QCloseEvent * event)
{
    if(this->isVisible()){
        event->ignore();
        this->hide();
    }
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason){
    case QSystemTrayIcon::Trigger:
        if(!this->isVisible()){
            this->show();
        } else {
            this->hide();
        }
    default:
        break;
    }
}

void MainWindow::createTrayIcon()
{


    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(this->style()->standardIcon(QStyle::SP_FileDialogDetailedView));
    trayIcon->setToolTip("Сервер паспортных данных");
    QMenu * menu = new QMenu(this);
    QAction * quitAction = new QAction(trUtf8("Закрыть"), this);
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));
    menu->addAction(quitAction);
    trayIcon->setContextMenu(menu);
    trayIcon->show();
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
}

void MainWindow::checkSettings()
{
    if(Settings::read(Param::AvitrackDB).isEmpty())
        Settings::write(Param::AvitrackDB, QApplication::applicationDirPath() + "/ApBaze.db");
    if(Settings::read(Param::PORT).isEmpty())
        Settings::write(Param::PORT, "2426");
}

void MainWindow::textReceived(QString text)
{
    static int i = 0;
    qDebug() << "test: " << i++;
    qDebug() << "Size: " << text.size();
    ui->textReceive->append(text + "\n");
}


void MainWindow::log(QString text)
{
    if(ui->textReceive->toPlainText().size() >= 1000000)
        ui->textReceive->clear();
    ui->textReceive->append(text);
    qInfo() << text;
}

MainWindow::~MainWindow()
{
    delete ui;
}

