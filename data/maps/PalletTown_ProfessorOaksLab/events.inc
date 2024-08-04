@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/PalletTown_ProfessorOaksLab/map.json
@

PalletTown_ProfessorOaksLab_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_SCIENTIST_1, 3, 11, 3, MOVEMENT_TYPE_LOOK_AROUND, 0, 0, TRAINER_TYPE_NONE, 0, PalletTown_ProfessorOaksLab_EventScript_Aide1, 0
	object_event 2, OBJ_EVENT_GFX_LASS, 2, 10, 3, MOVEMENT_TYPE_WANDER_UP_AND_DOWN, 0, 4, TRAINER_TYPE_NONE, 0, PalletTown_ProfessorOaksLab_EventScript_Aide3, 0
	object_event 3, OBJ_EVENT_GFX_SCIENTIST_1, 11, 10, 3, MOVEMENT_TYPE_LOOK_AROUND, 0, 0, TRAINER_TYPE_NONE, 0, PalletTown_ProfessorOaksLab_EventScript_Aide2, 0
	object_event 4, OBJ_EVENT_GFX_PROF_OAK, 6, 3, 3, MOVEMENT_TYPE_FACE_DOWN, 0, 0, TRAINER_TYPE_NONE, 0, PalletTown_ProfessorOaksLab_EventScript_ProfOak, FLAG_HIDE_OAK_IN_HIS_LAB
	object_event 5, OBJ_EVENT_GFX_ITEM_BALL, 8, 4, 0, MOVEMENT_TYPE_FACE_DOWN, 1, 1, TRAINER_TYPE_NONE, 0, PalletTown_ProfessorOaksLab_EventScript_BulbasaurBall, FLAG_HIDE_BULBASAUR_BALL
	object_event 6, OBJ_EVENT_GFX_ITEM_BALL, 9, 4, 0, MOVEMENT_TYPE_FACE_DOWN, 1, 1, TRAINER_TYPE_NONE, 0, PalletTown_ProfessorOaksLab_EventScript_SquirtleBall, FLAG_HIDE_SQUIRTLE_BALL
	object_event 7, OBJ_EVENT_GFX_ITEM_BALL, 10, 4, 0, MOVEMENT_TYPE_FACE_DOWN, 1, 1, TRAINER_TYPE_NONE, 0, PalletTown_ProfessorOaksLab_EventScript_CharmanderBall, FLAG_HIDE_CHARMANDER_BALL
	object_event 8, OBJ_EVENT_GFX_LEAF, 5, 4, 3, MOVEMENT_TYPE_FACE_DOWN, 1, 1, TRAINER_TYPE_NONE, 0, PalletTown_ProfessorOaksLab_EventScript_Rival, FLAG_HIDE_RIVAL_IN_LAB
	object_event 9, OBJ_EVENT_GFX_DEOXYS_TRIANGLE, 4, 1, 0, MOVEMENT_TYPE_FACE_DOWN, 1, 1, TRAINER_TYPE_NONE, 0, PalletTown_ProfessorOaksLab_EventScript_Pokedex, 0
	object_event 10, OBJ_EVENT_GFX_DEOXYS_TRIANGLE, 5, 1, 0, MOVEMENT_TYPE_FACE_DOWN, 1, 1, TRAINER_TYPE_NONE, 0, PalletTown_ProfessorOaksLab_EventScript_Pokedex, 0

PalletTown_ProfessorOaksLab_MapWarps:
	warp_def 6, 12, 0, 2, MAP_PALLET_TOWN
	warp_def 7, 12, 0, 2, MAP_PALLET_TOWN
	warp_def 5, 12, 0, 2, MAP_PALLET_TOWN

PalletTown_ProfessorOaksLab_MapCoordEvents:
	coord_event 5, 8, 0, VAR_MAP_SCENE_PALLET_TOWN_PROFESSOR_OAKS_LAB, 2, PalletTown_ProfessorOaksLab_EventScript_LeaveStarterSceneTrigger
	coord_event 6, 8, 0, VAR_MAP_SCENE_PALLET_TOWN_PROFESSOR_OAKS_LAB, 2, PalletTown_ProfessorOaksLab_EventScript_LeaveStarterSceneTrigger
	coord_event 7, 8, 0, VAR_MAP_SCENE_PALLET_TOWN_PROFESSOR_OAKS_LAB, 2, PalletTown_ProfessorOaksLab_EventScript_LeaveStarterSceneTrigger
	coord_event 5, 8, 0, VAR_MAP_SCENE_PALLET_TOWN_PROFESSOR_OAKS_LAB, 3, PalletTown_ProfessorOaksLab_EventScript_RivalBattleTriggerLeft
	coord_event 6, 8, 0, VAR_MAP_SCENE_PALLET_TOWN_PROFESSOR_OAKS_LAB, 3, PalletTown_ProfessorOaksLab_EventScript_RivalBattleTriggerLeft
	coord_event 7, 8, 0, VAR_MAP_SCENE_PALLET_TOWN_PROFESSOR_OAKS_LAB, 3, PalletTown_ProfessorOaksLab_EventScript_RivalBattleTriggerLeft

PalletTown_ProfessorOaksLab_MapBGEvents:
	bg_sign_event 2, 1, 0, BG_EVENT_PLAYER_FACING_ANY, PalletTown_ProfessorOaksLab_EventScript_Computer
	bg_sign_event 3, 1, 0, BG_EVENT_PLAYER_FACING_ANY, PalletTown_ProfessorOaksLab_EventScript_Computer
	bg_sign_event 6, 1, 0, BG_EVENT_PLAYER_FACING_ANY, PalletTown_ProfessorOaksLab_EventScript_RightSign
	bg_sign_event 7, 1, 0, BG_EVENT_PLAYER_FACING_ANY, PalletTown_ProfessorOaksLab_EventScript_LeftSign

PalletTown_ProfessorOaksLab_MapEvents::
	map_events PalletTown_ProfessorOaksLab_ObjectEvents, PalletTown_ProfessorOaksLab_MapWarps, PalletTown_ProfessorOaksLab_MapCoordEvents, PalletTown_ProfessorOaksLab_MapBGEvents

