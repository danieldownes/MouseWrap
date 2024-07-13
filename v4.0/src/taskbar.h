#ifndef TASKBAR_H
#define TASKBAR_H

#include <windows.h>
#include <shellapi.h>
#include "resource.h"
#include "main.h"
#include "mouse_wrap.h"

extern NOTIFYICONDATA nid;

void CreateTrayIcon(HWND hwnd, HINSTANCE hInst);
void ShowContextMenu(HWND hwnd);
void CreateContextMenu(void);
void TaskBarCheckCommand(WORD cmd);
void TaskBarCheckClick(LPARAM lParam, HWND hwnd);
void IconClicked(HWND hwnd);
void CleanUpTrayIcon(void);

#endif // TASKBAR_H