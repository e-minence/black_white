//============================================================================================
/**
 * @file		command_demo.h
 * @brief		�R�}���h�f����� �Ăяo��
 * @author	Hiroyuki Nakamura
 * @date		09.01.21
 */
//============================================================================================
#pragma	once

/*
#include "savedata/savedata_def.h"
#include "system/procsys.h"
#include "../src/field/field_glb_state.h"
*/


//============================================================================================
//	�萔��`
//============================================================================================

// �������[�h
enum {
	COMMANDDEMO_MODE_TEST = 0,		// �e�X�g���[�r�[
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

// �`���f���O���ݒ�f�[�^
typedef struct {
/*
	SAVEDATA * savedata;
	GLST_DATA	glst;
	u16	time;		// ���ԑсi�z�E�I�E�̂݁j
#ifdef PM_DEBUG
	u16	scene;		// �V�[���ԍ� 0-1�i�z�E�I�E�̂݁j�@���A���C�x���g�Ȃ̂ŁA����Ȃ�����
	u16	debug;
#endif	// PM_DEBUG;
*/

	// [ in ]
	u16	mode;		// �������[�h
	u8	skip;		// �X�L�b�v�ݒ�
	// [ out ]
	u8	ret;		// �I�����[�h

}COMMANDDEMO_DATA;

/*
#ifdef PM_DEBUG
typedef struct {
	LEGEND_DEMO_DATA * dat;
	u16	mode;
	u8	time;
	u8	scene;
	u16	play_mode;
	u16	seq;
}D_LD_WORK;
#endif	// PM_DEBUG
*/


//============================================================================================
//	�O���[�o��
//============================================================================================

// �f��PROC�f�[�^
extern const GFL_PROC_DATA COMMANDDEMO_ProcData;

FS_EXTERN_OVERLAY(command_demo);
