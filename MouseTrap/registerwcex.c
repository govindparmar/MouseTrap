#include "mousetrap.h"

// Registers the window class for the main application window.
//
// PARAMETERS:
//  hInstance is the value to populate the WNDCLASSEX::hInstance field with
//
// RETURNS:
//  An ATOM representing the newly registered window class. If it was not possible to register the window class, the return value will be <= 0.
ATOM RegisterWCEX(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(WNDCLASSEX));

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hIcon =
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hInstance = hInstance;
	wcex.lpfnWndProc = WindowProc;
	wcex.lpszClassName = g_szClassName;
	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);

	return RegisterClassEx(&wcex);

}