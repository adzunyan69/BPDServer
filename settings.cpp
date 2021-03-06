#include "settings.h"

QMap<Param, QString> Settings::settingsMap =
{
    // Файлы
    {AvitrackDB, "Settings/DirBDRad"},

    // IP
    {PORT, "Settings/Port_BPDServer"}

};

void Settings::write(Param param, const QString &value)
{
    QSettings settings(QCoreApplication::applicationDirPath() + "/settings.conf", QSettings::IniFormat);
    settings.setValue( settingsMap[param], value);
    settings.sync();
}

QString Settings::read(Param param)
{
    QSettings settings(QCoreApplication::applicationDirPath() + "/settings.conf", QSettings::IniFormat);
    return settings.value(settingsMap[param]).toString();
}
