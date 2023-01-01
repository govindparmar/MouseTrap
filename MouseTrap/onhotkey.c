#include "mousetrap.h"

// Message-cracker for the WM_HOTKEY message.
// 
// PARAMETERS:
//  hWnd is the window that received the message (the main application window)
//  nID is unused by this message handler.
//  nModify specifies the modifier keys (CTRL, ALT, SHIFT, etc.) that accompany the base key
//  vKey is the base key (A, B, C...) for the hotkey.
VOID WINAPI OnHotkey(HWND hWnd, INT nID, UINT nModify, UINT vKey)
{
	// The only hotkey this program is concerned with is CTRL+ALT+T
	if (nModify == (MOD_ALT | MOD_CONTROL) && vKey == 'T')
	{
		// If the "currently recording" flag is set, simulate a press of the "Stop" button to initiate the save dialog.
		// Otherwise, do nothing.
		if (g_bCurrentlyRecording)
		{
			HWND hStop;
			hStop = FindWindowExW(hWnd, NULL, L"BUTTON", NULL);
			hStop = FindWindowExW(hWnd, hStop, L"BUTTON", NULL);
			hStop = FindWindowExW(hWnd, hStop, L"BUTTON", NULL);
			SendMessageW(hWnd, WM_COMMAND, 0, (LPARAM)hStop);
		}
	}
}
