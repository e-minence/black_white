//============================================================
//       power_conv.rb �ŏo�͂��ꂽ�t�@�C���ł�
//============================================================

#pragma once

typedef enum{
	GPOWER_ID_DISTRIBUTION_START = 33,		//�z�zG�p���[�J�nID(����ID���܂�)
	GPOWER_ID_DISTRIBUTION_END = 38,		//�z�zG�p���[�I��ID(����ID���܂�)

	GPOWER_ID_PALACE_START = 39,		//�p���XG�p���[�J�nID(����ID���܂�)
	GPOWER_ID_PALACE_END = 47,		//�p���XG�p���[�I��ID(����ID���܂�)

	GPOWER_ID_MAX = 48,		//G�p���[�ő吔
	GPOWER_ID_NULL = GPOWER_ID_MAX,		//G�p���[���������Ă��Ȃ�
}GPOWER_ID;



//���ʌn���@�@!!�o�͌��f�[�^�ɂ����enum�̕��т��ς��ꍇ������܂�!!
typedef enum{
	GPOWER_TYPE_ENCOUNT_UP,		//0
	GPOWER_TYPE_ENCOUNT_DOWN,		//1
	GPOWER_TYPE_HATCH_UP,		//2
	GPOWER_TYPE_NATSUKI_UP,		//3
	GPOWER_TYPE_SALE,		//4
	GPOWER_TYPE_HP_RESTORE,		//5
	GPOWER_TYPE_PP_RESTORE,		//6
	GPOWER_TYPE_EXP_UP,		//7
	GPOWER_TYPE_EXP_DOWN,		//8
	GPOWER_TYPE_MONEY_UP,		//9
	GPOWER_TYPE_CAPTURE_UP,		//10
	GPOWER_TYPE_HAPPEN,		//11
	GPOWER_TYPE_DISTRIBUTION,		//12

	GPOWER_TYPE_MAX,
	GPOWER_TYPE_NULL = GPOWER_TYPE_MAX,
}GPOWER_TYPE;

