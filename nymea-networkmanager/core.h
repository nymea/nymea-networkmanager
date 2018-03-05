#ifndef CORE_H
#define CORE_H

#include <QObject>

#include "networkmanager.h"

class Core : public QObject
{
    Q_OBJECT
public:
    static Core* instance();
    void destroy();


private:
    explicit Core(QObject *parent = nullptr);
    static Core *s_instance;

    NetworkManager *m_networkManager = nullptr;

signals:

public slots:


};

#endif // CORE_H
