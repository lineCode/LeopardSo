#pragma once

#include "kfiledata/kfileconditionsearch.h"
#include "klistview.h"
#include "ktaskbase.h"

#define UPDATE_FILE_CONDITION_SEARCH WM_USER  + 105

class KConditionSearchTask : public KTaskBase
{
public:
    KConditionSearchTask(HWND hWnd, KThreadController* pThreadCtrl, KListView& ListView, 
        std::vector<DRIVE_AND_FRN>* pvFileInfo, BOOL* pbFileConditionSize, BOOL bFileConditionSizeSelected,
        const COleDateTime& FileConditionTimeLow, const COleDateTime& FileConditionTimeHigh
        , const CString& strFileName, KCriticalSesion& CritcalSess, BOOL bSearchWitchCase)
        : m_ListView(ListView)
        , m_bIsFinish(FALSE)
        , m_bIsValid(TRUE)
        , m_hWnd(hWnd)
        , m_pvFileInfo(pvFileInfo)
        , m_pThreadCtrl(pThreadCtrl)
        , m_pFileConditionSearch(NULL)
        , m_strFileName(strFileName)
        , m_CriticalSession(CritcalSess)
        , m_bSearchWitchCase(bSearchWitchCase)
        , m_pbFileConditionSize(pbFileConditionSize)
        , m_bFileConditionSizeSelected(bFileConditionSizeSelected)
        , m_FileConditionTimeHigh(FileConditionTimeHigh)
        , m_FileConditionTimeLow(FileConditionTimeLow)
    {

    }

    ~KConditionSearchTask()
    {

    }

    BOOL MallocMemory()
    {
        return TRUE;
    }

    void FreeMemory() 
    {
        if(NULL != m_pFileConditionSearch)
        {
            delete m_pFileConditionSearch;
            m_pFileConditionSearch = NULL;
        }
    }

    void SetTaskInValid()
    {
        m_bIsValid = FALSE;
    }

    void SetTaskState(BOOL bIsFilnish)
    {

    }

    BOOL GetTaskState()
    {
        return m_bIsFinish;
    } 

    void DoTask()
    {
        KLocker Locker(m_CriticalSession);
        if(NULL == m_pvFileInfo)
        {
            goto Exit0;
        }
        m_pvFileInfo->reserve(300000);
        m_pvFileInfo->clear();
        m_pThreadCtrl->FindFrnByName(m_strFileName, m_pvFileInfo, m_bSearchWitchCase);
        m_pFileConditionSearch = new KFileConditionSearch(m_pThreadCtrl);
        m_pFileConditionSearch->SearchWithCondition(*m_pvFileInfo, m_pbFileConditionSize,
            m_bFileConditionSizeSelected, m_FileConditionTimeLow, m_FileConditionTimeHigh);
        if(m_bIsValid)
        {
            ::SendMessage(m_hWnd, UPDATE_FILE_CONDITION_SEARCH, NULL, NULL);
            m_ListView.SetItemCountEx(static_cast<int>(m_pvFileInfo->size()), LVSICF_NOINVALIDATEALL);
            m_ListView.Invalidate(TRUE);
        }
        else
        {
            FreeMemory();
        }

Exit0:
        return;
    }

private:

    KListView& m_ListView; 
    std::vector<DRIVE_AND_FRN>* m_pvFileInfo;
    KFileConditionSearch* m_pFileConditionSearch;
    KCriticalSesion& m_CriticalSession;
    BOOL* m_pbFileConditionSize;
    BOOL m_bFileConditionSizeSelected;
    COleDateTime m_FileConditionTimeLow;
    COleDateTime m_FileConditionTimeHigh;
    BOOL m_bIsFinish;
    BOOL m_bIsValid;
    BOOL m_bSearchWitchCase;
    HWND m_hWnd;
    CString m_strFileName;
    KThreadController* m_pThreadCtrl;
};