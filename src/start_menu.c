#include "global.h"
#include "battle_pike.h"
#include "battle_pyramid.h"
#include "battle_pyramid_bag.h"
#include "bg.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "event_object_lock.h"
#include "event_scripts.h"
#include "decompress.h"
#include "fieldmap.h"
#include "field_effect.h"
#include "field_player_avatar.h"
#include "field_specials.h"
#include "field_weather.h"
#include "field_screen_effect.h"
#include "frontier_pass.h"
#include "frontier_util.h"
#include "gpu_regs.h"
#include "international_string_util.h"
#include "item_menu.h"
#include "link.h"
#include "load_save.h"
#include "main.h"
#include "menu.h"
#include "new_game.h"
#include "option_menu.h"
#include "overworld.h"
#include "palette.h"
#include "party_menu.h"
#include "pokedex.h"
#include "pokenav.h"
#include "safari_zone.h"
#include "save.h"
#include "scanline_effect.h"
#include "script.h"
#include "sprite.h"
#include "sound.h"
#include "start_menu.h"
#include "strings.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "text_window.h"
#include "trainer_card.h"
#include "window.h"
#include "union_room.h"
#include "constants/battle_frontier.h"
#include "constants/rgb.h"
#include "constants/songs.h"

// Menu actions
enum
{
    MENU_ACTION_POKEDEX,
    MENU_ACTION_POKEMON,
    MENU_ACTION_BAG,
    MENU_ACTION_POKENAV,
    MENU_ACTION_PLAYER,
    MENU_ACTION_SAVE,
    MENU_ACTION_OPTION,
    MENU_ACTION_EXIT,
    MENU_ACTION_RETIRE_SAFARI,
    MENU_ACTION_PLAYER_LINK,
    MENU_ACTION_REST_FRONTIER,
    MENU_ACTION_RETIRE_FRONTIER,
    MENU_ACTION_PYRAMID_BAG
};

// Save status
enum
{
    SAVE_IN_PROGRESS,
    SAVE_SUCCESS,
    SAVE_CANCELED,
    SAVE_ERROR
};

// IWRAM common
bool8 (*gMenuCallback)(void);

// EWRAM
EWRAM_DATA static u8 sSafariBallsWindowId = 0;
EWRAM_DATA static u8 sBattlePyramidFloorWindowId = 0;
EWRAM_DATA static u8 sStartMenuCursorPos = 0;
EWRAM_DATA static u8 sNumStartMenuActions = 0;
EWRAM_DATA static u8 sCurrentStartMenuActions[9] = {0};
EWRAM_DATA static s8 sInitStartMenuData[2] = {0};

EWRAM_DATA static u8 (*sSaveDialogCallback)(void) = NULL;
EWRAM_DATA static u8 sSaveDialogTimer = 0;
EWRAM_DATA static bool8 sSavingComplete = FALSE;
EWRAM_DATA static u8 sSaveInfoWindowId = 0;
EWRAM_DATA static u8 sSelectorSpriteIds[2];
EWRAM_DATA static u8 sSpriteIds[8];
EWRAM_DATA static u8 sSpriteIdCount;
EWRAM_DATA static u8 sLastY;

// Menu action callbacks
static bool8 StartMenuPokedexCallback(void);
static bool8 StartMenuPokemonCallback(void);
static bool8 StartMenuBagCallback(void);
static bool8 StartMenuPokeNavCallback(void);
static bool8 StartMenuPlayerNameCallback(void);
static bool8 StartMenuSaveCallback(void);
static bool8 StartMenuOptionCallback(void);
static bool8 StartMenuExitCallback(void);
static bool8 StartMenuSafariZoneRetireCallback(void);
static bool8 StartMenuLinkModePlayerNameCallback(void);
static bool8 StartMenuBattlePyramidRetireCallback(void);
static bool8 StartMenuBattlePyramidBagCallback(void);

// Menu callbacks
static bool8 SaveStartCallback(void);
static bool8 SaveCallback(void);
static bool8 BattlePyramidRetireStartCallback(void);
static bool8 BattlePyramidRetireReturnCallback(void);
static bool8 BattlePyramidRetireCallback(void);
static bool8 HandleStartMenuInput(void);

// Save dialog callbacks
static u8 SaveConfirmSaveCallback(void);
static u8 SaveYesNoCallback(void);
static u8 SaveConfirmInputCallback(void);
static u8 SaveFileExistsCallback(void);
static u8 SaveConfirmOverwriteDefaultNoCallback(void);
static u8 SaveConfirmOverwriteCallback(void);
static u8 SaveOverwriteInputCallback(void);
static u8 SaveSavingMessageCallback(void);
static u8 SaveDoSaveCallback(void);
static u8 SaveSuccessCallback(void);
static u8 SaveReturnSuccessCallback(void);
static u8 SaveErrorCallback(void);
static u8 SaveReturnErrorCallback(void);
static u8 BattlePyramidConfirmRetireCallback(void);
static u8 BattlePyramidRetireYesNoCallback(void);
static u8 BattlePyramidRetireInputCallback(void);

// Task callbacks
static void StartMenuTask(u8 taskId);
static void SaveGameTask(u8 taskId);
static void Task_SaveAfterLinkBattle(u8 taskId);
static void Task_WaitForBattleTowerLinkSave(u8 taskId);
static bool8 FieldCB_ReturnToFieldStartMenu(void);

// NEW!
// Config
#define DPPT_START_MENU_DISABLE_EXIT TRUE // This is required to fit everything on screen if every action is on screen at once.
                                          // While multi-page start screen implementation is not included here, it should be pretty simple to make your own compatible with this menu.

// Sprites
// A lot of this sprite defining code is referenced from Voluptua's start menu, credits to him: https://github.com/Voluptua/pokeemerald/blob/start_menu_1/src/heat_start_menu.c
static void SpriteCB_Pokedex(struct Sprite *sprite);
static void SpriteCB_Pokeball(struct Sprite *sprite);
static void SpriteCB_Bag(struct Sprite *sprite);
static void SpriteCB_Pokenav(struct Sprite *sprite);
static void SpriteCB_TrainerCard(struct Sprite *sprite);
static void SpriteCB_Save(struct Sprite *sprite);
static void SpriteCB_Options(struct Sprite *sprite);
static void SpriteCB_Cancel(struct Sprite *sprite);
static void SpriteCB_Retire(struct Sprite *sprite);
static void LoadStartMenuGfx(void);
static void DestroyStartMenuGfx(void);

#define TAG_SELECTOR_GFX           1251
#define TAG_POKEBALL_GFX           1252
#define TAG_POKEDEX_GFX            1253
#define TAG_BAG_GFX                1254
#define TAG_POKENAV_GFX            1255
#define TAG_TRAINER_CARD_GFX       1256
#define TAG_SAVE_GFX               1257
#define TAG_OPTIONS_GFX            1258
#define TAG_CANCEL_GFX             1259
#define TAG_RETIRE_GFX             1260
#define TAG_MENU_PAL               0x4650

static const u32 sSelector_Gfx[] = INCBIN_U32("graphics/dppt_start_menu/selector.4bpp.lz");
static const u16 sMenu_Pal[] = INCBIN_U16("graphics/dppt_start_menu/menu.gbapal");

static const u32 sPokeball_Gfx[] = INCBIN_U32("graphics/dppt_start_menu/pokeball.4bpp.lz");
static const u32 sPokedex_Gfx[] = INCBIN_U32("graphics/dppt_start_menu/pokedex.4bpp.lz");
static const u32 sBag_Gfx[] = INCBIN_U32("graphics/dppt_start_menu/bag.4bpp.lz");
static const u32 sPokenav_Gfx[] = INCBIN_U32("graphics/dppt_start_menu/pokenav.4bpp.lz");
static const u32 sTrainerCard_Gfx[] = INCBIN_U32("graphics/dppt_start_menu/trainer_card.4bpp.lz");
static const u32 sSave_Gfx[] = INCBIN_U32("graphics/dppt_start_menu/save.4bpp.lz");
static const u32 sOptions_Gfx[] = INCBIN_U32("graphics/dppt_start_menu/options.4bpp.lz");
static const u32 sCancel_Gfx[] = INCBIN_U32("graphics/dppt_start_menu/cancel.4bpp.lz");
static const u32 sRetire_Gfx[] = INCBIN_U32("graphics/dppt_start_menu/retire.4bpp.lz");

static const struct OamData sOamData_Selector =
{
    .size = SPRITE_SIZE(64x32),
    .shape = SPRITE_SHAPE(64x32),
    .priority = 0,
};

static const struct CompressedSpriteSheet sSpriteSheet_Selector =
{
    .data = sSelector_Gfx,
    .size = 64*32*4/2,
    .tag = TAG_SELECTOR_GFX,
};

static const struct SpritePalette sSpritePal_Selector =
{
    .data = sMenu_Pal,
    .tag = TAG_MENU_PAL
};

static const union AnimCmd sSpriteAnim_Selector[] =
{
    ANIMCMD_FRAME(0, 32),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd *const sSpriteAnimTable_Selector[] =
{
    sSpriteAnim_Selector,
};

static const struct SpriteTemplate sSpriteTemplate_Selector =
{
    .tileTag = TAG_SELECTOR_GFX,
    .paletteTag = TAG_MENU_PAL,
    .oam = &sOamData_Selector,
    .anims = sSpriteAnimTable_Selector,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

static const struct CompressedSpriteSheet sSpriteSheet_PokeballIcon[] = 
{
    {sPokeball_Gfx, 32*64/2 , TAG_POKEBALL_GFX},
    {NULL},
};

static const struct CompressedSpriteSheet sSpriteSheet_PokedexIcon[] = 
{
    {sPokedex_Gfx, 32*64/2 , TAG_POKEDEX_GFX},
    {NULL},
};

static const struct CompressedSpriteSheet sSpriteSheet_BagIcon[] = 
{
    {sBag_Gfx, 32*64/2 , TAG_BAG_GFX},
    {NULL},
};

static const struct CompressedSpriteSheet sSpriteSheet_PokenavIcon[] = 
{
    {sPokenav_Gfx, 32*64/2 , TAG_POKENAV_GFX},
    {NULL},
};

static const struct CompressedSpriteSheet sSpriteSheet_TrainerCardIcon[] = 
{
    {sTrainerCard_Gfx, 32*64/2 , TAG_TRAINER_CARD_GFX},
    {NULL},
};

static const struct CompressedSpriteSheet sSpriteSheet_SaveIcon[] = 
{
    {sSave_Gfx, 32*64/2 , TAG_SAVE_GFX},
    {NULL},
};

static const struct CompressedSpriteSheet sSpriteSheet_OptionsIcon[] = 
{
    {sOptions_Gfx, 32*64/2 , TAG_OPTIONS_GFX},
    {NULL},
};

static const struct CompressedSpriteSheet sSpriteSheet_CancelIcon[] = 
{
    {sCancel_Gfx, 32*64/2 , TAG_CANCEL_GFX},
    {NULL},
};

static const struct CompressedSpriteSheet sSpriteSheet_RetireIcon[] = 
{
    {sRetire_Gfx, 32*64/2 , TAG_RETIRE_GFX},
    {NULL},
};

static const struct OamData sOamData_Icon = {
    .y = 0,
    .affineMode = ST_OAM_AFFINE_DOUBLE,
    .objMode = 0,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x32),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(32x32),
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
};

static const union AnimCmd sSpriteAnim_Icon_Grey[] = {
    ANIMCMD_FRAME(16, 0),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sSpriteAnim_Icon_Default[] = {
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd *const sSpriteAnimTable_Icon[] = {
    sSpriteAnim_Icon_Grey,
    sSpriteAnim_Icon_Default,
};

static const union AffineAnimCmd sSpriteAffineAnim_IconNone[] =
{
    AFFINEANIMCMD_FRAME(0, 0, 0, 60),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sSpriteAffineAnim_Icon[] =
{
    AFFINEANIMCMD_FRAME(12, 12, 0, 8),
    AFFINEANIMCMD_FRAME(-12, -12, 0, 8),
    // This is taken from Voluptua's start menu, huge credits to him: https://github.com/Voluptua/pokeemerald/blob/start_menu_1/src/heat_start_menu.c#L358
    AFFINEANIMCMD_FRAME(0, 0, 1, 4),      // Begin rotating

    AFFINEANIMCMD_FRAME(0, 0, -1, 4),     // Loop starts from here ; Rotate/Tilt left 
    AFFINEANIMCMD_FRAME(0, 0, 0, 2),
    AFFINEANIMCMD_FRAME(0, 0, -1, 4),
    AFFINEANIMCMD_FRAME(0, 0, 0, 2),
    AFFINEANIMCMD_FRAME(0, 0, -1, 4),

    AFFINEANIMCMD_FRAME(0, 0, 1, 4),      // Rotate/Tilt Right
    AFFINEANIMCMD_FRAME(0, 0, 0, 2),
    AFFINEANIMCMD_FRAME(0, 0, 1, 4),
    AFFINEANIMCMD_FRAME(0, 0, 0, 2),
    AFFINEANIMCMD_FRAME(0, 0, 1, 4),

    AFFINEANIMCMD_JUMP(3),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd *const sAffineAnimCmds_Icon[] =
{   
    sSpriteAffineAnim_IconNone,
    sSpriteAffineAnim_Icon,
};

static const struct SpriteTemplate sSpriteTemplate_Pokeball = {
    .tileTag = TAG_POKEBALL_GFX,
    .paletteTag = TAG_MENU_PAL,
    .oam = &sOamData_Icon,
    .anims = sSpriteAnimTable_Icon,
    .images = NULL,
    .affineAnims = sAffineAnimCmds_Icon,
    .callback = SpriteCB_Pokeball,
};

static const struct SpriteTemplate sSpriteTemplate_Pokedex = {
    .tileTag = TAG_POKEDEX_GFX,
    .paletteTag = TAG_MENU_PAL,
    .oam = &sOamData_Icon,
    .anims = sSpriteAnimTable_Icon,
    .images = NULL,
    .affineAnims = sAffineAnimCmds_Icon,
    .callback = SpriteCB_Pokedex,
};

static const struct SpriteTemplate sSpriteTemplate_Bag = {
    .tileTag = TAG_BAG_GFX,
    .paletteTag = TAG_MENU_PAL,
    .oam = &sOamData_Icon,
    .anims = sSpriteAnimTable_Icon,
    .images = NULL,
    .affineAnims = sAffineAnimCmds_Icon,
    .callback = SpriteCB_Bag,
};

static const struct SpriteTemplate sSpriteTemplate_Pokenav = {
    .tileTag = TAG_POKENAV_GFX,
    .paletteTag = TAG_MENU_PAL,
    .oam = &sOamData_Icon,
    .anims = sSpriteAnimTable_Icon,
    .images = NULL,
    .affineAnims = sAffineAnimCmds_Icon,
    .callback = SpriteCB_Pokenav,
};

static const struct SpriteTemplate sSpriteTemplate_TrainerCard = {
    .tileTag = TAG_TRAINER_CARD_GFX,
    .paletteTag = TAG_MENU_PAL,
    .oam = &sOamData_Icon,
    .anims = sSpriteAnimTable_Icon,
    .images = NULL,
    .affineAnims = sAffineAnimCmds_Icon,
    .callback = SpriteCB_TrainerCard,
};

static const struct SpriteTemplate sSpriteTemplate_Save = {
    .tileTag = TAG_SAVE_GFX,
    .paletteTag = TAG_MENU_PAL,
    .oam = &sOamData_Icon,
    .anims = sSpriteAnimTable_Icon,
    .images = NULL,
    .affineAnims = sAffineAnimCmds_Icon,
    .callback = SpriteCB_Save,
};

static const struct SpriteTemplate sSpriteTemplate_Options = {
    .tileTag = TAG_OPTIONS_GFX,
    .paletteTag = TAG_MENU_PAL,
    .oam = &sOamData_Icon,
    .anims = sSpriteAnimTable_Icon,
    .images = NULL,
    .affineAnims = sAffineAnimCmds_Icon,
    .callback = SpriteCB_Options,
};

static const struct SpriteTemplate sSpriteTemplate_Cancel = {
    .tileTag = TAG_CANCEL_GFX,
    .paletteTag = TAG_MENU_PAL,
    .oam = &sOamData_Icon,
    .anims = sSpriteAnimTable_Icon,
    .images = NULL,
    .affineAnims = sAffineAnimCmds_Icon,
    .callback = SpriteCB_Cancel,
};

static const struct SpriteTemplate sSpriteTemplate_Retire = {
    .tileTag = TAG_RETIRE_GFX,
    .paletteTag = TAG_MENU_PAL,
    .oam = &sOamData_Icon,
    .anims = sSpriteAnimTable_Icon,
    .images = NULL,
    .affineAnims = sAffineAnimCmds_Icon,
    .callback = SpriteCB_Retire,
};

static const struct WindowTemplate sWindowTemplate_SafariBalls = {
    .bg = 0,
    .tilemapLeft = 1,
    .tilemapTop = 1,
    .width = 9,
    .height = 4,
    .paletteNum = 15,
    .baseBlock = 0x8
};

static const u8 *const sPyramidFloorNames[FRONTIER_STAGES_PER_CHALLENGE + 1] =
{
    gText_Floor1,
    gText_Floor2,
    gText_Floor3,
    gText_Floor4,
    gText_Floor5,
    gText_Floor6,
    gText_Floor7,
    gText_Peak
};

static const struct WindowTemplate sWindowTemplate_PyramidFloor = {
    .bg = 0,
    .tilemapLeft = 1,
    .tilemapTop = 1,
    .width = 10,
    .height = 4,
    .paletteNum = 15,
    .baseBlock = 0x8
};

static const struct WindowTemplate sWindowTemplate_PyramidPeak = {
    .bg = 0,
    .tilemapLeft = 1,
    .tilemapTop = 1,
    .width = 12,
    .height = 4,
    .paletteNum = 15,
    .baseBlock = 0x8
};

static const struct MenuAction sStartMenuItems[] =
{
    [MENU_ACTION_POKEDEX]         = {gText_MenuPokedex, {.u8_void = StartMenuPokedexCallback}},
    [MENU_ACTION_POKEMON]         = {gText_MenuPokemon, {.u8_void = StartMenuPokemonCallback}},
    [MENU_ACTION_BAG]             = {gText_MenuBag,     {.u8_void = StartMenuBagCallback}},
    [MENU_ACTION_POKENAV]         = {gText_MenuPokenav, {.u8_void = StartMenuPokeNavCallback}},
    [MENU_ACTION_PLAYER]          = {gText_MenuPlayer,  {.u8_void = StartMenuPlayerNameCallback}},
    [MENU_ACTION_SAVE]            = {gText_MenuSave,    {.u8_void = StartMenuSaveCallback}},
    [MENU_ACTION_OPTION]          = {gText_MenuOption,  {.u8_void = StartMenuOptionCallback}},
    [MENU_ACTION_EXIT]            = {gText_MenuExit,    {.u8_void = StartMenuExitCallback}},
    [MENU_ACTION_RETIRE_SAFARI]   = {gText_MenuRetire,  {.u8_void = StartMenuSafariZoneRetireCallback}},
    [MENU_ACTION_PLAYER_LINK]     = {gText_MenuPlayer,  {.u8_void = StartMenuLinkModePlayerNameCallback}},
    [MENU_ACTION_REST_FRONTIER]   = {gText_MenuRest,    {.u8_void = StartMenuSaveCallback}},
    [MENU_ACTION_RETIRE_FRONTIER] = {gText_MenuRetire,  {.u8_void = StartMenuBattlePyramidRetireCallback}},
    [MENU_ACTION_PYRAMID_BAG]     = {gText_MenuBag,     {.u8_void = StartMenuBattlePyramidBagCallback}}
};

static const struct SpriteTemplate * const sIconTemplates[] =
{
    [MENU_ACTION_POKEDEX]         = &sSpriteTemplate_Pokedex,
    [MENU_ACTION_POKEMON]         = &sSpriteTemplate_Pokeball,
    [MENU_ACTION_BAG]             = &sSpriteTemplate_Bag,
    [MENU_ACTION_POKENAV]         = &sSpriteTemplate_Pokenav,
    [MENU_ACTION_PLAYER]          = &sSpriteTemplate_TrainerCard,
    [MENU_ACTION_SAVE]            = &sSpriteTemplate_Save,
    [MENU_ACTION_OPTION]          = &sSpriteTemplate_Options,
    [MENU_ACTION_EXIT]            = &sSpriteTemplate_Cancel,
    [MENU_ACTION_RETIRE_SAFARI]   = &sSpriteTemplate_Retire,
    [MENU_ACTION_PLAYER_LINK]     = &sSpriteTemplate_TrainerCard,
    [MENU_ACTION_REST_FRONTIER]   = &sSpriteTemplate_Pokeball, // TODO ?
    [MENU_ACTION_RETIRE_FRONTIER] = &sSpriteTemplate_Retire,
    [MENU_ACTION_PYRAMID_BAG]     = &sSpriteTemplate_Bag,
};

static const struct BgTemplate sBgTemplates_LinkBattleSave[] =
{
    {
        .bg = 0,
        .charBaseIndex = 2,
        .mapBaseIndex = 31,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0
    }
};

static const struct WindowTemplate sWindowTemplates_LinkBattleSave[] =
{
    {
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 15,
        .width = 26,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 0x194
    },
    DUMMY_WIN_TEMPLATE
};

static const struct WindowTemplate sSaveInfoWindowTemplate = {
    .bg = 0,
    .tilemapLeft = 1,
    .tilemapTop = 1,
    .width = 14,
    .height = 10,
    .paletteNum = 15,
    .baseBlock = 8
};

// Local functions
static void BuildStartMenuActions(void);
static void AddStartMenuAction(u8 action);
static void BuildNormalStartMenu(void);
static void BuildSafariZoneStartMenu(void);
static void BuildLinkModeStartMenu(void);
static void BuildUnionRoomStartMenu(void);
static void BuildBattlePikeStartMenu(void);
static void BuildBattlePyramidStartMenu(void);
static void BuildMultiPartnerRoomStartMenu(void);
static void ShowSafariBallsWindow(void);
static void ShowPyramidFloorWindow(void);
static void RemoveExtraStartMenuWindows(void);
static bool32 PrintStartMenuActions(s8 *pIndex, u32 count);
static bool32 InitStartMenuStep(void);
static void InitStartMenu(void);
static void CreateStartMenuTask(TaskFunc followupFunc);
static void InitSave(void);
static u8 RunSaveCallback(void);
static void ShowSaveMessage(const u8 *message, u8 (*saveCallback)(void));
static void HideSaveMessageWindow(void);
static void HideSaveInfoWindow(void);
static void SaveStartTimer(void);
static bool8 SaveSuccesTimer(void);
static bool8 SaveErrorTimer(void);
static void InitBattlePyramidRetire(void);
static void VBlankCB_LinkBattleSave(void);
static bool32 InitSaveWindowAfterLinkBattle(u8 *par1);
static void CB2_SaveAfterLinkBattle(void);
static void ShowSaveInfoWindow(void);
static void RemoveSaveInfoWindow(void);
static void HideStartMenuWindow(void);

void SetDexPokemonPokenavFlags(void) // unused
{
    FlagSet(FLAG_SYS_POKEDEX_GET);
    FlagSet(FLAG_SYS_POKEMON_GET);
    FlagSet(FLAG_SYS_POKENAV_GET);
}

static void BuildStartMenuActions(void)
{
    sNumStartMenuActions = 0;

    if (IsOverworldLinkActive() == TRUE)
    {
        BuildLinkModeStartMenu();
    }
    else if (InUnionRoom() == TRUE)
    {
        BuildUnionRoomStartMenu();
    }
    else if (GetSafariZoneFlag() == TRUE)
    {
        BuildSafariZoneStartMenu();
    }
    else if (InBattlePike())
    {
        BuildBattlePikeStartMenu();
    }
    else if (InBattlePyramid())
    {
        BuildBattlePyramidStartMenu();
    }
    else if (InMultiPartnerRoom())
    {
        BuildMultiPartnerRoomStartMenu();
    }
    else
    {
        BuildNormalStartMenu();
    }
}

static void AddStartMenuAction(u8 action)
{
    AppendToList(sCurrentStartMenuActions, &sNumStartMenuActions, action);
}

static void BuildNormalStartMenu(void)
{
    if (FlagGet(FLAG_SYS_POKEDEX_GET) == TRUE)
    {
        AddStartMenuAction(MENU_ACTION_POKEDEX);
    }
    if (FlagGet(FLAG_SYS_POKEMON_GET) == TRUE)
    {
        AddStartMenuAction(MENU_ACTION_POKEMON);
    }

    AddStartMenuAction(MENU_ACTION_BAG);

    if (FlagGet(FLAG_SYS_POKENAV_GET) == TRUE)
    {
        AddStartMenuAction(MENU_ACTION_POKENAV);
    }

    AddStartMenuAction(MENU_ACTION_PLAYER);
    AddStartMenuAction(MENU_ACTION_SAVE);
    AddStartMenuAction(MENU_ACTION_OPTION);
    #if DPPT_START_MENU_DISABLE_EXIT == TRUE
    if (!(FlagGet(FLAG_SYS_POKEDEX_GET) && FlagGet(FLAG_SYS_POKEMON_GET) && FlagGet(FLAG_SYS_POKENAV_GET)))
        AddStartMenuAction(MENU_ACTION_EXIT);
    #endif
}

static void BuildSafariZoneStartMenu(void)
{
    // Enough room on screen to fit all actions
    AddStartMenuAction(MENU_ACTION_RETIRE_SAFARI);
    AddStartMenuAction(MENU_ACTION_POKEDEX);
    AddStartMenuAction(MENU_ACTION_POKEMON);
    AddStartMenuAction(MENU_ACTION_BAG);
    AddStartMenuAction(MENU_ACTION_PLAYER);
    AddStartMenuAction(MENU_ACTION_OPTION);
    AddStartMenuAction(MENU_ACTION_EXIT);
}

static void BuildLinkModeStartMenu(void)
{
    // Enough room on screen to fit all actions
    AddStartMenuAction(MENU_ACTION_POKEMON);
    AddStartMenuAction(MENU_ACTION_BAG);

    if (FlagGet(FLAG_SYS_POKENAV_GET) == TRUE)
    {
        AddStartMenuAction(MENU_ACTION_POKENAV);
    }

    AddStartMenuAction(MENU_ACTION_PLAYER_LINK);
    AddStartMenuAction(MENU_ACTION_OPTION);
    AddStartMenuAction(MENU_ACTION_EXIT);
}

static void BuildUnionRoomStartMenu(void)
{
    // Enough room on screen to fit all actions
    AddStartMenuAction(MENU_ACTION_POKEMON);
    AddStartMenuAction(MENU_ACTION_BAG);

    if (FlagGet(FLAG_SYS_POKENAV_GET) == TRUE)
    {
        AddStartMenuAction(MENU_ACTION_POKENAV);
    }

    AddStartMenuAction(MENU_ACTION_PLAYER);
    AddStartMenuAction(MENU_ACTION_OPTION);
    AddStartMenuAction(MENU_ACTION_EXIT);
}

static void BuildBattlePikeStartMenu(void)
{
    // Enough room on screen to fit all actions
    AddStartMenuAction(MENU_ACTION_POKEDEX);
    AddStartMenuAction(MENU_ACTION_POKEMON);
    AddStartMenuAction(MENU_ACTION_PLAYER);
    AddStartMenuAction(MENU_ACTION_OPTION);
    AddStartMenuAction(MENU_ACTION_EXIT);
}

static void BuildBattlePyramidStartMenu(void)
{
    // Enough room on screen to fit all actions
    AddStartMenuAction(MENU_ACTION_POKEMON);
    AddStartMenuAction(MENU_ACTION_PYRAMID_BAG);
    AddStartMenuAction(MENU_ACTION_PLAYER);
    AddStartMenuAction(MENU_ACTION_REST_FRONTIER);
    AddStartMenuAction(MENU_ACTION_RETIRE_FRONTIER);
    AddStartMenuAction(MENU_ACTION_OPTION);
    AddStartMenuAction(MENU_ACTION_EXIT);
}

static void BuildMultiPartnerRoomStartMenu(void)
{
    // Enough room on screen to fit all actions
    AddStartMenuAction(MENU_ACTION_POKEMON);
    AddStartMenuAction(MENU_ACTION_PLAYER);
    AddStartMenuAction(MENU_ACTION_OPTION);
    AddStartMenuAction(MENU_ACTION_EXIT);
}

static void ShowSafariBallsWindow(void)
{
    sSafariBallsWindowId = AddWindow(&sWindowTemplate_SafariBalls);
    PutWindowTilemap(sSafariBallsWindowId);
    DrawStdWindowFrame(sSafariBallsWindowId, FALSE);
    ConvertIntToDecimalStringN(gStringVar1, gNumSafariBalls, STR_CONV_MODE_RIGHT_ALIGN, 2);
    StringExpandPlaceholders(gStringVar4, gText_SafariBallStock);
    AddTextPrinterParameterized(sSafariBallsWindowId, FONT_NORMAL, gStringVar4, 0, 1, TEXT_SKIP_DRAW, NULL);
    CopyWindowToVram(sSafariBallsWindowId, COPYWIN_GFX);
}

static void ShowPyramidFloorWindow(void)
{
    if (gSaveBlock2Ptr->frontier.curChallengeBattleNum == FRONTIER_STAGES_PER_CHALLENGE)
        sBattlePyramidFloorWindowId = AddWindow(&sWindowTemplate_PyramidPeak);
    else
        sBattlePyramidFloorWindowId = AddWindow(&sWindowTemplate_PyramidFloor);

    PutWindowTilemap(sBattlePyramidFloorWindowId);
    DrawStdWindowFrame(sBattlePyramidFloorWindowId, FALSE);
    StringCopy(gStringVar1, sPyramidFloorNames[gSaveBlock2Ptr->frontier.curChallengeBattleNum]);
    StringExpandPlaceholders(gStringVar4, gText_BattlePyramidFloor);
    AddTextPrinterParameterized(sBattlePyramidFloorWindowId, FONT_NORMAL, gStringVar4, 0, 1, TEXT_SKIP_DRAW, NULL);
    CopyWindowToVram(sBattlePyramidFloorWindowId, COPYWIN_GFX);
}

static void RemoveExtraStartMenuWindows(void)
{
    if (GetSafariZoneFlag())
    {
        ClearStdWindowAndFrameToTransparent(sSafariBallsWindowId, FALSE);
        CopyWindowToVram(sSafariBallsWindowId, COPYWIN_GFX);
        RemoveWindow(sSafariBallsWindowId);
    }
    if (InBattlePyramid())
    {
        ClearStdWindowAndFrameToTransparent(sBattlePyramidFloorWindowId, FALSE);
        RemoveWindow(sBattlePyramidFloorWindowId);
    }
}

static bool32 PrintStartMenuActions(s8 *pIndex, u32 count)
{
    s8 index = *pIndex;
    u8 x = 168;

    do
    {
        if (sStartMenuItems[sCurrentStartMenuActions[index]].func.u8_void == StartMenuPlayerNameCallback)
        {
            PrintPlayerNameOnWindow(GetStartMenuWindowId(), sStartMenuItems[sCurrentStartMenuActions[index]].text, 32, index*20);
        }
        else
        {
            StringExpandPlaceholders(gStringVar4, sStartMenuItems[sCurrentStartMenuActions[index]].text);
            AddTextPrinterParameterized(GetStartMenuWindowId(), FONT_NORMAL, gStringVar4, 32, index*20, TEXT_SKIP_DRAW, NULL);
        }

        if (sIconTemplates[sCurrentStartMenuActions[index]] != NULL && sCurrentStartMenuActions[index] < ARRAY_COUNT(sIconTemplates))
        {
            sSpriteIds[sSpriteIdCount] = CreateSprite(sIconTemplates[sCurrentStartMenuActions[index]], x, 16+(index*20), 0);
            sSpriteIdCount++;
        }

        index++;
        if (index >= sNumStartMenuActions)
        {
            *pIndex = index;
            return TRUE;
        }

        count--;
    }
    while (count != 0);

    

    *pIndex = index;
    return FALSE;
}

static bool32 InitStartMenuStep(void)
{
    s8 state = sInitStartMenuData[0];

    switch (state)
    {
    case 0:
        sInitStartMenuData[0]++;
        break;
    case 1:
        BuildStartMenuActions();
        sInitStartMenuData[0]++;
        break;
    case 2:
        LoadMessageBoxAndBorderGfx();
        DrawStdWindowFrame(AddStartMenuWindow(sNumStartMenuActions), FALSE);
        sSpriteIdCount = 0;
        LoadStartMenuGfx();
        sInitStartMenuData[1] = 0;
        sInitStartMenuData[0]++;
        break;
    case 3:
        if (GetSafariZoneFlag())
            ShowSafariBallsWindow();
        if (InBattlePyramid())
            ShowPyramidFloorWindow();
        sInitStartMenuData[0]++;
        break;
    case 4:
        if (PrintStartMenuActions(&sInitStartMenuData[1], 2))
            sInitStartMenuData[0]++;
        break;
    case 5:
        sSelectorSpriteIds[0] = CreateSprite(&sSpriteTemplate_Selector, 181, 16+(20*sStartMenuCursorPos), 0);
        gSprites[sSelectorSpriteIds[0]].hFlip = TRUE;
        sSelectorSpriteIds[1] = CreateSprite(&sSpriteTemplate_Selector, 203, 16+(20*sStartMenuCursorPos), 0);
        sStartMenuCursorPos = InitMenuNormal(GetStartMenuWindowId(), FONT_NORMAL, -1, -1, -1, sNumStartMenuActions, sStartMenuCursorPos);
        CopyWindowToVram(GetStartMenuWindowId(), COPYWIN_MAP);
        return TRUE;
    }

    return FALSE;
}

static void InitStartMenu(void)
{
    sInitStartMenuData[0] = 0;
    sInitStartMenuData[1] = 0;
    while (!InitStartMenuStep())
        ;
}

static void StartMenuTask(u8 taskId)
{
    if (InitStartMenuStep() == TRUE)
        SwitchTaskToFollowupFunc(taskId);
}

static void CreateStartMenuTask(TaskFunc followupFunc)
{
    u8 taskId;

    sInitStartMenuData[0] = 0;
    sInitStartMenuData[1] = 0;
    taskId = CreateTask(StartMenuTask, 0x50);
    SetTaskFuncWithFollowupFunc(taskId, StartMenuTask, followupFunc);
}

static bool8 FieldCB_ReturnToFieldStartMenu(void)
{
    if (InitStartMenuStep() == FALSE)
    {
        return FALSE;
    }

    ReturnToFieldOpenStartMenu();
    return TRUE;
}

void ShowReturnToFieldStartMenu(void)
{
    sInitStartMenuData[0] = 0;
    sInitStartMenuData[1] = 0;
    gFieldCallback2 = FieldCB_ReturnToFieldStartMenu;
}

void Task_ShowStartMenu(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch(task->data[0])
    {
    case 0:
        if (InUnionRoom() == TRUE)
            SetUsingUnionRoomStartMenu();

        gMenuCallback = HandleStartMenuInput;
        task->data[0]++;
        break;
    case 1:
        if (gMenuCallback() == TRUE)
            DestroyTask(taskId);
        break;
    }
}

void ShowStartMenu(void)
{
    if (!IsOverworldLinkActive())
    {
        FreezeObjectEvents();
        PlayerFreeze();
        StopPlayerAvatar();
    }
    CreateStartMenuTask(Task_ShowStartMenu);
    LockPlayerFieldControls();
}

static bool8 HandleStartMenuInput(void)
{
    if (JOY_NEW(DPAD_UP))
    {
        PlaySE(SE_SELECT);
        sStartMenuCursorPos = Menu_MoveCursor(-1);
        gSprites[sSelectorSpriteIds[0]].y = 16+(20*sStartMenuCursorPos);
        gSprites[sSelectorSpriteIds[1]].y = 16+(20*sStartMenuCursorPos);
    }

    if (JOY_NEW(DPAD_DOWN))
    {
        PlaySE(SE_SELECT);
        sStartMenuCursorPos = Menu_MoveCursor(1);
        gSprites[sSelectorSpriteIds[0]].y = 16+(20*sStartMenuCursorPos);
        gSprites[sSelectorSpriteIds[1]].y = 16+(20*sStartMenuCursorPos);
    }

    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        if (sStartMenuItems[sCurrentStartMenuActions[sStartMenuCursorPos]].func.u8_void == StartMenuPokedexCallback)
        {
            if (GetNationalPokedexCount(FLAG_GET_SEEN) == 0)
                return FALSE;
        }

        gMenuCallback = sStartMenuItems[sCurrentStartMenuActions[sStartMenuCursorPos]].func.u8_void;

        if (gMenuCallback != StartMenuSaveCallback
            && gMenuCallback != StartMenuExitCallback
            && gMenuCallback != StartMenuSafariZoneRetireCallback
            && gMenuCallback != StartMenuBattlePyramidRetireCallback)
        {
           FadeScreen(FADE_TO_BLACK, 0);
        }

        return FALSE;
    }

    if (JOY_NEW(START_BUTTON | B_BUTTON))
    {
        RemoveExtraStartMenuWindows();
        HideStartMenu();
        return TRUE;
    }

    return FALSE;
}

static bool8 StartMenuPokedexCallback(void)
{
    if (!gPaletteFade.active)
    {
        IncrementGameStat(GAME_STAT_CHECKED_POKEDEX);
        PlayRainStoppingSoundEffect();
        RemoveExtraStartMenuWindows();
        CleanupOverworldWindowsAndTilemaps();
        SetMainCallback2(CB2_OpenPokedex);

        return TRUE;
    }

    return FALSE;
}

static bool8 StartMenuPokemonCallback(void)
{
    if (!gPaletteFade.active)
    {
        PlayRainStoppingSoundEffect();
        RemoveExtraStartMenuWindows();
        CleanupOverworldWindowsAndTilemaps();
        SetMainCallback2(CB2_PartyMenuFromStartMenu); // Display party menu

        return TRUE;
    }

    return FALSE;
}

static bool8 StartMenuBagCallback(void)
{
    if (!gPaletteFade.active)
    {
        PlayRainStoppingSoundEffect();
        RemoveExtraStartMenuWindows();
        CleanupOverworldWindowsAndTilemaps();
        SetMainCallback2(CB2_BagMenuFromStartMenu); // Display bag menu

        return TRUE;
    }

    return FALSE;
}

static bool8 StartMenuPokeNavCallback(void)
{
    if (!gPaletteFade.active)
    {
        PlayRainStoppingSoundEffect();
        RemoveExtraStartMenuWindows();
        CleanupOverworldWindowsAndTilemaps();
        SetMainCallback2(CB2_InitPokeNav);  // Display PokéNav

        return TRUE;
    }

    return FALSE;
}

static bool8 StartMenuPlayerNameCallback(void)
{
    if (!gPaletteFade.active)
    {
        PlayRainStoppingSoundEffect();
        RemoveExtraStartMenuWindows();
        CleanupOverworldWindowsAndTilemaps();

        if (IsOverworldLinkActive() || InUnionRoom())
            ShowPlayerTrainerCard(CB2_ReturnToFieldWithOpenMenu); // Display trainer card
        else if (FlagGet(FLAG_SYS_FRONTIER_PASS))
            ShowFrontierPass(CB2_ReturnToFieldWithOpenMenu); // Display frontier pass
        else
            ShowPlayerTrainerCard(CB2_ReturnToFieldWithOpenMenu); // Display trainer card

        return TRUE;
    }

    return FALSE;
}

static bool8 StartMenuSaveCallback(void)
{
    if (InBattlePyramid())
        RemoveExtraStartMenuWindows();

    gMenuCallback = SaveStartCallback; // Display save menu

    return FALSE;
}

static bool8 StartMenuOptionCallback(void)
{
    if (!gPaletteFade.active)
    {
        PlayRainStoppingSoundEffect();
        RemoveExtraStartMenuWindows();
        CleanupOverworldWindowsAndTilemaps();
        SetMainCallback2(CB2_InitOptionMenu); // Display option menu
        gMain.savedCallback = CB2_ReturnToFieldWithOpenMenu;

        return TRUE;
    }

    return FALSE;
}

static bool8 StartMenuExitCallback(void)
{
    RemoveExtraStartMenuWindows();
    HideStartMenu(); // Hide start menu

    return TRUE;
}

static bool8 StartMenuSafariZoneRetireCallback(void)
{
    RemoveExtraStartMenuWindows();
    HideStartMenu();
    SafariZoneRetirePrompt();

    return TRUE;
}

static bool8 StartMenuLinkModePlayerNameCallback(void)
{
    if (!gPaletteFade.active)
    {
        PlayRainStoppingSoundEffect();
        CleanupOverworldWindowsAndTilemaps();
        ShowTrainerCardInLink(gLocalLinkPlayerId, CB2_ReturnToFieldWithOpenMenu);

        return TRUE;
    }

    return FALSE;
}

static bool8 StartMenuBattlePyramidRetireCallback(void)
{
    gMenuCallback = BattlePyramidRetireStartCallback; // Confirm retire

    return FALSE;
}

// Functionally unused
void ShowBattlePyramidStartMenu(void)
{
    ClearDialogWindowAndFrameToTransparent(0, FALSE);
    ScriptUnfreezeObjectEvents();
    CreateStartMenuTask(Task_ShowStartMenu);
    LockPlayerFieldControls();
}

static bool8 StartMenuBattlePyramidBagCallback(void)
{
    if (!gPaletteFade.active)
    {
        PlayRainStoppingSoundEffect();
        RemoveExtraStartMenuWindows();
        CleanupOverworldWindowsAndTilemaps();
        SetMainCallback2(CB2_PyramidBagMenuFromStartMenu);

        return TRUE;
    }

    return FALSE;
}

static bool8 SaveStartCallback(void)
{
    InitSave();
    DestroyStartMenuGfx();
    gMenuCallback = SaveCallback;

    return FALSE;
}

static bool8 SaveCallback(void)
{
    switch (RunSaveCallback())
    {
    case SAVE_IN_PROGRESS:
        return FALSE;
    case SAVE_CANCELED: // Back to start menu
        ClearDialogWindowAndFrameToTransparent(0, FALSE);
        InitStartMenu();
        gMenuCallback = HandleStartMenuInput;
        return FALSE;
    case SAVE_SUCCESS:
    case SAVE_ERROR:    // Close start menu
        ClearDialogWindowAndFrameToTransparent(0, TRUE);
        ScriptUnfreezeObjectEvents();
        UnlockPlayerFieldControls();
        SoftResetInBattlePyramid();
        return TRUE;
    }

    return FALSE;
}

static bool8 BattlePyramidRetireStartCallback(void)
{
    InitBattlePyramidRetire();
    gMenuCallback = BattlePyramidRetireCallback;

    return FALSE;
}

static bool8 BattlePyramidRetireReturnCallback(void)
{
    InitStartMenu();
    gMenuCallback = HandleStartMenuInput;

    return FALSE;
}

static bool8 BattlePyramidRetireCallback(void)
{
    switch (RunSaveCallback())
    {
    case SAVE_SUCCESS: // No (Stay in battle pyramid)
        RemoveExtraStartMenuWindows();
        gMenuCallback = BattlePyramidRetireReturnCallback;
        return FALSE;
    case SAVE_IN_PROGRESS:
        return FALSE;
    case SAVE_CANCELED: // Yes (Retire from battle pyramid)
        ClearDialogWindowAndFrameToTransparent(0, TRUE);
        ScriptUnfreezeObjectEvents();
        UnlockPlayerFieldControls();
        ScriptContext_SetupScript(BattlePyramid_Retire);
        return TRUE;
    }

    return FALSE;
}

static void InitSave(void)
{
    SaveMapView();
    sSaveDialogCallback = SaveConfirmSaveCallback;
    sSavingComplete = FALSE;
}

static u8 RunSaveCallback(void)
{
    // True if text is still printing
    if (RunTextPrintersAndIsPrinter0Active() == TRUE)
    {
        return SAVE_IN_PROGRESS;
    }

    sSavingComplete = FALSE;
    return sSaveDialogCallback();
}

void SaveGame(void)
{
    InitSave();
    CreateTask(SaveGameTask, 0x50);
}

static void ShowSaveMessage(const u8 *message, u8 (*saveCallback)(void))
{
    StringExpandPlaceholders(gStringVar4, message);
    LoadMessageBoxAndFrameGfx(0, TRUE);
    AddTextPrinterForMessage_2(TRUE);
    sSavingComplete = TRUE;
    sSaveDialogCallback = saveCallback;
}

static void SaveGameTask(u8 taskId)
{
    u8 status = RunSaveCallback();

    switch (status)
    {
    case SAVE_CANCELED:
    case SAVE_ERROR:
        gSpecialVar_Result = 0;
        break;
    case SAVE_SUCCESS:
        gSpecialVar_Result = status;
        break;
    case SAVE_IN_PROGRESS:
        return;
    }

    DestroyTask(taskId);
    ScriptContext_Enable();
}

static void HideSaveMessageWindow(void)
{
    ClearDialogWindowAndFrame(0, TRUE);
}

static void HideSaveInfoWindow(void)
{
    RemoveSaveInfoWindow();
}

static void SaveStartTimer(void)
{
    sSaveDialogTimer = 60;
}

static bool8 SaveSuccesTimer(void)
{
    sSaveDialogTimer--;

    if (JOY_HELD(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        return TRUE;
    }
    if (sSaveDialogTimer == 0)
    {
        return TRUE;
    }

    return FALSE;
}

static bool8 SaveErrorTimer(void)
{
    if (sSaveDialogTimer != 0)
    {
        sSaveDialogTimer--;
    }
    else if (JOY_HELD(A_BUTTON))
    {
        return TRUE;
    }

    return FALSE;
}

static u8 SaveConfirmSaveCallback(void)
{
    ClearStdWindowAndFrame(GetStartMenuWindowId(), FALSE);
    RemoveStartMenuWindow();
    ShowSaveInfoWindow();

    if (InBattlePyramid())
    {
        ShowSaveMessage(gText_BattlePyramidConfirmRest, SaveYesNoCallback);
    }
    else
    {
        ShowSaveMessage(gText_ConfirmSave, SaveYesNoCallback);
    }

    return SAVE_IN_PROGRESS;
}

static u8 SaveYesNoCallback(void)
{
    DisplayYesNoMenuDefaultYes(); // Show Yes/No menu
    sSaveDialogCallback = SaveConfirmInputCallback;
    return SAVE_IN_PROGRESS;
}

static u8 SaveConfirmInputCallback(void)
{
    switch (Menu_ProcessInputNoWrapClearOnChoose())
    {
    case 0: // Yes
        switch (gSaveFileStatus)
        {
        case SAVE_STATUS_EMPTY:
        case SAVE_STATUS_CORRUPT:
            if (gDifferentSaveFile == FALSE)
            {
                sSaveDialogCallback = SaveFileExistsCallback;
                return SAVE_IN_PROGRESS;
            }

            sSaveDialogCallback = SaveSavingMessageCallback;
            return SAVE_IN_PROGRESS;
        default:
            sSaveDialogCallback = SaveFileExistsCallback;
            return SAVE_IN_PROGRESS;
        }
    case MENU_B_PRESSED:
    case 1: // No
        HideSaveInfoWindow();
        HideSaveMessageWindow();
        return SAVE_CANCELED;
    }

    return SAVE_IN_PROGRESS;
}

// A different save file exists
static u8 SaveFileExistsCallback(void)
{
    if (gDifferentSaveFile == TRUE)
    {
        ShowSaveMessage(gText_DifferentSaveFile, SaveConfirmOverwriteDefaultNoCallback);
    }
    else
    {
        ShowSaveMessage(gText_AlreadySavedFile, SaveConfirmOverwriteCallback);
    }

    return SAVE_IN_PROGRESS;
}

static u8 SaveConfirmOverwriteDefaultNoCallback(void)
{
    DisplayYesNoMenuWithDefault(1); // Show Yes/No menu (No selected as default)
    sSaveDialogCallback = SaveOverwriteInputCallback;
    return SAVE_IN_PROGRESS;
}

static u8 SaveConfirmOverwriteCallback(void)
{
    DisplayYesNoMenuDefaultYes(); // Show Yes/No menu
    sSaveDialogCallback = SaveOverwriteInputCallback;
    return SAVE_IN_PROGRESS;
}

static u8 SaveOverwriteInputCallback(void)
{
    switch (Menu_ProcessInputNoWrapClearOnChoose())
    {
    case 0: // Yes
        sSaveDialogCallback = SaveSavingMessageCallback;
        return SAVE_IN_PROGRESS;
    case MENU_B_PRESSED:
    case 1: // No
        HideSaveInfoWindow();
        HideSaveMessageWindow();
        return SAVE_CANCELED;
    }

    return SAVE_IN_PROGRESS;
}

static u8 SaveSavingMessageCallback(void)
{
    ShowSaveMessage(gText_SavingDontTurnOff, SaveDoSaveCallback);
    return SAVE_IN_PROGRESS;
}

static u8 SaveDoSaveCallback(void)
{
    u8 saveStatus;

    IncrementGameStat(GAME_STAT_SAVED_GAME);
    PausePyramidChallenge();

    if (gDifferentSaveFile == TRUE)
    {
        saveStatus = TrySavingData(SAVE_OVERWRITE_DIFFERENT_FILE);
        gDifferentSaveFile = FALSE;
    }
    else
    {
        saveStatus = TrySavingData(SAVE_NORMAL);
    }

    if (saveStatus == SAVE_STATUS_OK)
        ShowSaveMessage(gText_PlayerSavedGame, SaveSuccessCallback);
    else
        ShowSaveMessage(gText_SaveError, SaveErrorCallback);

    SaveStartTimer();
    return SAVE_IN_PROGRESS;
}

static u8 SaveSuccessCallback(void)
{
    if (!IsTextPrinterActive(0))
    {
        PlaySE(SE_SAVE);
        sSaveDialogCallback = SaveReturnSuccessCallback;
    }

    return SAVE_IN_PROGRESS;
}

static u8 SaveReturnSuccessCallback(void)
{
    if (!IsSEPlaying() && SaveSuccesTimer())
    {
        HideSaveInfoWindow();
        return SAVE_SUCCESS;
    }
    else
    {
        return SAVE_IN_PROGRESS;
    }
}

static u8 SaveErrorCallback(void)
{
    if (!IsTextPrinterActive(0))
    {
        PlaySE(SE_BOO);
        sSaveDialogCallback = SaveReturnErrorCallback;
    }

    return SAVE_IN_PROGRESS;
}

static u8 SaveReturnErrorCallback(void)
{
    if (!SaveErrorTimer())
    {
        return SAVE_IN_PROGRESS;
    }
    else
    {
        HideSaveInfoWindow();
        return SAVE_ERROR;
    }
}

static void InitBattlePyramidRetire(void)
{
    sSaveDialogCallback = BattlePyramidConfirmRetireCallback;
    sSavingComplete = FALSE;
}

static u8 BattlePyramidConfirmRetireCallback(void)
{
    ClearStdWindowAndFrame(GetStartMenuWindowId(), FALSE);
    RemoveStartMenuWindow();
    ShowSaveMessage(gText_BattlePyramidConfirmRetire, BattlePyramidRetireYesNoCallback);

    return SAVE_IN_PROGRESS;
}

static u8 BattlePyramidRetireYesNoCallback(void)
{
    DisplayYesNoMenuWithDefault(1); // Show Yes/No menu (No selected as default)
    sSaveDialogCallback = BattlePyramidRetireInputCallback;

    return SAVE_IN_PROGRESS;
}

static u8 BattlePyramidRetireInputCallback(void)
{
    switch (Menu_ProcessInputNoWrapClearOnChoose())
    {
    case 0: // Yes
        return SAVE_CANCELED;
    case MENU_B_PRESSED:
    case 1: // No
        HideSaveMessageWindow();
        return SAVE_SUCCESS;
    }

    return SAVE_IN_PROGRESS;
}

static void VBlankCB_LinkBattleSave(void)
{
    TransferPlttBuffer();
}

static bool32 InitSaveWindowAfterLinkBattle(u8 *state)
{
    switch (*state)
    {
    case 0:
        SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_MODE_0);
        SetVBlankCallback(NULL);
        ScanlineEffect_Stop();
        DmaClear16(3, PLTT, PLTT_SIZE);
        DmaFillLarge16(3, 0, (void *)VRAM, VRAM_SIZE, 0x1000);
        break;
    case 1:
        ResetSpriteData();
        ResetTasks();
        ResetPaletteFade();
        ScanlineEffect_Clear();
        break;
    case 2:
        ResetBgsAndClearDma3BusyFlags(0);
        InitBgsFromTemplates(0, sBgTemplates_LinkBattleSave, ARRAY_COUNT(sBgTemplates_LinkBattleSave));
        InitWindows(sWindowTemplates_LinkBattleSave);
        LoadUserWindowBorderGfx_(0, 8, BG_PLTT_ID(14));
        Menu_LoadStdPalAt(BG_PLTT_ID(15));
        break;
    case 3:
        ShowBg(0);
        BlendPalettes(PALETTES_ALL, 16, RGB_BLACK);
        SetVBlankCallback(VBlankCB_LinkBattleSave);
        EnableInterrupts(1);
        break;
    case 4:
        return TRUE;
    }

    (*state)++;
    return FALSE;
}

void CB2_SetUpSaveAfterLinkBattle(void)
{
    if (InitSaveWindowAfterLinkBattle(&gMain.state))
    {
        CreateTask(Task_SaveAfterLinkBattle, 0x50);
        SetMainCallback2(CB2_SaveAfterLinkBattle);
    }
}

static void CB2_SaveAfterLinkBattle(void)
{
    RunTasks();
    UpdatePaletteFade();
}

static void Task_SaveAfterLinkBattle(u8 taskId)
{
    s16 *state = gTasks[taskId].data;

    if (!gPaletteFade.active)
    {
        switch (*state)
        {
        case 0:
            FillWindowPixelBuffer(0, PIXEL_FILL(1));
            AddTextPrinterParameterized2(0,
                                        FONT_NORMAL,
                                        gText_SavingDontTurnOffPower,
                                        TEXT_SKIP_DRAW,
                                        NULL,
                                        TEXT_COLOR_DARK_GRAY,
                                        TEXT_COLOR_WHITE,
                                        TEXT_COLOR_LIGHT_GRAY);
            DrawTextBorderOuter(0, 8, 14);
            PutWindowTilemap(0);
            CopyWindowToVram(0, COPYWIN_FULL);
            BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);

            if (gWirelessCommType != 0 && InUnionRoom())
            {
                if (Link_AnyPartnersPlayingFRLG_JP())
                {
                    *state = 1;
                }
                else
                {
                    *state = 5;
                }
            }
            else
            {
                gSoftResetDisabled = TRUE;
                *state = 1;
            }
            break;
        case 1:
            SetContinueGameWarpStatusToDynamicWarp();
            WriteSaveBlock2();
            *state = 2;
            break;
        case 2:
            if (WriteSaveBlock1Sector())
            {
                ClearContinueGameWarpStatus2();
                *state = 3;
                gSoftResetDisabled = FALSE;
            }
            break;
        case 3:
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
            *state = 4;
            break;
        case 4:
            FreeAllWindowBuffers();
            SetMainCallback2(gMain.savedCallback);
            DestroyTask(taskId);
            break;
        case 5:
            CreateTask(Task_LinkFullSave, 5);
            *state = 6;
            break;
        case 6:
            if (!FuncIsActiveTask(Task_LinkFullSave))
            {
                *state = 3;
            }
            break;
        }
    }
}

static void ShowSaveInfoWindow(void)
{
    struct WindowTemplate saveInfoWindow = sSaveInfoWindowTemplate;
    u8 gender;
    u8 color;
    u32 xOffset;
    u32 yOffset;

    if (!FlagGet(FLAG_SYS_POKEDEX_GET))
    {
        saveInfoWindow.height -= 2;
    }

    sSaveInfoWindowId = AddWindow(&saveInfoWindow);
    DrawStdWindowFrame(sSaveInfoWindowId, FALSE);

    gender = gSaveBlock2Ptr->playerGender;
    color = TEXT_COLOR_RED;  // Red when female, blue when male.

    if (gender == MALE)
    {
        color = TEXT_COLOR_BLUE;
    }

    // Print region name
    yOffset = 1;
    BufferSaveMenuText(SAVE_MENU_LOCATION, gStringVar4, TEXT_COLOR_GREEN);
    AddTextPrinterParameterized(sSaveInfoWindowId, FONT_NORMAL, gStringVar4, 0, yOffset, TEXT_SKIP_DRAW, NULL);

    // Print player name
    yOffset += 16;
    AddTextPrinterParameterized(sSaveInfoWindowId, FONT_NORMAL, gText_SavingPlayer, 0, yOffset, TEXT_SKIP_DRAW, NULL);
    BufferSaveMenuText(SAVE_MENU_NAME, gStringVar4, color);
    xOffset = GetStringRightAlignXOffset(FONT_NORMAL, gStringVar4, 0x70);
    PrintPlayerNameOnWindow(sSaveInfoWindowId, gStringVar4, xOffset, yOffset);

    // Print badge count
    yOffset += 16;
    AddTextPrinterParameterized(sSaveInfoWindowId, FONT_NORMAL, gText_SavingBadges, 0, yOffset, TEXT_SKIP_DRAW, NULL);
    BufferSaveMenuText(SAVE_MENU_BADGES, gStringVar4, color);
    xOffset = GetStringRightAlignXOffset(FONT_NORMAL, gStringVar4, 0x70);
    AddTextPrinterParameterized(sSaveInfoWindowId, FONT_NORMAL, gStringVar4, xOffset, yOffset, TEXT_SKIP_DRAW, NULL);

    if (FlagGet(FLAG_SYS_POKEDEX_GET) == TRUE)
    {
        // Print Pokédex count
        yOffset += 16;
        AddTextPrinterParameterized(sSaveInfoWindowId, FONT_NORMAL, gText_SavingPokedex, 0, yOffset, TEXT_SKIP_DRAW, NULL);
        BufferSaveMenuText(SAVE_MENU_CAUGHT, gStringVar4, color);
        xOffset = GetStringRightAlignXOffset(FONT_NORMAL, gStringVar4, 0x70);
        AddTextPrinterParameterized(sSaveInfoWindowId, FONT_NORMAL, gStringVar4, xOffset, yOffset, TEXT_SKIP_DRAW, NULL);
    }

    // Print play time
    yOffset += 16;
    AddTextPrinterParameterized(sSaveInfoWindowId, FONT_NORMAL, gText_SavingTime, 0, yOffset, TEXT_SKIP_DRAW, NULL);
    BufferSaveMenuText(SAVE_MENU_PLAY_TIME, gStringVar4, color);
    xOffset = GetStringRightAlignXOffset(FONT_NORMAL, gStringVar4, 0x70);
    AddTextPrinterParameterized(sSaveInfoWindowId, FONT_NORMAL, gStringVar4, xOffset, yOffset, TEXT_SKIP_DRAW, NULL);

    CopyWindowToVram(sSaveInfoWindowId, COPYWIN_GFX);
}

static void RemoveSaveInfoWindow(void)
{
    ClearStdWindowAndFrame(sSaveInfoWindowId, FALSE);
    RemoveWindow(sSaveInfoWindowId);
}

static void Task_WaitForBattleTowerLinkSave(u8 taskId)
{
    if (!FuncIsActiveTask(Task_LinkFullSave))
    {
        DestroyTask(taskId);
        ScriptContext_Enable();
    }
}

#define tInBattleTower data[2]

void SaveForBattleTowerLink(void)
{
    u8 taskId = CreateTask(Task_LinkFullSave, 5);
    gTasks[taskId].tInBattleTower = TRUE;
    gTasks[CreateTask(Task_WaitForBattleTowerLinkSave, 6)].data[1] = taskId;
}

#undef tInBattleTower

static void HideStartMenuWindow(void)
{
    ClearStdWindowAndFrame(GetStartMenuWindowId(), TRUE);
    RemoveStartMenuWindow();
    ScriptUnfreezeObjectEvents();
    DestroyStartMenuGfx();
    UnlockPlayerFieldControls();
}

void HideStartMenu(void)
{
    PlaySE(SE_SELECT);
    HideStartMenuWindow();
}

void AppendToList(u8 *list, u8 *pos, u8 newEntry)
{
    list[*pos] = newEntry;
    (*pos)++;
}

// NEW

static void SpriteCB_Pokedex(struct Sprite* sprite) 
{
    if (sCurrentStartMenuActions[sStartMenuCursorPos] == MENU_ACTION_POKEDEX) // Easy way to see if we are hovered over the right action
    {
        StartSpriteAnimIfDifferent(sprite, 1);
        StartSpriteAffineAnimIfDifferent(sprite, 1);
    }
    else
    {
        StartSpriteAnimIfDifferent(sprite, 0);
        StartSpriteAffineAnimIfDifferent(sprite, 0);
    }
}

static void SpriteCB_Pokeball(struct Sprite* sprite) 
{
    if (sCurrentStartMenuActions[sStartMenuCursorPos] == MENU_ACTION_POKEMON)
    {
        StartSpriteAnimIfDifferent(sprite, 1);
        StartSpriteAffineAnimIfDifferent(sprite, 1);
    }
    else
    {
        StartSpriteAnimIfDifferent(sprite, 0);
        StartSpriteAffineAnimIfDifferent(sprite, 0);
    }
}

static void SpriteCB_Bag(struct Sprite* sprite) 
{
    if (sCurrentStartMenuActions[sStartMenuCursorPos] == MENU_ACTION_BAG || sCurrentStartMenuActions[sStartMenuCursorPos] == MENU_ACTION_PYRAMID_BAG)
    {
        StartSpriteAnimIfDifferent(sprite, 1);
        StartSpriteAffineAnimIfDifferent(sprite, 1);
    }
    else
    {
        StartSpriteAnimIfDifferent(sprite, 0);
        StartSpriteAffineAnimIfDifferent(sprite, 0);
    }
}

static void SpriteCB_Pokenav(struct Sprite* sprite) 
{
    if (sCurrentStartMenuActions[sStartMenuCursorPos] == MENU_ACTION_POKENAV)
    {
        StartSpriteAnimIfDifferent(sprite, 1);
        StartSpriteAffineAnimIfDifferent(sprite, 1);
    }
    else
    {
        StartSpriteAnimIfDifferent(sprite, 0);
        StartSpriteAffineAnimIfDifferent(sprite, 0);
    }
}

static void SpriteCB_TrainerCard(struct Sprite* sprite) 
{
    if (sCurrentStartMenuActions[sStartMenuCursorPos] == MENU_ACTION_PLAYER || sCurrentStartMenuActions[sStartMenuCursorPos] == MENU_ACTION_PLAYER_LINK)
    {
        StartSpriteAnimIfDifferent(sprite, 1);
        StartSpriteAffineAnimIfDifferent(sprite, 1);
    }
    else
    {
        StartSpriteAnimIfDifferent(sprite, 0);
        StartSpriteAffineAnimIfDifferent(sprite, 0);
    }
}

static void SpriteCB_Save(struct Sprite* sprite) 
{
    if (sCurrentStartMenuActions[sStartMenuCursorPos] == MENU_ACTION_SAVE)
    {
        StartSpriteAnimIfDifferent(sprite, 1);
        StartSpriteAffineAnimIfDifferent(sprite, 1);
    }
    else
    {
        StartSpriteAnimIfDifferent(sprite, 0);
        StartSpriteAffineAnimIfDifferent(sprite, 0);
    }
}

static void SpriteCB_Options(struct Sprite* sprite) 
{
    if (sCurrentStartMenuActions[sStartMenuCursorPos] == MENU_ACTION_OPTION)
    {
        StartSpriteAnimIfDifferent(sprite, 1);
        StartSpriteAffineAnimIfDifferent(sprite, 1);
    }
    else
    {
        StartSpriteAnimIfDifferent(sprite, 0);
        StartSpriteAffineAnimIfDifferent(sprite, 0);
    }
}

static void SpriteCB_Cancel(struct Sprite* sprite) 
{
    if (sCurrentStartMenuActions[sStartMenuCursorPos] == MENU_ACTION_EXIT)
    {
        StartSpriteAnimIfDifferent(sprite, 1);
        StartSpriteAffineAnimIfDifferent(sprite, 1);
    }
    else
    {
        StartSpriteAnimIfDifferent(sprite, 0);
        StartSpriteAffineAnimIfDifferent(sprite, 0);
    }
}

static void SpriteCB_Retire(struct Sprite* sprite) 
{
    if (sCurrentStartMenuActions[sStartMenuCursorPos] == MENU_ACTION_RETIRE_SAFARI || sCurrentStartMenuActions[sStartMenuCursorPos] == MENU_ACTION_RETIRE_FRONTIER)
    {
        StartSpriteAnimIfDifferent(sprite, 1);
        StartSpriteAffineAnimIfDifferent(sprite, 1);
    }
    else
    {
        StartSpriteAnimIfDifferent(sprite, 0);
        StartSpriteAffineAnimIfDifferent(sprite, 0);
    }
}

static void LoadStartMenuGfx(void)
{
    LoadSpritePalette(&sSpritePal_Selector);
    LoadCompressedSpriteSheet(&sSpriteSheet_Selector);
    if (FlagGet(FLAG_SYS_POKEDEX_GET))
        LoadCompressedSpriteSheet(sSpriteSheet_PokedexIcon);
    if (FlagGet(FLAG_SYS_POKEMON_GET))
        LoadCompressedSpriteSheet(sSpriteSheet_PokeballIcon);
    LoadCompressedSpriteSheet(sSpriteSheet_BagIcon);
    if (FlagGet(FLAG_SYS_POKENAV_GET))
        LoadCompressedSpriteSheet(sSpriteSheet_PokenavIcon);
    LoadCompressedSpriteSheet(sSpriteSheet_TrainerCardIcon);
    if (GetSafariZoneFlag())
        LoadCompressedSpriteSheet(sSpriteSheet_RetireIcon);
    else
        LoadCompressedSpriteSheet(sSpriteSheet_SaveIcon);
    LoadCompressedSpriteSheet(sSpriteSheet_OptionsIcon);
    LoadCompressedSpriteSheet(sSpriteSheet_CancelIcon);
}

static void DestroyStartMenuGfx(void)
{
    u32 i;
    FreeSpritePaletteByTag(TAG_MENU_PAL);
    for (i = 0; i < sSpriteIdCount; i++)
    {
        FreeSpriteOamMatrix(&gSprites[sSpriteIds[i]]);
        FreeSpriteTiles(&gSprites[sSpriteIds[i]]);
        DestroySprite(&gSprites[sSpriteIds[i]]);
        sSpriteIds[i] = SPRITE_NONE;
    }
    for (i = 0; i < 2; i++)
    {
        FreeSpriteOamMatrix(&gSprites[sSelectorSpriteIds[i]]);
        FreeSpriteTiles(&gSprites[sSelectorSpriteIds[i]]);
        DestroySprite(&gSprites[sSelectorSpriteIds[i]]);
        sSpriteIds[i] = SPRITE_NONE;
    }
    sSpriteIdCount = 0;
}