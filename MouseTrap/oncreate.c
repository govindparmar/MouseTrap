#include "mousetrap.h"

// Message-cracker for the WM_CREATE message.
// 
// PARAMETERS:
//  hWnd is the window being created
//  lpcs is not used by this message handler.
//
// RETURNS:
//  TRUE indicating that window creation may proceed
BOOL WINAPI OnCreate(HWND hWnd, LPCREATESTRUCTW lpcs)
{
	INITCOMMONCONTROLSEX iccx;
	HINSTANCE hInstance = lpcs->hInstance;
	HWND hDebugInfo, hRecord, hPlay, hStop;

	g_bHotkeyEnabled = RegisterHotKey(hWnd, IDH_HOTKEY1, MOD_ALT | MOD_CONTROL, 'T');
	if (!g_bHotkeyEnabled)
	{
		MessageBoxW(0, L"MouseTrap was unable to reserve the Ctrl+Alt+T key combination for itself. You will need to press the Stop button manually to stop recording.", L"Problem", MB_ICONEXCLAMATION);
	}
	iccx.dwICC = ICC_STANDARD_CLASSES;
	iccx.dwSize = sizeof(INITCOMMONCONTROLSEX);
	InitCommonControlsEx(&iccx);

	hRecord = CreateWindowW(L"BUTTON", L"Record", WS_VISIBLE | WS_CHILD | BS_LEFTTEXT, 0, 0, 130, 30, hWnd, NULL, hInstance, NULL);
	hPlay = CreateWindowW(L"BUTTON", L"Play", WS_VISIBLE | WS_CHILD | BS_LEFTTEXT | WS_DISABLED, 130, 0, 130, 30, hWnd, NULL, hInstance, NULL);
	hStop = CreateWindowW(L"BUTTON", L"Stop", WS_VISIBLE | WS_CHILD | BS_LEFTTEXT | WS_DISABLED, 260, 0, 130, 30, hWnd, NULL, hInstance, NULL);
	hDebugInfo = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"Welcome to MouseTrap v1.0!\r\nRead the Manual for usage instructions.\r\n\r\nMouseTrap is completely free software with the source code available at http://github.com/govindparmar/MouseTrap.  If you like it, donate to me!\r\n", WS_VISIBLE | WS_CHILD | ES_READONLY | ES_MULTILINE, 0, 30, 380, 200, hWnd, NULL, hInstance, NULL);

	return TRUE;
}
