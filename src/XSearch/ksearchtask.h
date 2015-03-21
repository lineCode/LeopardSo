#pragma once

#include "ktaskbase.h"
#include "kfiledata/kthreadcontroller.h"
#include "klistview.h"
#include "kfiledata/kcriticalsesion.h"
#include "kfiledata/klocker.h"

#define UPDATE_FILEDATA         WM_USER + 101

class KSearchTask : public KTaskBase
{
public:
    KSearchTask(HWND hWnd, KThreadController& ThreadCtrl, CString& strFileName
        , KListView& ListView, KCriticalSesion& CritcalSess, BOOL bSearchWitchCase)
        : m_ThreadCtrl(ThreadCtrl)
        , m_strFileName(strFileName)
        , m_ListView(ListView)
        , m_CriticalSession(CritcalSess)
        , m_hWnd(hWnd)
        , m_bIsFinish(FALSE)
        , m_pvFileInfo(NULL)
        , m_bIsValid(TRUE)
        , m_bSearchWitchCase(bSearchWitchCase)
    {
    }

    ~KSearchTask()
    {
        FreeMemory(); 
    }

    BOOL MallocMemory()
    {
        BOOL bRet = FALSE;
        m_pvFileInfo = new std::vector<DRIVE_AND_FRN>;
        if(m_pvFileInfo)
        {
            bRet = TRUE;
        }
        return bRet;
    } 

    void FreeMemory()
    {
        if(NULL != m_pvFileInfo)
        {
            m_pvFileInfo->clear();
            delete m_pvFileInfo;
            m_pvFileInfo = NULL;
        } 
    }

    void DoTask()
    {  
        if(NULL == m_pvFileInfo)
        {
            goto Exit0;
        } 

        m_pvFileInfo->reserve(300000);
        m_ThreadCtrl.FindFrnByName(m_strFileName, m_pvFileInfo, m_bSearchWitchCase);
        if(m_bIsValid)
        {
            ::SendMessage(m_hWnd, UPDATE_FILEDATA, NULL, reinterpret_cast<LPARAM>(this)); //通知更新数组,sendMessage执行完再返回   
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

    BOOL GetTaskState()
    {
        return m_bIsFinish;
    } 

    void SetTaskInValid()
    {
        m_bIsValid = FALSE;
    }

    void SetTaskState(BOOL bIsFilnish)
    {
        m_bIsFinish = bIsFilnish;
    }

    std::vector<DRIVE_AND_FRN>* GetFileInfoVector()
    {
        return m_pvFileInfo;
    }

private:

    HWND m_hWnd;
    KCriticalSesion& m_CriticalSession;
    KThreadController& m_ThreadCtrl;
    CString m_strFileName;
    KListView& m_ListView; 
    BOOL m_bIsFinish;
    BOOL m_bIsValid;
    BOOL m_bSearchWitchCase;
    std::vector<DRIVE_AND_FRN>* m_pvFileInfo;
};