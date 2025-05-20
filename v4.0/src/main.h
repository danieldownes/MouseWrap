#ifndef MAIN_H
#define MAIN_H


#include <windows.h>
#include "resource.h"
#include "mouse_wrap.h"
#include "taskbar.h"
#include "startup.h"

#define CLASS_NAME L"Mouse Wrap"
#define WINDOW_TITLE L"Mouse Wrap"
#define TITLE_VERSION WINDOW_TITLE L" " WPRODVER_STR

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)

// Fallback for PRODVER_STR if not defined by the build system
#ifndef PRODVER_STR
#define PRODVER_STR "undefined"
#endif

#define WPRODVER_STR WIDEN(PRODVER_STR)

extern HINSTANCE hInst;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CheckAlreadyRunning(void);
void TaskBarCheckClick(LPARAM lParam, HWND hwnd);

#endif // MAIN_H