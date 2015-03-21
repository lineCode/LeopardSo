#pragma once

#include <cstdio>
#include <utility>
#include <vector>
#include <string>
#include <map>
#include <cstdlib>
#include <queue>
#include <bkres/bkres.h>
#include "otherresource.h"
#include "bkwin/bkdlgview.h"
#include "kfiledata/kfilefilter.h"
#include "kfiledata/kfileinfosort.h"
#include "klistview.h"
#include "ksorttask.h"
#include "kfiledata/kfileconditionsearch.h"
#include "kfiledata/kfileoperation.h"
#include "kfiledata/kusinformation.h"
#include "kfiledata/kcriticalsesion.h"
#include "ksearchtask.h"
#include "kconditionsearchtask.h"
#include "kfiltertask.h"
#include "resource.h"
#include "kworker.h"
#include "ktaskbase.h"
#include "kmyedit.h"
#include "kglobalkeyboardhook.h"

class KMainDlg
    : public CBkDialogImpl<KMainDlg>
{
public:
    KMainDlg();
    ~KMainDlg();

protected:
    void ClearAllTask();
    void ClearHicon();
    void InitSearchEdit();
    void InitPreViewRichEdit();

    void InitListView();
    void InitOther();
    void InitConditionSearchControl();
    void SetWindowEllispeFrame(int nWidthEllipse, int nHeightEllipse);

    void SetAllTimer();
    void SetStatusBarInfo(const CString& strInfo);
    void OnSysClose();
    void HideDialog();

    void OnActivate(UINT uMsg, BOOL wParam, HWND lParam);
    void OnContextMenuGen(HWND wParam, _WTYPES_NS::CPoint cPoint);
    void OnCommandHandlerEX(UINT uNotifyCode, int nID, CWindow wndCtl);

    void ShowFileAtrributeInfo(const CString& strFilePath);
    void ShowFileBriefInfo(const CString& strFilePath);
    void ShowFileInfo(const CString& strFilePath);

    void ResetPreViewBtnState(int nSelectedRow);
    void DeleteSortedColumnIcon(HDITEM& hHdi, CHeaderCtrl& WndHdr);
    void ShowSortedColumnIcon();

    void GetFileName(const CString& strFilePath, CString& strFileName);
    void SetSubItemText(LV_ITEM* pItem, const CString& strFilePath);
    void SetItemImage(LV_ITEM* pItem, const CString& strFilePath);
    void InitUnknownFileIcon();

    void SetStatusBarFileCount(size_t uCount);
    void DeleteCompletedTask();
    void FillColInfoByColName(LV_ITEM* pItem, const CString& strFilePath, int nColName);
    void AnimateWndTransparent();

    void ShowMainDialog();
    void AnimatePreViewBtn();
    void OnTimer(UINT_PTR nIDEvent);
    void OnMouseMove(UINT nFlags, CPoint cPoint);

    void SetImgBtnState(CPoint cPoint);
    void SetPreViewBtnState(CPoint cPoint);
    void SetSelfDefineBtnState(UINT uItemID, CPoint cPoint);
    void ClearColumnHeadIcon();

    void OnAllFileSelected();
    void OnDocBtnSelected();
    void OnImgBtnSelected();
    void OnVedioBtnSelected();
    void OnMusicBtnSelected();

    void ExtendMainDialog();
    void OnDetailBtnDown();
    void OnPreviewBtnSelected();
    void OnFolderBtnSelected();
    void OnPreviewBtnDown();

    void OnSelfDefineImgBtnDown(UINT uItemID, BOOL& bBtnState);
    void OnMicroSmallCheck();
    void OnSmallCheck();
    void OnMidCheck();
    void OnLargeCheck();
    void OnSuperLargeCheck();
    void OnLUCaseBtnDown();
    void OnConfirmDown();
    void OnKindButtonDown();
    void OnHighLevelButtonDown(); 

    void OnTimePreRadio();
    void OnTimePostRadio();
    void OnTimeIntervalRadio();
    void OnSelectFileConditionSize();
    void OnSelectFileConditionTime();

    void FileConditionSearch( BOOL bFileConditionSizeSelected,
        const COleDateTime& FileConditionTimeLow, const COleDateTime& FileConditionTimeHigh);

    BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
    BOOL IsOverBtn(UINT uItemID , CPoint cPoint);
    int GetIconIndex(const CString& strFilePath);
    void SetHeadInfo(const CString& strFileInfo);

    void InitTray();
    void DeleteTray();
    void ShowDialog();
    void ShowAnimate();
    void FillTimerByType(int nTimer);

    LRESULT OnUpdateFileInfo(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnUpdateFileSort(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnUpdateFileFilter(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnUpdateListView(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnUpdateFileConditionSearch(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnUpdateFileCreateIndexCompleted(UINT uMsg, WPARAM wParam, LPARAM lParam);

    LRESULT OnOpenFile(UINT uParam, UINT wParam, HWND lParam, BOOL bHandled);
    LRESULT OnOpenFilePath(UINT uParam, UINT wParam, HWND lParam, BOOL bHandled);
    LRESULT OnCopyPath(UINT uParam, UINT wParam, HWND lParam, BOOL bHandled);
    LRESULT OnHotKey(int uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnClickList(int uMsg, LPNMHDR pNMHDR, BOOL Handled);
    LRESULT OnDoubleClickList(int uMsg, LPNMHDR pNMHDR, BOOL Handled);
    LRESULT SortAllFiles(int uMsg, LPNMHDR pNMHDR, BOOL Handled);
    LRESULT GetDispInfo(int uMsg, LPNMHDR pNMHDR, BOOL Handled);
    LRESULT OnSystemTrayIcon(UINT, WPARAM wParam, LPARAM lParam);
    LRESULT OpenDialog(UINT uParam, UINT wParam, HWND lParam, BOOL bHandled);
    LRESULT OnClose(UINT uParam, UINT wParam, HWND lParam, BOOL bHandled);
    LRESULT KMainDlg::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    BK_NOTIFY_MAP(IDC_RICHVIEW_WIN)
        BK_NOTIFY_ID_COMMAND(IDC_BTN_SYS_CLOSE, HideDialog)
        BK_NOTIFY_ID_COMMAND(IDC_BTN_ALLFILE, OnAllFileSelected)
        BK_NOTIFY_ID_COMMAND(IDC_BTN_DOC, OnDocBtnSelected)
        BK_NOTIFY_ID_COMMAND(IDC_BTN_IMG, OnImgBtnSelected)
        BK_NOTIFY_ID_COMMAND(IDC_BTN_VEDIO, OnVedioBtnSelected)
        BK_NOTIFY_ID_COMMAND(IDC_BTN_MUSIC, OnMusicBtnSelected)
        BK_NOTIFY_ID_COMMAND(IDC_BTN_FOLDER, OnFolderBtnSelected)
        BK_NOTIFY_ID_COMMAND(IDC_BTN_PREVIEW, OnPreviewBtnSelected)
        BK_NOTIFY_ID_COMMAND(IDC_BTN_LUCASE, OnLUCaseBtnDown)
        BK_NOTIFY_ID_COMMAND(IDC_BTN_DETAIL, OnDetailBtnDown)
        BK_NOTIFY_ID_COMMAND(IDC_BTN_CONFIRM, OnConfirmDown)
        BK_NOTIFY_ID_COMMAND(IDC_CHECK_MICRO_SMALL_SIZE, OnMicroSmallCheck)
        BK_NOTIFY_ID_COMMAND(IDC_CHECK_SMALL_SIZE, OnSmallCheck)
        BK_NOTIFY_ID_COMMAND(IDC_CHECK_MID_SIZE, OnMidCheck)
        BK_NOTIFY_ID_COMMAND(IDC_CHECK_LARGE_SIZE, OnLargeCheck)
        BK_NOTIFY_ID_COMMAND(IDC_CHECK_SUPER_LARGE_SIZE, OnSuperLargeCheck)
        BK_NOTIFY_ID_COMMAND(IDC_RADIO_PRE_TIME, OnTimePreRadio)
        BK_NOTIFY_ID_COMMAND(IDC_RADIO_POST_TIME, OnTimePostRadio)
        BK_NOTIFY_ID_COMMAND(IDC_RADIO_INTERVAL_TIME, OnTimeIntervalRadio)
        BK_NOTIFY_ID_COMMAND(IDC_BTN_KIND, OnKindButtonDown)
        BK_NOTIFY_ID_COMMAND(IDC_BTN_HIGHLEVEL, OnHighLevelButtonDown)
        BK_NOTIFY_MAP_END()

        BEGIN_MSG_MAP_EX(CBkDialogImpl<KMainDlg>)
        MSG_BK_NOTIFY(IDC_RICHVIEW_WIN)
        MSG_WM_INITDIALOG(OnInitDialog)
        COMMAND_HANDLER_EX(IDC_MAIN_SEARCH_EDIT, EN_CHANGE, OnCommandHandlerEX)
        MSG_WM_HOTKEY(OnHotKey)
        MSG_WM_ACTIVATE(OnActivate)
        MSG_WM_TIMER(OnTimer)
        MSG_WM_MOUSEMOVE(OnMouseMove)
        MSG_WM_CONTEXTMENU(OnContextMenuGen)
        MESSAGE_HANDLER_EX(WM_SYSTEMTRAYICON, OnSystemTrayIcon)
        MESSAGE_HANDLER_EX(UPDATE_FILE_FILTER, OnUpdateFileFilter)
        MESSAGE_HANDLER_EX(UPDATE_FILEDATA, OnUpdateFileInfo)
        MESSAGE_HANDLER_EX(UPDATE_FILESORT, OnUpdateFileSort)
        MESSAGE_HANDLER_EX(WM_UPDATE_LISTVIEW_STATE, OnUpdateListView)
        MESSAGE_HANDLER_EX(UPDATE_FILE_CONDITION_SEARCH, OnUpdateFileConditionSearch)
        MESSAGE_HANDLER_EX(UPDATE_FILE_CREATE_INDEX, OnUpdateFileCreateIndexCompleted)
        COMMAND_ID_HANDLER(ID_OPEN_FILE, OnOpenFile)
        COMMAND_ID_HANDLER(ID_OPEN_PATH, OnOpenFilePath)
        COMMAND_ID_HANDLER(ID_COPY_PATH, OnCopyPath)
        COMMAND_ID_HANDLER(ID_TRAYMENU_EXIT, OnClose)
        COMMAND_ID_HANDLER(ID_TRAYMENU_OPENMAINWND, OpenDialog)
        NOTIFY_HANDLER(IDC_MAIN_LIST, NM_CLICK, OnClickList)
        NOTIFY_HANDLER(IDC_MAIN_LIST, NM_DBLCLK, OnDoubleClickList)
        NOTIFY_HANDLER(IDC_MAIN_LIST, LVN_GETDISPINFO, GetDispInfo)
        NOTIFY_HANDLER(IDC_MAIN_LIST, LVN_COLUMNCLICK, SortAllFiles)
        REFLECT_NOTIFICATIONS_EX()
        CHAIN_MSG_MAP(CBkDialogImpl<KMainDlg>)
        END_MSG_MAP()

private:

    enum LIST_COL_NAME
    {
        COL_FILE_NAME = 0,
        COL_FILE_PATH,
        COL_FILE_SIZE,
        COL_FILE_MODIFIED_TIME
    };

    enum FILE_FILTER_TYPE
    {
        FILE_FILTER_NONE = 0,
        FILE_FILTER_DOCUMENT,
        FILE_FILTER_IMAGE,
        FILE_FILTER_VEDIO,
        FILE_FILTER_MUSIC,
        FILE_FILTER_DIRECORY,
        FILE_FILTER_FILE
    };

    enum FILE_CONDITION_SEARCH_SIZE_TYPE
    {
        FILE_CONDITION_SEARCH_SIZE_MICROSMALL = 0,
        FILE_CONDITION_SEARCH_SIZE_SMALL,
        FILE_CONDITION_SEARCH_SIZE_MID,
        FILE_CONDITION_SEARCH_SIZE_LARGE,
        FILE_CONDITION_SEARCH_SIZE_SUPERLARGE
    };

    enum FILE_CONDITION_SEARCH_TIME_TYPE
    {
        FILE_CONDITION_SEARCH_TIME_PRE = 0,
        FILE_CONDITION_SEARCH_TIME_POST,
        FILE_CONDITION_SEARCH_TIME_INTERVAL
    };

    BOOL _MapFillColTypeToHandle();
    void _SetColFileName(LV_ITEM* pItem, const CString& strFilePath);
    void _SetColFilePath(LV_ITEM* pItem, const CString& strFilePath);
    void _SetColFileSize(LV_ITEM* pItem, const CString& strFilePath);
    void _SetColFileModifyTime(LV_ITEM* pItem, const CString& strFilePath);
    void _SetCaseTip();
    void _SetKindBtnVisible(BOOL bVisible); 
    void _SetItemPosFromItemRect(UINT uItemID, RECT rcBtn); 
    void _SetPreViewBtnPos(RECT rcBtn);
    void _SetCOleDateTime(COleDateTime& FileConditionTimeLow
        , COleDateTime& FileConditionTimeHigh
        , SYSTEMTIME& SysTimePre
        , SYSTEMTIME& SysTimePost);   

    BOOL _MapTimerTypeToHandle();
    void _ShowMainDialogTimer();
    void _ClearTaskTimer();
    void _SearchWaittingTimer();
    void _AnimateTimer();
    void _WarningBeforeSort(CString& strFileSortInfo, BOOL& bSort); 
    void _SetTime(SYSTEMTIME& Systime);
    void _KindSortWarning(); 
    void _SizeSortWarning(); 
    void _TimeSortWarning();


    BOOL _FileConditionSizeSatisfied();
    BOOL _FileConditionTimeSatisfied();
    void _OnFilterBtnSelected(int nFilterType, const CString& strFilterInfo);
    void _AddSearchTask();
    void _AddNewSortTask(KTaskBase* &pTask); 
    void _NullSearchTextCommand();
    void _NotNullSearchTextCommand();
    int _GetSearchEditText();

    void _HidePreviewShowConditionSearch();
    void _HideConditionSearchShowPreview();

    CThreadPool<KWorker> m_ThreadPool;
    std::queue<KTaskBase*> m_queTaskQueue;
    KMyEdit* m_pSearchEditBar;
    CRichEditCtrl m_PreViewRichEdit;
    KListView m_ListView;
    DWORD m_dwCounter;
    DWORD m_dwTimeOut;
    DWORD m_dwSearchInputTime;
    int m_nSortedColumn;
    int m_nSortingColumn;
    int m_nFileConditionTimeType;
    KThreadController* m_pThreadController;
    KCriticalSesion m_Critical; 
    KSearchTask* m_pCurrentTask;
    KTaskBase* m_pPreSearchTask;
    KTaskBase* m_pPreFilterTask;
    KFileOperation m_FileOperation;
    CString m_strFileName;
    CImageList m_ImageHead;
    BOOL m_bSortByAscent;
    BOOL m_bIsSearching;
    BOOL m_bIsSorting;
    BOOL m_bIsConditionSearching;
    BOOL m_bCanSearch;
    BOOL m_bIsFiltering;
    BOOL m_bIsExtendDialog;
    BOOL m_bHasMessageBox;
    BOOL m_bSelectedFileConditionSize;
    BOOL m_bSelectedFileConditionTime;
    std::map<int, int> m_IconIndexMap;
    std::vector<HICON*> m_vHicon;
    BOOL m_bIsSmallDialgo;
    KKeyBoardHook m_KeyBoardHook;
    int m_nPreviewBtnSubIndex;
    int m_nFlashTimes; 
    int m_nAlpha;
    HINSTANCE m_hRichEditDll;
    NOTIFYICONDATA m_NotifyIconData;
    BOOL m_bPreviewBtnSelected;
    BOOL m_bLUCaseBtnSelected;
    BOOL m_bSelectedFileConditionSizeType[FILE_CONDITION_SIZE_SEARCH_TYPE_NUM];
    BOOL m_bDetailBtnSelected;
    BOOL m_bKindBtnSelected;  
    BOOL m_bHighBtnSelected; 
    CDateTimePickerCtrl m_DateTimePickerPre;
    CDateTimePickerCtrl m_DateTimePickerPost;
    CDateTimePickerCtrl m_DateTimePickerIntervalPre;
    CDateTimePickerCtrl m_DateTimePickerIntervalPost;
    typedef void (KMainDlg::* pFillCol)(LV_ITEM* pItem, const CString& struFileInfo);
    std::map<int, pFillCol> m_FillColTypeToHandle;
    typedef void (KMainDlg::* pFillTimer)();
    std::map<int, pFillTimer> m_FillTimerTyopeToHandle;
    KFileConditionSearch* m_pFileConditionSearch;
};

