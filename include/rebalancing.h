#ifndef GUARD_REBALANCING_H
#define GUARD_REBALANCING_H

#include "main.h"

const struct SpeciesInfo GetSpeciesInfo(u16 species);
bool8 IsStatRebalanced(u16 species, u8 stat);
bool8 IsAbilityRebalanced(u16 species, u16 ability, u8 slot);

#endif // GUARD_REBALANCING_H
