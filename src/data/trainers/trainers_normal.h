//
// DO NOT MODIFY THIS FILE! It is auto-generated from src/data/trainers/trainers_normal.party
//
// If you want to modify this file set COMPETITIVE_PARTY_SYNTAX to FALSE
// in include/config/general.h and remove this notice.
// Use sed -i '/^#line/d' 'src/data/trainers/trainers_normal.h' to remove #line markers.
//

#line 1 "src/data/trainers/trainers_normal.party"

#line 11
    [TRAINER_NONE] =
    {
#line 13
        .trainerClass = TRAINER_CLASS_PKMN_TRAINER_1,
#line 14
        .trainerPic = TRAINER_PIC_HIKER,
        .encounterMusic_gender = 
#line 16
            TRAINER_ENCOUNTER_MUSIC_MALE,
#line 17
        .doubleBattle = FALSE,
        .partySize = 0,
        .party = (const struct TrainerMon[])
        {
        },
    },
#line 19
    [TRAINER_RIVAL_OAKS_LAB_SQUIRTLE] =
    {
#line 20
        .trainerName = _("TERRY"),
#line 21
        .trainerClass = TRAINER_CLASS_RIVAL_EARLY,
#line 22
        .trainerPic = TRAINER_PIC_RIVAL_EARLY,
        .encounterMusic_gender = 
#line 24
            TRAINER_ENCOUNTER_MUSIC_MALE,
#line 25
        .doubleBattle = FALSE,
#line 26
        .aiFlags = AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY,
        .partySize = 1,
        .party = (const struct TrainerMon[])
        {
            {
#line 28
            .species = SPECIES_SQUIRTLE,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 30
            .iv = TRAINER_PARTY_IVS(0, 0, 0, 0, 0, 0),
#line 29
            .lvl = 5,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            },
        },
    },
#line 32
    [TRAINER_RIVAL_OAKS_LAB_BULBASAUR] =
    {
#line 33
        .trainerName = _("TERRY"),
#line 34
        .trainerClass = TRAINER_CLASS_RIVAL_EARLY,
#line 35
        .trainerPic = TRAINER_PIC_RIVAL_EARLY,
        .encounterMusic_gender = 
#line 37
            TRAINER_ENCOUNTER_MUSIC_MALE,
#line 38
        .doubleBattle = FALSE,
#line 39
        .aiFlags = AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY,
        .partySize = 1,
        .party = (const struct TrainerMon[])
        {
            {
#line 41
            .species = SPECIES_BULBASAUR,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 43
            .iv = TRAINER_PARTY_IVS(0, 0, 0, 0, 0, 0),
#line 42
            .lvl = 5,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            },
        },
    },
#line 45
    [TRAINER_RIVAL_OAKS_LAB_CHARMANDER] =
    {
#line 46
        .trainerName = _("TERRY"),
#line 47
        .trainerClass = TRAINER_CLASS_RIVAL_EARLY,
#line 48
        .trainerPic = TRAINER_PIC_RIVAL_EARLY,
        .encounterMusic_gender = 
#line 50
            TRAINER_ENCOUNTER_MUSIC_MALE,
#line 51
        .doubleBattle = FALSE,
#line 52
        .aiFlags = AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY,
        .partySize = 1,
        .party = (const struct TrainerMon[])
        {
            {
#line 54
            .species = SPECIES_CHARMANDER,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 56
            .iv = TRAINER_PARTY_IVS(0, 0, 0, 0, 0, 0),
#line 55
            .lvl = 5,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            },
        },
    },
