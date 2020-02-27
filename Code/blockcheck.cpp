#include "main.h"
#include "blockcheck.h"
#include "map_edit.h"
#include "mapdata.h"

BLOCK_TYPE BlockCheck(POS_F pos)
{
	int mapNo;
	POS mapIndex;
	BLOCK_TYPE ret;

	mapIndex = MapPosToIndex(pos);

	mapNo = mapData[0][mapIndex.y][mapIndex.x];

	switch (mapNo)
		{
		case 29:
		case 30:
		case 31:
		case 39:
			ret = PASSABLE;
			break;
		case -1:
		case 26:
		case 27:
		case 28:
		case 35:
		case 36:
		case 43:
		case 44:
		case 45:
		case 46:
		case 47:
		case 51:
		case 52:
		case 53:
		case 54:
		case 55:
		case 70:
		case 71:
		case 78:
		case 79:
		case 86:
		case 87:
			ret = WALKTHROUGH;
			break;
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
		case 23:
		case 48:
		case 56:
			ret = UNPASSABLE;
			break;
		case 58:
		case 59:
		case 66:
		case 67:
		case 74:
		case 75:
			ret = CRITICAL;
			break;
		default:
			ret = WALKTHROUGH;
			break;
		}

	if (ret == WALKTHROUGH)
	{
		mapNo = mapData[1][mapIndex.y][mapIndex.x];

		switch (mapNo)
		{
		case 29:
		case 30:
		case 31:
		case 39:
			ret = PASSABLE;
			break;
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
		case 23:
		case 48:
		case 56:
			ret = UNPASSABLE;
			break;
		case 58:
		case 59:
		case 66:
		case 67:
		case 74:
		case 75:
			ret = CRITICAL;
			break;
		default:
			ret = WALKTHROUGH;
			break;
		}
	}

	return ret;
}

