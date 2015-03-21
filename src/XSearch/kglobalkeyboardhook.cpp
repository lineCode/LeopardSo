#include "stdafx.h"
#include <bkres/bkres.h>
#include "kglobalkeyboardhook.h"

HWND KKeyBoardHook::ms_hWnd = NULL;
HHOOK KKeyBoardHook::ms_hHook = NULL;
BOOL KKeyBoardHook::ms_bCtrlKeyDownLastTime = FALSE;

KKeyBoardHook::KKeyBoardHook()
{

}

KKeyBoardHook::~KKeyBoardHook()
{
    if(ms_hHook)
    {
        UnHook();
    }
} 

void KKeyBoardHook::InstallHook(HWND hWnd)
{
    if(NULL == hWnd || ms_hHook != NULL) 
    {
        goto Exit0;
    }
    ms_hWnd = hWnd;
    ms_hHook = (HHOOK)SetWindowsHookEx(WH_KEYBOARD_LL,  LowLevelKeyboardProc,
        NULL, 0);

Exit0:
    return;	
} 

BOOL KKeyBoardHook::UnHook()
{
    BOOL bRet  =FALSE; 

    if (UnhookWindowsHookEx(ms_hHook)) 
    {
        bRet = TRUE;
        ms_hHook = NULL;
    }   
    return bRet;
}

LRESULT KKeyBoardHook::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)  
{
    KBDLLHOOKSTRUCT* pKeyBoardStruct = NULL;
    LRESULT nRet = 0;

    if (nCode >= HC_ACTION)
    {   
        if(wParam == WM_KEYUP)
        {
            pKeyBoardStruct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
            if(NULL == pKeyBoardStruct)
            {
                goto Exit0;
            }
            if(VK_LCONTROL == pKeyBoardStruct->vkCode || VK_RCONTROL == pKeyBoardStruct->vkCode)
            {
                SendMessage(ms_hWnd, WM_HOTKEY, NULL, NULL);
            }
        }
    }
    nRet = CallNextHookEx(ms_hHook, nCode, wParam, lParam);

Exit0:
    return nRet;
}