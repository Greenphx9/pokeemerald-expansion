#ifndef TERA_RAID_SCREEN_H
#define TERA_RAID_SCREEN_H

#include "gba/types.h"
#include "main.h"
#include "tera_raid.h"

/*
 * Entry tasks for the different versions of the UI. In `start_menu.c', we'll launch a task using one of
 * these functions. You can change which is used to change which menu is launched.
 */

extern struct TeraRaidMon gTeraRaidEncounter;
extern u8 gTeraRaidStars;
extern const u8 gTeraRaidStarToLevel[][2];
extern u8 gTeraRaidSelectedPartner;

extern const struct TeraRaidPartner gTeraRaidPartners[];

// Launch the basic version of the UI
void Task_OpenSampleUi_StartHere(u8 taskId);

#endif