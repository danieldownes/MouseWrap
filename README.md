# Mouse Wrap: Seamless Desktop Navigation

**Transform your Windows desktop experience with infinite mouse movement**

Mouse Wrap is a lightweight utility that revolutionizes how you navigate your Windows desktop by eliminating screen boundaries. Instead of your mouse cursor getting trapped at screen edges, Mouse Wrap creates a seamless, wraparound experience where moving off one edge instantly transports your cursor to the opposite side, and even supports any multi-monitor setup.

## How It Works

When your mouse pointer reaches any screen edge, it seamlessly wraps to the corresponding position on the opposite side:
- Move off the right edge → appear on the left
- Move off the left edge → appear on the right  
- Move off the top edge → appear at the bottom
- Move off the bottom edge → appear at the top

This creates an infinite desktop experience where distance becomes irrelevant.

## Key Benefits

**Speed & Efficiency**: Navigate faster across large screens and multi-monitor setups without lengthy cursor journeys

**Ergonomic Advantage**: Reduce wrist strain and repetitive movements, especially beneficial for extended computer use

**Professional Productivity**: Perfect for designers, developers, and power users working with multiple applications and large displays

**Gaming Enhancement**: Ideal for strategy games and applications requiring frequent screen-edge interactions

## Perfect For

- **Large Display Users**: 4K, ultrawide, and multi-monitor configurations
- **Creative Professionals**: Graphic designers, video editors, and digital artists
- **Power Users**: Anyone who values efficient desktop navigation
- **Accessibility**: Users seeking reduced physical strain during computer use

## A Quarter-Century of Excellence

Mouse Wrap has been trusted by users worldwide for **25 years**, continuously refined and maintained to deliver reliable performance across Windows versions. This longevity speaks to its essential utility and dedicated development.

## System Requirements

- Windows operating system
- Minimal system resources (runs efficiently in the background)
- Compatible with all screen resolutions and multi-monitor configurations

---

**Love what Mouse Wrap does for your productivity?** Support continued development with a coffee! Your contribution helps maintain this beloved utility that thousands rely on daily.

[☕ Buy Me a Coffee](https://buymeacoffee.com/danieldownes/mouse-wrap-4)  
[❤️ Sponsor me on GitHub (one-time or monthly)](https://github.com/sponsors/danieldownes)

*I make this for fun, with love and for my own usage. However I get a kick out of every contribution, no matter the size. Your contribution is deeply appreciated and directly supports ongoing development and maintenance.*

# Latest Download

[Mouse Wrap 4 is release](https://github.com/danieldownes/MouseWrap/releases/tag/v4.0) and supports

 - Multi-monitor aware mouse wrapping.
 - Theme aware traybar icon.
 - No install and portable (depending on vc_redist).
 - 56.5 KB download.


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
 [x] 7. Multi-monitor support.
 [ ] 8. Linux support.
 [ ] 9. Mac support.

## Multi-monitor support

This has something that has been requested during the lifetime of MouseWrap. Although now less common due to Ultra-wide screens, it is worthy addition.

It also presents somewhat of a fun challeging and for the sake of interest, the use of Gen AI is further used here to work on the high level logic in a high-level language C#, rather than C. This code will then be convert back by AI to C. But as I find it more familier to use C# for solving this logic problem, this is an interesting exercise.

The multi-monitor problem is interesting in itself too. Considering 2 or more screens, we obtain a list of virtual desktop resolutions, with the primary screen acting as the ancor for the coordinate system. With this list of adjoining rectangles, we then need to find the contour of this shape.

With a lot of GPT prod/prompt-ing, and the addition of a NUnit C# Test project to the solution, we have the logic, in C# to obtain a list of edges for our mouse detection process.