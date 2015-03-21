#include "stdafx.h"
#include <ATLComTime.h>
#include "kmaindlg.h"

KMainDlg::KMainDlg(): CBkDialogImpl<KMainDlg>(IDR_BK_MAIN_DIALOG)
, m_dwCounter(0)
, m_dwTimeOut(0)
, m_dwSearchInputTime(0)
, m_pFileConditionSearch(NULL)
, m_pCurrentTask(NULL)
, m_pPreSearchTask(NULL)
, m_pPreFilterTask(NULL)
, m_pSearchEditBar(NULL)
, m_bSortByAscent(TRUE)
, m_bIsSorting(FALSE)
, m_bHasMessageBox(FALSE)
, m_bIsConditionSearching(FALSE)
, m_bIsExtendDialog(FALSE)
, m_bIsFiltering(FALSE)
, m_pThreadController(NULL)
, m_bCanSearch(FALSE)
, m_nSortedColumn(-1)
, m_nSortingColumn(-2)
, m_nFileConditionTimeType(-1)
, m_nPreviewBtnSubIndex(0)
, m_nFlashTimes(0)
, m_nAlpha(ALPHA_LOWERBOUND)
, m_bIsSmallDialgo(TRUE)
, m_bIsSearching(FALSE)
, m_bPreviewBtnSelected(FALSE)
, m_bDetailBtnSelected(FALSE)
, m_bLUCaseBtnSelected(FALSE)
, m_bSelectedFileConditionSize(FALSE)
, m_bSelectedFileConditionTime(FALSE)
, m_bKindBtnSelected(FALSE)
, m_bHighBtnSelected(FALSE)
{
    m_hRichEditDll = ::LoadLibrary(CRichEditCtrl::GetLibraryName());
    ZeroMemory(m_bSelectedFileConditionSizeType, sizeof(m_bSelectedFileConditionSizeType));
    _MapFillColTypeToHandle();
    _MapTimerTypeToHandle();
}

KMainDlg::~KMainDlg()
{
    m_FillColTypeToHandle.clear(); 
    m_ThreadPool.Shutdown(); 
    ClearAllTask();
    ClearHicon();
    if(NULL != m_pSearchEditBar)
    {
        delete m_pSearchEditBar;
        m_pSearchEditBar = NULL;
    }
    if(NULL != m_pThreadController)
    {
        delete m_pThreadController;
        m_pThreadController = NULL;
    }
} 

void KMainDlg::ClearAllTask()
{
    while (!m_queTaskQueue.empty())
    {
        if(m_queTaskQueue.front())
        {
            m_queTaskQueue.front()->FreeMemory();
            delete m_queTaskQueue.front();
        }
        m_queTaskQueue.pop();
    }
}

void KMainDlg::ClearHicon()
{
    for(int i = 0; i < m_vHicon.size(); ++i)
    {
        if(NULL != m_vHicon[i])
        {
            DestroyIcon(*m_vHicon[i]);
        }
    }
    m_vHicon.clear();
}

void KMainDlg::InitSearchEdit()
{
    m_pSearchEditBar = new KMyEdit(m_hWnd);
    m_pSearchEditBar->Create(GetViewHWND(), 0, 0, WS_CHILD | WS_VISIBLE, 0, IDC_MAIN_SEARCH_EDIT);
    m_pSearchEditBar->SetFont(BkFontPool::GetFont(FALSE, FALSE, FALSE, 10));
    ::SetFocus(m_pSearchEditBar->m_hWnd);     
    m_pSearchEditBar->ShowCaret();
}

void KMainDlg::InitPreViewRichEdit()
{
    CHARFORMAT CharFormat;
    CFont fntText;

    m_PreViewRichEdit.Create(GetViewHWND(), 0, 0, WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | 
        ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN | WS_TABSTOP | ES_READONLY, 0, IDC_PREVIEW_RICHEDIT);
    fntText.CreateFont(15,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY ,DEFAULT_PITCH, _T("微软雅黑"));

    m_PreViewRichEdit.SetFont(fntText);
    m_PreViewRichEdit.GetDefaultCharFormat(CharFormat);
    CharFormat.dwMask = CFM_COLOR;
    CharFormat.dwEffects &= ~CFE_AUTOCOLOR;
    CharFormat.crTextColor = RGB(39, 40, 34);
    m_PreViewRichEdit.SetDefaultCharFormat(CharFormat);
}

void KMainDlg::InitListView()
{
    m_ListView.Create(GetViewHWND(), 0,  WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS 
        | WS_CLIPCHILDREN | LVS_REPORT | LVS_SINGLESEL | LVS_SHAREIMAGELISTS
        | LVS_OWNERDATA, WS_EX_CLIENTEDGE, IDC_MAIN_LIST, NULL);
    m_ListView.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_DOUBLEBUFFER );
    m_ImageHead.Create(16, 16, ILC_COLOR32 | ILC_MASK, 1, 1);
    m_ListView.SetImageList(m_ImageHead.m_hImageList, LVSIL_SMALL);
}

void KMainDlg::InitOther()
{
    LONG lStyle = GetWindowLong(GWL_EXSTYLE);
    lStyle |= WS_EX_LAYERED;
    SetWindowLong(GWL_EXSTYLE, lStyle);
    SetLayeredWindowAttributes(m_hWnd, NULL, ALPHA_UPPERBOUND, LWA_ALPHA);
    DontShowWindow();

    if(!RegisterHotKey(m_hWnd, HOTKEY_DCTRL, MOD_CONTROL, 0))
    {
        m_KeyBoardHook.InstallHook(m_hWnd);
    }
    InitUnknownFileIcon();
    m_pThreadController = new KThreadController(m_hWnd);
    m_pFileConditionSearch = new KFileConditionSearch(m_pThreadController);
    m_ThreadPool.Initialize(NULL, THREADPOOL_SIZE);
    SetItemVisible(IDC_PREVIEW_RICHEDIT, FALSE);
    SetItemVisible(IDC_BTN_DETAIL, FALSE);
    SetAllTimer(); 
    _SetKindBtnVisible(FALSE); 
    SetItemVisible(IDC_BTN_PREVIEW, FALSE);
}

void KMainDlg::_SetKindBtnVisible(BOOL bVisible)
{
    SetItemVisible(IDC_BTN_ALLFILE, bVisible);
    SetItemVisible(IDC_BTN_DOC, bVisible);
    SetItemVisible(IDC_BTN_MUSIC, bVisible);
    SetItemVisible(IDC_BTN_VEDIO, bVisible);
    SetItemVisible(IDC_BTN_IMG, bVisible); 
    SetItemVisible(IDC_BTN_FOLDER, bVisible); 
}
void KMainDlg::InitConditionSearchControl()
{
    m_DateTimePickerPre.Create(GetViewHWND(), 0, 0, WS_CHILD | WS_VISIBLE, 0, IDC_DATE_TIME_PICKER_PRE);
    m_DateTimePickerPre.SetFormat(L"yyyy-MM-dd");
    m_DateTimePickerPost.Create(GetViewHWND(), 0, 0, WS_CHILD | WS_VISIBLE, WS_EX_CLIENTEDGE, IDC_DATE_TIME_PICKER_POST);
    m_DateTimePickerPost.SetFormat(L"yyyy-MM-dd");
    m_DateTimePickerIntervalPre.Create(GetViewHWND(), 0, 0, WS_CHILD | WS_VISIBLE, WS_EX_CLIENTEDGE, IDC_DATE_TIME_PICKER_INTERVAL_PRE);
    m_DateTimePickerIntervalPre.SetFormat(L"yyyy-MM-dd");
    m_DateTimePickerIntervalPost.Create(GetViewHWND(), 0, 0, WS_CHILD | WS_VISIBLE, WS_EX_CLIENTEDGE, IDC_DATE_TIME_PICKER_INTERVAL_POST);
    m_DateTimePickerIntervalPost.SetFormat(L"yyyy-MM-dd");
}

BOOL KMainDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
    InitTray();
    InitSearchEdit();
    InitPreViewRichEdit();
    InitListView();
    InitConditionSearchControl();
    InitOther();
    return TRUE;
}

void KMainDlg::OnKindButtonDown()
{
    OnSelfDefineImgBtnDown(IDC_BTN_KIND, m_bKindBtnSelected);
    if(m_bKindBtnSelected)
    {
        RECT rcBtn = {0};
        GetItemRect(IDC_BTN_KIND, rcBtn);
        rcBtn.top = rcBtn.bottom + BUTTON_HIGHT*(BUTTON_LIST_NUM) + BUTTON_LIST_NUM;
        rcBtn.bottom = rcBtn.top + BUTTON_HIGHT; 
        _SetItemPosFromItemRect(IDC_BTN_HIGHLEVEL, rcBtn);     

        if(m_bHighBtnSelected)
        {
            _SetPreViewBtnPos(rcBtn);   
        }    
        _SetKindBtnVisible(TRUE);

    }
    else
    {
        RECT rcBtn = {0};
        GetItemRect(IDC_BTN_KIND, rcBtn);
        rcBtn.top += BUTTON_HIGHT + 1;
        rcBtn.bottom = rcBtn.top + BUTTON_HIGHT;  
        _SetItemPosFromItemRect(IDC_BTN_HIGHLEVEL, rcBtn);
        if(m_bHighBtnSelected)
        {
            _SetPreViewBtnPos(rcBtn);
        }    
        _SetKindBtnVisible(FALSE);
    }
}

void KMainDlg::_SetPreViewBtnPos(RECT rcBtn)
{
    rcBtn.top += BUTTON_HIGHT + 1;
    rcBtn.bottom = rcBtn.top + BUTTON_HIGHT;
    _SetItemPosFromItemRect(IDC_BTN_PREVIEW, rcBtn);
}

void KMainDlg::_SetItemPosFromItemRect(UINT uItemID, RECT rcBtn)
{
    CString strPos;
    strPos.Format(L"%d,%d",rcBtn.left,rcBtn.top - DIALOG_MINI_HEIGHT);
    SetItemAttribute(uItemID,"pos", CW2A(strPos, CP_UTF8));   
}
void KMainDlg::OnHighLevelButtonDown()
{
    OnSelfDefineImgBtnDown(IDC_BTN_HIGHLEVEL, m_bHighBtnSelected);
    if(m_bHighBtnSelected)
    {
        RECT rcBtn = {0};
        GetItemRect(IDC_BTN_HIGHLEVEL, rcBtn);
        _SetPreViewBtnPos(rcBtn);
        SetItemVisible(IDC_BTN_PREVIEW, TRUE);
    }
    else
    {
        SetItemVisible(IDC_BTN_PREVIEW, FALSE);
    } 
}

void KMainDlg::SetWindowEllispeFrame(int nWidthEllipse, int nHeightEllipse)
{
    HRGN hRgn = NULL;
    RECT rcRect = {0};

    GetWindowRect(&rcRect);
    hRgn = CreateRoundRectRgn(0, 0, rcRect.right - rcRect.left, rcRect.bottom - rcRect.top, nWidthEllipse, nHeightEllipse);
    SetWindowRgn(hRgn, TRUE);
}

void KMainDlg::SetAllTimer()
{
    SetTimer(TIMER_ID_ONE, TIME_INTERVAL);
    SetTimer(TIMER_ID_SHOW_MAIN_DIALGO, TIME_INTERVAL);
    SetTimer(TIMER_ID_CLEAR_UNUSED_TASK, TIME_INTERVAL_TO_CLEAR_TASK);
    SetTimer(TIMER_ID_SEARCH_WAITTING, TIME_INTERVAL);
}

void KMainDlg::SetStatusBarInfo(const CString& strInfo)
{
    SetItemText(IDC_STATUS_BAR_TEXT_ID, strInfo);
}

void KMainDlg::HideDialog()
{
    ShowWindow(SW_HIDE);
}

void KMainDlg::OnSysClose()
{
    UnregisterHotKey(m_hWnd, HOTKEY_DCTRL);
    DeleteTray();
    EndDialog(IDCLOSE);
}

LRESULT KMainDlg::OnHotKey(int uMsg, WPARAM wParam, LPARAM lParam)
{  
    DWORD dwCurrentTime = GetTickCount();
    if(m_dwCounter == 0)
    {
        m_dwTimeOut = dwCurrentTime;
        ++m_dwCounter;
    }
    else if(dwCurrentTime - m_dwTimeOut > TIMEOUT_LOWER && dwCurrentTime - m_dwTimeOut < TIMEOUT_UPPER)
    {
        m_dwCounter = 0;
        ShowDialog();
        ::SetFocus(*m_pSearchEditBar);
        ::ShowCaret(*m_pSearchEditBar);
    }
    else
    {
        m_dwCounter = 0;
    }  
    return 0;  
}

void KMainDlg::OnActivate(UINT uMsg, BOOL wParam, HWND lParam)
{
    if(LOWORD(wParam) == WA_INACTIVE && !m_bHasMessageBox)
    {
        ShowWindow(SW_HIDE);
        DeleteCompletedTask();
    }
}

void KMainDlg::OnContextMenuGen(HWND wParam, _WTYPES_NS::CPoint cPoint)
{
    RECT rListViewClientRect = {0};
    LVHITTESTINFO Hitinfo = {0};
    POINT Point = {0};
    CMenuHandle Menu;
    CMenuHandle Popup;
    int nItem = -1;

    ::GetWindowRect(m_ListView.m_hWnd, &rListViewClientRect);
    if(PtInRect(&rListViewClientRect, cPoint))
    {
        Menu.LoadMenu(IDR_MENU_RIGHTBUTTION);
        Popup = Menu.GetSubMenu(0);
        GetCursorPos(&Point);
        Popup.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, Point.x, Point.y, m_hWnd);
        Hitinfo.pt = Point;

        nItem = m_ListView.HitTest(&Hitinfo); 
        if(nItem != -1)
        {
            m_ListView.SetSelectedRow(nItem);
        }
    }

} 

void KMainDlg::InitTray()
{
    CString sWindowText = "X Search";
    ZeroMemory(&m_NotifyIconData, sizeof m_NotifyIconData);
    m_NotifyIconData.cbSize = NOTIFYICONDATAA_V1_SIZE;
    m_NotifyIconData.hWnd = m_hWnd;
    m_NotifyIconData.uID = 1;
    m_NotifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    m_NotifyIconData.uCallbackMessage = WM_SYSTEMTRAYICON;
    m_NotifyIconData.hIcon = AtlLoadIconImage(IDI_BEIKESAFE, LR_DEFAULTCOLOR, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON));
    _tcscpy_s(m_NotifyIconData.szTip, sWindowText);
    Shell_NotifyIcon(NIM_ADD, &m_NotifyIconData);
}

void KMainDlg::DeleteTray()
{
    if(m_NotifyIconData.cbSize)
    {
        Shell_NotifyIcon(NIM_DELETE, &m_NotifyIconData);
        ZeroMemory(&m_NotifyIconData, sizeof m_NotifyIconData);
    }
}

void KMainDlg::ShowDialog()
{
    ::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);  
    ShowWindow(SW_SHOW);
    SetFocus();
}

LRESULT KMainDlg::OpenDialog(UINT uParam, UINT wParam, HWND lParam, BOOL bHandled)
{
    ShowDialog();
    return 0;
}

LRESULT KMainDlg::OnSystemTrayIcon(UINT uParam, WPARAM wParam, LPARAM lParam)
{
    CMenu MenuPopup;
    CMenuHandle Menu;
    CPoint Position;
    if(WM_RBUTTONUP == lParam)
    {
        SetForegroundWindow(m_hWnd);
        MenuPopup.LoadMenu(IDR_TRAYMENU);
        Menu = MenuPopup.GetSubMenu(0);
        ATLVERIFY(GetCursorPos(&Position));
        Menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_BOTTOMALIGN, Position.x, Position.y, m_hWnd);
    }
    else if(WM_LBUTTONDBLCLK == lParam)
    {
        ShowDialog();
    }
    return 0;
}

LRESULT KMainDlg::OnClose(UINT uParam, UINT wParam, HWND lParam, BOOL bHandled)
{
    OnSysClose();
    return 0;
}

LRESULT KMainDlg::OnOpenFile(UINT uParam, UINT wParam, HWND lParam, BOOL bHandled)
{
    CString strFilePath;
    m_ListView.GetSelectedItemFilePath(strFilePath);
    m_FileOperation.OpenFile(strFilePath);
    return 0;
}  

LRESULT KMainDlg::OnOpenFilePath(UINT uParam, UINT wParam, HWND lParam, BOOL bHandled)
{
    CString strFilePath;
    m_ListView.GetSelectedItemFilePath(strFilePath);
    m_FileOperation.OpenFileDirectory(strFilePath);
    return 0;
}

LRESULT KMainDlg::OnCopyPath(UINT uParam, UINT wParam, HWND lParam, BOOL bHandled)
{
    const CString* pstrFilePath = NULL;
    CString strFilePath;
    int nSelectedRow = m_ListView.GetSelectedRow();
    if(-1 != nSelectedRow)
    {
        m_pThreadController->BuildPath(m_pCurrentTask->GetFileInfoVector()->at(nSelectedRow), &pstrFilePath);
        if(NULL == pstrFilePath)
        {
            goto Exit0;
        }
        strFilePath.Format(L"%c:%s%s", m_pCurrentTask->GetFileInfoVector()->at(nSelectedRow).m_cDriveName
            , *pstrFilePath, *(m_pCurrentTask->GetFileInfoVector()->at(nSelectedRow).m_pstrFileName));
    }
    m_FileOperation.CopyFilePathToClipPath(&strFilePath);

Exit0:
    return 0;
}

void KMainDlg::_NullSearchTextCommand()
{
    m_strFileName="";
    m_bIsExtendDialog = FALSE;
    m_bPreviewBtnSelected = TRUE;
    OnPreviewBtnDown();
    m_bDetailBtnSelected = TRUE;
    OnSelfDefineImgBtnDown(IDC_BTN_DETAIL, m_bDetailBtnSelected);
    if(!m_bIsSmallDialgo)
    {
        RECT rRect = {0};
        GetWindowRect(&rRect);
        int nHeight = DIALOG_MINI_HEIGHT;
        int nWidth = DIALOG_WIDTH;
        MoveWindow(rRect.left, rRect.top, nWidth, nHeight, TRUE);
        SetWindowEllispeFrame(WIDTH_ELLIPSE, HIEIGHT_ELLIPSE); 
        m_bIsSmallDialgo = TRUE;
        SetStatusBarInfo(L"");
        SetItemVisible(IDC_HEAD_INFO_TEXT_ID, TRUE);
        SetHeadInfo(L"已就绪");
        SetItemVisible(IDC_BTN_DETAIL, FALSE);
    }
}

void KMainDlg::_NotNullSearchTextCommand()
{
    CString strSearch;
    m_bIsSearching = TRUE;
    m_dwSearchInputTime = GetTickCount();
    if(m_bIsSmallDialgo)
    {
        RECT rRect = {0};
        GetWindowRect(&rRect);
        MoveWindow(rRect.left, rRect.top, DIALOG_WIDTH, DIALOG_NORMAL_HEIGHT, TRUE);
        SetWindowEllispeFrame(WIDTH_ELLIPSE, HIEIGHT_ELLIPSE); 
        m_bIsSmallDialgo = FALSE;
    }
    _SetCaseTip(); 
    strSearch.Format(L"正在搜索[%s]...", m_strFileName);
    SetStatusBarInfo(strSearch);
    SetItemVisible(IDC_BTN_DETAIL, TRUE);
}

int KMainDlg::_GetSearchEditText()
{
    TCHAR szFileNameBuffer[MAX_PATH] = {0};
    int nBufferLen = 0; 
    if(NULL != m_pPreSearchTask)
    {
        m_pPreSearchTask->SetTaskInValid();
    }
    nBufferLen = m_pSearchEditBar->GetLine(0, szFileNameBuffer, MAX_PATH);
    szFileNameBuffer[nBufferLen] = '\0';
    m_strFileName.Format(L"%s", szFileNameBuffer);
    return nBufferLen;
}

void KMainDlg::OnCommandHandlerEX(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    int nBufferLen = 0;

    if(!m_bCanSearch || m_bIsSorting || m_bIsFiltering || m_bIsConditionSearching)
    {
        goto Exit0;
    }

    nBufferLen = _GetSearchEditText();
    if(!nBufferLen)
    {
        _NullSearchTextCommand();
    }
    else
    {
        _NotNullSearchTextCommand();
    }
    ClearColumnHeadIcon();

Exit0:
    return;
}

void KMainDlg::ShowFileAtrributeInfo(const CString& strFilePath)
{
    CString strFileType;
    CString strFileCreateTime;
    CString strFileAccessTime;
    CString strFileSpaceSize;
    UINT64 ulFileSpaceSize = 0;
    HICON hIcon = NULL;

    ulFileSpaceSize = m_FileOperation.GetFileSpaceSize(strFilePath);
    m_FileOperation.GetFileAccessAndCreateTime(strFilePath, strFileCreateTime, strFileAccessTime);
    m_FileOperation.GetFileType(strFilePath, strFileType);
    m_FileOperation.FormatFileSpaceSize(ulFileSpaceSize, strFileSpaceSize);
    m_FileOperation.GetFileIcon(strFilePath, hIcon);
    if(NULL == hIcon)
    {
        SetItemAttribute(IDC_ICON_FILE_SMALL_ICON, "size", "128");
        SetItemAttribute(IDC_ICON_FILE_SMALL_ICON, "pos", "70,240");
    }
    else
    {
        SetItemAttribute(IDC_ICON_FILE_SMALL_ICON, "size", "32");
        SetItemAttribute(IDC_ICON_FILE_SMALL_ICON, "pos", "30,220");
    }
    SetItemIconHandle(IDC_ICON_FILE_SMALL_ICON, hIcon);
    SetItemText(IDC_TEXT_FILE_TYPE, strFileType);
    SetItemText(IDC_TEXT_FILE_SPACING_SIZE, strFileSpaceSize);
    SetItemText(IDC_TEXT_FILE_CREATE_TIME, strFileCreateTime);
    SetItemText(IDC_TEXT_FILE_ACCESS_TIME, strFileAccessTime);
}

void KMainDlg::ShowFileBriefInfo(const CString& strFilePath)
{
    CString strFileText;
    if(m_FileOperation.GetFileSize(strFilePath) > DEFAULT_OPEN_FILE_SIZE)
    {
        SetItemVisible(IDC_NO_PREVIEW, TRUE);
        SetItemVisible(IDC_PREVIEW_RICHEDIT, FALSE);
        goto Exit0;
    }
    if(!m_FileOperation.StringEndWithDocumentSuffix(strFilePath))
    {
        SetItemVisible(IDC_NO_PREVIEW, TRUE);
        SetItemVisible(IDC_PREVIEW_RICHEDIT, FALSE);
        goto Exit0;
    }
    SetItemVisible(IDC_NO_PREVIEW, FALSE);
    SetItemVisible(IDC_PREVIEW_RICHEDIT, TRUE);
    m_FileOperation.ShowFileBriefInfo(strFilePath, strFileText);
    SendMessage(m_PreViewRichEdit.m_hWnd, WM_VSCROLL, SB_TOP, NULL);
    SendMessage(m_PreViewRichEdit.m_hWnd, WM_HSCROLL, SB_LEFT, NULL);

Exit0:
    if(strFileText.IsEmpty())
    {
        strFileText = "暂无预览";
    }
    ::SetWindowText(m_PreViewRichEdit.m_hWnd, strFileText);
}

void KMainDlg::ShowFileInfo(const CString& strFilePath)
{
    ShowFileAtrributeInfo(strFilePath);
    ShowFileBriefInfo(strFilePath);
}

LRESULT KMainDlg::OnDoubleClickList(int uMsg, LPNMHDR pNMHDR, BOOL Handled)
{
    NMLISTVIEW* pNMListView = NULL;
    LVHITTESTINFO hitinfo = {0};
    int nItem = -1;
    const CString* pstrFilePath = NULL;
    CString strFilePath;

    pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
    if(NULL == pNMListView)
    {
        goto Exit0;
    }
    hitinfo.pt = pNMListView->ptAction;
    nItem = m_ListView.HitTest(&hitinfo);
    m_ListView.SetSelectedRow(nItem);
    if(-1 != nItem)
    {
        m_pThreadController->BuildPath(m_pCurrentTask->GetFileInfoVector()->at(nItem), &pstrFilePath);
        if(NULL == pstrFilePath)
        {
            goto Exit0;
        }
        strFilePath.Format(L"%c:%s%s", m_pCurrentTask->GetFileInfoVector()->at(nItem).m_cDriveName
            , *pstrFilePath, *(m_pCurrentTask->GetFileInfoVector()->at(nItem).m_pstrFileName));
        m_FileOperation.OpenFile(strFilePath);
    }

Exit0:
    return 0;
}

LRESULT KMainDlg::OnClickList(int uMsg, LPNMHDR pNMHDR, BOOL Handled) 
{
    NMLISTVIEW* pNMListView = NULL;
    LVHITTESTINFO hitinfo = {0};
    int nItem = -1;
    const CString* pstrFilePath = NULL;
    CString strFilePath;

    pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
    if(NULL == pNMListView)
    {
        goto Exit0;
    }

    hitinfo.pt = pNMListView->ptAction;
    nItem = m_ListView.HitTest(&hitinfo);
    m_ListView.SetSelectedRow(nItem);

    if(-1 != nItem)
    {
        m_pThreadController->BuildPath(m_pCurrentTask->GetFileInfoVector()->at(nItem), &pstrFilePath);
        if(NULL == pstrFilePath)
        {
            goto Exit0;
        }
        strFilePath.Format(L"%c:%s%s", m_pCurrentTask->GetFileInfoVector()->at(nItem).m_cDriveName
            , *pstrFilePath, *(m_pCurrentTask->GetFileInfoVector()->at(nItem).m_pstrFileName));
        ShowFileInfo(strFilePath);
    }
    if(!m_bPreviewBtnSelected)
    {
        ResetPreViewBtnState(nItem);
    }

Exit0:
    return 0;
}

void KMainDlg::ResetPreViewBtnState(int nSelectedRow)
{
    if(-1 != nSelectedRow)
    {
        m_nFlashTimes = 0;
    }
    m_nPreviewBtnSubIndex = 0;
    SetItemIntAttribute(IDC_BTN_PREVIEW, "sub", m_nPreviewBtnSubIndex);
}

void KMainDlg::DeleteSortedColumnIcon(HDITEM& hHdi, CHeaderCtrl& WndHdr)
{
    WndHdr = m_ListView.GetHeader();
    if(-1 != m_nSortedColumn)
    {
        WndHdr.GetItem(m_nSortedColumn, &hHdi);
        hHdi.fmt &= ~(HDF_SORTDOWN | HDF_SORTUP);
        WndHdr.SetItem(m_nSortedColumn, &hHdi);
    }
}

void KMainDlg::ShowSortedColumnIcon()
{
    HDITEM hHdi = {HDI_FORMAT};
    CHeaderCtrl WndHdr;

    DeleteSortedColumnIcon(hHdi, WndHdr);

    hHdi.mask = HDI_FORMAT;
    WndHdr.GetItem(m_nSortingColumn, &hHdi);
    hHdi.fmt |= m_bSortByAscent ? HDF_SORTUP : HDF_SORTDOWN;
    WndHdr.SetItem(m_nSortingColumn, &hHdi);
    m_nSortedColumn = m_nSortingColumn;
}

LRESULT KMainDlg::SortAllFiles(int uMsg, LPNMHDR pNMHDR, BOOL Handled)
{
    LPNMLISTVIEW pNMListView = NULL;
    KTaskBase* pTask = NULL;
    BOOL bSort = TRUE;
    CString strFileSortInfo;

    if(m_bIsSorting || m_bIsConditionSearching || m_bIsFiltering)
    {
        goto Exit0;
    }
    pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
    if(NULL == pNMListView || NULL == m_pCurrentTask 
        || NULL == m_pCurrentTask->GetFileInfoVector())
    {
        goto Exit0;
    }
    m_nSortingColumn = pNMListView->iSubItem;
    if(m_nSortingColumn == m_nSortedColumn)
    {
        m_bSortByAscent = !m_bSortByAscent;
    }
    else
    {
        m_bSortByAscent = TRUE;
    }
    _WarningBeforeSort(strFileSortInfo, bSort); 
    if(bSort)
    {
        _AddNewSortTask(pTask);
    }

Exit0:
    return 0;
}

void KMainDlg::_WarningBeforeSort(CString& strFileSortInfo, BOOL& bSort)
{
    if(COL_FILE_SIZE == m_nSortingColumn || COL_FILE_MODIFIED_TIME == m_nSortingColumn)
    {
        size_t nSize = m_pCurrentTask->GetFileInfoVector()->size();
        if(nSize > 100000)
        {
            m_bHasMessageBox = TRUE;
            strFileSortInfo.Format(L"Are you sure to sort %d itmes?\nGathering data and sorting could take several minutes.", nSize);
            if(IDCANCEL == MessageBox(strFileSortInfo, L"Confirm Sort", MB_OKCANCEL))
            {
                bSort = FALSE;
            }
            m_bHasMessageBox = FALSE;
        }
    }
}

void KMainDlg::_AddNewSortTask(KTaskBase* &pTask)
{
    pTask = new KSortTask(m_hWnd, m_pThreadController, m_ListView, 
        m_pCurrentTask->GetFileInfoVector(), m_nSortingColumn, m_bSortByAscent);
    if(NULL == pTask)
    {
        goto Exit0;
    }
    m_bIsSorting = TRUE;
    SetStatusBarInfo(L"正在排序...");
    ShowSortedColumnIcon();
    m_ThreadPool.QueueRequest( (KWorker::RequestType)pTask);
    m_queTaskQueue.push(pTask);
Exit0:
    return; 
}

void KMainDlg::GetFileName(const CString& strFilePath, CString& strFileName)
{
    int nPos = -1;
    int nLen = strFilePath.GetLength();

    nPos = strFilePath.ReverseFind('\\');
    if(-1 == nPos)
    {
        strFileName = strFilePath.Left(nLen - 1);
    }
    else
    {
        strFileName = strFilePath.Mid(nPos + 1, nLen - 1 - nPos);
    }
}


LRESULT KMainDlg::GetDispInfo(int uMsg, LPNMHDR pNMHDR, BOOL Handled) 
{
    LV_DISPINFO* pDispInfo = NULL;
    LV_ITEM* pItem = NULL;  
    DRIVE_AND_FRN struFileInfo = {0};
    const CString* pstrFilePath = NULL;
    CString strFilePath;

    if(NULL == m_pCurrentTask->GetFileInfoVector())
    {
        goto Exit0;
    }

    pDispInfo = reinterpret_cast<LV_DISPINFO*>(pNMHDR);
    if(NULL == pDispInfo)
    {
        goto Exit0;
    }
    pItem = &(pDispInfo)->item;
    struFileInfo = m_pCurrentTask->GetFileInfoVector()->at(pItem->iItem);
    m_pThreadController->BuildPath(struFileInfo, &pstrFilePath);
    if(NULL == pstrFilePath)
    {
        goto Exit0;
    }
    strFilePath.Format(L"%c:%s%s", struFileInfo.m_cDriveName, *pstrFilePath, *struFileInfo.m_pstrFileName);

    SetSubItemText(pItem, strFilePath);
    SetItemImage(pItem, strFilePath);

Exit0:
    return 0;
}

void KMainDlg::SetSubItemText(LV_ITEM* pItem, const CString& strFilePath)
{
    if (NULL != pItem && pItem->mask & LVIF_TEXT)
    {
        FillColInfoByColName(pItem, strFilePath, pItem->iSubItem);
    }
} 

void KMainDlg::SetItemImage(LV_ITEM* pItem, const CString& strFilePath)
{
    if(NULL != pItem && pItem->mask & LVIF_IMAGE) 
    {
        pItem->iImage = GetIconIndex(strFilePath);
    }
} 

void KMainDlg::InitUnknownFileIcon()
{
    SHFILEINFO    Shinfo = {0};
    SHGetFileInfo(L"unknown.file", 0, &Shinfo, 
        sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_USEFILEATTRIBUTES);
    m_ImageHead.AddIcon(Shinfo.hIcon);
    m_vHicon.push_back(&Shinfo.hIcon);
    m_IconIndexMap.insert(std::make_pair(Shinfo.iIcon, 0));
}


int KMainDlg::GetIconIndex(const CString& strFilePath)
{
    int nIconIndex = 0;
    SHFILEINFO    Shinfo = {0};
    SHGetFileInfo(strFilePath, FILE_ATTRIBUTE_NORMAL, &Shinfo, 
        sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_SMALLICON);
    std::map<int, int>::iterator it = m_IconIndexMap.find(Shinfo.iIcon);
    if(it == m_IconIndexMap.end())
    {
        if(Shinfo.hIcon)
        {
            m_vHicon.push_back(&Shinfo.hIcon);
            nIconIndex = m_ImageHead.AddIcon(Shinfo.hIcon);
            m_IconIndexMap.insert(std::make_pair(Shinfo.iIcon, nIconIndex));
        }
        else
        {
            nIconIndex = 0;
        }
    }
    else
    {
        nIconIndex = it->second;
        ::DestroyIcon(Shinfo.hIcon);
    }
    return nIconIndex;
}

void KMainDlg::SetStatusBarFileCount(size_t uCount)
{
    CString strInfo;
    strInfo.Format(L"%d Objects", uCount);
    SetStatusBarInfo(strInfo);
}

LRESULT KMainDlg::OnUpdateFileInfo(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if(NULL == lParam)
    {
        goto Exit0;
    } 
    if(m_pCurrentTask)
    {
        if(m_pCurrentTask->GetFileInfoVector())
        {
            m_pCurrentTask->FreeMemory();
        }
        m_pCurrentTask->SetTaskState(TRUE);
    }
    m_pCurrentTask = (KSearchTask*)lParam;

    if(!m_bIsSorting)
    {
        SetStatusBarFileCount(m_pCurrentTask->GetFileInfoVector()->size());
    }

Exit0:
    return 0;
}

LRESULT KMainDlg::OnUpdateFileSort(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    m_bIsSorting = !lParam;
    if(NULL != m_pCurrentTask && NULL != m_pCurrentTask->GetFileInfoVector())
    {
        SetStatusBarFileCount(m_pCurrentTask->GetFileInfoVector()->size());
    }  
    return 0;
}

LRESULT KMainDlg::OnUpdateListView(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if(!m_strFileName.IsEmpty())
    {
        ::SetFocus(m_ListView.m_hWnd);
        m_ListView.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
    }
    return 0;
}

LRESULT KMainDlg::OnUpdateFileFilter(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    m_bIsFiltering = FALSE;
    if(NULL != m_pCurrentTask && NULL != m_pCurrentTask->GetFileInfoVector())
    {
        SetStatusBarFileCount(m_pCurrentTask->GetFileInfoVector()->size());
    }
    return 0;
}

LRESULT KMainDlg::OnUpdateFileConditionSearch(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    m_bIsConditionSearching = FALSE;
    if(NULL != m_pCurrentTask && NULL != m_pCurrentTask->GetFileInfoVector())
    {
        SetStatusBarFileCount(m_pCurrentTask->GetFileInfoVector()->size());
    }
    return 0;
}

void KMainDlg::SetHeadInfo(const CString& strFileInfo)
{
    SetItemText(IDC_HEAD_INFO_TEXT_ID, strFileInfo);
}

LRESULT KMainDlg::OnUpdateFileCreateIndexCompleted(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    m_bCanSearch = lParam;
    SetHeadInfo(L"索引建立完成!");
    return 0;
}

void KMainDlg::DeleteCompletedTask()
{
    while (!m_queTaskQueue.empty())
    {
        if(NULL == m_queTaskQueue.front())
        {
            m_queTaskQueue.pop();
        }
        else if(m_queTaskQueue.front()->GetTaskState())
        {
            m_queTaskQueue.front()->FreeMemory();
            delete m_queTaskQueue.front();
            m_queTaskQueue.pop();  
        }
        else
        {
            break;
        }
    } 
}

void KMainDlg::FillColInfoByColName(LV_ITEM* pItem, const CString& strFilePath, int nColName)
{
    pFillCol pFillColFunc = NULL;
    BOOL bRet = FALSE; 
    std::map<int, pFillCol>::const_iterator it = m_FillColTypeToHandle.end();
    if(NULL == pItem)
    {
        goto Exit0;
    }
    it = m_FillColTypeToHandle.find(nColName);
    if(m_FillColTypeToHandle.end() == it)
    {
        goto Exit0;
    }
    pFillColFunc = it->second;
    (this->*pFillColFunc)(pItem, strFilePath); 
Exit0:
    return;  
}

void KMainDlg::AnimateWndTransparent()
{
    CPoint ptCurrentPos;
    CRect rcWnd;

    GetCursorPos(&ptCurrentPos);
    GetWindowRect(&rcWnd);

    if(rcWnd.PtInRect(ptCurrentPos))
    {
        if(m_nAlpha < ALPHA_UPPERBOUND)
        {
            m_nAlpha += ALPHA_CHANGE_INTERVAL;
            SetLayeredWindowAttributes(m_hWnd, NULL, m_nAlpha, LWA_ALPHA);
        }
    }
    else
    {
        if(m_nAlpha > ALPHA_LOWERBOUND) 
        {
            m_nAlpha -= ALPHA_CHANGE_INTERVAL;
            SetLayeredWindowAttributes(m_hWnd, NULL, m_nAlpha, LWA_ALPHA);
        }
    } 
}

void KMainDlg::ShowMainDialog()
{
    RECT rcRect={0};
    int nWidth = 0;
    int nHeight = 0;

    GetWindowRect(&rcRect);
    nWidth = rcRect.right - rcRect.left;
    nHeight = rcRect.bottom - rcRect.top;
    rcRect.top -= DEFAULT_INITIAL_HEIGHT; 

    MoveWindow(rcRect.left, rcRect.top, nWidth, nHeight);
    SetWindowEllispeFrame(WIDTH_ELLIPSE, HIEIGHT_ELLIPSE);
    ShowWindow(SW_SHOW);
    SetItemVisible(IDC_DLG_PREVIEW, FALSE);
    KillTimer(TIMER_ID_SHOW_MAIN_DIALGO);

    m_ListView.GetWindowRect(&rcRect);
    nWidth = rcRect.right - rcRect.left; 
    m_ListView.InitColumn(nWidth);  
}

void KMainDlg::AnimatePreViewBtn()
{
    if(-1 != m_ListView.GetSelectedRow() && m_nFlashTimes < ANIMATE_TIMES)
    {
        SetItemIntAttribute(IDC_BTN_PREVIEW, "sub", m_nPreviewBtnSubIndex);
        m_nPreviewBtnSubIndex = - (m_nPreviewBtnSubIndex - 3);
        ++m_nFlashTimes;         
    }
    else
    {        
        if(m_nPreviewBtnSubIndex == 3)
        {
            m_nPreviewBtnSubIndex = 0;
            SetItemIntAttribute(IDC_BTN_PREVIEW, "sub", m_nPreviewBtnSubIndex); 
        }    
    }
}

void KMainDlg::_AddSearchTask()
{
    KTaskBase* pTask = NULL;
    if(FALSE == m_bIsSearching)
    {
        goto Exit0;
    }

    if(GetTickCount() - m_dwSearchInputTime < DEFAULT_SEARCH_INTERVAL_TIME)
    {
        goto Exit0;
    }

    pTask  = new  KSearchTask(m_hWnd, *m_pThreadController
        , m_strFileName, m_ListView, m_Critical, m_bLUCaseBtnSelected);
    if(NULL == pTask || !pTask->MallocMemory())
    {
        goto Exit0;
    }
    if(NULL != m_pCurrentTask)
    {
        m_pCurrentTask->GetFileInfoVector()->clear();
        m_ListView.SetItemCountEx(static_cast<int>(m_pCurrentTask->GetFileInfoVector()->size())
            , LVSICF_NOINVALIDATEALL);
        m_ListView.Invalidate(TRUE);
    }
    m_pPreSearchTask = pTask;
    m_ThreadPool.QueueRequest( (KWorker::RequestType)pTask);
    m_queTaskQueue.push(pTask);
    m_bIsSearching = FALSE;

Exit0:
    return;
}

void KMainDlg::ShowAnimate()
{
    if(!m_bPreviewBtnSelected)
    {
        AnimatePreViewBtn();
    }
}

void KMainDlg::FillTimerByType(int nTimer)
{
    pFillTimer pFillTimerFunc = NULL;
    std::map<int, pFillTimer>::const_iterator it = m_FillTimerTyopeToHandle.find(nTimer);

    if(m_FillTimerTyopeToHandle.end() == it)
    {
        goto Exit0;
    }

    pFillTimerFunc = it->second;
    (this->*pFillTimerFunc)(); 

Exit0:
    return;  
}

void KMainDlg::OnTimer(UINT_PTR nIDEvent)
{
    FillTimerByType(nIDEvent);
}

void KMainDlg::OnMouseMove(UINT nFlags, CPoint cPoint)
{
    SetImgBtnState(cPoint);
}

void KMainDlg::SetImgBtnState(CPoint cPoint)
{
    if(!m_bPreviewBtnSelected)
    {
        SetPreViewBtnState(cPoint);
    }
    if(!m_bLUCaseBtnSelected)
    {
        SetSelfDefineBtnState(IDC_BTN_LUCASE ,cPoint);
    }
    if(!m_bDetailBtnSelected)
    {
        SetSelfDefineBtnState(IDC_BTN_DETAIL, cPoint);
    }
    if(!m_bKindBtnSelected)
    {
        SetSelfDefineBtnState(IDC_BTN_KIND, cPoint);
    } 
    if(!m_bHighBtnSelected)
    {
        SetSelfDefineBtnState(IDC_BTN_HIGHLEVEL, cPoint);
    } 
}

void KMainDlg::SetPreViewBtnState(CPoint cPoint)
{
    if(IsOverBtn(IDC_BTN_PREVIEW, cPoint))
    {
        m_nPreviewBtnSubIndex = 1;
        SetItemIntAttribute(IDC_BTN_PREVIEW, "sub", m_nPreviewBtnSubIndex);
        m_nFlashTimes = ANIMATE_TIMES;
    }
    else if(m_nFlashTimes >= ANIMATE_TIMES && m_nPreviewBtnSubIndex != 0)
    {
        m_nPreviewBtnSubIndex = 0;
        SetItemIntAttribute(IDC_BTN_PREVIEW, "sub", m_nPreviewBtnSubIndex);
    } 
} 

void KMainDlg::SetSelfDefineBtnState(UINT uItemID, CPoint cPoint)
{
    if(IsOverBtn(uItemID, cPoint))
    {
        SetItemIntAttribute(uItemID, "sub", 1);
    }
    else
    {
        SetItemIntAttribute(uItemID, "sub", 0);
    }
} 

BOOL KMainDlg::IsOverBtn(UINT uItemID , CPoint cPoint)
{
    BOOL bRet = FALSE; 
    RECT rcBtn = {0};
    GetItemRect(uItemID, rcBtn);
    if(PtInRect(&rcBtn, cPoint))
    {
        bRet = TRUE;
    }  
    return bRet;
}

void KMainDlg::_OnFilterBtnSelected(int nFilterType, const CString& strFilterInfo)
{
    KTaskBase* pTask = NULL;
    std::vector<DRIVE_AND_FRN>* pvFileInfo = NULL;

    if(m_bIsFiltering  || m_bIsConditionSearching || 
        m_bIsSorting|| NULL == m_pCurrentTask || 
        NULL == m_pCurrentTask->GetFileInfoVector())
    {
        goto Exit0;
    }

    pvFileInfo = m_pCurrentTask->GetFileInfoVector();
    pTask = new KFilterTask(m_hWnd, m_pThreadController, m_ListView, pvFileInfo, nFilterType, m_strFileName, m_Critical, m_bLUCaseBtnSelected);

    SetStatusBarInfo(strFilterInfo);
    if(NULL != m_pPreFilterTask)
    {
        m_pPreFilterTask->SetTaskInValid();
    }

    ClearColumnHeadIcon();
    m_pPreFilterTask = pTask;
    m_ThreadPool.QueueRequest( (KWorker::RequestType)pTask);
    m_queTaskQueue.push(pTask);
    m_bIsFiltering = TRUE;

Exit0:
    return;
}

void KMainDlg::OnAllFileSelected()
{
    _OnFilterBtnSelected(FILE_FILTER_NONE, L"正在显示所有文件...");
}

void KMainDlg::ClearColumnHeadIcon()
{
    HDITEM hHdi = {HDI_FORMAT};
    CHeaderCtrl WndHdr;

    DeleteSortedColumnIcon(hHdi, WndHdr);
    m_nSortedColumn = -1;
}

void KMainDlg::OnDocBtnSelected()
{
    _OnFilterBtnSelected(FILE_FILTER_DOCUMENT, L"正在过滤文档...");
}

void KMainDlg::OnImgBtnSelected()
{
    _OnFilterBtnSelected(FILE_FILTER_IMAGE, L"正在过滤图片...");
}

void KMainDlg::OnVedioBtnSelected()
{
    _OnFilterBtnSelected(FILE_FILTER_VEDIO, L"正在过滤视频...");
}

void KMainDlg::OnMusicBtnSelected()
{
    _OnFilterBtnSelected(FILE_FILTER_MUSIC, L"正在过滤音乐...");
}

void KMainDlg::ExtendMainDialog()
{
    RECT rRect={0};
    int nWidth = 0;
    int nHeight = 0;

    GetWindowRect(&rRect);
    nHeight = rRect.bottom - rRect.top;

    if(!m_bIsExtendDialog)
    {
        nWidth = rRect.right - rRect.left + DIALOG_NEW_ADD_WIDTH;
        m_bIsExtendDialog = TRUE;
    }
    else
    {
        nWidth = rRect.right - rRect.left - DIALOG_NEW_ADD_WIDTH;
        m_bIsExtendDialog = FALSE;
    }

    MoveWindow(rRect.left, rRect.top, nWidth , nHeight);
    SetWindowEllispeFrame(WIDTH_ELLIPSE, HIEIGHT_ELLIPSE);
}

void KMainDlg::_HideConditionSearchShowPreview()
{
    OnPreviewBtnDown();
    SetItemVisible(IDC_DLG_CONDITION_SEARCH, FALSE);
    SetItemVisible(IDC_BTN_CONFIRM, FALSE);
    SetItemVisible(IDC_DLG_PREVIEW, TRUE);
}

void KMainDlg::_HidePreviewShowConditionSearch()
{
    OnSelfDefineImgBtnDown(IDC_BTN_DETAIL, m_bDetailBtnSelected);
    SetItemVisible(IDC_DLG_PREVIEW, FALSE);
    SetItemVisible(IDC_DLG_CONDITION_SEARCH, TRUE);
    SetItemVisible(IDC_BTN_CONFIRM, TRUE);
}

void KMainDlg::OnDetailBtnDown()
{
    if(m_bPreviewBtnSelected)
    {
        OnPreviewBtnDown();
    }
    else
    {
        ExtendMainDialog();
    }
    _HidePreviewShowConditionSearch();
}

void KMainDlg::OnPreviewBtnSelected()
{
    if(m_bDetailBtnSelected)
    {
        OnSelfDefineImgBtnDown(IDC_BTN_DETAIL, m_bDetailBtnSelected);
    }
    else
    {
        ExtendMainDialog();
    }
    _HideConditionSearchShowPreview();
}

void KMainDlg::OnFolderBtnSelected()
{
    _OnFilterBtnSelected(FILE_FILTER_DIRECORY, L"正在过滤文件夹...");
}

void KMainDlg::OnPreviewBtnDown()
{
    if(!m_bPreviewBtnSelected)
    {
        m_bPreviewBtnSelected = TRUE;
        m_nFlashTimes = ANIMATE_TIMES;
        m_nPreviewBtnSubIndex = 2;
        SetItemIntAttribute(IDC_BTN_PREVIEW, "sub", m_nPreviewBtnSubIndex);          
    }
    else
    {
        m_bPreviewBtnSelected = FALSE;
        m_nPreviewBtnSubIndex = 0;
        SetItemIntAttribute(IDC_BTN_PREVIEW, "sub", m_nPreviewBtnSubIndex);    
    }

}

void KMainDlg::OnSelfDefineImgBtnDown(UINT uItemID, BOOL& bBtnState)
{
    if(!bBtnState)
    {
        bBtnState = TRUE;
        SetItemIntAttribute(uItemID, "sub", 2); 
    }
    else
    {
        bBtnState = FALSE;
        SetItemIntAttribute(uItemID, "sub", 0); 
    }
    _SetCaseTip(); 
}

void KMainDlg::OnMicroSmallCheck()
{
    if(!m_bSelectedFileConditionSizeType[FILE_CONDITION_SEARCH_SIZE_MICROSMALL])
    {
        m_bSelectedFileConditionSizeType[FILE_CONDITION_SEARCH_SIZE_MICROSMALL] = TRUE;
    }
    else
    {
        m_bSelectedFileConditionSizeType[FILE_CONDITION_SEARCH_SIZE_MICROSMALL] = FALSE;
    }
}

void KMainDlg::OnSmallCheck()
{
    if(!m_bSelectedFileConditionSizeType[FILE_CONDITION_SEARCH_SIZE_SMALL])
    {
        m_bSelectedFileConditionSizeType[FILE_CONDITION_SEARCH_SIZE_SMALL] = TRUE;
    }
    else
    {
        m_bSelectedFileConditionSizeType[FILE_CONDITION_SEARCH_SIZE_SMALL] = FALSE;
    }
}

void KMainDlg::OnMidCheck()
{
    if(!m_bSelectedFileConditionSizeType[FILE_CONDITION_SEARCH_SIZE_MID])
    {
        m_bSelectedFileConditionSizeType[FILE_CONDITION_SEARCH_SIZE_MID] = TRUE;
    }
    else
    {
        m_bSelectedFileConditionSizeType[FILE_CONDITION_SEARCH_SIZE_MID] = FALSE;
    }
}

void KMainDlg::OnLargeCheck()
{
    if(!m_bSelectedFileConditionSizeType[FILE_CONDITION_SEARCH_SIZE_LARGE])
    {
        m_bSelectedFileConditionSizeType[FILE_CONDITION_SEARCH_SIZE_LARGE] = TRUE;
    }
    else
    {
        m_bSelectedFileConditionSizeType[FILE_CONDITION_SEARCH_SIZE_LARGE] = FALSE;
    }
}

void KMainDlg::OnSuperLargeCheck()
{
    if(!m_bSelectedFileConditionSizeType[FILE_CONDITION_SEARCH_SIZE_SUPERLARGE])
    {
        m_bSelectedFileConditionSizeType[FILE_CONDITION_SEARCH_SIZE_SUPERLARGE] = TRUE;
    }
    else
    {
        m_bSelectedFileConditionSizeType[FILE_CONDITION_SEARCH_SIZE_SUPERLARGE] = FALSE;
    }
}

void KMainDlg::OnLUCaseBtnDown()
{
    OnSelfDefineImgBtnDown(IDC_BTN_LUCASE, m_bLUCaseBtnSelected);
}

BOOL KMainDlg::_FileConditionSizeSatisfied()
{
    BOOL bRet = FALSE;
    for(int i = 0; i < FILE_CONDITION_SIZE_SEARCH_TYPE_NUM; ++i)
    {
        if(GetItemCheck(IDC_CHECK_MICRO_SMALL_SIZE + i))
        {
            bRet = TRUE;
            break;
        }
    }
    return bRet;
}

BOOL KMainDlg::_FileConditionTimeSatisfied()
{
    return m_nFileConditionTimeType == -1 ? FALSE : TRUE;
}

void KMainDlg::_SetCaseTip()
{
    if(m_bLUCaseBtnSelected)
    {
        SetItemVisible(IDC_HEAD_INFO_TEXT_ID, TRUE);
        SetHeadInfo(L"区分大小写");
    }
    else
    {
        SetItemVisible(IDC_HEAD_INFO_TEXT_ID, FALSE);
    }
}

void KMainDlg::FileConditionSearch( BOOL bFileConditionSizeSelected,
                                   const COleDateTime& FileConditionTimeLow, const COleDateTime& FileConditionTimeHigh)
{
    KTaskBase* pTask = NULL;
    std::vector<DRIVE_AND_FRN>* pvFileInfo = NULL;

    if(m_bIsConditionSearching || m_bIsFiltering || 
        m_bIsSorting|| NULL == m_pCurrentTask || 
        NULL == m_pCurrentTask->GetFileInfoVector())
    {
        goto Exit0;
    }

    pvFileInfo = m_pCurrentTask->GetFileInfoVector();
    pTask = new KConditionSearchTask(m_hWnd, m_pThreadController, m_ListView, pvFileInfo
        , m_bSelectedFileConditionSizeType,bFileConditionSizeSelected, FileConditionTimeLow
        , FileConditionTimeHigh, m_strFileName, m_Critical, m_bLUCaseBtnSelected);

    SetStatusBarInfo(L"正在条件搜索...");
    if(NULL != m_pPreFilterTask)
    {
        m_pPreFilterTask->SetTaskInValid();
    }
    ClearColumnHeadIcon();
    m_pPreFilterTask = pTask;
    m_ThreadPool.QueueRequest( (KWorker::RequestType)pTask);
    m_queTaskQueue.push(pTask);
    m_bIsConditionSearching = TRUE;

Exit0:
    return;
}

void KMainDlg:: _SizeSortWarning()
{
    m_bHasMessageBox = TRUE;
    m_bHasMessageBox = FALSE;
}

void KMainDlg::_KindSortWarning()
{
    m_bHasMessageBox = TRUE;
    m_bHasMessageBox = FALSE;
}

void KMainDlg::_TimeSortWarning()
{
    m_bHasMessageBox = TRUE;
    m_bHasMessageBox = FALSE;
}
void KMainDlg::_SetCOleDateTime(COleDateTime& FileConditionTimeLow
                                , COleDateTime& FileConditionTimeHigh
                                , SYSTEMTIME& SysTimePre
                                , SYSTEMTIME& SysTimePost)
{
    m_DateTimePickerIntervalPre.GetSystemTime(&SysTimePre);
    _SetTime(SysTimePre);
    m_DateTimePickerIntervalPost.GetSystemTime(&SysTimePost);
    _SetTime(SysTimePost); 
    FileConditionTimeLow = COleDateTime(SysTimePre);
    FileConditionTimeHigh = COleDateTime(SysTimePost);
}

void KMainDlg::OnConfirmDown()
{
    BOOL bFileConditionSizeSelected = FALSE;
    BOOL bFileConditionTimeSelected = FALSE;
    COleDateTime FileConditionTimeLow;
    COleDateTime FileConditionTimeHigh;
    SYSTEMTIME SysTimePre = {0};
    SYSTEMTIME SysTimePost = {0};

    bFileConditionSizeSelected = GetItemCheck(IDC_CHECK_CONDITION_FILE_SIZE);
    bFileConditionTimeSelected = GetItemCheck(IDC_CHECK_CONDITION_FILE_TIME);

    if(!bFileConditionTimeSelected && !bFileConditionSizeSelected)
    {
        _KindSortWarning();
        goto Exit0;
    }

    if(bFileConditionSizeSelected)
    {
        if(!_FileConditionSizeSatisfied())
        {
            _SizeSortWarning();
            goto Exit0;
        }
    }

    if(bFileConditionTimeSelected)
    {
        if(!_FileConditionTimeSatisfied())
        {
            _TimeSortWarning();
            goto Exit0;
        }
        else
        {
            if(FILE_CONDITION_SEARCH_TIME_INTERVAL == m_nFileConditionTimeType)
            {
                _SetCOleDateTime(FileConditionTimeLow , FileConditionTimeHigh
                    , SysTimePre, SysTimePost); 
            }
            else if(FILE_CONDITION_SEARCH_TIME_PRE == m_nFileConditionTimeType)
            {
                m_DateTimePickerPre.GetSystemTime(&SysTimePre);
                FileConditionTimeLow = COleDateTime(SysTimePre);
            }
            else
            {
                m_DateTimePickerPost.GetSystemTime(&SysTimePost);
                FileConditionTimeHigh = COleDateTime(SysTimePost);
            }
        }
    }
    FileConditionSearch(bFileConditionSizeSelected, FileConditionTimeLow, FileConditionTimeHigh);

Exit0:
    return;
}



void KMainDlg::_SetTime(SYSTEMTIME& Systime)
{
    Systime.wHour = 0;
    Systime.wMilliseconds = 0;
    Systime.wMinute = 0;
    Systime.wSecond = 0;
}

void KMainDlg::OnTimePreRadio()
{
    m_nFileConditionTimeType =  FILE_CONDITION_SEARCH_TIME_PRE;
}

void KMainDlg::OnTimePostRadio()
{
    m_nFileConditionTimeType =  FILE_CONDITION_SEARCH_TIME_POST;
}

void KMainDlg::OnTimeIntervalRadio()
{
    m_nFileConditionTimeType =  FILE_CONDITION_SEARCH_TIME_INTERVAL;
}


void KMainDlg::OnSelectFileConditionSize()
{
    if(!m_bSelectedFileConditionSize)
    {
        m_bSelectedFileConditionSize = TRUE;
    }
    else
    {
        m_bSelectedFileConditionSize = FALSE;
    }
}

void KMainDlg::OnSelectFileConditionTime()
{
    if(!m_bSelectedFileConditionTime)
    {
        m_bSelectedFileConditionTime = TRUE;
    }
    else
    {
        m_bSelectedFileConditionTime = FALSE;
    }
}

BOOL KMainDlg::_MapFillColTypeToHandle()
{
    m_FillColTypeToHandle[COL_FILE_NAME] = &KMainDlg::_SetColFileName;
    m_FillColTypeToHandle[COL_FILE_PATH] = &KMainDlg::_SetColFilePath;
    m_FillColTypeToHandle[COL_FILE_SIZE] = &KMainDlg::_SetColFileSize;
    m_FillColTypeToHandle [COL_FILE_MODIFIED_TIME] = &KMainDlg::_SetColFileModifyTime;
    return TRUE;
}

BOOL KMainDlg::_MapTimerTypeToHandle()
{
    m_FillTimerTyopeToHandle[TIMER_ID_ONE] = &KMainDlg::_AnimateTimer;
    m_FillTimerTyopeToHandle[TIMER_ID_SHOW_MAIN_DIALGO] = &KMainDlg::_ShowMainDialogTimer;
    m_FillTimerTyopeToHandle[TIMER_ID_SEARCH_WAITTING] = &KMainDlg::_SearchWaittingTimer;
    m_FillTimerTyopeToHandle[TIMER_ID_CLEAR_UNUSED_TASK] = &KMainDlg::_ClearTaskTimer;
    return TRUE;
}

void KMainDlg::_AnimateTimer()
{
    ShowAnimate();
}

void KMainDlg::_ShowMainDialogTimer()
{
    ShowMainDialog();
}

void KMainDlg::_SearchWaittingTimer()
{
    _AddSearchTask();
}

void KMainDlg::_ClearTaskTimer()
{
    DeleteCompletedTask();
}

void KMainDlg::_SetColFileName(LV_ITEM* pItem, const CString& strFilePath)
{
    if(NULL != pItem)
    {
        CString strFileName;
        GetFileName(strFilePath, strFileName);
        lstrcpyn(pItem->pszText, strFileName, strFileName.GetLength() + 1);
    } 
}

void KMainDlg::_SetColFilePath(LV_ITEM* pItem, const CString& strFilePath)
{
    if(NULL != pItem) 
    {
        lstrcpyn(pItem->pszText, strFilePath, strFilePath.GetLength() + 1);
    }
}

void KMainDlg::_SetColFileSize(LV_ITEM* pItem, const CString& strFilePath)
{
    if(NULL != pItem) 
    {
        CString strFileSize = "";
        if(!(FILE_ATTRIBUTE_DIRECTORY & GetFileAttributes(strFilePath)))
        {
            UINT64 ulFileSize = 0;
            ulFileSize =  m_FileOperation.GetFileSize(strFilePath);
            m_FileOperation.GetFileSizeStr(ulFileSize, strFileSize);
        };
        lstrcpyn(pItem->pszText, strFileSize, strFileSize.GetLength() + 1);
    }
}

void KMainDlg::_SetColFileModifyTime(LV_ITEM* pItem, const CString& strFilePath)
{
    if(NULL != pItem) 
    {
        CString strFileModifyTime;
        m_FileOperation.GetFileModifyTime(strFilePath, strFileModifyTime);
        lstrcpyn(pItem->pszText, strFileModifyTime, strFileModifyTime.GetLength() + 1);
    }
}

