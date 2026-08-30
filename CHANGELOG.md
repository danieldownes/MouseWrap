# Mouse Wrap — Change Log

## Version 4.2 (2026)

### Wrapping behaves better

- **No more wrapping at the taskbar.** The line where the desktop meets the taskbar used to count as a screen edge, so the pointer jumped to the other side when you rested it against the taskbar. It now stays put; only the real outer edges of the screen wrap.
- **Every edge triggers at the same point.** The left and top edges wrapped one pixel before the pointer actually reached the edge, while right and bottom were exact. All four sides now wrap only when the pointer is on the very last pixel, and after a wrap the pointer always lands 5 px in from the opposite edge.

### New: "While dragging" setting

A dropdown in Options controls what happens while you hold the mouse button (dragging a window, selecting text, and so on):

| Choice | Effect while the button is held |
|---|---|
| **Delayed Wrap** (default) | Edges pause briefly before wrapping, so you can drop things near an edge without an accidental jump |
| **Wrap instantly** | Same as normal movement |
| **No Wrap** | Nothing wraps until you let go |

This setting takes precedence over each edge's own Wrap/Delayed colour. Edges set to **No Wrap** stay off no matter what. Hover the label for a reminder.

### Your edge settings survive monitor changes

If a monitor turns off, sleeps, or you change resolution, Mouse Wrap now keeps a separate set of edge settings for each monitor arrangement. When the original arrangement comes back, your settings come back with it — nothing is lost. Existing settings are carried over automatically the first time you run 4.2.

### Options window refreshed

- Modern themed controls; proper light and dark mode that switches live with Windows; Mica title bar on Windows 11.
- Sharp on any display, including mixed-DPI multi-monitor setups.
- Smoother monitor preview: rounded monitors, clean edge lines, a highlight and hand cursor when you hover an edge, and no flicker.
- Windows 11-style delay slider and a dropdown that matches the theme.

Still one small file: about 95 KB, no installer, no runtime to download.

### Housekeeping

- Support links now go to Ko-fi (tray menu, README, and the update-check page).
- README states Windows 10 or later and drops the outdated "needs vc_redist" note.

## Version 4.1 (2026)

- **Three-state edges.** Click any edge in the Options preview to set it to Wrap (green), Delayed (yellow) or No Wrap (red).
- **Delayed wrapping.** The pointer pauses at a Delayed edge before wrapping, giving a little resistance where you want it.
- **Delay slider.** Choose the pause from 200 ms to 1000 ms in 100 ms steps.
- **Live cursor dot** in the monitor preview, and a colour key explaining the three edge states.
- Edge states and the delay are remembered between runs.

## Version 4.0 (2026)

- **True multi-monitor wrapping.** Wraps around the real outer boundary of any monitor arrangement, including non-rectangular layouts.
- Tray icon follows the Windows light/dark theme.
- Per-monitor DPI awareness for sharp rendering on mixed-DPI setups.
- Only one copy can run at a time.
- Optional "wrap within workspace" mode that excludes taskbars.
- Rewritten in plain C for a smaller footprint and a simpler build.

## Version 2.04 (12 Jan 2005)

Windows keeps the pointer inside the work area while you drag or resize a window, so wrapping used to fail at the taskbar edge. 2.04 detects a drag or resize reaching the taskbar and wraps anyway, while staying light on resources. There is a very slight delay when wrapping from the edge of the taskbar, because that check is done sparingly. Mouse Wrap respects the size and position of your taskbar.

## Version 2.03 (25 Aug 2004)

- More meaningful error messages.
- Non-critical errors that Mouse Wrap can resolve itself are no longer shown.

## Version 2.02 (28 Jul 2003)

- Installer removes leftovers from Mouse Wrap v1.

## Version 2.01 (17 Jun 2003)

- Slightly smaller download.
- Minor improvements to the help file.
- Donate prompt text is no longer editable.

## Version 2.00 (13 Jun 2003)

- **Movement pattern recognition:** rapid left-and-right or back-and-forth motion triggers Cut, Copy, Paste, and Forward/Back navigation in Windows Explorer and Internet Explorer.
- Options window.
- Lower memory and CPU usage; smaller download; redesigned icons.
- Fixed: now adjusts automatically when the screen resolution changes.
- Fixed: multi-user environments no longer reported as an error.
- Fixed: the tray pop-up menu closes when you click elsewhere; better tray icon management.

## Version 1.00 (1 Dec 2001)

- Wraps the mouse pointer around the screen edges.
- Automatic update check.

Known issues at the time: did not adjust to resolution changes, problems with Windows XP user accounts, the tray pop-up menu did not close when clicking away, and no multi-monitor support.
