// *********************************************************************** //
//
//	MODULE:		DLGHELP (.CPP)
//
//	PURPOSE:	The two reference dialogs reachable from Advanced Options:
//				"Advanced Options - Help" (ADV_HELP), which explains each
//				advanced switch, and the 3D card notes list (ADV_3D).
//				Both are the same list-box-drives-an-edit-box machine, so
//				they share the item and text helpers.
//
//				Shipped NOLF.exe v1.003: 0x407ed0 - 0x408471.
//
// *********************************************************************** //

#include "stdafx.h"
#include "regmgr.h"
#include "launcher.h"


// Globals...

int		g_nHelpSel;				// 0x94b8 - remembered list selection


// Prototypes...

BOOL	Init3DCardList(HWND hWnd);
BOOL	InitAdvOptList(HWND hWnd);
BOOL	AddHelpItem(HWND hList, const char* sName, int nHelpID, int nNameID);
BOOL	UpdateHelpText(HWND hWnd);


// *********************************************************************** //
//
//	ROUTINE:	AdvHelpDlgProc   [0x407ed0]
//
//	PURPOSE:	Dialog procedure for the advanced options help dialog
//
// *********************************************************************** //

BOOL CALLBACK AdvHelpDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			HICON hIcon = LoadIcon(g_hInst, "ICON");
			if (hIcon)
			{
				SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
			}

			InitAdvOptList(hWnd);

			return(TRUE);
		}

		case WM_COMMAND:
		{
			if (wParam == 1)
			{
				if (g_pRegMgr)
				{
					g_pRegMgr->Set("AdvOptSel", g_nHelpSel);
				}

				EndDialog(hWnd, 1);
				return(TRUE);
			}
			else if (wParam == 2)
			{
				EndDialog(hWnd, 0);
				return(TRUE);
			}
			else if (HIWORD(wParam) == LBN_SELCHANGE)
			{
				UpdateHelpText(hWnd);
			}

			return(TRUE);
		}
	}

	return(FALSE);
}


// *********************************************************************** //
//
//	ROUTINE:	Adv3DDlgProc   [0x407f90]
//
//	PURPOSE:	Dialog procedure for the 3D card notes dialog
//
// *********************************************************************** //

BOOL CALLBACK Adv3DDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			HICON hIcon = LoadIcon(g_hInst, "ICON");
			if (hIcon)
			{
				SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
			}

			Init3DCardList(hWnd);

			return(TRUE);
		}

		case WM_COMMAND:
		{
			if (wParam == 1)
			{
				if (g_pRegMgr)
				{
					g_pRegMgr->Set("3DCardSel", g_nHelpSel);
				}

				EndDialog(hWnd, 1);
				return(TRUE);
			}
			else if (wParam == 2)
			{
				EndDialog(hWnd, 0);
				return(TRUE);
			}
			else if (HIWORD(wParam) == LBN_SELCHANGE)
			{
				UpdateHelpText(hWnd);
			}

			return(TRUE);
		}
	}

	return(FALSE);
}


// *********************************************************************** //
//
//	ROUTINE:	Init3DCardList   [0x408050]
//
//	PURPOSE:	Fills the 3D card list, one entry per chipset we have notes
//				for; the item data is the string table id of those notes
//
// *********************************************************************** //

BOOL Init3DCardList(HWND hWnd)
{
	if (!hWnd) return(FALSE);

	HWND hList = GetDlgItem(hWnd, 1072);
	if (!hList) return(FALSE);

	SendMessage(hList, LB_RESETCONTENT, 0, 0);

	AddHelpItem(hList, "3DFX Voodoo1",     19, 0);
	AddHelpItem(hList, "3DFX Voodoo2",     20, 0);
	AddHelpItem(hList, "3DFX Banshee",     21, 0);
	AddHelpItem(hList, "Intel i740",       22, 0);
	AddHelpItem(hList, "Matrox G200",      23, 0);
	AddHelpItem(hList, "Matrox M3D",       31, 0);
	AddHelpItem(hList, "Matrox MGA",       32, 0);
	AddHelpItem(hList, "nVidia Riva TNT",  24, 0);
	AddHelpItem(hList, "nVidia Riva 128",  25, 0);
	AddHelpItem(hList, "ATI Rage Pro",     26, 0);
	AddHelpItem(hList, "ATI Rage 128",     72, 0);
	AddHelpItem(hList, "3DLabs Permedia2", 27, 0);
	AddHelpItem(hList, "Rendition V1000",  33, 0);
	AddHelpItem(hList, "Rendition V2x00",  28, 0);
	AddHelpItem(hList, "S3 Savage",        29, 0);
	AddHelpItem(hList, "S3 Virge",         30, 0);

	DWORD dwSel = 0;
	if (g_pRegMgr)
	{
		dwSel = g_pRegMgr->Get("3DCardSel", 0);
	}

	SendMessage(hList, LB_SETCURSEL, dwSel, 0);

	UpdateHelpText(hWnd);

	return(TRUE);
}


// *********************************************************************** //
//
//	ROUTINE:	InitAdvOptList   [0x4081c0]
//
//	PURPOSE:	Fills the advanced options list; the item data is the string
//				table id of the option's explanation, and the fourth
//				argument the id of its display name
//
// *********************************************************************** //

BOOL InitAdvOptList(HWND hWnd)
{
	if (!hWnd) return(FALSE);

	HWND hList = GetDlgItem(hWnd, 1072);
	if (!hList) return(FALSE);

	SendMessage(hList, LB_RESETCONTENT, 0, 0);

	AddHelpItem(hList, "Disable sound",                34, 74);
	AddHelpItem(hList, "Disable music",                35, 75);
	AddHelpItem(hList, "Disable movies ",              36, 76);
	AddHelpItem(hList, "Disable fog",                  38, 77);
	AddHelpItem(hList, "Disable joysticks",            42, 78);
	AddHelpItem(hList, "Disable triple buffering",     45, 79);
	AddHelpItem(hList, "Disable hardware cursor",      46, 80);
	AddHelpItem(hList, "Disable animated load screen", 47, 81);
#ifdef NOLFSRC_MODERN
	// M-P1-29
	AddHelpItem(hList, "Disable launcher sound",       85, 86);
#endif
	AddHelpItem(hList, "Restore defaults",             48, 82);
	AddHelpItem(hList, "Command-line",                 49, 83);

	DWORD dwSel = 0;
	if (g_pRegMgr)
	{
		dwSel = g_pRegMgr->Get("AdvOptSel", 0);
	}

	SendMessage(hList, LB_SETCURSEL, dwSel, 0);

	UpdateHelpText(hWnd);

	return(TRUE);
}


// *********************************************************************** //
//
//	ROUTINE:	AddHelpItem   [0x4082d0]
//
//	PURPOSE:	Adds one list entry.  nNameID, when non-zero, replaces the
//				literal name with the localized string; nHelpID rides along
//				as the item data
//
// *********************************************************************** //

BOOL AddHelpItem(HWND hList, const char* sName, int nHelpID, int nNameID)
{
	if (!hList) return(FALSE);

	const char* sText = sName;

	char sBuf[256];
	if (nNameID)
	{
		if (LoadString(g_hInst, nNameID, sBuf, 250))
		{
			sText = sBuf;
		}
	}

	int nIndex = SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)sText);
	if (nIndex == LB_ERR) return(FALSE);

	SendMessage(hList, LB_SETITEMDATA, nIndex, nHelpID);

	return(TRUE);
}


// *********************************************************************** //
//
//	ROUTINE:	UpdateHelpText   [0x408360]
//
//	PURPOSE:	Loads the selected entry's explanation into the edit box.
//				The string table stores bare newlines; the edit control
//				needs CRLF, so they are expanded on the way through
//
// *********************************************************************** //

BOOL UpdateHelpText(HWND hWnd)
{
	char sText[2048];
	char sBuf[2048];

	if (!hWnd) return(FALSE);

	HWND hList = GetDlgItem(hWnd, 1072);
	if (!hList) return(FALSE);

	int nIndex = SendMessage(hList, LB_GETCURSEL, 0, 0);
	if (nIndex == LB_ERR) return(FALSE);

	int nHelpID = SendMessage(hList, LB_GETITEMDATA, nIndex, 0);
	if (nHelpID == LB_ERR) return(FALSE);

	g_nHelpSel = nIndex;

	if (!LoadString(g_hInst, nHelpID, sBuf, 2000)) return(FALSE);

	int nLen = strlen(sBuf);
	int nPos = 0;
	for (int i = 0; i < nLen; i++)
	{
		if (sBuf[i] == '\n')
		{
			sText[nPos] = '\r';
			nPos++;
		}

		sText[nPos] = sBuf[i];
		nPos++;
	}

	sText[nPos] = '\0';

	SetDlgItemText(hWnd, 1070, sText);

	return(TRUE);
}


// EOF...
