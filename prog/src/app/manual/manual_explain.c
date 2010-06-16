//============================================================================
/**
 *  @file   manual_explain.c
 *  @brief  �Q�[�����}�j���A��
 *  @author Koji Kawada
 *  @data   2010.04.26
 *  @note   
 *  ���W���[�����FMANUAL_EXPLAIN
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
#include "manual_touchbar.h"
#include "manual_explain.h"

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
// ���͏��
typedef enum
{
  INPUT_STATE_NONE,  // ���̓��̓A�j���[�V�������s���Ă��Ȃ��̂ŁA���͉\�B
  INPUT_STATE_END,   // �������̓��͂��󂯕t���Ȃ��B
  INPUT_STATE_TB,    // �^�b�`�o�[�̓��̓A�j���[�V�������s���Ă���Œ��Ȃ̂ŁA���͕s�\�B
}
INPUT_STATE;

// �e�L�X�g
enum
{
  TEXT_DUMMY,      // BG_FRAME_S_MAIN
  TEXT_TITLE,      // BG_FRAME_S_MAIN
  TEXT_EXPLAIN,    // BG_FRAME_S_MAIN
  TEXT_MAX,
};

static const u8 bmpwin_setup[TEXT_MAX][9] =
{
  // frmnum              posx  posy  sizx  sizy  palnum                dir                    x  y
  {  BG_FRAME_S_MAIN,       0,    0,    1,    1, BG_PAL_POS_S_TEXT,    GFL_BMP_CHRAREA_GET_F, 0, 0 },  // TEXT_DUMMY
  {  BG_FRAME_S_MAIN,       1,    0,   30,    2, BG_PAL_POS_S_TEXT,    GFL_BMP_CHRAREA_GET_F, 0, 1 },  // TEXT_TITLE
  {  BG_FRAME_S_MAIN,       0,    2,   32,   19, BG_PAL_POS_S_TEXT,    GFL_BMP_CHRAREA_GET_F, 4, 6 },  // TEXT_EXPLAIN
};

#define TEXT_TITLE_COLOR_L    (15)  // ������F
#define TEXT_TITLE_COLOR_S    ( 2)  // �����e�F
#define TEXT_TITLE_COLOR_B    ( 0)  // �����w�i�F(����)

#define TEXT_EXPLAIN_COLOR_L  ( 1)  // ������F
#define TEXT_EXPLAIN_COLOR_S  ( 2)  // �����e�F
#define TEXT_EXPLAIN_COLOR_B  ( 0)  // �����w�i�F(����)

// �I�����N�G�X�g
#define END_REQ_COUNT_START_VAL (1)   // �J�E���g�_�E���J�n�l
#define END_REQ_COUNT_NO        (-1)  // �J�E���g�_�E�����Ȃ��l


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// ���[�N
//=====================================
struct _MANUAL_EXPLAIN_WORK
{
  // �p�����[�^
  MANUAL_EXPLAIN_PARAM*       param;
  // ����
  MANUAL_COMMON_WORK*         cmn_wk;

  // �����ō쐬
  u8                          page_no;  // 0<= <work->param->page_num
  BOOL                        page_change_req;  // �y�[�W��ύX����K�v������Ƃ�TRUE

  // �e�L�X�g
  GFL_BMPWIN*                 text_bmpwin[TEXT_MAX];
  PRINT_QUE*                  print_que_text[TEXT_MAX];  // ���̂Ƃ���ō쐬�������̂̃|�C���^
  BOOL                        text_trans[TEXT_MAX];      // �]������K�v������ꍇTRUE

  // VBlank��TCB
  GFL_TCB*                    vblank_tcb;

  // ���͂̏��
  INPUT_STATE                 input_state;

  // �I�����N�G�X�g
  s8                          end_req_count;  // ���̂Ƃ��J�E���g�_�E�����Ȃ��B���̂Ƃ��J�E���g�_�E������B0�ɂȂ�����I���B
};


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// VBlank�֐�
static void Manual_Explain_VBlankFunc( GFL_TCB* tcb, void* wk );

// ����
static void Manual_Explain_Prepare( MANUAL_EXPLAIN_WORK* work );
// �]��(VBlank�]���ł͂Ȃ�)
static void Manual_Explain_Trans( MANUAL_EXPLAIN_WORK* work );
// �]��(VBlank�]��)
static void Manual_Explain_TransVBlank( MANUAL_EXPLAIN_WORK* work );
// ��Еt��
static void Manual_Explain_Finish( MANUAL_EXPLAIN_WORK* work );

// �e�L�X�g
static void Manual_Explain_TextInit( MANUAL_EXPLAIN_WORK* work );
static void Manual_Explain_TextExit( MANUAL_EXPLAIN_WORK* work );
static void Manual_Explain_TextMain( MANUAL_EXPLAIN_WORK* work );

// �y�[�W����(���͂ƃe�L�X�g�̐���)
static void Manual_Explain_PageChange( MANUAL_EXPLAIN_WORK* work );


//=============================================================================
/**
*  �O�����J�֐���`
*/
//=============================================================================
// ����������
MANUAL_EXPLAIN_WORK*  MANUAL_EXPLAIN_Init(
    MANUAL_EXPLAIN_PARAM*   param,
    MANUAL_COMMON_WORK*     cmn_wk
)
{
  // ���[�N
  MANUAL_EXPLAIN_WORK*  work  = GFL_HEAP_AllocClearMemory( cmn_wk->heap_id, sizeof(MANUAL_EXPLAIN_WORK) );

  // �p�����[�^
  work->param  = param;
  {
    // out
    work->param->result   = MANUAL_EXPLAIN_RESULT_RETURN;
  }
  // ����
  work->cmn_wk = cmn_wk;

  // �����ō쐬
  {
    work->page_no = 0;
    work->page_change_req = FALSE;
  }

  Manual_Explain_Prepare( work );
  //Manual_Explain_Trans( work );
  Manual_Explain_TransVBlank( work );

  Manual_Explain_TextInit( work );

  // VBlank��TCB
  work->vblank_tcb = GFUser_VIntr_CreateTCB( Manual_Explain_VBlankFunc, work, 1 );

  // ���͏��
  work->input_state = INPUT_STATE_NONE;

  // �I�����N�G�X�g
  work->end_req_count = END_REQ_COUNT_NO;

  // �}�j���A���^�b�`�o�[
  MANUAL_TOUCHBAR_SetType( work->cmn_wk->mtb_wk, MANUAL_TOUCHBAR_TYPE_EXPLAIN );
  MANUAL_TOUCHBAR_SetPage( work->cmn_wk->mtb_wk, work->param->page_num, work->page_no );

  // �ŏ��̃y�[�W
  {
    Manual_Explain_PageChange( work );
  }

  return work;
}

// �I������
void  MANUAL_EXPLAIN_Exit(
    MANUAL_EXPLAIN_WORK*     work
)
{
  // �摜���摜�Ȃ��̂Ƃ��̂��̂ɂ��Ă���
  MANUAL_COMMON_DrawBgMDcbmp( work->cmn_wk, MANUAL_DATA_ImageIdGetNoImage( work->cmn_wk->data_wk ) );

  // VBlank��TCB
  GFL_TCB_DeleteTask( work->vblank_tcb );

  // �����ō쐬
  //Manual_Explain_TextExit( work );

  // ���[�N
  GFL_HEAP_FreeMemory( work );
}

// �又��
BOOL  MANUAL_EXPLAIN_Main(
    MANUAL_EXPLAIN_WORK*     work
)
{
  BOOL b_end = FALSE;

  if( work->end_req_count == 0 )
  {
    b_end = TRUE;
  }
  else if( work->end_req_count > 0 )
  {
    if( work->end_req_count == END_REQ_COUNT_START_VAL )
    {
      Manual_Explain_TextExit( work );
      Manual_Explain_Finish( work );
    }
    work->end_req_count--;
  }

  // �e�L�X�g
  if( work->input_state != INPUT_STATE_END )
  {
    Manual_Explain_TextMain( work );
  }

  // �}�j���A���^�b�`�o�[�̓��͂𒲂ׂ�
  if(    work->input_state == INPUT_STATE_NONE
      || work->input_state == INPUT_STATE_TB )
  {
    // �}�j���A���^�b�`�o�[
    MANUAL_TOUCHBAR_Main( work->cmn_wk->mtb_wk );

    if( work->input_state == INPUT_STATE_NONE )
    {
      TOUCHBAR_ICON icon = MANUAL_TOUCHBAR_GetTouch( work->cmn_wk->mtb_wk );
      if( icon != TOUCHBAR_SELECT_NONE )
      {
        work->input_state = INPUT_STATE_TB;

        if( icon == TOUCHBAR_ICON_CUR_L )
        {
          if( work->page_no > 0 )
          {
            work->page_no--;
            work->page_change_req = TRUE;
          }
        }
        else if( icon == TOUCHBAR_ICON_CUR_R )
        {
          if( work->page_no < work->param->page_num -1 )
          {
            work->page_no++;
            work->page_change_req = TRUE;
          }
        }
        if( work->page_change_req ) Manual_Explain_PageChange( work );
      }
    }
    else
    {
      TOUCHBAR_ICON icon = MANUAL_TOUCHBAR_GetTrg( work->cmn_wk->mtb_wk );
      if( icon != TOUCHBAR_SELECT_NONE )
      {
        switch( icon )
        {
        case TOUCHBAR_ICON_RETURN:
          {
            work->param->result = MANUAL_EXPLAIN_RESULT_RETURN;
            work->input_state = INPUT_STATE_END;
            //b_end = TRUE;
            work->end_req_count = END_REQ_COUNT_START_VAL;
          }
          break;
        case TOUCHBAR_ICON_CUR_L:
        case TOUCHBAR_ICON_CUR_R:
          {
            work->input_state = INPUT_STATE_NONE;
            if( work->page_change_req )
            {
              MANUAL_TOUCHBAR_SetPage( work->cmn_wk->mtb_wk, work->param->page_num, work->page_no );
              work->page_change_req = FALSE;
            }
          }
          break;
        }
      }
    }
  }

  return b_end;
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
static void Manual_Explain_VBlankFunc( GFL_TCB* tcb, void* wk )
{
  MANUAL_EXPLAIN_WORK* work = (MANUAL_EXPLAIN_WORK*)wk;
}

// ����
static void Manual_Explain_Prepare( MANUAL_EXPLAIN_WORK* work )
{
  // �T�uBG
  GFL_ARCHDL_UTIL_TransVramPalette(
      work->cmn_wk->handle_system,
      NARC_manual_bg_NCLR,
      PALTYPE_SUB_BG,
      BG_PAL_POS_S_REAR * 0x20,
      BG_PAL_NUM_S_REAR * 0x20,
      work->cmn_wk->heap_id );

  GFL_ARCHDL_UTIL_TransVramBgCharacter(
      work->cmn_wk->handle_system,
      NARC_manual_bg_NCGR,
      BG_FRAME_S_REAR,
			0,
      0,  // �S�]��
      FALSE,
      work->cmn_wk->heap_id );

/*
  GFL_ARCHDL_UTIL_TransVramScreen(
      work->cmn_wk->handle_system,
      NARC_manual_bg3_NSCR,
      BG_FRAME_S_REAR,
      0,
      0,  // �S�]��
      FALSE,
      work->cmn_wk->heap_id );
*/

  GFL_ARCHDL_UTIL_TransVramScreenCharOfsVBlank(
      work->cmn_wk->handle_system,
      NARC_manual_bg3_NSCR,
      BG_FRAME_S_REAR,
      0,
      0,
      0,  // �S�]��
      FALSE,
      work->cmn_wk->heap_id );
}
// �]��(VBlank�]���ł͂Ȃ�)
static void Manual_Explain_Trans( MANUAL_EXPLAIN_WORK* work )
{
  GFL_BG_LoadScreenReq( BG_FRAME_S_REAR );
}
// �]��(VBlank�]��)
static void Manual_Explain_TransVBlank( MANUAL_EXPLAIN_WORK* work )
{
  GFL_BG_LoadScreenV_Req( BG_FRAME_S_REAR );
}
// ��Еt��
static void Manual_Explain_Finish( MANUAL_EXPLAIN_WORK* work )
{
  // �������Ȃ�
}

// �e�L�X�g
static void Manual_Explain_TextInit( MANUAL_EXPLAIN_WORK* work )
{
  // bmpwin
  u8 i;
  for( i=0; i<TEXT_MAX; i++ )
  {
    work->text_bmpwin[i] = GFL_BMPWIN_Create(
                                     bmpwin_setup[i][0],
                                     bmpwin_setup[i][1], bmpwin_setup[i][2], bmpwin_setup[i][3], bmpwin_setup[i][4],
                                     bmpwin_setup[i][5], bmpwin_setup[i][6] );
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_bmpwin[i]), 0 );
    GFL_BMPWIN_TransVramCharacter( work->text_bmpwin[i] );
    work->text_trans[i] = FALSE;
  }

  // print_que
  work->print_que_text[TEXT_DUMMY]   = work->cmn_wk->print_que_title;
  work->print_que_text[TEXT_TITLE]   = work->cmn_wk->print_que_title;
  work->print_que_text[TEXT_EXPLAIN] = work->cmn_wk->print_que_main;

  // title
  {
    STRBUF* strbuf = GFL_MSG_CreateString( work->cmn_wk->msgdata_main, work->param->title_str_id );
    PRINTSYS_PrintQueColor(
        work->print_que_text[TEXT_TITLE],
        GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_TITLE]),
        bmpwin_setup[TEXT_TITLE][7], bmpwin_setup[TEXT_TITLE][8],
        strbuf,
        work->cmn_wk->font,
        PRINTSYS_LSB_Make(TEXT_TITLE_COLOR_L,TEXT_TITLE_COLOR_S,TEXT_TITLE_COLOR_B) );
    GFL_STR_DeleteBuffer( strbuf );
    work->text_trans[TEXT_TITLE] = TRUE;
    // ���ɍς�ł��邩������Ȃ��̂ŁAMain��1�x�Ă�ł���
    Manual_Explain_TextMain( work );
  }
}
static void Manual_Explain_TextExit( MANUAL_EXPLAIN_WORK* work )
{
  u8 i;
  for( i=0; i<TEXT_MAX; i++ )
  {
    work->text_trans[i] = FALSE;
    PRINTSYS_QUE_Clear( work->print_que_text[i] );
    GFL_BMPWIN_ClearScreen( work->text_bmpwin[i] );  // ����BG�t���[���̃X�N���[���͎��̉�ʂőS��ʂ��������킯�ł͂Ȃ��̂ŁA
                                                     // �S�~���c���Ȃ��悤�ɂ��Ă����BAPP_TASKMENU_WIN_Delete���Q�l�ɂ����B
    GFL_BMPWIN_Delete( work->text_bmpwin[i] );
  }

  GFL_BG_LoadScreenV_Req( BG_FRAME_S_MAIN );
}
static void Manual_Explain_TextMain( MANUAL_EXPLAIN_WORK* work )
{
  u8 i;
  for( i=0; i<TEXT_MAX; i++ )
  {
    if( work->text_trans[i] )
    {
      if( !PRINTSYS_QUE_IsExistTarget( work->print_que_text[i], GFL_BMPWIN_GetBmp(work->text_bmpwin[i]) ) )
      {
        GFL_BMPWIN_MakeTransWindow_VBlank( work->text_bmpwin[i] );
        work->text_trans[i] = FALSE;
      }
    }
  }
}

// �y�[�W����(���͂ƃe�L�X�g�̐���)
static void Manual_Explain_PageChange( MANUAL_EXPLAIN_WORK* work )
{
  // work->page_no�͊��ɂ��ꂩ�琶������y�[�W�ԍ��ɂȂ��Ă��� 

  u32  image_size = 256*192*2;
  u32  size;
  u16* buf;


  // �e�L�X�g

  // ���܂ŕ\�����Ă������̂��N���A����
  PRINTSYS_QUE_Clear( work->print_que_text[TEXT_EXPLAIN] );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_EXPLAIN]), 0 );

  {
    STRBUF* strbuf = GFL_MSG_CreateString( work->cmn_wk->msgdata_main, work->param->page[work->page_no].str_id );
    PRINTSYS_PrintQueColor(
        work->print_que_text[TEXT_EXPLAIN],
        GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_EXPLAIN]),
        bmpwin_setup[TEXT_EXPLAIN][7], bmpwin_setup[TEXT_EXPLAIN][8],
        strbuf,
        work->cmn_wk->font,
        PRINTSYS_LSB_Make(TEXT_EXPLAIN_COLOR_L,TEXT_EXPLAIN_COLOR_S,TEXT_EXPLAIN_COLOR_B) );
    GFL_STR_DeleteBuffer( strbuf );
    work->text_trans[TEXT_EXPLAIN] = TRUE;
    // ���ɍς�ł��邩������Ȃ��̂ŁAMain��1�x�Ă�ł���
    Manual_Explain_TextMain( work );
  }


  // �摜

  MANUAL_COMMON_DrawBgMDcbmp( work->cmn_wk, work->param->page[work->page_no].image );  // work->param->page[work->page_no].image�ɂ͉摜ID��MANUAL_BG_M_DCBMP_NO_IMAGE�������Ă���
}

