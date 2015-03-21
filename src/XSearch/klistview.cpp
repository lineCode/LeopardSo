#include "klistview.h"

KListView::KListView() : m_nSelectedRow(-1)
{

} 

KListView:: ~KListView()
{

} 

HWND KListView::Create(HWND hWndParent, _U_RECT rect, DWORD dwStyle,
                       DWORD dwExStyle, _U_MENUorID MenuOrID, LPVOID lpCreateParam)
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
        _SetExtendedStyle();
    }

Exit0:
    return hWnd;
}

void KListView::SetSelectedRow(int nRowIndex)
{
    m_nSelectedRow = nRowIndex;
}

int KListView::GetSelectedRow()
{
    return m_nSelectedRow;
}

void KListView::GetSelectedItemFilePath(CString& strFilePath)
{
    CListViewCtrl::GetItemText(m_nSelectedRow, 1, strFilePath);
}

void KListView::InitColumn(int nWndWidth)
{
    CString strColumnName[] = {"文件名", "路径", "大小", "最后修改时间"};
    int nColumnWidth[] = {0.2 * nWndWidth, 0.4 * nWndWidth, 0.15 * nWndWidth, 0.25 * nWndWidth};
    int nColumnNum = sizeof(strColumnName) / sizeof(strColumnName[0]);

    for(int i = 0; i < nColumnNum; ++i)
    {
        InsertColumn(i, strColumnName[i], LVCFMT_LEFT, nColumnWidth[i]); 
    }
}

void KListView::_SetExtendedStyle()
{
    DWORD dwStyle = GetExtendedListViewStyle();
    dwStyle |= LVS_EX_FULLROWSELECT;
    dwStyle |= LVS_EX_GRIDLINES;
    dwStyle |= LVS_EX_FLATSB; 
    dwStyle |= LVS_OWNERDATA;
    SetExtendedListViewStyle(dwStyle); 
}