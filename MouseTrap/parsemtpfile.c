#include "mousetrap.h"

int ParseMTPFile(WCHAR *inFileName)
{
	HANDLE hFile, hHeap = GetProcessHeap();
	HWND dbgWnd = FindWindow(g_szClassName, NULL);
	//WCHAR mbBuf[4096] = L"";
	MIFILEHDR mifh;
	MOUSEINFO *miList;
	DWORD dwRead = 0, loLen, hiLen;
	int n = 0, i;
	//MessageBox(0, L"At the start of ParseMTPFile", L"Debug", MB_ICONASTERISK);
	hFile = CreateFile(inFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	loLen = GetFileSize(hFile, &hiLen);
	if (hiLen > 0)
	{
		CloseHandle(hFile);
		return -1; // cannot possibly be valid
	}
	if (loLen < sizeof(struct _MIFILEHDR))
	{
		CloseHandle(hFile);
		return -1; //cannot possibly be valid
	}
	SetFilePointer(hFile, 0, 0, FILE_BEGIN);
	ReadFile(hFile, &mifh, sizeof(struct _MIFILEHDR), &dwRead, NULL);

	if (VerifyMKLHeader(mifh) && VerifyMKLVersion(mifh))
	{
		n = GetMKLMIEntries(mifh);
		
	}
	else
	{
		WCHAR *wTxt = L"This does not appear to be a valid MouseTrap Playback file, or it is an unsupported version.";
		DebugPrint(dbgWnd, wTxt, wcslen(wTxt) + 1);
		return -1;
	}
	miList = (MOUSEINFO*)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, sizeof(struct _MOUSEINFO)*n);
	for (i = 0; i < n; i++)
	{
		
		ReadFile(hFile, &miList[i], sizeof(struct _MOUSEINFO), &dwRead, NULL);
		Sleep(miList[i].waitms);
		SetCursorPos(miList[i].x, miList[i].y);
		if(miList[i].click > 0)
		{
			if (miList[i].left > 0)
			{
				mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
				mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
			}
			else if (miList[i].left == 0)
			{
				mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
				mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
			}
		}
	}

	CloseHandle(hFile);

	//MessageBox(0, mbBuf, L"Report", MB_OK | MB_ICONINFORMATION);
	HeapFree(hHeap, 0, miList);

	// Restore mouse button state (if necessary)
	
	//mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	//mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
	return n;
}