#pragma once

#include "../stdafx.h"
#include <vector>
#include <map>
#include <hash_map>
#include <atlstr.h>
#include <windows.h>
#include "kusinformation.h"

class KPathBuilder
{
public:
	KPathBuilder(stdext::hash_map<DWORDLONG, std::vector<DWORDLONG>>& FrnForPath, 
		FrnNameMap& FrnToName, 
		stdext::hash_map<DWORDLONG, FileAndPath>& vNameToPath);
	~KPathBuilder();
	void Build();

private:
	void _BuildPath(DWORDLONG dwlFrn, CString strPath);

	stdext::hash_map<DWORDLONG, std::vector<DWORDLONG>>& m_FrnForPath;
	FrnNameMap& m_FrnToName;
	stdext::hash_map<DWORDLONG, FileAndPath>& m_vFrnToFileInfo;
};
