//==============================================================================
/**
 * @file	balloon.h
 * @brief	���D����̃w�b�_
 * @author	matsuda
 * @date	2007.11.06(��)
 */
//==============================================================================
#ifndef __BALLOON_H__
#define __BALLOON_H__

#include <procsys.h>
#include "net_app/wifi_lobby/minigame_tool.h"

//==============================================================================
//	�\���̒�`
//==============================================================================
//--------------------------------------------------------------
/**
 *	���D����Q�[���v���Z�X���[�N(parent_work)
 */
//--------------------------------------------------------------
typedef struct {
	WFLBY_MINIGAME_WK lobby_wk;
	SAVE_CONTROL_WORK* p_save;
	u8 vchat;
	u8 wifi_lobby;
	u8 pad[2];
	
#ifdef PM_DEBUG
	int debug_offline;		///<TRUE:�f�o�b�O�p�̃I�t���C�����[�h
#endif
}BALLOON_PROC_WORK;


//==============================================================================
//	�O���֐��錾
//==============================================================================
extern GFL_PROC_RESULT BalloonProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
extern GFL_PROC_RESULT BalloonProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
extern GFL_PROC_RESULT BalloonProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk );


#endif		// __BALLOON_H__

