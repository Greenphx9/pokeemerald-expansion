@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/PalletTown_RivalsHouse_1F/map.json
@

PalletTown_RivalsHouse_1F_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_DAISY, 10, 6, 0, MOVEMENT_TYPE_WANDER_AROUND, 1, 3, TRAINER_TYPE_NONE, 0, PalletTown_RivalsHouse_1F_EventScript_Daisy, 0
	object_event 2, OBJ_EVENT_GFX_ANABEL, 6, 4, 0, MOVEMENT_TYPE_FACE_DOWN, 1, 1, TRAINER_TYPE_NONE, 0, PalletTown_RivalsHouse_1F_EventScript_TownMap, FLAG_HIDE_TOWN_MAP

PalletTown_RivalsHouse_1F_MapWarps:
	warp_def 4, 8, 0, 1, MAP_PALLET_TOWN
	warp_def 5, 8, 0, 1, MAP_PALLET_TOWN
	warp_def 3, 8, 0, 1, MAP_PALLET_TOWN

PalletTown_RivalsHouse_1F_MapBGEvents:
	bg_sign_event 12, 1, 0, BG_EVENT_PLAYER_FACING_ANY, PalletTown_RivalsHouse_1F_EventScript_Bookshelf
	bg_sign_event 11, 1, 0, BG_EVENT_PLAYER_FACING_ANY, PalletTown_RivalsHouse_1F_EventScript_Bookshelf
	bg_sign_event 9, 1, 0, BG_EVENT_PLAYER_FACING_ANY, PalletTown_RivalsHouse_1F_EventScript_Picture

PalletTown_RivalsHouse_1F_MapEvents::
	map_events PalletTown_RivalsHouse_1F_ObjectEvents, PalletTown_RivalsHouse_1F_MapWarps, NULL, PalletTown_RivalsHouse_1F_MapBGEvents

