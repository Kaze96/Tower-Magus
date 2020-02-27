#include "main.h"
#include "background.h"
#include "stage.h"
#include "Character.h"

int bgImg;
int endImg;
int cloudImg[6];
int mountainImg[3];
int sampleImg;
int castleImg;

POS_F cloudPos[6];
POS_F castlePos;
POS_F castleVelocity;

void BackgroundSysInit(void)
{
	bgImg = LoadGraph("Image/Mountain/background.png");
	endImg = LoadGraph("Image/Ending/ending.png");

	char str[256];
	for (int i = 0; i < CLOUD_IMG_MAX; i++)
	{
		sprintf_s(str, "Image/Mountain/cloud %d.png", i + 1);
		cloudImg[i] = LoadGraph(str);
	}

	for (int i = 0; i < MOUNTAIN_IMG_MAX; i++)
	{
		sprintf_s(str, "Image/Mountain/mountains %d.png", i + 1);
		mountainImg[i] = LoadGraph(str);
	}

	castleImg = LoadGraph("Image/Castle/castle.png");
	sampleImg = LoadGraph("Image/Mountain/sample.png");
}

void BackgroundInit(void)
{
	for (int i = 0; i < CLOUD_IMG_MAX; i++)
	{
		cloudPos[i] = { 0.0f,0.0f };
	}

	castlePos = { 0.0f,0.0f };
	castleVelocity = { 0,0 };
}

void BackgroundMain(float castleVelocityIncr, float cloudPositionDesc1, float cloudPositionDesc2)
{
	cloudPos[0].x -= cloudPositionDesc1;

	for (int i = 1; i < CLOUD_IMG_MAX; i++)
	{
		cloudPos[i].x -= cloudPositionDesc2;
	}
	
	for (int i = 0; i < CLOUD_IMG_MAX; i++)
	{
		if (cloudPos[i].x <= -1920.0f) cloudPos[i].x = 0.0f;
	}

	// Return Castle Position
	if (scrollFlag)
	{
		float tmpVelocity = GetScrollVelocity();
		int tmpScrollPerFrame = scrollPerFrame;
		POS_F screenPos = AddScrollF(GetAllPlayerAveragePos());

		auto moveScrollPerFrame = [&](int frame)
		{
			if ((gameCnt % frame) == 0) castlePos.y += (tmpVelocity * 1.5);
		};

		if (scrollPerFrame != BOSS_SCROLL_SPEED)
		{
			if (screenPos.y <= MAP_CHIP_SIZE_Y * 2)
			{
				tmpScrollPerFrame = 5;
			}
			else if (screenPos.y > MAP_CHIP_SIZE_Y * 2 && screenPos.y < MAP_CHIP_SIZE_Y * 8)
			{
				tmpScrollPerFrame = 10;
			}
		}
		moveScrollPerFrame(tmpScrollPerFrame);

		if (castlePos.y >= 1080.0f - tmpVelocity)		// Return to original position
		{
			castlePos.y = 0.0f;
		}
	}
}

void BackgroundDraw(bool cloudFlag, bool castleFlag,bool endFlag)
{
	DrawGraph(0, 0, bgImg, true);

	for (int i = 0; i < MOUNTAIN_IMG_MAX; i++)
	{
		DrawGraph(0, 0, mountainImg[i], true);
		DrawGraph(0, -1080, mountainImg[i], true);
	}

	if (endFlag)
	{
		DrawGraph(0, 0, endImg, true);
	}
	if (cloudFlag)
	{
		for (int i = 0; i < CLOUD_IMG_MAX; i++)
		{
			DrawGraphF(cloudPos[i].x, 0.0f, cloudImg[i], true);
			DrawGraphF(cloudPos[i].x + 1920.0f, 0.0f, cloudImg[i], true);
		}
	}

	if (!castleFlag) return;
	
	POS_F tmpCastlePos = AddScrollF(castlePos);

	DrawGraphF(tmpCastlePos.x, castlePos.y, castleImg, true);
	DrawGraphF(tmpCastlePos.x, castlePos.y - 1080.0f, castleImg, true);
}