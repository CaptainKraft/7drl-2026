#pragma once

typedef enum {
    CR_TOP,
    CR_BOT,
    CR_LEFT,
    CR_RIGHT,
} CR_SIDE;

typedef struct {
    float min_x, min_y, max_x, max_y;
} CR_Rect;

typedef struct {
    CR_Rect *rect;
    CR_SIDE side;
} CR_Cut;

CR_Rect cr_left(CR_Rect *rect, float amt);
CR_Rect cr_right(CR_Rect *rect, float amt);
CR_Rect cr_top(CR_Rect *rect, float amt);
CR_Rect cr_bot(CR_Rect *rect, float amt);
CR_Rect cr_cut(CR_Cut cut, float amt);
void cr_shrink(CR_Rect *rect, float amt);
void cr_shrink_x(CR_Rect *rect, float amt);
void cr_shrink_y(CR_Rect *rect, float amt);
