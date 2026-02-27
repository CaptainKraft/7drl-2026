#pragma once

#include "platform.h"

void game_init(Mem mem, Font font, float font_spacing);
void game_shutdown(Mem mem);
void game_update(Mem mem);
void game_render(Mem mem);
