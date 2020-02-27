#include "main.h"
#include "fade.h"

int fadeCount[2];	//0 = Countdown , 1 = TotalTime
FADE fadeFlag;

void FadeSysInit(void)
{
	fadeFlag = FADE_DISABLED;
}

//Fade In Init (Fade Time)
void FadeInit(int time)
{
	if (fadeFlag == true)
	{
		return;
	}
	else
	{
		fadeFlag = FADE_IN;
	}

	fadeCount[0] = 255;
	fadeCount[1] = time;
}

bool FadeLoop(void)
{
	if (fadeFlag == FADE_IN) fadeCount[0] -= (255 / fadeCount[1]);
	if (fadeFlag == FADE_OUT) fadeCount[0] += (255 / fadeCount[1]);

	SetDrawBright(fadeCount[0], fadeCount[0], fadeCount[0]);
	if (fadeCount[0] <= 0 && fadeFlag == FADE_IN)
	{
		fadeFlag = FADE_OUT;
		return true;
	}

	if (fadeCount[0] >= 255 && fadeFlag == FADE_OUT)
	{
		fadeFlag = FADE_DISABLED;
		return true;
	}

	return false;
}
