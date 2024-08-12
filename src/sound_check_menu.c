#include "global.h"
#include "bg.h"
#include "sprite.h"
#include "palette.h"
#include "task.h"
#include "m4a.h"
#include "main.h"
#include "text.h"
#include "menu.h"
#include "gpu_regs.h"
#include "scanline_effect.h"
#include "string_util.h"
#include "title_screen.h"
#include "sound.h"
#include "trainer_pokemon_sprites.h"
#include "constants/rgb.h"
#include "constants/songs.h"

#define tWindowSelected data[0]
#define tBgmIndex data[1]
#define tSeIndex data[2]
#define tBgmIndexOld data[3]
#define tSeIndexOld data[4]
#define tWhichSubmenu data[14]
#define tState data[15]

// wonky dim access macro
#define MULTI_DIM_ARR(x, dim, y) ((x) * dim + (y))

// dim access enums
enum
{
    B_8 = 1,
    B_16 = 2,
    B_32 = 4
};

// windows
enum
{
    WIN_INFO,
    WIN_MUS,
    WIN_SE,
    WIN_COUNT
};

// selected test
enum
{
    TEST_MUS,
    TEST_SE
};

// driver test cry enums
enum
{
    CRY_TEST_VOICE,
    CRY_TEST_VOLUME,
    CRY_TEST_PANPOT,
    CRY_TEST_PITCH,
    CRY_TEST_LENGTH,
    CRY_TEST_RELEASE,
    CRY_TEST_PROGRESS,
    CRY_TEST_CHORUS,
    CRY_TEST_PRIORITY
};

// minmax range enums
enum
{
    MIN,
    MAX
};

extern struct ToneData gCryTable[];
extern struct ToneData gCryTable_Reverse[];

static EWRAM_DATA u8 sIsFastForwarding = 0;
static EWRAM_DATA u8 sDriverTest_IsCryPlaying = 0;
static EWRAM_DATA u8 sDriverTest_IsCryPlayingOld = 0;
static EWRAM_DATA int sSoundTestParams[9] = {0};
static EWRAM_DATA u8 sDriverTest_Reverse = 0;
static EWRAM_DATA u8 sDriverTest_Stereo = 0;

struct MusicPlayerInfo *gMPlayInfo_Cry;
extern struct MusicPlayerInfo gMPlayInfo_BGM;

static const struct BgTemplate sSoundCheckMenuBgTemplates[] =
{
    {
        .bg = 0,
        .charBaseIndex = 0,
        .mapBaseIndex = 31,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0
    },
};

static const struct WindowTemplate sSoundCheckMenuWindowTemplates[] =
{
    {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 1,
        .width = 28,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 20,
    },
    {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 8,
        .width = 28,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 132,
    },
    {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 15,
        .width = 28,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 244,
    },
    DUMMY_WIN_TEMPLATE,
};

static const struct WindowTemplate sSoundCheckMenuDriverTestWindowTemplates[] =
{
    {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 1,
        .width = 28,
        .height = 18,
        .paletteNum = 15,
        .baseBlock = 20,
    },
    DUMMY_WIN_TEMPLATE,
};

static void Task_InitSoundCheckMenu(u8 taskId);
static void Task_InitSoundCheckMenu_CreateWindows(u8);
static void Task_HandleDrawingSoundCheckMenuText(u8);
static void Task_ProcessSoundCheckMenuInputAndRedraw(u8);
static void Task_ExitToTitleScreen(u8);
static void HighlightSelectedWindow(u8);
static void PrintSoundNumber(u16, u8);
static void PrintPaddedString(const u8 *const, u8);
static void Task_DrawSubmenu(u8);
static void Task_ProcessDriverTestInput(u8);
static void AdjustSelectedDriverParam(s8);
static void PrintDriverTestMenuText(void);
static void PrintSignedNumber(int, u16, u16, u8);
static void Task_DrawDriverTestMenu(u8);
static void Task_DrawPanTestMenu(u8);
static void Task_ProcessPanTestInput(u8);
static void PrintPanTestMenuText(void);
static void InitGpuRegs(void);
static void ClearTasksAndGraphicalStructs(void);
static void SetupDispcnt(void);
static void DestroyWindow(u8 windowId);
static void PutWindowTilemapAndCopyWindowToVram(u8 windowId);

static void CB2_SoundCheckMenu(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void VBlankCB_SoundCheckMenu(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();

    if (sIsFastForwarding != 0)
    {
        m4aSoundMain();
        m4aSoundMain();
        m4aSoundMain();
    }
}

void CB2_StartSoundCheckMenu(void)
{
    u8 taskId;

    switch (gMain.state)
    {
    case 0:
        SetVBlankCallback(NULL);
        InitGpuRegs();
        gMain.state++;
        break;
    case 1:
        ClearTasksAndGraphicalStructs();
        gMain.state++;
        break;
    case 2:
        ResetBgsAndClearDma3BusyFlags(0);
        InitBgsFromTemplates(0, sSoundCheckMenuBgTemplates, ARRAY_COUNT(sSoundCheckMenuBgTemplates));
        ResetBgPositions();
        gMain.state++;
        break;
    case 3:
        if (IsDma3ManagerBusyWithBgCopy() == TRUE)
            return;
        ShowBg(0);
        gMain.state++;
        break;
    case 4:
        InitWindows(sSoundCheckMenuWindowTemplates);
        DeactivateAllTextPrinters();
        LoadMessageBoxAndBorderGfx();
        gMain.state++;
        break;
    case 5:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
        gMain.state++;
        break;
    case 6:
        SetupDispcnt();
        SetGpuReg(REG_OFFSET_WININ, WININ_WIN0_BG0 |
                                    WININ_WIN0_OBJ |
                                    WININ_WIN1_BG0 |
                                    WININ_WIN1_OBJ);
        SetGpuReg(REG_OFFSET_WINOUT, WINOUT_WIN01_BG0 |
                                     WINOUT_WIN01_OBJ |
                                     WINOUT_WIN01_CLR);
        SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_TGT1_BG0 |
                                     BLDCNT_TGT1_BD |
                                     BLDCNT_EFFECT_DARKEN);
        SetGpuReg(REG_OFFSET_BLDALPHA, 0);
        SetGpuReg(REG_OFFSET_BLDY, 7);
        EnableInterrupts(INTR_FLAG_VBLANK);
        SetVBlankCallback(VBlankCB_SoundCheckMenu);
        SetMainCallback2(CB2_SoundCheckMenu);
        taskId = CreateTask(Task_InitSoundCheckMenu_CreateWindows, 0);
        gTasks[taskId].tWindowSelected = TEST_MUS;
        gTasks[taskId].tBgmIndex = 0;
        gTasks[taskId].tSeIndex = 0;
        gTasks[taskId].tBgmIndexOld = 0;
        gTasks[taskId].tSeIndexOld = 0;
        sIsFastForwarding = 0;
        m4aSoundInit();
        gMain.state++;
        break;
    }
}

static void Task_InitSoundCheckMenu_CreateWindows(u8 taskId) // SanitizeDayCareMailForRuby
{
    const u8 soundcheckStr[] = _("SOUND TEST{CLEAR_TO 120}A: PLAY  B: EXIT");
    const u8 bgmStr[] = _("MUSIC");
    const u8 seStr[] = _("SOUND EFFECTS");
    const u8 upDownStr[] = _("{LEFT_ARROW}PREV {RIGHT_ARROW}NEXT");
    const u8 driverStr[] = _("R: CRY TEST");

    if (!gPaletteFade.active)
    {
        SetStandardWindowBorderStyle(WIN_INFO, FALSE);
        AddTextPrinterParameterized(WIN_INFO, 2, soundcheckStr, 0, 0, TEXT_SKIP_DRAW, NULL);
        AddTextPrinterParameterized(WIN_INFO, 2, driverStr, 120, 14, TEXT_SKIP_DRAW, NULL);
        PutWindowTilemapAndCopyWindowToVram(WIN_INFO);
        SetStandardWindowBorderStyle(WIN_MUS, FALSE);
        AddTextPrinterParameterized(WIN_MUS, 2, bgmStr, 0, 0, TEXT_SKIP_DRAW, NULL);
        AddTextPrinterParameterized(WIN_MUS, 2, upDownStr, 100, 0, TEXT_SKIP_DRAW, NULL);
        PutWindowTilemapAndCopyWindowToVram(WIN_MUS);
        SetStandardWindowBorderStyle(WIN_SE, FALSE);
        AddTextPrinterParameterized(WIN_SE, 2, seStr, 0, 0, TEXT_SKIP_DRAW, NULL);
        AddTextPrinterParameterized(WIN_SE, 2, upDownStr, 100, 0, TEXT_SKIP_DRAW, NULL);
        PutWindowTilemapAndCopyWindowToVram(WIN_SE);
        SetGpuReg(REG_OFFSET_WIN0H, WIN_RANGE(0, 239));
        SetGpuReg(REG_OFFSET_WIN0V, WIN_RANGE(0, 48));
        HighlightSelectedWindow(WIN_INFO);
        gTasks[taskId].func = Task_HandleDrawingSoundCheckMenuText;
    }
}

static const u8 *const gBGMNames[];
static const u8 *const gSENames[];

static void Task_HandleDrawingSoundCheckMenuText(u8 taskId) // sub_080E85F4
{
    FillWindowPixelRect(WIN_MUS, PIXEL_FILL(1), 0, 14, 224, 12);
    PrintSoundNumber(gTasks[taskId].tBgmIndex + (START_MUS - 1), WIN_MUS); // print by BGM index
    PrintPaddedString(gBGMNames[gTasks[taskId].tBgmIndex], WIN_MUS);
    FillWindowPixelRect(WIN_SE, PIXEL_FILL(1), 0, 14, 224, 12);
    PrintSoundNumber(gTasks[taskId].tSeIndex, WIN_SE);
    PrintPaddedString(gSENames[gTasks[taskId].tSeIndex], WIN_SE);
    gTasks[taskId].func = Task_ProcessSoundCheckMenuInputAndRedraw;
}

static bool8 Task_ProcessSoundCheckMenuInput(u8 taskId) // sub_080E8688
{
    if (JOY_NEW(R_BUTTON)) // driver test
    {
        gTasks[taskId].tWhichSubmenu = 1;
        gTasks[taskId].tState = 0;
        gTasks[taskId].func = Task_DrawSubmenu;
    }
    else if (JOY_NEW(L_BUTTON))
    {
        gTasks[taskId].tWhichSubmenu = 0;
        gTasks[taskId].tState = 0;
        gTasks[taskId].func = Task_DrawSubmenu;
    }
    else if (JOY_NEW(A_BUTTON))
    {
        if (gTasks[taskId].tWindowSelected != TEST_MUS)
        {
            if (gTasks[taskId].tSeIndexOld != 0)
            {
                if (gTasks[taskId].tSeIndex != 0)
                {
                    m4aSongNumStop(gTasks[taskId].tSeIndexOld);
                    m4aSongNumStart(gTasks[taskId].tSeIndex);
                    gTasks[taskId].tSeIndexOld = gTasks[taskId].tSeIndex;
                }
                else
                {
                    m4aSongNumStop(gTasks[taskId].tSeIndexOld);
                    gTasks[taskId].tSeIndexOld = 0;
                }
            }
            else if (gTasks[taskId].tSeIndex != 0)
            {
                m4aSongNumStart(gTasks[taskId].tSeIndex);
                gTasks[taskId].tSeIndexOld = gTasks[taskId].tSeIndex;
            }
        }
        else
        {
            if (gTasks[taskId].tBgmIndexOld != 0)
            {
                if (gTasks[taskId].tBgmIndex != 0)
                {
                    m4aSongNumStop(gTasks[taskId].tBgmIndexOld + (START_MUS - 1));
                    m4aSongNumStart(gTasks[taskId].tBgmIndex + (START_MUS - 1));
                    gTasks[taskId].tBgmIndexOld = gTasks[taskId].tBgmIndex;
                }
                else
                {
                    m4aSongNumStop(gTasks[taskId].tBgmIndexOld + (START_MUS - 1));
                    gTasks[taskId].tBgmIndexOld = 0;
                }
            }
            else if (gTasks[taskId].tBgmIndex != 0)
            {
                m4aSongNumStart(gTasks[taskId].tBgmIndex + (START_MUS - 1));
                gTasks[taskId].tBgmIndexOld = gTasks[taskId].tBgmIndex;
            }
        }
    }
    else if (JOY_NEW(B_BUTTON))
    {
        m4aSongNumStart(SE_SELECT);
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        gTasks[taskId].func = Task_ExitToTitleScreen;
    }
    else if (JOY_REPEAT(DPAD_UP | DPAD_DOWN))
    {
        gTasks[taskId].tWindowSelected ^= 1;
        HighlightSelectedWindow(gTasks[taskId].tWindowSelected);
        return FALSE;
    }
    else if (JOY_REPEAT(DPAD_LEFT))
    {
        if (gTasks[taskId].tWindowSelected != TEST_MUS)
        {
            if (gTasks[taskId].tSeIndex > 0)
                gTasks[taskId].tSeIndex--;
            else
                gTasks[taskId].tSeIndex = END_SE;
        }
        else
        {
            if (gTasks[taskId].tBgmIndex > 0)
                gTasks[taskId].tBgmIndex--;
            else
                gTasks[taskId].tBgmIndex = (END_MUS - (START_MUS - 1));
        }
        return TRUE;
    }
    else if (JOY_REPEAT(DPAD_RIGHT))
    {
        if (gTasks[taskId].tWindowSelected != TEST_MUS)
        {
            if (gTasks[taskId].tSeIndex < END_SE)
                gTasks[taskId].tSeIndex++;
            else
                gTasks[taskId].tSeIndex = 0;
        }
        else
        {
            if (gTasks[taskId].tBgmIndex < (END_MUS - (START_MUS - 1)))
                gTasks[taskId].tBgmIndex++;
            else
                gTasks[taskId].tBgmIndex = 0;
        }
        return TRUE;
    }
    else if (JOY_HELD(SELECT_BUTTON))
    {
        sIsFastForwarding = 1;
    }
    else
    {
        sIsFastForwarding = 0;
    }
    return FALSE;
}

static void Task_ProcessSoundCheckMenuInputAndRedraw(u8 taskId) // sub_080E8878
{
    if (Task_ProcessSoundCheckMenuInput(taskId) != FALSE)
        gTasks[taskId].func = Task_HandleDrawingSoundCheckMenuText;
}

static void Task_ExitToTitleScreen(u8 taskId) // sub_080E88A8
{
    u8 windowId;

    if (!gPaletteFade.active)
    {
        for (windowId = WIN_INFO; windowId < WIN_COUNT; windowId++)
        {
            DestroyWindow(windowId);
        }
        DestroyTask(taskId);
        SetMainCallback2(CB2_InitTitleScreen);
    }
}

static void HighlightSelectedWindow(u8 windowType) // sub_080E88E8
{
    switch (windowType)
    {
    case WIN_INFO:
        SetGpuReg(REG_OFFSET_WIN1H, WIN_RANGE(0, 239));
        SetGpuReg(REG_OFFSET_WIN1V, WIN_RANGE(56, 104));
        break;
    case WIN_MUS:
        SetGpuReg(REG_OFFSET_WIN1H, WIN_RANGE(0, 239));
        SetGpuReg(REG_OFFSET_WIN1V, WIN_RANGE(112, 160));
        break;
    }
}

static void PrintSoundNumber(u16 soundIndex, u8 windowId) // sub_080E8928
{
    ConvertIntToDecimalStringN(gStringVar1, soundIndex, STR_CONV_MODE_RIGHT_ALIGN, 4);
    
    gStringVar1[4] = CHAR_COLON;
    gStringVar1[5] = EOS;
    
    AddTextPrinterParameterized(windowId, 2, gStringVar1, 0, 14, TEXT_SKIP_DRAW, NULL);
    PutWindowTilemapAndCopyWindowToVram(windowId);
}

static void PrintPaddedString(const u8 *const string, u8 windowId) // sub_080E8978
{
    u8 i;
    u8 str[32];

    for (i = 0; i < 31; i++)
        str[i] = CHAR_SPACE; // pad string.

    str[31] = EOS;

    for (i = 0; string[i] != EOS && i < 31; i++)
        str[i] = string[i];

    AddTextPrinterParameterized(windowId, 2, str, 32, 14, TEXT_SKIP_DRAW, NULL);
    PutWindowTilemapAndCopyWindowToVram(windowId);
}

static void Task_DrawSubmenu(u8 taskId) // sub_080E89EC
{
    u8 windowId;

    switch (gTasks[taskId].tState)
    {
    case 0:
        for (windowId = WIN_INFO; windowId < WIN_COUNT; windowId++)
        {
            DestroyWindow(windowId);
        }
        FreeAllWindowBuffers();
        ResetBgsAndClearDma3BusyFlags(0);
        InitBgsFromTemplates(0, sSoundCheckMenuBgTemplates, ARRAY_COUNT(sSoundCheckMenuBgTemplates));
        gTasks[taskId].tState++;
        break;
    case 1:
        if (IsDma3ManagerBusyWithBgCopy() == TRUE)
            return;
        InitWindows(sSoundCheckMenuDriverTestWindowTemplates);
        SetStandardWindowBorderStyle(0, TRUE);
        gTasks[taskId].tState++;
        break;
    case 2:
        ShowBg(0);
        SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_MODE_0 |
                                      DISPCNT_OBJ_1D_MAP |
                                      DISPCNT_BG0_ON |
                                      DISPCNT_OBJ_ON |
                                      DISPCNT_WIN0_ON);
        if (gTasks[taskId].tWhichSubmenu == 0)
        {
            gTasks[taskId].func = Task_DrawPanTestMenu;
        }
        else
        {
            gTasks[taskId].func = Task_DrawDriverTestMenu;
        }
        break;
    }
}

static void Task_DrawDriverTestMenu(u8 taskId) // sub_080E8AA0
{
    const u8 bbackStr[] = _("B BUTTON: BACK");
    const u8 aplayStr[] = _("A BUTTON: PLAY");
    const u8 voiceStr[] = _("VOICE…………");
    const u8 volumeStr[] = _("VOLUME………");
    const u8 panpotStr[] = _("PANPOT………");
    const u8 pitchStr[] = _("PITCH…………");
    const u8 lengthStr[] = _("LENGTH………");
    const u8 releaseStr[] = _("RELEASE……");
    const u8 progressStr[] = _("PROGRESS…");
    const u8 chorusStr[] = _("CHORUS………");
    const u8 priorityStr[] = _("PRIORITY…");
    const u8 playingStr[] = _("PLAYING");
    const u8 reverseStr[] = _("REVERSE");
    const u8 stereoStr[] = _("STEREO");

    SetStandardWindowBorderStyle(WIN_INFO, FALSE);
    AddTextPrinterParameterized(WIN_INFO, 2, bbackStr, 136, 16, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(WIN_INFO, 2, aplayStr, 136, 32, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(WIN_INFO, 2, playingStr, 136, 80, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(WIN_INFO, 2, reverseStr, 136, 96, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(WIN_INFO, 2, stereoStr, 136, 112, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(WIN_INFO, 2, voiceStr, 8, 0, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(WIN_INFO, 2, volumeStr, 8, 16, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(WIN_INFO, 2, panpotStr, 8, 32, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(WIN_INFO, 2, pitchStr, 8, 48, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(WIN_INFO, 2, lengthStr, 8, 64, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(WIN_INFO, 2, releaseStr, 8, 80, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(WIN_INFO, 2, progressStr, 8, 96, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(WIN_INFO, 2, chorusStr, 8, 112, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(WIN_INFO, 2, priorityStr, 8, 128, TEXT_SKIP_DRAW, NULL);
    SetGpuReg(REG_OFFSET_WIN0H, WIN_RANGE(0, DISPLAY_WIDTH));
    SetGpuReg(REG_OFFSET_WIN0V, WIN_RANGE(0, DISPLAY_HEIGHT));
    sDriverTest_IsCryPlaying = 0;
    sDriverTest_IsCryPlayingOld = 0;
    gMPlayInfo_Cry = NULL;
    sDriverTest_Reverse = 0;
    sDriverTest_Stereo = 1;
    sSoundTestParams[CRY_TEST_VOICE] = 0;
    sSoundTestParams[CRY_TEST_VOLUME] = 120;
    sSoundTestParams[CRY_TEST_PANPOT] = 0;
    sSoundTestParams[CRY_TEST_PITCH] = 15360;
    sSoundTestParams[CRY_TEST_LENGTH] = 180;
    sSoundTestParams[CRY_TEST_PROGRESS] = 0;
    sSoundTestParams[CRY_TEST_RELEASE] = 0;
    sSoundTestParams[CRY_TEST_CHORUS] = 0;
    sSoundTestParams[CRY_TEST_PRIORITY] = 2;
    PrintDriverTestMenuText();
    InitMenuNormal(WIN_INFO, 2, 0, 0, 16, 9, 0);
    gTasks[taskId].func = Task_ProcessDriverTestInput;
}

static void Task_ProcessDriverTestInput(u8 taskId) // sub_080E8D68
{
    if (JOY_NEW(B_BUTTON))
    {
        gTasks[taskId].tState = 0;
        gTasks[taskId].func = Task_InitSoundCheckMenu;
        return;
    }
    if (JOY_REPEAT(DPAD_UP))
    {
        Menu_MoveCursorNoWrapAround(-1);
        return;
    }
    if (JOY_REPEAT(DPAD_DOWN))
    {
        Menu_MoveCursorNoWrapAround(1);
        return;
    }
    if (JOY_NEW(START_BUTTON))
    {
        sDriverTest_Reverse ^= 1;
        PrintDriverTestMenuText();
        return;
    }
    if (JOY_NEW(SELECT_BUTTON))
    {
        sDriverTest_Stereo ^= 1;
        PrintDriverTestMenuText();
        SetPokemonCryStereo(sDriverTest_Stereo);
        return;
    }
    if (JOY_REPEAT(R_BUTTON))
    {
        AdjustSelectedDriverParam(10);
        PrintDriverTestMenuText();
        return;
    }
    if (JOY_REPEAT(L_BUTTON))
    {
        AdjustSelectedDriverParam(-10);
        PrintDriverTestMenuText();
        return;
    }
    if (JOY_REPEAT(DPAD_LEFT))
    {
        AdjustSelectedDriverParam(-1);
        PrintDriverTestMenuText();
        return;
    }
    if (JOY_REPEAT(DPAD_RIGHT))
    {
        AdjustSelectedDriverParam(1);
        PrintDriverTestMenuText();
        return;
    }
    if (JOY_NEW(A_BUTTON))
    {
        u8 divide, remaining;

        SetPokemonCryVolume(sSoundTestParams[CRY_TEST_VOLUME]);
        SetPokemonCryPanpot(sSoundTestParams[CRY_TEST_PANPOT]);
        SetPokemonCryPitch(sSoundTestParams[CRY_TEST_PITCH]);
        SetPokemonCryLength(sSoundTestParams[CRY_TEST_LENGTH]);
        SetPokemonCryProgress(sSoundTestParams[CRY_TEST_PROGRESS]);
        SetPokemonCryRelease(sSoundTestParams[CRY_TEST_RELEASE]);
        SetPokemonCryChorus(sSoundTestParams[CRY_TEST_CHORUS]);
        SetPokemonCryPriority(sSoundTestParams[CRY_TEST_PRIORITY]);

        remaining = sSoundTestParams[CRY_TEST_VOICE] % 128;
        divide = sSoundTestParams[CRY_TEST_VOICE] / 128;

        switch (divide)
        {
        case 0:
            if (sDriverTest_Reverse)
                gMPlayInfo_Cry = SetPokemonCryTone(&gCryTable_Reverse[(128 * 0) + remaining]);
            else
                gMPlayInfo_Cry = SetPokemonCryTone(&gCryTable[(128 * 0) + remaining]);
            break;
        case 1:
            if (sDriverTest_Reverse)
                gMPlayInfo_Cry = SetPokemonCryTone(&gCryTable_Reverse[(128 * 1) + remaining]);
            else
                gMPlayInfo_Cry = SetPokemonCryTone(&gCryTable[(128 * 1) + remaining]);
            break;
        case 2:
            if (sDriverTest_Reverse)
                gMPlayInfo_Cry = SetPokemonCryTone(&gCryTable_Reverse[(128 * 2) + remaining]);
            else
                gMPlayInfo_Cry = SetPokemonCryTone(&gCryTable[(128 * 2) + remaining]);
            break;
        case 3:
            if (sDriverTest_Reverse)
                gMPlayInfo_Cry = SetPokemonCryTone(&gCryTable_Reverse[(128 * 3) + remaining]);
            else
                gMPlayInfo_Cry = SetPokemonCryTone(&gCryTable[(128 * 3) + remaining]);
            break;
        }
    }

    if (gMPlayInfo_Cry != NULL)
    {
        sDriverTest_IsCryPlaying = IsPokemonCryPlaying(gMPlayInfo_Cry);

        if (sDriverTest_IsCryPlaying != sDriverTest_IsCryPlayingOld)
            PrintDriverTestMenuText();

        sDriverTest_IsCryPlayingOld = sDriverTest_IsCryPlaying;
    }
}

static void AdjustSelectedDriverParam(s8 delta) // sub_080E8FA4
{
    // also ideally should be a MinMax struct, but any attempt to make this into a struct causes it to not match due to the weird multi dim access.
    const int paramRanges[] =
    {
        0, 387,         // Voice
        0, 127,         // Volume
        -127, 127,      // Panpot
        -128, 32639,    // Pitch
        0, 65535,       // Length
        0, 255,         // Release
        0, 65535,       // Progress
        -64, 63,        // Chorus
        0, 65535        // Priority
    };

    u8 cursorPos = Menu_GetCursorPos();

    sSoundTestParams[cursorPos] += delta;

    if (sSoundTestParams[cursorPos] > paramRanges[MULTI_DIM_ARR(cursorPos, B_16, MAX)])
        sSoundTestParams[cursorPos] = paramRanges[MULTI_DIM_ARR(cursorPos, B_16, MIN)];

    if (sSoundTestParams[cursorPos] < paramRanges[MULTI_DIM_ARR(cursorPos, B_16, MIN)])
        sSoundTestParams[cursorPos] = paramRanges[MULTI_DIM_ARR(cursorPos, B_16, MAX)];
}

static void PrintDriverTestMenuText(void) // sub_080E900C
{
    PrintSignedNumber(sSoundTestParams[CRY_TEST_VOICE] + 1, 80, 0, 5);
    PrintSignedNumber(sSoundTestParams[CRY_TEST_VOLUME], 80, 16, 5);
    PrintSignedNumber(sSoundTestParams[CRY_TEST_PANPOT], 80, 32, 5);
    PrintSignedNumber(sSoundTestParams[CRY_TEST_PITCH], 80, 48, 5);
    PrintSignedNumber(sSoundTestParams[CRY_TEST_LENGTH], 80, 64, 5);
    PrintSignedNumber(sSoundTestParams[CRY_TEST_RELEASE], 80, 80, 5);
    PrintSignedNumber(sSoundTestParams[CRY_TEST_PROGRESS], 80, 96, 5);
    PrintSignedNumber(sSoundTestParams[CRY_TEST_CHORUS], 80, 112, 5);
    PrintSignedNumber(sSoundTestParams[CRY_TEST_PRIORITY], 80, 128, 5);
    PrintSignedNumber(sDriverTest_IsCryPlaying, 200, 80, 1);
    PrintSignedNumber(sDriverTest_Reverse, 200, 96, 1);
    PrintSignedNumber(sDriverTest_Stereo, 200, 112, 1);
}

static void PrintSignedNumber(int n, u16 x, u16 y, u8 digits) // sub_080E90C0
{
    const int powersOfTen[6] =
    {
              1,
             10,
            100,
           1000,
          10000,
         100000
    };

    u8 str[8];
    s8 i;
    s8 negative;
    s8 someVar2;

    for (i = 0; i <= digits; i++)
        str[i] = CHAR_SPACE;
    str[digits + 1] = EOS;

    negative = FALSE;
    if (n < 0)
    {
        n = -n;
        negative = TRUE;
    }

    if (digits == 1)
        someVar2 = TRUE;
    else
        someVar2 = FALSE;

    for (i = digits - 1; i >= 0; i--)
    {
        s8 d = n / powersOfTen[i];

        if (d != 0 || someVar2 || i == 0)
        {
            if (negative && !someVar2)
                str[digits - i - 1] = CHAR_HYPHEN;
            str[digits - i] = CHAR_0 + d;
            someVar2 = TRUE;
        }
        n %= powersOfTen[i];
    }

    AddTextPrinterParameterized(WIN_INFO, 2, str, x, y, TEXT_SKIP_DRAW, NULL);
    PutWindowTilemapAndCopyWindowToVram(WIN_INFO);
}

static const s8 gUnknown_08566E58[5] = { 0x3F, 0x00, 0xC0, 0x7F, 0x80 };

static void Task_DrawPanTestMenu(u8 taskId) // sub_080E91E4
{
    const u8 seStr[] = _("SOUND EFFECT");
    const u8 panStr[] = _("PAN");

    AddTextPrinterParameterized(WIN_INFO, 2, seStr, 10, 14, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(WIN_INFO, 2, panStr, 10, 28, TEXT_SKIP_DRAW, NULL);

    SetGpuReg(REG_OFFSET_WIN0H, WIN_RANGE(0, DISPLAY_WIDTH));
    SetGpuReg(REG_OFFSET_WIN0V, WIN_RANGE(0, DISPLAY_HEIGHT));
    sSoundTestParams[CRY_TEST_VOICE] = 1;
    sSoundTestParams[CRY_TEST_PANPOT] = 0;
    sSoundTestParams[CRY_TEST_CHORUS] = 0;
    sSoundTestParams[CRY_TEST_PROGRESS] = 0;
    sSoundTestParams[CRY_TEST_RELEASE] = 0;
    PrintPanTestMenuText();
    gTasks[taskId].func = Task_ProcessPanTestInput;
}

static void Task_ProcessPanTestInput(u8 taskId) // sub_080E9284
{
    if (sSoundTestParams[CRY_TEST_PROGRESS])
    {
        if (sSoundTestParams[CRY_TEST_RELEASE])
        {
            sSoundTestParams[CRY_TEST_RELEASE]--;
        }
        else
        {
            s8 panpot = gUnknown_08566E58[sSoundTestParams[CRY_TEST_PANPOT]];
            if (panpot != -128)
            {
                if (panpot == 127)
                {
                    sSoundTestParams[CRY_TEST_CHORUS] += 2;
                    if (sSoundTestParams[CRY_TEST_CHORUS] < 63)
                        SE12PanpotControl(sSoundTestParams[CRY_TEST_CHORUS]);
                }
            }
            else
            {
                sSoundTestParams[CRY_TEST_CHORUS] -= 2;
                if (sSoundTestParams[CRY_TEST_CHORUS] > -64)
                    SE12PanpotControl(sSoundTestParams[CRY_TEST_CHORUS]);
            }
        }
    }

    if (JOY_NEW(B_BUTTON))
    {
        gTasks[taskId].tState = 0;
        gTasks[taskId].func = Task_InitSoundCheckMenu;
        return;
    }
    if (JOY_NEW(A_BUTTON))
    {
        s8 panpot = gUnknown_08566E58[sSoundTestParams[CRY_TEST_PANPOT]];
        if (panpot != -128)
        {
            if (panpot == 127)
            {
                PlaySE12WithPanning(sSoundTestParams[CRY_TEST_VOICE], -64);
                sSoundTestParams[CRY_TEST_CHORUS] = -64;
                sSoundTestParams[CRY_TEST_PROGRESS] = 1;
                sSoundTestParams[CRY_TEST_RELEASE] = 30;
                return;
            }
        }
        else
        {
            PlaySE12WithPanning(sSoundTestParams[CRY_TEST_VOICE], 63);
            sSoundTestParams[CRY_TEST_CHORUS] = 63;
            sSoundTestParams[CRY_TEST_PROGRESS] = 1;
            sSoundTestParams[CRY_TEST_RELEASE] = 30;
            return;
        }

        PlaySE12WithPanning(sSoundTestParams[CRY_TEST_VOICE], panpot);
        sSoundTestParams[CRY_TEST_PROGRESS] = 0;
        return;
    }
    if (JOY_NEW(L_BUTTON))
    {
        sSoundTestParams[CRY_TEST_PANPOT]++;
        if (sSoundTestParams[CRY_TEST_PANPOT] > 4)
            sSoundTestParams[CRY_TEST_PANPOT] = 0;
        PrintPanTestMenuText();
    }
    if (JOY_NEW(R_BUTTON))
    {
        sSoundTestParams[CRY_TEST_PANPOT]--;
        if (sSoundTestParams[CRY_TEST_PANPOT] < 0)
            sSoundTestParams[CRY_TEST_PANPOT] = 4;
        PrintPanTestMenuText();
    }
    if (JOY_REPEAT(DPAD_RIGHT))
    {
        sSoundTestParams[CRY_TEST_VOICE]++;
        if (sSoundTestParams[CRY_TEST_VOICE] > PH_NURSE_SOLO)
            sSoundTestParams[CRY_TEST_VOICE] = MUS_DUMMY;
        PrintPanTestMenuText();
    }
    else if (JOY_REPEAT(DPAD_LEFT))
    {
        sSoundTestParams[CRY_TEST_VOICE]--;
        if (sSoundTestParams[CRY_TEST_VOICE] < MUS_DUMMY)
            sSoundTestParams[CRY_TEST_VOICE] = PH_NURSE_SOLO;
        PrintPanTestMenuText();
    }
}

static void Task_InitSoundCheckMenu(u8 taskId) // sub_080E9410
{
    switch (gTasks[taskId].tState)
    {
    case 0:
        DestroyWindow(WIN_INFO);
        FreeAllWindowBuffers();
        ResetBgsAndClearDma3BusyFlags(0);
        InitBgsFromTemplates(0, sSoundCheckMenuBgTemplates, ARRAY_COUNT(sSoundCheckMenuBgTemplates));
        gTasks[taskId].tState++;
        break;
    case 1:
        if (IsDma3ManagerBusyWithBgCopy() == TRUE)
            return;
        InitWindows(sSoundCheckMenuWindowTemplates);
        gTasks[taskId].tState++;
        break;
    case 2:
        ShowBg(0);
        SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_MODE_0 |
                                      DISPCNT_OBJ_1D_MAP |
                                      DISPCNT_BG0_ON |
                                      DISPCNT_OBJ_ON |
                                      DISPCNT_WIN0_ON |
                                      DISPCNT_WIN1_ON);
        SetGpuReg(REG_OFFSET_WIN0H, WIN_RANGE(17, 223));
        SetGpuReg(REG_OFFSET_WIN0V, WIN_RANGE(1, 31));
        gTasks[taskId].func = Task_InitSoundCheckMenu_CreateWindows;
        break;
    }
}

static void PrintPanTestMenuText(void) // sub_080E94B8
{
    u8 lrStr[] = _("   L");
    u8 rlStr[] = _("   R");

    FillWindowPixelRect(WIN_INFO, PIXEL_FILL(1), 100, 14, 3, 28);
    PrintSignedNumber(sSoundTestParams[CRY_TEST_VOICE], 100, 14, 3);

    switch (gUnknown_08566E58[sSoundTestParams[CRY_TEST_PANPOT]])
    {
    case 127:
        AddTextPrinterParameterized(WIN_INFO, 2, lrStr, 100, 28, TEXT_SKIP_DRAW, NULL);
        break;
    case -128:
        AddTextPrinterParameterized(WIN_INFO, 2, rlStr, 100, 28, TEXT_SKIP_DRAW, NULL);
        break;
    default:
        PrintSignedNumber(gUnknown_08566E58[sSoundTestParams[CRY_TEST_PANPOT]], 100, 28, 3);
        break;
    }
}

static void InitGpuRegs(void) // sub_080E9560
{
    DmaClear16(3, VRAM, VRAM_SIZE);
    DmaClear32(3, OAM, OAM_SIZE);
    DmaClear16(3, PLTT, PLTT_SIZE);
    SetGpuReg(REG_OFFSET_DISPCNT, 0);
    SetGpuReg(REG_OFFSET_BG0CNT, 0);
    SetGpuReg(REG_OFFSET_BG0HOFS, 0);
    SetGpuReg(REG_OFFSET_BG0VOFS, 0);
    SetGpuReg(REG_OFFSET_BG1CNT, 0);
    SetGpuReg(REG_OFFSET_BG1HOFS, 0);
    SetGpuReg(REG_OFFSET_BG1VOFS, 0);
    SetGpuReg(REG_OFFSET_BG2CNT, 0);
    SetGpuReg(REG_OFFSET_BG2HOFS, 0);
    SetGpuReg(REG_OFFSET_BG2VOFS, 0);
    SetGpuReg(REG_OFFSET_BG3CNT, 0);
    SetGpuReg(REG_OFFSET_BG3HOFS, 0);
    SetGpuReg(REG_OFFSET_BG3VOFS, 0);
    SetGpuReg(REG_OFFSET_WIN0H, 0);
    SetGpuReg(REG_OFFSET_WIN0V, 0);
    SetGpuReg(REG_OFFSET_WIN1H, 0);
    SetGpuReg(REG_OFFSET_WIN1V, 0);
    SetGpuReg(REG_OFFSET_WININ, 0);
    SetGpuReg(REG_OFFSET_WINOUT, 0);
    SetGpuReg(REG_OFFSET_BLDCNT, 0);
    SetGpuReg(REG_OFFSET_BLDALPHA, 0);
    SetGpuReg(REG_OFFSET_BLDY, 0);
}

static void ClearTasksAndGraphicalStructs(void) // sub_080E96A8
{
    ScanlineEffect_Stop();
    ResetTasks();
    ResetSpriteData();
    ResetAllPicSprites();
    ResetPaletteFade();
    FreeAllSpritePalettes();
}

static void SetupDispcnt(void) // sub_080E96C8
{
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_MODE_0 |
                                  DISPCNT_OBJ_1D_MAP |
                                  DISPCNT_BG0_ON |
                                  DISPCNT_OBJ_ON |
                                  DISPCNT_WIN0_ON |
                                  DISPCNT_WIN1_ON);
}

static void PutWindowTilemapAndCopyWindowToVram(u8 windowId) // sub_080E9734
{
    PutWindowTilemap(windowId);
    CopyWindowToVram(windowId, 3);
}

static void DestroyWindow(u8 windowId) // sub_080E9750
{
    FillWindowPixelBuffer(windowId, PIXEL_FILL(0));
    ClearWindowTilemap(windowId);
    CopyWindowToVram(windowId, 2);
    RemoveWindow(windowId);
}

#define SOUND_LIST_BGM \
    X(MUS_STOP) \
    X(MUS_LITTLEROOT_TEST) \
    X(MUS_GSC_ROUTE38) \
    X(MUS_CAUGHT) \
    X(MUS_VICTORY_WILD) \
    X(MUS_VICTORY_GYM_LEADER) \
    X(MUS_VICTORY_LEAGUE) \
    X(MUS_C_COMM_CENTER) \
    X(MUS_GSC_PEWTER) \
    X(MUS_C_VS_LEGEND_BEAST) \
    X(MUS_ROUTE101) \
    X(MUS_ROUTE110) \
    X(MUS_ROUTE120) \
    X(MUS_PETALBURG) \
    X(MUS_OLDALE) \
    X(MUS_GYM) \
    X(MUS_SURF) \
    X(MUS_PETALBURG_WOODS) \
    X(MUS_LEVEL_UP) \
    X(MUS_HEAL) \
    X(MUS_OBTAIN_BADGE) \
    X(MUS_OBTAIN_ITEM) \
    X(MUS_EVOLVED) \
    X(MUS_OBTAIN_TMHM) \
    X(MUS_LILYCOVE_MUSEUM) \
    X(MUS_ROUTE122) \
    X(MUS_OCEANIC_MUSEUM) \
    X(MUS_EVOLUTION_INTRO) \
    X(MUS_EVOLUTION) \
    X(MUS_MOVE_DELETED) \
    X(MUS_ENCOUNTER_GIRL) \
    X(MUS_ENCOUNTER_MALE) \
    X(MUS_ABANDONED_SHIP) \
    X(MUS_FORTREE) \
    X(MUS_BIRCH_LAB) \
    X(MUS_B_TOWER_RS) \
    X(MUS_ENCOUNTER_SWIMMER) \
    X(MUS_CAVE_OF_ORIGIN) \
    X(MUS_OBTAIN_BERRY) \
    X(MUS_AWAKEN_LEGEND) \
    X(MUS_SLOTS_JACKPOT) \
    X(MUS_SLOTS_WIN) \
    X(MUS_TOO_BAD) \
    X(MUS_ROULETTE) \
    X(MUS_LINK_CONTEST_P1) \
    X(MUS_LINK_CONTEST_P2) \
    X(MUS_LINK_CONTEST_P3) \
    X(MUS_LINK_CONTEST_P4) \
    X(MUS_ENCOUNTER_RICH) \
    X(MUS_VERDANTURF) \
    X(MUS_RUSTBORO) \
    X(MUS_POKE_CENTER) \
    X(MUS_ROUTE104) \
    X(MUS_ROUTE119) \
    X(MUS_CYCLING) \
    X(MUS_POKE_MART) \
    X(MUS_LITTLEROOT) \
    X(MUS_MT_CHIMNEY) \
    X(MUS_ENCOUNTER_FEMALE) \
    X(MUS_LILYCOVE) \
    X(MUS_ROUTE111) \
    X(MUS_HELP) \
    X(MUS_UNDERWATER) \
    X(MUS_VICTORY_TRAINER) \
    X(MUS_TITLE) \
    X(MUS_INTRO) \
    X(MUS_ENCOUNTER_MAY) \
    X(MUS_ENCOUNTER_INTENSE) \
    X(MUS_ENCOUNTER_COOL) \
    X(MUS_ROUTE113) \
    X(MUS_ENCOUNTER_AQUA) \
    X(MUS_FOLLOW_ME) \
    X(MUS_ENCOUNTER_BRENDAN) \
    X(MUS_EVER_GRANDE) \
    X(MUS_ENCOUNTER_SUSPICIOUS) \
    X(MUS_VICTORY_AQUA_MAGMA) \
    X(MUS_CABLE_CAR) \
    X(MUS_GAME_CORNER) \
    X(MUS_DEWFORD) \
    X(MUS_SAFARI_ZONE) \
    X(MUS_VICTORY_ROAD) \
    X(MUS_AQUA_MAGMA_HIDEOUT) \
    X(MUS_SAILING) \
    X(MUS_MT_PYRE) \
    X(MUS_SLATEPORT) \
    X(MUS_MT_PYRE_EXTERIOR) \
    X(MUS_SCHOOL) \
    X(MUS_HALL_OF_FAME) \
    X(MUS_FALLARBOR) \
    X(MUS_SEALED_CHAMBER) \
    X(MUS_CONTEST_WINNER) \
    X(MUS_CONTEST) \
    X(MUS_ENCOUNTER_MAGMA) \
    X(MUS_INTRO_BATTLE) \
    X(MUS_WEATHER_KYOGRE) \
    X(MUS_WEATHER_GROUDON) \
    X(MUS_SOOTOPOLIS) \
    X(MUS_CONTEST_RESULTS) \
    X(MUS_HALL_OF_FAME_ROOM) \
    X(MUS_TRICK_HOUSE) \
    X(MUS_ENCOUNTER_TWINS) \
    X(MUS_ENCOUNTER_ELITE_FOUR) \
    X(MUS_ENCOUNTER_HIKER) \
    X(MUS_CONTEST_LOBBY) \
    X(MUS_ENCOUNTER_INTERVIEWER) \
    X(MUS_ENCOUNTER_CHAMPION) \
    X(MUS_CREDITS) \
    X(MUS_END) \
    X(MUS_VS_WILD) \
    X(MUS_VS_AQUA_MAGMA) \
    X(MUS_VS_TRAINER) \
    X(MUS_VS_GYM_LEADER) \
    X(MUS_VS_CHAMPION) \
    X(MUS_VS_REGI) \
    X(MUS_VS_KYOGRE_GROUDON) \
    X(MUS_VS_RIVAL) \
    X(MUS_VS_ELITE_FOUR) \
    X(MUS_VS_AQUA_MAGMA_LEADER) \
    X(MUS_RG_FOLLOW_ME) \
    X(MUS_RG_GAME_CORNER) \
    X(MUS_RG_ROCKET_HIDEOUT) \
    X(MUS_RG_GYM) \
    X(MUS_RG_JIGGLYPUFF) \
    X(MUS_RG_INTRO_FIGHT) \
    X(MUS_RG_TITLE) \
    X(MUS_RG_CINNABAR) \
    X(MUS_RG_LAVENDER) \
    X(MUS_RG_HEAL) \
    X(MUS_RG_CYCLING) \
    X(MUS_RG_ENCOUNTER_ROCKET) \
    X(MUS_RG_ENCOUNTER_GIRL) \
    X(MUS_RG_ENCOUNTER_BOY) \
    X(MUS_RG_HALL_OF_FAME) \
    X(MUS_RG_VIRIDIAN_FOREST) \
    X(MUS_RG_MT_MOON) \
    X(MUS_RG_POKE_MANSION) \
    X(MUS_RG_CREDITS) \
    X(MUS_RG_ROUTE1) \
    X(MUS_RG_ROUTE24) \
    X(MUS_RG_ROUTE3) \
    X(MUS_RG_ROUTE11) \
    X(MUS_RG_VICTORY_ROAD) \
    X(MUS_RG_VS_GYM_LEADER) \
    X(MUS_RG_VS_TRAINER) \
    X(MUS_RG_VS_WILD) \
    X(MUS_RG_VS_CHAMPION) \
    X(MUS_RG_PALLET) \
    X(MUS_RG_OAK_LAB) \
    X(MUS_RG_OAK) \
    X(MUS_RG_POKE_CENTER) \
    X(MUS_RG_SS_ANNE) \
    X(MUS_RG_SURF) \
    X(MUS_RG_POKE_TOWER) \
    X(MUS_RG_SILPH) \
    X(MUS_RG_FUCHSIA) \
    X(MUS_RG_CELADON) \
    X(MUS_RG_VICTORY_TRAINER) \
    X(MUS_RG_VICTORY_WILD) \
    X(MUS_RG_VICTORY_GYM_LEADER) \
    X(MUS_RG_VERMILLION) \
    X(MUS_RG_PEWTER) \
    X(MUS_RG_ENCOUNTER_RIVAL) \
    X(MUS_RG_RIVAL_EXIT) \
    X(MUS_RG_DEX_RATING) \
    X(MUS_RG_OBTAIN_KEY_ITEM) \
    X(MUS_RG_CAUGHT_INTRO) \
    X(MUS_RG_PHOTO) \
    X(MUS_RG_GAME_FREAK) \
    X(MUS_RG_CAUGHT) \
    X(MUS_RG_NEW_GAME_INSTRUCT) \
    X(MUS_RG_NEW_GAME_INTRO) \
    X(MUS_RG_NEW_GAME_EXIT) \
    X(MUS_RG_POKE_JUMP) \
    X(MUS_RG_UNION_ROOM) \
    X(MUS_RG_NET_CENTER) \
    X(MUS_RG_MYSTERY_GIFT) \
    X(MUS_RG_BERRY_PICK) \
    X(MUS_RG_SEVII_CAVE) \
    X(MUS_RG_TEACHY_TV_SHOW) \
    X(MUS_RG_SEVII_ROUTE) \
    X(MUS_RG_SEVII_DUNGEON) \
    X(MUS_RG_SEVII_123) \
    X(MUS_RG_SEVII_45) \
    X(MUS_RG_SEVII_67) \
    X(MUS_RG_POKE_FLUTE) \
    X(MUS_RG_VS_DEOXYS) \
    X(MUS_RG_VS_MEWTWO) \
    X(MUS_RG_VS_LEGEND) \
    X(MUS_RG_ENCOUNTER_GYM_LEADER) \
    X(MUS_RG_ENCOUNTER_DEOXYS) \
    X(MUS_RG_TRAINER_TOWER) \
    X(MUS_RG_SLOW_PALLET) \
    X(MUS_RG_TEACHY_TV_MENU) \
    X(MUS_ABNORMAL_WEATHER) \
    X(MUS_B_FRONTIER) \
    X(MUS_B_ARENA) \
    X(MUS_OBTAIN_B_POINTS) \
    X(MUS_REGISTER_MATCH_CALL) \
    X(MUS_B_PYRAMID) \
    X(MUS_B_PYRAMID_TOP) \
    X(MUS_B_PALACE) \
    X(MUS_RAYQUAZA_APPEARS) \
    X(MUS_B_TOWER) \
    X(MUS_OBTAIN_SYMBOL) \
    X(MUS_B_DOME) \
    X(MUS_B_PIKE) \
    X(MUS_B_FACTORY) \
    X(MUS_VS_RAYQUAZA) \
    X(MUS_VS_FRONTIER_BRAIN) \
    X(MUS_VS_MEW) \
    X(MUS_B_DOME_LOBBY) \
    X(MUS_DP_TWINLEAF_DAY) \
    X(MUS_DP_SANDGEM_DAY) \
    X(MUS_DP_FLOAROMA_DAY) \
    X(MUS_DP_SOLACEON_DAY) \
    X(MUS_DP_ROUTE225_DAY) \
    X(MUS_DP_VALOR_LAKEFRONT_DAY) \
    X(MUS_DP_JUBILIFE_DAY) \
    X(MUS_DP_CANALAVE_DAY) \
    X(MUS_DP_OREBURGH_DAY) \
    X(MUS_DP_ETERNA_DAY) \
    X(MUS_DP_HEARTHOME_DAY) \
    X(MUS_DP_VEILSTONE_DAY) \
    X(MUS_DP_SUNYSHORE_DAY) \
    X(MUS_DP_SNOWPOINT_DAY) \
    X(MUS_DP_POKEMON_LEAGUE_DAY) \
    X(MUS_DP_FIGHT_AREA_DAY) \
    X(MUS_DP_ROUTE201_DAY) \
    X(MUS_DP_ROUTE203_DAY) \
    X(MUS_DP_ROUTE205_DAY) \
    X(MUS_DP_ROUTE206_DAY) \
    X(MUS_DP_ROUTE209_DAY) \
    X(MUS_DP_ROUTE210_DAY) \
    X(MUS_DP_ROUTE216_DAY) \
    X(MUS_DP_ROUTE228_DAY) \
    X(MUS_DP_ROWAN) \
    X(MUS_DP_TV_BROADCAST) \
    X(MUS_DP_TWINLEAF_NIGHT) \
    X(MUS_DP_SANDGEM_NIGHT) \
    X(MUS_DP_FLOAROMA_NIGHT) \
    X(MUS_DP_SOLACEON_NIGHT) \
    X(MUS_DP_ROUTE225_NIGHT) \
    X(MUS_DP_VALOR_LAKEFRONT_NIGHT) \
    X(MUS_DP_JUBILIFE_NIGHT) \
    X(MUS_DP_CANALAVE_NIGHT) \
    X(MUS_DP_OREBURGH_NIGHT) \
    X(MUS_DP_ETERNA_NIGHT) \
    X(MUS_DP_HEARTHOME_NIGHT) \
    X(MUS_DP_VEILSTONE_NIGHT) \
    X(MUS_DP_SUNYSHORE_NIGHT) \
    X(MUS_DP_SNOWPOINT_NIGHT) \
    X(MUS_DP_POKEMON_LEAGUE_NIGHT) \
    X(MUS_DP_FIGHT_AREA_NIGHT) \
    X(MUS_DP_ROUTE201_NIGHT) \
    X(MUS_DP_ROUTE203_NIGHT) \
    X(MUS_DP_ROUTE205_NIGHT) \
    X(MUS_DP_ROUTE206_NIGHT) \
    X(MUS_DP_ROUTE209_NIGHT) \
    X(MUS_DP_ROUTE210_NIGHT) \
    X(MUS_DP_ROUTE216_NIGHT) \
    X(MUS_DP_ROUTE228_NIGHT) \
    X(MUS_DP_UNDERGROUND) \
    X(MUS_DP_FLAG_CAPTURED) \
    X(MUS_DP_VICTORY_ROAD) \
    X(MUS_DP_ETERNA_FOREST) \
    X(MUS_DP_OLD_CHATEAU) \
    X(MUS_DP_LAKE_CAVERNS) \
    X(MUS_DP_AMITY_SQUARE) \
    X(MUS_DP_GALACTIC_HQ) \
    X(MUS_DP_GALACTIC_ETERNA_BUILDING) \
    X(MUS_DP_GREAT_MARSH) \
    X(MUS_DP_LAKE) \
    X(MUS_DP_MT_CORONET) \
    X(MUS_DP_SPEAR_PILLAR) \
    X(MUS_DP_STARK_MOUNTAIN) \
    X(MUS_DP_OREBURGH_GATE) \
    X(MUS_DP_OREBURGH_MINE) \
    X(MUS_DP_INSIDE_POKEMON_LEAGUE) \
    X(MUS_DP_HALL_OF_FAME_ROOM) \
    X(MUS_DP_POKE_CENTER_DAY) \
    X(MUS_DP_POKE_CENTER_NIGHT) \
    X(MUS_DP_GYM) \
    X(MUS_DP_ROWAN_LAB) \
    X(MUS_DP_CONTEST_LOBBY) \
    X(MUS_DP_POKE_MART) \
    X(MUS_DP_GAME_CORNER) \
    X(MUS_DP_B_TOWER) \
    X(MUS_DP_TV_STATION) \
    X(MUS_DP_GALACTIC_HQ_BASEMENT) \
    X(MUS_DP_AZURE_FLUTE) \
    X(MUS_DP_HALL_OF_ORIGIN) \
    X(MUS_DP_GTS) \
    X(MUS_DP_ENCOUNTER_BOY) \
    X(MUS_DP_ENCOUNTER_TWINS) \
    X(MUS_DP_ENCOUNTER_INTENSE) \
    X(MUS_DP_ENCOUNTER_GALACTIC) \
    X(MUS_DP_ENCOUNTER_LADY) \
    X(MUS_DP_ENCOUNTER_HIKER) \
    X(MUS_DP_ENCOUNTER_RICH) \
    X(MUS_DP_ENCOUNTER_SAILOR) \
    X(MUS_DP_ENCOUNTER_SUSPICIOUS) \
    X(MUS_DP_ENCOUNTER_ACE_TRAINER) \
    X(MUS_DP_ENCOUNTER_GIRL) \
    X(MUS_DP_ENCOUNTER_CYCLIST) \
    X(MUS_DP_ENCOUNTER_ARTIST) \
    X(MUS_DP_ENCOUNTER_ELITE_FOUR) \
    X(MUS_DP_ENCOUNTER_CHAMPION) \
    X(MUS_DP_VS_WILD) \
    X(MUS_DP_VS_GYM_LEADER) \
    X(MUS_DP_VS_UXIE_MESPRIT_AZELF) \
    X(MUS_DP_VS_TRAINER) \
    X(MUS_DP_VS_GALACTIC_BOSS) \
    X(MUS_DP_VS_DIALGA_PALKIA) \
    X(MUS_DP_VS_CHAMPION) \
    X(MUS_DP_VS_GALACTIC) \
    X(MUS_DP_VS_RIVAL) \
    X(MUS_DP_VS_ARCEUS) \
    X(MUS_DP_VS_LEGEND) \
    X(MUS_DP_VICTORY_WILD) \
    X(MUS_DP_VICTORY_TRAINER) \
    X(MUS_DP_VICTORY_GYM_LEADER) \
    X(MUS_DP_VICTORY_CHAMPION) \
    X(MUS_DP_VICTORY_GALACTIC) \
    X(MUS_DP_VICTORY_ELITE_FOUR) \
    X(MUS_DP_VS_GALACTIC_COMMANDER) \
    X(MUS_DP_CONTEST) \
    X(MUS_DP_VS_ELITE_FOUR) \
    X(MUS_DP_FOLLOW_ME) \
    X(MUS_DP_RIVAL) \
    X(MUS_DP_LAKE_EVENT) \
    X(MUS_DP_EVOLUTION) \
    X(MUS_DP_LUCAS) \
    X(MUS_DP_DAWN) \
    X(MUS_DP_LEGEND_APPEARS) \
    X(MUS_DP_CATASTROPHE) \
    X(MUS_DP_POKE_RADAR) \
    X(MUS_DP_SURF) \
    X(MUS_DP_CYCLING) \
    X(MUS_DP_LETS_GO_TOGETHER) \
    X(MUS_DP_TV_END) \
    X(MUS_DP_LEVEL_UP) \
    X(MUS_DP_EVOLVED) \
    X(MUS_DP_OBTAIN_KEY_ITEM) \
    X(MUS_DP_OBTAIN_ITEM) \
    X(MUS_DP_CAUGHT_INTRO) \
    X(MUS_DP_DEX_RATING) \
    X(MUS_DP_OBTAIN_BADGE) \
    X(MUS_DP_POKETCH) \
    X(MUS_DP_OBTAIN_TMHM) \
    X(MUS_DP_OBTAIN_ACCESSORY) \
    X(MUS_DP_MOVE_DELETED) \
    X(MUS_DP_HEAL) \
    X(MUS_DP_OBTAIN_BERRY) \
    X(MUS_DP_CONTEST_DRESS_UP) \
    X(MUS_DP_HALL_OF_FAME) \
    X(MUS_DP_INTRO) \
    X(MUS_DP_TITLE) \
    X(MUS_DP_MYSTERY_GIFT) \
    X(MUS_DP_WFC) \
    X(MUS_DP_DANCE_EASY) \
    X(MUS_DP_DANCE_DIFFICULT) \
    X(MUS_DP_CONTEST_RESULTS) \
    X(MUS_DP_CONTEST_WINNER) \
    X(MUS_DP_POFFINS) \
    X(MUS_DP_SLOTS_WIN) \
    X(MUS_DP_SLOTS_JACKPOT) \
    X(MUS_DP_CREDITS) \
    X(MUS_DP_SLOTS_UNUSED) \
    X(MUS_PL_TV_BROADCAST) \
    X(MUS_PL_TV_END) \
    X(MUS_PL_INTRO) \
    X(MUS_PL_TITLE) \
    X(MUS_PL_DISTORTION_WORLD) \
    X(MUS_PL_B_ARCADE) \
    X(MUS_PL_B_HALL) \
    X(MUS_PL_B_CASTLE) \
    X(MUS_PL_B_FACTORY) \
    X(MUS_PL_GLOBAL_TERMINAL) \
    X(MUS_PL_LILYCOVE_BOSSA_NOVA) \
    X(MUS_PL_LOOKER) \
    X(MUS_PL_VS_GIRATINA) \
    X(MUS_PL_VS_FRONTIER_BRAIN) \
    X(MUS_PL_VICTORY_FRONTIER_BRAIN) \
    X(MUS_PL_VS_REGI) \
    X(MUS_PL_CONTEST_COOL) \
    X(MUS_PL_CONTEST_SMART) \
    X(MUS_PL_CONTEST_CUTE) \
    X(MUS_PL_CONTEST_TOUGH) \
    X(MUS_PL_CONTEST_BEAUTY) \
    X(MUS_PL_SPIN_TRADE) \
    X(MUS_PL_WIFI_MINIGAMES) \
    X(MUS_PL_WIFI_PLAZA) \
    X(MUS_PL_WIFI_PARADE) \
    X(MUS_PL_GIRATINA_APPEARS_1) \
    X(MUS_PL_GIRATINA_APPEARS_2) \
    X(MUS_PL_MYSTERY_GIFT) \
    X(MUS_PL_TWINLEAF_MUSIC_BOX) \
    X(MUS_PL_OBTAIN_ARCADE_POINTS) \
    X(MUS_PL_OBTAIN_CASTLE_POINTS) \
    X(MUS_PL_OBTAIN_B_POINTS) \
    X(MUS_PL_WIN_MINIGAME) \
    X(MUS_HG_INTRO) \
    X(MUS_HG_TITLE) \
    X(MUS_HG_NEW_GAME) \
    X(MUS_HG_EVOLUTION) \
    X(MUS_HG_EVOLUTION_NO_INTRO) \
    X(MUS_HG_CYCLING) \
    X(MUS_HG_SURF) \
    X(MUS_HG_HALL_OF_FAME) \
    X(MUS_HG_CREDITS) \
    X(MUS_HG_END) \
    X(MUS_HG_NEW_BARK) \
    X(MUS_HG_CHERRYGROVE) \
    X(MUS_HG_VIOLET) \
    X(MUS_HG_AZALEA) \
    X(MUS_HG_GOLDENROD) \
    X(MUS_HG_ECRUTEAK) \
    X(MUS_HG_CIANWOOD) \
    X(MUS_HG_ROUTE29) \
    X(MUS_HG_ROUTE30) \
    X(MUS_HG_ROUTE34) \
    X(MUS_HG_ROUTE38) \
    X(MUS_HG_ROUTE42) \
    X(MUS_HG_VERMILION) \
    X(MUS_HG_PEWTER) \
    X(MUS_HG_CERULEAN) \
    X(MUS_HG_LAVENDER) \
    X(MUS_HG_CELADON) \
    X(MUS_HG_PALLET) \
    X(MUS_HG_CINNABAR) \
    X(MUS_HG_ROUTE1) \
    X(MUS_HG_ROUTE3) \
    X(MUS_HG_ROUTE11) \
    X(MUS_HG_ROUTE24) \
    X(MUS_HG_ROUTE26) \
    X(MUS_HG_POKE_CENTER) \
    X(MUS_HG_POKE_MART) \
    X(MUS_HG_GYM) \
    X(MUS_HG_ELM_LAB) \
    X(MUS_HG_OAK) \
    X(MUS_HG_DANCE_THEATER) \
    X(MUS_HG_GAME_CORNER) \
    X(MUS_HG_B_TOWER) \
    X(MUS_HG_B_TOWER_RECEPTION) \
    X(MUS_HG_SPROUT_TOWER) \
    X(MUS_HG_UNION_CAVE) \
    X(MUS_HG_RUINS_OF_ALPH) \
    X(MUS_HG_NATIONAL_PARK) \
    X(MUS_HG_BURNED_TOWER) \
    X(MUS_HG_BELL_TOWER) \
    X(MUS_HG_LIGHTHOUSE) \
    X(MUS_HG_TEAM_ROCKET_HQ) \
    X(MUS_HG_ICE_PATH) \
    X(MUS_HG_DRAGONS_DEN) \
    X(MUS_HG_ROCK_TUNNEL) \
    X(MUS_HG_VIRIDIAN_FOREST) \
    X(MUS_HG_VICTORY_ROAD) \
    X(MUS_HG_POKEMON_LEAGUE) \
    X(MUS_HG_FOLLOW_ME_1) \
    X(MUS_HG_FOLLOW_ME_2) \
    X(MUS_HG_ENCOUNTER_RIVAL) \
    X(MUS_HG_RIVAL_EXIT) \
    X(MUS_HG_BUG_CONTEST_PREP) \
    X(MUS_HG_BUG_CATCHING_CONTEST) \
    X(MUS_HG_RADIO_ROCKET) \
    X(MUS_HG_ROCKET_TAKEOVER) \
    X(MUS_HG_MAGNET_TRAIN) \
    X(MUS_HG_SS_AQUA) \
    X(MUS_HG_MT_MOON_SQUARE) \
    X(MUS_HG_RADIO_JINGLE) \
    X(MUS_HG_RADIO_LULLABY) \
    X(MUS_HG_RADIO_MARCH) \
    X(MUS_HG_RADIO_UNOWN) \
    X(MUS_HG_RADIO_POKE_FLUTE) \
    X(MUS_HG_RADIO_OAK) \
    X(MUS_HG_RADIO_BUENA) \
    X(MUS_HG_EUSINE) \
    X(MUS_HG_CLAIR) \
    X(MUS_HG_ENCOUNTER_GIRL_1) \
    X(MUS_HG_ENCOUNTER_BOY_1) \
    X(MUS_HG_ENCOUNTER_SUSPICIOUS_1) \
    X(MUS_HG_ENCOUNTER_SAGE) \
    X(MUS_HG_ENCOUNTER_KIMONO_GIRL) \
    X(MUS_HG_ENCOUNTER_ROCKET) \
    X(MUS_HG_ENCOUNTER_GIRL_2) \
    X(MUS_HG_ENCOUNTER_BOY_2) \
    X(MUS_HG_ENCOUNTER_SUSPICIOUS_2) \
    X(MUS_HG_VS_WILD) \
    X(MUS_HG_VS_TRAINER) \
    X(MUS_HG_VS_GYM_LEADER) \
    X(MUS_HG_VS_RIVAL) \
    X(MUS_HG_VS_ROCKET) \
    X(MUS_HG_VS_SUICUNE) \
    X(MUS_HG_VS_ENTEI) \
    X(MUS_HG_VS_RAIKOU) \
    X(MUS_HG_VS_CHAMPION) \
    X(MUS_HG_VS_WILD_KANTO) \
    X(MUS_HG_VS_TRAINER_KANTO) \
    X(MUS_HG_VS_GYM_LEADER_KANTO) \
    X(MUS_HG_VICTORY_TRAINER) \
    X(MUS_HG_VICTORY_WILD) \
    X(MUS_HG_CAUGHT) \
    X(MUS_HG_VICTORY_GYM_LEADER) \
    X(MUS_HG_VS_HO_OH) \
    X(MUS_HG_VS_LUGIA) \
    X(MUS_HG_POKEATHLON_LOBBY) \
    X(MUS_HG_POKEATHLON_START) \
    X(MUS_HG_POKEATHLON_BEFORE) \
    X(MUS_HG_POKEATHLON_EVENT) \
    X(MUS_HG_POKEATHLON_FINALS) \
    X(MUS_HG_POKEATHLON_RESULTS) \
    X(MUS_HG_POKEATHLON_END) \
    X(MUS_HG_POKEATHLON_WINNER) \
    X(MUS_HG_B_FACTORY) \
    X(MUS_HG_B_HALL) \
    X(MUS_HG_B_ARCADE) \
    X(MUS_HG_B_CASTLE) \
    X(MUS_HG_VS_FRONTIER_BRAIN) \
    X(MUS_HG_VICTORY_FRONTIER_BRAIN) \
    X(MUS_HG_WFC) \
    X(MUS_HG_MYSTERY_GIFT) \
    X(MUS_HG_WIFI_PLAZA) \
    X(MUS_HG_WIFI_MINIGAMES) \
    X(MUS_HG_WIFI_PARADE) \
    X(MUS_HG_GLOBAL_TERMINAL) \
    X(MUS_HG_SPIN_TRADE) \
    X(MUS_HG_GTS) \
    X(MUS_HG_ROUTE47) \
    X(MUS_HG_SAFARI_ZONE_GATE) \
    X(MUS_HG_SAFARI_ZONE) \
    X(MUS_HG_ETHAN) \
    X(MUS_HG_LYRA) \
    X(MUS_HG_GAME_CORNER_WIN) \
    X(MUS_HG_KIMONO_GIRL_DANCE) \
    X(MUS_HG_KIMONO_GIRL) \
    X(MUS_HG_HO_OH_APPEARS) \
    X(MUS_HG_LUGIA_APPEARS) \
    X(MUS_HG_SPIKY_EARED_PICHU) \
    X(MUS_HG_SINJOU_RUINS) \
    X(MUS_HG_RADIO_ROUTE101) \
    X(MUS_HG_RADIO_ROUTE201) \
    X(MUS_HG_RADIO_TRAINER) \
    X(MUS_HG_RADIO_VARIETY) \
    X(MUS_HG_VS_KYOGRE_GROUDON) \
    X(MUS_HG_POKEWALKER) \
    X(MUS_HG_VS_ARCEUS) \
    X(MUS_HG_HEAL) \
    X(MUS_HG_LEVEL_UP) \
    X(MUS_HG_OBTAIN_ITEM) \
    X(MUS_HG_OBTAIN_KEY_ITEM) \
    X(MUS_HG_EVOLVED) \
    X(MUS_HG_OBTAIN_BADGE) \
    X(MUS_HG_OBTAIN_TMHM) \
    X(MUS_HG_OBTAIN_ACCESSORY) \
    X(MUS_HG_MOVE_DELETED) \
    X(MUS_HG_OBTAIN_BERRY) \
    X(MUS_HG_DEX_RATING_1) \
    X(MUS_HG_DEX_RATING_2) \
    X(MUS_HG_DEX_RATING_3) \
    X(MUS_HG_DEX_RATING_4) \
    X(MUS_HG_DEX_RATING_5) \
    X(MUS_HG_DEX_RATING_6) \
    X(MUS_HG_OBTAIN_EGG) \
    X(MUS_HG_BUG_CONTEST_1ST_PLACE) \
    X(MUS_HG_BUG_CONTEST_2ND_PLACE) \
    X(MUS_HG_BUG_CONTEST_3RD_PLACE) \
    X(MUS_HG_CARD_FLIP) \
    X(MUS_HG_CARD_FLIP_GAME_OVER) \
    X(MUS_HG_POKEGEAR_REGISTERED) \
    X(MUS_HG_LETS_GO_TOGETHER) \
    X(MUS_HG_POKEATHLON_READY) \
    X(MUS_HG_POKEATHLON_1ST_PLACE) \
    X(MUS_HG_RECEIVE_POKEMON) \
    X(MUS_HG_OBTAIN_ARCADE_POINTS) \
    X(MUS_HG_OBTAIN_CASTLE_POINTS) \
    X(MUS_HG_OBTAIN_B_POINTS) \
    X(MUS_HG_WIN_MINIGAME) \
    X(MUS_BW_INTRO_1) \
    X(MUS_BW_INTRO_2) \
    X(MUS_BW_TITLE) \
    X(MUS_BW_NEW_GAME_1) \
    X(MUS_BW_NEW_GAME_2) \
    X(MUS_BW_EVOLUTION_INTRO) \
    X(MUS_BW_EVOLUTION) \
    X(MUS_BW_CYCLING) \
    X(MUS_BW_SURF) \
    X(MUS_BW_UNDERWATER) \
    X(MUS_BW_HALL_OF_FAME) \
    X(MUS_BW_CREDITS) \
    X(MUS_BW_NUVEMA) \
    X(MUS_BW_ACCUMULA) \
    X(MUS_BW_ANVILLE) \
    X(MUS_BW_LACUNOSA) \
    X(MUS_BW_UNDELLA_WINTER) \
    X(MUS_BW_UNDELLA_SUMMER) \
    X(MUS_BW_STRIATON) \
    X(MUS_BW_NACRENE) \
    X(MUS_BW_CASTELIA) \
    X(MUS_BW_NIMBASA) \
    X(MUS_BW_DRIFTVEIL) \
    X(MUS_BW_MISTRALTON) \
    X(MUS_BW_ICIRRUS) \
    X(MUS_BW_OPELUCID_BLACK) \
    X(MUS_BW_OPELUCID_WHITE) \
    X(MUS_BW_BLACK_CITY) \
    X(MUS_BW_WHITE_FOREST) \
    X(MUS_BW_ROUTE1) \
    X(MUS_BW_ROUTE2_SPRING) \
    X(MUS_BW_ROUTE2_SUMMER) \
    X(MUS_BW_ROUTE2_AUTUMN) \
    X(MUS_BW_ROUTE2_WINTER) \
    X(MUS_BW_ROUTE4_SPRING) \
    X(MUS_BW_ROUTE4_SUMMER) \
    X(MUS_BW_ROUTE4_AUTUMN) \
    X(MUS_BW_ROUTE4_WINTER) \
    X(MUS_BW_ROUTE6_SPRING) \
    X(MUS_BW_ROUTE6_SUMMER) \
    X(MUS_BW_ROUTE6_AUTUMN) \
    X(MUS_BW_ROUTE6_WINTER) \
    X(MUS_BW_ROUTE12_SPRING) \
    X(MUS_BW_ROUTE12_SUMMER) \
    X(MUS_BW_ROUTE12_AUTUMN) \
    X(MUS_BW_ROUTE12_WINTER) \
    X(MUS_BW_ROUTE10) \
    X(MUS_BW_SKYARROW_BRIDGE) \
    X(MUS_BW_DRIFTVEIL_DRAWBRIDGE) \
    X(MUS_BW_TUBELINE_BRIDGE) \
    X(MUS_BW_VILLAGE_BRIDGE) \
    X(MUS_BW_MARVELOUS_BRIDGE) \
    X(MUS_BW_POKE_CENTER) \
    X(MUS_BW_SHOPPING_MALL_NINE) \
    X(MUS_BW_GYM) \
    X(MUS_BW_JUNIPER_LAB) \
    X(MUS_BW_GATE) \
    X(MUS_BW_GEAR_STATION) \
    X(MUS_BW_BATTLE_SUBWAY) \
    X(MUS_BW_POKE_TRANSFER_LAB) \
    X(MUS_BW_ROYAL_UNOVA) \
    X(MUS_BW_ENTRALINK) \
    X(MUS_BW_SOMEONES_ENTRALINK) \
    X(MUS_BW_UNITY_TOWER) \
    X(MUS_BW_POKEMON_LEAGUE) \
    X(MUS_BW_N_CASTLE_BRIDGE) \
    X(MUS_BW_N_CASTLE) \
    X(MUS_BW_DREAMYARD) \
    X(MUS_BW_CHARGESTONE_CAVE) \
    X(MUS_BW_COLD_STORAGE) \
    X(MUS_BW_RELIC_CASTLE) \
    X(MUS_BW_DRAGONSPIRAL_TOWER) \
    X(MUS_BW_LOSTLORN_FOREST) \
    X(MUS_BW_DRAGONSPIRAL_TOWER_TOP) \
    X(MUS_BW_VICTORY_ROAD) \
    X(MUS_BW_FOLLOW_ME_1) \
    X(MUS_BW_FOLLOW_ME_2) \
    X(MUS_BW_CHEREN) \
    X(MUS_BW_BIANCA) \
    X(MUS_BW_JUNIPER) \
    X(MUS_BW_CEDRIC_JUNIPER) \
    X(MUS_BW_PLASMA) \
    X(MUS_BW_N) \
    X(MUS_BW_PLASMA_PLOTS) \
    X(MUS_BW_GHETSIS) \
    X(MUS_BW_RELIC_SONG) \
    X(MUS_BW_EMOTION) \
    X(MUS_BW_TROUBLE) \
    X(MUS_BW_FAREWELL) \
    X(MUS_BW_LOOKER) \
    X(MUS_BW_ALDER) \
    X(MUS_BW_OPENING) \
    X(MUS_BW_FERRIS_WHEEL) \
    X(MUS_BW_N_CASTLE_APPEARS) \
    X(MUS_BW_LEGEND_AWAKENS) \
    X(MUS_BW_MUSICAL_THEATER) \
    X(MUS_BW_MUSICAL_DRESS_UP) \
    X(MUS_BW_MUSICAL_BEFORE) \
    X(MUS_BW_MUSICAL_1) \
    X(MUS_BW_MUSICAL_2) \
    X(MUS_BW_MUSICAL_3) \
    X(MUS_BW_MUSICAL_4) \
    X(MUS_BW_ENCOUNTER_BOY) \
    X(MUS_BW_ENCOUNTER_GIRL) \
    X(MUS_BW_ENCOUNTER_TWINS) \
    X(MUS_BW_ENCOUNTER_ACE_TRAINER) \
    X(MUS_BW_ENCOUNTER_ROUGHNECK) \
    X(MUS_BW_ENCOUNTER_LADY) \
    X(MUS_BW_ENCOUNTER_CYCLIST) \
    X(MUS_BW_ENCOUNTER_PSYCHIC) \
    X(MUS_BW_ENCOUNTER_SUSPICIOUS) \
    X(MUS_BW_ENCOUNTER_POKEFAN) \
    X(MUS_BW_ENCOUNTER_HIKER) \
    X(MUS_BW_ENCOUNTER_CLERK) \
    X(MUS_BW_ENCOUNTER_RICH) \
    X(MUS_BW_ENCOUNTER_PLASMA) \
    X(MUS_BW_VS_WILD) \
    X(MUS_BW_VS_WILD_STRONG) \
    X(MUS_BW_VS_TRAINER) \
    X(MUS_BW_VS_SUBWAY_TRAINER) \
    X(MUS_BW_VS_GYM_LEADER) \
    X(MUS_BW_VS_RIVAL) \
    X(MUS_BW_VS_PLASMA) \
    X(MUS_BW_VS_ELITE_FOUR) \
    X(MUS_BW_VS_CHAMPION) \
    X(MUS_BW_VS_N) \
    X(MUS_BW_VS_N_FINAL) \
    X(MUS_BW_VS_GHETSIS) \
    X(MUS_BW_VS_RESHIRAM_ZEKROM) \
    X(MUS_BW_VS_KYUREM) \
    X(MUS_BW_VS_LEGEND) \
    X(MUS_BW_VS_CYNTHIA) \
    X(MUS_BW_LOW_HEALTH) \
    X(MUS_BW_GYM_LEADER_LAST_MON) \
    X(MUS_BW_VICTORY_WILD) \
    X(MUS_BW_VICTORY_TRAINER) \
    X(MUS_BW_VICTORY_GYM_LEADER) \
    X(MUS_BW_VICTORY_N_GHETSIS) \
    X(MUS_BW_VICTORY_CHAMPION) \
    X(MUS_BW_WFC) \
    X(MUS_BW_MYSTERY_GIFT) \
    X(MUS_BW_GAME_SYNC) \
    X(MUS_BW_GLOBAL_TERMINAL) \
    X(MUS_BW_SPIN_TRADE) \
    X(MUS_BW_GTS) \
    X(MUS_BW_WIFI_BATTLE_LOBBY) \
    X(MUS_BW_ENCOUNTER_CYNTHIA) \
    X(MUS_BW_CHEREN_BIANCA) \
    X(MUS_BW_ROCKET) \
    X(MUS_BW_VS_WORLD_CHAMPIONSHIPS) \
    X(MUS_BW_N_LEGEND_APPEARS) \
    X(MUS_BW_VICTORY_PLASMA) \
    X(MUS_BW_N_ROOM) \
    X(MUS_BW_MUSICAL_DLC_1) \
    X(MUS_BW_MUSICAL_DLC_2) \
    X(MUS_BW_MUSICAL_DLC_3) \
    X(MUS_BW_MUSICAL_DLC_4) \
    X(MUS_BW_MUSICAL_DLC_5) \
    X(MUS_BW_HEAL) \
    X(MUS_BW_LEVEL_UP) \
    X(MUS_BW_OBTAIN_ITEM) \
    X(MUS_BW_OBTAIN_KEY_ITEM) \
    X(MUS_BW_EVOLVED) \
    X(MUS_BW_OBTAIN_BADGE) \
    X(MUS_BW_OBTAIN_TMHM) \
    X(MUS_BW_OBTAIN_ACCESSORY) \
    X(MUS_BW_MOVE_DELETED) \
    X(MUS_BW_OBTAIN_BERRY) \
    X(MUS_BW_DEX_RATING_1) \
    X(MUS_BW_DEX_RATING_2) \
    X(MUS_BW_DEX_RATING_3) \
    X(MUS_BW_DEX_RATING_4) \
    X(MUS_BW_DEX_RATING_5) \
    X(MUS_BW_DEX_RATING_6) \
    X(MUS_BW_OBTAIN_EGG) \
    X(MUS_BW_OBTAIN_B_POINTS) \
    X(MUS_BW_FEELING_CHECK_3) \
    X(MUS_BW_FEELING_CHECK_2) \
    X(MUS_BW_FEELING_CHECK_1) \
    X(MUS_BW_FEELING_CHECK_START) \
    X(MUS_BW_MISSION_START) \
    X(MUS_BW_MISSION_CLEAR) \
    X(MUS_BW_MISSION_FAILED) \
    X(MUS_BW_OBTAIN_LIGHT_DARK_STONE) \
    X(MUS_BW_XTRANSCEIVER_CALL) \
    X(MUS_BW_ME_QUIZ) \
    X(MUS_BW_ME_DEL_POWER) \
    X(MUS_BW_POKE_TRANSFER_BOX) \
    X(MUS_BW_POKE_TRANSFER_GAME) \
    X(MUS_B2_INTRO) \
    X(MUS_B2_CREDITS) \
    X(MUS_B2_DRIFTVEIL_GYM) \
    X(MUS_B2_N_CASTLE) \
    X(MUS_B2_BLACK_TOWER_LOBBY) \
    X(MUS_B2_WHITE_TREEHOLLOW_LOBBY) \
    X(MUS_B2_BLACK_TOWER) \
    X(MUS_B2_WHITE_TREEHOLLOW) \
    X(MUS_B2_CHEREN) \
    X(MUS_B2_BIANCA) \
    X(MUS_B2_N) \
    X(MUS_B2_NIMBASA_GYM_1) \
    X(MUS_B2_NIMBASA_GYM_2) \
    X(MUS_B2_CASTELIA_GYM) \
    X(MUS_B2_VIRBANK_GYM) \
    X(MUS_B2_OPENING) \
    X(MUS_B2_ASPERTIA) \
    X(MUS_B2_VIRBANK) \
    X(MUS_B2_HUMILAU) \
    X(MUS_B2_ENCOUNTER_NEO_PLASMA) \
    X(MUS_B2_VS_WILD) \
    X(MUS_B2_VS_TRAINER) \
    X(MUS_B2_VS_GYM_LEADER) \
    X(MUS_B2_VS_SINNOH_LEGEND) \
    X(MUS_B2_VS_N) \
    X(MUS_B2_VS_UXIE_MESPRIT_AZELF) \
    X(MUS_B2_VS_REGI) \
    X(MUS_B2_GYM_LEADER_LAST_MON) \
    X(MUS_B2_N_ROOM) \
    X(MUS_B2_ROUTE19_SPRING) \
    X(MUS_B2_ROUTE19_SUMMER) \
    X(MUS_B2_ROUTE19_AUTUMN) \
    X(MUS_B2_ROUTE19_WINTER) \
    X(MUS_B2_ROUTE22_SPRING) \
    X(MUS_B2_ROUTE22_SUMMER) \
    X(MUS_B2_ROUTE22_AUTUMN) \
    X(MUS_B2_ROUTE22_WINTER) \
    X(MUS_B2_ROUTE23) \
    X(MUS_B2_ROAD_TO_REVERSAL_MOUNTAIN) \
    X(MUS_B2_FLOCCESY) \
    X(MUS_B2_LENTIMAS) \
    X(MUS_B2_FLOCCESY_RANCH) \
    X(MUS_B2_CASTELIA_SEWERS) \
    X(MUS_B2_REVERSAL_MOUNTAIN_WHITE) \
    X(MUS_B2_REVERSAL_MOUNTAIN_BLACK) \
    X(MUS_B2_STRANGE_HOUSE) \
    X(MUS_B2_PLASMA_FRIGATE_1) \
    X(MUS_B2_PLASMA_FRIGATE_2) \
    X(MUS_B2_PLASMA_FRIGATE_3) \
    X(MUS_B2_SEALED_CHAMBER) \
    X(MUS_B2_CAVE_OF_BEING) \
    X(MUS_B2_MARINE_TUBE) \
    X(MUS_B2_PWT_LOBBY) \
    X(MUS_B2_PWT) \
    X(MUS_B2_PWT_WINNER) \
    X(MUS_B2_POKESTAR_STUDIOS) \
    X(MUS_B2_POKESTAR_STUDIOS_FILMING) \
    X(MUS_B2_POKESTAR_STUDIOS_BATTLE) \
    X(MUS_B2_POKESTAR_STUDIOS_GLORY) \
    X(MUS_B2_POKESTAR_STUDIOS_DEFEAT) \
    X(MUS_B2_POKESTAR_STUDIOS_INVASION) \
    X(MUS_B2_POKESTAR_STUDIOS_SUCCESS) \
    X(MUS_B2_POKESTAR_STUDIOS_RETREAT) \
    X(MUS_B2_POKESTAR_STUDIOS_WEIRD) \
    X(MUS_B2_POKESTAR_STUDIOS_LOVE) \
    X(MUS_B2_POKESTAR_STUDIOS_SORROW) \
    X(MUS_B2_POKESTAR_STUDIOS_HORROR) \
    X(MUS_B2_POKESTAR_STUDIOS_PURIFIED) \
    X(MUS_B2_POKESTAR_STUDIOS_FEAR) \
    X(MUS_B2_POKESTAR_STUDIOS_HUMOR) \
    X(MUS_B2_POKESTAR_STUDIOS_DESPAIR) \
    X(MUS_B2_POKESTAR_STUDIOS_SHOCK) \
    X(MUS_B2_MISTRALTON_GYM) \
    X(MUS_B2_HUMILAU_GYM) \
    X(MUS_B2_OPELUCID_GYM) \
    X(MUS_B2_XTRANSCEIVER_MINIGAMES) \
    X(MUS_B2_UNOVA_LINK) \
    X(MUS_B2_JOIN_AVENUE) \
    X(MUS_B2_FUNFEST_MISSION) \
    X(MUS_B2_KYUREM_SCENE_1) \
    X(MUS_B2_KYUREM_SCENE_2) \
    X(MUS_B2_KYUREM_SCENE_3) \
    X(MUS_B2_KYUREM_SCENE_4) \
    X(MUS_B2_OPELUCID_ATTACKED) \
    X(MUS_B2_PLASMA_FRIGATE_SCENE) \
    X(MUS_B2_RIVAL) \
    X(MUS_B2_COLRESS) \
    X(MUS_B2_SHADOW_TRIAD) \
    X(MUS_B2_ZINZOLIN) \
    X(MUS_B2_OPELUCID_FROZEN) \
    X(MUS_B2_ENCOUNTER_BEAUTY) \
    X(MUS_B2_ENCOUNTER_CLOWN) \
    X(MUS_B2_VS_LEADER_KANTO) \
    X(MUS_B2_VS_CHAMPION_KANTO) \
    X(MUS_B2_VS_LEADER_JOHTO) \
    X(MUS_B2_VS_CHAMPION_JOHTO) \
    X(MUS_B2_VS_LEADER_HOENN) \
    X(MUS_B2_VS_CHAMPION_HOENN) \
    X(MUS_B2_VS_LEADER_SINNOH) \
    X(MUS_B2_VS_CHAMPION_SINNOH) \
    X(MUS_B2_VS_PWT_FINALS) \
    X(MUS_B2_VS_BLACK_WHITE_KYUREM) \
    X(MUS_B2_VS_NEO_PLASMA) \
    X(MUS_B2_VS_COLRESS) \
    X(MUS_B2_VS_GHETSIS) \
    X(MUS_B2_VS_IRIS) \
    X(MUS_B2_VS_RIVAL) \
    X(MUS_B2_VICTORY_PWT) \
    X(MUS_B2_HIDDEN_GROTTO) \
    X(MUS_B2_JOIN_AVENUE_GREETING) \
    X(MUS_B2_NEO_PLASMA) \
    X(MUS_B2_END) \
    X(MUS_B2_XTRANSCEIVER_GAME_START) \
    X(MUS_B2_XTRANSCEIVER_GAME_WIN) \
    X(MUS_B2_XTRANSCEIVER_03) \
    X(MUS_B2_XTRANSCEIVER_04) \
    X(MUS_B2_MEDAL_BOX_UPGRADED) \
    X(MUS_B2_PARTICIPATION_MEDAL) \
    X(MUS_B2_MEDAL_RALLY_COMPLETE) \
    X(MUS_B2_TYMPOLE_CHOIR) \
    X(MUS_B2_JOIN_AVENUE_RANK_UP_1) \
    X(MUS_B2_JOIN_AVENUE_RANK_UP_2) \
    X(MUS_B2_JOIN_AVENUE_RAFFLE) \
    X(MUS_B2_XTRANSCEIVER_CALL_LONG) \
    X(MUS_B2_HABITAT_LIST_1) \
    X(MUS_B2_HABITAT_LIST_2) \
    X(MUS_B2_POKESTAR_FANFARE) \

#define SOUND_LIST_SE \
    X(SE_STOP) \
    X(SE_USE_ITEM) \
    X(SE_PC_LOGIN) \
    X(SE_PC_OFF) \
    X(SE_PC_ON) \
    X(SE_SELECT) \
    X(SE_WIN_OPEN) \
    X(SE_WALL_HIT) \
    X(SE_DOOR) \
    X(SE_EXIT) \
    X(SE_LEDGE) \
    X(SE_BIKE_BELL) \
    X(SE_NOT_EFFECTIVE) \
    X(SE_EFFECTIVE) \
    X(SE_SUPER_EFFECTIVE) \
    X(SE_BALL_OPEN) \
    X(SE_FAINT) \
    X(SE_FLEE) \
    X(SE_SLIDING_DOOR) \
    X(SE_SHIP) \
    X(SE_BANG) \
    X(SE_PIN) \
    X(SE_BOO) \
    X(SE_BALL) \
    X(SE_CONTEST_PLACE) \
    X(SE_A) \
    X(SE_I) \
    X(SE_U) \
    X(SE_E) \
    X(SE_O) \
    X(SE_N) \
    X(SE_SUCCESS) \
    X(SE_FAILURE) \
    X(SE_EXP) \
    X(SE_BIKE_HOP) \
    X(SE_SWITCH) \
    X(SE_CLICK) \
    X(SE_FU_ZAKU) \
    X(SE_CONTEST_CONDITION_LOSE) \
    X(SE_LAVARIDGE_FALL_WARP) \
    X(SE_ICE_STAIRS) \
    X(SE_ICE_BREAK) \
    X(SE_ICE_CRACK) \
    X(SE_FALL) \
    X(SE_UNLOCK) \
    X(SE_WARP_IN) \
    X(SE_WARP_OUT) \
    X(SE_REPEL) \
    X(SE_ROTATING_GATE) \
    X(SE_TRUCK_MOVE) \
    X(SE_TRUCK_STOP) \
    X(SE_TRUCK_UNLOAD) \
    X(SE_TRUCK_DOOR) \
    X(SE_BERRY_BLENDER) \
    X(SE_CARD) \
    X(SE_SAVE) \
    X(SE_BALL_BOUNCE_1) \
    X(SE_BALL_BOUNCE_2) \
    X(SE_BALL_BOUNCE_3) \
    X(SE_BALL_BOUNCE_4) \
    X(SE_BALL_TRADE) \
    X(SE_BALL_THROW) \
    X(SE_NOTE_C) \
    X(SE_NOTE_D) \
    X(SE_NOTE_E) \
    X(SE_NOTE_F) \
    X(SE_NOTE_G) \
    X(SE_NOTE_A) \
    X(SE_NOTE_B) \
    X(SE_NOTE_C_HIGH) \
    X(SE_PUDDLE) \
    X(SE_BRIDGE_WALK) \
    X(SE_ITEMFINDER) \
    X(SE_DING_DONG) \
    X(SE_BALLOON_RED) \
    X(SE_BALLOON_BLUE) \
    X(SE_BALLOON_YELLOW) \
    X(SE_BREAKABLE_DOOR) \
    X(SE_MUD_BALL) \
    X(SE_FIELD_POISON) \
    X(SE_ESCALATOR) \
    X(SE_THUNDERSTORM) \
    X(SE_THUNDERSTORM_STOP) \
    X(SE_DOWNPOUR) \
    X(SE_DOWNPOUR_STOP) \
    X(SE_RAIN) \
    X(SE_RAIN_STOP) \
    X(SE_THUNDER) \
    X(SE_THUNDER2) \
    X(SE_ELEVATOR) \
    X(SE_LOW_HEALTH) \
    X(SE_EXP_MAX) \
    X(SE_ROULETTE_BALL) \
    X(SE_ROULETTE_BALL2) \
    X(SE_TAILLOW_WING_FLAP) \
    X(SE_SHOP) \
    X(SE_CONTEST_HEART) \
    X(SE_CONTEST_CURTAIN_RISE) \
    X(SE_CONTEST_CURTAIN_FALL) \
    X(SE_CONTEST_ICON_CHANGE) \
    X(SE_CONTEST_ICON_CLEAR) \
    X(SE_CONTEST_MONS_TURN) \
    X(SE_SHINY) \
    X(SE_INTRO_BLAST) \
    X(SE_MUGSHOT) \
    X(SE_APPLAUSE) \
    X(SE_VEND) \
    X(SE_ORB) \
    X(SE_DEX_SCROLL) \
    X(SE_DEX_PAGE) \
    X(SE_POKENAV_ON) \
    X(SE_POKENAV_OFF) \
    X(SE_DEX_SEARCH) \
    X(SE_EGG_HATCH) \
    X(SE_BALL_TRAY_ENTER) \
    X(SE_BALL_TRAY_BALL) \
    X(SE_BALL_TRAY_EXIT) \
    X(SE_GLASS_FLUTE) \
    X(SE_M_THUNDERBOLT) \
    X(SE_M_THUNDERBOLT2) \
    X(SE_M_HARDEN) \
    X(SE_M_NIGHTMARE) \
    X(SE_M_VITAL_THROW) \
    X(SE_M_VITAL_THROW2) \
    X(SE_M_BUBBLE) \
    X(SE_M_BUBBLE2) \
    X(SE_M_BUBBLE3) \
    X(SE_M_RAIN_DANCE) \
    X(SE_M_CUT) \
    X(SE_M_STRING_SHOT) \
    X(SE_M_STRING_SHOT2) \
    X(SE_M_ROCK_THROW) \
    X(SE_M_GUST) \
    X(SE_M_GUST2) \
    X(SE_M_DOUBLE_SLAP) \
    X(SE_M_DOUBLE_TEAM) \
    X(SE_M_RAZOR_WIND) \
    X(SE_M_ICY_WIND) \
    X(SE_M_THUNDER_WAVE) \
    X(SE_M_COMET_PUNCH) \
    X(SE_M_MEGA_KICK) \
    X(SE_M_MEGA_KICK2) \
    X(SE_M_CRABHAMMER) \
    X(SE_M_JUMP_KICK) \
    X(SE_M_FLAME_WHEEL) \
    X(SE_M_FLAME_WHEEL2) \
    X(SE_M_FLAMETHROWER) \
    X(SE_M_FIRE_PUNCH) \
    X(SE_M_TOXIC) \
    X(SE_M_SACRED_FIRE) \
    X(SE_M_SACRED_FIRE2) \
    X(SE_M_EMBER) \
    X(SE_M_TAKE_DOWN) \
    X(SE_M_BLIZZARD) \
    X(SE_M_BLIZZARD2) \
    X(SE_M_SCRATCH) \
    X(SE_M_VICEGRIP) \
    X(SE_M_WING_ATTACK) \
    X(SE_M_FLY) \
    X(SE_M_SAND_ATTACK) \
    X(SE_M_RAZOR_WIND2) \
    X(SE_M_BITE) \
    X(SE_M_HEADBUTT) \
    X(SE_M_SURF) \
    X(SE_M_HYDRO_PUMP) \
    X(SE_M_WHIRLPOOL) \
    X(SE_M_HORN_ATTACK) \
    X(SE_M_TAIL_WHIP) \
    X(SE_M_MIST) \
    X(SE_M_POISON_POWDER) \
    X(SE_M_BIND) \
    X(SE_M_DRAGON_RAGE) \
    X(SE_M_SING) \
    X(SE_M_PERISH_SONG) \
    X(SE_M_PAY_DAY) \
    X(SE_M_DIG) \
    X(SE_M_DIZZY_PUNCH) \
    X(SE_M_SELF_DESTRUCT) \
    X(SE_M_EXPLOSION) \
    X(SE_M_ABSORB_2) \
    X(SE_M_ABSORB) \
    X(SE_M_SCREECH) \
    X(SE_M_BUBBLE_BEAM) \
    X(SE_M_BUBBLE_BEAM2) \
    X(SE_M_SUPERSONIC) \
    X(SE_M_BELLY_DRUM) \
    X(SE_M_METRONOME) \
    X(SE_M_BONEMERANG) \
    X(SE_M_LICK) \
    X(SE_M_PSYBEAM) \
    X(SE_M_FAINT_ATTACK) \
    X(SE_M_SWORDS_DANCE) \
    X(SE_M_LEER) \
    X(SE_M_SWAGGER) \
    X(SE_M_SWAGGER2) \
    X(SE_M_HEAL_BELL) \
    X(SE_M_CONFUSE_RAY) \
    X(SE_M_SNORE) \
    X(SE_M_BRICK_BREAK) \
    X(SE_M_GIGA_DRAIN) \
    X(SE_M_PSYBEAM2) \
    X(SE_M_SOLAR_BEAM) \
    X(SE_M_PETAL_DANCE) \
    X(SE_M_TELEPORT) \
    X(SE_M_MINIMIZE) \
    X(SE_M_SKETCH) \
    X(SE_M_SWIFT) \
    X(SE_M_REFLECT) \
    X(SE_M_BARRIER) \
    X(SE_M_DETECT) \
    X(SE_M_LOCK_ON) \
    X(SE_M_MOONLIGHT) \
    X(SE_M_CHARM) \
    X(SE_M_CHARGE) \
    X(SE_M_STRENGTH) \
    X(SE_M_HYPER_BEAM) \
    X(SE_M_WATERFALL) \
    X(SE_M_REVERSAL) \
    X(SE_M_ACID_ARMOR) \
    X(SE_M_SANDSTORM) \
    X(SE_M_TRI_ATTACK) \
    X(SE_M_TRI_ATTACK2) \
    X(SE_M_ENCORE) \
    X(SE_M_ENCORE2) \
    X(SE_M_BATON_PASS) \
    X(SE_M_MILK_DRINK) \
    X(SE_M_ATTRACT) \
    X(SE_M_ATTRACT2) \
    X(SE_M_MORNING_SUN) \
    X(SE_M_FLATTER) \
    X(SE_M_SAND_TOMB) \
    X(SE_M_GRASSWHISTLE) \
    X(SE_M_SPIT_UP) \
    X(SE_M_DIVE) \
    X(SE_M_EARTHQUAKE) \
    X(SE_M_TWISTER) \
    X(SE_M_SWEET_SCENT) \
    X(SE_M_YAWN) \
    X(SE_M_SKY_UPPERCUT) \
    X(SE_M_STAT_INCREASE) \
    X(SE_M_HEAT_WAVE) \
    X(SE_M_UPROAR) \
    X(SE_M_HAIL) \
    X(SE_M_COSMIC_POWER) \
    X(SE_M_TEETER_DANCE) \
    X(SE_M_STAT_DECREASE) \
    X(SE_M_HAZE) \
    X(SE_M_HYPER_BEAM2) \
    X(SE_RG_DOOR) \
    X(SE_RG_CARD_FLIP) \
    X(SE_RG_CARD_FLIPPING) \
    X(SE_RG_CARD_OPEN) \
    X(SE_RG_BAG_CURSOR) \
    X(SE_RG_BAG_POCKET) \
    X(SE_RG_BALL_CLICK) \
    X(SE_RG_SHOP) \
    X(SE_RG_SS_ANNE_HORN) \
    X(SE_RG_HELP_OPEN) \
    X(SE_RG_HELP_CLOSE) \
    X(SE_RG_HELP_ERROR) \
    X(SE_RG_DEOXYS_MOVE) \
    X(SE_RG_POKE_JUMP_SUCCESS) \
    X(SE_RG_POKE_JUMP_FAILURE) \
    X(SE_PHONE_CALL) \
    X(SE_PHONE_CLICK) \
    X(SE_ARENA_TIMEUP1) \
    X(SE_ARENA_TIMEUP2) \
    X(SE_PIKE_CURTAIN_CLOSE) \
    X(SE_PIKE_CURTAIN_OPEN) \
    X(SE_SUDOWOODO_SHAKE) \
    X(PH_TRAP_BLEND) \
    X(PH_TRAP_HELD) \
    X(PH_TRAP_SOLO) \
    X(PH_FACE_BLEND) \
    X(PH_FACE_HELD) \
    X(PH_FACE_SOLO) \
    X(PH_CLOTH_BLEND) \
    X(PH_CLOTH_HELD) \
    X(PH_CLOTH_SOLO) \
    X(PH_DRESS_BLEND) \
    X(PH_DRESS_HELD) \
    X(PH_DRESS_SOLO) \
    X(PH_FLEECE_BLEND) \
    X(PH_FLEECE_HELD) \
    X(PH_FLEECE_SOLO) \
    X(PH_KIT_BLEND) \
    X(PH_KIT_HELD) \
    X(PH_KIT_SOLO) \
    X(PH_PRICE_BLEND) \
    X(PH_PRICE_HELD) \
    X(PH_PRICE_SOLO) \
    X(PH_LOT_BLEND) \
    X(PH_LOT_HELD) \
    X(PH_LOT_SOLO) \
    X(PH_GOAT_BLEND) \
    X(PH_GOAT_HELD) \
    X(PH_GOAT_SOLO) \
    X(PH_THOUGHT_BLEND) \
    X(PH_THOUGHT_HELD) \
    X(PH_THOUGHT_SOLO) \
    X(PH_CHOICE_BLEND) \
    X(PH_CHOICE_HELD) \
    X(PH_CHOICE_SOLO) \
    X(PH_MOUTH_BLEND) \
    X(PH_MOUTH_HELD) \
    X(PH_MOUTH_SOLO) \
    X(PH_FOOT_BLEND) \
    X(PH_FOOT_HELD) \
    X(PH_FOOT_SOLO) \
    X(PH_GOOSE_BLEND) \
    X(PH_GOOSE_HELD) \
    X(PH_GOOSE_SOLO) \
    X(PH_STRUT_BLEND) \
    X(PH_STRUT_HELD) \
    X(PH_STRUT_SOLO) \
    X(PH_CURE_BLEND) \
    X(PH_CURE_HELD) \
    X(PH_CURE_SOLO) \
    X(PH_NURSE_BLEND) \
    X(PH_NURSE_HELD) \
    X(PH_NURSE_SOLO) \

// Create BGM list
#define X(songId) static const u8 sBGMName_##songId[] = _(#songId);
SOUND_LIST_BGM
#undef X

#define X(songId) sBGMName_##songId,
static const u8 *const gBGMNames[] =
{
SOUND_LIST_BGM
};
#undef X

// Create SE list
#define X(songId) static const u8 sSEName_##songId[] = _(#songId);
SOUND_LIST_SE
#undef X

#define X(songId) sSEName_##songId,
static const u8 *const gSENames[] =
{
SOUND_LIST_SE
};
#undef X

#undef tWindowSelected
#undef tBgmIndex
#undef tSeIndex
#undef tBgmIndexOld
#undef tSeIndexOld
#undef tWhichSubmenu
#undef tState