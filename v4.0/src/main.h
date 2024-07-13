#ifndef MAIN_H
#define MAIN_H

#include <windows.h>
#include <stdio.h>
#include "resource.h"
#include "mouse_wrap.h"
#include "taskbar.h"
#include "startup.h"

#define CLASS_NAME "Mouse Wrap"
#define WINDOW_TITLE "Mouse Wrap"
#define TITLE_VERSION WINDOW_TITLE " " PRODVER_STR

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define WPRODVER_STR WIDEN(PRODVER_STR)

extern HINSTANCE hInst;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

BOOL CheckAlreadyRunning();

void TaskBarCheckClick(LPARAM lParam, HWND hwnd);

#endif // MAIN_H
