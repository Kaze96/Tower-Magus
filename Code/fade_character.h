#pragma once

typedef struct {
	int graph;
	POS_F pos;
	bool visible;
	int animLasting;
	int filterRGB;
	DIR dir;
}CHARACTER_FADE;


void CharacterFadeInit(POS_F pos, int img, int RGB, DIR dir,int animLasting);
void CharacterFadeLoop(void);