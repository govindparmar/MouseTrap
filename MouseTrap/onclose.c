#include "mousetrap.h"
// Message-cracker method for the WM_CLOSE message
// 
// If the playback thread is still running, terminate it, then send WM_DESTROY.
VOID WINAPI OnClose(HWND hWnd)
{
	if (g_hThread != INVALID_HANDLE_VALUE) TerminateThread(g_hThread, 0L);
	DestroyWindow(hWnd);
}