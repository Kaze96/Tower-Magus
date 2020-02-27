#include "main.h"
#include "title.h"
#include "keycheck.h"
#include "background.h"
#include "enemy.h"
#include "fade.h"
#include "map_edit.h"
#include "mapdata.h"
#include "Character.h"

//-------------------------CUSTOMIZATION--------------------------------
#define TITLE_SIZE 1
#define TITLE_LOCATION_Y 250	//middle of graph
#define NUMBER_OF_BUTTON 4		//count from 1
#define BUTTON_LOCATION_Y 650	//middle of graph
#define BUTTON_SELECTION_DEFAULT 2
#define UNSELECTED_BUTTON_SIZE 1
#define SELECTED_BUTTON_SIZE 1.5
#define SCROLL_SIZE_Y 900	//image size of scroll (y)
//-------------------------CUSTOMIZATION--------------------------------

#ifdef _DEBUG
#define AST() {\
	CHAR ast_mes[256];\
	wsprintf(ast_mes, "%d %d", __FILE__, __LINE__);\
	MessageBox(0, ast_mes, "%d", MB_OK);\
	}
#else
#define AST()
#endif

int titleImg;
int titleScrollImg;
int titleScrollCnt;
int buttonImg[NUMBER_OF_BUTTON];
int buttonSelection;
int flashCnt;

void GameTitleSysInit(void)
{
	if ((titleImg = LoadGraph("image/Menu/title.png")) == -1) {
		AST();
	}
	if ((titleScrollImg = LoadGraph("image/Menu/titleback.png")) == -1) {
		AST();
	}

	char tmp[256];
	for (int i = 0; i < NUMBER_OF_BUTTON; i++)
	{
		sprintf_s(tmp, "Image/Menu/button%d.png", i);
		buttonImg[i] = LoadGraph(tmp);
	}
}

void GameTitleGameInit(void)
{
	buttonSelection = BUTTON_SELECTION_DEFAULT;	//Default = 2
	fadeFlag = FADE_DISABLED;
	flashCnt = 0;
}

void GameTitle(void)
{
	if(trgKey[KEY_F2]) GameModeChange(MAP_EDIT_INIT);

	if ((trgKey[KEY_LEFT] || trgButton[0][BUTTON_LEFT]) && fadeFlag == FADE_DISABLED)
	{
		if (buttonSelection > 2) buttonSelection--;
	}

	if ((trgKey[KEY_RIGHT] || trgButton[0][BUTTON_RIGHT]) && fadeFlag == FADE_DISABLED)
	{
		if (buttonSelection < NUMBER_OF_BUTTON - 1) buttonSelection++;
	}

	if (trgKey[KEY_Z] || trgButton[0][BUTTON_X] && buttonSelection == 2 && (playerCnt>0))
	{
		PlayerGameInit();
		FadeInit(50);
	}
	if (PadDetect()) playerCnt++;
	//scroll
	if (titleScrollCnt >= SCROLL_SIZE_Y)
	{
		titleScrollCnt = 0;
	}
	else
	{
		titleScrollCnt++;
	}

	BackgroundMain(0.01f, 1.0f, 0.2f);
	if (fadeFlag != FADE_DISABLED && (FadeLoop() == true && fadeFlag == FADE_OUT))
	{
		if (buttonSelection == 2)
		{
			MapLoad(MAPDATA_1P);
			GameModeChange(GAME_PLAY);
		}
	}
	GameTitleDraw();
}

void GameTitleDraw(void)
{
	POS tempImgSize;
	GetGraphSize(titleImg, &tempImgSize.x, &tempImgSize.y);

	tempImgSize.x *= TITLE_SIZE;
	tempImgSize.y *= TITLE_SIZE;

	//Background
	BackgroundDraw(true, false,false);

	//Draw Scroll
	DrawGraph(0, titleScrollCnt, titleScrollImg, true);
	DrawGraph(0, titleScrollCnt - SCROLL_SIZE_Y, titleScrollImg, true);

	//Draw Title
	DrawExtendGraph(SCREEN_SIZE_X / 2 - (tempImgSize.x / 2), TITLE_LOCATION_Y - (tempImgSize.y / 2), SCREEN_SIZE_X / 2 + (tempImgSize.x / 2), TITLE_LOCATION_Y + (tempImgSize.y/2), titleImg, true);

	//Draw Button
	POS buttonImgSize[NUMBER_OF_BUTTON];
	for (int i = 2; i < NUMBER_OF_BUTTON; i++)
	{
		//Get Button Size
		GetGraphSize(buttonImg[i], &buttonImgSize[i].x, &buttonImgSize[i].y);

		if (i == buttonSelection)
		{
			buttonImgSize[i].x *= SELECTED_BUTTON_SIZE;
			buttonImgSize[i].y *= SELECTED_BUTTON_SIZE;
		}
		else
		{
			buttonImgSize[i].x *= UNSELECTED_BUTTON_SIZE;
			buttonImgSize[i].y *= UNSELECTED_BUTTON_SIZE;
		}
	}

	for (int i = buttonSelection - 1; i >= 0; i--)
	{
		DrawExtendGraph
		(SCREEN_SIZE_X / 2 - buttonImgSize[buttonSelection].x / 2 - ((i + 1) * 20), BUTTON_LOCATION_Y - (buttonImgSize[i].y / 2),
			SCREEN_SIZE_X / 2 - buttonImgSize[buttonSelection].x / 2 - ((i + 1) * 20) + buttonImgSize[i].x, BUTTON_LOCATION_Y + (buttonImgSize[i].y / 2),
			buttonImg[i], true);
	}

	for (int i = NUMBER_OF_BUTTON; i >= buttonSelection + 1; i--)
	{
		DrawExtendGraph
		(SCREEN_SIZE_X / 2 + buttonImgSize[buttonSelection].x / 2 + (i * 10) - buttonImgSize[i].x, BUTTON_LOCATION_Y - (buttonImgSize[i].y / 2),
			SCREEN_SIZE_X / 2 + buttonImgSize[buttonSelection].x / 2 + (i * 10) , BUTTON_LOCATION_Y + (buttonImgSize[i].y / 2),
			buttonImg[i], true);
	}

	DrawExtendGraph
	(SCREEN_SIZE_X / 2 - buttonImgSize[buttonSelection].x / 2, BUTTON_LOCATION_Y - (buttonImgSize[buttonSelection].y / 2),
		SCREEN_SIZE_X / 2 + buttonImgSize[buttonSelection].x / 2, BUTTON_LOCATION_Y + (buttonImgSize[buttonSelection].y / 2),
		buttonImg[buttonSelection], true);
	
	// Draw Controller Join
	char joinChar[] = "PRESS START TO JOIN";
	int lengthTmp = GetStringLength(joinChar);
	int widthTmp = GetDrawExtendStringWidthToHandle(2, joinChar, lengthTmp, font);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, abs(flashCnt % 512 - 256));
	flashCnt+= 4;
	DrawExtendStringToHandle(SCREEN_SIZE_X/2 - widthTmp / 2, BUTTON_LOCATION_Y + 100,2,2,joinChar, 0x000000, font);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);

	for (int i = 0; i < playerCnt; i++)
	{
		DrawGraph((SCREEN_SIZE_X - buttonImgSize[buttonSelection].x) / 2 + 180 * i, BUTTON_LOCATION_Y - (buttonImgSize[buttonSelection].y / 2) - 40, GetPlayerImg(i), true);
	}
}