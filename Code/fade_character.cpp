#include "main.h"
#include "fade_character.h"
#include "stage.h"

#define CHARACTER_FADE_MAX 100

CHARACTER_FADE characterFade[CHARACTER_FADE_MAX];

void CharacterFadeInit(POS_F pos, int img, int RGB, DIR dir,int animLasting)
{
	for (int i = 0;i < CHARACTER_FADE_MAX;i++)
	{
		if (!characterFade[i].visible)
		{
			characterFade[i].pos = pos;		//座標
			characterFade[i].graph = img;
			characterFade[i].animLasting = animLasting;
			characterFade[i].filterRGB = RGB;			//レベル補正フィルタ
			characterFade[i].dir = dir;
			characterFade[i].visible = true;
			break;
		}
	}
}

void CharacterFadeLoop(void)
{
	for (int i = 0; i <= CHARACTER_FADE_MAX; i++)
	{
		if (characterFade[i].visible)
		{
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, characterFade[i].animLasting);
			POS_F tmpPos = AddScrollF(characterFade[i].pos);

			if (characterFade[i].dir == DIR_RIGHT)
			{
				DrawGraphF(tmpPos.x, tmpPos.y, characterFade[i].graph, true);
			}
			else if (characterFade[i].dir == DIR_LEFT)
			{
				DrawTurnGraphF(tmpPos.x, tmpPos.y, characterFade[i].graph, true);
			}
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			characterFade[i].animLasting--;
			if (characterFade[i].animLasting <= 0) characterFade[i].visible = false;
		}
	}
}