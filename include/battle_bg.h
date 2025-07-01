#ifndef GUARD_BATTLE_BG_H
#define GUARD_BATTLE_BG_H

struct BattleBackground
{
    const void *tileset;
    const void *tilemap;
    const void *entryTileset;
    const void *entryTilemap;
    const void *palette;
};

struct DNSBattleBackground
{
    const void *dayTileset;
    const void *dayTilemap;
    const u8 *dayPalette;
    const void *afternoonTileset;
    const void *afternoonTilemap;
    const u8 *afternoonPalette;
    const void *nightTileset;
    const void *nightTilemap;
    const u8 *nightPalette;
    const void *entryTileset;
    const void *entryTilemap;  
};

void BattleInitBgsAndWindows(void);
void InitBattleBgsVideo(void);
void LoadBattleMenuWindowGfx(void);
void DrawMainBattleBackground(void);
void LoadBattleTextboxAndBackground(void);
void InitLinkBattleVsScreen(u8 taskId);
void DrawBattleEntryBackground(void);
bool8 LoadChosenBattleElement(u8 caseId);
void DrawTerrainTypeBattleBackground(void);

#endif // GUARD_BATTLE_BG_H
