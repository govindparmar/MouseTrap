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
	WNDCLASSEXW wcex;
	ZeroMemory(&wcex, sizeof(WNDCLASSEXW));

	wcex.cbSize = sizeof(WNDCLASSEXW);
	wcex.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wcex.hCursor = LoadCursorW(NULL, IDC_ARROW);
	wcex.hIcon =
	wcex.hIconSm = LoadIconW(NULL, IDI_APPLICATION);
	wcex.hInstance = hInstance;
	wcex.lpfnWndProc = WindowProc;
	wcex.lpszClassName = g_szClassName;
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDR_MENU1);

	return RegisterClassExW(&wcex);

}