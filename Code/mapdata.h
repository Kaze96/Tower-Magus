#pragma once

void MapDataSysInit(void);
void MapDataGameInit(void);
void MapDataControl(void);
void MapDataDraw(void);
POS MapPosToIndex(POS pos);
POS MapPosToIndex(POS_F pos);
POS MapIndexToPos(POS Index);
POS_F MapIndexToPosF(POS Index);
void LoadMapLoopData(void);

extern int tilesetImg[88];
extern int mapData[LAYER_MAX][MAP_CHIP_Y][MAP_CHIP_X];
extern int enemySpawnData[MAP_CHIP_Y][MAP_CHIP_X];