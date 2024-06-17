#ifndef TASKBAR_H
#define TASKBAR_H

#include <windows.h>
#include "resource.h"
#include <shellapi.h>
#include <string.h>

void CreateTrayIcon(HWND hwnd, HINSTANCE hInst);
void ShowContextMenu(HWND hwnd);
void CreateContextMenu();
void CleanUpTrayIcon();

#endif // TASKBAR_H
