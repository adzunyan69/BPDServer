#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include <QObject>

class RequestHandler : public QObject
{
    Q_OBJECT
public:
    explicit RequestHandler(QObject *parent = nullptr);

signals:

};

#endif // REQUESTHANDLER_H
