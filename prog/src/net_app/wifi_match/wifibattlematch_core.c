//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifibattlematch_core.c
 *	@brief	WIFI�̃o�g���}�b�`�R�A���
 *	@author	Toru=Nagihashi
 *	@data		2009.11.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//	���C�u����
#include <gflib.h>

//WIFI�o�g���}�b�`�̗���
#include "wifibattlematch_rnd.h"
#include "wifibattlematch_wifi.h"

//�O�����J
#include "wifibattlematch_core.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================


//=============================================================================
/**
 *        ��`
*/
//=============================================================================
FS_EXTERN_OVERLAY(wifibattlematch_view);

//=============================================================================
/**
 *					�v���g�^�C�v
*/
//=============================================================================
//-------------------------------------
///	�v���Z�X
//=====================================
static GFL_PROC_RESULT WIFIBATTLEMATCH_CORE_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT WIFIBATTLEMATCH_CORE_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT WIFIBATTLEMATCH_CORE_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );

//=============================================================================
/**
 *					�O���Q��
*/
//=============================================================================
//-------------------------------------
///	PROC
//=====================================
const GFL_PROC_DATA	WifiBattleMatchCore_ProcData =
{	
	WIFIBATTLEMATCH_CORE_PROC_Init,
	WIFIBATTLEMATCH_CORE_PROC_Main,
	WIFIBATTLEMATCH_CORE_PROC_Exit,
};

//=============================================================================
/**
 *					PROC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	WIFI�o�g���}�b�`���	�R�A�v���Z�X������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WIFIBATTLEMATCH_CORE_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
  WIFIBATTLEMATCH_CORE_PARAM *p_param  = p_param_adrs;

  GFL_OVERLAY_Load( FS_OVERLAY_ID(wifibattlematch_view) );

  switch( p_param->p_param->mode )
  { 
  case WIFIBATTLEMATCH_MODE_RANDOM:  //�����_���ΐ�
    return WifiBattleMatchRnd_ProcData.init_func( p_proc, p_seq, p_param_adrs, p_wk_adrs );

  case WIFIBATTLEMATCH_MODE_WIFI:    //WIFI���
    return WifiBattleMatchWifi_ProcData.init_func( p_proc, p_seq, p_param_adrs, p_wk_adrs );

  case WIFIBATTLEMATCH_MODE_LIVE:    //���C�u���
    return WifiBattleMatchRnd_ProcData.init_func( p_proc, p_seq, p_param_adrs, p_wk_adrs );

  default:
    GF_ASSERT( 0 );
    return 0;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	WIFI�o�g���}�b�`���	�R�A�v���Z�X�j��
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WIFIBATTLEMATCH_CORE_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{
  GFL_PROC_RESULT ret;
  WIFIBATTLEMATCH_CORE_PARAM *p_param  = p_param_adrs;

  switch( p_param->p_param->mode )
  { 
  case WIFIBATTLEMATCH_MODE_RANDOM:  //�����_���ΐ�
    ret = WifiBattleMatchRnd_ProcData.end_func( p_proc, p_seq, p_param_adrs, p_wk_adrs );
    break;

  case WIFIBATTLEMATCH_MODE_WIFI:    //WIFI���
    ret = WifiBattleMatchWifi_ProcData.end_func( p_proc, p_seq, p_param_adrs, p_wk_adrs );
    break;

  case WIFIBATTLEMATCH_MODE_LIVE:    //���C�u���
    ret = WifiBattleMatchRnd_ProcData.end_func( p_proc, p_seq, p_param_adrs, p_wk_adrs );
    break;

  default:
    GF_ASSERT( 0 );
  }

  GFL_OVERLAY_Unload( FS_OVERLAY_ID(wifibattlematch_view) );

  return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief	WIFI�o�g���}�b�`���	���C���v���Z�X����
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WIFIBATTLEMATCH_CORE_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{
  WIFIBATTLEMATCH_CORE_PARAM *p_param  = p_param_adrs;

  switch( p_param->p_param->mode )
  { 
  case WIFIBATTLEMATCH_MODE_RANDOM:  //�����_���ΐ�
    return WifiBattleMatchRnd_ProcData.main_func( p_proc, p_seq, p_param_adrs, p_wk_adrs );

  case WIFIBATTLEMATCH_MODE_WIFI:    //WIFI���
    return WifiBattleMatchWifi_ProcData.main_func( p_proc, p_seq, p_param_adrs, p_wk_adrs );

  case WIFIBATTLEMATCH_MODE_LIVE:    //���C�u���
    return WifiBattleMatchRnd_ProcData.main_func( p_proc, p_seq, p_param_adrs, p_wk_adrs );

  default:
    GF_ASSERT( 0 );
    return 0;
  }
}
