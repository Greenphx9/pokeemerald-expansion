#ifndef TERA_RAID_H
#define TERA_RAID_H

#include "gba/types.h"
#include "main.h"
#include "global.h"
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
    u8 gender;
    u32 otId;
    u16 objectEventGfx;
    u8 trainerClass;
    u8 trainerBackPic;
    const struct TrainerMon* parties[7]; // 1-7 stars
    u8 partySizes[7];
};

enum TeraRaidExtraActions
{
    EXTRA_ACTION_USE_MOVE,
    EXTRA_ACTION_STEAL_TERA_ORB_CHARGE,
    EXTRA_ACTION_REMOVE_NEGATIVE_EFFECTS,
    EXTRA_ACTION_NULLIFY_PLAYER,
    EXTRA_ACTION_COUNT,
};


struct TeraRaidExtraAction
{
    enum TeraRaidExtraActions id;
    u16 moveId; // only set if extra action is EXTRA_ACTION_USE_MOVE
    u8 hpPercentage;
};

struct TeraRaidFixedDrop
{
    u16 itemId;
    u8 count;
};

struct TeraRaidRandomDrop
{
    u16 itemId;
    u8 chance;
    u8 count;
};

struct TeraRaidMon
{
    u16 species;
    u16 moves[4];
    u8 abilityNum;
    u8 evs[6];
    u8 teraType;

    // this is an unefficient way to do this, but
    // imo its cleaner than other flexible array solutions.
    // if you want, you can reduce the number of actions, drops
    // or completely rewrite how these are read to save space.
    struct 
    {
        struct TeraRaidExtraAction actions[5];
        u8 count;
    } extraActions;

    struct
    {
        struct TeraRaidFixedDrop drops[7];
        u8 count;
    } fixedDrops;

    struct 
    {
        struct TeraRaidRandomDrop drops[19];
        u8 count;
    } randomDrops;
};

struct TeraRaid
{
    const struct TeraRaidMon* mons;
    u16 amount;
};

#define EXTRA_ACTION_DUMMY { .id = EXTRA_ACTION_COUNT, .hpPercentage = 255 }

bool8 IsTeraRaidOver(void);
void ApplyTeraRaidHPMultiplier(u32 battler, struct Pokemon* mon);

#endif