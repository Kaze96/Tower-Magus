#pragma once
#include "enemy.h"

typedef struct {
	POS hitPosS;
	POS hitPosE;
	bool runFlag;
	bool atkFlag;
	bool dieFlag;
	bool lifeFlag;
	bool jumpFlag;
	DIR dir;
	double flyAngle;
	float speed;
	int attackRange;
	int attackInterval;
	int attackIntervalCnt;
	int targetPlayer;
}ENEMY_AI;

void EnemyAISysInit(void);
void EnemyAIInit(int index, int speed, int atkspeed, DIR dir, POS hitPosS, POS hitPosE);
bool EnemyAIMain(int index, ENEMY_TYPE type, POS_F pos);
ENEMY_TYPE EnemyClassification(int id);
void LastBossSwitch(bool boolean);