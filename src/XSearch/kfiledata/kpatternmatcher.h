#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>

#define BMH_CHARSET_SIZE 256

class KPatternMatcher
{
public:
	static BOOL Search(const std::wstring& strText, const std::wstring& strPattern);
	static BOOL PatternSearch(const wchar_t* pszRowString, const wchar_t* pszPattern);
	static int BMH2(const wchar_t* pszText, int nTextLen, 
		const wchar_t* pszPattern, int nPatLen, int nStart);

private:
	KPatternMatcher();
	KPatternMatcher(const KPatternMatcher&);
};
