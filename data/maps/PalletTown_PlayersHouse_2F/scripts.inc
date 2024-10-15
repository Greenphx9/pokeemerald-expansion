PalletTown_PlayersHouse_2F_MapScripts::
	.byte 0

PalletTown_PlayersHouse_2F_EventScript_PC::
	setvar VAR_0x8004, PC_LOCATION_KANTO_HOUSE
	special DoPCTurnOnEffect
	playse SE_PC_ON
	msgbox gText_PlayerHouseBootPC
	special BedroomPC
	waitstate
	releaseall
	end

EventScript_PalletTown_PlayersHouse_2F_ShutDownPC::
	setvar VAR_0x8004, PC_LOCATION_KANTO_HOUSE
	playse SE_PC_OFF
	special DoPCTurnOffEffect
	releaseall
	end