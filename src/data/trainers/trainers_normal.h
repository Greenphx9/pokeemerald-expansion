//
// DO NOT MODIFY THIS FILE! It is auto-generated from src/data/trainers/trainers_normal.party
//
// If you want to modify this file set COMPETITIVE_PARTY_SYNTAX to FALSE
// in include/config/general.h and remove this notice.
// Use sed -i '/^#line/d' 'src/data/trainers/trainers_normal.h' to remove #line markers.
//

#line 1 "src/data/trainers/trainers_normal.party"

#line 76
    [TRAINER_NONE] =
    {
#line 78
        .trainerClass = TRAINER_CLASS_PKMN_TRAINER_1,
#line 79
        .trainerPic = TRAINER_PIC_HIKER,
        .encounterMusic_gender = 
#line 81
            TRAINER_ENCOUNTER_MUSIC_MALE,
#line 82
        .doubleBattle = FALSE,
        .partySize = 0,
        .party = (const struct TrainerMon[])
        {
        },
    },
#line 84
    [TRAINER_RIVAL_OAKS_LAB_SQUIRTLE] =
    {
#line 85
        .trainerName = _("TERRY"),
#line 86
        .trainerClass = TRAINER_CLASS_RIVAL_EARLY,
#line 87
        .trainerPic = TRAINER_PIC_RIVAL_EARLY,
        .encounterMusic_gender = 
#line 89
            TRAINER_ENCOUNTER_MUSIC_MALE,
#line 90
        .doubleBattle = FALSE,
#line 91
        .aiFlags = AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY,
        .partySize = 1,
        .party = (const struct TrainerMon[])
        {
            {
#line 93
            .species = SPECIES_POPPLIO,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 95
            .iv = TRAINER_PARTY_IVS(0, 0, 0, 0, 0, 0),
#line 94
            .lvl = 5,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            },
        },
    },
#line 97
    [TRAINER_RIVAL_OAKS_LAB_BULBASAUR] =
    {
#line 98
        .trainerName = _("TERRY"),
#line 99
        .trainerClass = TRAINER_CLASS_RIVAL_EARLY,
#line 100
        .trainerPic = TRAINER_PIC_RIVAL_EARLY,
        .encounterMusic_gender = 
#line 102
            TRAINER_ENCOUNTER_MUSIC_MALE,
#line 103
        .doubleBattle = FALSE,
#line 104
        .aiFlags = AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY,
        .partySize = 1,
        .party = (const struct TrainerMon[])
        {
            {
#line 106
            .species = SPECIES_SPRIGATITO,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 108
            .iv = TRAINER_PARTY_IVS(0, 0, 0, 0, 0, 0),
#line 107
            .lvl = 5,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            },
        },
    },
#line 110
    [TRAINER_RIVAL_OAKS_LAB_CHARMANDER] =
    {
#line 111
        .trainerName = _("TERRY"),
#line 112
        .trainerClass = TRAINER_CLASS_RIVAL_EARLY,
#line 113
        .trainerPic = TRAINER_PIC_RIVAL_EARLY,
        .encounterMusic_gender = 
#line 115
            TRAINER_ENCOUNTER_MUSIC_MALE,
#line 116
        .doubleBattle = FALSE,
#line 117
        .aiFlags = AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY,
        .partySize = 1,
        .party = (const struct TrainerMon[])
        {
            {
#line 119
            .species = SPECIES_CHARMANDER,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 121
            .iv = TRAINER_PARTY_IVS(0, 0, 0, 0, 0, 0),
#line 120
            .lvl = 5,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            },
        },
    },
