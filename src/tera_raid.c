#include "gba/types.h"
#include "gba/defines.h"
#include "global.h"
#include "main.h"
#include "pokemon.h"
#include "tera_raid.h"
#include "tera_raid_screen.h"
#include "battle.h"
#include "battle_anim.h"
#include "constants/battle.h"

bool8 IsTeraRaidOver(void)
{
    return (gBattleTypeFlags & BATTLE_TYPE_TERA_RAID) && (gBattleMons[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].hp <= 0);
}

const u16 sTeraLevelScale[] =
{
    [ONE_STAR]    = 250,
    [TWO_STARS]   = 250,
    [THREE_STARS] = 400,
    [FOUR_STARS]  = 600,
    [FIVE_STARS]  = 1000,
    [SIX_STARS]   = 1250,
    [SEVEN_STARS] = 1500, // varies in game, here we have it at 1500 (15x multiplier), change below code to have custom multiplier
};

void ApplyTeraRaidHPMultiplier(u32 battler, struct Pokemon* mon)
{
    if (GetMonData(mon, MON_DATA_SPECIES) == SPECIES_SHEDINJA)
        return;
    else
    {
        u32 scale = sTeraLevelScale[gTeraRaidStars];
        u32 hp = (GetMonData(mon, MON_DATA_HP) * scale + 99) / 100;
        u32 maxHP = (GetMonData(mon, MON_DATA_MAX_HP) * scale + 99) / 100;
        SetMonData(mon, MON_DATA_HP, &hp);
        SetMonData(mon, MON_DATA_MAX_HP, &maxHP);
    }
}