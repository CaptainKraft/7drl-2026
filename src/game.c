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
#define UNIT_PREVIEW_ANIM_FPS 5.0f

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

typedef enum {
    INPUT_NONE,
    INPUT_MOVE_UP,
    INPUT_MOVE_DOWN,
    INPUT_MOVE_LEFT,
    INPUT_MOVE_RIGHT,
    INPUT_ACTION,
    INPUT_BACK,
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

typedef struct {
    Input input;
    Camera2D cam;
    Font font;
    float font_spacing;
    Texture2D units_texture;
    Texture2D items_texture;
    Texture2D world_texture;
} Game;

static int game_preview_anim_frame(void)
{
    int frame = (int)(GetTime() * UNIT_PREVIEW_ANIM_FPS);
    frame %= UNIT_ART_ANIM_FRAME_COUNT;
    if (frame < 0)
        frame += UNIT_ART_ANIM_FRAME_COUNT;
    return frame;
}

static int game_item_preview_anim_frame(void)
{
    int frame = (int)(GetTime() * UNIT_PREVIEW_ANIM_FPS);
    frame %= ITEM_ART_ANIM_FRAME_COUNT;
    if (frame < 0)
        frame += ITEM_ART_ANIM_FRAME_COUNT;
    return frame;
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

    int anim_frame = game_preview_anim_frame();
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
            Unit_Art_Pixel pixel =
                unit_art_get_pixel((UNIT_ART_KIND)kind, (u8)anim_frame, (u8)orientation);
            Rectangle src = {
                .x = (float)pixel.x,
                .y = (float)pixel.y,
                .width = (float)UNIT_ART_TILE_SIZE,
                .height = (float)UNIT_ART_TILE_SIZE,
            };

            Rectangle dst = {
                .x = sprite_row_x + (float)orientation * (sprite_size + UNIT_PREVIEW_SPRITE_GAP),
                .y = sprite_y,
                .width = sprite_size,
                .height = sprite_size,
            };

            DrawTexturePro(game->units_texture, src, dst, (Vector2){0}, 0.0f, WHITE);
        }
    }
}

static void game_draw_item_art_preview(Game *game, Vector2 origin)
{
    float sprite_size = ITEM_ART_TILE_SIZE * ITEM_PREVIEW_SPRITE_SCALE;
    int anim_frame = game_item_preview_anim_frame();

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

        ITEM_ART_KIND draw_kind = (ITEM_ART_KIND)kind;
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
            .x = sprite_x,
            .y = sprite_y,
            .width = sprite_size,
            .height = sprite_size,
        };

        DrawTexturePro(game->items_texture, src, dst, (Vector2){0}, 0.0f, WHITE);
    }
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

                float rotation =
                    world_art_get_wall_rotation_degrees((WORLD_WALL_DIRECTION)direction);
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

static void game_update_camera(Game *game)
{
    float frame_time = GetFrameTime();
    if (frame_time <= 0.0f)
        frame_time = CAMERA_FALLBACK_FRAME_TIME;

    float camera_step = CAMERA_KEYBOARD_PAN_SPEED * frame_time;

    if (game->input.down[INPUT_MOVE_UP] || IsKeyDown(KEY_W))
        game->cam.target.y -= camera_step;
    if (game->input.down[INPUT_MOVE_DOWN] || IsKeyDown(KEY_S))
        game->cam.target.y += camera_step;
    if (game->input.down[INPUT_MOVE_LEFT] || IsKeyDown(KEY_A))
        game->cam.target.x -= camera_step;
    if (game->input.down[INPUT_MOVE_RIGHT] || IsKeyDown(KEY_D))
        game->cam.target.x += camera_step;

    if (game->input.down[INPUT_MOUSE_MIDDLE]) {
        Vector2 mouse_delta = GetMouseDelta();
        game->cam.target.x -= mouse_delta.x / game->cam.zoom;
        game->cam.target.y -= mouse_delta.y / game->cam.zoom;
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

    game->cam = (Camera2D){
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

    // held input
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

    // pressed input
    input->pressed[INPUT_ACTION] =
        IsKeyPressed(KEY_SPACE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
    input->pressed[INPUT_MOUSE_LEFT] = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    input->pressed[INPUT_MOUSE_RIGHT] = IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
    input->pressed[INPUT_MOUSE_MIDDLE] = IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE);
    input->pressed[INPUT_BACK] = IsKeyPressed(KEY_ESCAPE);

    // released inputs
    input->released[INPUT_MOUSE_LEFT] = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
    input->released[INPUT_MOUSE_RIGHT] = IsMouseButtonReleased(MOUSE_BUTTON_RIGHT);
    input->released[INPUT_MOUSE_MIDDLE] = IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE);
}

void game_update(Mem mem)
{
    Game *game = arena_start(mem.perm, Game);
    arena_clear(mem.tmp);

    game_input(game);
    game_update_camera(game);
    if (game->input.pressed[INPUT_BACK])
        app_quit();
}

void game_render(Mem mem)
{
    Game *game = arena_start(mem.perm, Game);
    ClearBackground((Color){14, 25, 30, 255});
    BeginMode2D(game->cam);
    game_draw_art_previews(game);
    EndMode2D();
}
