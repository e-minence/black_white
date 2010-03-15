//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		livebattlematch_flow.h
 *	@brief  ���C�u���ԊO���ʐM���W���[��
 *	@author	Toru=Nagihashi
 *	@data		2010.03.07
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
#include "gamesystem/game_data.h"
#include "wifibattlematch_data.h"
#include "poke_tool/pokeparty.h"
#include "savedata/regulation.h"
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
#define LIVEBATTLEMATCH_IRC_CONNECT_NUM (2)

//-------------------------------------
///	�G���[����
//=====================================
typedef enum
{
  LIVEBATTLEMATCH_IRC_SYSERROR_NONE,  //�G���[�͔������Ă��Ȃ�
} LIVEBATTLEMATCH_IRC_SYSERROR;

//-------------------------------------
///	�G���[�����^�C�v
//=====================================
typedef enum
{
  LIVEBATTLEMATCH_IRC_ERROR_REPAIR_NONE,  //����
  LIVEBATTLEMATCH_IRC_ERROR_REPAIR_DISCONNECT,  //�ؒf
} LIVEBATTLEMATCH_IRC_ERROR_REPAIR_TYPE;

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�ԊO�����W���[��
//=====================================
typedef struct _LIVEBATTLEMATCH_IRC_WORK LIVEBATTLEMATCH_IRC_WORK;

//=============================================================================
/**
 *					�O�����J
*/
//=============================================================================
//-------------------------------------
///	���[�N�쐬�E�j���E����
//=====================================
extern LIVEBATTLEMATCH_IRC_WORK *LIVEBATTLEMATCH_IRC_Init( GAMEDATA *p_gamedata, HEAPID heapID );
extern void LIVEBATTLEMATCH_IRC_Exit( LIVEBATTLEMATCH_IRC_WORK *p_wk );
extern void LIVEBATTLEMATCH_IRC_Main( LIVEBATTLEMATCH_IRC_WORK *p_wk );

//-------------------------------------
///	�G���[
//=====================================
extern LIVEBATTLEMATCH_IRC_ERROR_REPAIR_TYPE LIVEBATTLEMATCH_IRC_CheckErrorRepairType( LIVEBATTLEMATCH_IRC_WORK *p_wk );

//-------------------------------------
///	�ڑ��E�ؒf
//=====================================
//�q����
extern BOOL LIVEBATTLEMATCH_IRC_StartConnect( LIVEBATTLEMATCH_IRC_WORK *p_wk );
extern BOOL LIVEBATTLEMATCH_IRC_WaitConnect( LIVEBATTLEMATCH_IRC_WORK *p_wk );
//�ڑ��L�����Z��
extern void LIVEBATTLEMATCH_IRC_StartCancelConnect( LIVEBATTLEMATCH_IRC_WORK *p_wk );
extern BOOL LIVEBATTLEMATCH_IRC_WaitCancelConnect( LIVEBATTLEMATCH_IRC_WORK *p_wk );
//�ؒf����
extern BOOL LIVEBATTLEMATCH_IRC_StartDisConnect( LIVEBATTLEMATCH_IRC_WORK *p_wk );
extern BOOL LIVEBATTLEMATCH_IRC_WaitDisConnect( LIVEBATTLEMATCH_IRC_WORK *p_wk );


//-------------------------------------
///	����̃f�[�^��M  ���݂��̃f�[�^�𑗂�  SENDDATA�n
//=====================================
//���݂��̏��𑗂肠��
extern BOOL LIVEBATTLEMATCH_IRC_StartEnemyData( LIVEBATTLEMATCH_IRC_WORK *p_wk, const void *cp_buff );
//�Ȃ�������n�����Ƃ����ƁAWIFIBATTLEMATCH_ENEMYDATA�̓t���L�V�u���ȍ\���̂Ȃ̂Ń|�C���^���󂯎��A�O��MemCopy���Ă��炤���߂ł��B
extern BOOL LIVEBATTLEMATCH_IRC_WaitEnemyData( LIVEBATTLEMATCH_IRC_WORK *p_wk, WIFIBATTLEMATCH_ENEMYDATA **pp_data );

//�|�P�p�[�e�C�𑗂肠��
extern BOOL LIVEBATTLEMATCH_IRC_SendPokeParty( LIVEBATTLEMATCH_IRC_WORK *p_wk, const POKEPARTY *cp_party );
extern BOOL LIVEBATTLEMATCH_IRC_RecvPokeParty( LIVEBATTLEMATCH_IRC_WORK *p_wk, POKEPARTY *p_party );


//-------------------------------------
///	���M�����[�V�����z�M
//=====================================
//���M�����[�V�������󂯎��
extern void LIVEBATTLEMATCH_IRC_StartRecvRegulation( LIVEBATTLEMATCH_IRC_WORK *p_wk, REGULATION_CARDDATA *p_recv );

typedef enum
{ 
  LIVEBATTLEMATCH_IRC_RET_UPDATE,  //������
  LIVEBATTLEMATCH_IRC_RET_SUCCESS,  //����Ɏ�M
  LIVEBATTLEMATCH_IRC_RET_FAILED,   //�s���ȃf�[�^���󂯎����
  LIVEBATTLEMATCH_IRC_RET_NOT_LAG,  //�����̌���̃��M�����[�V�������Ȃ�����
}LIVEBATTLEMATCH_IRC_RET;
extern LIVEBATTLEMATCH_IRC_RET LIVEBATTLEMATCH_IRC_WaitRecvRegulation( LIVEBATTLEMATCH_IRC_WORK *p_wk );

extern void LIVEBATTLEMATCH_IRC_StartCancelRecvRegulation( LIVEBATTLEMATCH_IRC_WORK *p_wk );
extern BOOL LIVEBATTLEMATCH_IRC_WaitCancelRecvRegulation( LIVEBATTLEMATCH_IRC_WORK *p_wk );
