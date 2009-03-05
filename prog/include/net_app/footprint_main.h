//==============================================================================
/**
 * @file	footprint_main.h
 * @brief	���Ճ{�[�h�̃��C���w�b�_
 * @author	matsuda
 * @date	2008.01.19(�y)
 */
//==============================================================================
#ifndef __FOOTPRINT_MAIN_H__
#define __FOOTPRINT_MAIN_H__

#include "net_app/wifi_lobby/wflby_system_def.h"

//==============================================================================
//	�萔��`
//==============================================================================
///�{�[�h�̎��
enum{
	FOOTPRINT_BOARD_TYPE_WHITE,		///<���{�[�h
	FOOTPRINT_BOARD_TYPE_BLACK,		///<���{�[�h

	FOOTPRINT_BOARD_TYPE_MAX,
};


//==============================================================================
//	�\���̒�`
//==============================================================================
typedef struct{
	WFLBY_SYSTEM *wflby_sys;		///<WIFI���r�[�V�X�e���ւ̃|�C���^
	u8 board_type;					///<�{�[�h�̎��(FOOTPRINT_BOARD_TYPE_???)
	
	u8 padding[3];
	
#ifdef PM_DEBUG
	SAVE_CONTROL_WORK *debug_sv;
#endif
}FOOTPRINT_PARAM;


//==============================================================================
//	�O���֐��錾
//==============================================================================
extern GFL_PROC_RESULT FootPrintProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
extern GFL_PROC_RESULT FootPrintProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
extern GFL_PROC_RESULT FootPrintProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk );


#endif	//__FOOTPRINT_MAIN_H__

