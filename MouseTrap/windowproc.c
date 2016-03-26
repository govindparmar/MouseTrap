#include "mousetrap.h"

HANDLE g_hThread = INVALID_HANDLE_VALUE;
HANDLE g_hMonitor = INVALID_HANDLE_VALUE;
BOOL g_bHotkeyEnabled = FALSE;
BOOL g_bCurrentlyRecording = FALSE;
BOOL g_bCurrentlyPlaying = FALSE;


// Callback procedure for handling messages posted to the main application window.
// 
// PARAMETERS:
//  hWnd is the window that the message was posted to (the main application window).
//  Msg identifies the type of message.
//  wParam varies in meaning depending on the message.
//  lParam varies in meaning depending on the message.
//
// RETURN:
//  The meaning of the return value varies depending on the message being processed.
//
// REMARKS:
//  The user-defined code for message handling is found in the various message crackers (OnXxx()); this method is a stub that invokes the appropriate cracker or DefWindowProc.
LRESULT CALLBACK WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
		HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_HOTKEY, OnHotkey);
		HANDLE_MSG(hWnd, WM_CLOSE, OnClose);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
	default:
		return DefWindowProc(hWnd, Msg, wParam, lParam);
	}
	return 0;
}