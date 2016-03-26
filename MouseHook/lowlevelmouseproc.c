#include "mousehook.h"
	
int g_mpLastX;
int g_mpLastY;

// Low-level mouse hooking procedure.
// 
// PARAMETERS:
//  nCode is used to determine whether the message should be processed or passed to the next system hook.
//  wParam identifies the type of mouse message (movement, left click, right click, etc...).
//  lParam points to an MSLLHOOKSTRUCT structure that contains information including the screen coordinates of the mouse activity.
//
// RETURNS:
//  The value returned by CallNextHookEx().
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	// If nCode is less than zero, the hook procedure must return CallNextHookEx right away without further processing.
	if (nCode < 0)
	{
		goto cleanup0;
	}

	// Otherwise, the hook procedure may process the message.
	else if (nCode == HC_ACTION)
	{
		// Case 1: Left or right click
		if (wParam == WM_LBUTTONDOWN  || wParam == WM_RBUTTONDOWN)
		{
			MSLLHOOKSTRUCT msllhs;
			MOUSEINFO mi;
			DWORD dwWritten = 0;
			DWORD tickCount = GetTickCount();
			
			// Determine the X- and Y-coordinates of the click and copy those into the MOUSEINFO struct
			msllhs = *((MSLLHOOKSTRUCT*)lParam);

			mi.x = msllhs.pt.x;
			mi.y = msllhs.pt.y;

			// Set the 'click' field to 1 (true)
			mi.click = 1;

			// Set the 'waitms' field to the current value returned by GetTickCount() minus the value stored in g_lastGTC (which is the previous wait time)
			mi.waitms = tickCount - g_lastGTC;
			// Update the previous wait time (for the next call)
			g_lastGTC = tickCount;

			// Set the 'left' field according to whether it was a left or right click.
			switch (wParam)
			{
				case WM_LBUTTONDOWN:
					mi.left = 1;
					break;
				
				case WM_RBUTTONDOWN:
					mi.left = 0;
					break;
			}

			// Write the MOUSEINFO struct into the file
			SetFilePointer(g_hFile, 0, 0, FILE_END);
			WriteFile(g_hFile, &mi, sizeof(struct _MOUSEINFO), &dwWritten, NULL);

			// Update the last X- and Y- coordinate global variables
			g_mpLastX = mi.x;
			g_mpLastY = mi.y;
		}

		// Case 2: This is a mouse movement event
		else if (wParam == WM_MOUSEMOVE) 
		{
			MSLLHOOKSTRUCT msllhs;
			MOUSEINFO mi;
			DWORD dwWritten = 0;
			DWORD tickCount = GetTickCount();

			msllhs = *((MSLLHOOKSTRUCT*)lParam);

			// Determine if the current mouse position is different enough from the last recorded mouse position to register as a "mouse move" event; if not, exit out
			if ((abs(g_mpLastX - msllhs.pt.x) <= MOUSEMOVE_DETECT_THRESHOLD) && (abs(g_mpLastY - msllhs.pt.y) <= MOUSEMOVE_DETECT_THRESHOLD))
			{
				goto cleanup0;
			}

			// Otherwise populate the MOUSEINFO struct:
			// 'click' is 0 indicating that this is a movement event
			mi.click = 0;

			// Same deal as previous case with the waitms/g_lastGTC
			mi.waitms = tickCount - g_lastGTC;
			g_lastGTC = tickCount;

			// And X- and Y- coordinates
			mi.x = msllhs.pt.x;
			mi.y = msllhs.pt.y;

			// Technically unnecessary but good practice to be consistent
			mi.left = 0;

			// Perform the write
			SetFilePointer(g_hFile, 0, 0, FILE_END);
			WriteFile(g_hFile, &mi, sizeof(struct _MOUSEINFO), &dwWritten, NULL);

			// Update last X- and Y-coordinate records
			g_mpLastX = mi.x;
			g_mpLastY = mi.y;

		}
	}
cleanup0:
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}