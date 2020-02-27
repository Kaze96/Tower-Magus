#include "main.h"
#include "map_edit.h"
#include "keycheck.h"
#include "stage.h"
#include "enemy.h"
#include "Character.h"
#include "mapdata.h"

POS mousePos;
bool editChange;		// false : マップの編集, true : 敵の編集
SAVE_DATA saveData;
int editChipId;
int editIdMax;
int animCnt;
int mapId;

POS chipSize = { MAP_CHIP_SIZE_X,MAP_CHIP_SIZE_Y };

void MapEditDraw(void);
void EditCursorControl(void);
void SetChipID(POS pos, int id);
void DeleteChipID(POS pos, int id);
void CopyChipID(POS pos);
void MapSave(MAPDATA data);

void MapEditInit(void)
{
	for (int z = 0; z < LAYER_MAX; z++)
	{
		for (int y = 0; y < MAP_CHIP_Y; y++)
		{
			for (int x = 0; x < MAP_CHIP_X; x++)
			{
				mapData[z][y][x] = -1;
			}
		}
	}

	for (int y = 0; y < MAP_CHIP_Y; y++)
	{
		for (int x = 0; x < MAP_CHIP_X; x++)
		{
			enemySpawnData[y][x] = -1;
		}
	}

	editChipId = 0;
	animCnt = 0;
	editChange = false;
	editIdMax = 88;
	mapId = MAPDATA_1P;
	MapLoad(MAPDATA_1P);
	GameModeChange(MAP_EDIT);
}

void MapEdit(void)
{
	EditCursorControl();
	MapEditDraw();
}

void MapEditDraw(void)
{
	DrawBox(0, 0, MAP_CHIP_X * MAP_CHIP_SIZE_X, MAP_CHIP_Y * MAP_CHIP_SIZE_Y, 0xffffff, true);

	// Draw Tileset
	for (int z = 0; z < LAYER_MAX; z++)
	{
		for (int y = 0; y < MAP_CHIP_Y; y++)
		{
			for (int x = 0; x < MAP_CHIP_X; x++)
			{
				POS tmpPos = AddScroll({ MAP_CHIP_SIZE_X * x, MAP_CHIP_SIZE_Y * y });
				DrawExtendGraph(
					tmpPos.x, tmpPos.y,
					tmpPos.x + 64, tmpPos.y + 64,
					tilesetImg[mapData[z][y][x]], true);
			}
		}
	}

	// Draw Enemy Spawn Place
	for (int y = 0; y < MAP_CHIP_Y; y++)
	{
		for (int x = 0; x < MAP_CHIP_X; x++)
		{
			POS tmpPos = AddScroll({ MAP_CHIP_SIZE_X * x,MAP_CHIP_SIZE_Y * y });
			DrawGraph(
				tmpPos.x, tmpPos.y, enemyImg[enemySpawnData[y][x]], true);
		}
	}

	// Draw Edit Cursor Tile or Enemy
	POS mouseIndex = MapPosToIndex(AddScrollMouse(mousePos));
	POS tmpCursorPos = AddScroll(MapIndexToPos(mouseIndex));

	if (!editChange)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, abs((int)(animCnt % 512) - 256));
		DrawExtendGraph(
			tmpCursorPos.x, tmpCursorPos.y,
			tmpCursorPos.x + 64, tmpCursorPos.y + 64,
			tilesetImg[editChipId], true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	else
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, abs((int)(animCnt % 512) - 256));
		DrawGraph(tmpCursorPos.x, tmpCursorPos.y, enemyImg[editChipId], true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	// Draw Loop Line
	DrawLineS(0, 27 * MAP_CHIP_SIZE_Y, MAP_CHIP_X * MAP_CHIP_SIZE_X, 27 * MAP_CHIP_SIZE_Y, 0xFF0000);

	// Show Map ID
	DrawFormatString(0, 0, 0xff0000, "mapId = %d", mapId);
}

void EditCursorControl(void)
{
	int mouseWheel = GetMouseWheelRotVol();
	POS tmpPos = mousePos;

	if (mouseWheel > 0)
	{
		editChipId++;
		if (editChipId > editIdMax) editChipId = 0;
	}
	else if (mouseWheel < 0)
	{
		editChipId--;
		if (editChipId < 0) editChipId = editIdMax;
	}

	if (mousePos.x > 0 && mousePos.x < MAP_CHIP_X * MAP_CHIP_SIZE_X &&
		mousePos.y > 0 && mousePos.y < MAP_CHIP_Y * MAP_CHIP_SIZE_Y)
	{

		POS tmpPos = AddScrollMouse(mousePos);
		if (trgKey[MOUSE_LEFT]) SetChipID(tmpPos, editChipId);
		if (newKey[MOUSE_RIGHT]) DeleteChipID(tmpPos, -1);
		if (trgKey[MOUSE_MIDDLE]) CopyChipID(tmpPos);
	}

	auto ResetMapData = []()
	{
		for (int z = 0; z < LAYER_MAX; z++)
		{
			for (int y = 0; y < MAP_CHIP_Y; y++)
			{
				for (int x = 0; x < MAP_CHIP_X; x++)
				{
					mapData[z][y][x] = -1;
				}
			}
		}

		for (int y = 0; y < MAP_CHIP_Y; y++)
		{
			for (int x = 0; x < MAP_CHIP_X; x++)
			{
				enemySpawnData[y][x] = -1;
			}
		}
	};

	if (trgKey[KEY_F4])
	{
		MapSave(MAPDATA(mapId));
		EnemyGameInit();
		ResetMapData();
		mapId++;
		if (mapId >= MAPDATA_MAX) mapId = 0;
		MapLoad(MAPDATA(mapId));
	}

	if (trgKey[KEY_F3])
	{
		MapSave(MAPDATA(mapId));
		EnemyGameInit();
		ResetMapData();
		mapId--;
		if (mapId < 0) mapId = MAPDATA_MAX - 1;
		MapLoad(MAPDATA(mapId));
	}

	if (trgKey[KEY_R])
	{
		editChange = !editChange;
		editChipId = 0;
		if(editChange) editIdMax = ENEMY_ID_MAX - 1;
		else editIdMax = 88;
	}

	animCnt += 8;

	if (trgKey[KEY_F2]) GameModeChange(GAME_INIT);
}

void GetMouseData(void)
{
	GetMousePoint(&mousePos.x, &mousePos.y);
}

void SetChipID(POS pos, int id)
{
	if (!editChange) mapData[newKey[KEY_SHIFT]][pos.y / chipSize.y][pos.x / chipSize.x] = id;
	else 
	{
		enemySpawnData[pos.y / chipSize.y][pos.x / chipSize.x] = id;
		EnemyInit(enemySpawnData[pos.y / chipSize.y][pos.x / chipSize.x], { float(pos.x),float(pos.y) });
	}
}

void DeleteChipID(POS pos, int id)
{
	if (!editChange)mapData[newKey[KEY_SHIFT]][pos.y / chipSize.y][pos.x / chipSize.x] = id;
	else enemySpawnData[pos.y / chipSize.y][pos.x / chipSize.x] = id;
}

void CopyChipID(POS pos)
{
	if(!editChange) editChipId = mapData[newKey[KEY_SHIFT]][pos.y / chipSize.y][pos.x / chipSize.x];
	else editChipId = enemySpawnData[pos.y / chipSize.y][pos.x / chipSize.x];
}

void MapSave(MAPDATA data)
{
	char save[256];
	sprintf_s(save, "Save File To Mapdata%d", data);

	if (MessageBox(NULL, save, "File", MB_YESNO) == IDYES)
	{
		sprintf_s(save, "Mapdata/mapdata%d.dat", data);

		FILE *fp;
		fopen_s(&fp, save, "wb");
		if (fp == NULL)
		{
			return;
		}
		else
		{
			for (int z = 0; z < LAYER_MAX; z++)
			{
				for (int y = 0; y < MAP_CHIP_Y; y++)
				{
					for (int x = 0; x < MAP_CHIP_X; x++)
					{
						saveData.mapData[z][y][x] = mapData[z][y][x];
					}
				}
			}

			for (int y = 0; y < MAP_CHIP_Y; y++)
			{
				for (int x = 0; x < MAP_CHIP_X; x++)
				{
					saveData.enemySpawnData[y][x] = enemySpawnData[y][x];
				}
			}
			fwrite(&saveData, sizeof(SAVE_DATA), 1, fp);
			fclose(fp);
		}	
	}
}

void MapLoad(MAPDATA data)
{
	char load[256];
	sprintf_s(load, "Mapdata/mapdata%d.dat", data);

	FILE *fp;
	fopen_s(&fp, load, "rb");
	if (fp == NULL)
	{
		return;
	}
	else
	{
		fread(&saveData, sizeof(SAVE_DATA), 1, fp);
		for (int z = 0; z < LAYER_MAX; z++)
		{
			for (int y = 0; y < MAP_CHIP_Y; y++)
			{
				for (int x = 0; x < MAP_CHIP_X; x++)
				{
					mapData[z][y][x] = saveData.mapData[z][y][x];
				}
			}
		}

		for (int y = 0; y < MAP_CHIP_Y; y++)
		{
			for (int x = 0; x < MAP_CHIP_X; x++)
			{
				enemySpawnData[y][x] = saveData.enemySpawnData[y][x];
				if (enemySpawnData[y][x] != -1) EnemyInit(enemySpawnData[y][x], MapIndexToPosF({ x,y }));
			}
		}
		fclose(fp);

		LoadMapLoopData();
	}
}