#pragma once

#include "microui/microui.h"
#include "ray/raylib.h"

typedef struct {
    Font font;
    int font_size;
    float font_spacing;
    float scale;
    float offset_x;
    float offset_y;
} Microui_Raylib;

void microui_raylib_init(Microui_Raylib *ui, mu_Context *ctx);
void microui_raylib_set_viewport(Microui_Raylib *ui, float scale, float offset_x, float offset_y);
void microui_raylib_draw(Microui_Raylib *ui, mu_Context *ctx);
