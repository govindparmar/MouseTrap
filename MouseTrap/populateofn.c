#include "mousetrap.h"

// Populates an OPENFILENAME structure.
//
// PARAMETERS:
//  hWnd is the parent of the "Open" and "Save As..." dialogs.
//  ofn is a pointer to an OPENFILENAME structure to populate.
//  szFileName is a pointer to a wide-character string that will be populated with either the file to open or the filename to write save data into.
//  bOpenOrSave determines whether to populate ofn with values appropriate for opening a file (if it is TRUE) or for saving a file (if it is FALSE).
//
// RETURNS:
//  No return value, but the OPENFILENAME structure will be populated with appropriate values.
VOID PopulateOFN(HWND hWnd, OPENFILENAME *pOFN, WCHAR *szFileName, BOOL bOpenOrSave)
{
	ZeroMemory(pOFN, sizeof(OPENFILENAME));
	pOFN->lStructSize = sizeof(OPENFILENAME);
	pOFN->hwndOwner = hWnd;
	pOFN->lpstrFile = szFileName;
	pOFN->nMaxFile = MAX_PATH;
	if (bOpenOrSave == TRUE)
	{
		pOFN->lpstrFilter = L"MouseTrap Playback Files (*.mtp)\0*.MTP\0All Files (*.*)\0*.*\0\0";
		pOFN->Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_ENABLEHOOK | OFN_HIDEREADONLY;

	}
	else
	{
		pOFN->Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLEHOOK;
		pOFN->lpstrDefExt = L"mtp";
		pOFN->lpstrFilter = L"MouseTrap Playback Files (*.mtp)\0*.MTP\0\0";
	}
	pOFN->nFilterIndex = 1;
	pOFN->lpstrFileTitle = NULL;
	pOFN->nMaxFileTitle = 0;
	pOFN->lpstrInitialDir = NULL;
}
