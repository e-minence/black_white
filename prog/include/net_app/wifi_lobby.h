//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wifi_lobby.h
 *	@brief		WiFi���r�[���C���V�X�e��
 *	@author		tomoya takahashi
 *	@data		2007.09.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WIFI_LOBBY_H__
#define __WIFI_LOBBY_H__

#include <procsys.h>
#include "savedata/save_control.h"
#include "field/wflby_counter.h"

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
///	�v���b�N�p�����[�^
//=====================================
typedef struct {
	WFLBY_COUNTER_TIME* p_wflby_counter;	// WiFi�Ђ�Γ������Ԋi�[��
	SAVE_CONTROL_WORK*			p_save;				// �Z�[�u�f�[�^
	BOOL				check_skip;			// �ڑ��m�F���X�L�b�v����@TRUE�F�X�L�b�v	FALSE�F�ʏ�
} WFLBY_PROC_PARAM;


//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
extern GFL_PROC_RESULT WFLBYProc_Init( PROC* p_proc, int* p_seq );
extern GFL_PROC_RESULT WFLBYProc_Main( PROC* p_proc, int* p_seq );
extern GFL_PROC_RESULT WFLBYProc_Exit( PROC* p_proc, int* p_seq );



#endif		// __WIFI_LOBBY_H__

