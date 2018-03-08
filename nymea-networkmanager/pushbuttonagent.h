#ifndef PUSHBUTTONAGENT_H
#define PUSHBUTTONAGENT_H

#include <QObject>
#include <QDBusConnection>

class PushButtonAgent : public QObject
{
    Q_OBJECT
public:
    explicit PushButtonAgent(QObject *parent = nullptr);

    bool init(QDBusConnection::BusType busType = QDBusConnection::SystemBus);

signals:
    Q_SCRIPTABLE void PushButtonPressed();

public slots:
    void sendButtonPressed();

};

#endif // PUSHBUTTONAGENT_H
