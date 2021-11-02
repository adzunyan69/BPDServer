#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include "settings.h"
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
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void createTrayIcon();
    void checkSettings();
public slots:

protected:
    void closeEvent(QCloseEvent * event);

private:
    Ui::MainWindow *ui;
    QSystemTrayIcon         * trayIcon;

private slots:
    void textReceived(QString text);
    void log(QString text);
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

};
#endif // MAINWINDOW_H
