#include <math.h>

#include "app_config.h"
#include "game.h"
#include "platform.h"

#define GAME_FONT_PATH "assets/PixelSerif_16px_v02.ttf"
#define GAME_FONT_SIZE 160
#define GAME_FONT_SPACING 0.0f

typedef struct {
    bool (*init)(void *user);
    void (*iterate)(void *user);
    void (*shutdown)(void *user);
    void *user;
    bool should_quit;
} App;
App app = {0};

void app_quit()
{
    app.should_quit = true;
}

typedef struct {
    Mem game_mem;
    RenderTexture2D framebuffer;
    Font font;
    double last_time;
    double accumulator;
} Game_App;

static bool game_app_init(void *user)
{
    Game_App *ctx = user;

    SetTraceLogLevel(0);
#if defined(PLATFORM_WEB)
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
#else
    SetConfigFlags(FLAG_WINDOW_HIGHDPI | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
#endif
    InitWindow(VIRTUAL_W, VIRTUAL_H, "Game");
#if !defined(NDEBUG)
    SetExitKey(KEY_Q);
#endif
#if !defined(PLATFORM_WEB)
    MaximizeWindow();
#endif

    ctx->framebuffer = LoadRenderTexture(VIRTUAL_W, VIRTUAL_H);
    SetTextureFilter(ctx->framebuffer.texture, TEXTURE_FILTER_BILINEAR);

    ctx->game_mem = (Mem){
        .tmp = arena_alloc(MB(16)),
        .perm = arena_alloc(MB(16)),
    };

    ctx->font = LoadFontEx(GAME_FONT_PATH, GAME_FONT_SIZE, 0, 0);
    SetTextureFilter(ctx->font.texture, TEXTURE_FILTER_BILINEAR);

    // set render target since game_init cares about render size
    BeginTextureMode(ctx->framebuffer);
    game_init(ctx->game_mem, ctx->font, GAME_FONT_SPACING);
    EndTextureMode();

    ctx->accumulator = 0.0;
    ctx->last_time = GetTime();

    return true;
}

static void game_app_iterate(void *user)
{
    Game_App *ctx = user;

    double current_time = GetTime();
    double dt = current_time - ctx->last_time;
    ctx->last_time = current_time;

    // lock step timing
    double target_frame_time = 1.0 / 60.0;
    if (fabs(dt - 1.0 / 120.0) < 0.0002)
        dt = 1.0 / 120.0;
    if (fabs(dt - 1.0 / 60.0) < 0.0002)
        dt = 1.0 / 60.0;
    if (fabs(dt - 1.0 / 30.0) < 0.0002)
        dt = 1.0 / 30.0;
    if (dt > target_frame_time)
        dt = target_frame_time;

    ctx->accumulator += dt;
    if (ctx->accumulator < target_frame_time)
        return;

    while (ctx->accumulator >= target_frame_time) {
#if defined(PLATFORM_WEB)
        game_update(ctx->game_mem);
        PollInputEvents();
#else
        PollInputEvents();
        game_update(ctx->game_mem);
#endif
        ctx->accumulator -= target_frame_time;
    }

    // preserve aspect ratio
    int screen_w = GetScreenWidth();
    int screen_h = GetScreenHeight();

    float scale_x = (float)screen_w / VIRTUAL_W;
    float scale_y = (float)screen_h / VIRTUAL_H;
    float scale = scale_x < scale_y ? scale_x : scale_y;

    float scaled_w = VIRTUAL_W * scale;
    float scaled_h = VIRTUAL_H * scale;
    float draw_x = ((float)screen_w - scaled_w) / 2.0f;
    float draw_y = ((float)screen_h - scaled_h) / 2.0f;

    Rectangle src_rect = {0, 0, (float)VIRTUAL_W, -(float)VIRTUAL_H};
    Rectangle dst_rect = {draw_x, draw_y, scaled_w, scaled_h};

    BeginDrawing();
    ClearBackground(PINK);
    game_render(ctx->game_mem);
    EndDrawing();
    SwapScreenBuffer();
}

static void game_app_shutdown(void *user)
{
    Game_App *ctx = user;

    game_shutdown(ctx->game_mem);
    UnloadRenderTexture(ctx->framebuffer);
    UnloadFont(ctx->font);
    CloseWindow();
}

static int app_run(App *app)
{
    if (!app->init(app->user))
        return 1;

    while (!app->should_quit && !WindowShouldClose()) {
        app->iterate(app->user);
    }

    app->shutdown(app->user);
    return 0;
}

int main(void)
{
    Game_App game_app = {0};

    app.init = game_app_init;
    app.iterate = game_app_iterate;
    app.shutdown = game_app_shutdown;
    app.user = &game_app;

    return app_run(&app);
}
