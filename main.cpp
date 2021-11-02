#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
#ifdef QT_DEBUG
    w.show();
#endif
    return a.exec();
}
