//============================================================
//       power_conv.rb �ŏo�͂��ꂽ�t�@�C���ł�
//============================================================

#pragma once

typedef enum{
	GPOWER_ID_SOUGUU_INC_A,	//0 : �G���J�E���g����1.5�{�㏸����B
	GPOWER_ID_SOUGUU_INC_B,	//1 : �G���J�E���g����2�{�㏸����B
	GPOWER_ID_SOUGUU_INC_C,	//2 : �G���J�E���g����3�{�㏸����B
	GPOWER_ID_SOUGUU_DEC_A,	//3 : �G���J�E���g����0.66�{�ɂȂ�B
	GPOWER_ID_SOUGUU_DEC_B,	//4 : �G���J�E���g����0.5�{�ɂȂ�B
	GPOWER_ID_SOUGUU_DEC_C,	//5 : �G���J�E���g����0.33�{�ɂȂ�B
	GPOWER_ID_EGG_INC_A,	//6 : �^�}�S�̛z����1.25�{�ɂȂ�B�i��̓I�Ȑ��l�͌���Ȃ��j
	GPOWER_ID_EGG_INC_B,	//7 : �^�}�S�̛z����1.5�{�ɂȂ�B�i��̓I�Ȑ��l�͌���Ȃ��j
	GPOWER_ID_EGG_INC_C,	//8 : �^�}�S�̛z����2�{�ɂȂ�B�i��̓I�Ȑ��l�͌���Ȃ��j
	GPOWER_ID_NATSUKI_A,	//9 : �|�P�����̂Ȃ��������A��Ɂ{�P�����B�i��̓I�Ȑ��l�͌���Ȃ��j
	GPOWER_ID_NATSUKI_B,	//10 : �|�P�����̂Ȃ��������A��Ɂ{�Q�����B�i��̓I�Ȑ��l�͌���Ȃ��j
	GPOWER_ID_NATSUKI_C,	//11 : �|�P�����̂Ȃ��������A��Ɂ{�R�����B�i��̓I�Ȑ��l�͌���Ȃ��j
	GPOWER_ID_SALE_A,	//12 : �������̒l�i���P�O���I�t�ɂȂ�
	GPOWER_ID_SALE_B,	//13 : �������̒l�i���Q�T���I�t�ɂȂ�
	GPOWER_ID_SALE_C,	//14 : �������̒l�i���T�O���I�t�ɂȂ�
	GPOWER_ID_HP_RESTORE_A,	//15 : �Ă����擪�̃|�P������HP�������񕜂���i����������j
	GPOWER_ID_HP_RESTORE_B,	//16 : �Ă����擪�̃|�P������HP���񕜂���i��������������j
	GPOWER_ID_HP_RESTORE_C,	//17 : �Ă����擪�̃|�P������HP���S�ĉ񕜂���i����������������j
	GPOWER_ID_PP_RESTORE_A,	//18 : �Ă����擪�̃|�P�����̂S��PP���T�񕜂���
	GPOWER_ID_PP_RESTORE_B,	//19 : �Ă����擪�̃|�P�����̂S��PP���P�O�񕜂���
	GPOWER_ID_PP_RESTORE_C,	//20 : �Ă����擪�̃|�P�����̂S��PP���S���񕜂���
	GPOWER_ID_EXP_INC_A,	//21 : �o�g����̌o���l��1.2�{�ɂȂ�B�i��̓I�Ȑ��l�͌���Ȃ��j
	GPOWER_ID_EXP_INC_B,	//22 : �o�g����̌o���l��1.5�{�ɂȂ�B�i��̓I�Ȑ��l�͌���Ȃ��j
	GPOWER_ID_EXP_INC_C,	//23 : �o�g����̌o���l���Q�{�ɂȂ�B�i��̓I�Ȑ��l�͌���Ȃ��j
	GPOWER_ID_EXP_DEC_A,	//24 : �o�g����̌o���l��0.8�{�ɂȂ�B�i��̓I�Ȑ��l�͌���Ȃ��j
	GPOWER_ID_EXP_DEC_B,	//25 : �o�g����̌o���l��0.66�{�ɂȂ�B�i��̓I�Ȑ��l�͌���Ȃ��j
	GPOWER_ID_EXP_DEC_C,	//26 : �o�g����̌o���l��0.5�{�ɂȂ�B�i��̓I�Ȑ��l�͌���Ȃ��j
	GPOWER_ID_MONEY_A,	//27 : �o�g����̂����Â�����1.5�{�ɂȂ�B�i��̓I�Ȑ��l�͌���Ȃ��j
	GPOWER_ID_MONEY_B,	//28 : �o�g����̂����Â������Q�{�ɂȂ�B�i��̓I�Ȑ��l�͌���Ȃ��j
	GPOWER_ID_MONEY_C,	//29 : �o�g����̂����Â������R�{�ɂȂ�B�i��̓I�Ȑ��l�͌���Ȃ��j
	GPOWER_ID_CAPTURE_A,	//30 : �|�P�����̕ߊl�����~�P.�P�{�����B�i��̓I�Ȑ��l�͌���Ȃ��j
	GPOWER_ID_CAPTURE_B,	//31 : �|�P�����̕ߊl�����~�P.�Q�{�����B�i��̓I�Ȑ��l�͌���Ȃ��j
	GPOWER_ID_CAPTURE_C,	//32 : �|�P�����̕ߊl�����~�P.�R�{�����B�i��̓I�Ȑ��l�͌���Ȃ��j
	GPOWER_ID_EGG_INC_S,	//33 : �^�}�S�̛z����2�{�ɂȂ�B�i�z�z�F�p���X�p�Ɍ��ʎ��ԂP�O�{�j
	GPOWER_ID_SALE_S,	//34 : �������̒l�i���T�O���I�t�ɂȂ�i�z�z�F�p���X�p�Ɍ��ʎ��ԂP�O�{�j
	GPOWER_ID_NATSUKI_S,	//35 : �|�P�����̂Ȃ��������A��Ɂ{�R�����B�i�z�z�F�p���X�p�Ɍ��ʎ��ԂP�O�{�j
	GPOWER_ID_EXP_INC_S,	//36 : �o�g����̌o���l��2�{�ɂȂ�B�i��̓I�Ȑ��l�͌���Ȃ��j�i�z�z�F�p���X�p�Ɍ��ʎ��ԂP�O�{�j�j
	GPOWER_ID_MONEY_S,	//37 : �o�g����̂����Â������R�{�ɂȂ�B�i��̓I�Ȑ��l�͌���Ȃ��j�i�z�z�F�p���X�p�Ɍ��ʎ��ԂP�O�{�j
	GPOWER_ID_CAPTURE_S,	//38 : �|�P�����̕ߊl�����~�P�D�R�{�����B�i��̓I�Ȑ��l�͌���Ȃ��j�i�z�z�F�p���X�p�Ɍ��ʎ��ԂP�O�{�j
	GPOWER_ID_HP_RESTORE_MAX,	//39 : �Ă����S�|�P������HP�����S�񕜂���B�|�P�Z���Ɠ����i�p���X�p�j
	GPOWER_ID_EGG_INC_MAX,	//40 : �^�}�S�̛z����2�{�ɂȂ�B�i�p���X�p�Ɍ��ʎ��ԂP���ԁj
	GPOWER_ID_SALE_MAX,	//41 : �������̒l�i���T�O���I�t�ɂȂ�i�p���X�p�Ɍ��ʎ��ԂP���ԁj
	GPOWER_ID_NATSUKI_MAX,	//42 : �|�P�����̂Ȃ��������A��Ɂ{�R�����B�i�p���X�p�Ɍ��ʎ��ԂP���ԁj
	GPOWER_ID_EXP_INC_MAX,	//43 : �o�g����̌o���l��2�{�ɂȂ�B�i��̓I�Ȑ��l�͌���Ȃ��j�i�p���X�p�Ɍ��ʎ��ԂP���ԁj
	GPOWER_ID_MONEY_MAX,	//44 : �o�g����̂����Â������R�{�ɂȂ�B�i��̓I�Ȑ��l�͌���Ȃ��j�i�p���X�p�Ɍ��ʎ��ԂP���ԁj
	GPOWER_ID_CAPTURE_MAX,	//45 : �|�P�����̕ߊl�����~�P�D�R�{�����B�i��̓I�Ȑ��l�͌���Ȃ��j�i�p���X�p�Ɍ��ʎ��ԂP���ԁj
	GPOWER_ID_HAPPEN_MAX,	//46 : �_�~�[�f�[�^
	GPOWER_ID_DISTRIBUTION_MAX,	//47 : �_�~�[�f�[�^

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

