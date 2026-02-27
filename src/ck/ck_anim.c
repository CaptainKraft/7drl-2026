#include "ck/ck_anim.h"

float ck_ease_linear(float t)
{
    return t;
}

float ck_ease_in(float t)
{
    return t * t;
}

float ck_ease_out(float t)
{
    return 1.0 - (t - 1.0) * (t - 1.0);
}

float ck_ease_in_out(float t)
{
    return (t * t) * (3.0 - 2.0 * t);
}

float ck_ease_overshoot_in(float t)
{
    return 2.7 * (t * t * t) - 1.7 * (t * t);
}

float ck_ease_overshoot_out(float t)
{
    return 1.0 + 2.7 * ((t - 1.0) * (t - 1.0) * (t - 1.0)) + 1.7 * ((t - 1.0) * (t - 1.0));
}

float ck_ease_overshoot_in_out(float t)
{
    t = ck_ease_in_out(t);

    float a = 4.0 * (t * t);
    float b = 1.0 - 4.0 * ((t - 1.0) * (t - 1.0));

    return a * (1.0 - t) + b * t;
}
