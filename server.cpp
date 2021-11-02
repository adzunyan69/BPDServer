#include <QDebug>
#include <QCoreApplication>
#include "server.h"

MyTcpServer::MyTcpServer(QObject *parent) : QObject(parent)
{

}

void MyTcpServer::startServer()
{
    mTcpServer = new QTcpServer(this);

     connect(mTcpServer, &QTcpServer::newConnection, this, &MyTcpServer::slotNewConnection);
     if(!mTcpServer->listen(QHostAddress::Any, Settings::read(Param::PORT).toInt())){
         log("Server start: fail");
         serverStarted = false;
     } else {
         log("Server start: ok");
         serverStarted = true;

     }
}

void MyTcpServer::slotNewConnection()
{
    log("new connection");

    Client *client = new Client(this);
    connect(client, &Client::log,
            this,   &MyTcpServer::log);
    connect(client, &Client::receivedText,
            this,   &MyTcpServer::text);
    client->setSocket(mTcpServer->nextPendingConnection());
    clients.push_back(client);

    log(QString::number(clients.size()));
}
