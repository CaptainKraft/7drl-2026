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

## Dijkstra Maps (AI foundation)

We are using Brogue-style Dijkstra maps as the base for turn-based movement and enemy decision-making.

References:

- https://www.roguebasin.com/index.php/The_Incredible_Power_of_Dijkstra_Maps
- https://www.roguebasin.com/index.php/Dijkstra_Maps_Visualized

Current implementation:

- Build a Dijkstra distance field from the player position over walkable floor cells.
- Rebuild this field when the player successfully takes a move turn.
- Each enemy checks the four cardinal neighbors and steps to the lowest value ("roll downhill").
- Enemy turns are processed after the player turn, one-by-one, using a shared map for that turn.

Design notes for future features:

- The same map can drive any number of enemies, so scaling actor count is cheap.
- Multiple goals are supported naturally by seeding several cells as map sources.
- Weighted behavior comes from combining or offsetting maps (approach, flee, hazards, loot, objectives).
- Fleeing behavior can be produced with Brogue's rescan method (invert/scale, then rescan).

This keeps AI logic deterministic, simple to debug, and easy to extend into richer behaviors.

## Debug controls

Dungeon developer-only controls are centralized as data in `src/game.c`:

- Press the grave key to show/hide the debug HUD in debug builds.
- Every debug action and feature toggle is a clickable HUD button (no dedicated debug hotkeys).
- Feature toggles in `game_debug_feature_defs` default to off so debug startup matches release visuals.

Release safety:

- Debug features are compiled behind `GAME_DEBUG_FEATURES` (`!defined(NDEBUG)`).
- `make build` uses `-DDEBUG` (developer build).
- `make release` uses `-DNDEBUG`, which disables these debug controls so players cannot access them.
