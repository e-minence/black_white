//============================================================================
/**
 *  @file   manual_title.c
 *  @brief  �Q�[�����}�j���A��
 *  @author Koji Kawada
 *  @data   2010.04.26
 *  @note   
 *  ���W���[�����FMANUAL_TITLE
 */
//============================================================================


// �f�o�b�O�@�\
//#define DEBUG_LONG_LIST  // ���ꂪ��`����Ă���Ƃ��A�������X�g���쐬����(���X�g�̃e�X�g�p�Ȃ̂őI�ԂƎ~�܂邩��)


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
#include "manual_title.h"

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
struct _MANUAL_TITLE_WORK
{
  // �p�����[�^
  MANUAL_TITLE_PARAM*         param;
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
static void Manual_Title_VBlankFunc( GFL_TCB* tcb, void* wk );


//=============================================================================
/**
*  �O�����J�֐���`
*/
//=============================================================================
// ����������
MANUAL_TITLE_WORK*  MANUAL_TITLE_Init(
    MANUAL_TITLE_PARAM*  param,
    MANUAL_COMMON_WORK*  cmn_wk
)
{
  // ���[�N
  MANUAL_TITLE_WORK*  work  = GFL_HEAP_AllocClearMemory( cmn_wk->heap_id, sizeof(MANUAL_TITLE_WORK) );

  // �p�����[�^
  work->param  = param;
  {
    // out
    work->param->result    = MANUAL_TITLE_RESULT_RETURN;
    work->param->serial_no = 0;  // serial_no�ƌ����Ă��邪���ۂ�title_idx�ł���B
  }
  // ����
  work->cmn_wk = cmn_wk;

  // �����ō쐬
  {
    if( work->param->type == MANUAL_TITLE_TYPE_CATEGORY )
    {
      work->list_param.type = MANUAL_LIST_TYPE_TITLE;

      {
        u16 title_order;
        u16 cate_idx  = work->param->cate_no;
        u16 title_num = MANUAL_DATA_CateGetTitleNum( work->cmn_wk->data_wk, cate_idx );
        
        work->list_param.num  = 0;
        work->list_param.item = GFL_HEAP_AllocClearMemory( work->cmn_wk->heap_id, sizeof(MANUAL_LIST_ITEM)*title_num );  // �ő���g��Ȃ����Ƃ��������낤���A�ő�Ŋm�ۂ��Ă���
        
        for( title_order=0; title_order<title_num; title_order++ )
        {
          u16 title_idx = MANUAL_DATA_CateGetTitleIdx( work->cmn_wk->data_wk, cate_idx, title_order );
          u16 open_flag = MANUAL_DATA_TitleGetOpenFlag( work->cmn_wk->data_wk, title_idx );
          if( MANUAL_DATA_OpenFlagIsOpen( work->cmn_wk->data_wk, open_flag, work->cmn_wk->gamedata ) )
          {
            u16 read_flag = MANUAL_DATA_TitleGetReadFlag( work->cmn_wk->data_wk, title_idx );
            BOOL is_read  = MANUAL_DATA_ReadFlagIsRead( work->cmn_wk->data_wk, read_flag, work->cmn_wk->gamedata );
            
            work->list_param.item[work->list_param.num].no     = title_idx;
            work->list_param.item[work->list_param.num].str_id = MANUAL_DATA_TitleGetTitleGmmId( work->cmn_wk->data_wk, title_idx );
            work->list_param.item[work->list_param.num].icon   = (is_read)?(MANUAL_LIST_ICON_NONE):(MANUAL_LIST_ICON_NEW);

            work->list_param.num++;
          }
        }
      }

      work->list_param.head_pos    = work->param->head_pos;
      work->list_param.cursor_pos  = work->param->cursor_pos;
    }
    else  // if( work->param->type == MANUAL_TITLE_TYPE_ALL )
    {

#ifdef DEBUG_LONG_LIST

      work->list_param.type = MANUAL_LIST_TYPE_TITLE;

      work->list_param.num  = 15;
      work->list_param.item = GFL_HEAP_AllocClearMemory( work->cmn_wk->heap_id, sizeof(MANUAL_LIST_ITEM)*work->list_param.num );

      {
        u16 i;
        for( i=0; i<work->list_param.num; i++ )
        {
          {
            work->list_param.item[i].no     = i;
            work->list_param.item[i].str_id = i;
            work->list_param.item[i].icon   = (i%3)?(MANUAL_LIST_ICON_NONE):(MANUAL_LIST_ICON_NEW);
          }
        }
      }

      work->list_param.head_pos    = work->param->head_pos;
      work->list_param.cursor_pos  = work->param->cursor_pos;

#else

      work->list_param.type = MANUAL_LIST_TYPE_TITLE;

      {
        u16 title_idx;
        u16 title_num = MANUAL_DATA_TitleRefGetTotalTitleNum( work->cmn_wk->data_wk );

        work->list_param.num  = 0;
        work->list_param.item = GFL_HEAP_AllocClearMemory( work->cmn_wk->heap_id, sizeof(MANUAL_LIST_ITEM)*title_num );  // �ő���g��Ȃ����Ƃ��������낤���A�ő�Ŋm�ۂ��Ă���

        for( title_idx=0; title_idx<title_num; title_idx++ )
        {
          u16 open_flag = MANUAL_DATA_TitleGetOpenFlag( work->cmn_wk->data_wk, title_idx );
          if( MANUAL_DATA_OpenFlagIsOpen( work->cmn_wk->data_wk, open_flag, work->cmn_wk->gamedata ) )
          {
            u16 read_flag = MANUAL_DATA_TitleGetReadFlag( work->cmn_wk->data_wk, title_idx );
            BOOL is_read  = MANUAL_DATA_ReadFlagIsRead( work->cmn_wk->data_wk, read_flag, work->cmn_wk->gamedata );
            
            work->list_param.item[work->list_param.num].no     = title_idx;
            work->list_param.item[work->list_param.num].str_id = MANUAL_DATA_TitleGetTitleGmmId( work->cmn_wk->data_wk, title_idx );
            work->list_param.item[work->list_param.num].icon   = (is_read)?(MANUAL_LIST_ICON_NONE):(MANUAL_LIST_ICON_NEW);

            work->list_param.num++;
          }
        }
      }

      work->list_param.head_pos    = work->param->head_pos;
      work->list_param.cursor_pos  = work->param->cursor_pos;

#endif

    }

    work->list_wk = MANUAL_LIST_Init( &work->list_param, cmn_wk );
  }

  // VBlank��TCB
  work->vblank_tcb = GFUser_VIntr_CreateTCB( Manual_Title_VBlankFunc, work, 1 );

  return work;
}

// �I������
void  MANUAL_TITLE_Exit(
    MANUAL_TITLE_WORK*     work
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
      work->param->result    = MANUAL_TITLE_RESULT_RETURN;
    }
    else if( work->list_param.result == MANUAL_LIST_RESULT_ITEM )
    {
      work->param->result    = MANUAL_TITLE_RESULT_ITEM;
      work->param->serial_no = work->list_param.item[ work->param->cursor_pos ].no;  // serial_no�ƌ����Ă��邪���ۂ�title_idx�ł���B
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
BOOL  MANUAL_TITLE_Main(
    MANUAL_TITLE_WORK*     work
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
static void Manual_Title_VBlankFunc( GFL_TCB* tcb, void* wk )
{
  MANUAL_TITLE_WORK* work = (MANUAL_TITLE_WORK*)wk;
}

