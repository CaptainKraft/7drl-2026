#pragma once

#include <assert.h>

#include "ck/ck.h"

#define WORLD_ART_TILE_SIZE 16
#define WORLD_ART_DISPLAY_NAME_CAP 64

#define WORLD_ART_COL_COUNT 16
#define WORLD_ART_DATA_ROW_COUNT 8
#define WORLD_ART_THEME_COUNT 4

#define WORLD_ART_WALL_VARIATION_COUNT 6
#define WORLD_ART_WALL_TOP_VARIATION_COUNT 6
#define WORLD_ART_FLOOR_VARIATION_COUNT 16

#define WORLD_ART_THEME_ROW_BASE 0
#define WORLD_ART_FLOOR_ROW_BASE 4

#define WORLD_ART_WALL_COL_BASE 0
#define WORLD_ART_WALL_TOP_COL_BASE (WORLD_ART_WALL_COL_BASE + WORLD_ART_WALL_VARIATION_COUNT)
#define WORLD_ART_STAIRS_DOWN_COL (WORLD_ART_WALL_TOP_COL_BASE + WORLD_ART_WALL_TOP_VARIATION_COUNT)
#define WORLD_ART_STAIRS_UP_COL (WORLD_ART_STAIRS_DOWN_COL + 1)
#define WORLD_ART_MISC1_COL (WORLD_ART_STAIRS_UP_COL + 1)
#define WORLD_ART_MISC2_COL (WORLD_ART_MISC1_COL + 1)

typedef u16 WORLD_ART_KIND;

typedef struct {
    u8 x;
    u8 y;
} World_Art_Tile;

typedef struct {
    u16 x;
    u16 y;
} World_Art_Pixel;

typedef enum {
    WORLD_ART_ROLE_WALL_NORTH,
    WORLD_ART_ROLE_WALL_TOP,
    WORLD_ART_ROLE_STAIRS_DOWN,
    WORLD_ART_ROLE_STAIRS_UP,
    WORLD_ART_ROLE_MISC1,
    WORLD_ART_ROLE_MISC2,
    WORLD_ART_ROLE_FLOOR,
} WORLD_ART_ROLE;

typedef enum {
    WORLD_WALL_DIRECTION_NORTH,
    WORLD_WALL_DIRECTION_EAST,
    WORLD_WALL_DIRECTION_SOUTH,
    WORLD_WALL_DIRECTION_WEST,
    NUM_WORLD_WALL_DIRECTIONS,
} WORLD_WALL_DIRECTION;

typedef enum {
    WORLD_ART_THEME_1,
    WORLD_ART_THEME_2,
    WORLD_ART_THEME_3,
    WORLD_ART_THEME_4,
    NUM_WORLD_ART_THEMES,
} WORLD_ART_THEME;

typedef enum {
    WORLD_ART_FLOOR_THEME_STONE,
    WORLD_ART_FLOOR_THEME_CLAY,
    WORLD_ART_FLOOR_THEME_DIRT,
    WORLD_ART_FLOOR_THEME_GRASS,
    WORLD_ART_FLOOR_THEME_EMPTY,
    WORLD_ART_FLOOR_THEME_FLAG,
    WORLD_ART_FLOOR_THEME_RED,
    WORLD_ART_FLOOR_THEME_COBBLE,
    WORLD_ART_FLOOR_THEME_MOSS,
    NUM_WORLD_ART_FLOOR_THEMES,
} WORLD_ART_FLOOR_THEME;

typedef struct {
    WORLD_ART_FLOOR_THEME theme;
    u8 row;
    u8 col_start;
    u8 variation_count;
} World_Art_Floor_Group;

enum {
    WORLD_ART_NONE = 0,
    NUM_WORLD_ART = (WORLD_ART_COL_COUNT * WORLD_ART_DATA_ROW_COUNT) + 1,
};

static const char *world_art_wall_theme_display_names[WORLD_ART_THEME_COUNT] = {
    "Stone",
    "Clay",
    "Moss",
    "Sand",
};

static const char *world_art_floor_theme_display_names[NUM_WORLD_ART_FLOOR_THEMES] = {
    [WORLD_ART_FLOOR_THEME_STONE] = "Stone", [WORLD_ART_FLOOR_THEME_CLAY] = "Clay",
    [WORLD_ART_FLOOR_THEME_DIRT] = "Dirt",   [WORLD_ART_FLOOR_THEME_GRASS] = "Grass",
    [WORLD_ART_FLOOR_THEME_EMPTY] = "Empty", [WORLD_ART_FLOOR_THEME_FLAG] = "Flag",
    [WORLD_ART_FLOOR_THEME_RED] = "Red",     [WORLD_ART_FLOOR_THEME_COBBLE] = "Cobble",
    [WORLD_ART_FLOOR_THEME_MOSS] = "Moss",
};

#define WORLD_ART_FLOOR_GROUP_COUNT 12

static const World_Art_Floor_Group world_art_floor_groups[WORLD_ART_FLOOR_GROUP_COUNT] = {
    {WORLD_ART_FLOOR_THEME_STONE, (u8)(WORLD_ART_FLOOR_ROW_BASE + 0), 0, 8},
    {WORLD_ART_FLOOR_THEME_CLAY, (u8)(WORLD_ART_FLOOR_ROW_BASE + 0), 8, 8},
    {WORLD_ART_FLOOR_THEME_DIRT, (u8)(WORLD_ART_FLOOR_ROW_BASE + 1), 0, 8},
    {WORLD_ART_FLOOR_THEME_GRASS, (u8)(WORLD_ART_FLOOR_ROW_BASE + 1), 8, 8},

    {WORLD_ART_FLOOR_THEME_EMPTY, (u8)(WORLD_ART_FLOOR_ROW_BASE + 2), 0, 1},
    {WORLD_ART_FLOOR_THEME_STONE, (u8)(WORLD_ART_FLOOR_ROW_BASE + 2), 1, 5},
    {WORLD_ART_FLOOR_THEME_CLAY, (u8)(WORLD_ART_FLOOR_ROW_BASE + 2), 6, 5},
    {WORLD_ART_FLOOR_THEME_FLAG, (u8)(WORLD_ART_FLOOR_ROW_BASE + 2), 11, 5},

    {WORLD_ART_FLOOR_THEME_EMPTY, (u8)(WORLD_ART_FLOOR_ROW_BASE + 3), 0, 1},
    {WORLD_ART_FLOOR_THEME_RED, (u8)(WORLD_ART_FLOOR_ROW_BASE + 3), 1, 5},
    {WORLD_ART_FLOOR_THEME_COBBLE, (u8)(WORLD_ART_FLOOR_ROW_BASE + 3), 6, 5},
    {WORLD_ART_FLOOR_THEME_MOSS, (u8)(WORLD_ART_FLOOR_ROW_BASE + 3), 11, 5},
};

static const u8 world_art_north_wall_weights[WORLD_ART_WALL_VARIATION_COUNT] = {
    40, 24, 14, 10, 7, 5,
};

static const u8 world_art_top_wall_weights[WORLD_ART_WALL_TOP_VARIATION_COUNT] = {
    40, 24, 14, 10, 7, 5,
};

_Static_assert(WORLD_ART_COL_COUNT <= 255 && WORLD_ART_DATA_ROW_COUNT <= 255,
               "World art tile coordinates exceed u8 range");
_Static_assert((WORLD_ART_COL_COUNT * WORLD_ART_DATA_ROW_COUNT) + 1 == NUM_WORLD_ART,
               "world art layout and WORLD_ART_KIND are out of sync");
_Static_assert(WORLD_ART_MISC2_COL + 1 == WORLD_ART_COL_COUNT, "World art columns must total 16");
_Static_assert((sizeof(world_art_wall_theme_display_names) /
                sizeof(world_art_wall_theme_display_names[0])) == WORLD_ART_THEME_COUNT,
               "world_art_wall_theme_display_names must match WORLD_ART_THEME_COUNT");
_Static_assert((sizeof(world_art_floor_theme_display_names) /
                sizeof(world_art_floor_theme_display_names[0])) == NUM_WORLD_ART_FLOOR_THEMES,
               "world_art_floor_theme_display_names must match NUM_WORLD_ART_FLOOR_THEMES");
_Static_assert((sizeof(world_art_floor_groups) / sizeof(world_art_floor_groups[0])) ==
                   WORLD_ART_FLOOR_GROUP_COUNT,
               "world_art_floor_groups must match WORLD_ART_FLOOR_GROUP_COUNT");
_Static_assert((sizeof(world_art_north_wall_weights) / sizeof(world_art_north_wall_weights[0])) ==
                   WORLD_ART_WALL_VARIATION_COUNT,
               "world_art_north_wall_weights must match WORLD_ART_WALL_VARIATION_COUNT");
_Static_assert((sizeof(world_art_top_wall_weights) / sizeof(world_art_top_wall_weights[0])) ==
                   WORLD_ART_WALL_TOP_VARIATION_COUNT,
               "world_art_top_wall_weights must match WORLD_ART_WALL_TOP_VARIATION_COUNT");

static inline u16 world_art_get_dense_index(WORLD_ART_KIND kind)
{
    assert(kind > WORLD_ART_NONE && kind < NUM_WORLD_ART);
    return (u16)(kind - 1);
}

static inline WORLD_ART_KIND world_art_get_kind_at(u8 col, u8 row)
{
    assert(col < WORLD_ART_COL_COUNT);
    assert(row < WORLD_ART_DATA_ROW_COUNT);
    return (WORLD_ART_KIND)(1 + ((row * WORLD_ART_COL_COUNT) + col));
}

static inline World_Art_Tile world_art_get_tile(WORLD_ART_KIND kind)
{
    u16 dense_index = world_art_get_dense_index(kind);
    return (World_Art_Tile){
        .x = (u8)(dense_index % WORLD_ART_COL_COUNT),
        .y = (u8)(dense_index / WORLD_ART_COL_COUNT),
    };
}

static inline WORLD_ART_KIND world_art_get_kind_from_tile(World_Art_Tile tile)
{
    return world_art_get_kind_at(tile.x, tile.y);
}

static inline World_Art_Pixel world_art_get_pixel(WORLD_ART_KIND kind)
{
    World_Art_Tile tile = world_art_get_tile(kind);
    return (World_Art_Pixel){
        .x = (u16)(tile.x * WORLD_ART_TILE_SIZE),
        .y = (u16)(tile.y * WORLD_ART_TILE_SIZE),
    };
}

static inline WORLD_ART_THEME world_art_get_theme_from_tile(World_Art_Tile tile)
{
    assert(tile.y < WORLD_ART_THEME_COUNT);
    return (WORLD_ART_THEME)tile.y;
}

static inline WORLD_ART_ROLE world_art_get_role_from_tile(World_Art_Tile tile)
{
    assert(tile.x < WORLD_ART_COL_COUNT);
    assert(tile.y < WORLD_ART_DATA_ROW_COUNT);

    if (tile.y >= WORLD_ART_FLOOR_ROW_BASE)
        return WORLD_ART_ROLE_FLOOR;

    if (tile.x < WORLD_ART_WALL_TOP_COL_BASE)
        return WORLD_ART_ROLE_WALL_NORTH;
    if (tile.x < WORLD_ART_STAIRS_DOWN_COL)
        return WORLD_ART_ROLE_WALL_TOP;
    if (tile.x == WORLD_ART_STAIRS_DOWN_COL)
        return WORLD_ART_ROLE_STAIRS_DOWN;
    if (tile.x == WORLD_ART_STAIRS_UP_COL)
        return WORLD_ART_ROLE_STAIRS_UP;
    if (tile.x == WORLD_ART_MISC1_COL)
        return WORLD_ART_ROLE_MISC1;
    return WORLD_ART_ROLE_MISC2;
}

static inline const char *world_art_get_floor_theme_display_name(WORLD_ART_FLOOR_THEME theme)
{
    assert(theme < NUM_WORLD_ART_FLOOR_THEMES);
    return world_art_floor_theme_display_names[theme];
}

static inline const World_Art_Floor_Group *world_art_get_floor_group_from_tile(World_Art_Tile tile)
{
    assert(world_art_get_role_from_tile(tile) == WORLD_ART_ROLE_FLOOR);

    for (u32 group_idx = 0; group_idx < WORLD_ART_FLOOR_GROUP_COUNT; group_idx++) {
        const World_Art_Floor_Group *group = &world_art_floor_groups[group_idx];
        u8 col_end = (u8)(group->col_start + group->variation_count);

        if (tile.y == group->row && tile.x >= group->col_start && tile.x < col_end)
            return group;
    }

    assert(0 && "Floor tile not mapped to floor groups");
    return &world_art_floor_groups[0];
}

static inline WORLD_ART_FLOOR_THEME world_art_get_floor_theme_from_tile(World_Art_Tile tile)
{
    return world_art_get_floor_group_from_tile(tile)->theme;
}

static inline u8 world_art_get_floor_variation_from_tile(World_Art_Tile tile)
{
    const World_Art_Floor_Group *group = world_art_get_floor_group_from_tile(tile);
    WORLD_ART_FLOOR_THEME theme = group->theme;
    u8 variation = 0;

    for (u32 group_idx = 0; group_idx < WORLD_ART_FLOOR_GROUP_COUNT; group_idx++) {
        const World_Art_Floor_Group *theme_group = &world_art_floor_groups[group_idx];
        if (theme_group->theme != theme)
            continue;

        u8 col_end = (u8)(theme_group->col_start + theme_group->variation_count);
        if (tile.y == theme_group->row && tile.x >= theme_group->col_start && tile.x < col_end)
            return (u8)(variation + (tile.x - theme_group->col_start));

        variation = (u8)(variation + theme_group->variation_count);
    }

    assert(0 && "Floor tile variation lookup failed");
    return 0;
}

static inline u8 world_art_get_variation_from_tile(World_Art_Tile tile)
{
    WORLD_ART_ROLE role = world_art_get_role_from_tile(tile);
    switch (role) {
    case WORLD_ART_ROLE_WALL_NORTH:
        return (u8)(tile.x - WORLD_ART_WALL_COL_BASE);
    case WORLD_ART_ROLE_WALL_TOP:
        return (u8)(tile.x - WORLD_ART_WALL_TOP_COL_BASE);
    case WORLD_ART_ROLE_FLOOR:
        return world_art_get_floor_variation_from_tile(tile);
    default:
        return 0;
    }
}

static inline World_Art_Tile world_art_get_north_wall_tile(WORLD_ART_THEME theme, u8 variation)
{
    assert(theme < WORLD_ART_THEME_COUNT);
    assert(variation < WORLD_ART_WALL_VARIATION_COUNT);
    return (World_Art_Tile){
        .x = (u8)(WORLD_ART_WALL_COL_BASE + variation),
        .y = (u8)(WORLD_ART_THEME_ROW_BASE + theme),
    };
}

static inline World_Art_Tile world_art_get_top_wall_tile(WORLD_ART_THEME theme, u8 variation)
{
    assert(theme < WORLD_ART_THEME_COUNT);
    assert(variation < WORLD_ART_WALL_TOP_VARIATION_COUNT);
    return (World_Art_Tile){
        .x = (u8)(WORLD_ART_WALL_TOP_COL_BASE + variation),
        .y = (u8)(WORLD_ART_THEME_ROW_BASE + theme),
    };
}

static inline World_Art_Tile world_art_get_down_stairs_tile(WORLD_ART_THEME theme)
{
    assert(theme < WORLD_ART_THEME_COUNT);
    return (World_Art_Tile){
        .x = WORLD_ART_STAIRS_DOWN_COL,
        .y = (u8)(WORLD_ART_THEME_ROW_BASE + theme),
    };
}

static inline World_Art_Tile world_art_get_up_stairs_tile(WORLD_ART_THEME theme)
{
    assert(theme < WORLD_ART_THEME_COUNT);
    return (World_Art_Tile){
        .x = WORLD_ART_STAIRS_UP_COL,
        .y = (u8)(WORLD_ART_THEME_ROW_BASE + theme),
    };
}

static inline World_Art_Tile world_art_get_misc1_tile(WORLD_ART_THEME theme)
{
    assert(theme < WORLD_ART_THEME_COUNT);
    return (World_Art_Tile){
        .x = WORLD_ART_MISC1_COL,
        .y = (u8)(WORLD_ART_THEME_ROW_BASE + theme),
    };
}

static inline World_Art_Tile world_art_get_misc2_tile(WORLD_ART_THEME theme)
{
    assert(theme < WORLD_ART_THEME_COUNT);
    return (World_Art_Tile){
        .x = WORLD_ART_MISC2_COL,
        .y = (u8)(WORLD_ART_THEME_ROW_BASE + theme),
    };
}

static inline World_Art_Tile world_art_get_floor_tile(u8 floor_row, u8 variation)
{
    assert(floor_row < WORLD_ART_THEME_COUNT);
    assert(variation < WORLD_ART_FLOOR_VARIATION_COUNT);
    return (World_Art_Tile){
        .x = variation,
        .y = (u8)(WORLD_ART_FLOOR_ROW_BASE + floor_row),
    };
}

static inline u8 world_art_get_north_wall_weight(u8 variation)
{
    assert(variation < WORLD_ART_WALL_VARIATION_COUNT);
    return world_art_north_wall_weights[variation];
}

static inline u8 world_art_get_top_wall_weight(u8 variation)
{
    assert(variation < WORLD_ART_WALL_TOP_VARIATION_COUNT);
    return world_art_top_wall_weights[variation];
}

static inline float world_art_get_wall_rotation_degrees(WORLD_WALL_DIRECTION direction)
{
    switch (direction) {
    case WORLD_WALL_DIRECTION_NORTH:
        return 0.0f;
    case WORLD_WALL_DIRECTION_EAST:
        return 90.0f;
    case WORLD_WALL_DIRECTION_SOUTH:
        return 180.0f;
    case WORLD_WALL_DIRECTION_WEST:
        return -90.0f;
    default:
        assert(0 && "Invalid WORLD_WALL_DIRECTION");
        return 0.0f;
    }
}

static inline const char *world_art_get_wall_direction_name(WORLD_WALL_DIRECTION direction)
{
    switch (direction) {
    case WORLD_WALL_DIRECTION_NORTH:
        return "North";
    case WORLD_WALL_DIRECTION_EAST:
        return "East";
    case WORLD_WALL_DIRECTION_SOUTH:
        return "South";
    case WORLD_WALL_DIRECTION_WEST:
        return "West";
    default:
        assert(0 && "Invalid WORLD_WALL_DIRECTION");
        return "Unknown";
    }
}

static inline u32 world_art_append_text(char *buffer, u32 buffer_cap, u32 write_idx,
                                        const char *text)
{
    while (text[0] != '\0' && write_idx + 1 < buffer_cap) {
        buffer[write_idx++] = *text;
        text++;
    }
    return write_idx;
}

static inline u32 world_art_append_u32(char *buffer, u32 buffer_cap, u32 write_idx, u32 value)
{
    char digits[10];
    u32 digit_count = 0;

    do {
        digits[digit_count++] = (char)('0' + (value % 10));
        value /= 10;
    } while (value > 0 && digit_count < (sizeof(digits) / sizeof(digits[0])));

    while (digit_count > 0 && write_idx + 1 < buffer_cap)
        buffer[write_idx++] = digits[--digit_count];

    return write_idx;
}

static inline void world_art_get_display_name(WORLD_ART_KIND kind, char *buffer, u32 buffer_cap)
{
    assert(kind > WORLD_ART_NONE && kind < NUM_WORLD_ART);
    assert(buffer != 0);
    assert(buffer_cap > 0);

    World_Art_Tile tile = world_art_get_tile(kind);
    WORLD_ART_ROLE role = world_art_get_role_from_tile(tile);
    u8 variation = world_art_get_variation_from_tile(tile);
    WORLD_ART_THEME wall_theme = WORLD_ART_THEME_1;
    WORLD_ART_FLOOR_THEME floor_theme = WORLD_ART_FLOOR_THEME_STONE;

    if (role == WORLD_ART_ROLE_FLOOR)
        floor_theme = world_art_get_floor_theme_from_tile(tile);
    else
        wall_theme = world_art_get_theme_from_tile(tile);

    u32 write_idx = 0;

    switch (role) {
    case WORLD_ART_ROLE_WALL_NORTH:
        write_idx = world_art_append_text(buffer, buffer_cap, write_idx,
                                          world_art_wall_theme_display_names[wall_theme]);
        write_idx = world_art_append_text(buffer, buffer_cap, write_idx, " ");
        write_idx = world_art_append_text(buffer, buffer_cap, write_idx, "North Wall ");
        write_idx = world_art_append_u32(buffer, buffer_cap, write_idx, (u32)variation + 1);
        break;
    case WORLD_ART_ROLE_WALL_TOP:
        write_idx = world_art_append_text(buffer, buffer_cap, write_idx,
                                          world_art_wall_theme_display_names[wall_theme]);
        write_idx = world_art_append_text(buffer, buffer_cap, write_idx, " ");
        write_idx = world_art_append_text(buffer, buffer_cap, write_idx, "Top Wall ");
        write_idx = world_art_append_u32(buffer, buffer_cap, write_idx, (u32)variation + 1);
        break;
    case WORLD_ART_ROLE_STAIRS_DOWN:
        write_idx = world_art_append_text(buffer, buffer_cap, write_idx,
                                          world_art_wall_theme_display_names[wall_theme]);
        write_idx = world_art_append_text(buffer, buffer_cap, write_idx, " ");
        write_idx = world_art_append_text(buffer, buffer_cap, write_idx, "Down Stairs");
        break;
    case WORLD_ART_ROLE_STAIRS_UP:
        write_idx = world_art_append_text(buffer, buffer_cap, write_idx,
                                          world_art_wall_theme_display_names[wall_theme]);
        write_idx = world_art_append_text(buffer, buffer_cap, write_idx, " ");
        write_idx = world_art_append_text(buffer, buffer_cap, write_idx, "Up Stairs");
        break;
    case WORLD_ART_ROLE_MISC1:
        write_idx = world_art_append_text(buffer, buffer_cap, write_idx,
                                          world_art_wall_theme_display_names[wall_theme]);
        write_idx = world_art_append_text(buffer, buffer_cap, write_idx, " ");
        write_idx = world_art_append_text(buffer, buffer_cap, write_idx, "Pool");
        break;
    case WORLD_ART_ROLE_MISC2:
        write_idx = world_art_append_text(buffer, buffer_cap, write_idx,
                                          world_art_wall_theme_display_names[wall_theme]);
        write_idx = world_art_append_text(buffer, buffer_cap, write_idx, " ");
        write_idx = world_art_append_text(buffer, buffer_cap, write_idx, "Spout");
        break;
    case WORLD_ART_ROLE_FLOOR:
        write_idx = world_art_append_text(buffer, buffer_cap, write_idx,
                                          world_art_get_floor_theme_display_name(floor_theme));
        if (floor_theme != WORLD_ART_FLOOR_THEME_EMPTY) {
            u32 floor_number = (u32)variation + 1;
            if (floor_number == 7) {
                write_idx = world_art_append_text(buffer, buffer_cap, write_idx, " Floor Hole");
            } else if (floor_number == 8) {
                write_idx = world_art_append_text(buffer, buffer_cap, write_idx, " Floor Pit");
            } else {
                write_idx = world_art_append_text(buffer, buffer_cap, write_idx, " Floor ");
                write_idx = world_art_append_u32(buffer, buffer_cap, write_idx, floor_number);
            }
        } else {
            write_idx = world_art_append_text(buffer, buffer_cap, write_idx, " ");
            write_idx = world_art_append_u32(buffer, buffer_cap, write_idx, (u32)variation + 1);
        }
        break;
    }

    buffer[write_idx] = '\0';
}
