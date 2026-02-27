# 7drl 2026
Delve through the dungeon with your trusty pet at your side. Toss him around and he'll help you hack and slash your way through the depths.

## Raylib changes
We build raylib from source and copy the static libs into `lib/` during `make init`. Keep these local changes in the raylib sources so the build is reproducible:

1. Custom frame control
    - File: `<raylib-path>/raylib/src/config.h`
    - Change: set `#define SUPPORT_CUSTOM_FRAME_CONTROL 1`
    - Why: enables manual frame handling so we can lock the framerate
        - (we call `SwapScreenBuffer()` ourselves)

2. macOS resize DPI fix
    - File: `../raylib/src/platforms/rcore_desktop_glfw.c` in `WindowSizeCallback`
    - Change: skip the `GetWindowScaleDPI()` division on macOS:
        ```c
        if (IsWindowState(FLAG_WINDOW_HIGHDPI)) {
        #if !defined(__APPLE__)
            width = (int)(width / GetWindowScaleDPI().x);
            height = (int)(height / GetWindowScaleDPI().y);
        #endif
        }
        ```
    - Why: GLFW already reports logical size on macOS with HiDPI, so dividing again shrinks the viewport

After updating raylib, run `make init` to rebuild and copy `libraylib-mac.a` and `libraylib-web.a` into `lib/`
