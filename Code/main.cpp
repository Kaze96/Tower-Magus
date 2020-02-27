#include <time.h>
#include "main.h"
#include "keycheck.h"
#include "map_edit.h"
#include "ending.h"
#include "stage.h"
#include "background.h"
#include "enemy.h"
#include "enemyAI.h"
#include "projectile.h"
#include "Character.h"
#include "title.h"
#include "fade.h"
#include "effect.h"
#include "checkhit.h"
#include "sound.h"
#include "fade_character.h"
#include "Score.h"
#include "shield.h"
#include "mapdata.h"
#include "tutorial.h"

//------------ Screen Initialization ------------
int x, add;
int WindowFullSize;
int WindowPosX;
int WindowPosY;
int DesktopSizeX;
int DesktopSizeY;
int SubScreen;
int WindowX;
POS_F tmp;
int font;
int jfont;

GAME_MODE gameMode;

int gameCnt;

//�v���g�^�C�v�錾
bool SystemInit(void);
void GameInit(void);
void GameMain(void);
void GameMainDraw(void);

// ========= WinMain�֐�s
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// �V�X�e���̏�����
	if (SystemInit() == false)
	{
		return -1;
	}

	// --------�Q�[�����[�v
	while (ProcessMessage() == 0)
	{
		KeyCheck();
		PadMove();
		GetMouseData();
		if (trgButton[0][BUTTON_OPTION] || trgKey[KEY_TAB]) ReSetupJoypad();
		
		///------------------------------------------------SCREEN SETTING START---------------------------------------
		// Escape Key to change window mode
		if (trgKey[KEY_F5] == true)
		{
			if (WindowFullSize)
			{
				// �t���X�N���[�����[�h�������ꍇ�͒ʏ탂�[�h�ɕύX����
				WindowFullSize = FALSE;

				// �E�C���h�E�X�^�C����W���ɕύX
				SetWindowStyleMode(0);

				// �E�C���h�E�T�C�Y�� 1600x900 �ɕύX
				SetWindowSize(SCREEN_SIZE_X, SCREEN_SIZE_Y);

				// �E�C���h�E���ő剻�O�̍��W�Ɉړ�
				SetWindowPosition(WindowPosX, WindowPosY);
			}
			else
			{
				// �ʏ탂�[�h�������ꍇ�̓t���X�N���[�����[�h�ɕύX����
				WindowFullSize = TRUE;

				// ���݂̃E�C���h�E�̈ʒu���擾���Ă���
				GetWindowPosition(&WindowPosX, &WindowPosY);

				// �E�C���h�E�X�^�C����g�Ȃ��ɕύX
				SetWindowStyleMode(4);

				// �E�C���h�E�̃T�C�Y���f�X�N�g�b�v�Ɠ����ɂ���
				SetWindowSize(DesktopSizeX, DesktopSizeY);

				// �E�C���h�E�̈ʒu����ʍ���ɕύX
				SetWindowPosition(0, 0);

				// ����ʂ��N���A
				SetDrawScreen(DX_SCREEN_BACK);
				ClearDrawScreen();
			}
		}

		// �ړ�
		x += add;
		if (x < 0 || x > 640) add = -add;

		// �t���X�N���[�����[�h���ǂ����ŏ����𕪊�
		if (WindowFullSize)
		{
			// �t���X�N���[�����[�h�̏ꍇ

			// �`��������ʂɕύX
			SetDrawScreen(SubScreen);
		}
		else
		{
			// �ʏ탂�[�h�̏ꍇ

			// �`���𗠉�ʂɕύX
			SetDrawScreen(DX_SCREEN_BACK);
		}
		///------------------------------------------------SCREEN SETTING END---------------------------------------

		ClsDrawScreen();	//��ʏ���

		switch (gameMode)
		{
		case GAME_INIT:
			GameInit();
			break;
		case MAP_EDIT_INIT:
			MapEditInit();
			break;
		case MAP_EDIT:
			MapEdit();
			StageMain();
			break;
		case GAME_PLAY:
			GameMain();
			break;
		case GAME_TITLE:
			GameTitle();
			break;
		case GAME_END:
			EndingMain();
			gameCnt++;
			break;
		}
		///------------------------------------------------SCREEN SETTING START---------------------------------------

		// �t���X�N���[�����[�h�̏ꍇ�̂݉���ʂ��g�債�ė���ʂɕ`�悷�鏈�����s��
		int DrawSizeX;
		int DrawSizeY;
		if (WindowFullSize)
		{
			// �`���𗠉�ʂɕύX
			SetDrawScreen(DX_SCREEN_BACK);

			// ����ʂ��f�X�N�g�b�v��ʂ̒��S�Ɋg�債�ĕ`�悷��
			if ((float)SCREEN_SIZE_X / SCREEN_SIZE_Y < (float)DesktopSizeX / DesktopSizeY)
			{
				DrawSizeX = DesktopSizeY * SCREEN_SIZE_X / SCREEN_SIZE_Y;
				DrawSizeY = DesktopSizeY;
			}
			else
			{
				DrawSizeX = DesktopSizeX;
				DrawSizeY = DesktopSizeX * SCREEN_SIZE_Y / SCREEN_SIZE_X;
			}

			DrawExtendGraph(
				(DesktopSizeX - DrawSizeX) / 2,
				(DesktopSizeY - DrawSizeY) / 2,
				DesktopSizeX - (DesktopSizeX - DrawSizeX) / 2,
				DesktopSizeY - (DesktopSizeY - DrawSizeY) / 2,
				SubScreen,
				FALSE
			);
		}
		///------------------------------------------------SCREEN SETTING END---------------------------------------
		
		ScreenFlip();		//����ʂ�\��ʂɏu�ԃR�s�[
	}

	// Game End -
	DxLib_End();			// DX���C�u�����̏I������
	return 0;				//���̃v���O�����̏I��
}

//---------------------------------------------
// �V�X�e���̏�����
bool SystemInit(void)
{
	// ----------�V�X�e������
	SetWindowText("Group Project");
	//�V�X�e������
	SetGraphMode(SCREEN_SIZE_X, SCREEN_SIZE_Y, 16);
	ChangeWindowMode(true);							//true:window false:�t���X�N���[��
	if (DxLib_Init() == -1)							//DX���C�u��������������
	{
		return false;
	}
	SetWindowSizeChangeEnableFlag(true);

	// �f�X�N�g�b�v�̃T�C�Y���擾����
	GetDefaultState(&DesktopSizeX, &DesktopSizeY, NULL);

	// �E�C���h�E�̃T�C�Y���ω����Ă��g�債�Ȃ��ݒ�ɂ���
	SetWindowSizeChangeEnableFlag(FALSE, FALSE);

	// ��ʃT�C�Y���f�X�N�g�b�v�T�C�Y�ɐݒ�
	SetGraphMode(DesktopSizeX, DesktopSizeY, 32);

	// �E�C���h�E�̏����T�C�Y�͎��ۂ̏����p�̃T�C�Y
	SetWindowSize(SCREEN_SIZE_X, SCREEN_SIZE_Y);

	// �t���X�N���[�����[�h���Ɏg�p������ۂ̏����p�̉𑜓x��������ʂ��쐬
	SubScreen = MakeScreen(SCREEN_SIZE_X, SCREEN_SIZE_Y);

	// �ʏ탂�[�h�������ꍇ�̓t���X�N���[�����[�h�ɕύX����
	WindowFullSize = FALSE;

	// ���݂̃E�C���h�E�̈ʒu���擾���Ă���
	SetWindowPosition(WindowPosX, WindowPosY);

	// �E�C���h�E�X�^�C����g�Ȃ��ɕύX
	SetWindowStyleMode(0);

	// �E�C���h�E�̃T�C�Y���f�X�N�g�b�v�Ɠ����ɂ���
	SetWindowSize(SCREEN_SIZE_X, SCREEN_SIZE_Y);

	// �E�C���h�E�̈ʒu����ʍ���ɕύX
	SetWindowPosition(0, 0);

	// ����ʂ��N���A
	SetDrawScreen(DX_SCREEN_BACK);
	ClearDrawScreen();

	// ���C�����[�v(�����L�[�������ꂽ�烋�[�v�𔲂���)
	x = 0;
	add = 8;
	tmp.x = 0;
	tmp.y = 0;
	// ------ �������@---------
	MapDataSysInit();
	BackgroundSysInit();
	EnemySysInit();
	EffectSysInit();
	PlayerSysInit();
	ProjectileSysInit();
	SoundSysInit();
	EnemyAISysInit();
	EndingSysInit();
	ShieldSysInit();
	GameTitleSysInit();
	TutorialSysInit();

	// ------�ϐ��������@-----------
	AddFontResourceEx("Font/Centurion 8x16 Monospaced.TTF", FR_PRIVATE, NULL);
	AddFontResourceEx("Font/GD-DOTFONT-DQ-OTF_008.otf", FR_PRIVATE, NULL);
	font = CreateFontToHandle("Centurion 8x16 Monospaced", 24, -1, DX_FONTTYPE_ANTIALIASING_EDGE_8X8);
	jfont = CreateFontToHandle("GD-DOTFONT-DQ-OTF", 48, -1, DX_FONTTYPE_ANTIALIASING_EDGE_8X8);
	gameMode = GAME_INIT;
	srand((unsigned int)time(NULL));

	
	return true;
}

void GameInit(void)
{
	DetectFlagInit();
	EnemyGameInit();
	MapDataGameInit();

	StageInit();
	BackgroundInit();
	
	EffectGameInit();
	ProjectileGameInit();

	GameTitleGameInit();
	ScoreGameInit();
	ShieldGameInit();
	TutorialGameInit();
	SoundGameInit();
	gameCnt = 0;
	playerCnt = 0;
	StopAllSound();
	gameMode = GAME_TITLE;
}

//---------------------------------------------
// �v���C������
void GameMain(void)
{	
	gameCnt++;

	if (WindowFullSize)
	{
		DrawString(4, 2, "borderless fullscreen", 0xFFFFFF);
	}
	else
	{
		DrawString(4, 2, "window(border)", 0xFFFFFF);
	}

	MapDataControl();
	StageMain();
	BackgroundMain(SCROLL_SPEED_PLUS, 1.20f, 0.40f);
	EnemyGameMain();
	EffectControl();
	PlayerControl();
	ProjectileLoop();
	ScoreControl();
	ShieldControl();
	TutorialControl();
	SoundControl();

	if (!endFlag)
	{
		FadeLoop();
	}
	else
	{
		if (FadeLoop() == true)
		{
			EndingInit();
			PlayerEndInit();
		}
	}

	if (livingCnt <= 0 || singleLiveCnt <= 0)
	{
		if (trgKey[KEY_Z] || trgButton[0][BUTTON_X])
		{
			GameInit();
		}
	}

	GameMainDraw();
}

void GameMainDraw(void)
{
	int Color = GetColor(255, 255, 255);
	
	BackgroundDraw(true, true,false);
	MapDataDraw();
	CharacterFadeLoop();
	ShieldDraw();
	PlayerDraw();
	EnemyGameDraw();
	EffectDraw();
	ProjectileDraw();
	ScoreDraw();
	TutorialDraw();
	
	if (livingCnt <= 0 || singleLiveCnt <= 0) DrawExtendStringToHandle(SCREEN_SIZE_X / 3, SCREEN_SIZE_Y / 2 - 100,4,4, "GAME OVER", 0xffffff, font);
	if (livingCnt <= 0 || singleLiveCnt <= 0) DrawExtendStringToHandle(SCREEN_SIZE_X / 3, SCREEN_SIZE_Y / 3 * 2, 2, 2, "PRESS START BUTTON", 0xffffff, font);

}

void GameModeChange(GAME_MODE mode)
{
	gameMode = mode;
}