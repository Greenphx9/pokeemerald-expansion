#ifndef GUARD_CONFIG_SAVE_H
#define GUARD_CONFIG_SAVE_H

// SaveBlock1 configs
#define FREE_EXTRA_SEEN_FLAGS_SAVEBLOCK1    TRUE    // Free up unused Pokédex seen flags (52 bytes).
#define FREE_TRAINER_HILL                   TRUE    // Frees up Trainer Hill data (28 bytes).
#define FREE_MYSTERY_EVENT_BUFFERS          TRUE    // Frees up ramScript (1104 bytes).
#define FREE_MATCH_CALL                     TRUE    // Frees up match call and rematch / VS Seeker data. (104 bytes).
#define FREE_UNION_ROOM_CHAT                TRUE    // Frees up union room chat (212 bytes).
#define FREE_ENIGMA_BERRY                   TRUE    // Frees up E-Reader Enigma Berry data (52 bytes).
#define FREE_LINK_BATTLE_RECORDS            TRUE    // Frees up link battle record data (88 bytes).
#define FREE_MYSTERY_GIFT                   TRUE    // Frees up Mystery Gift data (876 bytes).
                                            // SaveBlock1 total: 2516 bytes
// SaveBlock2 configs
#define FREE_BATTLE_TOWER_E_READER          TRUE    // Frees up Battle Tower E-Reader data (188 bytes).
#define FREE_POKEMON_JUMP                   TRUE    // Frees up Pokémon Jump data (16 bytes).
#define FREE_RECORD_MIXING_HALL_RECORDS     TRUE    // Frees up hall records for record mixing (1032 bytes).
#define FREE_EXTRA_SEEN_FLAGS_SAVEBLOCK2    TRUE    // Free up unused Pokédex seen flags (108 bytes).
                                            // SaveBlock2 total: 1274 bytes

                                            // Grand Total: 3790

// Options to free members in BoxMon that the user may not need
// Note that it only removes the value in BoxMon and associated cases in 
// Get/SetBoxMon, not the actual MON_DATA calls, no clue what will happen
// if you tried to get or set a value that doesn't exist!   
#define FREE_CONTEST_STATS                  FALSE   // Removes contest stats (cool, sheen, etc) (6 bytes)
#define FREE_RIBBONS                        FALSE   // Removes contest ribbons (3 bytes)
#define FREE_POKERUS                        FALSE   // Removes pokerus (1 byte)
#define FREE_LOST_HP                        FALSE   // Removes lost HP, note that putting in PC will restore HP! (2 bytes)
#define FREE_MARKINGS_AND_STATUS            FALSE   // Removes PC markings and compressed status (1 byte)
                                                    // Note that this does not change the PC UI, so players can still mark, although it will not save.
#define FREE_OT_NAME                        FALSE   // Reduces ot name to 1 byte, by using a lookup table (6 bytes)
                                                    // Pokemon traded between games will have incorrect ot names!
                                                    // See: gOtNames in src/pokemon.c if you enable this
#define FREE_MET_GAME_DYNAMAX_LEVEL         FALSE   // Removes the met game, not needed, and dynamax level (1 byte)

#define FREE_RANDOM_UNUSED_BYTES            FALSE   // Compiler somehow optimizes the struct when unused bits are removed (4 bytes)
#define FREE_OT_ID                          FALSE   // Reduces ot id to 1 byte, by using a lookup table (3 bytes)

                                            // Total: 27 bytes

#endif // GUARD_CONFIG_SAVE_H
