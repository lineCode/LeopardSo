#pragma once

#include <map>
#include "kthreadcontroller.h"
#include "kusinformation.h"

const CString g_strMusicSuffix[] = {"mp3", "wav", "wma", "ape", "mid", "acc", "ogg"};
const CString g_strImageSuffix[] = {"bmp", "tiff", "gif", "jpg", "png", "raw", "svg", "psd"};
const CString g_strVedioSuffix[] = {"avi", "mov", "asf", "wmv", "mp4", "3gp", "flv", "mkv", "rmvb", "rm", "mpeg"};
const CString g_strDocumentSuffix[] = {"pdf", "txt", "cpp", "c", "h", "html", "xml", "php", "ppt", "xls", "wps",
                                       "rtf", "cxx", "log", "ini", "js", "doc", "docx", "pptx"};

const int g_nMusicSuffixNum = 7;
const int g_nImageSuffixNum = 8;
const int g_nVedioSuffixNum = 11;
const int g_nDocumentSuffixNum = 19;

class KFileFilter
{
public:
    KFileFilter(KThreadController* pThreadCtrl);
    ~KFileFilter();
    void FilterFile(std::vector<DRIVE_AND_FRN>& vFileInfoVector, int nFileFilterType);

private:

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

    void _FilterFileBySearchFile(std::vector<DRIVE_AND_FRN>& vFileInfoVector);
    void _FilterFileBySearchDirectory(std::vector<DRIVE_AND_FRN>& vFileInfoVector);
    void _FilterFileByMusic(std::vector<DRIVE_AND_FRN>& vFileInfoVector);
    void _FilterFileByVedio(std::vector<DRIVE_AND_FRN>& vFileInfoVector);
    void _FilterFileByImage(std::vector<DRIVE_AND_FRN>& vFileInfoVector);
    void _FilterFileByDocument(std::vector<DRIVE_AND_FRN>& vFileInfoVector);
    void _FilterFileByNone(std::vector<DRIVE_AND_FRN>& vFileInfoVector);

    BOOL _StringEndWithMusicSuffix(const CString& strFilePath);
    BOOL _StringEndWithVedioSuffix(const CString& strFilePath);
    BOOL _StringEndWithImageSuffix(const CString& strFilePath);
    BOOL _StringEndWithDocumentSuffix(const CString& strFilePath);

    void _InitMusicMap();
    void _InitVedioMap();
    void _InitDocumentMap();
    void _InitImageMap();
    void _InitMap();

    BOOL _IsDirectory(const CString& strFilePath);
    BOOL _MapFilterTypeToHandle();
    void _GetFilePathSuffix(const CString& strFilePath, CString& strFileSuffix);

    KThreadController* m_pThreadCtrl;
    std::map<CString, BOOL> m_MusicSuffix;
    std::map<CString, BOOL> m_VedioSuffix;
    std::map<CString, BOOL> m_DocumentSuffix;
    std::map<CString, BOOL> m_ImageSuffix;
    typedef void (KFileFilter::* pFilter)(std::vector<DRIVE_AND_FRN>& vFileInfoVector);
    std::map<int, pFilter> m_FilterTypeToHandle;
};