#include "global.h"
#include "text.h"
#include "dynamic_placeholder_text_util.h"
#include "string_util.h"
#include "constants/event_objects.h"

static EWRAM_DATA const u8 *sStringPointers[8] = {};

static const u8 sTextColorTable[] =
{
    [OBJ_EVENT_GFX_BRENDAN_NORMAL] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_BRENDAN_MACH_BIKE] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_BRENDAN_SURFING] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_QUINTY_PLUMP] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_NINJA_BOY] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_TWIN] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_BOY_1] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_GIRL_1] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_BOY_2] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_GIRL_2] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_LITTLE_BOY] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_LITTLE_GIRL] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_BOY_3] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_GIRL_3] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_RICH_BOY] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_WOMAN_1] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_FAT_MAN] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_POKEFAN_F] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_MAN_1] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_WOMAN_2] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_EXPERT_M] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_EXPERT_F] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_MAN_2] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_WOMAN_3] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_POKEFAN_M] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_WOMAN_4] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_COOK] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_LINK_RECEPTIONIST] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_OLD_MAN] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_OLD_WOMAN] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_CAMPER] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_PICNICKER] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_MAN_3] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_WOMAN_5] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_YOUNGSTER] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_BUG_CATCHER] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_PSYCHIC_M] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_SCHOOL_KID_M] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_MANIAC] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_HEX_MANIAC] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_RAYQUAZA_STILL] = NPC_TEXT_COLOR_MON,
    [OBJ_EVENT_GFX_SWIMMER_M] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_SWIMMER_F] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_BLACK_BELT] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_BEAUTY] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_SCIENTIST_1] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_LASS] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_GENTLEMAN] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_SAILOR] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_FISHERMAN] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_RUNNING_TRIATHLETE_M] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_RUNNING_TRIATHLETE_F] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_TUBER_F] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_TUBER_M] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_HIKER] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_CYCLING_TRIATHLETE_M] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_CYCLING_TRIATHLETE_F] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_NURSE] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_ITEM_BALL] = NPC_TEXT_COLOR_NEUTRAL,
    [OBJ_EVENT_GFX_BERRY_TREE] = NPC_TEXT_COLOR_NEUTRAL,
    [OBJ_EVENT_GFX_BERRY_TREE_EARLY_STAGES] = NPC_TEXT_COLOR_NEUTRAL,
    [OBJ_EVENT_GFX_BERRY_TREE_LATE_STAGES] = NPC_TEXT_COLOR_NEUTRAL,
    [OBJ_EVENT_GFX_BRENDAN_ACRO_BIKE] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_PROF_OAK] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_MAN_4] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_MAN_5] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_REPORTER_M] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_REPORTER_F] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_BARD] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_ANABEL] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_TUCKER] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_GRETA] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_SPENSER] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_NOLAND] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_LUCY] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_CUTTABLE_TREE] = NPC_TEXT_COLOR_NEUTRAL,
    [OBJ_EVENT_GFX_MART_EMPLOYEE] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_ROOFTOP_SALE_WOMAN] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_TEALA] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_BREAKABLE_ROCK] = NPC_TEXT_COLOR_NEUTRAL,
    [OBJ_EVENT_GFX_PUSHABLE_BOULDER] = NPC_TEXT_COLOR_NEUTRAL,
    [OBJ_EVENT_GFX_MR_BRINEYS_BOAT] = NPC_TEXT_COLOR_NEUTRAL,
    [OBJ_EVENT_GFX_MAY_NORMAL] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_MAY_MACH_BIKE] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_MAY_ACRO_BIKE] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_MAY_SURFING] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_MAY_FIELD_MOVE] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_TRUCK] = NPC_TEXT_COLOR_NEUTRAL,
    [OBJ_EVENT_GFX_VIGOROTH_CARRYING_BOX] = NPC_TEXT_COLOR_MON,
    [OBJ_EVENT_GFX_VIGOROTH_FACING_AWAY] = NPC_TEXT_COLOR_MON,
    [OBJ_EVENT_GFX_BIRCHS_BAG] = NPC_TEXT_COLOR_NEUTRAL,
    [OBJ_EVENT_GFX_ZIGZAGOON_1] = NPC_TEXT_COLOR_NEUTRAL,
    [OBJ_EVENT_GFX_ARTIST] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_RIVAL_BRENDAN_NORMAL] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_RIVAL_BRENDAN_MACH_BIKE] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_RIVAL_BRENDAN_ACRO_BIKE] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_RIVAL_BRENDAN_SURFING] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_RIVAL_BRENDAN_FIELD_MOVE] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_RIVAL_MAY_NORMAL] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_RIVAL_MAY_MACH_BIKE] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_RIVAL_MAY_ACRO_BIKE] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_RIVAL_MAY_SURFING] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_RIVAL_MAY_FIELD_MOVE] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_CAMERAMAN] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_BRENDAN_UNDERWATER] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_MAY_UNDERWATER] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_MOVING_BOX] = NPC_TEXT_COLOR_NEUTRAL,
    [OBJ_EVENT_GFX_CABLE_CAR] = NPC_TEXT_COLOR_NEUTRAL,
    [OBJ_EVENT_GFX_SCIENTIST_2] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_DEVON_EMPLOYEE] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_AQUA_MEMBER_M] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_AQUA_MEMBER_F] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_MAGMA_MEMBER_M] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_MAGMA_MEMBER_F] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_SIDNEY] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_PHOEBE] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_GLACIA] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_DRAKE] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_ROXANNE] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_BRAWLY] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_WATTSON] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_FLANNERY] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_NORMAN] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_WINONA] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_LIZA] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_TATE] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_WALLACE] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_STEVEN] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_WALLY] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_LITTLE_BOY_3] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_BRENDAN_FISHING] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_MAY_FISHING] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_HOT_SPRINGS_OLD_WOMAN] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_SS_TIDAL] = NPC_TEXT_COLOR_NEUTRAL,
    [OBJ_EVENT_GFX_SUBMARINE_SHADOW] = NPC_TEXT_COLOR_NEUTRAL,
    [OBJ_EVENT_GFX_LATIAS] = NPC_TEXT_COLOR_MON,
    [OBJ_EVENT_GFX_LATIOS] = NPC_TEXT_COLOR_MON,
    [OBJ_EVENT_GFX_GAMEBOY_KID] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_CONTEST_JUDGE] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_BRENDAN_WATERING] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_MAY_WATERING] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_BRENDAN_DECORATING] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_MAY_DECORATING] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_ARCHIE] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_MAXIE] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_KYOGRE_FRONT] = NPC_TEXT_COLOR_MON,
    [OBJ_EVENT_GFX_GROUDON_FRONT] = NPC_TEXT_COLOR_MON,
    [OBJ_EVENT_GFX_FOSSIL] = NPC_TEXT_COLOR_MON,
    [OBJ_EVENT_GFX_REGIROCK] = NPC_TEXT_COLOR_MON,
    [OBJ_EVENT_GFX_REGICE] = NPC_TEXT_COLOR_MON,
    [OBJ_EVENT_GFX_REGISTEEL] = NPC_TEXT_COLOR_MON,
    [OBJ_EVENT_GFX_SKITTY] = NPC_TEXT_COLOR_MON,
    [OBJ_EVENT_GFX_KECLEON] = NPC_TEXT_COLOR_MON,
    [OBJ_EVENT_GFX_KYOGRE_ASLEEP] = NPC_TEXT_COLOR_MON,
    [OBJ_EVENT_GFX_GROUDON_ASLEEP] = NPC_TEXT_COLOR_MON,
    [OBJ_EVENT_GFX_RAYQUAZA] = NPC_TEXT_COLOR_MON,
    [OBJ_EVENT_GFX_ZIGZAGOON_2] = NPC_TEXT_COLOR_MON,
    [OBJ_EVENT_GFX_PIKACHU] = NPC_TEXT_COLOR_MON,
    [OBJ_EVENT_GFX_AZUMARILL] = NPC_TEXT_COLOR_MON,
    [OBJ_EVENT_GFX_WINGULL] = NPC_TEXT_COLOR_MON,
    [OBJ_EVENT_GFX_KECLEON_BRIDGE_SHADOW] = NPC_TEXT_COLOR_MON,
    [OBJ_EVENT_GFX_TUBER_M_SWIMMING] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_AZURILL] = NPC_TEXT_COLOR_MON,
    [OBJ_EVENT_GFX_MOM] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_LINK_BRENDAN] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_LINK_MAY] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_JUAN] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_SCOTT] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_POOCHYENA] = NPC_TEXT_COLOR_MON,
    [OBJ_EVENT_GFX_KYOGRE_SIDE] = NPC_TEXT_COLOR_MON,
    [OBJ_EVENT_GFX_GROUDON_SIDE] = NPC_TEXT_COLOR_MON,
    [OBJ_EVENT_GFX_MYSTERY_GIFT_MAN] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_TRICK_HOUSE_STATUE] = NPC_TEXT_COLOR_NEUTRAL,
    [OBJ_EVENT_GFX_KIRLIA] = NPC_TEXT_COLOR_MON, 
    [OBJ_EVENT_GFX_DUSCLOPS] = NPC_TEXT_COLOR_MON,
    [OBJ_EVENT_GFX_UNION_ROOM_NURSE] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_SUDOWOODO] = NPC_TEXT_COLOR_MON,
    [OBJ_EVENT_GFX_MEW] = NPC_TEXT_COLOR_MON,
    [OBJ_EVENT_GFX_RED] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_LEAF] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_DEOXYS] = NPC_TEXT_COLOR_MON,
    [OBJ_EVENT_GFX_DEOXYS_TRIANGLE] = NPC_TEXT_COLOR_NEUTRAL,
    [OBJ_EVENT_GFX_BRANDON] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_LINK_RS_BRENDAN] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_LINK_RS_MAY] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_LUGIA] = NPC_TEXT_COLOR_MON,
    [OBJ_EVENT_GFX_HOOH] = NPC_TEXT_COLOR_MON,
    [OBJ_EVENT_GFX_POKE_BALL] = NPC_TEXT_COLOR_NEUTRAL,
    [OBJ_EVENT_GFX_DAISY] = NPC_TEXT_COLOR_FEMALE,
    [OBJ_EVENT_GFX_BLUE] = NPC_TEXT_COLOR_MALE,
};

void DynamicPlaceholderTextUtil_Reset(void)
{
    const u8 **ptr;
    u8 *fillval;
    const u8 **ptr2;

    ptr = sStringPointers;
    fillval = NULL;
    ptr2 = ptr + (ARRAY_COUNT(sStringPointers) - 1);
    do
    {
        *ptr2-- = fillval;
    } while ((int)ptr2 >= (int)ptr);
}

void DynamicPlaceholderTextUtil_SetPlaceholderPtr(u8 idx, const u8 *ptr)
{
    if (idx < ARRAY_COUNT(sStringPointers))
    {
        sStringPointers[idx] = ptr;
    }
}

u8 *DynamicPlaceholderTextUtil_ExpandPlaceholders(u8 *dest, const u8 *src)
{
    while (*src != EOS)
    {
        if (*src != CHAR_DYNAMIC)
        {
            *dest++ = *src++;
        }
        else
        {
            src++;
            if (sStringPointers[*src] != NULL)
            {
                dest = StringCopy(dest, sStringPointers[*src]);
            }
            src++;
        }
    }
    *dest = EOS;
    return dest;
}

const u8 *DynamicPlaceholderTextUtil_GetPlaceholderPtr(u8 idx)
{
    return sStringPointers[idx];
}

u8 GetColorFromTextColorTable(u16 graphicId)
{
    if (graphicId >= NELEMS(sTextColorTable))
        return NPC_TEXT_COLOR_NEUTRAL;
    return sTextColorTable[graphicId];
}
