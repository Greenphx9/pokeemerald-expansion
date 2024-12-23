static const struct TeraRaidMon sTeraRaidMons_InsideOfTruck[] =
{
    {
        .species = SPECIES_VENONAT,
        .moves = { MOVE_PSYBEAM, MOVE_STRUGGLE_BUG, MOVE_DISABLE, MOVE_BUG_BITE },
        .abilityNum = 0,
        .evs = { 0, 0, 0, 0, 0, 0 },
        .extraActions = { 0, 0, 0, 0, 0 },
        .fixedDrops = { 0, 0, 0, 0, 0, 0, 0 },
        .randomDrops = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    },
};

const struct TeraRaid sTeraRaidsByMapSec[0xFF][STAR_COUNT] =
{
    [MAPSEC_INSIDE_OF_TRUCK] = 
    {
        [ONE_STAR ... SEVEN_STARS] = {sTeraRaidMons_InsideOfTruck, ARRAY_COUNT(sTeraRaidMons_InsideOfTruck)},
    },
};