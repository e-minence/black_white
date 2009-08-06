//==============================================================================
/**
 * @file	balance_ball.h
 * @brief	�ȒP�Ȑ���������
 * @author	goto
 * @date	2007.09.25(��)
 *
 * �����ɐF�X�ȉ�����������Ă��悢
 *
 */
//==============================================================================
#ifndef __BALANCE_BALL_H__
#define __BALANCE_BALL_H__

#include <procsys.h>
#include "savedata/save_control.h"
#include "net_app/wifi_lobby/minigame_tool.h"

//--------------------------------------------------------------
//	
//--------------------------------------------------------------
///�f�o�b�O�p��`
//WB�ڐA�ɂ������Ďb��I�ɖ��������镔�����͂�
#define WB_FIX			0	//0�Ŗ���������
//WB�ڐA�ɂ������Ĉꎞ�I�ɖ��������镔��(�ڐA��Ƃ���񂵂ɂ��鏊)
#define WB_TEMP_FIX		0	//0�Ŗ���������


typedef struct {
	WFLBY_MINIGAME_WK lobby_wk;

//	u8 voice_chat;
	SAVE_CONTROL_WORK*	p_save;
	BOOL		wifi_lobby;
	u32			vchat;			///< voice_chat
	
} BB_PROC_WORK;


extern GFL_PROC_RESULT BalanceBallProc_Init( GFL_PROC* proc, int* seq, void * pwk, void * mywk );
extern GFL_PROC_RESULT BalanceBallProc_Main( GFL_PROC* proc, int* seq, void * pwk, void * mywk );
extern GFL_PROC_RESULT BalanceBallProc_Exit( GFL_PROC* proc, int* seq, void * pwk, void * mywk );

#if (WB_FIX == 0)
extern void bb_gf_rand_init(void);
extern u32 bb_gf_get_seed(void);
extern void bb_gf_srand(u32 seed);
extern u16 bb_gf_rand(void);
#endif

#endif		// __BUCKET_H__

