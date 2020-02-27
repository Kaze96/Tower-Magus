#pragma once


enum KEY_MODE {
	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_SPACE,
	KEY_Z,
	KEY_X,
	KEY_C,
	KEY_A,
	KEY_S,
	KEY_D,
	KEY_Q,
	KEY_W,
	KEY_R,
	KEY_ESC,
	KEY_COMFIRM,
	KEY_CANCEL,
	KEY_OPTION,
	KEY_BACKSLASH,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_SHIFT,
	KEY_TAB,
	MOUSE_LEFT,
	MOUSE_RIGHT,
	MOUSE_MIDDLE,
	KEY_MAX
};

enum BUTTON_MODE {
	//0:up 1:down 2:left 3:right 4:start 5:screenshot 6:L3 7:R3 8:L1 9:R1 12:A 13:B 14:X 15:Y
	BUTTON_UP,
	BUTTON_DOWN,
	BUTTON_LEFT,
	BUTTON_RIGHT,
	BUTTON_START,
	BUTTON_OPTION,
	BUTTON_L3,
	BUTTON_R3,
	BUTTON_L2,
	BUTTON_R2,
	BUTTON_L1,
	BUTTON_R1,
	BUTTON_A,
	BUTTON_B,
	BUTTON_X,
	BUTTON_Y,
	BUTTON_MAX
};

extern bool newKey[KEY_MAX];
extern bool trgKey[KEY_MAX];
extern bool upKey[KEY_MAX];
extern bool oldKey[KEY_MAX];
extern bool trgButton[4][BUTTON_MAX];
extern bool upButton[4][BUTTON_MAX];
extern bool oldButton[4][BUTTON_MAX];
extern XINPUT_STATE input[4];

void DetectFlagInit(void);
void KeyCheck(void);
void PadMove(void);

bool PadDetect(void);