#pragma once
#include "DxLib.h"

#define SCREEN_SIZE_X 1600
#define SCREEN_SIZE_Y 900
#define MAP_CHIP_SIZE_X 64
#define MAP_CHIP_SIZE_Y 64
#define MAP_CHIP_X 29
#define MAP_CHIP_Y 128
#define LAYER_MAX 2

#define PI 3.142
#define SCROLL_SPEED_PLUS 2.5f
#define SCROLL_SPEED_MAX 15.0f

#define INIT_VELOCITY 70
#define SECOND_PER_FRAME 0.2	//1ïbñàÉtÉåÅ[ÉÄ
#define ACG_G 10 //èdóÕ
#define SPEED 5.0f

enum GAME_MODE {
	GAME_INIT,
	MAP_EDIT_INIT,
	MAP_EDIT,
	GAME_PLAY,
	GAME_TITLE,
	GAME_END
};

typedef struct {
	int x, y;
}POS;

typedef struct {
	float x, y;
}POS_F;

typedef struct {
	int mapData[LAYER_MAX][MAP_CHIP_Y][MAP_CHIP_X];
	int enemySpawnData[MAP_CHIP_Y][MAP_CHIP_X];
}SAVE_DATA;

enum DIR {
	DIR_LEFT,
	DIR_RIGHT,
	DIR_DOWN,
	DIR_UP,
	DIR_MAX
};

void GameModeChange(GAME_MODE mode);

extern int gameCnt;
extern int font;
extern int jfont;