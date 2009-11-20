//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_bvrank_proc.c
 *	@brief	�o�g���r�f�I�����N�v���Z�X
 *	@author	Toru=Nagihashi
 *	@data		2009.11.13
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>

//�V�X�e��
#include "system/gfl_use.h"
#include "system/main.h"  //HEAPID

//�A�[�J�C�u
#include "msg/msg_battle_rec.h"

//�����̃��W���[��//�����̃��W���[��
#include "br_pokesearch.h"
#include "br_inner.h"
#include "br_util.h"
#include "br_btn.h"

//�O���Q��
#include "br_bvrank_proc.h"

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
///	�o�g���r�f�I�����N���C�����[�N
//=====================================
typedef struct 
{
  BMP_MENULIST_DATA *p_list_data;
  BR_LIST_WORK      *p_list;
  BMPOAM_SYS_PTR	  p_bmpoam;	//BMPOAM�V�X�e��
  BR_BTN_WORK       *p_btn;
  BR_MSGWIN_WORK    *p_msgwin;
  PRINT_QUE         *p_que;
	HEAPID heapID;
} BR_BVRANK_WORK;


//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	BR�R�A�v���Z�X
//=====================================
static GFL_PROC_RESULT BR_BVRANK_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT BR_BVRANK_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT BR_BVRANK_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );

//=============================================================================
/**
 *					�O���Q��
*/
//=============================================================================
//-------------------------------------
///	�o�g���r�f�I�����N�v���Z�X�v���Z�X
//=====================================
const GFL_PROC_DATA BR_BVRANK_ProcData =
{	
	BR_BVRANK_PROC_Init,
	BR_BVRANK_PROC_Main,
	BR_BVRANK_PROC_Exit,
};

//=============================================================================
/**
 *					�f�[�^
 */
//=============================================================================

//=============================================================================
/**
 *					�o�g���r�f�I�����N�v���Z�X
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�o�g���r�f�I�����N�v���Z�X	������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_BVRANK_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	BR_BVRANK_WORK				*p_wk;
	BR_BVRANK_PROC_PARAM	*p_param	= p_param_adrs;
  GFL_FONT    *p_font;
  GFL_MSGDATA *p_msg;

	//�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(BR_BVRANK_WORK), BR_PROC_SYS_GetHeapID( p_param->p_procsys ) );
	GFL_STD_MemClear( p_wk, sizeof(BR_BVRANK_WORK) );	
	p_wk->heapID	= BR_PROC_SYS_GetHeapID( p_param->p_procsys );
  p_wk->p_bmpoam	= BmpOam_Init( p_wk->heapID, p_param->p_unit );
  p_wk->p_que     = PRINTSYS_QUE_Create( p_wk->heapID );

	//�O���t�B�b�N������
  BR_RES_LoadBG( p_param->p_res, BR_RES_BG_BVRANK, p_wk->heapID );
  BR_RES_LoadOBJ( p_param->p_res, BR_RES_OBJ_SHORT_BTN_S, p_wk->heapID );

  //���W���[��������
  p_font  = BR_RES_GetFont( p_param->p_res );
  p_msg   = BR_RES_GetMsgData( p_param->p_res );
  //�{�^��
  { 
    GFL_CLWK_DATA cldata;
    BR_RES_OBJ_DATA res;
    BOOL ret;


    GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );

    cldata.pos_x    = 80;
    cldata.pos_y    = 168;
    cldata.anmseq   = 0;
    cldata.softpri  = 1;

    ret = BR_RES_GetOBJRes( p_param->p_res, BR_RES_OBJ_SHORT_BTN_S, &res );
    GF_ASSERT( ret );

    p_wk->p_btn = BR_BTN_Init( &cldata, msg_05, CLSYS_DRAW_SUB, p_param->p_unit, p_wk->p_bmpoam, p_font, p_msg, &res, p_wk->heapID );
;
  }
  //���b�Z�[�W
  { 
    //�o�g���r�f�I������
    p_wk->p_msgwin  = BR_MSGWIN_Init( BG_FRAME_S_FONT, 11, 6, 10, 4, PLT_BG_S_FONT, p_wk->p_que, p_wk->heapID );
    BR_MSGWIN_PrintColor( p_wk->p_msgwin, p_msg, msg_603, p_font, BR_PRINT_COL_NORMAL );
  }
  //���X�g
  { 
    static const BR_LIST_PARAM *sc_list_param
    { 

    };
    p_wk->p_list_data = BmpMenuWork_ListCreate( p_wk->monsno_len, p_wk->heapID );
    
  }

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�o�g���r�f�I�����N�v���Z�X	������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_BVRANK_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	BR_BVRANK_WORK				*p_wk	= p_wk_adrs;
	BR_BVRANK_PROC_PARAM	*p_param	= p_param_adrs;

	//���W���[���j��
  BR_MSGWIN_Exit( p_wk->p_msgwin );
  BR_BTN_Exit( p_wk->p_btn );
  BR_RES_UnLoadOBJ( p_param->p_res, BR_RES_OBJ_SHORT_BTN_S );
  BR_RES_UnLoadBG( p_param->p_res, BR_RES_BG_BVRANK );

  BmpOam_Exit( p_wk->p_bmpoam ); 
  PRINTSYS_QUE_Delete( p_wk->p_que );
	//�v���Z�X���[�N�j��
	GFL_PROC_FreeWork( p_proc );

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�o�g���r�f�I�����N�v���Z�X	������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_BVRANK_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	BR_BVRANK_WORK	*p_wk	= p_wk_adrs;
	BR_BVRANK_PROC_PARAM	*p_param	= p_param_adrs;

  {
    u32 x, y;
    if( GFL_UI_TP_GetPointTrg( &x, &y ) )
    {
      if( BR_BTN_GetTrg( p_wk->p_btn, x, y ) )
      { 
        BR_PROC_SYS_Pop( p_param->p_procsys );
        return GFL_PROC_RES_FINISH;
      }
    }
  }

  BR_MSGWIN_PrintMain( p_wk->p_msgwin );
  PRINTSYS_QUE_Main( p_wk->p_que );

	return GFL_PROC_RES_CONTINUE;
}

