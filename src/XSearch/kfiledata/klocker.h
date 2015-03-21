#pragma once

#include <windows.h>
#include "kcriticalsesion.h"

class KLocker
{
public:
    KLocker(KCriticalSesion& Session);
    ~KLocker();

private:
    KCriticalSesion& m_Session;
};
