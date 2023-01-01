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
INT APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ INT nShowCmd)
{
	HWND hWnd;
	HMENU hMenu;
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
	StringCchPrintfW(g_szClickData, MAX_PATH, L"%s\\%s", appPathDir, L"clickdata.tmp");
	
	// Register the window class
	if (RegisterWCEX(hInstance) == (ATOM) 0)
	{
		MessageBoxW(NULL, L"Could not register window class.", L"Error", MB_OK | MB_ICONSTOP);
		return -1;
	}

	// Create the main application window and all the child windows (aka controls)
	hMenu = LoadMenuW(hInstance, MAKEINTRESOURCE(IDR_MENU1));
	hWnd = CreateWindowExW(WS_EX_OVERLAPPEDWINDOW, g_szClassName, L"MouseTrap v1.0", WS_SYSMENU | WS_VISIBLE, 100, 100, 400, 300, NULL, hMenu, hInstance, NULL);
	
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
	return (INT) Msg.wParam;
}