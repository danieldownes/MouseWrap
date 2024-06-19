Mouse Wrap is a small utility to enhance the mobility of the Windows mouse pointer. It does this by wrapping the movement of the mouse pointer around the edges of the screen. When the pointer touches an edge of the screen it will instantly appear on the opposite side of the screen as shown on the animation.

This edgeless experience in a desktop environment allows for faster interaction, especially when larger screen resolutions are involved.

The project has been running for 25 years! I love coffee, pledge support via [Buy Me a Coffee](https://buymeacoffee.com/danieldownes/mouse-wrap-4) and your contribution will be very much appreciated.

# Latest alpha: Version 4.0a1
Size: 63kb (requires VSControl lib), working to remove this
Only single Desktop, working to allow multiple desktop, with different resolution handling

# Latest Release: Version 3.3
Size: 1.1Mb
Version: 3.3 (4th Feb. 2013)
Windows XP and beyond
Linux Wine 1.1 or higher


# Mouse Wrap 4

## Goals:
 - Using pure Win32 C API to produce the most effient and highly optimised version of this program.
 - Release under GNU Version License.
 - Single portable exe.
 - Code is being generated using latest tooling.
 - Automated Testing.
 - Automated builds.


## Roadmap:
 [x] 1. Basic mouse wrapping on a single desktop.
 [x] 2. Edge case, consider task bar placement while dragging windows. 
 [x] 3. Tray icon with toggle control.
 [x] 4. Add/Remove to Windows Startup support (without installer).
 [x] 5. Ensure single-instance only can run.
 [ ] 6. Text-based option file.
 [ ] 7. Multi-monitor support.
 [ ] 8. (Planning: Visual Option window using WinUI 3.)


## Multi-monitor support

This has something that has been requested during the lifetime of MouseWrap. Although now less common due to Ultra-wide screens, it is worthy addition.
It also presents somewhat of a fun challeging and for the sake of interest, the use of Gen AI is further used here to work on the high level logic in a high-level language C#, rather than C. This code will then be convert back by AI to C. But as I find it more familier to use C# for solving this logic problem, this is an interesting exercise.

The multi-monitor problem is interesting in itself too. Considering 2 or more screens, we obtain a list of virtual desktop resolutions, with the primary screen acting as the ancor for the coordinate system. With this list of adjoining rectangles, we then need to find the contour of this shape.

With a lot of GPT prod/prompt-ing, and the addition of a NUnit C# Test project to the solution, we have the logic, in C# to obtain a list of edges for our mouse detection process.