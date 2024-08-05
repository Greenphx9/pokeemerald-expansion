//
// DO NOT MODIFY THIS FILE! It is auto-generated from src/data/trainers/trainers_casual.party
//
// If you want to modify this file set COMPETITIVE_PARTY_SYNTAX to FALSE
// in include/config/general.h and remove this notice.
// Use sed -i '/^#line/d' 'src/data/trainers/trainers_casual.h' to remove #line markers.
//

#line 1 "src/data/trainers/trainers_casual.party"

#line 10
    [TRAINER_NONE] =
    {
#line 12
        .trainerClass = TRAINER_CLASS_PKMN_TRAINER_1,
#line 13
        .trainerPic = TRAINER_PIC_HIKER,
        .encounterMusic_gender = 
#line 15
            TRAINER_ENCOUNTER_MUSIC_MALE,
#line 16
        .doubleBattle = FALSE,
        .partySize = 1,
        .party = (const struct TrainerMon[])
        {
            {
#line 18
            .species = SPECIES_BULBASAUR,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 20
            .iv = TRAINER_PARTY_IVS(0, 0, 0, 0, 0, 0),
#line 19
            .lvl = 10,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            },
        },
    },
