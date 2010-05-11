//============================================================================
/**
 *  @file   manual.c
 *  @brief  �Q�[�����}�j���A��
 *  @author Koji Kawada
 *  @data   2010.04.26
 *  @note   
 *  ���W���[�����FMANUAL
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
#include "manual_top.h"
#include "manual_category.h"
#include "manual_title.h"
#include "manual_explain.h"
#include "app/manual.h"

// �A�[�J�C�u
#include "arc_def.h"
#include "font/font.naix"
#include "message.naix"
#include "msg/msg_manual.h"
#include "manual.naix"
#include "manual_image.naix"


// �_�~�[
#include "msg/msg_zkn.h"


// �T�E���h

// �I�[�o�[���C


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================
#define HEAP_SIZE              (0x50000)               ///< �q�[�v�T�C�Y

// ProcMain�̃V�[�P���X
enum
{
  SEQ_START          = 0,
  SEQ_FADE_IN,
  SEQ_MAIN,
  SEQ_FADE_OUT,
  SEQ_END,
};

// �t�F�[�h
#define FADE_IN_WAIT  (0)
#define FADE_OUT_WAIT (0)

// �\�����
typedef enum
{
  DISP_TYPE_TOP,
  DISP_TYPE_CATEGORY,
  DISP_TYPE_TITLE,
  DISP_TYPE_EXPLAIN,
}
DISP_TYPE;


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// PROC ���[�N
//=====================================
typedef struct
{
  // �q�[�v�A�p�����[�^�Ȃ�
  HEAPID                      heap_id;
  MANUAL_PARAM*               param;
 
  // ����
  MANUAL_COMMON_WORK*         cmn_wk;

  // VBlank��TCB
  GFL_TCB*                    vblank_tcb;

  // �\�����
  DISP_TYPE                   disp_type;

  // �g�b�v���
  MANUAL_TOP_PARAM            top_param;
  MANUAL_TOP_WORK*            top_wk;
  // �J�e�S���I�����
  MANUAL_CATEGORY_PARAM       category_param;
  MANUAL_CATEGORY_WORK*       category_wk;
  // �^�C�g���I�����
  MANUAL_TITLE_PARAM          title_param;
  MANUAL_TITLE_WORK*          title_wk;
  // �������
  MANUAL_EXPLAIN_PARAM        explain_param;
  MANUAL_EXPLAIN_WORK*        explain_wk;
}
MANUAL_WORK;


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// VBlank�֐�
static void Manual_VBlankFunc( GFL_TCB* tcb, void* wk );

// ���C��BG�̐ݒ�
static void Manual_MainBgInit( MANUAL_WORK* work );
static void Manual_MainBgExit( MANUAL_WORK* work );


//=============================================================================
/**
*  PROC
*/
//=============================================================================
static GFL_PROC_RESULT Manual_ProcInit( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static GFL_PROC_RESULT Manual_ProcExit( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static GFL_PROC_RESULT Manual_ProcMain( GFL_PROC* proc, int* seq, void* pwk, void* mywk );

const GFL_PROC_DATA  MANUAL_ProcData =
{
  Manual_ProcInit,
  Manual_ProcMain,
  Manual_ProcExit,
};


//=============================================================================
/**
*  �O�����J�֐���`
*/
//=============================================================================
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


//=============================================================================
/**
*  ���[�J���֐���`(PROC)
*/
//=============================================================================
//-------------------------------------
/// PROC ����������
//=====================================
static GFL_PROC_RESULT Manual_ProcInit( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  MANUAL_WORK*     work;

  // �q�[�v�A�p�����[�^�Ȃ�
  {
    GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_MANUAL, HEAP_SIZE );
    work = GFL_PROC_AllocWork( proc, sizeof(MANUAL_WORK), HEAPID_MANUAL );
    GFL_STD_MemClear( work, sizeof(MANUAL_WORK) );
    
    work->heap_id       = HEAPID_MANUAL;
    work->param         = (MANUAL_PARAM*)pwk;

    work->param->result = MANUAL_RESULT_RETURN;
  }

  // ����
  work->cmn_wk = MANUAL_COMMON_Init( work->param->gamedata, work->heap_id );

  // ���C��BG
  //GFL_BG_SetPriority( BG_FRAME_M_PIC,     BG_FRAME_PRI_M_PIC );
  Manual_MainBgInit( work );
  // �T�uBG
  GFL_BG_SetPriority( BG_FRAME_S_REAR,    BG_FRAME_PRI_S_REAR );
  GFL_BG_SetPriority( BG_FRAME_S_MAIN,    BG_FRAME_PRI_S_MAIN );
  GFL_BG_SetPriority( BG_FRAME_S_TB_BAR,  BG_FRAME_PRI_S_TB_BAR );
  GFL_BG_SetPriority( BG_FRAME_S_TB_TEXT, BG_FRAME_PRI_S_TB_TEXT );

  // VBlank��TCB
  work->vblank_tcb = GFUser_VIntr_CreateTCB( Manual_VBlankFunc, work, 1 );
  
  // �t�F�[�h�C��(����������)
  GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, FADE_IN_WAIT );

  // �ʐM�A�C�R��
  GFL_NET_WirelessIconEasy_HoldLCD( FALSE, work->heap_id );
  GFL_NET_ReloadIcon();

  // �e��ʂ̃p�����[�^������
  {
    // �g�b�v���
    // in
    work->top_param.cursor_pos = 0;

    // �J�e�S���I�����
    // in
    work->category_param.head_pos   = 0;
    work->category_param.cursor_pos = 0;

    // �^�C�g���I�����
    // in
    work->title_param.type        = MANUAL_TITLE_TYPE_CATEGORY;
    work->title_param.cate_no     = 0;
    work->title_param.head_pos    = 0;
    work->title_param.cursor_pos  = 0;

    // �������
    // in
    work->explain_param.page_num       = 1;
    work->explain_param.title_str_id   = 0;
    work->explain_param.page[0].image  = MANUAL_EXPLAIN_NO_IMAGE;
    work->explain_param.page[0].str_id = 0;
  }
  
  // �\�����
  work->disp_type = DISP_TYPE_TOP;
  
  // �g�b�v���
  work->top_wk = MANUAL_TOP_Init( &work->top_param, work->cmn_wk );

  return GFL_PROC_RES_FINISH;
}

//-------------------------------------
/// PROC �I������
//=====================================
static GFL_PROC_RESULT Manual_ProcExit( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  MANUAL_WORK* work = (MANUAL_WORK*)mywk;

  switch( work->disp_type )
  {
  case DISP_TYPE_TOP:
    {
      // �g�b�v���
      MANUAL_TOP_Exit( work->top_wk );
    }
    break;
  case DISP_TYPE_CATEGORY:
    {
      // �J�e�S���I�����
      MANUAL_CATEGORY_Exit( work->category_wk );
    }
    break;
  case DISP_TYPE_TITLE:
    {
      // �^�C�g���I�����
      MANUAL_TITLE_Exit( work->title_wk );
    }
    break;
  case DISP_TYPE_EXPLAIN:
    {
      // �������
      MANUAL_EXPLAIN_Exit( work->explain_wk );
    }
    break;
  }

  // �ʐM�A�C�R��
  GFL_NET_WirelessIconEasy_DefaultLCD();

  // VBlank��TCB
  GFL_TCB_DeleteTask( work->vblank_tcb );

  // ���C��BG
  Manual_MainBgExit( work );

  // ����
  MANUAL_COMMON_Exit( work->cmn_wk );

  // �q�[�v�A�p�����[�^�Ȃ�
  {
    GFL_PROC_FreeWork( proc );
    GFL_HEAP_DeleteHeap( HEAPID_MANUAL );
  }

  return GFL_PROC_RES_FINISH;
}

//-------------------------------------
/// PROC �又��
//=====================================
static GFL_PROC_RESULT Manual_ProcMain( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  MANUAL_WORK* work = (MANUAL_WORK*)mywk;

  switch(*seq)
  {
  case SEQ_START:
    {
      *seq = SEQ_FADE_IN;
    }
    break;
  case SEQ_FADE_IN:
    {
      if( !GFL_FADE_CheckFade() )
      {
        *seq = SEQ_MAIN;
      }
    }
    break;
  case SEQ_MAIN:
    {
      BOOL b_end = FALSE;
      BOOL b_ret = FALSE;

      switch( work->disp_type )
      {
      case DISP_TYPE_TOP:
        {
          // �g�b�v���
          b_ret = MANUAL_TOP_Main( work->top_wk );
          if( b_ret )
          {
            if(    work->top_param.result == MANUAL_TOP_RESULT_CLOSE
                || work->top_param.result == MANUAL_TOP_RESULT_RETURN )
            {
              if( work->top_param.result == MANUAL_TOP_RESULT_CLOSE )        work->param->result = MANUAL_RESULT_CLOSE;
              else if( work->top_param.result == MANUAL_TOP_RESULT_RETURN )  work->param->result = MANUAL_RESULT_RETURN;
              b_end = TRUE;
            }
            else
            {
              // �g�b�v���
              MANUAL_TOP_Exit( work->top_wk );

              if( work->top_param.result == MANUAL_TOP_RESULT_CATEGORY )
              {
                // �J�e�S���I�����
                work->disp_type = DISP_TYPE_CATEGORY;
                work->category_param.head_pos   = 0;
                work->category_param.cursor_pos = 0;
                work->category_wk = MANUAL_CATEGORY_Init( &work->category_param, work->cmn_wk );
              }
              else if( work->top_param.result == MANUAL_TOP_RESULT_ALL )
              {
                // �^�C�g���I�����
                work->disp_type = DISP_TYPE_TITLE;
                work->title_param.type        = MANUAL_TITLE_TYPE_ALL;
                work->title_param.cate_no     = 0;
                work->title_param.head_pos    = 0;
                work->title_param.cursor_pos  = 0;
                work->title_wk = MANUAL_TITLE_Init( &work->title_param, work->cmn_wk );
              }
            }
          }
        }
        break;
      case DISP_TYPE_CATEGORY:
        {
          // �^�C�g���I�����
          b_ret = MANUAL_CATEGORY_Main( work->category_wk );
          if( b_ret )
          {
            // �^�C�g���I�����
            MANUAL_CATEGORY_Exit( work->category_wk );

            if( work->category_param.result == MANUAL_CATEGORY_RESULT_RETURN )
            {
              // �g�b�v���
              work->disp_type = DISP_TYPE_TOP;
              // �J�[�\���ʒu�͂��̂܂܂�
              work->top_wk = MANUAL_TOP_Init( &work->top_param, work->cmn_wk );
            }
            else
            {
              // �^�C�g���I�����
              work->disp_type = DISP_TYPE_TITLE;
              work->title_param.type        = MANUAL_TITLE_TYPE_CATEGORY;
              work->title_param.cate_no     = work->category_param.cate_no;
              work->title_param.head_pos    = 0;
              work->title_param.cursor_pos  = 0;
              work->title_wk = MANUAL_TITLE_Init( &work->title_param, work->cmn_wk );
            }
          }
        }
        break;
      case DISP_TYPE_TITLE:
        {
          // �^�C�g���I�����
          b_ret = MANUAL_TITLE_Main( work->title_wk );
          if( b_ret )
          {
            // �^�C�g���I�����
            MANUAL_TITLE_Exit( work->title_wk );

            if( work->title_param.result == MANUAL_TITLE_RESULT_RETURN )
            {
              if( work->title_param.type == MANUAL_TITLE_TYPE_CATEGORY )
              {
                // �J�e�S���I�����
                work->disp_type = DISP_TYPE_CATEGORY;
                // �J�[�\���ʒu�͂��̂܂܂�
                work->category_wk = MANUAL_CATEGORY_Init( &work->category_param, work->cmn_wk );
              }
              else if( work->title_param.type == MANUAL_TITLE_TYPE_ALL )
              {
                // �g�b�v���
                work->disp_type = DISP_TYPE_TOP;
                // �J�[�\���ʒu�͂��̂܂܂�
                work->top_wk = MANUAL_TOP_Init( &work->top_param, work->cmn_wk );
              }
            }
            else
            {
              // �������
              work->disp_type = DISP_TYPE_EXPLAIN;
              {
                work->explain_param.page_num       = 7;
                work->explain_param.title_str_id   = 0;
                {
                  u8 i;
                  for( i=0; i<work->explain_param.page_num; i++ )
                  {
                    work->explain_param.page[i].image  = MANUAL_EXPLAIN_NO_IMAGE;
                    work->explain_param.page[i].str_id = i;
                  }
                  work->explain_param.page[0].image  = 0;
                  work->explain_param.page[1].image  = 1;
                  work->explain_param.page[2].image  = 2;
                  work->explain_param.page[3].image  = MANUAL_EXPLAIN_NO_IMAGE;
                  work->explain_param.page[4].image  = 0;
                  work->explain_param.page[5].image  = 1;
                  work->explain_param.page[6].image  = 2;
                }
              }
              work->explain_wk = MANUAL_EXPLAIN_Init( &work->explain_param, work->cmn_wk );
            }
          }
        }
        break;
      case DISP_TYPE_EXPLAIN:
        {
          // �������
          b_ret = MANUAL_EXPLAIN_Main( work->explain_wk );
          if( b_ret )
          {
            // �������
            MANUAL_EXPLAIN_Exit( work->explain_wk );

            if( work->explain_param.result == MANUAL_EXPLAIN_RESULT_RETURN )
            {
              // �^�C�g���I�����
              work->disp_type = DISP_TYPE_TITLE;
              // �J�e�S�����S�\�����͂��̂܂܂�
              // �J�[�\���ʒu�͂��̂܂܂�
              work->title_wk = MANUAL_TITLE_Init( &work->title_param, work->cmn_wk );
            }
          }
        }
        break;
      }
     
      // �I��
      if( b_end )
      {
        *seq = SEQ_FADE_OUT;
        // �t�F�[�h�A�E�g(�����遨��)
        GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, FADE_OUT_WAIT );
      }
    }
    break;
  case SEQ_FADE_OUT:
    {
      if( !GFL_FADE_CheckFade() )
      {
        *seq = SEQ_END;
      }
    }
    break;
  case SEQ_END:
    {
      return GFL_PROC_RES_FINISH;
    }
    break;
  }

  // ����
  MANUAL_COMMON_Main( work->cmn_wk );

  return GFL_PROC_RES_CONTINUE;
}


//=============================================================================
/**
*  ���[�J���֐���`
*/
//=============================================================================
//-------------------------------------
/// VBlank�֐�
//=====================================
static void Manual_VBlankFunc( GFL_TCB* tcb, void* wk )
{
  MANUAL_WORK* work = (MANUAL_WORK*)wk;
}

//-------------------------------------
/// ���C��BG�̐ݒ�
//=====================================
static void Manual_MainBgInit( MANUAL_WORK* work )
{
  {
    G2_SetBG2ControlDCBmp( GX_BG_SCRSIZE_DCBMP_256x256, GX_BG_AREAOVER_XLU, GX_BG_BMPSCRBASE_0x00000 );
    G2_SetBG2Priority( BG_FRAME_PRI_M_PIC );
    G2_BG0Mosaic( FALSE );
  }

  GFL_BG_SetVisible( BG_FRAME_M_PIC, VISIBLE_ON );
}
static void Manual_MainBgExit( MANUAL_WORK* work )
{
  // �������Ȃ�
}

