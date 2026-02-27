#include <assert.h>
#include <string.h>

#include "app_config.h"
#include "ck/ck.h"

#include "game.h"
#include "unit_art.h"

#define UNITS_SHEET_PATH "assets/units.png"
#define UNIT_PREVIEW_MARGIN_X 24.0f
#define UNIT_PREVIEW_MARGIN_Y 24.0f
#define UNIT_PREVIEW_CELL_GAP_X 14.0f
#define UNIT_PREVIEW_CELL_GAP_Y 18.0f
#define UNIT_PREVIEW_CELL_PADDING_X 10.0f
#define UNIT_PREVIEW_CELL_PADDING_Y 8.0f
#define UNIT_PREVIEW_LABEL_SIZE 16.0f
#define UNIT_PREVIEW_LABEL_GAP 6.0f
#define UNIT_PREVIEW_SPRITE_SCALE 2.0f
#define UNIT_PREVIEW_SPRITE_GAP 6.0f
#define UNIT_PREVIEW_ANIM_FPS 5.0f

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
    Vector2 mouse_pos;
    float mouse_wheel;
} Input;

typedef struct {
    Input input;
    Camera2D cam;
    Font font;
    float font_spacing;
    Texture2D units_texture;
} Game;

static int game_preview_anim_frame(void)
{
    int frame = (int)(GetTime() * UNIT_PREVIEW_ANIM_FPS);
    frame %= UNIT_ART_ANIM_FRAME_COUNT;
    if (frame < 0)
        frame += UNIT_ART_ANIM_FRAME_COUNT;
    return frame;
}

static void game_draw_unit_art_preview(Game *game)
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

    i32 unit_count = NUM_UNIT_ART - 1;
    float min_cell_width =
        max(max_name_width, sprite_row_width) + (UNIT_PREVIEW_CELL_PADDING_X * 2.0f);
    float usable_w = (float)VIRTUAL_W - (UNIT_PREVIEW_MARGIN_X * 2.0f);
    i32 grid_cols =
        (i32)((usable_w + UNIT_PREVIEW_CELL_GAP_X) / (min_cell_width + UNIT_PREVIEW_CELL_GAP_X));
    grid_cols = clamp(grid_cols, 1, unit_count);

    i32 grid_rows = (unit_count + grid_cols - 1) / grid_cols;
    float total_gap_w = (float)(grid_cols - 1) * UNIT_PREVIEW_CELL_GAP_X;
    float total_gap_h = (float)(grid_rows - 1) * UNIT_PREVIEW_CELL_GAP_Y;

    float cell_w = (usable_w - total_gap_w) / (float)grid_cols;
    float cell_h =
        ((float)VIRTUAL_H - (UNIT_PREVIEW_MARGIN_Y * 2.0f) - total_gap_h) / (float)grid_rows;
    int anim_frame = game_preview_anim_frame();

    for (i32 kind = UNIT_ART_NONE + 1; kind < NUM_UNIT_ART; kind++) {
        i32 draw_idx = kind - 1;
        i32 col = draw_idx % grid_cols;
        i32 row = draw_idx / grid_cols;

        float cell_x = UNIT_PREVIEW_MARGIN_X + (float)col * (cell_w + UNIT_PREVIEW_CELL_GAP_X);
        float cell_y = UNIT_PREVIEW_MARGIN_Y + (float)row * (cell_h + UNIT_PREVIEW_CELL_GAP_Y);

        Rectangle panel = {cell_x, cell_y, cell_w, cell_h};
        DrawRectangleRounded(panel, 0.08f, 4, (Color){32, 47, 52, 255});
        DrawRectangleLinesEx(panel, 1.0f, (Color){71, 95, 104, 255});

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

void game_init(Mem mem, Font font, float font_spacing)
{
    Game *game = arena_push(mem.perm, sizeof(Game));
    memset(game, 0, sizeof(*game));

    if (font.texture.id == 0)
        font = GetFontDefault();
    game->font = font;
    game->font_spacing = font_spacing;

    int screen_w = VIRTUAL_W, screen_h = VIRTUAL_H;
    game->cam = (Camera2D){
        .offset = {screen_w / 2, screen_h / 2},
        .target = {0.0f, 0.0f},
        .zoom = 1.0f,
    };

    game->units_texture = LoadTexture(UNITS_SHEET_PATH);
    assert(game->units_texture.id != 0);
    SetTextureFilter(game->units_texture, TEXTURE_FILTER_POINT);
}

void game_shutdown(Mem mem)
{
    Game *game = arena_start(mem.perm, Game);
    if (game->units_texture.id != 0)
        UnloadTexture(game->units_texture);
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
    if (game->input.pressed[INPUT_BACK])
        app_quit();
}

void game_render(Mem mem)
{
    Game *game = arena_start(mem.perm, Game);
    ClearBackground((Color){14, 25, 30, 255});
    game_draw_unit_art_preview(game);
}
