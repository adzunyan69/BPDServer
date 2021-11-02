#ifndef TRACKINFO_H
#define TRACKINFO_H

#include <QObject>
#include "DatabaseAccessModule/databaseaccess.h"


struct TrackItem
{
    enum TrackItemType{ KM, STR, STAN, PCH, MOST, MOV, CUR, SPD, SLEEPER, CLASS, SCR, RAIL } type;

    QString name;
    QString numb;

    int km = -1;
    int beginKM = -1;
    int beginM = -1;
    int oKM = -1;
    int oM = -1;
    int endKM = -1;
    int endM = -1;
    int len = -1;


    int absBegin;
    int absO;
    int absEnd;

    bool operator<(const TrackItem &rhs)
    {
        return (this->beginKM < rhs.beginKM || (this->beginKM == rhs.beginKM) && this->beginM < rhs.beginM);
    }

};

class TrackInfo : public QObject
{
    Q_OBJECT

    DatabaseAccess dba;

    QString sqlPath;

    QString assetNum;

    QString dirCode;
    QString trackNum;
    QString dirName;

    QMap<TrackItem::TrackItemType, QVector<TrackItem>> itemsMap;
public:
    explicit TrackInfo(QObject *parent = nullptr);

    bool setAndOpenDatabase(QString databaseName, QString _sqlPath);
    void setAssetNum(QString _assetNum);
    void setDirInfo(QString _dirCode, QString _trackNum);
    QString getDirCode() { return dirCode; }
    QString getTrackNum() { return trackNum; }
    QString getDirName() { return dirName; }
    QVector<TrackItem> getVec(QString sqlName);

    bool checkKm(int km);
    QPair<int, int> getMinMaxKm();
    
    QMap<TrackItem::TrackItemType, QVector<TrackItem>> createItemsMap();
    QMap<TrackItem::TrackItemType, TrackItem> getItemsByCoord(int km, int m);

    void clear() { itemsMap.clear(); }

public slots:
    void errorFromDBA(QString msg);
signals:
    void error(QString msg);
    void log(QString msg);

};


#endif // TRACKINFO_H
