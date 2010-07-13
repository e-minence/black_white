//==============================================================================
/**
 * @file	codein.c
 * @brief	�������̓C���^�[�t�F�[�X
 * @author	goto
 * @date	2007.07.11(��)
 *
 *	GS���ڐA Ari090324
 *	�o�g�����R�[�_�[�p�ɕύX  nagihashi 09.11.23
 *
 */
//==============================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"
#include "arc_def.h"
#include "message.naix"
#include "font/font.naix"

#include "poke_tool/poke_tool.h"
#include "poke_tool/monsno_def.h"
//#include "poke_tool/pokeicon.h"
//#include "poke_tool/boxdata.h"
#include "system/bmp_menu.h"
#include "sound/pm_sndsys.h"

#include "br_codein_pv.h"




// ----------------------------------------
//
//	�v���g�^�C�v
//
// ----------------------------------------
static void CI_VBlank( GFL_TCB *tcb, void *work );


//----------------------------------------------------------------------------
/**
 *	@brief  �R�[�h�C��������
 *
 *	@param	const BR_CODEIN_PARAM *cp_param  �p�����[�^
 *	@param	heapID                        �q�[�vID 
 *
 *	@return ���[�N
 *
 * PROC�ŏo���Ă���CODEIN���V�X�e���ɒu��������
 */
//-----------------------------------------------------------------------------
BR_CODEIN_WORK * BR_CODEIN_Init( const BR_CODEIN_PARAM *cp_param, HEAPID heapID )
{
	BR_CODEIN_WORK* wk;
	
  wk  = GFL_HEAP_AllocMemory( heapID, sizeof(BR_CODEIN_WORK) );
	GFL_STD_MemFill( wk, 0, sizeof( BR_CODEIN_WORK ) );
  wk->param = *cp_param;
  wk->heapID  = heapID;
  wk->sys.cellUnit  = cp_param->p_unit;
  wk->sys.fontHandle  = BR_RES_GetFont( cp_param->p_res );

	
	///< �V�X�e�����[�N�̍쐬	
	wk->sys.p_handle = GFL_ARC_OpenDataHandle( ARCID_CODE_INPUT, wk->heapID );	
	
	///< �ݒ菉����
	CI_pv_ParamInit( wk );
	
	///< Touch Panel�ݒ�
	{
		CI_pv_ButtonManagerInit( wk );
	}
	
					   
	wk->sys.vBlankTcb = GFUser_VIntr_CreateTCB( CI_VBlank , wk , 16 );

  return wk;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �R�[�h�C�����C������
 *
 *	@param	BR_CODEIN_WORK *p_wk ��[��
 *	@param  TRUE�Ȃ�ΏI��  FALSE�Ȃ�΂܂�
 *
 * PROC�ŏo���Ă���CODEIN���V�X�e���ɒu��������
 */
//-----------------------------------------------------------------------------
void BR_CODEIN_Main( BR_CODEIN_WORK *wk )
{
	BOOL bUpdate;
	
	bUpdate = CI_pv_MainUpdate( wk );
	
	//GFL_CLACT_SYS_Main();
}

//----------------------------------------------------------------------------
/**
 *	@brief  �R�[�h�C�����͌��m
 *
 *	@param	const BR_CODEIN_WORK *wk   ���[�N
 *
 *	@return �I����������
 */
//-----------------------------------------------------------------------------
BR_CODEIN_SELECT BR_CODEIN_GetSelect( const BR_CODEIN_WORK *wk )
{ 
  return  wk->select;
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���͂������l���ꊇ�Ŏ擾
 *
 *	@param	const BR_CODEIN_WORK *wk ���[�N
 *
 *	@return ���͂������l���ꊇ�Ŏ擾
 */
//-----------------------------------------------------------------------------
u64 BR_CODEIN_GetNumber( const BR_CODEIN_WORK *wk )
{ 
  return wk->code_number;
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���͂������l���u���b�N�������Ď擾
 *
 *	@param	const BR_CODEIN_WORK *wk  ���[�N
 *	@param	*p_tbl                    �z��i��{��BR_CODE_BLOCK_MAX���̔z��j
 *	@param	tbl_max                   �z��̗v�f���i��{��BR_CODE_BLOCK_MAX�j
 */
//-----------------------------------------------------------------------------
void BR_CODEINT_GetNumberBlock( const BR_CODEIN_WORK *wk, int *p_tbl, int tbl_max )
{ 
  int i;
  for( i = 0; i < tbl_max && i < BR_CODE_BLOCK_MAX; i++ )
  { 
    p_tbl[i]  = wk->code_block[i];
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �R�[�h�C��  �I��
 *
 *	@param	BR_CODEIN_WORK *p_wk ��[��
 *
 * PROC�ŏo���Ă���CODEIN���V�X�e���ɒu��������
 */
//-----------------------------------------------------------------------------
void BR_CODEIN_Exit( BR_CODEIN_WORK *wk )
{
	GFL_TCB_DeleteTask( wk->sys.vBlankTcb );
	
	CI_pv_disp_CodeRes_Delete( wk );
	
	GFL_ARC_CloseDataHandle( wk->sys.p_handle );
	
	GFL_BMN_Delete( wk->sys.btn );
	
  GFL_HEAP_FreeMemory( wk );
}





//--------------------------------------------------------------
/**
 * @brief	BR_CODEIN_PARAM �̃��[�N���쐬����
 *
 * @param	heap_id	
 * @param	word_len	
 * @param	block	
 *
 * @retval	BR_CODEIN_PARAM*	
 *
 */
//--------------------------------------------------------------
BR_CODEIN_PARAM*	BR_CODEIN_ParamCreate( int heap_id, int word_len, GFL_CLUNIT *p_unit, BR_RES_WORK *p_res, BR_BALLEFF_WORK *p_balleff, int block[] )
{
	int i;
	BR_CODEIN_PARAM* wk = NULL;
	
	wk = GFL_HEAP_AllocMemory( heap_id, sizeof( BR_CODEIN_PARAM ) );
  wk->p_unit    = p_unit;
  wk->p_res     = p_res;
	wk->word_len 	= word_len;	
	wk->strbuf		= GFL_STR_CreateBuffer( word_len + 1, heap_id );
  wk->p_balleff = p_balleff;
	
	for ( i = 0; i < BR_CODE_BLOCK_MAX; i++ ){
		wk->block[ i ] = block[ i ];
		OS_Printf( "block %d = %d\n", i, wk->block[ i ] );
	}
	wk->block[ i ] = block[ i - 1 ];

	return wk;	
}

//--------------------------------------------------------------
/**
 * @brief	BR_CODEIN_PARAM �̃��[�N�����
 *
 * @param	codein_param	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_CODEIN_ParamDelete( BR_CODEIN_PARAM* codein_param )
{
	GF_ASSERT_HEAVY( codein_param->strbuf != NULL );
	GF_ASSERT_HEAVY( codein_param != NULL );
	
	GFL_STR_DeleteBuffer( codein_param->strbuf );
	GFL_HEAP_FreeMemory( codein_param );	
}



//--------------------------------------------------------------
/**
 * @brief	VBlank
 *
 * @param	work	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void CI_VBlank( GFL_TCB *tcb, void *work )
{
	BR_CODEIN_WORK* wk = work;
}
