#ifndef TASKBAR_H
#define TASKBAR_H

#include <windows.h>
#include "resource.h"
#include <shellapi.h>
#include <string.h>
#include "main.h"

extern NOTIFYICONDATA nid;

void CreateTrayIcon(HWND hwnd, HINSTANCE hInst);
void ShowContextMenu(HWND hwnd);
void CreateContextMenu();
void TaskBarCheckCommand(WORD cmd);
void TaskBarCheckClick(LPARAM lParam, HWND hwnd);
void IconClicked();
void CleanUpTrayIcon();

#endif // TASKBAR_H
