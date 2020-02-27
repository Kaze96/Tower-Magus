#pragma once

#define ENEMY_IMG_RUN_MAX 8
#define ENEMY_IMG_DIE_MAX 5
#define ENEMY_IMG_ATTACK_MAX 3
#define ENEMY_DIV_MAX 12
#define ENEMY_MAX 50
#define BOSS_MAX 2
#define ENEMY_REGENERATE_COOLDOWN 50


enum ENEMY_ID {
	BARBARIAN_1,
	BARBARIAN_2,
	DRAGON_1,
	DRAGON_2,
	DWARF_1,
	DWARF_2,
	ELF_FEMALE,
	ELF_MALE,
	FIRE_DEMON,
	GARGOYLE,
	GOBLIN_1,
	GOBLIN_2,
	KNIGHT,
	LIZARD_MAN_1,
	LIZARD_MAN_2,
	MEDUSA,
	MINOTAUR,
	ORC_1,
	ORC_2,
	PEASANT,
	PRINCESS,
	SCORPION,
	SKELETON_1,
	SKELETON_2,
	SKELETON_3,
	SOLDIER,
	SPIDER,
	TREANT,
	TROLL,
	WIZARD,
	WOLF,
	ENEMY_ID_MAX,
};

enum ENEMY_TYPE {
	LAND,
	MOUNT,
	FLY_MELEE,
	FLY_RANGE,
	RANGED,
	DEMON,
	BOSS1,
	BOSS2,
	ENEMY_TYPE_MAX,
};

typedef struct {
	POS_F pos;
	POS size;
	POS hitPosS;
	POS hitPosE;
	int aniCnt;
	int visible;
	bool hitFlag;
	bool runFlag;
	bool atkFlag;
	bool dieFlag;
	bool jumpFlag;
	int id;
	float speed;
	DIR dir;
	ENEMY_TYPE type;
	POS velocity;
	double flyAngle;
	float hitpoint;
	int hitFlagCooldown;
}ENEMY;

typedef struct {
	int index;
	int lives;
	bool shield;
	double movAngle;
	double shotAngle;
}BOSS;

//extern ENEMY enemy[ENEMY_MAX];
extern int enemyImg[ENEMY_ID_MAX];

void EnemySysInit(void);
void EnemyGameInit(void);
void EnemyGameMain(void);
void EnemyGameDraw(void);
void EnemyInit(int id, POS_F pos);
void EnemyOrder(int index, bool runF, bool atkF, DIR dir, bool jumpF, double flyAngle);
void BossOrder(int index, double movAngle, double shotAngle, int movSpeed);
POS_F GetEnemyPos(int index);	// “G‚ÌˆÊ’u‚ðŽæ‚é
POS GetEnemyImgSize2(int index);
int LoadEnemyImg(int index);	// “G‚Ì‰æ‘œ‚ðŽæ‚é
int GetEnemyImg(void);
bool GetEnemyFlag(int index);
POS LoadEnemyImgSize(int index);
POS GetEnemyImgSize(void);		// “G‚Ì‰æ‘œƒTƒCƒY‚ðŽæ‚é
bool EnemyCollideCheck(int id, POS_F pos, POS_F size);
bool EnemyGetHit(int index, DIR dir);
void BossEffect(int index);
void MoveBossToStart(void);