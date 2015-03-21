#pragma once

#include "../stdafx.h"
#include <vector>
#include <string>
#include "kcriticalsesion.h"
#include "klocker.h"
#include "kusinformation.h"

#define MAX_DRIVE_COUNT 26
#define UPDATE_FILE_CREATE_INDEX WM_USER + 103
#define BEGIN_DRIVE_NAME 'A'

class KThreadController
{
public:
	KThreadController(HWND hWnd);
	~KThreadController();

	BOOL FindFrnByName(CString& strName, FrnVector* pvFrn, BOOL bCasesensitive = FALSE);
	void BuildPath(const DRIVE_AND_FRN& DriveAndFrn, const CString** ppstrPath);

private:
	static DWORD WINAPI _NotifyThreadFunc(LPVOID);
	static DWORD WINAPI _WorkThreadFunc(LPVOID);
	static DWORD WINAPI _CleannerThreadFunc(LPVOID);

	void _NotifyThreadMain();
	void _WorkThreadMain(int nDriveNum);
	void _CleannerThreadMain(int nDriveNum);
	
    void _InitHandle();
	void _CreateThread();
	void _InitDriveName();
	void _InitUSNList();
	void _CreateNotifyThread();
	void _CreateSearchThread();
	void _CreateCleannerThread();
	void _CreateEvent();
	void _CloseHandle();
	void _WaitForEventsToContinue();

	KCriticalSesion m_Critical;
	HANDLE m_hNotifyThread;
	HANDLE m_phThread;
	HANDLE* m_phCleannerThreads;
	HANDLE* m_phEventsForSearch;
	HANDLE* m_phEvents;
	HANDLE* m_phEventsToCleanner;

    HWND m_hWnd;
	KUSNInformation m_USNs[MAX_DRIVE_COUNT];
	std::vector<std::wstring> m_vDriveName;
	int nDriveNum;
	int nDriveNumForMonitor;
	int nDriveNumForCleanner;
};
