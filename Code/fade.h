#pragma once

enum FADE {
	FADE_DISABLED,
	FADE_IN,
	FADE_OUT
};

extern int fadeCount[2];	//0 = Countdown , 1 = TotalTime
extern FADE fadeFlag;

void FadeSysInit(void);
void FadeInit(int);
bool FadeLoop(void);