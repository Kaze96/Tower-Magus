#pragma once

#define BOSS_SCROLL_SPEED 5

void StageInit(void);
void StageMain(void);

void DrawGraphS(int x, int y, int GrHandle, int TransFlag);
void DrawLineS(int x1, int y1, int x2, int y2, unsigned int Color, int Thickness = 1);
POS AddScroll(POS pos);
POS_F AddScrollF(POS_F pos);
POS_F MinusScrollF(POS_F pos);
POS AddScrollMouse(POS pos);
POS_F AddScrollMouseF(POS_F pos);
void StageTiltInit(int str,int length);
void StageTilt(void);
float GetScrollVelocity(void);
bool InScreen(POS_F pos);
void MoveScrollToStart(void);

extern bool scrollFlag;
extern int scrollPerFrame;