#include "tera_raid.h"

const struct TrainerMon sTeraRaidParty_Red1[] =
{
    {
        .species = SPECIES_BULBASAUR,
        .heldItem = ITEM_MIRACLE_SEED,
        .ev = TRAINER_PARTY_EVS(75, 75, 0, 0, 0, 0),
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
        .moves = { MOVE_RAZOR_LEAF, MOVE_TACKLE, MOVE_GROWTH, MOVE_GROWL },
        .ability = ABILITY_OVERGROW,
        .teraType = TYPE_GRASS,
        .nature = NATURE_ADAMANT,
    },
    {
        .species = SPECIES_CHARMANDER,
        .heldItem = ITEM_CHARCOAL,
        .ev = TRAINER_PARTY_EVS(4, 0, 0, 75, 75, 0),
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
        .moves = { MOVE_EMBER, MOVE_DRAGON_BREATH, MOVE_SCRATCH, MOVE_SUNNY_DAY },
        .ability = ABILITY_BLAZE,
        .teraType = TYPE_FIRE,
        .nature = NATURE_MODEST,
    },
    {
        .species = SPECIES_SQUIRTLE,
        .heldItem = ITEM_MYSTIC_WATER,
        .ev = TRAINER_PARTY_EVS(75, 36, 0, 0, 36, 0),
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
        .moves = { MOVE_WATER_GUN, MOVE_ICY_WIND, MOVE_RAPID_SPIN, MOVE_WITHDRAW },
        .ability = ABILITY_TORRENT,
        .teraType = TYPE_WATER,
        .nature = NATURE_RELAXED,
    },
};

const struct TrainerMon sTeraRaidParty_Red23[] =
{
    {
        .species = SPECIES_IVYSAUR,
        .heldItem = ITEM_MIRACLE_SEED,
        .ev = TRAINER_PARTY_EVS(150, 0, 0, 4, 150, 0),
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = { MOVE_GIGA_DRAIN, MOVE_SLUDGE, MOVE_GROWTH, MOVE_INGRAIN },
        .ability = ABILITY_CHLOROPHYLL,
        .teraType = TYPE_GRASS,
        .nature = NATURE_MODEST,
    },
    {
        .species = SPECIES_CHARMELEON,
        .heldItem = ITEM_CHARCOAL,
        .ev = TRAINER_PARTY_EVS(4, 150, 0, 150, 0, 0),
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
        .moves = { MOVE_FIRE_PUNCH, MOVE_THUNDER_PUNCH, MOVE_BITE, MOVE_BODY_SLAM },
        .ability = ABILITY_BLAZE,
        .teraType = TYPE_FIRE,
        .nature = NATURE_ADAMANT,
    },
    {
        .species = SPECIES_WARTORTLE,
        .heldItem = ITEM_MYSTIC_WATER,
        .ev = TRAINER_PARTY_EVS(4, 0, 0, 150, 150, 0),
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = { MOVE_CHILLING_WATER, MOVE_ICY_WIND, MOVE_MUD_SHOT, MOVE_LIFE_DEW },
        .ability = ABILITY_TORRENT,
        .teraType = TYPE_WATER,
        .nature = NATURE_MODEST,
    },
};

const struct TrainerMon sTeraRaidParty_Red4567[] =
{
    {
        .species = SPECIES_VENUSAUR,
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = { MOVE_ENERGY_BALL, MOVE_SLUDGE_BOMB, MOVE_EARTH_POWER, MOVE_SYNTHESIS },
        .ability = ABILITY_OVERGROW,
        .teraType = TYPE_GRASS,
        .nature = NATURE_TIMID,
    },
    {
        .species = SPECIES_CHARIZARD,
        .heldItem = ITEM_CHOICE_SPECS,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = { MOVE_FLAMETHROWER, MOVE_SCORCHING_SANDS, MOVE_HURRICANE, MOVE_FOCUS_BLAST },
        .ability = ABILITY_BLAZE,
        .teraType = TYPE_FIRE,
        .nature = NATURE_TIMID,
    },
    {
        .species = SPECIES_BLASTOISE,
        .heldItem = ITEM_WHITE_HERB,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = { MOVE_HYDRO_PUMP, MOVE_BLIZZARD, MOVE_DRAGON_PULSE, MOVE_SHELL_SMASH },
        .ability = ABILITY_TORRENT,
        .teraType = TYPE_WATER,
        .nature = NATURE_MODEST,
    },
};

const struct TeraRaidPartner gTeraRaidPartners[] = 
{
    {
        .trainerName = _("Red"),
        .gender = TRAINER_MON_MALE,
        .otId = 0x0,
        .objectEventGfx = OBJ_EVENT_GFX_RED,
        .trainerClass = TRAINER_CLASS_PKMN_TRAINER_1,
        .trainerBackPic = TRAINER_BACK_PIC_RED,
        .parties = 
        {
            [ONE_STAR]                   = sTeraRaidParty_Red1,
            [TWO_STARS  ... THREE_STARS] = sTeraRaidParty_Red23,
            [FOUR_STARS ... SEVEN_STARS] = sTeraRaidParty_Red4567,
        },
        .partySizes = 
        {
            [ONE_STAR]                   = ARRAY_COUNT(sTeraRaidParty_Red1),
            [TWO_STARS  ... THREE_STARS] = ARRAY_COUNT(sTeraRaidParty_Red23),
            [FOUR_STARS ... SEVEN_STARS] = ARRAY_COUNT(sTeraRaidParty_Red4567),
        },
    },
    {
        .trainerName = _("Green"),
        .gender = TRAINER_MON_FEMALE,
        .otId = 0x0,
        .objectEventGfx = OBJ_EVENT_GFX_LEAF,
        .trainerClass = TRAINER_CLASS_PKMN_TRAINER_1,
        .trainerBackPic = TRAINER_BACK_PIC_LEAF,
        .parties = 
        {
            [ONE_STAR]                   = sTeraRaidParty_Red1,
            [TWO_STARS  ... THREE_STARS] = sTeraRaidParty_Red23,
            [FOUR_STARS ... SEVEN_STARS] = sTeraRaidParty_Red4567,
        },
        .partySizes = 
        {
            [ONE_STAR]                   = ARRAY_COUNT(sTeraRaidParty_Red1),
            [TWO_STARS  ... THREE_STARS] = ARRAY_COUNT(sTeraRaidParty_Red23),
            [FOUR_STARS ... SEVEN_STARS] = ARRAY_COUNT(sTeraRaidParty_Red4567),
        },
    },
    {
        .trainerName = _("Wally"),
        .gender = TRAINER_MON_MALE,
        .otId = 0x0,
        .objectEventGfx = OBJ_EVENT_GFX_WALLY,
        .trainerClass = TRAINER_CLASS_PKMN_TRAINER_1,
        .trainerBackPic = TRAINER_BACK_PIC_WALLY,
        .parties = 
        {
            [ONE_STAR]                   = sTeraRaidParty_Red1,
            [TWO_STARS  ... THREE_STARS] = sTeraRaidParty_Red23,
            [FOUR_STARS ... SEVEN_STARS] = sTeraRaidParty_Red4567,
        },
        .partySizes = 
        {
            [ONE_STAR]                   = ARRAY_COUNT(sTeraRaidParty_Red1),
            [TWO_STARS  ... THREE_STARS] = ARRAY_COUNT(sTeraRaidParty_Red23),
            [FOUR_STARS ... SEVEN_STARS] = ARRAY_COUNT(sTeraRaidParty_Red4567),
        },
    },
};
