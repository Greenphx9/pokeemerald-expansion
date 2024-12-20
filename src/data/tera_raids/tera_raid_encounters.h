static const struct TeraRaidMon sTeraRaidMons_InsideOfTruck[] =
{
    {
        .species = SPECIES_PIKACHU,
        .moves = { MOVE_TACKLE, MOVE_TAIL_WHIP, MOVE_NONE, MOVE_NONE },
        .abilityNum = 0,
        .evs = { 0, 0, 0, 0, 0, 0 },
        .extraActions = { 0, 0, 0, 0, 0 },
        .fixedDrops = { 0, 0, 0, 0, 0, 0, 0 },
        .randomDrops = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    },
    {
        .species = SPECIES_IRON_VALIANT,
        .moves = { MOVE_TACKLE, MOVE_TAIL_WHIP, MOVE_NONE, MOVE_NONE },
        .abilityNum = 0,
        .evs = { 0, 0, 0, 0, 0, 0 },
        .extraActions = { 0, 0, 0, 0, 0 },
        .fixedDrops = { 0, 0, 0, 0, 0, 0, 0 },
        .randomDrops = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    },
    {
        .species = SPECIES_CHARMANDER,
        .moves = { MOVE_TACKLE, MOVE_TAIL_WHIP, MOVE_NONE, MOVE_NONE },
        .abilityNum = 0,
        .evs = { 0, 0, 0, 0, 0, 0 },
        .extraActions = { 0, 0, 0, 0, 0 },
        .fixedDrops = { 0, 0, 0, 0, 0, 0, 0 },
        .randomDrops = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    },
    {
        .species = SPECIES_CHARIZARD,
        .moves = { MOVE_TACKLE, MOVE_TAIL_WHIP, MOVE_NONE, MOVE_NONE },
        .abilityNum = 0,
        .evs = { 0, 0, 0, 0, 0, 0 },
        .extraActions = { 0, 0, 0, 0, 0 },
        .fixedDrops = { 0, 0, 0, 0, 0, 0, 0 },
        .randomDrops = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    }
};

const struct TeraRaid sTeraRaidsByMapSec[0xFF][STAR_COUNT] =
{
    [MAPSEC_INSIDE_OF_TRUCK] = 
    {
        [ONE_STAR ... SEVEN_STARS] = {sTeraRaidMons_InsideOfTruck, ARRAY_COUNT(sTeraRaidMons_InsideOfTruck)},
    },
};