#include "main.h"
#include "enemy.h"
#include "keycheck.h"
#include "enemyAI.h"
#include <math.h>
#include "stage.h"
#include "blockcheck.h"
#include "checkhit.h"
#include "map_edit.h"
#include "projectile.h"
#include "fade_character.h"
#include "Character.h"
#include "effect.h"
#include "sound.h"
#include "Score.h"
#include "shield.h"
#include "mapdata.h"
#include "ending.h"
#include "fade.h"

ENEMY enemy[ENEMY_MAX];
BOSS boss[BOSS_MAX];
int enemyCnt;

int enemyImg[ENEMY_ID_MAX];
int enemyImgRun[ENEMY_ID_MAX][ENEMY_IMG_RUN_MAX];
int enemyImgDie[ENEMY_ID_MAX][ENEMY_IMG_DIE_MAX];
int enemyImgAttack[ENEMY_ID_MAX][ENEMY_IMG_ATTACK_MAX];
int swooshImg[2];

int tmpEffectImg;
POS tmpEffectSize;

void EnemySpawnInit(void);
void EnemyHitPosInit(int id);
float EnemyGetHitDamage(int type);
int GetEnemyAttackInterval(int type);
int EnemyGetScore(int type);

const char *name[ENEMY_ID_MAX] = { 
	"Barbarian 1", "Barbarian 2","Dragon 1","Dragon 2","Dwarf 1","Dwarf 2",
	"Elf Female","Elf Male","Fire Demon","Gargoyle","Goblin 1","Goblin 2",
	"Knight","Lizard Man 1","Lizard Man 2","Medusa","Minotaur","Orc 1","Orc 2",
	"Peasant","Princess","Scorpion","Skeleton 1","Skeleton 2","Skeleton 3",
	"Soldier","Spider","Treant","Troll","Wizard 1","Wolf" };

const char *sname[ENEMY_ID_MAX] = {
	"barbarian_1", "barbarian","dragon","dragon","dwarf","dwarf",
	"elf_female","elf_male","fire_demon","gargoyle","goblin","goblin",
	"knight","lizard_man","lizard_man","medusa","minotaur","orc","orc",
	"peasant","princess","scorpion","skeleton","skeleton","skeleton",
	"soldier","spider","treant","troll","wizard_1","wolf" };

void EnemySysInit(void)
{
	char tmpstr[256];
	for (int j = 0; j < ENEMY_ID_MAX; j++)
	{
		sprintf_s(tmpstr, "Image/Enemy/%s/%s.png", name[j], sname[j]);
		enemyImg[j] = LoadGraph(tmpstr);

		char str[256];
		for (int i = 0; i < ENEMY_IMG_RUN_MAX; i++)
		{
			sprintf_s(str, "Image/Enemy/%s/%s_run_%03d.png", name[j],sname[j], i + 1);
			enemyImgRun[j][i] = LoadGraph(str);
		}

		for (int i = 0; i < ENEMY_IMG_DIE_MAX; i++)
		{
			sprintf_s(str, "Image/Enemy/%s/%s_die_%03d.png", name[j], sname[j], i + 1);
			enemyImgDie[j][i] = LoadGraph(str);
		}

		for (int i = 0; i < ENEMY_IMG_ATTACK_MAX; i++)
		{
			sprintf_s(str, "Image/Enemy/%s/%s_attack_%03d.png", name[j], sname[j], i + 1);
			enemyImgAttack[j][i] = LoadGraph(str);
		}
	}

	for (int i = 0; i < 2; i++)
	{
		sprintf_s(tmpstr, "Image/Effect/weapon_swoosh_%03d.png", i + 1);
		swooshImg[i] = LoadGraph(tmpstr);
	}
}

void EnemyGameInit(void)
{
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		enemy[i].aniCnt = 0;
		enemy[i].pos = { 0, 0 };
		enemy[i].id = 0;
		enemy[i].velocity = { 0,0 };
		enemy[i].hitpoint = 0;
		enemy[i].hitFlagCooldown = 0;
		enemy[i].runFlag = false;
		enemy[i].visible = false;
		enemy[i].dir = DIR_RIGHT;
		enemy[i].atkFlag = false;
		enemy[i].dieFlag = false;
		enemy[i].jumpFlag = false;
		enemy[i].type = ENEMY_TYPE_MAX;	//null
	}


	for (int i = 0; i < BOSS_MAX; i++)
	{
		boss[i].index = 0;
		boss[i].movAngle = 0;
		boss[i].shotAngle = 0;
	}

	//EnemyInit(GARGOYLE, {850.0f, 7530.0f});
	//MapLoad();
	enemyCnt = 0;
}

void EnemyInit(int id, POS_F pos)
{
	enemy[enemyCnt].aniCnt = 0;
	enemy[enemyCnt].pos = pos;
	GetGraphSize(enemyImg[enemy[enemyCnt].id], &enemy[enemyCnt].size.x, &enemy[enemyCnt].size.y);
	enemy[enemyCnt].speed = 5;
	enemy[enemyCnt].hitpoint = 1;
	enemy[enemyCnt].hitFlagCooldown = ENEMY_REGENERATE_COOLDOWN;
	enemy[enemyCnt].velocity = { 0,0 };
	enemy[enemyCnt].id = id;
	enemy[enemyCnt].runFlag = false;
	enemy[enemyCnt].visible = true;
	enemy[enemyCnt].dir = DIR_LEFT;
	enemy[enemyCnt].atkFlag = false;
	enemy[enemyCnt].dieFlag = false;
	EnemyHitPosInit(id);
	enemy[enemyCnt].type = EnemyClassification(id);
	if (!(enemy[enemyCnt].type < ENEMY_TYPE_MAX)) enemy[enemyCnt].visible = false;	//remove this unit
	if (!enemy[enemyCnt].visible)
	{
		enemy[enemyCnt].id = id;
		enemy[enemyCnt].aniCnt = 0;
		enemy[enemyCnt].pos = { 0,0 };
	}
	if (enemy[enemyCnt].type == BOSS1 || enemy[enemyCnt].type == BOSS2)
	{
		//boss initialization
		int numTmp;
		if (enemy[enemyCnt].type == BOSS1) numTmp = 0;
		if (enemy[enemyCnt].type == BOSS2) numTmp = 1;

		boss[numTmp].index = enemyCnt;
		boss[numTmp].lives = 3;
		boss[numTmp].shield = false;

		if (numTmp == 1)
		{
			boss[0].shield = true;
			EffectShieldInit(enemyCnt+1, 10);
		}
	}

	EnemyAIInit(enemyCnt, enemy[enemyCnt].speed, GetEnemyAttackInterval(id), enemy[enemyCnt].dir, enemy[enemyCnt].hitPosS, enemy[enemyCnt].hitPosE);

	enemyCnt++;
	if (enemyCnt > ENEMY_MAX) enemyCnt = 0;
}

void EnemyGameMain(void)
{
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		//animation
		if (!enemy[i].visible) continue;

		if (enemy[i].dieFlag)
		{
			if (((enemy[i].aniCnt / 6) % ENEMY_IMG_DIE_MAX) == 5)
			{
				enemy[i].visible = 0;
			}
		}

		enemy[i].aniCnt++;

		//敵HP回復
		if (enemy[i].hitFlagCooldown > 0) enemy[i].hitFlagCooldown--;
		if (enemy[i].hitpoint <= 0.99 && enemy[i].hitFlagCooldown <= 0) enemy[i].hitpoint += 0.01;

		if (EnemyAIMain(i, enemy[i].type, enemy[i].pos) == true)
		{
			//実際に敵を移動する
			switch (enemy[i].type)
			{
			case LAND:
			case MOUNT:
			case RANGED:
			case DEMON:
				if (enemy[i].runFlag && !enemy[i].atkFlag)
				{
					auto sidePos = [&](DIR dir, POS_F pos, int speed) {
						POS_F side;
						float tmp = float(speed);
						switch (dir)
						{
						case DIR_LEFT:
							side = { (0.0f - tmp - 1.0f) + float(enemy[i].hitPosS.x),float(enemy[i].hitPosE.y - 1) };
							break;
						case DIR_RIGHT:
							side = { tmp + float(enemy[i].hitPosE.x) ,float(enemy[i].hitPosE.y - 1) };
							break;
						default:
							break;
						}
						return POS_F{ pos.x + side.x,pos.y + side.y };
					};

					int checkTop = BlockCheck(sidePos(enemy[i].dir, enemy[i].pos, enemy[i].speed));			// 上半身のブロックのあたりチェック
					int checkBottom = BlockCheck(sidePos(enemy[i].dir, enemy[i].pos, enemy[i].speed));		// 下半身のブロックのあたりチェック

					if (enemy[i].dir == DIR_LEFT)
					{
						if ((checkTop == WALKTHROUGH && checkBottom == WALKTHROUGH) ||
							(checkTop == PASSABLE && checkBottom == WALKTHROUGH) ||
							(checkTop == WALKTHROUGH && checkBottom == PASSABLE))
						{
							enemy[i].pos.x -= enemy[i].speed;
						}
					}
					else if (enemy[i].dir == DIR_RIGHT)
					{
						if ((checkTop == WALKTHROUGH && checkBottom == WALKTHROUGH) ||
							(checkTop == PASSABLE && checkBottom == WALKTHROUGH) ||
							(checkTop == WALKTHROUGH && checkBottom == PASSABLE))
						{
							enemy[i].pos.x += enemy[i].speed;
						}
					}
				}
				break;
			case FLY_MELEE:
			case FLY_RANGE:
				enemy[i].runFlag = true;	//	常に飛んでいることにする
				enemy[i].pos.x += cos(enemy[i].flyAngle) * (enemy[i].speed + (8 * enemy[i].atkFlag));
				enemy[i].pos.y += sin(enemy[i].flyAngle) * (enemy[i].speed + (8 * enemy[i].atkFlag));
				break;
			case BOSS1:
				if (enemy[i].runFlag)
				{
					enemy[i].pos.x += cos(boss[0].movAngle) * enemy[i].speed;
					enemy[i].pos.y += sin(boss[0].movAngle) * enemy[i].speed;

					//enemy[i].pos.x = max(0.0f, enemy[i].pos.x);

					if (enemy[i].pos.x <= 0.0f)
					{
						enemy[i].pos.x = 0.0f;
					}

					if (enemy[i].pos.x >= SCREEN_SIZE_X)
					{
						enemy[i].pos.x = SCREEN_SIZE_X;
					}

					if (CheckHitKey(KEY_INPUT_F6))
					{
						enemy[i].pos = { SCREEN_SIZE_X / 2, enemy[boss[1].index].pos.y - SCREEN_SIZE_Y / 2 };
						
					}

					//enemy[i].pos.x = min(SCREEN_SIZE_X, enemy[i].pos.x);
				}
				break;
			case BOSS2:
				if (enemy[i].runFlag && !enemy[i].atkFlag)
				{
					enemy[i].pos.x += cos(boss[1].movAngle) * enemy[i].speed;
					enemy[i].pos.y += sin(boss[1].movAngle) * enemy[i].speed;
				}
				break;
			default:
				break;
			}

			//実際に敵を攻撃させる
			if (enemy[i].atkFlag && (enemy[i].aniCnt == 5 * ENEMY_IMG_ATTACK_MAX))
			{
				POS_F projectilePos = enemy[i].pos;
				switch (enemy[i].type)
				{
				case LAND:
				case MOUNT:
					//当たり判定をもう一回する
					POS_F tempPos;
					if (enemy[i].dir == DIR_LEFT)
					{
						tempPos.x = enemy[i].pos.x;
					}
					else
					{
						tempPos.x = enemy[i].pos.x + ((enemy[i].hitPosS.x + enemy[i].hitPosE.x) / 2);
					}
					tempPos.y = enemy[i].pos.y + ((enemy[i].hitPosS.y + enemy[i].hitPosE.y) / 2);//中間Y位置を取る
					for (int j = 0; j < PLAYER_MAX; j++)
					{
						if (PlayerCollideCheck(j, tempPos, { enemy[i].hitPosE.x , 5 }))
						{
							PlayerGetHit(j, enemy[i].dir);
						}
					}
					break;
				case RANGED:
					//敵弾をだす
					float angle;
					enemy[i].dir == DIR_RIGHT ? angle = 0 : angle = 3.142;

					projectilePos.x = (projectilePos.x + ((enemy[i].hitPosS.x + enemy[i].hitPosE.x) / 4)) + (cosf(angle) * ((enemy[i].hitPosS.x + enemy[i].hitPosE.x) / 4));
					projectilePos.y += ((enemy[i].hitPosS.y + enemy[i].hitPosE.y) / 4);

					//敵種類ごとに違う弾を撃たせる
					switch (enemy[i].id)
					{
					case SKELETON_2:
					default:
						ProjectileInit(PROJECTILE_ARROW, projectilePos, true, false, false, enemy[i].speed * 4, angle, 0, false, 100);
						break;
					case MEDUSA:
						ProjectileInit(PROJECTILE_POISON, projectilePos, false, true, true, enemy[i].speed * 2, angle - (0.5 + (GetRand(2) / 4)), 0, false, 120 + GetRand(45));
						break;
					case SKELETON_3:
						//プレイヤーごとに角度計算して発射する
						for (int j = 0; j < PLAYER_MAX; j++)
						{
							if (GetPlayerFlag(j) && !GetPlayerTrapFlag(j))
							{
								POS_F tmp = GetPlayerPos(j);
								tmp.y += SCREEN_SIZE_Y / 20;
								ProjectileInit(PROJECTILE_ICEBALL, projectilePos, false, false, true, enemy[i].speed,
									(atan2(tmp.y - projectilePos.y, tmp.x - projectilePos.x)), 0, false, 70);
							}
						}
						break;
					}
					break;
				case DEMON:
					for (int j = 0; j < PLAYER_MAX; j++)
					{
						SetEffect({ enemy[i].pos.x + ((enemy[i].hitPosS.x + enemy[i].hitPosE.x) / 2),
									enemy[i].pos.y + ((enemy[i].hitPosS.y + enemy[i].hitPosE.y) / 2) }, EFFECT_TYPE_PROJECTILE);
						if (PlayerCollideCheck(j, enemy[i].pos, { enemy[i].hitPosS.x + enemy[i].hitPosE.x , enemy[i].hitPosS.y + enemy[i].hitPosE.y }) == true)
						{
							PlayerGetHit(j, enemy[i].dir);
						}
					}
					break;
				case BOSS2:
					projectilePos.x = (projectilePos.x + ((enemy[i].hitPosS.x + enemy[i].hitPosE.x) / 4)) + (cosf(boss[1].movAngle) * ((enemy[i].hitPosS.x + enemy[i].hitPosE.x) / 3));
					projectilePos.y += ((enemy[i].hitPosS.y + enemy[i].hitPosE.y) / 4);

					ProjectileInit(PROJECTILE_BOSS2, projectilePos, false, false, true, 12, boss[1].shotAngle, 0, false, 300);
					break;
				default:
					break;
				}
			}

			if (enemy[i].atkFlag && enemy[i].type == FLY_MELEE)
			{
				POS_F tempPos;
				tempPos.x = enemy[i].pos.x + ((enemy[i].hitPosS.x + enemy[i].hitPosE.x) / 2);//中間X位置を取る
				tempPos.y = enemy[i].pos.y + ((enemy[i].hitPosS.y + enemy[i].hitPosE.y) / 2);//中間Y位置を取る
				for (int j = 0; j < PLAYER_MAX; j++)
				{
					if (PlayerCollideCheck(j, { tempPos.x - 5, tempPos.y - 5 }, { 10, 10 }))
					{
						PlayerGetHit(j, enemy[i].dir);
					}
				}
			}

			if (enemy[i].runFlag && enemy[i].speed > 30 && enemy[i].type == BOSS1)
			{
				for (int j = 0; j < PLAYER_MAX; j++)
				{
					if (PlayerCollideCheck(j, enemy[i].pos, { enemy[i].hitPosS.x + enemy[i].hitPosE.x , enemy[i].hitPosS.y + enemy[i].hitPosE.y }) == true)
					{
						PlayerGetHit(j, enemy[i].dir);
					}
				}
			}
		}

		//enemy physic
		if ((enemy[i].type != FLY_MELEE &&
			enemy[i].type != BOSS1 &&
			enemy[i].type != BOSS2) /*飛べる敵は物理引力と壁の当たり判定に影響されない*/
			&&
			(InScreen({ enemy[i].pos.x + enemy[i].hitPosE.x,enemy[i].pos.y + enemy[i].hitPosE.y })))
		{

			//　ジャンプ制御
		//movHitCheck の初期化
			POS_F movHitCheckMiddle;
			POS_F movHitCheckLeft;
			POS_F movHitCheckRight;
			POS_F mov;
			POS tmpIndex;
			POS_F jumpPos;

			auto movHitInit = [&](float head) {
				movHitCheckMiddle.y = mov.y + head;			//頭上の座標系計算
				movHitCheckMiddle.x = mov.x + enemy[i].hitPosS.x;

				movHitCheckLeft = movHitCheckMiddle;
				movHitCheckRight = movHitCheckMiddle;

				movHitCheckLeft.x = movHitCheckMiddle.x + (enemy[i].hitPosE.x - enemy[i].hitPosS.x) / 2;		// Middle Hit Point
				movHitCheckRight.x = movHitCheckMiddle.x + (enemy[i].hitPosE.x - enemy[i].hitPosS.x);			// Top Right Hit Point
			};

			mov = enemy[i].pos;
			movHitCheckMiddle = mov;
			movHitCheckLeft = mov;
			movHitCheckRight = mov;

			if (enemy[i].jumpFlag)
			{
				enemy[i].velocity.y -= ACG_G * SECOND_PER_FRAME;		//高さの計算
				if (enemy[i].velocity.y <= -80) enemy[i].velocity.y = -80;
				mov.y -= enemy[i].velocity.y * SECOND_PER_FRAME;		//加速度の計算
			}

			if (enemy[i].velocity.y > 0)
			{
				//頭上のブロックをチェックする。
				movHitInit(enemy[i].hitPosS.y); //頭上の座標系計算

				if ((BlockCheck(movHitCheckMiddle) == WALKTHROUGH && BlockCheck(movHitCheckLeft) == WALKTHROUGH && BlockCheck(movHitCheckRight) == WALKTHROUGH) ||
					(BlockCheck(movHitCheckMiddle) == PASSABLE || BlockCheck(movHitCheckLeft) == PASSABLE || BlockCheck(movHitCheckRight) == PASSABLE))
				{
					enemy[i].pos = mov;
				}
				else
				{
					enemy[i].velocity.y = 0;
					mov = enemy[i].pos;
				}
			}
			else if (enemy[i].velocity.y <= 0)
			{
				//足元のブロックっチェックする
				movHitInit(enemy[i].hitPosE.y); ////足元の座標計算

				if (BlockCheck(movHitCheckMiddle) == WALKTHROUGH
					&& BlockCheck(movHitCheckLeft) == WALKTHROUGH
					&& BlockCheck(movHitCheckRight) == WALKTHROUGH)
				{
					enemy[i].pos = mov;								//プレイヤーの座標を移動する
					enemy[i].jumpFlag = true;
				}
				else
				{
					POS tmpChipIndex = MapPosToIndex(movHitCheckMiddle);
					POS tmpChipPos = MapIndexToPos(tmpChipIndex);

					tmpChipPos.y += 16;

					if (movHitCheckMiddle.y > tmpChipPos.y)
					{
						enemy[i].pos = mov;
					}
					else
					{
						tmpIndex = MapPosToIndex(movHitCheckMiddle);			//マップ配列の場所を取る
						jumpPos = MapIndexToPosF(tmpIndex);				//足元の座標を算出：マップのブロック
						enemy[i].pos.y = jumpPos.y - enemy[i].hitPosE.y;
						enemy[i].velocity.y = 0;
						enemy[i].jumpFlag = false;

						if (	(BlockCheck(movHitCheckLeft) == CRITICAL
							||	 BlockCheck(movHitCheckRight) == CRITICAL)
							&& enemy[i].type == DEMON)
						{
							//死ぬ
							StageTiltInit(17, 8);
							tmpEffectImg = LoadEnemyImg(i);
							tmpEffectSize = LoadEnemyImgSize(i);
							SetEffect(enemy[i].pos, EFFECT_TYPE_EXPLOSION);
							PlaySoundType(SOUND_TYPE_DAMAGE_E);
							enemy[i].visible = false;
						}
					}
				}
			}

			//横velocity
			if (enemy[i].velocity.x != 0)
			{
				if (enemy[i].velocity.x > 0)
				{
					enemy[i].velocity.x -= 1;
				}
				else
				{
					enemy[i].velocity.x += 1;
				}
				
				mov.x -= enemy[i].velocity.x * SECOND_PER_FRAME;		//加速度の計算

				if (BlockCheck({ mov.x + enemy[i].hitPosS.x, enemy[i].pos.y  + enemy[i].hitPosE.y - 1 }) != UNPASSABLE
					&& BlockCheck({ mov.x + enemy[i].hitPosE.x, enemy[i].pos.y + enemy[i].hitPosE.y - 1 }) != UNPASSABLE)
				{
					enemy[i].pos = mov;
				}
			}
		}
	}

}

void EnemyGameDraw(void)
{
	int loopCnt;
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		POS_F tmpPos = AddScrollF(enemy[i].pos);

		if (enemy[i].visible)
		{
			loopCnt = 8;
			if (enemy[i].id == DRAGON_1 || DRAGON_2 || GARGOYLE) loopCnt = 4;
			else if (enemy[i].id == ORC_2 || WOLF) loopCnt = 5;

			int tmpImg = enemyImg[enemy[i].id];
			if (enemy[i].runFlag) tmpImg = enemyImgRun[enemy[i].id][(enemy[i].aniCnt / 6) % loopCnt];
			if (enemy[i].atkFlag)tmpImg = enemyImgAttack[enemy[i].id][(enemy[i].aniCnt / 6) % ENEMY_IMG_ATTACK_MAX];
			if (enemy[i].dieFlag) tmpImg = enemyImgDie[enemy[i].id][(enemy[i].aniCnt / 6) % ENEMY_IMG_DIE_MAX];
			//hp indicator
			if (enemy[i].hitpoint < 1) SetDrawBright(255, enemy[i].hitpoint * 255, enemy[i].hitpoint * 255);

			//vibration
			if (enemy[i].hitpoint <= 0.50)
			{
				tmpPos.x += (-(((1 - enemy[i].hitpoint) * 4))) + GetRand((1 - enemy[i].hitpoint) * 8);
				tmpPos.y += (-2) + GetRand(4);
			}

			if (enemy[i].dir == DIR_RIGHT)
			{
				DrawGraphF(tmpPos.x, tmpPos.y, tmpImg, true);
			}
			else
			{
				DrawTurnGraphF(tmpPos.x, tmpPos.y, tmpImg, true);
			}
			if (enemy[i].hitpoint < 1) SetDrawBright(255, 255, 255);

			//fade生成 
			if ((enemy[i].atkFlag && enemy[i].type == FLY_MELEE)
				||
				(enemy[i].speed >= 30 && enemy[i].type == BOSS1)) CharacterFadeInit(enemy[i].pos, tmpImg, 0, enemy[i].dir, 30);


			if (enemy[i].atkFlag)
			{
				if (enemy[i].type == LAND)
				{
					// Draw Sowrd Swoosh
					int tmpCnt = enemy[i].aniCnt / 6 % ENEMY_IMG_ATTACK_MAX;
					int tmpOffset = enemy[i].size.x / 2 - 96;

					if (tmpCnt == 1)
					{
						if (enemy[i].dir == DIR_RIGHT) DrawGraphF(tmpPos.x + enemy[i].size.x / 2, tmpPos.y, swooshImg[0], true); // Top Right
						else DrawTurnGraphF(tmpPos.x + tmpOffset, tmpPos.y, swooshImg[0], true); // Top Left
					}
					else if (tmpCnt == 2)
					{
						if (enemy[i].dir == DIR_RIGHT)DrawGraphF(tmpPos.x + enemy[i].size.x / 2, tmpPos.y + enemy[i].size.y / 2, swooshImg[1], true); // Bottom Right
						else DrawTurnGraphF(tmpPos.x + tmpOffset, tmpPos.y + enemy[i].size.y / 2, swooshImg[1], true); // Bottom Left
					}
				}
				if (enemy[i].aniCnt >= 6 * ENEMY_IMG_ATTACK_MAX)
				{
					enemy[i].atkFlag = false;
					enemy[i].aniCnt = 0;
				}
			}
		}

	}
}

void EnemySpawnInit(void)
{
	//GetGraphSize(enemyImg[enemy.id], &enemy.size.x, &enemy.size.y); // 画像のサイズを取る
}

void EnemyOrder(int index, bool runF, bool atkF, DIR dir, bool jumpF, double flyAngle)
{
	//enemyAI.cppからもらった命令を実行する
	enemy[index].runFlag = runF;
	if (!enemy[index].atkFlag && runF == false)
	{
		enemy[index].atkFlag = atkF;
		if (atkF) enemy[index].aniCnt = 0;
	}
	enemy[index].dir = dir;
	if (jumpF == true && enemy[index].jumpFlag == false)
	{
		enemy[index].jumpFlag = true;
		enemy[index].velocity.y = INIT_VELOCITY;
	}
	if (enemy[index].type == FLY_MELEE || enemy[index].type == FLY_RANGE)
	{
		enemy[index].flyAngle = flyAngle;
	}
}

void BossOrder(int index, double movAngle, double shotAngle, int movSpeed)
{
	enemy[index].speed = movSpeed;
	int numTmp;
	if (enemy[index].type == BOSS1) numTmp = 0;
	if (enemy[index].type == BOSS2) numTmp = 1;

	boss[numTmp].movAngle = movAngle;
	boss[numTmp].shotAngle = shotAngle;
}

POS_F GetEnemyPos(int index)
{
	return enemy[index].pos;
}

POS GetEnemyImgSize2(int index)
{
	return enemy[index].size;
}

int LoadEnemyImg(int index)
{
	return enemyImg[enemy[index].id];
}

int GetEnemyImg(void)
{
	return tmpEffectImg;
}

POS LoadEnemyImgSize(int index)
{
	return enemy[index].size;
}

POS GetEnemyImgSize(void)
{
	return tmpEffectSize;
}

bool EnemyCollideCheck(int id, POS_F pos, POS_F size)
{
	if (!enemy[id].visible) return enemy[id].visible;
	return CheckHit({ enemy[id].pos.x + enemy[id].hitPosS.x, enemy[id].pos.y + enemy[id].hitPosS.y }, pos, { enemy[id].hitPosE.x -enemy[id].hitPosS.x, enemy[id].hitPosE.y - enemy[id].hitPosS.y }, { int(size.x), int(size.y) });
}

void EnemyHitPosInit(int id)
{
	switch (id)
	{
	case BARBARIAN_1:
		enemy[enemyCnt].hitPosS = { 84,72 };
		enemy[enemyCnt].hitPosE = { 129,156 };
		break;
	case BARBARIAN_2:
		enemy[enemyCnt].hitPosS = { 87,66 };
		enemy[enemyCnt].hitPosE = { 126,156 };
		break;
	case DRAGON_1:
	case DRAGON_2:
		enemy[enemyCnt].hitPosS = { 30,45 };
		enemy[enemyCnt].hitPosE = { 188,192 };
		break;
	case DWARF_1:
	case DWARF_2:
		enemy[enemyCnt].hitPosS = { 96,90 };
		enemy[enemyCnt].hitPosE = { 150,168 };
		break;
	case FIRE_DEMON:
		enemy[enemyCnt].hitPosS = { 96,71 };
		enemy[enemyCnt].hitPosE = { 150,168 };
		break;
	case GARGOYLE:
		enemy[enemyCnt].hitPosS = { 104,69 };
		enemy[enemyCnt].hitPosE = { 153,153 };
		break;
	case GOBLIN_1:
		enemy[enemyCnt].hitPosS = { 87,72 };
		enemy[enemyCnt].hitPosE = { 126,156 };
		break;
	case GOBLIN_2:
		enemy[enemyCnt].hitPosS = { 62,60 };
		enemy[enemyCnt].hitPosE = { 120,144 };
		break;
	case KNIGHT:
		enemy[enemyCnt].hitPosS = { 90,77 };
		enemy[enemyCnt].hitPosE = { 129,156 };
		break;
	case LIZARD_MAN_1:
		enemy[enemyCnt].hitPosS = { 72,66 };
		enemy[enemyCnt].hitPosE = { 126,144 };
		break;
	case LIZARD_MAN_2:
		enemy[enemyCnt].hitPosS = { 74,63 };
		enemy[enemyCnt].hitPosE = { 135,156 };
		break;
	case MEDUSA:
		enemy[enemyCnt].hitPosS = { 75,90 };
		enemy[enemyCnt].hitPosE = { 141,168 };
		break;
	case MINOTAUR:
		enemy[enemyCnt].hitPosS = { 95,77 };
		enemy[enemyCnt].hitPosE = { 168,168 };
		break;
	case ORC_1:
		enemy[enemyCnt].hitPosS = { 81,63 };
		enemy[enemyCnt].hitPosE = { 132,156 };
		break;
	case ORC_2:
		enemy[enemyCnt].hitPosS = { 69,69 };
		enemy[enemyCnt].hitPosE = { 216,240 };
		break;
	case PEASANT:
		enemy[enemyCnt].hitPosS = { 90,69 };
		enemy[enemyCnt].hitPosE = { 129,150 };
		break;
	case PRINCESS:
		enemy[enemyCnt].hitPosS = { 87,75 };
		enemy[enemyCnt].hitPosE = { 129,156 };
		break;
	case SCORPION:
		enemy[enemyCnt].hitPosS = { 42,92 };
		enemy[enemyCnt].hitPosE = { 282,261 };
		break;
	case SKELETON_1:
		enemy[enemyCnt].hitPosS = { 86,75 };
		enemy[enemyCnt].hitPosE = { 131,155 };
		break;
	case SKELETON_2:
		enemy[enemyCnt].hitPosS = { 74,62 };
		enemy[enemyCnt].hitPosE = { 120,144 };
		break;
	case SKELETON_3:
		enemy[enemyCnt].hitPosS = { 66,53 };
		enemy[enemyCnt].hitPosE = { 122,143 };
		break;
	case SOLDIER:
		enemy[enemyCnt].hitPosS = { 90,66 };
		enemy[enemyCnt].hitPosE = { 129,156 };
		break;
	case SPIDER:
		enemy[enemyCnt].hitPosS = { 51,123 };
		enemy[enemyCnt].hitPosE = { 237,240 };
		break;
	case TREANT:
		enemy[enemyCnt].hitPosS = { 99,66 };
		enemy[enemyCnt].hitPosE = { 147,168 };
		break;
	case TROLL:
		enemy[enemyCnt].hitPosS = { 66,48 };
		enemy[enemyCnt].hitPosE = { 126,144 };
		break;
	case WIZARD:
		enemy[enemyCnt].hitPosS = { 69,54 };
		enemy[enemyCnt].hitPosE = { 120,144 };
		break;
	case WOLF:
		enemy[enemyCnt].hitPosS = { 42,111 };
		enemy[enemyCnt].hitPosE = { 195,216 };
		break;
	default:
		enemy[enemyCnt].hitPosS = { 0,0 };
		enemy[enemyCnt].hitPosE = { 0,0 };
		break;
	}
}

bool EnemyGetHit(int index, DIR dir)
{
	//ボス
	if (boss[0].index == index && enemy[index].id == DRAGON_1)
	{
		if (!boss[0].shield && boss[1].shield)
		{
			if (ShieldDamage() == true)
			{
				boss[0].lives--;
				boss[0].shield = true;
				boss[1].shield = false;
				EffectShieldInit(boss[0].index, 10);
				StageTiltInit(17, 8);

				if (boss[1].lives == 0)
				{
					StageTiltInit(30, 255);
					tmpEffectImg = LoadEnemyImg(index);
					tmpEffectSize = LoadEnemyImgSize(index);
					SetEffect(enemy[index].pos, EFFECT_TYPE_EXPLOSION);
					PlaySoundType(SOUND_TYPE_DAMAGE_E);
					enemy[index].visible = false;
					endFlag = true;
					FadeInit(255);
					StopSoundType(SOUND_TYPE_BOSS);
					PlaySoundType(SOUND_TYPE_BOSSDE1);
				}
			}
		}
		else if (boss[0].shield)
		{
			//damage blocked
			PlaySoundType(SOUND_TYPE_SHIELDH);
			return true;
		}
		return false;
	}
	else if (boss[1].index == index && enemy[index].id == DRAGON_2)
	{
		if (!boss[1].shield && boss[0].shield)
		{
			if (ShieldDamage() == true)
			{
				if (boss[1].lives == 3)
				{
					EnemyInit(DRAGON_1, {SCREEN_SIZE_X / 2, enemy[boss[1].index].pos.y - SCREEN_SIZE_Y / 2});
					PlaySoundType(SOUND_TYPE_BOSSEN2);
				}
				boss[1].lives--;
				boss[1].shield = true;
				boss[0].shield = false;
				EffectShieldInit(boss[1].index, 10);
				StageTiltInit(17, 8);
				PlaySoundType(SOUND_TYPE_SHIELD);

				if (boss[1].lives == 0)
				{
					StageTiltInit(30, 20);
					tmpEffectImg = LoadEnemyImg(index);
					tmpEffectSize = LoadEnemyImgSize(index);
					SetEffect(enemy[index].pos, EFFECT_TYPE_EXPLOSION);
					PlaySoundType(SOUND_TYPE_DAMAGE_E);
					enemy[index].visible = false;
					LastBossSwitch(true);
					PlaySoundType(SOUND_TYPE_BOSSDE1);
				}
			}
		}
		else if (boss[1].shield)
		{
			PlaySoundType(SOUND_TYPE_SHIELDH);
			return true;
		}
		return false;
	}

	enemy[index].hitpoint -= EnemyGetHitDamage(enemy[index].id);
	enemy[index].hitFlag = true;
	enemy[index].hitFlagCooldown = ENEMY_REGENERATE_COOLDOWN;

	if (enemy[index].hitpoint <= 0)
	{
		StageTiltInit(17, 8);
		tmpEffectImg = LoadEnemyImg(index);
		tmpEffectSize = LoadEnemyImgSize(index);
		SetEffect(enemy[index].pos, EFFECT_TYPE_EXPLOSION);
		PlaySoundType(SOUND_TYPE_DAMAGE_E);
		enemy[index].visible = false;

		int scoreTmp = EnemyGetScore(enemy[index].id);
		if (scoreTmp > 0) SetScoreText({ enemy[index].pos.x + (enemy[index].hitPosS.x + enemy[index].hitPosE.x) / 2,
			enemy[index].pos.y + (enemy[index].hitPosS.y + enemy[index].hitPosE.y) / 2 }, scoreTmp, 0x00AA00, 75 + (scoreTmp / 5));
	}

	//特殊処理が必要な敵タイプ
	if (enemy[index].id != FIRE_DEMON && enemy[index].type != FLY_MELEE && enemy[index].type != MOUNT) return false;

	if (dir == DIR_LEFT)
	{
		enemy[index].velocity.x += GetRand(5) + 10;
	}
	else
	{
		enemy[index].velocity.x -= GetRand(5) + 10;
	}
}

bool GetEnemyFlag(int index)
{
	return enemy[index].visible;
}

int GetEnemyAttackInterval(int type)
{
	int intervalTmp;

	switch (type)
	{
	case BARBARIAN_1:
		intervalTmp = 100;
		break;
	case BARBARIAN_2:
		intervalTmp = 100;
		break;
	case DRAGON_1:
		intervalTmp = 99;
		break;
	case DRAGON_2:
		intervalTmp = 50;
		break;
	case DWARF_1:
	case DWARF_2:
		intervalTmp = 100;
		break;
	case FIRE_DEMON:
		intervalTmp = 10;
		break;
	case GARGOYLE:
		intervalTmp = 100;
		break;
	case GOBLIN_1:
		intervalTmp = 100;
		break;
	case GOBLIN_2:
		intervalTmp = 100;
		break;
	case KNIGHT:
		intervalTmp = 100;
		break;
	case LIZARD_MAN_1:
		intervalTmp = 100;
		break;
	case LIZARD_MAN_2:
		intervalTmp = 100;
		break;
	case MEDUSA:
		intervalTmp = 70;
		break;
	case MINOTAUR:
		intervalTmp = 100;
		break;
	case ORC_1:
		intervalTmp = 100;
		break;
	case ORC_2:
		intervalTmp = 100;
		break;
	case PEASANT:
		intervalTmp = 100;
		break;
	case PRINCESS:
		intervalTmp = 80;
		break;
	case SCORPION:
		intervalTmp = 100;
		break;
	case SKELETON_1:
		intervalTmp = 50;
		break;
	case SKELETON_2:
		intervalTmp = 100;
		break;
	case SKELETON_3:
		intervalTmp = 90;
		break;
	case SOLDIER:
		intervalTmp = 100;
		break;
	case SPIDER:
		intervalTmp = 100;
		break;
	case TREANT:
		intervalTmp = 100;
		break;
	case TROLL:
		intervalTmp = 100;
		break;
	case WIZARD:
		intervalTmp = 100;
		break;
	case WOLF:
		intervalTmp = 5;
		break;
	default:
		intervalTmp = 100;
		break;
	}

	return intervalTmp;
}

float EnemyGetHitDamage(int type)
{
	float damageTmp;

	switch (type)
	{
	case BARBARIAN_1:
		damageTmp = 0.25;
		break;
	case BARBARIAN_2:
		damageTmp = 0.25;
		break;
	case DRAGON_1:
		damageTmp = 0.01;
		break;
	case DRAGON_2:
		damageTmp = 0.02;
		break;
	case DWARF_1:
	case DWARF_2:
		damageTmp = 0.25;
		break;
	case FIRE_DEMON:
		damageTmp = 0.01;
		break;
	case GARGOYLE:
		damageTmp = 0.85;
		break;
	case GOBLIN_1:
		damageTmp = 0.25;
		break;
	case GOBLIN_2:
		damageTmp = 0.25;
		break;
	case KNIGHT:
		damageTmp = 0.25;
		break;
	case LIZARD_MAN_1:
		damageTmp = 0.25;
		break;
	case LIZARD_MAN_2:
		damageTmp = 0.25;
		break;
	case MEDUSA:
		damageTmp = 0.11;
		break;
	case MINOTAUR:
		damageTmp = 0.25;
		break;
	case ORC_1:
		damageTmp = 0.25;
		break;
	case ORC_2:
		damageTmp = 0.25;
		break;
	case PEASANT:
		damageTmp = 0.25;
		break;
	case PRINCESS:
		damageTmp = 0.30;
		break;
	case SCORPION:
		damageTmp = 0.25;
		break;
	case SKELETON_1:
		damageTmp = 0.29 + GetRand(0.02);
		break;
	case SKELETON_2:
		damageTmp = 1 - (0.2 * GetLivingPlayerCount());
		break;
	case SKELETON_3:
		damageTmp = 0.30 - (0.05 * GetLivingPlayerCount());
		break;
	case SOLDIER:
		damageTmp = 0.25;
		break;
	case SPIDER:
		damageTmp = 0.25;
		break;
	case TREANT:
		damageTmp = 0.25;
		break;
	case TROLL:
		damageTmp = 0.25;
		break;
	case WIZARD:
		damageTmp = 0.25;
		break;
	case WOLF:
		damageTmp = 0.15;
		break;
	default:
		damageTmp = 0.25;
		break;
	}

	return damageTmp;
}

int EnemyGetScore(int type)
{
	float scoreTmp;

	switch (type)
	{
	case DRAGON_1:
	case DRAGON_2:
		scoreTmp = 0;
		break;
	case DWARF_1:
	case DWARF_2:
		scoreTmp = 0;
		break;
	case FIRE_DEMON:
		scoreTmp = 500;
		break;
	case GARGOYLE:
		scoreTmp = 150;
		break;
	case MEDUSA:
		scoreTmp = 300;
		break;
	case PRINCESS:
		scoreTmp = 300;
		break;
	case SKELETON_1:
		scoreTmp = 50;
		break;
	case SKELETON_2:
		scoreTmp = 75;
		break;
	case SKELETON_3:
		scoreTmp = 500;
		break;
	case WOLF:
		scoreTmp = 300;
		break;
	default:
		scoreTmp = 0;
		break;
	}

	return scoreTmp;
}

void BossEffect(int index)
{
	if (enemy[index].hitpoint <= 0.5) return;
	enemy[index].hitpoint -= EnemyGetHitDamage(enemy[index].id);
	enemy[index].hitFlag = true;
	enemy[index].hitFlagCooldown = ENEMY_REGENERATE_COOLDOWN/2;
}

void MoveBossToStart(void)
{
	enemy[boss[0].index].pos.y += (MAP_CHIP_Y * MAP_CHIP_SIZE_Y - SCREEN_SIZE_Y) - (20 * MAP_CHIP_SIZE_Y - SCREEN_SIZE_Y);
	enemy[boss[1].index].pos.y += (MAP_CHIP_Y * MAP_CHIP_SIZE_Y - SCREEN_SIZE_Y) - (20 * MAP_CHIP_SIZE_Y - SCREEN_SIZE_Y);
}