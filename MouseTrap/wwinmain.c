#include "mousetrap.h"

const WCHAR g_szClassName[] = L"MouseTrapWindowClass1";
WCHAR g_szClickData[MAX_PATH] = L"";

// Entry point for the application.
// 
// PARAMETERS:
//  hInstance is a handle to the current instance of this application.
//  hPrevInstance is no longer used by Windows.
//  lpCmdLine is a string containing the command line arguments passed to the program (unused).
//  nShowCmd indicates how the window is to be shown by ShowWindow (but this parameter is ignored and ShowWindow is called with SW_SHOW regardless).
//
// RETURNS:
//  The wParam of the last message posted to the window (which is typically WM_QUIT, where wParam contains the return code).
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	HWND hWnd, hDebugInfo, hRecord, hPlay, hStop;
	HMENU hMenu;
	HRESULT mkPath;
	BOOL bDir;
	WCHAR appPathDir[APPPATH_DIR_LENGTH];
	MSG Msg;
	NONCLIENTMETRICSW ncm;
	HFONT hfDefault;

	// NOTE: Replace this with a mutex instead
	if (FindWindowW(g_szClassName, NULL))
	{
		MessageBoxW(0, L"MouseTrap is already running! Please close any running copies of MouseTrap before starting another.  See the Manual for more information.", L"Error", MB_OK | MB_ICONSTOP);
		return -1;
	}

	ZeroMemory(&ncm, sizeof(NONCLIENTMETRICSW));
	ncm.cbSize = sizeof(NONCLIENTMETRICSW);
	SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICSW), &ncm, FALSE);
	hfDefault = CreateFontIndirectW(&ncm.lfMessageFont);

	// Create the %AppData%\MouseTrap folder
	GetEnvironmentVariableW(L"AppData", appPathDir, APPPATH_DIR_LENGTH);
	StringCchCatW(appPathDir, APPPATH_DIR_LENGTH, L"\\MouseTrap");
	bDir = CreateDirectoryW(appPathDir, NULL);

	if (bDir == FALSE && GetLastError() != ERROR_ALREADY_EXISTS)
	{
		MessageBoxW(0, L"An error occured while trying to create or access MouseTrap\'s data storage directory.  See the Manual for more information.", L"Error", MB_OK | MB_ICONSTOP);
		return -1;
	}

	// Populate g_szClickData with the path to clickdata.tmp in MouseTrap's application directory
	mkPath = StringCchPrintfW(g_szClickData, MAX_PATH, L"%s\\%s", appPathDir, L"clickdata.tmp");

	// This error is VERY unlikely given username length restrictions on Windows, but should be tested for regardless.
	if (mkPath != S_OK)
	{
		MessageBoxW(0, L"An error occured while trying to establish that the path to MouseTrap\'s data storage would not exceed the maximum path length permitted by Windows.  See the Manual for more information.", L"Error", MB_OK | MB_ICONSTOP);
		return -1;
	}
	
	// Register the window class
	if (RegisterWCEX(hInstance) == (ATOM) 0)
	{
		MessageBoxW(NULL, L"Could not register window class.", L"Error", MB_OK | MB_ICONSTOP);
		return -1;
	}

	// Create the main application window and all the child windows (aka controls)
	hMenu = LoadMenuW(hInstance, MAKEINTRESOURCE(IDR_MENU1));
	hWnd = CreateWindowExW(WS_EX_OVERLAPPEDWINDOW, g_szClassName, L"MouseTrap v1.0", WS_SYSMENU | WS_VISIBLE, 100, 100, 400, 300, NULL, hMenu, hInstance, NULL);
	hRecord = CreateWindowW(L"BUTTON", L"Record", WS_VISIBLE | WS_CHILD | BS_LEFTTEXT, 0, 0, 130, 30, hWnd, NULL, hInstance, NULL);
	hPlay = CreateWindowW(L"BUTTON", L"Play", WS_VISIBLE | WS_CHILD | BS_LEFTTEXT | WS_DISABLED, 130, 0, 130, 30, hWnd, NULL, hInstance, NULL);
	hStop = CreateWindowW(L"BUTTON", L"Stop", WS_VISIBLE | WS_CHILD | BS_LEFTTEXT | WS_DISABLED, 260, 0, 130, 30, hWnd, NULL, hInstance, NULL);
	hDebugInfo = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"Welcome to MouseTrap v1.0!\r\nRead the Manual for usage instructions.\r\n\r\nMouseTrap is completely free software with the source code available at http://github.com/govindparmar/MouseTrap.  If you like it, donate to me!\r\n", WS_VISIBLE | WS_CHILD | ES_READONLY | ES_MULTILINE , 0, 30, 380, 200, hWnd, NULL, hInstance, NULL);

	// Show the window
	ShowWindow(hWnd, SW_SHOW);
	// Process each of the child windows/controls and give them the default GUI font
	EnumChildWindows(hWnd, EnumChildProc, (LPARAM)&hfDefault);
	UpdateWindow(hWnd);

	// Message loop: Wait for messages to be posted to the queue and process them.
	while (GetMessageW(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessageW(&Msg);
	}

	// Since the last message posted to a window before quitting is WM_QUIT, where the wParam value specifies the return code, return that value.
	return (int) Msg.wParam;
}