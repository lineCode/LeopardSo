#pragma once

#include "bkwinres.h"

class KMyEdit : public CWindowImpl<KMyEdit, CEdit>
{
public:
    KMyEdit(HWND hWnd) : m_hWnd(hWnd)
    {

    }
    ~KMyEdit(){}

    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
    {
        if(VK_DOWN == nChar)
        {
            ::SendMessage(m_hWnd, WM_UPDATE_LISTVIEW_STATE, NULL, NULL);
        }
    }

public:
    HWND m_hWnd;
    BEGIN_MSG_MAP_EX(KMyEdit)
        MSG_WM_KEYDOWN(OnKeyDown)
    END_MSG_MAP()
};