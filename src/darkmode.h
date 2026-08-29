#ifndef DARKMODE_H
#define DARKMODE_H

#include <windows.h>

// Dark-mode support for the app's windows.
//
// Everything that relies on undocumented uxtheme.dll ordinals lives here and
// degrades gracefully: if an ordinal is missing the call is skipped and the
// window simply stays light.  The documented parts (DWM immersive dark title
// bar, SetWindowTheme) are used wherever they exist.

// Call once at startup, before any window is created.  Tells uxtheme the app
// is dark-mode aware so themed controls can be switched to their dark variants.
void DarkMode_Init(void);

// TRUE when Windows "Choose your default app mode" is set to Dark.
BOOL DarkMode_IsEnabled(void);

// Apply light/dark to a top-level window: dark title bar + window frame, and
// permission for its themed children to use dark visuals.
void DarkMode_ApplyToWindow(HWND hwnd, BOOL dark);

// Apply light/dark visuals to a single child control based on its class
// (button, combobox, trackbar, tooltip, scrollbar...).
void DarkMode_ApplyToControl(HWND hCtl, BOOL dark);

#endif // DARKMODE_H
