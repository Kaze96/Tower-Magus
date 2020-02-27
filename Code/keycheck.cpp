#include "main.h"
#include "Character.h"
#include "keycheck.h"

bool newKey[KEY_MAX];
bool trgKey[KEY_MAX];
bool upKey[KEY_MAX];
bool oldKey[KEY_MAX];
bool trgButton[PLAYER_MAX][BUTTON_MAX];
bool upButton[PLAYER_MAX][BUTTON_MAX];
bool oldButton[PLAYER_MAX][BUTTON_MAX];
//0:up 1:down 2:left 3:right 4:start 5:screenshot 6:L3 7:R3 8:L1 9:R1 12:A 13:B 14:X 15:Y
XINPUT_STATE input[PLAYER_MAX];
bool detectFlag[PLAYER_MAX]; 


void DetectFlagInit(void)
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		detectFlag[i] = false;
	}
}

void KeyCheck(void)
{
	for (int i = 0; i < KEY_MAX; i++) {
		newKey[i] = false;
		trgKey[i] = false;
		upKey[i] = false;
	}

	if (CheckHitKey(KEY_INPUT_UP)) newKey[KEY_UP] = true;
	if (CheckHitKey(KEY_INPUT_RIGHT)) newKey[KEY_RIGHT] = true;
	if (CheckHitKey(KEY_INPUT_DOWN)) newKey[KEY_DOWN] = true;
	if (CheckHitKey(KEY_INPUT_LEFT)) newKey[KEY_LEFT] = true;
	if (CheckHitKey(KEY_INPUT_Z)) newKey[KEY_Z] = true;
	if (CheckHitKey(KEY_INPUT_X)) newKey[KEY_X] = true;
	if (CheckHitKey(KEY_INPUT_A)) newKey[KEY_A] = true;
	if (CheckHitKey(KEY_INPUT_D)) newKey[KEY_D] = true;
	if (CheckHitKey(KEY_INPUT_W)) newKey[KEY_W] = true;
	if (CheckHitKey(KEY_INPUT_Q)) newKey[KEY_Q] = true;
	if (CheckHitKey(KEY_INPUT_ESCAPE)) newKey[KEY_ESC] = true;
	if (CheckHitKey(KEY_INPUT_SPACE)) newKey[KEY_SPACE] = true;
	if (CheckHitKey(KEY_INPUT_SPACE) || CheckHitKey(KEY_INPUT_Z) || CheckHitKey(KEY_INPUT_RETURN)) newKey[KEY_COMFIRM] = true;
	if (CheckHitKey(KEY_INPUT_X) || CheckHitKey(KEY_INPUT_BACK)) newKey[KEY_CANCEL] = true;
	if (CheckHitKey(KEY_INPUT_ESCAPE) == 0) newKey[KEY_OPTION] = true;
	if (CheckHitKey(KEY_INPUT_F2)) newKey[KEY_F2] = true;
	if (CheckHitKey(KEY_INPUT_F3)) newKey[KEY_F3] = true;
	if (CheckHitKey(KEY_INPUT_F4)) newKey[KEY_F4] = true;
	if (CheckHitKey(KEY_INPUT_F5)) newKey[KEY_F5] = true;
	if (CheckHitKey(KEY_INPUT_BACKSLASH)) newKey[KEY_BACKSLASH] = true;
	if (CheckHitKey(KEY_INPUT_LSHIFT)) newKey[KEY_SHIFT] = true;
	if (CheckHitKey(KEY_INPUT_C)) newKey[KEY_C] = true;
	if (CheckHitKey(KEY_INPUT_R)) newKey[KEY_R] = true;
	if (CheckHitKey(KEY_INPUT_TAB)) newKey[KEY_TAB] = true;

	// Mouse Input
	if (GetMouseInput() & MOUSE_INPUT_LEFT) newKey[MOUSE_LEFT] = true;
	if (GetMouseInput() & MOUSE_INPUT_RIGHT) newKey[MOUSE_RIGHT] = true;
	if (GetMouseInput() & MOUSE_INPUT_MIDDLE) newKey[MOUSE_MIDDLE] = true;

	for (int i = 0; i < KEY_MAX; i++) 
	{
		trgKey[i] = newKey[i] & ~oldKey[i];	// trgKey
		upKey[i] = ~newKey[i] & oldKey[i];	// upKey
		oldKey[i] = newKey[i];	// oldKey
	}
}

void PadMove(void)
{
	for (int j = 0; j < PLAYER_MAX; j++)
	{
		for (int i = 0; i < BUTTON_MAX; i++)
		{
			input[j].Buttons[i] = false;
			trgButton[j][i] = false;
			upButton[j][i] = false;
			
		}
		//XBox360 controller setup
		GetJoypadXInputState(j + 1, &input[j]);
		if (input[j].ThumbLX < -15000) input[j].Buttons[BUTTON_LEFT] = true;
		if (input[j].ThumbLX > 15000) input[j].Buttons[BUTTON_RIGHT] = true;
		/*if (input[j].ThumbLY > 15000) input[j].Buttons[BUTTON_UP] = true;
		if (input[j].ThumbLY < -15000) input[j].Buttons[BUTTON_DOWN] = true;*/


		for (int i = 0; i < BUTTON_MAX; i++)
		{
			//if (input[j].Buttons[i]) newKey[keyChgTbl[i]] = true;
			trgButton[j][i] = input[j].Buttons[i] & ~oldButton[j][i];	// trgKey
			upButton[j][i] = ~input[j].Buttons[i] & oldButton[j][i];	// upKey
			oldButton[j][i] = input[j].Buttons[i];	// oldKey
		}
	}
}

bool PadDetect(void)
{
	for (int j = 0; j < PLAYER_MAX; j++)
	{
		if (trgButton[j][BUTTON_START] && !detectFlag[j])
		{
			detectFlag[j] = true;
			return true;
		}
	}
	return false;
}
