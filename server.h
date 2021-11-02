#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMessageBox>
#include <QDomDocument>
#include <QApplication>
#include "settings.h"
#include "client.h"
#include "DatabaseAccessModule/trackinfo.h"
class MyTcpServer : public QObject
{
    Q_OBJECT
public:
    explicit MyTcpServer(QObject *parent = 0);

    void startServer();

public slots:
    void slotNewConnection();

private:
    QTcpServer * mTcpServer;

    QVector<Client*> clients;

    bool serverStarted = false;
signals:
    void text(QString text);
    void log(QString text);
};

#endif // SERVER_H
