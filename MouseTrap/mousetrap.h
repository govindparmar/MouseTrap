#pragma once
#include <Windows.h>
#include <WindowsX.h>
#include <stdlib.h>
#include <stdio.h>
#include <strsafe.h>
#include "resource.h"

// Coerces four 8-bit bytes into a 32-bit doubleword (little-endian)
#define FOURCH(c0, c1, c2, c3) \
	((unsigned __int32) (unsigned __int8) (c0)		  | ((unsigned __int32) (unsigned __int8) (c1) << 8) | \
	((unsigned __int32) (unsigned __int8) (c2) << 16) | ((unsigned __int32) (unsigned __int8) (c3) << 24))

// CreateDirectory can only support 248 characters according to documentation
#define APPPATH_DIR_LENGTH 248

// Uniquely identify the Alt+S (stop recording) shortcut
#define IDH_HOTKEY1 1001

// Disable padding in structs to minimize space in the MTP files
#pragma pack(push, 1)

// File header
typedef struct _MIFILEHDR
{
	unsigned __int32 magic; // Magic number; identifies as valid file (0x'G''P''M''P')
	unsigned __int32 numMI; // Number of MOUSEINFO structs in the file
	unsigned __int16 version; // 0x00000001 for this version
} MIFILEHDR;

// Information about a mouse click event
typedef struct _MOUSEINFO
{
	unsigned __int32 waitms; // How long to wait before performing this event
	unsigned __int32 x; // X-coordinate of the mouse event
	unsigned __int32 y; // Y-coordinate of the mouse event
	unsigned __int8 left; // 1 = Left button, 0 = Right button
	unsigned __int8 click; // 1 = Click, 0 = Movement
} MOUSEINFO;

// Restore the default padding setting
#pragma pack(pop)

// Global variables:
extern const WCHAR g_szClassName[]; // Class name for the main application window
extern MOUSEINFO *g_miFromFile; // Dynamic memory array of MOUSEINFO structs
extern WCHAR g_szClickData[MAX_PATH]; // Full path to the clickdata.tmp file
extern HANDLE g_hThread; // Handle to the ThreadProc() thread
extern BOOL g_bHotkeyEnabled; // Flag indicating whether the Ctrl+Alt+T hotkey was able to be reserved by MouseTrap or not
extern BOOL g_bCurrentlyRecording; // Flag indicating whether or not a recording is currently in progress

// Methods:
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd);
LRESULT CALLBACK WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
ATOM RegisterWCEX(HINSTANCE hInstance);
BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam);
int DebugPrint(HWND hWnd, WCHAR *newMsg, int cchNewMsg);
int LoadMIStructsFromFile(WCHAR *inFileName, WCHAR *outFileName);
BOOL CALLBACK DialogProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
DWORD WINAPI ThreadProc(LPVOID lpParam);
VOID PopulateOFN(HWND hWnd, OPENFILENAME *ofn, WCHAR *szFileName, BOOL bOpenOrSave);
VOID WINAPI OnCommand(HWND hWnd, int nID, HWND hwSource, UINT uNotify);
BOOL WINAPI OnCreate(HWND hWnd, LPCREATESTRUCT lpcs);
VOID WINAPI OnHotkey(HWND hWnd, INT nID, UINT nModify, UINT vKey);
VOID WINAPI OnClose(HWND hWnd);
VOID WINAPI OnDestroy(HWND hWnd);