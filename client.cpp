#include "client.h"

int Client::maxId { 0 };

Client::Client(QObject *parent) : QObject(parent)
{
    id = maxId++;
}

void Client::setSocket(QTcpSocket *socket)
{
    mSocket = socket;

    connect(mSocket,    &QTcpSocket::readyRead,
            this,       &Client::slotReadyRead);
    connect(mSocket,    &QTcpSocket::disconnected,
            this,       &Client::slotDisconnected);

}

void Client::slotReadyRead()
{
    log("Client::slotReadyRead()");
    log("Bytes available: " + QString::number(mSocket->bytesAvailable()));
    log("From client " + QString::number(id));
    while(mSocket->bytesAvailable()>0)
    {

        QByteArray buffer = mSocket->readAll();
        log("Query:\n");
        receivedText(buffer);

//        code = 1;
//        if(buffer.contains("</query>"))
//        {
//            if(parseQuery(buffer) == true)
//            {
//                code = 0;
//                log("Correct parsing.");

//            }
//            else
//            {
//                log("Error: parseQuery().");
//            }


//            QString xml = getResponseXml();
//            socket->write(xml.toUtf8());
//        }
//        else
//        {
//            log("Error: </query> not found...");
//        }

    }

}

void Client::slotDisconnected()
{
    log("Client::slotDisconnected()");
    mSocket->close();

    emit clientDisconnected();
}
