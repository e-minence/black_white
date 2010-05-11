//============================================================================
/**
 *  @file   manual_category.c
 *  @brief  �Q�[�����}�j���A��
 *  @author Koji Kawada
 *  @data   2010.04.26
 *  @note   
 *  ���W���[�����FMANUAL_CATEGORY
 */
//============================================================================
// �C���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "gamesystem/gamedata_def.h"
#include "gamesystem/game_data.h"
#include "print/gf_font.h"
#include "print/printsys.h"

#include "manual_graphic.h"
#include "manual_def.h"
#include "manual_common.h"
#include "manual_list.h"
#include "manual_category.h"

// �A�[�J�C�u
#include "arc_def.h"
#include "font/font.naix"
#include "message.naix"
#include "msg/msg_manual.h"
#include "manual.naix"

// �_�~�[
#include "msg/msg_zkn.h"
#include "zukan_gra.naix"


// �T�E���h

// �I�[�o�[���C


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// ���[�N
//=====================================
struct _MANUAL_CATEGORY_WORK
{
  // �p�����[�^
  MANUAL_CATEGORY_PARAM*      param;
  // ����
  MANUAL_COMMON_WORK*         cmn_wk;

  // �����ō쐬
  MANUAL_LIST_PARAM           list_param;
  MANUAL_LIST_WORK*           list_wk;

  // VBlank��TCB
  GFL_TCB*                    vblank_tcb;
};


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// VBlank�֐�
static void Manual_Category_VBlankFunc( GFL_TCB* tcb, void* wk );


//=============================================================================
/**
*  �O�����J�֐���`
*/
//=============================================================================
// ����������
MANUAL_CATEGORY_WORK*  MANUAL_CATEGORY_Init(
    MANUAL_CATEGORY_PARAM*  param,
    MANUAL_COMMON_WORK*     cmn_wk
)
{
  // ���[�N
  MANUAL_CATEGORY_WORK*  work  = GFL_HEAP_AllocClearMemory( cmn_wk->heap_id, sizeof(MANUAL_CATEGORY_WORK) );

  // �p�����[�^
  work->param  = param;
  {
    // out
    work->param->result   = MANUAL_CATEGORY_RESULT_RETURN;
    work->param->cate_no  = 0;
  }
  // ����
  work->cmn_wk = cmn_wk;

  // �����ō쐬
  {
    u16 i;
    work->list_param.type = MANUAL_LIST_TYPE_CATEGORY;
    work->list_param.num  = 4;
    work->list_param.item = GFL_HEAP_AllocClearMemory( work->cmn_wk->heap_id, sizeof(MANUAL_LIST_ITEM)*work->list_param.num );
    for( i=0; i<work->list_param.num; i++ )
    {
      work->list_param.item[i].no     = i;
      work->list_param.item[i].str_id = i;
      work->list_param.item[i].icon   = MANUAL_LIST_ICON_NONE;
    }
    work->list_param.head_pos    = work->param->head_pos;
    work->list_param.cursor_pos  = work->param->cursor_pos;

    work->list_wk = MANUAL_LIST_Init( &work->list_param, cmn_wk );
  }

  // VBlank��TCB
  work->vblank_tcb = GFUser_VIntr_CreateTCB( Manual_Category_VBlankFunc, work, 1 );

  return work;
}

// �I������
void  MANUAL_CATEGORY_Exit(
    MANUAL_CATEGORY_WORK*     work
)
{
  // VBlank��TCB
  GFL_TCB_DeleteTask( work->vblank_tcb );

  // �p�����[�^
  {
    // out
    work->param->head_pos   = work->list_param.head_pos;
    work->param->cursor_pos = work->list_param.cursor_pos;
    
    if( work->list_param.result == MANUAL_LIST_RESULT_RETURN )
    {
      work->param->result    = MANUAL_CATEGORY_RESULT_RETURN;
    }
    else if( work->list_param.result == MANUAL_LIST_RESULT_ITEM )
    {
      work->param->result   = MANUAL_CATEGORY_RESULT_TITLE;
      work->param->cate_no  = work->list_param.item[ work->param->cursor_pos ].no;
    }
  }

  // �����ō쐬
  {
    MANUAL_LIST_Exit( work->list_wk );
    GFL_HEAP_FreeMemory( work->list_param.item );
  }

  // ���[�N
  GFL_HEAP_FreeMemory( work );
}

// �又��
BOOL  MANUAL_CATEGORY_Main(
    MANUAL_CATEGORY_WORK*     work
)
{
  return MANUAL_LIST_Main( work->list_wk );
}


//------------------------------------------------------------------
/**
 *  @brief           
 *
 *  @param[in]       
 *  @param[in,out]       
 *
 *  @retval          
 */
//------------------------------------------------------------------

//-------------------------------------
/// 
//=====================================


//=============================================================================
/**
*  ���[�J���֐���`
*/
//=============================================================================
//-------------------------------------
/// VBlank�֐�
//=====================================
static void Manual_Category_VBlankFunc( GFL_TCB* tcb, void* wk )
{
  MANUAL_CATEGORY_WORK* work = (MANUAL_CATEGORY_WORK*)wk;
}

