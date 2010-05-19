//============================================================================================
/**
 * @file		cdemo_comm.h
 * @brief		�R�}���h�f����� �R�}���h����
 * @author	Hiroyuki Nakamura
 * @date		09.05.07
 */
//============================================================================================
#pragma	once


//============================================================================================
//	�萔��`
//============================================================================================

// �R�}���h
enum {
	CDEMOCOMM_WAIT = 0,						// 00: �E�F�C�g

	CDEMOCOMM_BLACK_IN,						// 01: �u���b�N�C��
	CDEMOCOMM_BLACK_OUT,					// 02: �u���b�N�A�E�g
	CDEMOCOMM_WHITE_IN,						// 03: �z���C�g�C��
	CDEMOCOMM_WHITE_OUT,					// 04: �z���C�g�A�E�g
	CDEMOCOMM_FADE_MAIN,					// 05: �t�F�[�h���C��

	CDEMOCOMM_BG_LOAD,						// 06: BG�ǂݍ���
	CDEMOCOMM_BG_VANISH,					// 07: BG�\���؂�ւ�
	CDEMOCOMM_BG_PRI_CHG,					// 08: BG�v���C�I���e�B�؂�ւ�
	CDEMOCOMM_ALPHA_INIT,					// 09: �A���t�@�u�����h�ݒ�
	CDEMOCOMM_ALPHA_START,				// 10: �A���t�@�u�����h�J�n
	CDEMOCOMM_ALPHA_NONE,					// 11: �A���t�@����

	CDEMOCOMM_BGM_SAVE,						// 12: �Đ�����BGM���L��
	CDEMOCOMM_BGM_STOP,						// 13: �Đ�����BGM���~
	CDEMOCOMM_BGM_PLAY,						// 14: BGM�Đ�

	CDEMOCOMM_OBJ_VANISH,					// 15: OBJ�\���؂�ւ�
	CDEMOCOMM_OBJ_PUT,						// 16: OBJ���W�ݒ�
	CDEMOCOMM_OBJ_BG_PRI_CHANGE,	// 17: OBJ��BG�̃v���C�I���e�B�ݒ�

	CDEMOCOMM_BG_FRM_ANIME,				// 18: BG�X�N���[���A�j��

	CDEMOCOMM_DEBUG_PRINT,				// 19: �f�o�b�O�v�����g

	CDEMOCOMM_END				// �I��
};

// �p�����[�^
enum {
	CDEMO_PRM_BLEND_PLANE_1 = 0,	// �A���t�@�u�����h�̑�P�Ώۖʎw��
	CDEMO_PRM_BLEND_PLANE_2,			// �A���t�@�u�����h�̑�Q�Ώۖʎw��

/*
	CDEMO_PRM_BGM_PLAY_SAVE = 0xffffffff,		// �L�������a�f�l���Đ�����

	CDEMO_PRM_OBJ_ARCEUS = 0,			// �A������A���Z�E�X�n�a�i
	CDEMO_PRM_OBJ_MAX,						// ���f�[�^����`�̂��ߎg�p�֎~

	CDEMO_OBJ_OFF = 0,						// OBJ��\��
	CDEMO_OBJ_ON,									// OBJ�\��
*/
};

// �R�}���h�߂�l
enum {
	CDEMOCOMM_RET_TRUE = 0,		// �R�}���h���s��
	CDEMOCOMM_RET_FALSE,			// �R�}���h�I��		( �����̃R�}���h���Đ����܂� )
	CDEMOCOMM_RET_NEXT,				// ���̃R�}���h��	( 1sync�҂��܂� )
};


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C������
 *
 * @param		wk		���[�N
 *
 * @return	�������e
 */
//--------------------------------------------------------------------------------------------
extern int CDEMOCOMM_Main( CDEMO_WORK * wk );
