const struct TrainerMon sTeraRaidParty_Red[] =
{
    {
        .species = SPECIES_BULBASAUR,
        .heldItem = ITEM_LEFTOVERS,
    },
    {
        .species = SPECIES_SQUIRTLE,
        .heldItem = ITEM_LEFTOVERS,
    },
    {
        .species = SPECIES_CHARMANDER,
        .heldItem = ITEM_LEFTOVERS,
    }
};

const struct TeraRaidPartner sTeraRaidPartners[] = 
{
    {
        .trainerName = _("Red"),
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
