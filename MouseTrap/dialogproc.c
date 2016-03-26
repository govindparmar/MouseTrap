#include "mousetrap.h"

// Callback procedure for the "About" dialog box.
// 
// RETURN:
// TRUE if the message was processed by the callback function; FALSE if the message should be processed by the window manager's default message handler
// 
// REMARKS:
// This dialog box does nothing but display information about the app and close when the user presses the "OK" button.
BOOL CALLBACK DialogProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_INITDIALOG:
		return TRUE;
	
	case WM_COMMAND:
		// Source of the command was the OK button
		if (LOWORD(wParam) == IDOK)
		{
			EndDialog(hWnd, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}