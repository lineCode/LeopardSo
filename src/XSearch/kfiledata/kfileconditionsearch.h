#pragma once

#include <ATLComTime.h>
#include "kthreadcontroller.h"
#include "kusinformation.h"
#include "kfileoperation.h"

static const int gs_nFileConditionSize[] = {0, 10240, 102400, 1048576, 16777216};

class KFileConditionSearch
{
public:
    KFileConditionSearch(KThreadController* pThreadCtrl);
    ~KFileConditionSearch();
    void SearchWithCondition(std::vector<DRIVE_AND_FRN>& vFileInfoVector, BOOL* pbFileConditionSize, BOOL bFileConditionSizeSelected,
        const COleDateTime& FileConditionTimeLow, const COleDateTime& FileConditionTimeHigh);

private:
    BOOL _SizeIsSatisfied(const DRIVE_AND_FRN& FileInfo, BOOL* pbFileConditionSize, BOOL bFileConditionSizeSelected);
    BOOL _TimeIsSatisfied(const DRIVE_AND_FRN& FileInfo, const COleDateTime& FileConditionTimeLow, 
        const COleDateTime& FileConditionTimeHigh);
    void _GetFileModifyTime(const CString& strFilePath, COleDateTime& ColeDateTime);

    KThreadController* m_pThreadCtrl;
    KFileOperation m_FileOperation;
};