#pragma once

#define SCORE_MAX 100

typedef struct {
	POS_F pos;
	int hp;
	int tmpNum;
	int color;
	bool visible;
}SCORE;

void ScoreGameInit(void);
void ScoreControl(void);
void ScoreDraw(void);
void SetScoreText(POS_F pos, int num, int color, int frame);