#include <cstdlib>
#include <iostream>
#include "kthreadcontroller.h"

KThreadController::KThreadController(HWND hWnd) : 
nDriveNum(0), 
nDriveNumForMonitor(0), 
nDriveNumForCleanner(0),
m_hWnd(hWnd)
{
    _InitDriveName();
    _InitUSNList();
    _CreateThread();
}

void KThreadController::_InitDriveName()
{
    CStringW strDrive;
    TCHAR cChar = BEGIN_DRIVE_NAME;

    for (int i = 0; i < MAX_DRIVE_COUNT; ++i)
    {
        strDrive.Format(L"%c", cChar);
        m_vDriveName.push_back(strDrive.GetString());
        cChar = cChar + 1;
    }
}

void KThreadController::_InitUSNList()
{
    TCHAR cChar = BEGIN_DRIVE_NAME;
    CString strDrive;

    for (int i = 0; i < MAX_DRIVE_COUNT; ++i)
    {
        strDrive.Format(L"%c", cChar);
        m_USNs[i].SetDrive(strDrive);
        cChar = cChar + 1;
    }
}

void KThreadController::_CloseHandle()
{
    if (NULL != m_hNotifyThread)
    {
        ::CloseHandle(m_hNotifyThread);
    }
    if (NULL != m_phThread)
    {
        ::CloseHandle(m_phThread);
    }
    for (int i = 0; i < MAX_DRIVE_COUNT; ++i)
    {
        if (NULL != m_phCleannerThreads[i])
        {
            ::CloseHandle(m_phCleannerThreads[i]);
        }
        if(NULL != m_phEvents[i])
        {
            ::CloseHandle(m_phEvents[i]);
        }
    }
}

void KThreadController::_WaitForEventsToContinue()
{
    ::WaitForSingleObject(m_phThread, INFINITE);
}

KThreadController::~KThreadController()
{
    _WaitForEventsToContinue();
    _CloseHandle();
}

void KThreadController::_NotifyThreadMain()
{
    ::WaitForMultipleObjects(MAX_DRIVE_COUNT, 
        m_phEventsForSearch, 
        TRUE, 
        INFINITE);
    SendMessage(m_hWnd, UPDATE_FILE_CREATE_INDEX, NULL, TRUE);
}

void KThreadController::_WorkThreadMain(int nDriveNum)
{
    for (int i = 0; i < MAX_DRIVE_COUNT; ++i)
    {
        if (m_USNs[i].IsNTFS())
        {
            m_USNs[i].Init();
        }
        ::SetEvent(m_phEvents[i]);
        ::SetEvent(m_phEventsToCleanner[i]);
        ::SetEvent(m_phEventsForSearch[i]);
    }
}

void KThreadController::_CleannerThreadMain(int nDriveNum)
{
    ::WaitForSingleObject(m_phEventsToCleanner[nDriveNum], INFINITE);
    if (m_USNs[nDriveNum].IsNTFS())
    {
        m_USNs[nDriveNum].DeleteTempData();
    }
}

DWORD WINAPI KThreadController::_NotifyThreadFunc(LPVOID pUSNInfo)
{
    KThreadController* pThreadCtrl = reinterpret_cast<KThreadController*>(pUSNInfo);
    if(NULL == pThreadCtrl)
    {
        goto Exit0;
    }
    pThreadCtrl->_NotifyThreadMain();

Exit0:
    return 0;
}

DWORD WINAPI KThreadController::_CleannerThreadFunc(LPVOID pUSNInfo)
{
    int nNum = 0;
    KThreadController* pThreadCtrl = reinterpret_cast<KThreadController*>(pUSNInfo);
    if(NULL == pThreadCtrl)
    {
        goto Exit0;
    }
    nNum = ++pThreadCtrl->nDriveNumForCleanner;
    pThreadCtrl->_CleannerThreadMain(nNum - 1);

Exit0:
    return 0;
}

DWORD WINAPI KThreadController::_WorkThreadFunc(LPVOID pUSNInfo)
{
    int nNum = 0;
    KThreadController* pThreadCtrl = reinterpret_cast<KThreadController*>(pUSNInfo);
    if(NULL == pThreadCtrl)
    {
        goto Exit0;
    }
    nNum = ++pThreadCtrl->nDriveNum;
    pThreadCtrl->_WorkThreadMain(nNum - 1);

Exit0:
    return 0;
}

void KThreadController::_CreateNotifyThread()
{
    m_hNotifyThread = ::CreateThread(NULL,
        0,
        _NotifyThreadFunc,
        this,
        0,
        NULL);
}

void KThreadController::_CreateSearchThread()
{
    m_phThread = ::CreateThread(NULL,
        0,
        _WorkThreadFunc,
        this,
        0,
        NULL);
}

void KThreadController::_CreateCleannerThread()
{
    for (int i = 0; i < MAX_DRIVE_COUNT; ++i)
    {
        m_phCleannerThreads[i] = ::CreateThread(NULL,
            0,
            _CleannerThreadFunc,
            this,
            0,
            NULL);
    }
}

void KThreadController::_InitHandle()
{
    m_phThread = new HANDLE;
    m_phCleannerThreads = new HANDLE[MAX_DRIVE_COUNT];
    m_hNotifyThread = new HANDLE;
    m_phEvents = new HANDLE[MAX_DRIVE_COUNT];
    m_phEventsToCleanner = new HANDLE[MAX_DRIVE_COUNT];
    m_phEventsForSearch = new HANDLE[MAX_DRIVE_COUNT];
}

void KThreadController::_CreateThread()
{
    _InitHandle();
    _CreateEvent();
    _CreateNotifyThread();
    _CreateSearchThread();
    _CreateCleannerThread();
}

void KThreadController::_CreateEvent()
{
    for (int i = 0; i < MAX_DRIVE_COUNT; ++i)
    {
        m_phEventsForSearch[i] = ::CreateEvent(NULL, TRUE, FALSE, NULL);
        m_phEvents[i] = ::CreateEvent(NULL, TRUE, FALSE, NULL);
        m_phEventsToCleanner[i] = ::CreateEvent(NULL, TRUE, FALSE, NULL);
    }
}

BOOL KThreadController::FindFrnByName(CString& strName, FrnVector* pvFrn, BOOL bCasesensitive/*=FALSE*/)
{
    int nIndex = 0;

    if(NULL == pvFrn)
    {
        goto Exit0;
    }

    for (int i = 0; i < MAX_DRIVE_COUNT; ++i)
    {
        if (m_USNs[i].IsNTFSAfterCalc())
        {
            m_USNs[i].FindFrnByName(strName, pvFrn, bCasesensitive);
        }
    }

    for (FrnVector::iterator it = pvFrn->begin(); it != pvFrn->end(); ++it)
    {
        it->m_nIndex = nIndex++;
    }

Exit0:
    return TRUE;
}

void KThreadController::BuildPath(const DRIVE_AND_FRN& DriveAndFrn, const CString** ppstrPath)
{
    int nDriveNum = 0;
    if(NULL == ppstrPath)
    {
        goto Exit0;
    }
    nDriveNum = DriveAndFrn.m_cDriveName - BEGIN_DRIVE_NAME;
    m_USNs[nDriveNum].BuildPath(DriveAndFrn.m_dwlFrn, ppstrPath);

Exit0:
    return;
}