#include "global.h"
#include "malloc.h"
#include "event_data.h"
#include "rebalancing.h"
#include "pokemon.h"
#include "constants/abilities.h"
#include "constants/pokemon.h"

#define SET_IF_REBALANCED(stat)                                  \
{                                                                \
    if (gRebalancedSpeciesInfo[species].stat != 0)               \
        info.stat = gRebalancedSpeciesInfo[species].stat;        \
}                                                                \

const struct SpeciesInfo GetSpeciesInfo(u16 species)
{
    struct SpeciesInfo info = gSpeciesInfo[species];

    #if R_REBALANCING_ENABLED == TRUE
    if (FlagGet(R_REBALANCING_FLAG))
    {
        SET_IF_REBALANCED(baseHP)
        SET_IF_REBALANCED(baseAttack)
        SET_IF_REBALANCED(baseDefense)
        SET_IF_REBALANCED(baseSpAttack)
        SET_IF_REBALANCED(baseSpDefense)
        SET_IF_REBALANCED(baseSpeed)

        if (gRebalancedSpeciesInfo[species].abilities[0] != ABILITY_NONE)
        {
            info.abilities[0] = gRebalancedSpeciesInfo[species].abilities[0];
            info.abilities[1] = gRebalancedSpeciesInfo[species].abilities[1];
            info.abilities[2] = gRebalancedSpeciesInfo[species].abilities[2];
        }
        if (gRebalancedSpeciesInfo[species].types[0] != TYPE_NONE)
        {
            info.types[0] = gRebalancedSpeciesInfo[species].types[0];
            info.types[1] = gRebalancedSpeciesInfo[species].types[1];
        }
    }
    #endif

    return info;
}

bool8 IsStatRebalanced(u16 species, u8 stat)
{
    if (!FlagGet(R_REBALANCING_FLAG))
        return FALSE;

    switch(stat)
    {
        case STAT_HP:
            return gRebalancedSpeciesInfo[species].baseHP != 0;
        case STAT_ATK:
            return gRebalancedSpeciesInfo[species].baseAttack != 0;
        case STAT_DEF:
            return gRebalancedSpeciesInfo[species].baseDefense != 0;
        case STAT_SPEED:
            return gRebalancedSpeciesInfo[species].baseSpeed != 0;
        case STAT_SPATK:
            return gRebalancedSpeciesInfo[species].baseSpAttack != 0;
        case STAT_SPDEF:
            return gRebalancedSpeciesInfo[species].baseSpDefense != 0;
        default:
            return FALSE;
    }
}

bool8 IsAbilityRebalanced(u16 species, u16 ability, u8 slot)
{
    if (!FlagGet(R_REBALANCING_FLAG))
        return FALSE;

    return gSpeciesInfo[species].abilities[slot] != ability;
}

#undef SET_IF_REBALANCED
