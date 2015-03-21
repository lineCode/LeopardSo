#pragma once

#include "../stdafx.h"
#include <stdio.h>
#include <vector>
#include <map>
#include <atlstr.h>
#include <ATLComTime.h>
#include <Windows.h>
#include "kcriticalsesion.h"
#include "klocker.h"
#include "kthreadcontroller.h"
#include "kusinformation.h"

class KGetFileInfoThread;

typedef struct tagSEG_FILE_INFO
{
	KGetFileInfoThread* m_pGetFileInfo;
	int m_nSegCount;
}SEG_FILE_INFO, *PSEG_FILE_INFO;

class KGetFileInfoThread
{
public:
	KGetFileInfoThread(KThreadController* pThreadController, 
		std::vector<DRIVE_AND_FRN>* pDriveAndFrn);
	~KGetFileInfoThread();

	void GetFileSize(std::map<int, UINT64>* pFileSizeMap);
	void GetFileTime(std::map<int, COleDateTime>* pFileTimeMap);

private:
	static DWORD WINAPI _GetFileSizeFunc(LPVOID);
	static DWORD WINAPI _GetFileTimeFunc(LPVOID);

	void _GetFileSizeMain(int nSegCount);
	void _GetFileTimeMain(int nSegCount);
	void _CreateFileSizeThread();
	void _CreateFileTimeThread();
	void _CloseSizeThread();
	void _CloseTimeThread();

	UINT64 _GetFileSize(const CString& strFilePath);
	void _GetFileModifyTime(const CString& strFilePath, COleDateTime& ColeDateTime);

	static const int ms_nThreadNums = 10;
	KThreadController* m_pThreadController;
	std::vector<DRIVE_AND_FRN>* m_pDriveAndFrn;
	HANDLE* m_phSizeThreads;
	HANDLE* m_phTimeThreads;
	int m_nSegCount;
	std::map<int, UINT64>* m_pFileSizeMap;
	std::map<int, COleDateTime>* m_pFileTimeMap;
	KCriticalSesion m_Critical;
	SEG_FILE_INFO m_fileInfo[ms_nThreadNums];
};