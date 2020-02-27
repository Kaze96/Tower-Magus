#include "main.h"
#include "projectile.h"
#include "stage.h"
#include "blockcheck.h"
#include "map_edit.h"
#include "checkhit.h"
#include "mapdata.h"

bool CheckHit(POS_F pos1, POS_F pos2, POS pos1size, POS pos2size)
{
	if (pos1.x + pos1size.x >= pos2.x &&
			pos1.x <= pos2.x + pos2size.x &&
			pos1.y + pos1size.y >= pos2.y &&
			pos1.y <= pos2.y + pos2size.y)
	{
		return true;
	}
	return false;
}

POS_F Knockback(POS_F pos, DIR dir, POS_F hitPosS, POS_F hitPosE, int speed,int velocity)
{
	//	プレイヤーの左右位置
	auto sidePos = [&](DIR dir, POS_F pos, int speed) 
{
		POS_F side;
		float tmpSpeed = float(speed);

		switch (dir)
		{
		case DIR_RIGHT:
			side = { (0.0f - tmpSpeed - 1.0f) + hitPosS.x,hitPosE.y - 1 };
			break;
		case DIR_LEFT:
			side = { tmpSpeed + hitPosE.x ,hitPosE.y - 1 };
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

	mov = pos;
	movHitCheck = mov;
	movHitCheck.y = pos.y - hitPosE.y + hitPosS.y - 1;

	int checkTop = BlockCheck(sidePos(dir, movHitCheck, speed));
	int checkBottom = BlockCheck(sidePos(dir, pos,speed));

	switch (dir)
	{
	case DIR_RIGHT:
		mov.x -= speed;
		break;
	case DIR_LEFT:
		mov.x += speed;
		break;
	}

	if ((checkTop == WALKTHROUGH && checkBottom == WALKTHROUGH) ||
		(checkTop == PASSABLE && checkBottom == WALKTHROUGH) ||
		(checkTop == WALKTHROUGH && checkBottom == PASSABLE))
	{
		pos = mov;
	}
	else
	{
		tmpIndex = MapPosToIndex(sidePos(dir, movHitCheck, speed));			//マップ配列の場所を取る
		if (dir == DIR_LEFT)
		{
			tmpPos = MapIndexToPosF(tmpIndex);
			pos.x = tmpPos.x - hitPosE.x - speed;
			//pos.x = tmpPos.x;
		}
		else
		{
			tmpIndex.x--;
			tmpPos = MapIndexToPosF(tmpIndex);
			pos.x = tmpPos.x + (hitPosE.x - hitPosS.x);
		}
	}
	return pos;
}