#pragma once

#define CHARACTER_SIZE_X 192
#define CHARACTER_SIZE_Y 192
#define PLAYER_IMG_RUN_MAX 8
#define PLAYER_IMG_DIE_MAX 5
#define PLAYER_IMG_ATK_MAX 4
#define PLAYER_IMG_HIT_MAX 2
#define PLAYER_IMG_JUMP_MAX 4
#define GAP_TIME 10 //攻撃間の間隔

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
bool PlayerCollideCheck(int id, POS_F enemyPos, POS size);	//プレイヤーが他の物をぶつかるのチェック
bool GetPlayerFlag(int id);
POS_F GetPlayerCenterPos(int id);		//プレイヤーの中心点
POS_F GetAllPlayerAveragePos(void);		//プレイヤー達の座標の平均
void PlayerGetHit(int index, DIR damageDirection);		//敵に攻撃される
int GetPlayerGraphFilter(void);			//画像フィルター
int CheckPlayerGraphFilter(int RGB);	//設定された色
void MoveAllPlayerToStart(void);		//エンディング遷移用
int GetLivingPlayerCount(void);			//生きているプレイヤー数
int GetPlayerScore(int index);
void PlayerEndInit(void);				//エンディングの初期化
int GetPlayerImg(int index);			//プレイヤー画像
bool GetPlayerTrapFlag(int index);		


typedef struct {
	POS_F pos;		//座標
	POS_F hitPosS;	//キャラーの左上の座標
	POS_F hitPosE;	//キャラーの右下の座標
	int velocity;	//速度
	bool runFlag;
	bool atkFlag;
	bool deathFlag;	
	bool jumpFlag;
	bool hitFlag;
	bool trapHitFlag;
	bool visible;
	int animCnt;	//アニメーション用
	int filterRGB; //レベル補正フィルタ
	int atkGap;		//攻撃間の間隔
	DIR dir;		//方向
	int reviveCnt;	//復活の間隔
	int deathCnt;	//死ぬカウンター
	DIR knockDir;	//ノックバック時の移動方向
	int trapHitCnt;	//トラップ当たった数
	int score;
}CHARACTER;

extern int livingCnt;
extern int playerCnt;
extern int singleLiveCnt;

//CHARACTER player[PLAYER_MAX];