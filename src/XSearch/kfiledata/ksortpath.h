#pragma once

#include "kusinformation.h"
#include "kthreadcontroller.h"

class KSortPath
{
public:
    KSortPath(KThreadController* pThreadCtrl, BOOL bSortByAscent) : m_pThreadCtrl(pThreadCtrl), m_bSortByAscent(bSortByAscent)
    {

    }
    BOOL operator()(const DRIVE_AND_FRN& FileOne, const DRIVE_AND_FRN& FileTwo)
    {
        BOOL bRet = FALSE;
        const CString* pstrFileOnePath = NULL;
        const CString* pstrFileTwoPath = NULL;

        if(FileOne.m_cDriveName < FileTwo.m_cDriveName)
        {
            bRet = TRUE;
        }
        else if(FileTwo.m_cDriveName == FileTwo.m_cDriveName)
        {
            m_pThreadCtrl->BuildPath(FileOne, &pstrFileOnePath);
            m_pThreadCtrl->BuildPath(FileTwo, &pstrFileTwoPath);
            if(*pstrFileOnePath < *pstrFileTwoPath)
            {
                bRet = TRUE;
            }
            else if(*pstrFileOnePath == *pstrFileTwoPath)
            {
                if(*(FileOne.m_pstrFileName) < *(FileTwo.m_pstrFileName))
                {
                    bRet = TRUE;
                }
            }
        }
        if(FALSE == m_bSortByAscent)
        {
            bRet = !bRet;
        }
        return bRet;
    }
private:
    KThreadController* m_pThreadCtrl;
    BOOL m_bSortByAscent;
};