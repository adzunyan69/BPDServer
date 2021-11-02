#include "client.h"

int Client::maxId { 0 };

Client::Client(QObject *parent) : QObject(parent)
{
    id = maxId++;
}

void Client::setSocket(QTcpSocket *socket)
{
    log(QString("Client %1: setSocket").arg(id));
    mSocket = socket;

    connect(mSocket,    &QTcpSocket::readyRead,
            this,       &Client::slotReadyRead);
    connect(mSocket,    &QTcpSocket::disconnected,
            this,       &Client::slotDisconnected);
    connect(&trackInfo, &TrackInfo::error,
            this,       &Client::logFromTrackInfo);
    connect(&trackInfo, &TrackInfo::log,
            this,       &Client::logFromTrackInfo);

    trackInfo.setAndOpenDatabase(Settings::read(Param::AvitrackDB),\
                                          QApplication::applicationDirPath() + "/sql");


}

void Client::slotReadyRead()
{
    log(QString("Client %1: Client::slotReadyRead()").arg(id));
    log(QString("Client %1: Bytes available: ").arg(id) + QString::number(mSocket->bytesAvailable()));
    log(QString("Client %1: From client ").arg(id) + QString::number(id));
    while(mSocket->bytesAvailable()>0)
    {

        QByteArray buffer = mSocket->readAll();
        log(QString("Client %1: Query:\n").arg(id));
        receivedText(buffer);

        // ***
        // TODO: Чтение длинных кусков сообщения от клиента
        // ***

        query = buffer;

        QString responseXML = getResponseXML();

        mSocket->write(responseXML.toUtf8());
    }

}

void Client::slotDisconnected()
{
    log(QString("Client %1: Client::slotDisconnected())").arg(id));
    trackInfo.clear();
    mSocket->close();

    emit clientDisconnected();
}

QString Client::getResponseXML()
{
    if(query.contains("</query>") == false)
        return errorXML("Error: </query> not found.");

    return responseXML();

}

QString Client::errorXML(QString errorText, int code)
{
    log(QString("Client %1: ").arg(id) + errorText);
    return QString("<response>\n"\
                   "\t<code>%1</code>\n"\
                   "\t<error_text>%2</error_text>\n"\
                   "\t<track_code>%3</track_code>\n"\
                   "</response>\n")\
            .arg(QString::number(code))\
            .arg(errorText)\
            .arg(currentAssetnum);
}

QString Client::responseXML()
{
    log(QString("Client %1: responseXML").arg(id));
    QDomDocument doc;
    bool isDocCreated;

    isDocCreated = doc.setContent(query);
    if(!isDocCreated)
        return errorXML("Error: doc.setContent.");

    QDomElement root = doc.documentElement();
    QDomElement type = root.firstChildElement("type");
    if(type.isNull())
        return errorXML("Type tag is null.");

    if(type.text() == "0")
    {
        log(QString("Client %1: Type: 0").arg(id));

        return trackInfoByCoordXML(root);


    }
    else
    {
        return errorXML("Unknown type: " + type.text());
    }
}

QString Client::trackInfoByCoordXML(QDomElement &root)
{
    log(QString("Client %1: trackInfoByCoordXML").arg(id));
    QDomElement trackCode = root.firstChildElement("track_code");
    if(trackCode.isNull())
        return errorXML("Track_code is null.");

    if(trackCode.text() != currentAssetnum)
    {
        if(trackCode.text().isEmpty())
            return errorXML("Empty assetnum.");

        log(QString("Client %1: New assetnum: ").arg(id) + trackCode.text());
        log(QString("Client %1: Creating buffer.").arg(id));

        createBuffer(trackCode.text());

    }

    QDomElement kmTag = root.firstChildElement("km");
    QDomElement mTag = root.firstChildElement("m");

    if(kmTag.isNull() || mTag.isNull()
            ||kmTag.text().isEmpty() || mTag.text().isEmpty())
        return errorXML("Invalid <km> or <m> tags.");

    if(checkKm(kmTag.text().toInt()) == false)
        return errorXML("Path coord is out of range.", 2);

    km = kmTag.text().toInt();
    m = mTag.text().toInt();

    log(QString("Client %1: Km: ").arg(id) + kmTag.text());
    log(QString("Client %1: M: ").arg(id) + mTag.text());

    return createXMLByCoord();
}

void Client::createBuffer(QString assetNum)
{
    log(QString("Client %1: createBuffer").arg(id));
     currentAssetnum = assetNum;
     trackInfo.setAssetNum(assetNum);
     trackInfo.createItemsMap();
}


bool Client::checkKm(int km)
{
    log(QString("Client %1: checkKm").arg(id));
    return trackInfo.checkKm(km);
}


QString Client::createXMLByCoord()
{
    log(QString("Client %1: createXMLByCoord").arg(id));
    QTime time5 = QTime::currentTime();
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



     log(QString("Client %1: ").arg(id) + QString::number(km) + " км " + QString::number(m) + " м");
     log(QString("Client %1: ").arg(id) + "BeginKM / BeginM / Name");
     log(QString("Client %1: ").arg(id) + QString() + "PCH: " + QString::number(items[TrackItem::PCH].beginKM) + "; " + QString::number(items[TrackItem::PCH].beginM) + "; " + items[TrackItem::PCH].name);
     log(QString("Client %1: ").arg(id) + QString() + "STAN: " + QString::number(items[TrackItem::STAN].beginKM) + "; " + QString::number(items[TrackItem::STAN].beginM) + "; " + items[TrackItem::STAN].name);
     log(QString("Client %1: ").arg(id) + QString() + "CLASS: " + QString::number(items[TrackItem::CLASS].beginKM) + "; " + QString::number(items[TrackItem::CLASS].beginM) + "; " + items[TrackItem::CLASS].name);
     log(QString("Client %1: ").arg(id) + QString() + "CUR: " + QString::number(items[TrackItem::CUR].beginKM) + "; " + QString::number(items[TrackItem::CUR].beginM) + "; " + items[TrackItem::CUR].oKM);
     log(QString("Client %1: ").arg(id) + QString() + "SPEED: " + QString::number(items[TrackItem::SPD].beginKM) + "; " + QString::number(items[TrackItem::SPD].beginM) + "; " + items[TrackItem::SPD].name);
     log(QString("Client %1: ").arg(id) + QString() + "SLEEPER: " + QString::number(items[TrackItem::SLEEPER].beginKM) + "; " + QString::number(items[TrackItem::SLEEPER].beginM) + "; " + items[TrackItem::SLEEPER].name);
     log(QString("Client %1: ").arg(id) + QString() + "SCR: " + QString::number(items[TrackItem::SCR].beginKM) + "; " + QString::number(items[TrackItem::SCR].beginM) + "; " + items[TrackItem::SCR].name);
     log(QString("Client %1: ").arg(id) + QString() + "RAILS: " + QString::number(items[TrackItem::RAIL].beginKM) + "; " + QString::number(items[TrackItem::RAIL].beginM) + "; " + items[TrackItem::RAIL].name);

     log(QString("Client %1: ").arg(id) + "Time: " + QString::number(time5.msecsTo(QTime::currentTime())));

     return xml;
}

void Client::logFromTrackInfo(QString msg)
{
    emit log(QString("Client %1: ").arg(id) + "TrackInfo: " + msg);
}
