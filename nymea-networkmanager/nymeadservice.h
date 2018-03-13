#ifndef NYMEADSERVICE_H
#define NYMEADSERVICE_H

#include <QObject>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusServiceWatcher>

#include "pushbuttonagent.h"

class NymeadService : public QObject
{
    Q_OBJECT
public:
    explicit NymeadService(bool pushbuttonEnabled, QObject *parent = nullptr);

    bool available() const;

    void enableBluetooth(const bool &enable);
    void pushButtonPressed();

private:
    QDBusServiceWatcher *m_serviceWatcher = nullptr;
    PushButtonAgent *m_pushButtonAgent = nullptr;

    QDBusInterface *m_nymeadHardwareInterface = nullptr;
    QDBusInterface *m_nymeadHardwareBluetoothInterface = nullptr;

    bool m_pushbuttonEnabled = false;
    bool m_available = false;

    void setAvailable(const bool &available);

    bool init();

signals:
    void availableChanged(const bool &available);

private slots:
    void serviceRegistered(const QString &serviceName);
    void serviceUnregistered(const QString &serviceName);

};

#endif // NYMEADSERVICE_H
