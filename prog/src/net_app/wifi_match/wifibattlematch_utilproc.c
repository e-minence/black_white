//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifibattlematch_utilproc.c
 *	@brief  wifibattlematch_sys�ŐF�X�ȏ����������Ȃ����߂ɁAPROC����������
 *	@author	Toru=Nagihashi
 *	@data		2010.02.16
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//	���C�u����
#include <gflib.h>

//	�V�X�e��
#include "system/main.h"
#include "gamesystem/game_data.h"
#include "net/dwc_error.h"

//WIFI�o�g���}�b�`�̃��W���[��
#include "wifibattlematch_net.h"

//�O�����J
#include "wifibattlematch_utilproc.h"

//-------------------------------------
///	�I�[�o�[���C
//=====================================
FS_EXTERN_OVERLAY(ui_common);
FS_EXTERN_OVERLAY(dpw_common);

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					���X�g��v���Z�X
 *					    �^�C�~���O���Ƃ���PokeParty�𑗂邾��
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	���X�g��v���Z�X���C�����[�N
//=====================================
typedef struct 
{
  //�l�b�g
  WIFIBATTLEMATCH_NET_WORK  *p_net;
} WIFIBATTLEMATCH_LISTAFTER_WORK;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	�v���Z�X
//=====================================
static GFL_PROC_RESULT WIFIBATTLEMATCH_LISTAFTER_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT WIFIBATTLEMATCH_LISTAFTER_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT WIFIBATTLEMATCH_LISTAFTER_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );

//=============================================================================
/**
 *					�O���Q��
*/
//=============================================================================
//-------------------------------------
///	PROC
//=====================================
const GFL_PROC_DATA	WifiBattleMatch_ListAfter_ProcData =
{	
	WIFIBATTLEMATCH_LISTAFTER_PROC_Init,
	WIFIBATTLEMATCH_LISTAFTER_PROC_Main,
	WIFIBATTLEMATCH_LISTAFTER_PROC_Exit,
};

//=============================================================================
/**
 *					PROC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	WIFI�o�g���}�b�`���	���X�g��v���Z�X������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WIFIBATTLEMATCH_LISTAFTER_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
  WIFIBATTLEMATCH_LISTAFTER_WORK  *p_wk;
  WIFIBATTLEMATCH_LISTAFTER_PARAM *p_param  = p_param_adrs;

  GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common));
	GFL_OVERLAY_Load( FS_OVERLAY_ID(dpw_common));

  //�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WIFIBATTLEMATCH_SUB, 0x10000 );

  //�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(WIFIBATTLEMATCH_LISTAFTER_WORK), HEAPID_WIFIBATTLEMATCH_SUB );
	GFL_STD_MemClear( p_wk, sizeof(WIFIBATTLEMATCH_LISTAFTER_WORK) );	
  p_param->result  = WIFIBATTLEMATCH_LISTAFTER_RESULT_SUCCESS;

  //�l�b�g���W���[���̍쐬
  p_wk->p_net   = WIFIBATTLEMATCH_NET_Init( p_param->p_param->p_game_data, NULL, HEAPID_WIFIBATTLEMATCH_SUB );
	return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	WIFI�o�g���}�b�`���	���X�g��v���Z�X�j��
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WIFIBATTLEMATCH_LISTAFTER_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{
  WIFIBATTLEMATCH_LISTAFTER_WORK  *p_wk     = p_wk_adrs;
  WIFIBATTLEMATCH_LISTAFTER_PARAM *p_param  = p_param_adrs;

  //�l�b�g���W���[���j��
  WIFIBATTLEMATCH_NET_Exit( p_wk->p_net );

  //�v���Z�X���[�N�j��
	GFL_PROC_FreeWork( p_proc );

  //�q�[�v�j��
	GFL_HEAP_DeleteHeap( HEAPID_WIFIBATTLEMATCH_SUB );

  GFL_OVERLAY_Unload( FS_OVERLAY_ID(dpw_common));
	GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common));

  return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	WIFI�o�g���}�b�`���	���X�g�チ�C���v���Z�X����
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WIFIBATTLEMATCH_LISTAFTER_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{
  enum
  { 
    SEQ_INIT,
    SEQ_START_TIMING,
    SEQ_WAIT_TIMING,
    SEQ_SEND_PARTY,
    SEQ_RECV_PARTY,
    SEQ_END,
  };

  WIFIBATTLEMATCH_LISTAFTER_WORK  *p_wk     = p_wk_adrs;
  WIFIBATTLEMATCH_LISTAFTER_PARAM *p_param  = p_param_adrs;

  switch( *p_seq )
  { 
  case SEQ_INIT:
    (*p_seq)++;
    break;

  case SEQ_START_TIMING:
    GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),17, WB_NET_WIFIMATCH);
    (*p_seq)++;
    break;

  case SEQ_WAIT_TIMING:
    if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),17, WB_NET_WIFIMATCH) )
    { 
      (*p_seq)++;
    }
    break;

  case SEQ_SEND_PARTY:
    if( WIFIBATTLEMATCH_NET_SendPokeParty( p_wk->p_net, p_param->p_player_btl_party )  )
    { 
      (*p_seq)++;
    }
    break;

  case SEQ_RECV_PARTY:
    if( WIFIBATTLEMATCH_NET_RecvPokeParty( p_wk->p_net, p_param->p_enemy_btl_party )  )
    { 
      (*p_seq)++;
    }
    break;

  case SEQ_END:
    return GFL_PROC_RES_FINISH;
  }

  //�G���[���������ŋN�����畜�A������̂Őؒf
  if( GFL_NET_IsInit() )
  { 
    if( GFL_NET_DWC_ERROR_ReqErrorDisp(TRUE) != GFL_NET_DWC_ERROR_RESULT_NONE )
    { 
      p_param->result = WIFIBATTLEMATCH_LISTAFTER_RESULT_ERROR_NEXT_LOGIN;
      return GFL_PROC_RES_FINISH;
    }
  }

  return GFL_PROC_RES_CONTINUE;
}
