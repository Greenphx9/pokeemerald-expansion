@
@ DO NOT MODIFY THIS FILE! It is auto-generated from data/maps/PalletTown_PlayersHouse_1F/map.json
@

PalletTown_PlayersHouse_1F_ObjectEvents:
	object_event 1, OBJ_EVENT_GFX_MOM, 8, 4, 0, MOVEMENT_TYPE_FACE_LEFT, 0, 0, TRAINER_TYPE_NONE, 0, PalletTown_PlayersHouse_1F_EventScript_Mom, 0

PalletTown_PlayersHouse_1F_MapWarps:
	warp_def 5, 8, 0, 0, MAP_PALLET_TOWN
	warp_def 4, 8, 0, 0, MAP_PALLET_TOWN
	warp_def 10, 2, 0, 0, MAP_PALLET_TOWN_PLAYERS_HOUSE_2F
	warp_def 3, 9, 0, 0, MAP_PALLET_TOWN

PalletTown_PlayersHouse_1F_MapBGEvents:
	bg_sign_event 6, 1, 0, BG_EVENT_PLAYER_FACING_ANY, PalletTown_PlayersHouse_1F_EventScript_TV

PalletTown_PlayersHouse_1F_MapEvents::
	map_events PalletTown_PlayersHouse_1F_ObjectEvents, PalletTown_PlayersHouse_1F_MapWarps, NULL, PalletTown_PlayersHouse_1F_MapBGEvents

