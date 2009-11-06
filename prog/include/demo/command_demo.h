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

	u32	mode;		// �������[�h

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
