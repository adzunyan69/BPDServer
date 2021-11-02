#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QDomDocument>
#include <QApplication>
#include "settings.h"
#include "DatabaseAccessModule/trackinfo.h"

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);
    void setSocket(QTcpSocket *socket);
    static int maxId;
private:
    QTcpSocket *mSocket = nullptr;

    int id;
    QByteArray query;
    QString currentAssetnum;
    TrackInfo trackInfo;
    int km, m;
    QString getResponseXML();
    QString errorXML(QString errorText, int code = 1);
    QString responseXML();

    QString trackInfoByCoordXML(QDomElement &root);
    void createBuffer(QString assetNum);
    bool checkKm(int km);
    QString createXMLByCoord();

private slots:
    void slotReadyRead();
    void slotDisconnected();
    void logFromTrackInfo(QString msg);
signals:
    void receivedText(QString text);
    void log(QString msg);
    void clientDisconnected();
};

#endif // CLIENT_H
