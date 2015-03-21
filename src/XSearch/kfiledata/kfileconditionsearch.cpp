#include "../stdafx.h"
#include "kfileconditionsearch.h"

KFileConditionSearch::KFileConditionSearch(KThreadController* pThreadCtrl) : m_pThreadCtrl(pThreadCtrl)
{

}

KFileConditionSearch::~KFileConditionSearch()
{

}

BOOL KFileConditionSearch::_SizeIsSatisfied(const DRIVE_AND_FRN& FileInfo,
                                            BOOL* pbFileConditionSize, BOOL bFileConditionSizeSelected)
{
    BOOL bRet = FALSE;
    const CString* pstrFilePath = NULL;
    UINT64 ulFileSize = 0;
    int nLen = 0;
    CString strFilePath;
    CString strFileModifyTime;

    if(NULL == pbFileConditionSize)
    {
        goto Exit0;
    }
    if(!bFileConditionSizeSelected)
    {
        bRet = TRUE;
        goto Exit0;
    }

    m_pThreadCtrl->BuildPath(FileInfo, &pstrFilePath);
    if(NULL == pstrFilePath)
    {
        goto Exit0;
    }
    strFilePath.Format(L"%c:%s%s", FileInfo.m_cDriveName, *pstrFilePath, *(FileInfo.m_pstrFileName));
    ulFileSize = m_FileOperation.GetFileSize(strFilePath);

    nLen = sizeof(gs_nFileConditionSize) / sizeof(int);
    for(int i = 1; i < nLen; ++i)
    {
        if(pbFileConditionSize[i - 1])
        {
            if(ulFileSize >= gs_nFileConditionSize[i - 1] && ulFileSize <= gs_nFileConditionSize[i])
            {
                bRet = TRUE;
                break;
            }
        }
    }
    if(pbFileConditionSize[nLen - 1] && ulFileSize >= gs_nFileConditionSize[nLen - 1])
    {
        bRet = TRUE;
    }

Exit0:
    return bRet;
}

BOOL KFileConditionSearch::_TimeIsSatisfied(const DRIVE_AND_FRN& FileInfo, const COleDateTime& FileConditionTimeLow,
                                            const COleDateTime& FileConditionTimeHigh)
{
    BOOL bRet = FALSE;
    const CString* pstrFilePath = NULL;
    CString strFilePath;
    COleDateTime FileModifyTime;

    m_pThreadCtrl->BuildPath(FileInfo, &pstrFilePath);
    if(NULL == pstrFilePath)
    {
        goto Exit0;
    }
    strFilePath.Format(L"%c:%s%s", FileInfo.m_cDriveName, *pstrFilePath, *(FileInfo.m_pstrFileName));
    _GetFileModifyTime(strFilePath, FileModifyTime);

    if(0 != FileConditionTimeLow)
    {
        if(0 != FileConditionTimeHigh)
        {
            bRet = (FileModifyTime >= FileConditionTimeLow && FileModifyTime <= FileConditionTimeHigh);
        }
        else
        {
            bRet = (FileModifyTime <= FileConditionTimeLow);
        }
    }
    else
    {
        if(0 != FileConditionTimeHigh)
        {
            bRet = (FileModifyTime >= FileConditionTimeHigh);
        }
        else
        {
            bRet = TRUE;
        }
    }

Exit0:
    return bRet;
}

void KFileConditionSearch::SearchWithCondition(std::vector<DRIVE_AND_FRN>& vFileInfoVector, 
                                               BOOL* pbFileConditionSize, 
                                               BOOL bFileConditionSizeSelected,
                                               const COleDateTime& FileConditionTimeLow, 
                                               const COleDateTime& FileConditionTimeHigh)
{
    std::vector<DRIVE_AND_FRN> vFileConditionVector;

    for(int i = 0; i < vFileInfoVector.size(); ++i)
    {
        if(_TimeIsSatisfied(vFileInfoVector[i], FileConditionTimeLow, FileConditionTimeHigh)
            && _SizeIsSatisfied(vFileInfoVector[i], pbFileConditionSize, bFileConditionSizeSelected))
        {
            vFileConditionVector.push_back(vFileInfoVector[i]);
        }
    }

    vFileInfoVector.clear();
    vFileInfoVector.resize(vFileConditionVector.size());
    copy(vFileConditionVector.begin(), vFileConditionVector.end(), vFileInfoVector.begin());
}

void KFileConditionSearch::_GetFileModifyTime(const CString& strFilePath, COleDateTime& ColeDateTime)
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

    stLocal.wHour = 0;
    stLocal.wMilliseconds = 0;
    stLocal.wMinute = 0;
    stLocal.wSecond = 0;
    ColeDateTime = COleDateTime(stLocal);

Exit0:
    if(INVALID_HANDLE_VALUE != hFile)
    {
        ::CloseHandle(hFile);
    }
}
