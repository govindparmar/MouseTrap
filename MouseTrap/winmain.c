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
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
	HWND hWnd, hDebugInfo, hRecord, hPlay, hStop;
	HMENU hMenu;
	HRESULT mkPath;
	BOOL bDir;
	WCHAR appPathDir[APPPATH_DIR_LENGTH];
	MSG Msg;

	// NOTE: Replace this with a mutex instead
	if (FindWindow(g_szClassName, NULL))
	{
		MessageBox(0, L"MouseTrap is already running! Please close any running copies of MouseTrap before starting another.  See the Manual for more information.", L"Error", MB_OK | MB_ICONSTOP);
		return -1;
	}

	// Create the %AppData%\MouseTrap folder
	GetEnvironmentVariable(L"AppData", appPathDir, APPPATH_DIR_LENGTH);
	StringCchCat(appPathDir, APPPATH_DIR_LENGTH, L"\\MouseTrap");
	bDir = CreateDirectory(appPathDir, NULL);

	// If CreateDirectory failed and the error was not "already exists", report the error to the user and quit.
	if (bDir == FALSE && GetLastError() != ERROR_ALREADY_EXISTS)
	{
		MessageBox(0, L"An error occured while trying to create or access MouseTrap\'s data storage directory.  See the Manual for more information.", L"Error", MB_OK | MB_ICONSTOP);
		return -1;
	}

	// Populate g_szClickData with the path to clickdata.tmp in MouseTrap's application directory
	mkPath = StringCchPrintf(g_szClickData, MAX_PATH, L"%s\\%s", appPathDir, L"clickdata.tmp");

	// This error is VERY unlikely given username length restrictions on Windows, but should be tested for regardless.
	if (mkPath != S_OK)
	{
		MessageBox(0, L"An error occured while trying to establish that the path to MouseTrap\'s data storage would not exceed the maximum path length permitted by Windows.  See the Manual for more information.", L"Error", MB_OK | MB_ICONSTOP);
		return -1;
	}
	
	// Register the window class
	RegisterWCEX(hInstance);

	// Create the main application window and all the child windows (aka controls)
	hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1));
	hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, g_szClassName, L"MouseTrap v1.0", WS_SYSMENU | WS_VISIBLE, 100, 100, 400, 300, NULL, hMenu, hInstance, NULL);
	hRecord = CreateWindow(L"BUTTON", L"Record", WS_VISIBLE | WS_CHILD | BS_LEFTTEXT, 0, 0, 130, 30, hWnd, NULL, hInstance, NULL);
	hPlay = CreateWindow(L"BUTTON", L"Play", WS_VISIBLE | WS_CHILD | BS_LEFTTEXT | WS_DISABLED, 130, 0, 130, 30, hWnd, NULL, hInstance, NULL);
	hStop = CreateWindow(L"BUTTON", L"Stop", WS_VISIBLE | WS_CHILD | BS_LEFTTEXT | WS_DISABLED, 260, 0, 130, 30, hWnd, NULL, hInstance, NULL);
	hDebugInfo = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"Welcome to MouseTrap v1.0!\r\nRead the Manual for usage instructions.\r\n\r\nMouseTrap is completely free software with the source code available at http://github.com/govindparmar/MouseTrap.  If you like it, donate to me!\r\n", WS_VISIBLE | WS_CHILD | ES_READONLY | ES_MULTILINE , 0, 30, 380, 200, hWnd, NULL, hInstance, NULL);

	// Show the window
	ShowWindow(hWnd, SW_SHOW);
	// Process each of the child windows/controls and give them the default GUI font
	EnumChildWindows(hWnd, EnumChildProc, 0L);
	UpdateWindow(hWnd);

	// Message loop: Wait for messages to be posted to the queue and process them.
	while (GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	// Since the last message posted to a window before quitting is WM_QUIT, where the wParam value specifies the return code, return that value.
	return Msg.wParam;
}