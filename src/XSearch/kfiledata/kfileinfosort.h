#pragma once

#include <map>
#include <vector>
#include <ATLComTime.h>
#include "ksortpath.h"
#include "kgetfileinfothread.h"
#include "kthreadcontroller.h"
#include "kusinformation.h"

static std::map<int, UINT64>* gs_pFileSize = NULL;
static std::map<int, COleDateTime>* gs_pFileModifyTime = NULL;

class CFileInfoSort
{
public:
    CFileInfoSort(KThreadController* pThreadCtrl);
    ~CFileInfoSort();
    BOOL SortFileInfoByFileAttribution(std::vector<DRIVE_AND_FRN>& vFileInfoVector, int nSortType, BOOL bSortByAscent);

private:
    void _SortByFileName(std::vector<DRIVE_AND_FRN>& vFileInfoVector);
    void _SortByFileSize(std::vector<DRIVE_AND_FRN>& vFileInfoVector);
    void _SortByFilePath(std::vector<DRIVE_AND_FRN>& vFileInfoVector);
    void _SortByFileModifiedTime(std::vector<DRIVE_AND_FRN>& vFileInfoVector);

    static BOOL _CompareFileName(const DRIVE_AND_FRN& FileOne, const DRIVE_AND_FRN& FileTwo);
    static BOOL _CompareFileSize(const DRIVE_AND_FRN& FileOne, const DRIVE_AND_FRN& FileTwo);
    static BOOL _CompareFilePath(const DRIVE_AND_FRN& FileOne, const DRIVE_AND_FRN& FileTwo);
    static BOOL _CompareFileModifiedTime(const DRIVE_AND_FRN& FileOne, const DRIVE_AND_FRN& FileTwo);

    BOOL _MapSortTypeToHandle();
    void _ClearFileSizeVector();
    void _ClearFileModifyTimeVector();

private:

    enum SORT_TYPE
    {
        SORT_BY_FILE_NAME = 0,
        SORT_BY_FILE_PATH,
        SORT_BY_FILE_SIZE,
        SORT_BY_FILE_MODIFIED_TIME,
    };

    typedef void (CFileInfoSort::* pSort)(std::vector<DRIVE_AND_FRN>& vFileInfoVector);
    std::map<int, pSort> m_SortTypeToHandle;
    static BOOL ms_bSortByAscent;
    KThreadController* m_pThreadCtrl;
};