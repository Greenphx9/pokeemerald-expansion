#include "global.h"
#include "text.h"
#include "text_window.h"
#include "window.h"
#include "palette.h"
#include "bg.h"
#include "graphics.h"
#include "menu.h"

const u8 gUserFrame_Type1_Gfx[] = INCBIN_U8("graphics/text_window/type1.4bpp");
static const u8 sUserFrame_Type2_Gfx[] = INCBIN_U8("graphics/text_window/type2.4bpp");
static const u8 sUserFrame_Type3_Gfx[] = INCBIN_U8("graphics/text_window/type3.4bpp");
static const u8 sUserFrame_Type4_Gfx[] = INCBIN_U8("graphics/text_window/type4.4bpp");
static const u8 sUserFrame_Type5_Gfx[] = INCBIN_U8("graphics/text_window/type5.4bpp");
static const u8 sUserFrame_Type6_Gfx[] = INCBIN_U8("graphics/text_window/type6.4bpp");
static const u8 sUserFrame_Type7_Gfx[] = INCBIN_U8("graphics/text_window/type7.4bpp");
static const u8 sUserFrame_Type8_Gfx[] = INCBIN_U8("graphics/text_window/type8.4bpp");
static const u8 sUserFrame_Type9_Gfx[] = INCBIN_U8("graphics/text_window/type9.4bpp");
static const u8 sUserFrame_Type10_Gfx[] = INCBIN_U8("graphics/text_window/type10.4bpp");

const u16 gUserFrame_Type1_Pal[] = INCBIN_U16("graphics/text_window/type1.gbapal");
static const u16 sUserFrame_Type2_Pal[] = INCBIN_U16("graphics/text_window/type2.gbapal");
static const u16 sUserFrame_Type3_Pal[] = INCBIN_U16("graphics/text_window/type3.gbapal");
static const u16 sUserFrame_Type4_Pal[] = INCBIN_U16("graphics/text_window/type4.gbapal");
static const u16 sUserFrame_Type5_Pal[] = INCBIN_U16("graphics/text_window/type5.gbapal");
static const u16 sUserFrame_Type6_Pal[] = INCBIN_U16("graphics/text_window/type6.gbapal");
static const u16 sUserFrame_Type7_Pal[] = INCBIN_U16("graphics/text_window/type7.gbapal");
static const u16 sUserFrame_Type8_Pal[] = INCBIN_U16("graphics/text_window/type8.gbapal");
static const u16 sUserFrame_Type9_Pal[] = INCBIN_U16("graphics/text_window/type9.gbapal");
static const u16 sUserFrame_Type10_Pal[] = INCBIN_U16("graphics/text_window/type10.gbapal");

static const u16 sStdTextWindow_Gfx[] = INCBIN_U16("graphics/text_window/std.4bpp");

static const u16 sTextWindowPalettes[][16] =
{
    INCBIN_U16("graphics/text_window/stdpal_0.gbapal"),
    INCBIN_U16("graphics/text_window/stdpal_1.gbapal"),
    INCBIN_U16("graphics/text_window/stdpal_2.gbapal"),
    INCBIN_U16("graphics/text_window/stdpal_3.gbapal"),
    INCBIN_U16("graphics/text_window/stdpal_4.gbapal")
};

static const struct TilesPal sWindowFrames[WINDOW_FRAMES_COUNT] =
{
    {gUserFrame_Type1_Gfx,  gUserFrame_Type1_Pal},
    {sUserFrame_Type2_Gfx,  sUserFrame_Type2_Pal},
    {sUserFrame_Type3_Gfx,  sUserFrame_Type3_Pal},
    {sUserFrame_Type4_Gfx,  sUserFrame_Type4_Pal},
    {sUserFrame_Type5_Gfx,  sUserFrame_Type5_Pal},
    {sUserFrame_Type6_Gfx,  sUserFrame_Type6_Pal},
    {sUserFrame_Type7_Gfx,  sUserFrame_Type7_Pal},
    {sUserFrame_Type8_Gfx,  sUserFrame_Type8_Pal},
    {sUserFrame_Type9_Gfx,  sUserFrame_Type9_Pal},
    {sUserFrame_Type10_Gfx, sUserFrame_Type10_Pal},
};

// code
const struct TilesPal *GetWindowFrameTilesPal(u8 id)
{
    if (id >= WINDOW_FRAMES_COUNT)
        return &sWindowFrames[0];
    else
        return &sWindowFrames[id];
}

void LoadMessageBoxGfx(u8 windowId, u16 destOffset, u8 palOffset)
{
    LoadBgTiles(GetWindowAttribute(windowId, WINDOW_BG), gMessageBox_Gfx, 0x280, destOffset);
    LoadPalette(GetTextWindowPalette(0), palOffset, PLTT_SIZE_4BPP);
}

void LoadSignBoxGfx(u8 windowId, u16 destOffset, u8 palOffset)
{
    LoadBgTiles(GetWindowAttribute(windowId, WINDOW_BG), gSignpostWindow_Gfx, 0x260, destOffset);
    LoadPalette(GetTextWindowPalette(1), palOffset, PLTT_SIZE_4BPP);
}

void LoadUserWindowBorderGfx_(u8 windowId, u16 destOffset, u8 palOffset)
{
    LoadUserWindowBorderGfx(windowId, destOffset, palOffset);
}

void LoadWindowGfx(u8 windowId, u8 frameId, u16 destOffset, u8 palOffset)
{
    LoadBgTiles(GetWindowAttribute(windowId, WINDOW_BG), sWindowFrames[frameId].tiles, 0x120, destOffset);
    LoadPalette(sWindowFrames[frameId].pal, palOffset, PLTT_SIZE_4BPP);
}

void LoadUserWindowBorderGfx(u8 windowId, u16 destOffset, u8 palOffset)
{
    LoadWindowGfx(windowId, gSaveBlock2Ptr->optionsWindowFrameType, destOffset, palOffset);
}

void LoadStdWindowGfx(u8 windowId, u16 destOffset, u8 palOffset)
{
    LoadBgTiles(GetWindowAttribute(windowId, WINDOW_BG), sStdTextWindow_Gfx, 0x120, destOffset);
    LoadPalette(GetTextWindowPalette(3), palOffset, PLTT_SIZE_4BPP);
}

void LoadStdWindowTiles(u8 windowId, u16 destOffset)
{
    LoadBgTiles(GetWindowAttribute(windowId, WINDOW_BG), sStdTextWindow_Gfx, 0x120, destOffset);
}

void DrawTextBorderOuter(u8 windowId, u16 tileNum, u8 palNum)
{
    u8 bgLayer = GetWindowAttribute(windowId, WINDOW_BG);
    u16 tilemapLeft = GetWindowAttribute(windowId, WINDOW_TILEMAP_LEFT);
    u16 tilemapTop = GetWindowAttribute(windowId, WINDOW_TILEMAP_TOP);
    u16 width = GetWindowAttribute(windowId, WINDOW_WIDTH);
    u16 height = GetWindowAttribute(windowId, WINDOW_HEIGHT);

    FillBgTilemapBufferRect(bgLayer, tileNum + 0, tilemapLeft - 1,      tilemapTop - 1,         1,      1,      palNum);
    FillBgTilemapBufferRect(bgLayer, tileNum + 1, tilemapLeft,          tilemapTop - 1,         width,  1,      palNum);
    FillBgTilemapBufferRect(bgLayer, tileNum + 2, tilemapLeft + width,  tilemapTop - 1,         1,      1,      palNum);
    FillBgTilemapBufferRect(bgLayer, tileNum + 3, tilemapLeft - 1,      tilemapTop,             1,      height, palNum);
    FillBgTilemapBufferRect(bgLayer, tileNum + 5, tilemapLeft + width,  tilemapTop,             1,      height, palNum);
    FillBgTilemapBufferRect(bgLayer, tileNum + 6, tilemapLeft - 1,      tilemapTop + height,    1,      1,      palNum);
    FillBgTilemapBufferRect(bgLayer, tileNum + 7, tilemapLeft,          tilemapTop + height,    width,  1,      palNum);
    FillBgTilemapBufferRect(bgLayer, tileNum + 8, tilemapLeft + width,  tilemapTop + height,    1,      1,      palNum);
}

void DrawTextBorderInner(u8 windowId, u16 tileNum, u8 palNum)
{
    u8 bgLayer = GetWindowAttribute(windowId, WINDOW_BG);
    u16 tilemapLeft = GetWindowAttribute(windowId, WINDOW_TILEMAP_LEFT);
    u16 tilemapTop = GetWindowAttribute(windowId, WINDOW_TILEMAP_TOP);
    u16 width = GetWindowAttribute(windowId, WINDOW_WIDTH);
    u16 height = GetWindowAttribute(windowId, WINDOW_HEIGHT);

    FillBgTilemapBufferRect(bgLayer, tileNum + 0, tilemapLeft,              tilemapTop,                 1,          1,          palNum);
    FillBgTilemapBufferRect(bgLayer, tileNum + 1, tilemapLeft + 1,          tilemapTop,                 width - 2,  1,          palNum);
    FillBgTilemapBufferRect(bgLayer, tileNum + 2, tilemapLeft + width - 1,  tilemapTop,                 1,          1,          palNum);
    FillBgTilemapBufferRect(bgLayer, tileNum + 3, tilemapLeft,              tilemapTop + 1,             1,          height - 2, palNum);
    FillBgTilemapBufferRect(bgLayer, tileNum + 5, tilemapLeft + width - 1,  tilemapTop + 1,             1,          height - 2, palNum);
    FillBgTilemapBufferRect(bgLayer, tileNum + 6, tilemapLeft,              tilemapTop + height - 1,    1,          1,          palNum);
    FillBgTilemapBufferRect(bgLayer, tileNum + 7, tilemapLeft + 1,          tilemapTop + height - 1,    width -     2,  1,      palNum);
    FillBgTilemapBufferRect(bgLayer, tileNum + 8, tilemapLeft + width - 1,  tilemapTop + height - 1,    1,          1,          palNum);
}

void rbox_fill_rectangle(u8 windowId)
{
    u8 bgLayer = GetWindowAttribute(windowId, WINDOW_BG);
    u16 tilemapLeft = GetWindowAttribute(windowId, WINDOW_TILEMAP_LEFT);
    u16 tilemapTop = GetWindowAttribute(windowId, WINDOW_TILEMAP_TOP);
    u16 width = GetWindowAttribute(windowId, WINDOW_WIDTH);
    u16 height = GetWindowAttribute(windowId, WINDOW_HEIGHT);

    FillBgTilemapBufferRect(bgLayer, 0, tilemapLeft - 1, tilemapTop - 1, width + 2, height + 2, 0x11);
}

const u16 *GetTextWindowPalette(u8 id)
{
    switch (id)
    {
    case 0:
        id = 0x00;
        break;
    case 1:
        id = 0x10;
        break;
    case 2:
        id = 0x20;
        break;
    case 3:
        id = 0x30;
        break;
    case 4:
    default:
        id = 0x40;
        break;
    }

    return (const u16 *)(sTextWindowPalettes) + id;
}

const u16 *GetOverworldTextboxPalettePtr(void)
{
    return gMessageBox_Pal;
}

// Effectively LoadUserWindowBorderGfx but specifying the bg directly instead of a window from that bg
void LoadUserWindowBorderGfxOnBg(u8 bg, u16 destOffset, u8 palOffset)
{
    LoadBgTiles(bg, sWindowFrames[gSaveBlock2Ptr->optionsWindowFrameType].tiles, 0x120, destOffset);
    LoadPalette(GetWindowFrameTilesPal(gSaveBlock2Ptr->optionsWindowFrameType)->pal, palOffset, PLTT_SIZE_4BPP);
}
