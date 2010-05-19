//============================================================================================
/**
 * @file		command_demo.h
 * @brief		�R�}���h�f����� �Ăяo��
 * @author	Hiroyuki Nakamura
 * @date		09.01.21
 */
//============================================================================================
#pragma	once


//============================================================================================
//	�萔��`
//============================================================================================

// �������[�h
enum {
	COMMANDDEMO_MODE_GF_LOGO = 0,		// �Q�[���t���[�N���S
	COMMANDDEMO_MODE_OP_MOVIE1,			// �I�[�v�j���O���[�r�[�P
	COMMANDDEMO_MODE_OP_MOVIE2,			// �I�[�v�j���O���[�r�[�Q
};

// �X�L�b�v�ݒ�
enum {
	COMMANDDEMO_SKIP_OFF = 0,			// �X�L�b�v�Ȃ�
	COMMANDDEMO_SKIP_ON,					// �X�L�b�v����
	COMMANDDEMO_SKIP_DEBUG,				// �X�L�b�v����i�f�o�b�O�p�j
};

// �I�����[�h
enum {
	COMMANDDEMO_RET_NORMAL = 0,		// �ʏ�
	COMMANDDEMO_RET_SKIP,					// �X�L�b�v����
	COMMANDDEMO_RET_SKIP_DEBUG,		// �X�L�b�v�����i�f�o�b�O�p�j
};

// �O���ݒ�f�[�^
typedef struct {
	// [ in ]
	u16	mode;		// �������[�h
	u8	skip;		// �X�L�b�v�ݒ�
	// [ out ]
	u8	ret;		// �I�����[�h
}COMMANDDEMO_DATA;


//============================================================================================
//	�O���[�o��
//============================================================================================

// �f��PROC�f�[�^
extern const GFL_PROC_DATA COMMANDDEMO_ProcData;

FS_EXTERN_OVERLAY(command_demo);
