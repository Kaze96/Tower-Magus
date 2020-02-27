#include "main.h"
#include "fade.h"
#include "background.h"
#include "map_edit.h"
#include "character.h"
#include "stage.h"
#include "keycheck.h"
#include "sound.h"
#include "ending.h"
#include <math.h>
#include "mapdata.h"

#define ENDSCENE_X SCREEN_SIZE_X / MAP_CHIP_SIZE_X
#define ENDSCENE_Y SCREEN_SIZE_Y / MAP_CHIP_SIZE_Y
#define RANKING_MAX 10

#define CLOUD_MAX 3
#define ROCK_MAX 5

POS endImgPos;
bool endFlag;
int endTime;


int endCloudImg[CLOUD_MAX];
int endRockImg[ROCK_MAX];
float endRockAngle[ROCK_MAX];

POS_F endCloudPos[CLOUD_MAX];
POS_F endRockPos[ROCK_MAX];

void EndCloudMove(float speed1, float speed2, float speed3);
void EndRockMove(float waveSpeed1, float waveSpeed2, float waveSpeed3, float waveSpeed4, float waveSpeed5);

void EndingSysInit(void)
{
	char tmpString[256];
	for (int i = 0; i < CLOUD_MAX; i++)
	{
		sprintf_s(tmpString, "Image/Ending/cloud %d.png", i + 1);
		endCloudImg[i] = LoadGraph(tmpString);
	}

	for (int i = 0; i < ROCK_MAX; i++)
	{
		sprintf_s(tmpString, "Image/Ending/rock %d.png", i + 1);
		endRockImg[i] = LoadGraph(tmpString);
	}
}

void EndingInit(void)
{
	endImgPos = { SCREEN_SIZE_X / 2, -200 };
	endFlag = true;
	endTime = gameCnt;
	StageInit();
	EndingLoad();

	for (int i = 0; i < CLOUD_MAX; i++) endCloudPos[i] = { 0.0f,0.0f };
	for (int i = 0; i < ROCK_MAX; i++)
	{
		endRockPos[i] = { 0.0f,0.0f };
		endRockAngle[i] = rand() % 360;
	}

	GameModeChange(GAME_END);
}

void EndingMain(void)
{
	PlaySoundType(SOUND_TYPE_ENDING);
	BackgroundMain(SCROLL_SPEED_PLUS, 1.20f, 0.40f);
	PlayerControl();

	FadeLoop();
	EndingDraw();
	if (endImgPos.y < SCREEN_SIZE_Y / 3 )
	{
		endImgPos.y += 4;
	}
	if (trgKey[KEY_Z])
	{
		endFlag = false;
		StopSoundType(SOUND_TYPE_ENDING);
		GameModeChange(GAME_INIT);
	}

	EndCloudMove(-0.2f, -0.4f, -0.8f);
	EndRockMove(5.0f, 10.0f, 20.0f, 35.0f, 50.0f);
	for (int i = 0; i < ROCK_MAX; i++) endRockAngle[i]++;
}

void EndingDraw(void)
{
	
	BackgroundDraw(false, false, true);

	for (int i = 0; i < CLOUD_MAX; i++)
	{
		DrawGraph(endCloudPos[i].x, endCloudPos[i].y, endCloudImg[i], true);
		DrawGraph(endCloudPos[i].x + 1921.0f, endCloudPos[i].y, endCloudImg[i], true);
	}

	for (int i = 0; i < ROCK_MAX; i++)
	{
		DrawGraph(endRockPos[i].x, endRockPos[i].y, endRockImg[i], true);
		DrawGraph(endRockPos[i].x + 1920.0f, endRockPos[i].y, endRockImg[i], true);
	}

	// Draw Tileset
	for (int y = 0; y < ENDSCENE_Y + 1; y++)
	{
		for (int x = 0; x < ENDSCENE_X; x++)
		{
			POS tmpPos = AddScroll({ MAP_CHIP_SIZE_X * x, MAP_CHIP_SIZE_Y * y });
			if (x <= ENDSCENE_X / 2 || y > ENDSCENE_Y - 2)
			{
			DrawExtendGraph(
				tmpPos.x, tmpPos.y,
				tmpPos.x + 64, tmpPos.y + 64,
				tilesetImg[mapData[0][y][x]], true);
			}
			else
			{
				//Å@îΩì]Ç∑ÇÈ
				DrawExtendGraph(
					tmpPos.x + 64, tmpPos.y ,
					tmpPos.x , tmpPos.y + 64,
					tilesetImg[mapData[0][y][x]], true);
			}
		}
	}
	PlayerDraw();
	char endString[] = "Game Clear";
	int endStringLength = GetStringLength(endString);
	int endStringWidth = GetDrawExtendStringWidthToHandle(2,endString,endStringLength,font);
	DrawExtendStringToHandle(endImgPos.x - endStringWidth / 2, endImgPos.y,2,2, endString, 0xffffff,font);
}

void EndingLoad(void)
{
	char load[] = "Mapdata/mapdata3.dat";
	char rank[] = "Mapdata/rankdata1.dat";

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
			for (int y = 0; y < ENDSCENE_Y + 1; y++)
			{
				for (int x = 0; x < ENDSCENE_X; x++)
				{
					mapData[z][y][x] = -1;
					mapData[0][y][x] = saveData.mapData[0][y][x];
				}
			}
		}
		fclose(fp);
	}
}



void EndCloudMove(float speed1,float speed2,float speed3)
{
	endCloudPos[0].x += speed1;
	endCloudPos[1].x += speed2;
	endCloudPos[2].x += speed3;
	for (int i = 0; i < CLOUD_MAX; i++)
	{
		if (endCloudPos[i].x <= -1921.0f) endCloudPos[i].x = 0.0f;
	}
}

void EndRockMove(float waveSpeed1,float waveSpeed2,float waveSpeed3,float waveSpeed4,float waveSpeed5)
{
	endRockPos[0].y += (waveSpeed1 / 60.0f) * sin(endRockAngle[1] * PI / 180);
	endRockPos[1].y += (waveSpeed2 / 60.0f) * sin(endRockAngle[1] * PI / 180);
	endRockPos[2].y += (waveSpeed3 / 60.0f) * cos(endRockAngle[1] * PI / 180);
	endRockPos[3].y += (waveSpeed4 / 60.0f) * cos(endRockAngle[1] * PI / 180);
	endRockPos[4].y += (waveSpeed5 / 60.0f) * sin(endRockAngle[4] * PI / 180);
}