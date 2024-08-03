#include "option_menu.h"
#include "global.h"
#include "battle_pike.h"
#include "battle_pyramid.h"
#include "battle_pyramid_bag.h"
#include "bg.h"
#include "decompress.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "event_object_lock.h"
#include "event_scripts.h"
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
#include "malloc.h"
#include "menu.h"
#include "new_game.h"
#include "option_menu.h"
#include "overworld.h"
#include "palette.h"
#include "party_menu.h"
#include "pokedex.h"
#include "pokenav.h"
#include "pokemon.h"
#include "random.h"
#include "safari_zone.h"
#include "save.h"
#include "scanline_effect.h"
#include "script_menu.h"
#include "script.h"
#include "sprite.h"
#include "sound.h"
#include "start_menu.h"
#include "strings.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "text_window.h"
#include "trainer_pokemon_sprites.h"
#include "window.h"
#include "union_room.h"
#include "constants/battle_frontier.h"
#include "constants/rgb.h"
#include "constants/songs.h"
#include "rtc.h"
#include "event_object_movement.h"
#include "gba/isagbprint.h"

struct StarterChoice
{
    u16 tilemapBuffers[4][0x400];
    MainCallback savedCallback;
    struct Pokemon mons[9];
    u8 selectedMon;
    u8 spriteId;
    u32 windowId;
};

static const u16 sGrassStarters[9] =
{
    SPECIES_BULBASAUR,
    SPECIES_CHIKORITA,
    SPECIES_TREECKO,
    SPECIES_TURTWIG,
    SPECIES_SNIVY,
    SPECIES_CHESPIN,
    SPECIES_ROWLET,
    SPECIES_GROOKEY,
    SPECIES_SPRIGATITO,
};

static const u16 sWaterStarters[9] =
{
    SPECIES_SQUIRTLE,
    SPECIES_TOTODILE,
    SPECIES_MUDKIP,
    SPECIES_PIPLUP,
    SPECIES_OSHAWOTT,
    SPECIES_FROAKIE,
    SPECIES_POPPLIO,
    SPECIES_SOBBLE,
    SPECIES_QUAXLY,
};

static const u16 sFireStarters[9] =
{
    SPECIES_CHARMANDER,
    SPECIES_CYNDAQUIL,
    SPECIES_TORCHIC,
    SPECIES_CHIMCHAR,
    SPECIES_TEPIG,
    SPECIES_FENNEKIN,
    SPECIES_LITTEN,
    SPECIES_SCORBUNNY,
    SPECIES_FUECOCO,
};

static EWRAM_DATA struct StarterChoice *sStarterChoice = NULL;

static const u32 sStarterChoiceTiles[] = INCBIN_U32("graphics/starter_choice/bg_tiles.4bpp.lz");
static const u32 sStarterChoiceTilemap[] = INCBIN_U32("graphics/starter_choice/bg2.bin.lz");
static const u32 sStarterChoicePalette[] = INCBIN_U32("graphics/starter_choice/bg_tiles.gbapal.lz");

static const struct WindowTemplate sWindowTemplate_MonStats = {
    .bg = 0,
    .tilemapLeft = 1,
    .tilemapTop = 1,
    .width = 9,
    .height = 4,
    .paletteNum = 15,
    .baseBlock = 0x8
};

static void StarterChoice_LoadBgGfx(void);
static void Task_StarterChoice_HandleMainInput(u8 taskId);
static void StarterChoice_CreateMonGfx(u16 species, bool8 isShiny, u32 personality);
static void StarterChoice_Destroy(void);
static void StarterChoice_PrintMonStats(void);

void StarterChoice_Init(void) 
{
    u8 nature, i;
    u16 species;
    struct Pokemon curMon;
    if (!IsOverworldLinkActive())
    {
        FreezeObjectEvents();
        PlayerFreeze();
        StopPlayerAvatar();
    }

    LockPlayerFieldControls();

    if (sStarterChoice == NULL) 
    {
        sStarterChoice = AllocZeroed(sizeof(struct StarterChoice));
    }

    for (i = 0; i < 9; i++)
    {
        nature = Random() % NUM_NATURES;
        switch(gSpecialVar_0x8004)
        {
            case 0:
            default:
                // Do it like this so we can print stats, and show shinies
                CreateMonWithNature(&sStarterChoice->mons[i], sGrassStarters[i], 5, 32, nature);
                break;
            case 1:
                CreateMonWithNature(&sStarterChoice->mons[i], sWaterStarters[i], 5, 32, nature);
                break;
            case 2:
                CreateMonWithNature(&sStarterChoice->mons[i], sFireStarters[i], 5, 32, nature);
                break;
            
        }
        
    }

    curMon = sStarterChoice->mons[sStarterChoice->selectedMon];
    species = GetMonData(&curMon, MON_DATA_SPECIES);
    sStarterChoice->windowId = CreateWindowFromRect(10, 3, 8, 8);
    PlayCry_Normal(species, 0);
    StarterChoice_CreateMonGfx(species, GetMonData(&curMon, MON_DATA_IS_SHINY), GetMonData(&curMon, MON_DATA_PERSONALITY));
    LoadPalette(GetOverworldTextboxPalettePtr(), BG_PLTT_ID(15), PLTT_SIZE_4BPP);
    SetStandardWindowBorderStyle(sStarterChoice->windowId, TRUE);
    ScheduleBgCopyTilemapToVram(0);

    StarterChoice_PrintMonStats();

    CreateTask(Task_StarterChoice_HandleMainInput, 0);
}

static void StarterChoice_LoadBgGfx(void) 
{
    DecompressAndCopyTileDataToVram(1, sStarterChoiceTiles, 0x0, 0x0, 0);
    LZDecompressWram(sStarterChoiceTilemap, sStarterChoice->tilemapBuffers[0]);
    LoadCompressedPalette(sStarterChoicePalette, BG_PLTT_ID(12), PLTT_SIZE_4BPP);
}

static void StarterChoice_Destroy(void)
{
    if (sStarterChoice != NULL)
    {
        Free(sStarterChoice);
        sStarterChoice = NULL;
    }
    ScriptUnfreezeObjectEvents();  
    UnlockPlayerFieldControls();
    ScriptContext_Enable();
}

static void Task_StarterChoice_HandleMainInput(u8 taskId)
{
    if (JOY_NEW(DPAD_LEFT))
    {
        struct Pokemon mon;
        u16 species;
        FreeAndDestroyMonPicSprite(sStarterChoice->spriteId);
        if (sStarterChoice->selectedMon == 0)
            sStarterChoice->selectedMon = 8;
        else
            sStarterChoice->selectedMon--;
        mon = sStarterChoice->mons[sStarterChoice->selectedMon];
        species = GetMonData(&mon, MON_DATA_SPECIES);
        PlayCry_Normal(species, 0);
        StarterChoice_CreateMonGfx(species, GetMonData(&mon, MON_DATA_IS_SHINY), GetMonData(&mon, MON_DATA_PERSONALITY));
        StarterChoice_PrintMonStats();
    }
    else if (JOY_NEW(DPAD_RIGHT))
    {
        struct Pokemon mon;
        u16 species;
        FreeAndDestroyMonPicSprite(sStarterChoice->spriteId);
        if (sStarterChoice->selectedMon == 8)
            sStarterChoice->selectedMon = 0;
        else
            sStarterChoice->selectedMon++;
        mon = sStarterChoice->mons[sStarterChoice->selectedMon];
        species = GetMonData(&mon, MON_DATA_SPECIES);
        PlayCry_Normal(species, 0);
        StarterChoice_CreateMonGfx(species, GetMonData(&mon, MON_DATA_IS_SHINY), GetMonData(&mon, MON_DATA_PERSONALITY));
        StarterChoice_PrintMonStats();
    }
    else if (JOY_NEW(A_BUTTON))
    {
        FreeAndDestroyMonPicSprite(sStarterChoice->spriteId);
        ClearToTransparentAndRemoveWindow(sStarterChoice->windowId);
        StringCopy(gStringVar1, GetSpeciesName(GetMonData(&sStarterChoice->mons[sStarterChoice->selectedMon], MON_DATA_SPECIES)));
        CopyMon(&gPlayerParty[0], &sStarterChoice->mons[sStarterChoice->selectedMon], sizeof(struct Pokemon));
        StarterChoice_Destroy();
        DestroyTask(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        FreeAndDestroyMonPicSprite(sStarterChoice->spriteId);
        ClearToTransparentAndRemoveWindow(sStarterChoice->windowId);
        StarterChoice_Destroy();
        DestroyTask(taskId);
    }
}

static void StarterChoice_CreateMonGfx(u16 species, bool8 isShiny, u32 personality)
{
    sStarterChoice->spriteId = CreateMonPicSprite(species, isShiny, personality, TRUE, 120, 64, 0, species);
    PreservePaletteInWeather(IndexOfSpritePaletteTag(species) + 0x10);
    gSprites[sStarterChoice->spriteId].callback = SpriteCallbackDummy;
    gSprites[sStarterChoice->spriteId].oam.priority = 0;
}

static const u8 sText_MonData[] = _("Species:");
static void StarterChoice_PrintMonStats(void)
{
    //struct Pokemon mon = sStarterChoice->mons[sStarterChoice->selectedMon];
    //u16 species = GetMonData(&mon, MON_DATA_SPECIES);
    //StringCopy(gStringVar2, GetSpeciesName(species));
    //DebugPrintf("printing, window: %d", sStarterChoice->textWindowId);
   // AddTextPrinterParameterized(sStarterChoice->textWindowId, FONT_SHORT, sText_MonData, 0, 0, 0xFF, NULL);
}