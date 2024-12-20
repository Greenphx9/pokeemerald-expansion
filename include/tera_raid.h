#ifndef TERA_RAID_H
#define TERA_RAID_H

#include "gba/types.h"
#include "main.h"
#include "constants/global.h"

#define ONE_STAR    0
#define TWO_STARS   1
#define THREE_STARS 2
#define FOUR_STARS  3
#define FIVE_STARS  4
#define SIX_STARS   5
#define SEVEN_STARS 6 // not available naturally 
#define STAR_COUNT  7 

struct TeraRaidPartner
{
    u8 trainerName[TRAINER_NAME_LENGTH + 1];
    u32 otId;
    u16 objectEventGfx;
    u8 trainerClass;
    u8 trainerBackPic;
    const struct TrainerMon* parties[7]; // 1-7 stars
    u8 partySizes[7];
};

struct TeraRaidMon
{
    u16 species;
    u16 moves[4];
    u8 abilityNum;
    u8 evs[6];
    u8 extraActions[5];
    u16 fixedDrops[7];
    u16 randomDrops[19];
};

struct TeraRaid
{
    const struct TeraRaidMon* mons;
    u16 amount;
};

#endif