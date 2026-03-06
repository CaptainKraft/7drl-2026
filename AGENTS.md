### Testing

Since the system for testing is so straightforward, there is no need to mention testing strategies in your responses.

### Memory Management

Use arena allocators exclusively:
```c
Arena *arena = arena_alloc(MB(16));
void *memory = arena_push(arena, size);
arena_clear(arena);  // Reset arena
arena_release(arena); // Free arena
```

- Prefer fixed-capacity arrays or stack storage; memory management should be rare.
- Do not introduce new arena usage (`Mem`, `arena_push`, etc.) without explicit user confirmation.
- Do not introduce new heap allocations or new stb_ds containers without explicit user
  confirmation.
- No explicit `malloc`/`calloc`/`realloc`/`free` calls in project code.
- stb may allocate internally, but do not call stb allocation helpers directly.
- Any change that touches memory handling or allocation patterns must be called out and
  confirmed before proceeding.

### Random Generation
- Use only `ck_rng` (`RNG`, `ck_rand`, `ck_rand_int`, `ck_rand_float`). Do not use
  Raylib random helpers or `rand()`.
- All randomness must be deterministic and derived from an explicit seed to support seeded
  runs.
- When adding a new randomness purpose, add a new RNG stream in `Game` and seed it
  deterministically before use.

## Important Notes

- **Platform APIs**: Prefer Raylib helpers (file, path, timing, etc.) where possible; if a needed
  helper is missing, ask before using platform-specific APIs so we can pick a cross-platform
  approach.
- **File IO**: Use Raylib file helpers (LoadFileText/SaveFileText/FileExists/etc.) instead of stdio.
- **Graphics**: Raylib with custom framebuffer rendering
- **UI Shapes**: Do not use rounded corners anywhere; all UI panels, cards, and buttons should be rectangular.
- **Pixel Art Scale**: Keep visuals at a consistent pixel-art scale. Use integer scale multiples of source art and avoid subpixel placement/scaling for sprites.
- **Frame rate**: Fixed timestep at 60 FPS
- **Data format**: Do not use JSON anywhere in this project
