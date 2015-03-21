#include "../stdafx.h"
#include <Windows.h>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <atlstr.h>
#include "kpathbuilder.h"
#include "kusinformation.h"

#define RESERVE_NUMBER      300000

KUSNInformation::KUSNInformation(const CString& strDriveName)
: m_dwRetLength(0)
, m_strDriveName(strDriveName)
, m_bIsNTFS(FALSE)
, m_bCaseSensitive(FALSE)
, m_hVol(INVALID_HANDLE_VALUE)
, m_pFrnName(NULL)
, m_pFrnNameMap(NULL)
, m_pFrnForPath(NULL)
{
    m_strDriveName += L":\\";
    m_pFrnNameMap = new FrnNameMap;
    m_pFrnName = new PFRN_NAME;
    m_pFrnForPath = new stdext::hash_map<DWORDLONG, std::vector<DWORDLONG>>;
}

KUSNInformation::KUSNInformation()
: m_hVol(INVALID_HANDLE_VALUE)
, m_dwRetLength(0)
, m_pFrnName(NULL)
, m_pFrnNameMap(NULL)
, m_bIsNTFS(FALSE)
, m_bCaseSensitive(FALSE)
, m_pFrnForPath(NULL)
{
    m_pFrnNameMap = new FrnNameMap;
    m_pFrnName = new PFRN_NAME;
    m_pFrnForPath = new stdext::hash_map<DWORDLONG, std::vector<DWORDLONG>>;
}

void KUSNInformation::SetDrive(const CString& strDriveName)
{
    m_strDriveName = strDriveName;
    m_strDriveName += L":\\";
}

void KUSNInformation::_CloseHandle()
{
    if (m_hVol)
    {
        ::CloseHandle(m_hVol);
    }
}

KUSNInformation::~KUSNInformation()
{
    _DeleteUSN();
    _CloseHandle();
    DeleteTempData();
}

BOOL KUSNInformation::IsNTFS()
{
    BOOL bRet = FALSE;
    TCHAR szNameBuf[MAX_PATH] = {0}; 
    BOOL bStatus = FALSE;

    bStatus = GetVolumeInformation(m_strDriveName.GetBuffer(), NULL, 
        0, NULL, NULL, NULL, szNameBuf, MAX_PATH);
    if (FALSE != bStatus)
    {   
        if (0 == _tcscmp(szNameBuf, L"NTFS"))
        {  
            m_bIsNTFS = TRUE;
            bRet = TRUE;
        }
    }   
    return bRet;
}

BOOL KUSNInformation::_GetDriveHandle()
{
    BOOL bRet = FALSE;
    CString strVolumePath;
    strVolumePath.Format(L"\\\\.\\%c:", m_strDriveName[0]);

    m_hVol = CreateFileW(strVolumePath, GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_READONLY, NULL); 
    if(INVALID_HANDLE_VALUE != m_hVol)
    {  
        bRet = TRUE; 
    }
    return bRet; 
}

BOOL KUSNInformation::_CreateUSN()
{
    BOOL bRet = FALSE;
    CREATE_USN_JOURNAL_DATA Cujd = {0};
    BOOL bStatus = FALSE;

    bStatus = DeviceIoControl(m_hVol, FSCTL_CREATE_USN_JOURNAL,
        &Cujd, sizeof(Cujd), NULL, 0, &m_dwRetLength, NULL);
    if(FALSE != bStatus)
    {  
        bRet = TRUE;
    }
    return bRet;
}

BOOL KUSNInformation::_GetInfo()
{ 
    BOOL bRet = FALSE;
    BOOL bStatus = FALSE;

    bStatus = DeviceIoControl(m_hVol, FSCTL_QUERY_USN_JOURNAL,
        NULL, 0, &m_UsnInfo, sizeof(m_UsnInfo), &m_dwRetLength, NULL);
    if(FALSE != bStatus)
    {  
        bRet = TRUE;
    }
    return bRet;
}

void KUSNInformation::_InitMed(MFT_ENUM_DATA* pMed)
{
    if(NULL == pMed)
    {
        goto Exit0;
    }
    pMed->StartFileReferenceNumber = 0;
    pMed->LowUsn = 0;
    pMed->HighUsn = m_UsnInfo.NextUsn;

Exit0:
    return;
}

void KUSNInformation::_GetData()
{
    MFT_ENUM_DATA Med = {0};
    TCHAR szBuffer[BUF_LEN] = {0};
    DWORD dwUsnDataSize = 0;
    DWORD dwRetBytes = 0;
    PUSN_RECORD UsnRecord = {0};  

    _InitMed(&Med);

    while (FALSE != DeviceIoControl(m_hVol, FSCTL_ENUM_USN_DATA,  
        &Med, sizeof (Med), szBuffer, BUF_LEN, &dwUsnDataSize, NULL))
    {  
        dwRetBytes = dwUsnDataSize - sizeof (USN);  
        UsnRecord = (PUSN_RECORD)(((PCHAR)szBuffer) + sizeof (USN));  

        while (dwRetBytes > 0)
        {
            CStringW strFileName(UsnRecord->FileName, UsnRecord->FileNameLength / 2);

            _GetFrnNameMap(strFileName, UsnRecord);
            _GetFrnForPath(UsnRecord);
            _GetNextUSN(&UsnRecord, &dwRetBytes);
        }  
        Med.StartFileReferenceNumber = *(reinterpret_cast<USN*>(&szBuffer));
    }   
    _BuildFrnPath();
}

void KUSNInformation::_BuildFrnPath()
{
    KPathBuilder builder(*m_pFrnForPath, *m_pFrnNameMap, m_vFrnToFileInfo);
    builder.Build();
}

void KUSNInformation::_GetFrnForPath(const PUSN_RECORD& UsnRecord)
{
    (*m_pFrnForPath)[UsnRecord->ParentFileReferenceNumber].push_back(UsnRecord->FileReferenceNumber);
}

void KUSNInformation::_GetNextUSN(PUSN_RECORD* pUsnRecord, DWORD* pdwRetBytes)
{
    DWORD dwRecordLen = 0;
    if(NULL == pUsnRecord || NULL == pdwRetBytes)
    {
        goto Exit0;
    }
    dwRecordLen = (*pUsnRecord)->RecordLength;
    *pdwRetBytes = *pdwRetBytes - dwRecordLen;
    *pUsnRecord = (PUSN_RECORD)(((PCHAR)*pUsnRecord) + dwRecordLen);

Exit0:
    return;
}

void KUSNInformation::_GetFrnNameMap(const CString& strFileName, const PUSN_RECORD& UsnRecord)
{
    m_pFrnName->m_strFilename = strFileName;
    m_pFrnName->m_nPfrn = UsnRecord->ParentFileReferenceNumber;
    m_pFrnNameMap->insert(std::make_pair(UsnRecord->FileReferenceNumber, *m_pFrnName));
}

BOOL KUSNInformation::_DeleteUSN()
{
    BOOL bRet = FALSE;
    BOOL bStatus = FALSE;

    {	
        KLocker Lock(m_Critical);
        DELETE_USN_JOURNAL_DATA Dujd = {0};

        Dujd.UsnJournalID = m_UsnInfo.UsnJournalID;  
        Dujd.DeleteFlags = USN_DELETE_FLAG_DELETE;  

        bStatus = DeviceIoControl(m_hVol, FSCTL_DELETE_USN_JOURNAL,
            &Dujd, sizeof (Dujd), NULL, 0, &m_dwRetLength, NULL);
        if (FALSE != bStatus)
        {
            bRet = TRUE;
        }
    }
    return bRet;
}

void KUSNInformation::FindWidePathVectorByName(const CString& strName, FrnVector* pvFrn)
{
    if(NULL == pvFrn || strName.IsEmpty())
    {
        goto Exit0;
    }

    _AddMatchFile(strName, pvFrn);

Exit0:
    return;
}

void KUSNInformation::FindWidePathVectorByNameWithWidecard(const CString& strName, FrnVector* pvFrn)
{
    DRIVE_AND_FRN DriveAndFrn = {0};
    int nIndex = -1;

    if(NULL == pvFrn)
    {
        goto Exit0;
    }

    nIndex = KPatternMatcher::BMH2(strName.GetString(), strName.GetLength(), L"\\", 1, 0);
    if (-1 != nIndex)
    {
        for (stdext::hash_map<DWORDLONG, FileAndPath>::iterator it = m_vFrnToFileInfo.begin();
            it != m_vFrnToFileInfo.end(); ++it)
        {
            _MatchPathWithWidecard(it, strName, DriveAndFrn, pvFrn);
        }
    }
    else
    {
        for (stdext::hash_map<DWORDLONG, FileAndPath>::iterator it = m_vFrnToFileInfo.begin();
            it != m_vFrnToFileInfo.end(); ++it)
        {
            _MatchNameWithWidecard(it, strName, DriveAndFrn, pvFrn);
        }
    }

Exit0:
    return;
}

void KUSNInformation::_MatchNameWithWidecard(FrnFileIter& it, const CString& strName, DRIVE_AND_FRN& DriveAndFrn, FrnVector* pvFrn)
{
    if(NULL == pvFrn)
    {
        goto Exit0;
    }
    if (MatchStringWithWidecard(it->second.m_strFileName, strName))
    {
        _FillDriveAndFrnStruct(DriveAndFrn, it, pvFrn);
    }

Exit0:
    return;
}

void KUSNInformation::_MatchPathWithWidecard(FrnFileIter& it, const CString& strName, DRIVE_AND_FRN& DriveAndFrn, FrnVector* pvFrn)
{
    if(NULL == pvFrn)
    {
        goto Exit0;
    }
    if (MatchStringWithWidecard(it->second.m_strPathName, strName))
    {
        _FillDriveAndFrnStruct(DriveAndFrn, it, pvFrn);
    }

Exit0:
    return;
}

void KUSNInformation::_MatchName(FrnFileIter& it, const CString& strName, DRIVE_AND_FRN& DriveAndFrn, FrnVector* pvFrn)
{
    if(NULL == pvFrn)
    {
        goto Exit0;
    }
    if (MatchString(it->second.m_strFileName, strName))
    {
        _FillDriveAndFrnStruct(DriveAndFrn, it, pvFrn);
    }

Exit0:
    return;
}

void KUSNInformation::_AddMatchFile(const CString& strName, FrnVector* pvFrn)
{
    DRIVE_AND_FRN DriveAndFrn = {0};
    int nIndex = -1;

    if(NULL == pvFrn)
    {
        goto Exit0;
    }

    nIndex = KPatternMatcher::BMH2(strName.GetString(), strName.GetLength(), L"\\", 1, 0);
    if (-1 != nIndex)
    {
        for (stdext::hash_map<DWORDLONG, FileAndPath>::iterator it = m_vFrnToFileInfo.begin();
            it != m_vFrnToFileInfo.end(); ++it)
        {
            _MatchPath(it, strName, DriveAndFrn, pvFrn);
        }
    }
    else
    {
        for (stdext::hash_map<DWORDLONG, FileAndPath>::iterator it = m_vFrnToFileInfo.begin();
            it != m_vFrnToFileInfo.end(); ++it)
        {
            _MatchName(it, strName, DriveAndFrn, pvFrn);
        }
    }

Exit0:
    return;
}

BOOL KUSNInformation::_IsDriveEqual(const CString& strName)
{
    BOOL bRet= FALSE;
    CString strPatternDrive = strName[0];
    CString strMyDrive = m_strDriveName[0];

    if (!m_bCaseSensitive)
    {
        strPatternDrive.MakeLower();
        strMyDrive.MakeLower();
    }

    if (strPatternDrive == strMyDrive)
    {
        bRet = TRUE;
    }
    return bRet;
}

void KUSNInformation::_MatchPathWithDrive(FrnFileIter& it, const CString& strName, DRIVE_AND_FRN& DriveAndFrn, FrnVector* pvFrn)
{
    CString strCRow;
    CString strFullName;
    CString strPattern;

    if(NULL == pvFrn)
    {
        goto Exit0;
    }

    strCRow = strName.Right(strName.GetLength() - 3);
    strFullName = it->second.m_strPathName + it->second.m_strFileName;
    strPattern = strFullName.Right(strFullName.GetLength() - 1);
    strPattern = strPattern.Left(strCRow.GetLength());

    if (!_IsDriveEqual(strName))
    {
        goto Exit0;
    }

    if (!m_bCaseSensitive)
    {
        strCRow.MakeLower();
        strPattern.MakeLower();
    }

    if (strCRow == strPattern)
    {
        _FillDriveAndFrnStruct(DriveAndFrn, it, pvFrn);
    }

Exit0:
    return;
}

void KUSNInformation::_FillDriveAndFrnStruct(DRIVE_AND_FRN& DriveAndFrn, FrnFileIter& it, FrnVector* pvFrn)
{
    if(NULL == pvFrn)
    {
        goto Exit0;
    }

    DriveAndFrn.m_cDriveName = m_strDriveName[0];
    DriveAndFrn.m_dwlFrn = it->first;
    DriveAndFrn.m_pstrFileName = &(it->second.m_strFileName);
    pvFrn->push_back(DriveAndFrn);

Exit0:
    return;
}

void KUSNInformation::_MatchPathWithoutDrive(FrnFileIter& it, const CString& strName, DRIVE_AND_FRN& DriveAndFrn, FrnVector* pvFrn)
{
    if(NULL == pvFrn)
    {
        goto Exit0;
    }

    if (MatchString(it->second.m_strPathName, strName))
    {
        _FillDriveAndFrnStruct(DriveAndFrn, it, pvFrn);
    }

Exit0:
    return;
}

void KUSNInformation::_MatchPath(FrnFileIter& it, const CString& strName, DRIVE_AND_FRN& DriveAndFrn, FrnVector* pvFrn)
{
    if(NULL == pvFrn)
    {
        goto Exit0;
    }

    if (-1 != strName.Find(':'))
    {
        _MatchPathWithDrive(it, strName, DriveAndFrn, pvFrn);
    }
    else
    {
        _MatchPathWithoutDrive(it, strName, DriveAndFrn, pvFrn);
    }

Exit0:
    return;
}

BOOL KUSNInformation::_ISWildcard(const CString& strName)
{
    BOOL bRet = ((strName.Find('*') != -1) || (strName.Find('?') != -1));
    return bRet;
}

void KUSNInformation::_SetCaseSensitive(BOOL bCasesensitive)
{
    m_bCaseSensitive = bCasesensitive;
}

BOOL KUSNInformation::FindFrnByName(const CString& strName,FrnVector* pvFrn, BOOL bCasesensitive)
{
    if(NULL == pvFrn)
    {
        goto Exit0;
    }

    _SetCaseSensitive(bCasesensitive);

    if (!_ISWildcard(strName))
    {
        FindWidePathVectorByName(strName, pvFrn);
    }
    else
    {
        FindWidePathVectorByNameWithWidecard(strName, pvFrn);
    }

Exit0:
    return TRUE;
}

HANDLE KUSNInformation::GetVolHandle() 
{
    return m_hVol;
}

BOOL KUSNInformation::IsNTFSAfterCalc()
{
    return m_bIsNTFS;
}

void KUSNInformation::DeleteTempData()
{
    KLocker Locker(m_Critical);

    if (m_pFrnForPath)
    {
        delete m_pFrnForPath;
        m_pFrnForPath = NULL;
    }

    if (m_pFrnNameMap)
    {
        delete m_pFrnNameMap;
        m_pFrnNameMap = NULL;
    }

    if (m_pFrnName)
    {
        delete m_pFrnName;
        m_pFrnName = NULL;
    }
}

void KUSNInformation::Init()
{
    if (!_GetDriveHandle())
    {
        goto Exit0;
    }

    if(!_CreateUSN())
    {
        goto Exit0;
    }

    if (!_GetInfo())
    {
        goto Exit0;
    }

    _GetData();

Exit0:
    return;
}

BOOL KUSNInformation::MatchStringWithWidecard(CString& strRowString, const CString& strPattern)
{
    CString strCRow(strRowString);
    CString strCPat(strPattern);
    BOOL bSuccess = FALSE;

    if (!m_bCaseSensitive)
    {
        strCRow.MakeLower();
        strCPat.MakeLower();
    }

    bSuccess = KPatternMatcher::PatternSearch(strCRow.GetString(), strCPat.GetString());
    return bSuccess;
}

BOOL KUSNInformation::MatchString(CString& strRowString, const CString& strPattern)
{
    BOOL bRet = FALSE;
    int nIndex = -1;
    CString strCRow(strRowString);
    CString strCPat(strPattern);

    if (!m_bCaseSensitive)
    {
        strCRow.MakeLower();
        strCPat.MakeLower();
    }

    nIndex = KPatternMatcher::BMH2(strCRow.GetString(), strCRow.GetLength(), 
        strCPat.GetString(), strCPat.GetLength(), 0);
    if (-1 != nIndex)
    {
        bRet = TRUE;
        goto Exit0;
    }

Exit0:
    return bRet;
}

size_t KUSNInformation::GetFileCount()
{
    return m_vFrnToFileInfo.size();
}

void KUSNInformation::BuildPath(DWORDLONG dwlFrn, const CString** ppstrPath)
{
    FrnFileIter it;

    if(NULL == ppstrPath)
    {
        goto Exit0;
    }

    it = m_vFrnToFileInfo.find(dwlFrn);
    if (it != m_vFrnToFileInfo.end())
    {
        *ppstrPath = &(it->second.m_strPathName);
        goto Exit0;
    }
    *ppstrPath = NULL;

Exit0:
    return;
}