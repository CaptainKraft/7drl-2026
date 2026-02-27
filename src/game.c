#include <assert.h>
#include <string.h>

#include "app_config.h"
#include "ck/ck.h"

#include "game.h"
#include "item_art.h"
#include "unit_art.h"
#include "world_art.h"

#define UNITS_SHEET_PATH "assets/units.png"
#define ITEMS_SHEET_PATH "assets/items.png"
#define WORLD_SHEET_PATH "assets/world.png"
#define PREVIEW_TOP_MARGIN_Y 24.0f
#define PREVIEW_SECTION_GAP_Y 40.0f

#define UNIT_PREVIEW_CELL_GAP_X 14.0f
#define UNIT_PREVIEW_CELL_GAP_Y 18.0f
#define UNIT_PREVIEW_CELL_PADDING_X 10.0f
#define UNIT_PREVIEW_CELL_PADDING_Y 8.0f
#define UNIT_PREVIEW_LABEL_SIZE 16.0f
#define UNIT_PREVIEW_LABEL_GAP 6.0f
#define UNIT_PREVIEW_SPRITE_SCALE 2.0f
#define UNIT_PREVIEW_SPRITE_GAP 6.0f

#define ITEM_PREVIEW_CELL_GAP_X 14.0f
#define ITEM_PREVIEW_CELL_GAP_Y 18.0f
#define ITEM_PREVIEW_CELL_PADDING_X 10.0f
#define ITEM_PREVIEW_CELL_PADDING_Y 8.0f
#define ITEM_PREVIEW_LABEL_SIZE 16.0f
#define ITEM_PREVIEW_LABEL_GAP 6.0f
#define ITEM_PREVIEW_SPRITE_SCALE 3.0f

#define WORLD_PREVIEW_CELL_GAP_X 14.0f
#define WORLD_PREVIEW_CELL_GAP_Y 18.0f
#define WORLD_PREVIEW_CELL_PADDING_X 10.0f
#define WORLD_PREVIEW_CELL_PADDING_Y 8.0f
#define WORLD_PREVIEW_LABEL_SIZE 16.0f
#define WORLD_PREVIEW_LABEL_GAP 6.0f
#define WORLD_PREVIEW_SPRITE_SCALE 3.0f

#define WORLD_WALL_TEST_CELL_GAP_X 14.0f
#define WORLD_WALL_TEST_CELL_GAP_Y 18.0f
#define WORLD_WALL_TEST_CELL_PADDING_X 10.0f
#define WORLD_WALL_TEST_CELL_PADDING_Y 8.0f
#define WORLD_WALL_TEST_LABEL_SIZE 16.0f
#define WORLD_WALL_TEST_LABEL_GAP 6.0f
#define WORLD_WALL_TEST_DIRECTION_LABEL_SIZE 13.0f
#define WORLD_WALL_TEST_DIRECTION_LABEL_GAP 4.0f
#define WORLD_WALL_TEST_SPRITE_SCALE 2.5f
#define WORLD_WALL_TEST_SPRITE_GAP 6.0f

#define CAMERA_KEYBOARD_PAN_SPEED 1800.0f
#define CAMERA_FALLBACK_FRAME_TIME (1.0f / 60.0f)

#define DUNGEON_COL_COUNT 50
#define DUNGEON_ROW_COUNT 30
#define DUNGEON_TILE_SCALE 2.0f
#define DUNGEON_PANEL_PADDING 18.0f

#define DUNGEON_MAX_WORLD_FEATURES 12
#define DUNGEON_MAX_ITEMS 24
#define DUNGEON_MAX_UNITS 24

#define ITEM_KIND_GOLD_KEY ITEM_ART_KIND_AT(3, 0)
#define ITEM_KIND_SCROLL ITEM_ART_KIND_AT(6, 0)
#define ITEM_KIND_HEART ITEM_ART_KIND_AT(8, 0)
#define ITEM_KIND_RED_POTION ITEM_ART_KIND_AT(1, 1)
#define ITEM_KIND_RED_GEM ITEM_ART_KIND_AT(6, 1)
#define ITEM_KIND_BANDAGE ITEM_ART_KIND_AT(13, 3)
#define ITEM_KIND_SHORT_SWORD ITEM_ART_KIND_AT(0, 4)
#define ITEM_KIND_BOW ITEM_ART_KIND_AT(9, 4)
#define ITEM_KIND_WOODEN_SHIELD ITEM_ART_KIND_AT(12, 4)
#define ITEM_KIND_GOLD_COIN ITEM_ART_KIND_AT(15, 1)
#define ITEM_KIND_GREEN_HERB ITEM_ART_KIND_AT(9, 5)
#define ITEM_KIND_MAP ITEM_ART_KIND_AT(15, 5)

typedef enum {
    INPUT_NONE,
    INPUT_MOVE_UP,
    INPUT_MOVE_DOWN,
    INPUT_MOVE_LEFT,
    INPUT_MOVE_RIGHT,
    INPUT_ACTION,
    INPUT_BACK,
    INPUT_TOGGLE_VIEW,
    INPUT_SELECT_WALL_THEME_1,
    INPUT_SELECT_WALL_THEME_2,
    INPUT_SELECT_WALL_THEME_3,
    INPUT_SELECT_WALL_THEME_4,
    INPUT_MOUSE_LEFT,
    INPUT_MOUSE_RIGHT,
    INPUT_MOUSE_MIDDLE,
    NUM_INPUTS,
} INPUT;

typedef struct {
    bool down[NUM_INPUTS];
    bool pressed[NUM_INPUTS];
    bool released[NUM_INPUTS];
} Input;

typedef enum {
    DUNGEON_CELL_WALL,
    DUNGEON_CELL_FLOOR,
} DUNGEON_CELL;

typedef struct {
    i16 x;
    i16 y;
    World_Art_Tile tile;
} Dungeon_World_Feature;

typedef struct {
    i16 x;
    i16 y;
    ITEM_ART_KIND kind;
} Dungeon_Item;

typedef struct {
    i16 x;
    i16 y;
    UNIT_ART_KIND kind;
    u8 orientation;
} Dungeon_Unit;

typedef struct {
    u8 floor_row;
    u8 variation_start;
    u8 variation_count;
} Dungeon_Floor_Palette;

typedef struct {
    Input input;
    Camera2D dungeon_cam;
    Camera2D preview_cam;
    Font font;
    float font_spacing;
    bool show_dungeon_map;
    WORLD_ART_THEME dungeon_wall_theme;
    Texture2D units_texture;
    Texture2D items_texture;
    Texture2D world_texture;

    u8 dungeon_cells[DUNGEON_ROW_COUNT][DUNGEON_COL_COUNT];

    Dungeon_World_Feature world_features[DUNGEON_MAX_WORLD_FEATURES];
    u8 world_feature_count;

    Dungeon_Item items[DUNGEON_MAX_ITEMS];
    u8 item_count;

    Dungeon_Unit units[DUNGEON_MAX_UNITS];
    u8 unit_count;
} Game;

static int game_unit_anim_frame(void)
{
    int frame = (int)(GetTime() * 5.0f);
    frame %= UNIT_ART_ANIM_FRAME_COUNT;
    if (frame < 0)
        frame += UNIT_ART_ANIM_FRAME_COUNT;
    return frame;
}

static int game_item_anim_frame(void)
{
    int frame = (int)(GetTime() * 5.0f);
    frame %= ITEM_ART_ANIM_FRAME_COUNT;
    if (frame < 0)
        frame += ITEM_ART_ANIM_FRAME_COUNT;
    return frame;
}

static bool game_dungeon_cell_in_bounds(i32 x, i32 y)
{
    return x >= 0 && x < DUNGEON_COL_COUNT && y >= 0 && y < DUNGEON_ROW_COUNT;
}

static bool game_dungeon_cell_is_floor(const Game *game, i32 x, i32 y)
{
    if (!game_dungeon_cell_in_bounds(x, y))
        return false;
    return game->dungeon_cells[y][x] == DUNGEON_CELL_FLOOR;
}

static void game_dungeon_fill(Game *game, DUNGEON_CELL cell)
{
    for (i32 y = 0; y < DUNGEON_ROW_COUNT; y++) {
        for (i32 x = 0; x < DUNGEON_COL_COUNT; x++)
            game->dungeon_cells[y][x] = (u8)cell;
    }
}

static void game_dungeon_carve_rect(Game *game, i32 x, i32 y, i32 w, i32 h)
{
    i32 x0 = max(x, 0);
    i32 y0 = max(y, 0);
    i32 x1 = min(x + w, DUNGEON_COL_COUNT);
    i32 y1 = min(y + h, DUNGEON_ROW_COUNT);

    if (x1 <= x0 || y1 <= y0)
        return;

    for (i32 row = y0; row < y1; row++) {
        for (i32 col = x0; col < x1; col++)
            game->dungeon_cells[row][col] = DUNGEON_CELL_FLOOR;
    }
}

static void game_dungeon_carve_h_tunnel(Game *game, i32 x0, i32 x1, i32 y)
{
    if (y < 0 || y >= DUNGEON_ROW_COUNT)
        return;

    i32 start_x = min(x0, x1);
    i32 end_x = max(x0, x1);

    start_x = max(start_x, 0);
    end_x = min(end_x, DUNGEON_COL_COUNT - 1);

    for (i32 x = start_x; x <= end_x; x++)
        game->dungeon_cells[y][x] = DUNGEON_CELL_FLOOR;
}

static void game_dungeon_carve_v_tunnel(Game *game, i32 x, i32 y0, i32 y1)
{
    if (x < 0 || x >= DUNGEON_COL_COUNT)
        return;

    i32 start_y = min(y0, y1);
    i32 end_y = max(y0, y1);

    start_y = max(start_y, 0);
    end_y = min(end_y, DUNGEON_ROW_COUNT - 1);

    for (i32 y = start_y; y <= end_y; y++)
        game->dungeon_cells[y][x] = DUNGEON_CELL_FLOOR;
}

static void game_dungeon_add_world_feature(Game *game, i32 x, i32 y, World_Art_Tile tile)
{
    assert(game->world_feature_count < DUNGEON_MAX_WORLD_FEATURES);
    assert(game_dungeon_cell_in_bounds(x, y));
    assert(game_dungeon_cell_is_floor(game, x, y));

    game->world_features[game->world_feature_count++] = (Dungeon_World_Feature){
        .x = (i16)x,
        .y = (i16)y,
        .tile = tile,
    };
}

static void game_dungeon_add_item(Game *game, i32 x, i32 y, ITEM_ART_KIND kind)
{
    assert(game->item_count < DUNGEON_MAX_ITEMS);
    assert(kind > ITEM_ART_NONE && kind < NUM_ITEM_ART);
    assert(game_dungeon_cell_in_bounds(x, y));
    assert(game_dungeon_cell_is_floor(game, x, y));

    game->items[game->item_count++] = (Dungeon_Item){
        .x = (i16)x,
        .y = (i16)y,
        .kind = kind,
    };
}

static void game_dungeon_add_unit(Game *game, i32 x, i32 y, UNIT_ART_KIND kind, u8 orientation)
{
    assert(game->unit_count < DUNGEON_MAX_UNITS);
    assert(kind > UNIT_ART_NONE && kind < NUM_UNIT_ART);
    assert(orientation < UNIT_ART_ORIENTATION_COUNT);
    assert(game_dungeon_cell_in_bounds(x, y));
    assert(game_dungeon_cell_is_floor(game, x, y));

    game->units[game->unit_count++] = (Dungeon_Unit){
        .x = (i16)x,
        .y = (i16)y,
        .kind = kind,
        .orientation = orientation,
    };
}

static void game_build_test_dungeon(Game *game)
{
    game->world_feature_count = 0;
    game->item_count = 0;
    game->unit_count = 0;

    game_dungeon_fill(game, DUNGEON_CELL_WALL);

    game_dungeon_carve_rect(game, 2, 2, 12, 8);   // entry
    game_dungeon_carve_rect(game, 20, 3, 14, 10); // hall
    game_dungeon_carve_rect(game, 38, 4, 12, 9);  // barracks

    game_dungeon_carve_rect(game, 4, 17, 10, 10); // kennels
    game_dungeon_carve_rect(game, 18, 18, 16, 9); // crypt
    game_dungeon_carve_rect(game, 40, 19, 10, 8); // treasury

    game_dungeon_carve_h_tunnel(game, 13, 20, 5);
    game_dungeon_carve_h_tunnel(game, 33, 38, 7);

    game_dungeon_carve_v_tunnel(game, 27, 12, 18);
    game_dungeon_carve_h_tunnel(game, 13, 18, 22);
    game_dungeon_carve_h_tunnel(game, 33, 40, 22);

    game_dungeon_carve_v_tunnel(game, 9, 9, 17);
    game_dungeon_carve_h_tunnel(game, 9, 18, 17);
    game_dungeon_carve_v_tunnel(game, 44, 12, 19);

    game_dungeon_add_world_feature(game, 5, 5, world_art_get_up_stairs_tile(WORLD_ART_THEME_1));
    game_dungeon_add_world_feature(game, 46, 23, world_art_get_down_stairs_tile(WORLD_ART_THEME_2));
    game_dungeon_add_world_feature(game, 26, 7, world_art_get_misc1_tile(WORLD_ART_THEME_1));
    game_dungeon_add_world_feature(game, 27, 7, world_art_get_misc2_tile(WORLD_ART_THEME_1));
    game_dungeon_add_world_feature(game, 45, 9, world_art_get_misc1_tile(WORLD_ART_THEME_2));

    game_dungeon_add_item(game, 7, 5, ITEM_KIND_SHORT_SWORD);
    game_dungeon_add_item(game, 8, 5, ITEM_KIND_WOODEN_SHIELD);
    game_dungeon_add_item(game, 24, 6, ITEM_KIND_SCROLL);
    game_dungeon_add_item(game, 25, 8, ITEM_KIND_RED_POTION);
    game_dungeon_add_item(game, 45, 7, ITEM_KIND_GOLD_KEY);
    game_dungeon_add_item(game, 42, 10, ITEM_KIND_BOW);
    game_dungeon_add_item(game, 10, 21, ITEM_KIND_BANDAGE);
    game_dungeon_add_item(game, 30, 23, ITEM_KIND_RED_GEM);
    game_dungeon_add_item(game, 23, 20, ITEM_KIND_GREEN_HERB);
    game_dungeon_add_item(game, 41, 21, ITEM_KIND_MAP);
    game_dungeon_add_item(game, 47, 22, ITEM_KIND_GOLD_COIN);
    game_dungeon_add_item(game, 29, 20, ITEM_KIND_HEART);
    game_dungeon_add_item(game, 15, 5, ITEM_ART_TORCH_1);
    game_dungeon_add_item(game, 37, 7, ITEM_ART_TORCH_2);
    game_dungeon_add_item(game, 40, 22, ITEM_ART_TORCH_1);

    game_dungeon_add_unit(game, 6, 6, UNIT_ART_ADVENTURER, 1);
    game_dungeon_add_unit(game, 8, 6, UNIT_ART_DOG, 1);
    game_dungeon_add_unit(game, 24, 5, UNIT_ART_GOBLIN_GRUNT, 2);
    game_dungeon_add_unit(game, 30, 9, UNIT_ART_GOBLIN_SHAMAN, 3);
    game_dungeon_add_unit(game, 43, 7, UNIT_ART_TROLL, 2);
    game_dungeon_add_unit(game, 10, 23, UNIT_ART_MERCHANT, 0);
    game_dungeon_add_unit(game, 6, 20, UNIT_ART_SLIME, 1);
    game_dungeon_add_unit(game, 21, 22, UNIT_ART_SKELETON_GRUNT, 2);
    game_dungeon_add_unit(game, 27, 24, UNIT_ART_SKELETON_MAGE, 3);
    game_dungeon_add_unit(game, 45, 22, UNIT_ART_MIMIC, 0);
    game_dungeon_add_unit(game, 47, 24, UNIT_ART_MINOTAUR, 2);
    game_dungeon_add_unit(game, 42, 24, UNIT_ART_DRAGON, 3);
}

static u32 game_dungeon_hash(i32 x, i32 y)
{
    u32 ux = (u32)(x + 97);
    u32 uy = (u32)(y + 131);
    return (ux * 73856093u) ^ (uy * 19349663u);
}

static u8 game_dungeon_get_variation(i32 x, i32 y, u8 variation_count)
{
    assert(variation_count > 0);
    return (u8)(game_dungeon_hash(x, y) % variation_count);
}

static bool game_dungeon_cell_in_rect(i32 x, i32 y, i32 rx, i32 ry, i32 rw, i32 rh)
{
    return x >= rx && x < (rx + rw) && y >= ry && y < (ry + rh);
}

static Dungeon_Floor_Palette game_dungeon_get_floor_palette(i32 x, i32 y)
{
    if (game_dungeon_cell_in_rect(x, y, 2, 2, 12, 8))
        return (Dungeon_Floor_Palette){.floor_row = 0, .variation_start = 0, .variation_count = 8};
    if (game_dungeon_cell_in_rect(x, y, 20, 3, 14, 10))
        return (Dungeon_Floor_Palette){.floor_row = 0, .variation_start = 8, .variation_count = 8};
    if (game_dungeon_cell_in_rect(x, y, 38, 4, 12, 9))
        return (Dungeon_Floor_Palette){.floor_row = 1, .variation_start = 0, .variation_count = 8};
    if (game_dungeon_cell_in_rect(x, y, 4, 17, 10, 10))
        return (Dungeon_Floor_Palette){.floor_row = 1, .variation_start = 8, .variation_count = 8};
    if (game_dungeon_cell_in_rect(x, y, 18, 18, 16, 9))
        return (Dungeon_Floor_Palette){.floor_row = 3, .variation_start = 6, .variation_count = 5};
    if (game_dungeon_cell_in_rect(x, y, 40, 19, 10, 8))
        return (Dungeon_Floor_Palette){.floor_row = 3, .variation_start = 11, .variation_count = 5};

    return (Dungeon_Floor_Palette){.floor_row = 0, .variation_start = 0, .variation_count = 8};
}

static bool game_dungeon_wall_shows_face(const Game *game, i32 x, i32 y)
{
    return game_dungeon_cell_is_floor(game, x, y + 1);
}

static WORLD_WALL_DIRECTION game_get_opposite_wall_direction(WORLD_WALL_DIRECTION direction)
{
    return (WORLD_WALL_DIRECTION)((direction + 2) % NUM_WORLD_WALL_DIRECTIONS);
}

static World_Art_Tile game_dungeon_get_floor_tile(i32 x, i32 y)
{
    Dungeon_Floor_Palette palette = game_dungeon_get_floor_palette(x, y);
    u32 hash = game_dungeon_hash(x, y);
    assert(palette.floor_row < WORLD_ART_THEME_COUNT);
    assert(palette.variation_count > 0);

    u8 variation = (u8)(palette.variation_start + (u8)(hash % (u32)palette.variation_count));
    return world_art_get_floor_tile(palette.floor_row, variation);
}

static void game_draw_world_tile(Game *game, World_Art_Tile tile, Vector2 top_left,
                                 float sprite_size, float rotation_degrees)
{
    Rectangle src = {
        .x = (float)(tile.x * WORLD_ART_TILE_SIZE),
        .y = (float)(tile.y * WORLD_ART_TILE_SIZE),
        .width = (float)WORLD_ART_TILE_SIZE,
        .height = (float)WORLD_ART_TILE_SIZE,
    };

    Rectangle dst = {
        .x = top_left.x + (sprite_size * 0.5f),
        .y = top_left.y + (sprite_size * 0.5f),
        .width = sprite_size,
        .height = sprite_size,
    };

    Vector2 origin = {sprite_size * 0.5f, sprite_size * 0.5f};
    DrawTexturePro(game->world_texture, src, dst, origin, rotation_degrees, WHITE);
}

static void game_draw_item_tile(Game *game, ITEM_ART_KIND kind, Vector2 top_left, float sprite_size,
                                int anim_frame)
{
    ITEM_ART_KIND draw_kind = kind;
    if (draw_kind == ITEM_ART_TORCH_1 || draw_kind == ITEM_ART_TORCH_2) {
        bool second_torch = draw_kind == ITEM_ART_TORCH_2;
        bool use_second_frame = ((anim_frame + (second_torch ? 1 : 0)) % 2) != 0;
        draw_kind = use_second_frame ? ITEM_ART_TORCH_2 : ITEM_ART_TORCH_1;
    }

    Item_Art_Pixel pixel = item_art_get_pixel(draw_kind);
    Rectangle src = {
        .x = (float)pixel.x,
        .y = (float)pixel.y,
        .width = (float)ITEM_ART_TILE_SIZE,
        .height = (float)ITEM_ART_TILE_SIZE,
    };

    Rectangle dst = {
        .x = top_left.x,
        .y = top_left.y,
        .width = sprite_size,
        .height = sprite_size,
    };

    DrawTexturePro(game->items_texture, src, dst, (Vector2){0}, 0.0f, WHITE);
}

static void game_draw_unit_tile(Game *game, UNIT_ART_KIND kind, u8 orientation, Vector2 top_left,
                                float sprite_size, int anim_frame)
{
    Unit_Art_Pixel pixel = unit_art_get_pixel(kind, (u8)anim_frame, orientation);
    Rectangle src = {
        .x = (float)pixel.x,
        .y = (float)pixel.y,
        .width = (float)UNIT_ART_TILE_SIZE,
        .height = (float)UNIT_ART_TILE_SIZE,
    };

    Rectangle dst = {
        .x = top_left.x,
        .y = top_left.y,
        .width = sprite_size,
        .height = sprite_size,
    };

    DrawTexturePro(game->units_texture, src, dst, (Vector2){0}, 0.0f, WHITE);
}

static Vector2 game_get_unit_art_preview_size(Game *game)
{
    float sprite_size = UNIT_ART_TILE_SIZE * UNIT_PREVIEW_SPRITE_SCALE;
    float sprite_row_width = (sprite_size * UNIT_ART_ORIENTATION_COUNT) +
                             ((UNIT_ART_ORIENTATION_COUNT - 1) * UNIT_PREVIEW_SPRITE_GAP);

    float max_name_width = 0.0f;
    for (i32 kind = UNIT_ART_NONE + 1; kind < NUM_UNIT_ART; kind++) {
        char display_name[UNIT_ART_DISPLAY_NAME_CAP];
        unit_art_get_display_name((UNIT_ART_KIND)kind, display_name, UNIT_ART_DISPLAY_NAME_CAP);

        Vector2 text_size =
            MeasureTextEx(game->font, display_name, UNIT_PREVIEW_LABEL_SIZE, game->font_spacing);
        max_name_width = max(max_name_width, text_size.x);
    }

    i32 grid_cols = UNIT_ART_COL_COUNT;
    i32 grid_rows = UNIT_ART_ROW_COUNT;
    float cell_w = max(max_name_width, sprite_row_width) + (UNIT_PREVIEW_CELL_PADDING_X * 2.0f);
    float cell_h = (UNIT_PREVIEW_CELL_PADDING_Y * 2.0f) + UNIT_PREVIEW_LABEL_SIZE +
                   UNIT_PREVIEW_LABEL_GAP + sprite_size;

    return (Vector2){
        .x = (float)grid_cols * cell_w + (float)(grid_cols - 1) * UNIT_PREVIEW_CELL_GAP_X,
        .y = (float)grid_rows * cell_h + (float)(grid_rows - 1) * UNIT_PREVIEW_CELL_GAP_Y,
    };
}

static Vector2 game_get_item_art_preview_size(Game *game)
{
    float sprite_size = ITEM_ART_TILE_SIZE * ITEM_PREVIEW_SPRITE_SCALE;

    float max_name_width = 0.0f;
    for (i32 kind = ITEM_ART_NONE + 1; kind < NUM_ITEM_ART; kind++) {
        char display_name[ITEM_ART_DISPLAY_NAME_CAP];
        item_art_get_display_name((ITEM_ART_KIND)kind, display_name, ITEM_ART_DISPLAY_NAME_CAP);

        Vector2 text_size =
            MeasureTextEx(game->font, display_name, ITEM_PREVIEW_LABEL_SIZE, game->font_spacing);
        max_name_width = max(max_name_width, text_size.x);
    }

    i32 grid_cols = ITEM_ART_COL_COUNT;
    i32 grid_rows = ITEM_ART_ROW_COUNT;
    float cell_w = max(max_name_width, sprite_size) + (ITEM_PREVIEW_CELL_PADDING_X * 2.0f);
    float cell_h = (ITEM_PREVIEW_CELL_PADDING_Y * 2.0f) + ITEM_PREVIEW_LABEL_SIZE +
                   ITEM_PREVIEW_LABEL_GAP + sprite_size;

    return (Vector2){
        .x = (float)grid_cols * cell_w + (float)(grid_cols - 1) * ITEM_PREVIEW_CELL_GAP_X,
        .y = (float)grid_rows * cell_h + (float)(grid_rows - 1) * ITEM_PREVIEW_CELL_GAP_Y,
    };
}

static void game_draw_unit_art_preview(Game *game, Vector2 origin)
{
    float sprite_size = UNIT_ART_TILE_SIZE * UNIT_PREVIEW_SPRITE_SCALE;
    float sprite_row_width = (sprite_size * UNIT_ART_ORIENTATION_COUNT) +
                             ((UNIT_ART_ORIENTATION_COUNT - 1) * UNIT_PREVIEW_SPRITE_GAP);

    float max_name_width = 0.0f;
    for (i32 kind = UNIT_ART_NONE + 1; kind < NUM_UNIT_ART; kind++) {
        char display_name[UNIT_ART_DISPLAY_NAME_CAP];
        unit_art_get_display_name((UNIT_ART_KIND)kind, display_name, UNIT_ART_DISPLAY_NAME_CAP);

        Vector2 text_size =
            MeasureTextEx(game->font, display_name, UNIT_PREVIEW_LABEL_SIZE, game->font_spacing);
        max_name_width = max(max_name_width, text_size.x);
    }

    i32 grid_cols = UNIT_ART_COL_COUNT;
    i32 grid_rows = UNIT_ART_ROW_COUNT;
    float cell_w = max(max_name_width, sprite_row_width) + (UNIT_PREVIEW_CELL_PADDING_X * 2.0f);
    float cell_h = (UNIT_PREVIEW_CELL_PADDING_Y * 2.0f) + UNIT_PREVIEW_LABEL_SIZE +
                   UNIT_PREVIEW_LABEL_GAP + sprite_size;

    Vector2 content_size = game_get_unit_art_preview_size(game);

    Rectangle content_panel = {
        .x = origin.x - UNIT_PREVIEW_CELL_PADDING_X,
        .y = origin.y - UNIT_PREVIEW_CELL_PADDING_Y,
        .width = content_size.x + (UNIT_PREVIEW_CELL_PADDING_X * 2.0f),
        .height = content_size.y + (UNIT_PREVIEW_CELL_PADDING_Y * 2.0f),
    };
    DrawRectangleRounded(content_panel, 0.04f, 8, (Color){20, 33, 38, 255});
    DrawRectangleLinesEx(content_panel, 1.0f, (Color){55, 79, 86, 255});

    int anim_frame = game_unit_anim_frame();
    for (i32 kind = UNIT_ART_NONE + 1; kind < NUM_UNIT_ART; kind++) {
        i32 draw_idx = kind - 1;
        i32 col = draw_idx % grid_cols;
        i32 row = draw_idx / grid_cols;

        float cell_x = origin.x + (float)col * (cell_w + UNIT_PREVIEW_CELL_GAP_X);
        float cell_y = origin.y + (float)row * (cell_h + UNIT_PREVIEW_CELL_GAP_Y);

        Rectangle panel = {cell_x, cell_y, cell_w, cell_h};
        DrawRectangleRounded(panel, 0.08f, 4, (Color){32, 47, 52, 255});
        DrawRectangleLinesEx(panel, 1.0f, (Color){74, 97, 104, 255});

        char display_name[UNIT_ART_DISPLAY_NAME_CAP];
        unit_art_get_display_name((UNIT_ART_KIND)kind, display_name, UNIT_ART_DISPLAY_NAME_CAP);

        Vector2 text_size =
            MeasureTextEx(game->font, display_name, UNIT_PREVIEW_LABEL_SIZE, game->font_spacing);
        Vector2 text_pos = {
            .x = cell_x + (cell_w - text_size.x) * 0.5f,
            .y = cell_y + UNIT_PREVIEW_CELL_PADDING_Y,
        };
        DrawTextEx(game->font, display_name, text_pos, UNIT_PREVIEW_LABEL_SIZE, game->font_spacing,
                   RAYWHITE);

        float sprite_y = text_pos.y + text_size.y + UNIT_PREVIEW_LABEL_GAP;
        float sprite_row_x = cell_x + (cell_w - sprite_row_width) * 0.5f;

        for (i32 orientation = 0; orientation < UNIT_ART_ORIENTATION_COUNT; orientation++) {
            Vector2 top_left = {
                .x = sprite_row_x + (float)orientation * (sprite_size + UNIT_PREVIEW_SPRITE_GAP),
                .y = sprite_y,
            };
            game_draw_unit_tile(game, (UNIT_ART_KIND)kind, (u8)orientation, top_left, sprite_size,
                                anim_frame);
        }
    }
}

static void game_draw_item_art_preview(Game *game, Vector2 origin)
{
    float sprite_size = ITEM_ART_TILE_SIZE * ITEM_PREVIEW_SPRITE_SCALE;
    int anim_frame = game_item_anim_frame();

    float max_name_width = 0.0f;
    for (i32 kind = ITEM_ART_NONE + 1; kind < NUM_ITEM_ART; kind++) {
        char display_name[ITEM_ART_DISPLAY_NAME_CAP];
        item_art_get_display_name((ITEM_ART_KIND)kind, display_name, ITEM_ART_DISPLAY_NAME_CAP);

        Vector2 text_size =
            MeasureTextEx(game->font, display_name, ITEM_PREVIEW_LABEL_SIZE, game->font_spacing);
        max_name_width = max(max_name_width, text_size.x);
    }

    i32 grid_cols = ITEM_ART_COL_COUNT;
    i32 grid_rows = ITEM_ART_ROW_COUNT;
    float cell_w = max(max_name_width, sprite_size) + (ITEM_PREVIEW_CELL_PADDING_X * 2.0f);
    float cell_h = (ITEM_PREVIEW_CELL_PADDING_Y * 2.0f) + ITEM_PREVIEW_LABEL_SIZE +
                   ITEM_PREVIEW_LABEL_GAP + sprite_size;

    Vector2 content_size = game_get_item_art_preview_size(game);

    Rectangle content_panel = {
        .x = origin.x - ITEM_PREVIEW_CELL_PADDING_X,
        .y = origin.y - ITEM_PREVIEW_CELL_PADDING_Y,
        .width = content_size.x + (ITEM_PREVIEW_CELL_PADDING_X * 2.0f),
        .height = content_size.y + (ITEM_PREVIEW_CELL_PADDING_Y * 2.0f),
    };
    DrawRectangleRounded(content_panel, 0.04f, 8, (Color){20, 33, 38, 255});
    DrawRectangleLinesEx(content_panel, 1.0f, (Color){55, 79, 86, 255});

    for (i32 kind = ITEM_ART_NONE + 1; kind < NUM_ITEM_ART; kind++) {
        i32 draw_idx = kind - 1;
        i32 col = draw_idx % grid_cols;
        i32 row = draw_idx / grid_cols;

        float cell_x = origin.x + (float)col * (cell_w + ITEM_PREVIEW_CELL_GAP_X);
        float cell_y = origin.y + (float)row * (cell_h + ITEM_PREVIEW_CELL_GAP_Y);

        Rectangle panel = {cell_x, cell_y, cell_w, cell_h};
        DrawRectangleRounded(panel, 0.08f, 4, (Color){32, 47, 52, 255});
        DrawRectangleLinesEx(panel, 1.0f, (Color){74, 97, 104, 255});

        char display_name[ITEM_ART_DISPLAY_NAME_CAP];
        item_art_get_display_name((ITEM_ART_KIND)kind, display_name, ITEM_ART_DISPLAY_NAME_CAP);

        Vector2 text_size =
            MeasureTextEx(game->font, display_name, ITEM_PREVIEW_LABEL_SIZE, game->font_spacing);
        Vector2 text_pos = {
            .x = cell_x + (cell_w - text_size.x) * 0.5f,
            .y = cell_y + ITEM_PREVIEW_CELL_PADDING_Y,
        };
        DrawTextEx(game->font, display_name, text_pos, ITEM_PREVIEW_LABEL_SIZE, game->font_spacing,
                   RAYWHITE);

        float sprite_y = text_pos.y + text_size.y + ITEM_PREVIEW_LABEL_GAP;
        float sprite_x = cell_x + (cell_w - sprite_size) * 0.5f;

        game_draw_item_tile(game, (ITEM_ART_KIND)kind, (Vector2){sprite_x, sprite_y}, sprite_size,
                            anim_frame);
    }
}

static Vector2 game_get_world_art_preview_size(Game *game)
{
    float sprite_size = WORLD_ART_TILE_SIZE * WORLD_PREVIEW_SPRITE_SCALE;

    float max_name_width = 0.0f;
    for (i32 kind = WORLD_ART_NONE + 1; kind < NUM_WORLD_ART; kind++) {
        char display_name[WORLD_ART_DISPLAY_NAME_CAP];
        world_art_get_display_name((WORLD_ART_KIND)kind, display_name, WORLD_ART_DISPLAY_NAME_CAP);

        Vector2 text_size =
            MeasureTextEx(game->font, display_name, WORLD_PREVIEW_LABEL_SIZE, game->font_spacing);
        max_name_width = max(max_name_width, text_size.x);
    }

    i32 grid_cols = WORLD_ART_COL_COUNT;
    i32 grid_rows = WORLD_ART_DATA_ROW_COUNT;
    float cell_w = max(max_name_width, sprite_size) + (WORLD_PREVIEW_CELL_PADDING_X * 2.0f);
    float cell_h = (WORLD_PREVIEW_CELL_PADDING_Y * 2.0f) + WORLD_PREVIEW_LABEL_SIZE +
                   WORLD_PREVIEW_LABEL_GAP + sprite_size;

    return (Vector2){
        .x = (float)grid_cols * cell_w + (float)(grid_cols - 1) * WORLD_PREVIEW_CELL_GAP_X,
        .y = (float)grid_rows * cell_h + (float)(grid_rows - 1) * WORLD_PREVIEW_CELL_GAP_Y,
    };
}

static void game_draw_world_art_preview(Game *game, Vector2 origin)
{
    float sprite_size = WORLD_ART_TILE_SIZE * WORLD_PREVIEW_SPRITE_SCALE;

    float max_name_width = 0.0f;
    for (i32 kind = WORLD_ART_NONE + 1; kind < NUM_WORLD_ART; kind++) {
        char display_name[WORLD_ART_DISPLAY_NAME_CAP];
        world_art_get_display_name((WORLD_ART_KIND)kind, display_name, WORLD_ART_DISPLAY_NAME_CAP);

        Vector2 text_size =
            MeasureTextEx(game->font, display_name, WORLD_PREVIEW_LABEL_SIZE, game->font_spacing);
        max_name_width = max(max_name_width, text_size.x);
    }

    i32 grid_cols = WORLD_ART_COL_COUNT;
    i32 grid_rows = WORLD_ART_DATA_ROW_COUNT;
    float cell_w = max(max_name_width, sprite_size) + (WORLD_PREVIEW_CELL_PADDING_X * 2.0f);
    float cell_h = (WORLD_PREVIEW_CELL_PADDING_Y * 2.0f) + WORLD_PREVIEW_LABEL_SIZE +
                   WORLD_PREVIEW_LABEL_GAP + sprite_size;

    Vector2 content_size = game_get_world_art_preview_size(game);

    Rectangle content_panel = {
        .x = origin.x - WORLD_PREVIEW_CELL_PADDING_X,
        .y = origin.y - WORLD_PREVIEW_CELL_PADDING_Y,
        .width = content_size.x + (WORLD_PREVIEW_CELL_PADDING_X * 2.0f),
        .height = content_size.y + (WORLD_PREVIEW_CELL_PADDING_Y * 2.0f),
    };
    DrawRectangleRounded(content_panel, 0.04f, 8, (Color){20, 33, 38, 255});
    DrawRectangleLinesEx(content_panel, 1.0f, (Color){55, 79, 86, 255});

    for (i32 kind = WORLD_ART_NONE + 1; kind < NUM_WORLD_ART; kind++) {
        i32 draw_idx = kind - 1;
        i32 col = draw_idx % grid_cols;
        i32 row = draw_idx / grid_cols;

        float cell_x = origin.x + (float)col * (cell_w + WORLD_PREVIEW_CELL_GAP_X);
        float cell_y = origin.y + (float)row * (cell_h + WORLD_PREVIEW_CELL_GAP_Y);

        Rectangle panel = {cell_x, cell_y, cell_w, cell_h};
        DrawRectangleRounded(panel, 0.08f, 4, (Color){32, 47, 52, 255});
        DrawRectangleLinesEx(panel, 1.0f, (Color){74, 97, 104, 255});

        char display_name[WORLD_ART_DISPLAY_NAME_CAP];
        world_art_get_display_name((WORLD_ART_KIND)kind, display_name, WORLD_ART_DISPLAY_NAME_CAP);

        Vector2 text_size =
            MeasureTextEx(game->font, display_name, WORLD_PREVIEW_LABEL_SIZE, game->font_spacing);
        Vector2 text_pos = {
            .x = cell_x + (cell_w - text_size.x) * 0.5f,
            .y = cell_y + WORLD_PREVIEW_CELL_PADDING_Y,
        };
        DrawTextEx(game->font, display_name, text_pos, WORLD_PREVIEW_LABEL_SIZE, game->font_spacing,
                   RAYWHITE);

        float sprite_y = text_pos.y + text_size.y + WORLD_PREVIEW_LABEL_GAP;
        float sprite_x = cell_x + (cell_w - sprite_size) * 0.5f;

        game_draw_world_tile(game, world_art_get_tile((WORLD_ART_KIND)kind),
                             (Vector2){sprite_x, sprite_y}, sprite_size, 0.0f);
    }
}

static float game_get_world_wall_test_direction_label_height(Game *game)
{
    float label_height = 0.0f;
    for (i32 direction = 0; direction < NUM_WORLD_WALL_DIRECTIONS; direction++) {
        const char *direction_name =
            world_art_get_wall_direction_name((WORLD_WALL_DIRECTION)direction);
        Vector2 text_size = MeasureTextEx(game->font, direction_name,
                                          WORLD_WALL_TEST_DIRECTION_LABEL_SIZE, game->font_spacing);
        label_height = max(label_height, text_size.y);
    }
    return label_height;
}

static Vector2 game_get_world_wall_test_preview_size(Game *game)
{
    float sprite_size = WORLD_ART_TILE_SIZE * WORLD_WALL_TEST_SPRITE_SCALE;
    float sprite_row_width = (sprite_size * NUM_WORLD_WALL_DIRECTIONS) +
                             ((NUM_WORLD_WALL_DIRECTIONS - 1) * WORLD_WALL_TEST_SPRITE_GAP);

    float max_name_width = 0.0f;
    for (i32 theme = 0; theme < WORLD_ART_THEME_COUNT; theme++) {
        for (i32 variation = 0; variation < WORLD_ART_WALL_VARIATION_COUNT; variation++) {
            World_Art_Tile tile =
                world_art_get_north_wall_tile((WORLD_ART_THEME)theme, (u8)variation);
            WORLD_ART_KIND kind = world_art_get_kind_from_tile(tile);

            char display_name[WORLD_ART_DISPLAY_NAME_CAP];
            world_art_get_display_name(kind, display_name, WORLD_ART_DISPLAY_NAME_CAP);

            Vector2 text_size = MeasureTextEx(game->font, display_name, WORLD_WALL_TEST_LABEL_SIZE,
                                              game->font_spacing);
            max_name_width = max(max_name_width, text_size.x);
        }
    }

    float direction_label_height = game_get_world_wall_test_direction_label_height(game);
    i32 grid_cols = WORLD_ART_WALL_VARIATION_COUNT;
    i32 grid_rows = WORLD_ART_THEME_COUNT;
    float cell_w = max(max_name_width, sprite_row_width) + (WORLD_WALL_TEST_CELL_PADDING_X * 2.0f);
    float cell_h = (WORLD_WALL_TEST_CELL_PADDING_Y * 2.0f) + WORLD_WALL_TEST_LABEL_SIZE +
                   WORLD_WALL_TEST_LABEL_GAP + direction_label_height +
                   WORLD_WALL_TEST_DIRECTION_LABEL_GAP + sprite_size;

    return (Vector2){
        .x = (float)grid_cols * cell_w + (float)(grid_cols - 1) * WORLD_WALL_TEST_CELL_GAP_X,
        .y = (float)grid_rows * cell_h + (float)(grid_rows - 1) * WORLD_WALL_TEST_CELL_GAP_Y,
    };
}

static void game_draw_world_wall_test_preview(Game *game, Vector2 origin)
{
    float sprite_size = WORLD_ART_TILE_SIZE * WORLD_WALL_TEST_SPRITE_SCALE;
    float sprite_row_width = (sprite_size * NUM_WORLD_WALL_DIRECTIONS) +
                             ((NUM_WORLD_WALL_DIRECTIONS - 1) * WORLD_WALL_TEST_SPRITE_GAP);

    float max_name_width = 0.0f;
    for (i32 theme = 0; theme < WORLD_ART_THEME_COUNT; theme++) {
        for (i32 variation = 0; variation < WORLD_ART_WALL_VARIATION_COUNT; variation++) {
            World_Art_Tile tile =
                world_art_get_north_wall_tile((WORLD_ART_THEME)theme, (u8)variation);
            WORLD_ART_KIND kind = world_art_get_kind_from_tile(tile);

            char display_name[WORLD_ART_DISPLAY_NAME_CAP];
            world_art_get_display_name(kind, display_name, WORLD_ART_DISPLAY_NAME_CAP);

            Vector2 text_size = MeasureTextEx(game->font, display_name, WORLD_WALL_TEST_LABEL_SIZE,
                                              game->font_spacing);
            max_name_width = max(max_name_width, text_size.x);
        }
    }

    float direction_label_height = game_get_world_wall_test_direction_label_height(game);
    i32 grid_cols = WORLD_ART_WALL_VARIATION_COUNT;
    i32 grid_rows = WORLD_ART_THEME_COUNT;
    float cell_w = max(max_name_width, sprite_row_width) + (WORLD_WALL_TEST_CELL_PADDING_X * 2.0f);
    float cell_h = (WORLD_WALL_TEST_CELL_PADDING_Y * 2.0f) + WORLD_WALL_TEST_LABEL_SIZE +
                   WORLD_WALL_TEST_LABEL_GAP + direction_label_height +
                   WORLD_WALL_TEST_DIRECTION_LABEL_GAP + sprite_size;

    Vector2 content_size = game_get_world_wall_test_preview_size(game);

    Rectangle content_panel = {
        .x = origin.x - WORLD_WALL_TEST_CELL_PADDING_X,
        .y = origin.y - WORLD_WALL_TEST_CELL_PADDING_Y,
        .width = content_size.x + (WORLD_WALL_TEST_CELL_PADDING_X * 2.0f),
        .height = content_size.y + (WORLD_WALL_TEST_CELL_PADDING_Y * 2.0f),
    };
    DrawRectangleRounded(content_panel, 0.04f, 8, (Color){20, 33, 38, 255});
    DrawRectangleLinesEx(content_panel, 1.0f, (Color){55, 79, 86, 255});

    for (i32 theme = 0; theme < grid_rows; theme++) {
        for (i32 variation = 0; variation < grid_cols; variation++) {
            float cell_x = origin.x + (float)variation * (cell_w + WORLD_WALL_TEST_CELL_GAP_X);
            float cell_y = origin.y + (float)theme * (cell_h + WORLD_WALL_TEST_CELL_GAP_Y);

            Rectangle panel = {cell_x, cell_y, cell_w, cell_h};
            DrawRectangleRounded(panel, 0.08f, 4, (Color){32, 47, 52, 255});
            DrawRectangleLinesEx(panel, 1.0f, (Color){74, 97, 104, 255});

            World_Art_Tile north_tile =
                world_art_get_north_wall_tile((WORLD_ART_THEME)theme, (u8)variation);
            WORLD_ART_KIND north_kind = world_art_get_kind_from_tile(north_tile);

            char display_name[WORLD_ART_DISPLAY_NAME_CAP];
            world_art_get_display_name(north_kind, display_name, WORLD_ART_DISPLAY_NAME_CAP);

            Vector2 title_size = MeasureTextEx(game->font, display_name, WORLD_WALL_TEST_LABEL_SIZE,
                                               game->font_spacing);
            Vector2 title_pos = {
                .x = cell_x + (cell_w - title_size.x) * 0.5f,
                .y = cell_y + WORLD_WALL_TEST_CELL_PADDING_Y,
            };
            DrawTextEx(game->font, display_name, title_pos, WORLD_WALL_TEST_LABEL_SIZE,
                       game->font_spacing, RAYWHITE);

            float direction_label_y = title_pos.y + title_size.y + WORLD_WALL_TEST_LABEL_GAP;
            float sprite_y =
                direction_label_y + direction_label_height + WORLD_WALL_TEST_DIRECTION_LABEL_GAP;
            float sprite_row_x = cell_x + (cell_w - sprite_row_width) * 0.5f;

            for (i32 direction = 0; direction < NUM_WORLD_WALL_DIRECTIONS; direction++) {
                float sprite_x =
                    sprite_row_x + (float)direction * (sprite_size + WORLD_WALL_TEST_SPRITE_GAP);

                const char *direction_name =
                    world_art_get_wall_direction_name((WORLD_WALL_DIRECTION)direction);
                Vector2 direction_size =
                    MeasureTextEx(game->font, direction_name, WORLD_WALL_TEST_DIRECTION_LABEL_SIZE,
                                  game->font_spacing);
                Vector2 direction_pos = {
                    .x = sprite_x + (sprite_size - direction_size.x) * 0.5f,
                    .y = direction_label_y,
                };
                DrawTextEx(game->font, direction_name, direction_pos,
                           WORLD_WALL_TEST_DIRECTION_LABEL_SIZE, game->font_spacing,
                           (Color){181, 211, 220, 255});

                WORLD_WALL_DIRECTION wall_direction =
                    game_get_opposite_wall_direction((WORLD_WALL_DIRECTION)direction);
                float rotation = world_art_get_wall_rotation_degrees(wall_direction);
                game_draw_world_tile(game, north_tile, (Vector2){sprite_x, sprite_y}, sprite_size,
                                     rotation);
            }
        }
    }
}

static void game_draw_art_previews(Game *game)
{
    Vector2 unit_preview_size = game_get_unit_art_preview_size(game);
    Vector2 item_preview_size = game_get_item_art_preview_size(game);
    Vector2 world_preview_size = game_get_world_art_preview_size(game);
    Vector2 wall_test_preview_size = game_get_world_wall_test_preview_size(game);

    Vector2 unit_origin = {
        .x = ((float)VIRTUAL_W - unit_preview_size.x) * 0.5f,
        .y = PREVIEW_TOP_MARGIN_Y,
    };

    Vector2 item_origin = {
        .x = ((float)VIRTUAL_W - item_preview_size.x) * 0.5f,
        .y = unit_origin.y + unit_preview_size.y + PREVIEW_SECTION_GAP_Y,
    };

    Vector2 world_origin = {
        .x = ((float)VIRTUAL_W - world_preview_size.x) * 0.5f,
        .y = item_origin.y + item_preview_size.y + PREVIEW_SECTION_GAP_Y,
    };

    Vector2 wall_test_origin = {
        .x = ((float)VIRTUAL_W - wall_test_preview_size.x) * 0.5f,
        .y = world_origin.y + world_preview_size.y + PREVIEW_SECTION_GAP_Y,
    };

    game_draw_unit_art_preview(game, unit_origin);
    game_draw_item_art_preview(game, item_origin);
    game_draw_world_art_preview(game, world_origin);
    game_draw_world_wall_test_preview(game, wall_test_origin);
}

static Vector2 game_dungeon_get_origin(void)
{
    float tile_size = WORLD_ART_TILE_SIZE * DUNGEON_TILE_SCALE;
    float map_w = (float)DUNGEON_COL_COUNT * tile_size;
    float map_h = (float)DUNGEON_ROW_COUNT * tile_size;
    return (Vector2){
        .x = ((float)VIRTUAL_W - map_w) * 0.5f,
        .y = ((float)VIRTUAL_H - map_h) * 0.5f,
    };
}

static Vector2 game_dungeon_get_cell_top_left(Vector2 origin, i32 x, i32 y, float tile_size)
{
    return (Vector2){
        .x = origin.x + (float)x * tile_size,
        .y = origin.y + (float)y * tile_size,
    };
}

static void game_draw_test_dungeon(Game *game)
{
    float tile_size = WORLD_ART_TILE_SIZE * DUNGEON_TILE_SCALE;
    float map_w = (float)DUNGEON_COL_COUNT * tile_size;
    float map_h = (float)DUNGEON_ROW_COUNT * tile_size;
    Vector2 origin = game_dungeon_get_origin();

    Rectangle panel = {
        .x = origin.x - DUNGEON_PANEL_PADDING,
        .y = origin.y - DUNGEON_PANEL_PADDING,
        .width = map_w + (DUNGEON_PANEL_PADDING * 2.0f),
        .height = map_h + (DUNGEON_PANEL_PADDING * 2.0f),
    };

    DrawRectangleRec(panel, (Color){20, 33, 38, 255});
    DrawRectangleLinesEx(panel, 1.0f, (Color){57, 82, 89, 255});

    for (i32 y = 0; y < DUNGEON_ROW_COUNT; y++) {
        for (i32 x = 0; x < DUNGEON_COL_COUNT; x++) {
            Vector2 top_left = game_dungeon_get_cell_top_left(origin, x, y, tile_size);

            if (game->dungeon_cells[y][x] == DUNGEON_CELL_FLOOR) {
                game_draw_world_tile(game, game_dungeon_get_floor_tile(x, y), top_left, tile_size,
                                     0.0f);
            } else {
                WORLD_ART_THEME theme = game->dungeon_wall_theme;
                if (game_dungeon_wall_shows_face(game, x, y)) {
                    u8 variation = game_dungeon_get_variation(x, y, WORLD_ART_WALL_VARIATION_COUNT);
                    game_draw_world_tile(game, world_art_get_north_wall_tile(theme, variation),
                                         top_left, tile_size, 0.0f);
                } else {
                    u8 variation =
                        game_dungeon_get_variation(x, y, WORLD_ART_WALL_TOP_VARIATION_COUNT);
                    game_draw_world_tile(game, world_art_get_top_wall_tile(theme, variation),
                                         top_left, tile_size, 0.0f);
                }
            }
        }
    }

    for (u8 feature_idx = 0; feature_idx < game->world_feature_count; feature_idx++) {
        Dungeon_World_Feature feature = game->world_features[feature_idx];
        Vector2 top_left = game_dungeon_get_cell_top_left(origin, feature.x, feature.y, tile_size);
        game_draw_world_tile(game, feature.tile, top_left, tile_size, 0.0f);
    }

    int item_anim_frame = game_item_anim_frame();
    for (u8 item_idx = 0; item_idx < game->item_count; item_idx++) {
        Dungeon_Item item = game->items[item_idx];
        Vector2 top_left = game_dungeon_get_cell_top_left(origin, item.x, item.y, tile_size);
        game_draw_item_tile(game, item.kind, top_left, tile_size, item_anim_frame);
    }

    int unit_anim_frame = game_unit_anim_frame();
    for (u8 unit_idx = 0; unit_idx < game->unit_count; unit_idx++) {
        Dungeon_Unit unit = game->units[unit_idx];
        Vector2 top_left = game_dungeon_get_cell_top_left(origin, unit.x, unit.y, tile_size);
        game_draw_unit_tile(game, unit.kind, unit.orientation, top_left, tile_size,
                            unit_anim_frame);
    }
}

static void game_update_camera(Game *game)
{
    Camera2D *cam = game->show_dungeon_map ? &game->dungeon_cam : &game->preview_cam;

    float frame_time = GetFrameTime();
    if (frame_time <= 0.0f)
        frame_time = CAMERA_FALLBACK_FRAME_TIME;

    float camera_step = CAMERA_KEYBOARD_PAN_SPEED * frame_time;

    if (game->input.down[INPUT_MOVE_UP] || IsKeyDown(KEY_W))
        cam->target.y -= camera_step;
    if (game->input.down[INPUT_MOVE_DOWN] || IsKeyDown(KEY_S))
        cam->target.y += camera_step;
    if (game->input.down[INPUT_MOVE_LEFT] || IsKeyDown(KEY_A))
        cam->target.x -= camera_step;
    if (game->input.down[INPUT_MOVE_RIGHT] || IsKeyDown(KEY_D))
        cam->target.x += camera_step;

    if (game->input.down[INPUT_MOUSE_MIDDLE]) {
        Vector2 mouse_delta = GetMouseDelta();
        cam->target.x -= mouse_delta.x / cam->zoom;
        cam->target.y -= mouse_delta.y / cam->zoom;
    }
}

void game_init(Mem mem, Font font, float font_spacing)
{
    Game *game = arena_push(mem.perm, sizeof(Game));
    memset(game, 0, sizeof(*game));

    if (font.texture.id == 0)
        font = GetFontDefault();
    game->font = font;
    game->font_spacing = font_spacing;

    game->dungeon_cam = (Camera2D){
        .offset = {0.0f, 0.0f},
        .target = {0.0f, 0.0f},
        .zoom = 1.0f,
    };
    game->preview_cam = game->dungeon_cam;

    game->units_texture = LoadTexture(UNITS_SHEET_PATH);
    assert(game->units_texture.id != 0);
    SetTextureFilter(game->units_texture, TEXTURE_FILTER_POINT);

    game->items_texture = LoadTexture(ITEMS_SHEET_PATH);
    assert(game->items_texture.id != 0);
    SetTextureFilter(game->items_texture, TEXTURE_FILTER_POINT);

    game->world_texture = LoadTexture(WORLD_SHEET_PATH);
    assert(game->world_texture.id != 0);
    SetTextureFilter(game->world_texture, TEXTURE_FILTER_POINT);

    game->show_dungeon_map = true;
    game->dungeon_wall_theme = WORLD_ART_THEME_1;
    game_build_test_dungeon(game);
}

void game_shutdown(Mem mem)
{
    Game *game = arena_start(mem.perm, Game);
    if (game->units_texture.id != 0)
        UnloadTexture(game->units_texture);
    if (game->items_texture.id != 0)
        UnloadTexture(game->items_texture);
    if (game->world_texture.id != 0)
        UnloadTexture(game->world_texture);
}

static void game_input(Game *game)
{
    Input *input = &game->input;
    for (int i = 0; i < NUM_INPUTS; i++) {
        input->down[i] = false;
        input->pressed[i] = false;
        input->released[i] = false;
    }

    input->down[INPUT_MOVE_UP] =
        IsKeyDown(KEY_W) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_UP);
    input->down[INPUT_MOVE_DOWN] =
        IsKeyDown(KEY_S) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN);
    input->down[INPUT_MOVE_LEFT] =
        IsKeyDown(KEY_A) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT);
    input->down[INPUT_MOVE_RIGHT] =
        IsKeyDown(KEY_D) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT);
    input->down[INPUT_ACTION] =
        IsKeyDown(KEY_SPACE) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
    input->down[INPUT_MOUSE_LEFT] = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    input->down[INPUT_MOUSE_RIGHT] = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
    input->down[INPUT_MOUSE_MIDDLE] = IsMouseButtonDown(MOUSE_BUTTON_MIDDLE);

    input->pressed[INPUT_ACTION] =
        IsKeyPressed(KEY_SPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
    input->pressed[INPUT_MOUSE_LEFT] = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    input->pressed[INPUT_MOUSE_RIGHT] = IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
    input->pressed[INPUT_MOUSE_MIDDLE] = IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE);
    input->pressed[INPUT_BACK] = IsKeyPressed(KEY_ESCAPE);
    input->pressed[INPUT_TOGGLE_VIEW] = IsKeyPressed(KEY_TAB);
    input->pressed[INPUT_SELECT_WALL_THEME_1] = IsKeyPressed(KEY_ONE);
    input->pressed[INPUT_SELECT_WALL_THEME_2] = IsKeyPressed(KEY_TWO);
    input->pressed[INPUT_SELECT_WALL_THEME_3] = IsKeyPressed(KEY_THREE);
    input->pressed[INPUT_SELECT_WALL_THEME_4] = IsKeyPressed(KEY_FOUR);

    input->released[INPUT_MOUSE_LEFT] = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
    input->released[INPUT_MOUSE_RIGHT] = IsMouseButtonReleased(MOUSE_BUTTON_RIGHT);
    input->released[INPUT_MOUSE_MIDDLE] = IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE);
}

void game_update(Mem mem)
{
    Game *game = arena_start(mem.perm, Game);
    arena_clear(mem.tmp);

    game_input(game);

    if (game->input.pressed[INPUT_TOGGLE_VIEW])
        game->show_dungeon_map = !game->show_dungeon_map;

    if (game->input.pressed[INPUT_SELECT_WALL_THEME_1])
        game->dungeon_wall_theme = WORLD_ART_THEME_1;
    if (game->input.pressed[INPUT_SELECT_WALL_THEME_2])
        game->dungeon_wall_theme = WORLD_ART_THEME_2;
    if (game->input.pressed[INPUT_SELECT_WALL_THEME_3])
        game->dungeon_wall_theme = WORLD_ART_THEME_3;
    if (game->input.pressed[INPUT_SELECT_WALL_THEME_4])
        game->dungeon_wall_theme = WORLD_ART_THEME_4;

    game_update_camera(game);

    if (game->input.pressed[INPUT_BACK])
        app_quit();
}

void game_render(Mem mem)
{
    Game *game = arena_start(mem.perm, Game);
    Camera2D active_cam = game->show_dungeon_map ? game->dungeon_cam : game->preview_cam;

    ClearBackground((Color){14, 25, 30, 255});
    BeginMode2D(active_cam);
    if (game->show_dungeon_map)
        game_draw_test_dungeon(game);
    else
        game_draw_art_previews(game);
    EndMode2D();
}
