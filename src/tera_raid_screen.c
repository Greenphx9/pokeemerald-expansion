#include "tera_raid_screen.h"

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
#include "constants/trainers.h"
#include "constants/pokemon.h"
#include "constants/abilities.h"

#include "data/tera_raids/tera_raid_partners.h"
#include "data/tera_raids/tera_raid_encounters.h"

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
 * 2) `Task_OpenSampleUi_Simple' waits for any active fades to finish, then it calls our init code in `TeraRaidScreen_Init'
 *    which changes the gMainCallback2 to our `TeraRaidScreen_SetupCB'.
 *
 * 3) `TeraRaidScreen_SetupCB' runs each frame, bit-by-bit getting our menu initialized. Once initialization has finished,
 *    this callback:
 *       1) Sets up a new task `Task_TeraRaidScreenWaitFadeIn' which waits until we fade back in before hotswapping itself for
 *          `Task_TeraRaidScreenMainInput' which reads input and updates the menu state.
 *       2) Starts a palette fade to bring the screen from black back to regular colors
 *       3) Sets our VBlank callback to `TeraRaidScreen_VBlankCB' (which is called every VBlank as part of the VBlank
 *          interrupt service routine). This callback transfers our OAM and palette buffers into VRAM, among other
 *          things
 *       4) Sets gMainCallback2 to our menu's main callback `TeraRaidScreen_MainCB', which does the standard processing of
 *          tasks, animating of sprites, etc.
 *
 * 4) We have reached our standard menu state. Every frame `TeraRaidScreen_MainCB' runs, which calls `Task_TeraRaidScreenMainInput`
 *    to get input from the user and update menu state and backing graphics buffers. `TeraRaidScreen_MainCB' also updates
 *    other important gamestate. Then, when VBlank occurs, our `TeraRaidScreen_VBlankCB' copies palettes and OAM into VRAM
 *    before pending DMA transfers fire and copy any screen graphics updates into VRAM.
 */

/*
 * Various state for the UI -- we'll allocate this on the heap since none of it needs to be preserved after we exit the
 * menu.
 */
struct TeraRaidScreenState
{
    // Save the callback to run when we exit: i.e. where do we want to go after closing the menu
    MainCallback savedCallback;
    // We will use this later to track some loading state
    u8 loadState;
    // Store the current dex mode, we'll have a mode that shows dex number/description as well as a few others
    u8 mode;
    // The sprite ID of the current mon icon, we need this so we can destroy the sprite when the user scrolls
    u8 monIconSpriteId;
    // The dex num of the currently displayed mon
    u16 monIconDexNum;

    u8 partnerIndexes[3];
    u8 chosenPartnerIndex;
    u8 partnerSpriteId[3];
    u8 partnerMonSpriteId[3][3];
    u8 arrowSpriteId;
    u8 arrowPos;
    u8 encounterSpriteId;
    bool8 outlinedSprite;
    u8 typeIconSpriteIds[2];
};

/*
 * Both of these can be pointers that live in EWRAM -- allocating the actual data on the heap will save precious WRAM
 * space since none of this data matters outside the context of our menu. We can easily cleanup when we're done. It's
 * worth noting that every time the game re-loads into the overworld, the heap gets nuked from orbit. However, it is
 * still good practice to clean up after oneself, so we will be sure to free everything before exiting.
 */
static EWRAM_DATA struct TeraRaidScreenState *sTeraRaidScreenState = NULL;
static EWRAM_DATA u8 *sBg1TilemapBuffer = NULL;
EWRAM_DATA struct TeraRaidMon gTeraRaidEncounter;
EWRAM_DATA u8 gTeraRaidStars;
EWRAM_DATA u8 gTeraRaidSelectedPartner;

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
    WIN_UI_RECOMMENDED_LEVEL,
    WIN_UI_BATTLE_ENDS,
    WIN_UI_CONTROLS,
    WIN_UI_AVAILABLE_PARTNERS,
};
static const struct WindowTemplate sTeraRaidScreenWindowTemplates[] =
{
    [WIN_UI_RECOMMENDED_LEVEL] =
    {
        .bg = 0,
        .tilemapLeft = 16,
        .tilemapTop = 0,
        .width = 16,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 1
    },
    [WIN_UI_BATTLE_ENDS] =
    {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 13,
        .width = 10,
        .height = 6,
        .paletteNum = 15,
        .baseBlock = 33
    },
    [WIN_UI_CONTROLS] =
    {
        .bg = 0,
        .tilemapLeft = 9,
        .tilemapTop = 18,
        .width = 23,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 93
    },
    [WIN_UI_AVAILABLE_PARTNERS] =
    {
        .bg = 0,
        .tilemapLeft = 16,
        .tilemapTop = 3,
        .width = 14,
        .height = 3,
        .paletteNum = 15,
        .baseBlock = 139,
    },
    DUMMY_WIN_TEMPLATE
};
/*
 * The tile file is generated from properly a indexed tile PNG image. You MUST use an indexed PNG with 4bpp indexing
 * (you can technically get away with 8bpp indexing as long as each individual index is between 0-15). The easiest way
 * to make indexed PNGs is using a program like GraphicsGale or Aseprite (in Index mode).
 */
static const u32 sTeraRaidScreenTiles[] = INCBIN_U32("graphics/tera_raid_screen/tera_raid_screen.4bpp.lz");

/*
 * I created this tilemap in TilemapStudio using the above tile PNG. I highly recommend TilemapStudio for exporting maps
 * like this.
 */
static const u32 sTeraRaidScreenTilemap[] = INCBIN_U32("graphics/tera_raid_screen/tera_raid_screen.bin.lz");

/*
 * This palette was built from a JASC palette file that you can export using GraphicsGale or Aseprite. Please note: the
 * palette conversion tool REQUIRES that JASC palette files end in CRLF, the standard Windows line ending. If you are
 * using the Mac/Linux version of a tool like Aseprite, you may get errors complaining that your lines end in LF and not
 * CRLF. To remedy this, run your JASC palette file through a tool like unix2dos and you shouldn't have any more
 * problems.
 */
static const u16 sTeraRaidScreenPalette[] = INCBIN_U16("graphics/tera_raid_screen/tera_raid_screen.gbapal");

#define GFXTAG_ARROWCURSOR 0x4000
#define PALTAG_ARROWCURSOR 0x4001

static const u32 sArrow_Gfx[]     = INCBIN_U32("graphics/tera_raid_screen/arrow.4bpp.lz");
static const u16 sArrow_Pal[]     = INCBIN_U16("graphics/tera_raid_screen/arrow.gbapal"); 

static const struct OamData sOamData_Arrow =
{
    .size = SPRITE_SIZE(32x32),
    .shape = SPRITE_SHAPE(32x32),
    .priority = 0,
};

static const struct CompressedSpriteSheet sSpriteSheet_Arrow =
{
    .data = sArrow_Gfx,
    .size = 32*32,
    .tag = GFXTAG_ARROWCURSOR,
};

static const struct SpritePalette sSpritePal_Arrow =
{
    .data = sArrow_Pal,
    .tag = PALTAG_ARROWCURSOR
};

static const union AnimCmd sSpriteAnim_Arrow[] =
{
    ANIMCMD_FRAME(0, 32),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd *const sSpriteAnimTable_Arrow[] =
{
    sSpriteAnim_Arrow,
};

static void SpriteCB_Arrow(struct Sprite* sprite);

static const struct SpriteTemplate sSpriteTemplate_Arrow =
{
    .tileTag = GFXTAG_ARROWCURSOR,
    .paletteTag = PALTAG_ARROWCURSOR,
    .oam = &sOamData_Arrow,
    .anims = sSpriteAnimTable_Arrow,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_Arrow
};

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

const u8 gTeraRaidStarToLevel[][2] = // fight & catch level
{
    [ONE_STAR]    = {12,  12},
    [TWO_STARS]   = {20,  20},
    [THREE_STARS] = {35,  35},
    [FOUR_STARS]  = {45,  45},
    [FIVE_STARS]  = {75,  75},
    [SIX_STARS]   = {90,  75},
    [SEVEN_STARS] = {100, 100},
};

// Callbacks for the sample UI
static void TeraRaidScreen_SetupCB(void);
static void TeraRaidScreen_MainCB(void);
static void TeraRaidScreen_VBlankCB(void);

// Sample UI tasks
static void Task_TeraRaidScreenWaitFadeIn(u8 taskId);
static void Task_TeraRaidScreenMainInput(u8 taskId);
static void Task_TeraRaidScreenWaitFadeAndBail(u8 taskId);
static void Task_TeraRaidScreenWaitFadeAndExitGracefully(u8 taskId);

// Sample UI helper functions
void TeraRaidScreen_Init(MainCallback callback);
static void TeraRaidScreen_ResetGpuRegsAndBgs(void);
static bool8 TeraRaidScreen_InitBgs(void);
static void TeraRaidScreen_FadeAndBail(void);
static bool8 TeraRaidScreen_LoadGraphics(void);
static void TeraRaidScreen_InitWindows(void);
static void TeraRaidScreen_FreeResources(void);
static void TeraRaidScreen_PrintWindowText(void);
static void TeraRaidScreen_LoadPartnerGfx(void);
static void TeraRaidscreen_LoadArrowGfx(void);
static void TeraRaidScreen_LoadEncounterGfx(void);
static void TeraRaidScreen_LoadTypesGfx(void);
static void TeraRaidScreen_LoadStarGfx(void);

static u32 GetRaidStars(void);
static void DetermineRaidEncounter(void);

// Declared in sample_ui.h
void Task_OpenSampleUi_StartHere(u8 taskId)
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
        TeraRaidScreen_Init(CB2_ReturnToFieldWithOpenMenu);
        // Our setup is done, so destroy ourself.
        DestroyTask(taskId);
    }
}

void TeraRaidScreen_Init(MainCallback callback)
{
    sTeraRaidScreenState = AllocZeroed(sizeof(struct TeraRaidScreenState));
    if (sTeraRaidScreenState == NULL)
    {
        /*
         * If the heap allocation failed for whatever reason, then set the callback to just return to the overworld.
         * This really shouldn't ever happen but if it does, this is better than hard crashing and making the user think
         * you bricked their Game Boy.
         */
        SetMainCallback2(callback);
        return;
    }

    sTeraRaidScreenState->loadState = 0;
    sTeraRaidScreenState->savedCallback = callback;

    /*
     * Next frame start running UI setup code. SetMainCallback2 also resets gMain.state to zero, which we will need for
     * the SetupCB
     */
    SetMainCallback2(TeraRaidScreen_SetupCB);
}

// Credit: Jaizu, pret
static void TeraRaidScreen_ResetGpuRegsAndBgs(void)
{
    SetGpuReg(REG_OFFSET_DISPCNT, 0);

    /*
     * Explicitly re-enable sprites. This isn't actually necessary if you aren't displaying sprites. However, let's do
     * this here anyway, otherwise you might be confused wondering why your sprites aren't showing up!
     *
     * For more information, see GBATEK's documentation on the Display Control register:
     *     https://problemkaputt.de/gbatek.htm#lcdiodisplaycontrol
     */
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_ON);

    SetGpuReg(REG_OFFSET_BG3CNT, 0);
    SetGpuReg(REG_OFFSET_BG2CNT, 0);
    SetGpuReg(REG_OFFSET_BG1CNT, 0);
    SetGpuReg(REG_OFFSET_BG0CNT, 0);
    ChangeBgX(0, 0, BG_COORD_SET);
    ChangeBgY(0, 0, BG_COORD_SET);
    ChangeBgX(1, 0, BG_COORD_SET);
    ChangeBgY(1, 0, BG_COORD_SET);
    ChangeBgX(2, 0, BG_COORD_SET);
    ChangeBgY(2, 0, BG_COORD_SET);
    ChangeBgX(3, 0, BG_COORD_SET);
    ChangeBgY(3, 0, BG_COORD_SET);
    SetGpuReg(REG_OFFSET_BLDCNT, 0);
    SetGpuReg(REG_OFFSET_BLDY, 0);
    SetGpuReg(REG_OFFSET_BLDALPHA, 0);
    SetGpuReg(REG_OFFSET_WIN0H, 0);
    SetGpuReg(REG_OFFSET_WIN0V, 0);
    SetGpuReg(REG_OFFSET_WIN1H, 0);
    SetGpuReg(REG_OFFSET_WIN1V, 0);
    SetGpuReg(REG_OFFSET_WININ, 0);
    SetGpuReg(REG_OFFSET_WINOUT, 0);
    CpuFill16(0, (void *)VRAM, VRAM_SIZE);
    CpuFill32(0, (void *)OAM, OAM_SIZE);
}

static void TeraRaidScreen_SetupCB(void)
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
        //TeraRaidScreen_ResetGpuRegsAndBgs();
        // Use DMA to completely clear VRAM
        DmaClearLarge16(3, (void *)VRAM, VRAM_SIZE, 0x1000);
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
        if (TeraRaidScreen_InitBgs())
        {
            // If we successfully init the BGs, we can move on
            sTeraRaidScreenState->loadState = 0;
            gMain.state++;
        }
        else
        {
            /*
             * Otherwise, fade out, free the heap data, and return to main menu. Like before, this shouldn't ever really
             * happen but it's better to handle it then have a surprise hard-crash.
             */
            TeraRaidScreen_FadeAndBail();
            return;
        }
        break;
    case 3:
        // `TeraRaidScreen_LoadGraphics' has its own giant switch statement, so keep calling until it returns TRUE at the end
        if (TeraRaidScreen_LoadGraphics() == TRUE)
        {
            // Only advance the state of this load switch statment once all the LoadGraphics logic has finished.
            gMain.state++;
        }
        break;
    case 4:
        // Set up our text windows
        TeraRaidScreen_InitWindows();
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
        LoadMonIconPalettes();
        DetermineRaidEncounter();
        gTeraRaidStars = GetRaidStars();
        TeraRaidScreen_PrintWindowText();
        TeraRaidscreen_LoadArrowGfx();
        TeraRaidScreen_LoadPartnerGfx();
        TeraRaidScreen_LoadEncounterGfx();
        TeraRaidScreen_LoadTypesGfx();
        TeraRaidScreen_LoadStarGfx();
        // Create a task that does nothing until the palette fade is done. We will start the palette fade next frame.
        CreateTask(Task_TeraRaidScreenWaitFadeIn, 0);
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
        SetVBlankCallback(TeraRaidScreen_VBlankCB);
        SetMainCallback2(TeraRaidScreen_MainCB);
        break;
    }
}

static void TeraRaidScreen_MainCB(void)
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

static void TeraRaidScreen_VBlankCB(void)
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

static void Task_TeraRaidScreenWaitFadeIn(u8 taskId)
{
     // Do nothing until the palette fade finishes, then replace ourself with the main menu task.
    if (!gPaletteFade.active)
    {
        gTasks[taskId].func = Task_TeraRaidScreenMainInput;
    }
}

static void Task_TeraRaidScreenMainInput(u8 taskId)
{
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_PC_ON);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTeraRaidSelectedPartner = sTeraRaidScreenState->partnerIndexes[sTeraRaidScreenState->arrowPos];
        gTasks[taskId].func = Task_TeraRaidScreenWaitFadeAndExitGracefully;
    }
    // Exit the menu when the player presses B
    if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_PC_OFF);
        // Fade screen to black
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        // Replace ourself with the "exit gracefully" task function
        gTasks[taskId].func = Task_TeraRaidScreenWaitFadeAndExitGracefully;
    }
    if (JOY_NEW(DPAD_DOWN))
    {
        PlaySE(SE_SELECT);
        if (sTeraRaidScreenState->arrowPos == 2)
            sTeraRaidScreenState->arrowPos = 0;
        else
            sTeraRaidScreenState->arrowPos++;
    }
    if (JOY_NEW(DPAD_UP))
    {
        PlaySE(SE_SELECT);
        if (sTeraRaidScreenState->arrowPos == 0)
            sTeraRaidScreenState->arrowPos = 2;
        else
            sTeraRaidScreenState->arrowPos--;
    }
    if (JOY_NEW(START_BUTTON))
    {
        PlaySE(SE_PC_ON);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        sTeraRaidScreenState->arrowPos = Random() % 3;
        gTeraRaidSelectedPartner = sTeraRaidScreenState->partnerIndexes[sTeraRaidScreenState->arrowPos];
        gTasks[taskId].func = Task_TeraRaidScreenWaitFadeAndExitGracefully;
    }
}

static void SpriteCB_Arrow(struct Sprite* sprite)
{
    sprite->y = 59 + (sTeraRaidScreenState->arrowPos * 33);

	if (sprite->data[1])
	{
		sprite->data[0] -= 1;
		if (sprite->data[0] == 0)
			sprite->data[1] = FALSE;
	}
	else
	{
		sprite->data[0] += 1;
		if (sprite->data[0] == 20)
			sprite->data[1] = TRUE;
	}

	sprite->x2 = 5 - sprite->data[0] / 4;
}

static void Task_TeraRaidScreenWaitFadeAndBail(u8 taskId)
{
    // Wait until the screen fades to black before we start doing cleanup
    if (!gPaletteFade.active)
    {
        SetMainCallback2(sTeraRaidScreenState->savedCallback);
        TeraRaidScreen_FreeResources();
        DestroyTask(taskId);
    }
}

static void Task_TeraRaidScreenWaitFadeAndExitGracefully(u8 taskId)
{
    /*
     * This function is basically the same as Task_TeraRaidScreenWaitFadeAndBail. However, for this sample we broke it out
     * because typically you might want to do something different if the user gracefully exits a menu vs if you got
     * booted from a menu due to heap allocation failure.
     */

    // E.g. you could do some other processing here

    // Wait until the screen fades to black before we start doing final cleanup
    if (!gPaletteFade.active)
    {
        SetMainCallback2(CB2_ReturnToFieldContinueScript);
        TeraRaidScreen_FreeResources();
        DestroyTask(taskId);
    }
}

static bool8 TeraRaidScreen_InitBgs(void)
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

static void TeraRaidScreen_FadeAndBail(void)
{
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    CreateTask(Task_TeraRaidScreenWaitFadeAndBail, 0);

    /*
     * Set callbacks to ours while we wait for the fade to finish, then our above task will cleanup and swap the
     * callbacks back to the one we saved earlier (which should re-load the overworld)
     */
    SetVBlankCallback(TeraRaidScreen_VBlankCB);
    SetMainCallback2(TeraRaidScreen_MainCB);
}

static bool8 TeraRaidScreen_LoadGraphics(void)
{
    switch (sTeraRaidScreenState->loadState)
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
        DecompressAndCopyTileDataToVram(1, sTeraRaidScreenTiles, 0, 0, 0);
        sTeraRaidScreenState->loadState++;
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
            LZDecompressWram(sTeraRaidScreenTilemap, sBg1TilemapBuffer);
            sTeraRaidScreenState->loadState++;
        }
        break;
    case 2:
        /*
         * Copy our palette into the game's BG palette buffer, slot 0 -- this step does not directly get the palette
         * into VRAM. That only happens during VBlank if the current callback specifies a buffer transfer.
         */
        LoadPalette(sTeraRaidScreenPalette, BG_PLTT_ID(0), PLTT_SIZE_4BPP);
        /*
         * Copy the message box palette into BG palette buffer, slot 15. Our window is set to use palette 15 and our
         * text color constants are defined assuming we are indexing into this palette.
         */
        LoadPalette(gMessageBox_Pal, BG_PLTT_ID(15), PLTT_SIZE_4BPP);
        sTeraRaidScreenState->loadState++;
    default:
        sTeraRaidScreenState->loadState = 0;
        return TRUE;
    }
    return FALSE;
}

static void TeraRaidScreen_InitWindows(void)
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
    FillWindowPixelBuffer(WIN_UI_RECOMMENDED_LEVEL, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    FillWindowPixelBuffer(WIN_UI_BATTLE_ENDS, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    FillWindowPixelBuffer(WIN_UI_CONTROLS, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    FillWindowPixelBuffer(WIN_UI_AVAILABLE_PARTNERS, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));

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
    PutWindowTilemap(WIN_UI_RECOMMENDED_LEVEL);
    PutWindowTilemap(WIN_UI_BATTLE_ENDS);
    PutWindowTilemap(WIN_UI_CONTROLS);
    PutWindowTilemap(WIN_UI_AVAILABLE_PARTNERS);

    /*
     * Copy (well, schedule to copy) each window into VRAM using DMA3 under the hood. The COPYWIN_FULL argument means we
     * copy BOTH the tilemap buffer (which recall is just an incrementing sequence of references into our tile canvas)
     * and the tiles themselves. Typically when updating text on a window, you only need to copy the tile canvas (i.e.
     * using COPYWIN_GFX) since the tilemap should never change. But to init the window we need to get both into VRAM.
     */
    CopyWindowToVram(WIN_UI_RECOMMENDED_LEVEL, COPYWIN_FULL);
    CopyWindowToVram(WIN_UI_BATTLE_ENDS, COPYWIN_FULL);
    CopyWindowToVram(WIN_UI_CONTROLS, COPYWIN_FULL);
    CopyWindowToVram(WIN_UI_AVAILABLE_PARTNERS, COPYWIN_FULL);
}

static void TeraRaidScreen_FreeResources(void)
{
    // Free our data struct and our BG1 tilemap buffer
    if (sTeraRaidScreenState != NULL)
    {
        Free(sTeraRaidScreenState);
    }
    if (sBg1TilemapBuffer != NULL)
    {
        Free(sBg1TilemapBuffer);
    }
    // Free all allocated tilemap and pixel buffers associated with the windows.
    FreeAllWindowBuffers();
    // Reset all sprite data
    ResetSpriteData();
}

static const u8 sText_RecommendedLevel[] = _("Recommended Level: ");
static const u8 sText_BattleEnds[] = _("Battle ends if:\n4 Pokémon faint\n10 turns pass");
static const u8 sText_Controls[] = _("{DPAD_UPDOWN}Pick {A_BUTTON}Choose {START_BUTTON}Random {B_BUTTON}Cancel");
static const u8 sText_AvailablePartners[] = _("Available Partners");
static void TeraRaidScreen_PrintWindowText(void)
{
	StringCopy(gStringVar1, sText_RecommendedLevel);
	ConvertIntToDecimalStringN(gStringVar2, gTeraRaidStarToLevel[gTeraRaidStars][0], 0, 3);
	StringAppend(gStringVar1, gStringVar2);
    AddTextPrinterParameterized4(WIN_UI_RECOMMENDED_LEVEL, FONT_SMALL, 2, 0, 0, 0, sTeraRaidScreenWindowFontColors[FONT_WHITE], TEXT_SKIP_DRAW, gStringVar1);
    AddTextPrinterParameterized4(WIN_UI_BATTLE_ENDS, FONT_SMALL, 0, 2, 2, 1, sTeraRaidScreenWindowFontColors[FONT_WHITE], TEXT_SKIP_DRAW, sText_BattleEnds);
    AddTextPrinterParameterized4(WIN_UI_CONTROLS, FONT_SMALL, 0, 4, 0, 0, sTeraRaidScreenWindowFontColors[FONT_WHITE], TEXT_SKIP_DRAW, sText_Controls);
    AddTextPrinterParameterized4(WIN_UI_AVAILABLE_PARTNERS, FONT_NORMAL, 1, 3, 0, 0, sTeraRaidScreenWindowFontColors[FONT_BLACK], TEXT_SKIP_DRAW, sText_AvailablePartners);
    CopyWindowToVram(WIN_UI_RECOMMENDED_LEVEL, COPYWIN_GFX);
    CopyWindowToVram(WIN_UI_BATTLE_ENDS, COPYWIN_GFX);
    CopyWindowToVram(WIN_UI_CONTROLS, COPYWIN_GFX);
    CopyWindowToVram(WIN_UI_AVAILABLE_PARTNERS, COPYWIN_GFX);
}

#define OW_ICON_X 126
#define MON_ICON_1_X 136
#define MON_ICON_2_X 168
#define MON_ICON_3_X 200

// This is borrowed from Skeli's Dynamax Raids
static u32 GetRaidRandomNumber(void)
{
	//Make sure no values are 0
	u8 dayOfWeek = (RtcGetDayOfWeek() == 0) ? 8 : RtcGetDayOfWeek();
	u8 hour = (RtcGetHour() == 0) ? 24 : RtcGetHour();
	u8 day = (RtcGetDay() == 0) ? 32 : RtcGetDay();
	u8 month = (RtcGetMonth() == 0) ? 13 : RtcGetMonth();
	u8 lastMapGroup = (gSaveBlock1Ptr->dynamicWarp.mapGroup == 0) ? 0xFF : gSaveBlock1Ptr->dynamicWarp.mapGroup;
	u8 lastMapNum = (gSaveBlock1Ptr->dynamicWarp.mapNum == 0) ? 0xFF : gSaveBlock1Ptr->dynamicWarp.mapNum;
	u8 lastWarpId = (gSaveBlock1Ptr->dynamicWarp.warpId == 0) ? 0xFF : gSaveBlock1Ptr->dynamicWarp.warpId;
	u16 lastPos = (gSaveBlock1Ptr->dynamicWarp.x + gSaveBlock1Ptr->dynamicWarp.y == 0) ? 0xFFFF : (u16) (gSaveBlock1Ptr->dynamicWarp.x + gSaveBlock1Ptr->dynamicWarp.y);
	#ifdef VAR_RAID_NUMBER_OFFSET
	u16 offset = VarGet(VAR_RAID_NUMBER_OFFSET); //Setting this var changes all the raid spawns for the current hour (helps with better Wishing Piece)
	#else
	u16 offset = 0;
	#endif

	return ((hour * (day + month) * lastMapGroup * (lastMapNum + lastWarpId + lastPos)) + ((hour * (day + month)) ^ dayOfWeek) + offset) ^ T1_READ_32(gSaveBlock2Ptr->playerTrainerId);
}

// source: https://bulbapedia.bulbagarden.net/wiki/Tera_Raid_Battle#Encounters
static u32 GetRaidStars(void)
{
    u8 badgeCount = 0;
    u32 i;
    u32 rand = GetRaidRandomNumber() % 100;

    for (i = FLAG_BADGE01_GET; i < FLAG_BADGE01_GET + NUM_BADGES; i++)
    {
        if (FlagGet(i))
            badgeCount++;
    }


    if (FlagGet(FLAG_SYS_GAME_CLEAR))
    {
        if (rand <= 40)
            return THREE_STARS;
        else if (rand <= 75)
            return FOUR_STARS;
        else 
            return FIVE_STARS;
    }
    else if (badgeCount >= 6)
    {
        if (rand <= 20)
            return ONE_STAR;
        else if (rand <= 40)
            return TWO_STARS;
        else if (rand <= 70)
            return THREE_STARS;
        else
            return FOUR_STARS;   
    }
    else if (badgeCount >= 3)
    {
        if (rand <= 30)
            return ONE_STAR;
        else if (rand <= 40)
            return TWO_STARS;
        else 
            return THREE_STARS;
    }
    else
        if (rand <= 80)
            return ONE_STAR;
        else
            return TWO_STARS;
}

// Based off Skeli's Dynamax Raids but not directly copied
void DetermineRaidPartners(u32* goodIndexes, u8 maxPartners)
{
    u32 i, j, k, foundPartners = 0;
    u32 randNum = GetRaidRandomNumber();
    bool8 checkedPartners[ARRAY_COUNT(gTeraRaidPartners)];
    for (k = 0; k < ARRAY_COUNT(checkedPartners); k++)
        checkedPartners[k] = FALSE;
    for (i = 0; i < 0xFFFF; i++)
    {
        if (randNum == 0)
            randNum = 0xFFFF;
        randNum ^= i;
        j = randNum % ARRAY_COUNT(gTeraRaidPartners);
        if (!checkedPartners[j])
        {
            if (gTeraRaidPartners[j].parties[gTeraRaidStars] != NULL)
            {
                checkedPartners[j] = TRUE;
                goodIndexes[foundPartners] = j;
                foundPartners++;
            }
        }
        if (foundPartners >= maxPartners)
            break;
    }
}


static void TeraRaidScreen_LoadPartnerGfx(void)
{
    u32 i, j;
    u32 indexes[3];
    for (i = 0; i < 3; i++)
        indexes[i] = 0xFFFF;
    DetermineRaidPartners(indexes, 3);
    for (i = 0; i < 3; i++)
    {
        u32 index = indexes[i];
        sTeraRaidScreenState->partnerIndexes[i] = index;
        sTeraRaidScreenState->partnerSpriteId[i] = CreateObjectGraphicsSprite(gTeraRaidPartners[index].objectEventGfx, SpriteCallbackDummy, 126, 59 + (i * 33), 0);
        for (j = 0; j < 3; j++)
        {
            sTeraRaidScreenState->partnerMonSpriteId[i][j] = CreateMonIconNoPersonality(gTeraRaidPartners[index].parties[0][j].species, SpriteCB_MonIcon, 158 + (32 * j), 59 + (i * 33), 1);
        }
    }
}

static void TeraRaidscreen_LoadArrowGfx(void)
{
    LoadCompressedSpriteSheet(&sSpriteSheet_Arrow);
    LoadSpritePalette(&sSpritePal_Arrow);    
    sTeraRaidScreenState->arrowSpriteId = CreateSprite(&sSpriteTemplate_Arrow, 95, 59, 0);
}

static void DetermineRaidEncounter(void)
{
    u32 playerMapSec = gMapHeader.regionMapSectionId;
    const struct TeraRaid* teraRaid = &sTeraRaidsByMapSec[playerMapSec][gTeraRaidStars];
    gTeraRaidEncounter = teraRaid->mons[GetRaidRandomNumber() % (teraRaid->amount-1)];
}

// credits to AgustinGDLV
static void TeraRaidScreen_LoadEncounterGfx(void)
{
    u32 i, j, paletteOffset, spriteId;
	sTeraRaidScreenState->encounterSpriteId  = CreateMonPicSprite(gTeraRaidEncounter.species, FALSE, 0xFFFFFFFF, TRUE, 45, 57, 14, TAG_NONE);
    gSprites[sTeraRaidScreenState->encounterSpriteId].oam.priority = 0;

	paletteOffset = 14 * 16 + 0x100;
    BlendPalette(paletteOffset, 16, 16, RGB(4, 4, 4));
    CpuCopy32(gPlttBufferFaded + paletteOffset, gPlttBufferUnfaded + paletteOffset, 32);

	// Create white outline.
    // TODO: Nicer looking way to do this with fancy VRAM trick, see CFRU.
    for (i = 0; i < 2; i++)
    {
        for (j = 0; j < 2; j++)
        {
            spriteId = CreateMonPicSprite(gTeraRaidEncounter.species, FALSE, 0xFFFFFFFF, TRUE, 44 + i*2, 56 + j*2, gSprites[sTeraRaidScreenState->encounterSpriteId].oam.paletteNum + 1, TAG_NONE);
            gSprites[spriteId].oam.priority = 1;
        }
    }
    paletteOffset += 16;
    FillPalette(RGB_WHITE, paletteOffset, 32);
    CpuCopy32(gPlttBufferFaded + paletteOffset, gPlttBufferUnfaded + paletteOffset, 32);
}

//Type Icon
static void SetSpriteInvisibility(u8 spriteArrayId, bool8 invisible)
{
    gSprites[sTeraRaidScreenState->typeIconSpriteIds[spriteArrayId]].invisible = invisible;
}

static void SetTypeIconPosAndPal(u8 typeId, u8 x, u8 y, u8 spriteArrayId)
{
    struct Sprite *sprite;

    sprite = &gSprites[sTeraRaidScreenState->typeIconSpriteIds[spriteArrayId]];
    StartSpriteAnim(sprite, typeId);
    sprite->oam.paletteNum = gTypesInfo[typeId].palette - 2;
    sprite->x = x + 16;
    sprite->y = y + 8;
    SetSpriteInvisibility(spriteArrayId, FALSE);
}

static void TeraRaidScreen_LoadTypesGfx(void)
{
    u32 i;
    u16 species = gTeraRaidEncounter.species;
    u8 type1 = gSpeciesInfo[species].types[0], type2 = gSpeciesInfo[species].types[1];
    sTeraRaidScreenState->typeIconSpriteIds[0] = 0xFF;
    sTeraRaidScreenState->typeIconSpriteIds[1] = 0xFF;
    LoadCompressedPalette(gMoveTypes_Pal, OBJ_PLTT_ID(11), 3 * PLTT_SIZE_4BPP);
    LoadCompressedSpriteSheet(&gSpriteSheet_MoveTypes);
    for (i = 0; i < 2; i++)
    {
        if (sTeraRaidScreenState->typeIconSpriteIds[i] == 0xFF)
            sTeraRaidScreenState->typeIconSpriteIds[i] = CreateSprite(&gSpriteTemplate_MoveTypes, 10, 10, 2);

        SetSpriteInvisibility(i, TRUE);
    }
    SetTypeIconPosAndPal(type1, 64, 0, 0);
    if (type2 != type1)
        SetTypeIconPosAndPal(type2, 97, 0, 1);
}

static void TeraRaidScreen_LoadStarGfx(void)
{
    LoadCompressedSpriteSheet(&sSpriteSheet_Star);
    LoadSpritePalette(&sSpritePal_Star); 
    for (int i = 0; i < gTeraRaidStars + 1; ++i)
		CreateSprite(&sSpriteTemplate_Star, 6 + (9 * i), 8, 0);
}

// specials
void IsRaidAvailable(void)
{
    const struct TeraRaid *teraRaid = &sTeraRaidsByMapSec[gMapHeader.regionMapSectionId][GetRaidStars()];
    gSpecialVar_Result = teraRaid->mons != NULL;
}

// assumes you have called IsRaidAvailable
void InitTeraRaidScreen(void)
{
    CreateTask(Task_OpenSampleUi_StartHere, 0);
}