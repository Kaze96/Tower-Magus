#include "main.h"
#include "sound.h"

int stageMusic;
int soundBall;
int soundHit;
int damgeSound[4];
int soundRevive;
int soundWalk;
int soundJump;
int soundIceball;
int soundPoison;
int soundWallImpact;
int soundMovement;
int soundCharge;
int soundArrow;
int soundSlash[2];
int soundBeBouss;
int soundBossMusic;
int soundEnding;
int soundBossEn[2];
int soundBossDe[2];
int soundShield[2];
int soundFireBall;
bool stopFlag;
int volume;
int tmpStopMusic;

void SoundSysInit(void)
{
	stageMusic = LoadSoundMem("Sound/Stage Music.wav");
	soundBall = LoadSoundMem("Sound/Ball.wav");
	soundHit = LoadSoundMem("Sound/Hit.wav");
	soundRevive = LoadSoundMem("Sound/Revive.wav");
	soundWalk = LoadSoundMem("Sound/Walk.wav");
	soundJump = LoadSoundMem("Sound/Jump.wav");
	soundIceball = LoadSoundMem("Sound/Iceball.wav");
	soundPoison = LoadSoundMem("Sound/Poison.wav");
	soundWallImpact = LoadSoundMem("Sound/wallimpact.wav");
	soundMovement = LoadSoundMem("Sound/highspeed-movement1.wav");
	soundCharge = LoadSoundMem("Sound/charge.wav");
	soundArrow = LoadSoundMem("Sound/Arrow.wav");
	soundSlash[0] = LoadSoundMem("Sound/Slash01.wav");
	soundSlash[1] = LoadSoundMem("Sound/Slash02.wav");
	soundBeBouss = LoadSoundMem("Sound/before-boss.wav");
	soundBossMusic = LoadSoundMem("Sound/Boss.wav");
	soundEnding = LoadSoundMem("Sound/Ending.ogg");
	soundShield[0] = LoadSoundMem("Sound/Shield Start.wav");
	soundShield[1] = LoadSoundMem("Sound/Shield Hit.wav");
	soundFireBall = LoadSoundMem("Sound/fireball.wav");

	char tmpString[256];
	for (int i = 0; i < 4; i++)
	{
		sprintf_s(tmpString, "Sound/Damage%02d.wav", i + 1);
		damgeSound[i] = LoadSoundMem(tmpString);
	}
	for (int i = 0; i < 2; i++)
	{
		sprintf_s(tmpString, "Sound/Boss%d Entrance.wav", i + 1);
		soundBossEn[i] = LoadSoundMem(tmpString);

		sprintf_s(tmpString, "Sound/Boss%d Dead.wav", i + 1);
		soundBossDe[i] = LoadSoundMem(tmpString);
	}

	SetLoopSamplePosSoundMem(325506, stageMusic);
	ChangeVolumeSoundMem(255 * 60 / 100, soundJump);
}

void SoundGameInit(void)
{
	stopFlag = false;
	volume = 255;
	tmpStopMusic = 0;
	ChangeVolumeSoundMem(255, stageMusic);
	ChangeVolumeSoundMem(255, soundBossMusic);
}

void SoundControl(void)
{
	if (stopFlag)
	{
		ChangeVolumeSoundMem(volume, tmpStopMusic);
		volume--;
		if (volume <= 0)
		{
			StopSoundMem(tmpStopMusic);
			volume = 255;
			stopFlag = false;
		}
	}
}

void PlaySoundType(SOUND_TYPE id)
{
	int tmpRand;
	switch (id)
	{
	case SOUND_TYPE_STAGE:
		PlaySoundMem(stageMusic, DX_PLAYTYPE_LOOP, true);
		break;
	case SOUND_TYPE_BALL:
		PlaySoundMem(soundBall, DX_PLAYTYPE_BACK, true);
		break;
	case SOUND_TYPE_HIT:
		PlaySoundMem(soundHit, DX_PLAYTYPE_BACK, true);
		break;
	case SOUND_TYPE_DAMAGE_P:
		tmpRand = GetRand(1);
		PlaySoundMem(damgeSound[tmpRand], DX_PLAYTYPE_BACK, true);
		break;
	case SOUND_TYPE_DAMAGE_E:
		PlaySoundMem(damgeSound[3], DX_PLAYTYPE_BACK, true);
		break;
	case SOUND_TYPE_DIE:
		PlaySoundMem(damgeSound[2], DX_PLAYTYPE_BACK, true);
		break;
	case SOUND_TYPE_REVIVE:
		PlaySoundMem(soundRevive, DX_PLAYTYPE_BACK, true);
		break;
	case SOUND_TYPE_WALK:
		if (CheckSoundMem(soundWalk) == 0) PlaySoundMem(soundWalk, DX_PLAYTYPE_BACK, false);
		break;
	case SOUND_TYPE_JUMP:
		PlaySoundMem(soundJump, DX_PLAYTYPE_BACK, true);
		break;
	case SOUND_TYPE_ICEBALL:
		PlaySoundMem(soundIceball, DX_PLAYTYPE_BACK, true);
		break;
	case SOUND_TYPE_POISON:
		PlaySoundMem(soundPoison, DX_PLAYTYPE_BACK, true);
		break;
	case SOUND_TYPE_WALLIMPACT:
		PlaySoundMem(soundWallImpact, DX_PLAYTYPE_BACK, true);
		break;
	case SOUND_TYPE_MOVEMENT:
		PlaySoundMem(soundMovement, DX_PLAYTYPE_BACK, true);
		break;
	case SOUND_TYPE_CHARGE:
		PlaySoundMem(soundCharge, DX_PLAYTYPE_BACK, true);
		break;
	case SOUND_TYPE_ARROW:
		PlaySoundMem(soundArrow, DX_PLAYTYPE_BACK, true);
		break;
	case SOUND_TYPE_SLASH:
		tmpRand = GetRand(1);
		if (CheckSoundMem(soundSlash[0]) == 0 && CheckSoundMem(soundSlash[1] == 0))
		{
			PlaySoundMem(soundSlash[tmpRand], DX_PLAYTYPE_BACK, true);
		}
		break;
	case SOUND_TYPE_BOSSEN1:
		PlaySoundMem(soundBossEn[0], DX_PLAYTYPE_BACK, true);
		break;
	case SOUND_TYPE_BOSSEN2:
		PlaySoundMem(soundBossEn[1], DX_PLAYTYPE_BACK, true);
		break;
	case SOUND_TYPE_BOSSDE1:
		PlaySoundMem(soundBossDe[0], DX_PLAYTYPE_BACK, true);
		break;
	case SOUND_TYPE_BOSSDE2:
		PlaySoundMem(soundBossDe[1], DX_PLAYTYPE_BACK, true);
		break;
	case SOUND_TYPE_SHIELD:
		PlaySoundMem(soundShield[0], DX_PLAYTYPE_BACK, true);
		break;
	case SOUND_TYPE_SHIELDH:
		PlaySoundMem(soundShield[1], DX_PLAYTYPE_BACK, true);
		break;
	case SOUND_TYPE_BEBOSS:
		PlaySoundMem(soundBeBouss, DX_PLAYTYPE_BACK, true);
		break;
	case SOUND_TYPE_BOSS:
		SetLoopSamplePosSoundMem(423360, soundBossMusic);
		PlaySoundMem(soundBossMusic, DX_PLAYTYPE_LOOP, true);
		break;
	case SOUND_TYPE_FIREBALL:
		PlaySoundMem(soundFireBall, DX_PLAYTYPE_BACK, true);
		break;
	case SOUND_TYPE_ENDING:
		PlaySoundMem(soundEnding, DX_PLAYTYPE_LOOP,false);
		break;
	}
}

void StopSoundType(SOUND_TYPE id)
{
	if (id == SOUND_TYPE_STAGE)
	{
		if (CheckSoundMem(stageMusic) == 1)
		{
			stopFlag = true;
			tmpStopMusic = stageMusic;
		}
	}
	if (id == SOUND_TYPE_BOSS)
	{
		if (CheckSoundMem(soundBossMusic) == 1)
		{
			stopFlag = true;
			tmpStopMusic = soundBossMusic;
		}
	}
	if (id == SOUND_TYPE_ENDING) StopSoundMem(soundEnding);
	if (id == SOUND_TYPE_BEBOSS) StopSoundMem(soundBeBouss);
}

void StopAllSound(void)
{
	StopSoundMem(stageMusic);
	StopSoundMem(soundBossMusic);
	StopSoundMem(soundEnding);
}
