const struct Tileset gTileset_General_Kanto =
{
    .isCompressed = TRUE,
    .isSecondary = FALSE,
    .tiles = gTilesetTiles_General_Kanto,
    .palettes = gTilesetPalettes_General_Kanto,
    .metatiles = gMetatiles_General_Kanto,
    .metatileAttributes = gMetatileAttributes_General_Kanto,
    .callback = InitTilesetAnim_General_Kanto,
};

const struct Tileset gTileset_PalletTown =
{
    .isCompressed = TRUE,
    .isSecondary = TRUE,
    .tiles = gTilesetTiles_PalletTown,
    .palettes = gTilesetPalettes_PalletTown,
    .metatiles = gMetatiles_PalletTown,
    .metatileAttributes = gMetatileAttributes_PalletTown,
    .callback = NULL,
};

const struct Tileset gTileset_Building_Kanto =
{
    .isCompressed = TRUE,
    .isSecondary = FALSE,
    .tiles = gTilesetTiles_Building_Kanto,
    .palettes = gTilesetPalettes_Building_Kanto,
    .metatiles = gMetatiles_Building_Kanto,
    .metatileAttributes = gMetatileAttributes_Building_Kanto,
    .callback = NULL,
};

const struct Tileset gTileset_GenericBuilding1 =
{
    .isCompressed = TRUE,
    .isSecondary = TRUE,
    .tiles = gTilesetTiles_GenericBuilding1,
    .palettes = gTilesetPalettes_GenericBuilding1,
    .metatiles = gMetatiles_GenericBuilding1,
    .metatileAttributes = gMetatileAttributes_GenericBuilding1,
    .callback = NULL,
};
