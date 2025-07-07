PalletTown_PlayersHouse_1F_MapScripts::
	.byte 0


PalletTown_PlayersHouse_1F_EventScript_TV::
# 8 "data/maps/PalletTown_PlayersHouse_1F/scripts.pory"
	lockall
# 9 "data/maps/PalletTown_PlayersHouse_1F/scripts.pory"
	compare VAR_FACING, DIR_NORTH
	goto_if_ne PalletTown_PlayersHouse_1F_EventScript_TV_2
# 15 "data/maps/PalletTown_PlayersHouse_1F/scripts.pory"
	checkplayergender
# 16 "data/maps/PalletTown_PlayersHouse_1F/scripts.pory"
	compare VAR_RESULT, MALE
	goto_if_eq PalletTown_PlayersHouse_1F_EventScript_TV_5
# 23 "data/maps/PalletTown_PlayersHouse_1F/scripts.pory"
	msgbox PalletTown_PlayersHouse_1F_EventScript_TV_Text_2
PalletTown_PlayersHouse_1F_EventScript_TV_1:
# 27 "data/maps/PalletTown_PlayersHouse_1F/scripts.pory"
	releaseall
	end

PalletTown_PlayersHouse_1F_EventScript_TV_2:
# 11 "data/maps/PalletTown_PlayersHouse_1F/scripts.pory"
	msgbox PalletTown_PlayersHouse_1F_EventScript_TV_Text_0
	goto PalletTown_PlayersHouse_1F_EventScript_TV_1

PalletTown_PlayersHouse_1F_EventScript_TV_5:
# 18 "data/maps/PalletTown_PlayersHouse_1F/scripts.pory"
	msgbox PalletTown_PlayersHouse_1F_EventScript_TV_Text_1
	goto PalletTown_PlayersHouse_1F_EventScript_TV_1


PalletTown_PlayersHouse_1F_EventScript_TV_Text_0:
# 11 "data/maps/PalletTown_PlayersHouse_1F/scripts.pory"
	.string "Oops, wrong side…$"

PalletTown_PlayersHouse_1F_EventScript_TV_Text_1:
# 18 "data/maps/PalletTown_PlayersHouse_1F/scripts.pory"
	.string "There's a movie on TV.\nFour boys are walking on railroad\ltracks.\p…I better go, too.$"

PalletTown_PlayersHouse_1F_EventScript_TV_Text_2:
# 23 "data/maps/PalletTown_PlayersHouse_1F/scripts.pory"
	.string "There's a movie on TV.\nA girl with her hair in pigtails is\lwalking up a brick road.\p…I better go, too.$"
