#include "stdafx.h"
#include "kmaindlg.h"

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int /*nCmdShow*/)
{
    BkWinManager bkWinmgr;
    BkFontPool::SetDefaultFont(_T("微软雅黑"), -12);
    CString strPath;

    GetModuleFileName((HMODULE)&__ImageBase, strPath.GetBuffer(MAX_PATH + 10), MAX_PATH);
    strPath.ReleaseBuffer();
    strPath.Truncate(strPath.ReverseFind(L'\\') + 1);
    strPath += L"res";

    BkResManager::SetResourcePath(strPath);
    BkSkin::LoadSkins(IDR_BK_SKIN_DEF);
    BkStyle::LoadStyles(IDR_BK_STYLE_DEF);
    BkString::Load(IDR_BK_STRING_DEF);
    HANDLE hMutex = ::CreateMutex(NULL, TRUE, L"X Search");
    if(GetLastError() == ERROR_ALREADY_EXISTS)
    {
        ::MessageBox(NULL, L"X Search程序已经运行", L"提示", MB_OK);
        return 0;
    }
    KMainDlg dlg;
    dlg.DoModal(NULL);
    if(hMutex)
    {
        ::ReleaseMutex(hMutex);
    }
    return 0;
}
