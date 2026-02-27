#include <string.h>

#include "microui_raylib.h"

static Microui_Raylib *microui_active = NULL;

static Color microui_color(mu_Color color)
{
    return (Color){color.r, color.g, color.b, color.a};
}

static int microui_text_width(mu_Font font, const char *str, int len)
{
    (void)font;
    if (!microui_active || !str)
        return 0;

    if (len < 0)
        return (int)MeasureTextEx(microui_active->font, str, (float)microui_active->font_size,
                                  microui_active->font_spacing)
            .x;

    const char *sub = TextSubtext(str, 0, len);
    return (int)MeasureTextEx(microui_active->font, sub, (float)microui_active->font_size,
                              microui_active->font_spacing)
        .x;
}

static int microui_text_height(mu_Font font)
{
    (void)font;
    if (!microui_active)
        return 0;

    return microui_active->font_size;
}

static Rectangle microui_scale_rect(Microui_Raylib *ui, mu_Rect rect)
{
    float x = ui->offset_x + rect.x * ui->scale;
    float y = ui->offset_y + rect.y * ui->scale;
    float w = rect.w * ui->scale;
    float h = rect.h * ui->scale;
    return (Rectangle){x, y, w, h};
}

static void microui_draw_icon(Microui_Raylib *ui, int id, mu_Rect rect, mu_Color color)
{
    Color col = microui_color(color);
    Rectangle dst = microui_scale_rect(ui, rect);
    float thickness = 2.0f * ui->scale;

    switch (id) {
    case MU_ICON_CLOSE: {
        Vector2 a = {dst.x + dst.width * 0.25f, dst.y + dst.height * 0.25f};
        Vector2 b = {dst.x + dst.width * 0.75f, dst.y + dst.height * 0.75f};
        Vector2 c = {dst.x + dst.width * 0.75f, dst.y + dst.height * 0.25f};
        Vector2 d = {dst.x + dst.width * 0.25f, dst.y + dst.height * 0.75f};
        DrawLineEx(a, b, thickness, col);
        DrawLineEx(c, d, thickness, col);
    } break;
    case MU_ICON_CHECK: {
        Vector2 a = {dst.x + dst.width * 0.2f, dst.y + dst.height * 0.55f};
        Vector2 b = {dst.x + dst.width * 0.45f, dst.y + dst.height * 0.75f};
        Vector2 c = {dst.x + dst.width * 0.8f, dst.y + dst.height * 0.3f};
        DrawLineEx(a, b, thickness, col);
        DrawLineEx(b, c, thickness, col);
    } break;
    case MU_ICON_COLLAPSED: {
        Vector2 a = {dst.x + dst.width * 0.35f, dst.y + dst.height * 0.25f};
        Vector2 b = {dst.x + dst.width * 0.35f, dst.y + dst.height * 0.75f};
        Vector2 c = {dst.x + dst.width * 0.7f, dst.y + dst.height * 0.5f};
        DrawTriangle(a, b, c, col);
    } break;
    case MU_ICON_EXPANDED: {
        Vector2 a = {dst.x + dst.width * 0.25f, dst.y + dst.height * 0.35f};
        Vector2 b = {dst.x + dst.width * 0.75f, dst.y + dst.height * 0.35f};
        Vector2 c = {dst.x + dst.width * 0.5f, dst.y + dst.height * 0.7f};
        DrawTriangle(a, b, c, col);
    } break;
    default:
        break;
    }
}

void microui_raylib_init(Microui_Raylib *ui, mu_Context *ctx)
{
    memset(ui, 0, sizeof(*ui));
    ui->font = GetFontDefault();
    ui->font_size = 10;
    ui->font_spacing = 1.0f;
    ui->scale = 1.0f;
    ui->offset_x = 0.0f;
    ui->offset_y = 0.0f;

    microui_active = ui;
    ctx->text_width = microui_text_width;
    ctx->text_height = microui_text_height;
    ctx->style->font = &ui->font;
    ctx->style->size.y = ui->font_size;
    ctx->style->title_height = ui->font_size + 8;
}

void microui_raylib_set_viewport(Microui_Raylib *ui, float scale, float offset_x, float offset_y)
{
    ui->scale = scale;
    ui->offset_x = offset_x;
    ui->offset_y = offset_y;
}

void microui_raylib_draw(Microui_Raylib *ui, mu_Context *ctx)
{
    microui_active = ui;
    mu_Command *cmd = NULL;
    bool scissor_active = false;

    while (mu_next_command(ctx, &cmd)) {
        switch (cmd->type) {
        case MU_COMMAND_TEXT: {
            Color color = microui_color(cmd->text.color);
            Vector2 pos = {ui->offset_x + cmd->text.pos.x * ui->scale,
                           ui->offset_y + cmd->text.pos.y * ui->scale};
            pos.x = (float)((int)(pos.x + 0.5f));
            pos.y = (float)((int)(pos.y + 0.5f));
            float size = ui->font_size * ui->scale;
            float spacing = ui->font_spacing * ui->scale;
            DrawTextEx(ui->font, cmd->text.str, pos, size, spacing, color);
        } break;
        case MU_COMMAND_RECT: {
            Color color = microui_color(cmd->rect.color);
            Rectangle rect = microui_scale_rect(ui, cmd->rect.rect);
            DrawRectangleRec(rect, color);
        } break;
        case MU_COMMAND_ICON:
            microui_draw_icon(ui, cmd->icon.id, cmd->icon.rect, cmd->icon.color);
            break;
        case MU_COMMAND_CLIP: {
            Rectangle rect = microui_scale_rect(ui, cmd->clip.rect);
            if (scissor_active)
                EndScissorMode();
            BeginScissorMode((int)rect.x, (int)rect.y, (int)rect.width, (int)rect.height);
            scissor_active = true;
        } break;
        default:
            break;
        }
    }

    if (scissor_active)
        EndScissorMode();
}
