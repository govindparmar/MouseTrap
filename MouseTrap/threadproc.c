#include "mousetrap.h"

// Thread that executes playback of mouse events from an MTP file.
// 
// PARAMETERS:
//  lpParam is a pointer to a wide-character string indicating the file to read from for playback.
//
// RETURNS:
//  0 always, indicating successful execution of the thread.
DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	HANDLE hFile, hHeap;
	HWND hWnd;
	LPWSTR lpszFilename = (LPWSTR)lpParam;
	MIFILEHDR mifh;
	MOUSEINFO *miList = NULL;
	DWORD dwRead = 0;
	ATOM aFlag = 0x0000;
	int i, n = 0;

	hWnd = FindWindowW(g_szClassName, NULL);
	hHeap = GetProcessHeap();

	// Attempt to open the MTP file
	hFile = CreateFileW(lpszFilename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		TooltipPrint(hWnd, L"Unable to read the MTP file.", 30);
		goto cleanup0;
	}

	// Establish validity of the MTP file format
	if (!ReadFile(hFile, &mifh, sizeof(struct _MIFILEHDR), &dwRead, NULL))
	{
		TooltipPrint(hWnd, L"Unable to read from the MTP file.", 34);
		goto cleanup0;
	}
	if (mifh.magic == FOURCH('G', 'P', 'M', 'P') && mifh.version==1)
	{
		n = mifh.numMI;
	}
	else
	{
		TooltipPrint(hWnd, L"This is not a valid MTP file.", 31);
		goto cleanup0;
	}

	// Allocate memory for the list of mouse event structures
	miList = (MOUSEINFO*)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, n*sizeof(struct _MOUSEINFO));
	if (NULL == miList)
	{
		TooltipPrint(hWnd, L"Fatal - insufficient memory on system.", 39);
		ExitProcess(ERROR_OUTOFMEMORY);
	}
	for (i = 0; i < n; i++)
	{
		// Read the list struct by struct
		if (!ReadFile(hFile, &miList[i], sizeof(struct _MOUSEINFO), &dwRead, NULL))
		{
			TooltipPrint(hWnd, L"Unable to read from the MTP file.", 34);
		}

		// Wait for the number of milliseconds specified by this mouse event
		Sleep(miList[i].waitms);

		// Move the cursor to the x and y coordinates specified by this mouse event
		SetCursorPos(miList[i].x, miList[i].y);

		// If it is a click event, perform the appropriate button click
		if (miList[i].click > 0)
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
	// Free up the memory allocated for mouse event structures, close the handle to the MTP file, and return 0 indicating successful thread execution.
	HeapFree(hHeap, 0, miList);
cleanup0:
	CloseHandle(hFile);
	return 0;
}
