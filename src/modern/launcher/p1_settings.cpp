// ----------------------------------------------------------------------- //
//
// MODULE  : p1_settings.cpp
//
// PURPOSE : M-P1-28, the launcher's settings in a file (modern build only).
//
//           The launcher (NOLF.exe, NOLFLauncher.exe) and the dedicated
//           server's custom-rez page (NolfServ.exe) keep their settings
//           through Monolith's CRegMgr, in the registry: HKLM\Software\
//           Monolith Productions\No One Lives Forever\1.0. Under Wine and
//           Proton the registry belongs to a prefix, which is easily lost or
//           replaced (issue #46). This is CRegMgr again, the same class
//           (regmgr.h) linked in place of RegMgr.lib, keeping the values in
//           launcher.ini beside the program instead: one section per
//           registry key ("No One Lives Forever\1.0"), strings quoted,
//           numbers in decimal, binary values as "hex:" and their bytes.
//           The registry is neither read nor written.
//
// ----------------------------------------------------------------------- //

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "regmgr.h"

namespace
{
	const char	kFileName[]		= "launcher.ini";
	const char	kMissing[]		= "\x01";	// GetPrivateProfileString's default: no such value
	const char	kHexPrefix[]	= "hex:";

	// launcher.ini beside the running program
	const char* SettingsFile()
	{
		static char s_szFile[MAX_PATH] = "";
		if (!s_szFile[0])
		{
			char szDir[MAX_PATH];
			DWORD n = GetModuleFileName(NULL, szDir, sizeof(szDir));
			char* pSlash = (n && n < sizeof(szDir)) ? strrchr(szDir, '\\') : NULL;
			if (pSlash)
			{
				pSlash[1] = '\0';
				_snprintf(s_szFile, sizeof(s_szFile) - 1, "%s%s", szDir, kFileName);
			}
			else
			{
				strcpy(s_szFile, kFileName);
			}
			s_szFile[sizeof(s_szFile) - 1] = '\0';
		}
		return s_szFile;
	}

	// A value as the file holds it, LTFALSE if there is none.
	BOOL ReadRaw(const char* pSection, const char* pKey, char* pBuf, DWORD nBufLen)
	{
		GetPrivateProfileString(pSection, pKey, kMissing, pBuf, nBufLen, SettingsFile());
		return strcmp(pBuf, kMissing) != 0;
	}

	BOOL WriteString(const char* pSection, const char* pKey, const char* pValue)
	{
		// quoted, so that leading and trailing spaces survive
		int nLen = strlen(pValue);
		char* pQuoted = (char*)malloc(nLen + 3);
		if (!pQuoted) return FALSE;
		pQuoted[0] = '"';
		memcpy(pQuoted + 1, pValue, nLen);
		pQuoted[nLen + 1] = '"';
		pQuoted[nLen + 2] = '\0';
		BOOL bOK = WritePrivateProfileString(pSection, pKey, pQuoted, SettingsFile());
		free(pQuoted);
		return bOK;
	}

	BOOL WriteNumber(const char* pSection, const char* pKey, DWORD nValue)
	{
		char szNum[16];
		sprintf(szNum, "%lu", nValue);
		return WritePrivateProfileString(pSection, pKey, szNum, SettingsFile());
	}

	BOOL WriteBinary(const char* pSection, const char* pKey, const void* pValue, int nLen)
	{
		char* pHex = (char*)malloc(sizeof(kHexPrefix) + nLen * 2);
		if (!pHex) return FALSE;
		strcpy(pHex, kHexPrefix);
		char* p = pHex + strlen(kHexPrefix);
		for (int i = 0; i < nLen; i++, p += 2)
			sprintf(p, "%02x", ((const unsigned char*)pValue)[i]);
		*p = '\0';
		BOOL bOK = WritePrivateProfileString(pSection, pKey, pHex, SettingsFile());
		free(pHex);
		return bOK;
	}
}

// The section in use is m_sApp ("<app>\<version>", and "\<subkey>" after
// SetSubKey); m_sSubKey keeps the bare "<app>\<version>".

BOOL CRegMgr::Init(const char* sCompany, const char* sApp, const char* sVersion, const char* sSubKey, HANDLE hRootKey, char* sRoot2)
{
	if (!sCompany || !sApp || !sVersion) return FALSE;

	_snprintf(m_sSubKey, sizeof(m_sSubKey) - 1, "%s\\%s", sApp, sVersion);
	m_sSubKey[sizeof(m_sSubKey) - 1] = '\0';
	strcpy(m_sApp, m_sSubKey);

	m_bInitialized = TRUE;
	if (!SetSubKey(sSubKey))
	{
		m_bInitialized = FALSE;
		return FALSE;
	}
	return TRUE;
}

void CRegMgr::Term()
{
	m_bInitialized = FALSE;
}

BOOL CRegMgr::SetSubKey(const char* sSubKey)
{
	if (!m_bInitialized) return FALSE;

	char szBase[256];
	strcpy(szBase, m_sSubKey);
	if (sSubKey)
		_snprintf(m_sApp, sizeof(m_sApp) - 1, "%s\\%s", szBase, sSubKey);
	else
		strcpy(m_sApp, szBase);
	m_sApp[sizeof(m_sApp) - 1] = '\0';
	return TRUE;
}

BOOL CRegMgr::Set(const char* sKey, const char* sValue)
{
	if (!m_bInitialized || !sKey || !sValue) return FALSE;
	return WriteString(m_sApp, sKey, sValue);
}

BOOL CRegMgr::Set(const char* sKey, void* pValue, int nLen)
{
	if (!m_bInitialized || !sKey || !pValue || nLen < 0) return FALSE;
	return WriteBinary(m_sApp, sKey, pValue, nLen);
}

BOOL CRegMgr::Set(const char* sKey, DWORD nValue)
{
	if (!m_bInitialized || !sKey) return FALSE;
	return WriteNumber(m_sApp, sKey, nValue);
}

char* CRegMgr::Get(const char* sKey, char* sBuf, UINT32& nBufSize, const char* sDef)
{
	// As the registry version: a value too long for the buffer is not there
	if (m_bInitialized && sKey && sBuf && nBufSize > 0)
	{
		char szValue[4096];
		if (ReadRaw(m_sApp, sKey, szValue, sizeof(szValue)) &&
			strncmp(szValue, kHexPrefix, strlen(kHexPrefix)) != 0 &&
			strlen(szValue) < nBufSize)
		{
			strcpy(sBuf, szValue);
			nBufSize = strlen(sBuf) + 1;
			return sBuf;
		}
	}

	if (sDef && sBuf)
	{
		strcpy(sBuf, sDef);
		nBufSize = strlen(sBuf);
		return sBuf;
	}
	nBufSize = 0;
	return NULL;
}

void* CRegMgr::Get(const char* sKey, void* pBuf, UINT32& nBufSize, void* pDef, UINT32 nDefSize)
{
	if (m_bInitialized && sKey && pBuf && nBufSize > 0)
	{
		char szValue[4096];
		int nPrefix = strlen(kHexPrefix);
		if (ReadRaw(m_sApp, sKey, szValue, sizeof(szValue)) &&
			strncmp(szValue, kHexPrefix, nPrefix) == 0)
		{
			UINT32 nLen = (UINT32)(strlen(szValue) - nPrefix) / 2;
			if (nLen <= nBufSize)
			{
				for (UINT32 i = 0; i < nLen; i++)
				{
					char szByte[3] = { szValue[nPrefix + 2 * i], szValue[nPrefix + 2 * i + 1], 0 };
					((unsigned char*)pBuf)[i] = (unsigned char)strtoul(szByte, NULL, 16);
				}
				nBufSize = nLen;
				return pBuf;
			}
		}
	}

	if (pDef && nDefSize > 0 && pBuf)
	{
		memcpy(pBuf, pDef, nDefSize);
		nBufSize = nDefSize;
		return pBuf;
	}
	nBufSize = 0;
	return NULL;
}

DWORD CRegMgr::Get(const char* sKey, DWORD nDef)
{
	if (!m_bInitialized || !sKey) return nDef;

	// a number, and nothing else (a string value is not a number, as in
	// the registry)
	char szValue[64];
	if (!ReadRaw(m_sApp, sKey, szValue, sizeof(szValue)) || !szValue[0]) return nDef;
	char* pEnd = NULL;
	DWORD nValue = strtoul(szValue, &pEnd, 10);
	return (pEnd && !*pEnd) ? nValue : nDef;
}

BOOL CRegMgr::Delete(const char* sKey)
{
	if (!m_bInitialized || !sKey) return FALSE;
	return WritePrivateProfileString(m_sApp, sKey, NULL, SettingsFile());
}

BOOL CRegMgr::CreateKey(HKEY hKey, const char* sSubkey, HKEY& hNewKey)
{
	return FALSE;	// (the registry's; not used here)
}

BOOL CRegMgr::DeleteApp()
{
	return WritePrivateProfileString(m_sSubKey, NULL, NULL, SettingsFile());
}

BOOL CRegMgr::DeleteSubKey()
{
	return WritePrivateProfileString(m_sApp, NULL, NULL, SettingsFile());
}

BOOL CRegMgr::DeleteUnderSubKey(const char* sSubKey)
{
	if (!sSubKey) return FALSE;
	char szSection[600];
	_snprintf(szSection, sizeof(szSection) - 1, "%s\\%s", m_sApp, sSubKey);
	szSection[sizeof(szSection) - 1] = '\0';
	return WritePrivateProfileString(szSection, NULL, NULL, SettingsFile());
}
