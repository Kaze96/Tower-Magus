#pragma once

enum BLOCK_TYPE
{
	WALKTHROUGH,	//�w�i�`�b�v
	PASSABLE,		//�W�����v���ʂ��
	UNPASSABLE,		//�W�����v���ʂ�Ȃ�
	CRITICAL,		//�g���b�v
	JUMPPAD,
	BLOCK_TYPE_MAX
};

BLOCK_TYPE BlockCheck(POS_F pos);