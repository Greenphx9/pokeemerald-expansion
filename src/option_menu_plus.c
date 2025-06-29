#include "gba/types.h"
#include "gba/defines.h"
#include "global.h"
#include "main.h"
#include "bg.h"
#include "text_window.h"
#include "window.h"
#include "palette.h"
#include "task.h"
#include "overworld.h"
#include "malloc.h"
#include "gba/macro.h"
#include "menu_helpers.h"
#include "menu.h"
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
#include "option_menu_plus.h"
#include "list_menu.h"
#include "strings.h"

struct OptionMenuPlusUiState
{
    MainCallback savedCallback;
    u8 loadState;
    u8 mode;

    struct ListMenuItem listItems[10];
    u8 optionNames[10][16];

    u8 listTaskId;
    u16 itemsAbove;
    u16 cursorPos;

    u8 scrollIndicatorTaskId;
    u8 optionIndicatorTaskId;
    u8 listMenuTaskId;

    u8 textSpeed;
    u8 battleScene;
    u8 battleStyle;
    u8 sound;
    u8 buttonMode;
    u8 frameType;
    u8 wildMusic;
    u8 trainerMusic;
    u8 gymMusic;
    u8 e4Music;
    u8 championMusic;

    u8 sendToBox;
    u8 giveNickname;

    u8 page;

    u8 realCursorPos; // for some reason, this is not equal to cursorPos?
    u16 selectedVal;
};

enum WindowIds
{
    WINDOW_OPTIONS,
    WINDOW_DESC,
    WINDOW_PAGE,
    WINDOW_CONTROLS,
};

static EWRAM_DATA struct OptionMenuPlusUiState *sOptionMenuPlusUiState = NULL;
static EWRAM_DATA u8 *sBg1TilemapBuffer = NULL;

static const struct BgTemplate sOptionMenuPlusBgTemplates[] =
{
    {
        .bg = 0,
        .charBaseIndex = 0,
        .mapBaseIndex = 31,
        .priority = 1
    },
    {
        .bg = 1,
        .charBaseIndex = 3,
        .mapBaseIndex = 30,
        .priority = 2
    }
};

static const struct WindowTemplate sOptionMenuPlusWindowTemplates[] =
{
    [WINDOW_OPTIONS] =
    {
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 7,
        .width = 26,
        .height = 12,
        .paletteNum = 15,
        .baseBlock = 1
    },
    [WINDOW_DESC] =
    {
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 3,
        .width = 26,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 313
    },
    [WINDOW_PAGE] =
    {
        .bg = 0,
        .tilemapLeft = 0,
        .tilemapTop = 0,
        .width = 5,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 365
    },
    [WINDOW_CONTROLS] =
    {
        .bg = 0,
        .tilemapLeft = 9,
        .tilemapTop = 0,
        .width = 21,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 375
    },
    DUMMY_WIN_TEMPLATE
};

static const u32 sOptionMenuPlusTiles[] = INCBIN_U32("graphics/option_menu_plus/tiles.4bpp.lz");

static const u32 sOptionMenuPlusTilemap[] = INCBIN_U32("graphics/option_menu_plus/tiles.bin.lz");

static const u16 sOptionMenuPlusPalette[] = INCBIN_U16("graphics/option_menu_plus/tiles.gbapal");

enum FontColor
{
    FONT_BLACK,
    FONT_WHITE,
    FONT_RED,
};
static const u8 sOptionMenuPlusWindowFontColors[][3] =
{
    [FONT_BLACK]  = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_DARK_GRAY,  TEXT_COLOR_LIGHT_GRAY},
    [FONT_WHITE]  = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_WHITE,      TEXT_COLOR_DARK_GRAY},
    [FONT_RED]    = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_RED,        TEXT_COLOR_LIGHT_GRAY},
};

static const u8 sOptionMenuPickSwitchCancelTextColor[] = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_WHITE, TEXT_COLOR_DARK_GRAY};

#define TAG_SCROLL_ARROW 5108

enum 
{
    MENUPAGE_OVERWORLD,
    MENUPAGE_BATTLE,
    MENUPAGE_MUSIC,
    MENUPAGE_COUNT,
};

enum
{
    MENUITEM_TEXTSPEED,
    MENUITEM_SOUND,
    MENUITEM_BUTTONMODE,
    MENUITEM_FRAMETYPE,
    MENUITEM_CANCELOVERWORLD,
    MENUITEM_COUNTOVERWORLD,
};

enum
{
    MENUITEM_BATTLEEFFECTS,
    MENUITEM_BATTLESTYLE,
    MENUITEM_SENDTOBOX,
    MENUITEM_GIVENICKNAMES,
    MENUITEM_CANCELBATTLE,
    MENUITEM_COUNTBATTLE,
};

enum 
{
    MENUITEM_WILDMUSIC,
    MENUITEM_TRAINERMUSIC,
    MENUITEM_GYMMUSIC,
    MENUITEM_E4MUSIC,
    MENUITEM_CHAMPIONMUSIC,
    MENUITEM_CANCELMUSIC,
    MENUITEM_COUNTMUSIC,
};

static const u8 *const sTextSpeedOptions[] =
{
    COMPOUND_STRING("Slow"), 
    COMPOUND_STRING("Mid"), 
    COMPOUND_STRING("Fast")
};

static const u8 *const sBattleEffectOptions[] =
{
    COMPOUND_STRING("On"), 
    COMPOUND_STRING("Off"), 
};

static const u8 *const sBattleStyleOptions[] =
{
    COMPOUND_STRING("Shift"), 
    COMPOUND_STRING("Set"), 
};

static const u8 *const sSoundOptions[] =
{
    COMPOUND_STRING("Mono"), 
    COMPOUND_STRING("Stereo"), 
};

static const u8 *const sButtonModeOptions[] =
{
    COMPOUND_STRING("Normal"), 
    COMPOUND_STRING("LR"), 
    COMPOUND_STRING("L=A"),
};

static const u8 *const sFrameOptions[] =
{
    COMPOUND_STRING("1"),  COMPOUND_STRING("2"),  COMPOUND_STRING("3"),
    COMPOUND_STRING("4"),  COMPOUND_STRING("5"),  COMPOUND_STRING("6"),
    COMPOUND_STRING("7"),  COMPOUND_STRING("8"),  COMPOUND_STRING("9"),
    COMPOUND_STRING("10"),
};

static const u8 *const sMusicOptions[] =
{
    COMPOUND_STRING("RSE"),
    COMPOUND_STRING("FRLG"),
    COMPOUND_STRING("DPPT"),
    COMPOUND_STRING("HGSS"),
    COMPOUND_STRING("BW"),
};

static const u8 *const sSendToBoxOptions[] =
{
    COMPOUND_STRING("Manual"),
    COMPOUND_STRING("Automatic"),
};

static const u8 *const sTextSpeedDescs[] =
{
    COMPOUND_STRING("Adjust text print speed."),
};

static const u8 *const sBattleEffectDescs[] =
{
    COMPOUND_STRING("Battle animations will play."),
    COMPOUND_STRING("Battle animations will not play."),
};

static const u8 *const sBattleStyleDescs[] =
{
    COMPOUND_STRING("Get a free switch after a KO."),
    COMPOUND_STRING("Forced to stay in after a KO."),
};

static const u8 *const sSoundDescs[] =
{
    COMPOUND_STRING("Sound plays out of one speaker."),
    COMPOUND_STRING("Sound plays out of both speakers."),
};

static const u8 *const sButtonModeDescs[] =
{
    COMPOUND_STRING("Change what the L button does."),
};

static const u8 *const sFrameDescs[] =
{
    COMPOUND_STRING("Change border around text."),
};

static const u8 *const sWildMusicDescs[] =
{
    COMPOUND_STRING("Music that'll play in wild battles."),
};

static const u8 *const sTrainerMusicDescs[] =
{
    COMPOUND_STRING("Music that'll play in trainer battles."),
};

static const u8 *const sGymMusicDescs[] =
{
    COMPOUND_STRING("Music that'll play in gym battles."),
};

static const u8 *const sE4MusicDescs[] =
{
    COMPOUND_STRING("Music that'll play in elite 4 battles."),
};

static const u8 *const sChampionMusicDescs[] =
{
    COMPOUND_STRING("Music that'll play in the champion battle."),
};

static const u8 *const sSendToBoxDescs[] =
{
    COMPOUND_STRING("Choose to send Pokémon to boxes."),
    COMPOUND_STRING("Automatically send Pokémon to boxes."),
};

static const u8 *const sGiveNicknameDescs[] =
{
    COMPOUND_STRING("Choose to give Pokémon nicknames."),
    COMPOUND_STRING("Pokémon do not get nicknames."),
};

const u8 sText_PickSwitchCancel[] = _("{DPAD_UPDOWN}Pick {DPAD_LEFTRIGHT}Switch {L_BUTTON}{R_BUTTON}Page {B_BUTTON}Cancel");

#define OPTION_X 150

// Callbacks for the Option Menu Plus
static void OptionMenuPlus_MainCB(void);
static void OptionMenuPlus_VBlankCB(void);

// Option Menu Plus UI tasks
static void Task_OptionMenuPlusWaitFadeIn(u8 taskId);
static void Task_OptionMenuPlusMainInput(u8 taskId);
static void OptionMenuPlus_VerticalDpad(bool8 up);
static void Task_OptionMenuPlusWaitFadeAndBail(u8 taskId);
static void Task_OptionMenuPlusWaitFadeAndExitGracefully(u8 taskId);

// Option Menu Plus helper functions
static void OptionMenuPlus_Init(MainCallback callback);
static void OptionMenuPlus_ResetGpuRegsAndBgs(void);
static bool8 OptionMenuPlus_InitBgs(void);
static void OptionMenuPlus_FadeAndBail(void);
static bool8 OptionMenuPlus_LoadGraphics(void);
static void OptionMenuPlus_InitWindows(void);
static void OptionMenuPlus_FreeResources(void);

static void OptionMenuPlus_CreateListMenu(void);
static void OptionMenuPlus_AddScrollIndicator(void);
static void OptionMenuPlus_RemoveScrollIndicator(void);
static void OptionMenuPlus_MoveCursor(s32 id, bool8 onInit, struct ListMenu *list);
static void OptionMenuPlus_PrintMenuItem(u8 windowId, u32 id, u8 yOffset);
static void OptionMenuPlus_UpdateMenuItem(void);
static void OptionMenuPlus_PrintDescription(u8 id);
static void OptionMenuPlus_UpdateLeftRightScrollIndicator(void);
static void OptionMenuPlus_PrintPageName(void);
static void OptionMenuPlus_PrintControls(void);
static bool8 OptionMenuPlus_IsCancel(u8 id);
static u8 OptionMenuPlus_GetPageCount(void);
static void OptionMenuPlus_LoadNewPage(void);

static void OptionMenuPlus_LoadOptionValues(void);
static void OptionMenuPlus_SaveOptionValues(void);
static u16 OptionMenuPlus_TextSpeedFunc(u8 value);
static u16 OptionMenuPlus_BattleSceneFunc(u8 value);
static u16 OptionMenuPlus_BattleStyleFunc(u8 value);
static u16 OptionMenuPlus_SoundFunc(u8 value);
static u16 OptionMenuPlus_ButtonModeFunc(u8 value);
static u16 OptionMenuPlus_FrameFunc(u8 value);
static u16 OptionMenuPlus_WildMusicFunc(u8 value);
static u16 OptionMenuPlus_TrainerMusicFunc(u8 value);
static u16 OptionMenuPlus_GymMusicFunc(u8 value);
static u16 OptionMenuPlus_E4MusicFunc(u8 value);
static u16 OptionMenuPlus_ChampionMusicFunc(u8 value);
static u16 OptionMenuPlus_SendToBoxFunc(u8 value);
static u16 OptionMenuPlus_GiveNicknamesFunc(u8 value);

const struct Option sOptionMenuPlus_GeneralPage[MENUITEM_COUNTOVERWORLD] = 
{
    [MENUITEM_TEXTSPEED] =
    {
        .name = COMPOUND_STRING("Text Speed"),
        .options = sTextSpeedOptions,
        .optionsDesc = sTextSpeedDescs,
        .sameDesc = TRUE,
        .optionCount = ARRAY_COUNT(sTextSpeedOptions),
        .func = OptionMenuPlus_TextSpeedFunc,
    },
    [MENUITEM_SOUND] =
    {
        .name = COMPOUND_STRING("Sound"),
        .options = sSoundOptions,
        .optionsDesc = sSoundDescs,
        .sameDesc = FALSE,
        .optionCount = ARRAY_COUNT(sSoundOptions),
        .func = OptionMenuPlus_SoundFunc,
    },
    [MENUITEM_BUTTONMODE] =
    {
        .name = COMPOUND_STRING("Button Mode"),
        .options = sButtonModeOptions,
        .optionsDesc = sButtonModeDescs,
        .sameDesc = TRUE,
        .optionCount = ARRAY_COUNT(sButtonModeOptions),
        .func = OptionMenuPlus_ButtonModeFunc,
    },
    [MENUITEM_FRAMETYPE] =
    {
        .name = COMPOUND_STRING("Frame"),
        .options = sFrameOptions,
        .optionsDesc = sFrameDescs,
        .sameDesc = TRUE,
        .optionCount = ARRAY_COUNT(sFrameOptions),
        .func = OptionMenuPlus_FrameFunc,
    },
    [MENUITEM_CANCELOVERWORLD] = // handled specially
    {
        .name = gText_OptionMenuCancel,
    },
};

const struct Option sOptionMenuPlus_BattlePage[MENUITEM_COUNTBATTLE] = 
{
    [MENUITEM_BATTLEEFFECTS] =
    {
        .name = COMPOUND_STRING("Battle Effects"),
        .options = sBattleEffectOptions,
        .optionsDesc = sBattleEffectDescs,
        .sameDesc = FALSE,
        .optionCount = ARRAY_COUNT(sBattleEffectOptions),
        .func = OptionMenuPlus_BattleSceneFunc,
    },
    [MENUITEM_BATTLESTYLE] =
    {
        .name = COMPOUND_STRING("Battle Style"),
        .options = sBattleStyleOptions,
        .optionsDesc = sBattleStyleDescs,
        .sameDesc = FALSE,
        .optionCount = ARRAY_COUNT(sBattleStyleOptions),
        .func = OptionMenuPlus_BattleStyleFunc,
    },
    [MENUITEM_SENDTOBOX] =
    {
        .name = COMPOUND_STRING("Send To Boxes"),
        .options = sSendToBoxOptions,
        .optionsDesc = sSendToBoxDescs,
        .sameDesc = FALSE,
        .optionCount = ARRAY_COUNT(sSendToBoxOptions),
        .func = OptionMenuPlus_SendToBoxFunc,
    },
    [MENUITEM_GIVENICKNAMES] =
    {
        .name = COMPOUND_STRING("Give Nicknames"),
        .options = sBattleEffectOptions,
        .optionsDesc = sGiveNicknameDescs,
        .sameDesc = FALSE,
        .optionCount = ARRAY_COUNT(sBattleEffectOptions),
        .func = OptionMenuPlus_GiveNicknamesFunc,
    },
    [MENUITEM_CANCELBATTLE] = // handled specially
    {
        .name = gText_OptionMenuCancel,
    },
};

const struct Option sOptionMenuPlus_MusicPage[MENUITEM_COUNTMUSIC] = 
{
    [MENUITEM_WILDMUSIC] =
    {
        .name = COMPOUND_STRING("Wild Music"),
        .options = sMusicOptions,
        .optionsDesc = sWildMusicDescs,
        .sameDesc = TRUE,
        .optionCount = ARRAY_COUNT(sMusicOptions),
        .func = OptionMenuPlus_WildMusicFunc,
    },
    [MENUITEM_TRAINERMUSIC] =
    {
        .name = COMPOUND_STRING("Trainer Music"),
        .options = sMusicOptions,
        .optionsDesc = sTrainerMusicDescs,
        .sameDesc = TRUE,
        .optionCount = ARRAY_COUNT(sMusicOptions),
        .func = OptionMenuPlus_TrainerMusicFunc,
    },
    [MENUITEM_GYMMUSIC] =
    {
        .name = COMPOUND_STRING("Gym Music"),
        .options = sMusicOptions,
        .optionsDesc = sGymMusicDescs,
        .sameDesc = TRUE,
        .optionCount = ARRAY_COUNT(sMusicOptions),
        .func = OptionMenuPlus_GymMusicFunc,
    },
    [MENUITEM_E4MUSIC] =
    {
        .name = COMPOUND_STRING("Elite 4 Music"),
        .options = sMusicOptions,
        .optionsDesc = sE4MusicDescs,
        .sameDesc = TRUE,
        .optionCount = ARRAY_COUNT(sMusicOptions),
        .func = OptionMenuPlus_E4MusicFunc,
    },
    [MENUITEM_CHAMPIONMUSIC] =
    {
        .name = COMPOUND_STRING("Champion Music"),
        .options = sMusicOptions,
        .optionsDesc = sChampionMusicDescs,
        .sameDesc = TRUE,
        .optionCount = ARRAY_COUNT(sMusicOptions),
        .func = OptionMenuPlus_ChampionMusicFunc,
    },
    [MENUITEM_CANCELMUSIC] = // handled specially
    {
        .name = gText_OptionMenuCancel,
    },
};

const struct OptionPage sOptionMenuPlus_Pages[MENUPAGE_COUNT] =
{
    [MENUPAGE_OVERWORLD] =
    {
        .name = COMPOUND_STRING("General"),
        .options = sOptionMenuPlus_GeneralPage,
    },
    [MENUPAGE_BATTLE] =
    {
        .name = COMPOUND_STRING("Battle"),
        .options = sOptionMenuPlus_BattlePage,
    },
    [MENUPAGE_MUSIC] =
    {
        .name = COMPOUND_STRING("Music"),
        .options = sOptionMenuPlus_MusicPage,
    }
};

void Task_OpenOptionsMenuPlus(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        CleanupOverworldWindowsAndTilemaps();
        OptionMenuPlus_Init(gMain.savedCallback);
        DestroyTask(taskId);
    }
}

static void OptionMenuPlus_Init(MainCallback callback)
{
    sOptionMenuPlusUiState = AllocZeroed(sizeof(struct OptionMenuPlusUiState));
    if (sOptionMenuPlusUiState == NULL)
    {
        SetMainCallback2(callback);
        return;
    }

    sOptionMenuPlusUiState->loadState = 0;
    sOptionMenuPlusUiState->savedCallback = callback;

    SetMainCallback2(OptionMenuPlus_SetupCB);
}

static void OptionMenuPlus_ResetGpuRegsAndBgs(void)
{
    /* These are needed otherwise there will be graphical issues if
       launching from the main menu.
    */
     SetGpuReg(REG_OFFSET_DISPCNT, 0);
     SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_1D_MAP | DISPCNT_OBJ_ON);

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

void OptionMenuPlus_SetupCB(void)
{
    switch (gMain.state)
    {
    case 0:
        OptionMenuPlus_ResetGpuRegsAndBgs();
        SetVBlankHBlankCallbacksToNull();
        ClearScheduledBgCopiesToVram();
        gMain.state++;
        break;
    case 1:
        ScanlineEffect_Stop();
        FreeAllSpritePalettes();
        ResetPaletteFade();
        ResetSpriteData();
        ResetTasks();
        gMain.state++;
        break;
    case 2:
        if (OptionMenuPlus_InitBgs())
        {
            sOptionMenuPlusUiState->loadState = 0;
            gMain.state++;
        }
        else
        {
            OptionMenuPlus_FadeAndBail();
            return;
        }
        break;
    case 3:
        if (OptionMenuPlus_LoadGraphics() == TRUE)
        {
            gMain.state++;
        }
        break;
    case 4:
        OptionMenuPlus_InitWindows();
        gMain.state++;
        break;
    case 5:
        sOptionMenuPlusUiState->scrollIndicatorTaskId = TASK_NONE;
        sOptionMenuPlusUiState->optionIndicatorTaskId = TASK_NONE;

        OptionMenuPlus_LoadOptionValues();
        OptionMenuPlus_PrintPageName();
        OptionMenuPlus_PrintControls();

        OptionMenuPlus_CreateListMenu();
        CreateTask(Task_OptionMenuPlusWaitFadeIn, 0);
        gMain.state++;
        break;
    case 6:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        gMain.state++;
        break;
    case 7:
        SetVBlankCallback(OptionMenuPlus_VBlankCB);
        SetMainCallback2(OptionMenuPlus_MainCB);
        break;
    }
}

static void OptionMenuPlus_MainCB(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static void OptionMenuPlus_VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void Task_OptionMenuPlusWaitFadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        gTasks[taskId].func = Task_OptionMenuPlusMainInput;
    }
}

static void Task_OptionMenuPlusMainInput(u8 taskId)
{
    if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_PC_OFF);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        OptionMenuPlus_SaveOptionValues();
        gTasks[taskId].func = Task_OptionMenuPlusWaitFadeAndExitGracefully;
    }
    else if (JOY_NEW(A_BUTTON))
    {
        if (OptionMenuPlus_IsCancel(sOptionMenuPlusUiState->realCursorPos))
        {
            PlaySE(SE_PC_OFF);
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
            OptionMenuPlus_SaveOptionValues();
            gTasks[taskId].func = Task_OptionMenuPlusWaitFadeAndExitGracefully;
        }
        else
            PlaySE(SE_FAILURE);
    }
    else if (JOY_NEW(R_BUTTON))
    {
        u8 page = sOptionMenuPlusUiState->page;
        if (page < MENUPAGE_COUNT - 1)
        {
            PlaySE(SE_WIN_OPEN);
            sOptionMenuPlusUiState->page++;
            OptionMenuPlus_LoadNewPage();
        }
    }
    else if (JOY_NEW(L_BUTTON))
    {
        u8 page = sOptionMenuPlusUiState->page;
        if (page > 0)
        {
            PlaySE(SE_WIN_OPEN);
            sOptionMenuPlusUiState->page--;
            OptionMenuPlus_LoadNewPage();
        }
    }
    else if (JOY_NEW(DPAD_RIGHT) || JOY_REPEAT(DPAD_RIGHT))
    {
        u8 cursorPos = sOptionMenuPlusUiState->realCursorPos;
        u8 page = sOptionMenuPlusUiState->page;
        if (sOptionMenuPlus_Pages[page].options[cursorPos].func(0) < sOptionMenuPlus_Pages[page].options[cursorPos].optionCount - 1)
        {
            PlaySE(SE_SELECT);
            sOptionMenuPlus_Pages[page].options[cursorPos].func(1);
            sOptionMenuPlusUiState->selectedVal++;
            OptionMenuPlus_UpdateMenuItem();
            if (page == MENUPAGE_OVERWORLD && cursorPos == MENUITEM_FRAMETYPE)
            {
                gSaveBlock2Ptr->optionsWindowFrameType++;
                LoadMessageBoxAndBorderGfx();
            }
            else if (page == MENUPAGE_OVERWORLD && cursorPos == MENUITEM_BUTTONMODE)
                gSaveBlock2Ptr->optionsButtonMode++;
            OptionMenuPlus_PrintDescription(cursorPos);
            OptionMenuPlus_UpdateLeftRightScrollIndicator();
        }
            
    }
    else if (JOY_NEW(DPAD_LEFT) || JOY_REPEAT(DPAD_LEFT))
    {
        u8 cursorPos = sOptionMenuPlusUiState->realCursorPos;
        u8 page = sOptionMenuPlusUiState->page;
        if (sOptionMenuPlus_Pages[page].options[cursorPos].func(0) > 0)
        {
            PlaySE(SE_SELECT);
            sOptionMenuPlus_Pages[page].options[cursorPos].func(-1);
            sOptionMenuPlusUiState->selectedVal--;
            OptionMenuPlus_UpdateMenuItem();
            OptionMenuPlus_UpdateMenuItem();
            if (page == MENUPAGE_OVERWORLD && cursorPos == MENUITEM_FRAMETYPE)
            {
                gSaveBlock2Ptr->optionsWindowFrameType--;
                LoadMessageBoxAndBorderGfx();
            }
            else if (page == MENUPAGE_OVERWORLD && cursorPos == MENUITEM_BUTTONMODE)
                gSaveBlock2Ptr->optionsButtonMode--;
            OptionMenuPlus_PrintDescription(cursorPos);
            OptionMenuPlus_UpdateLeftRightScrollIndicator();
        }
    }
    else if ((JOY_NEW(DPAD_DOWN) || JOY_REPEAT(DPAD_DOWN)))
    {
        OptionMenuPlus_VerticalDpad(FALSE);
    }
    else if ((JOY_NEW(DPAD_UP) || JOY_REPEAT(DPAD_UP)))
    {
        OptionMenuPlus_VerticalDpad(TRUE);
    }
}

static void OptionMenuPlus_VerticalDpad(bool8 up)
{
    ListMenu_ProcessInput(sOptionMenuPlusUiState->listTaskId);
    ListMenuGetScrollAndRow(sOptionMenuPlusUiState->listTaskId, &sOptionMenuPlusUiState->itemsAbove, &sOptionMenuPlusUiState->cursorPos); 

    if (up)
    {
        if (sOptionMenuPlusUiState->realCursorPos > 0)
            sOptionMenuPlusUiState->realCursorPos--;    
    }
    else
    {
        if (sOptionMenuPlusUiState->realCursorPos < OptionMenuPlus_GetPageCount() - 1)
            sOptionMenuPlusUiState->realCursorPos++;    
    }
    
    if (!OptionMenuPlus_IsCancel(sOptionMenuPlusUiState->realCursorPos))
        sOptionMenuPlusUiState->selectedVal = sOptionMenuPlus_Pages[sOptionMenuPlusUiState->page].options[sOptionMenuPlusUiState->realCursorPos].func(0);

    OptionMenuPlus_UpdateLeftRightScrollIndicator();
    OptionMenuPlus_PrintDescription(sOptionMenuPlusUiState->realCursorPos); 
}

static void Task_OptionMenuPlusWaitFadeAndBail(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(sOptionMenuPlusUiState->savedCallback);
        OptionMenuPlus_FreeResources();
        DestroyTask(taskId);
    }
}

static void Task_OptionMenuPlusWaitFadeAndExitGracefully(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(sOptionMenuPlusUiState->savedCallback);
        OptionMenuPlus_FreeResources();
        DestroyTask(taskId);
    }
}
#define TILEMAP_BUFFER_SIZE (1024 * 2)
static bool8 OptionMenuPlus_InitBgs(void)
{
    ResetAllBgsCoordinates();

    sBg1TilemapBuffer = AllocZeroed(TILEMAP_BUFFER_SIZE);
    if (sBg1TilemapBuffer == NULL)
    {
        return FALSE;
    }

    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sOptionMenuPlusBgTemplates, NELEMS(sOptionMenuPlusBgTemplates));

    SetBgTilemapBuffer(1, sBg1TilemapBuffer);
    ScheduleBgCopyTilemapToVram(1);

    ShowBg(0);
    ShowBg(1);

    return TRUE;
}
#undef TILEMAP_BUFFER_SIZE

static void OptionMenuPlus_FadeAndBail(void)
{
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    CreateTask(Task_OptionMenuPlusWaitFadeAndBail, 0);
    SetVBlankCallback(OptionMenuPlus_VBlankCB);
    SetMainCallback2(OptionMenuPlus_MainCB);
}

static bool8 OptionMenuPlus_LoadGraphics(void)
{
    switch (sOptionMenuPlusUiState->loadState)
    {
    case 0:
        ResetTempTileDataBuffers();
        DecompressAndCopyTileDataToVram(1, sOptionMenuPlusTiles, 0, 0, 0);
        sOptionMenuPlusUiState->loadState++;
        break;
    case 1:
        if (FreeTempTileDataBuffersIfPossible() != TRUE)
        {
            LZDecompressWram(sOptionMenuPlusTilemap, sBg1TilemapBuffer);
            sOptionMenuPlusUiState->loadState++;
        }
        break;
    case 2:
        LoadPalette(sOptionMenuPlusPalette, BG_PLTT_ID(0), PLTT_SIZE_4BPP);
        LoadPalette(gMessageBox_HGSS_Pal, BG_PLTT_ID(15), PLTT_SIZE_4BPP);
        sOptionMenuPlusUiState->loadState++;
        break;
    case 3:
        LoadMessageBoxAndBorderGfx();
        sOptionMenuPlusUiState->loadState++;
    default:
        sOptionMenuPlusUiState->loadState = 0;
        return TRUE;
    }
    return FALSE;
}

static void OptionMenuPlus_InitWindows(void)
{
    InitWindows(sOptionMenuPlusWindowTemplates);
    DeactivateAllTextPrinters();
    ScheduleBgCopyTilemapToVram(0);
    FillWindowPixelBuffer(WINDOW_OPTIONS, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    FillWindowPixelBuffer(WINDOW_DESC, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    FillWindowPixelBuffer(WINDOW_PAGE, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    FillWindowPixelBuffer(WINDOW_CONTROLS, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    PutWindowTilemap(WINDOW_OPTIONS);
    PutWindowTilemap(WINDOW_DESC);
    PutWindowTilemap(WINDOW_PAGE);
    PutWindowTilemap(WINDOW_CONTROLS);
    DrawStdWindowFrame(WINDOW_OPTIONS, FALSE);
    CopyWindowToVram(WINDOW_OPTIONS, 3);
    CopyWindowToVram(WINDOW_DESC, 3);
    CopyWindowToVram(WINDOW_PAGE, 3);
    CopyWindowToVram(WINDOW_CONTROLS, 3);
}

static void OptionMenuPlus_FreeResources(void)
{
    if (sOptionMenuPlusUiState != NULL)
    {
        Free(sOptionMenuPlusUiState);
    }
    if (sBg1TilemapBuffer != NULL)
    {
        Free(sBg1TilemapBuffer);
    }
    FreeAllWindowBuffers();
    ResetSpriteData();
}

static const struct ListMenuTemplate sOptionMenuPlus_ListMenu =
{
    .items = NULL,
    .moveCursorFunc = OptionMenuPlus_MoveCursor,
    .itemPrintFunc = OptionMenuPlus_PrintMenuItem,
    .totalItems = 0,
    .maxShowed = 0,
    .windowId = 0,
    .header_X = 0,
    .item_X = 10,
    .cursor_X = 2,
    .upText_Y = 0,
    .cursorPal = 2,
    .fillValue = 0,
    .cursorShadowPal = 3,
    .lettersSpacing = FALSE,
    .itemVerticalPadding = 0,
    .scrollMultiple = LIST_NO_MULTIPLE_SCROLL,
    .fontId = FONT_SHORT,
    .cursorKind = CURSOR_BLACK_ARROW,
    .textNarrowWidth = 0,
};

static void OptionMenuPlus_CreateListMenu(void)
{
    u32 i;
    u8 page = sOptionMenuPlusUiState->page;
    for (i = 0; i < OptionMenuPlus_GetPageCount(); i++)
    {
        StringCopy(&sOptionMenuPlusUiState->optionNames[i][0], sOptionMenuPlus_Pages[page].options[i].name);
        sOptionMenuPlusUiState->listItems[i].name = &sOptionMenuPlusUiState->optionNames[i][0];
        sOptionMenuPlusUiState->listItems[i].id = i;
    }

    gMultiuseListMenuTemplate = sOptionMenuPlus_ListMenu;
    gMultiuseListMenuTemplate.windowId = WINDOW_OPTIONS;
    gMultiuseListMenuTemplate.totalItems = OptionMenuPlus_GetPageCount();
    gMultiuseListMenuTemplate.items = sOptionMenuPlusUiState->listItems;
    gMultiuseListMenuTemplate.maxShowed = 7;

    sOptionMenuPlusUiState->listMenuTaskId = ListMenuInit(&gMultiuseListMenuTemplate, sOptionMenuPlusUiState->itemsAbove, sOptionMenuPlusUiState->cursorPos);
    if (OptionMenuPlus_GetPageCount() > 7)
        OptionMenuPlus_AddScrollIndicator();

    sOptionMenuPlusUiState->selectedVal = sOptionMenuPlus_Pages[page].options[0].func(0);
    OptionMenuPlus_UpdateLeftRightScrollIndicator();

    // print initial desc
    OptionMenuPlus_PrintDescription(0); 
}

static void OptionMenuPlus_AddScrollIndicator(void)
{
    if (sOptionMenuPlusUiState->scrollIndicatorTaskId == TASK_NONE)
        sOptionMenuPlusUiState->scrollIndicatorTaskId = AddScrollIndicatorArrowPairParameterized(SCROLL_ARROW_UP, 120, 60, 147,
                                                                                                OptionMenuPlus_GetPageCount() - 7,
                                                                                                TAG_SCROLL_ARROW,
                                                                                                TAG_SCROLL_ARROW,
                                                                                                &sOptionMenuPlusUiState->itemsAbove);
}

static void OptionMenuPlus_RemoveScrollIndicator(void)
{
    if (sOptionMenuPlusUiState->scrollIndicatorTaskId != TASK_NONE)
    {
        RemoveScrollIndicatorArrowPair(sOptionMenuPlusUiState->scrollIndicatorTaskId);
        sOptionMenuPlusUiState->scrollIndicatorTaskId = TASK_NONE;
    }
}

static void OptionMenuPlus_MoveCursor(s32 id, bool8 onInit, struct ListMenu *list)
{
    if (onInit != TRUE)
        PlaySE(SE_SELECT);
}

static void OptionMenuPlus_PrintMenuItem(u8 windowId, u32 id, u8 yOffset)
{ 
    u8 page = sOptionMenuPlusUiState->page;
    if (OptionMenuPlus_IsCancel(id)) return;
    StringCopy(gStringVar1, sOptionMenuPlus_Pages[page].options[id].options[sOptionMenuPlus_Pages[page].options[id].func(0)]);
    AddTextPrinterParameterized4(windowId, FONT_SHORT, OPTION_X, yOffset, 0, 0, sOptionMenuPlusWindowFontColors[FONT_BLACK], TEXT_SKIP_DRAW, gStringVar1);
}

static void OptionMenuPlus_UpdateMenuItem(void)
{
    u8 x = OPTION_X;
    u8 y = sOptionMenuPlusUiState->cursorPos * 14;
    u8 id = sOptionMenuPlusUiState->realCursorPos;
    u8 page = sOptionMenuPlusUiState->page;
    FillWindowPixelRect(WINDOW_OPTIONS, 1, x, y, 120, 14);
    StringCopy(gStringVar1, sOptionMenuPlus_Pages[page].options[id].options[sOptionMenuPlus_Pages[page].options[id].func(0)]);
    AddTextPrinterParameterized4(WINDOW_OPTIONS, FONT_SHORT, x, y, 0, 0, sOptionMenuPlusWindowFontColors[FONT_BLACK], TEXT_SKIP_DRAW, gStringVar1);
    PutWindowTilemap(WINDOW_OPTIONS);
    CopyWindowToVram(WINDOW_OPTIONS, COPYWIN_FULL);
}

static void OptionMenuPlus_PrintDescription(u8 id)
{
    struct Option option = sOptionMenuPlus_Pages[sOptionMenuPlusUiState->page].options[id];
    FillWindowPixelBuffer(WINDOW_DESC, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    if (!OptionMenuPlus_IsCancel(id))
    {
        if (option.sameDesc)
            StringCopy(gStringVar1, option.optionsDesc[0]);
        else
            StringCopy(gStringVar1, option.optionsDesc[option.func(0)]);
        AddTextPrinterParameterized4(WINDOW_DESC, FONT_SHORT, 0, 0, 0, 0, sOptionMenuPlusWindowFontColors[FONT_BLACK], TEXT_SKIP_DRAW, gStringVar1);
    }
    PutWindowTilemap(WINDOW_DESC);
    CopyWindowToVram(WINDOW_DESC, COPYWIN_FULL);
}

static void OptionMenuPlus_UpdateLeftRightScrollIndicator(void)
{
    u8 page = sOptionMenuPlusUiState->page;
    if (sOptionMenuPlusUiState->optionIndicatorTaskId != TASK_NONE)
    {
        RemoveScrollIndicatorArrowPair(sOptionMenuPlusUiState->optionIndicatorTaskId);
        sOptionMenuPlusUiState->optionIndicatorTaskId = TASK_NONE;
    }
    
    if (!OptionMenuPlus_IsCancel(sOptionMenuPlusUiState->realCursorPos))
    {
        sOptionMenuPlusUiState->optionIndicatorTaskId = AddScrollIndicatorArrowPairParameterized(SCROLL_ARROW_RIGHT, 63 + (sOptionMenuPlusUiState->cursorPos * 14), OPTION_X + 10, OPTION_X + 22 + GetStringWidth(FONT_SHORT, sOptionMenuPlus_Pages[page].options[sOptionMenuPlusUiState->realCursorPos].options[sOptionMenuPlusUiState->selectedVal], 0), sOptionMenuPlus_Pages[page].options[sOptionMenuPlusUiState->realCursorPos].optionCount - 1, TAG_SCROLL_ARROW, TAG_SCROLL_ARROW, &sOptionMenuPlusUiState->selectedVal);
    }
}

static void OptionMenuPlus_PrintPageName(void)
{
    u8 page = sOptionMenuPlusUiState->page;
    FillWindowPixelRect(WINDOW_PAGE, TEXT_COLOR_TRANSPARENT, 0, 0, 120, 14);
    StringCopy(gStringVar1, sOptionMenuPlus_Pages[page].name);
    AddTextPrinterParameterized4(WINDOW_PAGE, FONT_SMALL, 2, 0, 0, 0, sOptionMenuPickSwitchCancelTextColor, TEXT_SKIP_DRAW, gStringVar1);
    PutWindowTilemap(WINDOW_PAGE);
    CopyWindowToVram(WINDOW_PAGE, COPYWIN_FULL);
}

static void OptionMenuPlus_PrintControls(void)
{
    AddTextPrinterParameterized4(WINDOW_CONTROLS, FONT_SMALL, 4, 0, 0, 0, sOptionMenuPickSwitchCancelTextColor, TEXT_SKIP_DRAW, sText_PickSwitchCancel);
    PutWindowTilemap(WINDOW_CONTROLS);
    CopyWindowToVram(WINDOW_CONTROLS, COPYWIN_FULL);
}

static bool8 OptionMenuPlus_IsCancel(u8 id)
{
    u8 page = sOptionMenuPlusUiState->page;
    if (page == MENUPAGE_OVERWORLD && id == MENUITEM_CANCELOVERWORLD)
        return TRUE;
    else if (page == MENUPAGE_BATTLE && id == MENUITEM_CANCELBATTLE)
        return TRUE;
    else if (page == MENUPAGE_MUSIC && id == MENUITEM_CANCELMUSIC)
        return TRUE;
    return FALSE;
}

static bool8 OptionMenuPlus_GetPageCount(void)
{
    u8 page = sOptionMenuPlusUiState->page;
    if (page == MENUPAGE_OVERWORLD)
        return MENUITEM_COUNTOVERWORLD;
    else if (page == MENUPAGE_BATTLE)
        return MENUITEM_COUNTBATTLE;
    else if (page == MENUPAGE_MUSIC)
        return MENUITEM_COUNTMUSIC;
    return 0;
}

static void OptionMenuPlus_LoadNewPage(void)
{
    DestroyListMenuTask(sOptionMenuPlusUiState->listMenuTaskId, &sOptionMenuPlusUiState->itemsAbove, &sOptionMenuPlusUiState->cursorPos);
    sOptionMenuPlusUiState->itemsAbove = 0;
    sOptionMenuPlusUiState->cursorPos = 0;
    sOptionMenuPlusUiState->realCursorPos = 0;
    OptionMenuPlus_RemoveScrollIndicator();
    OptionMenuPlus_CreateListMenu();
    OptionMenuPlus_PrintPageName();
}

static void OptionMenuPlus_LoadOptionValues(void)
{
    sOptionMenuPlusUiState->textSpeed = gSaveBlock2Ptr->optionsTextSpeed;
    sOptionMenuPlusUiState->battleStyle = gSaveBlock2Ptr->optionsBattleSceneOff;
    sOptionMenuPlusUiState->battleScene = gSaveBlock2Ptr->optionsBattleStyle;
    sOptionMenuPlusUiState->sound = gSaveBlock2Ptr->optionsSound;
    sOptionMenuPlusUiState->buttonMode = gSaveBlock2Ptr->optionsButtonMode;
    sOptionMenuPlusUiState->frameType = gSaveBlock2Ptr->optionsWindowFrameType;

    sOptionMenuPlusUiState->wildMusic = gSaveBlock2Ptr->optionsWildMusic;
    sOptionMenuPlusUiState->trainerMusic = gSaveBlock2Ptr->optionsTrainerMusic;
    sOptionMenuPlusUiState->gymMusic = gSaveBlock2Ptr->optionsGymMusic;
    sOptionMenuPlusUiState->e4Music = gSaveBlock2Ptr->optionsE4Music;
    sOptionMenuPlusUiState->championMusic = gSaveBlock2Ptr->optionsChampionMusic;

    sOptionMenuPlusUiState->sendToBox = gSaveBlock2Ptr->optionsSendToBox;
    sOptionMenuPlusUiState->giveNickname = gSaveBlock2Ptr->optionsGiveNicknames;
}

static void OptionMenuPlus_SaveOptionValues(void)
{
    gSaveBlock2Ptr->optionsTextSpeed = sOptionMenuPlusUiState->textSpeed;
    gSaveBlock2Ptr->optionsBattleSceneOff = sOptionMenuPlusUiState->battleStyle;
    gSaveBlock2Ptr->optionsBattleStyle = sOptionMenuPlusUiState->battleScene;
    gSaveBlock2Ptr->optionsSound = sOptionMenuPlusUiState->sound;
    gSaveBlock2Ptr->optionsButtonMode = sOptionMenuPlusUiState->buttonMode;
    gSaveBlock2Ptr->optionsWindowFrameType = sOptionMenuPlusUiState->frameType;

    gSaveBlock2Ptr->optionsWildMusic = sOptionMenuPlusUiState->wildMusic;
    gSaveBlock2Ptr->optionsTrainerMusic = sOptionMenuPlusUiState->trainerMusic;
    gSaveBlock2Ptr->optionsGymMusic = sOptionMenuPlusUiState->gymMusic;
    gSaveBlock2Ptr->optionsE4Music = sOptionMenuPlusUiState->e4Music;
    gSaveBlock2Ptr->optionsSendToBox = sOptionMenuPlusUiState->sendToBox;
    gSaveBlock2Ptr->optionsGiveNicknames = sOptionMenuPlusUiState->giveNickname;
}

static u16 OptionMenuPlus_TextSpeedFunc(u8 value)
{
    if (value == 0) 
        return sOptionMenuPlusUiState->textSpeed;
    else 
    {
        sOptionMenuPlusUiState->textSpeed += value;
        return sOptionMenuPlusUiState->textSpeed;
    }
}

static u16 OptionMenuPlus_BattleSceneFunc(u8 value)
{
    if (value == 0) 
        return sOptionMenuPlusUiState->battleScene;
    else 
    {
        sOptionMenuPlusUiState->battleScene += value;
        return sOptionMenuPlusUiState->battleScene;
    }
}

static u16 OptionMenuPlus_BattleStyleFunc(u8 value)
{
    if (value == 0) 
        return sOptionMenuPlusUiState->battleStyle;
    else 
    {
        sOptionMenuPlusUiState->battleStyle += value;
        return sOptionMenuPlusUiState->battleStyle;
    }
}

static u16 OptionMenuPlus_SoundFunc(u8 value)
{
    if (value == 0) 
        return sOptionMenuPlusUiState->sound;
    else 
    {
        sOptionMenuPlusUiState->sound += value;
        return sOptionMenuPlusUiState->sound;
    }
}

static u16 OptionMenuPlus_ButtonModeFunc(u8 value)
{
    if (value == 0) 
        return sOptionMenuPlusUiState->buttonMode;
    else 
    {
        sOptionMenuPlusUiState->buttonMode += value;
        return sOptionMenuPlusUiState->buttonMode;
    }
}

static u16 OptionMenuPlus_FrameFunc(u8 value)
{
    if (value == 0) 
        return sOptionMenuPlusUiState->frameType;
    else 
    {
        sOptionMenuPlusUiState->frameType += value;
        return sOptionMenuPlusUiState->frameType;
    }
}

static u16 OptionMenuPlus_WildMusicFunc(u8 value)
{
    if (value == 0) 
        return sOptionMenuPlusUiState->wildMusic;
    else 
    {
        sOptionMenuPlusUiState->wildMusic += value;
        return sOptionMenuPlusUiState->wildMusic;
    }
}

static u16 OptionMenuPlus_TrainerMusicFunc(u8 value)
{
    if (value == 0) 
        return sOptionMenuPlusUiState->trainerMusic;
    else 
    {
        sOptionMenuPlusUiState->trainerMusic += value;
        return sOptionMenuPlusUiState->trainerMusic;
    }
}

static u16 OptionMenuPlus_GymMusicFunc(u8 value)
{
    if (value == 0) 
        return sOptionMenuPlusUiState->gymMusic;
    else 
    {
        sOptionMenuPlusUiState->gymMusic += value;
        return sOptionMenuPlusUiState->gymMusic;
    }
}

static u16 OptionMenuPlus_E4MusicFunc(u8 value)
{
    if (value == 0) 
        return sOptionMenuPlusUiState->e4Music;
    else 
    {
        sOptionMenuPlusUiState->e4Music += value;
        return sOptionMenuPlusUiState->e4Music;
    }
}

static u16 OptionMenuPlus_ChampionMusicFunc(u8 value)
{
    if (value == 0) 
        return sOptionMenuPlusUiState->championMusic;
    else 
    {
        sOptionMenuPlusUiState->championMusic += value;
        return sOptionMenuPlusUiState->championMusic;
    }
}

static u16 OptionMenuPlus_SendToBoxFunc(u8 value)
{
    if (value == 0) 
        return sOptionMenuPlusUiState->sendToBox;
    else 
    {
        sOptionMenuPlusUiState->sendToBox += value;
        return sOptionMenuPlusUiState->sendToBox;
    }
}

static u16 OptionMenuPlus_GiveNicknamesFunc(u8 value)
{
    if (value == 0) 
        return sOptionMenuPlusUiState->giveNickname;
    else 
    {
        sOptionMenuPlusUiState->giveNickname += value;
        return sOptionMenuPlusUiState->giveNickname;
    }
}