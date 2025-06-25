#ifndef GUARD_OPTION_MENU_PLUS_H
#define GUARD_OPTION_MENU_PLUS_H

// see sOptionMenuPlus_Pages and use the template to add more options or pages!

void OptionMenuPlus_SetupCB(void);
void Task_OpenOptionsMenuPlus(u8 taskId);

struct OptionPage
{
    const u8 *name;
    const struct Option *options;
};

struct Option
{
    const u8 *name;
    const u8 *const *options;
    const u8 *const *optionsDesc;
    bool8 sameDesc;
    u8 optionCount;
    u16 (*func)(u8 action);  // 0 = get, non-zero = set/add
};

#endif // GUARD_OPTION_MENU_PLUS_H
