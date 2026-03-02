#ifndef STARTUP_H
#define STARTUP_H

#include <windows.h>

void AddToStartup();
void RemoveFromStartup();
BOOL IsStartupKeySet();
void UpdateStartupPathIfMoved();

#endif // STARTUP_H
