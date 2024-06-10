#ifndef MAIN_H
#define MAIN_H

#include <windows.h>

// Global variables
#define CLASS_NAME "Sample Window Class"
#define WINDOW_TITLE "Sample Window"
#define WM_TRAYICON (WM_USER + 1)
#define IDT_TIMER1 1 // Timer ID for mouse wrap

extern NOTIFYICONDATA nid;
extern HINSTANCE hInst;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif // MAIN_H
