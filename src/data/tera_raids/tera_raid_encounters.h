static const struct TeraRaidMon sTeraRaidMons_InsideOfTruck[] =
{
    {
        .species = SPECIES_WALKING_WAKE,
        .moves = { MOVE_HYDRO_STEAM, MOVE_DRAGON_PULSE, MOVE_NOBLE_ROAR, MOVE_FLAMETHROWER },
        .abilityNum = 0,
        .evs = { 0, 0, 0, 0, 0, 0 },
        .extraActions = 
        { 
            .actions = 
            { 
                { .id = EXTRA_ACTION_NULLIFY_PLAYER, .hpPercentage = 50, },
            },
            .count = 1,
        },
        .fixedDrops =
        {
            .drops =
            {
                { .itemId = ITEM_EXP_CANDY_L, .count = 1, },
                { .itemId = ITEM_EXP_CANDY_XL, .count = 1, },
                { .itemId = ITEM_HEALTH_FEATHER, .count = 3, },
            },
            .count = 3,
        },
        .randomDrops =
        {
            .drops =
            {
                { .itemId = ITEM_EXP_CANDY_L, .chance = 23, .count = 1, },
                { .itemId = ITEM_EXP_CANDY_L, .chance = 11, .count = 2, },
                { .itemId = ITEM_RARE_CANDY, .chance = 4, .count = 1, },
                { .itemId = ITEM_HEALTH_FEATHER, .chance = 10, .count = 2, },
                { .itemId = ITEM_TAMATO_BERRY, .chance = 8, .count = 3, },
                { .itemId = ITEM_BIG_PEARL, .chance = 12, .count = 1, },
                { .itemId = ITEM_PEARL_STRING, .chance = 2, .count = 1, },
                { .itemId = ITEM_NUGGET, .chance = 5, .count = 1, },
                { .itemId = ITEM_NAIVE_MINT, .chance = 2, .count = 1, },
                { .itemId = ITEM_PP_UP, .chance = 3, .count = 1, },
                { .itemId = ITEM_BOTTLE_CAP, .chance = 2, .count = 1, },
                { .itemId = ITEM_ABILITY_CAPSULE, .chance = 2, .count = 1, },
            },
            .count = 12,
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