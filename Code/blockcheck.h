#pragma once

enum BLOCK_TYPE
{
	WALKTHROUGH,	//背景チップ
	PASSABLE,		//ジャンプが通れる
	UNPASSABLE,		//ジャンプが通れない
	CRITICAL,		//トラップ
	JUMPPAD,
	BLOCK_TYPE_MAX
};

BLOCK_TYPE BlockCheck(POS_F pos);