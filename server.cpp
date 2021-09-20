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
     connect(&trackInfo, SIGNAL(error(QString)), SIGNAL(log(QString)));
     if(!mTcpServer->listen(QHostAddress::Any, 2426)){
         log("Server start: fail");
         serverStarted = false;
     } else {
         log("Server start: ok");
         serverStarted = true;
         trackInfo.setAndOpenDatabase(QApplication::applicationDirPath() + "/ApBaze.db",\
                                      QApplication::applicationDirPath() + "/sql_full");

     }
}

void MyTcpServer::slotNewConnection()
{
    log("new connection");
    // QTcpSocket *mTcpSocket = mTcpServer->nextPendingConnection();

    Client *client = new Client(this);
    client->setSocket(mTcpServer->nextPendingConnection());
    connect(client, &Client::log,
            this,   &MyTcpServer::log);
    connect(client, &Client::receivedText,
            this,   &MyTcpServer::text);
    clients.push_back(client);
    log(QString::number(clients.size()));
//    connect(mTcpSocket, &QTcpSocket::readyRead, this, &MyTcpServer::slotServerRead);
//    connect(mTcpSocket, &QTcpSocket::disconnected, this, &MyTcpServer::slotClientDisconnected);
}

void MyTcpServer::slotServerRead()
{
    QTcpSocket *socket = (QTcpSocket*)sender();
    log("Bytes available: " + QString::number(socket->bytesAvailable()));
    while(socket->bytesAvailable()>0)
    {

        QByteArray buffer = socket->readAll();
        log("Query:\n");
        text(buffer);

        code = 1;
        if(buffer.contains("</query>"))
        {
            if(parseQuery(buffer) == true)
            {
                code = 0;
                log("Correct parsing.");

            }
            else
            {
                log("Error: parseQuery().");
            }


            QString xml = getResponseXml();
            socket->write(xml.toUtf8());
        }
        else
        {
            log("Error: </query> not found...");
        }

    }
}

void MyTcpServer::slotClientDisconnected()
{
    QTcpSocket *socket = (QTcpSocket*)sender();
    log("client disconnected");
    socket->close();
}

bool MyTcpServer::parseQuery(QString xml)
{
    QDomDocument doc;
    bool isDocCreated;

    isDocCreated = doc.setContent(xml);
    if(!isDocCreated)
    {
        log("Ошибка парсинга xml.");
        error_text = "Error: doc.setContent.";
        return false;
    }

    QDomElement root = doc.documentElement();
    QDomElement type = root.firstChildElement("type");
    if(type.isNull())
    {
        log("Type tag is null.");
        error_text = "Type tag is null.";
        return false;
    }
    if(type.text() == "0")
    {
        log("Type: 0");

        QDomElement trackCode = root.firstChildElement("track_code");
        if(trackCode.isNull())
        {
            log("track_code is null.");
            error_text = "Track_code is null.";
            return false;
        }
        if(trackCode.text() != currentAssetnum)
        {
            if(trackCode.text().isEmpty())
            {
                log("Empty assetnum.");
                error_text = "Empty assetnum.";
                return false;
            }


            log("New assetnum: " + trackCode.text());
            log("Creating buffer.");
            createBuffer(trackCode.text());

        }

        QDomElement kmTag = root.firstChildElement("km");
        QDomElement mTag = root.firstChildElement("m");

        if(kmTag.isNull() || mTag.isNull()
                ||kmTag.text().isEmpty() || mTag.text().isEmpty())
        {
            log("Invalid <km> or <m> tags.");
            error_text = "Invalid <km> or <m> tags.";
            return false;
        }

        if(checkKm(kmTag.text().toInt()) == false)
        {
            code = 2;
            log("Error: <km> is out of range.");
            error_text = "Path coord is out of range.";
            return false;
        }

        km = kmTag.text().toInt();
        m = mTag.text().toInt();

        log("Km: " + kmTag.text());
        log("M: " + mTag.text());

        return true;
    }
    else
    {
        log("Unknown type: " + type.text());
        error_text = "Unknown type: " + type.text() + ".";
        return false;
    }

}

void MyTcpServer::createBuffer(QString assetNum)
{
     currentAssetnum = assetNum;
     trackInfo.setAssetNum(assetNum);
     trackInfo.createItemsMap();
}

bool MyTcpServer::checkKm(int km)
{
    return trackInfo.checkKm(km);
}

QString MyTcpServer::getResponseXml()
{
    QTime time5 = QTime::currentTime();

    if(code != 0)
    {
        return QString("<response>\n"\
                       "\t<code>%1</code>\n"\
                       "\t<error_text>%2</error_text>\n"\
                       "\t<track_code>%3</track_code>\n"\
                       "</response>\n")\
                .arg(QString::number(code))\
                .arg(error_text)\
                .arg(currentAssetnum);

    }

    QMap<TrackItem::TrackItemType, TrackItem> items = trackInfo.getItemsByCoord(km, m);

    QString xml = QString("<response>\n"\
                  "\t<code>0</code>\n"\
                  "\t<track_code>%1</track_code>\n"\
                  "\t<km>%2</km>\n"\
                  "\t<m>%3</m>\n"\
                  "\t<pch>%4</pch>\n"\
                  "\t<trackClass>%5</trackClass>\n"\
                  "\t<curve>%6</curve>\n"\
                  "\t<velocity>%7</velocity>\n"\
                  "\t<sleeper>%8</sleeper>\n"\
                  "\t<bonding>%9</bonding>\n"\
                  "\t<rails>%10</rails>\n"\
                  "</response>")\
                .arg(currentAssetnum)\
                .arg(km)\
                .arg(m)\
                .arg(items[TrackItem::PCH].name)\
                .arg(items[TrackItem::CLASS].name)\
                .arg(items[TrackItem::CUR].oKM)\
                .arg(items[TrackItem::SPD].name)\
                .arg(items[TrackItem::SLEEPER].name)\
                .arg(items[TrackItem::SCR].name)\
                .arg(items[TrackItem::RAIL].name);



    log(QString::number(km) + " км " + QString::number(m) + " м");
    log("BeginKM / BeginM / Name");
    log(QString() + "PCH: " + QString::number(items[TrackItem::PCH].beginKM) + "; " + QString::number(items[TrackItem::PCH].beginM) + "; " + items[TrackItem::PCH].name);
    log(QString() + "STAN: " + QString::number(items[TrackItem::STAN].beginKM) + "; " + QString::number(items[TrackItem::STAN].beginM) + "; " + items[TrackItem::STAN].name);
    log(QString() + "CLASS: " + QString::number(items[TrackItem::CLASS].beginKM) + "; " + QString::number(items[TrackItem::CLASS].beginM) + "; " + items[TrackItem::CLASS].name);
    log(QString() + "CUR: " + QString::number(items[TrackItem::CUR].beginKM) + "; " + QString::number(items[TrackItem::CUR].beginM) + "; " + items[TrackItem::CUR].oKM);
    log(QString() + "SPEED: " + QString::number(items[TrackItem::SPD].beginKM) + "; " + QString::number(items[TrackItem::SPD].beginM) + "; " + items[TrackItem::SPD].name);
    log(QString() + "SLEEPER: " + QString::number(items[TrackItem::SLEEPER].beginKM) + "; " + QString::number(items[TrackItem::SLEEPER].beginM) + "; " + items[TrackItem::SLEEPER].name);
    log(QString() + "SCR: " + QString::number(items[TrackItem::SCR].beginKM) + "; " + QString::number(items[TrackItem::SCR].beginM) + "; " + items[TrackItem::SCR].name);
    log(QString() + "RAILS: " + QString::number(items[TrackItem::RAIL].beginKM) + "; " + QString::number(items[TrackItem::RAIL].beginM) + "; " + items[TrackItem::RAIL].name);

    log("Time: " + QString::number(time5.msecsTo(QTime::currentTime())));

    return xml;
}
