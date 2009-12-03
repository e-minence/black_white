//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifibattlematch_net.h
 *	@brief  WIFI�o�g���p�l�b�g���W���[��
 *	@author	Toru=Nagihashi
 *	@date		2009.11.25
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
#include "sc/dwc_sc.h"
#include "gdb/dwc_gdb.h"
#include "battle/battle.h"
#include "wifibattlematch_data.h"
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�}�b�`�V�[�P���X
//=====================================
typedef enum
{
  WIFIBATTLEMATCH_NET_SEQ_MATCH_IDLE,
  WIFIBATTLEMATCH_NET_SEQ_MATCH_START,
  WIFIBATTLEMATCH_NET_SEQ_MATCH_WAIT,

  WIFIBATTLEMATCH_NET_SEQ_CONNECT_START,
  WIFIBATTLEMATCH_NET_SEQ_CONNECT_CHILD,
  WIFIBATTLEMATCH_NET_SEQ_CONNECT_PARENT,
  WIFIBATTLEMATCH_NET_SEQ_TIMING_END,

  WIFIBATTLEMATCH_NET_SEQ_MATCH_END,
} WIFIBATTLEMATCH_NET_SEQ;
//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================

//-------------------------------------
///	SAKE����̃f�[�^�󂯎��
//=====================================
typedef struct
{ 
  BOOL is_get;
  u16 single_win;
  u16 single_lose;
  u16 double_win;
  u16 double_lose;
  u16 rot_win;
  u16 rot_lose;
  u16 triple_win;
  u16 triple_lose;
  u16 shooter_win;
  u16 shooter_lose;
  u16 single_rate;
  u16 double_rate;
  u16 rot_rate;
  u16 triple_rate;
  u16 shooter_rate;
  u16 disconnect;
  u16 cheat;
} WIFIBATTLEMATCH_GDB_RND_SCORE_DATA;


//-------------------------------------
///	�l�b�g���W���[��
//=====================================
typedef struct _WIFIBATTLEMATCH_NET_WORK WIFIBATTLEMATCH_NET_WORK;


//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	���[�N�쐬
//=====================================
extern WIFIBATTLEMATCH_NET_WORK * WIFIBATTLEMATCH_NET_Init( HEAPID heapID );
extern void WIFIBATTLEMATCH_NET_Exit( WIFIBATTLEMATCH_NET_WORK *p_wk );
extern void WIFIBATTLEMATCH_NET_Main( WIFIBATTLEMATCH_NET_WORK *p_wk );

//-------------------------------------
///	�}�b�`���O
//=====================================
//�G���[
extern BOOL WIFIBATTLEMATCH_NET_IsError( const WIFIBATTLEMATCH_NET_WORK *cp_wk );

//�}�b�`���O
extern void WIFIBATTLEMATCH_NET_StartMatchMake( WIFIBATTLEMATCH_NET_WORK *p_wk );
extern BOOL WIFIBATTLEMATCH_NET_WaitMatchMake( WIFIBATTLEMATCH_NET_WORK *p_wk );
extern WIFIBATTLEMATCH_NET_SEQ WIFIBATTLEMATCH_NET_GetSeqMatchMake( const WIFIBATTLEMATCH_NET_WORK *cp_wk );

//�ؒf
extern BOOL WIFIBATTLEMATCH_NET_SetDisConnect( WIFIBATTLEMATCH_NET_WORK *p_wk, BOOL is_return );

//�L�����Z��
extern BOOL WIFIBATTLEMATCH_NET_SetCancelState( const WIFIBATTLEMATCH_NET_WORK *cp_wk );
extern BOOL WIFIBATTLEMATCH_NET_CancelDisable( const WIFIBATTLEMATCH_NET_WORK *cp_wk );

//-------------------------------------
///	���v�E�����֌W�iSC�j
//=====================================
extern void WIFIBATTLEMATCH_SC_Start( WIFIBATTLEMATCH_NET_WORK *p_wk, BtlRule rule, BtlResult result );
extern BOOL WIFIBATTLEMATCH_SC_Process( WIFIBATTLEMATCH_NET_WORK *p_wk, const DWCUserData *cp_user_data, DWCScResult *p_result );

//-------------------------------------
///	�f�[�^�x�[�X�֌W�iGDB)
//=====================================
typedef enum
{ 
  WIFIBATTLEMATCH_GDB_GET_RND_SCORE,
}WIFIBATTLEMATCH_GDB_GETTYPE;
//WIFIBATTLEMATCH_GDB_GET_RND_SCORE
extern void WIFIBATTLEMATCH_GDB_Start( WIFIBATTLEMATCH_NET_WORK *p_wk, WIFIBATTLEMATCH_GDB_GETTYPE type, void *p_wk_adrs );
extern BOOL WIFIBATTLEMATCH_GDB_Process( WIFIBATTLEMATCH_NET_WORK *p_wk, const DWCUserData *cp_user_data, DWCGdbError *p_result );

//-------------------------------------
///	����̃f�[�^��M
//=====================================
extern BOOL WIFIBATTLEMATCH_NET_StartEnemyData( WIFIBATTLEMATCH_NET_WORK *p_wk, const void *cp_buff );
extern BOOL WIFIBATTLEMATCH_NET_WaitEnemyData( WIFIBATTLEMATCH_NET_WORK *p_wk, WIFIBATTLEMATCH_ENEMYDATA **pp_data );
