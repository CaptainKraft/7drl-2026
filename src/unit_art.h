#pragma once

#include <assert.h>

#include "ck/ck.h"

#define UNIT_ART_TILE_SIZE 16
#define UNIT_ART_ORIENTATION_COUNT 4
#define UNIT_ART_ANIM_FRAME_COUNT 2
#define UNIT_ART_DISPLAY_NAME_CAP 64
#define UNIT_ART_SHEET_TILE_WIDTH 16
#define UNIT_ART_SHEET_TILE_HEIGHT 32
#define UNIT_ART_COL_COUNT (UNIT_ART_SHEET_TILE_WIDTH / UNIT_ART_ORIENTATION_COUNT)
#define UNIT_ART_ROW_COUNT (UNIT_ART_SHEET_TILE_HEIGHT / UNIT_ART_ANIM_FRAME_COUNT)

typedef struct {
    u8 x;
    u8 y;
} Unit_Art_Tile;

typedef struct {
    u16 x;
    u16 y;
} Unit_Art_Pixel;

typedef struct {
    const char *id;
    Unit_Art_Tile base_tile;
} Unit_Art_Def;

typedef struct {
    float x;
    float y;
} Unit_Art_Anchor;

// clang-format off
#define UNIT_ART_LIST \
    UNIT_ART_ENTRY(FIGHTER, "fighter", 0, 0) \
    UNIT_ART_ENTRY(BARBARIAN, "barbarian", 4, 0) \
    UNIT_ART_ENTRY(ROGUE, "rogue", 8, 0) \
    UNIT_ART_ENTRY(PALADIN, "paladin", 12, 0) \
    UNIT_ART_ENTRY(ASSASSIN, "assassin", 0, 2) \
    UNIT_ART_ENTRY(PYROMANCER, "pyromancer", 4, 2) \
    UNIT_ART_ENTRY(SORCERER, "sorcerer", 8, 2) \
    UNIT_ART_ENTRY(CLERIC, "cleric", 12, 2) \
    UNIT_ART_ENTRY(DRUID, "druid", 0, 4) \
    UNIT_ART_ENTRY(WARLOCK, "warlock", 4, 4) \
    UNIT_ART_ENTRY(ARCHER, "archer", 8, 4) \
    UNIT_ART_ENTRY(BARD, "bard", 12, 4) \
    UNIT_ART_ENTRY(GOBLIN_GRUNT, "goblin_grunt", 0, 6) \
    UNIT_ART_ENTRY(GOBLIN_SHAMAN, "goblin_shaman", 4, 6) \
    UNIT_ART_ENTRY(GOBLIN_WARRIOR, "goblin_warrior", 8, 6) \
    UNIT_ART_ENTRY(TROLL, "troll", 12, 6) \
    UNIT_ART_ENTRY(SKELETON_GRUNT, "skeleton_grunt", 0, 8) \
    UNIT_ART_ENTRY(SKELTON_WARRIOR, "skelton_warrior", 4, 8) \
    UNIT_ART_ENTRY(SKELETON_MAGE, "skeleton_mage", 8, 8) \
    UNIT_ART_ENTRY(SKELETON_KING, "skeleton_king", 12, 8) \
    UNIT_ART_ENTRY(VAMPIRE, "vampire", 0, 10) \
    UNIT_ART_ENTRY(ZOMBIE, "zombie", 4, 10) \
    UNIT_ART_ENTRY(BEHOLDER, "beholder", 8, 10) \
    UNIT_ART_ENTRY(DEATH_KNIGHT, "death_knight", 12, 10) \
    UNIT_ART_ENTRY(IMP, "imp", 0, 12) \
    UNIT_ART_ENTRY(SPIRIT, "spirit", 4, 12) \
    UNIT_ART_ENTRY(FAIRY, "fairy", 8, 12) \
    UNIT_ART_ENTRY(ELEMENTAL, "elemental", 12, 12) \
    UNIT_ART_ENTRY(DOG, "dog", 0, 14) \
    UNIT_ART_ENTRY(CAT, "cat", 4, 14) \
    UNIT_ART_ENTRY(BAT, "bat", 8, 14) \
    UNIT_ART_ENTRY(COBRA, "cobra", 12, 14) \
    UNIT_ART_ENTRY(SPIDER, "spider", 0, 16) \
    UNIT_ART_ENTRY(RAT, "rat", 4, 16) \
    UNIT_ART_ENTRY(SLIME, "slime", 8, 16) \
    UNIT_ART_ENTRY(MOTH, "moth", 12, 16) \
    UNIT_ART_ENTRY(ADVENTURER, "adventurer", 0, 18) \
    UNIT_ART_ENTRY(LADY, "lady", 4, 18) \
    UNIT_ART_ENTRY(MERCHANT, "merchant", 8, 18) \
    UNIT_ART_ENTRY(DRAGON, "dragon", 12, 18) \
    UNIT_ART_ENTRY(KING, "king", 0, 20) \
    UNIT_ART_ENTRY(QUEEN, "queen", 4, 20) \
    UNIT_ART_ENTRY(REAPER, "reaper", 8, 20) \
    UNIT_ART_ENTRY(MINOTAUR, "minotaur", 12, 20) \
    UNIT_ART_ENTRY(PHOENIX, "phoenix", 0, 22) \
    UNIT_ART_ENTRY(MIMIC, "mimic", 4, 22) \
    UNIT_ART_ENTRY(CENTAUR, "centaur", 8, 22) \
    UNIT_ART_ENTRY(SATYR, "satyr", 12, 22) \
    UNIT_ART_ENTRY(ANGEL, "angel", 0, 24) \
    UNIT_ART_ENTRY(DWARF, "dwarf", 4, 24) \
    UNIT_ART_ENTRY(HALFLING, "halfling", 8, 24) \
    UNIT_ART_ENTRY(GRIFFON, "griffon", 12, 24) \
    UNIT_ART_ENTRY(MAN_EATER, "man_eater", 0, 26) \
    UNIT_ART_ENTRY(BEETLE, "beetle", 4, 26) \
    UNIT_ART_ENTRY(HORSE, "horse", 8, 26) \
    UNIT_ART_ENTRY(UNICORN, "unicorn", 12, 26) \
    UNIT_ART_ENTRY(MUMMY, "mummy", 0, 28) \
    UNIT_ART_ENTRY(GELATINOUS_CUBE, "gelatinous_cube", 4, 28) \
    UNIT_ART_ENTRY(VOID_ORB, "void_orb", 8, 28) \
    UNIT_ART_ENTRY(FIREBALL, "fireball", 12, 28) \
    UNIT_ART_ENTRY(GIANT, "giant", 0, 30) \
    UNIT_ART_ENTRY(TREANT, "treant", 4, 30) \
    UNIT_ART_ENTRY(YETI, "yeti", 8, 30) \
    UNIT_ART_ENTRY(FLIES, "flies", 12, 30)
// clang-format on

typedef enum {
    UNIT_ART_NONE,
#define UNIT_ART_ENTRY(_name, _id, _tile_x, _tile_y) UNIT_ART_##_name,
    UNIT_ART_LIST
#undef UNIT_ART_ENTRY
        NUM_UNIT_ART,
} UNIT_ART_KIND;

static const Unit_Art_Def unit_art_defs[NUM_UNIT_ART] = {[UNIT_ART_NONE] = {0},
#define UNIT_ART_ENTRY(_name, _id, _tile_x, _tile_y)                                               \
    [UNIT_ART_##_name] = {                                                                         \
        .id = _id,                                                                                 \
        .base_tile = {.x = (u8)(_tile_x), .y = (u8)(_tile_y)},                                     \
    },
                                                         UNIT_ART_LIST
#undef UNIT_ART_ENTRY
};

static const Unit_Art_Anchor unit_art_anchor_defs[NUM_UNIT_ART] = {
    [UNIT_ART_NONE] = {0},
    [UNIT_ART_FIGHTER] = {8.000000f, 15.500000f},
    [UNIT_ART_BARBARIAN] = {8.000000f, 15.500000f},
    [UNIT_ART_ROGUE] = {8.000000f, 15.500000f},
    [UNIT_ART_PALADIN] = {8.000000f, 15.500000f},
    [UNIT_ART_ASSASSIN] = {8.000000f, 15.500000f},
    [UNIT_ART_PYROMANCER] = {8.500000f, 15.500000f},
    [UNIT_ART_SORCERER] = {8.500000f, 15.500000f},
    [UNIT_ART_CLERIC] = {8.500000f, 15.500000f},
    [UNIT_ART_DRUID] = {8.500000f, 15.500000f},
    [UNIT_ART_WARLOCK] = {8.500000f, 15.500000f},
    [UNIT_ART_ARCHER] = {8.000000f, 15.500000f},
    [UNIT_ART_BARD] = {8.000000f, 15.500000f},
    [UNIT_ART_GOBLIN_GRUNT] = {8.500000f, 15.500000f},
    [UNIT_ART_GOBLIN_SHAMAN] = {9.000000f, 15.500000f},
    [UNIT_ART_GOBLIN_WARRIOR] = {9.000000f, 15.500000f},
    [UNIT_ART_TROLL] = {8.500000f, 15.500000f},
    [UNIT_ART_SKELETON_GRUNT] = {8.000000f, 15.500000f},
    [UNIT_ART_SKELTON_WARRIOR] = {8.000000f, 15.500000f},
    [UNIT_ART_SKELETON_MAGE] = {8.500000f, 15.500000f},
    [UNIT_ART_SKELETON_KING] = {7.500000f, 15.500000f},
    [UNIT_ART_VAMPIRE] = {8.500000f, 15.500000f},
    [UNIT_ART_ZOMBIE] = {8.000000f, 15.500000f},
    [UNIT_ART_BEHOLDER] = {8.000000f, 12.000000f},
    [UNIT_ART_DEATH_KNIGHT] = {7.500000f, 15.500000f},
    [UNIT_ART_IMP] = {8.000000f, 15.500000f},
    [UNIT_ART_SPIRIT] = {7.500000f, 15.500000f},
    [UNIT_ART_FAIRY] = {7.500000f, 15.500000f},
    [UNIT_ART_ELEMENTAL] = {7.500000f, 15.500000f},
    [UNIT_ART_DOG] = {7.500000f, 15.500000f},
    [UNIT_ART_CAT] = {7.500000f, 15.500000f},
    [UNIT_ART_BAT] = {7.500000f, 15.500000f},
    [UNIT_ART_COBRA] = {8.000000f, 15.500000f},
    [UNIT_ART_SPIDER] = {8.000000f, 15.500000f},
    [UNIT_ART_RAT] = {8.000000f, 15.500000f},
    [UNIT_ART_SLIME] = {8.000000f, 15.500000f},
    [UNIT_ART_MOTH] = {7.000000f, 15.500000f},
    [UNIT_ART_ADVENTURER] = {7.500000f, 15.500000f},
    [UNIT_ART_LADY] = {7.500000f, 15.500000f},
    [UNIT_ART_MERCHANT] = {7.081081f, 12.500000f},
    [UNIT_ART_DRAGON] = {9.500000f, 15.500000f},
    [UNIT_ART_KING] = {7.184877f, 13.000000f},
    [UNIT_ART_QUEEN] = {7.184877f, 13.000000f},
    [UNIT_ART_REAPER] = {8.000000f, 15.500000f},
    [UNIT_ART_MINOTAUR] = {8.227273f, 14.000000f},
    [UNIT_ART_PHOENIX] = {7.500000f, 15.500000f},
    [UNIT_ART_MIMIC] = {8.000000f, 14.500000f},
    [UNIT_ART_CENTAUR] = {9.000000f, 15.500000f},
    [UNIT_ART_SATYR] = {8.000000f, 15.500000f},
    [UNIT_ART_ANGEL] = {7.500000f, 15.500000f},
    [UNIT_ART_DWARF] = {8.000000f, 15.500000f},
    [UNIT_ART_HALFLING] = {8.000000f, 15.500000f},
    [UNIT_ART_GRIFFON] = {7.900776f, 14.500000f},
    [UNIT_ART_MAN_EATER] = {7.500000f, 11.500000f},
    [UNIT_ART_BEETLE] = {8.000000f, 15.500000f},
    [UNIT_ART_HORSE] = {8.250000f, 15.500000f},
    [UNIT_ART_UNICORN] = {8.250000f, 15.500000f},
    [UNIT_ART_MUMMY] = {8.000000f, 15.500000f},
    [UNIT_ART_GELATINOUS_CUBE] = {8.000000f, 15.500000f},
    [UNIT_ART_VOID_ORB] = {8.000000f, 11.500000f},
    [UNIT_ART_FIREBALL] = {8.000000f, 11.500000f},
    [UNIT_ART_GIANT] = {8.500000f, 15.500000f},
    [UNIT_ART_TREANT] = {8.000000f, 15.500000f},
    [UNIT_ART_YETI] = {8.500000f, 15.500000f},
    [UNIT_ART_FLIES] = {11.500000f, 15.500000f},
};

#undef UNIT_ART_LIST

_Static_assert((UNIT_ART_ROW_COUNT * UNIT_ART_COL_COUNT) + 1 == NUM_UNIT_ART,
               "units.png layout and UNIT_ART_KIND are out of sync");
_Static_assert((sizeof(unit_art_defs) / sizeof(unit_art_defs[0])) == NUM_UNIT_ART,
               "unit_art_defs must contain every UNIT_ART_KIND");
_Static_assert((sizeof(unit_art_anchor_defs) / sizeof(unit_art_anchor_defs[0])) == NUM_UNIT_ART,
               "unit_art_anchor_defs must contain every UNIT_ART_KIND");

static inline const char *unit_art_get_id(UNIT_ART_KIND kind) {
    assert(kind > UNIT_ART_NONE && kind < NUM_UNIT_ART);
    return unit_art_defs[kind].id;
}

static inline void unit_art_get_display_name(UNIT_ART_KIND kind, char *buffer, u32 buffer_cap) {
    assert(kind > UNIT_ART_NONE && kind < NUM_UNIT_ART);
    assert(buffer != 0);
    assert(buffer_cap > 0);

    const char *id = unit_art_get_id(kind);
    u32 write_idx = 0;
    u8 capitalize_next = 1;

    while (id[0] != '\0' && write_idx + 1 < buffer_cap) {
        char c = *id++;
        if (c == '_') {
            buffer[write_idx++] = ' ';
            capitalize_next = 1;
            continue;
        }

        if (capitalize_next && c >= 'a' && c <= 'z')
            c = (char)(c - ('a' - 'A'));

        buffer[write_idx++] = c;
        capitalize_next = 0;
    }

    buffer[write_idx] = '\0';
}

static inline Unit_Art_Tile unit_art_get_tile(UNIT_ART_KIND kind, u8 anim_frame, u8 orientation) {
    assert(kind > UNIT_ART_NONE && kind < NUM_UNIT_ART);
    assert(anim_frame < UNIT_ART_ANIM_FRAME_COUNT);
    assert(orientation < UNIT_ART_ORIENTATION_COUNT);

    Unit_Art_Tile base = unit_art_defs[kind].base_tile;
    return (Unit_Art_Tile){
        .x = (u8)(base.x + orientation),
        .y = (u8)(base.y + anim_frame),
    };
}

static inline Unit_Art_Pixel unit_art_get_pixel(UNIT_ART_KIND kind, u8 anim_frame, u8 orientation) {
    Unit_Art_Tile tile = unit_art_get_tile(kind, anim_frame, orientation);
    return (Unit_Art_Pixel){
        .x = (u16)(tile.x * UNIT_ART_TILE_SIZE),
        .y = (u16)(tile.y * UNIT_ART_TILE_SIZE),
    };
}

static inline Unit_Art_Anchor unit_art_get_anchor(UNIT_ART_KIND kind, u8 orientation) {
    assert(kind > UNIT_ART_NONE && kind < NUM_UNIT_ART);
    assert(orientation < UNIT_ART_ORIENTATION_COUNT);
    (void)orientation;
    return unit_art_anchor_defs[kind];
}
