#pragma once

#include "stdafx.h"
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>
#include <atlstr.h>
#include <bkres/bkres.h>
#include "bkwin/bkdlgview.h"
#include "kfiledata/kusinformation.h"
#include "kfiledata/kthreadcontroller.h"

typedef CWinTraits<WS_CHILD | WS_TABSTOP | WS_VISIBLE 
| LVS_REPORT | LVS_OWNERDATA | LVS_SHOWSELALWAYS 
| LVS_SINGLESEL | LVS_NOCOLUMNHEADER, 0> CBkListViewTraits;

template <class T, class TBase = CListViewCtrl, class TWinTraits = CBkListViewTraits>
class KListViewImpl
    : public ATL::CWindowImpl<T, TBase, TWinTraits>
    , public CBkViewImpl<T>
    , public CCustomDraw<T>
    ,public CBkContainerWnd
{
public:
    KListViewImpl(){}
    ~KListViewImpl(){}

    typedef ATL::CWindowImpl<T, TBase, TWinTraits>  __baseClass;
    typedef KListViewImpl<T, TBase, TWinTraits>   __thisClass;

    HWND Create(HWND hWndParent, _U_RECT rect = NULL, DWORD dwStyle = 0,
        DWORD dwExStyle = 0, _U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
    {
        HWND hWnd = NULL;

        if (NULL != m_hWnd)
        {
            hWnd = m_hWnd;
            goto Exit0;
        }

        hWnd = __baseClass::Create(hWndParent, rect, NULL, dwStyle, dwExStyle, MenuOrID, lpCreateParam);
        if (hWnd)
        {
            SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);
            InsertColumn(0, L"", LVCFMT_LEFT, 1);
        }

Exit0:
        return hWnd;
    }

    HWND Create(HWND hWndParent, _U_MENUorID MenuOrID)
    {
        return Create(hWndParent, NULL, 0, 0, MenuOrID, NULL);
    }

    BOOL IsClass(LPCSTR lpszName)
    {
        return TRUE;
    } 

    LPCSTR GetObjectClass()
    {
        return 0;
    }

    DWORD OnPrePaint(int nIdCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
    {        
        return  CDRF_NOTIFYITEMDRAW;
    }

    DWORD OnItemPrePaint(int nIdCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
    {
        return CDRF_DODEFAULT;
    }



private:
    BEGIN_MSG_MAP_EX(CBkListViewImpl)
        CHAIN_MSG_MAP_ALT(CCustomDraw<T>, TRUE)
        DEFAULT_REFLECTION_HANDLER()
    END_MSG_MAP()
};

