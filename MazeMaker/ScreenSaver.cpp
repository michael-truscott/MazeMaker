#include <Windows.h>
#include <ScrnSave.h>
#include "main.h"
#include "Util.h"

const int TIMER = 1;

LRESULT WINAPI ScreenSaverProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
	switch (message) {
	case WM_CREATE:
		if (!Init((void*)(hWnd)))
		{
			PostQuitMessage(0);
			return 0;
		}
		SetTimer(hWnd, TIMER, (int)(TIME_STEP * 1000), NULL);
		return 0;
	case WM_DESTROY:
		Shutdown();
		return 0;
	case WM_TIMER:
		Update(TIME_STEP);
		return 0;
	}

	return DefScreenSaverProc(hWnd, message, wParam, lParam);
}

BOOL WINAPI ScreenSaverConfigureDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}

BOOL WINAPI RegisterDialogClasses(HANDLE hInst) {
	return TRUE;
}