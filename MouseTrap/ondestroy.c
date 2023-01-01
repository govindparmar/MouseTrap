#include "mousetrap.h"

// Message-cracker for the WM_DESTROY message.
//
// PARAMETERS:
//  hWnd is the window that receives the message.
VOID WINAPI OnDestroy(HWND hWnd)
{
	PostQuitMessage(ERROR_SUCCESS);
}
