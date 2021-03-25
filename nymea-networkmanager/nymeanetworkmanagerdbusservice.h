#ifndef NYMEANETWORKMANAGERDBUSSERVICE_H
#define NYMEANETWORKMANAGERDBUSSERVICE_H

#include <QObject>
#include <QDBusConnection>

class NymeaNetworkManagerDBusService : public QObject
{
    Q_OBJECT
public:
    explicit NymeaNetworkManagerDBusService(QDBusConnection::BusType busType, QObject *parent = nullptr);

public slots:
    Q_SCRIPTABLE void enableBluetoothServer();

signals:
    void enableBluetoothServerCalled();

private:
    QDBusConnection m_connection;

};

#endif // NYMEANETWORKMANAGERDBUSSERVICE_H
