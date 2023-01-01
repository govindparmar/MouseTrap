#include "mousetrap.h"

// Message-cracker for WM_COMMAND message
// 
// PARAMETERS:
//  hWnd is the window that the message was posted to (the main application window)
//  nID is the identifier for control that generated the command
//  hwSource is the window handle to source of the command, if applicable
//  uNotify is unused by this handler.
//
// REMARKS:
//  This handler may benefit from being broken down further due to its excessive length.

VOID WINAPI OnCommand(HWND hWnd, int nID, HWND hwSource, UINT uNotify)
{
	static WCHAR openFile[MAX_PATH] = L"";
	static HMENU hMenu;
	static HINSTANCE hInstDLL;
	static HHOOK hHook;

	HWND hRecord, hPlay, hStop;
	int loopVal = 0;

	
	// Get the HWNDs of the three buttons
	hRecord = FindWindowExW(hWnd, NULL, L"BUTTON", NULL);
	hPlay = FindWindowExW(hWnd, hRecord, L"BUTTON", NULL);
	hStop = FindWindowExW(hWnd, hPlay, L"BUTTON", NULL);

	
	// Handle the "Record" button event
	if (hwSource == hRecord)
	{
		HOOKPROC hookProc;

		// Try to delete any previous temporary click data file before beginning a new recording.
		if (DeleteFileW(g_szClickData) == FALSE)
		{
			WCHAR msg[2000] = L"";
			DWORD le = GetLastError();

			// Nothing to delete
			if (le == ERROR_FILE_NOT_FOUND)
			{
				StringCchCopyW(msg, 1000, L"No previous click data to delete.");
			}
			// Other error
			else
			{
				StringCchPrintfW(msg, 2000, L"Deleting previous click data failed: %I32u", le);
			}
			TooltipPrint(hWnd, msg, wcslen(msg) + 1);
		}
		// Success: there was a click data file, and it was deleted without any problem.
		else
		{
			WCHAR *msg = L"Deleting previous click data succeeded.";
			TooltipPrint(hWnd, msg, wcslen(msg) + 1);
		}

		// Load up the DLL file MouseHook.dll and get the address of the LowLevelMouseProc hook function
		hInstDLL = LoadLibraryW(L"MouseHook.dll");
		if (NULL == hInstDLL)
		{
			TooltipPrint(hWnd, L"Fatal - MouseHook.dll could not be loaded.", 43);
			ExitProcess(ERROR_DLL_NOT_FOUND);
		}
		hookProc = (HOOKPROC)GetProcAddress(hInstDLL, "LowLevelMouseProc");

		// Set the "currently recording" flag to true (necessary for the Ctrl+Alt+T hotkey to function properly)
		g_bCurrentlyRecording = TRUE;
		
		// Update the debug output to:
		// 1. Inform the user that recording has begun
		// 2. Inform the user of how to stop recording
		TooltipPrint(hWnd, L"Press Ctrl+Alt+T or use the Stop button to stop recording.", 62);

		// Install the low-level mouse hook procedure
		hHook = SetWindowsHookExW(WH_MOUSE_LL, hookProc, hInstDLL, 0);

		// Only the stop button should be enabled during recording
		EnableWindow(hPlay, FALSE);
		EnableWindow(hRecord, FALSE);
		EnableWindow(hStop, TRUE);
		return;
	}
	// Handle the "Play" button
	else if (hwSource == hPlay)
	{
		// If no file is open, nothing to play
		if (wcslen(openFile) == 0)
		{
			WCHAR *msg = L"Please open an MTP file first.";
			TooltipPrint(hWnd, msg, wcslen(msg) + 1);
		}
		else
		{
			// Before beginning playback, determine the state of the "continuous playback" option, so we know whether to run the playback once or repeatedly until the user presses F8 to stop.
			DWORD dwState;
			hMenu = GetMenu(hWnd);

			dwState = GetMenuState(hMenu, (UINT)ID_SETTINGS_CONTINUOUSPLAYBACK, MF_BYCOMMAND);
			if ((dwState & MF_CHECKED) != 0)
			{
				loopVal = 1;
			}

			// Disable all the buttons during playback.
			EnableWindow(hRecord, FALSE);
			EnableWindow(hStop, FALSE);
			EnableWindow(hPlay, FALSE);

			// Update the debug window to:
			// 1. Inform the user that playback has begun
			// 2. Inform the user of how to stop playback 
			TooltipPrint(hWnd, L"Press F8 to stop playback.", 28);
		
			// If the "continuous playback" setting is not applied, loopVal will be 0 and the do-while loop will only execute once. Otherwise, loopVal is 1 and will loop until VK_F8 is pressed.
			do
			{
				// Launch the mouse movement thread and wait for it to complete
				g_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&ThreadProc, (LPVOID)openFile, 0, NULL);
				if (NULL == g_hThread)
				{
					TooltipPrint(hWnd, L"Failed to start mouse movement monitor.", 40);
					ExitProcess(GetLastError());
				}
				WaitForSingleObject(g_hThread, INFINITE);

				// If the user has pressed F8, set the loop condition to 0 to break out.
				if (GetAsyncKeyState(VK_F8)) loopVal = 0;
			}
			while (loopVal);

			// The value of g_hThread may be used by future releases to determine if certain actions are appropriate to take based on the running/not running status of the thread.  
			// Resetting g_hThread to INVALID_HANDLE_VALUE after execution of the playback routine will allow other threads and timers in the program to know if a recording is currently in session.
			g_hThread = INVALID_HANDLE_VALUE;

			// After playing back, re-enable the record and play buttons (stop is not appropriate if no recording is in progress).
			EnableWindow(hRecord, TRUE);
			EnableWindow(hPlay, TRUE);
			EnableWindow(hStop, FALSE);
			
		}
		return;
	}
	// Handle the "Stop" button
	else if (hwSource == hStop)
	{
		OPENFILENAME ofn = { 0 };
		WCHAR saveFile[MAX_PATH] = L"";

		// Uninstall the low-level mouse hook and post WM_NULL to all the windows on the system to force uninstallation of the DLL
		UnhookWindowsHookEx(hHook);
		Sleep(10);
		PostMessageW(HWND_BROADCAST, WM_NULL, 0, 0);

		// Reset the "currently recording" flag to false (hotkey handler will no longer open up the "save file" dialog)
		g_bCurrentlyRecording = FALSE;

		// Populate the "OPENFILENAME" structure with appropriate values for saving the recorded mouse events into an MTP file.	
		PopulateOFN(hWnd, &ofn, saveFile, FALSE);

		// If the user did not cancel on the "save" dialog, write the data out to the filename they specified.
		if (GetSaveFileNameW(&ofn))
		{
			LoadMIStructsFromFile(g_szClickData, saveFile);
		}

		// Unload the DLL from memory entirely
		FreeLibrary(hInstDLL);

		// Restore default button state
		EnableWindow(hRecord, TRUE);
		EnableWindow(hPlay, TRUE);
		EnableWindow(hStop, FALSE);
		return;
	}

	// Menu commands
	switch (nID)
	{

		// File->Exit: post WM_CLOSE to the window
		case ID_FILE_EXIT:
			SendMessageW(hWnd, WM_CLOSE, 0, 0L);
			break;

		// Help->Manual: NYI
		case ID_HELP_MANUAL:
			MessageBoxW(0, L"The help manual is coming soon! Check the project\'s GitHub page for updates!", L"Information", MB_OK | MB_ICONINFORMATION);
			break;

		// Help->About: Display the "About" dialog box
		case ID_HELP_ABOUT:
			DialogBoxW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(IDD_ABOUTBOX), hWnd, DialogProc);
			break;

		// Help->Donate: Open a browser link to the donation page
		case ID_HELP_DONATE:

			ShellExecuteW(NULL, L"open", L"http://paypal.me/govind", NULL, NULL, SW_SHOW);
			break;

		// Settings->Continuous Playback: Toggle the playback setting from single to continuous
		case ID_SETTINGS_CONTINUOUSPLAYBACK:
		{
			DWORD dwState;
			hMenu = GetMenu(hWnd);

			// Determine the state of the menu option for continuous playback
			dwState = GetMenuState(hMenu, (UINT)ID_SETTINGS_CONTINUOUSPLAYBACK, MF_BYCOMMAND);

			// Then toggle it
			if ((dwState & MF_CHECKED) == 0)
			{
				CheckMenuItem(hMenu, ID_SETTINGS_CONTINUOUSPLAYBACK, MF_BYCOMMAND | MF_CHECKED);
			}
			else
			{
				CheckMenuItem(hMenu, ID_SETTINGS_CONTINUOUSPLAYBACK, MF_BYCOMMAND | MF_UNCHECKED);
			}
		}
		break;

		// File->Open: Open an MTP file for playback
		case ID_FILE_OPEN:
		{
			BOOL bOpened = FALSE;
			OPENFILENAME ofn = { 0 };

			// Populate the "OPENFILENAME" struct with appropriate values for opening an MTP file, and allow the user to open a file.
			PopulateOFN(hWnd, &ofn, openFile, TRUE);
			bOpened = GetOpenFileNameW(&ofn);

			// If they did open a file, enable the Play button (which will then play that file).
			if (bOpened)
			{
				EnableWindow(hPlay, TRUE);
			}

		}
		break;
	}
}
