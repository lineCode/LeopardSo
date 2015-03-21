#pragma once

class KKeyBoardHook
{
public:
    KKeyBoardHook();
    ~KKeyBoardHook();

    void InstallHook(HWND hWnd);
    static LRESULT __stdcall LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
    BOOL UnHook();

private:
    static HHOOK ms_hHook; 
    static HWND ms_hWnd;    
    static BOOL ms_bCtrlKeyDownLastTime;
};
