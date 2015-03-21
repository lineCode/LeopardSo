#pragma once

#include <ATLComTime.h>
#include <map>

#define DOCUMENT_TYPE_NUM               13
#define DEFAULT_FILE_MINIMIZE_SIZE      1024

const CString g_strTextSuffix[] = {"txt", "c", "cpp", "log", "ini", "h", "cc", "php", "cxx", "xml", "html", "js", "hpp"};

class KFileOperation
{
public:
    KFileOperation();
    ~KFileOperation();

    void ShowFileBriefInfo(const CString& strFilePath, CString& strFileText);
    UINT64 GetFileSize(const CString& strFilePath);
    void GetFileSizeStr(UINT64 ulFileSize, CString& strFileSize);
    void FormatFileSpaceSize(UINT64 ulFileSpaceSize, CString& strFileSpaceSize);
    void GetFileType(const CString& strFilePath, CString& strFileType);
    void GetFileIcon(const CString& strFilePath, HICON& hIcon);
    void CopyFilePathToClipPath(const CString* pstrFilePath);
    void OpenFileDirectory(CString& strFilePath);
    void OpenFile(const CString& strFilePath);
    void GetFileModifyTime(const CString& strFilePath, CString& strFileModifyTime);
    void GetFileModifyTime(const CString& strFilePath, COleDateTime& ColeDateTime);
    void GetFileAccessAndCreateTime(const CString& strFilePath, CString& strFileCreateTime, CString& strFileAccessTime);
    UINT64 GetFileSpaceSize(const CString& strFilePath);
    BOOL StringEndWithDocumentSuffix(const CString& strFilePath);

private:
    void _InitPreProcessSuffixName();
    SHORT _GetTextCodeType(const CString& strFilePath);
    void _ShowUnicodeText(const CString& strFilePath, CString& strFileText);
    void _ShowAsciiText(const CString& strFilePath, CString& strFileText);
    void _ShowUtf8Text(const CString& strFilePath, CString& strFileText);
    void _FormatFileSizeString(CString& strFileSize);
    void _GetFilePathSuffix(const CString& strFilePath, CString& strFileSuffix);

    static const int m_nUnicodeTextHead = -257;
    static const int m_nUft8TextHead = -17425;
    std::map<CString, BOOL> gs_PreProcessSuffixName;
};