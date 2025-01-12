#include "global.h"
#include "event_data.h"
#include "palette.h"
#include "character_customization.h"
#include "constants/event_objects.h"
#include "constants/rgb.h"

// vars
#define VAR_PLAYER_HAT_COLOR VAR_UNUSED_0x40F7

// color arrays
const u16 sCharacterHatColors[HAT_COLOR_COUNT][2] =
{
    [HAT_COLOR_GREEN] = { RGB(14, 25, 14), RGB(9, 18, 10) },
    [HAT_COLOR_RED] = { RGB(25, 14, 14), RGB(18, 9, 9) },
    [HAT_COLOR_BLUE] = { RGB(14, 14, 25), RGB(9, 9, 18) },
    [HAT_COLOR_YELLOW] = { RGB(30, 30, 14), RGB(25, 25, 9) },
    [HAT_COLOR_PURPLE] = { RGB(20, 14, 20), RGB(13, 9, 13) },
    [HAT_COLOR_PINK] = { RGB(25, 14, 18), RGB(18, 9, 14) },
};

// color id offset
#define HAT_OFFSET 10

// functions
void UpdateCharacterPalette(u32 objEventPalOff)
{
    u32 offset = 0x100 + (objEventPalOff * 16);
    #if VAR_PLAYER_HAT_COLOR == 0
    u32 hatColor = HAT_COLOR_GREEN;
    #else
    u32 hatColor = VarGet(VAR_PLAYER_HAT_COLOR);
    #endif
    CpuCopy16(&sCharacterHatColors[hatColor], &gPlttBufferUnfaded[offset + HAT_OFFSET], 2 * sizeof(u16));
}   