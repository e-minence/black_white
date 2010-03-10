//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		src\test\nagihashi\debug_utilproc.c
 *	@brief
 *	@author		Toru=Nagihashi
 *	@data		2010.03.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//	���C�u����
#include <gflib.h>

//�V�X�e��
#include "system/main.h"
#include "gamesystem/game_data.h"

//�Z�[�u�f�[�^
#include "savedata/regulation.h"

//LIVE�}�b�`�̃��W���[��
#include "net/delivery_irc.h"

//�O�����J
#include "debug_utilproc.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	���C�����[�N
//=====================================
typedef struct
{
  DELIVERY_IRC_WORK         *p_delivery;
  GAMEDATA                  *p_gamedata;
  REGULATION_CARDDATA       carddata;
} DEBUG_NAGI_IRC_REGULATION_WORK;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	�v���Z�X
//=====================================
static GFL_PROC_RESULT DEBUG_NAGI_IRC_REGULATION_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT DEBUG_NAGI_IRC_REGULATION_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT DEBUG_NAGI_IRC_REGULATION_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );

//=============================================================================
/**
 *					�f�[�^
*/
//=============================================================================
//-------------------------------------
///	�ԊO�����M�����[�V�������M�v���Z�X
//=====================================
const GFL_PROC_DATA	DEBUG_NAGI_IRC_REGULATION_ProcData =
{ 
  DEBUG_NAGI_IRC_REGULATION_PROC_Init,
  DEBUG_NAGI_IRC_REGULATION_PROC_Main,
  DEBUG_NAGI_IRC_REGULATION_PROC_Exit
};
//=============================================================================
/**
 *					PROC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�ԊO�����M�����[�V�����z�M�ł΂���  ������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT DEBUG_NAGI_IRC_REGULATION_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
  DEBUG_NAGI_IRC_REGULATION_WORK  *p_wk;

  //�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_NAGI_DEBUG_SUB, 0x10000 );

  //�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(DEBUG_NAGI_IRC_REGULATION_WORK), HEAPID_NAGI_DEBUG_SUB );
	GFL_STD_MemClear( p_wk, sizeof(DEBUG_NAGI_IRC_REGULATION_WORK) );	
  p_wk->p_gamedata  = GAMEDATA_Create( HEAPID_NAGI_DEBUG_SUB );

  { 
    DELIVERY_IRC_INIT init;
    GFL_STD_MemClear( &init, sizeof(DELIVERY_IRC_INIT) );
    init.NetDevID = WB_NET_IRC_BATTLE;
    init.datasize = sizeof(REGULATION_CARDDATA);
    init.pData  = (u8*)&p_wk->carddata;
    init.ConfusionID  = 0;
    init.heapID = HEAPID_NAGI_DEBUG_SUB;
    p_wk->p_delivery  = DELIVERY_IRC_Init(&init);
  }

  Regulation_SetDebugData( &p_wk->carddata );

	return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�ԊO�����M�����[�V�����z�M�ł΂���  �j��
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT DEBUG_NAGI_IRC_REGULATION_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{
  DEBUG_NAGI_IRC_REGULATION_WORK  *p_wk     = p_wk_adrs;

  GAMEDATA_Delete( p_wk->p_gamedata );

  //�v���Z�X���[�N�j��
	GFL_PROC_FreeWork( p_proc );

  //�q�[�v�j��
	GFL_HEAP_DeleteHeap( HEAPID_NAGI_DEBUG_SUB );

  return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�ԊO�����M�����[�V�����z�M�ł΂���  ���C������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT DEBUG_NAGI_IRC_REGULATION_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{
  enum
  { 
    SEQ_START_RECVCARD,
    SEQ_WAIT_RECVCARD,
    SEQ_DELETE,
    SEQ_END,
  };

  DEBUG_NAGI_IRC_REGULATION_WORK  *p_wk     = p_wk_adrs;

  DELIVERY_IRC_Main( p_wk->p_delivery );

  switch( *p_seq )
  { 
  case SEQ_START_RECVCARD:
    if( DELIVERY_IRC_SendStart( p_wk->p_delivery) )
    { 
      *p_seq  = SEQ_WAIT_RECVCARD;
    }
    break;
  case SEQ_WAIT_RECVCARD:
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B )
    { 
      *p_seq  = SEQ_DELETE;
    }
    break;
  case SEQ_DELETE:
    DELIVERY_IRC_End( p_wk->p_delivery );
    *p_seq  = SEQ_END;
    break;
  case SEQ_END:

    if( GFL_NET_IsResetEnable() )
    {
      p_wk->p_delivery  = NULL;
      return GFL_PROC_RES_FINISH;
    }
  }



  return GFL_PROC_RES_CONTINUE;
}
