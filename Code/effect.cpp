#include "main.h"
#include "effect.h"
#include "stage.h"
#include "enemy.h"
#include <math.h>
#include "keycheck.h"
#include "projectile.h"
#include "blockcheck.h"
#include "Character.h"

EFFECT effect[EFFECT_MAX];
int reviveEffectImg[119];

void ExplosionSpawnInit(POS_F pos, EFFECT_TYPE type);
void ExplosionControl(int index);
void ExplosionDraw(POS_F pos, int index);
void ProjSpawnInit(POS_F pos, EFFECT_TYPE type);
void ProjControl(int index);
void ProjEffDraw(POS_F pos, int index);
void JPSpawnInit(POS_F pos, EFFECT_TYPE type);
void JPControl(int index);
void JPDraw(POS_F pos, int index);
void HESpawnInit(POS_F pos, EFFECT_TYPE type);
void HEControl(int index);
void HEDraw(POS_F pos, int index);
void ReviveSpawnInit(POS_F pos, EFFECT_TYPE type);
void ReviveControl(int index);
void ReviveDraw(POS_F pos, int index);
void TestSpawnInit(POS_F pos, EFFECT_TYPE type);
void TestControl(int index);
void TestDraw(POS_F pos, int index);
void DeleteEffect(int index);

void EffectSysInit(void)
{
	char tmpString[256];
	for (int i = 0; i < 119; i++)
	{
		sprintf_s(tmpString, "Image/Effect/Revive/frame%04d.png", i);
		reviveEffectImg[i] = LoadGraph(tmpString);
	}
}

void EffectGameInit(void)
{
	// Effectの初期化
	for (int i = 0; i < EFFECT_MAX; i++)
	{
		effect[i].pos = { 0.0f,0.0f };
		effect[i].speed = 0;
		effect[i].angle = 0.0f;
		effect[i].movPos = { 0.0f,0.0f };
		effect[i].type = EFFECT_TYPE_NONE;
		effect[i].tmpID = 0;
		effect[i].hpMax = 0;
		effect[i].hp = effect[i].hpMax;
		effect[i].rgbFilter = 0;
		effect[i].velocity = { 0.0f,0.0f };
		effect[i].aniCnt = 0;
		effect[i].visible = false;
	}
}

void EffectControl(void)
{
	for (int i = 0; i < EFFECT_MAX; i++)
	{
		if (effect[i].visible)
		{
			switch (effect[i].type)
			{
			case EFFECT_TYPE_NONE:
				break;
			case EFFECT_TYPE_EXPLOSION:
				ExplosionControl(i);
				break;
			case EFFECT_TYPE_PROJECTILE:
				ProjControl(i);
				break;
			case EFFECT_TYPE_JUMP:
				JPControl(i);
				break;
			case EFFECT_TYPE_HIT:
				HEControl(i);
				break;
			case EFFECT_TYPE_REVIVE:
				ReviveControl(i);
				break;
			case EFFECT_TYPE_TEST:
				TestControl(i);
				break;
			default:
				break;
			}

			DeleteEffect(i);

			effect[i].hp--;
		}
	}

	if (trgKey[KEY_C])
	{
		//SetEffect(GetEnemyPos(), EFFECT_TYPE_EXPLOSION);
	}
}

void EffectDraw(void)
{
	for (int i = 0; i < EFFECT_MAX; i++)
	{
		if (effect[i].visible)
		{
			float blendCnt = 255.0f / effect[i].hpMax;
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, blendCnt * effect[i].hp);

			POS_F tmpPos = AddScrollF(effect[i].pos);

			switch (effect[i].type)
			{
			case EFFECT_TYPE_NONE:
				break;
			case EFFECT_TYPE_EXPLOSION:
				ExplosionDraw(tmpPos,i);
				break;
			case EFFECT_TYPE_PROJECTILE:
				ProjEffDraw(tmpPos,i);
				break;
			case EFFECT_TYPE_JUMP:
				JPDraw(tmpPos, i);
				break;
			case EFFECT_TYPE_HIT:
				HEDraw(tmpPos, i);
				break;
			case EFFECT_TYPE_REVIVE:
				ReviveDraw(tmpPos, i);
				break;
			default:
				break;
			}

			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

			if (effect[i].type == EFFECT_TYPE_TEST) TestDraw(tmpPos, i);
		}
	}
}

void SetEffect(POS_F pos, EFFECT_TYPE type)
{
	switch (type)
	{
	case EFFECT_TYPE_NONE:
		break;
	case EFFECT_TYPE_EXPLOSION:
		ExplosionSpawnInit(pos, type);
		break;
	case EFFECT_TYPE_PROJECTILE:
		ProjSpawnInit(pos, type);
		break;
	case EFFECT_TYPE_JUMP:
		JPSpawnInit(pos, type);
		break;
	case EFFECT_TYPE_HIT:
		HESpawnInit(pos, type);
		break;
	case EFFECT_TYPE_REVIVE:
		ReviveSpawnInit(pos, type);
		break;
	case EFFECT_TYPE_TEST:
		TestSpawnInit(pos, type);
		break;
	default:
		break;
	}
}

// Enemy Particle
void ExplosionSpawnInit(POS_F pos, EFFECT_TYPE type)
{
	int tmpCnt = 0;

	for (int i = 0; i < EFFECT_MAX; i++)
	{
		if (!effect[i].visible)
		{
			effect[i].pos = pos;
			effect[i].speed = 15;
			effect[i].angle = (GetRand(360.0f) * PI / 180.0f);
			effect[i].movPos.x = effect[i].speed * cos(effect[i].angle);
			effect[i].movPos.y = effect[i].speed * sin(effect[i].angle);
			effect[i].type = type;
			effect[i].tmpID = tmpCnt;
			effect[i].hpMax = 60;
			effect[i].hp = effect[i].hpMax;
			effect[i].rgbFilter = 0;
			effect[i].velocity = { 0.0f,0.0f };
			effect[i].visible = true;
			effect[i].img = GetEnemyImg();
			effect[i].size = GetEnemyImgSize();
			if (effect[i].visible) tmpCnt++;
		}
		if (tmpCnt >= EXPLOSION_DIV_LINE * EXPLOSION_DIV_LINE) break;
	}
}

void ExplosionControl(int index)
{
	effect[index].pos.x += effect[index].movPos.x;
	effect[index].pos.y += effect[index].movPos.y;
}

void ExplosionDraw(POS_F pos, int index)
{
	// 敵の画像のチップサイズを取る
	POS tmpSize = { effect[index].size.x / EXPLOSION_DIV_LINE ,effect[index].size.y / EXPLOSION_DIV_LINE };

	// チップのIDを決める
	POS tmpIndex = { effect[index].tmpID % EXPLOSION_DIV_LINE,effect[index].tmpID / EXPLOSION_DIV_LINE };

	DrawRectRotaGraphF(
		pos.x + tmpIndex.x * tmpSize.x,
		pos.y + tmpIndex.y * tmpSize.y,
		tmpSize.x * tmpIndex.x,
		tmpSize.y * tmpIndex.y,
		tmpSize.x,
		tmpSize.y,
		1.0, GetRand(360) / PI * 180, effect[index].img, true, false);
}

// Projectile Particle
void ProjSpawnInit(POS_F pos, EFFECT_TYPE type)
{
	int tmpCnt = 0;

	for (int i = 0; i < EFFECT_MAX; i++)
	{
		if (!effect[i].visible)
		{
			effect[i].pos = pos;
			effect[i].speed = 5;
			effect[i].angle = (GetRand(360.0f) * PI / 180.0f);
			effect[i].movPos.x = effect[i].speed * cos(effect[i].angle);
			effect[i].movPos.y = effect[i].speed * sin(effect[i].angle);
			effect[i].type = type;
			effect[i].tmpID = tmpCnt;
			effect[i].hpMax = 20;
			effect[i].hp = effect[i].hpMax;
			effect[i].rgbFilter = GetProjectileGraphFilter();
			effect[i].velocity = { 0.0f,0.0f };
			effect[i].visible = true;
			if (effect[i].visible) tmpCnt++;
		}
		if (tmpCnt >= 60) break;
	}
}

void ProjControl(int index)
{
	effect[index].pos.x += effect[index].movPos.x;
	effect[index].pos.y += effect[index].movPos.y;
}

void ProjEffDraw(POS_F pos, int index)
{
	if (effect[index].visible)
	{
		int tmpColor = CheckPlayerGraphFilter(effect[index].rgbFilter);
		
		DrawCircle(pos.x , pos.y , 3, tmpColor, true);
		SetDrawBright(255, 255, 255);
	}
}

// Jump Particle
void JPSpawnInit(POS_F pos, EFFECT_TYPE type)
{
	int tmpCnt = 0;
	int tmpMax = 3 + GetRand(3);

	for (int i = 0; i < EFFECT_MAX; i++)
	{
		if (!effect[i].visible)
		{
			effect[i].pos = pos;
			effect[i].speed = 3;
			effect[i].angle = ((225.0f + GetRand(90)) * PI / 180.0f);
			effect[i].movPos.x = effect[i].speed * cos(effect[i].angle);
			effect[i].movPos.y = effect[i].speed * sin(effect[i].angle);
			effect[i].type = type;
			effect[i].tmpID = tmpCnt;
			effect[i].hpMax = 150;
			effect[i].hp = effect[i].hpMax;
			effect[i].rgbFilter = 0;
			effect[i].velocity = { 0.0f,0.0f };
			effect[i].visible = true;
			if (effect[i].visible) tmpCnt++;
		}
		if (tmpCnt >= tmpMax) break;
	}
}

void JPControl(int index)
{
	if (BlockCheck(effect[index].pos) == WALKTHROUGH)
	{
		effect[index].pos.x += effect[index].movPos.x;
		effect[index].pos.y += effect[index].movPos.y;

		effect[index].velocity.y += 1.0f;

		effect[index].movPos.x = effect[index].speed * cos(effect[index].angle);
		effect[index].movPos.y = effect[index].speed * sin(effect[index].angle) + effect[index].velocity.y;
	}	
}

void JPDraw(POS_F pos, int index)
{
	SetDrawBright(230, 230, 230);
	DrawCircle(pos.x, pos.y, 3, 0xFFFFFF, true);
	SetDrawBright(255, 255, 255);
}

// Hit Effect
void HESpawnInit(POS_F pos, EFFECT_TYPE type)
{
	int tmpCnt = 0;
	int tmpMax = 2 + GetRand(1);
	int tmpDir = GetRand(1);

	for (int i = 0; i < EFFECT_MAX; i++)
	{
		if (!effect[i].visible)
		{
			effect[i].pos = pos;
			effect[i].speed = 8;
			if (tmpDir == 0) effect[i].angle = (-22.5f + GetRand(45) * PI / 180.0f);
			else effect[i].angle = (157.5f + GetRand(45) * PI / 180.0f);
			effect[i].movPos.x = effect[i].speed * cos(effect[i].angle);
			effect[i].movPos.y = effect[i].speed * sin(effect[i].angle);
			effect[i].type = type;
			effect[i].tmpID = tmpCnt;
			effect[i].hpMax = 360;
			effect[i].hp = effect[i].hpMax;
			effect[i].rgbFilter = GetPlayerGraphFilter();
			effect[i].velocity = { 0.0f,30.0f };
			effect[i].visible = true;
			if (effect[i].visible) tmpCnt++;
		}
		if (tmpCnt >= tmpMax) break;
	}
}

void HEControl(int index)
{
	POS_F mov = effect[index].pos;
	effect[index].velocity.y -= ACG_G * SECOND_PER_FRAME;
	mov.y -= effect[index].velocity.y * SECOND_PER_FRAME;

	if (BlockCheck(effect[index].pos) == WALKTHROUGH)
	{
		mov.x += effect[index].movPos.x;
		mov.y += effect[index].movPos.y;

		effect[index].pos = mov;
	}
}

void HEDraw(POS_F pos, int index)
{
	float blendCnt = 255 / effect[index].hpMax;

	int tmpColor = CheckPlayerGraphFilter(effect[index].rgbFilter);

	DrawCircle(pos.x, pos.y, 2, tmpColor, true);
	SetDrawBright(255, 255, 255);
}

// Revive Effect
void ReviveSpawnInit(POS_F pos, EFFECT_TYPE type)
{
	for (int i = 0; i < EFFECT_MAX; i++)
	{
		if (!effect[i].visible)
		{
			effect[i].pos = pos;
			effect[i].hpMax = 60 * 5;
			effect[i].hp = effect[i].hpMax;
			effect[i].type = type;
			effect[i].aniCnt = 0;
			effect[i].visible = true;
			break;
		}
	}
}

void ReviveControl(int index)
{
	effect[index].aniCnt++;
}

void ReviveDraw(POS_F pos, int index)
{
	for (int j = 0; j < 119; j++)
	{
		DrawExtendGraphF(pos.x, pos.y, pos.x + 128.0f, pos.y + 128.0f, reviveEffectImg[effect[index].aniCnt], true);
	}
}

// Test Effect
void TestSpawnInit(POS_F pos, EFFECT_TYPE type)
{
	for (int i = 0; i < EFFECT_MAX; i++)
	{
		if (!effect[i].visible)
		{
			pos.x += GetRand(36);
			pos.y += GetRand(76);
			effect[i].pos = pos;
			effect[i].type = type;
			effect[i].hpMax = 4;
			effect[i].hp = effect[i].hpMax;
			effect[i].visible = true;
			break;
		}
	}
}

void TestControl(int index)
{

}

void TestDraw(POS_F pos, int index)
{
	int tmpColor = 0xaf0808;
	if (effect[index].hp <= effect[index].hpMax / 4) tmpColor = 0xffffff;

	DrawCircleAA(pos.x, pos.y, 50, 100, tmpColor, true, 1.0f);
}

void DeleteEffect(int index)
{
	switch (effect[index].type)
	{
	case EFFECT_TYPE_NONE:
		break;
	case EFFECT_TYPE_EXPLOSION:
		break;
	case EFFECT_TYPE_PROJECTILE:
		if (BlockCheck(effect[index].pos)) effect[index].visible = false;
		break;
	}

	if (effect[index].visible && effect[index].hp <= 0) effect[index].visible = false;
}