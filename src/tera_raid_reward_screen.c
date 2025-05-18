#include "tera_raid_screen.h"
#include "tera_raid_reward_screen.h"

#include "gba/types.h"
#include "gba/defines.h"
#include "global.h"
#include "main.h"
#include "bg.h"
#include "text_window.h"
#include "window.h"
#include "constants/characters.h"
#include "palette.h"
#include "task.h"
#include "overworld.h"
#include "malloc.h"
#include "gba/macro.h"
#include "menu_helpers.h"
#include "menu.h"
#include "malloc.h"
#include "scanline_effect.h"
#include "sprite.h"
#include "constants/rgb.h"
#include "decompress.h"
#include "constants/songs.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "pokemon_icon.h"
#include "graphics.h"
#include "data.h"
#include "pokedex.h"
#include "gpu_regs.h"
#include "constants/event_objects.h"
#include "event_object_movement.h"
#include "rtc.h"
#include "event_data.h"
#include "constants/flags.h"
#include "trainer_pokemon_sprites.h"
#include "pokemon_summary_screen.h"
#include "battle_main.h"
#include "util.h"
#include "battle.h"
#include "item.h"
#include "list_menu.h"
#include "strings.h"
#include "item_icon.h"
#include "international_string_util.h"
#include "constants/trainers.h"
#include "constants/pokemon.h"
#include "constants/abilities.h"

// This code is based on Ghoulslash's excellent UI tutorial:
// https://www.pokecommunity.com/showpost.php?p=10441093

/*
 * Glossary of acronyms and abbreviations -- because sometimes I go crazy with these
 *
 * BG = background
 * GF = Game Freak
 * DMA = Direct Memory Access
 * GBA = Game Boy Advance
 */

/*
 * Some general resources you should bookmark:
 * TONC GBA tutorial: https://www.coranac.com/tonc/text/toc.htm
 * Computer systems course with focus on GBA: https://ianfinlayson.net/class/cpsc305/notes
 * GBATEK reference docs: https://problemkaputt.de/gbatek.htm
 * Copetti GBA Architecture: https://www.copetti.org/writings/consoles/game-boy-advance/
 */

/*
 * The meat of the menu code lives in this file. The tutorial comments assume the following:
 *
 * 1) You understand the basics of GBA graphics i.e. the multiple BG layers and how they are controlled, the various
 *    GBA graphics modes, the basics of hardware sprites, etc
 *
 * 2) You are familiar with the concept of tiles (tilesets) vs tilemaps. These are two distinct concepts and you need to
 *    understand them to understand any UI code.
 *
 * 3) You are familiar with the concept of a charblock and a screenblock, which are crucial to proper VRAM layout.
 *
 * 4) You understand the basics of the pokeemerald Callback and Task systems, which structure the menu's control flow.
 *
 * 5) You have a basic grasp of the GBA memory layout -- you know the difference between VRAM, EWRAM, IWRAM, etc.
 *
 * 6) Some of the UI code makes use of DMA (Direct Memory Access). This assumes you are familiar with what DMA is and
 *    how it works on the GBA. Also note that a lot of the GF library code does not perform DMA directly, rather it
 *    schedules a DMA copy to occur during the next VBlank using a circle buffer of DMA transfer request objects. See
 *    the code in `gflib/dma3_manager.{h,c} for more details.
 *
 * If you are unsure about any of these concepts, please check the following resources:
 *
 * GBA graphics basics:
 *     + https://www.coranac.com/tonc/text/video.htm
 *     + https://ianfinlayson.net/class/cpsc305/notes/07-gba1
 *
 * Tiles, tilemaps, charblocks, screenblocks:
 *     + https://www.coranac.com/tonc/text/objbg.htm
 *     + https://www.coranac.com/tonc/text/regbg.htm
 *     + https://ianfinlayson.net/class/cpsc305/notes/14-tiles <--- this is a highly recommended resource
 *
 * Callbacks and Tasks:
 *     + https://github.com/pret/pokeemerald/wiki/Overview%E2%88%B6-The-Game-Loop
 *     + https://github.com/pret/pokeemerald/wiki/Overview%E2%88%B6-The-Task-System
 *
 * GBA Memory Layout:
 *     + https://www.coranac.com/tonc/text/hardware.htm#sec-memory
 *     + https://problemkaputt.de/gbatek.htm#gbamemorymap
 *
 * DMA:
 *     + https://www.coranac.com/tonc/text/dma.htm
 *     + https://ianfinlayson.net/class/cpsc305/notes/13-memory
 */

/*
 * Basic Code Flow
 * A summary of the basic control flow path for this code. This may help aid in understanding which functions get
 * called, and when.
 *
 * 1) Any code that would like to launch this menu must include `sample_ui.h' so it has the right hook-in, specifically
 *    `Task_OpenSampleUi_Simple'. It must then setup the transition (however is relevant based on its context) and set
 *    the active task to `Task_OpenSampleUi_Simple'. In our case, `start_menu.c' is the caller of this task.
 *
 * 2) `Task_OpenSampleUi_Simple' waits for any active fades to finish, then it calls our init code in `TeraRaidRewardScreen_Init'
 *    which changes the gMainCallback2 to our `TeraRaidRewardScreen_SetupCB'.
 *
 * 3) `TeraRaidRewardScreen_SetupCB' runs each frame, bit-by-bit getting our menu initialized. Once initialization has finished,
 *    this callback:
 *       1) Sets up a new task `Task_TeraRaidRewardScreenWaitFadeIn' which waits until we fade back in before hotswapping itself for
 *          `Task_TeraRaidRewardScreenMainInput' which reads input and updates the menu state.
 *       2) Starts a palette fade to bring the screen from black back to regular colors
 *       3) Sets our VBlank callback to `TeraRaidRewardScreen_VBlankCB' (which is called every VBlank as part of the VBlank
 *          interrupt service routine). This callback transfers our OAM and palette buffers into VRAM, among other
 *          things
 *       4) Sets gMainCallback2 to our menu's main callback `TeraRaidRewardScreen_MainCB', which does the standard processing of
 *          tasks, animating of sprites, etc.
 *
 * 4) We have reached our standard menu state. Every frame `TeraRaidRewardScreen_MainCB' runs, which calls `Task_TeraRaidRewardScreenMainInput`
 *    to get input from the user and update menu state and backing graphics buffers. `TeraRaidRewardScreen_MainCB' also updates
 *    other important gamestate. Then, when VBlank occurs, our `TeraRaidRewardScreen_VBlankCB' copies palettes and OAM into VRAM
 *    before pending DMA transfers fire and copy any screen graphics updates into VRAM.
 */

/*
 * Various state for the UI -- we'll allocate this on the heap since none of it needs to be preserved after we exit the
 * menu.
 */
struct TeraRaidRewardScreenState
{
    // Save the callback to run when we exit: i.e. where do we want to go after closing the menu
    MainCallback savedCallback;
    // We will use this later to track some loading state
    u8 loadState;

    u8 encounterSpriteId;
    bool8 outlinedSprite;
    u8 typeIconSpriteId;

    struct ListMenuItem listItems[24];
    u8 itemNames[24][ITEM_NAME_LENGTH + 10];

    u8 listTaskId;
    u16 itemsAbove;
    u16 cursorPos;

    u8 itemSpriteId;
    u8 outlineItemSpriteIds[8];

    u8 scrollIndicatorTaskId;
};

/*
 * Both of these can be pointers that live in EWRAM -- allocating the actual data on the heap will save precious WRAM
 * space since none of this data matters outside the context of our menu. We can easily cleanup when we're done. It's
 * worth noting that every time the game re-loads into the overworld, the heap gets nuked from orbit. However, it is
 * still good practice to clean up after oneself, so we will be sure to free everything before exiting.
 */
static EWRAM_DATA struct TeraRaidRewardScreenState *sTeraRaidRewardScreenState = NULL;
static EWRAM_DATA u8 *sBg1TilemapBuffer = NULL;
static EWRAM_DATA struct TeraRaidFixedDrop sTeraRaidRewardDrops[23] = {}; // reuse data struct
static EWRAM_DATA u8 sTeraRaidRewardDropCount = 0;

/*
 * Defines and read-only data for on-screen dex.
 */

/*
 * Dex mode defines. Info mode shows dex number and description. Stats and Other modes will show placeholder text, but
 * you can change this to show whatever info you want.
 */

/*
 * BgTemplates are just a nice way to setup various BG attributes without having to deal with manually writing to the
 * various display and BG control registers that the Game Boy actually cares about. GF handles that for you with lots of
 * clever BG management code. All you have to do is tell it what you want. If you want to see the gory details of BG
 * control registers, check out this resource and the code in `gflib/bg.{h,c}':
 * https://problemkaputt.de/gbatek.htm#lcdiobgcontrol
 */
static const struct BgTemplate sSampleUiBgTemplates[] =
{
    {
        // We will use BG0 for the text windows
        .bg = 0,
        // Use charblock 0 for BG0 tiles
        .charBaseIndex = 0,
        // Use screenblock 31 for BG0 tilemap
        // (It has become customary to put tilemaps in the final few screenblocks)
        .mapBaseIndex = 31,
        // Draw the text windows on top of the main BG
        .priority = 1
    },
    {
        // The Main BG: we will use BG1 for the menu graphics
        .bg = 1,
        // Use charblock 3 for BG1 tiles
        .charBaseIndex = 3,
        // Use screenblock 30 for BG1 tilemap
        .mapBaseIndex = 30,
        // Draw this BG below BG0, since we want text drawn on top of the menu graphics
        .priority = 2
    }
    /*
     * I encourage you to open the mGBA Tools/Game State Views/View Tiles menu to see how this above code gets
     * translated into an actual VRAM layout. Quiz yourself by changing the charBaseIndex and mapBaseIndex and guessing
     * what will happen before looking at the Tile Viewer.
     */
};

/*
 * Like the above BgTemplates, WindowTemplates are just an easy way to setup some software windows (which are provided
 * by the GameFreak library). GF's window system is used primarily for drawing dynamic text on the background layers.
 * Their inner workings are quite confusing, so hopefully this tutorial clears some things up.
 *
 * One big thing to note is that GF windows use a technique called tile rendering to draw text. With normal BG
 * rendering, you have a pre-drawn tileset, and then you dynamically update the tilemap, based on gamestate, to change
 * what's shown on screen. With tile rendering, this process is basically flipped on its head. GF tile-rendered windows
 * instead write a preset increasing sequence of tile indexes into the parts of the BG tilemap that represent a given
 * window. Then, to draw text, it dynamically draws to a backing pixel buffer (the `u8 *tileData' in the Window struct)
 * that is copied into VRAM just like regular tile data. This effectively allows the text rendering code to treat the
 * window BG as a pseudo-framebuffer, as if we were in a simple bitmap mode like Mode 3. This technique is advantageous
 * because it allows for maximum flexibility with font sizing and spacing. You aren't locked into 8x8 tiles for each
 * character.
 *
 * For more information about tile rendering and text rendering sytems in general, check out the TTE TONC tutorial and
 * the tile rendering section specifically. You can also consult Game Freak's code in `gflib/window.c' and
 * `gflib/text.c'.
 * https://www.coranac.com/tonc/text/tte.htm#sec-chr
 */

// GF window system passes window IDs around, so define this to avoid using magic numbers everywhere
enum WindowIds
{
    WIN_UI_CONTROLS,
    WIN_UI_CAUGHT_MON,
    WIN_UI_ITEM_LIST,
    WIN_UI_REWARDS,
    WIN_UI_ITEM_DESC,
};
static const struct WindowTemplate sTeraRaidScreenWindowTemplates[] =
{
    [WIN_UI_CONTROLS] =
    {
        .bg = 0,
        .tilemapLeft = 25,
        .tilemapTop = 18,
        .width = 8,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 93
    },
    [WIN_UI_CAUGHT_MON] =
    {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 0,
        .width = 16,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 109
    },
    [WIN_UI_ITEM_LIST] =
    {
        .bg = 0,
        .tilemapLeft = 14,
        .tilemapTop = 5,
        .width = 18,
        .height = 13,
        .paletteNum = 15,
        .baseBlock = 141
    },
    [WIN_UI_REWARDS] =
    {
        .bg = 0,
        .tilemapLeft = 14,
        .tilemapTop = 3,
        .width = 7,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 375,
    },
    [WIN_UI_ITEM_DESC] =
    {
        .bg = 0,
        .tilemapLeft = 0,
        .tilemapTop = 14,
        .width = 12,
        .height = 5,
        .paletteNum = 15,
        .baseBlock = 389,
    },
    DUMMY_WIN_TEMPLATE
};
/*
 * The tile file is generated from properly a indexed tile PNG image. You MUST use an indexed PNG with 4bpp indexing
 * (you can technically get away with 8bpp indexing as long as each individual index is between 0-15). The easiest way
 * to make indexed PNGs is using a program like GraphicsGale or Aseprite (in Index mode).
 */
static const u32 sTeraRaidRewardScreenTiles[] = INCBIN_U32("graphics/tera_raid_screen/tera_raid_reward_screen.4bpp.lz");

/*
 * I created this tilemap in TilemapStudio using the above tile PNG. I highly recommend TilemapStudio for exporting maps
 * like this.
 */
static const u32 sTeraRaidRewardScreenTilemap[] = INCBIN_U32("graphics/tera_raid_screen/tera_raid_reward_screen.bin.lz");

/*
 * This palette was built from a JASC palette file that you can export using GraphicsGale or Aseprite. Please note: the
 * palette conversion tool REQUIRES that JASC palette files end in CRLF, the standard Windows line ending. If you are
 * using the Mac/Linux version of a tool like Aseprite, you may get errors complaining that your lines end in LF and not
 * CRLF. To remedy this, run your JASC palette file through a tool like unix2dos and you shouldn't have any more
 * problems.
 */
static const u16 sTeraRaidRewardScreenPalette[] = INCBIN_U16("graphics/tera_raid_screen/tera_raid_reward_screen.gbapal");

#define TAG_ITEM_ICON    5110
#define TAG_SCROLL_ARROW 5108

#define GFXTAG_STAR 0x4002
#define PALTAG_STAR 0x4003

static const u32 sStar_Gfx[]     = INCBIN_U32("graphics/tera_raid_screen/star.4bpp.lz");
static const u16 sStar_Pal[]     = INCBIN_U16("graphics/tera_raid_screen/star.gbapal");

static const struct OamData sOamData_Star =
{
    .size = SPRITE_SIZE(8x8),
    .shape = SPRITE_SHAPE(8x8),
    .priority = 0,
};

static const struct CompressedSpriteSheet sSpriteSheet_Star =
{
    .data = sStar_Gfx,
    .size = 8*8,
    .tag = GFXTAG_STAR,
};

static const struct SpritePalette sSpritePal_Star =
{
    .data = sStar_Pal,
    .tag = PALTAG_STAR
};

static const union AnimCmd sSpriteAnim_Star[] =
{
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd *const sSpriteAnimTable_Star[] =
{
    sSpriteAnim_Star,
};

static const struct SpriteTemplate sSpriteTemplate_Star =
{
    .tileTag = GFXTAG_STAR,
    .paletteTag = PALTAG_STAR,
    .oam = &sOamData_Star,
    .anims = sSpriteAnimTable_Star,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

// Define some font color values that will index into our font color table below.
enum FontColor
{
    FONT_BLACK,
    FONT_WHITE,
    FONT_RED,
    FONT_BLUE,
};
static const u8 sTeraRaidScreenWindowFontColors[][3] =
{
    /*
     * The TEXT_COLOR_X macros here are just palette indexes. Specifically, these indexes match the colors in
     * `gMessageBox_Pal', so we will need to make sure that palette is loaded. Since our window is set to use
     * palette 15, we'll load it into BG palette slot 15 in the menu loading code.
     */
    [FONT_BLACK]  = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_DARK_GRAY,  TEXT_COLOR_LIGHT_GRAY},
    [FONT_WHITE]  = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_WHITE,      TEXT_COLOR_DARK_GRAY},
    [FONT_RED]    = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_RED,        TEXT_COLOR_LIGHT_GRAY},
    [FONT_BLUE]   = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_BLUE,       TEXT_COLOR_LIGHT_GRAY},
};

// Callbacks for the sample UI
static void TeraRaidRewardScreen_SetupCB(void);
static void TeraRaidRewardScreen_MainCB(void);
static void TeraRaidRewardScreen_VBlankCB(void);

// Sample UI tasks
static void Task_TeraRaidRewardScreenWaitFadeIn(u8 taskId);
static void Task_TeraRaidRewardScreenMainInput(u8 taskId);
static void Task_TeraRaidRewardScreenWaitFadeAndBail(u8 taskId);
static void Task_TeraRaidRewardScreenWaitFadeAndExitGracefully(u8 taskId);

// Sample UI helper functions
void TeraRaidRewardScreen_Init(MainCallback callback);
static bool8 TeraRaidRewardScreen_InitBgs(void);
static void TeraRaidRewardScreen_FadeAndBail(void);
static bool8 TeraRaidRewardScreen_LoadGraphics(void);
static void TeraRaidRewardScreen_InitWindows(void);
static void TeraRaidRewardScreen_FreeResources(void);
static void TeraRaidRewardScreen_PrintWindowText(void);
static void TeraRaidRewardScreen_LoadEncounterGfx(void);
static void TeraRaidRewardScreen_LoadTypeGfx(void);
static void TeraRaidRewardScreen_LoadStarGfx(void);
static void TeraRaidRewardScreen_GiveItems(void);
static void TeraRaidRewardScreen_CreateListMenu(void);
static void TeraRaidRewardScreen_AddScrollIndicator(void);
static void TeraRaidRewardScreen_RemoveScrollIndicator(void);

static void TeraRaidRewardScreen_MoveCursor(s32 id, bool8 onInit, struct ListMenu *list);
static void TeraRaidRewardScreen_PrintMenuItem(u8 windowId, u32 id, u8 yOffset);

// Declared in sample_ui.h
void Task_OpenTeraRaidRewardScreen_StartHere(u8 taskId)
{
    /*
     * We are still in the overworld callback, so wait until the palette fade is finished (i.e. the screen is entirely
     * black) before we start cleaning things up and changing callbacks (which might affect displayed graphics and cause
     * artifacting)
     */
    if (!gPaletteFade.active)
    {
        /*
         * Free overworld related heap stuff -- if you are entering this menu from somewhere else you may want to do
         * other cleanup. We're entering from overworld start menu so this works for us.
         */
        CleanupOverworldWindowsAndTilemaps();
        // Allocate heap space for menu state and set up callbacks
        TeraRaidRewardScreen_Init(CB2_ReturnToFieldWithOpenMenu);
        // Our setup is done, so destroy ourself.
        DestroyTask(taskId);
    }
}

void TeraRaidRewardScreen_Init(MainCallback callback)
{
    sTeraRaidRewardScreenState = AllocZeroed(sizeof(struct TeraRaidRewardScreenState));
    if (sTeraRaidRewardScreenState == NULL)
    {
        /*
         * If the heap allocation failed for whatever reason, then set the callback to just return to the overworld.
         * This really shouldn't ever happen but if it does, this is better than hard crashing and making the user think
         * you bricked their Game Boy.
         */
        SetMainCallback2(callback);
        return;
    }

    sTeraRaidRewardScreenState->loadState = 0;
    sTeraRaidRewardScreenState->savedCallback = callback;

    /*
     * Next frame start running UI setup code. SetMainCallback2 also resets gMain.state to zero, which we will need for
     * the SetupCB
     */
    SetMainCallback2(TeraRaidRewardScreen_SetupCB);
}

// credits to Skeli & CFRU: https://github.com/Skeli789/Complete-Fire-Red-Upgrade/blob/master/src/raid_intro.c#L691
static void TeraRaidRewardScreen_ClearVramOamPlttRegs(void)
{
	DmaFill16(3, 0, VRAM, VRAM_SIZE);
	DmaFill32(3, 0, OAM, OAM_SIZE);
	DmaFill16(3, 0, PLTT, PLTT_SIZE);
	SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP);
	SetGpuReg(REG_OFFSET_BG3CNT, DISPCNT_MODE_0);
	SetGpuReg(REG_OFFSET_BG2CNT, DISPCNT_MODE_0);
	SetGpuReg(REG_OFFSET_BG1CNT, DISPCNT_MODE_0);
	SetGpuReg(REG_OFFSET_BG0CNT, DISPCNT_MODE_0);
	SetGpuReg(REG_OFFSET_BG3HOFS, DISPCNT_MODE_0);
	SetGpuReg(REG_OFFSET_BG3VOFS, DISPCNT_MODE_0);
	SetGpuReg(REG_OFFSET_BG2HOFS, DISPCNT_MODE_0);
	SetGpuReg(REG_OFFSET_BG2VOFS, DISPCNT_MODE_0);
	SetGpuReg(REG_OFFSET_BG1HOFS, DISPCNT_MODE_0);
	SetGpuReg(REG_OFFSET_BG1VOFS, DISPCNT_MODE_0);
	SetGpuReg(REG_OFFSET_BG0HOFS, DISPCNT_MODE_0);
	SetGpuReg(REG_OFFSET_BG0VOFS, DISPCNT_MODE_0);
}

static void TeraRaidRewardScreen_SetupCB(void)
{
    /*
     * You may ask: why are these tasks in a giant switch statement? For one thing, it is because this is how GameFreak
     * does things, and cargo cult programming is a glorious institution. On a more serious note, the intention is to
     * control how much work is done each frame, and to prevent our setup code from getting interrupted in the middle of
     * something important. So ideally, we do a small bit of work in each case statement, exit the function, and then
     * wait for VBlank. Then next frame, we come back around and complete the work in the subsequent case statement.
     *
     * You may ask further: how can we be sure that the code in each case finishes before the end of the frame? The
     * short answer is (besides counting cycles, which is really hard to do given the complexity of the code being
     * called) we can't. The size of each case is generally a best guess, a crude attempt at doing a small amount of
     * work before stopping and letting the next frame handle the next bit.
     */
    switch (gMain.state)
    {
    case 0:
        /*
         * Reset all graphics registers and clear VRAM / OAM. There may be garbage values from previous screens that
         * could screw up your UI. It's safer to just reset everything so you have a blank slate.
         */
        TeraRaidRewardScreen_ClearVramOamPlttRegs();
        // Null out V/H blanking callbacks since we are not drawing anything atm
        SetVBlankHBlankCallbacksToNull();
        /*
         * If previous game state had scheduled any copies to VRAM, cancel these now so we don't accidentally draw
         * garbage to the screen.
         */
        ClearScheduledBgCopiesToVram();
        gMain.state++;
        break;
    case 1:
        /*
         * Unclear on what this does, I think it is related to some of the screen transition effects. In any case, we
         * don't want any of those since this is a menu, dammit
         */
        ScanlineEffect_Stop();
        /*
         * Clear all sprite palette tags in the sprite system. Sprite palette tags will be explained in more detail in
         * the next tutorial. For now, just accept that we need to clear them for things to work properly.
         */
        FreeAllSpritePalettes();
        /*
         * Reset palette fade settings -- we are currently in a fade-to-black initiated by whatever code opened this
         * menu screen. Since we don't know what they were doing with the palettes, just reset everything so we can do a
         * simple fade-in when we're done loading.
         */
        ResetPaletteFade();
        // Completely clear all sprite buffers and settings
        ResetSpriteData();
        /*
         * Completely clear all task data. There should be no tasks running right now so make sure nothing is hanging
         * around from whatever code got us into this menu.
         */
        ResetTasks();
        gMain.state++;
        break;
    case 2:
        // Try to run the BG init code
        if (TeraRaidRewardScreen_InitBgs())
        {
            // If we successfully init the BGs, we can move on
            sTeraRaidRewardScreenState->loadState = 0;
            gMain.state++;
        }
        else
        {
            /*
             * Otherwise, fade out, free the heap data, and return to main menu. Like before, this shouldn't ever really
             * happen but it's better to handle it then have a surprise hard-crash.
             */
            TeraRaidRewardScreen_FadeAndBail();
            return;
        }
        break;
    case 3:
        // `TeraRaidRewardScreen_LoadGraphics' has its own giant switch statement, so keep calling until it returns TRUE at the end
        if (TeraRaidRewardScreen_LoadGraphics() == TRUE)
        {
            // Only advance the state of this load switch statment once all the LoadGraphics logic has finished.
            gMain.state++;
        }
        break;
    case 4:
        // Set up our text windows
        TeraRaidRewardScreen_InitWindows();
        gMain.state++;
        break;
    case 5:
        /*
         * Free all mon icon palettes just to make sure nothing is left over from previous screen. The sprite system
         * uses a technique called palette tagging to help the game keep track of which palettes are in use, and by
         * which sprites. As mentioned above, we will cover palette tags in more detail in the next tutorial.
         */
        FreeMonIconPalettes();

        /*
         * Since this is a small demo without many sprites, we can just load all 6 default mon icon palettes at once.
         * If you have a more complex UI with lots of unique-palette sprites in addition to mon icons, you may instead
         * want to manage mon icon palettes dynamically based on which mon icons are currently on screen. You can do
         * this with the more granular `LoadMonIconPalette(u16 species)' and `FreeMonIconPalette(u16 species)'
         * functions.
         */

        sTeraRaidRewardScreenState->cursorPos = 0;
        sTeraRaidRewardScreenState->itemsAbove = 0;
        sTeraRaidRewardScreenState->scrollIndicatorTaskId = TASK_NONE;

        TeraRaidRewardScreen_GiveItems();
        TeraRaidRewardScreen_CreateListMenu();
        TeraRaidRewardScreen_PrintWindowText();
        TeraRaidRewardScreen_LoadStarGfx();
        TeraRaidRewardScreen_LoadTypeGfx();
        TeraRaidRewardScreen_LoadEncounterGfx();
        // Create a task that does nothing until the palette fade is done. We will start the palette fade next frame.
        CreateTask(Task_TeraRaidRewardScreenWaitFadeIn, 0);
        gMain.state++;
        break;
    case 6:
        /*
         * Fade screen in from black, this will take multiple frames to finish so we'll want the above active task poll
         * the fade to completion before continuing processing.
         */
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        gMain.state++;
        break;
    case 7:
        // Finally we can set our main callbacks since loading is finished
        SetVBlankCallback(TeraRaidRewardScreen_VBlankCB);
        SetMainCallback2(TeraRaidRewardScreen_MainCB);
        break;
    }
}

static void TeraRaidRewardScreen_MainCB(void)
{
    // Iterate through the Tasks list and run any active task callbacks
    RunTasks();
    // For all active sprites, call their callbacks and update their animation state
    AnimateSprites();
    /*
     * After all sprite state is updated, we need to sort their information into the OAM buffer which will be copied
     * into actual OAM during VBlank. This makes sure sprites are drawn at the correct positions and in the correct
     * order (recall sprite draw order determines which sprites appear on top of each other).
     */
    BuildOamBuffer();
    /*
     * This one is a little confusing because there are actually two layers of scheduling. Regular game code can call
     * `ScheduleBgCopyTilemapToVram(u8 bgId)' which will simply mark the tilemap for `bgId' as "ready to be copied".
     * Then, calling `DoScheduledBgTilemapCopiesToVram' here does not actually perform the copy. Rather it simply adds a
     * DMA transfer request to the DMA manager for this buffer copy. Only during VBlank when DMA transfers are processed
     * does the copy into VRAM actually occur.
     */
    DoScheduledBgTilemapCopiesToVram();
    // If a palette fade is active, tick the udpate
    UpdatePaletteFade();
}

static void TeraRaidRewardScreen_VBlankCB(void)
{
    /*
     * Handle direct CPU copies here during the VBlank period. All of these transfers affect what is displayed on
     * screen, so we wait until VBlank to make the copies from the backbuffers.
     */

    // Transfer OAM buffer into VRAM OAM area
    LoadOam();
    /*
     * Sprite animation code may have updated frame image for sprites, so copy all these updated frames into the correct
     * VRAM location.
     */
    ProcessSpriteCopyRequests();
    // Transfer the processed palette buffer into VRAM palette area
    TransferPlttBuffer();
}

static void Task_TeraRaidRewardScreenWaitFadeIn(u8 taskId)
{
     // Do nothing until the palette fade finishes, then replace ourself with the main menu task.
    if (!gPaletteFade.active)
    {
        gTasks[taskId].func = Task_TeraRaidRewardScreenMainInput;
    }
}

static void Task_TeraRaidRewardScreenMainInput(u8 taskId)
{
    // Exit the menu when the player presses B
    if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_PC_OFF);
        gSpecialVar_Result = 0;
        // Fade screen to black
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        // Replace ourself with the "exit gracefully" task function
        gTasks[taskId].func = Task_TeraRaidRewardScreenWaitFadeAndExitGracefully;
    }
    else
    {
        ListMenu_ProcessInput(sTeraRaidRewardScreenState->listTaskId);
        ListMenuGetScrollAndRow(sTeraRaidRewardScreenState->listTaskId, &sTeraRaidRewardScreenState->itemsAbove, &sTeraRaidRewardScreenState->cursorPos);
    }
}

static void Task_TeraRaidRewardScreenWaitFadeAndBail(u8 taskId)
{
    // Wait until the screen fades to black before we start doing cleanup
    if (!gPaletteFade.active)
    {
        SetMainCallback2(sTeraRaidRewardScreenState->savedCallback);
        TeraRaidRewardScreen_FreeResources();
        DestroyTask(taskId);
    }
}

static void Task_TeraRaidRewardScreenWaitFadeAndExitGracefully(u8 taskId)
{
    /*
     * This function is basically the same as Task_TeraRaidRewardScreenWaitFadeAndBail. However, for this sample we broke it out
     * because typically you might want to do something different if the user gracefully exits a menu vs if you got
     * booted from a menu due to heap allocation failure.
     */

    // E.g. you could do some other processing here

    // Wait until the screen fades to black before we start doing final cleanup
    if (!gPaletteFade.active)
    {
        SetMainCallback2(CB2_ReturnToFieldContinueScript);
        TeraRaidRewardScreen_FreeResources();
        DestroyTask(taskId);
    }
}

static bool8 TeraRaidRewardScreen_InitBgs(void)
{
    /*
     * 1 screenblock is 2 KiB, so that should be a good size for our tilemap buffer. We don't need more than one
     * screenblock since BG1's size setting is 0, which tells the GBA we are using a 32x32 tile background:
     *      (32 tile * 32 tile * 2 bytes/tile = 2048)
     * For more info on tilemap entries and how they work:
     * https://www.coranac.com/tonc/text/regbg.htm#sec-map
     */
    const u32 TILEMAP_BUFFER_SIZE = (1024 * 2);

    // BG registers may have scroll values left over from the previous screen. Reset all scroll values to 0.
    ResetAllBgsCoordinates();

    // Allocate our tilemap buffer on the heap
    sBg1TilemapBuffer = AllocZeroed(TILEMAP_BUFFER_SIZE);
    if (sBg1TilemapBuffer == NULL)
    {
        // Bail if the allocation fails
        return FALSE;
    }

    /*
     * Clear all BG-related data buffers and mark DMAs as ready. Also resets the BG and mode bits of reg DISPCNT to 0.
     * This will effectively turn off all BGs and activate Mode 0.
     * LTODO explain the underlying sDmaBusyBitfield here
     */
    ResetBgsAndClearDma3BusyFlags(0);

    /*
     * Use the BG templates defined at the top of the file to init various cached BG attributes. These attributes will
     * be used by the various load methods to correctly setup VRAM per the user template. Some of the attributes can
     * also be pushed into relevant video regs using the provided functions in `bg.h'
     */
    InitBgsFromTemplates(0, sSampleUiBgTemplates, NELEMS(sSampleUiBgTemplates));

    // Set the BG manager to use our newly allocated tilemap buffer for BG1's tilemap
    SetBgTilemapBuffer(1, sBg1TilemapBuffer);

    /*
     * Schedule to copy the tilemap buffer contents (remember we zeroed it out earlier) into VRAM on the next VBlank.
     * Right now, BG1 will just use Tile 0 for every tile. We will change this once we load in our true tilemap
     * values from sTeraRaidScreenTilemap.
     */
    ScheduleBgCopyTilemapToVram(1);

    // Set reg DISPCNT to show BG0, BG1. Try commenting these out to see what happens.
    ShowBg(0);
    ShowBg(1);

    return TRUE;
}

static void TeraRaidRewardScreen_FadeAndBail(void)
{
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    CreateTask(Task_TeraRaidRewardScreenWaitFadeAndBail, 0);

    /*
     * Set callbacks to ours while we wait for the fade to finish, then our above task will cleanup and swap the
     * callbacks back to the one we saved earlier (which should re-load the overworld)
     */
    SetVBlankCallback(TeraRaidRewardScreen_VBlankCB);
    SetMainCallback2(TeraRaidRewardScreen_MainCB);
}

static bool8 TeraRaidRewardScreen_LoadGraphics(void)
{
    switch (sTeraRaidRewardScreenState->loadState)
    {
    case 0:
        /*
         * Reset leftover temp buffers from any previous code that used them to load graphics. The loading code in
         * `menu.c' basically saves pointers to the decompression buffers after it copies to VRAM. Here, we just reset
         * all the pointers to NULL and set the tracking index to 0. This obviously assumes the previous screen freed
         * the buffers for us.
         */
        ResetTempTileDataBuffers();

        /*
         * Decompress our tileset and copy it into VRAM using the parameters we set in the BgTemplates at the top -- we
         * pass 1 for the bgId so it uses the charblock setting from the BG1 template.
         * The size, offset, mode parameters are set to 0:
         *
         *      Size is 0 because that tells the function to set the size dynamically based on the decompressed data.
         *
         *      Offset is 0 because we want to tiles loaded right at whatever charblock we set in the BgTemplate.
         *
         *      Mode is 0 because we are copying tiles and not a tilemap, and 0 tells the bg library to use the tile
         *      loading code as opposed to the tilemap loading code (unfortunately GameFreak didn't provide a mode
         *      enum -- it was probably a preprocessor define in the original game code. So you have to look at the
         *      library functions to figure out what the mode values mean).
         *
         * `menu.c' also has a alternative function `DecompressAndLoadBgGfxUsingHeap', which does the same thing but
         * automatically frees the decompression buffer for you. If you want, you can use that here instead and remove
         * the `ResetTempTileDataBuffers' call above, since it doesn't use the temp tile data buffers.
         */
        DecompressAndCopyTileDataToVram(1, sTeraRaidRewardScreenTiles, 0, 0, 0);
        sTeraRaidRewardScreenState->loadState++;
        break;
    case 1:
        /*
         * Each frame, keep trying to free the temp data buffer we used last frame to copy the tile data into VRAM. We
         * have to do a poll here because this free may not occur depending on the state of the DMA manager. If instead
         * you chose to load graphics using the alternative `DecompressAndLoadBgGfxUsingHeap', you can remove this call
         * and wrapping if statement since the polling/freeing is handled for you under the hood.
         * LTODO explain this better, like above sDmaBusyBitfield is being used here, this might be confusing
         */
        if (FreeTempTileDataBuffersIfPossible() != TRUE)
        {
            /*
             * This basically just wraps the LZ77UnCompWram system call. It reads and decompresses whatever data is
             * provided in the `src' (argument 1), and writes the decompressed data to a WRAM location given in `dest'
             * (argument 2). In our case `dest' is just the tilemap buffer we heap-allocated earlier.
             */
            LZDecompressWram(sTeraRaidRewardScreenTilemap, sBg1TilemapBuffer);
            sTeraRaidRewardScreenState->loadState++;
        }
        break;
    case 2:
        /*
         * Copy our palette into the game's BG palette buffer, slot 0 -- this step does not directly get the palette
         * into VRAM. That only happens during VBlank if the current callback specifies a buffer transfer.
         */
        LoadPalette(sTeraRaidRewardScreenPalette, BG_PLTT_ID(0), PLTT_SIZE_4BPP);
        /*
         * Copy the message box palette into BG palette buffer, slot 15. Our window is set to use palette 15 and our
         * text color constants are defined assuming we are indexing into this palette.
         */
        LoadPalette(gMessageBox_Pal, BG_PLTT_ID(15), PLTT_SIZE_4BPP);
        sTeraRaidRewardScreenState->loadState++;
    default:
        sTeraRaidRewardScreenState->loadState = 0;
        return TRUE;
    }
    return FALSE;
}

static void TeraRaidRewardScreen_InitWindows(void)
{
    /*
     * Initialize windows from the window templates we specified above. This makes two important allocations:
     *
     * 1) It allocates a tilemap buffer for the window tilemap, the size of which is based on the screen size setting
     *    specified in the BgTemplate (we didn't set one, so it defaults to 0, see BGXCNT register documentation for
     *    more details on screen sizes). For this UI, the size setting is 0 which just means use a single screen. We
     *    only allocate a single tilemap for the entire BG layer. So if you have multiple windows on the same BG, they
     *    will all share the same tilemap buffer.
     *
     * 2) It allocates one `tileData' buffer (often also called the pixel buffer in the code) for each window. *Each
     *    window has its own pixel buffer.* This is the buffer into which we actually draw text, and it gets copied upon
     *    request to the tile area of VRAM based on the BG charblock and window baseblock.
     *
     * It's also worth noting that the window API allows you to allocate and set your own tilemap buffer for the window
     * BG layer, just like we did earlier for BG1. However, it's better to just let the window API do the allocation and
     * setup for you through `InitWindows()' -- just make sure to call `FreeAllWindowBuffers()' before closing up shop
     * to return your memory.
     */
    InitWindows(sTeraRaidScreenWindowTemplates);

    // Marks all text printers as inactive. Basically just setting flags. That's it.
    DeactivateAllTextPrinters();

    /*
     * Schedule a copy of BG0 tilemap buffer to VRAM. This buffer was allocated for us by `InitWindows()' since we
     * specified a window on BG0 and had not yet set that layer's tilemap buffer. Note that the buffer was also zeroed
     * for us by `InitWindows()'
     */
    ScheduleBgCopyTilemapToVram(0);

    /*
     * Fill each entire window pixel buffer (i.e. window.tileData) with the given value. In this case, fill it with 0s
     * to make the window completely transparent. We will draw text into the window pixel buffer later.
     */
    FillWindowPixelBuffer(WIN_UI_CONTROLS, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    FillWindowPixelBuffer(WIN_UI_CAUGHT_MON, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    FillWindowPixelBuffer(WIN_UI_ITEM_LIST, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    FillWindowPixelBuffer(WIN_UI_REWARDS, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    FillWindowPixelBuffer(WIN_UI_ITEM_DESC, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    /*
     * Write an increasing sequence of tile indexes into each window's tilemap buffer, based on the offset provided by
     * the window.baseBlock. Why? Because the window text will be drawn onto the tiles themselves. So we just want each
     * subsequent tilemap entry to point to the next tile in VRAM. That way, we can treat the tiles as a
     * pseudo-framebuffer and render our text however we want. If you dive deeper into the implementation of
     * `PutWindowTilemap', you will see that it takes some fancy calculation to determine which entries in the
     * tilemap buffer we actually need to modify (because we are mapping from GameFreak windows, which are logically 2D,
     * to VRAM locations, which in reality is basically a 1D array). I will leave exploration of the inner-workings of
     * this function as an exercise to the reader.
     */
    PutWindowTilemap(WIN_UI_CONTROLS);
    PutWindowTilemap(WIN_UI_CAUGHT_MON);
    PutWindowTilemap(WIN_UI_ITEM_LIST);
    PutWindowTilemap(WIN_UI_REWARDS);
    PutWindowTilemap(WIN_UI_ITEM_DESC);

    /*
     * Copy (well, schedule to copy) each window into VRAM using DMA3 under the hood. The COPYWIN_FULL argument means we
     * copy BOTH the tilemap buffer (which recall is just an incrementing sequence of references into our tile canvas)
     * and the tiles themselves. Typically when updating text on a window, you only need to copy the tile canvas (i.e.
     * using COPYWIN_GFX) since the tilemap should never change. But to init the window we need to get both into VRAM.
     */
    CopyWindowToVram(WIN_UI_CONTROLS, COPYWIN_FULL);
    CopyWindowToVram(WIN_UI_CAUGHT_MON, COPYWIN_FULL);
    CopyWindowToVram(WIN_UI_ITEM_LIST, COPYWIN_FULL);
    CopyWindowToVram(WIN_UI_REWARDS, COPYWIN_FULL);
    CopyWindowToVram(WIN_UI_ITEM_DESC, COPYWIN_FULL);
}

static void TeraRaidRewardScreen_FreeResources(void)
{
    // Free our data struct and our BG1 tilemap buffer
    if (sTeraRaidRewardScreenState != NULL)
    {
        Free(sTeraRaidRewardScreenState);
    }
    if (sBg1TilemapBuffer != NULL)
    {
        Free(sBg1TilemapBuffer);
    }
    TeraRaidRewardScreen_RemoveScrollIndicator();
    // Free all allocated tilemap and pixel buffers associated with the windows.
    FreeAllWindowBuffers();
    // Reset all sprite data
    ResetSpriteData();
}

static const u8 sText_Controls[] = _("{B_BUTTON}Close");
static const u8 sText_CaughtMon[] = _("You caught {STR_VAR_1}!");
static const u8 sText_Rewards[] = _("Rewards");
static void TeraRaidRewardScreen_PrintWindowText(void)
{
    StringCopy(gStringVar1, GetSpeciesName(gTeraRaidEncounter.species));
    StringExpandPlaceholders(gStringVar2, sText_CaughtMon);
    AddTextPrinterParameterized4(WIN_UI_CONTROLS, FONT_SMALL, 4, 4, 0, 0, sTeraRaidScreenWindowFontColors[FONT_WHITE], TEXT_SKIP_DRAW, sText_Controls);
    AddTextPrinterParameterized4(WIN_UI_CAUGHT_MON, FONT_SMALL, 0, 0, 0, 0, sTeraRaidScreenWindowFontColors[FONT_WHITE], TEXT_SKIP_DRAW, gStringVar2);
    AddTextPrinterParameterized4(WIN_UI_REWARDS, FONT_NORMAL, 7, 0, 0, 0, sTeraRaidScreenWindowFontColors[FONT_BLACK], TEXT_SKIP_DRAW, sText_Rewards);
    CopyWindowToVram(WIN_UI_CONTROLS, COPYWIN_GFX);
    CopyWindowToVram(WIN_UI_CAUGHT_MON, COPYWIN_GFX);
    CopyWindowToVram(WIN_UI_REWARDS, COPYWIN_GFX);
}

static void TeraRaidRewardScreen_LoadStarGfx(void)
{
    LoadCompressedSpriteSheet(&sSpriteSheet_Star);
    LoadSpritePalette(&sSpritePal_Star); 
    for (int i = 0; i < gTeraRaidStars + 1; ++i)
		CreateSprite(&sSpriteTemplate_Star, 196 - (9 * i), 8, 0);
}

//Type Icon
static void SetSpriteInvisibility(u8 spriteArrayId, bool8 invisible)
{
    gSprites[sTeraRaidRewardScreenState->typeIconSpriteId].invisible = invisible;
}

static void SetTypeIconPosAndPal(u8 typeId, u8 x, u8 y, u8 spriteArrayId)
{
    struct Sprite *sprite;

    sprite = &gSprites[sTeraRaidRewardScreenState->typeIconSpriteId];
    StartSpriteAnim(sprite, typeId);
    sprite->oam.paletteNum = gTypesInfo[typeId].palette - 2;
    sprite->x = x + 16;
    sprite->y = y + 8;
    SetSpriteInvisibility(spriteArrayId, FALSE);
}

static void TeraRaidRewardScreen_LoadTypeGfx(void)
{
    sTeraRaidRewardScreenState->typeIconSpriteId = 0xFF;
    LoadCompressedPalette(gMoveTypes_Pal, OBJ_PLTT_ID(11), 3 * PLTT_SIZE_4BPP);
    LoadCompressedSpriteSheet(&gSpriteSheet_MoveTypes);
    sTeraRaidRewardScreenState->typeIconSpriteId = CreateSprite(&gSpriteTemplate_MoveTypes, 10, 10, 2);
    SetTypeIconPosAndPal(gTeraRaidType, 200, 0, 0);
}

// credits to AgustinGDLV
static void TeraRaidRewardScreen_LoadEncounterGfx(void)
{
    u32 i, j, paletteOffset, spriteId;
	sTeraRaidRewardScreenState->encounterSpriteId = CreateMonPicSprite(gTeraRaidEncounter.species, FALSE, 0xFFFFFFFF, TRUE, 50, 53, 14, TAG_NONE);
    gSprites[sTeraRaidRewardScreenState->encounterSpriteId].oam.priority = 0;

	// Create white outline.
    // TODO: Nicer looking way to do this with fancy VRAM trick, see CFRU.
    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 3; j++)
        {
            if (i == 1 && j == 1)
                continue;
            spriteId = CreateMonPicSprite(gTeraRaidEncounter.species, FALSE, 0xFFFFFFFF, TRUE, 49 + i, 52 + j, gSprites[sTeraRaidRewardScreenState->encounterSpriteId].oam.paletteNum + 1, TAG_NONE);
            gSprites[spriteId].oam.priority = 1;
        }
    }
    paletteOffset = 15 * 16 + 0x100;
    FillPalette(RGB_WHITE, paletteOffset, 32);
    CpuCopy32(gPlttBufferFaded + paletteOffset, gPlttBufferUnfaded + paletteOffset, 32);
}

// spaghetti below, be warned
static void TeraRaidRewardScreen_GiveItems(void)
{
    u32 i, j;
    bool32 found = FALSE;
    
    for (i = 0; i < sTeraRaidRewardDropCount; i++)
    {
        sTeraRaidRewardDrops[i].itemId = ITEM_NONE;
        sTeraRaidRewardDrops[i].count = 0;
    }
    sTeraRaidRewardDropCount = 0;

    for (i = 0; i < gTeraRaidEncounter.fixedDrops.count; i++)
    {
        sTeraRaidRewardDrops[i].itemId = gTeraRaidEncounter.fixedDrops.drops[i].itemId;
        sTeraRaidRewardDrops[i].count = gTeraRaidEncounter.fixedDrops.drops[i].count;
        sTeraRaidRewardDropCount++;
    }

    for (i = 0; i < gTeraRaidEncounter.randomDrops.count; i++)
    {
        struct TeraRaidRandomDrop rDrop = gTeraRaidEncounter.randomDrops.drops[i];
        u32 rand = Random() % 100;
        if (rand < rDrop.chance)
        {
            found = FALSE;
            for (j = 0; j < sTeraRaidRewardDropCount; j++)
            {
                if (rDrop.itemId == sTeraRaidRewardDrops[j].itemId)
                {
                    found = TRUE;
                    sTeraRaidRewardDrops[j].count += rDrop.count;
                    break;
                }
            }
            if (!found)
            {
                sTeraRaidRewardDrops[sTeraRaidRewardDropCount].itemId = rDrop.itemId;
                sTeraRaidRewardDrops[sTeraRaidRewardDropCount].count = rDrop.count;
                sTeraRaidRewardDropCount += 1;
            }
        }
    }
    for (i = 0; i < sTeraRaidRewardDropCount; i++)
    {
        AddBagItem(sTeraRaidRewardDrops[i].itemId, sTeraRaidRewardDrops[i].count);
    }
}

static const struct ListMenuTemplate sTeraRaidRewardMenu_ItemStorage =
{
    .items = NULL,
    .moveCursorFunc = TeraRaidRewardScreen_MoveCursor,
    .itemPrintFunc = TeraRaidRewardScreen_PrintMenuItem,
    .totalItems = 0,
    .maxShowed = 0,
    .windowId = 0,
    .header_X = 0,
    .item_X = 14,
    .cursor_X = 6,
    .upText_Y = 12,
    .cursorPal = 2,
    .fillValue = 0,
    .cursorShadowPal = 3,
    .lettersSpacing = FALSE,
    .itemVerticalPadding = 0,
    .scrollMultiple = LIST_NO_MULTIPLE_SCROLL,
    .fontId = FONT_SMALL,
    .cursorKind = CURSOR_BLACK_ARROW,
    .textNarrowWidth = 74,
};

static void TeraRaidRewardScreen_DrawItemIcon(u16 itemId)
{
    u32 i, j, k = 0;
    u8 spriteId;
    u8 *spriteIdLoc = &sTeraRaidRewardScreenState->itemSpriteId;

    if (*spriteIdLoc == SPRITE_NONE)
    {
        FreeSpriteTilesByTag(TAG_ITEM_ICON);
        FreeSpritePaletteByTag(TAG_ITEM_ICON);
        spriteId = AddItemIconSprite(TAG_ITEM_ICON, TAG_ITEM_ICON, itemId);
        if (spriteId != MAX_SPRITES)
        {
            *spriteIdLoc = spriteId;
            gSprites[spriteId].oam.priority = 0;
            gSprites[spriteId].x = 51;
            gSprites[spriteId].y = 109;
        }
        for (i = 0; i < 3; i++)
        {
            for (j = 0; j < 3; j++)
            {
                if (i == 1 && j == 1)
                    continue;
                sTeraRaidRewardScreenState->outlineItemSpriteIds[k] = AddItemIconSprite2(TAG_ITEM_ICON + k + 1, TAG_ITEM_ICON + k + 1, itemId, FALSE);
                gSprites[sTeraRaidRewardScreenState->outlineItemSpriteIds[k]].oam.priority = 1;
                gSprites[sTeraRaidRewardScreenState->outlineItemSpriteIds[k]].x = 50 + i;
                gSprites[sTeraRaidRewardScreenState->outlineItemSpriteIds[k]].y = 108 + j;
                gSprites[sTeraRaidRewardScreenState->outlineItemSpriteIds[k]].oam.paletteNum = 15;
                k++;
            }
        }
    }
}

static void TeraRaidRewardScreen_EraseItemIcon(void)
{
    u32 i;
    u8 *spriteIdLoc = &sTeraRaidRewardScreenState->itemSpriteId;
    if (*spriteIdLoc != SPRITE_NONE)
    {
        FreeSpriteTilesByTag(TAG_ITEM_ICON);
        FreeSpritePaletteByTag(TAG_ITEM_ICON);
        DestroySprite(&gSprites[*spriteIdLoc]);
        *spriteIdLoc = SPRITE_NONE;
        for (i = 0; i < 8; i++)
        {
            FreeSpriteTilesByTag(TAG_ITEM_ICON + i + 1);
            FreeSpritePaletteByTag(TAG_ITEM_ICON + i + 1);
            DestroySprite(&gSprites[sTeraRaidRewardScreenState->outlineItemSpriteIds[i]]);
        }
    }
}

static void TeraRaidRewardScreen_PrintDescription(s32 id)
{
    const u8 *description;
    u8 windowId = WIN_UI_ITEM_DESC;

    description = (u8 *)ItemId_GetDescription(sTeraRaidRewardDrops[id].itemId);

    FillWindowPixelBuffer(windowId, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, 2, 4, 0, 2, sTeraRaidScreenWindowFontColors[FONT_WHITE], 0, description);
}

static void TeraRaidRewardScreen_MoveCursor(s32 id, bool8 onInit, struct ListMenu *list)
{
    if (onInit != TRUE)
        PlaySE(SE_SELECT);
    TeraRaidRewardScreen_EraseItemIcon();
    TeraRaidRewardScreen_DrawItemIcon(sTeraRaidRewardDrops[id].itemId);
    TeraRaidRewardScreen_PrintDescription(id);
}

static void TeraRaidRewardScreen_PrintMenuItem(u8 windowId, u32 id, u8 yOffset)
{
    ConvertIntToDecimalStringN(gStringVar1, sTeraRaidRewardDrops[id].count, STR_CONV_MODE_RIGHT_ALIGN, 3);
    StringExpandPlaceholders(gStringVar4, gText_xVar1);
    AddTextPrinterParameterized4(windowId, FONT_SMALL, GetStringRightAlignXOffset(FONT_NARROW, gStringVar4, 122), yOffset, 0, 0, sTeraRaidScreenWindowFontColors[FONT_BLACK], TEXT_SKIP_DRAW, gStringVar4);
}

static void TeraRaidRewardScreen_CreateListMenu(void)
{
    u32 i;
    for (i = 0; i < sTeraRaidRewardDropCount; i++)
    {
        CopyItemName(sTeraRaidRewardDrops[i].itemId, &sTeraRaidRewardScreenState->itemNames[i][0]);
        sTeraRaidRewardScreenState->listItems[i].name = &sTeraRaidRewardScreenState->itemNames[i][0];
        sTeraRaidRewardScreenState->listItems[i].id = i;
    }

    gMultiuseListMenuTemplate = sTeraRaidRewardMenu_ItemStorage;
    gMultiuseListMenuTemplate.windowId = WIN_UI_ITEM_LIST;
    gMultiuseListMenuTemplate.totalItems = sTeraRaidRewardDropCount;
    gMultiuseListMenuTemplate.items = sTeraRaidRewardScreenState->listItems;
    gMultiuseListMenuTemplate.maxShowed = 7;

    ListMenuInit(&gMultiuseListMenuTemplate, sTeraRaidRewardScreenState->itemsAbove, sTeraRaidRewardScreenState->cursorPos);
    if (sTeraRaidRewardDropCount > 7)
        TeraRaidRewardScreen_AddScrollIndicator();
}

static void TeraRaidRewardScreen_AddScrollIndicator(void)
{
    if (sTeraRaidRewardScreenState->scrollIndicatorTaskId == TASK_NONE)
        sTeraRaidRewardScreenState->scrollIndicatorTaskId = AddScrollIndicatorArrowPairParameterized(SCROLL_ARROW_UP, 178, 44, 148,
                                                                                                sTeraRaidRewardDropCount - 7,
                                                                                                TAG_SCROLL_ARROW,
                                                                                                TAG_SCROLL_ARROW,
                                                                                                &sTeraRaidRewardScreenState->itemsAbove);
}

static void TeraRaidRewardScreen_RemoveScrollIndicator(void)
{
    if (sTeraRaidRewardScreenState->scrollIndicatorTaskId != TASK_NONE)
    {
        RemoveScrollIndicatorArrowPair(sTeraRaidRewardScreenState->scrollIndicatorTaskId);
        sTeraRaidRewardScreenState->scrollIndicatorTaskId = TASK_NONE;
    }
}


void InitTeraRaidRewardScreen(void)
{
    CreateTask(Task_OpenTeraRaidRewardScreen_StartHere, 0);
}