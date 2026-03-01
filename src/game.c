#include <assert.h>
#include <stdio.h>
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
#define UNIT_PREVIEW_SPRITE_SCALE 3.0f
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

#define CAMERA_FOLLOW_SPEED_NEAR 2.0f
#define CAMERA_FOLLOW_SPEED_FAR 20.0f
#define CAMERA_FOLLOW_ACCEL_DISTANCE_TILES 10.0f
#define CAMERA_FALLBACK_FRAME_TIME (1.0f / 60.0f)
#define CAMERA_SETTLE_SCREEN_PX 0.5f
#define PREVIEW_CAMERA_PAN_SPEED 900.0f

#define DUNGEON_SIZE_SCALE 2
#define DUNGEON_COL_COUNT (50 * DUNGEON_SIZE_SCALE)
#define DUNGEON_ROW_COUNT (30 * DUNGEON_SIZE_SCALE)
#define DUNGEON_TILE_SCALE 4.0f
#define DUNGEON_PANEL_PADDING 18.0f

#define PLAYER_START_ORIENTATION 1

#define DUNGEON_SEED 0x7d2602a5f3d91c4bull

#define DUNGEON_HBW_MAX_SHORT_SIDE 16
#define DUNGEON_HBW_MAX_H_TILES 512
#define DUNGEON_HBW_MAX_V_TILES 512
#define DUNGEON_HBW_MAX_TILE_PIXELS (DUNGEON_HBW_MAX_SHORT_SIDE * DUNGEON_HBW_MAX_SHORT_SIDE * 2)
#define DUNGEON_HBW_GEN_MAX_X (DUNGEON_COL_COUNT + 8)
#define DUNGEON_HBW_GEN_MAX_Y (DUNGEON_ROW_COUNT + 8)

#define DUNGEON_CAMERA_ZOOM_MIN 0.15f
#define DUNGEON_CAMERA_ZOOM_MAX 2.25f
#define DUNGEON_CAMERA_ZOOM_STEP 0.1f
#define DUNGEON_CAMERA_ZOOM_RESET 1.0f

#define DUNGEON_RNG_STREAM_LAYOUT 0x4c41594f5554ull
#define DUNGEON_RNG_STREAM_POPULATE 0x504f50554c415445ull
#define DUNGEON_RNG_STREAM_CULL 0x43554c4cull
#define DUNGEON_RNG_STREAM_RUN_SEED 0x52554e53454544ull

#define DUNGEON_MAX_WORLD_FEATURES 12
#define DUNGEON_MAX_ITEMS 24
#define DUNGEON_MAX_UNITS 24
#define DUNGEON_CELL_COUNT (DUNGEON_COL_COUNT * DUNGEON_ROW_COUNT)
#define DUNGEON_MAX_FLOOR_DEPTH 512
#define DUNGEON_EXIT_MIN_PATH_STEPS 80
#define DUNGEON_FLOOR_VALIDATION_RETRY_LIMIT 256
#define DUNGEON_MAIN_PATH_KEEP_PERCENT 25
#define DUNGEON_MAIN_PATH_CULL_PICK_SAMPLES 4
#define DUNGEON_PRIMARY_VARIATION_WEIGHT 70
#define DUNGEON_LOS_RADIUS_TILES 12
#define DUNGEON_SPRITE_ANIM_FPS 3.0f
#define DUNGEON_GOBLIN_GRUNT_COUNT 7
#define DUNGEON_DIJKSTRA_UNREACHABLE 0x3fff
#define DUNGEON_ENEMY_DORMANT_DELAY_TURNS 5

#define DUNGEON_MINIMAP_MAX_WIDTH 320.0f
#define DUNGEON_MINIMAP_MAX_HEIGHT 220.0f
#define DUNGEON_MINIMAP_MARGIN 18.0f
#define DUNGEON_MINIMAP_PANEL_PADDING 10.0f
#define DUNGEON_MINIMAP_LABEL_SIZE 16.0f
#define DUNGEON_MINIMAP_LABEL_GAP 6.0f

#define DUNGEON_PLAYER_PANEL_MARGIN 16.0f
#define DUNGEON_PLAYER_PANEL_PADDING 12.0f
#define DUNGEON_PLAYER_PANEL_HEART_SIZE 24.0f
#define DUNGEON_PLAYER_PANEL_HEART_GAP 4.0f

#define DUNGEON_HOVER_PANEL_PADDING 10.0f

#define ITEM_KIND_GOLD_KEY ITEM_ART_KIND_AT(3, 0)
#define ITEM_KIND_SCROLL ITEM_ART_KIND_AT(6, 0)
#define ITEM_KIND_EMPTY_HEART ITEM_ART_KIND_AT(7, 0)
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

#if !defined(NDEBUG)
#define GAME_DEBUG_FEATURES 1
#else
#define GAME_DEBUG_FEATURES 0
#endif

typedef enum {
    INPUT_NONE,
    INPUT_MOVE_UP,
    INPUT_MOVE_DOWN,
    INPUT_MOVE_LEFT,
    INPUT_MOVE_RIGHT,
    INPUT_DEBUG_NEXT_FLOOR,
    INPUT_DEBUG_WIN_MENU,
    INPUT_BACK,
    INPUT_CONFIRM,
    INPUT_DEBUG_TOGGLE_VIEW,
    INPUT_DEBUG_TOGGLE_PATH,
    INPUT_DEBUG_TOGGLE_REVEAL_MAP,
    INPUT_DEBUG_TOGGLE_DIJKSTRA_OVERLAY,
    INPUT_SELECT_WALL_THEME_1,
    INPUT_SELECT_WALL_THEME_2,
    INPUT_SELECT_WALL_THEME_3,
    INPUT_SELECT_WALL_THEME_4,
    INPUT_SELECT_TEMPLATE_PREV,
    INPUT_SELECT_TEMPLATE_NEXT,
    INPUT_ZOOM_IN,
    INPUT_ZOOM_OUT,
    INPUT_ZOOM_RESET,
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
    DEBUG_FEATURE_SHOW_SPAWN_TO_EXIT_PATH,
    DEBUG_FEATURE_REVEAL_MAP,
    DEBUG_FEATURE_SHOW_PLAYER_DIJKSTRA,
    NUM_DEBUG_FEATURES,
} DEBUG_FEATURE;

typedef struct {
    DEBUG_FEATURE feature;
    INPUT toggle_input;
    const char *name;
    const char *hotkey;
    bool default_enabled;
} Debug_Feature_Def;

static const Debug_Feature_Def game_debug_feature_defs[NUM_DEBUG_FEATURES] = {
    {
        .feature = DEBUG_FEATURE_SHOW_SPAWN_TO_EXIT_PATH,
        .toggle_input = INPUT_DEBUG_TOGGLE_PATH,
        .name = "Spawn->Exit path",
        .hotkey = "P",
        .default_enabled = true,
    },
    {
        .feature = DEBUG_FEATURE_REVEAL_MAP,
        .toggle_input = INPUT_DEBUG_TOGGLE_REVEAL_MAP,
        .name = "Reveal map",
        .hotkey = "L",
        .default_enabled = false,
    },
    {
        .feature = DEBUG_FEATURE_SHOW_PLAYER_DIJKSTRA,
        .toggle_input = INPUT_DEBUG_TOGGLE_DIJKSTRA_OVERLAY,
        .name = "Dijkstra overlay",
        .hotkey = "J",
        .default_enabled = false,
    },
};

typedef enum {
    DEBUG_ACTION_TOGGLE_VIEW,
    DEBUG_ACTION_NEXT_FLOOR,
    DEBUG_ACTION_WIN_MENU,
    NUM_DEBUG_ACTIONS,
} DEBUG_ACTION;

typedef struct {
    DEBUG_ACTION action;
    INPUT action_input;
    const char *name;
    const char *hotkey;
} Debug_Action_Def;

static const Debug_Action_Def game_debug_action_defs[NUM_DEBUG_ACTIONS] = {
    {
        .action = DEBUG_ACTION_TOGGLE_VIEW,
        .action_input = INPUT_DEBUG_TOGGLE_VIEW,
        .name = "Toggle map/preview",
        .hotkey = "Tab",
    },
    {
        .action = DEBUG_ACTION_NEXT_FLOOR,
        .action_input = INPUT_DEBUG_NEXT_FLOOR,
        .name = "Generate next floor",
        .hotkey = "Space",
    },
    {
        .action = DEBUG_ACTION_WIN_MENU,
        .action_input = INPUT_DEBUG_WIN_MENU,
        .name = "Show win menu",
        .hotkey = "V",
    },
};

typedef enum {
    DUNGEON_CELL_EMPTY,
    DUNGEON_CELL_WALL,
    DUNGEON_CELL_FLOOR,
} DUNGEON_CELL;

typedef enum {
    END_MENU_NONE,
    END_MENU_DEATH,
    END_MENU_WIN,
} END_MENU_STATE;

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
    u8 max_health;
    u8 health;
    u8 damage;
    u8 speed;
} Unit_Stats;

typedef struct {
    i16 x;
    i16 y;
    UNIT_ART_KIND kind;
    u8 orientation;
    Unit_Stats stats;
    bool is_awake;
    u8 turns_out_of_player_los;
} Dungeon_Unit;

typedef struct {
    u8 floor_row;
    u8 variation_start;
    u8 variation_count;
} Dungeon_Floor_Palette;

typedef struct {
    const char *path;
    const char *name;
} Dungeon_HBW_Template_Def;

typedef struct {
    i8 a;
    i8 b;
    i8 c;
    i8 d;
    i8 e;
    i8 f;
    u8 floor_mask[DUNGEON_HBW_MAX_TILE_PIXELS];
} Dungeon_HBW_Tile;

typedef struct {
    bool valid;
    bool is_corner;
    i32 short_side_len;
    i32 num_color[6];
    i32 num_vary_x;
    i32 num_vary_y;
    i32 num_h_tiles;
    i32 num_v_tiles;
    Dungeon_HBW_Tile h_tiles[DUNGEON_HBW_MAX_H_TILES];
    Dungeon_HBW_Tile v_tiles[DUNGEON_HBW_MAX_V_TILES];
} Dungeon_HBW_Tileset;

typedef struct {
    bool is_corner;
    i32 short_side_len;
    i32 num_color[6];
    i32 num_vary_x;
    i32 num_vary_y;
} Dungeon_HBW_Config;

typedef struct {
    Input input;
    Camera2D dungeon_cam;
    Camera2D preview_cam;
    Font font;
    float font_spacing;
    bool show_dungeon_map;
    END_MENU_STATE end_menu_state;
    u32 debug_feature_mask;
    WORLD_ART_THEME dungeon_wall_theme;
    Texture2D units_texture;
    Texture2D items_texture;
    Texture2D world_texture;

    u64 dungeon_seed;
    u32 dungeon_floor_index;
    u32 dungeon_depth;
    u32 dungeon_floor_seed_by_depth[DUNGEON_MAX_FLOOR_DEPTH];
    u32 dungeon_floor_seed_count;
    i32 dungeon_template_index;
    RNG dungeon_run_seed_rng;
    RNG dungeon_layout_rng;
    RNG dungeon_populate_rng;
    RNG dungeon_cull_rng;
    Dungeon_HBW_Tileset dungeon_tileset;
    char dungeon_template_error[160];

    u8 dungeon_cells[DUNGEON_ROW_COUNT][DUNGEON_COL_COUNT];
    u8 dungeon_visible[DUNGEON_ROW_COUNT][DUNGEON_COL_COUNT];
    u8 dungeon_explored[DUNGEON_ROW_COUNT][DUNGEON_COL_COUNT];

    Dungeon_World_Feature world_features[DUNGEON_MAX_WORLD_FEATURES];
    u8 world_feature_count;

    Dungeon_Item items[DUNGEON_MAX_ITEMS];
    u8 item_count;

    Dungeon_Unit units[DUNGEON_MAX_UNITS];
    u8 unit_count;

    i16 player_x;
    i16 player_y;
    u8 player_orientation;
    Unit_Stats player_stats;

    i16 player_spawn_x;
    i16 player_spawn_y;

    i16 exit_x;
    i16 exit_y;
    bool has_exit;

    i32 spawn_to_exit_path_len;
    u8 spawn_to_exit_path[DUNGEON_ROW_COUNT][DUNGEON_COL_COUNT];

    i16 player_dijkstra_distance[DUNGEON_ROW_COUNT][DUNGEON_COL_COUNT];
    bool player_dijkstra_distance_valid;

} Game;

static const Dungeon_HBW_Template_Def game_dungeon_hbw_templates[] = {
    {.path = "assets/herringbone_templates/template_simple_caves_2_wide.png",
     .name = "Simple Caves 2 Wide"},
    {.path = "assets/herringbone_templates/template_horizontal_corridors_v3.png",
     .name = "Horizontal Corridors V3"},
    {.path = "assets/herringbone_templates/template_rooms_and_corridors_2_wide_diagonal_bias.png",
     .name = "Diagonal Bias Rooms"},
    {.path = "assets/herringbone_templates/template_sean_dungeon.png", .name = "Sean Dungeon"},
    {.path = "assets/herringbone_templates/template_rooms_and_corridors.png",
     .name = "Rooms And Corridors"},
    {.path = "assets/herringbone_templates/template_rooms_limit_connectivity.png",
     .name = "Rooms Limit Connectivity"},
    {.path = "assets/herringbone_templates/template_horizontal_corridors_v1.png",
     .name = "Horizontal Corridors V1"},
    {.path = "assets/herringbone_templates/template_horizontal_corridors_v2.png",
     .name = "Horizontal Corridors V2"},
    {.path = "assets/herringbone_templates/template_open_areas.png", .name = "Open Areas"},
    {.path = "assets/herringbone_templates/template_round_rooms_diagonal_corridors.png",
     .name = "Round Rooms"},
    {.path = "assets/herringbone_templates/template_caves_limit_connectivity.png",
     .name = "Caves Limit Connectivity"},
    {.path = "assets/herringbone_templates/template_caves_tiny_corridors.png",
     .name = "Caves Tiny Corridors"},
    {.path = "assets/herringbone_templates/template_corner_caves.png", .name = "Corner Caves"},
    {.path = "assets/herringbone_templates/template_ref2_corner_caves.png",
     .name = "Corner Caves Ref2"},
    {.path = "assets/herringbone_templates/template_maze_2_wide.png", .name = "Maze 2 Wide"},
    {.path = "assets/herringbone_templates/template_maze_plus_2_wide.png",
     .name = "Maze Plus 2 Wide"},
    {.path = "assets/herringbone_templates/template_limited_connectivity.png",
     .name = "Limited Connectivity"},
    {.path = "assets/herringbone_templates/template_limit_connectivity_fat.png",
     .name = "Limit Connectivity Fat"},
    {.path = "assets/herringbone_templates/template_square_rooms_with_random_rects.png",
     .name = "Square Rooms Random Rects"},
};

#define DUNGEON_HBW_TEMPLATE_COUNT                                                                 \
    ((i32)(sizeof(game_dungeon_hbw_templates) / sizeof(game_dungeon_hbw_templates[0])))

static const char *game_dungeon_active_template_name(const Game *game)
{
    if (game->dungeon_template_index < 0 ||
        game->dungeon_template_index >= DUNGEON_HBW_TEMPLATE_COUNT)
        return "Unknown";
    return game_dungeon_hbw_templates[game->dungeon_template_index].name;
}

static int game_unit_anim_frame(void)
{
    int frame = (int)(GetTime() * DUNGEON_SPRITE_ANIM_FPS);
    frame %= UNIT_ART_ANIM_FRAME_COUNT;
    if (frame < 0)
        frame += UNIT_ART_ANIM_FRAME_COUNT;
    return frame;
}

static int game_item_anim_frame(void)
{
    int frame = (int)(GetTime() * DUNGEON_SPRITE_ANIM_FPS);
    frame %= ITEM_ART_ANIM_FRAME_COUNT;
    if (frame < 0)
        frame += ITEM_ART_ANIM_FRAME_COUNT;
    return frame;
}

static bool game_dungeon_cell_in_bounds(i32 x, i32 y)
{
    return x >= 0 && x < DUNGEON_COL_COUNT && y >= 0 && y < DUNGEON_ROW_COUNT;
}

static u32 game_debug_feature_bit(DEBUG_FEATURE feature)
{
    return 1u << (u32)feature;
}

static bool game_debug_feature_is_enabled(const Game *game, DEBUG_FEATURE feature)
{
#if GAME_DEBUG_FEATURES
    return (game->debug_feature_mask & game_debug_feature_bit(feature)) != 0;
#else
    (void)game;
    (void)feature;
    return false;
#endif
}

static void game_debug_set_feature_enabled(Game *game, DEBUG_FEATURE feature, bool enabled)
{
#if GAME_DEBUG_FEATURES
    u32 bit = game_debug_feature_bit(feature);
    if (enabled)
        game->debug_feature_mask |= bit;
    else
        game->debug_feature_mask &= ~bit;
#else
    (void)game;
    (void)feature;
    (void)enabled;
#endif
}

static void game_debug_reset_feature_defaults(Game *game)
{
    game->debug_feature_mask = 0;
    for (i32 idx = 0; idx < NUM_DEBUG_FEATURES; idx++)
        game_debug_set_feature_enabled(game, game_debug_feature_defs[idx].feature,
                                       game_debug_feature_defs[idx].default_enabled);
}

static bool game_end_menu_is_active(const Game *game)
{
    return game->end_menu_state != END_MENU_NONE;
}

static void game_open_end_menu(Game *game, END_MENU_STATE menu_state)
{
    if (menu_state == END_MENU_NONE)
        return;

    game->end_menu_state = menu_state;
    game->show_dungeon_map = true;
}

static void game_debug_handle_feature_toggles(Game *game)
{
#if GAME_DEBUG_FEATURES
    for (i32 idx = 0; idx < NUM_DEBUG_FEATURES; idx++) {
        const Debug_Feature_Def *def = &game_debug_feature_defs[idx];
        if (!game->input.pressed[def->toggle_input])
            continue;

        bool enabled = game_debug_feature_is_enabled(game, def->feature);
        game_debug_set_feature_enabled(game, def->feature, !enabled);
    }
#else
    (void)game;
#endif
}

static void game_debug_handle_actions(Game *game, bool *out_rebuild_floor)
{
#if GAME_DEBUG_FEATURES
    for (i32 idx = 0; idx < NUM_DEBUG_ACTIONS; idx++) {
        const Debug_Action_Def *def = &game_debug_action_defs[idx];
        if (!game->input.pressed[def->action_input])
            continue;

        switch (def->action) {
        case DEBUG_ACTION_TOGGLE_VIEW:
            game->show_dungeon_map = !game->show_dungeon_map;
            break;
        case DEBUG_ACTION_NEXT_FLOOR:
            if (game->dungeon_depth + 1 < DUNGEON_MAX_FLOOR_DEPTH) {
                game->dungeon_depth++;
                *out_rebuild_floor = true;
            }
            break;
        case DEBUG_ACTION_WIN_MENU:
            game_open_end_menu(game, END_MENU_WIN);
            break;
        default:
            break;
        }
    }
#else
    (void)game;
    (void)out_rebuild_floor;
#endif
}

static bool game_dungeon_cell_is_floor(const Game *game, i32 x, i32 y)
{
    if (!game_dungeon_cell_in_bounds(x, y))
        return false;
    return game->dungeon_cells[y][x] == DUNGEON_CELL_FLOOR;
}

static bool game_dungeon_cell_blocks_vision(const Game *game, i32 x, i32 y)
{
    if (!game_dungeon_cell_in_bounds(x, y))
        return true;
    return game->dungeon_cells[y][x] != DUNGEON_CELL_FLOOR;
}

static bool game_dungeon_cell_is_visible(const Game *game, i32 x, i32 y)
{
    if (!game_dungeon_cell_in_bounds(x, y))
        return false;
    if (game_debug_feature_is_enabled(game, DEBUG_FEATURE_REVEAL_MAP))
        return true;
    return game->dungeon_visible[y][x] != 0;
}

static bool game_dungeon_cell_is_in_player_los(const Game *game, i32 x, i32 y)
{
    if (!game_dungeon_cell_in_bounds(x, y))
        return false;
    return game->dungeon_visible[y][x] != 0;
}

static bool game_dungeon_cell_is_explored(const Game *game, i32 x, i32 y)
{
    if (!game_dungeon_cell_in_bounds(x, y))
        return false;
    if (game_debug_feature_is_enabled(game, DEBUG_FEATURE_REVEAL_MAP))
        return true;
    return game->dungeon_explored[y][x] != 0;
}

static void game_dungeon_clear_visible_cells(Game *game)
{
    memset(game->dungeon_visible, 0, sizeof(game->dungeon_visible));
}

static void game_dungeon_reset_visibility(Game *game)
{
    game_dungeon_clear_visible_cells(game);
    memset(game->dungeon_explored, 0, sizeof(game->dungeon_explored));
}

static bool game_dungeon_has_line_of_sight(const Game *game, i32 x0, i32 y0, i32 x1, i32 y1)
{
    if (!game_dungeon_cell_in_bounds(x0, y0) || !game_dungeon_cell_in_bounds(x1, y1))
        return false;

    i32 x = x0;
    i32 y = y0;
    i32 dx = x1 - x0;
    if (dx < 0)
        dx = -dx;
    i32 sx = x0 < x1 ? 1 : -1;
    i32 dy = y1 - y0;
    if (dy < 0)
        dy = -dy;
    dy = -dy;
    i32 sy = y0 < y1 ? 1 : -1;
    i32 err = dx + dy;

    while (x != x1 || y != y1) {
        i32 prev_x = x;
        i32 prev_y = y;
        i32 e2 = err * 2;
        bool moved_x = false;
        bool moved_y = false;

        if (e2 >= dy) {
            err += dy;
            x += sx;
            moved_x = true;
        }
        if (e2 <= dx) {
            err += dx;
            y += sy;
            moved_y = true;
        }

        if (x == x1 && y == y1)
            break;

        if (moved_x && moved_y && game_dungeon_cell_blocks_vision(game, prev_x + sx, prev_y) &&
            game_dungeon_cell_blocks_vision(game, prev_x, prev_y + sy)) {
            return false;
        }

        if (game_dungeon_cell_blocks_vision(game, x, y))
            return false;
    }

    return true;
}

static void game_dungeon_rebuild_line_of_sight(Game *game)
{
    game_dungeon_clear_visible_cells(game);

    i32 origin_x = game->player_x;
    i32 origin_y = game->player_y;
    if (!game_dungeon_cell_in_bounds(origin_x, origin_y))
        return;

    i32 radius = DUNGEON_LOS_RADIUS_TILES;
    i32 radius_sq = radius * radius;
    i32 min_x = max(0, origin_x - radius);
    i32 max_x = min(DUNGEON_COL_COUNT - 1, origin_x + radius);
    i32 min_y = max(0, origin_y - radius);
    i32 max_y = min(DUNGEON_ROW_COUNT - 1, origin_y + radius);

    for (i32 y = min_y; y <= max_y; y++) {
        i32 dy = y - origin_y;
        for (i32 x = min_x; x <= max_x; x++) {
            i32 dx = x - origin_x;
            if (dx * dx + dy * dy > radius_sq)
                continue;
            if (!game_dungeon_has_line_of_sight(game, origin_x, origin_y, x, y))
                continue;

            game->dungeon_visible[y][x] = 1;
            game->dungeon_explored[y][x] = 1;
        }
    }

    game->dungeon_visible[origin_y][origin_x] = 1;
    game->dungeon_explored[origin_y][origin_x] = 1;
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

static void game_dungeon_build_walls(Game *game)
{
    for (i32 y = 0; y < DUNGEON_ROW_COUNT; y++) {
        for (i32 x = 0; x < DUNGEON_COL_COUNT; x++) {
            if (game->dungeon_cells[y][x] != DUNGEON_CELL_FLOOR)
                continue;

            i32 wall_y0 = max(y - 1, 0);
            i32 wall_y1 = min(y + 1, DUNGEON_ROW_COUNT - 1);
            i32 wall_x0 = max(x - 1, 0);
            i32 wall_x1 = min(x + 1, DUNGEON_COL_COUNT - 1);

            for (i32 wall_y = wall_y0; wall_y <= wall_y1; wall_y++) {
                for (i32 wall_x = wall_x0; wall_x <= wall_x1; wall_x++) {
                    if (game->dungeon_cells[wall_y][wall_x] == DUNGEON_CELL_EMPTY)
                        game->dungeon_cells[wall_y][wall_x] = DUNGEON_CELL_WALL;
                }
            }
        }
    }
}

static void game_dungeon_clear_walls(Game *game)
{
    for (i32 y = 0; y < DUNGEON_ROW_COUNT; y++) {
        for (i32 x = 0; x < DUNGEON_COL_COUNT; x++) {
            if (game->dungeon_cells[y][x] == DUNGEON_CELL_WALL)
                game->dungeon_cells[y][x] = DUNGEON_CELL_EMPTY;
        }
    }
}

static void game_dungeon_enforce_edge_walls(Game *game)
{
    for (i32 x = 0; x < DUNGEON_COL_COUNT; x++) {
        if (game->dungeon_cells[0][x] == DUNGEON_CELL_FLOOR)
            game->dungeon_cells[0][x] = DUNGEON_CELL_WALL;
        if (game->dungeon_cells[DUNGEON_ROW_COUNT - 1][x] == DUNGEON_CELL_FLOOR)
            game->dungeon_cells[DUNGEON_ROW_COUNT - 1][x] = DUNGEON_CELL_WALL;
    }

    for (i32 y = 0; y < DUNGEON_ROW_COUNT; y++) {
        if (game->dungeon_cells[y][0] == DUNGEON_CELL_FLOOR)
            game->dungeon_cells[y][0] = DUNGEON_CELL_WALL;
        if (game->dungeon_cells[y][DUNGEON_COL_COUNT - 1] == DUNGEON_CELL_FLOOR)
            game->dungeon_cells[y][DUNGEON_COL_COUNT - 1] = DUNGEON_CELL_WALL;
    }
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

static Unit_Stats game_make_unit_stats(i32 max_health, i32 damage, i32 speed)
{
    i32 safe_max_health = max(1, max_health);
    i32 safe_damage = max(0, damage);
    i32 safe_speed = max(1, speed);
    return (Unit_Stats){
        .max_health = (u8)safe_max_health,
        .health = (u8)safe_max_health,
        .damage = (u8)safe_damage,
        .speed = (u8)safe_speed,
    };
}

static bool game_unit_stats_take_damage(Unit_Stats *stats, i32 damage)
{
    assert(stats != 0);

    i32 next_health = (i32)stats->health - max(0, damage);
    stats->health = (u8)max(0, next_health);
    return (i32)stats->health <= 0;
}

static Unit_Stats game_get_player_base_stats(void)
{
    return game_make_unit_stats(6, 2, 1);
}

static Unit_Stats game_get_unit_base_stats(UNIT_ART_KIND kind)
{
    switch (kind) {
    case UNIT_ART_GOBLIN_GRUNT:
        return game_make_unit_stats(3, 1, 1);
    case UNIT_ART_GOBLIN_WARRIOR:
        return game_make_unit_stats(5, 2, 1);
    case UNIT_ART_GOBLIN_SHAMAN:
        return game_make_unit_stats(4, 2, 1);
    case UNIT_ART_TROLL:
        return game_make_unit_stats(8, 3, 1);
    case UNIT_ART_RAT:
        return game_make_unit_stats(2, 1, 2);
    case UNIT_ART_SPIDER:
        return game_make_unit_stats(3, 1, 2);
    case UNIT_ART_SLIME:
        return game_make_unit_stats(5, 1, 1);
    case UNIT_ART_SKELETON_GRUNT:
        return game_make_unit_stats(4, 1, 1);
    case UNIT_ART_SKELETON_MAGE:
        return game_make_unit_stats(4, 2, 1);
    case UNIT_ART_SKELETON_KING:
        return game_make_unit_stats(9, 3, 1);
    case UNIT_ART_DRAGON:
        return game_make_unit_stats(14, 4, 1);
    default:
        return game_make_unit_stats(4, 1, 1);
    }
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
        .stats = game_get_unit_base_stats(kind),
        .is_awake = false,
        .turns_out_of_player_los = 0,
    };
}

static __uint128_t game_make_seed128(u64 seed)
{
    return ((__uint128_t)seed << 64) | (seed ^ 0x9e3779b97f4a7c15ull);
}

static void game_seed_run_seed_rng(Game *game)
{
    RNG run_seed_rng = {.seed = game_make_seed128(DUNGEON_SEED)};
    game->dungeon_run_seed_rng = ck_rng_fork(&run_seed_rng, DUNGEON_RNG_STREAM_RUN_SEED);
}

static u64 game_next_dungeon_seed(Game *game)
{
    u64 next_seed = ck_rand(&game->dungeon_run_seed_rng);
    if (next_seed == game->dungeon_seed)
        next_seed ^= 0x9e3779b97f4a7c15ull;
    return next_seed;
}

static void game_seed_dungeon_rng_streams(Game *game)
{
    u64 floor_seed = game->dungeon_seed ^ ((u64)game->dungeon_floor_index * 0xd1342543de82ef95ull);
    floor_seed ^= (u64)(game->dungeon_template_index + 1) * 0x9e3779b97f4a7c15ull;
    RNG floor_rng = {.seed = game_make_seed128(floor_seed)};
    game->dungeon_layout_rng = ck_rng_fork(&floor_rng, DUNGEON_RNG_STREAM_LAYOUT);
    game->dungeon_populate_rng = ck_rng_fork(&floor_rng, DUNGEON_RNG_STREAM_POPULATE);
    game->dungeon_cull_rng = ck_rng_fork(&floor_rng, DUNGEON_RNG_STREAM_CULL);
}

static bool game_dungeon_hbw_pixel_is_floor(u8 r, u8 g, u8 b)
{
    return !(r == 255 && g == 255 && b == 255);
}

static void game_dungeon_hbw_get_template_info(const Dungeon_HBW_Config *config, i32 *out_w,
                                               i32 *out_h, i32 *out_h_count, i32 *out_v_count)
{
    i32 size_x = 0;
    i32 size_y = 0;
    i32 horz_count = 0;
    i32 vert_count = 0;
    i32 side = config->short_side_len;

    if (config->is_corner) {
        i32 horz_w =
            config->num_color[1] * config->num_color[2] * config->num_color[3] * config->num_vary_x;
        i32 horz_h =
            config->num_color[0] * config->num_color[1] * config->num_color[2] * config->num_vary_y;
        i32 vert_w =
            config->num_color[0] * config->num_color[3] * config->num_color[2] * config->num_vary_y;
        i32 vert_h =
            config->num_color[1] * config->num_color[0] * config->num_color[3] * config->num_vary_x;

        i32 horz_x = horz_w * (2 * side + 3);
        i32 horz_y = horz_h * (side + 3);
        i32 vert_x = vert_w * (side + 3);
        i32 vert_y = vert_h * (2 * side + 3);

        horz_count = horz_w * horz_h;
        vert_count = vert_w * vert_h;

        size_x = max(horz_x, vert_x);
        size_y = 2 + horz_y + 2 + vert_y;
    } else {
        i32 horz_w =
            config->num_color[0] * config->num_color[1] * config->num_color[2] * config->num_vary_x;
        i32 horz_h =
            config->num_color[3] * config->num_color[4] * config->num_color[2] * config->num_vary_y;
        i32 vert_w =
            config->num_color[0] * config->num_color[5] * config->num_color[1] * config->num_vary_y;
        i32 vert_h =
            config->num_color[3] * config->num_color[4] * config->num_color[5] * config->num_vary_x;

        i32 horz_x = horz_w * (2 * side + 3);
        i32 horz_y = horz_h * (side + 3);
        i32 vert_x = vert_w * (side + 3);
        i32 vert_y = vert_h * (2 * side + 3);

        horz_count = horz_w * horz_h;
        vert_count = vert_w * vert_h;

        size_x = max(horz_x, vert_x);
        size_y = 2 + horz_y + 2 + vert_y;
    }

    if (out_w)
        *out_w = size_x;
    if (out_h)
        *out_h = size_y;
    if (out_h_count)
        *out_h_count = horz_count;
    if (out_v_count)
        *out_v_count = vert_count;
}

static bool game_dungeon_hbw_read_config(const u8 *pixels, i32 width,
                                         Dungeon_HBW_Config *out_config, char *error, i32 error_cap)
{
    if (width <= 0) {
        snprintf(error, (size_t)error_cap, "template has invalid width");
        return false;
    }

    u8 header[9];
    for (i32 i = 0; i < 9; i++) {
        i32 idx = width * 3 - 1 - i;
        header[i] = (u8)(pixels[idx] ^ (u8)(i * 55));
    }

    memset(out_config, 0, sizeof(*out_config));

    if (header[7] == 0xc0) {
        out_config->is_corner = true;
        out_config->num_color[0] = header[0];
        out_config->num_color[1] = header[1];
        out_config->num_color[2] = header[2];
        out_config->num_color[3] = header[3];
        out_config->num_vary_x = header[4];
        out_config->num_vary_y = header[5];
        out_config->short_side_len = header[6];
    } else {
        out_config->is_corner = false;
        out_config->num_color[0] = header[0];
        out_config->num_color[1] = header[1];
        out_config->num_color[2] = header[2];
        out_config->num_color[3] = header[3];
        out_config->num_color[4] = header[4];
        out_config->num_color[5] = header[5];
        out_config->num_vary_x = header[6];
        out_config->num_vary_y = header[7];
        out_config->short_side_len = header[8];
    }

    if (out_config->num_vary_x <= 0 || out_config->num_vary_y <= 0) {
        snprintf(error, (size_t)error_cap, "template variation counts are invalid");
        return false;
    }

    if (out_config->short_side_len <= 0 ||
        out_config->short_side_len > DUNGEON_HBW_MAX_SHORT_SIDE) {
        snprintf(error, (size_t)error_cap, "template side %d unsupported (max %d)",
                 out_config->short_side_len, DUNGEON_HBW_MAX_SHORT_SIDE);
        return false;
    }

    i32 color_count = out_config->is_corner ? 4 : 6;
    for (i32 i = 0; i < color_count; i++) {
        if (out_config->num_color[i] <= 0) {
            snprintf(error, (size_t)error_cap, "template color count %d is invalid", i);
            return false;
        }
    }

    return true;
}

static bool game_dungeon_hbw_store_h_tile(Dungeon_HBW_Tileset *tileset, const u8 *pixels,
                                          i32 image_w, i32 image_h, i32 xpos, i32 ypos, i8 a, i8 b,
                                          i8 c, i8 d, i8 e, i8 f)
{
    if (tileset->num_h_tiles >= DUNGEON_HBW_MAX_H_TILES)
        return false;

    i32 side = tileset->short_side_len;
    i32 src_x0 = xpos + 1;
    i32 src_y0 = ypos + 1;
    if (src_x0 < 0 || src_y0 < 0 || src_x0 + side * 2 > image_w || src_y0 + side > image_h)
        return false;

    Dungeon_HBW_Tile *tile = &tileset->h_tiles[tileset->num_h_tiles++];
    tile->a = a;
    tile->b = b;
    tile->c = c;
    tile->d = d;
    tile->e = e;
    tile->f = f;

    i32 dst = 0;
    for (i32 y = 0; y < side; y++) {
        for (i32 x = 0; x < side * 2; x++) {
            i32 src_idx = ((src_y0 + y) * image_w + (src_x0 + x)) * 3;
            tile->floor_mask[dst++] = game_dungeon_hbw_pixel_is_floor(
                                          pixels[src_idx], pixels[src_idx + 1], pixels[src_idx + 2])
                                          ? 1
                                          : 0;
        }
    }

    return true;
}

static bool game_dungeon_hbw_store_v_tile(Dungeon_HBW_Tileset *tileset, const u8 *pixels,
                                          i32 image_w, i32 image_h, i32 xpos, i32 ypos, i8 a, i8 b,
                                          i8 c, i8 d, i8 e, i8 f)
{
    if (tileset->num_v_tiles >= DUNGEON_HBW_MAX_V_TILES)
        return false;

    i32 side = tileset->short_side_len;
    i32 src_x0 = xpos + 1;
    i32 src_y0 = ypos + 1;
    if (src_x0 < 0 || src_y0 < 0 || src_x0 + side > image_w || src_y0 + side * 2 > image_h)
        return false;

    Dungeon_HBW_Tile *tile = &tileset->v_tiles[tileset->num_v_tiles++];
    tile->a = a;
    tile->b = b;
    tile->c = c;
    tile->d = d;
    tile->e = e;
    tile->f = f;

    i32 dst = 0;
    for (i32 y = 0; y < side * 2; y++) {
        for (i32 x = 0; x < side; x++) {
            i32 src_idx = ((src_y0 + y) * image_w + (src_x0 + x)) * 3;
            tile->floor_mask[dst++] = game_dungeon_hbw_pixel_is_floor(
                                          pixels[src_idx], pixels[src_idx + 1], pixels[src_idx + 2])
                                          ? 1
                                          : 0;
        }
    }

    return true;
}

static bool game_dungeon_hbw_parse_h_row(Dungeon_HBW_Tileset *tileset, const u8 *pixels,
                                         i32 image_w, i32 image_h, i32 xpos, i32 ypos, i32 a0,
                                         i32 a1, i32 b0, i32 b1, i32 c0, i32 c1, i32 d0, i32 d1,
                                         i32 e0, i32 e1, i32 f0, i32 f1, i32 variants)
{
    i32 side = tileset->short_side_len;
    for (i32 v = 0; v < variants; v++) {
        for (i32 f = f0; f <= f1; f++) {
            for (i32 e = e0; e <= e1; e++) {
                for (i32 d = d0; d <= d1; d++) {
                    for (i32 c = c0; c <= c1; c++) {
                        for (i32 b = b0; b <= b1; b++) {
                            for (i32 a = a0; a <= a1; a++) {
                                if (!game_dungeon_hbw_store_h_tile(
                                        tileset, pixels, image_w, image_h, xpos, ypos, (i8)a, (i8)b,
                                        (i8)c, (i8)d, (i8)e, (i8)f)) {
                                    return false;
                                }
                                xpos += 2 * side + 3;
                            }
                        }
                    }
                }
            }
        }
    }

    return true;
}

static bool game_dungeon_hbw_parse_v_row(Dungeon_HBW_Tileset *tileset, const u8 *pixels,
                                         i32 image_w, i32 image_h, i32 xpos, i32 ypos, i32 a0,
                                         i32 a1, i32 b0, i32 b1, i32 c0, i32 c1, i32 d0, i32 d1,
                                         i32 e0, i32 e1, i32 f0, i32 f1, i32 variants)
{
    i32 side = tileset->short_side_len;
    for (i32 v = 0; v < variants; v++) {
        for (i32 f = f0; f <= f1; f++) {
            for (i32 e = e0; e <= e1; e++) {
                for (i32 d = d0; d <= d1; d++) {
                    for (i32 c = c0; c <= c1; c++) {
                        for (i32 b = b0; b <= b1; b++) {
                            for (i32 a = a0; a <= a1; a++) {
                                if (!game_dungeon_hbw_store_v_tile(
                                        tileset, pixels, image_w, image_h, xpos, ypos, (i8)a, (i8)b,
                                        (i8)c, (i8)d, (i8)e, (i8)f)) {
                                    return false;
                                }
                                xpos += side + 3;
                            }
                        }
                    }
                }
            }
        }
    }

    return true;
}

static bool game_dungeon_hbw_parse_tileset(Dungeon_HBW_Tileset *tileset, const u8 *pixels,
                                           i32 image_w, i32 image_h, char *error, i32 error_cap)
{
    Dungeon_HBW_Config config = {0};
    memset(tileset, 0, sizeof(*tileset));

    if (!game_dungeon_hbw_read_config(pixels, image_w, &config, error, error_cap))
        return false;

    i32 expected_w = 0;
    i32 expected_h = 0;
    i32 expected_h_count = 0;
    i32 expected_v_count = 0;
    game_dungeon_hbw_get_template_info(&config, &expected_w, &expected_h, &expected_h_count,
                                       &expected_v_count);

    if (image_w < expected_w || image_h < expected_h) {
        snprintf(error, (size_t)error_cap, "template image too small for encoded layout");
        return false;
    }

    if (expected_h_count > DUNGEON_HBW_MAX_H_TILES || expected_v_count > DUNGEON_HBW_MAX_V_TILES) {
        snprintf(error, (size_t)error_cap, "template has too many tiles (%d h, %d v)",
                 expected_h_count, expected_v_count);
        return false;
    }

    tileset->is_corner = config.is_corner;
    tileset->short_side_len = config.short_side_len;
    tileset->num_vary_x = config.num_vary_x;
    tileset->num_vary_y = config.num_vary_y;
    memcpy(tileset->num_color, config.num_color, sizeof(tileset->num_color));

    i32 ypos = 2;
    if (config.is_corner) {
        for (i32 k = 0; k < config.num_color[2]; k++) {
            for (i32 j = 0; j < config.num_color[1]; j++) {
                for (i32 i = 0; i < config.num_color[0]; i++) {
                    for (i32 q = 0; q < config.num_vary_y; q++) {
                        if (!game_dungeon_hbw_parse_h_row(
                                tileset, pixels, image_w, image_h, 0, ypos, 0,
                                config.num_color[1] - 1, 0, config.num_color[2] - 1, 0,
                                config.num_color[3] - 1, i, i, j, j, k, k, config.num_vary_x)) {
                            snprintf(error, (size_t)error_cap,
                                     "failed while parsing horizontal corner tiles");
                            return false;
                        }
                        ypos += config.short_side_len + 3;
                    }
                }
            }
        }

        ypos += 2;
        for (i32 k = 0; k < config.num_color[3]; k++) {
            for (i32 j = 0; j < config.num_color[0]; j++) {
                for (i32 i = 0; i < config.num_color[1]; i++) {
                    for (i32 q = 0; q < config.num_vary_x; q++) {
                        if (!game_dungeon_hbw_parse_v_row(
                                tileset, pixels, image_w, image_h, 0, ypos, 0,
                                config.num_color[0] - 1, 0, config.num_color[3] - 1, 0,
                                config.num_color[2] - 1, i, i, j, j, k, k, config.num_vary_y)) {
                            snprintf(error, (size_t)error_cap,
                                     "failed while parsing vertical corner tiles");
                            return false;
                        }
                        ypos += config.short_side_len * 2 + 3;
                    }
                }
            }
        }
    } else {
        for (i32 k = 0; k < config.num_color[3]; k++) {
            for (i32 j = 0; j < config.num_color[4]; j++) {
                for (i32 i = 0; i < config.num_color[2]; i++) {
                    for (i32 q = 0; q < config.num_vary_y; q++) {
                        if (!game_dungeon_hbw_parse_h_row(
                                tileset, pixels, image_w, image_h, 0, ypos, 0,
                                config.num_color[2] - 1, k, k, 0, config.num_color[1] - 1, j, j, 0,
                                config.num_color[0] - 1, i, i, config.num_vary_x)) {
                            snprintf(error, (size_t)error_cap,
                                     "failed while parsing horizontal edge tiles");
                            return false;
                        }
                        ypos += config.short_side_len + 3;
                    }
                }
            }
        }

        ypos += 2;
        for (i32 k = 0; k < config.num_color[3]; k++) {
            for (i32 j = 0; j < config.num_color[4]; j++) {
                for (i32 i = 0; i < config.num_color[5]; i++) {
                    for (i32 q = 0; q < config.num_vary_x; q++) {
                        if (!game_dungeon_hbw_parse_v_row(
                                tileset, pixels, image_w, image_h, 0, ypos, 0,
                                config.num_color[0] - 1, i, i, 0, config.num_color[1] - 1, j, j, 0,
                                config.num_color[5] - 1, k, k, config.num_vary_y)) {
                            snprintf(error, (size_t)error_cap,
                                     "failed while parsing vertical edge tiles");
                            return false;
                        }
                        ypos += config.short_side_len * 2 + 3;
                    }
                }
            }
        }
    }

    if (tileset->num_h_tiles <= 0 || tileset->num_v_tiles <= 0) {
        snprintf(error, (size_t)error_cap, "template did not produce any tiles");
        return false;
    }

    tileset->valid = true;
    return true;
}

static bool game_dungeon_load_template(Game *game, i32 template_index)
{
    assert(template_index >= 0 && template_index < DUNGEON_HBW_TEMPLATE_COUNT);
    memset(game->dungeon_template_error, 0, sizeof(game->dungeon_template_error));

    const Dungeon_HBW_Template_Def *def = &game_dungeon_hbw_templates[template_index];
    const char *template_path = def->path;
    char fallback_path[256] = {0};
    if (!FileExists(template_path)) {
        snprintf(fallback_path, sizeof(fallback_path), "../%s", def->path);
        if (FileExists(fallback_path))
            template_path = fallback_path;
    }

    Image image = LoadImage(template_path);
    if (image.data == NULL) {
        snprintf(game->dungeon_template_error, sizeof(game->dungeon_template_error),
                 "failed to load %s", template_path);
        game->dungeon_tileset.valid = false;
        return false;
    }

    ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_R8G8B8);
    bool ok = game_dungeon_hbw_parse_tileset(
        &game->dungeon_tileset, (const u8 *)image.data, image.width, image.height,
        game->dungeon_template_error, (i32)sizeof(game->dungeon_template_error));
    UnloadImage(image);

    if (!ok) {
        game->dungeon_tileset.valid = false;
        return false;
    }

    game->dungeon_template_index = template_index;
    return true;
}

static bool game_dungeon_hbw_tile_matches(const Dungeon_HBW_Tile *tile, const i8 *a, const i8 *b,
                                          const i8 *c, const i8 *d, const i8 *e, const i8 *f)
{
    return (*a < 0 || *a == tile->a) && (*b < 0 || *b == tile->b) && (*c < 0 || *c == tile->c) &&
           (*d < 0 || *d == tile->d) && (*e < 0 || *e == tile->e) && (*f < 0 || *f == tile->f);
}

static const Dungeon_HBW_Tile *game_dungeon_hbw_choose_tile(Game *game,
                                                            const Dungeon_HBW_Tile *tiles,
                                                            i32 tile_count, i8 *a, i8 *b, i8 *c,
                                                            i8 *d, i8 *e, i8 *f)
{
    const Dungeon_HBW_Tile *choice = NULL;
    i32 match_count = 0;

    for (i32 i = 0; i < tile_count; i++) {
        const Dungeon_HBW_Tile *tile = &tiles[i];
        if (!game_dungeon_hbw_tile_matches(tile, a, b, c, d, e, f))
            continue;

        match_count++;
        if (ck_rand_int(&game->dungeon_layout_rng, 0, match_count) == 0)
            choice = tile;
    }

    if (!choice)
        return NULL;

    *a = choice->a;
    *b = choice->b;
    *c = choice->c;
    *d = choice->d;
    *e = choice->e;
    *f = choice->f;
    return choice;
}

static void game_dungeon_hbw_stamp_h_tile(Game *game, const Dungeon_HBW_Tile *tile, i32 xpos,
                                          i32 ypos, i32 side)
{
    i32 src = 0;
    for (i32 y = 0; y < side; y++) {
        for (i32 x = 0; x < side * 2; x++, src++) {
            i32 map_x = xpos + x;
            i32 map_y = ypos + y;
            if (!game_dungeon_cell_in_bounds(map_x, map_y))
                continue;
            game->dungeon_cells[map_y][map_x] =
                tile->floor_mask[src] ? DUNGEON_CELL_FLOOR : DUNGEON_CELL_EMPTY;
        }
    }
}

static void game_dungeon_hbw_stamp_v_tile(Game *game, const Dungeon_HBW_Tile *tile, i32 xpos,
                                          i32 ypos, i32 side)
{
    i32 src = 0;
    for (i32 y = 0; y < side * 2; y++) {
        for (i32 x = 0; x < side; x++, src++) {
            i32 map_x = xpos + x;
            i32 map_y = ypos + y;
            if (!game_dungeon_cell_in_bounds(map_x, map_y))
                continue;
            game->dungeon_cells[map_y][map_x] =
                tile->floor_mask[src] ? DUNGEON_CELL_FLOOR : DUNGEON_CELL_EMPTY;
        }
    }
}

static bool game_dungeon_hbw_generate_edge_layout(Game *game)
{
    Dungeon_HBW_Tileset *tileset = &game->dungeon_tileset;
    i32 side = tileset->short_side_len;
    i8 h_color[DUNGEON_HBW_GEN_MAX_Y][DUNGEON_HBW_GEN_MAX_X];
    i8 v_color[DUNGEON_HBW_GEN_MAX_Y][DUNGEON_HBW_GEN_MAX_X];
    memset(h_color, -1, sizeof(h_color));
    memset(v_color, -1, sizeof(v_color));

    i32 ypos = -side;
    for (i32 j = -1; ypos < DUNGEON_ROW_COUNT; j++, ypos += side) {
        i32 phase = j & 3;
        i32 i = (phase == 0) ? 0 : (phase - 4);
        for (;; i += 4) {
            i32 xpos = i * side;
            if (xpos >= DUNGEON_COL_COUNT)
                break;

            if (xpos + side * 2 >= 0 && ypos >= 0) {
                if (j + 3 < 0 || j + 3 >= DUNGEON_HBW_GEN_MAX_Y || i + 4 < 0 ||
                    i + 4 >= DUNGEON_HBW_GEN_MAX_X) {
                    snprintf(game->dungeon_template_error, sizeof(game->dungeon_template_error),
                             "edge layout grid overflow");
                    return false;
                }

                i8 *a = &h_color[j + 2][i + 2];
                i8 *b = &h_color[j + 2][i + 3];
                i8 *c = &v_color[j + 2][i + 2];
                i8 *d = &v_color[j + 2][i + 4];
                i8 *e = &h_color[j + 3][i + 2];
                i8 *f = &h_color[j + 3][i + 3];
                const Dungeon_HBW_Tile *tile = game_dungeon_hbw_choose_tile(
                    game, tileset->h_tiles, tileset->num_h_tiles, a, b, c, d, e, f);
                if (!tile) {
                    snprintf(game->dungeon_template_error, sizeof(game->dungeon_template_error),
                             "no matching horizontal tile for edge constraints");
                    return false;
                }
                game_dungeon_hbw_stamp_h_tile(game, tile, xpos, ypos, side);
            }

            xpos += side * 3;
            if (xpos < DUNGEON_COL_COUNT) {
                if (j + 4 < 0 || j + 4 >= DUNGEON_HBW_GEN_MAX_Y || i + 6 < 0 ||
                    i + 6 >= DUNGEON_HBW_GEN_MAX_X) {
                    snprintf(game->dungeon_template_error, sizeof(game->dungeon_template_error),
                             "edge layout grid overflow");
                    return false;
                }

                i8 *a = &h_color[j + 2][i + 5];
                i8 *b = &v_color[j + 2][i + 5];
                i8 *c = &v_color[j + 2][i + 6];
                i8 *d = &v_color[j + 3][i + 5];
                i8 *e = &v_color[j + 3][i + 6];
                i8 *f = &h_color[j + 4][i + 5];
                const Dungeon_HBW_Tile *tile = game_dungeon_hbw_choose_tile(
                    game, tileset->v_tiles, tileset->num_v_tiles, a, b, c, d, e, f);
                if (!tile) {
                    snprintf(game->dungeon_template_error, sizeof(game->dungeon_template_error),
                             "no matching vertical tile for edge constraints");
                    return false;
                }
                game_dungeon_hbw_stamp_v_tile(game, tile, xpos, ypos, side);
            }
        }
    }

    return true;
}

static bool game_dungeon_hbw_generate_corner_layout(Game *game)
{
    Dungeon_HBW_Tileset *tileset = &game->dungeon_tileset;
    i32 side = tileset->short_side_len;
    i8 c_color[DUNGEON_HBW_GEN_MAX_Y][DUNGEON_HBW_GEN_MAX_X];
    memset(c_color, 0, sizeof(c_color));

    i32 xmax = DUNGEON_COL_COUNT / side + 6;
    i32 ymax = DUNGEON_ROW_COUNT / side + 6;
    if (xmax > DUNGEON_HBW_GEN_MAX_X || ymax > DUNGEON_HBW_GEN_MAX_Y) {
        snprintf(game->dungeon_template_error, sizeof(game->dungeon_template_error),
                 "corner layout grid exceeds capacity");
        return false;
    }

    for (i32 y = 0; y < ymax; y++) {
        for (i32 x = 0; x < xmax; x++) {
            i32 corner_type = (x - y + 1) & 3;
            i32 color_count = max(tileset->num_color[corner_type], 1);
            c_color[y][x] = (i8)ck_rand_int(&game->dungeon_layout_rng, 0, color_count);
        }
    }

    i32 ypos = -side;
    for (i32 j = -1; ypos < DUNGEON_ROW_COUNT; j++, ypos += side) {
        i32 phase = j & 3;
        i32 i = (phase == 0) ? 0 : (phase - 4);
        for (;; i += 4) {
            i32 xpos = i * side;
            if (xpos >= DUNGEON_COL_COUNT)
                break;

            if (xpos + side * 2 >= 0 && ypos >= 0) {
                if (j + 3 < 0 || j + 3 >= DUNGEON_HBW_GEN_MAX_Y || i + 4 < 0 ||
                    i + 4 >= DUNGEON_HBW_GEN_MAX_X) {
                    snprintf(game->dungeon_template_error, sizeof(game->dungeon_template_error),
                             "corner layout grid overflow");
                    return false;
                }

                i8 *a = &c_color[j + 2][i + 2];
                i8 *b = &c_color[j + 2][i + 3];
                i8 *c = &c_color[j + 2][i + 4];
                i8 *d = &c_color[j + 3][i + 2];
                i8 *e = &c_color[j + 3][i + 3];
                i8 *f = &c_color[j + 3][i + 4];
                const Dungeon_HBW_Tile *tile = game_dungeon_hbw_choose_tile(
                    game, tileset->h_tiles, tileset->num_h_tiles, a, b, c, d, e, f);
                if (!tile) {
                    snprintf(game->dungeon_template_error, sizeof(game->dungeon_template_error),
                             "no matching horizontal tile for corner constraints");
                    return false;
                }
                game_dungeon_hbw_stamp_h_tile(game, tile, xpos, ypos, side);
            }

            xpos += side * 3;
            if (xpos < DUNGEON_COL_COUNT) {
                if (j + 4 < 0 || j + 4 >= DUNGEON_HBW_GEN_MAX_Y || i + 6 < 0 ||
                    i + 6 >= DUNGEON_HBW_GEN_MAX_X) {
                    snprintf(game->dungeon_template_error, sizeof(game->dungeon_template_error),
                             "corner layout grid overflow");
                    return false;
                }

                i8 *a = &c_color[j + 2][i + 5];
                i8 *b = &c_color[j + 3][i + 5];
                i8 *c = &c_color[j + 4][i + 5];
                i8 *d = &c_color[j + 2][i + 6];
                i8 *e = &c_color[j + 3][i + 6];
                i8 *f = &c_color[j + 4][i + 6];
                const Dungeon_HBW_Tile *tile = game_dungeon_hbw_choose_tile(
                    game, tileset->v_tiles, tileset->num_v_tiles, a, b, c, d, e, f);
                if (!tile) {
                    snprintf(game->dungeon_template_error, sizeof(game->dungeon_template_error),
                             "no matching vertical tile for corner constraints");
                    return false;
                }
                game_dungeon_hbw_stamp_v_tile(game, tile, xpos, ypos, side);
            }
        }
    }

    return true;
}

static bool game_dungeon_generate_herringbone_layout(Game *game)
{
    if (!game->dungeon_tileset.valid)
        return false;

    if (game->dungeon_tileset.is_corner)
        return game_dungeon_hbw_generate_corner_layout(game);

    return game_dungeon_hbw_generate_edge_layout(game);
}

static bool game_dungeon_cell_has_world_feature(const Game *game, i32 x, i32 y)
{
    for (u8 idx = 0; idx < game->world_feature_count; idx++) {
        if (game->world_features[idx].x == x && game->world_features[idx].y == y)
            return true;
    }
    return false;
}

static bool game_dungeon_get_world_feature_role_at(const Game *game, i32 x, i32 y,
                                                   WORLD_ART_ROLE *out_role)
{
    for (u8 idx = 0; idx < game->world_feature_count; idx++) {
        Dungeon_World_Feature feature = game->world_features[idx];
        if (feature.x != x || feature.y != y)
            continue;

        if (out_role)
            *out_role = world_art_get_role_from_tile(feature.tile);
        return true;
    }

    return false;
}

static bool game_dungeon_find_world_feature_by_role(const Game *game, WORLD_ART_ROLE role,
                                                    i16 *out_x, i16 *out_y)
{
    for (u8 idx = 0; idx < game->world_feature_count; idx++) {
        Dungeon_World_Feature feature = game->world_features[idx];
        if (world_art_get_role_from_tile(feature.tile) != role)
            continue;

        if (out_x)
            *out_x = feature.x;
        if (out_y)
            *out_y = feature.y;
        return true;
    }

    return false;
}

static bool game_dungeon_cell_has_item(const Game *game, i32 x, i32 y)
{
    for (u8 idx = 0; idx < game->item_count; idx++) {
        if (game->items[idx].x == x && game->items[idx].y == y)
            return true;
    }
    return false;
}

static bool game_dungeon_cell_has_unit(const Game *game, i32 x, i32 y)
{
    for (u8 idx = 0; idx < game->unit_count; idx++) {
        if (game->units[idx].x == x && game->units[idx].y == y)
            return true;
    }
    return false;
}

static const Dungeon_Unit *game_dungeon_get_unit_at(const Game *game, i32 x, i32 y)
{
    for (u8 idx = 0; idx < game->unit_count; idx++) {
        if (game->units[idx].x == x && game->units[idx].y == y)
            return &game->units[idx];
    }
    return 0;
}

static i32 game_dungeon_get_unit_index_at(const Game *game, i32 x, i32 y)
{
    for (u8 idx = 0; idx < game->unit_count; idx++) {
        if (game->units[idx].x == x && game->units[idx].y == y)
            return idx;
    }

    return -1;
}

static void game_dungeon_remove_unit_at(Game *game, i32 unit_idx)
{
    assert(unit_idx >= 0);
    assert(unit_idx < game->unit_count);

    u8 last_idx = (u8)(game->unit_count - 1);
    if ((u8)unit_idx != last_idx)
        game->units[unit_idx] = game->units[last_idx];
    game->unit_count--;
}

static bool game_dungeon_cell_is_occupied(const Game *game, i32 x, i32 y)
{
    if (game->player_x == x && game->player_y == y)
        return true;
    if (game_dungeon_cell_has_world_feature(game, x, y))
        return true;
    if (game_dungeon_cell_has_item(game, x, y))
        return true;
    return game_dungeon_cell_has_unit(game, x, y);
}

static bool game_dungeon_cell_has_unit_excluding(const Game *game, i32 x, i32 y, i32 excluded_idx)
{
    for (u8 idx = 0; idx < game->unit_count; idx++) {
        if ((i32)idx == excluded_idx)
            continue;
        if (game->units[idx].x == x && game->units[idx].y == y)
            return true;
    }

    return false;
}

static bool game_dungeon_build_dijkstra_map(const Game *game, const i16 *goal_x, const i16 *goal_y,
                                            i32 goal_count,
                                            i16 out_distance[DUNGEON_ROW_COUNT][DUNGEON_COL_COUNT])
{
    for (i32 y = 0; y < DUNGEON_ROW_COUNT; y++) {
        for (i32 x = 0; x < DUNGEON_COL_COUNT; x++) {
            out_distance[y][x] =
                game_dungeon_cell_is_floor(game, x, y) ? DUNGEON_DIJKSTRA_UNREACHABLE : -1;
        }
    }

    if (goal_count <= 0)
        return false;

    i32 queue[DUNGEON_CELL_COUNT];
    i32 head = 0;
    i32 tail = 0;

    for (i32 goal_idx = 0; goal_idx < goal_count; goal_idx++) {
        i32 x = goal_x[goal_idx];
        i32 y = goal_y[goal_idx];
        if (!game_dungeon_cell_is_floor(game, x, y))
            continue;
        if (out_distance[y][x] == 0)
            continue;

        out_distance[y][x] = 0;
        queue[tail++] = y * DUNGEON_COL_COUNT + x;
    }

    if (tail <= 0)
        return false;

    static const i32 neighbor_offsets[4][2] = {
        {1, 0},
        {0, 1},
        {-1, 0},
        {0, -1},
    };

    while (head < tail) {
        i32 idx = queue[head++];
        i32 x = idx % DUNGEON_COL_COUNT;
        i32 y = idx / DUNGEON_COL_COUNT;
        i16 next_distance = (i16)(out_distance[y][x] + 1);

        for (i32 n = 0; n < 4; n++) {
            i32 nx = x + neighbor_offsets[n][0];
            i32 ny = y + neighbor_offsets[n][1];
            if (!game_dungeon_cell_in_bounds(nx, ny))
                continue;
            if (out_distance[ny][nx] < 0)
                continue;
            if (next_distance >= out_distance[ny][nx])
                continue;

            out_distance[ny][nx] = next_distance;
            queue[tail++] = ny * DUNGEON_COL_COUNT + nx;
        }
    }

    return true;
}

static void game_dungeon_rebuild_player_dijkstra_map(Game *game)
{
    i16 goal_x[1] = {game->player_x};
    i16 goal_y[1] = {game->player_y};
    game->player_dijkstra_distance_valid =
        game_dungeon_build_dijkstra_map(game, goal_x, goal_y, 1, game->player_dijkstra_distance);
}

static u8 game_dungeon_get_orientation_from_step(i32 dx, i32 dy, u8 fallback)
{
    if (dx > 0)
        return 0;
    if (dy > 0)
        return 1;
    if (dy < 0)
        return 2;
    if (dx < 0)
        return 3;
    return fallback;
}

static void game_dungeon_take_enemy_turns(Game *game)
{
    if (game->unit_count <= 0)
        return;

    game_dungeon_rebuild_player_dijkstra_map(game);
    bool has_player_dijkstra = game->player_dijkstra_distance_valid;

    static const i32 neighbor_offsets[4][2] = {
        {1, 0},
        {0, 1},
        {-1, 0},
        {0, -1},
    };

    for (u8 unit_idx = 0; unit_idx < game->unit_count; unit_idx++) {
        Dungeon_Unit *unit = &game->units[unit_idx];
        i32 start_x = unit->x;
        i32 start_y = unit->y;
        if (!game_dungeon_cell_in_bounds(start_x, start_y))
            continue;

        bool in_player_los = game_dungeon_cell_is_in_player_los(game, start_x, start_y);
        if (in_player_los) {
            unit->is_awake = true;
            unit->turns_out_of_player_los = 0;
        } else if (unit->is_awake) {
            if (unit->turns_out_of_player_los >= DUNGEON_ENEMY_DORMANT_DELAY_TURNS)
                unit->is_awake = false;
            else
                unit->turns_out_of_player_los++;
        }

        if (!unit->is_awake)
            continue;

        i32 to_player_x = game->player_x - start_x;
        i32 to_player_y = game->player_y - start_y;
        bool player_is_adjacent = (to_player_x == 0 && (to_player_y == -1 || to_player_y == 1)) ||
                                  (to_player_y == 0 && (to_player_x == -1 || to_player_x == 1));
        if (player_is_adjacent) {
            unit->orientation =
                game_dungeon_get_orientation_from_step(to_player_x, to_player_y, unit->orientation);
            bool player_defeated =
                game_unit_stats_take_damage(&game->player_stats, unit->stats.damage);
            if (player_defeated)
                return;
            continue;
        }

        if (!has_player_dijkstra)
            continue;

        i16 best_distance = game->player_dijkstra_distance[start_y][start_x];
        if (best_distance <= 0 || best_distance >= DUNGEON_DIJKSTRA_UNREACHABLE)
            continue;

        i32 best_x = start_x;
        i32 best_y = start_y;

        for (i32 n = 0; n < 4; n++) {
            i32 nx = start_x + neighbor_offsets[n][0];
            i32 ny = start_y + neighbor_offsets[n][1];
            if (!game_dungeon_cell_is_floor(game, nx, ny))
                continue;
            if (game->player_x == nx && game->player_y == ny)
                continue;
            if (game_dungeon_cell_has_unit_excluding(game, nx, ny, unit_idx))
                continue;

            i16 next_distance = game->player_dijkstra_distance[ny][nx];
            if (next_distance < 0 || next_distance >= DUNGEON_DIJKSTRA_UNREACHABLE)
                continue;
            if (next_distance >= best_distance)
                continue;

            best_distance = next_distance;
            best_x = nx;
            best_y = ny;
        }

        if (best_x == start_x && best_y == start_y)
            continue;

        unit->x = (i16)best_x;
        unit->y = (i16)best_y;
        unit->orientation = game_dungeon_get_orientation_from_step(
            best_x - start_x, best_y - start_y, unit->orientation);
    }
}

static void game_dungeon_clear_spawn_to_exit_path(Game *game)
{
    memset(game->spawn_to_exit_path, 0, sizeof(game->spawn_to_exit_path));
    game->spawn_to_exit_path_len = 0;
    game->has_exit = false;
    game->exit_x = -1;
    game->exit_y = -1;
}

static bool game_dungeon_find_exit_cell(const Game *game, i16 *out_x, i16 *out_y)
{
    return game_dungeon_find_world_feature_by_role(game, WORLD_ART_ROLE_STAIRS_DOWN, out_x, out_y);
}

static bool game_dungeon_build_spawn_to_exit_path(Game *game)
{
    game_dungeon_clear_spawn_to_exit_path(game);

    i16 exit_x = 0;
    i16 exit_y = 0;
    if (!game_dungeon_find_exit_cell(game, &exit_x, &exit_y))
        return false;

    game->has_exit = true;
    game->exit_x = exit_x;
    game->exit_y = exit_y;

    i32 start_x = game->player_x;
    i32 start_y = game->player_y;
    if (!game_dungeon_cell_is_floor(game, start_x, start_y))
        return false;
    if (!game_dungeon_cell_is_floor(game, exit_x, exit_y))
        return false;

    i32 start_idx = start_y * DUNGEON_COL_COUNT + start_x;
    i32 exit_idx = exit_y * DUNGEON_COL_COUNT + exit_x;

    i32 parent[DUNGEON_CELL_COUNT];
    i32 queue[DUNGEON_CELL_COUNT];
    for (i32 idx = 0; idx < DUNGEON_CELL_COUNT; idx++)
        parent[idx] = -2;

    i32 head = 0;
    i32 tail = 0;
    queue[tail++] = start_idx;
    parent[start_idx] = -1;

    static const i32 neighbor_offsets[4][2] = {
        {1, 0},
        {0, 1},
        {-1, 0},
        {0, -1},
    };

    bool found = start_idx == exit_idx;
    while (!found && head < tail) {
        i32 current_idx = queue[head++];
        i32 current_x = current_idx % DUNGEON_COL_COUNT;
        i32 current_y = current_idx / DUNGEON_COL_COUNT;

        for (i32 n = 0; n < 4; n++) {
            i32 next_x = current_x + neighbor_offsets[n][0];
            i32 next_y = current_y + neighbor_offsets[n][1];
            if (!game_dungeon_cell_is_floor(game, next_x, next_y))
                continue;

            i32 next_idx = next_y * DUNGEON_COL_COUNT + next_x;
            if (parent[next_idx] != -2)
                continue;

            parent[next_idx] = current_idx;
            if (next_idx == exit_idx) {
                found = true;
                break;
            }

            queue[tail++] = next_idx;
        }
    }

    if (!found)
        return false;

    for (i32 idx = exit_idx; idx >= 0; idx = parent[idx]) {
        i32 x = idx % DUNGEON_COL_COUNT;
        i32 y = idx / DUNGEON_COL_COUNT;
        game->spawn_to_exit_path[y][x] = 1;
        game->spawn_to_exit_path_len++;
    }

    return game->spawn_to_exit_path_len > 0;
}

static bool game_dungeon_cell_is_on_spawn_to_exit_path(const Game *game, i32 x, i32 y)
{
    return game_dungeon_cell_in_bounds(x, y) && game->spawn_to_exit_path[y][x] != 0;
}

static bool
game_dungeon_build_distance_from_main_path(const Game *game,
                                           i16 distance[DUNGEON_ROW_COUNT][DUNGEON_COL_COUNT])
{
    for (i32 y = 0; y < DUNGEON_ROW_COUNT; y++) {
        for (i32 x = 0; x < DUNGEON_COL_COUNT; x++)
            distance[y][x] = -1;
    }

    i32 queue[DUNGEON_CELL_COUNT];
    i32 head = 0;
    i32 tail = 0;

    for (i32 y = 0; y < DUNGEON_ROW_COUNT; y++) {
        for (i32 x = 0; x < DUNGEON_COL_COUNT; x++) {
            if (!game_dungeon_cell_is_floor(game, x, y))
                continue;
            if (!game_dungeon_cell_is_on_spawn_to_exit_path(game, x, y))
                continue;

            distance[y][x] = 0;
            queue[tail++] = y * DUNGEON_COL_COUNT + x;
        }
    }

    if (tail <= 0)
        return false;

    static const i32 neighbor_offsets[4][2] = {
        {1, 0},
        {0, 1},
        {-1, 0},
        {0, -1},
    };

    while (head < tail) {
        i32 idx = queue[head++];
        i32 x = idx % DUNGEON_COL_COUNT;
        i32 y = idx / DUNGEON_COL_COUNT;
        i32 next_distance = (i32)distance[y][x] + 1;

        for (i32 n = 0; n < 4; n++) {
            i32 nx = x + neighbor_offsets[n][0];
            i32 ny = y + neighbor_offsets[n][1];
            if (!game_dungeon_cell_is_floor(game, nx, ny))
                continue;
            if (distance[ny][nx] >= 0)
                continue;

            distance[ny][nx] = (i16)next_distance;
            queue[tail++] = ny * DUNGEON_COL_COUNT + nx;
        }
    }

    return true;
}

static void game_dungeon_cull_outside_main_path(Game *game, RNG *rng)
{
    i32 floor_count = 0;
    i32 main_path_floor_count = 0;
    for (i32 y = 0; y < DUNGEON_ROW_COUNT; y++) {
        for (i32 x = 0; x < DUNGEON_COL_COUNT; x++) {
            if (!game_dungeon_cell_is_floor(game, x, y))
                continue;

            floor_count++;
            if (game_dungeon_cell_is_on_spawn_to_exit_path(game, x, y))
                main_path_floor_count++;
        }
    }

    if (floor_count <= 0 || main_path_floor_count <= 0)
        return;

    i32 keep_target = (floor_count * DUNGEON_MAIN_PATH_KEEP_PERCENT + 99) / 100;
    keep_target = max(keep_target, main_path_floor_count);
    if (keep_target >= floor_count)
        return;

    i16 path_distance[DUNGEON_ROW_COUNT][DUNGEON_COL_COUNT];
    if (!game_dungeon_build_distance_from_main_path(game, path_distance))
        return;

    u8 keep_state[DUNGEON_ROW_COUNT][DUNGEON_COL_COUNT];
    memset(keep_state, 0, sizeof(keep_state));

    i32 frontier[DUNGEON_CELL_COUNT];
    i32 frontier_count = 0;
    i32 kept_count = 0;

    static const i32 neighbor_offsets[4][2] = {
        {1, 0},
        {0, 1},
        {-1, 0},
        {0, -1},
    };

    for (i32 y = 0; y < DUNGEON_ROW_COUNT; y++) {
        for (i32 x = 0; x < DUNGEON_COL_COUNT; x++) {
            if (!game_dungeon_cell_is_floor(game, x, y))
                continue;

            bool must_keep = game_dungeon_cell_is_on_spawn_to_exit_path(game, x, y) ||
                             game_dungeon_cell_is_occupied(game, x, y);
            if (must_keep) {
                keep_state[y][x] = 3;
                kept_count++;
            } else {
                keep_state[y][x] = 1;
            }
        }
    }

    if (kept_count <= 0)
        return;

    for (i32 y = 0; y < DUNGEON_ROW_COUNT; y++) {
        for (i32 x = 0; x < DUNGEON_COL_COUNT; x++) {
            if (keep_state[y][x] != 3)
                continue;

            for (i32 n = 0; n < 4; n++) {
                i32 nx = x + neighbor_offsets[n][0];
                i32 ny = y + neighbor_offsets[n][1];
                if (!game_dungeon_cell_in_bounds(nx, ny))
                    continue;
                if (keep_state[ny][nx] != 1)
                    continue;

                keep_state[ny][nx] = 2;
                frontier[frontier_count++] = ny * DUNGEON_COL_COUNT + nx;
            }
        }
    }

    while (kept_count < keep_target && frontier_count > 0) {
        i32 best_slot = ck_rand_int(rng, 0, frontier_count);
        i32 best_score = -1000000;

        for (i32 sample = 0; sample < DUNGEON_MAIN_PATH_CULL_PICK_SAMPLES; sample++) {
            i32 slot = ck_rand_int(rng, 0, frontier_count);
            i32 idx = frontier[slot];
            i32 x = idx % DUNGEON_COL_COUNT;
            i32 y = idx / DUNGEON_COL_COUNT;
            if (keep_state[y][x] != 2)
                continue;

            i32 distance = path_distance[y][x];
            i32 kept_neighbor_count = 0;
            i32 floor_neighbor_count = 0;
            for (i32 n = 0; n < 4; n++) {
                i32 nx = x + neighbor_offsets[n][0];
                i32 ny = y + neighbor_offsets[n][1];
                if (!game_dungeon_cell_is_floor(game, nx, ny))
                    continue;

                floor_neighbor_count++;
                if (game_dungeon_cell_in_bounds(nx, ny) && keep_state[ny][nx] == 3)
                    kept_neighbor_count++;
            }

            i32 score = ck_rand_int(rng, 0, 24);
            if (distance >= 0)
                score += max(0, 36 - distance * 4);

            if (kept_neighbor_count <= 1)
                score += 10;
            else if (kept_neighbor_count == 2)
                score += 4;
            else
                score -= 8;

            if (floor_neighbor_count <= 2)
                score += 6;

            if (score > best_score) {
                best_score = score;
                best_slot = slot;
            }
        }

        i32 chosen_idx = frontier[best_slot];
        frontier[best_slot] = frontier[frontier_count - 1];
        frontier_count--;

        i32 chosen_x = chosen_idx % DUNGEON_COL_COUNT;
        i32 chosen_y = chosen_idx / DUNGEON_COL_COUNT;
        if (keep_state[chosen_y][chosen_x] != 2)
            continue;

        keep_state[chosen_y][chosen_x] = 3;
        kept_count++;

        for (i32 n = 0; n < 4; n++) {
            i32 nx = chosen_x + neighbor_offsets[n][0];
            i32 ny = chosen_y + neighbor_offsets[n][1];
            if (!game_dungeon_cell_in_bounds(nx, ny))
                continue;
            if (keep_state[ny][nx] != 1)
                continue;

            keep_state[ny][nx] = 2;
            frontier[frontier_count++] = ny * DUNGEON_COL_COUNT + nx;
        }
    }

    i32 removed_count = 0;
    for (i32 y = 0; y < DUNGEON_ROW_COUNT; y++) {
        for (i32 x = 0; x < DUNGEON_COL_COUNT; x++) {
            if (!game_dungeon_cell_is_floor(game, x, y))
                continue;
            if (keep_state[y][x] == 3)
                continue;

            game->dungeon_cells[y][x] = DUNGEON_CELL_EMPTY;
            removed_count++;
        }
    }

    if (removed_count > 0) {
        game_dungeon_clear_walls(game);
        game_dungeon_build_walls(game);
        game_dungeon_enforce_edge_walls(game);
    }
}

static bool game_dungeon_find_first_floor_cell(const Game *game, i16 *out_x, i16 *out_y)
{
    for (i32 y = 0; y < DUNGEON_ROW_COUNT; y++) {
        for (i32 x = 0; x < DUNGEON_COL_COUNT; x++) {
            if (!game_dungeon_cell_is_floor(game, x, y))
                continue;
            *out_x = (i16)x;
            *out_y = (i16)y;
            return true;
        }
    }

    return false;
}

static bool game_dungeon_pick_random_floor_cell(Game *game, RNG *rng, bool require_empty,
                                                i16 *out_x, i16 *out_y)
{
    for (i32 attempt = 0; attempt < 512; attempt++) {
        i32 x = ck_rand_int(rng, 0, DUNGEON_COL_COUNT);
        i32 y = ck_rand_int(rng, 0, DUNGEON_ROW_COUNT);
        if (!game_dungeon_cell_is_floor(game, x, y))
            continue;
        if (require_empty && game_dungeon_cell_is_occupied(game, x, y))
            continue;

        *out_x = (i16)x;
        *out_y = (i16)y;
        return true;
    }

    i32 total_cells = DUNGEON_COL_COUNT * DUNGEON_ROW_COUNT;
    i32 start_cell = ck_rand_int(rng, 0, total_cells);
    for (i32 step = 0; step < total_cells; step++) {
        i32 idx = (start_cell + step) % total_cells;
        i32 x = idx % DUNGEON_COL_COUNT;
        i32 y = idx / DUNGEON_COL_COUNT;
        if (!game_dungeon_cell_is_floor(game, x, y))
            continue;
        if (require_empty && game_dungeon_cell_is_occupied(game, x, y))
            continue;

        *out_x = (i16)x;
        *out_y = (i16)y;
        return true;
    }

    return false;
}

static bool game_dungeon_pick_exit_floor_cell(Game *game, RNG *rng, i32 min_path_steps, i16 *out_x,
                                              i16 *out_y)
{
    i32 start_x = game->player_spawn_x;
    i32 start_y = game->player_spawn_y;
    if (!game_dungeon_cell_is_floor(game, start_x, start_y))
        return false;

    i16 distance[DUNGEON_ROW_COUNT][DUNGEON_COL_COUNT];
    for (i32 y = 0; y < DUNGEON_ROW_COUNT; y++) {
        for (i32 x = 0; x < DUNGEON_COL_COUNT; x++)
            distance[y][x] = -1;
    }

    i32 queue[DUNGEON_CELL_COUNT];
    i32 head = 0;
    i32 tail = 0;
    queue[tail++] = start_y * DUNGEON_COL_COUNT + start_x;
    distance[start_y][start_x] = 0;

    static const i32 neighbor_offsets[4][2] = {
        {1, 0},
        {0, 1},
        {-1, 0},
        {0, -1},
    };

    while (head < tail) {
        i32 idx = queue[head++];
        i32 x = idx % DUNGEON_COL_COUNT;
        i32 y = idx / DUNGEON_COL_COUNT;
        i32 next_distance = (i32)distance[y][x] + 1;

        for (i32 n = 0; n < 4; n++) {
            i32 nx = x + neighbor_offsets[n][0];
            i32 ny = y + neighbor_offsets[n][1];
            if (!game_dungeon_cell_is_floor(game, nx, ny))
                continue;
            if (distance[ny][nx] >= 0)
                continue;

            distance[ny][nx] = (i16)next_distance;
            queue[tail++] = ny * DUNGEON_COL_COUNT + nx;
        }
    }

    i32 target_count = 0;
    i16 target_x = -1;
    i16 target_y = -1;

    for (i32 y = 0; y < DUNGEON_ROW_COUNT; y++) {
        for (i32 x = 0; x < DUNGEON_COL_COUNT; x++) {
            i16 steps = distance[y][x];
            if (steps <= 0)
                continue;
            if (game_dungeon_cell_is_occupied(game, x, y))
                continue;

            if (steps >= min_path_steps) {
                target_count++;
                if (ck_rand_int(rng, 0, target_count) == 0) {
                    target_x = (i16)x;
                    target_y = (i16)y;
                }
            }
        }
    }

    if (target_count > 0) {
        *out_x = target_x;
        *out_y = target_y;
        return true;
    }

    return false;
}

static bool game_dungeon_spawn_is_in_largest_component(const Game *game)
{
    i32 spawn_x = game->player_spawn_x;
    i32 spawn_y = game->player_spawn_y;
    if (!game_dungeon_cell_is_floor(game, spawn_x, spawn_y))
        return false;

    u8 visited[DUNGEON_ROW_COUNT][DUNGEON_COL_COUNT];
    memset(visited, 0, sizeof(visited));

    i32 queue[DUNGEON_CELL_COUNT];
    i32 spawn_component_size = 0;
    i32 largest_component_size = 0;

    static const i32 neighbor_offsets[4][2] = {
        {1, 0},
        {0, 1},
        {-1, 0},
        {0, -1},
    };

    for (i32 y = 0; y < DUNGEON_ROW_COUNT; y++) {
        for (i32 x = 0; x < DUNGEON_COL_COUNT; x++) {
            if (!game_dungeon_cell_is_floor(game, x, y) || visited[y][x] != 0)
                continue;

            i32 head = 0;
            i32 tail = 0;
            queue[tail++] = y * DUNGEON_COL_COUNT + x;
            visited[y][x] = 1;

            i32 component_size = 0;
            bool contains_spawn = false;

            while (head < tail) {
                i32 idx = queue[head++];
                i32 current_x = idx % DUNGEON_COL_COUNT;
                i32 current_y = idx / DUNGEON_COL_COUNT;
                component_size++;

                if (current_x == spawn_x && current_y == spawn_y)
                    contains_spawn = true;

                for (i32 n = 0; n < 4; n++) {
                    i32 next_x = current_x + neighbor_offsets[n][0];
                    i32 next_y = current_y + neighbor_offsets[n][1];
                    if (!game_dungeon_cell_is_floor(game, next_x, next_y))
                        continue;
                    if (visited[next_y][next_x] != 0)
                        continue;

                    visited[next_y][next_x] = 1;
                    queue[tail++] = next_y * DUNGEON_COL_COUNT + next_x;
                }
            }

            if (component_size > largest_component_size)
                largest_component_size = component_size;

            if (contains_spawn)
                spawn_component_size = component_size;
        }
    }

    return spawn_component_size > 0 && spawn_component_size == largest_component_size;
}

static void game_dungeon_populate_test_entities(Game *game, bool include_up_stairs)
{
    i16 x = 0;
    i16 y = 0;

    if (game_dungeon_pick_exit_floor_cell(game, &game->dungeon_populate_rng,
                                          DUNGEON_EXIT_MIN_PATH_STEPS, &x, &y)) {
        WORLD_ART_THEME theme =
            (WORLD_ART_THEME)ck_rand_int(&game->dungeon_populate_rng, 0, WORLD_ART_THEME_COUNT);
        game_dungeon_add_world_feature(game, x, y, world_art_get_down_stairs_tile(theme));

        if (game_dungeon_build_spawn_to_exit_path(game))
            game_dungeon_cull_outside_main_path(game, &game->dungeon_cull_rng);
    }

    if (include_up_stairs) {
        WORLD_ART_THEME up_stairs_theme =
            (WORLD_ART_THEME)ck_rand_int(&game->dungeon_populate_rng, 0, WORLD_ART_THEME_COUNT);
        game_dungeon_add_world_feature(game, game->player_spawn_x, game->player_spawn_y,
                                       world_art_get_up_stairs_tile(up_stairs_theme));
    }

    for (i32 grunt_idx = 0;
         grunt_idx < DUNGEON_GOBLIN_GRUNT_COUNT && game->unit_count < DUNGEON_MAX_UNITS;
         grunt_idx++) {
        if (!game_dungeon_pick_random_floor_cell(game, &game->dungeon_populate_rng, true, &x, &y))
            break;

        u8 orientation =
            (u8)ck_rand_int(&game->dungeon_populate_rng, 0, UNIT_ART_ORIENTATION_COUNT);
        game_dungeon_add_unit(game, x, y, UNIT_ART_GOBLIN_GRUNT, orientation);
    }
}

static bool game_build_test_dungeon_candidate(Game *game, u32 floor_index, bool include_up_stairs)
{
    game->dungeon_floor_index = floor_index;
    game_seed_dungeon_rng_streams(game);

    game->world_feature_count = 0;
    game->item_count = 0;
    game->unit_count = 0;
    game->player_x = -1;
    game->player_y = -1;
    game->player_orientation = PLAYER_START_ORIENTATION;
    game->player_dijkstra_distance_valid = false;
    game->player_spawn_x = -1;
    game->player_spawn_y = -1;
    game_dungeon_clear_spawn_to_exit_path(game);
    game_dungeon_reset_visibility(game);

    if (game->dungeon_tileset.valid)
        game->dungeon_template_error[0] = '\0';

    game_dungeon_fill(game, DUNGEON_CELL_EMPTY);
    bool generated = game_dungeon_generate_herringbone_layout(game);
    i16 first_floor_x = 0;
    i16 first_floor_y = 0;
    if (!generated || !game_dungeon_find_first_floor_cell(game, &first_floor_x, &first_floor_y))
        game_dungeon_carve_rect(game, 2, 2, DUNGEON_COL_COUNT - 4, DUNGEON_ROW_COUNT - 4);
    game_dungeon_build_walls(game);
    game_dungeon_enforce_edge_walls(game);

    i16 player_x = 0;
    i16 player_y = 0;
    bool found_player_cell = game_dungeon_pick_random_floor_cell(game, &game->dungeon_populate_rng,
                                                                 true, &player_x, &player_y);
    if (!found_player_cell)
        found_player_cell = game_dungeon_find_first_floor_cell(game, &player_x, &player_y);

    assert(found_player_cell);
    game->player_x = player_x;
    game->player_y = player_y;
    game->player_spawn_x = player_x;
    game->player_spawn_y = player_y;

    if (!game_dungeon_spawn_is_in_largest_component(game))
        return false;

    game_dungeon_populate_test_entities(game, include_up_stairs);
    if (game->unit_count != DUNGEON_GOBLIN_GRUNT_COUNT)
        return false;

    game_dungeon_build_spawn_to_exit_path(game);

    if (!game->has_exit || game->spawn_to_exit_path_len <= 0)
        return false;
    if (!game_dungeon_spawn_is_in_largest_component(game))
        return false;

    i32 step_count = game->spawn_to_exit_path_len - 1;
    if (step_count < DUNGEON_EXIT_MIN_PATH_STEPS)
        return false;

    assert(game_dungeon_cell_is_floor(game, game->player_x, game->player_y));
    game_dungeon_rebuild_line_of_sight(game);
    game_dungeon_rebuild_player_dijkstra_map(game);
    return game->player_dijkstra_distance_valid;
}

static bool game_dungeon_resolve_floor_index_for_depth(Game *game, u32 depth, u32 *out_floor_index)
{
    assert(out_floor_index != 0);

    if (depth >= DUNGEON_MAX_FLOOR_DEPTH) {
        snprintf(game->dungeon_template_error, sizeof(game->dungeon_template_error),
                 "Reached max floor depth (%d)", DUNGEON_MAX_FLOOR_DEPTH);
        return false;
    }

    while (game->dungeon_floor_seed_count <= depth) {
        u32 resolving_depth = game->dungeon_floor_seed_count;
        u32 start_floor_index =
            resolving_depth == 0 ? 0 : game->dungeon_floor_seed_by_depth[resolving_depth - 1] + 1;
        bool include_up_stairs = resolving_depth > 0;
        bool found_floor = false;

        for (i32 attempt = 0; attempt < DUNGEON_FLOOR_VALIDATION_RETRY_LIMIT; attempt++) {
            u32 candidate_floor_index = start_floor_index + (u32)attempt;
            if (!game_build_test_dungeon_candidate(game, candidate_floor_index, include_up_stairs))
                continue;

            game->dungeon_floor_seed_by_depth[resolving_depth] = candidate_floor_index;
            game->dungeon_floor_seed_count++;
            found_floor = true;
            break;
        }

        if (!found_floor) {
            snprintf(game->dungeon_template_error, sizeof(game->dungeon_template_error),
                     "No valid floor found for level %u in %d seeds", resolving_depth + 1,
                     DUNGEON_FLOOR_VALIDATION_RETRY_LIMIT);
            return false;
        }
    }

    *out_floor_index = game->dungeon_floor_seed_by_depth[depth];
    return true;
}

static bool game_build_test_dungeon_for_depth(Game *game, u32 depth, WORLD_ART_ROLE arrival_role)
{
    u32 floor_index = 0;
    if (!game_dungeon_resolve_floor_index_for_depth(game, depth, &floor_index))
        return false;

    bool include_up_stairs = depth > 0;
    if (!game_build_test_dungeon_candidate(game, floor_index, include_up_stairs)) {
        snprintf(game->dungeon_template_error, sizeof(game->dungeon_template_error),
                 "Failed to regenerate level %u", depth + 1);
        return false;
    }

    game->dungeon_depth = depth;

    if (arrival_role == WORLD_ART_ROLE_STAIRS_DOWN || arrival_role == WORLD_ART_ROLE_STAIRS_UP) {
        i16 arrival_x = 0;
        i16 arrival_y = 0;
        if (!game_dungeon_find_world_feature_by_role(game, arrival_role, &arrival_x, &arrival_y)) {
            snprintf(game->dungeon_template_error, sizeof(game->dungeon_template_error),
                     "Level %u is missing arrival stairs", depth + 1);
            return false;
        }

        game->player_x = arrival_x;
        game->player_y = arrival_y;
        game_dungeon_build_spawn_to_exit_path(game);
        game_dungeon_rebuild_line_of_sight(game);
        game_dungeon_rebuild_player_dijkstra_map(game);
    }

    return true;
}

static u32 game_dungeon_hash(i32 x, i32 y)
{
    u32 ux = (u32)(x + 97);
    u32 uy = (u32)(y + 131);
    return (ux * 73856093u) ^ (uy * 19349663u);
}

static u8 game_dungeon_pick_primary_weighted_variation(u32 hash, u8 variation_count)
{
    assert(variation_count > 0);
    assert(DUNGEON_PRIMARY_VARIATION_WEIGHT > 0);
    assert(DUNGEON_PRIMARY_VARIATION_WEIGHT < 100);
    if (variation_count == 1)
        return 0;

    u32 secondary_count = (u32)variation_count - 1;
    u32 secondary_weight = 100u - DUNGEON_PRIMARY_VARIATION_WEIGHT;
    u32 weighted_range = 100u * secondary_count;
    u32 primary_threshold = DUNGEON_PRIMARY_VARIATION_WEIGHT * secondary_count;
    u32 bucket = hash % weighted_range;
    if (bucket < primary_threshold)
        return 0;

    return (u8)(1u + ((bucket - primary_threshold) / secondary_weight));
}

static u8 game_dungeon_get_variation(i32 x, i32 y, u8 variation_count)
{
    u32 hash = game_dungeon_hash(x, y);
    return game_dungeon_pick_primary_weighted_variation(hash, variation_count);
}

static Dungeon_Floor_Palette game_dungeon_get_floor_palette(const Game *game)
{
    switch (game->dungeon_wall_theme) {
    case WORLD_ART_THEME_1:
        return (Dungeon_Floor_Palette){.floor_row = 0, .variation_start = 0, .variation_count = 6};
    case WORLD_ART_THEME_2:
        return (Dungeon_Floor_Palette){.floor_row = 1, .variation_start = 0, .variation_count = 6};
    case WORLD_ART_THEME_3:
        return (Dungeon_Floor_Palette){.floor_row = 1, .variation_start = 8, .variation_count = 6};
    case WORLD_ART_THEME_4:
        return (Dungeon_Floor_Palette){.floor_row = 0, .variation_start = 8, .variation_count = 6};
    default:
        return (Dungeon_Floor_Palette){.floor_row = 0, .variation_start = 0, .variation_count = 6};
    }
}

static bool game_dungeon_wall_shows_face(const Game *game, i32 x, i32 y)
{
    return game_dungeon_cell_is_floor(game, x, y + 1);
}

static World_Art_Tile game_dungeon_get_floor_tile(const Game *game, i32 x, i32 y)
{
    Dungeon_Floor_Palette palette = game_dungeon_get_floor_palette(game);
    u32 hash = game_dungeon_hash(x, y);
    assert(palette.floor_row < WORLD_ART_THEME_COUNT);
    assert(palette.variation_count > 0);

    u8 variation_offset =
        game_dungeon_pick_primary_weighted_variation(hash, palette.variation_count);
    u8 variation = (u8)(palette.variation_start + variation_offset);
    return world_art_get_floor_tile(palette.floor_row, variation);
}

static void game_draw_world_tile_tinted(Game *game, World_Art_Tile tile, Vector2 top_left,
                                        float sprite_size, float rotation_degrees, Color tint)
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
    DrawTexturePro(game->world_texture, src, dst, origin, rotation_degrees, tint);
}

static void game_draw_world_tile(Game *game, World_Art_Tile tile, Vector2 top_left,
                                 float sprite_size, float rotation_degrees)
{
    game_draw_world_tile_tinted(game, tile, top_left, sprite_size, rotation_degrees, WHITE);
}

static ITEM_ART_KIND game_get_item_draw_kind(ITEM_ART_KIND kind, int anim_frame)
{
    ITEM_ART_KIND draw_kind = kind;
    if (draw_kind == ITEM_ART_TORCH_1 || draw_kind == ITEM_ART_TORCH_2) {
        bool second_torch = draw_kind == ITEM_ART_TORCH_2;
        bool use_second_frame = ((anim_frame + (second_torch ? 1 : 0)) % 2) != 0;
        draw_kind = use_second_frame ? ITEM_ART_TORCH_2 : ITEM_ART_TORCH_1;
    }

    return draw_kind;
}

static void game_draw_item_tile(Game *game, ITEM_ART_KIND kind, Vector2 top_left, float sprite_size,
                                int anim_frame)
{
    ITEM_ART_KIND draw_kind = game_get_item_draw_kind(kind, anim_frame);

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

static void game_draw_item_tile_with_anchor(Game *game, ITEM_ART_KIND kind, Vector2 anchor_position,
                                            float sprite_size, int anim_frame)
{
    ITEM_ART_KIND draw_kind = game_get_item_draw_kind(kind, anim_frame);
    Item_Art_Pixel pixel = item_art_get_pixel(draw_kind);
    Item_Art_Anchor anchor = item_art_get_anchor(draw_kind);
    float source_to_dest_scale = sprite_size / (float)ITEM_ART_TILE_SIZE;

    Rectangle src = {
        .x = (float)pixel.x,
        .y = (float)pixel.y,
        .width = (float)ITEM_ART_TILE_SIZE,
        .height = (float)ITEM_ART_TILE_SIZE,
    };

    Rectangle dst = {
        .x = anchor_position.x - (anchor.x * source_to_dest_scale),
        .y = anchor_position.y - (anchor.y * source_to_dest_scale),
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

static void game_draw_unit_tile_with_feet_anchor(Game *game, UNIT_ART_KIND kind, u8 orientation,
                                                 Vector2 feet_position, float sprite_size,
                                                 int anim_frame)
{
    Unit_Art_Pixel pixel = unit_art_get_pixel(kind, (u8)anim_frame, orientation);
    Unit_Art_Anchor anchor = unit_art_get_anchor(kind, orientation);
    float source_to_dest_scale = sprite_size / (float)UNIT_ART_TILE_SIZE;

    Rectangle src = {
        .x = (float)pixel.x,
        .y = (float)pixel.y,
        .width = (float)UNIT_ART_TILE_SIZE,
        .height = (float)UNIT_ART_TILE_SIZE,
    };

    Rectangle dst = {
        .x = feet_position.x - (anchor.x * source_to_dest_scale),
        .y = feet_position.y - (anchor.y * source_to_dest_scale),
        .width = sprite_size,
        .height = sprite_size,
    };

    DrawTexturePro(game->units_texture, src, dst, (Vector2){0}, 0.0f, WHITE);
}

static void game_draw_anchor_cross(Vector2 anchor_position, float cross_half)
{
    Vector2 horizontal_start = {anchor_position.x - cross_half, anchor_position.y};
    Vector2 horizontal_end = {anchor_position.x + cross_half, anchor_position.y};
    Vector2 vertical_start = {anchor_position.x, anchor_position.y - cross_half};
    Vector2 vertical_end = {anchor_position.x, anchor_position.y + cross_half};
    DrawLineEx(horizontal_start, horizontal_end, 3.0f, (Color){8, 20, 24, 230});
    DrawLineEx(vertical_start, vertical_end, 3.0f, (Color){8, 20, 24, 230});
    DrawLineEx(horizontal_start, horizontal_end, 1.0f, (Color){255, 107, 94, 255});
    DrawLineEx(vertical_start, vertical_end, 1.0f, (Color){255, 107, 94, 255});
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

            Unit_Art_Anchor anchor = unit_art_get_anchor((UNIT_ART_KIND)kind, (u8)orientation);
            float sprite_scale = sprite_size / (float)UNIT_ART_TILE_SIZE;
            Vector2 anchor_pos = {
                .x = top_left.x + anchor.x * sprite_scale,
                .y = top_left.y + anchor.y * sprite_scale,
            };
            float cross_half = max(1.0f, sprite_size * 0.08f);
            game_draw_anchor_cross(anchor_pos, cross_half);
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
        Vector2 top_left = {sprite_x, sprite_y};

        game_draw_item_tile(game, (ITEM_ART_KIND)kind, top_left, sprite_size, anim_frame);

        ITEM_ART_KIND draw_kind = game_get_item_draw_kind((ITEM_ART_KIND)kind, anim_frame);
        Item_Art_Anchor anchor = item_art_get_anchor(draw_kind);
        float sprite_scale = sprite_size / (float)ITEM_ART_TILE_SIZE;
        Vector2 anchor_pos = {
            .x = top_left.x + anchor.x * sprite_scale,
            .y = top_left.y + anchor.y * sprite_scale,
        };
        float cross_half = max(1.0f, sprite_size * 0.08f);
        game_draw_anchor_cross(anchor_pos, cross_half);
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

static void game_draw_art_previews(Game *game)
{
    Vector2 unit_preview_size = game_get_unit_art_preview_size(game);
    Vector2 item_preview_size = game_get_item_art_preview_size(game);
    Vector2 world_preview_size = game_get_world_art_preview_size(game);
    float screen_w = (float)GetScreenWidth();

    Vector2 unit_origin = {
        .x = (screen_w - unit_preview_size.x) * 0.5f,
        .y = PREVIEW_TOP_MARGIN_Y,
    };

    Vector2 item_origin = {
        .x = (screen_w - item_preview_size.x) * 0.5f,
        .y = unit_origin.y + unit_preview_size.y + PREVIEW_SECTION_GAP_Y,
    };

    Vector2 world_origin = {
        .x = (screen_w - world_preview_size.x) * 0.5f,
        .y = item_origin.y + item_preview_size.y + PREVIEW_SECTION_GAP_Y,
    };

    game_draw_unit_art_preview(game, unit_origin);
    game_draw_item_art_preview(game, item_origin);
    game_draw_world_art_preview(game, world_origin);
}

static Vector2 game_dungeon_get_origin(void)
{
    float screen_w = (float)GetScreenWidth();
    float screen_h = (float)GetScreenHeight();
    float tile_size = WORLD_ART_TILE_SIZE * DUNGEON_TILE_SCALE;
    float map_w = (float)DUNGEON_COL_COUNT * tile_size;
    float map_h = (float)DUNGEON_ROW_COUNT * tile_size;
    return (Vector2){
        .x = (screen_w - map_w) * 0.5f,
        .y = (screen_h - map_h) * 0.5f,
    };
}

static Vector2 game_dungeon_get_cell_top_left(Vector2 origin, i32 x, i32 y, float tile_size)
{
    return (Vector2){
        .x = origin.x + (float)x * tile_size,
        .y = origin.y + (float)y * tile_size,
    };
}

static Vector2 game_dungeon_get_cell_center(Vector2 origin, i32 x, i32 y, float tile_size)
{
    return (Vector2){
        .x = origin.x + ((float)x + 0.5f) * tile_size,
        .y = origin.y + ((float)y + 0.5f) * tile_size,
    };
}

static Vector2 game_dungeon_get_player_center(const Game *game, float tile_size)
{
    Vector2 origin = game_dungeon_get_origin();
    return game_dungeon_get_cell_center(origin, game->player_x, game->player_y, tile_size);
}

static void game_update_dungeon_camera_offset(Game *game)
{
    game->dungeon_cam.offset = (Vector2){
        .x = (float)GetScreenWidth() * 0.5f,
        .y = (float)GetScreenHeight() * 0.5f,
    };
}

static void game_center_dungeon_camera_on_player(Game *game)
{
    game_update_dungeon_camera_offset(game);

    float tile_size = WORLD_ART_TILE_SIZE * DUNGEON_TILE_SCALE;
    Vector2 player_center = game_dungeon_get_player_center(game, tile_size);
    game->dungeon_cam.target = player_center;
}

static float game_round_camera_value_to_screen_px(float value, float zoom)
{
    float safe_zoom = zoom > 0.0f ? zoom : 1.0f;
    return roundf(value * safe_zoom) / safe_zoom;
}

static Camera2D game_get_active_camera(const Game *game)
{
    Camera2D cam = game->show_dungeon_map ? game->dungeon_cam : game->preview_cam;
    if (!game->show_dungeon_map)
        return cam;

    cam.offset.x = roundf(cam.offset.x);
    cam.offset.y = roundf(cam.offset.y);
    cam.target.x = game_round_camera_value_to_screen_px(cam.target.x, cam.zoom);
    cam.target.y = game_round_camera_value_to_screen_px(cam.target.y, cam.zoom);
    return cam;
}

static bool game_point_in_rect(Vector2 point, Rectangle rect)
{
    return point.x >= rect.x && point.y >= rect.y && point.x < (rect.x + rect.width) &&
           point.y < (rect.y + rect.height);
}

static bool game_dungeon_get_mouse_cell(const Game *game, i32 *out_x, i32 *out_y)
{
    assert(out_x != 0);
    assert(out_y != 0);

    Camera2D cam = game_get_active_camera(game);
    Vector2 mouse_world = GetScreenToWorld2D(GetMousePosition(), cam);
    float tile_size = WORLD_ART_TILE_SIZE * DUNGEON_TILE_SCALE;
    Vector2 origin = game_dungeon_get_origin();

    float local_x = mouse_world.x - origin.x;
    float local_y = mouse_world.y - origin.y;
    if (local_x < 0.0f || local_y < 0.0f)
        return false;

    i32 cell_x = (i32)(local_x / tile_size);
    i32 cell_y = (i32)(local_y / tile_size);
    if (!game_dungeon_cell_in_bounds(cell_x, cell_y))
        return false;

    *out_x = cell_x;
    *out_y = cell_y;
    return true;
}

static void game_draw_dungeon_ui_panel(Rectangle panel, Color outer_fill, Color inner_fill,
                                       Color border)
{
    Rectangle shadow = panel;
    shadow.x += 2.0f;
    shadow.y += 2.0f;
    DrawRectangleRounded(shadow, 0.08f, 8, (Color){0, 0, 0, 110});

    DrawRectangleRounded(panel, 0.08f, 8, outer_fill);
    DrawRectangleRounded(
        (Rectangle){panel.x + 3.0f, panel.y + 3.0f, panel.width - 6.0f, panel.height - 6.0f}, 0.08f,
        8, inner_fill);
    DrawRectangleLinesEx(panel, 2.0f, border);
    DrawRectangleLinesEx(
        (Rectangle){panel.x + 3.0f, panel.y + 3.0f, panel.width - 6.0f, panel.height - 6.0f}, 1.0f,
        (Color){82, 60, 40, 255});
}

static void game_draw_health_hearts(Game *game, Vector2 top_left, i32 health, i32 max_health,
                                    float icon_size, float gap)
{
    i32 clamped_max = max(1, max_health);
    i32 clamped_health = min(max(health, 0), clamped_max);
    int anim_frame = game_item_anim_frame();

    for (i32 idx = 0; idx < clamped_max; idx++) {
        ITEM_ART_KIND kind = idx < clamped_health ? ITEM_KIND_HEART : ITEM_KIND_EMPTY_HEART;
        Vector2 icon_pos = {
            .x = top_left.x + (float)idx * (icon_size + gap),
            .y = top_left.y,
        };
        game_draw_item_tile(game, kind, icon_pos, icon_size, anim_frame);
    }
}

static Rectangle game_draw_player_stats_panel(Game *game)
{
    char player_name[UNIT_ART_DISPLAY_NAME_CAP];
    unit_art_get_display_name(UNIT_ART_WARLOCK, player_name, UNIT_ART_DISPLAY_NAME_CAP);

    char stat_line[96];
    snprintf(stat_line, sizeof(stat_line), "Damage %d   Speed %d   Floor %u",
             (i32)game->player_stats.damage, (i32)game->player_stats.speed,
             game->dungeon_depth + 1);

    char health_text[32];
    snprintf(health_text, sizeof(health_text), "%d/%d", (i32)game->player_stats.health,
             (i32)game->player_stats.max_health);

    float title_size = 24.0f;
    float line_size = 17.0f;
    i32 heart_count = max((i32)game->player_stats.max_health, 1);
    float hearts_w = (float)heart_count * DUNGEON_PLAYER_PANEL_HEART_SIZE +
                     (float)(heart_count - 1) * DUNGEON_PLAYER_PANEL_HEART_GAP;

    Vector2 title_measure = MeasureTextEx(game->font, player_name, title_size, game->font_spacing);
    Vector2 line_measure = MeasureTextEx(game->font, stat_line, line_size, game->font_spacing);
    Vector2 health_measure = MeasureTextEx(game->font, health_text, line_size, game->font_spacing);

    float content_w = max(title_measure.x, line_measure.x);
    content_w = max(content_w, hearts_w + health_measure.x + 14.0f);

    float panel_w = content_w + (DUNGEON_PLAYER_PANEL_PADDING * 2.0f);
    float panel_h = (DUNGEON_PLAYER_PANEL_PADDING * 2.0f) + title_size + 10.0f +
                    DUNGEON_PLAYER_PANEL_HEART_SIZE + 8.0f + line_size;

    Rectangle panel = {
        .x = DUNGEON_PLAYER_PANEL_MARGIN,
        .y = DUNGEON_PLAYER_PANEL_MARGIN,
        .width = panel_w,
        .height = panel_h,
    };

    game_draw_dungeon_ui_panel(panel, (Color){20, 15, 12, 236}, (Color){32, 23, 17, 232},
                               (Color){146, 113, 72, 255});

    Rectangle header = {
        .x = panel.x + 7.0f,
        .y = panel.y + 7.0f,
        .width = panel.width - 14.0f,
        .height = title_size + 4.0f,
    };
    DrawRectangleRounded(header, 0.12f, 6, (Color){54, 38, 25, 242});

    Vector2 title_pos = {
        .x = panel.x + DUNGEON_PLAYER_PANEL_PADDING,
        .y = panel.y + DUNGEON_PLAYER_PANEL_PADDING - 1.0f,
    };
    DrawTextEx(game->font, player_name, title_pos, title_size, game->font_spacing,
               (Color){236, 215, 178, 255});

    Vector2 hearts_pos = {
        .x = panel.x + DUNGEON_PLAYER_PANEL_PADDING,
        .y = title_pos.y + title_size + 8.0f,
    };
    game_draw_health_hearts(game, hearts_pos, game->player_stats.health,
                            game->player_stats.max_health, DUNGEON_PLAYER_PANEL_HEART_SIZE,
                            DUNGEON_PLAYER_PANEL_HEART_GAP);

    Vector2 health_text_pos = {
        .x = panel.x + panel.width - DUNGEON_PLAYER_PANEL_PADDING - health_measure.x,
        .y = hearts_pos.y + 3.0f,
    };
    DrawTextEx(game->font, health_text, health_text_pos, line_size, game->font_spacing,
               (Color){219, 196, 154, 255});

    Vector2 line_pos = {
        .x = panel.x + DUNGEON_PLAYER_PANEL_PADDING,
        .y = hearts_pos.y + DUNGEON_PLAYER_PANEL_HEART_SIZE + 8.0f,
    };
    DrawTextEx(game->font, stat_line, line_pos, line_size, game->font_spacing,
               (Color){185, 164, 133, 255});

    return panel;
}

static void game_draw_hovered_unit_stats(Game *game, Rectangle player_panel,
                                         Rectangle minimap_panel)
{
    Vector2 mouse_screen = GetMousePosition();
    if (game_point_in_rect(mouse_screen, player_panel))
        return;
    if (game_point_in_rect(mouse_screen, minimap_panel))
        return;

    i32 hover_x = 0;
    i32 hover_y = 0;
    if (!game_dungeon_get_mouse_cell(game, &hover_x, &hover_y))
        return;

    bool has_hovered_unit = false;
    Unit_Stats hovered_stats = {0};
    char hovered_name[UNIT_ART_DISPLAY_NAME_CAP];

    if (game->player_x == hover_x && game->player_y == hover_y &&
        game_dungeon_cell_is_visible(game, hover_x, hover_y)) {
        char player_name[UNIT_ART_DISPLAY_NAME_CAP];
        unit_art_get_display_name(UNIT_ART_WARLOCK, player_name, UNIT_ART_DISPLAY_NAME_CAP);
        snprintf(hovered_name, sizeof(hovered_name), "%s (You)", player_name);
        hovered_stats = game->player_stats;
        has_hovered_unit = true;
    } else {
        const Dungeon_Unit *unit = game_dungeon_get_unit_at(game, hover_x, hover_y);
        if (unit == 0)
            return;
        if (!game_dungeon_cell_is_visible(game, unit->x, unit->y))
            return;

        unit_art_get_display_name(unit->kind, hovered_name, UNIT_ART_DISPLAY_NAME_CAP);
        hovered_stats = unit->stats;
        has_hovered_unit = true;
    }

    if (!has_hovered_unit)
        return;

    char health_line[32];
    snprintf(health_line, sizeof(health_line), "Health  %d/%d", (i32)hovered_stats.health,
             (i32)hovered_stats.max_health);

    char damage_line[32];
    snprintf(damage_line, sizeof(damage_line), "Damage  %d", (i32)hovered_stats.damage);

    char speed_line[32];
    snprintf(speed_line, sizeof(speed_line), "Speed   %d", (i32)hovered_stats.speed);

    float title_size = 19.0f;
    float line_size = 16.0f;
    float line_gap = 4.0f;

    Vector2 title_measure = MeasureTextEx(game->font, hovered_name, title_size, game->font_spacing);
    Vector2 health_measure = MeasureTextEx(game->font, health_line, line_size, game->font_spacing);
    Vector2 damage_measure = MeasureTextEx(game->font, damage_line, line_size, game->font_spacing);
    Vector2 speed_measure = MeasureTextEx(game->font, speed_line, line_size, game->font_spacing);

    float panel_w = max(title_measure.x, health_measure.x);
    panel_w = max(panel_w, damage_measure.x);
    panel_w = max(panel_w, speed_measure.x);
    panel_w += DUNGEON_HOVER_PANEL_PADDING * 2.0f;

    float panel_h = (DUNGEON_HOVER_PANEL_PADDING * 2.0f) + title_size + 8.0f + (line_size * 3.0f) +
                    (line_gap * 2.0f);

    Rectangle panel = {
        .x = mouse_screen.x + 18.0f,
        .y = mouse_screen.y + 20.0f,
        .width = panel_w,
        .height = panel_h,
    };

    float screen_w = (float)GetScreenWidth();
    float screen_h = (float)GetScreenHeight();

    if (panel.x + panel.width > screen_w - 8.0f)
        panel.x = mouse_screen.x - panel.width - 18.0f;
    if (panel.y + panel.height > screen_h - 8.0f)
        panel.y = screen_h - panel.height - 8.0f;
    panel.x = clamp(panel.x, 8.0f, screen_w - panel.width - 8.0f);
    panel.y = clamp(panel.y, 8.0f, screen_h - panel.height - 8.0f);

    game_draw_dungeon_ui_panel(panel, (Color){19, 15, 12, 244}, (Color){31, 23, 18, 240},
                               (Color){143, 108, 71, 255});

    Vector2 title_pos = {
        .x = panel.x + DUNGEON_HOVER_PANEL_PADDING,
        .y = panel.y + DUNGEON_HOVER_PANEL_PADDING - 1.0f,
    };
    DrawTextEx(game->font, hovered_name, title_pos, title_size, game->font_spacing,
               (Color){236, 214, 175, 255});

    Vector2 line_pos = {
        .x = panel.x + DUNGEON_HOVER_PANEL_PADDING,
        .y = title_pos.y + title_size + 8.0f,
    };
    DrawTextEx(game->font, health_line, line_pos, line_size, game->font_spacing,
               (Color){215, 194, 158, 255});

    line_pos.y += line_size + line_gap;
    DrawTextEx(game->font, damage_line, line_pos, line_size, game->font_spacing,
               (Color){194, 173, 141, 255});

    line_pos.y += line_size + line_gap;
    DrawTextEx(game->font, speed_line, line_pos, line_size, game->font_spacing,
               (Color){194, 173, 141, 255});
}

static void game_draw_dungeon_dijkstra_overlay(Game *game, Vector2 origin, float tile_size)
{
    if (!game_debug_feature_is_enabled(game, DEBUG_FEATURE_SHOW_PLAYER_DIJKSTRA))
        return;
    if (!game->player_dijkstra_distance_valid)
        return;

    float text_size = clamp(tile_size * 0.2f, 10.0f, 24.0f);

    for (i32 y = 0; y < DUNGEON_ROW_COUNT; y++) {
        for (i32 x = 0; x < DUNGEON_COL_COUNT; x++) {
            if (!game_dungeon_cell_is_explored(game, x, y))
                continue;

            i16 distance = game->player_dijkstra_distance[y][x];
            Color text_color = game_dungeon_cell_is_visible(game, x, y)
                                   ? (Color){236, 248, 250, 245}
                                   : (Color){154, 172, 178, 230};
            char text[8];

            if (distance < 0) {
                text_color = game_dungeon_cell_is_visible(game, x, y) ? (Color){115, 130, 136, 230}
                                                                      : (Color){89, 100, 106, 220};
                snprintf(text, sizeof(text), "##");
            } else if (distance >= DUNGEON_DIJKSTRA_UNREACHABLE) {
                text_color = game_dungeon_cell_is_visible(game, x, y) ? (Color){255, 145, 124, 240}
                                                                      : (Color){188, 112, 98, 230};
                snprintf(text, sizeof(text), "##");
            } else {
                snprintf(text, sizeof(text), "%d", (i32)distance);
            }

            Vector2 top_left = game_dungeon_get_cell_top_left(origin, x, y, tile_size);
            Vector2 text_draw_size = MeasureTextEx(game->font, text, text_size, game->font_spacing);
            Vector2 text_pos = {
                .x = top_left.x + (tile_size - text_draw_size.x) * 0.5f,
                .y = top_left.y + (tile_size - text_draw_size.y) * 0.5f,
            };
            DrawTextEx(game->font, text, text_pos, text_size, game->font_spacing, text_color);
        }
    }
}

static void game_draw_test_dungeon(Game *game)
{
    float tile_size = WORLD_ART_TILE_SIZE * DUNGEON_TILE_SCALE;
    Vector2 origin = game_dungeon_get_origin();
    bool show_spawn_to_exit_path =
        game_debug_feature_is_enabled(game, DEBUG_FEATURE_SHOW_SPAWN_TO_EXIT_PATH);
    Color remembered_tint = (Color){128, 142, 148, 255};
    Color path_visible_tint = (Color){56, 176, 218, 88};
    Color path_remembered_tint = (Color){35, 94, 110, 72};

    for (i32 y = 0; y < DUNGEON_ROW_COUNT; y++) {
        for (i32 x = 0; x < DUNGEON_COL_COUNT; x++) {
            if (!game_dungeon_cell_is_explored(game, x, y))
                continue;

            Vector2 top_left = game_dungeon_get_cell_top_left(origin, x, y, tile_size);
            u8 cell = game->dungeon_cells[y][x];
            Color tile_tint = game_dungeon_cell_is_visible(game, x, y) ? WHITE : remembered_tint;

            if (cell == DUNGEON_CELL_FLOOR) {
                game_draw_world_tile_tinted(game, game_dungeon_get_floor_tile(game, x, y), top_left,
                                            tile_size, 0.0f, tile_tint);
            } else if (cell == DUNGEON_CELL_WALL) {
                WORLD_ART_THEME theme = game->dungeon_wall_theme;
                if (game_dungeon_wall_shows_face(game, x, y)) {
                    u8 variation = game_dungeon_get_variation(x, y, WORLD_ART_WALL_VARIATION_COUNT);
                    game_draw_world_tile_tinted(game,
                                                world_art_get_north_wall_tile(theme, variation),
                                                top_left, tile_size, 0.0f, tile_tint);
                } else {
                    u8 variation =
                        game_dungeon_get_variation(x, y, WORLD_ART_WALL_TOP_VARIATION_COUNT);
                    game_draw_world_tile_tinted(game, world_art_get_top_wall_tile(theme, variation),
                                                top_left, tile_size, 0.0f, tile_tint);
                }
            }
        }
    }

    if (show_spawn_to_exit_path) {
        for (i32 y = 0; y < DUNGEON_ROW_COUNT; y++) {
            for (i32 x = 0; x < DUNGEON_COL_COUNT; x++) {
                if (game->spawn_to_exit_path[y][x] == 0)
                    continue;
                if (!game_dungeon_cell_is_explored(game, x, y))
                    continue;

                Vector2 top_left = game_dungeon_get_cell_top_left(origin, x, y, tile_size);
                Color path_tint = game_dungeon_cell_is_visible(game, x, y) ? path_visible_tint
                                                                           : path_remembered_tint;
                DrawRectangleRec((Rectangle){top_left.x, top_left.y, tile_size, tile_size},
                                 path_tint);
            }
        }
    }

    for (u8 feature_idx = 0; feature_idx < game->world_feature_count; feature_idx++) {
        Dungeon_World_Feature feature = game->world_features[feature_idx];
        if (!game_dungeon_cell_is_explored(game, feature.x, feature.y))
            continue;

        Color feature_tint =
            game_dungeon_cell_is_visible(game, feature.x, feature.y) ? WHITE : remembered_tint;
        Vector2 top_left = game_dungeon_get_cell_top_left(origin, feature.x, feature.y, tile_size);
        game_draw_world_tile_tinted(game, feature.tile, top_left, tile_size, 0.0f, feature_tint);
    }

    int item_anim_frame = game_item_anim_frame();
    for (u8 item_idx = 0; item_idx < game->item_count; item_idx++) {
        Dungeon_Item item = game->items[item_idx];
        if (!game_dungeon_cell_is_visible(game, item.x, item.y))
            continue;

        Vector2 anchor_position = game_dungeon_get_cell_center(origin, item.x, item.y, tile_size);
        game_draw_item_tile_with_anchor(game, item.kind, anchor_position, tile_size,
                                        item_anim_frame);
    }

    int unit_anim_frame = game_unit_anim_frame();
    for (u8 unit_idx = 0; unit_idx < game->unit_count; unit_idx++) {
        Dungeon_Unit unit = game->units[unit_idx];
        if (!game_dungeon_cell_is_visible(game, unit.x, unit.y))
            continue;

        Vector2 feet_position = game_dungeon_get_cell_center(origin, unit.x, unit.y, tile_size);
        game_draw_unit_tile_with_feet_anchor(game, unit.kind, unit.orientation, feet_position,
                                             tile_size, unit_anim_frame);
    }

    Vector2 player_feet =
        game_dungeon_get_cell_center(origin, game->player_x, game->player_y, tile_size);
    game_draw_unit_tile_with_feet_anchor(game, UNIT_ART_WARLOCK, game->player_orientation,
                                         player_feet, tile_size, unit_anim_frame);

    if (show_spawn_to_exit_path && game->player_spawn_x >= 0 && game->player_spawn_y >= 0 &&
        game_dungeon_cell_is_explored(game, game->player_spawn_x, game->player_spawn_y)) {
        Color spawn_color =
            game_dungeon_cell_is_visible(game, game->player_spawn_x, game->player_spawn_y)
                ? (Color){124, 255, 172, 230}
                : (Color){84, 161, 118, 220};
        Vector2 spawn_top_left = game_dungeon_get_cell_top_left(origin, game->player_spawn_x,
                                                                game->player_spawn_y, tile_size);
        DrawRectangleLinesEx((Rectangle){spawn_top_left.x, spawn_top_left.y, tile_size, tile_size},
                             3.0f, spawn_color);
    }

    if (show_spawn_to_exit_path && game->has_exit &&
        game_dungeon_cell_is_explored(game, game->exit_x, game->exit_y)) {
        Color exit_color = game_dungeon_cell_is_visible(game, game->exit_x, game->exit_y)
                               ? (Color){255, 208, 108, 230}
                               : (Color){171, 133, 72, 220};
        Vector2 exit_top_left =
            game_dungeon_get_cell_top_left(origin, game->exit_x, game->exit_y, tile_size);
        DrawRectangleLinesEx((Rectangle){exit_top_left.x, exit_top_left.y, tile_size, tile_size},
                             3.0f, exit_color);
    }

    game_draw_dungeon_dijkstra_overlay(game, origin, tile_size);
}

static bool game_update_player(Game *game)
{
    i32 move_x = 0;
    i32 move_y = 0;

    if (game->input.pressed[INPUT_MOVE_UP]) {
        move_y = -1;
        game->player_orientation = 2;
    } else if (game->input.pressed[INPUT_MOVE_DOWN]) {
        move_y = 1;
        game->player_orientation = 1;
    } else if (game->input.pressed[INPUT_MOVE_LEFT]) {
        move_x = -1;
        game->player_orientation = 3;
    } else if (game->input.pressed[INPUT_MOVE_RIGHT]) {
        move_x = 1;
        game->player_orientation = 0;
    }

    if (move_x == 0 && move_y == 0)
        return false;

    i32 next_x = game->player_x + move_x;
    i32 next_y = game->player_y + move_y;
    if (!game_dungeon_cell_is_floor(game, next_x, next_y))
        return false;

    i32 target_unit_idx = game_dungeon_get_unit_index_at(game, next_x, next_y);
    if (target_unit_idx >= 0) {
        Dungeon_Unit *target_unit = &game->units[target_unit_idx];
        target_unit->is_awake = true;
        target_unit->turns_out_of_player_los = 0;

        bool unit_defeated =
            game_unit_stats_take_damage(&target_unit->stats, game->player_stats.damage);
        if (unit_defeated)
            game_dungeon_remove_unit_at(game, target_unit_idx);
        return true;
    }

    game->player_x = (i16)next_x;
    game->player_y = (i16)next_y;

    WORLD_ART_ROLE stepped_feature_role = WORLD_ART_ROLE_FLOOR;
    if (game_dungeon_get_world_feature_role_at(game, next_x, next_y, &stepped_feature_role)) {
        if (stepped_feature_role == WORLD_ART_ROLE_STAIRS_DOWN) {
            if (game->dungeon_depth + 1 < DUNGEON_MAX_FLOOR_DEPTH &&
                game_build_test_dungeon_for_depth(game, game->dungeon_depth + 1,
                                                  WORLD_ART_ROLE_STAIRS_UP)) {
                game_center_dungeon_camera_on_player(game);
                return true;
            }
        }

        if (stepped_feature_role == WORLD_ART_ROLE_STAIRS_UP && game->dungeon_depth > 0) {
            if (game_build_test_dungeon_for_depth(game, game->dungeon_depth - 1,
                                                  WORLD_ART_ROLE_STAIRS_DOWN)) {
                game_center_dungeon_camera_on_player(game);
                return true;
            }
        }
    }

    game_dungeon_build_spawn_to_exit_path(game);
    game_dungeon_rebuild_line_of_sight(game);
    game_dungeon_rebuild_player_dijkstra_map(game);
    return true;
}

static void game_update_camera(Game *game)
{
    if (!game->show_dungeon_map)
        return;

    game_update_dungeon_camera_offset(game);

    float frame_time = GetFrameTime();
    if (frame_time <= 0.0f)
        frame_time = CAMERA_FALLBACK_FRAME_TIME;

    float tile_size = WORLD_ART_TILE_SIZE * DUNGEON_TILE_SCALE;
    Vector2 desired_target = game_dungeon_get_player_center(game, tile_size);

    float delta_x = desired_target.x - game->dungeon_cam.target.x;
    float delta_y = desired_target.y - game->dungeon_cam.target.y;
    float abs_delta_x = delta_x < 0.0f ? -delta_x : delta_x;
    float abs_delta_y = delta_y < 0.0f ? -delta_y : delta_y;
    float delta_tiles = max(abs_delta_x, abs_delta_y) / tile_size;
    float speed_t = clamp(delta_tiles / CAMERA_FOLLOW_ACCEL_DISTANCE_TILES, 0.0f, 1.0f);
    float follow_speed =
        CAMERA_FOLLOW_SPEED_NEAR + (CAMERA_FOLLOW_SPEED_FAR - CAMERA_FOLLOW_SPEED_NEAR) * speed_t;
    float follow_alpha = clamp(follow_speed * frame_time, 0.0f, 1.0f);

    game->dungeon_cam.target.x += delta_x * follow_alpha;
    game->dungeon_cam.target.y += delta_y * follow_alpha;

    float settle_distance = CAMERA_SETTLE_SCREEN_PX / max(game->dungeon_cam.zoom, 0.001f);
    if (abs_delta_x <= settle_distance)
        game->dungeon_cam.target.x = desired_target.x;
    if (abs_delta_y <= settle_distance)
        game->dungeon_cam.target.y = desired_target.y;
}

static void game_adjust_dungeon_zoom(Game *game, float delta)
{
    game->dungeon_cam.zoom =
        clamp(game->dungeon_cam.zoom + delta, DUNGEON_CAMERA_ZOOM_MIN, DUNGEON_CAMERA_ZOOM_MAX);
}

static Rectangle game_draw_dungeon_minimap(Game *game)
{
    float screen_w = (float)GetScreenWidth();
    float screen_h = (float)GetScreenHeight();
    float map_max_w = min(DUNGEON_MINIMAP_MAX_WIDTH, screen_w - (DUNGEON_MINIMAP_MARGIN * 2.0f) -
                                                         (DUNGEON_MINIMAP_PANEL_PADDING * 2.0f));
    float map_max_h = min(DUNGEON_MINIMAP_MAX_HEIGHT, screen_h - (DUNGEON_MINIMAP_MARGIN * 2.0f) -
                                                          (DUNGEON_MINIMAP_PANEL_PADDING * 2.0f));
    map_max_w = max(map_max_w, 1.0f);
    map_max_h = max(map_max_h, 1.0f);

    i32 minimap_cell_px =
        (i32)min(map_max_w / (float)DUNGEON_COL_COUNT, map_max_h / (float)DUNGEON_ROW_COUNT);
    minimap_cell_px = max(minimap_cell_px, 1);

    float cell_size = (float)minimap_cell_px;
    float map_w = (float)DUNGEON_COL_COUNT * cell_size;
    float map_h = (float)DUNGEON_ROW_COUNT * cell_size;
    float panel_w = map_w + (DUNGEON_MINIMAP_PANEL_PADDING * 2.0f);
    float panel_h = map_h + (DUNGEON_MINIMAP_PANEL_PADDING * 2.0f);

    Rectangle panel = {
        .x = screen_w - DUNGEON_MINIMAP_MARGIN - panel_w,
        .y = DUNGEON_MINIMAP_MARGIN,
        .width = panel_w,
        .height = panel_h,
    };
    panel.x = max(panel.x, DUNGEON_MINIMAP_MARGIN);

    game_draw_dungeon_ui_panel(panel, (Color){20, 15, 12, 234}, (Color){29, 22, 17, 232},
                               (Color){133, 103, 67, 255});

    Vector2 map_origin = {
        .x = panel.x + DUNGEON_MINIMAP_PANEL_PADDING,
        .y = panel.y + DUNGEON_MINIMAP_PANEL_PADDING,
    };
    DrawRectangleRec((Rectangle){map_origin.x, map_origin.y, map_w, map_h}, (Color){10, 8, 7, 255});

    Color floor_visible = (Color){161, 148, 126, 255};
    Color floor_explored = (Color){97, 88, 74, 255};
    Color wall_visible = (Color){121, 108, 88, 255};
    Color wall_explored = (Color){70, 60, 47, 255};
    Color exit_visible = (Color){241, 195, 92, 255};
    Color exit_explored = (Color){150, 111, 58, 255};

    for (i32 y = 0; y < DUNGEON_ROW_COUNT; y++) {
        for (i32 x = 0; x < DUNGEON_COL_COUNT; x++) {
            if (!game_dungeon_cell_is_explored(game, x, y))
                continue;

            u8 cell = game->dungeon_cells[y][x];
            bool visible = game_dungeon_cell_is_visible(game, x, y);
            Color tint = floor_explored;
            if (cell == DUNGEON_CELL_WALL)
                tint = visible ? wall_visible : wall_explored;
            else if (cell == DUNGEON_CELL_FLOOR)
                tint = visible ? floor_visible : floor_explored;
            if (game->has_exit && x == game->exit_x && y == game->exit_y)
                tint = visible ? exit_visible : exit_explored;

            DrawRectangleRec((Rectangle){map_origin.x + (float)x * cell_size,
                                         map_origin.y + (float)y * cell_size, cell_size, cell_size},
                             tint);
        }
    }

    DrawRectangleLinesEx((Rectangle){map_origin.x, map_origin.y, map_w, map_h}, 1.0f,
                         (Color){90, 70, 46, 255});

    float enemy_radius = max(1.0f, cell_size * 0.5f);
    for (u8 unit_idx = 0; unit_idx < game->unit_count; unit_idx++) {
        Dungeon_Unit unit = game->units[unit_idx];
        if (!game_dungeon_cell_in_bounds(unit.x, unit.y))
            continue;
        if (!game_dungeon_cell_is_visible(game, unit.x, unit.y))
            continue;

        Vector2 enemy_center = {
            .x = map_origin.x + ((float)unit.x + 0.5f) * cell_size,
            .y = map_origin.y + ((float)unit.y + 0.5f) * cell_size,
        };
        DrawCircleV(enemy_center, enemy_radius, (Color){230, 104, 92, 255});
        if (enemy_radius > 1.5f) {
            DrawCircleLines((i32)(enemy_center.x + 0.5f), (i32)(enemy_center.y + 0.5f),
                            enemy_radius, (Color){87, 32, 31, 255});
        }
    }

    if (game_dungeon_cell_in_bounds(game->player_x, game->player_y) &&
        game_dungeon_cell_is_explored(game, game->player_x, game->player_y)) {
        Vector2 player_center = {
            .x = map_origin.x + ((float)game->player_x + 0.5f) * cell_size,
            .y = map_origin.y + ((float)game->player_y + 0.5f) * cell_size,
        };
        float radius = max(2.0f, cell_size * 0.75f);
        DrawCircleV(player_center, radius, (Color){255, 224, 121, 255});
        DrawCircleLines((i32)(player_center.x + 0.5f), (i32)(player_center.y + 0.5f), radius,
                        (Color){101, 63, 24, 255});
    }

    return panel;
}

static void game_draw_dungeon_hud(Game *game)
{
    Rectangle player_panel = game_draw_player_stats_panel(game);

    float label_size = 18.0f;
    Vector2 origin = {18.0f, player_panel.y + player_panel.height + 14.0f};
    Color text_color = (Color){222, 201, 166, 255};
    Color subtle = (Color){180, 160, 131, 255};
    bool show_spawn_to_exit_path =
        game_debug_feature_is_enabled(game, DEBUG_FEATURE_SHOW_SPAWN_TO_EXIT_PATH);

    char line[256];
    if (GAME_DEBUG_FEATURES) {
        snprintf(line, sizeof(line), "Template [Q/E]: %s (%d/%d)",
                 game_dungeon_active_template_name(game), game->dungeon_template_index + 1,
                 DUNGEON_HBW_TEMPLATE_COUNT);
        DrawTextEx(game->font, line, origin, label_size, game->font_spacing, text_color);

        origin.y += 22.0f;
    }

    if (GAME_DEBUG_FEATURES) {
        snprintf(line, sizeof(line), "Wheel/-/=: zoom  0: reset  Zoom: %.2fx",
                 game->dungeon_cam.zoom);
        DrawTextEx(game->font, line, origin, label_size - 1.0f, game->font_spacing, subtle);
    }

    if (GAME_DEBUG_FEATURES) {
        origin.y += 22.0f;
        DrawTextEx(game->font, "Debug controls", origin, label_size - 1.0f, game->font_spacing,
                   (Color){220, 190, 145, 255});

        for (i32 idx = 0; idx < NUM_DEBUG_ACTIONS; idx++) {
            const Debug_Action_Def *def = &game_debug_action_defs[idx];

            origin.y += 20.0f;
            snprintf(line, sizeof(line), "[%s] %s", def->hotkey, def->name);
            DrawTextEx(game->font, line, origin, label_size - 1.0f, game->font_spacing,
                       (Color){164, 146, 121, 255});
        }

        for (i32 idx = 0; idx < NUM_DEBUG_FEATURES; idx++) {
            const Debug_Feature_Def *def = &game_debug_feature_defs[idx];
            bool enabled = game_debug_feature_is_enabled(game, def->feature);

            origin.y += 20.0f;
            snprintf(line, sizeof(line), "[%s] %s: %s", def->hotkey, def->name,
                     enabled ? "ON" : "off");
            DrawTextEx(game->font, line, origin, label_size - 1.0f, game->font_spacing,
                       enabled ? (Color){197, 221, 166, 255} : (Color){164, 146, 121, 255});
        }

        origin.y += 24.0f;
        if (!show_spawn_to_exit_path) {
            snprintf(line, sizeof(line), "Player -> Exit path: hidden");
            DrawTextEx(game->font, line, origin, label_size - 1.0f, game->font_spacing,
                       (Color){180, 160, 131, 255});
        } else if (!game->has_exit) {
            snprintf(line, sizeof(line), "Player -> Exit path: no exit placed");
            DrawTextEx(game->font, line, origin, label_size - 1.0f, game->font_spacing,
                       (Color){235, 183, 111, 255});
        } else if (game->spawn_to_exit_path_len > 0) {
            i32 step_count = game->spawn_to_exit_path_len - 1;
            snprintf(line, sizeof(line), "Player -> Exit path: %d steps remaining", step_count);
            DrawTextEx(game->font, line, origin, label_size - 1.0f, game->font_spacing,
                       (Color){187, 214, 173, 255});
        } else {
            snprintf(line, sizeof(line), "Player -> Exit path: no route on this floor");
            DrawTextEx(game->font, line, origin, label_size - 1.0f, game->font_spacing,
                       (Color){235, 183, 111, 255});
        }
    }

    if (game->dungeon_template_error[0] != '\0') {
        origin.y += 22.0f;
        snprintf(line, sizeof(line), "Template warning: %s", game->dungeon_template_error);
        DrawTextEx(game->font, line, origin, label_size - 1.0f, game->font_spacing,
                   (Color){235, 183, 111, 255});
    }

    Rectangle minimap_panel = game_draw_dungeon_minimap(game);
    game_draw_hovered_unit_stats(game, player_panel, minimap_panel);
}

static Rectangle game_end_menu_panel_rect(void)
{
    float screen_w = (float)GetScreenWidth();
    float screen_h = (float)GetScreenHeight();

    float panel_w = clamp(screen_w * 0.74f, 300.0f, 560.0f);
    float panel_h = clamp(screen_h * 0.50f, 220.0f, 320.0f);
    panel_w = min(panel_w, screen_w - 24.0f);
    panel_h = min(panel_h, screen_h - 24.0f);

    return (Rectangle){
        .x = roundf((screen_w - panel_w) * 0.5f),
        .y = roundf((screen_h - panel_h) * 0.5f),
        .width = panel_w,
        .height = panel_h,
    };
}

static Rectangle game_end_menu_restart_button_rect(Rectangle panel)
{
    float button_w = min(panel.width - 48.0f, 240.0f);
    button_w = max(button_w, 140.0f);
    float button_h = clamp(panel.height * 0.20f, 44.0f, 56.0f);

    return (Rectangle){
        .x = panel.x + (panel.width - button_w) * 0.5f,
        .y = panel.y + panel.height - button_h - 24.0f,
        .width = button_w,
        .height = button_h,
    };
}

static bool game_end_menu_restart_requested(Game *game)
{
    Rectangle panel = game_end_menu_panel_rect();
    Rectangle button = game_end_menu_restart_button_rect(panel);
    bool button_hovered = game_point_in_rect(GetMousePosition(), button);
    bool clicked_button = button_hovered && game->input.pressed[INPUT_MOUSE_LEFT];
    return clicked_button || game->input.pressed[INPUT_CONFIRM];
}

static void game_draw_end_menu(Game *game)
{
    if (!game_end_menu_is_active(game))
        return;

    bool is_win_menu = game->end_menu_state == END_MENU_WIN;
    const char *title = is_win_menu ? "You Win" : "You Died";
    const char *subtitle =
        is_win_menu ? "You conquer this run of the dungeon." : "Your warlock falls in the dungeon.";
    const char *button_text = "Restart";
    const char *hint_text = "Click Restart or press Enter";

    Rectangle panel = game_end_menu_panel_rect();
    Rectangle button = game_end_menu_restart_button_rect(panel);
    bool button_hovered = game_point_in_rect(GetMousePosition(), button);

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){5, 4, 3, 184});

    Color outer_fill = is_win_menu ? (Color){18, 24, 17, 246} : (Color){30, 14, 14, 246};
    Color inner_fill = is_win_menu ? (Color){27, 36, 24, 241} : (Color){43, 22, 20, 241};
    Color border = is_win_menu ? (Color){123, 165, 84, 255} : (Color){171, 93, 81, 255};
    game_draw_dungeon_ui_panel(panel, outer_fill, inner_fill, border);

    float title_size = clamp(panel.height * 0.18f, 32.0f, 48.0f);
    float subtitle_size = clamp(panel.height * 0.08f, 16.0f, 22.0f);
    float hint_size = max(subtitle_size - 1.0f, 14.0f);

    Vector2 title_measure = MeasureTextEx(game->font, title, title_size, game->font_spacing);
    Vector2 title_pos = {
        .x = panel.x + (panel.width - title_measure.x) * 0.5f,
        .y = panel.y + 24.0f,
    };
    DrawTextEx(game->font, title, title_pos, title_size, game->font_spacing,
               is_win_menu ? (Color){230, 243, 203, 255} : (Color){248, 210, 205, 255});

    Vector2 subtitle_measure =
        MeasureTextEx(game->font, subtitle, subtitle_size, game->font_spacing);
    Vector2 subtitle_pos = {
        .x = panel.x + (panel.width - subtitle_measure.x) * 0.5f,
        .y = title_pos.y + title_size + 14.0f,
    };
    DrawTextEx(game->font, subtitle, subtitle_pos, subtitle_size, game->font_spacing,
               (Color){214, 195, 168, 255});

    Vector2 hint_measure = MeasureTextEx(game->font, hint_text, hint_size, game->font_spacing);
    Vector2 hint_pos = {
        .x = panel.x + (panel.width - hint_measure.x) * 0.5f,
        .y = button.y - hint_size - 10.0f,
    };
    DrawTextEx(game->font, hint_text, hint_pos, hint_size, game->font_spacing,
               (Color){180, 163, 139, 255});

    Color button_fill =
        is_win_menu ? (button_hovered ? (Color){100, 144, 66, 255} : (Color){78, 116, 53, 255})
                    : (button_hovered ? (Color){156, 76, 64, 255} : (Color){126, 58, 52, 255});
    DrawRectangleRounded(button, 0.20f, 8, button_fill);
    DrawRectangleLinesEx(button, 2.0f,
                         is_win_menu ? (Color){193, 229, 152, 255} : (Color){246, 174, 162, 255});

    float button_text_size = clamp(button.height * 0.46f, 18.0f, 26.0f);
    Vector2 button_text_measure =
        MeasureTextEx(game->font, button_text, button_text_size, game->font_spacing);
    Vector2 button_text_pos = {
        .x = button.x + (button.width - button_text_measure.x) * 0.5f,
        .y = button.y + (button.height - button_text_measure.y) * 0.5f,
    };
    DrawTextEx(game->font, button_text, button_text_pos, button_text_size, game->font_spacing,
               (Color){255, 244, 225, 255});
}

static bool game_start_new_dungeon_run(Game *game)
{
    game->dungeon_seed = game_next_dungeon_seed(game);
    game->dungeon_floor_index = 0;
    game->dungeon_depth = 0;
    game->dungeon_floor_seed_count = 0;
    game->player_stats = game_get_player_base_stats();
    game->show_dungeon_map = true;
    game->dungeon_cam.zoom = DUNGEON_CAMERA_ZOOM_RESET;

    if (!game_build_test_dungeon_for_depth(game, game->dungeon_depth, WORLD_ART_ROLE_FLOOR))
        return false;

    game_center_dungeon_camera_on_player(game);
    game->end_menu_state = END_MENU_NONE;
    return true;
}

static bool game_update_end_menu(Game *game)
{
    if (!game_end_menu_is_active(game))
        return false;

    if (game_end_menu_restart_requested(game))
        game_start_new_dungeon_run(game);

    if (game->input.pressed[INPUT_BACK])
        app_quit();

    return true;
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
        .offset = {(float)VIRTUAL_W * 0.5f, (float)VIRTUAL_H * 0.5f},
        .target = {0.0f, 0.0f},
        .zoom = 1.0f,
    };
    game->preview_cam = (Camera2D){
        .offset = {0.0f, 0.0f},
        .target = {0.0f, 0.0f},
        .zoom = 1.0f,
    };

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
    game->end_menu_state = END_MENU_NONE;
    game_debug_reset_feature_defaults(game);
    game->dungeon_wall_theme = WORLD_ART_THEME_1;
    game->dungeon_seed = DUNGEON_SEED;
    game->dungeon_template_index = 0;
    game->dungeon_cam.zoom = DUNGEON_CAMERA_ZOOM_RESET;
    game_seed_run_seed_rng(game);
    game_dungeon_load_template(game, game->dungeon_template_index);
    game_start_new_dungeon_run(game);
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
    input->down[INPUT_DEBUG_NEXT_FLOOR] =
        IsKeyDown(KEY_SPACE) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
    input->down[INPUT_CONFIRM] = IsKeyDown(KEY_ENTER) || IsKeyDown(KEY_KP_ENTER) ||
                                 IsKeyDown(KEY_SPACE) ||
                                 IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
    input->down[INPUT_MOUSE_LEFT] = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    input->down[INPUT_MOUSE_RIGHT] = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
    input->down[INPUT_MOUSE_MIDDLE] = IsMouseButtonDown(MOUSE_BUTTON_MIDDLE);

    input->pressed[INPUT_DEBUG_NEXT_FLOOR] =
        IsKeyPressed(KEY_SPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
    input->pressed[INPUT_CONFIRM] = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER) ||
                                    IsKeyPressed(KEY_SPACE) ||
                                    IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
    input->pressed[INPUT_MOVE_UP] = IsKeyPressed(KEY_W) || IsKeyPressedRepeat(KEY_W) ||
                                    IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP);
    input->pressed[INPUT_MOVE_DOWN] = IsKeyPressed(KEY_S) || IsKeyPressedRepeat(KEY_S) ||
                                      IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN);
    input->pressed[INPUT_MOVE_LEFT] = IsKeyPressed(KEY_A) || IsKeyPressedRepeat(KEY_A) ||
                                      IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT);
    input->pressed[INPUT_MOVE_RIGHT] = IsKeyPressed(KEY_D) || IsKeyPressedRepeat(KEY_D) ||
                                       IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT);
    input->pressed[INPUT_MOUSE_LEFT] = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    input->pressed[INPUT_MOUSE_RIGHT] = IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
    input->pressed[INPUT_MOUSE_MIDDLE] = IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE);
    input->pressed[INPUT_BACK] = IsKeyPressed(KEY_ESCAPE);
    input->pressed[INPUT_DEBUG_WIN_MENU] = IsKeyPressed(KEY_V);
    input->pressed[INPUT_DEBUG_TOGGLE_VIEW] = IsKeyPressed(KEY_TAB);
    input->pressed[INPUT_DEBUG_TOGGLE_PATH] = IsKeyPressed(KEY_P);
    input->pressed[INPUT_DEBUG_TOGGLE_REVEAL_MAP] = IsKeyPressed(KEY_L);
    input->pressed[INPUT_DEBUG_TOGGLE_DIJKSTRA_OVERLAY] = IsKeyPressed(KEY_J);
    input->pressed[INPUT_SELECT_WALL_THEME_1] = IsKeyPressed(KEY_ONE);
    input->pressed[INPUT_SELECT_WALL_THEME_2] = IsKeyPressed(KEY_TWO);
    input->pressed[INPUT_SELECT_WALL_THEME_3] = IsKeyPressed(KEY_THREE);
    input->pressed[INPUT_SELECT_WALL_THEME_4] = IsKeyPressed(KEY_FOUR);
    input->pressed[INPUT_SELECT_TEMPLATE_PREV] = IsKeyPressed(KEY_Q);
    input->pressed[INPUT_SELECT_TEMPLATE_NEXT] = IsKeyPressed(KEY_E);
    input->pressed[INPUT_ZOOM_IN] = IsKeyPressed(KEY_EQUAL);
    input->pressed[INPUT_ZOOM_OUT] = IsKeyPressed(KEY_MINUS);
    input->pressed[INPUT_ZOOM_RESET] = IsKeyPressed(KEY_ZERO);

    input->released[INPUT_MOUSE_LEFT] = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
    input->released[INPUT_MOUSE_RIGHT] = IsMouseButtonReleased(MOUSE_BUTTON_RIGHT);
    input->released[INPUT_MOUSE_MIDDLE] = IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE);
}

void game_update(Mem mem)
{
    Game *game = arena_start(mem.perm, Game);
    arena_clear(mem.tmp);

    game_input(game);

    if ((i32)game->player_stats.health <= 0 && !game_end_menu_is_active(game))
        game_open_end_menu(game, END_MENU_DEATH);

    if (game_update_end_menu(game))
        return;

    bool rebuild_floor = false;
    game_debug_handle_actions(game, &rebuild_floor);
    game_debug_handle_feature_toggles(game);

    if (game_update_end_menu(game))
        return;

    if (game->input.pressed[INPUT_SELECT_WALL_THEME_1])
        game->dungeon_wall_theme = WORLD_ART_THEME_1;
    if (game->input.pressed[INPUT_SELECT_WALL_THEME_2])
        game->dungeon_wall_theme = WORLD_ART_THEME_2;
    if (game->input.pressed[INPUT_SELECT_WALL_THEME_3])
        game->dungeon_wall_theme = WORLD_ART_THEME_3;
    if (game->input.pressed[INPUT_SELECT_WALL_THEME_4])
        game->dungeon_wall_theme = WORLD_ART_THEME_4;

    if (GAME_DEBUG_FEATURES) {
        i32 template_delta = 0;
        if (game->input.pressed[INPUT_SELECT_TEMPLATE_PREV])
            template_delta -= 1;
        if (game->input.pressed[INPUT_SELECT_TEMPLATE_NEXT])
            template_delta += 1;

        if (template_delta != 0) {
            i32 next_template =
                (game->dungeon_template_index + template_delta + DUNGEON_HBW_TEMPLATE_COUNT) %
                DUNGEON_HBW_TEMPLATE_COUNT;
            if (game_dungeon_load_template(game, next_template)) {
                game->dungeon_floor_index = 0;
                game->dungeon_depth = 0;
                game->dungeon_floor_seed_count = 0;
                rebuild_floor = true;
            }
        }
    }

    if (game->show_dungeon_map) {
        if (GAME_DEBUG_FEATURES) {
            if (game->input.pressed[INPUT_ZOOM_IN])
                game_adjust_dungeon_zoom(game, DUNGEON_CAMERA_ZOOM_STEP);
            if (game->input.pressed[INPUT_ZOOM_OUT])
                game_adjust_dungeon_zoom(game, -DUNGEON_CAMERA_ZOOM_STEP);
            if (game->input.pressed[INPUT_ZOOM_RESET])
                game->dungeon_cam.zoom = DUNGEON_CAMERA_ZOOM_RESET;

            float wheel = GetMouseWheelMove();
            if (wheel != 0.0f)
                game_adjust_dungeon_zoom(game, wheel * DUNGEON_CAMERA_ZOOM_STEP);
        }
    } else {
        float frame_time = GetFrameTime();
        if (frame_time <= 0.0f)
            frame_time = CAMERA_FALLBACK_FRAME_TIME;

        float zoom = max(game->preview_cam.zoom, 0.001f);
        float pan_delta = PREVIEW_CAMERA_PAN_SPEED * frame_time / zoom;
        if (game->input.down[INPUT_MOVE_LEFT])
            game->preview_cam.target.x -= pan_delta;
        if (game->input.down[INPUT_MOVE_RIGHT])
            game->preview_cam.target.x += pan_delta;
        if (game->input.down[INPUT_MOVE_UP])
            game->preview_cam.target.y -= pan_delta;
        if (game->input.down[INPUT_MOVE_DOWN])
            game->preview_cam.target.y += pan_delta;

        if (game->input.down[INPUT_MOUSE_MIDDLE]) {
            Vector2 drag = GetMouseDelta();
            game->preview_cam.target.x -= drag.x / zoom;
            game->preview_cam.target.y -= drag.y / zoom;
        }
    }

    if (rebuild_floor) {
        if (game_build_test_dungeon_for_depth(game, game->dungeon_depth, WORLD_ART_ROLE_FLOOR))
            game_center_dungeon_camera_on_player(game);
    }

    bool player_took_turn = false;
    if (game->show_dungeon_map)
        player_took_turn = game_update_player(game);

    if (player_took_turn)
        game_dungeon_take_enemy_turns(game);

    if ((i32)game->player_stats.health <= 0) {
        game_open_end_menu(game, END_MENU_DEATH);
        return;
    }

    game_update_camera(game);

    if (game->input.pressed[INPUT_BACK])
        app_quit();
}

void game_render(Mem mem)
{
    Game *game = arena_start(mem.perm, Game);
    Camera2D active_cam = game_get_active_camera(game);

    ClearBackground((Color){11, 8, 7, 255});
    BeginMode2D(active_cam);
    if (game->show_dungeon_map)
        game_draw_test_dungeon(game);
    else
        game_draw_art_previews(game);
    EndMode2D();

    if (game->show_dungeon_map && !game_end_menu_is_active(game))
        game_draw_dungeon_hud(game);

    if (game_end_menu_is_active(game))
        game_draw_end_menu(game);
}
