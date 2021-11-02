#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QMap>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>

enum Param{
    AvitrackDB,
    PORT
};

class Settings : QObject
{
    Q_OBJECT
private:
    static QMap<Param, QString> settingsMap;
public:
    Settings() = delete;

    static void write(Param param, const QString &value);
    static QString read(Param param);



};


#endif // SETTINGS_H
