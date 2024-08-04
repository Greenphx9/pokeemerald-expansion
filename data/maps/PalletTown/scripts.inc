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


PalletTown_EventScript_SignLady::
# 27 "data/maps/PalletTown/scripts.pory"
	lock
# 28 "data/maps/PalletTown/scripts.pory"
	faceplayer
# 29 "data/maps/PalletTown/scripts.pory"
	msgbox PalletTown_EventScript_SignLady_Text_0
# 30 "data/maps/PalletTown/scripts.pory"
	release
	end


PalletTown_EventScript_LeafsHouseSign::
# 36 "data/maps/PalletTown/scripts.pory"
	msgbox PalletTown_EventScript_LeafsHouseSign_Text_0, MSGBOX_SIGN
	end


# 40 "data/maps/PalletTown/scripts.pory"

# 41 "data/maps/PalletTown/scripts.pory"
.equ LOCALID_SIGN_LADY, 1
# 42 "data/maps/PalletTown/scripts.pory"
.equ LOCALID_PROF_OAK,  3
# 43 "data/maps/PalletTown/scripts.pory"

# 44 "data/maps/PalletTown/scripts.pory"
.equ SIGN_LADY_READY, VAR_TEMP_2
# 45 "data/maps/PalletTown/scripts.pory"

# 46 "data/maps/PalletTown/scripts.pory"
PalletTown_OnTransition::
# 47 "data/maps/PalletTown/scripts.pory"
    setworldmapflag FLAG_WORLD_MAP_PALLET_TOWN
# 48 "data/maps/PalletTown/scripts.pory"
    call_if_set FLAG_PALLET_LADY_NOT_BLOCKING_SIGN, PalletTown_EventScript_TryReadySignLady
# 49 "data/maps/PalletTown/scripts.pory"
    call_if_eq VAR_MAP_SCENE_PALLET_TOWN_SIGN_LADY, 0, PalletTown_EventScript_SetSignLadyPos
# 50 "data/maps/PalletTown/scripts.pory"
    call_if_eq VAR_MAP_SCENE_PALLET_TOWN_SIGN_LADY, 1, PalletTown_EventScript_SetSignLadyDone
# 51 "data/maps/PalletTown/scripts.pory"
    end
# 52 "data/maps/PalletTown/scripts.pory"

# 53 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_TryReadySignLady::
# 54 "data/maps/PalletTown/scripts.pory"
    goto_if_unset FLAG_OPENED_START_MENU, Common_EventScript_NopReturn
# 55 "data/maps/PalletTown/scripts.pory"
    goto_if_ge VAR_MAP_SCENE_PALLET_TOWN_SIGN_LADY, 1, Common_EventScript_NopReturn
# 56 "data/maps/PalletTown/scripts.pory"
    setvar VAR_MAP_SCENE_PALLET_TOWN_SIGN_LADY, 1
# 57 "data/maps/PalletTown/scripts.pory"
    return
# 58 "data/maps/PalletTown/scripts.pory"

# 59 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_SetSignLadyDone::
# 60 "data/maps/PalletTown/scripts.pory"
    setvar VAR_MAP_SCENE_PALLET_TOWN_SIGN_LADY, 2
# 61 "data/maps/PalletTown/scripts.pory"
    return
# 62 "data/maps/PalletTown/scripts.pory"

# 63 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_SetSignLadyPos::
# 64 "data/maps/PalletTown/scripts.pory"
    goto_if_set FLAG_PALLET_LADY_NOT_BLOCKING_SIGN, PalletTown_EventScript_MoveSignLadyToRouteEntrance
# 65 "data/maps/PalletTown/scripts.pory"
    setobjectxyperm LOCALID_SIGN_LADY, 5, 15
# 66 "data/maps/PalletTown/scripts.pory"
    setobjectmovementtype LOCALID_SIGN_LADY, MOVEMENT_TYPE_FACE_UP
# 67 "data/maps/PalletTown/scripts.pory"
    return
# 68 "data/maps/PalletTown/scripts.pory"

# 69 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_MoveSignLadyToRouteEntrance::
# 70 "data/maps/PalletTown/scripts.pory"
    setobjectxyperm LOCALID_SIGN_LADY, 12, 2
# 71 "data/maps/PalletTown/scripts.pory"
    setobjectmovementtype LOCALID_SIGN_LADY, MOVEMENT_TYPE_FACE_DOWN
# 72 "data/maps/PalletTown/scripts.pory"
    setvar SIGN_LADY_READY, TRUE
# 73 "data/maps/PalletTown/scripts.pory"
    return
# 74 "data/maps/PalletTown/scripts.pory"

# 75 "data/maps/PalletTown/scripts.pory"
PalletTown_OnFrame::
# 76 "data/maps/PalletTown/scripts.pory"
    map_script_2 VAR_MAP_SCENE_PALLET_TOWN_OAK, 2, PalletTown_EventScript_OakRatingScene
# 77 "data/maps/PalletTown/scripts.pory"
    .2byte 0
# 78 "data/maps/PalletTown/scripts.pory"

# 79 "data/maps/PalletTown/scripts.pory"
@ Oak approaches player after exiting their house post Elite Four to check if they should receieve the National Dex
# 80 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_OakRatingScene::
# 81 "data/maps/PalletTown/scripts.pory"
    lockall
# 82 "data/maps/PalletTown/scripts.pory"
    textcolor NPC_TEXT_COLOR_MALE
# 83 "data/maps/PalletTown/scripts.pory"
    setobjectxyperm LOCALID_PROF_OAK, 14, 14
# 84 "data/maps/PalletTown/scripts.pory"
    setobjectmovementtype LOCALID_PROF_OAK, MOVEMENT_TYPE_FACE_LEFT
# 85 "data/maps/PalletTown/scripts.pory"
    addobject LOCALID_PROF_OAK
# 86 "data/maps/PalletTown/scripts.pory"
    applymovement LOCALID_PROF_OAK, PalletTown_Movement_OakWalkToPlayersDoor
# 87 "data/maps/PalletTown/scripts.pory"
    waitmovement 0
# 88 "data/maps/PalletTown/scripts.pory"
    goto_if_unset FLAG_WORLD_MAP_ONE_ISLAND, PalletTown_EventScript_NotBeenToOneIslandYet
# 89 "data/maps/PalletTown/scripts.pory"
    msgbox PalletTown_Text_OakLetMeSeePokedex
# 90 "data/maps/PalletTown/scripts.pory"
    setvar VAR_0x8004, 0
# 91 "data/maps/PalletTown/scripts.pory"
    specialvar VAR_RESULT, ScriptGetPokedexInfo
# 92 "data/maps/PalletTown/scripts.pory"
    copyvar VAR_0x8008, VAR_0x8005
# 93 "data/maps/PalletTown/scripts.pory"
    copyvar VAR_0x8009, VAR_0x8006
# 94 "data/maps/PalletTown/scripts.pory"
    buffernumberstring STR_VAR_1, VAR_0x8008
# 95 "data/maps/PalletTown/scripts.pory"
    buffernumberstring STR_VAR_2, VAR_0x8009
# 96 "data/maps/PalletTown/scripts.pory"
    goto_if_lt VAR_0x8009, 60, PalletTown_EventScript_NotEnoughMonsForNationalDex
# 97 "data/maps/PalletTown/scripts.pory"
    msgbox PalletTown_Text_CaughtXImpressiveFollowMe
# 98 "data/maps/PalletTown/scripts.pory"
    closemessage
# 99 "data/maps/PalletTown/scripts.pory"
    playbgm MUS_FOLLOW_ME, 0
# 100 "data/maps/PalletTown/scripts.pory"
    applymovement OBJ_EVENT_ID_PLAYER, PalletTown_Movement_PlayerWalkToLabFromHouse
# 101 "data/maps/PalletTown/scripts.pory"
    applymovement LOCALID_PROF_OAK, PalletTown_Movement_OakWalkToLabFromHouse
# 102 "data/maps/PalletTown/scripts.pory"
    waitmovement 0
# 103 "data/maps/PalletTown/scripts.pory"
    opendoor 16, 13
# 104 "data/maps/PalletTown/scripts.pory"
    waitdooranim
# 105 "data/maps/PalletTown/scripts.pory"
    applymovement LOCALID_PROF_OAK, PalletTown_Movement_OakEnterLab
# 106 "data/maps/PalletTown/scripts.pory"
    applymovement OBJ_EVENT_ID_PLAYER, PalletTown_Movement_PlayerEnterLab
# 107 "data/maps/PalletTown/scripts.pory"
    waitmovement 0
# 108 "data/maps/PalletTown/scripts.pory"
    closedoor 16, 13
# 109 "data/maps/PalletTown/scripts.pory"
    waitdooranim
# 110 "data/maps/PalletTown/scripts.pory"
    setvar VAR_MAP_SCENE_PALLET_TOWN_PROFESSOR_OAKS_LAB, 7
# 111 "data/maps/PalletTown/scripts.pory"
    setvar VAR_MAP_SCENE_PALLET_TOWN_OAK, 3
# 112 "data/maps/PalletTown/scripts.pory"
    setflag FLAG_HIDE_OAK_IN_PALLET_TOWN
# 113 "data/maps/PalletTown/scripts.pory"
    warp MAP_PALLET_TOWN_PROFESSOR_OAKS_LAB, 6, 12
# 114 "data/maps/PalletTown/scripts.pory"
    waitstate
# 115 "data/maps/PalletTown/scripts.pory"
    releaseall
# 116 "data/maps/PalletTown/scripts.pory"
    end
# 117 "data/maps/PalletTown/scripts.pory"

# 118 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_EndOakRatingScene::
# 119 "data/maps/PalletTown/scripts.pory"
    closemessage
# 120 "data/maps/PalletTown/scripts.pory"
    applymovement LOCALID_PROF_OAK, PalletTown_Movement_OakExit
# 121 "data/maps/PalletTown/scripts.pory"
    waitmovement 0
# 122 "data/maps/PalletTown/scripts.pory"
    removeobject LOCALID_PROF_OAK
# 123 "data/maps/PalletTown/scripts.pory"
    setvar VAR_MAP_SCENE_PALLET_TOWN_OAK, 3
# 124 "data/maps/PalletTown/scripts.pory"
    releaseall
# 125 "data/maps/PalletTown/scripts.pory"
    end
# 126 "data/maps/PalletTown/scripts.pory"

# 127 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_NotEnoughMonsForNationalDex::
# 128 "data/maps/PalletTown/scripts.pory"
    msgbox PalletTown_Text_CaughtXPuttingInHonestEffort
# 129 "data/maps/PalletTown/scripts.pory"
    goto PalletTown_EventScript_EndOakRatingScene
# 130 "data/maps/PalletTown/scripts.pory"
    end
# 131 "data/maps/PalletTown/scripts.pory"

# 132 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_NotBeenToOneIslandYet::
# 133 "data/maps/PalletTown/scripts.pory"
    msgbox PalletTown_Text_OakYouEnjoyingTraveling
# 134 "data/maps/PalletTown/scripts.pory"
    goto PalletTown_EventScript_EndOakRatingScene
# 135 "data/maps/PalletTown/scripts.pory"
    end
# 136 "data/maps/PalletTown/scripts.pory"

# 137 "data/maps/PalletTown/scripts.pory"
PalletTown_Movement_OakWalkToPlayersDoor::
# 138 "data/maps/PalletTown/scripts.pory"
    walk_left
# 139 "data/maps/PalletTown/scripts.pory"
    walk_left
# 140 "data/maps/PalletTown/scripts.pory"
    walk_left
# 141 "data/maps/PalletTown/scripts.pory"
    walk_up
# 142 "data/maps/PalletTown/scripts.pory"
    walk_up
# 143 "data/maps/PalletTown/scripts.pory"
    walk_up
# 144 "data/maps/PalletTown/scripts.pory"
    walk_up
# 145 "data/maps/PalletTown/scripts.pory"
    walk_up
# 146 "data/maps/PalletTown/scripts.pory"
    walk_left
# 147 "data/maps/PalletTown/scripts.pory"
    walk_left
# 148 "data/maps/PalletTown/scripts.pory"
    walk_left
# 149 "data/maps/PalletTown/scripts.pory"
    walk_left
# 150 "data/maps/PalletTown/scripts.pory"
    walk_left
# 151 "data/maps/PalletTown/scripts.pory"
    walk_in_place_faster_up
# 152 "data/maps/PalletTown/scripts.pory"
    step_end
# 153 "data/maps/PalletTown/scripts.pory"

# 154 "data/maps/PalletTown/scripts.pory"
PalletTown_Movement_OakExit::
# 155 "data/maps/PalletTown/scripts.pory"
    walk_right
# 156 "data/maps/PalletTown/scripts.pory"
    walk_right
# 157 "data/maps/PalletTown/scripts.pory"
    walk_right
# 158 "data/maps/PalletTown/scripts.pory"
    walk_right
# 159 "data/maps/PalletTown/scripts.pory"
    walk_right
# 160 "data/maps/PalletTown/scripts.pory"
    walk_down
# 161 "data/maps/PalletTown/scripts.pory"
    walk_down
# 162 "data/maps/PalletTown/scripts.pory"
    walk_down
# 163 "data/maps/PalletTown/scripts.pory"
    walk_down
# 164 "data/maps/PalletTown/scripts.pory"
    walk_down
# 165 "data/maps/PalletTown/scripts.pory"
    walk_right
# 166 "data/maps/PalletTown/scripts.pory"
    walk_right
# 167 "data/maps/PalletTown/scripts.pory"
    walk_right
# 168 "data/maps/PalletTown/scripts.pory"
    step_end
# 169 "data/maps/PalletTown/scripts.pory"

# 170 "data/maps/PalletTown/scripts.pory"
PalletTown_Movement_OakWalkToLabFromHouse::
# 171 "data/maps/PalletTown/scripts.pory"
    walk_right
# 172 "data/maps/PalletTown/scripts.pory"
    walk_right
# 173 "data/maps/PalletTown/scripts.pory"
    walk_right
# 174 "data/maps/PalletTown/scripts.pory"
    walk_right
# 175 "data/maps/PalletTown/scripts.pory"
    walk_right
# 176 "data/maps/PalletTown/scripts.pory"
    walk_down
# 177 "data/maps/PalletTown/scripts.pory"
    walk_down
# 178 "data/maps/PalletTown/scripts.pory"
    walk_down
# 179 "data/maps/PalletTown/scripts.pory"
    walk_down
# 180 "data/maps/PalletTown/scripts.pory"
    walk_down
# 181 "data/maps/PalletTown/scripts.pory"
    walk_right
# 182 "data/maps/PalletTown/scripts.pory"
    walk_right
# 183 "data/maps/PalletTown/scripts.pory"
    walk_right
# 184 "data/maps/PalletTown/scripts.pory"
    walk_right
# 185 "data/maps/PalletTown/scripts.pory"
    walk_right
# 186 "data/maps/PalletTown/scripts.pory"
    walk_in_place_faster_up
# 187 "data/maps/PalletTown/scripts.pory"
    step_end
# 188 "data/maps/PalletTown/scripts.pory"

# 189 "data/maps/PalletTown/scripts.pory"
PalletTown_Movement_PlayerWalkToLabFromHouse::
# 190 "data/maps/PalletTown/scripts.pory"
    walk_down
# 191 "data/maps/PalletTown/scripts.pory"
    walk_right
# 192 "data/maps/PalletTown/scripts.pory"
    walk_right
# 193 "data/maps/PalletTown/scripts.pory"
    walk_right
# 194 "data/maps/PalletTown/scripts.pory"
    walk_right
# 195 "data/maps/PalletTown/scripts.pory"
    walk_right
# 196 "data/maps/PalletTown/scripts.pory"
    walk_down
# 197 "data/maps/PalletTown/scripts.pory"
    walk_down
# 198 "data/maps/PalletTown/scripts.pory"
    walk_down
# 199 "data/maps/PalletTown/scripts.pory"
    walk_down
# 200 "data/maps/PalletTown/scripts.pory"
    walk_down
# 201 "data/maps/PalletTown/scripts.pory"
    walk_right
# 202 "data/maps/PalletTown/scripts.pory"
    walk_right
# 203 "data/maps/PalletTown/scripts.pory"
    walk_right
# 204 "data/maps/PalletTown/scripts.pory"
    walk_right
# 205 "data/maps/PalletTown/scripts.pory"
    step_end
# 206 "data/maps/PalletTown/scripts.pory"

# 207 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_OakTriggerLeft::
# 208 "data/maps/PalletTown/scripts.pory"
    lockall
# 209 "data/maps/PalletTown/scripts.pory"
    setvar VAR_TEMP_1, 0
# 210 "data/maps/PalletTown/scripts.pory"
    goto PalletTown_EventScript_OakTrigger
# 211 "data/maps/PalletTown/scripts.pory"
    end
# 212 "data/maps/PalletTown/scripts.pory"

# 213 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_OakTriggerRight::
# 214 "data/maps/PalletTown/scripts.pory"
    lockall
# 215 "data/maps/PalletTown/scripts.pory"
    setvar VAR_TEMP_1, 1
# 216 "data/maps/PalletTown/scripts.pory"
    goto PalletTown_EventScript_OakTrigger
# 217 "data/maps/PalletTown/scripts.pory"
    end
# 218 "data/maps/PalletTown/scripts.pory"

# 219 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_OakTrigger::
# 220 "data/maps/PalletTown/scripts.pory"
    @famechecker FAMECHECKER_OAK, FCPICKSTATE_COLORED, UpdatePickStateFromSpecialVar8005
# 221 "data/maps/PalletTown/scripts.pory"
    textcolor NPC_TEXT_COLOR_MALE
# 222 "data/maps/PalletTown/scripts.pory"
    delay 30
# 223 "data/maps/PalletTown/scripts.pory"
    playbgm MUS_RG_OAK, 0
# 224 "data/maps/PalletTown/scripts.pory"
    message PalletTown_Text_OakDontGoOut
# 225 "data/maps/PalletTown/scripts.pory"
    waitmessage
# 226 "data/maps/PalletTown/scripts.pory"
    delay 85
# 227 "data/maps/PalletTown/scripts.pory"
    closemessage
# 228 "data/maps/PalletTown/scripts.pory"
    applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFasterDown
# 229 "data/maps/PalletTown/scripts.pory"
    waitmovement 0
# 230 "data/maps/PalletTown/scripts.pory"
    playse SE_PIN
# 231 "data/maps/PalletTown/scripts.pory"
    applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_ExclamationMark
# 232 "data/maps/PalletTown/scripts.pory"
    waitmovement 0
# 233 "data/maps/PalletTown/scripts.pory"
    delay 30
# 234 "data/maps/PalletTown/scripts.pory"
    addobject LOCALID_PROF_OAK
# 235 "data/maps/PalletTown/scripts.pory"
    call_if_eq VAR_TEMP_1, 0, PalletTown_EventScript_OakEnterLeft
# 236 "data/maps/PalletTown/scripts.pory"
    call_if_eq VAR_TEMP_1, 1, PalletTown_EventScript_OakEnterRight
# 237 "data/maps/PalletTown/scripts.pory"
    delay 30
# 238 "data/maps/PalletTown/scripts.pory"
    msgbox PalletTown_Text_OakGrassUnsafeNeedMon
# 239 "data/maps/PalletTown/scripts.pory"
    closemessage
# 240 "data/maps/PalletTown/scripts.pory"
    delay 30
# 241 "data/maps/PalletTown/scripts.pory"
    call_if_eq VAR_TEMP_1, 0, PalletTown_EventScript_OakLeadPlayerToLabLeft
# 242 "data/maps/PalletTown/scripts.pory"
    call_if_eq VAR_TEMP_1, 1, PalletTown_EventScript_OakLeadPlayerToLabRight
# 243 "data/maps/PalletTown/scripts.pory"
    opendoor 16, 13
# 244 "data/maps/PalletTown/scripts.pory"
    waitdooranim
# 245 "data/maps/PalletTown/scripts.pory"
    applymovement LOCALID_PROF_OAK, PalletTown_Movement_OakEnterLab
# 246 "data/maps/PalletTown/scripts.pory"
    applymovement OBJ_EVENT_ID_PLAYER, PalletTown_Movement_PlayerEnterLab
# 247 "data/maps/PalletTown/scripts.pory"
    waitmovement 0
# 248 "data/maps/PalletTown/scripts.pory"
    closedoor 16, 13
# 249 "data/maps/PalletTown/scripts.pory"
    waitdooranim
# 250 "data/maps/PalletTown/scripts.pory"
    setvar VAR_MAP_SCENE_PALLET_TOWN_PROFESSOR_OAKS_LAB, 1
# 251 "data/maps/PalletTown/scripts.pory"
    clearflag FLAG_HIDE_OAK_IN_HIS_LAB
# 252 "data/maps/PalletTown/scripts.pory"
    setvar VAR_MAP_SCENE_PALLET_TOWN_OAK, 1
# 253 "data/maps/PalletTown/scripts.pory"
    setflag FLAG_HIDE_OAK_IN_PALLET_TOWN
# 254 "data/maps/PalletTown/scripts.pory"
    setflag FLAG_DONT_TRANSITION_MUSIC
# 255 "data/maps/PalletTown/scripts.pory"
    warp MAP_PALLET_TOWN_PROFESSOR_OAKS_LAB, 6, 12
# 256 "data/maps/PalletTown/scripts.pory"
    waitstate
# 257 "data/maps/PalletTown/scripts.pory"
    releaseall
# 258 "data/maps/PalletTown/scripts.pory"
    end
# 259 "data/maps/PalletTown/scripts.pory"

# 260 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_OakEnterLeft::
# 261 "data/maps/PalletTown/scripts.pory"
    applymovement LOCALID_PROF_OAK, PalletTown_Movement_OakEnterLeft
# 262 "data/maps/PalletTown/scripts.pory"
    waitmovement 0
# 263 "data/maps/PalletTown/scripts.pory"
    return
# 264 "data/maps/PalletTown/scripts.pory"

# 265 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_OakEnterRight::
# 266 "data/maps/PalletTown/scripts.pory"
    applymovement LOCALID_PROF_OAK, PalletTown_Movement_OakEnterRight
# 267 "data/maps/PalletTown/scripts.pory"
    waitmovement 0
# 268 "data/maps/PalletTown/scripts.pory"
    return
# 269 "data/maps/PalletTown/scripts.pory"

# 270 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_OakLeadPlayerToLabLeft::
# 271 "data/maps/PalletTown/scripts.pory"
    applymovement LOCALID_PROF_OAK, PalletTown_Movement_OakWalkToLabLeft
# 272 "data/maps/PalletTown/scripts.pory"
    applymovement OBJ_EVENT_ID_PLAYER, PalletTown_Movement_PlayerWalkToLabLeft
# 273 "data/maps/PalletTown/scripts.pory"
    waitmovement 0
# 274 "data/maps/PalletTown/scripts.pory"
    return
# 275 "data/maps/PalletTown/scripts.pory"

# 276 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_OakLeadPlayerToLabRight::
# 277 "data/maps/PalletTown/scripts.pory"
    applymovement LOCALID_PROF_OAK, PalletTown_Movement_OakWalkToLabRight
# 278 "data/maps/PalletTown/scripts.pory"
    applymovement OBJ_EVENT_ID_PLAYER, PalletTown_Movement_PlayerWalkToLabRight
# 279 "data/maps/PalletTown/scripts.pory"
    waitmovement 0
# 280 "data/maps/PalletTown/scripts.pory"
    return
# 281 "data/maps/PalletTown/scripts.pory"

# 282 "data/maps/PalletTown/scripts.pory"
PalletTown_Movement_OakEnterLeft::
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
    walk_right
# 289 "data/maps/PalletTown/scripts.pory"
    walk_up
# 290 "data/maps/PalletTown/scripts.pory"
    walk_up
# 291 "data/maps/PalletTown/scripts.pory"
    step_end
# 292 "data/maps/PalletTown/scripts.pory"

# 293 "data/maps/PalletTown/scripts.pory"
PalletTown_Movement_OakEnterRight::
# 294 "data/maps/PalletTown/scripts.pory"
    walk_right
# 295 "data/maps/PalletTown/scripts.pory"
    walk_up
# 296 "data/maps/PalletTown/scripts.pory"
    walk_up
# 297 "data/maps/PalletTown/scripts.pory"
    walk_right
# 298 "data/maps/PalletTown/scripts.pory"
    walk_up
# 299 "data/maps/PalletTown/scripts.pory"
    walk_up
# 300 "data/maps/PalletTown/scripts.pory"
    walk_right
# 301 "data/maps/PalletTown/scripts.pory"
    walk_up
# 302 "data/maps/PalletTown/scripts.pory"
    walk_up
# 303 "data/maps/PalletTown/scripts.pory"
    step_end
# 304 "data/maps/PalletTown/scripts.pory"

# 305 "data/maps/PalletTown/scripts.pory"
    .macro walk_to_lab
# 306 "data/maps/PalletTown/scripts.pory"
    walk_left
# 307 "data/maps/PalletTown/scripts.pory"
    walk_down
# 308 "data/maps/PalletTown/scripts.pory"
    walk_down
# 309 "data/maps/PalletTown/scripts.pory"
    walk_down
# 310 "data/maps/PalletTown/scripts.pory"
    walk_down
# 311 "data/maps/PalletTown/scripts.pory"
    walk_down
# 312 "data/maps/PalletTown/scripts.pory"
    walk_down
# 313 "data/maps/PalletTown/scripts.pory"
    walk_down
# 314 "data/maps/PalletTown/scripts.pory"
    walk_down
# 315 "data/maps/PalletTown/scripts.pory"
    walk_down
# 316 "data/maps/PalletTown/scripts.pory"
    walk_down
# 317 "data/maps/PalletTown/scripts.pory"
    walk_down
# 318 "data/maps/PalletTown/scripts.pory"
    walk_right
# 319 "data/maps/PalletTown/scripts.pory"
    walk_right
# 320 "data/maps/PalletTown/scripts.pory"
    walk_right
# 321 "data/maps/PalletTown/scripts.pory"
    walk_right
# 322 "data/maps/PalletTown/scripts.pory"
    .endm
# 323 "data/maps/PalletTown/scripts.pory"

# 324 "data/maps/PalletTown/scripts.pory"
PalletTown_Movement_OakWalkToLabLeft::
# 325 "data/maps/PalletTown/scripts.pory"
    walk_down
# 326 "data/maps/PalletTown/scripts.pory"
    walk_to_lab
# 327 "data/maps/PalletTown/scripts.pory"
    walk_right
# 328 "data/maps/PalletTown/scripts.pory"
    walk_in_place_faster_up
# 329 "data/maps/PalletTown/scripts.pory"
    step_end
# 330 "data/maps/PalletTown/scripts.pory"

# 331 "data/maps/PalletTown/scripts.pory"
PalletTown_Movement_OakWalkToLabRight::
# 332 "data/maps/PalletTown/scripts.pory"
    walk_down
# 333 "data/maps/PalletTown/scripts.pory"
    walk_left
# 334 "data/maps/PalletTown/scripts.pory"
    walk_to_lab
# 335 "data/maps/PalletTown/scripts.pory"
    walk_right
# 336 "data/maps/PalletTown/scripts.pory"
    walk_in_place_faster_up
# 337 "data/maps/PalletTown/scripts.pory"
    step_end
# 338 "data/maps/PalletTown/scripts.pory"

# 339 "data/maps/PalletTown/scripts.pory"
PalletTown_Movement_OakEnterLab::
# 340 "data/maps/PalletTown/scripts.pory"
    walk_up
# 341 "data/maps/PalletTown/scripts.pory"
    set_invisible
# 342 "data/maps/PalletTown/scripts.pory"
    step_end
# 343 "data/maps/PalletTown/scripts.pory"

# 344 "data/maps/PalletTown/scripts.pory"
PalletTown_Movement_PlayerWalkToLabLeft::
# 345 "data/maps/PalletTown/scripts.pory"
    walk_down
# 346 "data/maps/PalletTown/scripts.pory"
    walk_down
# 347 "data/maps/PalletTown/scripts.pory"
    walk_to_lab
# 348 "data/maps/PalletTown/scripts.pory"
    step_end
# 349 "data/maps/PalletTown/scripts.pory"

# 350 "data/maps/PalletTown/scripts.pory"
PalletTown_Movement_PlayerWalkToLabRight::
# 351 "data/maps/PalletTown/scripts.pory"
    walk_down
# 352 "data/maps/PalletTown/scripts.pory"
    walk_down
# 353 "data/maps/PalletTown/scripts.pory"
    walk_left
# 354 "data/maps/PalletTown/scripts.pory"
    walk_to_lab
# 355 "data/maps/PalletTown/scripts.pory"
    step_end
# 356 "data/maps/PalletTown/scripts.pory"

# 357 "data/maps/PalletTown/scripts.pory"
PalletTown_Movement_PlayerEnterLab::
# 358 "data/maps/PalletTown/scripts.pory"
    walk_right
# 359 "data/maps/PalletTown/scripts.pory"
    walk_up
# 360 "data/maps/PalletTown/scripts.pory"
    set_invisible
# 361 "data/maps/PalletTown/scripts.pory"
    step_end
# 362 "data/maps/PalletTown/scripts.pory"

# 363 "data/maps/PalletTown/scripts.pory"
PalletTown_Movement_SignLadyMoveOutOfWayRight::
# 364 "data/maps/PalletTown/scripts.pory"
    walk_right
# 365 "data/maps/PalletTown/scripts.pory"
    walk_in_place_faster_left
# 366 "data/maps/PalletTown/scripts.pory"
    step_end
# 367 "data/maps/PalletTown/scripts.pory"

# 368 "data/maps/PalletTown/scripts.pory"
PalletTown_Movement_SignLadyMoveOutOfWayLeft::
# 369 "data/maps/PalletTown/scripts.pory"
    walk_left
# 370 "data/maps/PalletTown/scripts.pory"
    walk_in_place_faster_right
# 371 "data/maps/PalletTown/scripts.pory"
    step_end
# 372 "data/maps/PalletTown/scripts.pory"

# 373 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_FatMan::
# 374 "data/maps/PalletTown/scripts.pory"
    msgbox PalletTown_Text_CanStoreItemsAndMonsInPC, MSGBOX_NPC
# 375 "data/maps/PalletTown/scripts.pory"
    end
# 376 "data/maps/PalletTown/scripts.pory"

# 377 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_OaksLabSign::
# 378 "data/maps/PalletTown/scripts.pory"
    lockall
# 379 "data/maps/PalletTown/scripts.pory"
    @famechecker FAMECHECKER_OAK, 0
# 380 "data/maps/PalletTown/scripts.pory"
    msgbox PalletTown_Text_OakPokemonResearchLab
# 381 "data/maps/PalletTown/scripts.pory"
    releaseall
# 382 "data/maps/PalletTown/scripts.pory"
    end
# 383 "data/maps/PalletTown/scripts.pory"

# 384 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_PlayersHouseSign::
# 385 "data/maps/PalletTown/scripts.pory"
    msgbox PalletTown_Text_PlayersHouse, MSGBOX_SIGN
# 386 "data/maps/PalletTown/scripts.pory"
    end
# 387 "data/maps/PalletTown/scripts.pory"

# 388 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_RivalsHouseSign::
# 389 "data/maps/PalletTown/scripts.pory"
    msgbox PalletTown_Text_RivalsHouse, MSGBOX_SIGN
# 390 "data/maps/PalletTown/scripts.pory"
    end
# 391 "data/maps/PalletTown/scripts.pory"

# 392 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_TownSign::
# 393 "data/maps/PalletTown/scripts.pory"
    msgbox PalletTown_Text_TownSign, MSGBOX_SIGN
# 394 "data/maps/PalletTown/scripts.pory"
    end
# 395 "data/maps/PalletTown/scripts.pory"

# 396 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_TrainerTips::
# 397 "data/maps/PalletTown/scripts.pory"
    lockall
# 398 "data/maps/PalletTown/scripts.pory"
    msgbox PalletTown_Text_PressStartToOpenMenu
# 399 "data/maps/PalletTown/scripts.pory"
    setvar VAR_MAP_SCENE_PALLET_TOWN_SIGN_LADY, 1
# 400 "data/maps/PalletTown/scripts.pory"
    releaseall
# 401 "data/maps/PalletTown/scripts.pory"
    end
# 402 "data/maps/PalletTown/scripts.pory"

# 403 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_SignLadyTrigger::
# 404 "data/maps/PalletTown/scripts.pory"
    lockall
# 405 "data/maps/PalletTown/scripts.pory"
    applymovement LOCALID_SIGN_LADY, Common_Movement_WalkInPlaceFasterRight
# 406 "data/maps/PalletTown/scripts.pory"
    applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFasterLeft
# 407 "data/maps/PalletTown/scripts.pory"
    waitmovement 0
# 408 "data/maps/PalletTown/scripts.pory"
    call PalletTown_EventScript_SignLadyShowSign
# 409 "data/maps/PalletTown/scripts.pory"
    releaseall
# 410 "data/maps/PalletTown/scripts.pory"
    end
# 411 "data/maps/PalletTown/scripts.pory"

# 412 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_SignLadyShowSign::
# 413 "data/maps/PalletTown/scripts.pory"
    textcolor NPC_TEXT_COLOR_FEMALE
# 414 "data/maps/PalletTown/scripts.pory"
    msgbox PalletTown_Text_LookCopiedTrainerTipsSign
# 415 "data/maps/PalletTown/scripts.pory"
    closemessage
# 416 "data/maps/PalletTown/scripts.pory"
    delay 20
# 417 "data/maps/PalletTown/scripts.pory"
    textcolor NPC_TEXT_COLOR_NEUTRAL
# 418 "data/maps/PalletTown/scripts.pory"
    setflag FLAG_OPENED_START_MENU
# 419 "data/maps/PalletTown/scripts.pory"
    setvar VAR_MAP_SCENE_PALLET_TOWN_SIGN_LADY, 1
# 420 "data/maps/PalletTown/scripts.pory"
    setvar SIGN_LADY_READY, FALSE
# 421 "data/maps/PalletTown/scripts.pory"
    @ TODO: Implement sign functionality
# 422 "data/maps/PalletTown/scripts.pory"
    special SetWalkingIntoSignVars
# 423 "data/maps/PalletTown/scripts.pory"
    special DisableMsgBoxWalkaway
# 424 "data/maps/PalletTown/scripts.pory"
    signmsg
# 425 "data/maps/PalletTown/scripts.pory"
    msgbox PalletTown_Text_PressStartToOpenMenuCopy
# 426 "data/maps/PalletTown/scripts.pory"
    normalmsg
# 427 "data/maps/PalletTown/scripts.pory"
    return
# 428 "data/maps/PalletTown/scripts.pory"

# 429 "data/maps/PalletTown/scripts.pory"
PalletTown_EventScript_SignLadyStartShowSign::
# 430 "data/maps/PalletTown/scripts.pory"
    applymovement LOCALID_SIGN_LADY, Common_Movement_FacePlayer
# 431 "data/maps/PalletTown/scripts.pory"
    waitmovement 0
# 432 "data/maps/PalletTown/scripts.pory"
    call PalletTown_EventScript_SignLadyShowSign
# 433 "data/maps/PalletTown/scripts.pory"
    release
# 434 "data/maps/PalletTown/scripts.pory"
    end
# 435 "data/maps/PalletTown/scripts.pory"

# 436 "data/maps/PalletTown/scripts.pory"
PalletTown_Text_OakDontGoOut::
# 437 "data/maps/PalletTown/scripts.pory"
    .string "Oak: Hey! Wait!\n"
# 438 "data/maps/PalletTown/scripts.pory"
    .string "Don't go out!$"
# 439 "data/maps/PalletTown/scripts.pory"

# 440 "data/maps/PalletTown/scripts.pory"
PalletTown_Text_OakGrassUnsafeNeedMon::
# 441 "data/maps/PalletTown/scripts.pory"
    .string "Oak: It's unsafe!\n"
# 442 "data/maps/PalletTown/scripts.pory"
    .string "Wild Pokémon live in tall grass!\p"
# 443 "data/maps/PalletTown/scripts.pory"
    .string "You need your own Pokémon for\n"
# 444 "data/maps/PalletTown/scripts.pory"
    .string "your protection.\p"
# 445 "data/maps/PalletTown/scripts.pory"
    .string "I know!\n"
# 446 "data/maps/PalletTown/scripts.pory"
    .string "Here, come with me!$"
# 447 "data/maps/PalletTown/scripts.pory"

# 448 "data/maps/PalletTown/scripts.pory"
PalletTown_Text_RaisingMonsToo::
# 449 "data/maps/PalletTown/scripts.pory"
    .string "I'm raising Pokémon, too.\p"
# 450 "data/maps/PalletTown/scripts.pory"
    .string "When they get strong, they can\n"
# 451 "data/maps/PalletTown/scripts.pory"
    .string "protect me.$"
# 452 "data/maps/PalletTown/scripts.pory"

# 453 "data/maps/PalletTown/scripts.pory"
PalletTown_Text_CanStoreItemsAndMonsInPC::
# 454 "data/maps/PalletTown/scripts.pory"
    .string "Technology is incredible!\p"
# 455 "data/maps/PalletTown/scripts.pory"
    .string "You can now store and recall items\n"
# 456 "data/maps/PalletTown/scripts.pory"
    .string "and Pokémon as data via PC.$"
# 457 "data/maps/PalletTown/scripts.pory"

# 458 "data/maps/PalletTown/scripts.pory"
PalletTown_Text_OakPokemonResearchLab::
# 459 "data/maps/PalletTown/scripts.pory"
    .string "Oak Pokémon Research Lab$"
# 460 "data/maps/PalletTown/scripts.pory"

# 461 "data/maps/PalletTown/scripts.pory"
PalletTown_Text_PlayersHouse::
# 462 "data/maps/PalletTown/scripts.pory"
    .string "{PLAYER}'s house$"
# 463 "data/maps/PalletTown/scripts.pory"

# 464 "data/maps/PalletTown/scripts.pory"
PalletTown_Text_RivalsHouse::
# 465 "data/maps/PalletTown/scripts.pory"
    .string "{RIVAL}'s house$"
# 466 "data/maps/PalletTown/scripts.pory"

# 467 "data/maps/PalletTown/scripts.pory"
PalletTown_Text_TownSign::
# 468 "data/maps/PalletTown/scripts.pory"
    .string "PALLET TOWN\n"
# 469 "data/maps/PalletTown/scripts.pory"
    .string "Shades of your journey await!$"
# 470 "data/maps/PalletTown/scripts.pory"

# 471 "data/maps/PalletTown/scripts.pory"
PalletTown_Text_OakLetMeSeePokedex::
# 472 "data/maps/PalletTown/scripts.pory"
    .string "OAK: Ah, {PLAYER}!\n"
# 473 "data/maps/PalletTown/scripts.pory"
    .string "You're back, are you?\p"
# 474 "data/maps/PalletTown/scripts.pory"
    .string "How much have you filled in your\n"
# 475 "data/maps/PalletTown/scripts.pory"
    .string "POKéDEX?\p"
# 476 "data/maps/PalletTown/scripts.pory"
    .string "May I see it?\p"
# 477 "data/maps/PalletTown/scripts.pory"
    .string "Let's see…$"
# 478 "data/maps/PalletTown/scripts.pory"

# 479 "data/maps/PalletTown/scripts.pory"
PalletTown_Text_CaughtXPuttingInHonestEffort::
# 480 "data/maps/PalletTown/scripts.pory"
    .string "You've caught {STR_VAR_2}…\p"
# 481 "data/maps/PalletTown/scripts.pory"
    .string "Hm, it looks as if you're putting\n"
# 482 "data/maps/PalletTown/scripts.pory"
    .string "in an honest effort.\p"
# 483 "data/maps/PalletTown/scripts.pory"
    .string "When you manage to fill it some\n"
# 484 "data/maps/PalletTown/scripts.pory"
    .string "more, come show me, please.$"
# 485 "data/maps/PalletTown/scripts.pory"

# 486 "data/maps/PalletTown/scripts.pory"
PalletTown_Text_CaughtXImpressiveFollowMe::
# 487 "data/maps/PalletTown/scripts.pory"
    .string "You've caught… {STR_VAR_2}!?\n"
# 488 "data/maps/PalletTown/scripts.pory"
    .string "Now, this is impressive!\p"
# 489 "data/maps/PalletTown/scripts.pory"
    .string "There's something I wanted to ask\n"
# 490 "data/maps/PalletTown/scripts.pory"
    .string "of you, {PLAYER}.\p"
# 491 "data/maps/PalletTown/scripts.pory"
    .string "Come.\n"
# 492 "data/maps/PalletTown/scripts.pory"
    .string "Follow me.$"
# 493 "data/maps/PalletTown/scripts.pory"

# 494 "data/maps/PalletTown/scripts.pory"
PalletTown_Text_OakYouEnjoyingTraveling::
# 495 "data/maps/PalletTown/scripts.pory"
    .string "OAK: Ah, {PLAYER}!\n"
# 496 "data/maps/PalletTown/scripts.pory"
    .string "You seem to be enjoying traveling.\p"
# 497 "data/maps/PalletTown/scripts.pory"
    .string "Knowing you, {PLAYER}, I can easily\n"
# 498 "data/maps/PalletTown/scripts.pory"
    .string "imagine you going out to even more\l"
# 499 "data/maps/PalletTown/scripts.pory"
    .string "exotic locales.\p"
# 500 "data/maps/PalletTown/scripts.pory"
    .string "Good for you, good for you.\n"
# 501 "data/maps/PalletTown/scripts.pory"
    .string "Hohoho.$"
# 502 "data/maps/PalletTown/scripts.pory"

# 503 "data/maps/PalletTown/scripts.pory"
PalletTown_Text_HmmIsThatRight::
# 504 "data/maps/PalletTown/scripts.pory"
    .string "Hmm…\n"
# 505 "data/maps/PalletTown/scripts.pory"
    .string "Is that right…$"
# 506 "data/maps/PalletTown/scripts.pory"

# 507 "data/maps/PalletTown/scripts.pory"
PalletTown_Text_OhLookLook::
# 508 "data/maps/PalletTown/scripts.pory"
    .string "Oh!\n"
# 509 "data/maps/PalletTown/scripts.pory"
    .string "Look, look!$"
# 510 "data/maps/PalletTown/scripts.pory"

# 511 "data/maps/PalletTown/scripts.pory"
PalletTown_Text_ReadItReadIt::
# 512 "data/maps/PalletTown/scripts.pory"
    .string "Read it, read it!$"
# 513 "data/maps/PalletTown/scripts.pory"

# 514 "data/maps/PalletTown/scripts.pory"
@ Used by sign
# 515 "data/maps/PalletTown/scripts.pory"
PalletTown_Text_PressStartToOpenMenu::
# 516 "data/maps/PalletTown/scripts.pory"
    .string "TRAINER TIPS\p"
# 517 "data/maps/PalletTown/scripts.pory"
    .string "Press START to open the menu!$"
# 518 "data/maps/PalletTown/scripts.pory"

# 519 "data/maps/PalletTown/scripts.pory"
PalletTown_Text_SignsAreUsefulArentThey::
# 520 "data/maps/PalletTown/scripts.pory"
    .string "Signs are useful, aren't they?$"
# 521 "data/maps/PalletTown/scripts.pory"

# 522 "data/maps/PalletTown/scripts.pory"
PalletTown_Text_LookCopiedTrainerTipsSign::
# 523 "data/maps/PalletTown/scripts.pory"
    .string "Look, look!\p"
# 524 "data/maps/PalletTown/scripts.pory"
    .string "I copied what it said on one of\n"
# 525 "data/maps/PalletTown/scripts.pory"
    .string "those TRAINER TIPS signs!$"
# 526 "data/maps/PalletTown/scripts.pory"

# 527 "data/maps/PalletTown/scripts.pory"
@ Used by NPC
# 528 "data/maps/PalletTown/scripts.pory"
PalletTown_Text_PressStartToOpenMenuCopy::
# 529 "data/maps/PalletTown/scripts.pory"
    .string "TRAINER TIPS!\p"
# 530 "data/maps/PalletTown/scripts.pory"
    .string "Press START to open the menu!$"

PalletTown_EventScript_SignLady_Text_0:
# 29 "data/maps/PalletTown/scripts.pory"
	.string "I heard a new girl, Leaf, just moved\nin this house here.\pI wonder if she's a strong Pokémon\ntrainer!$"

PalletTown_EventScript_LeafsHouseSign_Text_0:
# 36 "data/maps/PalletTown/scripts.pory"
	.string "Leaf's house$"
