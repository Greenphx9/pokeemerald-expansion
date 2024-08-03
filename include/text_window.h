#ifndef GUARD_TEXT_WINDOW_H
#define GUARD_TEXT_WINDOW_H

#define WINDOW_FRAMES_COUNT 20

struct TilesPal
{
    const u8 *tiles;
    const u16 *pal;
};

extern const u16 gSignpostWindow_Gfx[];
extern const u8 gTextWindowFrame1_Gfx[];
extern const u16 gTextWindowFrame1_Pal[];
extern const u16 gStdTextWindow_KantoGfx[];
extern const u16 gTextWindowPalettesKanto[][16];

const struct TilesPal *GetWindowFrameTilesPal(u8 id);
void LoadMessageBoxGfx(u8 windowId, u16 destOffset, u8 palOffset);
void LoadMessageBoxGfx_Kanto(u8 windowId, u16 destOffset, u8 palOffset);
void LoadMenuMessageWindowGfx(u8 windowId, u16 tileStart, u8 palette);
void LoadWindowGfx(u8 windowId, u8 frameId, u16 destOffset, u8 palOffset);
void LoadUserWindowBorderGfx(u8 windowId, u16 destOffset, u8 palOffset);
void LoadUserWindowBorderGfx_(u8 windowId, u16 destOffset, u8 palOffset);
void LoadUserWindowBorderGfxOnBg(u8 bg, u16 destOffset, u8 palOffset);
void LoadSignpostWindowGfx(u8 windowId, u16 destOffset, u8 palIdx);
void DrawTextBorderOuter(u8 windowId, u16 tileNum, u8 palNum);
void DrawTextBorderInner(u8 windowId, u16 tileNum, u8 palNum);
void rbox_fill_rectangle(u8 windowId);
const u16 *GetTextWindowPalette(u8 id);
const u16 *GetOverworldTextboxPalettePtr(void);
const u16 *GetTextWindowPaletteKanto(u8 id);

#endif // GUARD_TEXT_WINDOW_H
