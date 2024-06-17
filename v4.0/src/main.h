#ifndef MAIN_H
#define MAIN_H

#include <windows.h>
#include "resource.h"
#include "mouse_wrap.h"
#include "taskbar.h"

#define CLASS_NAME "Mouse Wrap"
#define WINDOW_TITLE "Mouse Wrap"
#define IDT_TIMER1 1

extern HINSTANCE hInst;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void TaskBarCheckClick(LPARAM lParam, HWND hwnd);

#endif // MAIN_H
