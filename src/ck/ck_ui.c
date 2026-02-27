#include "ck_ui.h"
#include "ck.h"

#include <stdbool.h>
#include <assert.h>

CR_Rect cr_left(CR_Rect *rect, float amt)
{
    float old_min_x = rect->min_x;
    rect->min_x = min(rect->min_x + amt, rect->max_x);

    return (CR_Rect){old_min_x, rect->min_y, rect->min_x, rect->max_y};
}

CR_Rect cr_right(CR_Rect *rect, float amt)
{
    float old_max_x = rect->max_x;
    rect->max_x = max(rect->min_x, rect->max_x - amt);

    return (CR_Rect){rect->max_x, rect->min_y, old_max_x, rect->max_y};
}

CR_Rect cr_top(CR_Rect *rect, float amt)
{
    float old_min_y = rect->min_y;
    rect->min_y = min(rect->min_y + amt, rect->max_y);

    return (CR_Rect){rect->min_x, old_min_y, rect->max_x, rect->min_y};
}

CR_Rect cr_bot(CR_Rect *rect, float amt)
{
    float old_max_y = rect->max_y;
    rect->max_y = max(rect->min_y, rect->max_y - amt);

    return (CR_Rect){rect->min_x, rect->max_y, rect->max_x, old_max_y};
}

CR_Rect cr_cut(CR_Cut cut, float amt)
{
    switch (cut.side) {
    case CR_TOP:
        return cr_top(cut.rect, amt);
    case CR_BOT:
        return cr_bot(cut.rect, amt);
    case CR_LEFT:
        return cr_left(cut.rect, amt);
    case CR_RIGHT:
        return cr_right(cut.rect, amt);
    default:
        assert(false);
    }
}

void cr_shrink(CR_Rect *rect, float amt)
{
    cr_shrink_x(rect, amt);
    cr_shrink_y(rect, amt);
}

void cr_shrink_x(CR_Rect *rect, float amt)
{
    cr_left(rect, amt);
    cr_right(rect, amt);
}

void cr_shrink_y(CR_Rect *rect, float amt)
{
    cr_top(rect, amt);
    cr_bot(rect, amt);
}
