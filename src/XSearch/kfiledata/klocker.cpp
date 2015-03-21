#include <windows.h>
#include "kcriticalsesion.h"
#include "klocker.h"

KLocker::KLocker(KCriticalSesion& Session) : m_Session(Session)
{
    m_Session.Lock();
}

KLocker::~KLocker()
{
    m_Session.Unlock();
}
