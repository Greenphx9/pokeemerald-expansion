EventScript_Raid_Start::
    special IsRaidAvailable
	special InitTeraRaidScreen
	waitstate
	goto_if_eq VAR_RESULT, 0, EventScript_Raid_End
	special SavePlayerParty
	fadescreen FADE_TO_BLACK
	special ChooseHalfPartyForBattle
	waitstate
	goto_if_eq VAR_RESULT, 0, EventScript_Raid_End
	special CreateTeraRaidMon
	special SavePlayerParty
	special ReducePlayerPartyToSelectedMons
	setvar VAR_0x8004, FRONTIER_UTIL_FUNC_SET_DATA
	setvar VAR_0x8005, FRONTIER_DATA_SELECTED_MON_ORDER
	special CallFrontierUtilFunc @ saves the mon order, so the non-selected mons get restored afterwards
	setvar VAR_0x8004, SPECIAL_BATTLE_MULTI
	setvar VAR_0x8005, MULTI_BATTLE_TERA_RAID | MULTI_BATTLE_CHOOSE_MONS
	setvar VAR_0x8006, 0
	special DoSpecialTrainerBattle
	waitstate
	setvar VAR_0x8004, FRONTIER_UTIL_FUNC_SAVE_PARTY
	special CallFrontierUtilFunc
	special LoadPlayerParty
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, EventScript_Raid_ShowRewardScreen
	goto_if_eq VAR_RESULT, B_OUTCOME_RAN, EventScript_Raid_ShowRewardScreen
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, EventScript_Raid_ShowRewardScreen
	release
	end

EventScript_Raid_ShowRewardScreen:
	special InitTeraRaidRewardScreen
	release
	end

EventScript_Raid_End:
	release
	end
