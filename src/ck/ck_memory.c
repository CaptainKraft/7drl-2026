#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "ck_memory.h"

#ifndef max
#define max(x, y) (((x) > (y)) ? (x) : (y))
#endif

Arena *arena(uint8_t *mem, uint64_t size)
{
    uint64_t base = sizeof(Arena);
    Arena *arena = (Arena *)mem;
    arena->pos = 0;
    arena->cap = size - base;

    return arena;
}

Arena *arena_alloc(uint64_t cap)
{
    uint64_t base = sizeof(Arena);
    uint64_t total_cap = base + cap;
    Arena *arena = (Arena *)calloc(1, total_cap);
    arena->pos = 0;
    arena->cap = cap;

    return arena;
}

void arena_release(Arena *arena)
{
    free(arena);
}

void *arena_push(Arena *arena, uint64_t size)
{
    uint64_t base = sizeof(Arena);
    uint64_t new_pos = arena->pos + size;
    assert(new_pos <= arena->cap);
    arena->pos = new_pos;
    return (uint8_t *)arena + base + arena->pos - size;
}

void arena_pop(Arena *arena, uint64_t size)
{
    arena->pos = max(0, arena->pos - size);
}

void arena_clear(Arena *arena)
{
    arena->pos = 0;
}
