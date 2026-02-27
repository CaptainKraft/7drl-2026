#pragma once

#include "ck/ck.h"
#include "ray/raylib.h"
#include "ray/raymath.h"

typedef struct {
    Arena *tmp, *perm;
} Mem;

void app_quit();
