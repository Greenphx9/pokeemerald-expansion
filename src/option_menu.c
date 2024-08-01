#include "global.h"
#include "option_menu.h"
#include "bg.h"
#include "field_screen_effect.h"
#include "gpu_regs.h"
#include "international_string_util.h"
#include "main.h"
#include "malloc.h"
#include "menu.h"
#include "menu_helpers.h"
#include "overworld.h"
#include "palette.h"
#include "scanline_effect.h"
#include "sound.h"
#include "sprite.h"
#include "strings.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "text_window.h"
#include "window.h"
#include "gba/m4a_internal.h"
#include "constants/rgb.h"
#include "constants/songs.h"

// Menu items
enum
{
    MENUITEM_TEXTSPEED = 0,
    MENUITEM_BATTLESCENE,
    MENUITEM_BATTLESTYLE,
    MENUITEM_SOUND,
    MENUITEM_BUTTONMODE,
    MENUITEM_FRAMETYPE,
    MENUITEM_CANCEL_PG_1,
    MENUITEM_COUNT_PG_1,
};

enum
{
    MENUITEM_WILDMUSIC = 0,
    MENUITEM_TRAINERMUSIC,
    MENUITEM_LEADERMUSIC,
    MENUITEM_E4MUSIC,
    MENUITEM_CHAMPIONMUSIC,
    MENUITEM_CANCEL_PG_2,
    MENUITEM_COUNT_PG_2,
};

enum
{
    MENUITEM_FASTINTRO = 0,
    MENUITEM_FASTHPDRAIN,
    MENUITEM_FASTEXPGROW,
    MENUITEM_WILDSCALING,
    MENUITEM_CANCEL_PG_3,
    MENUITEM_COUNT_PG_3,
};

// Window Ids
enum
{
    WIN_TEXT_OPTION,
    WIN_OPTIONS
};

enum
{
    OPTIONS_PAGE_1,
    OPTIONS_PAGE_2,
    OPTIONS_PAGE_3,
    OPTIONS_PAGE_COUNT,
};

// RAM symbols
struct OptionMenu
{
    /*0x00*/ u16 option[OPTIONS_PAGE_COUNT][MENUITEM_COUNT_PG_1];
    /*0x0E*/ u16 cursorPos;
    /*0x10*/ u8 loadState;
    /*0x11*/ u8 state;
    /*0x12*/ u8 loadPaletteState;
    /*0x13*/ u8 page:2;
};

static EWRAM_DATA struct OptionMenu *sOptionMenuPtr = NULL;

//Function Declarataions
static void CB2_InitOptionMenu(void);
static void VBlankCB_OptionMenu(void);
static void OptionMenu_InitCallbacks(void);
static void OptionMenu_SetVBlankCallback(void);
static void CB2_OptionMenu(void);
static void SetOptionMenuTask(void);
static void InitOptionMenuBg(void);
static void OptionMenu_PickSwitchCancel(void);
static void OptionMenu_ResetSpriteData(void);
static bool8 LoadOptionMenuPalette(void);
static void Task_OptionMenu(u8 taskId);
static u8 OptionMenu_ProcessInput(void);
static void BufferOptionMenuString(u8 page, u8 selection);
static void CloseAndSaveOptionMenu(u8 taskId);
static void PrintOptionMenuHeader(void);
static void DrawOptionMenuBg(void);
static void LoadOptionMenuItemNames(void);
static void UpdateSettingSelectionDisplay(u16 selection);

// Data Definitions
static const struct WindowTemplate sOptionMenuWinTemplates[] =
{
    {
        .bg = 1,
        .tilemapLeft = 2,
        .tilemapTop = 3,
        .width = 26,
        .height = 2,
        .paletteNum = 1,
        .baseBlock = 2
    },
    {
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 7,
        .width = 26,
        .height = 12,
        .paletteNum = 1,
        .baseBlock = 0x36
    },
    {
        .bg = 2,
        .tilemapLeft = 0,
        .tilemapTop = 0,
        .width = 30,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 0x16e
    },
    DUMMY_WIN_TEMPLATE
};

static const struct BgTemplate sOptionMenuBgTemplates[] =
{
   {
       .bg = 1,
       .charBaseIndex = 1,
       .mapBaseIndex = 30,
       .screenSize = 0,
       .paletteMode = 0,
       .priority = 0,
       .baseTile = 0
   },
   {
       .bg = 0,
       .charBaseIndex = 1,
       .mapBaseIndex = 31,
       .screenSize = 0,
       .paletteMode = 0,
       .priority = 1,
       .baseTile = 0
   },
   {
       .bg = 2,
       .charBaseIndex = 1,
       .mapBaseIndex = 29,
       .screenSize = 0,
       .paletteMode = 0,
       .priority = 2,
       .baseTile = 0
   },
};

static const u16 sOptionMenuPalette[] = INCBIN_U16("graphics/misc/option_menu.gbapal");
static const u16 sOptionMenuItemCountsPg1[MENUITEM_COUNT_PG_1] = {3, 2, 2, 2, 2, 10, 0};
static const u16 sOptionMenuItemCountsPg2[MENUITEM_COUNT_PG_2] = {2, 2, 2, 2, 2, 0};
static const u16 sOptionMenuItemCountsPg3[MENUITEM_COUNT_PG_3] = {2, 2, 2, 2, 0};

static const u8 sText_WildMusic[] = _("Wild Music");
static const u8 sText_TrainerMusic[] = _("Trainer Music");
static const u8 sText_LeaderMusic[] = _("Leader Music");
static const u8 sText_E4Music[] = _("Elite 4 Music");
static const u8 sText_ChampionMusic[] = _("Champion Music");

static const u8 sText_FastIntro[] = _("Fast Intro");
static const u8 sText_FastHP[] = _("Fast HP Bars");
static const u8 sText_FastEXP[] = _("Fast EXP Bars");
static const u8 sText_ScaleWild[] = _("Scale Wild");

static const u8 *const sOptionMenuItemsNamesPg1[MENUITEM_COUNT_PG_1] =
{
    [MENUITEM_TEXTSPEED]   = gText_TextSpeed,
    [MENUITEM_BATTLESCENE] = gText_BattleScene,
    [MENUITEM_BATTLESTYLE] = gText_BattleStyle,
    [MENUITEM_SOUND]       = gText_Sound,
    [MENUITEM_BUTTONMODE]  = gText_ButtonMode,
    [MENUITEM_FRAMETYPE]   = gText_Frame,
    [MENUITEM_CANCEL_PG_1] = gText_OptionMenuCancel,
};

static const u8 *const sOptionMenuItemsNamesPg2[MENUITEM_COUNT_PG_2] =
{
    [MENUITEM_WILDMUSIC]  = sText_WildMusic,
    [MENUITEM_TRAINERMUSIC]  = sText_TrainerMusic,
    [MENUITEM_LEADERMUSIC]  = sText_LeaderMusic,
    [MENUITEM_E4MUSIC]  = sText_E4Music,
    [MENUITEM_CHAMPIONMUSIC]  = sText_ChampionMusic,
    [MENUITEM_CANCEL_PG_2] = gText_OptionMenuCancel,   
};

static const u8 *const sOptionMenuItemsNamesPg3[MENUITEM_COUNT_PG_3] =
{
    [MENUITEM_FASTINTRO] = sText_FastIntro,
    [MENUITEM_FASTHPDRAIN] = sText_FastHP,
    [MENUITEM_FASTEXPGROW] = sText_FastEXP,
    [MENUITEM_WILDSCALING] = sText_ScaleWild,
    [MENUITEM_CANCEL_PG_3] = gText_OptionMenuCancel,  
};
 

static const u8 *const sTextSpeedOptions[] =
{
    gText_TextSpeedSlow, 
    gText_TextSpeedMid, 
    gText_TextSpeedFast
};

static const u8 *const sBattleSceneOptions[] =
{
    gText_BattleSceneOn, 
    gText_BattleSceneOff
};

static const u8 *const sBattleStyleOptions[] =
{
    gText_BattleStyleShift,
    gText_BattleStyleSet
};

static const u8 *const sSoundOptions[] =
{
    gText_SoundMono, 
    gText_SoundStereo
};

static const u8 *const sButtonTypeOptions[] =
{
	gText_ButtonTypeLR,
	gText_ButtonTypeLEqualsA
};

static const u8 sText_FRLG[] = _("FR/LG");
static const u8 sText_Emerald[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}Emerald");

static const u8 *const sWildMusicOptions[] =
{
	sText_FRLG,
	sText_Emerald
};

static const u8 *const sOffOnOptions[] =
{
    gText_BattleSceneOff, 
    gText_BattleSceneOn
};

static const u8 sOptionMenuPickSwitchCancelTextColor[] = {TEXT_DYNAMIC_COLOR_6, TEXT_COLOR_WHITE, TEXT_COLOR_DARK_GRAY};
static const u8 sOptionMenuTextColor[] = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_LIGHT_RED, TEXT_COLOR_RED};

static const u8 sText_TextSpeedDesc[] = _("Changes how fast text draws.");
static const u8 sText_BattleSceneDesc[] = _("Enable/disable battle animations.");
static const u8 sText_BattleStyleDesc[] = _("If shift, you can switch before opponent.");
static const u8 sText_SoundDesc[] = _("Change how sound is outputted.");
static const u8 sText_ButtonModeDesc[] = _("Change what the {L_BUTTON} & {R_BUTTON} buttons do.");
static const u8 sText_FrameDesc[] = _("Change the message box frame.");
static const u8 sText_CancelDesc[] = _("Exit options menu.");

static const u8 sText_WildMusicDesc[] = _("Change music that plays in wild battles.");
static const u8 sText_LeaderMusicDesc[] = _("Change music that plays in leader battles.");
static const u8 sText_TrainerMusicDesc[] = _("Change music that plays in trainer battles.");
static const u8 sText_E4MusicDesc[] = _("Change music that plays in Elite 4 battles.");
static const u8 sText_ChampionMusicDesc[] = _("Change music that plays in champion battles.");

static const u8 sText_FastIntroDesc[] = _("Battle text plays while mon animation plays.");
static const u8 sText_FastHPDesc[] = _("HP bars move faster.");
static const u8 sText_FastEXPDesc[] = _("EXP bars move faster.");
static const u8 sText_ScaleWildDesc[] = _("Wild mon levels get scaled to your levels.");

static const u8 *const sOptionMenuItemsDescPg1[MENUITEM_COUNT_PG_1] =
{
    [MENUITEM_TEXTSPEED]   = sText_TextSpeedDesc,
    [MENUITEM_BATTLESCENE] = sText_BattleSceneDesc,
    [MENUITEM_BATTLESTYLE] = sText_BattleStyleDesc,
    [MENUITEM_SOUND]       = sText_SoundDesc,
    [MENUITEM_BUTTONMODE]  = sText_ButtonModeDesc,
    [MENUITEM_FRAMETYPE]   = sText_FrameDesc,
    [MENUITEM_CANCEL_PG_1] = sText_CancelDesc,
};

static const u8 *const sOptionMenuItemsDescPg2[MENUITEM_COUNT_PG_2] =
{
    [MENUITEM_WILDMUSIC]  = sText_WildMusicDesc,
    [MENUITEM_TRAINERMUSIC]  = sText_TrainerMusicDesc,
    [MENUITEM_LEADERMUSIC]  = sText_LeaderMusicDesc,
    [MENUITEM_E4MUSIC]  = sText_E4MusicDesc,
    [MENUITEM_CHAMPIONMUSIC]  = sText_ChampionMusicDesc,
    [MENUITEM_CANCEL_PG_2] = sText_CancelDesc,
};

static const u8 *const sOptionMenuItemsDescPg3[MENUITEM_COUNT_PG_3] =
{
    [MENUITEM_FASTINTRO] = sText_FastIntroDesc,
    [MENUITEM_FASTHPDRAIN] = sText_FastHPDesc,
    [MENUITEM_FASTEXPGROW] = sText_FastEXPDesc,
    [MENUITEM_WILDSCALING] = sText_ScaleWildDesc, 
    [MENUITEM_CANCEL_PG_3] = sText_CancelDesc,
};

// Functions
static void CB2_InitOptionMenu(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void VBlankCB_OptionMenu(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

void CB2_OptionsMenuFromStartMenu(void)
{
    u8 i;
    
    if (gMain.savedCallback == NULL)
        gMain.savedCallback = CB2_ReturnToFieldWithOpenMenu;
    sOptionMenuPtr = AllocZeroed(sizeof(struct OptionMenu));
    sOptionMenuPtr->loadState = 0;
    sOptionMenuPtr->loadPaletteState = 0;
    sOptionMenuPtr->state = 0;
    sOptionMenuPtr->cursorPos = 0;
    sOptionMenuPtr->option[OPTIONS_PAGE_1][MENUITEM_TEXTSPEED] = gSaveBlock2Ptr->optionsTextSpeed;
    sOptionMenuPtr->option[OPTIONS_PAGE_1][MENUITEM_BATTLESCENE] = gSaveBlock2Ptr->optionsBattleSceneOff;
    sOptionMenuPtr->option[OPTIONS_PAGE_1][MENUITEM_BATTLESTYLE] = gSaveBlock2Ptr->optionsBattleStyle;
    sOptionMenuPtr->option[OPTIONS_PAGE_1][MENUITEM_SOUND] = gSaveBlock2Ptr->optionsSound;
    sOptionMenuPtr->option[OPTIONS_PAGE_1][MENUITEM_BUTTONMODE] = gSaveBlock2Ptr->optionsButtonMode;
    sOptionMenuPtr->option[OPTIONS_PAGE_1][MENUITEM_FRAMETYPE] = gSaveBlock2Ptr->optionsWindowFrameType;
    sOptionMenuPtr->option[OPTIONS_PAGE_2][MENUITEM_WILDMUSIC] = gSaveBlock2Ptr->optionsWildMusic;
    sOptionMenuPtr->option[OPTIONS_PAGE_2][MENUITEM_TRAINERMUSIC] = gSaveBlock2Ptr->optionsTrainerMusic;
    sOptionMenuPtr->option[OPTIONS_PAGE_2][MENUITEM_LEADERMUSIC] = gSaveBlock2Ptr->optionsLeaderMusic;
    sOptionMenuPtr->option[OPTIONS_PAGE_2][MENUITEM_E4MUSIC] = gSaveBlock2Ptr->optionsE4Music;
    sOptionMenuPtr->option[OPTIONS_PAGE_2][MENUITEM_CHAMPIONMUSIC] = gSaveBlock2Ptr->optionsChampionMusic;
    sOptionMenuPtr->option[OPTIONS_PAGE_3][MENUITEM_FASTINTRO] = gSaveBlock2Ptr->optionsFastIntro;
    sOptionMenuPtr->option[OPTIONS_PAGE_3][MENUITEM_FASTHPDRAIN] = gSaveBlock2Ptr->optionsFastHP;
    sOptionMenuPtr->option[OPTIONS_PAGE_3][MENUITEM_FASTEXPGROW] = gSaveBlock2Ptr->optionsFastEXP;
    sOptionMenuPtr->option[OPTIONS_PAGE_3][MENUITEM_WILDSCALING] = gSaveBlock2Ptr->optionsWildScale;
    sOptionMenuPtr->page = OPTIONS_PAGE_1;
    
    for (i = 0; i < MENUITEM_COUNT_PG_1 - 1; i++)
    {
        if (sOptionMenuPtr->option[OPTIONS_PAGE_1][i] > (sOptionMenuItemCountsPg1[i]) - 1)
            sOptionMenuPtr->option[OPTIONS_PAGE_1][i] = 0;
    }
    for (i = 0; i < MENUITEM_COUNT_PG_2 - 1; i++)
    {
        if (sOptionMenuPtr->option[OPTIONS_PAGE_2][i] > (sOptionMenuItemCountsPg2[i]) - 1)
            sOptionMenuPtr->option[OPTIONS_PAGE_2][i] = 0;
    }
    for (i = 0; i < MENUITEM_COUNT_PG_3 - 1; i++)
    {
        if (sOptionMenuPtr->option[OPTIONS_PAGE_3][i] > (sOptionMenuItemCountsPg3[i]) - 1)
            sOptionMenuPtr->option[OPTIONS_PAGE_3][i] = 0;
    }
    SetMainCallback2(CB2_OptionMenu);
}

static void OptionMenu_InitCallbacks(void)
{
    SetVBlankCallback(NULL);
    SetHBlankCallback(NULL);
}

static void OptionMenu_SetVBlankCallback(void)
{
    SetVBlankCallback(VBlankCB_OptionMenu);
}

static void CB2_OptionMenu(void)
{
    u8 i, state;
    state = sOptionMenuPtr->state;
    switch (state)
    {
    case 0:
        OptionMenu_InitCallbacks();
        break;
    case 1:
        InitOptionMenuBg();
        break;
    case 2:
        OptionMenu_ResetSpriteData();
        break;
    case 3:
        if (LoadOptionMenuPalette() != TRUE)
            return;
        break;
    case 4:
        PrintOptionMenuHeader();
        break;
    case 5:
        DrawOptionMenuBg();
        break;
    case 6:
        LoadOptionMenuItemNames();
        break;
    case 7:
        for (i = 0; i < MENUITEM_COUNT_PG_1; i++)
            BufferOptionMenuString(OPTIONS_PAGE_1, i);
        break;
    case 8:
        UpdateSettingSelectionDisplay(sOptionMenuPtr->cursorPos);
        break;
    case 9:
        OptionMenu_PickSwitchCancel();
        break;
    default:
        SetOptionMenuTask();
		break;
    }
    sOptionMenuPtr->state++;
}

static void SetOptionMenuTask(void)
{
    CreateTask(Task_OptionMenu, 0);
    SetMainCallback2(CB2_InitOptionMenu);
}

static void InitOptionMenuBg(void)
{
    void *dest = (void *)VRAM;
    DmaClearLarge16(3, dest, VRAM_SIZE, 0x1000);    
    DmaClear32(3, (void *)OAM, OAM_SIZE);
    DmaClear16(3, (void *)PLTT, PLTT_SIZE);    
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_MODE_0);
    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sOptionMenuBgTemplates, NELEMS(sOptionMenuBgTemplates));
    ChangeBgX(0, 0, 0);
    ChangeBgY(0, 0, 0);
    ChangeBgX(1, 0, 0);
    ChangeBgY(1, 0, 0);
    ChangeBgX(2, 0, 0);
    ChangeBgY(2, 0, 0);
    ChangeBgX(3, 0, 0);
    ChangeBgY(3, 0, 0);
    InitWindows(sOptionMenuWinTemplates);
    DeactivateAllTextPrinters();
    SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_TGT1_BG0 | BLDCNT_EFFECT_BLEND | BLDCNT_EFFECT_LIGHTEN);
    SetGpuReg(REG_OFFSET_BLDY, BLDCNT_TGT1_BG1);
    SetGpuReg(REG_OFFSET_WININ, WININ_WIN0_BG0);
    SetGpuReg(REG_OFFSET_WINOUT, WINOUT_WIN01_BG0 | WINOUT_WIN01_BG1 | WINOUT_WIN01_BG2 | WINOUT_WIN01_CLR);
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_1D_MAP | DISPCNT_OBJ_ON | DISPCNT_WIN0_ON);
    ShowBg(0);
    ShowBg(1);
    ShowBg(2);
};

static u8 OptionMenu_GetPage(void)
{
    return sOptionMenuPtr->page;
}

static const u8 sText_General[] = _("General");
static const u8 sText_Music[] = _("Music");
static const u8 sText_Battle[] = _("Battle");

static const u8 *const sOptionMenuHeaderNames[OPTIONS_PAGE_COUNT] =
{
    sText_General,
    sText_Music,
    sText_Battle,
};

static void OptionMenu_PickSwitchCancel(void)
{
    s32 x;
    x = 0xE4 - GetStringWidth(FONT_SMALL, gText_PickSwitchCancel, 0);
    FillWindowPixelBuffer(2, PIXEL_FILL(15)); 
    AddTextPrinterParameterized3(2, FONT_SMALL, x, 0, sOptionMenuPickSwitchCancelTextColor, 0, gText_PickSwitchCancel);
    AddTextPrinterParameterized3(2, FONT_SMALL, 0x05, 0, sOptionMenuPickSwitchCancelTextColor, 0, sOptionMenuHeaderNames[OptionMenu_GetPage()]);
    PutWindowTilemap(2);
    CopyWindowToVram(2, COPYWIN_FULL);
}

static void OptionMenu_ResetSpriteData(void)
{
    ResetSpriteData();
    ResetPaletteFade();
    FreeAllSpritePalettes();
    ResetTasks();
    ScanlineEffect_Stop();
}

static bool8 LoadOptionMenuPalette(void)
{
    switch (sOptionMenuPtr->loadPaletteState)
    {
    case 0:
        LoadBgTiles(1, GetWindowFrameTilesPal(sOptionMenuPtr->option[OPTIONS_PAGE_1][MENUITEM_FRAMETYPE])->tiles, 0x120, 0x1AA);
        break;
    case 1:
        LoadPalette(GetWindowFrameTilesPal(sOptionMenuPtr->option[OPTIONS_PAGE_1][MENUITEM_FRAMETYPE])->pal, BG_PLTT_ID(2), PLTT_SIZE_4BPP);
        break;
    case 2:
        LoadPalette(sOptionMenuPalette, BG_PLTT_ID(1), sizeof(sOptionMenuPalette));
        LoadPalette(GetTextWindowPalette(2), BG_PLTT_ID(15), PLTT_SIZE_4BPP);
        break;
    case 3:
        LoadMessageBoxGfx_Kanto(1, 0x1B3, BG_PLTT_ID(3));
        break;
    default:
        return TRUE;
    }
    sOptionMenuPtr->loadPaletteState++;
    return FALSE;
}

static void Task_OptionMenu(u8 taskId)
{
    u8 i;
    switch (sOptionMenuPtr->loadState)
    {
    case 0:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0x10, 0, RGB_BLACK);
        OptionMenu_SetVBlankCallback();
        sOptionMenuPtr->loadState++;
        break;
    case 1:
        if (gPaletteFade.active)
            return;
        sOptionMenuPtr->loadState++;
        break;
    case 2:
        if (IsActiveOverworldLinkBusy() == TRUE)
            break;
        switch (OptionMenu_ProcessInput())
        {
        case 0:
            break;
        case 1:
            sOptionMenuPtr->loadState++;
            break;
        case 2:
            LoadBgTiles(1, GetWindowFrameTilesPal(sOptionMenuPtr->option[OPTIONS_PAGE_1][MENUITEM_FRAMETYPE])->tiles, 0x120, 0x1AA);
            LoadPalette(GetWindowFrameTilesPal(sOptionMenuPtr->option[OPTIONS_PAGE_1][MENUITEM_FRAMETYPE])->pal, BG_PLTT_ID(2), PLTT_SIZE_4BPP);
            BufferOptionMenuString(OptionMenu_GetPage(), sOptionMenuPtr->cursorPos);
            break;
        case 3:
            UpdateSettingSelectionDisplay(sOptionMenuPtr->cursorPos);
            break;
        case 4:
            BufferOptionMenuString(OptionMenu_GetPage(), sOptionMenuPtr->cursorPos);
            break;
        case 5:
            switch(OptionMenu_GetPage())
            {
                case OPTIONS_PAGE_1:
                    for(i = 0; i < MENUITEM_COUNT_PG_1; i++)
                        BufferOptionMenuString(OPTIONS_PAGE_1, i);
                    break;
                case OPTIONS_PAGE_2:
                    for(i = MENUITEM_WILDMUSIC; i < MENUITEM_COUNT_PG_2; i++)
                        BufferOptionMenuString(OPTIONS_PAGE_2, i);
                    break;
                case OPTIONS_PAGE_3:
                    for(i = MENUITEM_FASTINTRO; i < MENUITEM_COUNT_PG_3; i++)
                        BufferOptionMenuString(OPTIONS_PAGE_3, i);
                    break;
            }
            UpdateSettingSelectionDisplay(sOptionMenuPtr->cursorPos);
            OptionMenu_PickSwitchCancel();
            break;
        }
        break;
    case 3:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 0x10, RGB_BLACK);
        sOptionMenuPtr->loadState++;
        break;
    case 4:
        if (gPaletteFade.active)
            return;
        sOptionMenuPtr->loadState++;
        break;
    case 5:
        CloseAndSaveOptionMenu(taskId);
        break;
    }
}

static u16 GetOptionsPageCount(void)
{
    if (OptionMenu_GetPage() == OPTIONS_PAGE_1)
        return sOptionMenuItemCountsPg1[sOptionMenuPtr->cursorPos];
    else if (OptionMenu_GetPage() == OPTIONS_PAGE_2)
        return sOptionMenuItemCountsPg2[sOptionMenuPtr->cursorPos];
    else if (OptionMenu_GetPage() == OPTIONS_PAGE_3)
        return sOptionMenuItemCountsPg3[sOptionMenuPtr->cursorPos];
    return 0;
}

static u8 OptionMenu_ProcessInput(void)
{ 
    u16 current;
    u16 *curr;
    if (JOY_REPEAT(DPAD_RIGHT))
    {
        current = sOptionMenuPtr->option[OptionMenu_GetPage()][(sOptionMenuPtr->cursorPos)];
        if (current == (GetOptionsPageCount() - 1))
            sOptionMenuPtr->option[OptionMenu_GetPage()][sOptionMenuPtr->cursorPos] = 0;
        else
            sOptionMenuPtr->option[OptionMenu_GetPage()][sOptionMenuPtr->cursorPos] = current + 1;
        if (sOptionMenuPtr->cursorPos == MENUITEM_FRAMETYPE)
            return 2;
        else
            return 4;
    }
    else if (JOY_REPEAT(DPAD_LEFT))
    {
        curr = &sOptionMenuPtr->option[OptionMenu_GetPage()][sOptionMenuPtr->cursorPos];
        if (*curr == 0)
            *curr = GetOptionsPageCount() - 1;
        else
            --*curr;
        
        if (sOptionMenuPtr->cursorPos == MENUITEM_FRAMETYPE)
            return 2;
        else
            return 4;
    }
    else if (JOY_REPEAT(DPAD_UP))
    {
        if (sOptionMenuPtr->cursorPos == MENUITEM_TEXTSPEED && OptionMenu_GetPage() == OPTIONS_PAGE_1)
            sOptionMenuPtr->cursorPos = MENUITEM_CANCEL_PG_1;
        else if (sOptionMenuPtr->cursorPos == MENUITEM_WILDMUSIC && OptionMenu_GetPage() == OPTIONS_PAGE_2)
            sOptionMenuPtr->cursorPos = MENUITEM_CANCEL_PG_2;
        else if (sOptionMenuPtr->cursorPos == MENUITEM_FASTINTRO && OptionMenu_GetPage() == OPTIONS_PAGE_3)
            sOptionMenuPtr->cursorPos = MENUITEM_CANCEL_PG_3;
        else
            sOptionMenuPtr->cursorPos = sOptionMenuPtr->cursorPos - 1;
        PrintOptionMenuHeader();
        return 3;        
    }
    else if (JOY_REPEAT(DPAD_DOWN))
    {
        if (sOptionMenuPtr->cursorPos == MENUITEM_CANCEL_PG_1 && OptionMenu_GetPage() == OPTIONS_PAGE_1) 
            sOptionMenuPtr->cursorPos = MENUITEM_TEXTSPEED;
        else if (sOptionMenuPtr->cursorPos == MENUITEM_CANCEL_PG_2 && OptionMenu_GetPage() == OPTIONS_PAGE_2)
            sOptionMenuPtr->cursorPos = MENUITEM_WILDMUSIC;
        else if (sOptionMenuPtr->cursorPos == MENUITEM_CANCEL_PG_3 && OptionMenu_GetPage() == OPTIONS_PAGE_3)
            sOptionMenuPtr->cursorPos = MENUITEM_FASTINTRO;
        else
            sOptionMenuPtr->cursorPos = sOptionMenuPtr->cursorPos + 1;
        PrintOptionMenuHeader();
        return 3;
    }
    else if (JOY_REPEAT(L_BUTTON) || JOY_NEW(L_BUTTON))
    {
        if (OptionMenu_GetPage() != OPTIONS_PAGE_1)
        {
            sOptionMenuPtr->page--;
            if (sOptionMenuPtr->cursorPos >= MENUITEM_COUNT_PG_1)
                sOptionMenuPtr->cursorPos = MENUITEM_COUNT_PG_1 - 1;
        }
        LoadOptionMenuItemNames();
        PrintOptionMenuHeader();
        PlaySE(SE_SELECT);
        return 5;
    }
    else if (JOY_REPEAT(R_BUTTON) || JOY_NEW(R_BUTTON))
    {
        if (OptionMenu_GetPage() != OPTIONS_PAGE_3)
        {
            sOptionMenuPtr->page++;
            if (sOptionMenuPtr->cursorPos >= MENUITEM_COUNT_PG_2 && OptionMenu_GetPage() == OPTIONS_PAGE_2)
                sOptionMenuPtr->cursorPos = MENUITEM_COUNT_PG_2 - 1;
            else if (sOptionMenuPtr->cursorPos >= MENUITEM_COUNT_PG_3 && OptionMenu_GetPage() == OPTIONS_PAGE_3)
                sOptionMenuPtr->cursorPos = MENUITEM_COUNT_PG_3 - 1;
        }
        LoadOptionMenuItemNames();
        PrintOptionMenuHeader();
        PlaySE(SE_SELECT);
        return 5;
    }
    else if (JOY_NEW(B_BUTTON) || JOY_NEW(A_BUTTON))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static void BufferOptionMenuString(u8 page, u8 selection)
{
    u8 str[20];
    u8 buf[12];
    u8 dst[3];
    u8 x, y;
    
    memcpy(dst, sOptionMenuTextColor, 3);
    x = 0x82;
    y = ((GetFontAttribute(FONT_SHORT, FONTATTR_MAX_LETTER_HEIGHT) - 1) * selection) + 2;
    FillWindowPixelRect(1, 1, x, y, 0x46, GetFontAttribute(FONT_SHORT, FONTATTR_MAX_LETTER_HEIGHT));

    if (page == OPTIONS_PAGE_1)
    {
        switch (selection)
        {
        case MENUITEM_TEXTSPEED:
            AddTextPrinterParameterized3(1, FONT_SHORT, x, y, dst, -1, sTextSpeedOptions[sOptionMenuPtr->option[page][selection]]);
            break;
        case MENUITEM_BATTLESCENE:
            AddTextPrinterParameterized3(1, FONT_SHORT, x, y, dst, -1, sBattleSceneOptions[sOptionMenuPtr->option[page][selection]]);
            break;
        case MENUITEM_BATTLESTYLE:
            AddTextPrinterParameterized3(1, FONT_SHORT, x, y, dst, -1, sBattleStyleOptions[sOptionMenuPtr->option[page][selection]]);
            break;
        case MENUITEM_SOUND:
            AddTextPrinterParameterized3(1, FONT_SHORT, x, y, dst, -1, sSoundOptions[sOptionMenuPtr->option[page][selection]]);
            break;
        case MENUITEM_BUTTONMODE:
            AddTextPrinterParameterized3(1, FONT_SHORT, x, y, dst, -1, sButtonTypeOptions[sOptionMenuPtr->option[page][selection]]);
            break;
        case MENUITEM_FRAMETYPE:
            StringCopy(str, gText_FrameType);
            ConvertIntToDecimalStringN(buf, sOptionMenuPtr->option[OPTIONS_PAGE_1][selection] + 1, 1, 2);
            StringAppendN(str, buf, 3);
            AddTextPrinterParameterized3(1, FONT_SHORT, x, y, dst, -1, str);
            break;
        default:
            break;
        }
    }
    else if (page == OPTIONS_PAGE_2)
    {
        switch (selection)
        {
        case MENUITEM_WILDMUSIC:
        case MENUITEM_TRAINERMUSIC:
        case MENUITEM_LEADERMUSIC:
        case MENUITEM_E4MUSIC:
        case MENUITEM_CHAMPIONMUSIC:
            AddTextPrinterParameterized3(1, FONT_SHORT, x, y, dst, -1, sWildMusicOptions[sOptionMenuPtr->option[page][selection]]);
            break;
        default:
            break;
        }
    }
    else
    {
        switch (selection)
        {
        case MENUITEM_FASTINTRO:
        case MENUITEM_FASTHPDRAIN:
        case MENUITEM_FASTEXPGROW:
        case MENUITEM_WILDSCALING:
            AddTextPrinterParameterized3(1, FONT_SHORT, x, y, dst, -1, sOffOnOptions[sOptionMenuPtr->option[page][selection]]);
            break;
        default:
            break;
        } 
    }
    PutWindowTilemap(1);
    CopyWindowToVram(1, COPYWIN_FULL);
}

static void CloseAndSaveOptionMenu(u8 taskId)
{
    gFieldCallback = FieldCB_DefaultWarpExit;
    SetMainCallback2(gMain.savedCallback);
    FreeAllWindowBuffers();
    gSaveBlock2Ptr->optionsTextSpeed = sOptionMenuPtr->option[OPTIONS_PAGE_1][MENUITEM_TEXTSPEED];
    gSaveBlock2Ptr->optionsBattleSceneOff = sOptionMenuPtr->option[OPTIONS_PAGE_1][MENUITEM_BATTLESCENE];
    gSaveBlock2Ptr->optionsBattleStyle = sOptionMenuPtr->option[OPTIONS_PAGE_1][MENUITEM_BATTLESTYLE];
    gSaveBlock2Ptr->optionsSound = sOptionMenuPtr->option[OPTIONS_PAGE_1][MENUITEM_SOUND];
    gSaveBlock2Ptr->optionsButtonMode = sOptionMenuPtr->option[OPTIONS_PAGE_1][MENUITEM_BUTTONMODE];
    gSaveBlock2Ptr->optionsWindowFrameType = sOptionMenuPtr->option[OPTIONS_PAGE_1][MENUITEM_FRAMETYPE];
    gSaveBlock2Ptr->optionsWildMusic = sOptionMenuPtr->option[OPTIONS_PAGE_2][MENUITEM_WILDMUSIC];
    gSaveBlock2Ptr->optionsTrainerMusic = sOptionMenuPtr->option[OPTIONS_PAGE_2][MENUITEM_TRAINERMUSIC];
    gSaveBlock2Ptr->optionsLeaderMusic = sOptionMenuPtr->option[OPTIONS_PAGE_2][MENUITEM_LEADERMUSIC];
    gSaveBlock2Ptr->optionsE4Music = sOptionMenuPtr->option[OPTIONS_PAGE_2][MENUITEM_E4MUSIC];
    gSaveBlock2Ptr->optionsChampionMusic = sOptionMenuPtr->option[OPTIONS_PAGE_2][MENUITEM_CHAMPIONMUSIC];
    gSaveBlock2Ptr->optionsFastIntro = sOptionMenuPtr->option[OPTIONS_PAGE_3][MENUITEM_FASTINTRO]; 
    gSaveBlock2Ptr->optionsFastHP = sOptionMenuPtr->option[OPTIONS_PAGE_3][MENUITEM_FASTHPDRAIN]; 
    gSaveBlock2Ptr->optionsFastEXP = sOptionMenuPtr->option[OPTIONS_PAGE_3][MENUITEM_FASTEXPGROW];
    gSaveBlock2Ptr->optionsWildScale = sOptionMenuPtr->option[OPTIONS_PAGE_3][MENUITEM_WILDSCALING];
    SetPokemonCryStereo(gSaveBlock2Ptr->optionsSound);
    FREE_AND_SET_NULL(sOptionMenuPtr);
    DestroyTask(taskId);
}

static void PrintOptionMenuHeader(void)
{
    FillWindowPixelBuffer(0, PIXEL_FILL(1));
    if (OptionMenu_GetPage() == OPTIONS_PAGE_1)
        AddTextPrinterParameterized(WIN_TEXT_OPTION, FONT_SHORT_NARROW, sOptionMenuItemsDescPg1[sOptionMenuPtr->cursorPos], 8, 1, TEXT_SKIP_DRAW, NULL);
    else if (OptionMenu_GetPage() == OPTIONS_PAGE_2)
        AddTextPrinterParameterized(WIN_TEXT_OPTION, FONT_SHORT_NARROW, sOptionMenuItemsDescPg2[sOptionMenuPtr->cursorPos], 8, 1, TEXT_SKIP_DRAW, NULL);
    else if (OptionMenu_GetPage() == OPTIONS_PAGE_3)
        AddTextPrinterParameterized(WIN_TEXT_OPTION, FONT_SHORT_NARROW, sOptionMenuItemsDescPg3[sOptionMenuPtr->cursorPos], 8, 1, TEXT_SKIP_DRAW, NULL);
    PutWindowTilemap(0);
    CopyWindowToVram(0, COPYWIN_FULL);
}

static void DrawOptionMenuBg(void)
{
    u8 h;
    h = 2;
    
    FillBgTilemapBufferRect(1, 0x1B3, 1, 2, 1, 1, 3);
    FillBgTilemapBufferRect(1, 0x1B4, 2, 2, 0x1B, 1, 3);
    FillBgTilemapBufferRect(1, 0x1B5, 0x1C, 2, 1, 1, 3);
    FillBgTilemapBufferRect(1, 0x1B6, 1, 3, 1, h, 3);
    FillBgTilemapBufferRect(1, 0x1B8, 0x1C, 3, 1, h, 3);
    FillBgTilemapBufferRect(1, 0x1B9, 1, 5, 1, 1, 3);
    FillBgTilemapBufferRect(1, 0x1BA, 2, 5, 0x1B, 1, 3);
    FillBgTilemapBufferRect(1, 0x1BB, 0x1C, 5, 1, 1, 3);
    FillBgTilemapBufferRect(1, 0x1AA, 1, 6, 1, 1, h);
    FillBgTilemapBufferRect(1, 0x1AB, 2, 6, 0x1A, 1, h);
    FillBgTilemapBufferRect(1, 0x1AC, 0x1C, 6, 1, 1, h);
    FillBgTilemapBufferRect(1, 0x1AD, 1, 7, 1, 0x10, h);
    FillBgTilemapBufferRect(1, 0x1AF, 0x1C, 7, 1, 0x10, h);
    FillBgTilemapBufferRect(1, 0x1B0, 1, 0x13, 1, 1, h);
    FillBgTilemapBufferRect(1, 0x1B1, 2, 0x13, 0x1A, 1, h);
    FillBgTilemapBufferRect(1, 0x1B2, 0x1C, 0x13, 1, 1, h);
    CopyBgTilemapBufferToVram(1);
}

static void LoadOptionMenuItemNames(void)
{
    u8 i;
    
    FillWindowPixelBuffer(1, PIXEL_FILL(1));
    switch(OptionMenu_GetPage())
    {
        case OPTIONS_PAGE_1:
            for (i = 0; i < MENUITEM_COUNT_PG_1; i++)
            {
                AddTextPrinterParameterized(WIN_OPTIONS, FONT_SHORT, sOptionMenuItemsNamesPg1[i], 8, (u8)((i * (GetFontAttribute(FONT_SHORT, FONTATTR_MAX_LETTER_HEIGHT))) + 2) - i, TEXT_SKIP_DRAW, NULL);    
            }
            break;
        case OPTIONS_PAGE_2:
            for (i = 0; i < MENUITEM_COUNT_PG_2; i++)
            {
                AddTextPrinterParameterized(WIN_OPTIONS, FONT_SHORT, sOptionMenuItemsNamesPg2[i], 8, (u8)((i * (GetFontAttribute(FONT_SHORT, FONTATTR_MAX_LETTER_HEIGHT))) + 2) - i, TEXT_SKIP_DRAW, NULL);    
            }
            break;
        case OPTIONS_PAGE_3:
            for (i = 0; i < MENUITEM_COUNT_PG_3; i++)
            {
                AddTextPrinterParameterized(WIN_OPTIONS, FONT_SHORT, sOptionMenuItemsNamesPg3[i], 8, (u8)((i * (GetFontAttribute(FONT_SHORT, FONTATTR_MAX_LETTER_HEIGHT))) + 2) - i, TEXT_SKIP_DRAW, NULL);    
            }
            break;
    }
}

static void UpdateSettingSelectionDisplay(u16 selection)
{
    u16 maxLetterHeight, y;
    
    maxLetterHeight = GetFontAttribute(FONT_SHORT, FONTATTR_MAX_LETTER_HEIGHT);
    y = selection * (maxLetterHeight - 1) + 0x3A;
    SetGpuReg(REG_OFFSET_WIN0V, WIN_RANGE(y, y + maxLetterHeight));
    SetGpuReg(REG_OFFSET_WIN0H, WIN_RANGE(0x10, 0xE0));
}
