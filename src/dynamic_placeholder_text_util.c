#include "global.h"
#include "text.h"
#include "dynamic_placeholder_text_util.h"
#include "string_util.h"
#include "constants/event_objects.h"

static EWRAM_DATA const u8 *sStringPointers[8] = {};

static const u8 sTextColorTable[] =
{
    [OBJ_EVENT_GFX_BRENDAN_NORMAL] = NPC_TEXT_COLOR_MALE,
    [OBJ_EVENT_GFX_MOM] = NPC_TEXT_COLOR_FEMALE,
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
        return NPC_TEXT_COLOR_DEFAULT;
    DebugPrintf("graphicId: %d", graphicId);
    return sTextColorTable[graphicId];
}
