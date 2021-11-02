#include "trackinfo.h"

// #define debug_info

TrackInfo::TrackInfo(QObject *parent) : QObject(parent)
{
    connect(&dba, SIGNAL(error(QString)), this, SLOT(errorFromDBA(QString)));
}

void TrackInfo::errorFromDBA(QString msg)
{
    log("Error from dba: " + msg);
    emit error(msg);
}

void TrackInfo::setAssetNum(QString _assetNum)
{
    log("AssetNum: " + _assetNum);
    if(_assetNum.isEmpty() == true)
    {
        log("Error: assetNum is empty");
        emit error("Ошибка: код пути пуст.");
        return;
    }

    if(_assetNum.size() <= 5)
    {
        log("Error: assetNum <= 5");
        emit error("Ошибка: некорректный код пути (" + _assetNum + "). Отредактируйте дерево проездов.");
        return;
    }
    QMap<QString, QVariant> bindValue;
    bindValue.insert(":ASSET_NUM", _assetNum);



    QSqlQuery query = dba.execQueryFileBind(sqlPath + "/ASSETNUM.sql",
                                            bindValue);

    if(query.size() == 0)
    {
        log("Error: assetNum isn't exist");
        emit error("Ошибка: код пути " + _assetNum + " не был найден в базе данных.");
        return;
    }
    query.first();
    setDirInfo(query.value("KOD").toString(), query.value("PUT").toString());

    log(dirCode + " " + trackNum);
}

void TrackInfo::setDirInfo(QString _dirCode, QString _trackNum)
{
    log("DirCode: " + _dirCode + ", trackNum: " + _trackNum);
    dirCode = _dirCode;
    trackNum = _trackNum;

    QMap<QString, QVariant> bindValue;
    bindValue.insert(":DIR_CODE", dirCode);
    QSqlQuery query = dba.execQueryFileBind(sqlPath + "/WAY.sql", bindValue);
    if(query.first() == false)
    {
        log("Error while getting dirName");
        log(query.lastError().text());
        emit error("Ошибка при выполнении запроса для конвертации кода пути в код направления и номер пути (" +
                   _dirCode + ", " + _trackNum + "\nТекст ошибки: " + query.lastError().text());
    }

    dirName = query.value("NAME").toString();
}

bool TrackInfo::setAndOpenDatabase(QString databaseName, QString _sqlPath)
{
    log("TEST: " + _sqlPath);
    if(dba.openDatabase(databaseName) == false)
    {
        log("Error: database error");
        log(dba.lastError());
        // emit error("Ошибка при открытии БД: " + dba.lastError());
        return false;
    }

    sqlPath = _sqlPath;
    return true;
}

QVector<TrackItem> TrackInfo::getVec(QString sqlName)
{
    QTime time = QTime::currentTime();
    QVector<TrackItem> result;
    QMap<QString, QVariant> bindValue;
    log("Sql path: " + sqlName);
    if(dirCode.isEmpty() && trackNum.isEmpty())
    {
        log("dir Code && track Num is empty");
        if(assetNum.isEmpty() == true)
        {
            log("Asset num is empty");
            // emit error("Отсутствует код пути и код направления.");
            return result;
        }
        bindValue.insert(":ASSET_NUM", assetNum);
    }
    else
        bindValue.insert(":DIR_CODE", dirCode);
        bindValue.insert(":TRACK_NUM", trackNum);

    QSqlQuery query = dba.execQueryFileBind(sqlPath +
                                            sqlName, bindValue);

    if(query.first() == false)
    {
        log(sqlName + " - нет данных");
        return QVector<TrackItem>();
    }
    do
    {
        TrackItem item;
        QString objType = query.value("OBJ").toString();

        if(objType == "STN")
        {
            item.type = TrackItem::STAN;
            item.name = query.value("NAME").toString();
            item.beginKM = query.value("B_KM").toInt();
            item.beginM = query.value("B_M").toInt();
            item.oKM = query.value("O_KM").toInt();
            item.oM = query.value("O_M").toInt();
            item.endKM = query.value("E_KM").toInt();
            item.endM = query.value("E_M").toInt();
        }
        else if(objType == "PCH")
        {
            item.type = TrackItem::PCH;
            item.name = query.value("type").toString() + " - " + query.value("NUMB").toString();
            item.beginKM = query.value("B_KM").toInt();
            item.beginM = query.value("B_M").toInt();
            item.endKM = query.value("E_KM").toInt();
            item.endM = query.value("E_M").toInt();
        }
        else if(objType == "MST")
        {
            item.type = TrackItem::MOST;
            item.beginKM = query.value("B_KM").toInt();
            item.beginM = query.value("B_M").toInt();
            item.endKM = query.value("E_KM").toInt();
            item.endM = query.value("E_M").toInt();

        }
        else if(objType == "MOV")
        {
            item.type = TrackItem::MOV;
            item.beginKM = query.value("B_KM").toInt();
            item.beginM = query.value("B_M").toInt();
            item.endKM = query.value("E_KM").toInt();
            item.endM = query.value("E_M").toInt();

        }
        else if(objType == "CUR")
        {
            item.type = TrackItem::CUR;
            item.beginKM = query.value("B_KM").toInt();
            item.beginM = query.value("B_M").toInt();
            item.oKM = query.value("O_KM").toInt();
            item.oM = query.value("O_M").toInt();
        }
        else if(objType == "SPD")
        {
            item.type = TrackItem::SPD;

            item.beginKM = query.value("B_KM").toInt();
            item.beginM = query.value("B_M").toInt();
            item.endKM = query.value("E_KM").toInt();
            item.endM = query.value("E_M").toInt();


            QString VVSK = query.value("VVSK").toString();
            QString VSK = query.value("VSK").toString();
            QString VSTR = query.value("VSTR").toString();
            QString VALL = query.value("VALL").toString();
            item.name = (VVSK == "0" ? "" : ("С" + VVSK + "/")) +
                        (VSK == "0" ? "" : ("Л" + VSK)) +
                        (VSTR == "0" ? "" : ("Ст" + VSTR + "/")) +
                        (VALL == "0" ? "" : ("А" + VALL)) +
                        ";" +
                        query.value("VPS").toString() + "/" +
                        query.value("VGR").toString() + "/" +
                        query.value("VPR").toString();

        }
        else if(objType == "SLP")
        {
            item.type = TrackItem::SLEEPER;

            item.beginKM = query.value("B_KM").toInt();
            item.beginM = query.value("B_M").toInt();
            item.endKM = query.value("E_KM").toInt();
            item.endM = query.value("E_M").toInt();

            item.name = query.value("NAME").toString();
            item.numb = query.value("NUMB").toString();
        }
        else if(objType == "CLS")
        {
            item.type = TrackItem::CLASS;

            item.beginKM = query.value("B_KM").toInt();
            item.beginM = query.value("B_M").toInt();
            item.endKM = query.value("E_KM").toInt();
            item.endM = query.value("E_M").toInt();

            item.name = query.value("CLASS_NOM").toString() + " " + query.value("GROUP_CODE").toString();
        }
        else if(objType == "SCR")
        {
            item.type = TrackItem::SCR;

            item.beginKM = query.value("B_KM").toInt();
            item.beginM = query.value("B_M").toInt();
            item.endKM = query.value("E_KM").toInt();
            item.endM = query.value("E_M").toInt();

            item.name = query.value("TYPE").toString();
        }
        else if(objType == "RAIL")
        {
            item.type = TrackItem::RAIL;

            item.beginKM = query.value("B_KM").toInt();
            item.beginM = query.value("B_M").toInt();
            item.endKM = query.value("E_KM").toInt();
            item.endM = query.value("E_M").toInt();

            item.name = query.value("TYPE").toString();
        }

        result.push_back(item);
    } while(query.next());


    log(QString() + "Query time: " + time.msecsTo(QTime::currentTime()) + " ms");
    log("Query  " + sqlName + " size: " + result.size());

    return result;


}

bool TrackInfo::checkKm(int km)
{
    QMap<QString, QVariant> bindValue;
    if(dirCode.isEmpty() && trackNum.isEmpty())
    {
        log("dir Code && track Num is empty");
        return false;
    }
    else
        bindValue.insert(":DIR_CODE", dirCode);
        bindValue.insert(":TRACK_NUM", trackNum);

    QSqlQuery query = dba.execQueryFileBind(sqlPath + "/KM.sql", bindValue);

    while(query.next())
    {
        if(query.value("KM").toInt() == km)
            return true;
    }

    return false;
}

QPair<int, int> TrackInfo::getMinMaxKm()
{
    QMap<QString, QVariant> bindValue;
    if(dirCode.isEmpty() && trackNum.isEmpty())
    {
        log("dir Code && track Num is empty");
        return QPair<int, int>(-1, -1);
    }
    else
        bindValue.insert(":DIR_CODE", dirCode);
        bindValue.insert(":TRACK_NUM", trackNum);

    QSqlQuery query = dba.execQueryFileBind(sqlPath + "/KM_MINMAX.sql", bindValue);

    if(query.next())
        return QPair<int, int>
                (query.value("MIN(KM)").toInt(),
                 query.value("MAX(KM)").toInt());
    else
        return QPair<int, int>(-1, -1);
}

QMap<TrackItem::TrackItemType, QVector<TrackItem>> TrackInfo::createItemsMap()
{
    itemsMap.clear();

    QVector<TrackItem> stan =  getVec( "/STAN.sql");
    QVector<TrackItem> pch =  getVec( "/PCH.sql");
    QVector<TrackItem> most =  getVec( "/MOST.sql");
    QVector<TrackItem> mov =  getVec( "/MOV.sql");
    QVector<TrackItem> cur =  getVec( "/CUR.sql");
    QVector<TrackItem> spd =  getVec( "/SPD.sql");
    QVector<TrackItem> sleeper = getVec("/SLEEPER.sql");
    QVector<TrackItem> cls = getVec("/CLASS.sql");
    QVector<TrackItem> scr = getVec("/SCR.sql");
    QVector<TrackItem> rails = getVec("/RAILS.sql");
    // calculateAbsCoord(sleeper, km);


//    std::sort(str.begin(), str.end());
//    std::sort(cur.begin(), cur.end());

    QTime time = QTime::currentTime();
    itemsMap.insert(TrackItem::STAN, stan);
    itemsMap.insert(TrackItem::PCH, pch);
    itemsMap.insert(TrackItem::MOST, most);
    itemsMap.insert(TrackItem::MOV, mov);
    itemsMap.insert(TrackItem::CUR, cur);
    itemsMap.insert(TrackItem::SPD, spd);
    itemsMap.insert(TrackItem::SLEEPER, sleeper);
    itemsMap.insert(TrackItem::CLASS, cls);
    itemsMap.insert(TrackItem::SCR, scr);
    itemsMap.insert(TrackItem::RAIL, rails);
    log("Time for insert: " + time.msecsTo(QTime::currentTime()));
    return itemsMap;
}

QMap<TrackItem::TrackItemType, TrackItem> TrackInfo::getItemsByCoord(int km, int m)
{
    /*1.       Предприятие (аббревиатура). ПЧ
    2.       Перегон/станция.
    3.       Путь (№ пути, главный/станционный).
    4.       Класс пути.
    5.       План пути (прямая/кривая, R, м).
    6.       Скорость установленная (пасс/груз/порож – км/ч)
    7.       Тип шпал.
    8.       Тип скреплений.
    9.       Тип рельса.
    10.  ИССО (информация о наличии ИССО и подходов к ним). */

    int coord = km * 1000 + m;
    QMap<TrackItem::TrackItemType, TrackItem> itemsByCoord;

    const QList<TrackItem::TrackItemType> keys = itemsMap.keys();
    for(const auto &key: keys)
    {
        const QVector<TrackItem> &items = itemsMap[key];
        if(key == TrackItem::STAN)
        {
            for(auto it = items.begin(); it != items.end(); ++it)
            {
                TrackItem stan = *it;
                if((it->beginKM * 1000 + it->beginM) < coord &&
                        (it->endKM * 1000 + it->endM) > coord)
                {
                    itemsByCoord.insert(key, stan);
                    break;
                }

                else if(it == items.begin() && (it->beginKM * 1000 + it->beginM) > coord)
                {
                    stan.name = " - " + it->name;
                    itemsByCoord.insert(key, stan);
                    break;
                }

                else if(it == (items.end() - 1) && (it->endKM * 1000 + it->endM) < coord)
                {
                    stan.name =  it->name + " - ";
                    itemsByCoord.insert(key, stan);
                    break;
                }

                else if((it->beginKM * 1000 + it->beginM) > coord)
                {
                    stan.name = (it - 1)->name + " - " + it->name;
                    itemsByCoord.insert(key, stan);
                    break;
                }

            }
        }

        else if(key == TrackItem::CUR)
        {
            for(auto it = items.begin(); it != items.end(); ++it)
            {

                if((it->beginKM * 1000 + it->beginM) <= coord &&
                        it != (items.end() - 1) &&
                        ((it + 1)->beginKM * 1000 + (it + 1)->beginM) > coord)
                {
                    itemsByCoord.insert(key, *it);
                    break;
                }
            }
        }
        else
        {
            for(const auto &item: items)
            {
                if((item.beginKM * 1000 + item.beginM) < coord &&
                        (item.endKM * 1000 + item.endM) > coord)
                {
                    itemsByCoord.insert(key, item);
                    break;
                }
            }
        }
    }

    return itemsByCoord;
}

