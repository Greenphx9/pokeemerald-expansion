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
    [ONE_STAR]    = TERA_RAID_ONE_STAR_HP_MULT,
    [TWO_STARS]   = TERA_RAID_TWO_STAR_HP_MULT,
    [THREE_STARS] = TERA_RAID_THREE_STAR_HP_MULT,
    [FOUR_STARS]  = TERA_RAID_FOUR_STAR_HP_MULT,
    [FIVE_STARS]  = TERA_RAID_FIVE_STAR_HP_MULT,
    [SIX_STARS]   = TERA_RAID_SIX_STAR_HP_MULT,
    [SEVEN_STARS] = TERA_RAID_SEVEN_STAR_HP_MULT,
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