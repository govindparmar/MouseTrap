#include "mousetrap.h"

MOUSEINFO *g_miFromFile = NULL;

// Loads the raw click data from %apppath%\mousetrap\clickdata.tmp into a structured save file.
//
// PARAMETERS:
//  inFileName is the path to the input file (wide character string)
//  outFileName is the path to the output file (wide character string)
//
// RETURN:
//  The number of mouse events saved into the structured file on successful execution, or -1 if an error occurs.
int LoadMIStructsFromFile(WCHAR *inFileName, WCHAR *outFileName)
{
	HWND dbgWnd = FindWindowW(g_szClassName, NULL);
	HANDLE hInFile, hOutFile, hHeap;
	DWORD hiLen, loLen, dwRead = 0, dwWritten = 0;
	MIFILEHDR mifh = { 0 };
	WCHAR msg[1000];
	int n = 0, i;

	// Step 1: Open up the temporary raw-data file
	hInFile = CreateFileW(inFileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hInFile == INVALID_HANDLE_VALUE)
	{
		DWORD le = GetLastError();
		StringCchPrintfW(msg, 1000, L"Problem loading recorded mouse data into new file: %I32u.\r\n", le);
		DebugPrint(dbgWnd, msg, wcslen(msg) + 1);
		return -1;
	}

	// Step 2: Get the size of the file
	loLen = GetFileSize(hInFile, &hiLen);

	// Fail on file size check is unlikely (since the CreateFile call requesting read access has succeeded at this point) but needs to be handled regardless.
	if (loLen == INVALID_FILE_SIZE)
	{
		StringCchCopyW(msg, 1000, L"Problem loading recorded mouse data into new file.\r\n");
		DebugPrint(dbgWnd, msg, wcslen(msg) + 1);
		CloseHandle(hInFile);
		return -1;
	}
	// If the high order 32-bits of the file's size are not zero, the file is too large to process and almost certainly not a valid MouseTrap data file; 
	if (hiLen > 0)
	{
		StringCchCopyW(msg, 1000, L"Data file is too large to process.\r\n");
		DebugPrint(dbgWnd, msg, wcslen(msg) + 1);
		CloseHandle(hInFile);
		return -1;
	}

	// Step 3: Load data from the temporary file into memory
	hHeap = GetProcessHeap();

	// The number of events in the data file is obviously the size of the file divided by the size of a mouse event structure
	n = loLen / sizeof(struct _MOUSEINFO);

	StringCchPrintfW(msg, 1000, L"Processed %d recorded mouse events from temporary file...\r\n", n);
	DebugPrint(dbgWnd, msg, wcslen(msg) + 1);

	// Allocate memory and load it into the dynamic memory array
	g_miFromFile = (MOUSEINFO*)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, n*sizeof(struct _MOUSEINFO));
	if (NULL == g_miFromFile)
	{
		DebugPrint(dbgWnd, "Fatal - System is out of memory.", 33);
		ExitProcess(ERROR_OUTOFMEMORY);
	}
	if (!ReadFile(hInFile, g_miFromFile, loLen, &dwRead, NULL))
	{
		DebugPrint(dbgWnd, L"Could not read MTP file data.", 30);
		return -1;
	}

	// No longer need the file to be open at this point.
	CloseHandle(hInFile);

	// Populate the MTP file header with the appropriate values
	mifh.magic = FOURCH('G', 'P', 'M', 'P');
	mifh.version = 1;
	mifh.numMI = n;
	
	// Create the output file and report any errors to the user.
	hOutFile = CreateFileW(outFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hOutFile == INVALID_HANDLE_VALUE)
	{
		WCHAR *msg = L"Reading mouse click data succeeded, but creating the file to save it into failed.";
		DebugPrint(dbgWnd, msg, wcslen(msg) + 1);
		HeapFree(hHeap, 0, g_miFromFile);
		return -1;
	}
	
	// Write the file header out to the file
	WriteFile(hOutFile, &mifh, sizeof(struct _MIFILEHDR), &dwWritten, NULL);

	// Then write all the mouse events into the file
	for (i = 0; i < n; i++)
	{
		SetFilePointer(hOutFile, 0, 0, SEEK_END);
		WriteFile(hOutFile, &g_miFromFile[i], sizeof(struct _MOUSEINFO), &dwWritten, NULL);
	}

	// Report save file with # of events to user
	StringCchPrintfW(msg, 1000, L"Saved new file \'%s\' with %d recorded mouse activities.\r\n", outFileName, n);
	DebugPrint(dbgWnd, msg, wcslen(msg) + 1);

	// Clean up and exit
	CloseHandle(hOutFile);
	HeapFree(hHeap, 0, g_miFromFile);
	return n;

}
