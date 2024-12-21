const struct TrainerMon sTeraRaidParty_Red[] =
{
            {
#line 9
            .nickname = COMPOUND_STRING("Bubbles"),
#line 9
            .species = SPECIES_WOBBUFFET,
#line 9
            .gender = TRAINER_MON_FEMALE,
#line 9
            .heldItem = ITEM_ASSAULT_VEST,
#line 14
            .ev = TRAINER_PARTY_EVS(252, 0, 0, 252, 4, 0),
#line 13
            .iv = TRAINER_PARTY_IVS(25, 26, 27, 28, 29, 30),
#line 12
            .ability = ABILITY_SHADOW_TAG,
#line 11
            .lvl = 1,
#line 17
            .ball = ITEM_MASTER_BALL,
#line 15
            .friendship = 42,
#line 10
            .nature = NATURE_HASTY,
#line 16
            .isShiny = TRUE,
#line 18
            .dynamaxLevel = 5,
            .shouldUseDynamax = TRUE,
            .moves = {
#line 19
                MOVE_PSYCHIC,
                MOVE_PSYCHIC,
                MOVE_PSYCHIC,
                MOVE_PSYCHIC,
            },
            },
            {
#line 24
            .species = SPECIES_WOBBUFFET,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 27
            .iv = TRAINER_PARTY_IVS(0, 0, 0, 0, 0, 0),
#line 26
            .ability = ABILITY_SHADOW_TAG,
#line 25
            .lvl = 5,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            },
            {
#line 29
            .species = SPECIES_WYNAUT,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 31
            .iv = TRAINER_PARTY_IVS(0, 0, 0, 0, 0, 0),
#line 30
            .lvl = 5,
            .nature = NATURE_HARDY,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            },
};

const struct TeraRaidPartner gTeraRaidPartners[] = 
{
    {
        .trainerName = _("Red"),
        .gender = TRAINER_MON_MALE,
        .otId = 0x0,
        .objectEventGfx = OBJ_EVENT_GFX_RED,
        .trainerClass = TRAINER_CLASS_PKMN_TRAINER_1,
        .trainerBackPic = TRAINER_BACK_PIC_RED,
        .parties = 
        {
            [0 ... 6] = sTeraRaidParty_Red,
        },
        .partySizes = 
        {
            [0 ... 6] = ARRAY_COUNT(sTeraRaidParty_Red),
        },
    },
    {
        .trainerName = _("Red2"),
        .gender = TRAINER_MON_FEMALE,
        .otId = 0x0,
        .objectEventGfx = OBJ_EVENT_GFX_LEAF,
        .trainerClass = TRAINER_CLASS_PKMN_TRAINER_1,
        .trainerBackPic = TRAINER_BACK_PIC_RED,
        .parties = 
        {
            [0 ... 6] = sTeraRaidParty_Red,
        },
        .partySizes = 
        {
            [0 ... 6] = ARRAY_COUNT(sTeraRaidParty_Red),
        },
    },
    {
        .trainerName = _("Red3"),
        .gender = TRAINER_MON_MALE,
        .otId = 0x0,
        .objectEventGfx = OBJ_EVENT_GFX_AQUA_MEMBER_M,
        .trainerClass = TRAINER_CLASS_PKMN_TRAINER_1,
        .trainerBackPic = TRAINER_BACK_PIC_RED,
        .parties = 
        {
            [0 ... 6] = sTeraRaidParty_Red,
        },
        .partySizes = 
        {
            [0 ... 6] = ARRAY_COUNT(sTeraRaidParty_Red),
        },
    },
    {
        .trainerName = _("Red4"),
        .gender = TRAINER_MON_FEMALE,
        .otId = 0x0,
        .objectEventGfx = OBJ_EVENT_GFX_AQUA_MEMBER_F,
        .trainerClass = TRAINER_CLASS_PKMN_TRAINER_1,
        .trainerBackPic = TRAINER_BACK_PIC_RED,
        .parties = 
        {
            [0 ... 6] = sTeraRaidParty_Red,
        },
        .partySizes = 
        {
            [0 ... 6] = ARRAY_COUNT(sTeraRaidParty_Red),
        },
    },
    {
        .trainerName = _("Red5"),
        .gender = TRAINER_MON_MALE,
        .otId = 0x0,
        .objectEventGfx = OBJ_EVENT_GFX_BRENDAN_NORMAL,
        .trainerClass = TRAINER_CLASS_PKMN_TRAINER_1,
        .trainerBackPic = TRAINER_BACK_PIC_RED,
        .parties = 
        {
            [0 ... 6] = sTeraRaidParty_Red,
        },
        .partySizes = 
        {
            [0 ... 6] = ARRAY_COUNT(sTeraRaidParty_Red),
        },
    }
};
