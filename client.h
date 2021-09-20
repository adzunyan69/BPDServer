#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>


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

private slots:
    void slotReadyRead();
    void slotDisconnected();
signals:
    void receivedText(QString text);
    void log(QString msg);
    void clientDisconnected();
};

#endif // CLIENT_H
