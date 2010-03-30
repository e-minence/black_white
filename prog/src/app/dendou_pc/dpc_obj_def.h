//============================================================================================
/**
 * @file		dpc_obj.h
 * @brief		�a������m�F��� �n�a�i�֘A
 * @author	Hiroyuki Nakamura
 * @date		10.03.29
 */
//============================================================================================
#pragma	once


//============================================================================================
//	�萔��`
//============================================================================================

// OBJ ID
enum {
	DPCOBJ_ID_POKE01 = 0,		// �|�P�����P
	DPCOBJ_ID_POKE02,
	DPCOBJ_ID_POKE03,
	DPCOBJ_ID_POKE04,
	DPCOBJ_ID_POKE05,
	DPCOBJ_ID_POKE06,

	DPCOBJ_ID_POKE11,				// �|�P�����P�i�X���b�v�p�j
	DPCOBJ_ID_POKE12,
	DPCOBJ_ID_POKE13,
	DPCOBJ_ID_POKE14,
	DPCOBJ_ID_POKE15,
	DPCOBJ_ID_POKE16,

	DPCOBJ_ID_ARROW_L,			// �y�[�W�؂�ւ������
	DPCOBJ_ID_ARROW_R,			// �y�[�W�؂�ւ��E���
	DPCOBJ_ID_EXIT,					// �I���{�^��
	DPCOBJ_ID_RETURN,				// �߂�{�^��

	DPCOBJ_ID_MAX
};

// �L�������\�[�X
enum {
	DPCOBJ_CHRRES_POKE01 = 0,		// �|�P�����P
	DPCOBJ_CHRRES_POKE02,
	DPCOBJ_CHRRES_POKE03,
	DPCOBJ_CHRRES_POKE04,
	DPCOBJ_CHRRES_POKE05,
	DPCOBJ_CHRRES_POKE06,

	DPCOBJ_CHRRES_POKE11,				// �|�P�����P�i�X���b�v�p�j
	DPCOBJ_CHRRES_POKE12,
	DPCOBJ_CHRRES_POKE13,
	DPCOBJ_CHRRES_POKE14,
	DPCOBJ_CHRRES_POKE15,
	DPCOBJ_CHRRES_POKE16,

	DPCOBJ_CHRRES_TB,						// �^�b�`�o�[

	DPCOBJ_CHRRES_MAX
};

// �p���b�g���\�[�X
enum {
	DPCOBJ_PALRES_POKE01 = 0,		// �|�P�����P
	DPCOBJ_PALRES_POKE02,
	DPCOBJ_PALRES_POKE03,
	DPCOBJ_PALRES_POKE04,
	DPCOBJ_PALRES_POKE05,
	DPCOBJ_PALRES_POKE06,

	DPCOBJ_PALRES_POKE11,				// �|�P�����P�i�X���b�v�p�j
	DPCOBJ_PALRES_POKE12,
	DPCOBJ_PALRES_POKE13,
	DPCOBJ_PALRES_POKE14,
	DPCOBJ_PALRES_POKE15,
	DPCOBJ_PALRES_POKE16,

	DPCOBJ_PALRES_TB,						// �^�b�`�o�[

	DPCOBJ_PALRES_MAX
};

// �Z�����\�[�X
enum {
	DPCOBJ_CELRES_POKE01 = 0,		// �|�P�����P
	DPCOBJ_CELRES_POKE02,
	DPCOBJ_CELRES_POKE03,
	DPCOBJ_CELRES_POKE04,
	DPCOBJ_CELRES_POKE05,
	DPCOBJ_CELRES_POKE06,

	DPCOBJ_CELRES_POKE11,				// �|�P�����P�i�X���b�v�p�j
	DPCOBJ_CELRES_POKE12,
	DPCOBJ_CELRES_POKE13,
	DPCOBJ_CELRES_POKE14,
	DPCOBJ_CELRES_POKE15,
	DPCOBJ_CELRES_POKE16,

	DPCOBJ_CELRES_TB,						// �^�b�`�o�[

	DPCOBJ_CELRES_MAX
};
