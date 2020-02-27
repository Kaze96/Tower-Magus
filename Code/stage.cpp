#include "main.h"
#include "stage.h"
#include "keycheck.h"
#include "Character.h"
#include "ending.h"
#include "sound.h"
#include "fade.h"
#include "tutorial.h"

int scrollSpeed;
POS_F scrollVelocity;
POS_F scroll;
int tiltLen;
int strength;
POS tilt;
bool scrollFlag;
int endCnt;

bool editFlag;
int scrollPerFrame;

void StageInit(void)
{
	scrollSpeed = 5;
	scroll = { 0.0f ,MAP_CHIP_Y * MAP_CHIP_SIZE_Y - SCREEN_SIZE_Y};
	if (endFlag) scroll = { 0.0f,0.0f };
	tiltLen = 0;
	strength = 0;
	tilt.x = 0;
	tilt.y = 0;
	scrollVelocity = { 0.0f,5.0f };
	scrollFlag = false;
	endCnt = 0;

	editFlag = false;
	scrollPerFrame = 30;
}

void StageMain(void)
{
	int tiltnum = 20;
	int dura = 14;
	int tmp = 5;

	if (trgKey[KEY_W] == true)
	{
		StageTiltInit(tiltnum, dura);
	}
	if (trgKey[KEY_D] == true)
	{
		StageTiltInit(tmp, dura);
	}
	StageTilt();

	//if (editFlag)
	//{
	//	if (newKey[KEY_SHIFT]) scrollSpeed = 20;
	//	else scrollSpeed = 5;
	//	if (newKey[KEY_UP]) scroll.y -= scrollSpeed;
	//	if (newKey[KEY_RIGHT]) scroll.x += scrollSpeed;
	//	if (newKey[KEY_DOWN]) scroll.y += scrollSpeed;
	//	if (newKey[KEY_LEFT]) scroll.x -= scrollSpeed;
	//}

	if (scroll.y <= 0.0f)
	{
		scroll.y = 0.0f;
	}

	if (scroll.y >= MAP_CHIP_Y * MAP_CHIP_SIZE_Y - SCREEN_SIZE_Y)
	{
		scroll.y = MAP_CHIP_Y * MAP_CHIP_SIZE_Y - SCREEN_SIZE_Y;
	}

	if (scroll.x <= MAP_CHIP_SIZE_X / 3)
	{
		scroll.x = MAP_CHIP_SIZE_X / 3;
	}

	if (scroll.x >= MAP_CHIP_X * MAP_CHIP_SIZE_X - SCREEN_SIZE_X - MAP_CHIP_SIZE_X / 3)
	{
		scroll.x = MAP_CHIP_X * MAP_CHIP_SIZE_X - SCREEN_SIZE_X - MAP_CHIP_SIZE_X / 3;
	}

	/*if (scroll.y == 7292)
	{
		SetText({SCREEN_SIZE_X / 4 + scroll.x + 150,  7700}, "ƒCƒhƒE", 0xffffff, DPAD, 0);
		SetText({ SCREEN_SIZE_X / 4 + scroll.x  + 50,  7700 }, "/", 0xffffff, JOYSTICK, 1);
	}*/

	DrawString(200, scroll.y - 100, "test", 0xffffff);

	POS_F screenPos = AddScrollF(GetAllPlayerAveragePos());
	
	if (!editFlag)
	{
		if (screenPos.x >= SCREEN_SIZE_X / 2 + 100) scroll.x += scrollSpeed;
		else if (screenPos.x <= SCREEN_SIZE_X / 2 - 100) scroll.x -= scrollSpeed;
		
		if (scrollFlag)
		{
			int tmpPerFrame = scrollPerFrame;

			auto moveScrollPerFrame = [&](int frame)
			{
				if ((gameCnt % frame) == 0) scroll.y -= scrollVelocity.y;
			};

			if (scrollPerFrame != BOSS_SCROLL_SPEED)
			{
				if (screenPos.y <= MAP_CHIP_SIZE_Y * 2)
				{
					tmpPerFrame = 5;
				}
				else if (screenPos.y > MAP_CHIP_SIZE_Y * 2 && screenPos.y < MAP_CHIP_SIZE_Y * 8)
				{
					tmpPerFrame = 10;
				}
			}
			moveScrollPerFrame(tmpPerFrame);
		}

		if (!scrollFlag)
		{
			if ((screenPos.y <= MAP_CHIP_SIZE_Y * 3) || ((gameCnt % 540) == 0))
			{
				scrollFlag = true;
				PlaySoundType(SOUND_TYPE_STAGE);
			}
		}
	}

	if (endFlag)
	{
		scroll.y -= 16;
	}
	if (trgKey[KEY_TAB]) editFlag = !editFlag; // ƒ}ƒbƒv•ÒWƒ‚[ƒh‚Ì•ÏŠ·
}

void DrawGraphS(int x,int y,int GrHandle,int TransFlag)
{
	DrawGraph(x - scroll.x, y - scroll.y, GrHandle, TransFlag);
}

void DrawLineS(int x1, int y1, int x2, int y2, unsigned int Color,int Thickness)
{
	DrawLine(x1 - scroll.x, y1 - scroll.y, x2 - scroll.x, y2 - scroll.y, Color, Thickness);
}

POS AddScroll(POS pos)
{
	pos.x = pos.x - scroll.x;
	pos.y = pos.y - scroll.y;

	return pos;
}

POS_F AddScrollF(POS_F pos)
{
	pos.x = pos.x - scroll.x;
	pos.y = pos.y - scroll.y;

	return pos;
}

POS_F MinusScrollF(POS_F pos)
{
	pos.x = pos.x + scroll.x;
	pos.y = pos.y + scroll.y;

	return pos;
}

POS AddScrollMouse(POS pos)
{
	pos.x = pos.x + scroll.x;
	pos.y = pos.y + scroll.y;

	return pos;
}

POS_F AddScrollMouseF(POS_F pos)
{
	pos.x = pos.x + scroll.x;
	pos.y = pos.y + scroll.y;

	return pos;
}


void StageTiltInit(int str, int length)
{
	tiltLen += length;
	strength = str;

	int vibratePow = 600;
	//U“®‚³‚¹‚éŽžŠÔ‚Íƒ~ƒŠ•b’PˆÊ‚È‚Ì‚Å‚P‚O‚O‚O‚ð‚©‚¯‚é
	int milliSecToSec = 1000;
	StartJoypadVibration(DX_INPUT_PAD1, vibratePow, (length * milliSecToSec));
	StartJoypadVibration(DX_INPUT_PAD2, vibratePow, (length * milliSecToSec));
	StartJoypadVibration(DX_INPUT_PAD3, vibratePow, (length * milliSecToSec));
	StartJoypadVibration(DX_INPUT_PAD4, vibratePow, (length * milliSecToSec));


	while (tiltLen % 4 > 0)
	{
		tiltLen++;
	}
}

void StageTilt()
{
	if (tiltLen == 0)
	{
		return;
	}
	
	switch (tiltLen % 4)
	{
	case 0:
		scroll.x += strength;
		tilt.x += strength;
		break;
	case 1:
		scroll.y -= strength;
		tilt.y -= strength;
		break;
	case 2:
		scroll.x -= strength;
		tilt.x -= strength;
		break;
	case 3:
		scroll.y += strength;
		tilt.y += strength;
		break;
	}

	tiltLen--;

	if (tiltLen == 0)
	{
		strength = 0;
		scroll.x -= tilt.x;
		scroll.y -= tilt.y;
		tilt.x = 0;
		tilt.y = 0;
	}

}

float GetScrollVelocity(void)
{
	return scrollVelocity.y;
}

bool InScreen(POS_F pos)
{
	POS_F screenPos = AddScrollF(pos);

	if (screenPos.y >= 0 && screenPos.y < SCREEN_SIZE_Y)
	{
		return true;
	}
	return false;
}

void MoveScrollToStart(void)
{
	scroll.y += (MAP_CHIP_Y * MAP_CHIP_SIZE_Y - SCREEN_SIZE_Y) - (20 * MAP_CHIP_SIZE_Y - SCREEN_SIZE_Y);
}
