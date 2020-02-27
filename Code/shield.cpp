#include "main.h"
#include "shield.h"
#include "enemy.h"
#include "stage.h"

int shieldImg[6];
SHIELD shield;

void ShieldSysInit(void)
{
	char tmpString[256];
	for (int i = 0; i < 6; i++)
	{
		sprintf_s(tmpString, "Image/Shield/character-shield-VFX_%03d.png", i + 1);
		shieldImg[i] = LoadGraph(tmpString);
	}
}

void ShieldGameInit(void)
{
	shield.hpMax = 0;
	shield.hp = shield.hpMax;
	shield.aniCnt = 0;
	shield.visible = false;
}

void ShieldControl(void)
{
	if (shield.visible)
	{
		shield.aniCnt++;
	}
	else shield.aniCnt = 0;
}

bool ShieldDamage(void)
{
	shield.hp--;
	if (shield.hp <= 0) return true;
	return false;
}

void ShieldDraw(void)
{
	if (shield.visible)
	{
		if (GetEnemyFlag(shield.index) == false) return;
		POS_F shieldPos = AddScrollF(GetEnemyPos(shield.index));
		POS shieldSize = GetEnemyImgSize2(shield.index);
		shieldSize.x *= 1.2;
		shieldSize.y *= 1.2;
		int tmpCnt = (shield.aniCnt / 5) % 6;
		int tmpBlend = (255.0f / shield.hpMax) * shield.hp;
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, tmpBlend);

		DrawExtendGraphF(
			shieldPos.x, shieldPos.y,
			shieldPos.x + shieldSize.x,
			shieldPos.y + shieldSize.y,
			shieldImg[tmpCnt], true);

		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}

void EffectShieldInit(int index,int hp)
{
	shield.hpMax = hp;
	shield.hp = shield.hpMax;
	shield.index = index;
	shield.visible = true;
}

void EffectShieldLoop(void)
{

}