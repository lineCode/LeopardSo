#include "kpathbuilder.h"

#define ROOT_DRIVER_NUMBER      1407374883553285

KPathBuilder::KPathBuilder(stdext::hash_map<DWORDLONG, std::vector<DWORDLONG>>& FrnForPath, 
                           FrnNameMap& FrnToName, 
                           stdext::hash_map<DWORDLONG, FileAndPath>& vNameToPath) : 
m_FrnForPath(FrnForPath), 
m_FrnToName(FrnToName), 
m_vFrnToFileInfo(vNameToPath)
{
}

KPathBuilder::~KPathBuilder()
{
}

void KPathBuilder::Build()
{
    CString strPath;
    _BuildPath(ROOT_DRIVER_NUMBER, strPath);
}

void KPathBuilder::_BuildPath(DWORDLONG dwlFrn, CString strPath)
{
    FileAndPath FileInfo;
    stdext::hash_map<DWORDLONG, std::vector<DWORDLONG>>::iterator it;

    FileInfo.m_strFileName = m_FrnToName[dwlFrn].m_strFilename;
    FileInfo.m_strPathName = strPath;
	FileInfo.m_bIsDir = TRUE;

    it = m_FrnForPath.find(dwlFrn);
    if (it == m_FrnForPath.end())
    {
		FileInfo.m_bIsDir = FALSE;
		m_vFrnToFileInfo.insert(std::make_pair(dwlFrn, FileInfo));
        goto Exit0;
    }

	m_vFrnToFileInfo.insert(std::make_pair(dwlFrn, FileInfo));
    strPath  = strPath + m_FrnToName[dwlFrn].m_strFilename + "\\";
    for (std::vector<DWORDLONG>::iterator vIt = it->second.begin(); 
        vIt != it->second.end(); ++vIt)
    {
        _BuildPath(*vIt, strPath);
    }

Exit0:
    return;
}