#pragma once

enum MAPDATA {
	MAPDATA_1P,
	MAPDATA_2P,
	MAPDATA_4P,
	MAPDATA_END,
	MAPDATA_TUTORIAL,
	MAPDATA_MAX
};

void MapEditInit(void);
void MapEdit(void);
void GetMouseData(void);
void MapLoad(MAPDATA data);

extern SAVE_DATA saveData;