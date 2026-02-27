#pragma once

#include <stdint.h>

#define i64 int64_t
#define i32 int32_t
#define i16 int16_t
#define i8 int8_t
#define u64 uint64_t
#define u32 uint32_t
#define u16 uint16_t
#define u8 uint8_t

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef clamp
#define clamp(x, xmin, xmax) (((x) < (xmin)) ? (xmin) : ((x) > (xmax)) ? (xmax) : (x))
#endif
#ifndef mid
#define mid(a, b) (((a) + (b)) / 2.0)
#endif

#include "ck/ck_ui.h"
// #include "ck/ck_math.h"
#include "ck/ck_rng.h"
#include "ck/ck_anim.h"
#include "ck/ck_memory.h"
// #include "ck/ck_string.h"

#define KB(x) ((x) * 1024)
#define MB(x) ((KB(x)) * 1024)
#define GB(x) ((MB(x)) * 1024)
