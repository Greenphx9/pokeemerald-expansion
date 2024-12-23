#include "tera_raid.h"
#include "tera_raid_screen.h"

#include "gba/types.h"
#include "gba/defines.h"
#include "global.h"
#include "main.h"
#include "battle.h"
#include "battle_anim.h"
#include "constants/battle.h"

bool8 IsTeraRaidOver(void)
{
    return (gBattleTypeFlags & BATTLE_TYPE_TERA_RAID) && (gBattleMons[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].hp <= 0);
}