#include <windows.h>
#include "kcriticalsesion.h"

KCriticalSesion::KCriticalSesion()
{
    ::InitializeCriticalSection(&m_Session);
}

KCriticalSesion::~KCriticalSesion()
{
    ::DeleteCriticalSection(&m_Session);
}

void KCriticalSesion::Lock()
{
    ::EnterCriticalSection(&m_Session);
}

void KCriticalSesion::Unlock()
{
    ::LeaveCriticalSection(&m_Session);
}