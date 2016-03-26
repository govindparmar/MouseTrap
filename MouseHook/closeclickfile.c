#include "mousehook.h"

void __stdcall CloseClickDataFileHandle()
{
	SetFilePointer(g_hFile, 0, 0, FILE_BEGIN);
	SetEndOfFile(g_hFile);
	CloseHandle(g_hFile);
}