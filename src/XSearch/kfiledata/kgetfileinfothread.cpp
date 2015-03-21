#include "kgetfileinfothread.h"

KGetFileInfoThread::KGetFileInfoThread(KThreadController* pThreadController, 
                                       std::vector<DRIVE_AND_FRN>* pDriveAndFrn)
                                       : m_pDriveAndFrn(pDriveAndFrn)
                                       , m_pThreadController(pThreadController)
                                       , m_phSizeThreads(NULL)
                                       , m_phTimeThreads(NULL)
                                       , m_pFileSizeMap(NULL)
                                       , m_pFileTimeMap(NULL)
{
    m_phSizeThreads = new HANDLE[ms_nThreadNums];
    m_phTimeThreads = new HANDLE[ms_nThreadNums];
    for(int i = 0; i < ms_nThreadNums; ++i)
    {
        m_phTimeThreads[i] = NULL;
        m_phSizeThreads[i] = NULL;
    }
    m_nSegCount = static_cast<int>(pDriveAndFrn->size());
}

void KGetFileInfoThread::_CloseTimeThread()
{
    for (int i = 0; i < ms_nThreadNums; ++i)
    {
        if (m_phTimeThreads[i])
        {
            ::CloseHandle(m_phTimeThreads[i]);
            m_phTimeThreads[i] = NULL;
        }
    }
}

void KGetFileInfoThread::_CloseSizeThread()
{
    for (int i = 0; i < ms_nThreadNums; ++i)
    {
        if (m_phSizeThreads[i])
        {
            ::CloseHandle(m_phSizeThreads[i]);
            m_phSizeThreads[i] = NULL;
        }
    }
}

KGetFileInfoThread::~KGetFileInfoThread()
{
    _CloseSizeThread();	
    _CloseTimeThread();
}

void KGetFileInfoThread::GetFileSize(std::map<int, UINT64>* pFileSizeMap)
{
    if(NULL == pFileSizeMap)
    {
        goto Exit0;
    }
    m_pFileSizeMap = pFileSizeMap;
    _CreateFileSizeThread();
    ::WaitForMultipleObjects(ms_nThreadNums, m_phSizeThreads, TRUE, INFINITE);

Exit0:
    return;
}

void KGetFileInfoThread::GetFileTime(std::map<int, COleDateTime>* pFileTimeMap)
{
    if(NULL == pFileTimeMap)
    {
        goto Exit0;
    }
    m_pFileTimeMap = pFileTimeMap;
    _CreateFileTimeThread();
    ::WaitForMultipleObjects(ms_nThreadNums, m_phTimeThreads, TRUE, INFINITE);

Exit0:
    return;
}

void KGetFileInfoThread::_GetFileSizeMain(int nSegCount)
{
    int nStartIndex = nSegCount * m_nSegCount / ms_nThreadNums;
    const CString* pstrPath = NULL;
    CString strPath;

    for (int j = 0; j < m_nSegCount / ms_nThreadNums + 1; ++j, ++nStartIndex)
    {
        if (nStartIndex >= (nSegCount + 1) * m_nSegCount / ms_nThreadNums)
        {
            break;
        }

        m_pThreadController->BuildPath((*m_pDriveAndFrn)[nStartIndex], &pstrPath);
        if(NULL == pstrPath)
        {
            continue;
        }
        strPath.Format(L"%c:%s%s", (*m_pDriveAndFrn)[nStartIndex].m_cDriveName, (*pstrPath), (*((*m_pDriveAndFrn)[nStartIndex].m_pstrFileName)));

        {
            KLocker lock(m_Critical);
            m_pFileSizeMap->insert(std::make_pair((*m_pDriveAndFrn)[nStartIndex].m_nIndex, 
                _GetFileSize(strPath)));
        }
    }
}

void KGetFileInfoThread::_GetFileTimeMain(int nSegCount)
{
    int nStartIndex = nSegCount * m_nSegCount / ms_nThreadNums;
    const CString* pstrPath = NULL;
    CString strPath;
    for (int j = 0; j < m_nSegCount / ms_nThreadNums + 1; ++j, ++nStartIndex)
    {
        if (nStartIndex >= (nSegCount + 1) * m_nSegCount / ms_nThreadNums)
        {
            break;
        }

        m_pThreadController->BuildPath((*m_pDriveAndFrn)[nStartIndex], &pstrPath);
        if(NULL == pstrPath)
        {
            continue;
        }
        strPath.Format(L"%c:%s%s", (*m_pDriveAndFrn)[nStartIndex].m_cDriveName, (*pstrPath), (*((*m_pDriveAndFrn)[nStartIndex].m_pstrFileName)));

        COleDateTime DateTime;
        _GetFileModifyTime(strPath, DateTime);

        {
            KLocker Lock(m_Critical);
            m_pFileTimeMap->insert(std::make_pair((*m_pDriveAndFrn)[nStartIndex].m_nIndex, 
                DateTime));
        }
    }
}

DWORD WINAPI KGetFileInfoThread::_GetFileSizeFunc(LPVOID pGetFileInfo)
{
    SEG_FILE_INFO* pGetSize = reinterpret_cast<SEG_FILE_INFO*>(pGetFileInfo);
    if(NULL == pGetFileInfo)
    {
        goto Exit0;
    }
    pGetSize->m_pGetFileInfo->_GetFileSizeMain(pGetSize->m_nSegCount);

Exit0:
    return 0;
}

DWORD WINAPI KGetFileInfoThread::_GetFileTimeFunc(LPVOID pGetFileInfo)
{
    SEG_FILE_INFO* pGetTime = reinterpret_cast<SEG_FILE_INFO*>(pGetFileInfo);
    if(NULL == pGetTime)
    {
        goto Exit0;
    }
    pGetTime->m_pGetFileInfo->_GetFileTimeMain(pGetTime->m_nSegCount);

Exit0:
    return 0;
}

void KGetFileInfoThread::_CreateFileSizeThread()
{
    for (int i = 0; i < ms_nThreadNums; ++i)
    {
        m_fileInfo[i].m_pGetFileInfo = this;
        m_fileInfo[i].m_nSegCount = i;

        m_phSizeThreads[i] = ::CreateThread(NULL,
            0,
            _GetFileSizeFunc,
            &m_fileInfo[i],
            0,
            NULL);
    }
}

void KGetFileInfoThread::_CreateFileTimeThread()
{
    for (int i = 0; i < ms_nThreadNums; ++i)
    {
        m_fileInfo[i].m_pGetFileInfo = this;
        m_fileInfo[i].m_nSegCount = i;

        m_phTimeThreads[i] = ::CreateThread(NULL,
            0,
            _GetFileTimeFunc,
            &m_fileInfo[i],
            0,
            NULL);
    }
}

UINT64 KGetFileInfoThread::_GetFileSize(const CString& strFilePath)
{
    UINT64 uFileSize = 0;
    HANDLE hFile = INVALID_HANDLE_VALUE;

    if(::GetFileAttributes(strFilePath) & FILE_ATTRIBUTE_DIRECTORY)
    {
        goto Exit0;
    }

    hFile = ::CreateFile(strFilePath, GENERIC_READ, FILE_SHARE_READ,
        NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
    if(INVALID_HANDLE_VALUE == hFile)
    {
        goto Exit0;
    }
    ::GetFileSizeEx(hFile, reinterpret_cast<PLARGE_INTEGER>(&uFileSize));

Exit0:
    if(INVALID_HANDLE_VALUE != hFile)
    {
        ::CloseHandle(hFile);
    }
    return uFileSize;
}

void KGetFileInfoThread::_GetFileModifyTime(const CString& strFilePath, COleDateTime& ColeDateTime)
{
    HANDLE hFile = INVALID_HANDLE_VALUE;
    FILETIME ftModify = {0};
    SYSTEMTIME stLocal = {0};
    FILETIME ftLocalTime = {0};

    hFile = ::CreateFile(strFilePath, GENERIC_READ, FILE_SHARE_READ,
        NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
    if(INVALID_HANDLE_VALUE == hFile)
    {
        goto Exit0;
    }

    ::GetFileTime(hFile, NULL, NULL, &ftModify);
    ::FileTimeToLocalFileTime(&ftModify, &ftLocalTime);
    ::FileTimeToSystemTime(&ftLocalTime, &stLocal);
    ColeDateTime = COleDateTime(stLocal);

Exit0:
    if(INVALID_HANDLE_VALUE != hFile)
    {
        ::CloseHandle(hFile);
    }
}