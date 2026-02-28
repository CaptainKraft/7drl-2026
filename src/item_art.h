#pragma once

#include <assert.h>

#include "ck/ck.h"

#define ITEM_ART_TILE_SIZE 16
#define ITEM_ART_DISPLAY_NAME_CAP 64
#define ITEM_ART_COL_COUNT 16
#define ITEM_ART_ROW_COUNT 6
#define ITEM_ART_SHEET_TILE_WIDTH ITEM_ART_COL_COUNT
#define ITEM_ART_SHEET_TILE_HEIGHT ITEM_ART_ROW_COUNT
#define ITEM_ART_ANIM_FRAME_COUNT 2
#define ITEM_ART_KIND_AT(_col, _row) ((ITEM_ART_KIND)(1 + (((_row) * ITEM_ART_COL_COUNT) + (_col))))
#define ITEM_ART_TORCH_1 ITEM_ART_KIND_AT(14, 0)
#define ITEM_ART_TORCH_2 ITEM_ART_KIND_AT(15, 0)

typedef u16 ITEM_ART_KIND;

enum {
    ITEM_ART_NONE = 0,
    NUM_ITEM_ART = (ITEM_ART_COL_COUNT * ITEM_ART_ROW_COUNT) + 1,
};

typedef struct {
    u8 x;
    u8 y;
} Item_Art_Tile;

typedef struct {
    u16 x;
    u16 y;
} Item_Art_Pixel;

typedef struct {
    float x;
    float y;
} Item_Art_Anchor;

static const char *item_art_display_names[ITEM_ART_COL_COUNT * ITEM_ART_ROW_COUNT] = {
    "Brown Spellbook",
    "Dark Spellbook",
    "Green Spellbook",
    "Gold Key",
    "Silver Key",
    "Blue Key",
    "Scroll",
    "Empty Heart",
    "Heart",
    "Empty Mana",
    "Mana",
    "Skull",
    "Golden Skull",
    "Drumstick",
    "Torch1",
    "Torch2",

    "Blue Potion",
    "Red Potion",
    "Green Potion",
    "Orange Potion",
    "Dark Potion",
    "Purple Gem",
    "Red Gem",
    "Blue Gem",
    "Green Gem",
    "Amber Gem",
    "Copper",
    "Silver",
    "Gold",
    "Copper Coin",
    "Silver Coin",
    "Gold Coin",

    "Leather Helmet",
    "Leather Armor",
    "Leather Gloves",
    "Leather Pants",
    "Leather Boots",
    "Iron Helmet",
    "Iron Armor",
    "Iron Gloves",
    "Iron Pants",
    "Iron Boots",
    "Blue Helmet",
    "Blue Armor",
    "Blue Gloves",
    "Blue Pants",
    "Blue Boots",
    "Coin Purse",

    "Leather Wizard Hat",
    "Purple Wizard Hat",
    "Blue Wizard Hat",
    "Leather Cape",
    "Purple Cape",
    "Blue Cape",
    "Iron Crown",
    "Gold Crown",
    "Purple Crown",
    "Quiver",
    "Arrow",
    "Bolt",
    "Shuriken",
    "Bandage",
    "Cross",
    "Red Feather",

    "Short Sword",
    "Long Sword",
    "Greatsword",
    "Battle Axe",
    "Mace",
    "War Hammer",
    "Club",
    "Flail",
    "Quarterstaff",
    "Bow",
    "Crossbow",
    "Slingshot",
    "Wooden Shield",
    "Iron Shield",
    "Blue Shield",
    "Rock",

    "Silver Ring",
    "Gold Ring",
    "Blue Ring",
    "Amethyst Ring",
    "Ruby Ring",
    "Sapphire Ring",
    "Red Flask",
    "Blue Flask",
    "Green Flask",
    "Green Herb",
    "Red Herb",
    "Golden Herb",
    "Lasso",
    "Darts",
    "Watch",
    "Map",
};

static const Item_Art_Anchor item_art_anchor_defs[NUM_ITEM_ART] = {
    [ITEM_ART_NONE] = {0},
    [ITEM_ART_KIND_AT(0, 0)] = {8.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(1, 0)] = {8.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(2, 0)] = {8.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(3, 0)] = {8.000000f, 10.500000f},
    [ITEM_ART_KIND_AT(4, 0)] = {8.000000f, 10.500000f},
    [ITEM_ART_KIND_AT(5, 0)] = {8.000000f, 10.500000f},
    [ITEM_ART_KIND_AT(6, 0)] = {8.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(7, 0)] = {8.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(8, 0)] = {8.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(9, 0)] = {8.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(10, 0)] = {8.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(11, 0)] = {7.500000f, 13.500000f},
    [ITEM_ART_KIND_AT(12, 0)] = {7.500000f, 13.500000f},
    [ITEM_ART_KIND_AT(13, 0)] = {7.000000f, 12.500000f},
    [ITEM_ART_KIND_AT(14, 0)] = {6.000000f, 12.500000f},
    [ITEM_ART_KIND_AT(15, 0)] = {6.000000f, 12.500000f},
    [ITEM_ART_KIND_AT(0, 1)] = {7.500000f, 14.500000f},
    [ITEM_ART_KIND_AT(1, 1)] = {7.500000f, 14.500000f},
    [ITEM_ART_KIND_AT(2, 1)] = {7.500000f, 14.500000f},
    [ITEM_ART_KIND_AT(3, 1)] = {7.500000f, 14.500000f},
    [ITEM_ART_KIND_AT(4, 1)] = {7.500000f, 14.500000f},
    [ITEM_ART_KIND_AT(5, 1)] = {8.500000f, 13.500000f},
    [ITEM_ART_KIND_AT(6, 1)] = {8.500000f, 13.500000f},
    [ITEM_ART_KIND_AT(7, 1)] = {8.500000f, 13.500000f},
    [ITEM_ART_KIND_AT(8, 1)] = {8.500000f, 13.500000f},
    [ITEM_ART_KIND_AT(9, 1)] = {8.500000f, 13.500000f},
    [ITEM_ART_KIND_AT(10, 1)] = {11.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(11, 1)] = {11.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(12, 1)] = {11.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(13, 1)] = {8.000000f, 12.500000f},
    [ITEM_ART_KIND_AT(14, 1)] = {8.000000f, 12.500000f},
    [ITEM_ART_KIND_AT(15, 1)] = {8.000000f, 12.500000f},
    [ITEM_ART_KIND_AT(0, 2)] = {8.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(1, 2)] = {8.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(2, 2)] = {10.500000f, 13.500000f},
    [ITEM_ART_KIND_AT(3, 2)] = {8.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(4, 2)] = {7.500000f, 13.500000f},
    [ITEM_ART_KIND_AT(5, 2)] = {8.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(6, 2)] = {8.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(7, 2)] = {10.500000f, 13.500000f},
    [ITEM_ART_KIND_AT(8, 2)] = {8.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(9, 2)] = {7.500000f, 13.500000f},
    [ITEM_ART_KIND_AT(10, 2)] = {8.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(11, 2)] = {8.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(12, 2)] = {10.500000f, 13.500000f},
    [ITEM_ART_KIND_AT(13, 2)] = {8.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(14, 2)] = {7.500000f, 13.500000f},
    [ITEM_ART_KIND_AT(15, 2)] = {8.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(0, 3)] = {8.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(1, 3)] = {8.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(2, 3)] = {8.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(3, 3)] = {8.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(4, 3)] = {8.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(5, 3)] = {8.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(6, 3)] = {8.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(7, 3)] = {8.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(8, 3)] = {8.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(9, 3)] = {7.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(10, 3)] = {6.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(11, 3)] = {6.500000f, 13.500000f},
    [ITEM_ART_KIND_AT(12, 3)] = {7.500000f, 13.500000f},
    [ITEM_ART_KIND_AT(13, 3)] = {7.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(14, 3)] = {5.500000f, 13.500000f},
    [ITEM_ART_KIND_AT(15, 3)] = {5.500000f, 13.500000f},
    [ITEM_ART_KIND_AT(0, 4)] = {6.500000f, 11.500000f},
    [ITEM_ART_KIND_AT(1, 4)] = {5.500000f, 12.500000f},
    [ITEM_ART_KIND_AT(2, 4)] = {5.500000f, 13.500000f},
    [ITEM_ART_KIND_AT(3, 4)] = {6.500000f, 13.500000f},
    [ITEM_ART_KIND_AT(4, 4)] = {7.500000f, 13.500000f},
    [ITEM_ART_KIND_AT(5, 4)] = {6.500000f, 13.500000f},
    [ITEM_ART_KIND_AT(6, 4)] = {6.500000f, 13.500000f},
    [ITEM_ART_KIND_AT(7, 4)] = {6.500000f, 13.500000f},
    [ITEM_ART_KIND_AT(8, 4)] = {6.500000f, 13.500000f},
    [ITEM_ART_KIND_AT(9, 4)] = {6.500000f, 13.500000f},
    [ITEM_ART_KIND_AT(10, 4)] = {9.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(11, 4)] = {6.500000f, 13.500000f},
    [ITEM_ART_KIND_AT(12, 4)] = {8.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(13, 4)] = {8.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(14, 4)] = {8.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(15, 4)] = {8.000000f, 11.500000f},
    [ITEM_ART_KIND_AT(0, 5)] = {8.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(1, 5)] = {8.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(2, 5)] = {8.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(3, 5)] = {8.000000f, 12.500000f},
    [ITEM_ART_KIND_AT(4, 5)] = {8.000000f, 12.500000f},
    [ITEM_ART_KIND_AT(5, 5)] = {8.000000f, 12.500000f},
    [ITEM_ART_KIND_AT(6, 5)] = {8.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(7, 5)] = {8.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(8, 5)] = {8.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(9, 5)] = {7.500000f, 13.500000f},
    [ITEM_ART_KIND_AT(10, 5)] = {7.500000f, 13.500000f},
    [ITEM_ART_KIND_AT(11, 5)] = {7.500000f, 13.500000f},
    [ITEM_ART_KIND_AT(12, 5)] = {7.500000f, 14.500000f},
    [ITEM_ART_KIND_AT(13, 5)] = {10.000000f, 13.500000f},
    [ITEM_ART_KIND_AT(14, 5)] = {8.000000f, 14.500000f},
    [ITEM_ART_KIND_AT(15, 5)] = {7.500000f, 14.500000f},
};

_Static_assert((ITEM_ART_COL_COUNT * ITEM_ART_ROW_COUNT) + 1 == NUM_ITEM_ART,
               "items.png layout and ITEM_ART_KIND are out of sync");
_Static_assert(ITEM_ART_COL_COUNT <= 255 && ITEM_ART_ROW_COUNT <= 255,
               "Item art tile coordinates exceed u8 range");
_Static_assert((sizeof(item_art_display_names) / sizeof(item_art_display_names[0])) ==
                   (ITEM_ART_COL_COUNT * ITEM_ART_ROW_COUNT),
               "item_art_display_names must match items sheet tile count");
_Static_assert((sizeof(item_art_anchor_defs) / sizeof(item_art_anchor_defs[0])) == NUM_ITEM_ART,
               "item_art_anchor_defs must contain every ITEM_ART_KIND");

static inline u16 item_art_get_dense_index(ITEM_ART_KIND kind) {
    assert(kind > ITEM_ART_NONE && kind < NUM_ITEM_ART);
    return (u16)(kind - 1);
}

static inline Item_Art_Tile item_art_get_tile(ITEM_ART_KIND kind) {
    u16 dense_index = item_art_get_dense_index(kind);
    return (Item_Art_Tile){
        .x = (u8)(dense_index % ITEM_ART_COL_COUNT),
        .y = (u8)(dense_index / ITEM_ART_COL_COUNT),
    };
}

static inline Item_Art_Pixel item_art_get_pixel(ITEM_ART_KIND kind) {
    Item_Art_Tile tile = item_art_get_tile(kind);
    return (Item_Art_Pixel){
        .x = (u16)(tile.x * ITEM_ART_TILE_SIZE),
        .y = (u16)(tile.y * ITEM_ART_TILE_SIZE),
    };
}

static inline Item_Art_Anchor item_art_get_anchor(ITEM_ART_KIND kind) {
    assert(kind > ITEM_ART_NONE && kind < NUM_ITEM_ART);
    return item_art_anchor_defs[kind];
}

static inline void item_art_get_display_name(ITEM_ART_KIND kind, char *buffer, u32 buffer_cap) {
    assert(buffer != 0);
    assert(buffer_cap > 0);

    u16 dense_index = item_art_get_dense_index(kind);
    const char *display_name = item_art_display_names[dense_index];
    assert(display_name != 0);

    u32 write_idx = 0;
    while (display_name[write_idx] != '\0' && write_idx + 1 < buffer_cap) {
        buffer[write_idx] = display_name[write_idx];
        write_idx++;
    }

    buffer[write_idx] = '\0';
}
