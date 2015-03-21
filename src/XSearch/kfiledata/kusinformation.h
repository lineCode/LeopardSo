#pragma once

#include "../stdafx.h"
#include <map>
#include <hash_map>
#include <vector>
#include <string>
#include <atlstr.h>
#include <Windows.h>
#include "kcriticalsesion.h"
#include "kpatternmatcher.h"
#include "klocker.h"

#define BUF_LEN 4096

typedef struct tagPFRN_NAME
{
	DWORDLONG m_nPfrn;
	CString m_strFilename;
}PFRN_NAME, *PPFRN_NAME;

typedef struct tagFileAndPath
{
	BOOL m_bIsDir;
	CString m_strFileName;
	CString m_strPathName;
}FileAndPath, *PFileAndPath;

typedef struct tagDRIVE_AND_FRN
{
	int m_nIndex;
	const CString* m_pstrFileName;
	wchar_t m_cDriveName;
	DWORDLONG m_dwlFrn;
}DRIVE_AND_FRN, *PDRIVE_AND_FRN;

typedef stdext::hash_map<DWORDLONG, PFRN_NAME> FrnNameMap;
typedef stdext::hash_map<DWORDLONG, FileAndPath>::iterator FrnFileIter;
typedef std::vector<DRIVE_AND_FRN> FrnVector;

class KUSNInformation
{
public:
	KUSNInformation();
	KUSNInformation(const CString& strDriveName);
	~KUSNInformation();

	void SetDrive(const CString& strDriveName);
	BOOL IsNTFS();
	BOOL IsNTFSAfterCalc();   //第二次判断是否是NTFS结构，不需要重新计算
	void Init();
	HANDLE GetVolHandle();
	BOOL FindFrnByName(const CString& strName, FrnVector* pvFrn, BOOL bCasesensitive = FALSE);
	void FindWidePathVectorByName(const CString& strName, FrnVector* pvFrn);
	void FindWidePathVectorByNameWithWidecard(const CString& strName, FrnVector* pvFrn);
	BOOL MatchString(CString& strSource, const CString& strPattern);
	void DeleteTempData();
	size_t GetFileCount();
	void BuildPath(DWORDLONG dwlFrn, const CString** ptrPath);

private:
	BOOL _GetDriveHandle();
	BOOL _CreateUSN();
	BOOL _GetInfo();
	void _GetData();
    void _SetCaseSensitive(BOOL bCasesensitive);
	BOOL _DeleteUSN();
	void _GetFrnNameMap(const CString& fileName, const PUSN_RECORD& UsnRecord);
	void _GetFrnForPath(const PUSN_RECORD& UsnRecord);
	void _InitMed(MFT_ENUM_DATA* pMed);
	void _GetNextUSN(PUSN_RECORD* pUsnRecord, DWORD* pdwRetBytes);
	void _CloseHandle();
	void _AddMatchFile(const CString& strName, FrnVector* pvFrn);
	void _BuildFrnPath();
	void _FillDriveAndFrnStruct(DRIVE_AND_FRN& DriveAndFrn, FrnFileIter& it, FrnVector* pvFrn);

	void _MatchName(FrnFileIter& it, const CString& strName, DRIVE_AND_FRN& DriveAndFrn, FrnVector* pvFrn);
	void _MatchPath(FrnFileIter& it, const CString& strName, DRIVE_AND_FRN& DriveAndFrn, FrnVector* pvFrn);
	void _MatchPathWithDrive(FrnFileIter& it, const CString& strName, DRIVE_AND_FRN& DriveAndFrn, FrnVector* pvFrn);
	void _MatchPathWithoutDrive(FrnFileIter& it, const CString& strName, DRIVE_AND_FRN& DriveAndFrn, FrnVector* pvFrn);
	void _MatchNameWithWidecard(FrnFileIter& it, const CString& strName, DRIVE_AND_FRN& DriveAndFrn, FrnVector* pvFrn);
	void _MatchPathWithWidecard(FrnFileIter& it, const CString& strName, DRIVE_AND_FRN& DriveAndFrn, FrnVector* pvFrn);
	BOOL MatchStringWithWidecard(CString& strRowString, const CString& strPattern);
	BOOL _IsDriveEqual(const CString& strName);
	BOOL _ISWildcard(const CString& strName);

private:
	BOOL m_bIsNTFS;
	CString m_strDriveName;
	HANDLE m_hVol;

	USN_JOURNAL_DATA m_UsnInfo;
	DWORD m_dwRetLength;

	FrnNameMap* m_pFrnNameMap;
	PFRN_NAME* m_pFrnName;
	stdext::hash_map<DWORDLONG, std::vector<DWORDLONG>>* m_pFrnForPath;
	stdext::hash_map<DWORDLONG, FileAndPath> m_vFrnToFileInfo;

	KCriticalSesion m_Critical;
	BOOL m_bCaseSensitive;
};
