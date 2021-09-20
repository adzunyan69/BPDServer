#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    server = new MyTcpServer(this);

    connect(server, SIGNAL(text(QString)), this, SLOT(textReceived(QString)));
    connect(server, SIGNAL(log(QString)), this, SLOT(log(QString)));
    server->startServer();


}

void MainWindow::timerSlot()
{

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
    static int i = 0;
    ui->textReceive->append(text);
}
MainWindow::~MainWindow()
{
    delete ui;
}

