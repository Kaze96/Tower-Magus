#include "main.h"
#include "tutorial.h"
#include "stage.h"

#define TEXT_MAX 10

int tutorialImg[4];
TEXT text[TEXT_MAX];
bool textFlag[10];

void TutorialSysInit(void)
{
	tutorialImg[0] = LoadGraph("Image/Xbox/Button A.png");
	tutorialImg[1] = LoadGraph("Image/Xbox/Button B.png");
	tutorialImg[2] = LoadGraph("Image/Xbox/Direction.png");
	tutorialImg[3] = LoadGraph("Image/Xbox/Joystick L.png");
}

void TutorialGameInit(void)
{
	for (int i = 0; i < TEXT_MAX; i++)
	{
		text[i].pos = { 0.0f,0.0f };
		text[i].tmpText = "";
		text[i].hp = 0;
		text[i].color = 0;
		text[i].type = BUTTON_NONE;
		text[i].visible = false;
	}
}

void TutorialControl(void)
{
	for (int i = 0; i < TEXT_MAX; i++)
	{
		if (text[i].visible)
		{
			text[i].hp += 4;
			if (text[i].hp >= 255) text[i].hp = 255;
		}
		else
		{
			text[i].hp -= 4;
			if (text[i].hp <= 0) text[i].hp = 0;
		}

		POS_F screenPos = AddScrollF(text[i].pos);
		if (screenPos.y >= SCREEN_SIZE_Y - MAP_CHIP_SIZE_Y * 3) text[i].visible = false;
	}
}

void TutorialDraw(void)
{
	for (int i = 0; i < TEXT_MAX; i++)
	{
		if (text[i].visible)
		{
			POS_F tmpPos = AddScrollF(text[i].pos);

			SetDrawBlendMode(DX_BLENDMODE_ALPHA, text[i].hp);
			DrawStringToHandle(tmpPos.x + 70, tmpPos.y - 4, text[i].tmpText, text[i].color, jfont);

			if (text[i].type != BUTTON_NONE)
			{
				DrawGraph(tmpPos.x, tmpPos.y, tutorialImg[text[i].type], true);
			}
		}
	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void SetText(POS_F pos, const char *string, int color, BUTTON_IMG type, int flag)
{
	if (textFlag[flag])
	{
		return;
	}
	else
	{
		textFlag[flag] = true;
	}
	for (int i = 0; i < TEXT_MAX; i++)
	{
		if (!text[i].visible)
		{
			text[i].pos = pos;
			text[i].tmpText = string;
			text[i].color = color;
			text[i].hp = 0;
			text[i].type = type;
			text[i].visible = true;
			break;
		}
	}
}