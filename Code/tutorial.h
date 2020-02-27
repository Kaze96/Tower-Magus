#pragma once

enum BUTTON_IMG {
	BUTTON_NONE = -1,
	BUTTON_IMG_A,
	BUTTON_IMG_B,
	DPAD,
	JOYSTICK,
	BUTTON_IMG_MAX
};

typedef struct {
	POS_F pos;
	bool visible;
	int hp;
	int color;
	const char *tmpText;
	BUTTON_IMG type;
}TEXT;

void TutorialSysInit(void);
void TutorialGameInit(void);
void TutorialControl(void);
void TutorialDraw(void);
void SetText(POS_F pos, const char *string, int color, BUTTON_IMG type, int flag);