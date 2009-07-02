//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		bucket.h
 *	@brief		�o�P�b�g�Q�[��
 *	@author		tomoya takahashi
 *	@data		2007.06.19
 *															
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __BUCKET_H__
#define __BUCKET_H__

#include "savedata/save_control.h"

#include <procsys.h>

#include "net_app/wifi_lobby/minigame_tool.h"


//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�o�P�b�g�Q�[���v���Z�X���[�N
//=====================================
typedef struct {
	WFLBY_MINIGAME_WK	lobby_wk;		//  �����ɃK�W�F�b�g�̏�Ԃ������Ă��܂��B�Q�[����͂��̒l����{���ɐݒ肵�܂��B
	SAVE_CONTROL_WORK*		p_save;
	u8 vchat;
	u8 wifi_lobby;
	u8 pad[2];
} BUCKET_PROC_WORK;


//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

extern GFL_PROC_RESULT BucketProc_Init( GFL_PROC * p_proc, int * p_seq, void * pwk, void * mywk );
extern GFL_PROC_RESULT BucketProc_Main( GFL_PROC * p_proc, int * p_seq, void * pwk, void * mywk );
extern GFL_PROC_RESULT BucketProc_End( GFL_PROC * p_proc, int * p_seq, void * pwk, void * mywk );


#endif		// __BUCKET_H__

