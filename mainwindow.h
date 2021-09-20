#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "server.h"
#include "DatabaseAccessModule/databaseaccess.h"
#include "DatabaseAccessModule/trackinfo.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    MyTcpServer *server;
    // TrackInfo trackInfo;
    // QTimer timer;
    // QTcpSocket *server;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:
    void textReceived(QString text);
    void log(QString text);
    void timerSlot();
    // void slotProcessDatagrams();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
