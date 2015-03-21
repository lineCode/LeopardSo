#pragma once

#include "kfiledata/kfilefilter.h"
#include "klistview.h"
#include "ktaskbase.h"

#define UPDATE_FILE_FILTER      WM_USER  + 104

class KFilterTask : public KTaskBase
{
public:
    KFilterTask(HWND hWnd, KThreadController* pThreadCtrl, KListView& ListView, 
        std::vector<DRIVE_AND_FRN>* pvFileInfo, int nFileFilterType
        , const CString& strFileName, KCriticalSesion& CritcalSess, BOOL bSearchWitchCase)
        : m_ListView(ListView)
        , m_nFileFilterType(nFileFilterType)
        , m_bIsFinish(FALSE)
        , m_bIsValid(TRUE)
        , m_hWnd(hWnd)
        , m_pvFileInfo(pvFileInfo)
        , m_pThreadCtrl(pThreadCtrl)
        , m_pFileFilter(NULL)
        , m_strFileName(strFileName)
        , m_CriticalSession(CritcalSess)
        , m_bSearchWitchCase(bSearchWitchCase)
    {

    }

    ~KFilterTask()
    {

    }

    BOOL MallocMemory()
    {
        return TRUE;
    }

    void FreeMemory() 
    {
        if(NULL != m_pFileFilter)
        {
            delete m_pFileFilter;
            m_pFileFilter = NULL;
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
        m_pFileFilter = new KFileFilter(m_pThreadCtrl);
        m_pFileFilter->FilterFile(*m_pvFileInfo, m_nFileFilterType);
        if(m_bIsValid)
        {
            ::SendMessage(m_hWnd, UPDATE_FILE_FILTER, NULL, NULL);
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
    KFileFilter* m_pFileFilter;
    KCriticalSesion& m_CriticalSession;
    int m_nFileFilterType;
    BOOL m_bIsFinish;
    BOOL m_bIsValid;
    BOOL m_bSearchWitchCase;
    HWND m_hWnd;
    CString m_strFileName;
    KThreadController* m_pThreadCtrl;
};