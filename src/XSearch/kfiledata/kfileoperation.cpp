#include "../stdafx.h"
#include "kfileoperation.h"

KFileOperation::KFileOperation()
{
    _InitPreProcessSuffixName();
}

KFileOperation::~KFileOperation()
{

}

SHORT KFileOperation::_GetTextCodeType(const CString& strFilePath)
{
    FILE* pFile = NULL;
    SHORT nTextCodeType = 0;
    USES_CONVERSION;
    pFile = fopen(reinterpret_cast<const char*>(T2A(strFilePath)), "rb");
    if(NULL == pFile)
    {
        goto Exit0;
    }
    fread(&nTextCodeType, 1, sizeof(SHORT), pFile);

Exit0:
    if(NULL != pFile)
    {
        fclose(pFile);
        pFile = NULL;
    }
    return nTextCodeType;
}

void KFileOperation::_ShowUnicodeText(const CString& strFilePath, CString& strFileText)
{
    FILE* pFile = NULL;
    long lLen = 10240;
    SHORT nTextHead = 0;
    TCHAR* pszTextW = NULL;

    USES_CONVERSION;
    fopen_s(&pFile, reinterpret_cast<const char*>(T2A(strFilePath)), "rb, ccs=UNICODE");
    if(NULL == pFile)
    {
        goto Exit0;
    }

    fread(&nTextHead, sizeof(SHORT), 1, pFile);

    pszTextW = new TCHAR[lLen + 1];
    if(NULL == pszTextW)
    {
        goto Exit0;
    }
    fread(pszTextW, sizeof(TCHAR), lLen, pFile);
    pszTextW[lLen] = '\0';
    strFileText.Format(L"%s", pszTextW);

Exit0:
    if(NULL != pFile)
    {
        fclose(pFile);
        pFile = NULL;
    }
    if(NULL != pszTextW)
    {
        delete []pszTextW;
        pszTextW = NULL;
    }
}

void KFileOperation::_ShowAsciiText(const CString& strFilePath, CString& strFileText)
{
    FILE* pFile = NULL;
    long lLen = 10240;
    CHAR* pszText = NULL;

    USES_CONVERSION;
    pFile = fopen(reinterpret_cast<const char*>(T2A(strFilePath)), "rb");
    if(NULL == pFile)
    {
        goto Exit0;
    }
    pszText = new CHAR[lLen + 1];
    if(NULL == pszText)
    {
        goto Exit0;
    }
    fread(pszText, sizeof(CHAR), lLen, pFile);
    pszText[lLen] = '\0';
    strFileText.Format(L"%s", A2W(pszText));

Exit0:
    if(NULL != pFile)
    {
        fclose(pFile);
        pFile = NULL;
    }
    if(NULL != pszText)
    {
        delete []pszText;
        pszText = NULL;
    }

}

void KFileOperation::_ShowUtf8Text(const CString& strFilePath, CString& strFileText)
{
    FILE* pFile = NULL;
    long lLen = 10240;
    SHORT nTextHead = 0;
    TCHAR* pszTextUtf8 = NULL;
    TCHAR* pszTextUnicode = NULL;

    USES_CONVERSION;
    fopen_s(&pFile, reinterpret_cast<const char*>(T2A(strFilePath)), "rb, ccs=UTF-8");
    if(NULL == pFile)
    {
        goto Exit0;
    }

    fread(&nTextHead, sizeof(SHORT), 1, pFile);

    pszTextUtf8 = new TCHAR[lLen + 1];
    if(NULL == pszTextUtf8)
    {
        goto Exit0;
    }
    fread(pszTextUtf8, sizeof(TCHAR), lLen, pFile);
    pszTextUtf8[lLen] = '\0';

    lLen = ::MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pszTextUtf8, -1, NULL, 0);
    if(0 == lLen)
    {
        goto Exit0;
    }
    pszTextUnicode = new TCHAR[lLen + 1];
    if(NULL == pszTextUnicode)
    {
        goto Exit0;
    }

    ::MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pszTextUtf8, -1, pszTextUnicode, lLen);
    strFileText.Format(L"%s", pszTextUnicode);

Exit0:
    if(NULL != pFile)
    {
        fclose(pFile);
        pFile = NULL;
    }
    if(NULL != pszTextUnicode)
    {
        delete []pszTextUnicode;
        pszTextUnicode = NULL;
    }
    if(NULL != pszTextUtf8)
    {
        delete []pszTextUtf8;
        pszTextUtf8 = NULL;
    }
}



void KFileOperation::ShowFileBriefInfo(const CString& strFilePath, CString& strFileText)
{
    SHORT nTextCodeType = _GetTextCodeType(strFilePath);
    if(m_nUnicodeTextHead == nTextCodeType)
    {
        _ShowUnicodeText(strFilePath, strFileText);
    }
    else if(m_nUft8TextHead == nTextCodeType)
    {
        _ShowUtf8Text(strFilePath, strFileText);
    }
    else
    {
        _ShowAsciiText(strFilePath, strFileText);
    }
}

void KFileOperation::_InitPreProcessSuffixName()
{
    for(int i = 0; i < DOCUMENT_TYPE_NUM; ++i)
    {
        gs_PreProcessSuffixName.insert(std::make_pair(g_strTextSuffix[i], TRUE));
    }
}

UINT64 KFileOperation::GetFileSize(const CString& strFilePath)
{
    UINT64 uFileSize = 0;
    HANDLE hFile = INVALID_HANDLE_VALUE;

    if(::GetFileAttributes(strFilePath) & FILE_ATTRIBUTE_DIRECTORY)
    {
        goto Exit0;
    }

    hFile = ::CreateFile(strFilePath, GENERIC_READ, FILE_SHARE_READ,
        NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
    if(INVALID_HANDLE_VALUE == hFile)
    {
        goto Exit0;
    }
    ::GetFileSizeEx(hFile, reinterpret_cast<PLARGE_INTEGER>(&uFileSize));

Exit0:
    if(INVALID_HANDLE_VALUE != hFile)
    {
        ::CloseHandle(hFile);
    }
    return uFileSize;
}

void KFileOperation::GetFileSizeStr(UINT64 ulFileSize, CString& strFileSize)
{
    if(ulFileSize < DEFAULT_FILE_MINIMIZE_SIZE)
    {
        if(ulFileSize > 0)
        {
            strFileSize = L"1KB";
        }
    }
    else
    {
        ulFileSize /= DEFAULT_FILE_MINIMIZE_SIZE;
        strFileSize.Format(L"%lu", ulFileSize);
        _FormatFileSizeString(strFileSize);
        strFileSize += "KB";
    }
}

void KFileOperation::_FormatFileSizeString(CString& strFileSize)
{
    int nLen = strFileSize.GetLength();
    for(int i = nLen - 1, nDigitNum = 0; i >= 0; --i)
    {
        nDigitNum++;
        if (nDigitNum % 3 == 0 && nDigitNum != nLen)
        {
            strFileSize.Insert(i, ',');
        }
    }
}

void KFileOperation::FormatFileSpaceSize(UINT64 ulFileSpaceSize, CString& strFileSpaceSize)
{
    CString strFormatFileSize;
    float fSize = 0;

    if(0 == ulFileSpaceSize)
    {
        goto Exit0;
    }
    strFormatFileSize.Format(L"%lu", ulFileSpaceSize);
    if(ulFileSpaceSize / 1024 / 1024 / 1024 > 0)
    {
        fSize = ulFileSpaceSize / 1024.0 / 1024 / 1024;
        strFileSpaceSize.Format(L"%.2f GB", fSize);
    }
    else
    {
        if(ulFileSpaceSize / 1024 / 1024 > 0)
        {
            fSize = ulFileSpaceSize / 1024.0 / 1024;
            strFileSpaceSize.Format(L"%.2f MB", fSize);
        }
        else
        {
            if(ulFileSpaceSize / 1024 > 0)
            {
                fSize = (double)ulFileSpaceSize / 1024.0;
                strFileSpaceSize.Format(L"%.2f KB", fSize);
            }
            else
            {
                strFileSpaceSize.Format(L"%lu B", fSize);
            }
        }
    }

Exit0:
    return;
}

void KFileOperation::GetFileType(const CString& strFilePath, CString& strFileType)
{
    SHFILEINFO Shinfo = {0};
    SHGetFileInfo(strFilePath, FILE_ATTRIBUTE_NORMAL, &Shinfo, 
        sizeof(SHFILEINFO), SHGFI_TYPENAME);
    strFileType.Format(L"%s", Shinfo.szTypeName);
}

void KFileOperation::GetFileIcon(const CString& strFilePath, HICON& hIcon)
{
    SHFILEINFO Shinfo = {0};
    SHGetFileInfo(strFilePath, FILE_ATTRIBUTE_NORMAL, &Shinfo, 
        sizeof(SHFILEINFO), SHGFI_ICON);
    hIcon = Shinfo.hIcon;
}

void KFileOperation::CopyFilePathToClipPath(const CString* pstrFilePath)
{
    CHAR* pszFilePath = NULL;
    int nLen = 0;
    CHAR* pMemStr = NULL;
    HGLOBAL hFilePath = NULL;

    if(NULL == pstrFilePath || !OpenClipboard(NULL) || !EmptyClipboard())
    {
        goto Exit0;
    } 

    USES_CONVERSION;
    pszFilePath = reinterpret_cast<char*>(W2A(*pstrFilePath));
    if(NULL == pszFilePath)
    {
        goto Exit0;
    }
    nLen = static_cast<int>(strlen(pszFilePath));

    hFilePath = ::GlobalAlloc(GHND, nLen + 1);
    if(NULL == hFilePath)
    {
        goto Exit0;
    }
    pMemStr = reinterpret_cast<char*>(::GlobalLock(hFilePath));
    if(NULL == pMemStr)
    {
        goto Exit0;
    }

    memcpy_s(pMemStr, nLen, pszFilePath, nLen);
    if((::SetClipboardData(CF_TEXT, hFilePath)) == NULL)
    {
        goto Exit0;
    }
    ::CloseClipboard();

Exit0:
    
    if(NULL != hFilePath)
    {
        ::GlobalUnlock(hFilePath);
    }
    if(NULL != pMemStr)
    {
        ::GlobalFree(pMemStr);
    }
    return ;
}

void KFileOperation::OpenFileDirectory(CString& strFilePath)
{
    CString strFile;
    if(!strFilePath.IsEmpty())
    {
        Wow64EnableWow64FsRedirection(FALSE);
        strFile = "/select,";
        strFilePath = strFile + strFilePath;
        ShellExecute(NULL, L"open", L"explorer.exe", strFilePath, NULL, SW_SHOWNORMAL);
        Wow64EnableWow64FsRedirection(TRUE);
    }
}

void KFileOperation::OpenFile(const CString& strFilePath)
{
    if(!strFilePath.IsEmpty())
    {
        Wow64EnableWow64FsRedirection(FALSE);
        ShellExecute(NULL, L"open", L"explorer.exe", strFilePath, NULL, SW_SHOWNORMAL);
        Wow64EnableWow64FsRedirection(TRUE);
    }
}

void KFileOperation::GetFileModifyTime(const CString& strFilePath, CString& strFileModifyTime)
{
    HANDLE hFile = INVALID_HANDLE_VALUE;
    FILETIME ftModify = {0};
    SYSTEMTIME stLocal = {0};
    FILETIME ftLocalTime = {0};

    hFile = ::CreateFile(strFilePath, GENERIC_READ, FILE_SHARE_READ,
        NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
    if(INVALID_HANDLE_VALUE == hFile)
    {
        goto Exit0;
    }

    ::GetFileTime(hFile, NULL, NULL, &ftModify);
    FileTimeToLocalFileTime(&ftModify, &ftLocalTime);
    FileTimeToSystemTime(&ftLocalTime, &stLocal);
    strFileModifyTime.Format(L"%04d/%02d/%02d %02d:%02d:%02d", stLocal.wYear, stLocal.wMonth,
        stLocal.wDay, stLocal.wHour, stLocal.wMinute, stLocal.wSecond);

Exit0:
    if(INVALID_HANDLE_VALUE != hFile)
    {
        ::CloseHandle(hFile);
    }
}

void KFileOperation::GetFileAccessAndCreateTime(const CString& strFilePath, CString& strFileCreateTime, CString& strFileAccessTime)
{
    HANDLE hFile = INVALID_HANDLE_VALUE;
    FILETIME ftAccess = {0};
    FILETIME ftCreate = {0};
    SYSTEMTIME stLocal = {0};
    FILETIME ftLocalTime = {0};

    hFile = ::CreateFile(strFilePath, GENERIC_READ, FILE_SHARE_READ,
        NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
    if(INVALID_HANDLE_VALUE == hFile)
    {
        goto Exit0;
    }

    ::GetFileTime(hFile, &ftCreate, &ftAccess, NULL);
    FileTimeToLocalFileTime(&ftCreate, &ftLocalTime);
    FileTimeToSystemTime(&ftLocalTime, &stLocal);
    strFileCreateTime.Format(L"%04d/%02d/%02d %02d:%02d:%02d", stLocal.wYear, stLocal.wMonth,
        stLocal.wDay, stLocal.wHour, stLocal.wMinute, stLocal.wSecond);

    FileTimeToLocalFileTime(&ftAccess, &ftLocalTime);
    FileTimeToSystemTime(&ftLocalTime, &stLocal);
    strFileAccessTime.Format(L"%04d/%02d/%02d %02d:%02d:%02d", stLocal.wYear, stLocal.wMonth,
        stLocal.wDay, stLocal.wHour, stLocal.wMinute, stLocal.wSecond);

Exit0:
    if(INVALID_HANDLE_VALUE != hFile)
    {
        ::CloseHandle(hFile);
    }
}

UINT64 KFileOperation::GetFileSpaceSize(const CString& strFilePath)
{
    UINT64 ulFileSize = 0;
    UINT64 uFileSpaceSize = 0;
    CString szVolumePathName;
    DWORD dwSectorsPerCluster = 0;
    DWORD dwBytesPerSector = 0;
    DWORD dwNumberOfFreeClusters = 0;
    DWORD dwTotalNumberOfClusters = 0;
    DWORD dwClusterSize = 0;
    int nPos = -1;

    nPos = strFilePath.Find('\\');
    if(-1 == nPos)
    {
        goto Exit0;
    }
    ulFileSize =  GetFileSize(strFilePath);
    szVolumePathName = strFilePath.Left(nPos + 1);

    ::GetVolumePathName(strFilePath, szVolumePathName.GetBuffer(szVolumePathName.GetLength()), szVolumePathName.GetLength());
    ::GetDiskFreeSpace(szVolumePathName, &dwSectorsPerCluster, &dwBytesPerSector, &dwNumberOfFreeClusters, &dwTotalNumberOfClusters);

    dwClusterSize = dwSectorsPerCluster * dwBytesPerSector;
    uFileSpaceSize = static_cast<UINT64>(ceil(ulFileSize / static_cast<double>(dwClusterSize)) * dwClusterSize);

Exit0:
    return ulFileSize;
}

BOOL KFileOperation::StringEndWithDocumentSuffix(const CString& strFilePath)
{
    BOOL bRet = FALSE;
    std::map<CString, BOOL>::iterator it;
    CString strFileSuffix;

    _GetFilePathSuffix(strFilePath, strFileSuffix);
    it = gs_PreProcessSuffixName.find(strFileSuffix.MakeLower());
    if(gs_PreProcessSuffixName.end() != it)
    {
        bRet = TRUE;
    }
    return bRet;
}

void KFileOperation::_GetFilePathSuffix(const CString& strFilePath, CString& strFileSuffix)
{
    int nPos = 0;
    int nLen = strFilePath.GetLength();
    if(0 > nLen)
    {
        goto Exit0;
    }
    nPos = strFilePath.ReverseFind('.');
    if(-1 != nPos)
    {
        strFileSuffix = strFilePath.Right(nLen - nPos - 1);
    }

Exit0:
    return;
}