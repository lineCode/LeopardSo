#pragma once

#include "klistviewimpl.h"

class KListView : public KListViewImpl<KListView>
{
public:
    KListView();
    ~KListView();

    HWND Create(HWND hWndParent, _U_RECT rect = NULL, DWORD dwStyle = 0, 
        DWORD dwExStyle = 0, _U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL);
    void InitColumn(int nWndWidth);
    void GetSelectedItemFilePath(CString& strFilePath); 
    void SetSelectedRow(int nRowIndex); 
    int GetSelectedRow();

private:

    enum COLUMN_NAME
    {
        COLUMN_FILE_NAME = 0,
        COLUMN_FILE_PATH,
        COLUMN_FILE_SIZE,
        COLUMN_FILE_MODIFY_TIME
    };

    void _SetExtendedStyle();

    int m_nSelectedRow;
};

