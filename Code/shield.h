#pragma once

typedef struct {
	int index;
	int hpMax;
	int hp;
	bool visible;
	int aniCnt;
}SHIELD;

void ShieldSysInit(void);
void ShieldGameInit(void);
void ShieldControl(void);
bool ShieldDamage(void);
void ShieldDraw(void);
void EffectShieldInit(int index, int hp);