#ifndef GUARD_FRLG_ITEM_MENU_H
#define GUARD_FRLG_ITEM_MENU_H

#include "global.h"
#include "main.h"
#include "task.h"
#include "constants/item.h"
#include "constants/item_menu.h"

// Exported type declarations

// Exported RAM declarations

struct BagStruct
{
    MainCallback bagCallback;
    u8 location;
    bool8 bagOpen;
    u16 pocket;
    u16 itemsAbove[POCKETS_COUNT + 1];
    u16 cursorPos[POCKETS_COUNT + 1];
};

extern struct BagStruct gBagMenuState;

extern u16 gSpecialVar_ItemId;

// Exported ROM declarations
void CB2_FRLG_SetUpReshowBattleScreenAfterMenu(void);
void ResetBagCursorPositions(void);
void InitPokedudeBag(u8);
void CB2_FRLG_BagMenuFromStartMenu(void);
void FRLG_MoveItemSlotInList(struct ItemSlot * itemSlots_, u32 from, u32 to_);
void ItemMenu_StartFadeToExitCallback(u8 taskId);
void Bag_BeginCloseWin0Animation(void);
void ItemMenu_SetExitCallback(void (*)(void));
void DisplayItemMessageInBag(u8 taskId, u8 fontId, const u8 * string, TaskFunc followUpFunc);
void Task_ReturnToBagFromContextMenu(u8 taskId);
void CB2_FRLG_BagMenuFromBattle(void);
void InitOldManBag(void);
void Pocket_CalculateNItemsAndMaxShowed(u8 pocketId);
void PocketCalculateInitialCursorPosAndItemsAbove(u8 pocketId);
void FRLG_GoToBagMenu(u8 menuType, u8 pocket, MainCallback callback);
bool8 FRLG_UseRegisteredKeyItemOnField(void);

#endif //GUARD_FRLG_ITEM_MENU_H
