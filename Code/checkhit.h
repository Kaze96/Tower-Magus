#pragma once

enum HIT_TYPE{
	HIT_PLAYERSHOT,	
	HIT_ENEMYSHOT,
	HIT_MAX
};

bool CheckHit(POS_F pos1, POS_F pos2,POS pos1size,POS pos2size);	//当たり判定チェック
POS_F Knockback(POS_F pos, DIR dir, POS_F hitPosS, POS_F hitPosE,int speed,int velocity);	