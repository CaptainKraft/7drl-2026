#pragma once

#include <stdint.h>

// LINKED LISTS

#define sll_queue_push(f, l, n)                                                                    \
    ((f) == 0 ? (f) = (l) = (n) : ((l)->next = (n), (l) = (n)), (n)->next = 0)
#define sll_queue_push_front(f, l, n)                                                              \
    ((f) == 0 ? ((f) = (l) = (n), (n)->next = 0) : ((n)->next = (f), (f) = (n)))
#define sll_queue_pop(f, l) ((f) == (l) ? (f) = (l) = 0 : (f) = (f)->next)

// ARENAS

typedef struct {
    uint64_t pos;
    uint64_t cap;
} Arena;

Arena *arena(uint8_t *mem, uint64_t size);
Arena *arena_alloc(uint64_t cap);
void arena_release(Arena *arena);

#define arena_start(arena, type) ((type *)(((uint8_t *)(arena)) + sizeof(Arena)));

void *arena_push(Arena *arena, uint64_t size);
void arena_pop(Arena *arena, uint64_t size);
void arena_clear(Arena *arena);
