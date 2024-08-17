@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/ViridianCity/map.json
@

ViridianCity_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_FAT_MAN, 8, 26, 3, MOVEMENT_TYPE_FACE_DOWN, 0, 0, TRAINER_TYPE_NONE, 0, ViridianCity_EventScript_DreamEaterTutor, 0
	object_event 2, OBJ_EVENT_GFX_CUTTABLE_TREE, 11, 24, 3, MOVEMENT_TYPE_FACE_DOWN, 1, 1, TRAINER_TYPE_NONE, 0, EventScript_CutTree, FLAG_TEMP_11
	object_event 3, OBJ_EVENT_GFX_OLD_MAN, 34, 11, 3, MOVEMENT_TYPE_FACE_UP, 0, 0, TRAINER_TYPE_NONE, 0, ViridianCity_EventScript_OldMan, 0
	object_event 4, OBJ_EVENT_GFX_VAR_0, 21, 6, 3, MOVEMENT_TYPE_LOOK_AROUND, 2, 3, TRAINER_TYPE_NONE, 0, ViridianCity_EventScript_TutorialOldMan, 0
	object_event 5, OBJ_EVENT_GFX_WOMAN_3, 20, 12, 3, MOVEMENT_TYPE_FACE_UP, 1, 3, TRAINER_TYPE_NONE, 0, ViridianCity_EventScript_Woman, 0
	object_event 6, OBJ_EVENT_GFX_YOUNGSTER, 33, 26, 3, MOVEMENT_TYPE_WANDER_UP_AND_DOWN, 0, 2, TRAINER_TYPE_NONE, 0, ViridianCity_EventScript_Youngster, 0
	object_event 7, OBJ_EVENT_GFX_FISHERMAN, 16, 22, 3, MOVEMENT_TYPE_WANDER_AROUND, 1, 2, TRAINER_TYPE_NONE, 0, ViridianCity_EventScript_Boy, 0
	object_event 8, OBJ_EVENT_GFX_CUTTABLE_TREE, 18, 5, 3, MOVEMENT_TYPE_FACE_DOWN, 1, 1, TRAINER_TYPE_NONE, 0, EventScript_CutTree, FLAG_TEMP_12
	object_event 9, OBJ_EVENT_GFX_ITEM_BALL, 17, 5, 3, MOVEMENT_TYPE_FACE_DOWN, 1, 1, TRAINER_TYPE_NONE, ITEM_POTION, Common_EventScript_FindItem, FLAG_HIDE_VIRIDIAN_CITY_POTION

ViridianCity_MapWarps:
	warp_def 26, 26, 0, 1, MAP_DYNAMIC
	warp_def 25, 11, 3, 1, MAP_DYNAMIC
	warp_def 36, 10, 0, 1, MAP_DYNAMIC
	warp_def 25, 18, 0, 1, MAP_DYNAMIC
	warp_def 36, 19, 0, 1, MAP_DYNAMIC

ViridianCity_MapCoordEvents:
	coord_event 22, 11, 3, VAR_MAP_SCENE_VIRIDIAN_CITY_OLD_MAN, 0, ViridianCity_EventScript_RoadBlocked
	coord_event 36, 11, 3, VAR_MAP_SCENE_VIRIDIAN_CITY_GYM_DOOR, 0, ViridianCity_EventScript_GymDoorLocked
	coord_event 20, 8, 3, VAR_MAP_SCENE_VIRIDIAN_CITY_OLD_MAN, 1, ViridianCity_EventScript_TutorialTriggerLeft
	coord_event 22, 8, 3, VAR_MAP_SCENE_VIRIDIAN_CITY_OLD_MAN, 1, ViridianCity_EventScript_TutorialTriggerRight

ViridianCity_MapBGEvents:
	bg_sign_event 23, 1, 3, BG_EVENT_PLAYER_FACING_ANY, ViridianCity_EventScript_TrainerTips1
	bg_sign_event 32, 10, 0, BG_EVENT_PLAYER_FACING_ANY, ViridianCity_EventScript_GymSign
	bg_sign_event 20, 31, 0, BG_EVENT_PLAYER_FACING_ANY, ViridianCity_EventScript_TrainerTips2
	bg_sign_event 20, 16, 0, BG_EVENT_PLAYER_FACING_ANY, ViridianCity_EventScript_CitySign
	bg_sign_event 36, 10, 0, BG_EVENT_PLAYER_FACING_ANY, ViridianCity_EventScript_GymDoor

ViridianCity_MapEvents::
	map_events ViridianCity_ObjectEvents, ViridianCity_MapWarps, ViridianCity_MapCoordEvents, ViridianCity_MapBGEvents

