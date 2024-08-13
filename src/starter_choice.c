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
#include "field_message_box.h"
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
#include "trig.h"
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
    struct Pokemon mons[9];
    u8 selectedMon;
    u8 spriteId;
    u8 windowId;
    u8 arrowSpriteIds[2];
    u8 arrowCounter;
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

#define TAG_ARROW_SC_GFX           1261
#define TAG_ARROW_SC_PAL           0x4651

static const u32 sArrow_Gfx[] = INCBIN_U32("graphics/starter_choice/arrow.4bpp.lz");
static const u16 sArrow_Pal[] = INCBIN_U16("graphics/starter_choice/arrow.gbapal");

static const struct OamData sOamData_Arrow =
{
    .size = SPRITE_SIZE(16x16),
    .shape = SPRITE_SHAPE(16x16),
    .priority = 0,
};

static const struct CompressedSpriteSheet sSpriteSheet_Arrow =
{
    .data = sArrow_Gfx,
    .size = 16*16,
    .tag = TAG_ARROW_SC_GFX,
};

static const struct SpritePalette sSpritePal_Arrow =
{
    .data = sArrow_Pal,
    .tag = TAG_ARROW_SC_PAL
};

static const union AnimCmd sSpriteAnim_Arrow[] =
{
    ANIMCMD_FRAME(0, 16),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd *const sSpriteAnimTable_Arrow[] =
{
    sSpriteAnim_Arrow,
};

static const u8 sUpCoords[] =
{
    0, 0, 0,
    1, 1, 1, 1,
    2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3,
    2, 2, 2, 2, 2,
    1, 1, 1, 1,
    0, 0, 0,
};

static void SpriteCallback_Arrow(struct Sprite *sprite)
{
    if (sprite->data[2] == FALSE)
        sprite->y = sprite->data[1] - sUpCoords[sprite->data[0]];
    else
        sprite->y = sprite->data[1] + sUpCoords[sprite->data[0]];
    if (sprite->data[0] >= ARRAY_COUNT(sUpCoords))
        sprite->data[0] = 0;
    else
        sprite->data[0]++;
}

static const struct SpriteTemplate sSpriteTemplate_Arrow =
{
    .tileTag = TAG_ARROW_SC_GFX,
    .paletteTag = TAG_ARROW_SC_PAL,
    .oam = &sOamData_Arrow,
    .anims = sSpriteAnimTable_Arrow,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallback_Arrow
};

static const struct WindowTemplate sWindowTemplate_MonStats = {
    .bg = 0,
    .tilemapLeft = 1,
    .tilemapTop = 1,
    .width = 9,
    .height = 4,
    .paletteNum = 15,
    .baseBlock = 0x8
};

static void Task_StarterChoice_HandleMainInput(u8 taskId);
static void StarterChoice_CreateMonGfx(u16 species, bool8 isShiny, u32 personality);
static void StarterChoice_Destroy(void);
static void StarterChoice_PrintMonStats(void);

extern const u8 PalletTown_ProfessorOaksLab_Text_Controls[];

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

    LoadCompressedSpriteSheet(&sSpriteSheet_Arrow);
    LoadSpritePalette(&sSpritePal_Arrow);    

    sStarterChoice->arrowSpriteIds[0] = CreateSprite(&sSpriteTemplate_Arrow, 120, 15, 0);
    gSprites[sStarterChoice->arrowSpriteIds[0]].data[1] = 15;
    gSprites[sStarterChoice->arrowSpriteIds[0]].data[2] = FALSE;
    sStarterChoice->arrowSpriteIds[1] = CreateSprite(&sSpriteTemplate_Arrow, 120, 113, 0);
    gSprites[sStarterChoice->arrowSpriteIds[1]].data[1] = 113;
    gSprites[sStarterChoice->arrowSpriteIds[1]].data[2] = TRUE;
    gSprites[sStarterChoice->arrowSpriteIds[1]].vFlip = TRUE;

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

    ShowFieldMessage(PalletTown_ProfessorOaksLab_Text_Controls);

    CreateTask(Task_StarterChoice_HandleMainInput, 0);
}

static void StarterChoice_Destroy(void)
{
    FreeSpritePaletteByTag(TAG_ARROW_SC_PAL);
    FreeSpriteOamMatrix(&gSprites[sStarterChoice->arrowSpriteIds[0]]);
    DestroySprite(&gSprites[sStarterChoice->arrowSpriteIds[0]]);
    FreeSpriteOamMatrix(&gSprites[sStarterChoice->arrowSpriteIds[1]]);
    DestroySprite(&gSprites[sStarterChoice->arrowSpriteIds[1]]);
    FreeSpriteTilesByTag(TAG_ARROW_SC_GFX);
    if (sStarterChoice != NULL)
    {
        Free(sStarterChoice);
        sStarterChoice = NULL;
    }
    ScriptUnfreezeObjectEvents();  
    UnlockPlayerFieldControls();
    ScriptContext_Enable();
    HideFieldMessageBox();
}

static void Task_StarterChoice_HandleMainInput(u8 taskId)
{
    if (JOY_NEW(DPAD_UP))
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
    else if (JOY_NEW(DPAD_DOWN))
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
        VarSet(VAR_0x8001, 1);
        DestroyTask(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        FreeAndDestroyMonPicSprite(sStarterChoice->spriteId);
        ClearToTransparentAndRemoveWindow(sStarterChoice->windowId);
        StarterChoice_Destroy();
        VarSet(VAR_0x8001, 0);
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