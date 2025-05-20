static const struct TeraRaidMon sTeraRaidMons_InsideOfTruck[] =
{
    {
        .species = SPECIES_RAICHU,
        .moves = {MOVE_DISCHARGE, MOVE_IRON_TAIL, MOVE_CHARM, MOVE_NUZZLE },
        .evs = TRAINER_PARTY_EVS(0, 0, 0, 0, 0, 0),
        .extraActions =
        {
            .actions =
            {
                { .id = EXTRA_ACTION_USE_MOVE, .hpPercentage = 85, .moveId = MOVE_ELECTRIC_TERRAIN },
                { .id = EXTRA_ACTION_USE_MOVE, .hpPercentage = 75, .moveId = MOVE_THUNDER_WAVE },
                { .id = EXTRA_ACTION_NULLIFY_PLAYER, .hpPercentage = 50 },
                { .id = EXTRA_ACTION_REMOVE_NEGATIVE_EFFECTS, .hpPercentage = 50 },
                { .id = EXTRA_ACTION_USE_MOVE, .hpPercentage = 35, .moveId = MOVE_ELECTRIC_TERRAIN },
            },
            .count = 5
        },
        .fixedDrops =
        {
            .drops =
            {
                { .itemId = ITEM_EXP_CANDY_L, .count = 1 },
                { .itemId = ITEM_EXP_CANDY_XL, .count = 1 },
                { .itemId = ITEM_SWIFT_FEATHER, .count = 3 },
            },
            .count = 3
        },
        .randomDrops =
        {
            .drops =
            {
                { .itemId = ITEM_EXP_CANDY_L, .chance = 22, .count = 1 },
                { .itemId = ITEM_BIG_MUSHROOM, .chance = 11, .count = 1 },
                { .itemId = ITEM_EXP_CANDY_L, .chance = 10, .count = 2 },
                { .itemId = ITEM_SWIFT_FEATHER, .chance = 9, .count = 2 },
                { .itemId = ITEM_GREPA_BERRY, .chance = 8, .count = 3 },
                { .itemId = ITEM_NUGGET, .chance = 5, .count = 1 },
                { .itemId = ITEM_BOTTLE_CAP, .chance = 4, .count = 1 },
                { .itemId = ITEM_RARE_CANDY, .chance = 4, .count = 1 },
                { .itemId = ITEM_PP_UP, .chance = 3, .count = 1 },
                { .itemId = ITEM_TIMID_MINT, .chance = 2, .count = 1 },
                { .itemId = ITEM_HASTY_MINT, .chance = 2, .count = 1 },
                { .itemId = ITEM_NAIVE_MINT, .chance = 2, .count = 1 },
                { .itemId = ITEM_JOLLY_MINT, .chance = 2, .count = 1 },
                { .itemId = ITEM_ABILITY_CAPSULE, .chance = 2, .count = 1 },
                { .itemId = ITEM_BALM_MUSHROOM, .chance = 2, .count = 1 },
            },
            .count = 16
        },
    },
    {
        .species = SPECIES_ARCANINE,
        .moves = {MOVE_FLAMETHROWER, MOVE_CRUNCH, MOVE_EXTREME_SPEED, MOVE_FIRE_FANG },
        .evs = TRAINER_PARTY_EVS(0, 0, 0, 0, 0, 0),
        .extraActions =
        {
            .actions =
            {
                { .id = EXTRA_ACTION_USE_MOVE, .hpPercentage = 85, .moveId = MOVE_SUNNY_DAY },
                { .id = EXTRA_ACTION_USE_MOVE, .hpPercentage = 75, .moveId = MOVE_LEER },
                { .id = EXTRA_ACTION_STEAL_TERA_ORB_CHARGE, .hpPercentage = 50 },
                { .id = EXTRA_ACTION_REMOVE_NEGATIVE_EFFECTS, .hpPercentage = 50 },
                { .id = EXTRA_ACTION_USE_MOVE, .hpPercentage = 35, .moveId = MOVE_SUNNY_DAY },
            },
            .count = 5
        },
        .fixedDrops =
        {
            .drops =
            {
                { .itemId = ITEM_EXP_CANDY_L, .count = 1 },
                { .itemId = ITEM_EXP_CANDY_XL, .count = 1 },
                { .itemId = ITEM_MUSCLE_FEATHER, .count = 3 },
            },
            .count = 3
        },
        .randomDrops =
        {
            .drops =
            {
                { .itemId = ITEM_EXP_CANDY_L, .chance = 22, .count = 1 },
                { .itemId = ITEM_STAR_PIECE, .chance = 12, .count = 1 },
                { .itemId = ITEM_EXP_CANDY_L, .chance = 11, .count = 2 },
                { .itemId = ITEM_MUSCLE_FEATHER, .chance = 10, .count = 2 },
                { .itemId = ITEM_KELPSY_BERRY, .chance = 8, .count = 3 },
                { .itemId = ITEM_NUGGET, .chance = 5, .count = 1 },
                { .itemId = ITEM_RARE_CANDY, .chance = 4, .count = 1 },
                { .itemId = ITEM_PP_UP, .chance = 3, .count = 1 },
                { .itemId = ITEM_BRAVE_MINT, .chance = 2, .count = 1 },
                { .itemId = ITEM_NAUGHTY_MINT, .chance = 2, .count = 1 },
                { .itemId = ITEM_LONELY_MINT, .chance = 2, .count = 1 },
                { .itemId = ITEM_ADAMANT_MINT, .chance = 2, .count = 1 },
                { .itemId = ITEM_ABILITY_CAPSULE, .chance = 2, .count = 1 },
                { .itemId = ITEM_BOTTLE_CAP, .chance = 2, .count = 1 },
                { .itemId = ITEM_COMET_SHARD, .chance = 2, .count = 1 },
            },
            .count = 16
        },
    },
    {
        .species = SPECIES_SLOWBRO,
        .moves = {MOVE_ZEN_HEADBUTT, MOVE_LIQUIDATION, MOVE_YAWN, MOVE_WATER_PULSE },
        .evs = TRAINER_PARTY_EVS(0, 0, 0, 0, 0, 0),
        .extraActions =
        {
            .actions =
            {
                { .id = EXTRA_ACTION_USE_MOVE, .hpPercentage = 85, .moveId = MOVE_CURSE },
                { .id = EXTRA_ACTION_USE_MOVE, .hpPercentage = 70, .moveId = MOVE_YAWN },
                { .id = EXTRA_ACTION_REMOVE_NEGATIVE_EFFECTS, .hpPercentage = 50 },
                { .id = EXTRA_ACTION_NULLIFY_PLAYER, .hpPercentage = 45 },
                { .id = EXTRA_ACTION_USE_MOVE, .hpPercentage = 35, .moveId = MOVE_CURSE },
            },
            .count = 5
        },
        .fixedDrops =
        {
            .drops =
            {
                { .itemId = ITEM_EXP_CANDY_L, .count = 1 },
                { .itemId = ITEM_EXP_CANDY_XL, .count = 1 },
                { .itemId = ITEM_RESIST_FEATHER, .count = 3 },
            },
            .count = 3
        },
        .randomDrops =
        {
            .drops =
            {
                { .itemId = ITEM_EXP_CANDY_L, .chance = 22, .count = 1 },
                { .itemId = ITEM_BIG_PEARL, .chance = 12, .count = 1 },
                { .itemId = ITEM_EXP_CANDY_L, .chance = 11, .count = 2 },
                { .itemId = ITEM_RESIST_FEATHER, .chance = 10, .count = 2 },
                { .itemId = ITEM_QUALOT_BERRY, .chance = 8, .count = 3 },
                { .itemId = ITEM_NUGGET, .chance = 5, .count = 1 },
                { .itemId = ITEM_RARE_CANDY, .chance = 4, .count = 1 },
                { .itemId = ITEM_PP_UP, .chance = 3, .count = 1 },
                { .itemId = ITEM_RELAXED_MINT, .chance = 2, .count = 1 },
                { .itemId = ITEM_LAX_MINT, .chance = 2, .count = 1 },
                { .itemId = ITEM_IMPISH_MINT, .chance = 2, .count = 1 },
                { .itemId = ITEM_BOLD_MINT, .chance = 2, .count = 1 },
                { .itemId = ITEM_ABILITY_CAPSULE, .chance = 2, .count = 1 },
                { .itemId = ITEM_BOTTLE_CAP, .chance = 2, .count = 1 },
                { .itemId = ITEM_PEARL_STRING, .chance = 2, .count = 1 },
            },
            .count = 16
        },
    },
    {
        .species = SPECIES_CLOYSTER,
        .moves = {MOVE_ICICLE_SPEAR, MOVE_HYDRO_PUMP, MOVE_ICE_SHARD, MOVE_SUPERSONIC },
        .evs = TRAINER_PARTY_EVS(0, 0, 0, 0, 0, 0),
        .extraActions =
        {
            .actions =
            {
                { .id = EXTRA_ACTION_STEAL_TERA_ORB_CHARGE, .hpPercentage = 85 },
                { .id = EXTRA_ACTION_REMOVE_NEGATIVE_EFFECTS, .hpPercentage = 50 },
                { .id = EXTRA_ACTION_USE_MOVE, .hpPercentage = 35, .moveId = MOVE_SHELL_SMASH },
                { .id = EXTRA_ACTION_REMOVE_NEGATIVE_EFFECTS, .hpPercentage = 30 },
            },
            .count = 4
        },
        .fixedDrops =
        {
            .drops =
            {
                { .itemId = ITEM_EXP_CANDY_L, .count = 1 },
                { .itemId = ITEM_EXP_CANDY_XL, .count = 1 },
                { .itemId = ITEM_RESIST_FEATHER, .count = 3 },
            },
            .count = 3
        },
        .randomDrops =
        {
            .drops =
            {
                { .itemId = ITEM_EXP_CANDY_L, .chance = 22, .count = 1 },
                { .itemId = ITEM_BIG_PEARL, .chance = 12, .count = 1 },
                { .itemId = ITEM_EXP_CANDY_L, .chance = 11, .count = 2 },
                { .itemId = ITEM_RESIST_FEATHER, .chance = 10, .count = 2 },
                { .itemId = ITEM_QUALOT_BERRY, .chance = 8, .count = 3 },
                { .itemId = ITEM_NUGGET, .chance = 5, .count = 1 },
                { .itemId = ITEM_RARE_CANDY, .chance = 4, .count = 1 },
                { .itemId = ITEM_PP_UP, .chance = 3, .count = 1 },
                { .itemId = ITEM_RELAXED_MINT, .chance = 2, .count = 1 },
                { .itemId = ITEM_LAX_MINT, .chance = 2, .count = 1 },
                { .itemId = ITEM_IMPISH_MINT, .chance = 2, .count = 1 },
                { .itemId = ITEM_BOLD_MINT, .chance = 2, .count = 1 },
                { .itemId = ITEM_ABILITY_CAPSULE, .chance = 2, .count = 1 },
                { .itemId = ITEM_BOTTLE_CAP, .chance = 2, .count = 1 },
                { .itemId = ITEM_PEARL_STRING, .chance = 2, .count = 1 },
            },
            .count = 16
        },
    },
};

const struct TeraRaid sTeraRaidsByMapSec[0xFF][STAR_COUNT] =
{
    [MAPSEC_INSIDE_OF_TRUCK] = 
    {
        [ONE_STAR ... SEVEN_STARS] = {sTeraRaidMons_InsideOfTruck, ARRAY_COUNT(sTeraRaidMons_InsideOfTruck)},
    },
};