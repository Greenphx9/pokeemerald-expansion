#define CHECK(sp, perc) {.species = sp, .percentage = perc}
#define CHECK_END {.species = CHECKS_END, .percentage = 0}

static const struct Checks sNoneChecks[] = {
    CHECK_END
};

static const struct Checks sLandoTChecks[] =
{
    CHECK(SPECIES_CLOYSTER, 85),
    CHECK(SPECIES_KYUREM, 84),
    CHECK(SPECIES_LUDICOLO, 79),
    CHECK(SPECIES_LATIAS, 79),
    CHECK(SPECIES_CELESTEELA, 79),
    CHECK(SPECIES_OGERPON_WELLSPRING, 77),
    CHECK(SPECIES_DARKRAI, 77),
    CHECK(SPECIES_GYARADOS, 77),
    CHECK(SPECIES_LATIOS, 77),
    CHECK(SPECIES_SERPERIOR, 76),
    CHECK(SPECIES_LILLIGANT_HISUIAN, 76),
    CHECK_END
};