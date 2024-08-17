#ifndef GUARD_TM_CASE_H
#define GUARD_TM_CASE_H

#define TM_CASE_GREYSCALE       0
#define TM_CASE_TRANSLUCENT     1

// TM CASE OPTIONS
// Note that all fonts in the TM case use the FRLG font. Without this, the text display gets broken and I don't have the knowledge to fix it. Sorry about that.
#define TM_CASE_MON_ICONS               FALSE                // If TRUE, shows mon icons in the tm case. These go grey / become translucent if they cannot learn the move. Also changes the tm case tilemap.
#define TM_CASE_MON_ICON_TYPE           TM_CASE_GREYSCALE    // When GREYSCALE, mons that cannot learn the move have their icon greyscaled. When TRANSLUCENT, they have their icon become translucent / see through.
#define TM_CASE_DISABLE_ANIM_KNOWS_MOVE FALSE                // If TRUE, disables the mon icon anim if the mon already knows the move. This is taken from Skeli's tm case code.


// Values for 'type' argument to InitTMCase
enum {
    TMCASE_FIELD,
    TMCASE_GIVE_PARTY,
    TMCASE_SELL,
    TMCASE_GIVE_PC,
    TMCASE_REOPENING,
};

// Alternative value for 'allowSelectClose' argument to InitTMCase.
// Indicates that the previous value should be preserved
#define TMCASE_KEEP_PREV 0xFF

void InitTMCase(u8 type, void (* exitCallback)(void), bool8 allowSelectClose);
void ResetTMCaseCursorPos(void);

#endif //GUARD_TM_CASE_H
