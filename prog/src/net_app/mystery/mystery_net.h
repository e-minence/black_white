//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		mystery_net.h
 *	@brief  �ӂ����Ȃ�������̒ʐM����
 *	@author	Toru=Nagihashi
 *	@data		2009.12.15
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include "gamesystem/game_comm.h"
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�ʐM�V�X�e���ւ̏�ԃ��N�G�X�g
//=====================================
typedef enum
{
  MYSTERY_NET_STATE_WAIT, //�ҋ@��

  //�r�[�R��
  MYSTERY_NET_STATE_START_BEACON,   //START�I�����AMAIN�֏���ɏ�ԑJ��
  MYSTERY_NET_STATE_MAIN_BEACON,
  MYSTERY_NET_STATE_END_BEACON,

  //WiFi�_�E�����[�h
  MYSTERY_NET_STATE_WIFI_DOWNLOAD,          //
  MYSTERY_NET_STATE_CANCEL_WIFI_DOWNLOAD,  //�_�E�����[�h�̃L�����Z��

  //WIFI���O�A�E�g  ( ���O�C���͊O���ōs�����߁@)
  MYSTERY_NET_STATE_LOGOUT_WIFI,

} MYSTERY_NET_STATE;

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�ӂ����Ȃ�������̒ʐM���[�N
//=====================================
typedef struct _MYSTERY_NET_WORK  MYSTERY_NET_WORK;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//�������B�j���A���C������
extern MYSTERY_NET_WORK * MYSTERY_NET_Init( const SAVE_CONTROL_WORK *cp_sv, HEAPID heapID );
extern void MYSTERY_NET_Exit( MYSTERY_NET_WORK *p_wk );
extern void MYSTERY_NET_Main( MYSTERY_NET_WORK *p_wk );

//��ԕψ�.��Ԏ擾
extern void MYSTERY_NET_ChangeStateReq( MYSTERY_NET_WORK *p_wk, MYSTERY_NET_STATE state );
extern MYSTERY_NET_STATE MYSTERY_NET_GetState( const MYSTERY_NET_WORK *cp_wk );

//���݂̒ʐM�����m
extern GAME_COMM_STATUS_BIT MYSTERY_NET_GetCommStatus( const MYSTERY_NET_WORK *cp_wk );

//�_�E�����[�h�����f�[�^�����Ƃ�
extern BOOL MYSTERY_NET_GetDownloadData( const MYSTERY_NET_WORK *cp_wk, void *p_data, u32 size );
