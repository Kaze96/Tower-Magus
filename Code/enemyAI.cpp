#include "main.h"
#include "enemyAI.h"
#include "enemy.h"
#include "checkhit.h"
#include "blockcheck.h"
#include "stage.h"
#include "character.h"
#include <math.h>
#include <cmath> 
#include "sound.h"

#define POSSIBLE_PLAYER 4

ENEMY_AI enemyAI[ENEMY_MAX];
int bossAI[2];
bool lastBossSwitch;	//ゲームの最後にonにして　一番難しいパターンを出す

float DistanceCalculation(POS_F p1, POS_F p2);
int TargetPlayer(int originalTarget, bool distanceCompare, POS_F AIpos);

void EnemyAISysInit(void)
{
	for (int i = 0; i <= ENEMY_MAX; i++)
	{
		enemyAI[i].atkFlag = false;
		enemyAI[i].dieFlag = false;
		enemyAI[i].dir = DIR_RIGHT;
		enemyAI[i].lifeFlag = false;
		enemyAI[i].runFlag = false;
		enemyAI[i].hitPosS = { 0,0 };
		enemyAI[i].hitPosE = { 0,0 };
		enemyAI[i].speed = 0;
		enemyAI[i].attackRange = 0; 
		enemyAI[i].flyAngle = 0;
		enemyAI[i].targetPlayer = -1;
	}
	bossAI[0] = 0;
	bossAI[1] = 0;
	lastBossSwitch = false;
}

void EnemyAIInit(int index, int speed, int atkspeed, DIR dir, POS hitPosS, POS hitPosE)
{
	//constant variable
	enemyAI[index].lifeFlag = true;
	enemyAI[index].atkFlag = false;
	enemyAI[index].dieFlag = false;
	enemyAI[index].runFlag = false;
	enemyAI[index].jumpFlag = false;
	enemyAI[index].targetPlayer = -1;	//-1はまだ何も決めていない
	//manipulate variable
	enemyAI[index].dir = dir;
	enemyAI[index].hitPosS = hitPosS;
	enemyAI[index].hitPosE = hitPosE;
	enemyAI[index].speed = speed;
	enemyAI[index].attackRange = hitPosE.x - hitPosS.x;
	enemyAI[index].attackInterval = atkspeed;
	enemyAI[index].attackIntervalCnt = enemyAI[index].attackInterval;
}

ENEMY_TYPE EnemyClassification(int id)
{
	//ここで敵を分類しています
	ENEMY_TYPE temp;
	switch (id)
	{
	case BARBARIAN_1:
	case BARBARIAN_2:
	case DWARF_1:
	case DWARF_2:
	case GOBLIN_1:
	case KNIGHT:
	case ORC_1:
	case LIZARD_MAN_2:
	case MINOTAUR:
	case PRINCESS:
	case SKELETON_1:
	case SOLDIER:
	case TREANT:
	case TROLL:
	case WIZARD:
		temp = LAND;
		break;
	case GARGOYLE:
		temp = FLY_MELEE;
		break;
	case GOBLIN_2:
	case LIZARD_MAN_1:
	case MEDUSA:
	case SKELETON_2:
	case SKELETON_3:
		temp = RANGED;
		break;
	case ORC_2:
	case SCORPION:
	case SPIDER:
	case WOLF:
		temp = MOUNT;
		break;
	case FIRE_DEMON:
		temp = DEMON;
		break;
	case DRAGON_1:
		temp = BOSS1;
		bossAI[0] = 0;
	break;
	case DRAGON_2:
		temp = BOSS2;
		bossAI[1] = 0;
		break;
	default:
		temp = ENEMY_TYPE_MAX;
		break;
	}

	return temp;
}

bool EnemyAIMain(int index, ENEMY_TYPE type, POS_F pos)
{
	//プレイヤースクリーンにいる敵しかAIは付ける
	POS_F tempPos = AddScrollF(pos);
	if (tempPos.y + enemyAI[index].hitPosE.y < 0 && type != BOSS1) return false;	//スクリーン外です

	//この先に道があるかチェック
	POS_F movHitCheckLeft = pos;
	POS_F movHitCheckRight = pos;
		//左下
	movHitCheckLeft.x += enemyAI[index].hitPosS.x;
	movHitCheckLeft.y = movHitCheckLeft.y + enemyAI[index].hitPosE.y + 1;
		//右下
	movHitCheckRight.x += enemyAI[index].hitPosE.x;
	movHitCheckRight.y = movHitCheckRight.y + enemyAI[index].hitPosE.y + 1;

	enemyAI[index].attackIntervalCnt--;	//攻撃頻度を計算する

	//とりあえずフラグをfalseにする
	enemyAI[index].runFlag = false;
	enemyAI[index].atkFlag = false;
	enemyAI[index].jumpFlag = false;

	POS_F targetPos;
	//どの行動種類か分類する
	switch (type)
	{
	case LAND:
		//立ってるブロックしか歩かない敵。ジャンプ知能は与えていない

		//＝＝＝＝移動
		if (enemyAI[index].dir == DIR_LEFT)
		{
			tempPos.x = pos.x + enemyAI[index].hitPosS.x - enemyAI[index].speed; //移動先
			tempPos.y = pos.y + ((enemyAI[index].hitPosS.y + enemyAI[index].hitPosE.y) / 2);//中間Y位置を取る
			if (BlockCheck(tempPos) == WALKTHROUGH
				&& BlockCheck(movHitCheckLeft) != WALKTHROUGH)
			{
				enemyAI[index].runFlag = true;
				//攻撃範囲
				tempPos.x -= enemyAI[index].attackRange;
			}
			else
			{
				enemyAI[index].dir = DIR_RIGHT;
				enemyAI[index].runFlag = false;
			}
		}
		else if (enemyAI[index].dir == DIR_RIGHT)
		{
			tempPos.x = pos.x + enemyAI[index].hitPosE.x + enemyAI[index].speed; //移動先
			tempPos.y = pos.y + ((enemyAI[index].hitPosS.y + enemyAI[index].hitPosE.y) / 2);//中間Y位置を取る
			if (BlockCheck(tempPos) == WALKTHROUGH
				&& BlockCheck(movHitCheckRight) != WALKTHROUGH)
			{
				enemyAI[index].runFlag = true;
			}
			else
			{
				enemyAI[index].dir = DIR_LEFT;
				enemyAI[index].runFlag = false;
				//攻撃範囲
				tempPos.x -= enemyAI[index].attackRange;
			}
		}

		//＝＝＝＝攻撃行為
		for (int j = 0; j < POSSIBLE_PLAYER; j++)
		{
			//攻撃範囲内か判定チェック
			if (PlayerCollideCheck(j, tempPos, { enemyAI[index].attackRange , 5 }) == true && enemyAI[index].attackIntervalCnt <= 0)
			{
				enemyAI[index].runFlag = false;	//移動しないにする
				enemyAI[index].atkFlag = true;
				enemyAI[index].attackIntervalCnt = enemyAI[index].attackInterval;
				PlaySoundType(SOUND_TYPE_SLASH);
			}
		}

		//攻撃終わったら向きを反対にする
		if (enemyAI[index].attackInterval - enemyAI[index].attackIntervalCnt == 6 * ENEMY_IMG_ATTACK_MAX)
		{
			enemyAI[index].dir == DIR_RIGHT ? enemyAI[index].dir = DIR_LEFT : enemyAI[index].dir = DIR_RIGHT;
		}
		break;
	case RANGED:
		//基本移動しない敵。遠いから攻撃してくる

		//＝＝＝＝攻撃行為
		tempPos.x = pos.x + ((enemyAI[index].hitPosS.x + enemyAI[index].hitPosE.x) / 2);//中間X位置を取る
		tempPos.y = pos.y + ((enemyAI[index].hitPosS.y + enemyAI[index].hitPosE.y) / 2);//中間Y位置を取る

		POS_F playerPos;
		POS_F diff;
		POS_F playerTargetPos;
		POS_F tempScrollPos;
		for (int j = 0; j < POSSIBLE_PLAYER; j++)
		{
			//攻撃範囲内か判定チェック
			if (PlayerCollideCheck(j, { 0 , tempPos.y - (SCREEN_SIZE_Y / 3) }, { SCREEN_SIZE_X , (SCREEN_SIZE_Y / 3) * 2 }) == true 
				&& enemyAI[index].attackIntervalCnt <= 0 
				&& GetPlayerFlag(j))
			{
				enemyAI[index].atkFlag = true;
				enemyAI[index].attackIntervalCnt = enemyAI[index].attackInterval;
				playerPos = GetPlayerCenterPos(j);
				if (j != enemyAI[index].targetPlayer)enemyAI[index].targetPlayer = j;
				playerPos.x <= tempPos.x ? enemyAI[index].dir = DIR_LEFT : enemyAI[index].dir = DIR_RIGHT;
			}
		}
		break;
	case FLY_MELEE:
		//常に飛んでいる敵。　プレイヤーに寄って攻撃する
		//移動
		tempPos.x = pos.x + ((enemyAI[index].hitPosS.x + enemyAI[index].hitPosE.x) / 2);//中間X位置を取る
		tempPos.y = pos.y + ((enemyAI[index].hitPosS.y + enemyAI[index].hitPosE.y) / 2);//中間Y位置を取る

		//ターゲットを変える
		if (!GetPlayerFlag(enemyAI[index].targetPlayer) || enemyAI[index].attackIntervalCnt == 0)
		{
			enemyAI[index].targetPlayer = TargetPlayer(enemyAI[index].targetPlayer, true, tempPos);
		}

		playerTargetPos = GetPlayerCenterPos(enemyAI[index].targetPlayer);	//決まったターゲットプレイヤーの座標を取る
		//プレイヤーを向けて移動する
		if (enemyAI[index].attackIntervalCnt <= 0)
		{
			playerTargetPos.x <= tempPos.x ? enemyAI[index].dir = DIR_LEFT : enemyAI[index].dir = DIR_RIGHT;
			//移動角度を計算する
			enemyAI[index].flyAngle = (atan2(playerTargetPos.y - tempPos.y, playerTargetPos.x - tempPos.x));
			enemyAI[index].flyAngle += ((double)rand() * (1 - 0.3)) / (double)RAND_MAX + 0.3;
		}

		//＝＝＝＝攻撃行為
		tempPos.y = pos.y + ((enemyAI[index].hitPosS.y + enemyAI[index].hitPosE.y) / 2);//中間Y位置を取る

		tempPos.x += cos(enemyAI[index].flyAngle) * enemyAI[index].hitPosS.x;
		tempPos.y += sin(enemyAI[index].flyAngle) * enemyAI[index].hitPosS.y;

		for (int j = 0; j < 4; j++)
		{
			//攻撃範囲内か判定チェック
			if (PlayerCollideCheck(j, tempPos, { enemyAI[index].hitPosS.x , enemyAI[index].hitPosS.y}) == true && enemyAI[index].attackIntervalCnt <= 0)
			{
				enemyAI[index].atkFlag = true;
				enemyAI[index].attackIntervalCnt = enemyAI[index].attackInterval;
			}
		}
		break;
	case FLY_RANGE:
		//常に飛んでいる敵。遠距離からプレイヤーを攻撃する
		break;
	case MOUNT:
		//ジャンプ機能ついてる敵
		//移動
		tempPos.x = pos.x + ((enemyAI[index].hitPosS.x + enemyAI[index].hitPosE.x) / 2);//中間X位置を取る
		tempPos.y = pos.y + ((enemyAI[index].hitPosS.y + enemyAI[index].hitPosE.y) / 2);//中間Y位置を取る

		//ターゲットを変える
		enemyAI[index].targetPlayer = TargetPlayer(enemyAI[index].targetPlayer, true, tempPos);

		playerTargetPos = GetPlayerCenterPos(enemyAI[index].targetPlayer);	//決まったターゲットプレイヤーの座標を取る

		diff.x = abs(tempPos.x - playerTargetPos.x); //Xの差を求める
		diff.y = abs(tempPos.y - playerTargetPos.y); //Yの差を求める

		//プレイヤーを向けて移動する
		playerTargetPos.x <= tempPos.x ? enemyAI[index].dir = DIR_LEFT : enemyAI[index].dir = DIR_RIGHT;

		//移動する必要かあるか判定する
		if (DistanceCalculation(GetPlayerCenterPos(enemyAI[index].targetPlayer), tempPos) > (enemyAI[index].speed + enemyAI[index].hitPosS.x))
		{
			enemyAI[index].runFlag = true;
		}

		//＝＝＝＝移動
		if (enemyAI[index].runFlag)
		{

			if (enemyAI[index].dir == DIR_LEFT)
			{
				tempPos.x = pos.x + enemyAI[index].hitPosS.x - enemyAI[index].speed; //移動先
				tempPos.y = tempPos.y = pos.y + enemyAI[index].hitPosE.y;//足元Yを取る
				if (BlockCheck(movHitCheckLeft) != WALKTHROUGH)
				{
					enemyAI[index].dir = DIR_LEFT;
				}
				else
				{
					//ジャンプ
					if (diff.x > enemyAI[index].hitPosE.x && diff.y < enemyAI[index].hitPosE.y)
					{
						enemyAI[index].jumpFlag = true;
					}
				}
			}
			else if (enemyAI[index].dir == DIR_RIGHT)
			{
				tempPos.x = pos.x + enemyAI[index].hitPosE.x + enemyAI[index].speed; //移動先
				tempPos.y = pos.y + enemyAI[index].hitPosE.y;//足元Yを取る
				if (BlockCheck(movHitCheckRight) != WALKTHROUGH)
				{
					enemyAI[index].dir = DIR_RIGHT;
				}
				else
				{
					//ジャンプ
					if (diff.x > enemyAI[index].hitPosE.x && diff.y < enemyAI[index].hitPosE.y)
					{
						enemyAI[index].jumpFlag = true;
					}
				}
			}
		}

		//＝＝＝＝攻撃行為
		tempPos.y = pos.y + ((enemyAI[index].hitPosS.y + enemyAI[index].hitPosE.y) / 2);//中間Y位置を取る
		for (int j = 0; j < 4; j++)
		{
			//攻撃範囲内か判定チェック
			if (PlayerCollideCheck(j, tempPos, { 50 , 5 }) == true && enemyAI[index].attackIntervalCnt <= 0)
			{
				enemyAI[index].runFlag = false;	//移動しないにする
				enemyAI[index].atkFlag = true;
				enemyAI[index].attackIntervalCnt = enemyAI[index].attackInterval;
			}
		}

		//攻撃終わったらターゲットをランダムで変える
		enemyAI[index].targetPlayer = TargetPlayer(enemyAI[index].targetPlayer, true, tempPos);
		break;
	case DEMON:
		//右だけ移動する
		enemyAI[index].dir = DIR_RIGHT;
		tempPos.x = pos.x + enemyAI[index].hitPosE.x + enemyAI[index].speed; //移動先
		tempPos.y = pos.y + ((enemyAI[index].hitPosS.y + enemyAI[index].hitPosE.y) / 2);//中間Y位置を取る
		if (BlockCheck(tempPos) == WALKTHROUGH
			&& BlockCheck(movHitCheckRight) != WALKTHROUGH)
		{
			enemyAI[index].runFlag = true;
		}
		else
		{
			enemyAI[index].runFlag = false;
			//攻撃範囲の定義
			tempPos.x -= enemyAI[index].attackRange;
		}

		//攻撃
		for (int j = 0; j < POSSIBLE_PLAYER; j++)
		{
			//攻撃範囲内か判定チェック
			if (PlayerCollideCheck(j, pos, { enemyAI[index].hitPosS.x + enemyAI[index].hitPosE.x , enemyAI[index].hitPosS.y + enemyAI[index].hitPosE.y }) == true && enemyAI[index].attackIntervalCnt <= 0)
			{
				enemyAI[index].runFlag = false;	//移動しないにする
				enemyAI[index].atkFlag = true;
				enemyAI[index].attackIntervalCnt = enemyAI[index].attackInterval;
			}
		}
		break;
	case BOSS1:
		double angleTemp;
		tempPos.x = pos.x + ((enemyAI[index].hitPosS.x + enemyAI[index].hitPosE.x) / 2);//中間X位置を取る
		tempPos.y = pos.y + ((enemyAI[index].hitPosS.y + enemyAI[index].hitPosE.y) / 2);//中間Y位置を取る
		if (CheckHitKey(KEY_INPUT_F6))
		{
			bossAI[0] = 0;
		}
		//スクリーンの上位置
		tempScrollPos = tempPos;
		tempScrollPos = AddScrollF(tempScrollPos);
		switch (bossAI[0])
		{
		case 0:
			//intro
			BossOrder(index, 1.5708, 0, 1);
			enemyAI[index].runFlag = true;

			if (tempScrollPos.y >= 200)
			{
				bossAI[0] = 1; 
				enemyAI[index].targetPlayer = TargetPlayer(enemyAI[index].targetPlayer, true, tempPos);
			}
			break;
		case 1:
			//プレイヤーのY水平位置まで移動する
			playerTargetPos = GetPlayerCenterPos(enemyAI[index].targetPlayer);	//決まったターゲットプレイヤーの座標を取る
			playerTargetPos.y += 5;
			playerTargetPos.x <= tempPos.x ? enemyAI[index].dir = DIR_LEFT : enemyAI[index].dir = DIR_RIGHT;
			playerTargetPos.y <= tempPos.y ? angleTemp = -1.5708 : angleTemp = 1.5708;
			enemyAI[index].runFlag = true;
			enemyAI[index].atkFlag = false;

			BossOrder(index,
				angleTemp,
				angleTemp, enemyAI[index].speed * 2);

			if (tempScrollPos.y <= 0 || tempScrollPos.y >= SCREEN_SIZE_Y)
			{
				bossAI[0] = 7;
			}

			if (tempPos.y >= (playerTargetPos.y - 8) &&
				tempPos.y <= (playerTargetPos.y + 8))
			{
				bossAI[0] = 2;
				enemyAI[index].targetPlayer = TargetPlayer(enemyAI[index].targetPlayer, true, tempPos);
				enemyAI[index].attackIntervalCnt = enemyAI[index].attackInterval;
			}
			break;
		case 2:
			//攻撃を貯める
			BossEffect(index);
			enemyAI[index].runFlag = false;
			BossOrder(index, 0, 0, enemyAI[index].speed * 10);

			if (enemyAI[index].attackIntervalCnt <= enemyAI[index].attackInterval / 2)
			{
				PlaySoundType(SOUND_TYPE_CHARGE);
			}
			if (enemyAI[index].attackIntervalCnt == 0)
			{
				int patternTemp[3] = {3, 5, 6};
				bossAI[0] = patternTemp[GetRand(2)];
				PlaySoundType(SOUND_TYPE_MOVEMENT);
			}
			else if (enemyAI[index].attackIntervalCnt >= 10)
			{
				BossOrder(index, 0, 0, 0);
			}
			break;
		case 3:
			//攻撃行為
			enemyAI[index].dir == DIR_LEFT ? angleTemp = 3.142 : angleTemp = 0;
			enemyAI[index].runFlag = true;
			enemyAI[index].atkFlag = true;

			BossOrder(index, angleTemp, angleTemp, enemyAI[index].speed * 10);

			if ((enemyAI[index].dir == DIR_LEFT && tempScrollPos.x < 100)
				|| (enemyAI[index].dir == DIR_RIGHT && (tempScrollPos.x > SCREEN_SIZE_X - 150)))
			{
				StageTiltInit(17, 7);
				PlaySoundType(SOUND_TYPE_WALLIMPACT);
				if (GetRand(1) == 0)
				{
					playerTargetPos = GetPlayerCenterPos(enemyAI[index].targetPlayer);	//ターゲットプレイヤーの座標を取る
					if (tempPos.y < playerTargetPos.y - 100)
					{
						bossAI[0] = 5;
					}
					else if (tempPos.y > playerTargetPos.y + 100)
					{
						bossAI[0] = 6;
					}
					BossEffect(index);
					PlaySoundType(SOUND_TYPE_MOVEMENT);
					enemyAI[index].dir == DIR_LEFT ? enemyAI[index].dir = DIR_RIGHT : enemyAI[index].dir = DIR_LEFT;
				}
				else
				{
					bossAI[0] = 4;
				}

				if (lastBossSwitch)
				{
					int patternTemp[5] = { 4, 3, 5, 6, 7 };
					bossAI[0] = patternTemp[GetRand(4)];
				}
			}
			break;
		case 4:
			//真ん中に戻る
			enemyAI[index].dir == DIR_LEFT ? angleTemp = 0 : angleTemp = 3.142;
			enemyAI[index].runFlag = true;

			BossOrder(index, angleTemp, angleTemp, 5);

			if (tempScrollPos.x >= (SCREEN_SIZE_X / 2) - 5 &&
				tempScrollPos.x <= (SCREEN_SIZE_X / 2) + 5)
			{
				bossAI[0] = 7;
				enemyAI[index].targetPlayer = TargetPlayer(enemyAI[index].targetPlayer, true, tempPos);
			}
			break;
		case 5:
			//下斜め攻撃
			if (GetRand(1) == 0)
			{
				enemyAI[index].dir == DIR_LEFT ? angleTemp = 3 : angleTemp = 0.142;
			}
			else
			{
				enemyAI[index].dir == DIR_LEFT ? angleTemp = 3.284 : angleTemp = -0.142;
			}
			enemyAI[index].runFlag = true;
			enemyAI[index].atkFlag = true;

			BossOrder(index, angleTemp, angleTemp, enemyAI[index].speed * 10);

			if ((enemyAI[index].dir == DIR_LEFT && tempScrollPos.x < 100)
				|| (enemyAI[index].dir == DIR_RIGHT && (tempScrollPos.x > SCREEN_SIZE_X - 150)))
			{
				StageTiltInit(17, 7);
				PlaySoundType(SOUND_TYPE_WALLIMPACT);
				if (GetRand(1) == 0)
				{
					playerTargetPos = GetPlayerCenterPos(enemyAI[index].targetPlayer);	//ターゲットプレイヤーの座標を取る
					if (tempPos.y < playerTargetPos.y - 100)
					{
						bossAI[0] = 5;
					}
					else if (tempPos.y > playerTargetPos.y + 100)
					{
						bossAI[0] = 6;
					}
					BossEffect(index);
					PlaySoundType(SOUND_TYPE_MOVEMENT);
					enemyAI[index].dir == DIR_LEFT ? enemyAI[index].dir = DIR_RIGHT : enemyAI[index].dir = DIR_LEFT;
				}
				else
				{
					bossAI[0] = 4;
				}

				if (lastBossSwitch)
				{
					int patternTemp[5] = { 4, 3, 5, 6, 7 };
					bossAI[0] = patternTemp[GetRand(4)];
				}
			}
			break;
		case 6:
			//上斜め攻撃
			enemyAI[index].dir == DIR_LEFT ? angleTemp = 3.284 : angleTemp = -0.142;
			enemyAI[index].runFlag = true;
			enemyAI[index].atkFlag = true;

			BossOrder(index, angleTemp, angleTemp, enemyAI[index].speed * 10);

			if ((enemyAI[index].dir == DIR_LEFT && tempScrollPos.x < 100)
				|| (enemyAI[index].dir == DIR_RIGHT && (tempScrollPos.x > SCREEN_SIZE_X - 150)))
			{
				StageTiltInit(17, 7);
				PlaySoundType(SOUND_TYPE_WALLIMPACT);
				if (GetRand(1) == 0)
				{
					playerTargetPos = GetPlayerCenterPos(enemyAI[index].targetPlayer);	//ターゲットプレイヤーの座標を取る
					if (tempPos.y < playerTargetPos.y - 100)
					{
						bossAI[0] = 5;
					}
					else if (tempPos.y > playerTargetPos.y + 100)
					{
						bossAI[0] = 6;
					}
					BossEffect(index);
					PlaySoundType(SOUND_TYPE_MOVEMENT);
					enemyAI[index].dir == DIR_LEFT ? enemyAI[index].dir = DIR_RIGHT : enemyAI[index].dir = DIR_LEFT;
				}
				else
				{
					bossAI[0] = 4;
				}

				if (lastBossSwitch)
				{
					int patternTemp[5] = { 4, 3, 5, 6, 7 };
					bossAI[0] = patternTemp[GetRand(4)];
				}
			}
			break;
		case 7:
			//プレイヤー探索
			if (!GetPlayerFlag(enemyAI[index].targetPlayer))
			{
				enemyAI[index].targetPlayer = TargetPlayer(enemyAI[index].targetPlayer, true, tempPos);
			}
			enemyAI[index].runFlag = false;
			enemyAI[index].atkFlag = false;
			BossOrder(index, 0, 0, 0);

			if (GetPlayerFlag(enemyAI[index].targetPlayer))
			{
				bossAI[0] = 1;
			}
			break;
		default:
			bossAI[0] = 1;
			break;
		}
		break;
	case BOSS2:
		//赤いドラゴン
		double angleTemp2;
		tempPos.x = pos.x + ((enemyAI[index].hitPosS.x + enemyAI[index].hitPosE.x) / 2);//中間X位置を取る
		tempPos.y = pos.y + ((enemyAI[index].hitPosS.y + enemyAI[index].hitPosE.y) / 2);//中間Y位置を取る

		//スクリーンの上位置
		tempScrollPos = tempPos;
		tempScrollPos = AddScrollF(tempScrollPos);

		switch (bossAI[1])
		{
		case 0:
			//intro
			BossOrder(index, 1.5708, 0, 1);
			enemyAI[index].runFlag = true;

			if (tempScrollPos.y >= 200)
			{
				bossAI[1] = 1;
			}
			break;
		case 1:
			//move to right
			BossOrder(index, 0, 0, 15);
			enemyAI[index].dir = DIR_RIGHT;
			enemyAI[index].atkFlag = false;
			enemyAI[index].runFlag = true;

			if (tempScrollPos.x > SCREEN_SIZE_X - 50)
			{
				enemyAI[index].dir = DIR_LEFT;
				bossAI[1] = GetRand(1) + 3;
			}
			break;
		case 2:
			//move to left
			BossOrder(index, 3.142, 0, 15);
			enemyAI[index].dir = DIR_LEFT;
			enemyAI[index].runFlag = true;
			enemyAI[index].atkFlag = false;

			if (tempScrollPos.x < 100)
			{
				enemyAI[index].dir = DIR_RIGHT;
				bossAI[1] = GetRand(1) + 3;
			}
			break;
		case 3:
			//move down while shoot
			enemyAI[index].dir == DIR_LEFT ? angleTemp2 = 3.142 : angleTemp2 = 0;

			BossOrder(index, 1.5708, angleTemp2, enemyAI[index].speed);
			enemyAI[index].runFlag = true;

			if (enemyAI[index].attackIntervalCnt <= 0)
			{
				enemyAI[index].atkFlag = true;
				enemyAI[index].runFlag = false;
				enemyAI[index].attackIntervalCnt = enemyAI[index].attackInterval;
			}

			if (tempScrollPos.y > (SCREEN_SIZE_Y - 100))
			{
				if (GetRand(1) == 0)
				{
					bossAI[1] = 2;
				}
				else
				{
					bossAI[1] = 5;
				}
			}
			break;
		case 4:
			//move up while shoot
			enemyAI[index].dir == DIR_LEFT ? angleTemp2 = 3.142 : angleTemp2 = 0;

			BossOrder(index, -1.5708, angleTemp2, enemyAI[index].speed);
			enemyAI[index].runFlag = true;

			if (enemyAI[index].attackIntervalCnt <= 0)
			{
				enemyAI[index].atkFlag = true;
				enemyAI[index].runFlag = false;
				enemyAI[index].attackIntervalCnt = enemyAI[index].attackInterval;
			}

			if (tempScrollPos.y < 40)
			{
				if (GetRand(1) == 0)
				{
					bossAI[1] = 1;
				}
				else
				{
					bossAI[1] = 5;
				}
			}
			break;
		case 5:
			//move to center of screen
			tempScrollPos.x > SCREEN_SIZE_X / 2 ? enemyAI[index].dir = DIR_LEFT : enemyAI[index].dir = DIR_RIGHT;
			enemyAI[index].dir == DIR_LEFT ? angleTemp2 = 3.142 : angleTemp2 = 0;

			enemyAI[index].runFlag = true;
			enemyAI[index].atkFlag = false;
			BossOrder(index, 
				(atan2((SCREEN_SIZE_Y / 2) - tempScrollPos.y,
				(SCREEN_SIZE_X / 2) - tempScrollPos.x)),
				angleTemp2, enemyAI[index].speed);
			
			if (tempScrollPos.y >= (SCREEN_SIZE_Y / 2) - 50 &&
				tempScrollPos.y <= (SCREEN_SIZE_Y / 2) + 50 &&
				tempScrollPos.x >= (SCREEN_SIZE_X / 2) - 50 &&
				tempScrollPos.x <= (SCREEN_SIZE_X / 2) + 50)
			{
				bossAI[1] = 9;
				enemyAI[index].targetPlayer = TargetPlayer(enemyAI[index].targetPlayer, true, tempPos);
				enemyAI[index].attackIntervalCnt = enemyAI[index].attackInterval / 2;
			}
			break;
		case 6:
			//shoot at middle
			enemyAI[index].targetPlayer = TargetPlayer(enemyAI[index].targetPlayer, true, tempPos);
			playerTargetPos = GetPlayerCenterPos(enemyAI[index].targetPlayer);	//決まったターゲットプレイヤーの座標を取る
			playerTargetPos.x <= tempPos.x ? enemyAI[index].dir = DIR_LEFT : enemyAI[index].dir = DIR_RIGHT;
			enemyAI[index].dir == DIR_LEFT ? angleTemp2 = 3.142 : angleTemp2 = 0;
			if (enemyAI[index].attackIntervalCnt <= -10)
			{
				if (GetRand(1) > 0)
				{
					bossAI[1] = GetRand(1) + 1;
				}
				else
				{
					bossAI[1] = 7;
					enemyAI[index].targetPlayer = TargetPlayer(enemyAI[index].targetPlayer, true, tempPos);
				}
				enemyAI[index].attackIntervalCnt = enemyAI[index].attackInterval;
				enemyAI[index].runFlag = true;
				enemyAI[index].atkFlag = false;
			}
			else if (enemyAI[index].attackIntervalCnt >= 0)
			{
				enemyAI[index].runFlag = false;
				enemyAI[index].atkFlag = true;
				BossOrder(index, -angleTemp2,
					(atan2(playerTargetPos.y - tempPos.y, playerTargetPos.x - tempPos.x)) - double(enemyAI[index].attackIntervalCnt / 100) + double(GetRand(enemyAI[index].attackIntervalCnt)) / 200, 1);
			}
			break;
		case 7:
			//move to target player y
			playerTargetPos = GetPlayerCenterPos(enemyAI[index].targetPlayer);	//決まったターゲットプレイヤーの座標を取る
			playerTargetPos.y += 25;
			playerTargetPos.x <= tempPos.x ? enemyAI[index].dir = DIR_LEFT : enemyAI[index].dir = DIR_RIGHT;
			playerTargetPos.y <= tempPos.y ? angleTemp2 = -1.5708 : angleTemp2 = 1.5708;
			enemyAI[index].runFlag = true;
			enemyAI[index].atkFlag = false;

			BossOrder(index,
				angleTemp2,
				angleTemp2, enemyAI[index].speed);

			if (tempPos.y >= (playerTargetPos.y - 8) &&
				tempPos.y <= (playerTargetPos.y + 8))
			{
				bossAI[1] = 9;
				enemyAI[index].attackIntervalCnt = enemyAI[index].attackInterval;
			}
			break;
		case 8:
			//shoot intensively
			//shoot at middle
			enemyAI[index].targetPlayer = TargetPlayer(enemyAI[index].targetPlayer, true, tempPos);
			playerTargetPos = GetPlayerCenterPos(enemyAI[index].targetPlayer);	//決まったターゲットプレイヤーの座標を取る
			playerTargetPos.x <= tempPos.x ? enemyAI[index].dir = DIR_LEFT : enemyAI[index].dir = DIR_RIGHT;
			enemyAI[index].dir == DIR_LEFT ? angleTemp2 = 3.142 : angleTemp2 = 0;

			if (enemyAI[index].attackIntervalCnt <= -10)
			{
				enemyAI[index].atkFlag = false;
				enemyAI[index].runFlag = true;
				bossAI[1] = GetRand(1) + 1;
				enemyAI[index].attackIntervalCnt = enemyAI[index].attackInterval;
			}
			else if (enemyAI[index].attackIntervalCnt >= 0)
			{
				enemyAI[index].runFlag = false;
				enemyAI[index].atkFlag = true;
				BossOrder(index,
					-angleTemp2,
					angleTemp2, enemyAI[index].speed * 2);
			}
			break;
		case 9:
			//攻撃を貯める
			BossEffect(index);
			enemyAI[index].runFlag = false;

			if (enemyAI[index].attackIntervalCnt == -20)
			{
				if (tempScrollPos.y >= (SCREEN_SIZE_Y / 2) - 50 &&
					tempScrollPos.y <= (SCREEN_SIZE_Y / 2) + 50 &&
					tempScrollPos.x >= (SCREEN_SIZE_X / 2) - 50 &&
					tempScrollPos.x <= (SCREEN_SIZE_X / 2) + 50)
				{
					bossAI[1] = 6; 
					enemyAI[index].targetPlayer = TargetPlayer(enemyAI[index].targetPlayer, true, tempPos);
					enemyAI[index].attackIntervalCnt = enemyAI[index].attackInterval;
				}
				else
				{
					bossAI[1] = 8;
					enemyAI[index].attackIntervalCnt = enemyAI[index].attackInterval;
				}
			}
			break;
		default:
			bossAI[1] = 1;
			break;
		}
		break;
	case ENEMY_TYPE_MAX:
	default:
		//存在してない敵のタイプです。falseをreturnします
		return false;
		break;
	}

	EnemyOrder(index, enemyAI[index].runFlag, enemyAI[index].atkFlag, enemyAI[index].dir, enemyAI[index].jumpFlag, enemyAI[index].flyAngle);
	return true;
}

int TargetPlayer(int originalTarget, bool distanceCompare, POS_F AIpos)
{
	int target = 0;

	//存在しているプレイヤーの総数を出す
	int livingPlayerNum = 0;
	int livingPlayer[POSSIBLE_PLAYER] = { 0, 0, 0, 0 };

	for (int i = 0; i < POSSIBLE_PLAYER; i++)
	{
		if (GetPlayerFlag(i) == true && InScreen(GetPlayerCenterPos(i)) == true)
		{
			livingPlayer[livingPlayerNum] = i;
			livingPlayerNum++;
		}
	}

	//プレイヤー数が１の場合
	if (livingPlayerNum == 1) target = livingPlayer[0];

	//プレイヤー数が０の場合
	if (livingPlayerNum == 0) target = 0;

	//プレイヤー数が1を超えた場合
	if (livingPlayerNum > 1 && distanceCompare == false)
	{
		//元々のターゲットと違うプレイヤーを狙う
		do
		{
			target = GetRand(livingPlayerNum);
		} while (target == livingPlayer[livingPlayerNum]);
	}

	//距離を比較する
	if (livingPlayerNum > 1 && distanceCompare == true)
	{
		float distanceCompare;
		float distanceTmp;

		distanceCompare = 0;//初期化
		distanceTmp = 0;

		//比較を始める
		for (int j = 0; j < livingPlayerNum; j++)
		{
			///*auto tmp = AddScrollF(GetPlayerPos(j));
			//if (tmp.y + 192 > SCREEN_SIZE_Y) continue;*/
			distanceTmp = DistanceCalculation(GetPlayerCenterPos(livingPlayer[j]), AIpos);

			//一番遠いプレイヤーを狙う
			if (distanceCompare <= distanceTmp)
			{
				distanceCompare = distanceTmp;
				target = livingPlayer[j];	//目標として狙っている敵を記録する
			}
		}
	}

	return target;
}

float DistanceCalculation(POS_F p1, POS_F p2)
{
	int distanceX;
	int distanceY;
	float finalDistance;

	distanceX = (p2.x - p1.x);
	distanceX *= distanceX;
	distanceY = (p2.y - p1.y);
	distanceY *= distanceY;

	finalDistance = sqrt(distanceX + distanceY);

	return finalDistance;
}

void LastBossSwitch(bool boolean)
{
	lastBossSwitch = boolean;
}