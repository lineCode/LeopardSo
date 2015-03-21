#include <algorithm>
#include "kpatternmatcher.h"

KPatternMatcher::KPatternMatcher()
{
}

BOOL KPatternMatcher::Search(const std::wstring& strText, const std::wstring& strPattern)
{
    int bRet = FALSE;
    size_t nLoc = strText.find(strPattern);

    if (nLoc != std::wstring::npos)
    {
        bRet = TRUE;
    }
    return bRet;
}

int KPatternMatcher::BMH2(const wchar_t* pszText, int nTextLen, 
                          const wchar_t* pszPattern, int nPatternLen, 
                          int nStart)
{
    int nRet = -1;
    int nPatternStrLen = 0; 
    UINT uSkipTable[BMH_CHARSET_SIZE];

    if(NULL == pszText || NULL == pszPattern)
    {
        goto Exit0;
    }

    for (int i = 0; i < BMH_CHARSET_SIZE; i++)
    {
        uSkipTable[i] = (UINT)nPatternLen;
    }

    nPatternStrLen = nPatternLen - 1;
    for (int i = 0; i < nPatternStrLen; ++i) 
    {
        uSkipTable[0xFF & pszPattern[i]] = (UINT)(nPatternStrLen - i);
    }

    for (int k = nStart + nPatternStrLen, j = 0; k < nTextLen; k += uSkipTable[0xFF & pszText[k]])
    {
        for (int i = k, j = nPatternStrLen; ; i--, j--)
        {
            if (j < 0)
            {
                nRet = i + 1;
                goto Exit0;
            }
            if (pszText[i] != pszPattern[j])
            {
                break;
            }
        }
    }

Exit0:
    return nRet;
}

BOOL KPatternMatcher::PatternSearch(const wchar_t *pstrRowString, const wchar_t *pstrWildCard) 
{
    const wchar_t *pszCp = NULL;
    const wchar_t *pszMp = NULL;
    BOOL bRet = FALSE;

    if(NULL == pstrRowString || NULL == pstrWildCard)
    {
        goto Exit0;
    }

    while ((*pstrRowString) && ('*' != *pstrWildCard)) 
    {
        if ((*pstrWildCard != *pstrRowString) && ('?' != *pstrWildCard)) 
        {
            goto Exit0;
        }

        pstrWildCard++;
        pstrRowString++;
    }

    while (*pstrRowString) 
    {
        if ('*' == *pstrWildCard) 
        {
            if (!*++pstrWildCard) 
            {
                bRet = TRUE;
                goto Exit0;
            }
            pszMp = pstrWildCard;
            pszCp = pstrRowString + 1;
        } 
        else if ((*pstrWildCard == *pstrRowString) || ('?' == *pstrWildCard)) 
        {
            pstrWildCard++;
            pstrRowString++;
        } 
        else 
        {
            pstrWildCard = pszMp;
            pstrRowString = pszCp++;
        }
    }

    while ('*' == *pstrWildCard) 
    {
        pstrWildCard++;
    }
    bRet = !*pstrWildCard;

Exit0:
    return bRet;
}
