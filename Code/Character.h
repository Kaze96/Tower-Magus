#pragma once

#define CHARACTER_SIZE_X 192
#define CHARACTER_SIZE_Y 192
#define PLAYER_IMG_RUN_MAX 8
#define PLAYER_IMG_DIE_MAX 5
#define PLAYER_IMG_ATK_MAX 4
#define PLAYER_IMG_HIT_MAX 2
#define PLAYER_IMG_JUMP_MAX 4
#define GAP_TIME 10 //�U���Ԃ̊Ԋu

enum PLAYER {
	PLAYER_1,
	PLAYER_2,
	PLAYER_3,
	PLAYER_4,
	PLAYER_MAX,
};

void PlayerSysInit(void);
void PlayerGameInit(void);
void PlayerControl(void);
void PlayerDraw(void);

POS_F GetPlayerPos(int id);
bool PlayerCollideCheck(int id, POS_F enemyPos, POS size);	//�v���C���[�����̕����Ԃ���̃`�F�b�N
bool GetPlayerFlag(int id);
POS_F GetPlayerCenterPos(int id);		//�v���C���[�̒��S�_
POS_F GetAllPlayerAveragePos(void);		//�v���C���[�B�̍��W�̕���
void PlayerGetHit(int index, DIR damageDirection);		//�G�ɍU�������
int GetPlayerGraphFilter(void);			//�摜�t�B���^�[
int CheckPlayerGraphFilter(int RGB);	//�ݒ肳�ꂽ�F
void MoveAllPlayerToStart(void);		//�G���f�B���O�J�ڗp
int GetLivingPlayerCount(void);			//�����Ă���v���C���[��
int GetPlayerScore(int index);
void PlayerEndInit(void);				//�G���f�B���O�̏�����
int GetPlayerImg(int index);			//�v���C���[�摜
bool GetPlayerTrapFlag(int index);		


typedef struct {
	POS_F pos;		//���W
	POS_F hitPosS;	//�L�����[�̍���̍��W
	POS_F hitPosE;	//�L�����[�̉E���̍��W
	int velocity;	//���x
	bool runFlag;
	bool atkFlag;
	bool deathFlag;	
	bool jumpFlag;
	bool hitFlag;
	bool trapHitFlag;
	bool visible;
	int animCnt;	//�A�j���[�V�����p
	int filterRGB; //���x���␳�t�B���^
	int atkGap;		//�U���Ԃ̊Ԋu
	DIR dir;		//����
	int reviveCnt;	//�����̊Ԋu
	int deathCnt;	//���ʃJ�E���^�[
	DIR knockDir;	//�m�b�N�o�b�N���̈ړ�����
	int trapHitCnt;	//�g���b�v����������
	int score;
}CHARACTER;

extern int livingCnt;
extern int playerCnt;
extern int singleLiveCnt;

//CHARACTER player[PLAYER_MAX];