#pragma once

enum BLOCK_TYPE
{
	WALKTHROUGH,//background chip
	PASSABLE, //can jump up
	UNPASSABLE, //cannot pass
	CRITICAL, //killing block
	JUMPPAD,
	BLOCK_TYPE_MAX
};

BLOCK_TYPE BlockCheck(POS_F pos);