#pragma once
#include <Windows.h>
#include <strsafe.h>
#include <stdlib.h>
#include <stdio.h>

// Number of pixels the mouse must move (in either direction) for it to be registered as a "mouse move" event by the recorder
// Decreasing this increases precision of the playback but also increases the size of the file, owing to the need to store more data
#define MOUSEMOVE_DETECT_THRESHOLD 3

// Disable alignment on adjacent data types
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

// Restore the default alignment option (/Zp8)
#pragma pack(pop)

// Global variables: 
extern int g_lastGTC; // The last millisecond wait time written into clickdata.tmp (used to determine relative times between mouse events)
extern int g_mpLastX; // The last X-coordinate written into clickdata.tmp (used to determine whether MOUSEMOVE_DETECT_THRESHOLD has been reached)
extern int g_mpLastY; // The last Y-coordinate written into clickdata.tmp (same as above)
extern HANDLE g_hFile; // Handle to clickdata.tmp

BOOL APIENTRY DllMain(HANDLE hInstDLL, DWORD dwReason, LPVOID lpReserved);
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);