PalletTown_MapScripts::
# 8 "data/maps/PalletTown/scripts.pory"
	map_script MAP_SCRIPT_ON_LOAD, PalletTown_MapScripts_MAP_SCRIPT_ON_LOAD
	.byte 0

PalletTown_MapScripts_MAP_SCRIPT_ON_LOAD:
# 9 "data/maps/PalletTown/scripts.pory"
	setflag FLAG_TEMP_1
# 10 "data/maps/PalletTown/scripts.pory"
	setflag FLAG_TEMP_2
# 12 "data/maps/PalletTown/scripts.pory"
	random 3
# 13 "data/maps/PalletTown/scripts.pory"
	switch VAR_RESULT
# 14 "data/maps/PalletTown/scripts.pory"
	case 0, PalletTown_MapScripts_MAP_SCRIPT_ON_LOAD_2
# 16 "data/maps/PalletTown/scripts.pory"
	case 1, PalletTown_MapScripts_MAP_SCRIPT_ON_LOAD_3
# 18 "data/maps/PalletTown/scripts.pory"
	case 2, PalletTown_MapScripts_MAP_SCRIPT_ON_LOAD_4
	return

PalletTown_MapScripts_MAP_SCRIPT_ON_LOAD_2:
# 15 "data/maps/PalletTown/scripts.pory"
	clearflag FLAG_TEMP_1
	return

PalletTown_MapScripts_MAP_SCRIPT_ON_LOAD_3:
# 17 "data/maps/PalletTown/scripts.pory"
	clearflag FLAG_TEMP_2
	return

PalletTown_MapScripts_MAP_SCRIPT_ON_LOAD_4:
# 19 "data/maps/PalletTown/scripts.pory"
	clearflag FLAG_TEMP_1
# 20 "data/maps/PalletTown/scripts.pory"
	clearflag FLAG_TEMP_2
	return


# 25 "data/maps/PalletTown/scripts.pory"

# 26 "data/maps/PalletTown/scripts.pory"
.equ LOCALID_SIGN_LADY, 1
# 27 "data/maps/PalletTown/scripts.pory"
.equ LOCALID_PROF_OAK,  3
# 28 "data/maps/PalletTown/scripts.pory"

# 29 "data/maps/PalletTown/scripts.pory"
.equ SIGN_LADY_READY, VAR_TEMP_2
# 30 "data/maps/PalletTown/scripts.pory"

# 31 "data/maps/PalletTown/scripts.pory"
PalletTown_OnTransition::
# 32 "data/maps/PalletTown/scripts.pory"
	setworldmapflag FLAG_WORLD_MAP_PALLET_TOWN
# 33 "data/maps/PalletTown/scripts.pory"
	call_if_set FLAG_PALLET_LADY_NOT_BLOCKING_SIGN, PalletTown_EventScript_TryReadySignLady
# 34 "data/maps/PalletTown/scripts.pory"
	call_if_eq VAR_MAP_SCENE_PALLET_TOWN_SIGN_LADY, 0, PalletTown_EventScript_SetSignLadyPos
# 35 "data/maps/PalletTown/scripts.pory"
	call_if_eq VAR_MAP_SCENE_PALLET_TOWN_SIGN_LADY, 1, PalletTown_EventScript_SetSignLadyDone
# 36 "data/maps/PalletTown/scripts.pory"
	end
# 37 "data/maps/PalletTown/scripts.pory"

# 38 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_TryReadySignLady::
# 39 "data/maps/PalletTown/scripts.pory"
	goto_if_unset FLAG_OPENED_START_MENU, Common_EventScript_NopReturn
# 40 "data/maps/PalletTown/scripts.pory"
	goto_if_ge VAR_MAP_SCENE_PALLET_TOWN_SIGN_LADY, 1, Common_EventScript_NopReturn
# 41 "data/maps/PalletTown/scripts.pory"
	setvar VAR_MAP_SCENE_PALLET_TOWN_SIGN_LADY, 1
# 42 "data/maps/PalletTown/scripts.pory"
	return
# 43 "data/maps/PalletTown/scripts.pory"

# 44 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_SetSignLadyDone::
# 45 "data/maps/PalletTown/scripts.pory"
	setvar VAR_MAP_SCENE_PALLET_TOWN_SIGN_LADY, 2
# 46 "data/maps/PalletTown/scripts.pory"
	return
# 47 "data/maps/PalletTown/scripts.pory"

# 48 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_SetSignLadyPos::
# 49 "data/maps/PalletTown/scripts.pory"
	goto_if_set FLAG_PALLET_LADY_NOT_BLOCKING_SIGN, PalletTown_EventScript_MoveSignLadyToRouteEntrance
# 50 "data/maps/PalletTown/scripts.pory"
	setobjectxyperm LOCALID_SIGN_LADY, 5, 15
# 51 "data/maps/PalletTown/scripts.pory"
	setobjectmovementtype LOCALID_SIGN_LADY, MOVEMENT_TYPE_FACE_UP
# 52 "data/maps/PalletTown/scripts.pory"
	return
# 53 "data/maps/PalletTown/scripts.pory"

# 54 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_MoveSignLadyToRouteEntrance::
# 55 "data/maps/PalletTown/scripts.pory"
	setobjectxyperm LOCALID_SIGN_LADY, 12, 2
# 56 "data/maps/PalletTown/scripts.pory"
	setobjectmovementtype LOCALID_SIGN_LADY, MOVEMENT_TYPE_FACE_DOWN
# 57 "data/maps/PalletTown/scripts.pory"
	setvar SIGN_LADY_READY, TRUE
# 58 "data/maps/PalletTown/scripts.pory"
	return
# 59 "data/maps/PalletTown/scripts.pory"

# 60 "data/maps/PalletTown/scripts.pory"
PalletTown_OnFrame::
# 61 "data/maps/PalletTown/scripts.pory"
	map_script_2 VAR_MAP_SCENE_PALLET_TOWN_OAK, 2, PalletTown_EventScript_OakRatingScene
# 62 "data/maps/PalletTown/scripts.pory"
	.2byte 0
# 63 "data/maps/PalletTown/scripts.pory"

# 64 "data/maps/PalletTown/scripts.pory"
@ Oak approaches player after exiting their house post Elite Four to check if they should receieve the National Dex
# 65 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_OakRatingScene::
# 66 "data/maps/PalletTown/scripts.pory"
	lockall
# 67 "data/maps/PalletTown/scripts.pory"
	textcolor NPC_TEXT_COLOR_MALE
# 68 "data/maps/PalletTown/scripts.pory"
	setobjectxyperm LOCALID_PROF_OAK, 14, 14
# 69 "data/maps/PalletTown/scripts.pory"
	setobjectmovementtype LOCALID_PROF_OAK, MOVEMENT_TYPE_FACE_LEFT
# 70 "data/maps/PalletTown/scripts.pory"
	addobject LOCALID_PROF_OAK
# 71 "data/maps/PalletTown/scripts.pory"
	applymovement LOCALID_PROF_OAK, PalletTown_Movement_OakWalkToPlayersDoor
# 72 "data/maps/PalletTown/scripts.pory"
	waitmovement 0
# 73 "data/maps/PalletTown/scripts.pory"
	goto_if_unset FLAG_WORLD_MAP_ONE_ISLAND, PalletTown_EventScript_NotBeenToOneIslandYet
# 74 "data/maps/PalletTown/scripts.pory"
	msgbox PalletTown_Text_OakLetMeSeePokedex
# 75 "data/maps/PalletTown/scripts.pory"
	setvar VAR_0x8004, 0
# 76 "data/maps/PalletTown/scripts.pory"
	specialvar VAR_RESULT, ScriptGetPokedexInfo
# 77 "data/maps/PalletTown/scripts.pory"
	copyvar VAR_0x8008, VAR_0x8005
# 78 "data/maps/PalletTown/scripts.pory"
	copyvar VAR_0x8009, VAR_0x8006
# 79 "data/maps/PalletTown/scripts.pory"
	buffernumberstring STR_VAR_1, VAR_0x8008
# 80 "data/maps/PalletTown/scripts.pory"
	buffernumberstring STR_VAR_2, VAR_0x8009
# 81 "data/maps/PalletTown/scripts.pory"
	goto_if_lt VAR_0x8009, 60, PalletTown_EventScript_NotEnoughMonsForNationalDex
# 82 "data/maps/PalletTown/scripts.pory"
	msgbox PalletTown_Text_CaughtXImpressiveFollowMe
# 83 "data/maps/PalletTown/scripts.pory"
	closemessage
# 84 "data/maps/PalletTown/scripts.pory"
	playbgm MUS_FOLLOW_ME, 0
# 85 "data/maps/PalletTown/scripts.pory"
	applymovement OBJ_EVENT_ID_PLAYER, PalletTown_Movement_PlayerWalkToLabFromHouse
# 86 "data/maps/PalletTown/scripts.pory"
	applymovement LOCALID_PROF_OAK, PalletTown_Movement_OakWalkToLabFromHouse
# 87 "data/maps/PalletTown/scripts.pory"
	waitmovement 0
# 88 "data/maps/PalletTown/scripts.pory"
	opendoor 16, 13
# 89 "data/maps/PalletTown/scripts.pory"
	waitdooranim
# 90 "data/maps/PalletTown/scripts.pory"
	applymovement LOCALID_PROF_OAK, PalletTown_Movement_OakEnterLab
# 91 "data/maps/PalletTown/scripts.pory"
	applymovement OBJ_EVENT_ID_PLAYER, PalletTown_Movement_PlayerEnterLab
# 92 "data/maps/PalletTown/scripts.pory"
	waitmovement 0
# 93 "data/maps/PalletTown/scripts.pory"
	closedoor 16, 13
# 94 "data/maps/PalletTown/scripts.pory"
	waitdooranim
# 95 "data/maps/PalletTown/scripts.pory"
	setvar VAR_MAP_SCENE_PALLET_TOWN_PROFESSOR_OAKS_LAB, 7
# 96 "data/maps/PalletTown/scripts.pory"
	setvar VAR_MAP_SCENE_PALLET_TOWN_OAK, 3
# 97 "data/maps/PalletTown/scripts.pory"
	setflag FLAG_HIDE_OAK_IN_PALLET_TOWN
# 98 "data/maps/PalletTown/scripts.pory"
	@warp MAP_PALLET_TOWN_PROFESSOR_OAKS_LAB, 6, 12
# 99 "data/maps/PalletTown/scripts.pory"
	waitstate
# 100 "data/maps/PalletTown/scripts.pory"
	releaseall
# 101 "data/maps/PalletTown/scripts.pory"
	end
# 102 "data/maps/PalletTown/scripts.pory"

# 103 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_EndOakRatingScene::
# 104 "data/maps/PalletTown/scripts.pory"
	closemessage
# 105 "data/maps/PalletTown/scripts.pory"
	applymovement LOCALID_PROF_OAK, PalletTown_Movement_OakExit
# 106 "data/maps/PalletTown/scripts.pory"
	waitmovement 0
# 107 "data/maps/PalletTown/scripts.pory"
	removeobject LOCALID_PROF_OAK
# 108 "data/maps/PalletTown/scripts.pory"
	setvar VAR_MAP_SCENE_PALLET_TOWN_OAK, 3
# 109 "data/maps/PalletTown/scripts.pory"
	releaseall
# 110 "data/maps/PalletTown/scripts.pory"
	end
# 111 "data/maps/PalletTown/scripts.pory"

# 112 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_NotEnoughMonsForNationalDex::
# 113 "data/maps/PalletTown/scripts.pory"
	msgbox PalletTown_Text_CaughtXPuttingInHonestEffort
# 114 "data/maps/PalletTown/scripts.pory"
	goto PalletTown_EventScript_EndOakRatingScene
# 115 "data/maps/PalletTown/scripts.pory"
	end
# 116 "data/maps/PalletTown/scripts.pory"

# 117 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_NotBeenToOneIslandYet::
# 118 "data/maps/PalletTown/scripts.pory"
	msgbox PalletTown_Text_OakYouEnjoyingTraveling
# 119 "data/maps/PalletTown/scripts.pory"
	goto PalletTown_EventScript_EndOakRatingScene
# 120 "data/maps/PalletTown/scripts.pory"
	end
# 121 "data/maps/PalletTown/scripts.pory"

# 122 "data/maps/PalletTown/scripts.pory"
PalletTown_Movement_OakWalkToPlayersDoor::
# 123 "data/maps/PalletTown/scripts.pory"
	walk_left
# 124 "data/maps/PalletTown/scripts.pory"
	walk_left
# 125 "data/maps/PalletTown/scripts.pory"
	walk_left
# 126 "data/maps/PalletTown/scripts.pory"
	walk_up
# 127 "data/maps/PalletTown/scripts.pory"
	walk_up
# 128 "data/maps/PalletTown/scripts.pory"
	walk_up
# 129 "data/maps/PalletTown/scripts.pory"
	walk_up
# 130 "data/maps/PalletTown/scripts.pory"
	walk_up
# 131 "data/maps/PalletTown/scripts.pory"
	walk_left
# 132 "data/maps/PalletTown/scripts.pory"
	walk_left
# 133 "data/maps/PalletTown/scripts.pory"
	walk_left
# 134 "data/maps/PalletTown/scripts.pory"
	walk_left
# 135 "data/maps/PalletTown/scripts.pory"
	walk_left
# 136 "data/maps/PalletTown/scripts.pory"
	walk_in_place_faster_up
# 137 "data/maps/PalletTown/scripts.pory"
	step_end
# 138 "data/maps/PalletTown/scripts.pory"

# 139 "data/maps/PalletTown/scripts.pory"
PalletTown_Movement_OakExit::
# 140 "data/maps/PalletTown/scripts.pory"
	walk_right
# 141 "data/maps/PalletTown/scripts.pory"
	walk_right
# 142 "data/maps/PalletTown/scripts.pory"
	walk_right
# 143 "data/maps/PalletTown/scripts.pory"
	walk_right
# 144 "data/maps/PalletTown/scripts.pory"
	walk_right
# 145 "data/maps/PalletTown/scripts.pory"
	walk_down
# 146 "data/maps/PalletTown/scripts.pory"
	walk_down
# 147 "data/maps/PalletTown/scripts.pory"
	walk_down
# 148 "data/maps/PalletTown/scripts.pory"
	walk_down
# 149 "data/maps/PalletTown/scripts.pory"
	walk_down
# 150 "data/maps/PalletTown/scripts.pory"
	walk_right
# 151 "data/maps/PalletTown/scripts.pory"
	walk_right
# 152 "data/maps/PalletTown/scripts.pory"
	walk_right
# 153 "data/maps/PalletTown/scripts.pory"
	step_end
# 154 "data/maps/PalletTown/scripts.pory"

# 155 "data/maps/PalletTown/scripts.pory"
PalletTown_Movement_OakWalkToLabFromHouse::
# 156 "data/maps/PalletTown/scripts.pory"
	walk_right
# 157 "data/maps/PalletTown/scripts.pory"
	walk_right
# 158 "data/maps/PalletTown/scripts.pory"
	walk_right
# 159 "data/maps/PalletTown/scripts.pory"
	walk_right
# 160 "data/maps/PalletTown/scripts.pory"
	walk_right
# 161 "data/maps/PalletTown/scripts.pory"
	walk_down
# 162 "data/maps/PalletTown/scripts.pory"
	walk_down
# 163 "data/maps/PalletTown/scripts.pory"
	walk_down
# 164 "data/maps/PalletTown/scripts.pory"
	walk_down
# 165 "data/maps/PalletTown/scripts.pory"
	walk_down
# 166 "data/maps/PalletTown/scripts.pory"
	walk_right
# 167 "data/maps/PalletTown/scripts.pory"
	walk_right
# 168 "data/maps/PalletTown/scripts.pory"
	walk_right
# 169 "data/maps/PalletTown/scripts.pory"
	walk_right
# 170 "data/maps/PalletTown/scripts.pory"
	walk_right
# 171 "data/maps/PalletTown/scripts.pory"
	walk_in_place_faster_up
# 172 "data/maps/PalletTown/scripts.pory"
	step_end
# 173 "data/maps/PalletTown/scripts.pory"

# 174 "data/maps/PalletTown/scripts.pory"
PalletTown_Movement_PlayerWalkToLabFromHouse::
# 175 "data/maps/PalletTown/scripts.pory"
	walk_down
# 176 "data/maps/PalletTown/scripts.pory"
	walk_right
# 177 "data/maps/PalletTown/scripts.pory"
	walk_right
# 178 "data/maps/PalletTown/scripts.pory"
	walk_right
# 179 "data/maps/PalletTown/scripts.pory"
	walk_right
# 180 "data/maps/PalletTown/scripts.pory"
	walk_right
# 181 "data/maps/PalletTown/scripts.pory"
	walk_down
# 182 "data/maps/PalletTown/scripts.pory"
	walk_down
# 183 "data/maps/PalletTown/scripts.pory"
	walk_down
# 184 "data/maps/PalletTown/scripts.pory"
	walk_down
# 185 "data/maps/PalletTown/scripts.pory"
	walk_down
# 186 "data/maps/PalletTown/scripts.pory"
	walk_right
# 187 "data/maps/PalletTown/scripts.pory"
	walk_right
# 188 "data/maps/PalletTown/scripts.pory"
	walk_right
# 189 "data/maps/PalletTown/scripts.pory"
	walk_right
# 190 "data/maps/PalletTown/scripts.pory"
	step_end
# 191 "data/maps/PalletTown/scripts.pory"

# 192 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_OakTriggerLeft::
# 193 "data/maps/PalletTown/scripts.pory"
	lockall
# 194 "data/maps/PalletTown/scripts.pory"
	setvar VAR_TEMP_1, 0
# 195 "data/maps/PalletTown/scripts.pory"
	goto PalletTown_EventScript_OakTrigger
# 196 "data/maps/PalletTown/scripts.pory"
	end
# 197 "data/maps/PalletTown/scripts.pory"

# 198 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_OakTriggerRight::
# 199 "data/maps/PalletTown/scripts.pory"
	lockall
# 200 "data/maps/PalletTown/scripts.pory"
	setvar VAR_TEMP_1, 1
# 201 "data/maps/PalletTown/scripts.pory"
	goto PalletTown_EventScript_OakTrigger
# 202 "data/maps/PalletTown/scripts.pory"
	end
# 203 "data/maps/PalletTown/scripts.pory"

# 204 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_OakTrigger::
# 205 "data/maps/PalletTown/scripts.pory"
	@famechecker FAMECHECKER_OAK, FCPICKSTATE_COLORED, UpdatePickStateFromSpecialVar8005
# 206 "data/maps/PalletTown/scripts.pory"
	textcolor NPC_TEXT_COLOR_MALE
# 207 "data/maps/PalletTown/scripts.pory"
	delay 30
# 208 "data/maps/PalletTown/scripts.pory"
	playbgm MUS_RG_OAK, 0
# 209 "data/maps/PalletTown/scripts.pory"
	message PalletTown_Text_OakDontGoOut
# 210 "data/maps/PalletTown/scripts.pory"
	waitmessage
# 211 "data/maps/PalletTown/scripts.pory"
	delay 85
# 212 "data/maps/PalletTown/scripts.pory"
	closemessage
# 213 "data/maps/PalletTown/scripts.pory"
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFasterDown
# 214 "data/maps/PalletTown/scripts.pory"
	waitmovement 0
# 215 "data/maps/PalletTown/scripts.pory"
	playse SE_PIN
# 216 "data/maps/PalletTown/scripts.pory"
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_ExclamationMark
# 217 "data/maps/PalletTown/scripts.pory"
	waitmovement 0
# 218 "data/maps/PalletTown/scripts.pory"
	delay 30
# 219 "data/maps/PalletTown/scripts.pory"
	addobject LOCALID_PROF_OAK
# 220 "data/maps/PalletTown/scripts.pory"
	call_if_eq VAR_TEMP_1, 0, PalletTown_EventScript_OakEnterLeft
# 221 "data/maps/PalletTown/scripts.pory"
	call_if_eq VAR_TEMP_1, 1, PalletTown_EventScript_OakEnterRight
# 222 "data/maps/PalletTown/scripts.pory"
	delay 30
# 223 "data/maps/PalletTown/scripts.pory"
	msgbox PalletTown_Text_OakGrassUnsafeNeedMon
# 224 "data/maps/PalletTown/scripts.pory"
	closemessage
# 225 "data/maps/PalletTown/scripts.pory"
	delay 30
# 226 "data/maps/PalletTown/scripts.pory"
	call_if_eq VAR_TEMP_1, 0, PalletTown_EventScript_OakLeadPlayerToLabLeft
# 227 "data/maps/PalletTown/scripts.pory"
	call_if_eq VAR_TEMP_1, 1, PalletTown_EventScript_OakLeadPlayerToLabRight
# 228 "data/maps/PalletTown/scripts.pory"
	opendoor 16, 13
# 229 "data/maps/PalletTown/scripts.pory"
	waitdooranim
# 230 "data/maps/PalletTown/scripts.pory"
	applymovement LOCALID_PROF_OAK, PalletTown_Movement_OakEnterLab
# 231 "data/maps/PalletTown/scripts.pory"
	applymovement OBJ_EVENT_ID_PLAYER, PalletTown_Movement_PlayerEnterLab
# 232 "data/maps/PalletTown/scripts.pory"
	waitmovement 0
# 233 "data/maps/PalletTown/scripts.pory"
	closedoor 16, 13
# 234 "data/maps/PalletTown/scripts.pory"
	waitdooranim
# 235 "data/maps/PalletTown/scripts.pory"
	setvar VAR_MAP_SCENE_PALLET_TOWN_PROFESSOR_OAKS_LAB, 1
# 236 "data/maps/PalletTown/scripts.pory"
	clearflag FLAG_HIDE_OAK_IN_HIS_LAB
# 237 "data/maps/PalletTown/scripts.pory"
	setvar VAR_MAP_SCENE_PALLET_TOWN_OAK, 1
# 238 "data/maps/PalletTown/scripts.pory"
	setflag FLAG_HIDE_OAK_IN_PALLET_TOWN
# 239 "data/maps/PalletTown/scripts.pory"
	setflag FLAG_DONT_TRANSITION_MUSIC
# 240 "data/maps/PalletTown/scripts.pory"
	@warp MAP_PALLET_TOWN_PROFESSOR_OAKS_LAB, 6, 12
# 241 "data/maps/PalletTown/scripts.pory"
	waitstate
# 242 "data/maps/PalletTown/scripts.pory"
	releaseall
# 243 "data/maps/PalletTown/scripts.pory"
	end
# 244 "data/maps/PalletTown/scripts.pory"

# 245 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_OakEnterLeft::
# 246 "data/maps/PalletTown/scripts.pory"
	applymovement LOCALID_PROF_OAK, PalletTown_Movement_OakEnterLeft
# 247 "data/maps/PalletTown/scripts.pory"
	waitmovement 0
# 248 "data/maps/PalletTown/scripts.pory"
	return
# 249 "data/maps/PalletTown/scripts.pory"

# 250 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_OakEnterRight::
# 251 "data/maps/PalletTown/scripts.pory"
	applymovement LOCALID_PROF_OAK, PalletTown_Movement_OakEnterRight
# 252 "data/maps/PalletTown/scripts.pory"
	waitmovement 0
# 253 "data/maps/PalletTown/scripts.pory"
	return
# 254 "data/maps/PalletTown/scripts.pory"

# 255 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_OakLeadPlayerToLabLeft::
# 256 "data/maps/PalletTown/scripts.pory"
	applymovement LOCALID_PROF_OAK, PalletTown_Movement_OakWalkToLabLeft
# 257 "data/maps/PalletTown/scripts.pory"
	applymovement OBJ_EVENT_ID_PLAYER, PalletTown_Movement_PlayerWalkToLabLeft
# 258 "data/maps/PalletTown/scripts.pory"
	waitmovement 0
# 259 "data/maps/PalletTown/scripts.pory"
	return
# 260 "data/maps/PalletTown/scripts.pory"

# 261 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_OakLeadPlayerToLabRight::
# 262 "data/maps/PalletTown/scripts.pory"
	applymovement LOCALID_PROF_OAK, PalletTown_Movement_OakWalkToLabRight
# 263 "data/maps/PalletTown/scripts.pory"
	applymovement OBJ_EVENT_ID_PLAYER, PalletTown_Movement_PlayerWalkToLabRight
# 264 "data/maps/PalletTown/scripts.pory"
	waitmovement 0
# 265 "data/maps/PalletTown/scripts.pory"
	return
# 266 "data/maps/PalletTown/scripts.pory"

# 267 "data/maps/PalletTown/scripts.pory"
PalletTown_Movement_OakEnterLeft::
# 268 "data/maps/PalletTown/scripts.pory"
	walk_up
# 269 "data/maps/PalletTown/scripts.pory"
	walk_up
# 270 "data/maps/PalletTown/scripts.pory"
	walk_right
# 271 "data/maps/PalletTown/scripts.pory"
	walk_up
# 272 "data/maps/PalletTown/scripts.pory"
	walk_up
# 273 "data/maps/PalletTown/scripts.pory"
	walk_right
# 274 "data/maps/PalletTown/scripts.pory"
	walk_up
# 275 "data/maps/PalletTown/scripts.pory"
	walk_up
# 276 "data/maps/PalletTown/scripts.pory"
	step_end
# 277 "data/maps/PalletTown/scripts.pory"

# 278 "data/maps/PalletTown/scripts.pory"
PalletTown_Movement_OakEnterRight::
# 279 "data/maps/PalletTown/scripts.pory"
	walk_right
# 280 "data/maps/PalletTown/scripts.pory"
	walk_up
# 281 "data/maps/PalletTown/scripts.pory"
	walk_up
# 282 "data/maps/PalletTown/scripts.pory"
	walk_right
# 283 "data/maps/PalletTown/scripts.pory"
	walk_up
# 284 "data/maps/PalletTown/scripts.pory"
	walk_up
# 285 "data/maps/PalletTown/scripts.pory"
	walk_right
# 286 "data/maps/PalletTown/scripts.pory"
	walk_up
# 287 "data/maps/PalletTown/scripts.pory"
	walk_up
# 288 "data/maps/PalletTown/scripts.pory"
	step_end
# 289 "data/maps/PalletTown/scripts.pory"

# 290 "data/maps/PalletTown/scripts.pory"
	.macro walk_to_lab
# 291 "data/maps/PalletTown/scripts.pory"
	walk_left
# 292 "data/maps/PalletTown/scripts.pory"
	walk_down
# 293 "data/maps/PalletTown/scripts.pory"
	walk_down
# 294 "data/maps/PalletTown/scripts.pory"
	walk_down
# 295 "data/maps/PalletTown/scripts.pory"
	walk_down
# 296 "data/maps/PalletTown/scripts.pory"
	walk_down
# 297 "data/maps/PalletTown/scripts.pory"
	walk_down
# 298 "data/maps/PalletTown/scripts.pory"
	walk_down
# 299 "data/maps/PalletTown/scripts.pory"
	walk_down
# 300 "data/maps/PalletTown/scripts.pory"
	walk_down
# 301 "data/maps/PalletTown/scripts.pory"
	walk_down
# 302 "data/maps/PalletTown/scripts.pory"
	walk_down
# 303 "data/maps/PalletTown/scripts.pory"
	walk_right
# 304 "data/maps/PalletTown/scripts.pory"
	walk_right
# 305 "data/maps/PalletTown/scripts.pory"
	walk_right
# 306 "data/maps/PalletTown/scripts.pory"
	walk_right
# 307 "data/maps/PalletTown/scripts.pory"
	.endm
# 308 "data/maps/PalletTown/scripts.pory"

# 309 "data/maps/PalletTown/scripts.pory"
PalletTown_Movement_OakWalkToLabLeft::
# 310 "data/maps/PalletTown/scripts.pory"
	walk_down
# 311 "data/maps/PalletTown/scripts.pory"
	walk_to_lab
# 312 "data/maps/PalletTown/scripts.pory"
	walk_right
# 313 "data/maps/PalletTown/scripts.pory"
	walk_in_place_faster_up
# 314 "data/maps/PalletTown/scripts.pory"
	step_end
# 315 "data/maps/PalletTown/scripts.pory"

# 316 "data/maps/PalletTown/scripts.pory"
PalletTown_Movement_OakWalkToLabRight::
# 317 "data/maps/PalletTown/scripts.pory"
	walk_down
# 318 "data/maps/PalletTown/scripts.pory"
	walk_left
# 319 "data/maps/PalletTown/scripts.pory"
	walk_to_lab
# 320 "data/maps/PalletTown/scripts.pory"
	walk_right
# 321 "data/maps/PalletTown/scripts.pory"
	walk_in_place_faster_up
# 322 "data/maps/PalletTown/scripts.pory"
	step_end
# 323 "data/maps/PalletTown/scripts.pory"

# 324 "data/maps/PalletTown/scripts.pory"
PalletTown_Movement_OakEnterLab::
# 325 "data/maps/PalletTown/scripts.pory"
	walk_up
# 326 "data/maps/PalletTown/scripts.pory"
	set_invisible
# 327 "data/maps/PalletTown/scripts.pory"
	step_end
# 328 "data/maps/PalletTown/scripts.pory"

# 329 "data/maps/PalletTown/scripts.pory"
PalletTown_Movement_PlayerWalkToLabLeft::
# 330 "data/maps/PalletTown/scripts.pory"
	walk_down
# 331 "data/maps/PalletTown/scripts.pory"
	walk_down
# 332 "data/maps/PalletTown/scripts.pory"
	walk_to_lab
# 333 "data/maps/PalletTown/scripts.pory"
	step_end
# 334 "data/maps/PalletTown/scripts.pory"

# 335 "data/maps/PalletTown/scripts.pory"
PalletTown_Movement_PlayerWalkToLabRight::
# 336 "data/maps/PalletTown/scripts.pory"
	walk_down
# 337 "data/maps/PalletTown/scripts.pory"
	walk_down
# 338 "data/maps/PalletTown/scripts.pory"
	walk_left
# 339 "data/maps/PalletTown/scripts.pory"
	walk_to_lab
# 340 "data/maps/PalletTown/scripts.pory"
	step_end
# 341 "data/maps/PalletTown/scripts.pory"

# 342 "data/maps/PalletTown/scripts.pory"
PalletTown_Movement_PlayerEnterLab::
# 343 "data/maps/PalletTown/scripts.pory"
	walk_right
# 344 "data/maps/PalletTown/scripts.pory"
	walk_up
# 345 "data/maps/PalletTown/scripts.pory"
	set_invisible
# 346 "data/maps/PalletTown/scripts.pory"
	step_end
# 347 "data/maps/PalletTown/scripts.pory"

# 348 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_SignLady::
# 349 "data/maps/PalletTown/scripts.pory"
	lock
# 350 "data/maps/PalletTown/scripts.pory"
	goto_if_eq VAR_MAP_SCENE_PALLET_TOWN_SIGN_LADY, 2, PalletTown_EventScript_SignLadyDone
# 351 "data/maps/PalletTown/scripts.pory"
	goto_if_eq VAR_MAP_SCENE_PALLET_TOWN_SIGN_LADY, 1, PalletTown_EventScript_SignLadyJustShowedSign
# 352 "data/maps/PalletTown/scripts.pory"
	goto_if_eq SIGN_LADY_READY, TRUE, PalletTown_EventScript_SignLadyStartShowSign
# 353 "data/maps/PalletTown/scripts.pory"
	goto_if_set FLAG_TEMP_2, PalletTown_EventScript_SignLadyGoReadSign
# 354 "data/maps/PalletTown/scripts.pory"
	msgbox PalletTown_Text_HmmIsThatRight
# 355 "data/maps/PalletTown/scripts.pory"
	applymovement LOCALID_SIGN_LADY, Common_Movement_FacePlayer
# 356 "data/maps/PalletTown/scripts.pory"
	waitmovement 0
# 357 "data/maps/PalletTown/scripts.pory"
	playse SE_PIN
# 358 "data/maps/PalletTown/scripts.pory"
	applymovement LOCALID_SIGN_LADY, Common_Movement_ExclamationMark
# 359 "data/maps/PalletTown/scripts.pory"
	waitmovement 0
# 360 "data/maps/PalletTown/scripts.pory"
	applymovement LOCALID_SIGN_LADY, Common_Movement_Delay48
# 361 "data/maps/PalletTown/scripts.pory"
	waitmovement 0
# 362 "data/maps/PalletTown/scripts.pory"
	msgbox PalletTown_Text_OhLookLook
# 363 "data/maps/PalletTown/scripts.pory"
	closemessage
# 364 "data/maps/PalletTown/scripts.pory"
	call_if_eq VAR_FACING, DIR_EAST, PalletTown_EventScript_SignLadyMoveOutOfWayRight
# 365 "data/maps/PalletTown/scripts.pory"
	call_if_ne VAR_FACING, DIR_EAST, PalletTown_EventScript_SignLadyMoveOutOfWayLeft
# 366 "data/maps/PalletTown/scripts.pory"
	copyobjectxytoperm LOCALID_SIGN_LADY
# 367 "data/maps/PalletTown/scripts.pory"
	setflag FLAG_TEMP_2
# 368 "data/maps/PalletTown/scripts.pory"
	release
# 369 "data/maps/PalletTown/scripts.pory"
	end
# 370 "data/maps/PalletTown/scripts.pory"

# 371 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_SignLadyMoveOutOfWayRight::
# 372 "data/maps/PalletTown/scripts.pory"
	applymovement LOCALID_SIGN_LADY, PalletTown_Movement_SignLadyMoveOutOfWayRight
# 373 "data/maps/PalletTown/scripts.pory"
	waitmovement 0
# 374 "data/maps/PalletTown/scripts.pory"
	return
# 375 "data/maps/PalletTown/scripts.pory"

# 376 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_SignLadyMoveOutOfWayLeft::
# 377 "data/maps/PalletTown/scripts.pory"
	applymovement LOCALID_SIGN_LADY, PalletTown_Movement_SignLadyMoveOutOfWayLeft
# 378 "data/maps/PalletTown/scripts.pory"
	waitmovement 0
# 379 "data/maps/PalletTown/scripts.pory"
	return
# 380 "data/maps/PalletTown/scripts.pory"

# 381 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_SignLadyDone::
# 382 "data/maps/PalletTown/scripts.pory"
	applymovement LOCALID_SIGN_LADY, Common_Movement_FacePlayer
# 383 "data/maps/PalletTown/scripts.pory"
	waitmovement 0
# 384 "data/maps/PalletTown/scripts.pory"
	msgbox PalletTown_Text_RaisingMonsToo
# 385 "data/maps/PalletTown/scripts.pory"
	release
# 386 "data/maps/PalletTown/scripts.pory"
	end
# 387 "data/maps/PalletTown/scripts.pory"

# 388 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_SignLadyGoReadSign::
# 389 "data/maps/PalletTown/scripts.pory"
	applymovement LOCALID_SIGN_LADY, Common_Movement_FacePlayer
# 390 "data/maps/PalletTown/scripts.pory"
	waitmovement 0
# 391 "data/maps/PalletTown/scripts.pory"
	msgbox PalletTown_Text_ReadItReadIt
# 392 "data/maps/PalletTown/scripts.pory"
	release
# 393 "data/maps/PalletTown/scripts.pory"
	end
# 394 "data/maps/PalletTown/scripts.pory"

# 395 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_SignLadyJustShowedSign::
# 396 "data/maps/PalletTown/scripts.pory"
	applymovement LOCALID_SIGN_LADY, Common_Movement_FacePlayer
# 397 "data/maps/PalletTown/scripts.pory"
	waitmovement 0
# 398 "data/maps/PalletTown/scripts.pory"
	msgbox PalletTown_Text_SignsAreUsefulArentThey
# 399 "data/maps/PalletTown/scripts.pory"
	release
# 400 "data/maps/PalletTown/scripts.pory"
	end
# 401 "data/maps/PalletTown/scripts.pory"

# 402 "data/maps/PalletTown/scripts.pory"
PalletTown_Movement_SignLadyMoveOutOfWayRight::
# 403 "data/maps/PalletTown/scripts.pory"
	walk_right
# 404 "data/maps/PalletTown/scripts.pory"
	walk_in_place_faster_left
# 405 "data/maps/PalletTown/scripts.pory"
	step_end
# 406 "data/maps/PalletTown/scripts.pory"

# 407 "data/maps/PalletTown/scripts.pory"
PalletTown_Movement_SignLadyMoveOutOfWayLeft::
# 408 "data/maps/PalletTown/scripts.pory"
	walk_left
# 409 "data/maps/PalletTown/scripts.pory"
	walk_in_place_faster_right
# 410 "data/maps/PalletTown/scripts.pory"
	step_end
# 411 "data/maps/PalletTown/scripts.pory"

# 412 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_FatMan::
# 413 "data/maps/PalletTown/scripts.pory"
	msgbox PalletTown_Text_CanStoreItemsAndMonsInPC, MSGBOX_NPC
# 414 "data/maps/PalletTown/scripts.pory"
	end
# 415 "data/maps/PalletTown/scripts.pory"

# 416 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_OaksLabSign::
# 417 "data/maps/PalletTown/scripts.pory"
	lockall
# 418 "data/maps/PalletTown/scripts.pory"
	@famechecker FAMECHECKER_OAK, 0
# 419 "data/maps/PalletTown/scripts.pory"
	msgbox PalletTown_Text_OakPokemonResearchLab
# 420 "data/maps/PalletTown/scripts.pory"
	releaseall
# 421 "data/maps/PalletTown/scripts.pory"
	end
# 422 "data/maps/PalletTown/scripts.pory"

# 423 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_PlayersHouseSign::
# 424 "data/maps/PalletTown/scripts.pory"
	msgbox PalletTown_Text_PlayersHouse, MSGBOX_SIGN
# 425 "data/maps/PalletTown/scripts.pory"
	end
# 426 "data/maps/PalletTown/scripts.pory"

# 427 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_RivalsHouseSign::
# 428 "data/maps/PalletTown/scripts.pory"
	msgbox PalletTown_Text_RivalsHouse, MSGBOX_SIGN
# 429 "data/maps/PalletTown/scripts.pory"
	end
# 430 "data/maps/PalletTown/scripts.pory"

# 431 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_TownSign::
# 432 "data/maps/PalletTown/scripts.pory"
	msgbox PalletTown_Text_TownSign, MSGBOX_SIGN
# 433 "data/maps/PalletTown/scripts.pory"
	end
# 434 "data/maps/PalletTown/scripts.pory"

# 435 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_TrainerTips::
# 436 "data/maps/PalletTown/scripts.pory"
	lockall
# 437 "data/maps/PalletTown/scripts.pory"
	msgbox PalletTown_Text_PressStartToOpenMenu
# 438 "data/maps/PalletTown/scripts.pory"
	setvar VAR_MAP_SCENE_PALLET_TOWN_SIGN_LADY, 1
# 439 "data/maps/PalletTown/scripts.pory"
	releaseall
# 440 "data/maps/PalletTown/scripts.pory"
	end
# 441 "data/maps/PalletTown/scripts.pory"

# 442 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_SignLadyTrigger::
# 443 "data/maps/PalletTown/scripts.pory"
	lockall
# 444 "data/maps/PalletTown/scripts.pory"
	applymovement LOCALID_SIGN_LADY, Common_Movement_WalkInPlaceFasterRight
# 445 "data/maps/PalletTown/scripts.pory"
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFasterLeft
# 446 "data/maps/PalletTown/scripts.pory"
	waitmovement 0
# 447 "data/maps/PalletTown/scripts.pory"
	call PalletTown_EventScript_SignLadyShowSign
# 448 "data/maps/PalletTown/scripts.pory"
	releaseall
# 449 "data/maps/PalletTown/scripts.pory"
	end
# 450 "data/maps/PalletTown/scripts.pory"

# 451 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_SignLadyShowSign::
# 452 "data/maps/PalletTown/scripts.pory"
	textcolor NPC_TEXT_COLOR_FEMALE
# 453 "data/maps/PalletTown/scripts.pory"
	msgbox PalletTown_Text_LookCopiedTrainerTipsSign
# 454 "data/maps/PalletTown/scripts.pory"
	closemessage
# 455 "data/maps/PalletTown/scripts.pory"
	delay 20
# 456 "data/maps/PalletTown/scripts.pory"
	textcolor NPC_TEXT_COLOR_NEUTRAL
# 457 "data/maps/PalletTown/scripts.pory"
	setflag FLAG_OPENED_START_MENU
# 458 "data/maps/PalletTown/scripts.pory"
	setvar VAR_MAP_SCENE_PALLET_TOWN_SIGN_LADY, 1
# 459 "data/maps/PalletTown/scripts.pory"
	setvar SIGN_LADY_READY, FALSE
# 460 "data/maps/PalletTown/scripts.pory"
	@ TODO: Implement sign functionality
# 461 "data/maps/PalletTown/scripts.pory"
	special SetWalkingIntoSignVars
# 462 "data/maps/PalletTown/scripts.pory"
	special DisableMsgBoxWalkaway
# 463 "data/maps/PalletTown/scripts.pory"
	signmsg
# 464 "data/maps/PalletTown/scripts.pory"
	msgbox PalletTown_Text_PressStartToOpenMenuCopy
# 465 "data/maps/PalletTown/scripts.pory"
	normalmsg
# 466 "data/maps/PalletTown/scripts.pory"
	return
# 467 "data/maps/PalletTown/scripts.pory"

# 468 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_SignLadyStartShowSign::
# 469 "data/maps/PalletTown/scripts.pory"
	applymovement LOCALID_SIGN_LADY, Common_Movement_FacePlayer
# 470 "data/maps/PalletTown/scripts.pory"
	waitmovement 0
# 471 "data/maps/PalletTown/scripts.pory"
	call PalletTown_EventScript_SignLadyShowSign
# 472 "data/maps/PalletTown/scripts.pory"
	release
# 473 "data/maps/PalletTown/scripts.pory"
	end
# 474 "data/maps/PalletTown/scripts.pory"

# 475 "data/maps/PalletTown/scripts.pory"
PalletTown_Text_OakDontGoOut::
# 476 "data/maps/PalletTown/scripts.pory"
    .string "OAK: Hey! Wait!\n"
# 477 "data/maps/PalletTown/scripts.pory"
    .string "Don't go out!$"
# 478 "data/maps/PalletTown/scripts.pory"

# 479 "data/maps/PalletTown/scripts.pory"
PalletTown_Text_OakGrassUnsafeNeedMon::
# 480 "data/maps/PalletTown/scripts.pory"
    .string "OAK: It's unsafe!\n"
# 481 "data/maps/PalletTown/scripts.pory"
    .string "Wild POKéMON live in tall grass!\p"
# 482 "data/maps/PalletTown/scripts.pory"
    .string "You need your own POKéMON for\n"
# 483 "data/maps/PalletTown/scripts.pory"
    .string "your protection.\p"
# 484 "data/maps/PalletTown/scripts.pory"
    .string "I know!\n"
# 485 "data/maps/PalletTown/scripts.pory"
    .string "Here, come with me!$"
# 486 "data/maps/PalletTown/scripts.pory"

# 487 "data/maps/PalletTown/scripts.pory"
PalletTown_Text_RaisingMonsToo::
# 488 "data/maps/PalletTown/scripts.pory"
    .string "I'm raising POKéMON, too.\p"
# 489 "data/maps/PalletTown/scripts.pory"
    .string "When they get strong, they can\n"
# 490 "data/maps/PalletTown/scripts.pory"
    .string "protect me.$"
# 491 "data/maps/PalletTown/scripts.pory"

# 492 "data/maps/PalletTown/scripts.pory"
PalletTown_Text_CanStoreItemsAndMonsInPC::
# 493 "data/maps/PalletTown/scripts.pory"
    .string "Technology is incredible!\p"
# 494 "data/maps/PalletTown/scripts.pory"
    .string "You can now store and recall items\n"
# 495 "data/maps/PalletTown/scripts.pory"
    .string "and POKéMON as data via PC.$"
# 496 "data/maps/PalletTown/scripts.pory"

# 497 "data/maps/PalletTown/scripts.pory"
PalletTown_Text_OakPokemonResearchLab::
# 498 "data/maps/PalletTown/scripts.pory"
    .string "OAK POKéMON RESEARCH LAB$"
# 499 "data/maps/PalletTown/scripts.pory"

# 500 "data/maps/PalletTown/scripts.pory"
PalletTown_Text_PlayersHouse::
# 501 "data/maps/PalletTown/scripts.pory"
    .string "{PLAYER}'s house$"
# 502 "data/maps/PalletTown/scripts.pory"

# 503 "data/maps/PalletTown/scripts.pory"
PalletTown_Text_RivalsHouse::
# 504 "data/maps/PalletTown/scripts.pory"
    .string "{RIVAL}'s house$"
# 505 "data/maps/PalletTown/scripts.pory"

# 506 "data/maps/PalletTown/scripts.pory"
PalletTown_Text_TownSign::
# 507 "data/maps/PalletTown/scripts.pory"
    .string "PALLET TOWN\n"
# 508 "data/maps/PalletTown/scripts.pory"
    .string "Shades of your journey await!$"
# 509 "data/maps/PalletTown/scripts.pory"

# 510 "data/maps/PalletTown/scripts.pory"
PalletTown_Text_OakLetMeSeePokedex::
# 511 "data/maps/PalletTown/scripts.pory"
    .string "OAK: Ah, {PLAYER}!\n"
# 512 "data/maps/PalletTown/scripts.pory"
    .string "You're back, are you?\p"
# 513 "data/maps/PalletTown/scripts.pory"
    .string "How much have you filled in your\n"
# 514 "data/maps/PalletTown/scripts.pory"
    .string "POKéDEX?\p"
# 515 "data/maps/PalletTown/scripts.pory"
    .string "May I see it?\p"
# 516 "data/maps/PalletTown/scripts.pory"
    .string "Let's see…$"
# 517 "data/maps/PalletTown/scripts.pory"

# 518 "data/maps/PalletTown/scripts.pory"
PalletTown_Text_CaughtXPuttingInHonestEffort::
# 519 "data/maps/PalletTown/scripts.pory"
    .string "You've caught {STR_VAR_2}…\p"
# 520 "data/maps/PalletTown/scripts.pory"
    .string "Hm, it looks as if you're putting\n"
# 521 "data/maps/PalletTown/scripts.pory"
    .string "in an honest effort.\p"
# 522 "data/maps/PalletTown/scripts.pory"
    .string "When you manage to fill it some\n"
# 523 "data/maps/PalletTown/scripts.pory"
    .string "more, come show me, please.$"
# 524 "data/maps/PalletTown/scripts.pory"

# 525 "data/maps/PalletTown/scripts.pory"
PalletTown_Text_CaughtXImpressiveFollowMe::
# 526 "data/maps/PalletTown/scripts.pory"
    .string "You've caught… {STR_VAR_2}!?\n"
# 527 "data/maps/PalletTown/scripts.pory"
    .string "Now, this is impressive!\p"
# 528 "data/maps/PalletTown/scripts.pory"
    .string "There's something I wanted to ask\n"
# 529 "data/maps/PalletTown/scripts.pory"
    .string "of you, {PLAYER}.\p"
# 530 "data/maps/PalletTown/scripts.pory"
    .string "Come.\n"
# 531 "data/maps/PalletTown/scripts.pory"
    .string "Follow me.$"
# 532 "data/maps/PalletTown/scripts.pory"

# 533 "data/maps/PalletTown/scripts.pory"
PalletTown_Text_OakYouEnjoyingTraveling::
# 534 "data/maps/PalletTown/scripts.pory"
    .string "OAK: Ah, {PLAYER}!\n"
# 535 "data/maps/PalletTown/scripts.pory"
    .string "You seem to be enjoying traveling.\p"
# 536 "data/maps/PalletTown/scripts.pory"
    .string "Knowing you, {PLAYER}, I can easily\n"
# 537 "data/maps/PalletTown/scripts.pory"
    .string "imagine you going out to even more\l"
# 538 "data/maps/PalletTown/scripts.pory"
    .string "exotic locales.\p"
# 539 "data/maps/PalletTown/scripts.pory"
    .string "Good for you, good for you.\n"
# 540 "data/maps/PalletTown/scripts.pory"
    .string "Hohoho.$"
# 541 "data/maps/PalletTown/scripts.pory"

# 542 "data/maps/PalletTown/scripts.pory"
PalletTown_Text_HmmIsThatRight::
# 543 "data/maps/PalletTown/scripts.pory"
	.string "Hmm…\n"
# 544 "data/maps/PalletTown/scripts.pory"
	.string "Is that right…$"
# 545 "data/maps/PalletTown/scripts.pory"

# 546 "data/maps/PalletTown/scripts.pory"
PalletTown_Text_OhLookLook::
# 547 "data/maps/PalletTown/scripts.pory"
	.string "Oh!\n"
# 548 "data/maps/PalletTown/scripts.pory"
	.string "Look, look!$"
# 549 "data/maps/PalletTown/scripts.pory"

# 550 "data/maps/PalletTown/scripts.pory"
PalletTown_Text_ReadItReadIt::
# 551 "data/maps/PalletTown/scripts.pory"
	.string "Read it, read it!$"
# 552 "data/maps/PalletTown/scripts.pory"

# 553 "data/maps/PalletTown/scripts.pory"
@ Used by sign
# 554 "data/maps/PalletTown/scripts.pory"
PalletTown_Text_PressStartToOpenMenu::
# 555 "data/maps/PalletTown/scripts.pory"
	.string "TRAINER TIPS\p"
# 556 "data/maps/PalletTown/scripts.pory"
	.string "Press START to open the MENU!$"
# 557 "data/maps/PalletTown/scripts.pory"

# 558 "data/maps/PalletTown/scripts.pory"
PalletTown_Text_SignsAreUsefulArentThey::
# 559 "data/maps/PalletTown/scripts.pory"
	.string "Signs are useful, aren't they?$"
# 560 "data/maps/PalletTown/scripts.pory"

# 561 "data/maps/PalletTown/scripts.pory"
PalletTown_Text_LookCopiedTrainerTipsSign::
# 562 "data/maps/PalletTown/scripts.pory"
	.string "Look, look!\p"
# 563 "data/maps/PalletTown/scripts.pory"
	.string "I copied what it said on one of\n"
# 564 "data/maps/PalletTown/scripts.pory"
	.string "those TRAINER TIPS signs!$"
# 565 "data/maps/PalletTown/scripts.pory"

# 566 "data/maps/PalletTown/scripts.pory"
@ Used by NPC
# 567 "data/maps/PalletTown/scripts.pory"
PalletTown_Text_PressStartToOpenMenuCopy::
# 568 "data/maps/PalletTown/scripts.pory"
	.string "TRAINER TIPS!\p"
# 569 "data/maps/PalletTown/scripts.pory"
	.string "Press START to open the MENU!$"
