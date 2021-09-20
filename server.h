#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMessageBox>
#include <QDomDocument>
#include <QApplication>
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
    void slotServerRead();
    void slotClientDisconnected();

private:
    QTcpServer * mTcpServer;
    TrackInfo trackInfo;

    QVector<Client*> clients;

    bool serverStarted = false;
    int code;
    QString error_text;
    QString currentAssetnum = "";
    int km, m;

    bool parseQuery(QString xml);
    void createBuffer(QString assetNum);
    bool checkKm(int km);
    QString getResponseXml();
signals:
    void text(QString text);
    void log(QString text);
};

#endif // SERVER_H
