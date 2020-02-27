#include "main.h"
#include "Score.h"
#include "stage.h"

SCORE score[SCORE_MAX];

void ScoreGameInit(void)
{
	for (int i = 0; i < SCORE_MAX; i++)
	{
		score[i].pos = { 0.0f,0.0f };
		score[i].tmpNum = 0;
		score[i].hp = 0;
		score[i].color = 0;
		score[i].visible = false;
	}
}

void ScoreControl(void)
{
	for (int i = 0; i < SCORE_MAX; i++)
	{
		if (score[i].visible)
		{
			score[i].pos.y--;
			score[i].hp--;
			if (score[i].hp <= 0) score[i].visible = false;
		}
	}
}

void ScoreDraw(void)
{
	for (int i = 0; i < SCORE_MAX; i++)
	{
		if (score[i].visible)
		{
			POS_F tmpPos = AddScrollF(score[i].pos);
			int length = GetDrawFormatStringWidthToHandle(font, "%d", score[i].tmpNum);
			float blendCnt = 255 / 60;

			SetDrawBlendMode(DX_BLENDMODE_ALPHA, blendCnt * score[i].hp);
			DrawFormatStringToHandle(tmpPos.x - length / 2, tmpPos.y, score[i].color, font, "%d", score[i].tmpNum);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
	}
}

void SetScoreText(POS_F pos, int num, int color,int frame)
{
	for (int i = 0; i < SCORE_MAX; i++)
	{
		if (!score[i].visible)
		{
			score[i].pos = pos;
			score[i].tmpNum = num;
			score[i].hp = frame;
			score[i].color = color;
			score[i].visible = true;
			break;
		}
	}
}