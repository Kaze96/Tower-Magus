#include <math.h>
#include "main.h"
#include "projectile.h"
#include "character.h"
#include "keycheck.h"
#include "blockcheck.h"
#include "checkhit.h"
#include "map_edit.h"
#include "ending.h"
#include "stage.h"
#include "effect.h"
#include "fade.h"
#include "fade_character.h"
#include "sound.h"
#include "mapdata.h"

#define REVIVE_TIME 600
#define PROJECTILE_SIZE 96

CHARACTER player[PLAYER_MAX];
int playerImg[PLAYER_MAX];
int playerAtk[PLAYER_MAX][PLAYER_IMG_ATK_MAX];
int playerMoveAnim[PLAYER_MAX][PLAYER_IMG_RUN_MAX];
int playerDeathAnim[PLAYER_MAX][PLAYER_IMG_DIE_MAX];
int playerHit[PLAYER_MAX][PLAYER_IMG_HIT_MAX];
int playerJump[PLAYER_MAX][PLAYER_IMG_JUMP_MAX];
int reviveImg[PLAYER_MAX];

int playerCnt;
int livingCnt;
int tmpColor;

//--一人モード
int singleRevive;
int singleLiveCnt;
//---

void PlayerKnockback(int index, DIR knockDirection);		//ノックバック制御
void PlayerJump(int index);									//ジャンプ制御
void TrapCheck(POS_F pos1, POS_F pos2, POS_F pos3, POS_F mov, int index);	//周りのトラップのチェック
bool PlayerRevive(int index);								//プレイヤー復活制御
int LoadPlayerGraphFilter(int index);

// 弾サイズのオブセット
int projSizeTbl[DIR_MAX] = {
	-PROJECTILE_SIZE / 2,
	PROJECTILE_SIZE / 2,
	PROJECTILE_SIZE / 2,
	-PROJECTILE_SIZE / 2 
};

//ほかのプレイヤーのID
int collideTbl[PLAYER_MAX][PLAYER_MAX - 1] =
{
	1,2,3, //プレイヤー１(0番)
	0,2,3, //プレイヤー2(1番)
	0,1,3, //プレイヤー3(2番)
	0,1,2 //プレイヤー4(3番)
};
////ほかのプレイヤー当たり判定
bool collideFlagTbl[PLAYER_MAX][PLAYER_MAX - 1] =
{
	false,false,false,
	false,false,false,
	false,false,false,
	false,false,false

};
//左右移動のスピード
int speedTbl[DIR_MAX] = { -SPEED,SPEED,0,0 };
// 0ならばKEY、１ならばBUTTON
int controlTbl[2][DIR_MAX - 2]
{
	KEY_LEFT,KEY_RIGHT,
	BUTTON_LEFT,BUTTON_RIGHT
};

bool endInitflag;

void PlayerSysInit(void)
{
	//レベル補正フィルタの値
	player[0].filterRGB = 1;
	player[1].filterRGB = 60;
	player[2].filterRGB = 180;
	player[3].filterRGB = 300;

	char tmpString[256];
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		player[i].hitPosS = { 76,70 }; // Top Left
		player[i].hitPosE = { 112,142 }; // Btm Right
		for (int k = 0; k < PLAYER_IMG_ATK_MAX; k++)
		{
			sprintf_s(tmpString, "Image/Player/wizard_2_attack_0%d%d.png", i, k + 1);
			playerAtk[i][k] = LoadGraph(tmpString);
			GraphFilter(playerAtk[i][k], DX_GRAPH_FILTER_HSB, 1, player[i].filterRGB, 360, 0);
		}
		for (int j = 0; j < PLAYER_IMG_RUN_MAX; j++)
		{
			sprintf_s(tmpString, "Image/Player/wizard_2_run_%03d.png", j + 1);
			playerMoveAnim[i][j] = LoadGraph(tmpString);
			GraphFilter(playerMoveAnim[i][j], DX_GRAPH_FILTER_HSB, 1, player[i].filterRGB, 360, 0);
		}

		for (int m = 0; m < PLAYER_IMG_DIE_MAX; m++)
		{
			sprintf_s(tmpString, "Image/Player/wizard_2_die_%03d.png", m + 1);
			playerDeathAnim[i][m] = LoadGraph(tmpString);
			GraphFilter(playerDeathAnim[i][m], DX_GRAPH_FILTER_HSB, 1, player[i].filterRGB, 360, 0);
		}
		for (int n = 0; n < PLAYER_IMG_HIT_MAX; n++)
		{
			sprintf_s(tmpString, "Image/Player/wizard_2_hit_%03d.png", n + 1);
			playerHit[i][n] = LoadGraph(tmpString);
			GraphFilter(playerHit[i][n], DX_GRAPH_FILTER_HSB, 1, player[i].filterRGB, 360, 0);
		}
		for (int p = 0; p < PLAYER_IMG_JUMP_MAX; p++)
		{
			sprintf_s(tmpString, "Image/Player/wizard_2_jump_%03d.png", p + 1);
			playerJump[i][p] = LoadGraph(tmpString);
			GraphFilter(playerJump[i][p], DX_GRAPH_FILTER_HSB, 1, player[i].filterRGB, 360, 0);
		}

		playerImg[i] = LoadGraph("Image/Player/wizard_2.png");
		GraphFilter(playerImg[i], DX_GRAPH_FILTER_HSB, 1, player[i].filterRGB, 360, 0);

		reviveImg[i] = LoadGraph("Image/Player/ReviveImg.png");
		GraphFilter(reviveImg[i], DX_GRAPH_FILTER_HSB, 1, player[i].filterRGB, 360, 0);
	}
	playerCnt = 0;

}

void PlayerGameInit(void)
{	
	endInitflag = false;
	if (playerCnt <= 0) playerCnt = 2;
	livingCnt = playerCnt;
	singleRevive = 0;
	singleLiveCnt = 3;

	for (int i = 0; i < 4; i++)
	{
		player[i].pos.x = 100.0f + 100.0f * i;
		player[i].pos.y = 123.0f * MAP_CHIP_SIZE_Y;
		player[i].velocity = 0;
		player[i].runFlag = false;
		player[i].atkFlag = false;
		player[i].deathFlag = false;
		player[i].jumpFlag = false;
		player[i].visible = true;
		player[i].hitFlag = false;
		player[i].trapHitFlag = false;
		player[i].animCnt = 0;
		player[i].dir = DIR_RIGHT;
		player[i].knockDir = DIR_RIGHT;
		player[i].atkGap = GAP_TIME;
		player[i].reviveCnt = REVIVE_TIME;
		player[i].deathCnt = 0;
		player[i].trapHitCnt = 0;
	}
	if ( playerCnt == 4)
	{ }
	else
	{
		for (int i = playerCnt; i < 4; i++)
		{
			player[i].deathFlag = true;
			player[i].visible = false;
		}
	}
	
	tmpColor = 0;
}

void PlayerControl(void)
{
	POS_F tmpPos[4];
	POS_F leftTmp[4];

	for (int i = 0; i < playerCnt; i++)
	{
		//debug
		if (trgKey[KEY_X])
		{
			player[i].deathFlag = false;
			player[i].visible = true;
			player[i].trapHitCnt = 0;
			singleLiveCnt = 12;
		}
		
		player[i].jumpFlag = true; 

		//　キャラーが攻撃時に弾の位置の調整
		tmpPos[i]= { player[i].pos.x + player[i].hitPosE.x,player[i].pos.y + (player[i].hitPosE.y - player[i].hitPosS.y) / 4 * 3 };
		leftTmp[i] = { player[i].pos.x + player[i].hitPosS.x - PROJECTILE_SIZE, player[i].pos.y + (player[i].hitPosE.y - player[i].hitPosS.y) / 4 * 3 };

		//	プレイヤーの復活制御
		if (!PlayerRevive(i)) continue;
		

		//	プレイヤーの左右位置
		auto sidePos = [&](DIR dir, POS_F pos, int speed) {
			POS_F side;
			float tmp = float(speed);
			switch (dir)
			{
			case DIR_LEFT:
				side = { (0.0f - tmp - 1.0f)+ player[0].hitPosS.x,player[0].hitPosE.y - 1 };
				break;
			case DIR_RIGHT:
				side = {tmp + player[0].hitPosE.x ,player[0].hitPosE.y -1 };
				break;
			default:
				break;
			}
			return POS_F{ pos.x + side.x,pos.y + side.y };
		};

		// キャラの移動制御
		auto playerMove = [&](DIR dir) {
			POS_F movHitCheck = player[i].pos;
			movHitCheck.y = player[i].pos.y - player[i].hitPosE.y + player[i].hitPosS.y - 1;

			int checkTop = BlockCheck(sidePos(dir, movHitCheck, SPEED));
			int checkBottom = BlockCheck(sidePos(dir, player[i].pos, SPEED));		// 下半身のブロックのあたりチェック
			

			if (newKey[controlTbl[0][dir]] || input[i].Buttons[controlTbl[0][dir]])
			{
				player[i].dir = dir;

				
				if ((player[i].pos.x - SPEED <= 0 - MAP_CHIP_SIZE_X && (dir == DIR_LEFT)) 
					|| (player[i].pos.x + SPEED >= MAP_CHIP_X * MAP_CHIP_SIZE_X - MAP_CHIP_SIZE_X / 2 && (dir == DIR_RIGHT)))
				{
					player[i].pos.x = player[i].pos.x;

				}
				else if (checkTop == CRITICAL || checkBottom == CRITICAL) //　トラップチェック
				{

					TrapCheck(sidePos(dir, movHitCheck, SPEED), sidePos(dir, player[i].pos, SPEED), sidePos(dir, player[i].pos, SPEED), { player[i].pos.x + speedTbl[dir],player[i].pos.y }, i);
				}
				else if ((checkTop != UNPASSABLE && checkBottom != UNPASSABLE)
					&& player[i].hitFlag == false)
				{
					if (playerCnt > 1)
					{
						//　左右のほかのプレイヤーにぶつかる判定
						for (int j = 0; j < playerCnt - 1; j++)
						{
							if (!player[collideTbl[i][j]].visible) continue;

							POS_F collideTmp = { player[collideTbl[i][j]].pos.x + player[collideTbl[i][j]].hitPosS.x, player[collideTbl[i][j]].pos.y + player[collideTbl[i][j]].hitPosS.y };
							POS tmpSize = { int(player[collideTbl[i][j]].hitPosE.x - player[collideTbl[i][j]].hitPosS.x), int(player[collideTbl[i][j]].hitPosE.y - player[collideTbl[i][j]].hitPosS.y) };

							//左右移動によって、オブセットを足す
							if ((player[i].pos.x + player[i].hitPosS.x - (SPEED * (dir == DIR_LEFT))  <= collideTmp.x + tmpSize.x + (SPEED * (dir == DIR_LEFT))
								&& player[i].pos.x + player[i].hitPosE.x + (SPEED * (dir == DIR_RIGHT)) >= collideTmp.x - (SPEED * (dir == DIR_RIGHT)))
								&& player[i].pos.y == player[collideTbl[i][j]].pos.y)
							{

								player[i].pos.x = player[i].pos.x;
								collideFlagTbl[i][j] = true;
							}
						}
						if (!(collideFlagTbl[i][0] || collideFlagTbl[i][1] || collideFlagTbl[i][2]))
						{
							player[i].pos.x += speedTbl[dir];
						}

						for (int j = 0; j < playerCnt - 1; j++)
						{
							collideFlagTbl[i][j] = false;
						}

					}
					else player[i].pos.x += speedTbl[dir];
				}
			}
		};

		//　プレイヤーのキー操作
		if (!endFlag && !player[i].hitFlag)
		{
			//　走るアニメーション
			if (newKey[KEY_LEFT]|| newKey[KEY_RIGHT] || input[i].Buttons[BUTTON_LEFT] || input[i].Buttons[BUTTON_RIGHT])
			{
				player[i].runFlag = true;
			}
			else
			{
				player[i].runFlag = false;
			}
			
			//　移動判定
			playerMove(DIR_LEFT);
			playerMove(DIR_RIGHT);
	
			//　攻撃制御
			if (newKey[KEY_Z] || input[i].Buttons[BUTTON_X])
			{
				player[i].atkFlag = true;
			}
			else if (player[i].atkFlag && (player[i].animCnt >= 2))
			{
				if ((player[i].animCnt / 2) % PLAYER_IMG_ATK_MAX == 0)
				//アニメーションとあわせてフラグを閉じる
				player[i].atkFlag = false;
			}
		}
		
		//　弾の生成
		POS_F lengthPos = player[i].pos;
		auto attackCheck = [&](DIR dir,POS_F pos,float angle) {
			if (player[i].dir == dir)
			{
				lengthPos.x += projSizeTbl[dir];

				if ((BlockCheck(sidePos(player[i].dir, lengthPos, 15)) != WALKTHROUGH) ||
					BlockCheck(sidePos(player[i].dir, {player[i].pos.x + 96,player[i].pos.y}, 15)) != WALKTHROUGH)
				{
					
					ProjectileInit(PROJECTILE_TYPE(i), POS_F{ player[i].pos.x + PROJECTILE_SIZE /2,pos.y }, false, false, false, 15,
						angle, player[i].filterRGB, true, 150);
				}
				else
				{
					ProjectileInit(PROJECTILE_TYPE(i), pos, false, false, false, 15,
						angle, player[i].filterRGB, true, 150);
				}
			}
		};

		// 攻撃間隔
		if (player[i].atkFlag && player[i].atkGap >= GAP_TIME && player[i].hitFlag == false)
		{
			
			attackCheck(DIR_RIGHT, tmpPos[i], 0);
			attackCheck(DIR_LEFT, leftTmp[i], 3.1415926536);
			player[i].atkGap = 0;
		}

		//　プレイヤージャンプ制御
		PlayerJump(i);

		//　プレイヤーノックバック
		PlayerKnockback(i, player[i].knockDir);

		//　ジャンプキー
		if (!player[i].jumpFlag)
		{
			if (newKey[KEY_UP] || newKey[KEY_SPACE] || input[i].Buttons[BUTTON_A])
			{
				if (!endFlag && !player[i].hitFlag)
				{
					player[i].jumpFlag = true;
					player[i].velocity = INIT_VELOCITY;
					POS_F tmpPos = player[i].pos;
					tmpPos.x += (player[i].hitPosS.x + (player[i].hitPosE.x - player[i].hitPosS.x) / 2);
					tmpPos.y += player[i].hitPosE.y - 1.0f;

					SetEffect(tmpPos, EFFECT_TYPE_JUMP);
					PlaySoundType(SOUND_TYPE_JUMP);
				}
			}
		}

		player[i].atkGap++;

		//　マップ外落ちる
		POS_F fallTmp = AddScrollF(player[i].pos);
		if (!endFlag)
		{
			if (fallTmp.y > SCREEN_SIZE_Y)
			{
				player[i].deathFlag = true;
				player[i].visible = false;
				PlaySoundType(SOUND_TYPE_DIE);
			}
		}

		if (player[i].deathFlag && playerCnt > 1) livingCnt--;
		if (player[i].runFlag && !player[i].jumpFlag) PlaySoundType(SOUND_TYPE_WALK);
	}
}

void PlayerJump(int index)
{
	POS_F mov;
	POS_F movHitCheck;

	POS_F movHitCheck1;
	POS_F movHitCheck2;


	POS tmpIndex;
	POS_F jumpPos;
	//　ジャンプ制御
	//movHitCheck の初期化
	auto movHitInit = [&](float head) {
		movHitCheck.y = mov.y + head;			//頭上の座標系計算
		movHitCheck.x = mov.x + player[index].hitPosS.x;

		movHitCheck1 = movHitCheck;
		movHitCheck2 = movHitCheck;

		movHitCheck1.x = movHitCheck.x + (player[index].hitPosE.x - player[index].hitPosS.x) / 2;		// Middle Hit Point
		movHitCheck2.x = movHitCheck.x + (player[index].hitPosE.x - player[index].hitPosS.x);			// Top Right Hit Point
	};

	if (player[index].jumpFlag)
	{
		//　キー離すと、早く落ちる
		if ((upKey[KEY_UP] || upKey[KEY_SPACE] || upButton[index][BUTTON_A]) && player[index].velocity > 0) player[index].velocity /= 2;

		mov = player[index].pos;
		movHitCheck = mov;
		movHitCheck1 = mov;
		movHitCheck2 = mov;

		player[index].velocity -= ACG_G * SECOND_PER_FRAME;		//高さの計算
		if (player[index].velocity <= -80) player[index].velocity = -80;
		mov.y -= player[index].velocity * SECOND_PER_FRAME;		//加速度の計算
		
		if (player[index].velocity > 0)
		{
			//頭上のブロックをチェックする。
			movHitInit(player[index].hitPosS.y); //頭上の座標系計算

			if (BlockCheck(movHitCheck) == CRITICAL
				|| BlockCheck(movHitCheck1) == CRITICAL
				|| BlockCheck( movHitCheck2) == CRITICAL)
			{
				if (player[index].trapHitCnt < 5)
				{
					TrapCheck(movHitCheck, movHitCheck1, movHitCheck2, mov, index);
				}
			}
			else if ((BlockCheck(movHitCheck) == WALKTHROUGH && BlockCheck(movHitCheck1) == WALKTHROUGH && BlockCheck(movHitCheck2) == WALKTHROUGH) ||
					(BlockCheck(movHitCheck) == PASSABLE || BlockCheck(movHitCheck1) == PASSABLE || BlockCheck(movHitCheck2) == PASSABLE))
			{
				player[index].pos = mov;
			}
			else
			{
				player[index].velocity = 0;
				mov = player[index].pos;
			}
		}
		else if (player[index].velocity <= 0)
		{
			//足元のブロックっチェックする
			movHitInit(player[index].hitPosE.y); ////足元の座標計算

			if (BlockCheck(movHitCheck) == CRITICAL
				|| BlockCheck(movHitCheck1) == CRITICAL
				|| BlockCheck(movHitCheck2) == CRITICAL)
			{
				if (player[index].trapHitCnt < 5)
				{
					TrapCheck(movHitCheck, movHitCheck1, movHitCheck2, mov, index);
				}
			}
			else if (BlockCheck(movHitCheck) == WALKTHROUGH
				&& BlockCheck(movHitCheck1) == WALKTHROUGH
				&& BlockCheck(movHitCheck2) == WALKTHROUGH)
			{
				player[index].pos = mov;								//プレイヤーの座標を移動する

			}
			else
			{
				POS tmpChipIndex = MapPosToIndex(movHitCheck);
				POS tmpChipPos = MapIndexToPos(tmpChipIndex);

				tmpChipPos.y += 16;

				if (movHitCheck.y > tmpChipPos.y)
				{
					player[index].pos = mov;
				}
				else
				{
					tmpIndex = MapPosToIndex(movHitCheck);			//マップ配列の場所を取る
					jumpPos = MapIndexToPosF(tmpIndex);				//足元の座標を算出：マップのブロック
					player[index].pos.y = jumpPos.y - player[index].hitPosE.y;
					player[index].velocity = 0;
					player[index].jumpFlag = false;
					if (player[index].trapHitFlag)
					{
						player[index].trapHitFlag = false;
						player[index].deathFlag = true;
					}
					if (player[index].hitFlag) player[index].hitFlag = false;
				}
			}
		}		
		for (int j = 0; j < playerCnt - 1; j++)
		{
			//プレイヤーが死ぬ、死んでる状態だったら処理しない
			if (!player[collideTbl[index][j]].visible || player[index].trapHitFlag || player[collideTbl[index][j]].trapHitFlag)
			{
				continue;
			}
			POS_F targetPos = { player[collideTbl[index][j]].pos.x + player[collideTbl[index][j]].hitPosS.x,
				player[collideTbl[index][j]].pos.y + player[collideTbl[index][j]].hitPosS.y };
			POS targetSize = { int(player[collideTbl[index][j]].hitPosE.x - player[collideTbl[index][j]].hitPosS.x),
				int(player[collideTbl[index][j]].hitPosE.y - player[collideTbl[index][j]].hitPosS.y) };
			if (PlayerCollideCheck(index, targetPos, targetSize))
			{
				if ((player[index].jumpFlag && (!player[collideTbl[index][j]].jumpFlag)) || 
					(!player[index].jumpFlag && (player[collideTbl[index][j]].jumpFlag)))
				{
					if (player[index].pos.y > player[collideTbl[index][j]].pos.y)
					{
						player[collideTbl[index][j]].hitFlag = false;
						if (player[index].velocity > 0)
						{
							player[collideTbl[index][j]].velocity = player[index].velocity * 1.75;
						}
						else
						{
							player[collideTbl[index][j]].velocity = INIT_VELOCITY / 1.5;
						}
						if (player[collideTbl[index][j]].velocity >= INIT_VELOCITY * 1.25)
						{
							player[collideTbl[index][j]].velocity = INIT_VELOCITY * 1.25;
						}
						if (player[index].velocity > 0)
						{
							player[index].velocity *= -1;
						}
						PlaySoundType(SOUND_TYPE_JUMP);
					}
				}
				else
				{

					if (player[index].pos.y < player[collideTbl[index][j]].pos.y)
					{
						player[index].hitFlag = false;
						if (player[collideTbl[index][j]].velocity > 0)
						{
							player[index].velocity = player[collideTbl[index][j]].velocity * 1.75;
						}
						else
						{
							player[index].velocity = INIT_VELOCITY / 1.5;
						}
						if (player[index].velocity >= INIT_VELOCITY * 1.25)
						{
							player[index].velocity = INIT_VELOCITY * 1.25;
						}
						if (player[collideTbl[index][j]].velocity > 0)
						{
							player[collideTbl[index][j]].velocity *= -1;
						}
						PlaySoundType(SOUND_TYPE_JUMP);
					}
				}

			}
		}
	}
}

void TrapCheck(POS_F pos1,POS_F pos2, POS_F pos3,POS_F mov ,int index)
{
	POS_F tmpCheck[3] = { pos1,pos2,pos3 };

	POS trapIndex;
	POS_F trapPos;
	for (int i = 0; i < 3; i++)
	{
		if (BlockCheck(tmpCheck[i]) == CRITICAL)
		{
			trapIndex = MapPosToIndex(tmpCheck[i]);
		}
	}
	for (int j = 0; j < LAYER_MAX; j++)
	{
		if (mapData[j][trapIndex.y][trapIndex.x] == 74 || mapData[j][trapIndex.y][trapIndex.x] == 75)
		{
			trapPos = MapIndexToPosF(trapIndex);
			trapPos.y += MAP_CHIP_SIZE_Y / 4 * (mapData[j][trapIndex.y][trapIndex.x] == 74);
			if (PlayerCollideCheck(index, trapPos, { MAP_CHIP_SIZE_X,MAP_CHIP_SIZE_Y / 4 * 3 }))
			{
				DIR tmpReverse[DIR_MAX] = { DIR_RIGHT,DIR_LEFT,DIR_UP,DIR_DOWN };
				player[index].trapHitFlag = true;
				player[index].trapHitCnt ++;
				PlayerGetHit(index, tmpReverse[player[index].dir]);
				player[index].animCnt = 0;
				
				//player[index].deathFlag = true;
			}
			else
			{
				player[index].pos = mov;
			}
		}
		else if (mapData[j][trapIndex.y][trapIndex.x] == 59 || mapData[j][trapIndex.y][trapIndex.x] == 66)
		{
			DIR tmpDir = DIR_LEFT;
			trapPos = MapIndexToPosF(trapIndex);
			trapPos.x += MAP_CHIP_SIZE_X / 4 * (mapData[j][trapIndex.y][trapIndex.x] == 66);
			if (PlayerCollideCheck(index, trapPos, { MAP_CHIP_SIZE_X / 4 * 3,MAP_CHIP_SIZE_Y }))
			{
				player[index].trapHitFlag = true;
				player[index].trapHitCnt ++;
				PlayerGetHit(index, DIR(DIR_RIGHT - (mapData[j][trapIndex.y][trapIndex.x] == 66)));
				player[index].animCnt = 0;
				//player[index].deathFlag = true;
			}
			else
			{
				player[index].pos = mov;
			}
		}
	}
}

void PlayerDraw(void)
{
	int img[PLAYER_MAX];
	bool tmpFlag;

	for (int i = 0; i < playerCnt; i++)
	{
		if (player[i].deathFlag)
		{
			if (playerCnt > 1)
			{
				int tmpColor = CheckPlayerGraphFilter(player[i].filterRGB);
				DrawCircleGauge((SCREEN_SIZE_X / (playerCnt * 2)) + (SCREEN_SIZE_X / playerCnt) * i, 50, 100.0, reviveImg[i], (100.0 / REVIVE_TIME) * player[i].reviveCnt);

				int length = GetDrawFormatStringWidthToHandle(font, "%2.1f", player[i].reviveCnt / 60);
				DrawFormatStringToHandle((SCREEN_SIZE_X / (playerCnt * 2)) + (SCREEN_SIZE_X / playerCnt) * i - length / 2, 95, tmpColor, font, "%2.1f", player[i].reviveCnt / 60.0f);
				DrawRotaFormatStringToHandle(
					(SCREEN_SIZE_X / (playerCnt * 2)) + (SCREEN_SIZE_X / playerCnt) * i - 60, 120,
					1.0, 1.0,
					0, 0, (180 * PI / 180), tmpColor, font, 0x000000, true, "PLAYER%d", i + 1);
			}
		}

		if (player[i].visible)
		{
			if ((!player[i].runFlag) && (!player[i].atkFlag) && (!player[i].deathFlag) && (!player[i].hitFlag) && (!player[i].jumpFlag)) player[i].animCnt = 0;
			img[i] = playerImg[i];
			if (player[i].runFlag)	 img[i] = playerMoveAnim[i][(player[i].animCnt / 5) % PLAYER_IMG_RUN_MAX];
			
			if (player[i].jumpFlag)
			{
				img[i] = playerJump[i][(player[i].animCnt / 2) % (PLAYER_IMG_JUMP_MAX)];
				
				if (player[i].animCnt >= 6)
				{
					img[i] = playerJump[i][3];
				}
			}
			if (player[i].atkFlag)   img[i] = playerAtk[i][(player[i].animCnt / 2) % PLAYER_IMG_ATK_MAX];
			if (player[i].hitFlag)
			{
				if (player[i].velocity > 0) img[i] = playerHit[i][0];
				else img[i] = playerHit[i][1];
			}
			if (player[i].deathFlag)
			{
				img[i] = playerDeathAnim[i][(player[i].animCnt / 15) % PLAYER_IMG_DIE_MAX];
				if (player[i].animCnt >= 15)
				{
					if (player[i].animCnt / 15 % PLAYER_IMG_DIE_MAX == 0)
					{
						player[i].visible = false;
						continue;
					}
				}
			}

			POS_F tmpPos = AddScrollF(player[i].pos);

			if (player[i].dir == DIR_RIGHT)
			{
				DrawGraphF(tmpPos.x, tmpPos.y, img[i], true);
			}
			else if (player[i].dir == DIR_LEFT)
			{
				DrawTurnGraphF(tmpPos.x, tmpPos.y, img[i], true);
			}

			//fade生成

			if (player[i].velocity > 0 && player[i].jumpFlag) CharacterFadeInit(player[i].pos, img[i], player[i].filterRGB, player[i].dir, 18);

			player[i].animCnt++;
		}
		
	}
}

POS_F GetPlayerPos(int id)
{
	return player[id].pos;
}

void PlayerGetHit(int index, DIR damageDirection)
{
	if (player[index].visible)
	{
		PlaySoundType(SOUND_TYPE_DAMAGE_P);
		player[index].jumpFlag = true;
		player[index].velocity = INIT_VELOCITY / 2;
		if (player[index].trapHitFlag) player[index].velocity = INIT_VELOCITY * 2/3 ;
		player[index].hitFlag = true;
		player[index].knockDir = damageDirection;
		StartJoypadVibration(index + 1, 300, 250);
	}
}

void PlayerKnockback(int index, DIR knockDirection)
{
	if (player[index].hitFlag)
	{
		POS_F tmpEffectPos = player[index].pos;
		tmpEffectPos.x += CHARACTER_SIZE_X / 2;
		tmpEffectPos.y += CHARACTER_SIZE_Y / 2;
		tmpColor = LoadPlayerGraphFilter(index);
		if(player[index].trapHitFlag) SetEffect(tmpEffectPos, EFFECT_TYPE_HIT);

		tmpEffectPos.x = player[index].pos.x + player[index].hitPosS.x;
		tmpEffectPos.y = player[index].pos.y + player[index].hitPosS.y;

		//	プレイヤーの左右位置
		auto sidePos = [&]( POS_F pos, int speed)
		{
			POS_F side;
			float tmpSpeed = float(speed);

			switch (knockDirection)
			{
			case DIR_LEFT:
				side = { (0.0f - tmpSpeed - 1.0f) + player[index].hitPosS.x,player[index].hitPosE.y - 1 };
				break;
			case DIR_RIGHT:
				side = { tmpSpeed + player[index].hitPosE.x ,player[index].hitPosE.y - 1 };
				break;
			default:
				break;
			}
			return POS_F{ pos.x + side.x,pos.y + side.y };
		};

		POS_F mov;
		POS_F movHitCheck;

		POS_F movHitCheck1;
		POS_F movHitCheck2;

		POS tmpIndex;
		POS_F tmpPos;

		mov = player[index].pos;
		movHitCheck = mov;
		movHitCheck.y = player[index].pos.y - player[index].hitPosE.y + player[index].hitPosS.y - 1;
		int checkTop = BlockCheck(sidePos(movHitCheck, SPEED));
		int checkBottom = BlockCheck(sidePos(player[index].pos, SPEED));
		
		switch (knockDirection)
		{
		case DIR_RIGHT:
			mov.x += 4.0f;
			break;
		case DIR_LEFT:
			mov.x -= 4.0f;
			break;
		}

		if ((player[index].pos.x - SPEED <= 0 - MAP_CHIP_SIZE_X && (knockDirection == DIR_LEFT)) 
			|| (player[index].pos.x + SPEED >= (MAP_CHIP_X * MAP_CHIP_SIZE_X) - MAP_CHIP_SIZE_X/2 && (knockDirection == DIR_RIGHT)))
		{
			player[index].pos.x = player[index].pos.x;

		}
		else if ((checkTop == CRITICAL || checkBottom == CRITICAL) && !player[index].trapHitFlag)
		{
			if (player[index].trapHitCnt < 5)
			{
				TrapCheck(sidePos(movHitCheck, SPEED), sidePos(player[index].pos, SPEED), sidePos(player[index].pos, SPEED), mov, index);
			}
		}
		else if ((checkTop != UNPASSABLE && checkBottom != UNPASSABLE))
		{
			player[index].pos = mov;


		}
		else
		{
			//　無限ループを防ぐ
			if (player[index].trapHitFlag && player[index].trapHitCnt >= 5)
			{
				//player[index].trapHitFlag = false;
				player[index].trapHitCnt = 0;

				if (knockDirection == DIR_RIGHT) player[index].dir = DIR_RIGHT;
				else if (knockDirection == DIR_LEFT) player[index].dir = DIR_LEFT;
			}
			tmpIndex = MapPosToIndex(sidePos( movHitCheck, SPEED));			//マップ配列の場所を取る
			if (knockDirection == DIR_RIGHT)
			{
				tmpPos = MapIndexToPosF(tmpIndex);
				player[index].pos.x = tmpPos.x - player[index].hitPosE.x - SPEED;
			}
			else
			{
				tmpIndex.x++;
				tmpPos = MapIndexToPosF(tmpIndex);
				player[index].pos.x = tmpPos.x - player[index].hitPosS.x + SPEED;
			}
			

		}
	}
}

bool PlayerCollideCheck(int id, POS_F pos, POS size)
{

	return CheckHit({ player[id].pos.x + player[id].hitPosS.x, player[id].pos.y + player[id].hitPosS.y }, pos, { int(player[id].hitPosE.x - player[id].hitPosS.x), int(player[id].hitPosE.y - player[id].hitPosS.y) }, size);
}

bool GetPlayerFlag(int id)
{
	return !player[id].deathFlag;
}

POS_F GetPlayerCenterPos(int id)
{
	POS_F tmp;
	tmp.x = player[id].pos.x + ((player[id].hitPosS.x + player[id].hitPosE.x) / 2);//中間X位置を取る
	tmp.y = player[id].pos.y + ((player[id].hitPosS.y + player[id].hitPosE.y) / 2);//中間Y位置を取る
	return tmp;
}

POS_F GetAllPlayerAveragePos(void)
{
	POS_F retPos = { 0,0 };
	int playerCount = 0;
	
	for (int i = 0; i < playerCnt; i++)
	{
		if (!player[i].deathFlag)
		{
			retPos.x += player[i].pos.x;
			retPos.y += player[i].pos.y;
			playerCount++;
		}
	}

	if (playerCnt != 0)
	{
		retPos.x /= playerCount;
		retPos.y /= playerCount;
	}
	return retPos;
}

bool PlayerRevive(int index)
{
	//　falseならば操作できない
	//	復活エッフェクト
	auto reviveEff = [&]() {
		POS_F tmpPos = player[index].pos;
		tmpPos.x = player[index].pos.x + (player[index].hitPosE.x - player[index].hitPosS.x - 1.0f);
		tmpPos.y = player[index].pos.y + player[index].hitPosE.y - 128.0f + SPEED * 2;
		SetEffect(tmpPos, EFFECT_TYPE_REVIVE);
		PlaySoundType(SOUND_TYPE_REVIVE);
	};

	auto blockReviveCheck = [&]() {
		for (int y = player[index].pos.y / MAP_CHIP_SIZE_Y; y > 3; y--)
		{
			POS_F block1 = { player[index].pos.x + player[index].hitPosS.x ,float(y * MAP_CHIP_SIZE_Y - MAP_CHIP_SIZE_Y * 2) };
			POS_F block2 = { player[index].pos.x + player[index].hitPosS.x ,float(y * MAP_CHIP_SIZE_Y - MAP_CHIP_SIZE_Y * 3) };
			POS_F block3 = { player[index].pos.x + player[index].hitPosS.x ,float(y * MAP_CHIP_SIZE_Y - MAP_CHIP_SIZE_Y * 4) };



			if ((BlockCheck(block1) == UNPASSABLE || BlockCheck(block1) == PASSABLE)
				&& (BlockCheck(block2) == WALKTHROUGH) && (BlockCheck(block3) == WALKTHROUGH || BlockCheck(block3) == PASSABLE )
				&& InScreen(block1) && InScreen(block2) && InScreen(block3))
			{
				player[index].pos.y = block3.y;
				return true;
			}
		}
		return false;
	};
	singleRevive -= (singleRevive > 0);
	player[index].visible = true;
	if ((singleRevive / 3) % 2)
	{
		player[index].visible = false;
	}
	if (singleLiveCnt <= 0)
	{
		player[index].visible = false;
		return false;
	}
	if (player[index].deathFlag)
	{
		//-------一人モード-------
		if (playerCnt == 1)
		{
			
			if (AddScrollF(player[index].pos).y > SCREEN_SIZE_Y)
			{
				while (!blockReviveCheck())
				{
					player[index].pos.x += MAP_CHIP_SIZE_X * (-1 * rand() % 2);
					if (player[index].pos.x < 0) player[index].pos.x += (MAP_CHIP_SIZE_X * 3);
					if (player[index].pos.x > SCREEN_SIZE_X) player[index].pos.x -= (MAP_CHIP_SIZE_X * 3);
				}
				
				player[index].deathFlag = false;
				player[index].jumpFlag = true;
				player[index].velocity = INIT_VELOCITY ;

				singleLiveCnt--;
				singleRevive = 60;
				player[index].trapHitCnt = 0;

				reviveEff();
				return true;
			}
			if (((player[index].animCnt / 15) == 5))
			{
				player[index].deathFlag = false;
				player[index].hitFlag = false;
				player[index].visible = true;
				player[index].jumpFlag = true;
				player[index].velocity = INIT_VELOCITY / 2;
				
				
				singleLiveCnt--;
				singleRevive = 60;
				player[index].trapHitCnt = 0;

				if (singleLiveCnt > 0) reviveEff();
				return true;
			}
			return false;
		}
		//---------------------------
		

		if (livingCnt <= 0) return false;
		
		//死んだら操作できない
		if (((player[index].animCnt / 15) == 5))
		{
			player[index].visible = false;
		}
		player[index].reviveCnt--;
		if (player[index].reviveCnt < 0)
		{
			int tmp;
			do {
				tmp = rand() % (int)PLAYER_MAX;
			} while (!(tmp != index && player[tmp].visible &&  !player[tmp].deathFlag));
			player[index].deathFlag = false;
			player[index].hitFlag = false;
			player[index].visible = true;
			player[index].pos = player[tmp].pos;
			player[index].jumpFlag = true;
			player[index].velocity = INIT_VELOCITY /2;
			player[index].reviveCnt = REVIVE_TIME;
			player[index].trapHitFlag = false;
			player[index].trapHitCnt = 0;
			livingCnt++;

			reviveEff();
			return true;
		}
		return false;
	}
	return true;
}

int LoadPlayerGraphFilter(int  index)
{
	return player[index].filterRGB;
}

int GetPlayerGraphFilter(void)
{
	return tmpColor;
}

int GetLivingPlayerCount(void)
{
	int tmp = 0;

	for (int i = 0; i < playerCnt; i++)
	{
		if (player[i].visible && !player[i].deathFlag)
		{
			tmp++;
		}
	}

	return tmp;
}

int GetPlayerScore(int index)
{
	return player[index].score;
}

void PlayerEndInit(void)
{
	for (int i = 0; i < playerCnt; i++)
	{
		//　エンディングの初期化
		if (endFlag)
		{
			if (!endInitflag)
			{
				player[i].pos.x = 100 + ((MAP_CHIP_SIZE_X * 4) * (i >= 2)) + SCREEN_SIZE_X / 5 * i;
				player[i].pos.y = float(SCREEN_SIZE_Y - CHARACTER_SIZE_Y - (MAP_CHIP_SIZE_Y * 2));
				player[i].deathFlag = false;
				player[i].visible = true;
				player[i].runFlag = false;
				player[i].atkFlag = false;
				if (i < 2)
				{
					player[i].dir = DIR_RIGHT;
				}
				else
				{
					player[i].dir = DIR_LEFT;
				}
				if (i == playerCnt - 1)
				{
					endInitflag = true;
				}
			}
		}
	}
}

int GetPlayerImg(int index)
{
	return playerImg[index];

}

bool GetPlayerTrapFlag(int index)
{
	return player[index].trapHitFlag;
}

int CheckPlayerGraphFilter(int RGB)
{
	switch (RGB)
	{
	case 1:
		tmpColor = 0xFF3A3A;
		break;
	case 60:
		tmpColor = 0xF1FF51;
		break;
	case 180:
		tmpColor = 0x6FFFF0;
		break;
	case 300:
		tmpColor = 0xEE76FF;
		break;
	default:
		tmpColor = 0xFFFFFF;
		break;
	}

	return tmpColor;
}

void MoveAllPlayerToStart(void)
{
	for (int i = 0; i < playerCnt; i++)
	{
		player[i].pos.y += (MAP_CHIP_Y * MAP_CHIP_SIZE_Y - SCREEN_SIZE_Y) - (20 * MAP_CHIP_SIZE_Y - SCREEN_SIZE_Y);
	}
}