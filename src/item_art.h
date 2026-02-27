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

_Static_assert((ITEM_ART_COL_COUNT * ITEM_ART_ROW_COUNT) + 1 == NUM_ITEM_ART,
               "items.png layout and ITEM_ART_KIND are out of sync");
_Static_assert(ITEM_ART_COL_COUNT <= 255 && ITEM_ART_ROW_COUNT <= 255,
               "Item art tile coordinates exceed u8 range");
_Static_assert((sizeof(item_art_display_names) / sizeof(item_art_display_names[0])) ==
                   (ITEM_ART_COL_COUNT * ITEM_ART_ROW_COUNT),
               "item_art_display_names must match items sheet tile count");

static inline u16 item_art_get_dense_index(ITEM_ART_KIND kind)
{
    assert(kind > ITEM_ART_NONE && kind < NUM_ITEM_ART);
    return (u16)(kind - 1);
}

static inline Item_Art_Tile item_art_get_tile(ITEM_ART_KIND kind)
{
    u16 dense_index = item_art_get_dense_index(kind);
    return (Item_Art_Tile){
        .x = (u8)(dense_index % ITEM_ART_COL_COUNT),
        .y = (u8)(dense_index / ITEM_ART_COL_COUNT),
    };
}

static inline Item_Art_Pixel item_art_get_pixel(ITEM_ART_KIND kind)
{
    Item_Art_Tile tile = item_art_get_tile(kind);
    return (Item_Art_Pixel){
        .x = (u16)(tile.x * ITEM_ART_TILE_SIZE),
        .y = (u16)(tile.y * ITEM_ART_TILE_SIZE),
    };
}

static inline void item_art_get_display_name(ITEM_ART_KIND kind, char *buffer, u32 buffer_cap)
{
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
