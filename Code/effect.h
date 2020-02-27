#pragma once

#define EFFECT_MAX 10000
#define EXPLOSION_DIV_LINE 12

enum EFFECT_TYPE{
	EFFECT_TYPE_NONE = -1,
	EFFECT_TYPE_EXPLOSION,
	EFFECT_TYPE_PROJECTILE,
	EFFECT_TYPE_JUMP,
	EFFECT_TYPE_HIT,
	EFFECT_TYPE_FADE,
	EFFECT_TYPE_REVIVE,
	EFFECT_TYPE_TEST,
	EFFECT_TYPE_MAX
};

typedef struct {
	POS_F pos;
	POS_F movPos;
	bool visible;
	int type;
	int speed;
	float angle;
	int tmpID;
	int hp;
	int hpMax;
	int rgbFilter;
	int img;
	POS_F velocity;
	POS size;
	int aniCnt;
}EFFECT;

void EffectSysInit(void);
void EffectGameInit(void);
void EffectControl(void);
void EffectDraw(void);
void SetEffect(POS_F pos, EFFECT_TYPE type);