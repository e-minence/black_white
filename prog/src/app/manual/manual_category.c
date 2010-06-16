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
#include "manual_data.h"
#include "manual_common.h"
#include "manual_list.h"
#include "manual_category.h"

// �A�[�J�C�u
#include "arc_def.h"
#include "font/font.naix"
#include "message.naix"
#include "msg/msg_manual.h"


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
    work->list_param.type = MANUAL_LIST_TYPE_CATEGORY;

    {
      u16 cate_idx;
      u16 cate_num = MANUAL_DATA_CateRefGetTotalCateNum( work->cmn_wk->data_wk );
      
      work->list_param.num  = 0;
      work->list_param.item = GFL_HEAP_AllocClearMemory( work->cmn_wk->heap_id, sizeof(MANUAL_LIST_ITEM)*cate_num );  // �ő���g��Ȃ����Ƃ��������낤���A�ő�Ŋm�ۂ��Ă���
      
      for( cate_idx=0; cate_idx<cate_num; cate_idx++ )
      {
        u16 open_title_num = 0;
        u16 title_num = MANUAL_DATA_CateGetTitleNum( work->cmn_wk->data_wk, cate_idx );
        u16 title_order; 
        for( title_order=0; title_order<title_num; title_order++ )
        {
          u16 title_idx = MANUAL_DATA_CateGetTitleIdx( work->cmn_wk->data_wk, cate_idx, title_order );
          u16 open_flag = MANUAL_DATA_TitleGetOpenFlag( work->cmn_wk->data_wk, title_idx );
          if( MANUAL_DATA_OpenFlagIsOpen( work->cmn_wk->data_wk, open_flag, work->cmn_wk->gamedata ) )
          {
            open_title_num++;
          }
        }
        if( open_title_num > 0 )
        {
          work->list_param.item[work->list_param.num].no     = cate_idx;
          work->list_param.item[work->list_param.num].str_id = MANUAL_DATA_CateGetCateGmmId( work->cmn_wk->data_wk, cate_idx );
          work->list_param.item[work->list_param.num].icon   = MANUAL_LIST_ICON_NONE;
          work->list_param.num++;
        }
      }
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

