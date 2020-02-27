#include <math.h>
#include "main.h"
#include "projectile.h"
#include "character.h"
#include "enemy.h"
#include "checkhit.h"
#include "keycheck.h"
#include "stage.h"
#include "blockcheck.h"
#include "sound.h"
#include "effect.h"

#define PROJECTILE_GRAVITY 0.008f

int projectileImg[3][2][PROJECTILE_TYPE_MAX];
PROJECTILE projectileHandle[PROJECTILE_MAX];
int projectileCount;
int porjectileColor;

int projectileFilter[4];

int LoadProjectileGraphFilter(int index);
DIR ConvertAngleToDirection(double angle);

void ProjectileHitPosInit(PROJECTILE_TYPE type, int index);
void ProjectileAnimCntInit(PROJECTILE_TYPE type, int index);
void ProjectileSoundEffect(PROJECTILE_TYPE type, int index);

void ProjectileSysInit(void)
{
	//レベル補正フィルタの値
	projectileFilter[0] = 1;
	projectileFilter[1] = 60;
	projectileFilter[2] = 180;
	projectileFilter[3] = 300;

	char tmp[256];
	for (int i = 0; i < 4; i++)
	{
		sprintf_s(tmp, "Image/Ball/fireball_%03d.png", i + 1);
		projectileImg[i][0][0] = LoadGraph(tmp);
		sprintf_s(tmp, "Image/Ball/fireball_impact_%03d.png", i + 1);
		projectileImg[i][1][0] = LoadGraph(tmp);

		sprintf_s(tmp, "Image/Ball/fireball_%03d.png", i + 1);
		projectileImg[i][0][1] = LoadGraph(tmp);
		sprintf_s(tmp, "Image/Ball/fireball_impact_%03d.png", i + 1);
		projectileImg[i][1][1] = LoadGraph(tmp);

		sprintf_s(tmp, "Image/Ball/fireball_%03d.png", i + 1);
		projectileImg[i][0][2] = LoadGraph(tmp);
		sprintf_s(tmp, "Image/Ball/fireball_impact_%03d.png", i + 1);
		projectileImg[i][1][2] = LoadGraph(tmp);

		sprintf_s(tmp, "Image/Ball/fireball_%03d.png", i + 1);
		projectileImg[i][0][3] = LoadGraph(tmp);
		sprintf_s(tmp, "Image/Ball/fireball_impact_%03d.png", i + 1);
		projectileImg[i][1][3] = LoadGraph(tmp);

		for (int j = 0; j < 2;j++)
		{
			for (int k = 0;k < 4;k++)
			{
				GraphFilter(projectileImg[i][j][k], DX_GRAPH_FILTER_HSB, 1, projectileFilter[k], 360, 0);
			}
		}

		sprintf_s(tmp, "Image/Ball/fireball_%03d.png", i + 1);
		projectileImg[i][0][8] = LoadGraph(tmp);
		sprintf_s(tmp, "Image/Ball/fireball_impact_%03d.png", i + 1);
		projectileImg[i][1][8] = LoadGraph(tmp);

		sprintf_s(tmp, "Image/GreenPoison/green_poison_%03d.png", i + 1);
		projectileImg[i][0][5] = LoadGraph(tmp);
		sprintf_s(tmp, "Image/GreenPoison/green_poison_impact_%03d.png", i + 1);
		projectileImg[i][1][5] = LoadGraph(tmp);
		sprintf_s(tmp, "Image/Iceball/improved_iceball_%03d.png", i + 1);
		projectileImg[i][0][6] = LoadGraph(tmp);
		sprintf_s(tmp, "Image/Iceball/improved_iceball_impact_%03d.png", i + 1);
		projectileImg[i][1][6] = LoadGraph(tmp);
		sprintf_s(tmp, "Image/Fire/dragon_fireball_%03d.png", i + 1);
		projectileImg[i][0][7] = LoadGraph(tmp);
		sprintf_s(tmp, "Image/Fire/dragon_fireball_impact_%03d.png", i + 1);
		projectileImg[i][1][7] = LoadGraph(tmp);
	}
	sprintf_s(tmp, "Image/Arrow/arrow.png");
	projectileImg[0][0][4] = LoadGraph(tmp);
}

void ProjectileGameInit(void)
{
	projectileCount = 0;
	for (int i = 0; i <= PROJECTILE_MAX; i++)
	{
		projectileHandle[i].animCnt = 0;
		projectileHandle[i].animCntMax = 0;
		projectileHandle[i].filterRGB = 0;
		projectileHandle[i].lasting = 0;
		projectileHandle[i].lastingCnt = 0;
		projectileHandle[i].speed = 0;
		projectileHandle[i].pos1 = { 0, 0 };
		projectileHandle[i].size = { 0, 0 };
		projectileHandle[i].gravity = false;
		projectileHandle[i].pierce = false;
		projectileHandle[i].spin = false;
		projectileHandle[i].flag = false;
		projectileHandle[i].reached = false;
		projectileHandle[i].angle = 0;
		projectileHandle[i].size.x = 0;
		projectileHandle[i].size.y = 0;
		projectileHandle[i].offset = 0;
		projectileHandle[i].type = PROJECTILE_CNT_MAX;
	}
}

void ProjectileInit(PROJECTILE_TYPE type, POS_F pos, bool gravity, bool spin, bool pierce,int speed, double angle, int RGB, int RGBFlag, int lasting)
{
	projectileHandle[projectileCount].type = type;
	projectileHandle[projectileCount].pos1 = pos;
	projectileHandle[projectileCount].angle = angle;
	projectileHandle[projectileCount].gravity = gravity;
	projectileHandle[projectileCount].spin = spin;
	projectileHandle[projectileCount].pierce = pierce;
	projectileHandle[projectileCount].speed = speed;
	projectileHandle[projectileCount].filterRGB = RGB;
	projectileHandle[projectileCount].RGBFlag = RGBFlag;
	projectileHandle[projectileCount].lastingCnt = 0;
	projectileHandle[projectileCount].lasting = lasting;
	GetGraphSize(projectileImg[0][0][type], &projectileHandle[projectileCount].size.x, &projectileHandle[projectileCount].size.y);
	//当たり判定
	ProjectileHitPosInit(type, projectileCount);
	//フレーム数
	ProjectileAnimCntInit(type, projectileCount);
	//音SE
	ProjectileSoundEffect(type, projectileCount);
	//GetGraphSize(projectileImg[0][0], &projectileHandle[projectileCount].size.x, &projectileHandle[projectileCount].size.y);
	projectileHandle[projectileCount].flag = true;
	projectileHandle[projectileCount].reached = false;
	projectileCount++;
	if (projectileCount >= PROJECTILE_MAX)
	{
		projectileCount = 0;
	}
}

void ProjectileLoop(void)
{
	for (int i = 0; i <= PROJECTILE_MAX; i++)
	{
		if (projectileHandle[i].flag == true)
		{
			projectileHandle[i].animCnt++;

			projectileHandle[i].lastingCnt++;
			if (projectileHandle[i].reached == false)
			{
				//当たる時の制御
				auto hitTrig = [&]() {
					projectileHandle[i].reached = true;
					porjectileColor = LoadProjectileGraphFilter(i);

					projectileHandle[i].animCnt = 0;
					projectileHandle[i].lastingCnt = 0;
					projectileHandle[i].lasting = projectileHandle[i].animCntMax * 6;

					
				/*	POS_F tmpPos = hitPos;
					if (projectileHandle[i].angle == 0) tmpPos.x = hitPos.x + 33.5 - projectileHandle[i].speed;
					else tmpPos.x = hitPos.x - 34.5 + projectileHandle[i].speed;*/


				};

				projectileHandle[i].pos1.x += cos(projectileHandle[i].angle) * projectileHandle[i].speed;
				projectileHandle[i].pos1.y += sin(projectileHandle[i].angle) * projectileHandle[i].speed;

				//	重力
				if (projectileHandle[i].gravity)
				{
					if (ConvertAngleToDirection(projectileHandle[i].angle) == DIR_RIGHT)
					{
						projectileHandle[i].angle += PROJECTILE_GRAVITY;
					}
					else if (ConvertAngleToDirection(projectileHandle[i].angle) == DIR_LEFT)
					{
						projectileHandle[i].angle -= PROJECTILE_GRAVITY;
					}
				}

				//	回転
				if (projectileHandle[i].spin)
				{
					projectileHandle[i].angle -= 0.001 * GetRand(100);
				}

				//　一定の時間たったら
				if (projectileHandle[i].lastingCnt == (projectileHandle[i].lasting - projectileHandle[i].animCntMax))
				{
					hitTrig();
				}

				POS_F hitTemp;
				hitTemp.x = projectileHandle[i].pos1.x + (projectileHandle[i].size.x / 2);
				hitTemp.y = projectileHandle[i].pos1.y + (projectileHandle[i].size.y / 2);

				hitTemp.x += cos(projectileHandle[i].angle) * projectileHandle[i].offset;
				hitTemp.y += sin(projectileHandle[i].angle) * projectileHandle[i].offset;

				//　壁当たり判定
				if (!projectileHandle[i].pierce)	//ピアス設定がないときだけブロックチェック行う
				{
					if ((BlockCheck({ hitTemp.x + cosf(projectileHandle[i].angle) * projectileHandle[i].hitPosS.x, hitTemp.y + sinf(projectileHandle[i].angle) * projectileHandle[i].hitPosS.y }) != WALKTHROUGH)
						&& (BlockCheck({ hitTemp.x + cosf(projectileHandle[i].angle) * projectileHandle[i].hitPosS.x, hitTemp.y + sinf(projectileHandle[i].angle) * projectileHandle[i].hitPosS.y }) != CRITICAL)
						||
						(BlockCheck({ hitTemp.x + cosf(projectileHandle[i].angle) * projectileHandle[i].hitPosE.x, hitTemp.y + sinf(projectileHandle[i].angle) * projectileHandle[i].hitPosE.y }) != WALKTHROUGH)
						&& (BlockCheck({ hitTemp.x + cosf(projectileHandle[i].angle) * projectileHandle[i].hitPosE.x, hitTemp.y + sinf(projectileHandle[i].angle) * projectileHandle[i].hitPosE.y }) != CRITICAL))
					{
						hitTrig();

						POS_F tmpPos = hitTemp;

						if (projectileHandle[i].angle == 0) tmpPos.x = hitTemp.x - projectileHandle[i].speed / 2;
						else tmpPos.x = hitTemp.x + projectileHandle[i].speed / 2;

						SetEffect(tmpPos, EFFECT_TYPE_PROJECTILE);
						PlaySoundType(SOUND_TYPE_HIT);
					}
				}

				// エネミー(キャラクター)当たり判定
				if (projectileHandle[i].type < PLAYER_MAX)	//プレイヤー弾
				{
					for (int j = 0; j < ENEMY_MAX; j++)
					{
						if (EnemyCollideCheck(j, 
							hitTemp, 
							{projectileHandle[i].hitPosE.x,
							projectileHandle[i].hitPosE.y }))
						{
							POS_F tmpPos = hitTemp;

							if (projectileHandle[i].angle == 0) tmpPos.x = hitTemp.x - projectileHandle[i].speed / 2;
							else tmpPos.x = hitTemp.x + projectileHandle[i].speed / 2;

							SetEffect(tmpPos, EFFECT_TYPE_PROJECTILE);
							PlaySoundType(SOUND_TYPE_HIT);
							if (EnemyGetHit(j, ConvertAngleToDirection(projectileHandle[i].angle)) == true)
							{
								//反射
								projectileHandle[i].pierce = true;
								projectileHandle[i].gravity = true;
								projectileHandle[i].speed += 2;
								projectileHandle[i].type = PROJECTILE_FIREBALL5;
								projectileHandle[i].angle -= 3.142;

								if (ConvertAngleToDirection(projectileHandle[i].angle) == DIR_RIGHT)
								{
									projectileHandle[i].angle -= PROJECTILE_GRAVITY * (10 + GetRand(50));
								}
								else if (ConvertAngleToDirection(projectileHandle[i].angle) == DIR_LEFT)
								{
									projectileHandle[i].angle += PROJECTILE_GRAVITY * (10 + GetRand(50));
								}
							}
							else
							{
								//当たり
								hitTrig();
							}
						}
					}
				}
				else
				{
					for (int j = 0; j < PLAYER_MAX; j++)
					{
						if (GetPlayerFlag(j))
						{
							if (PlayerCollideCheck(j, hitTemp, { int(projectileHandle[i].hitPosE.x),
								int(projectileHandle[i].hitPosE.y) }))
							{
								//当たり
								hitTrig();
								SetEffect(hitTemp, EFFECT_TYPE_PROJECTILE);
								PlayerGetHit(j, ConvertAngleToDirection(projectileHandle[i].angle));
							}
						}
					}
				}
			}
			if (projectileHandle[i].lastingCnt >= projectileHandle[i].lasting) projectileHandle[i].flag = false;
		}
	}
}

void ProjectileDraw(void)
{
	int imgTmp;
	
	for (int i = 0; i <= PROJECTILE_MAX; i++)
	{
		POS_F tmpPos = AddScrollF({ projectileHandle[i].pos1.x + (projectileHandle[i].size.x / 2),
				projectileHandle[i].pos1.y + (projectileHandle[i].size.y / 2) });
		if (projectileHandle[i].flag == true)
		{
			if (projectileHandle[i].type < PLAYER_MAX) SetDrawBlendMode(DX_BLENDMODE_ALPHA, 153);
			imgTmp = projectileImg[(projectileHandle[i].animCnt / 6) % projectileHandle[i].animCntMax][projectileHandle[i].reached][projectileHandle[i].type];
			//　プレイヤーの色と同じ
			//if (projectileHandle[i].RGBFlag) GraphFilter(imgTmp, DX_GRAPH_FILTER_HSB, 1, projectileHandle[i].filterRGB, 360, 0);
			DrawRotaGraph3F(
				tmpPos.x,
				tmpPos.y,
				(projectileHandle[i].size.x / 2), (projectileHandle[i].size.y / 2),
				1, 1,
				projectileHandle[i].angle, imgTmp,
				true, false);
			if (projectileHandle[i].type < PLAYER_MAX) SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
	}
}

void ProjectileHitPosInit(PROJECTILE_TYPE type, int index)
{
	switch (type)
	{
	case PROJECTILE_FIREBALL1:
	case PROJECTILE_FIREBALL2:
	case PROJECTILE_FIREBALL3:
	case PROJECTILE_FIREBALL4:
		projectileHandle[index].hitPosS = { -13, -9 };
		projectileHandle[index].hitPosE = { 10,10 };
		projectileHandle[index].offset = 24.0f;
		break;
	case PROJECTILE_ARROW:
		projectileHandle[index].hitPosS = { -5,-6 };
		projectileHandle[index].hitPosE = { -4, 2 };
		projectileHandle[index].offset = 18.0f;
		break;
	case PROJECTILE_POISON:
		projectileHandle[index].hitPosS = { -23,-18 };
		projectileHandle[index].hitPosE = { 22, 18 };
		projectileHandle[index].offset = 59.0f;
		break;
	case PROJECTILE_ICEBALL:
		projectileHandle[index].hitPosS = { -13, -9 };
		projectileHandle[index].hitPosE = { 10,10 };
		projectileHandle[index].offset = 24.0f;
		break;
	case PROJECTILE_BOSS2:
		projectileHandle[index].hitPosS = { -22, -18 };
		projectileHandle[index].hitPosE = { 22,18 };
		projectileHandle[index].offset = 52.0f;
		break;
	default:
		projectileHandle[index].hitPosS = { 0,0 };
		projectileHandle[index].hitPosE = { 0,0 };
		projectileHandle[index].offset = 0;
		break;
	}
}

void ProjectileAnimCntInit(PROJECTILE_TYPE type, int index)
{
	switch (type)
	{
	case PROJECTILE_FIREBALL1:
	case PROJECTILE_FIREBALL2:
	case PROJECTILE_FIREBALL3:
	case PROJECTILE_FIREBALL4:
	case PROJECTILE_FIREBALL5:
		projectileHandle[index].animCntMax = 3;
		break;
	case PROJECTILE_ARROW:
		projectileHandle[index].animCntMax = 1;
		break;
	case PROJECTILE_POISON:
		projectileHandle[index].animCntMax = 3;
		break;
	case PROJECTILE_ICEBALL:
		projectileHandle[index].animCntMax = 3;
		break;
	case PROJECTILE_BOSS2:
		projectileHandle[index].animCntMax = 3;
		break;
	default:
		projectileHandle[index].animCntMax = 0;
		break;
	}
}

void ProjectileSoundEffect(PROJECTILE_TYPE type, int index)
{
	switch (type)
	{
	case PROJECTILE_FIREBALL1:
	case PROJECTILE_FIREBALL2:
	case PROJECTILE_FIREBALL3:
	case PROJECTILE_FIREBALL4:
	case PROJECTILE_FIREBALL5:
		PlaySoundType(SOUND_TYPE_BALL);
		break;
	case PROJECTILE_ARROW:
		PlaySoundType(SOUND_TYPE_ARROW);
		break;
	case PROJECTILE_POISON:
		PlaySoundType(SOUND_TYPE_POISON);
		break;
	case PROJECTILE_ICEBALL:
		PlaySoundType(SOUND_TYPE_ICEBALL);
		break;
	case PROJECTILE_BOSS2:
		PlaySoundType(SOUND_TYPE_FIREBALL);
		break;
	default:
		break;
	}
}

int LoadProjectileGraphFilter(int index)
{
	return projectileHandle[index].filterRGB;
}

int GetProjectileGraphFilter(void)
{
	return porjectileColor;
}

DIR ConvertAngleToDirection(double angle) {
	DIR direction;

	if (cos(angle) < 0) direction = DIR_LEFT;
	else direction = DIR_RIGHT;

	return direction;
}