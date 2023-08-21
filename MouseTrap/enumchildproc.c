#include "mousetrap.h"

// Callback function for enumerating all the child windows in the main application window.
// Sets the font used by all the child windows/controls to the default GUI font, Tahoma.
// Without this, the child windows in the app use the default system font.
//
// RETURN:
// TRUE to continue processing child windows in the app (if the callback function ever returns FALSE, enumeration stops).
BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam)
{
	HFONT hfDefault = *(HFONT *) lParam;
	SendMessageW(hWnd, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(TRUE, 0));
	return TRUE;
}
