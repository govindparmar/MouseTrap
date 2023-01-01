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
	g_bHotkeyEnabled = RegisterHotKey(hWnd, IDH_HOTKEY1, MOD_ALT | MOD_CONTROL, 'T');
	if (!g_bHotkeyEnabled)
	{
		// NOTE: DebugPrint is inappropriate here because the window hasn't been created yet!
		MessageBoxW(0, L"MouseTrap was unable to reserve the Ctrl+Alt+T key combination for itself. You will need to press the Stop button manually to stop recording.", L"Problem", MB_ICONEXCLAMATION);
	}
	iccx.dwICC = ICC_STANDARD_CLASSES;
	iccx.dwSize = sizeof(INITCOMMONCONTROLSEX);
	InitCommonControlsEx(&iccx);
	return TRUE;
}
