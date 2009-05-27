//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		compatible_irc_sys.h
 *	@brief	�����f�f�Q�[���p�ԊO���ڑ��V�X�e��
 *	@author	Toru=Nagihashi
 *	@date		2009.05.13
 *
 *	�����f�f�Q�[���́A������ڑ����邪�A�ڑ��Ɛڑ��̊Ԃ����߂�
 *	�^�C���A�E�g���Ă��܂��̂ŁA����A�l�S�V�G�[�V���������蒼���B
 *	�������A���ꂾ�ƒN�Ƃ��q�����Ă��܂��̂ŁA
 *	�ŏ��ɂȂ����Ƃ������MacAddr��Ⴂ�A���̑���Ƃ̂ݒʐM����悤�ɂ���B
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include "net\network_define.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
#define COMPATIBLE_IRC_CONNEXT_NUM		(2)     // �ő�ڑ��l��
#define COMPATIBLE_IRC_SENDDATA_BYTE  (32)    // �ő呗�M�o�C�g��
#define COMPATIBLE_IRC_GETBEACON_NUM	(16)    // �ő�r�[�R�����W��

//-------------------------------------
///	�^�C�~���ONO�̏d���͋�����Ȃ����߁A
//	�K�v���邽�ё��₵�Ă����B
//	255�܂�
//	�������A���j���[����߂��Ă��čēx�^�C�~���O��
//	�s�����Ƃ��ɒʉ߂��Ă��܂����Ƃɒ���
//	�i�l�b�g�ڑ����̓����^�C�~���ONO�ł̓�����
//	��xOK�ɂȂ��Ă��܂��΁A���Ƃ͒ʉ߂��Ă��܂��j
//=====================================
typedef enum
{	
	COMPATIBLE_TIMING_NO_MENU_START,
}COMPATIBLE_TIMING_NO;

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
extern COMPATIBLE_IRC_SYS * COMPATIBLE_IRC_CreateSystem( u32 irc_timeout, HEAPID heapID );
extern void COMPATIBLE_IRC_DeleteSystem( COMPATIBLE_IRC_SYS *p_sys );

extern BOOL COMPATIBLE_IRC_InitWait( COMPATIBLE_IRC_SYS *p_sys );
extern BOOL COMPATIBLE_IRC_ExitWait( COMPATIBLE_IRC_SYS *p_sys );
extern BOOL COMPATIBLE_IRC_ConnextWait( COMPATIBLE_IRC_SYS *p_sys );

extern BOOL COMPATIBLE_IRC_DisConnextWait( COMPATIBLE_IRC_SYS *p_sys );
extern BOOL COMPATIBLE_IRC_TimingSyncWait( COMPATIBLE_IRC_SYS *p_sys, COMPATIBLE_TIMING_NO timing_no );

extern void COMPATIBLE_IRC_Cancel( COMPATIBLE_IRC_SYS *p_sys );

extern BOOL COMPATIBLE_IRC_IsError( COMPATIBLE_IRC_SYS *p_sys );


//�e�ԊO���~�j�Q�[���Ŏg�p
extern BOOL COMPATIBLE_IRC_SendData( COMPATIBLE_IRC_SYS *p_sys, u16 send_command, u16 size, const void *cp_data );
extern BOOL COMPATIBLE_IRC_SendDataEx( COMPATIBLE_IRC_SYS *p_sys, u16 send_command, u16 size, const void *cp_data, const NetID sendID, const BOOL b_fast, const BOOL b_repeat, const BOOL b_send_buff_lock );
extern void COMPATIBLE_IRC_AddCommandTable( COMPATIBLE_IRC_SYS *p_sys, int cmdkindID, const NetRecvFuncTable *cp_net_recv_tbl, int tbl_max, void *p_param );
extern void COMPATIBLE_IRC_DelCommandTable( COMPATIBLE_IRC_SYS *p_sys, int cmdkindID );
