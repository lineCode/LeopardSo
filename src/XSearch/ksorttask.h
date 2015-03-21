#pragma once

#include "kfiledata/kfileinfosort.h"
#include "klistview.h"
#include "ktaskbase.h"

#define UPDATE_FILESORT WM_USER + 102

class KSortTask : public KTaskBase
{
public:
    KSortTask(HWND hWnd, KThreadController* pThreadCtrl, KListView& ListView, 
        std::vector<DRIVE_AND_FRN>* pArrFileInfo, int nSubItem, BOOL bSortByAscent)
        : m_ListView(ListView)
        , m_pvFileInfo(pArrFileInfo)
        , m_nSubItem(nSubItem)
        , m_bIsFinish(FALSE)
        , m_bSortByAscent(bSortByAscent)
        , m_hWnd(hWnd)
        , m_pThreadCtrl(pThreadCtrl)
        , m_pFileInfoSort(NULL)
    {

    }

    ~KSortTask()
    {

    }

    BOOL MallocMemory()
    {
        return TRUE;
    }

    void FreeMemory() 
    {
        if(NULL != m_pFileInfoSort)
        {
            delete m_pFileInfoSort;
            m_pFileInfoSort = NULL;
        }
    }

    void SetTaskInValid()
    {

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
        if(NULL == m_pvFileInfo)
        {
            goto Exit0;
        }
        m_pFileInfoSort = new CFileInfoSort(m_pThreadCtrl);
        m_pFileInfoSort->SortFileInfoByFileAttribution(*m_pvFileInfo, m_nSubItem, m_bSortByAscent);
        m_bIsFinish = TRUE;
        ::SendMessage(m_hWnd, UPDATE_FILESORT, NULL, (LPARAM)m_bIsFinish);
        m_ListView.Invalidate(TRUE);

Exit0:
        return;
    }

private:

    KListView& m_ListView; 
    std::vector<DRIVE_AND_FRN>* m_pvFileInfo;
    CFileInfoSort* m_pFileInfoSort;
    BOOL m_bSortByAscent;
    int m_nSubItem;
    BOOL m_bIsFinish;
    HWND m_hWnd;
    KThreadController* m_pThreadCtrl;
};