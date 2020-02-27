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

//プロトタイプ宣言
bool SystemInit(void);
void GameInit(void);
void GameMain(void);
void GameMainDraw(void);

// ========= WinMain関数s
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// システムの初期化
	if (SystemInit() == false)
	{
		return -1;
	}

	// --------ゲームループ
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
				// フルスクリーンモードだった場合は通常モードに変更する
				WindowFullSize = FALSE;

				// ウインドウスタイルを標準に変更
				SetWindowStyleMode(0);

				// ウインドウサイズを 1600x900 に変更
				SetWindowSize(SCREEN_SIZE_X, SCREEN_SIZE_Y);

				// ウインドウを最大化前の座標に移動
				SetWindowPosition(WindowPosX, WindowPosY);
			}
			else
			{
				// 通常モードだった場合はフルスクリーンモードに変更する
				WindowFullSize = TRUE;

				// 現在のウインドウの位置を取得しておく
				GetWindowPosition(&WindowPosX, &WindowPosY);

				// ウインドウスタイルを枠なしに変更
				SetWindowStyleMode(4);

				// ウインドウのサイズをデスクトップと同じにする
				SetWindowSize(DesktopSizeX, DesktopSizeY);

				// ウインドウの位置を画面左上に変更
				SetWindowPosition(0, 0);

				// 裏画面をクリア
				SetDrawScreen(DX_SCREEN_BACK);
				ClearDrawScreen();
			}
		}

		// 移動
		x += add;
		if (x < 0 || x > 640) add = -add;

		// フルスクリーンモードかどうかで処理を分岐
		if (WindowFullSize)
		{
			// フルスクリーンモードの場合

			// 描画先を仮画面に変更
			SetDrawScreen(SubScreen);
		}
		else
		{
			// 通常モードの場合

			// 描画先を裏画面に変更
			SetDrawScreen(DX_SCREEN_BACK);
		}
		///------------------------------------------------SCREEN SETTING END---------------------------------------

		ClsDrawScreen();	//画面消去

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

		// フルスクリーンモードの場合のみ仮画面を拡大して裏画面に描画する処理を行う
		int DrawSizeX;
		int DrawSizeY;
		if (WindowFullSize)
		{
			// 描画先を裏画面に変更
			SetDrawScreen(DX_SCREEN_BACK);

			// 仮画面をデスクトップ画面の中心に拡大して描画する
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
		
		ScreenFlip();		//裏画面を表画面に瞬間コピー
	}

	// Game End -
	DxLib_End();			// DXライブラリの終了処理
	return 0;				//このプログラムの終了
}

//---------------------------------------------
// システムの初期化
bool SystemInit(void)
{
	// ----------システム処理
	SetWindowText("Group Project");
	//システム処理
	SetGraphMode(SCREEN_SIZE_X, SCREEN_SIZE_Y, 16);
	ChangeWindowMode(true);							//true:window false:フルスクリーン
	if (DxLib_Init() == -1)							//DXライブラリ初期化処理
	{
		return false;
	}
	SetWindowSizeChangeEnableFlag(true);

	// デスクトップのサイズを取得する
	GetDefaultState(&DesktopSizeX, &DesktopSizeY, NULL);

	// ウインドウのサイズが変化しても拡大しない設定にする
	SetWindowSizeChangeEnableFlag(FALSE, FALSE);

	// 画面サイズをデスクトップサイズに設定
	SetGraphMode(DesktopSizeX, DesktopSizeY, 32);

	// ウインドウの初期サイズは実際の処理用のサイズ
	SetWindowSize(SCREEN_SIZE_X, SCREEN_SIZE_Y);

	// フルスクリーンモード時に使用する実際の処理用の解像度を持つ仮画面を作成
	SubScreen = MakeScreen(SCREEN_SIZE_X, SCREEN_SIZE_Y);

	// 通常モードだった場合はフルスクリーンモードに変更する
	WindowFullSize = FALSE;

	// 現在のウインドウの位置を取得しておく
	SetWindowPosition(WindowPosX, WindowPosY);

	// ウインドウスタイルを枠なしに変更
	SetWindowStyleMode(0);

	// ウインドウのサイズをデスクトップと同じにする
	SetWindowSize(SCREEN_SIZE_X, SCREEN_SIZE_Y);

	// ウインドウの位置を画面左上に変更
	SetWindowPosition(0, 0);

	// 裏画面をクリア
	SetDrawScreen(DX_SCREEN_BACK);
	ClearDrawScreen();

	// メインループ(何かキーが押されたらループを抜ける)
	x = 0;
	add = 8;
	tmp.x = 0;
	tmp.y = 0;
	// ------ 初期化　---------
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

	// ------変数初期化　-----------
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
// プレイ中処理
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