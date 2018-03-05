#include "core.h"

Core* Core::s_instance = 0;

Core *Core::instance()
{
    if (!s_instance)
        s_instance = new Core();

    return s_instance;
}

void Core::destroy()
{
    if (s_instance)
        delete s_instance;

    s_instance = 0;
}

Core::Core(QObject *parent) :
    QObject(parent)
{

    m_networkManager = new NetworkManager(this);



}
