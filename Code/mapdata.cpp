#include "main.h"
#include "mapdata.h"
#include "stage.h"
#include "enemy.h"
#include "Character.h"
#include "sound.h"

int mapData[LAYER_MAX][MAP_CHIP_Y][MAP_CHIP_X];
int mapLoopData[LAYER_MAX][27][MAP_CHIP_X];
int enemySpawnData[MAP_CHIP_Y][MAP_CHIP_X];
int tilesetImg[88];
bool loopFlag;
int bossStage;

void MapDataSysInit(void)
{
	LoadDivGraph("Image/Tileset/Tileset.png", 88, 8, 11, 16, 16, tilesetImg);
}

void MapDataGameInit(void)
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

	for (int z = 0; z < LAYER_MAX; z++)
	{
		for (int y = 0; y < 27; y++)
		{
			for (int x = 0; x < MAP_CHIP_X; x++)
			{
				mapLoopData[z][y][x] = -1;
			}
		}
	}

	loopFlag = false;
	bossStage = 0;
}

void MapDataControl(void)
{
	// Change Map Data To Map Loop Data
	POS tmpScroll = AddScroll({ 0,0 });
	tmpScroll = { tmpScroll.x * -1,tmpScroll.y * -1 };
	if (tmpScroll.y <= 0)
	{
		MoveScrollToStart();
		MoveBossToStart();
		MoveAllPlayerToStart();
		if (!loopFlag)
		{
			for (int z = 0; z < LAYER_MAX; z++)
			{
				for (int y = 0; y < MAP_CHIP_Y; y++)
				{
					for (int x = 0; x < MAP_CHIP_X; x++)
					{
						mapData[z][y][x] = mapLoopData[z][y % 27][x];
					}
				}
			}
			EnemyGameInit();
			scrollPerFrame = BOSS_SCROLL_SPEED;
			PlaySoundType(SOUND_TYPE_BOSS);
			EnemyInit(DRAGON_2, {(800.0f - tmpScroll.x),(MAP_CHIP_Y * MAP_CHIP_SIZE_Y - SCREEN_SIZE_Y) - (20 * MAP_CHIP_SIZE_Y - SCREEN_SIZE_Y) });
			PlaySoundType(SOUND_TYPE_BOSSEN1);
			StopSoundType(SOUND_TYPE_BEBOSS);
			loopFlag = true;
		}
	}

	if (bossStage != 2)
	{
		// Stop Stage Music
		if (tmpScroll.y <= 20 * MAP_CHIP_SIZE_Y && bossStage == 0)
		{
			StopSoundType(SOUND_TYPE_STAGE);
			bossStage = 1;
		}
		if (tmpScroll.y <= 8 * MAP_CHIP_SIZE_Y && bossStage == 1)
		{
			PlaySoundType(SOUND_TYPE_BEBOSS);
			bossStage = 2;
		}
	}
}
	

void MapDataDraw(void)
{
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
}

POS MapPosToIndex(POS pos)
{
	POS mapIndex;

	mapIndex = { pos.x / MAP_CHIP_SIZE_X,pos.y / MAP_CHIP_SIZE_Y };

	return mapIndex;
}

POS MapPosToIndex(POS_F pos)
{
	POS_F mapIndex;

	mapIndex = { pos.x / MAP_CHIP_SIZE_X,pos.y / MAP_CHIP_SIZE_Y };

	return POS{ int(mapIndex.x),int(mapIndex.y) };
}

POS MapIndexToPos(POS index)
{
	POS mapPos;

	mapPos = { index.x * MAP_CHIP_SIZE_X,index.y * MAP_CHIP_SIZE_Y };

	return mapPos;
}

POS_F MapIndexToPosF(POS index)
{
	POS mapPos;

	mapPos = { index.x * MAP_CHIP_SIZE_X,index.y * MAP_CHIP_SIZE_Y };

	return POS_F{ float(mapPos.x),float(mapPos.y) };
}

void LoadMapLoopData(void)
{
	for (int z = 0; z < LAYER_MAX; z++)
	{
		for (int y = 0; y < 27; y++)
		{
			for (int x = 0; x < MAP_CHIP_X; x++)
			{
				mapLoopData[z][y][x] = mapData[z][y][x];
			}
		}
	}
}