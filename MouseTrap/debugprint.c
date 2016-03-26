#include "mousetrap.h"

// Sets the contents of the edit control in the main application window.
//
// PARAMETERS:
// hWnd is the handle to the main application window, *not* the edit control
// newMsg is a wide-character string specifying the new contents of the edit control
// cchNewMsg is the length of the string newMsg in characters including the null-terminator.
//
// RETURNS:
// The difference in characters between the previous and current message.
//
// REMARKS:
// cchNewMsg and the return value presently are not used by the program in any way, but may be as the program is updated.
// Future versions of the program will probably include options for appending to the debug text instead of replacing it.

int DebugPrint(HWND hWnd, WCHAR *newMsg, int cchNewMsg)
{
	HWND hEdit = FindWindowEx(hWnd, NULL, L"EDIT", NULL);
	int prevLen = GetWindowTextLength(hEdit);
	SetWindowText(hEdit, newMsg);
	UpdateWindow(hEdit);
	return (cchNewMsg - prevLen);
}