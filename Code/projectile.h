#pragma once
#define PROJECTILE_MAX 360
#define PROJECTILE_TYPE_MAX 9


enum PROJECTILE_TYPE
{
	PROJECTILE_FIREBALL1,
	PROJECTILE_FIREBALL2,
	PROJECTILE_FIREBALL3,
	PROJECTILE_FIREBALL4,
	PROJECTILE_ARROW,
	PROJECTILE_POISON,
	PROJECTILE_ICEBALL,
	PROJECTILE_BOSS2,
	PROJECTILE_FIREBALL5,
	PROJECTILE_CNT_MAX
};

void ProjectileSysInit(void);
void ProjectileGameInit(void);
void ProjectileInit(PROJECTILE_TYPE type, POS_F pos, bool gravity, bool spin, bool pierce, int speed, double angle, int RGB, int RGBFlag, int lasting);
void ProjectileLoop(void);
void ProjectileDraw(void);
int GetProjectileGraphFilter(void);

typedef struct {
	POS_F pos1;
	POS size;
	int lasting;
	int lastingCnt;
	int animCnt;
	int animCntMax;
	int speed;
	int filterRGB; //レベル補正フィルタ
	bool RGBFlag;
	bool gravity;
	bool spin;
	bool pierce;
	double angle;
	bool flag;
	bool reached;
	PROJECTILE_TYPE type;
	POS_F hitPosS;
	POS_F hitPosE;
	float offset;
}PROJECTILE;