#include "../stdafx.h"
#include <algorithm>
#include <utility>
#include "kfileinfosort.h"

CFileInfoSort::CFileInfoSort(KThreadController* pThreadCtrl) : m_pThreadCtrl(pThreadCtrl)
{
    _MapSortTypeToHandle();
}

CFileInfoSort::~CFileInfoSort()
{
    m_SortTypeToHandle.clear();
}

BOOL CFileInfoSort::ms_bSortByAscent = TRUE;

void CFileInfoSort::_ClearFileModifyTimeVector()
{
    if(NULL != gs_pFileModifyTime)
    {
        gs_pFileModifyTime->clear();
        delete gs_pFileModifyTime;
        gs_pFileModifyTime = NULL;
    }
}

void CFileInfoSort::_ClearFileSizeVector()
{
    if(NULL != gs_pFileSize)
    {
        gs_pFileSize->clear();
        delete gs_pFileSize;
        gs_pFileSize = NULL;
    }
}

BOOL CFileInfoSort::SortFileInfoByFileAttribution(std::vector<DRIVE_AND_FRN>& vFileInfoVector,
                                                  int nSortType, BOOL bSortByAscent)
{
    BOOL bRet = TRUE;

    std::map<int, pSort>::const_iterator it = m_SortTypeToHandle.find(nSortType);
    if(m_SortTypeToHandle.end() == it)
    {
        bRet = FALSE;
        goto Exit0;
    }

    ms_bSortByAscent = bSortByAscent;
    pSort pSortFunc = it->second;
    (this->*pSortFunc)(vFileInfoVector);

Exit0:
    return bRet;
}

BOOL CFileInfoSort::_MapSortTypeToHandle()
{
    m_SortTypeToHandle[SORT_BY_FILE_NAME] = &CFileInfoSort::_SortByFileName;
    m_SortTypeToHandle[SORT_BY_FILE_SIZE] = &CFileInfoSort::_SortByFileSize;
    m_SortTypeToHandle[SORT_BY_FILE_PATH] = &CFileInfoSort::_SortByFilePath;
    m_SortTypeToHandle[SORT_BY_FILE_MODIFIED_TIME] = &CFileInfoSort::_SortByFileModifiedTime;
    return TRUE;
}

void CFileInfoSort::_SortByFileName(std::vector<DRIVE_AND_FRN>& vFileInfoVector)
{
    sort(vFileInfoVector.begin(), vFileInfoVector.end(), _CompareFileName);
}

BOOL CFileInfoSort::_CompareFileName(const DRIVE_AND_FRN& FileOne, const DRIVE_AND_FRN& FileTwo)
{
    BOOL bRet = FALSE;
    if(*(FileOne.m_pstrFileName) < *(FileTwo.m_pstrFileName))
    {
        bRet = TRUE;
    }
    if(FALSE == ms_bSortByAscent)
    {
        bRet = !bRet;
    }
    return bRet;
}

void CFileInfoSort::_SortByFileSize(std::vector<DRIVE_AND_FRN>& vFileInfoVector)
{
    KGetFileInfoThread* pGetFileInfoThread = NULL;
    gs_pFileSize = new std::map<int, UINT64>;

    if(NULL == gs_pFileSize)
    {
        goto Exit0;
    }
    pGetFileInfoThread = new KGetFileInfoThread(m_pThreadCtrl, &vFileInfoVector);
    if(NULL == pGetFileInfoThread)
    {
        goto Exit0;
    }
    pGetFileInfoThread->GetFileSize(gs_pFileSize);
    sort(vFileInfoVector.begin(), vFileInfoVector.end(), _CompareFileSize);

Exit0:
    if(NULL != pGetFileInfoThread)
    {
        delete pGetFileInfoThread;
        pGetFileInfoThread = NULL;
    }
    _ClearFileSizeVector();
}

BOOL CFileInfoSort::_CompareFileSize(const DRIVE_AND_FRN& FileOne, const DRIVE_AND_FRN& FileTwo)
{
    BOOL bRet = FALSE;
    std::map<int, UINT64>::iterator FileOneIt = gs_pFileSize->find(FileOne.m_nIndex);
    std::map<int, UINT64>::iterator FileTwoIt = gs_pFileSize->find(FileTwo.m_nIndex);

    if(FileOneIt == gs_pFileSize->end() || FileTwoIt == gs_pFileSize->end())
    {
        goto Exit0;
    }
    if(FileOneIt->second < FileTwoIt->second)
    {
        bRet = TRUE;
    }
    if(FALSE == ms_bSortByAscent)
    {
        bRet = !bRet;
    }

Exit0:
    return bRet;
}

void CFileInfoSort::_SortByFilePath(std::vector<DRIVE_AND_FRN>& vFileInfoVector)
{
    sort(vFileInfoVector.begin(), vFileInfoVector.end(), KSortPath(m_pThreadCtrl, ms_bSortByAscent));
}

void CFileInfoSort::_SortByFileModifiedTime(std::vector<DRIVE_AND_FRN>& vFileInfoVector)
{
    KGetFileInfoThread* pGetFileInfoThread = NULL;
    gs_pFileModifyTime = new std::map<int, COleDateTime>;

    if(NULL == gs_pFileModifyTime)
    {
        goto Exit0;
    }
    pGetFileInfoThread = new KGetFileInfoThread(m_pThreadCtrl, &vFileInfoVector);
    if(NULL == pGetFileInfoThread)
    {
        goto Exit0;
    }
    pGetFileInfoThread->GetFileTime(gs_pFileModifyTime);
    sort(vFileInfoVector.begin(), vFileInfoVector.end(), _CompareFileModifiedTime);

Exit0:
    if(NULL != pGetFileInfoThread)
    {
        delete pGetFileInfoThread;
        pGetFileInfoThread = NULL;
    }
    _ClearFileModifyTimeVector();
}

BOOL CFileInfoSort::_CompareFileModifiedTime(const DRIVE_AND_FRN& FileOne, const DRIVE_AND_FRN& FileTwo)
{
    BOOL bRet = FALSE;
    std::map<int, COleDateTime>::iterator FileOneIt = gs_pFileModifyTime->find(FileOne.m_nIndex);
    std::map<int, COleDateTime>::iterator FileTwoIt = gs_pFileModifyTime->find(FileTwo.m_nIndex);

    if(FileOneIt == gs_pFileModifyTime->end() || FileTwoIt == gs_pFileModifyTime->end())
    {
        goto Exit0;
    }

    if(FileOneIt->second < FileTwoIt->second)
    {
        bRet = TRUE;
    }
    if(FALSE == ms_bSortByAscent)
    {
        bRet = !bRet;
    }

Exit0:
    return bRet;
}