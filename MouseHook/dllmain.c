#include "mousehook.h"

int g_lastGTC = 0;
HANDLE g_hFile;

// Entry point function; called whenever the DLL is attached or detached from a thread or process
// 
// PARAMETERS:
//  hInstDLL is a handle to the DLL.
//  dwReason specifies why DllMain is being called (attach/detach; process/thread)
//  lpReserved is reserved by the system.
BOOL APIENTRY DllMain(HANDLE hInstDLL, DWORD dwReason, LPVOID lpReserved)
{
	WCHAR clickDataFile[MAX_PATH];
	g_lastGTC = GetTickCount();
	switch (dwReason)
	{
		// If MouseHook.dll is being attached to a process, determine the user's %AppData% environment variable and path to clickdata.tmp
		case DLL_PROCESS_ATTACH:
		{
			GetEnvironmentVariable(L"AppData", clickDataFile, MAX_PATH);
			StringCchCat(clickDataFile, MAX_PATH, L"\\MouseTrap\\clickdata.tmp");

			g_hFile = CreateFile(clickDataFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

			// If clickdata.tmp could not be created, inform the user, fail DllMain and exit.
			if (g_hFile == INVALID_HANDLE_VALUE && GetLastError() != ERROR_SHARING_VIOLATION)
			{
				WCHAR mbBuf[256];
				DWORD le = GetLastError();
				StringCchPrintf(mbBuf, 256, L"Error creating the temporary file to store mouse click information in while the program is recording: %d (0x%.8X)", le, le);
				MessageBox(0, mbBuf, L"Error", MB_OK | MB_ICONSTOP);
				return FALSE;
			}
			
		}
		break;
		// If MouseHook.dll is being detached/unloaded, simply close the open handle to clickdata.tmp.
		case DLL_PROCESS_DETACH:
		{
			CloseHandle(g_hFile);
		}
		break;
	}
	return TRUE;
}