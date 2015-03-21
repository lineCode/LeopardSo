#pragma once

#include <windows.h>

class KCriticalSesion
{
public:
    KCriticalSesion();
    ~KCriticalSesion();

    void Lock();
    void Unlock();

private:
    CRITICAL_SECTION m_Session;
};

