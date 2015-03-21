#include "../stdafx.h"
#include "kfilefilter.h"

KFileFilter::KFileFilter(KThreadController* pThreadCtrl)
: m_pThreadCtrl(pThreadCtrl)
{
    _InitMap();
    _MapFilterTypeToHandle();
}

KFileFilter::~KFileFilter()
{

}

void KFileFilter::_InitMap()
{
    _InitMusicMap();
    _InitVedioMap();
    _InitDocumentMap();
    _InitImageMap();
}

BOOL KFileFilter::_IsDirectory(const CString& strFilePath)
{
    return GetFileAttributes(strFilePath) & FILE_ATTRIBUTE_DIRECTORY;
}

void KFileFilter::FilterFile(std::vector<DRIVE_AND_FRN>& vFileInfoVector, int nFileFilterType)
{
    std::map<int, pFilter>::const_iterator it = m_FilterTypeToHandle.find(nFileFilterType);
    if(m_FilterTypeToHandle.end() == it)
    {
        goto Exit0;
    }

    pFilter pFilterFunc = it->second;
    (this->*pFilterFunc)(vFileInfoVector);

Exit0:
    return;
}

BOOL KFileFilter::_MapFilterTypeToHandle()
{
    m_FilterTypeToHandle[FILE_FILTER_NONE] = &KFileFilter::_FilterFileByNone;
    m_FilterTypeToHandle[FILE_FILTER_DOCUMENT] = &KFileFilter::_FilterFileByDocument;
    m_FilterTypeToHandle[FILE_FILTER_IMAGE] = &KFileFilter::_FilterFileByImage;
    m_FilterTypeToHandle[FILE_FILTER_VEDIO] = &KFileFilter::_FilterFileByVedio;
    m_FilterTypeToHandle[FILE_FILTER_MUSIC] = &KFileFilter::_FilterFileByMusic;
    m_FilterTypeToHandle[FILE_FILTER_DIRECORY] = &KFileFilter::_FilterFileBySearchDirectory;
    m_FilterTypeToHandle[FILE_FILTER_FILE] = &KFileFilter::_FilterFileBySearchFile;
    return TRUE;
}

void KFileFilter::_InitMusicMap()
{
    for(int i = 0; i < g_nMusicSuffixNum; ++i)
    {
        m_MusicSuffix.insert(std::make_pair(g_strMusicSuffix[i], TRUE));
    }
}

void KFileFilter::_InitImageMap()
{
    for(int i = 0; i < g_nImageSuffixNum; ++i)
    {
        m_ImageSuffix.insert(std::make_pair(g_strImageSuffix[i], TRUE));
    }
}

void KFileFilter::_InitDocumentMap()
{
    for(int i = 0; i < g_nDocumentSuffixNum; ++i)
    {
        m_DocumentSuffix.insert(std::make_pair(g_strDocumentSuffix[i], TRUE));
    }
}

void KFileFilter::_InitVedioMap()
{
    for(int i = 0; i < g_nVedioSuffixNum; ++i)
    {
        m_VedioSuffix.insert(std::make_pair(g_strVedioSuffix[i], TRUE));
    }
}

void KFileFilter::_GetFilePathSuffix(const CString& strFilePath, CString& strFileSuffix)
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

BOOL KFileFilter::_StringEndWithMusicSuffix(const CString& strFilePath)
{
    BOOL bRet = FALSE;
    int nPos = 0;
    CString strFileSuffix;
    std::map<CString, BOOL>::iterator it;

    _GetFilePathSuffix(strFilePath, strFileSuffix);
    it = m_MusicSuffix.find(strFileSuffix);
    if(m_MusicSuffix.end() != it)
    {
        bRet = TRUE;
    }
    return bRet;
}

BOOL KFileFilter::_StringEndWithVedioSuffix(const CString& strFilePath)
{
    BOOL bRet = FALSE;
    std::map<CString, BOOL>::iterator it;
    CString strFileSuffix;

    _GetFilePathSuffix(strFilePath, strFileSuffix);
    it = m_VedioSuffix.find(strFileSuffix);
    if(m_VedioSuffix.end() != it)
    {
        bRet = TRUE;
    }
    return bRet;
}

BOOL KFileFilter::_StringEndWithDocumentSuffix(const CString& strFilePath)
{
    BOOL bRet = FALSE;
    std::map<CString, BOOL>::iterator it;
    CString strFileSuffix;

    _GetFilePathSuffix(strFilePath, strFileSuffix);
    it = m_DocumentSuffix.find(strFileSuffix.MakeLower());
    if(m_DocumentSuffix.end() != it)
    {
        bRet = TRUE;
    }
    return bRet;
}

BOOL KFileFilter::_StringEndWithImageSuffix(const CString& strFilePath)
{
    BOOL bRet = FALSE;
    std::map<CString, BOOL>::iterator it;
    CString strFileSuffix;

    _GetFilePathSuffix(strFilePath, strFileSuffix);
    it = m_ImageSuffix.find(strFileSuffix);
    if(m_ImageSuffix.end() != it)
    {
        bRet = TRUE;
    }
    return bRet;
}


void KFileFilter::_FilterFileBySearchDirectory(std::vector<DRIVE_AND_FRN>& vFileInfoVector)
{
    const CString* pstrFilePath = NULL;
    CString strFilePath;
    std::vector<DRIVE_AND_FRN> vFileDirectoryVector;

    for(int i = 0; i < vFileInfoVector.size(); ++i)
    {
        m_pThreadCtrl->BuildPath(vFileInfoVector[i], &pstrFilePath);
        if(NULL == pstrFilePath)
        {
            goto Exit0;
        }
        strFilePath.Format(L"%c:%s%s", vFileInfoVector[i].m_cDriveName, *pstrFilePath, *(vFileInfoVector[i].m_pstrFileName));
        if(_IsDirectory(strFilePath))
        {
            vFileDirectoryVector.push_back(vFileInfoVector[i]);
        }
    }

    vFileInfoVector.clear();
    vFileInfoVector.resize(vFileDirectoryVector.size());
    copy(vFileDirectoryVector.begin(), vFileDirectoryVector.end(), vFileInfoVector.begin());

Exit0:
    return;
}

void KFileFilter::_FilterFileBySearchFile(std::vector<DRIVE_AND_FRN>& vFileInfoVector)
{
    const CString* pstrFilePath = NULL;
    CString strFilePath;
    std::vector<DRIVE_AND_FRN> vFileVector;
    for(int i = 0; i < vFileInfoVector.size(); ++i)
    {
        m_pThreadCtrl->BuildPath(vFileInfoVector[i], &pstrFilePath);
        if(NULL == pstrFilePath)
        {
            goto Exit0;
        }
        strFilePath.Format(L"%c:%s%s", vFileInfoVector[i].m_cDriveName, *pstrFilePath, *(vFileInfoVector[i].m_pstrFileName));
        if(!_IsDirectory(strFilePath))
        {
            vFileVector.push_back(vFileInfoVector[i]);
        }
    }
    vFileInfoVector.clear();
    vFileInfoVector.resize(vFileVector.size());
    copy(vFileVector.begin(), vFileVector.end(), vFileInfoVector.begin());

Exit0:
    return;
}

void KFileFilter::_FilterFileByMusic(std::vector<DRIVE_AND_FRN>& vFileInfoVector)
{
    const CString* pstrFilePath = NULL;
    CString strFilePath;
    std::vector<DRIVE_AND_FRN> vFileMusicVector;
    for(int i = 0; i < vFileInfoVector.size(); ++i)
    {
        m_pThreadCtrl->BuildPath(vFileInfoVector[i], &pstrFilePath);
        if(NULL == pstrFilePath)
        {
            goto Exit0;
        }
        strFilePath.Format(L"%c:%s%s", vFileInfoVector[i].m_cDriveName, *pstrFilePath, *(vFileInfoVector[i].m_pstrFileName));
        if(_StringEndWithMusicSuffix(*vFileInfoVector[i].m_pstrFileName))
        {
            vFileMusicVector.push_back(vFileInfoVector[i]);
        }
    }
    vFileInfoVector.clear();
    vFileInfoVector.resize(vFileMusicVector.size());
    copy(vFileMusicVector.begin(), vFileMusicVector.end(), vFileInfoVector.begin());

Exit0:
    return;
}

void KFileFilter::_FilterFileByVedio(std::vector<DRIVE_AND_FRN>& vFileInfoVector)
{
    const CString* pstrFilePath = NULL;
    CString strFilePath;
    std::vector<DRIVE_AND_FRN> vFileVedioVector;
    for(int i = 0; i < vFileInfoVector.size(); ++i)
    {
        m_pThreadCtrl->BuildPath(vFileInfoVector[i], &pstrFilePath);
        if(NULL == pstrFilePath)
        {
            goto Exit0;
        }
        strFilePath.Format(L"%c:%s%s", vFileInfoVector[i].m_cDriveName, *pstrFilePath, *(vFileInfoVector[i].m_pstrFileName));
        if( _StringEndWithVedioSuffix(*vFileInfoVector[i].m_pstrFileName))
        {
            vFileVedioVector.push_back(vFileInfoVector[i]);
        }
    }
    vFileInfoVector.clear();
    vFileInfoVector.resize(vFileVedioVector.size());
    copy(vFileVedioVector.begin(), vFileVedioVector.end(), vFileInfoVector.begin());

Exit0:
    return;
}

void KFileFilter::_FilterFileByImage(std::vector<DRIVE_AND_FRN>& vFileInfoVector)
{
    const CString* pstrFilePath = NULL;
    CString strFilePath;
    std::vector<DRIVE_AND_FRN> vFileImageVector;
    for(int i = 0; i < vFileInfoVector.size(); ++i)
    {
        m_pThreadCtrl->BuildPath(vFileInfoVector[i], &pstrFilePath);
        if(NULL == pstrFilePath)
        {
            goto Exit0;
        }
        strFilePath.Format(L"%c:%s%s", vFileInfoVector[i].m_cDriveName, *pstrFilePath, *(vFileInfoVector[i].m_pstrFileName));
        if(_StringEndWithImageSuffix(*vFileInfoVector[i].m_pstrFileName))
        {
            vFileImageVector.push_back(vFileInfoVector[i]);
        }
    }
    vFileInfoVector.clear();
    vFileInfoVector.resize(vFileImageVector.size());
    copy(vFileImageVector.begin(), vFileImageVector.end(), vFileInfoVector.begin());

Exit0:
    return;
}

void KFileFilter::_FilterFileByDocument(std::vector<DRIVE_AND_FRN>& vFileInfoVector)
{
    const CString* pstrFilePath = NULL;
    CString strFilePath;
    std::vector<DRIVE_AND_FRN> vFileDocumentVector;
    for(int i = 0; i < vFileInfoVector.size(); ++i)
    {
        m_pThreadCtrl->BuildPath(vFileInfoVector[i], &pstrFilePath);
        if(NULL == pstrFilePath)
        {
            goto Exit0;
        }
        strFilePath.Format(L"%c:%s%s", vFileInfoVector[i].m_cDriveName, *pstrFilePath, *(vFileInfoVector[i].m_pstrFileName));
        if(_StringEndWithDocumentSuffix(*vFileInfoVector[i].m_pstrFileName))
        {
            vFileDocumentVector.push_back(vFileInfoVector[i]);
        }
    }
    vFileInfoVector.clear();
    vFileInfoVector.resize(vFileDocumentVector.size());
    copy(vFileDocumentVector.begin(), vFileDocumentVector.end(), vFileInfoVector.begin());

Exit0:
    return;
}

void KFileFilter::_FilterFileByNone(std::vector<DRIVE_AND_FRN>& vFileInfoVector)
{
    return;
}


