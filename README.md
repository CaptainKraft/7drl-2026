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

## Pathfinding and AI

Turn-based movement and target selection are driven by straightforward breadth-first path searches on walkable floor cells.

Current implementation:

- Units query path distances on demand from the current board state instead of relying on persistent global distance maps.
- Enemy turns choose the nearest reachable target (player or summoned ally) and move one cardinal step that reduces distance.
- Familiar target choice prioritizes enemies that most recently damaged the player, then enemies closer to the player, then enemies with lower health.
- Tie-breaks are deterministic, so repeated seeded runs produce consistent turn behavior.

## Debug controls

Dungeon developer-only controls are centralized as data in `src/game.c`:

- Press the grave key to show/hide the debug HUD in debug builds.
- Every debug action and feature toggle is a clickable HUD button (no dedicated debug hotkeys).
- Feature toggles in `game_debug_feature_defs` default to off so debug startup matches release visuals.

Release safety:

- Debug features are compiled behind `GAME_DEBUG_FEATURES` (`defined(DEBUG)`, unless `GAME_DISABLE_DEBUG_FEATURES` is set).
- `make build` uses `-DDEBUG` (developer build).
- `make release` uses `-DNDEBUG`, and `make web` uses `-DGAME_DISABLE_DEBUG_FEATURES`, so players cannot access debug controls.
