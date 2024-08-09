//
// DO NOT MODIFY THIS FILE! It is auto-generated from src/data/trainers/trainers_casual.party
//
// If you want to modify this file set COMPETITIVE_PARTY_SYNTAX to FALSE
// in include/config/general.h and remove this notice.
// Use sed -i '/^#line/d' 'src/data/trainers/trainers_casual.h' to remove #line markers.
//

#line 1 "src/data/trainers/trainers_casual.party"

#line 75
    [TRAINER_NONE] =
    {
#line 77
        .trainerClass = TRAINER_CLASS_PKMN_TRAINER_1,
#line 78
        .trainerPic = TRAINER_PIC_HIKER,
        .encounterMusic_gender = 
#line 80
            TRAINER_ENCOUNTER_MUSIC_MALE,
#line 81
        .doubleBattle = FALSE,
        .partySize = 1,
        .party = (const struct TrainerMon[])
        {
            {
#line 83
            .species = SPECIES_BULBASAUR,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 85
            .iv = TRAINER_PARTY_IVS(0, 0, 0, 0, 0, 0),
#line 84
            .lvl = 10,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            },
        },
    },
