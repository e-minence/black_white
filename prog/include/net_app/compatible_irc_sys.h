//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		compatible_irc_sys.h
 *	@brief	�����f�f�Q�[���p�ԊO���ڑ��V�X�e��
 *	@author	Toru=Nagihashi
 *	@date		2009.05.13
 *
 *	�ԊO���ڑ��V�X�e��
 *	�O�D��{
 *	�ڑ�����ؒf�܂Ŋ�{�I�ɂ����Ɛڑ����A�f�[�^�𑗂葱���܂��i��O����j
 *
 *
 *	�P�D��ʃ`�F�b�N
 *	���Ȃ炸�A�����𑗐M����Ƃ��́A���A�������ǂ̏�ʂɂ��邩���ꏏ�ɑ���܂��B
 *	�����A���������s���A�ʁX�̏ꏊ�ŐԊO����ʐM�����ꍇ�A�ǂ̏�ʂɂ��邩�Ƃ�����񂩂�A
 *	���̏�ʂɂ���
 *
 *
 *	�Q�D�ؒf��
 *	�ؒf���̂݁A�^�C���A�E�g��݂��܂��B
 *	�ؒf���̓��������s�����Ƃ��́A�^�C���A�E�g�ŏI�����܂��B
 *
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include "net\network_define.h"
#include "gamesystem/gamesystem.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
#define COMPATIBLE_IRC_CONNEXT_NUM		(2)     // �ő�ڑ��l��
#define COMPATIBLE_IRC_SENDDATA_BYTE  (32)    // �ő呗�M�o�C�g��
#define COMPATIBLE_IRC_GETBEACON_NUM	(16)    // �ő�r�[�R�����W��

#define COMPATIBLE_IRC_SENDATA_CNT		(60)

#define TITLE_STR_SCALE_X	(FX32_CONST(0.73))
#define TITLE_STR_SCALE_Y	(FX32_CONST(0.73))

//-------------------------------------
///		�^�C�~���O
//=====================================
typedef enum
{	
	COMPATIBLE_TIMING_NO_MENU_START,
	COMPATIBLE_TIMING_NO_MENU_END,
	COMPATIBLE_TIMING_NO_RHYTHM_RESULT,
	COMPATIBLE_TIMING_NO_RHYTHM_END,
	COMPATIBLE_TIMING_NO_AURA_RESULT,
	COMPATIBLE_TIMING_NO_AURA_END,

	COMPATIBLE_TIMING_NO_SCENE_ERROR,

	COMPATIBLE_TIMING_NO_MAX,
}COMPATIBLE_TIMING_NO;


//-------------------------------------
///		���݂̐ԊO���ꏊ
//=====================================
typedef enum {
	COMPATIBLE_SCENE_MENU,
	COMPATIBLE_SCENE_RHYTHM,
	COMPATIBLE_SCENE_AURA,
	COMPATIBLE_SCENE_RESULT,
} COMPATIBLE_SCENE;


//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
typedef struct _COMPATIBLE_IRC_SYS COMPATIBLE_IRC_SYS;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//IRC_COMPATIBLE�̂ݎg�p
extern COMPATIBLE_IRC_SYS * COMPATIBLE_IRC_CreateSystem( u32 irc_timeout, HEAPID heapID, BOOL is_only_play );
extern void COMPATIBLE_IRC_DeleteSystem( COMPATIBLE_IRC_SYS *p_sys );

extern BOOL COMPATIBLE_IRC_InitWait( COMPATIBLE_IRC_SYS *p_sys );
extern BOOL COMPATIBLE_IRC_ExitWait( COMPATIBLE_IRC_SYS *p_sys );
extern BOOL COMPATIBLE_IRC_ConnextWait( COMPATIBLE_IRC_SYS *p_sys );
extern BOOL COMPATIBLE_IRC_DisConnextWait( COMPATIBLE_IRC_SYS *p_sys );

extern BOOL COMPATIBLE_IRC_IsConnext( const COMPATIBLE_IRC_SYS *cp_sys );
extern BOOL COMPATIBLE_IRC_IsInit( const COMPATIBLE_IRC_SYS *cp_sys );
extern BOOL COMPATIBLE_IRC_TimingSyncWait( COMPATIBLE_IRC_SYS *p_sys, COMPATIBLE_TIMING_NO timing_no );

extern void COMPATIBLE_IRC_Cancel( COMPATIBLE_IRC_SYS *p_sys );

extern BOOL COMPATIBLE_IRC_IsError( COMPATIBLE_IRC_SYS *p_sys );

extern void COMPATIBLE_IRC_SetScene( COMPATIBLE_IRC_SYS *p_sys, COMPATIBLE_SCENE scene );
extern void COMPATIBLE_IRC_ResetScene( COMPATIBLE_IRC_SYS *p_sys );
extern BOOL COMPATIBLE_IRC_SendSceneContinue( COMPATIBLE_IRC_SYS *p_sys );
extern BOOL COMPATIBLE_IRC_SendScene( COMPATIBLE_IRC_SYS *p_sys );
extern int COMPATIBLE_IRC_CompScene( const COMPATIBLE_IRC_SYS *cp_sys );

//�e�ԊO���~�j�Q�[���Ŏg�p
extern BOOL COMPATIBLE_IRC_SendData( COMPATIBLE_IRC_SYS *p_sys, u16 send_command, u16 size, const void *cp_data );
extern BOOL COMPATIBLE_IRC_SendDataEx( COMPATIBLE_IRC_SYS *p_sys, u16 send_command, u16 size, const void *cp_data, const NetID sendID, const BOOL b_fast, const BOOL b_repeat, const BOOL b_send_buff_lock );
extern void COMPATIBLE_IRC_AddCommandTable( COMPATIBLE_IRC_SYS *p_sys, int cmdkindID, const NetRecvFuncTable *cp_net_recv_tbl, int tbl_max, void *p_param );
extern void COMPATIBLE_IRC_DelCommandTable( COMPATIBLE_IRC_SYS *p_sys, int cmdkindID );

//���j���[�Ŏg�p
extern BOOL COMPATIBLE_MENU_SendMenuData( COMPATIBLE_IRC_SYS *p_sys, u32 ms, u32 select );
extern BOOL COMPATIBLE_MENU_RecvMenuData( COMPATIBLE_IRC_SYS *p_sys );
extern void COMPATIBLE_MENU_GetMenuData( const COMPATIBLE_IRC_SYS *cp_sys, u32 *p_ms, u32 *p_select );
extern BOOL COMPATIBLE_MENU_SendReturnMenu( COMPATIBLE_IRC_SYS *p_sys );
extern BOOL COMPATIBLE_MENU_RecvReturnMenu( COMPATIBLE_IRC_SYS *p_sys );
extern BOOL COMPATIBLE_MENU_SendStatusData( COMPATIBLE_IRC_SYS *p_sys, GAMESYS_WORK *p_gamesys );
extern void COMPATIBLE_MENU_GetStatusData( const COMPATIBLE_IRC_SYS *cp_sys, MYSTATUS *p_status );
