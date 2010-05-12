//============================================================================
/**
 *  @file   manual_list.c
 *  @brief  �Q�[�����}�j���A��
 *  @author Koji Kawada
 *  @data   2010.04.26
 *  @note   
 *  ���W���[�����FMANUAL_LIST
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
#include "app/app_taskmenu.h"

#include "manual_graphic.h"
#include "manual_def.h"
#include "manual_common.h"
#include "manual_touchbar.h"
#include "manual_list.h"

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
// APP_TASKMENU_WIN�̍���
typedef enum
{
          ATM_WIN_ITEM_UP_HALF,
  ATM_WIN_ITEM_ENABLE_START,     // ATM_WIN_ITEM_ENABLE_START<= <ATM_WIN_ITEM_ENABLE_END
          ATM_WIN_ITEM_0             = ATM_WIN_ITEM_ENABLE_START,
          ATM_WIN_ITEM_1,
          ATM_WIN_ITEM_2,
          ATM_WIN_ITEM_3,
          ATM_WIN_ITEM_4,
          ATM_WIN_ITEM_5,
  ATM_WIN_ITEM_ENABLE_END,
          ATM_WIN_ITEM_DOWN_HALF     = ATM_WIN_ITEM_ENABLE_END,
  ATM_WIN_ITEM_MAX,

  ATM_WIN_ITEM_ENABLE_NUM   = ATM_WIN_ITEM_ENABLE_END - ATM_WIN_ITEM_ENABLE_START,
}
ATM_WIN_ITEM;

#define ATM_PLATE_WIDTH (28)  // �L����
#define ATM_PLATE_X     (2)   // �L����
#define ATM_PLATE_Y     (0)   // �L����

#define SCROLL_Y  (APP_TASKMENU_PLATE_HEIGHT*8/2)

// ���͏��
typedef enum
{
  INPUT_STATE_NONE,  // ���̓��̓A�j���[�V�������s���Ă��Ȃ��̂ŁA���͉\�B
  INPUT_STATE_END,   // �������̓��͂��󂯕t���Ȃ��B
  INPUT_STATE_SB,    // �X�N���[���o�[�̓��͂��s���Ă���Œ��B
  INPUT_STATE_TB,    // �^�b�`�o�[�̓��̓A�j���[�V�������s���Ă���Œ��Ȃ̂ŁA���͕s�\�B
  INPUT_STATE_ATM,   // APP_TASKMENU_WIN�̓��̓A�j���[�V�������s���Ă���Œ��Ȃ̂ŁA���͕s�\�B
}
INPUT_STATE;

// OBJ
enum
{
  OBJ_LIST_RES_NCG,
  OBJ_LIST_RES_NCL,
  OBJ_LIST_RES_NCE,
  OBJ_RES_MAX,
};

// �X�N���[���o�[�̃��C���̃^�b�`�̈�A�X�N���[���o�[�̃J�[�\���̔z�u�͈�
#define SB_LINE_TOUCH_X_MIN  (240)  // MIN<= <=MAX
#define SB_LINE_TOUCH_X_MAX  (247)
#define SB_LINE_TOUCH_Y_MIN  ( 17)  // MIN<= <=MAX
#define SB_LINE_TOUCH_Y_MAX  (150)
#define SB_CURSOR_X          (240)
#define SB_CURSOR_Y_MIN      ( 17)  // MIN<= <=MAX
#define SB_CURSOR_Y_MAX      (135)
#define SB_CURSOR_Y_OFFSET   (  8)  // �J�[�\���̍��オ(0,0)�ƂȂ��Ă���̂ŁA�J�[�\���̒��S�ɂ���ɂ͂��ꂾ�����炷�K�v������B

// NEW�A�C�R��
#define NEW_ICON_X       (0)
#define NEW_ICON_Y       (-SCROLL_Y+4)
#define NEW_ICON_ANMSEQ  (1)


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// ���[�N
//=====================================
struct _MANUAL_LIST_WORK
{
  // �p�����[�^
  MANUAL_LIST_PARAM*          param;
  // ����
  MANUAL_COMMON_WORK*         cmn_wk;

  // �����ō쐬
  // APP_TASKMENU_WIN
  APP_TASKMENU_RES*           atm_res;
  APP_TASKMENU_WIN_WORK*      atm_win_wk[ATM_WIN_ITEM_MAX];  // NULL�̂Ƃ��Ȃ�
  BOOL                        atm_win_is_show[ATM_WIN_ITEM_MAX];  // TRUE�̂Ƃ��\�����Ă���AFALSE�̂Ƃ��\�����Ă��Ȃ�
                                                                  // atm_win_wk[i]��NULL�łȂ��Ƃ������L���Ȓl�������Ă���
  
  // OBJ
  u32                         obj_res[OBJ_RES_MAX];
  GFL_CLWK*                   sb_cursor_clwk;
  GFL_CLWK*                   new_icon_clwk[ATM_WIN_ITEM_MAX];

  // VBlank��TCB
  GFL_TCB*                    vblank_tcb;

  // ���͏��
  INPUT_STATE                 input_state;
  BOOL                        b_sb_touch;  // TRUE�̂Ƃ��X�N���[���o�[���^�b�`���Ă���
  BOOL                        b_sb_cursor_pos_change;  // TRUE�̂Ƃ��X�N���[���o�[���^�b�`�������߂�param->cursor_pos�̈ʒu���ς�����B
};


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// VBlank�֐�
static void Manual_List_VBlankFunc( GFL_TCB* tcb, void* wk );

// ����
static void Manual_List_Prepare( MANUAL_LIST_WORK* work );
static void Manual_List_PrepareAppTaskmenuWin( MANUAL_LIST_WORK* work );
// �]��(VBlank�]���ł͂Ȃ�)
static void Manual_List_Trans( MANUAL_LIST_WORK* work );
// ����
static BOOL Manual_List_InputAppTaskmenuWin( MANUAL_LIST_WORK* work );
static BOOL Manual_List_InputSb( MANUAL_LIST_WORK* work );

// �^�b�`�̓����蔻��e�[�u���̍쐬
static void Manual_List_MakeAtmTpHittbl( GFL_UI_TP_HITTBL* atm_tp_hittbl );  // atm_tp_hittbl[ATM_WIN_ITEM_ENABLE_NUM +1]

// MANUAL_LIST_PARAM��cursor_pos��ATM_WIN_ITEM�ɕϊ�����
static ATM_WIN_ITEM Manual_List_ConvertCursorPosToAtmWinItem( MANUAL_LIST_WORK* work );
// ��ʂ�1�ԍŌ�ɂ���S���ڂ̒��ł̔ԍ��𓾂�(��ʂ�1�ԉ�(DOWN_HALF�͏���)����X�ɑ����ꍇ�������͂��傤�ǂ����ɓ͂��Ă���ꍇ�́A
// ��ʂ�1�ԉ�(DOWN_HALF�͏���)�ɂ��鍀�ڂ̔ԍ��ɂȂ�B�����ɓ͂��Ă��Ȃ��ꍇ�́Awork->param->num-1�ɂȂ肻�̏ꏊ�͉�ʂ�1�ԉ�����ł���B)
static u16 Manual_List_GetTailPos( MANUAL_LIST_WORK* work );
// ��ʂɏo�Ă���I���ł���p�l���̌�
static u16 Manual_List_GetEnableNum( MANUAL_LIST_WORK* work );
// head_pos�̎�邱�Ƃ��ł���ő�l(�܂ށA0���炱�̒l�܂ŕύX����OK)�𓾂�
static u16 Manual_List_GetHeadPosMax( MANUAL_LIST_WORK* work );

// ���X�g���������̂ł��蒼��
static void Manual_List_UpdateAppTaskmenuWin( MANUAL_LIST_WORK* work );

// �^�b�`���Ă���y���W����head_pos�����߂�
static u16 Manual_List_GetHeadPosFromY( MANUAL_LIST_WORK* work, u16 num_for_head_pos, u16 y );
// head_pos����X�N���[���o�[�̃J�[�\����u��y���W�����߂�(SB_CURSOR_Y_OFFSET���l�����Ă���̂ŁA�߂�l��y�����̂܂�clwk�Ɏg���Ă��΂悢)
static u16 Manual_List_GetSbCursorYFromHeadPos( MANUAL_LIST_WORK* work, u16 num_for_head_pos, u16 head_pos );


//=============================================================================
/**
*  �O�����J�֐���`
*/
//=============================================================================
// ����������
MANUAL_LIST_WORK*  MANUAL_LIST_Init(
    MANUAL_LIST_PARAM*   param,
    MANUAL_COMMON_WORK*  cmn_wk
)
{
  // ���[�N
  MANUAL_LIST_WORK*  work  = GFL_HEAP_AllocClearMemory( cmn_wk->heap_id, sizeof(MANUAL_LIST_WORK) );

  // �p�����[�^
  work->param  = param;
  {
    // out
    work->param->result = MANUAL_LIST_RESULT_RETURN;
  }
  // ����
  work->cmn_wk = cmn_wk;

  // 0, NULL�ŏ�����
  {
    u8 i;
    for( i=0; i<ATM_WIN_ITEM_MAX; i++ )
    {
      work->atm_win_wk[i] = NULL;
      work->atm_win_is_show[i] = FALSE;
    }
  }

  // �����ō쐬
  Manual_List_Prepare( work );
  Manual_List_Trans( work );

  // �X�N���[��
  GFL_BG_SetScrollReq( BG_FRAME_S_MAIN, GFL_BG_SCROLL_Y_SET, SCROLL_Y );

  // �X�N���[���o�[�̃J�[�\����\�����邩�A��\���ɂ��邩
  {
    GFL_CLACT_WK_SetDrawEnable( work->sb_cursor_clwk, ( work->param->num > ATM_WIN_ITEM_ENABLE_NUM ) );
  }

  // �X�N���[���o�[�̃J�[�\���̈ʒu
  {
    GFL_CLACTPOS pos;
    u16 y = Manual_List_GetSbCursorYFromHeadPos( work, Manual_List_GetHeadPosMax(work)+1, work->param->head_pos );
    pos.x = SB_CURSOR_X;
    pos.y = y; 
    GFL_CLACT_WK_SetPos( work->sb_cursor_clwk, &pos, CLSYS_DEFREND_SUB );
  }

  // VBlank��TCB
  work->vblank_tcb = GFUser_VIntr_CreateTCB( Manual_List_VBlankFunc, work, 1 );

  // ���͏��
  work->input_state = INPUT_STATE_NONE;
  work->b_sb_touch = FALSE;
  work->b_sb_cursor_pos_change = FALSE;
  
  // �}�j���A���^�b�`�o�[
  {
    MANUAL_TOUCHBAR_TYPE t = MANUAL_TOUCHBAR_TYPE_TITLE;
    if( work->param->type == MANUAL_LIST_TYPE_CATEGORY )   t = MANUAL_TOUCHBAR_TYPE_CATEGORY;
    else if( work->param->type == MANUAL_LIST_TYPE_TITLE ) t = MANUAL_TOUCHBAR_TYPE_TITLE;
    MANUAL_TOUCHBAR_SetType( work->cmn_wk->mtb_wk, t );
  }

  return work;
}

// �I������
void  MANUAL_LIST_Exit(
    MANUAL_LIST_WORK*     work
)
{
  // VBlank��TCB
  GFL_TCB_DeleteTask( work->vblank_tcb );

  // �X�N���[��
  GFL_BG_SetScrollReq( BG_FRAME_S_MAIN, GFL_BG_SCROLL_Y_SET, 0 );

  // �����ō쐬
  // APP_TASKMENU_WIN
  {
    u8 i;
    // ���[�N
    for( i=0; i<ATM_WIN_ITEM_MAX; i++ )
    {
      if( work->atm_win_wk[i] ) APP_TASKMENU_WIN_Delete( work->atm_win_wk[i] );
    }
    // ���\�[�X
    APP_TASKMENU_RES_Delete( work->atm_res );
  }

  // OBJ
  {
    // clwk
    // new_icon_clwk
    {
      u8 i;
      for( i=0; i<ATM_WIN_ITEM_MAX; i++ )
      {
        GFL_CLACT_WK_Remove( work->new_icon_clwk[i] );
      }
    }
    // sb_cursor_clwk
    {
      GFL_CLACT_WK_Remove( work->sb_cursor_clwk );
    }
    // ���\�[�X
    GFL_CLGRP_CELLANIM_Release( work->obj_res[OBJ_LIST_RES_NCE] );
    GFL_CLGRP_CGR_Release( work->obj_res[OBJ_LIST_RES_NCG] );
    GFL_CLGRP_PLTT_Release( work->obj_res[OBJ_LIST_RES_NCL] );
  }

  // ���[�N
  GFL_HEAP_FreeMemory( work );
}

// �又��
BOOL  MANUAL_LIST_Main(
    MANUAL_LIST_WORK*     work
)
{
  BOOL b_end = FALSE;

  {
    // APP_TASKMENU_WIN
    u8 i;
    for( i=0; i<ATM_WIN_ITEM_MAX; i++ )
    {
      if( work->atm_win_wk[i] && work->atm_win_is_show[i] ) APP_TASKMENU_WIN_Update( work->atm_win_wk[i] );
    }
  }

  // �X�N���[���o�[�̓��͂𒲂ׂ�
  if(    work->input_state == INPUT_STATE_NONE
      || work->input_state == INPUT_STATE_SB )
  {
    // �X�N���[���o�[
    BOOL b_sb = Manual_List_InputSb( work );
    if( b_sb ) work->input_state = INPUT_STATE_SB;
    else       work->input_state = INPUT_STATE_NONE;
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
            work->param->result = MANUAL_LIST_RESULT_RETURN;
          }
          break;
        }
        work->input_state = INPUT_STATE_END;
        b_end = TRUE;
      }
    }
  }

  // APP_TASKMENU_WIN�̓��͂𒲂ׂ�
  if(    work->input_state == INPUT_STATE_NONE
      || work->input_state == INPUT_STATE_ATM )
  {
/*
�`����X�V���邾���Ȃ̂ŁA���t���[��APP_TASKMENU_WIN_Update���Ăяo���Ă����v�Ȃ͂�������A�O�ɏo�����B
    // APP_TASKMENU_WIN
    u8 i;
    for( i=0; i<ATM_WIN_ITEM_MAX; i++ )
    {
      if( work->atm_win_wk[i] && work->atm_win_is_show[i] ) APP_TASKMENU_WIN_Update( work->atm_win_wk[i] );
    }
 */

    if( work->input_state == INPUT_STATE_NONE )
    {
      BOOL b_decide = Manual_List_InputAppTaskmenuWin( work );
      if( b_decide ) work->input_state = INPUT_STATE_ATM;
    }
    else
    {
      BOOL b_finish = APP_TASKMENU_WIN_IsFinish( work->atm_win_wk[Manual_List_ConvertCursorPosToAtmWinItem(work)] );
      if( b_finish )
      {
        work->param->result = MANUAL_LIST_RESULT_ITEM;
        work->input_state = INPUT_STATE_END;
        b_end = TRUE;
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
static void Manual_List_VBlankFunc( GFL_TCB* tcb, void* wk )
{
  MANUAL_LIST_WORK* work = (MANUAL_LIST_WORK*)wk;

  // VBlank�]������Ȃ炱���ŁB
  //Manual_List_Trans( work );
}

// ����
static void Manual_List_Prepare( MANUAL_LIST_WORK* work )
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

  GFL_ARCHDL_UTIL_TransVramScreen(
      work->cmn_wk->handle_system,
      NARC_manual_bg2_NSCR,
      BG_FRAME_S_REAR,
      0,
      0,  // �S�]��
      FALSE,
      work->cmn_wk->heap_id );

  // OBJ
  {
    // ���\�[�X
    work->obj_res[OBJ_LIST_RES_NCL] = GFL_CLGRP_PLTT_RegisterEx(
        work->cmn_wk->handle_system,
        NARC_manual_obj_NCLR,
        CLSYS_DRAW_SUB,
        OBJ_PAL_POS_S_LIST * 0x20,
        0,
        OBJ_PAL_NUM_S_LIST,
        work->cmn_wk->heap_id );
    work->obj_res[OBJ_LIST_RES_NCG] = GFL_CLGRP_CGR_Register(
        work->cmn_wk->handle_system,
        NARC_manual_obj_NCGR,
        FALSE,
        CLSYS_DRAW_SUB,
        work->cmn_wk->heap_id );
    work->obj_res[OBJ_LIST_RES_NCE] = GFL_CLGRP_CELLANIM_Register(
        work->cmn_wk->handle_system,
        NARC_manual_obj_NCER,
        NARC_manual_obj_NANR,
        work->cmn_wk->heap_id );

    // clwk
    // sb_cursor_clwk
    {
      GFL_CLWK_DATA data =
      {
        SB_CURSOR_X, SB_CURSOR_Y_MIN,
        0,
        0,
        BG_FRAME_PRI_S_REAR
      };
      work->sb_cursor_clwk = GFL_CLACT_WK_Create(
          MANUAL_GRAPHIC_GetClunit(work->cmn_wk->graphic),
          work->obj_res[OBJ_LIST_RES_NCG],
          work->obj_res[OBJ_LIST_RES_NCL],
          work->obj_res[OBJ_LIST_RES_NCE],
          &data,
          CLSYS_DEFREND_SUB,
          work->cmn_wk->heap_id );
      GFL_CLACT_WK_SetDrawEnable( work->sb_cursor_clwk, FALSE );
      GFL_CLACT_WK_SetAutoAnmFlag( work->sb_cursor_clwk, TRUE );
    }

    // new_icon_clwk
    {
      GFL_CLWK_DATA data =
      {
        NEW_ICON_X, NEW_ICON_Y,
        NEW_ICON_ANMSEQ,
        0,
        BG_FRAME_PRI_S_MAIN
      };
      u8 i;
      for( i=0; i<ATM_WIN_ITEM_MAX; i++ )
      {
        data.pos_y = NEW_ICON_Y + APP_TASKMENU_PLATE_HEIGHT*i *8;
        work->new_icon_clwk[i] = GFL_CLACT_WK_Create(
            MANUAL_GRAPHIC_GetClunit(work->cmn_wk->graphic),
            work->obj_res[OBJ_LIST_RES_NCG],
            work->obj_res[OBJ_LIST_RES_NCL],
            work->obj_res[OBJ_LIST_RES_NCE],
            &data,
            CLSYS_DEFREND_SUB,
            work->cmn_wk->heap_id );
        GFL_CLACT_WK_SetDrawEnable( work->new_icon_clwk[i], FALSE );
        GFL_CLACT_WK_SetAutoAnmFlag( work->new_icon_clwk[i], TRUE );
      }
    }
  }

  // APP_TASKMENU_WIN
  Manual_List_PrepareAppTaskmenuWin( work );
}

static void Manual_List_PrepareAppTaskmenuWin( MANUAL_LIST_WORK* work )
{
  // APP_TASKMENU_WIN
  // ���\�[�X
  {
    work->atm_res = APP_TASKMENU_RES_Create(
        BG_FRAME_S_MAIN,
        BG_PAL_POS_S_ATM,
        work->cmn_wk->font,
        work->cmn_wk->print_que_main,
        work->cmn_wk->heap_id );
  }

  // ���X�g�𓮂������ۂɌĂяo���֐��𗘗p���āA�ŏ��̕\�����쐬����
  Manual_List_UpdateAppTaskmenuWin(work);

  // �J�[�\���̕\��
  if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_KEY )
  {
    APP_TASKMENU_WIN_SetActive( work->atm_win_wk[Manual_List_ConvertCursorPosToAtmWinItem(work)], TRUE );
    work->b_sb_cursor_pos_change = FALSE;
  }
}

// �]��(VBlank�]���ł͂Ȃ�)
static void Manual_List_Trans( MANUAL_LIST_WORK* work )
{
  GFL_BG_LoadScreenReq( BG_FRAME_S_REAR );
}

// ����
static BOOL Manual_List_InputAppTaskmenuWin( MANUAL_LIST_WORK* work )
{
  // ���肵���Ƃ�TRUE��Ԃ�

  int tp_hit;
  int enable_num;
  u16 tail_pos;

  GFL_UI_TP_HITTBL  atm_tp_hittbl[ATM_WIN_ITEM_ENABLE_NUM +1];
  Manual_List_MakeAtmTpHittbl(atm_tp_hittbl);

  tp_hit = GFL_UI_TP_HitTrg(atm_tp_hittbl);

  enable_num = Manual_List_GetEnableNum(work);
  tail_pos   = Manual_List_GetTailPos(work);

  // �^�b�`or�L�[�̐؂�ւ��`�F�b�N
  // �L�[���͂̂Ƃ�
  if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_KEY )
  {
    if( 0<=tp_hit && tp_hit<enable_num )
    {
      // �^�b�`�ɕύX
      GFL_UI_SetTouchOrKey(GFL_APP_END_TOUCH);
      // �^�b�`�ɕύX�����ꍇ�́A���̂܂܏����𑱂���
      APP_TASKMENU_WIN_SetActive( work->atm_win_wk[Manual_List_ConvertCursorPosToAtmWinItem(work)], FALSE );
    }
  }
  // �^�b�`���͂̂Ƃ�
  else
  {
    if( GFL_UI_KEY_GetTrg() )
    {
      if( !( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B ) )  // �^�b�`�o�[�ւ̓��͈ȊO�Ȃ�
      {
        // �L�[�ɕύX
        GFL_UI_SetTouchOrKey(GFL_APP_END_KEY);
        // �L�[�ɕύX�����ꍇ�́A�J�[�\����\�����āA�������I����
        APP_TASKMENU_WIN_SetActive( work->atm_win_wk[Manual_List_ConvertCursorPosToAtmWinItem(work)], TRUE );
        work->b_sb_cursor_pos_change = FALSE;
        PMSND_PlaySE(MANUAL_SND_ATM_MOVE);
        return FALSE;
      }
    }
  }

  // �L�[���͎󂯕t��
  if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_KEY )
  {
    // �L�[�̂܂ܕύX�Ȃ��Ȃ̂ŁAGFL_UI_SetTouchOrKey�͕s�v�B
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
    {
      // ����
      APP_TASKMENU_WIN_SetDecide( work->atm_win_wk[Manual_List_ConvertCursorPosToAtmWinItem(work)], TRUE );
      PMSND_PlaySE(MANUAL_SND_ATM_DECIDE);
      return TRUE;
    }
    if( work->param->num > 1 )
    {
      if( GFL_UI_KEY_GetRepeat() & PAD_KEY_UP )
      {
        APP_TASKMENU_WIN_SetActive( work->atm_win_wk[Manual_List_ConvertCursorPosToAtmWinItem(work)], FALSE );
        
        if( work->param->cursor_pos > work->param->head_pos )  // Active�̈ړ������ōςޏꍇ
        {
          work->param->cursor_pos--;
        }
        else
        {
          if( work->param->num <= ATM_WIN_ITEM_ENABLE_NUM )  // Active�̈ړ������ōςޏꍇ  // �Ō���փ��[�v
          {
            work->param->cursor_pos = work->param->num -1;
          }
          else  // ���X�g�𓮂����K�v������Ƃ�
          {
            if( work->param->cursor_pos > 0 )
            {
              work->param->head_pos--;
              work->param->cursor_pos--;
            }
            else  // �Ō���փ��[�v
            {
              work->param->head_pos = Manual_List_GetHeadPosMax(work);
              work->param->cursor_pos = work->param->num -1;
            }
            Manual_List_UpdateAppTaskmenuWin(work);

            // �X�N���[���o�[�̃J�[�\���̈ʒu
            {
              GFL_CLACTPOS pos;
              u16 y = Manual_List_GetSbCursorYFromHeadPos( work, Manual_List_GetHeadPosMax(work)+1, work->param->head_pos );
              pos.x = SB_CURSOR_X;
              pos.y = y; 
              GFL_CLACT_WK_SetPos( work->sb_cursor_clwk, &pos, CLSYS_DEFREND_SUB );
            }
          }
        }

        APP_TASKMENU_WIN_SetActive( work->atm_win_wk[Manual_List_ConvertCursorPosToAtmWinItem(work)], TRUE );
        work->b_sb_cursor_pos_change = FALSE;
        PMSND_PlaySE(MANUAL_SND_ATM_MOVE);
      }
      else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN )
      {
        APP_TASKMENU_WIN_SetActive( work->atm_win_wk[Manual_List_ConvertCursorPosToAtmWinItem(work)], FALSE );

        if( work->param->cursor_pos < tail_pos )  // Active�̈ړ������ōςޏꍇ
        {
          work->param->cursor_pos++;
        }
        else
        {
          if( work->param->num <= ATM_WIN_ITEM_ENABLE_NUM )  // Active�̈ړ������ōςޏꍇ  // �擪�փ��[�v
          {
            work->param->cursor_pos = 0;
          }
          else  // ���X�g�𓮂����K�v������Ƃ�
          {
            if( work->param->cursor_pos < work->param->num -1 )
            {
              work->param->head_pos++;
              work->param->cursor_pos++;
            }
            else  // �擪�փ��[�v
            {
              work->param->head_pos = 0;
              work->param->cursor_pos = 0;
            }
            Manual_List_UpdateAppTaskmenuWin(work);

            // �X�N���[���o�[�̃J�[�\���̈ʒu
            {
              GFL_CLACTPOS pos;
              u16 y = Manual_List_GetSbCursorYFromHeadPos( work, Manual_List_GetHeadPosMax(work)+1, work->param->head_pos );
              pos.x = SB_CURSOR_X;
              pos.y = y; 
              GFL_CLACT_WK_SetPos( work->sb_cursor_clwk, &pos, CLSYS_DEFREND_SUB );
            }
          }
        }

        APP_TASKMENU_WIN_SetActive( work->atm_win_wk[Manual_List_ConvertCursorPosToAtmWinItem(work)], TRUE );
        work->b_sb_cursor_pos_change = FALSE;
        PMSND_PlaySE(MANUAL_SND_ATM_MOVE);
      }
      else if( GFL_UI_KEY_GetRepeat() & ( PAD_KEY_LEFT | PAD_BUTTON_L ) )
      {

      }
      else if( GFL_UI_KEY_GetRepeat() & ( PAD_KEY_RIGHT | PAD_BUTTON_R ) )
      {

      }
    }
  }
  // �^�b�`���͎󂯕t��
  else
  {
    // �^�b�`�̂܂ܕύX�Ȃ��Ȃ̂ŁAGFL_UI_SetTouchOrKey�͕s�v�B
    if( 0<=tp_hit && tp_hit<enable_num )
    {
      // ����
      work->param->cursor_pos = work->param->head_pos + tp_hit;
      APP_TASKMENU_WIN_SetActive( work->atm_win_wk[Manual_List_ConvertCursorPosToAtmWinItem(work)], TRUE );
      work->b_sb_cursor_pos_change = FALSE;
      APP_TASKMENU_WIN_SetDecide( work->atm_win_wk[Manual_List_ConvertCursorPosToAtmWinItem(work)], TRUE );
      PMSND_PlaySE(MANUAL_SND_ATM_DECIDE);
      return TRUE;
    }
  }

  return FALSE;
}

static BOOL Manual_List_InputSb( MANUAL_LIST_WORK* work )
{
  // �߂�l(���̓��͂��󂯕t���Ă͂Ȃ�Ȃ��Ƃ�TRUE��Ԃ�)
  // �^�b�`�����^�b�`��                          TRUE
  // �^�b�`�����^�b�`���Ă��Ȃ��Ƃ�              TRUE
  // �^�b�`���Ă��Ȃ��Ƃ����^�b�`��              TRUE
  // �^�b�`���Ă��Ȃ��Ƃ����^�b�`���Ă��Ȃ��Ƃ�  FALSE
  // �X�N���[���o�[�̃J�[�\������\���̂Ƃ�      FALSE


  // �X�N���[���o�[�̃J�[�\������\���̂Ƃ�
  if( !GFL_CLACT_WK_GetDrawEnable( work->sb_cursor_clwk ) )
  {
    return FALSE;
  }

  if( work->b_sb_touch )
  {
    // �^�b�`��
    u32 x, y;  
    if( GFL_UI_TP_GetPointCont( &x, &y ) )
    {
      GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
      APP_TASKMENU_WIN_SetActive( work->atm_win_wk[Manual_List_ConvertCursorPosToAtmWinItem(work)], FALSE );
      
      // �X�N���[���o�[�̃J�[�\���̈ʒu
      {
        GFL_CLACTPOS pos;
        pos.x = SB_CURSOR_X;
        pos.y = (y >= SB_CURSOR_Y_OFFSET)?(y - SB_CURSOR_Y_OFFSET):(0);
        if( pos.y < SB_CURSOR_Y_MIN )      pos.y = SB_CURSOR_Y_MIN;
        else if( pos.y > SB_CURSOR_Y_MAX ) pos.y = SB_CURSOR_Y_MAX;
        GFL_CLACT_WK_SetPos( work->sb_cursor_clwk, &pos, CLSYS_DEFREND_SUB );
      }

      {
        u16 head_pos_new = Manual_List_GetHeadPosFromY( work, Manual_List_GetHeadPosMax(work)+1, (u16)y );
        if( head_pos_new != work->param->head_pos )  // ���X�g�𓮂����K�v������Ƃ�
        {
          u16 tail_pos;
          work->param->head_pos = head_pos_new;
          tail_pos = Manual_List_GetTailPos( work );
          if( work->b_sb_cursor_pos_change || work->param->cursor_pos<work->param->head_pos || tail_pos<work->param->cursor_pos )
          {  // �X�N���[���o�[���^�b�`����param->cursor_pos��ύX���Ă���Ȃ�A�p�����ĕύX�����Đ擪��I�΂���悤�ɂ���B
            work->param->cursor_pos = work->param->head_pos;
            work->b_sb_cursor_pos_change = TRUE;
          }

          Manual_List_UpdateAppTaskmenuWin(work);

          PMSND_PlaySE( MANUAL_SND_SB_MOVE );
        }
      }
    }
    else
    {
      work->b_sb_touch = FALSE;
    }

    return TRUE; 
  }
  else
  {
    // �^�b�`���Ă��Ȃ��Ƃ�
    u32 x, y;  
    if( GFL_UI_TP_GetPointTrg( &x, &y ) )
    {
      if(    SB_LINE_TOUCH_X_MIN<=x && x<=SB_LINE_TOUCH_X_MAX
          && SB_LINE_TOUCH_Y_MIN<=y && y<=SB_LINE_TOUCH_Y_MAX )
      {
        GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
        APP_TASKMENU_WIN_SetActive( work->atm_win_wk[Manual_List_ConvertCursorPosToAtmWinItem(work)], FALSE );
        
        PMSND_PlaySE( MANUAL_SND_SB_MOVE );
        work->b_sb_touch = TRUE;

        // �X�N���[���o�[�̃J�[�\���̈ʒu
        {
          GFL_CLACTPOS pos;
          pos.x = SB_CURSOR_X;
          pos.y = (y >= SB_CURSOR_Y_OFFSET)?(y - SB_CURSOR_Y_OFFSET):(0);
          if( pos.y < SB_CURSOR_Y_MIN )      pos.y = SB_CURSOR_Y_MIN;
          else if( pos.y > SB_CURSOR_Y_MAX ) pos.y = SB_CURSOR_Y_MAX;
          GFL_CLACT_WK_SetPos( work->sb_cursor_clwk, &pos, CLSYS_DEFREND_SUB );
        }

        {
          u16 head_pos_new = Manual_List_GetHeadPosFromY( work, Manual_List_GetHeadPosMax(work)+1, (u16)y );
          if( head_pos_new != work->param->head_pos )  // ���X�g�𓮂����K�v������Ƃ�
          {
            u16 tail_pos;
            work->param->head_pos = head_pos_new;
            tail_pos = Manual_List_GetTailPos( work );
            if( work->b_sb_cursor_pos_change || work->param->cursor_pos<work->param->head_pos || tail_pos<work->param->cursor_pos )
            {  // �X�N���[���o�[���^�b�`����param->cursor_pos��ύX���Ă���Ȃ�A�p�����ĕύX�����Đ擪��I�΂���悤�ɂ���B
              work->param->cursor_pos = work->param->head_pos;
              work->b_sb_cursor_pos_change = TRUE;
            }
            Manual_List_UpdateAppTaskmenuWin(work);
          }
        }
      }
    }
    if( work->b_sb_touch ) return TRUE;
    else                   return FALSE;
  }
}

// �^�b�`�̓����蔻��e�[�u���̍쐬
static void Manual_List_MakeAtmTpHittbl( GFL_UI_TP_HITTBL* atm_tp_hittbl )  // atm_tp_hittbl[ATM_WIN_ITEM_ENABLE_NUM +1]
{
  u8 i;
  for( i=0; i<ATM_WIN_ITEM_ENABLE_NUM; i++ )
  {
    atm_tp_hittbl[i].rect.top    = SCROLL_Y + ( ATM_PLATE_Y + APP_TASKMENU_PLATE_HEIGHT*(i+0) ) *8; 
    atm_tp_hittbl[i].rect.bottom = SCROLL_Y + ( ATM_PLATE_Y + APP_TASKMENU_PLATE_HEIGHT*(i+1) ) *8 -1;
    atm_tp_hittbl[i].rect.left   = ( ATM_PLATE_X ) *8;
    atm_tp_hittbl[i].rect.right  = ( ATM_PLATE_X + ATM_PLATE_WIDTH ) *8 -1;
  }
  atm_tp_hittbl[ATM_WIN_ITEM_ENABLE_NUM].rect.top    = GFL_UI_TP_HIT_END;
  atm_tp_hittbl[ATM_WIN_ITEM_ENABLE_NUM].rect.bottom = 0;
  atm_tp_hittbl[ATM_WIN_ITEM_ENABLE_NUM].rect.left   = 0;
  atm_tp_hittbl[ATM_WIN_ITEM_ENABLE_NUM].rect.right  = 0;
}

// MANUAL_LIST_PARAM��cursor_pos��ATM_WIN_ITEM�ɕϊ�����
static ATM_WIN_ITEM Manual_List_ConvertCursorPosToAtmWinItem( MANUAL_LIST_WORK* work )
{
  u16 item = work->param->cursor_pos - work->param->head_pos + ATM_WIN_ITEM_ENABLE_START;
  return item;
}
// ��ʂ�1�ԍŌ�ɂ���S���ڂ̒��ł̔ԍ��𓾂�(��ʂ�1�ԉ�(DOWN_HALF�͏���)����X�ɑ����ꍇ�������͂��傤�ǂ����ɓ͂��Ă���ꍇ�́A
// ��ʂ�1�ԉ�(DOWN_HALF�͏���)�ɂ��鍀�ڂ̔ԍ��ɂȂ�B�����ɓ͂��Ă��Ȃ��ꍇ�́Awork->param->num-1�ɂȂ肻�̏ꏊ�͉�ʂ�1�ԉ�����ł���B)
static u16 Manual_List_GetTailPos( MANUAL_LIST_WORK* work )
{
  u16 tail_pos = work->param->head_pos + ATM_WIN_ITEM_ENABLE_NUM -1;
  if( tail_pos >= work->param->num ) tail_pos = work->param->num -1;
  return tail_pos;
}
// ��ʂɏo�Ă���I���ł���p�l���̌�
static u16 Manual_List_GetEnableNum( MANUAL_LIST_WORK* work )
{
  return Manual_List_GetTailPos(work) - work->param->head_pos +1;
}
// head_pos�̎�邱�Ƃ��ł���S���ڂ̒��ł̔ԍ��̍ő�l(�܂ށA0���炱�̒l�܂ŕύX����OK)�𓾂�
static u16 Manual_List_GetHeadPosMax( MANUAL_LIST_WORK* work )
{
  if( work->param->num <= ATM_WIN_ITEM_ENABLE_NUM )
  {
    return 0;
  }
  else
  {
    return (work->param->num - ATM_WIN_ITEM_ENABLE_NUM);
  }
}

// ���X�g���������̂ł��蒼��
static void Manual_List_UpdateAppTaskmenuWin( MANUAL_LIST_WORK* work )
{
  u8  atm_win_start_idx = ATM_WIN_ITEM_UP_HALF;
  u16 item_start_idx    = work->param->head_pos -1;

  u8  atm_win_end_idx;  // �܂�
  u16 item_end_idx;     // �܂�

  // �擪�̒���
  if( work->param->head_pos == 0 )
  {
    if( work->atm_win_wk[ATM_WIN_ITEM_UP_HALF] )
    {
      // UP_HALF���\���ɂ���
      APP_TASKMENU_WIN_Hide(work->atm_win_wk[ATM_WIN_ITEM_UP_HALF]);
      work->atm_win_is_show[ATM_WIN_ITEM_UP_HALF] = FALSE;
    }
    atm_win_start_idx = ATM_WIN_ITEM_ENABLE_START;
    item_start_idx    = work->param->head_pos;
      
    // NEW�A�C�R��
    GFL_CLACT_WK_SetDrawEnable( work->new_icon_clwk[ATM_WIN_ITEM_UP_HALF], FALSE );
  }

  // �Ō���̒���
  {
    u8 i;

    item_end_idx = Manual_List_GetTailPos(work) +1;
    if( item_end_idx >= work->param->num ) item_end_idx--;

    atm_win_end_idx = atm_win_start_idx + item_end_idx - item_start_idx;

    for( i=atm_win_end_idx +1; i<=ATM_WIN_ITEM_DOWN_HALF; i++ )
    {
      if( work->atm_win_wk[i] )
      {
        // �Ō���̃f�[�^�����͔�\���ɂ���
        APP_TASKMENU_WIN_Hide(work->atm_win_wk[i]);
        work->atm_win_is_show[i] = FALSE;
      }

      // NEW�A�C�R��
      GFL_CLACT_WK_SetDrawEnable( work->new_icon_clwk[i], FALSE );
    }
  }

  // ���[�N
  {
    u8  i;
    u16 item_idx = item_start_idx;
    
    APP_TASKMENU_ITEMWORK atm_item_wk;

    for( i=atm_win_start_idx; i<=atm_win_end_idx; i++ )
    {
      if( work->atm_win_wk[i] ) APP_TASKMENU_WIN_Delete( work->atm_win_wk[i] );

      // ���̐ݒ�
      atm_item_wk.str       = GFL_MSG_CreateString( work->cmn_wk->msgdata_main, work->param->item[item_idx].str_id );
      atm_item_wk.msgColor  = APP_TASKMENU_ITEM_MSGCOLOR;
      atm_item_wk.type      = APP_TASKMENU_WIN_TYPE_NORMAL;

      work->atm_win_wk[i] = APP_TASKMENU_WIN_Create(
          work->atm_res,
          &atm_item_wk,
          ATM_PLATE_X, ATM_PLATE_Y + APP_TASKMENU_PLATE_HEIGHT*i,
          ATM_PLATE_WIDTH,
          work->cmn_wk->heap_id );
      APP_TASKMENU_WIN_Update( work->atm_win_wk[i] );  // print_que�`������邽�߂�APP_TASKMENU_WIN_Create�����1��APP_TASKMENU_WIN_Update���Ă�ł���
      work->atm_win_is_show[i] = TRUE;

      // ��������
      GFL_STR_DeleteBuffer( atm_item_wk.str );

      // NEW�A�C�R��
      {
        if( work->param->item[item_idx].icon == MANUAL_LIST_ICON_NONE )
        {
          GFL_CLACT_WK_SetDrawEnable( work->new_icon_clwk[i], FALSE );
        }
        else if( work->param->item[item_idx].icon == MANUAL_LIST_ICON_NEW )
        {
          //GFL_CLACT_WK_SetAnmSeq( work->new_icon_clwk[i], NEW_ICON_ANMSEQ );  // 0�t���[���ڂ���Đ������悤�ɁA�A�j����ݒ肵�����B
                                                                                // �X�N���[�����邾���Ȃ̂ŁA�����̃t���[������ł悢����R�����g�A�E�g�B
          GFL_CLACT_WK_SetDrawEnable( work->new_icon_clwk[i], TRUE );
        }
      }

      item_idx++; 
    } 
  }
}

// �^�b�`���Ă���y���W����head_pos�����߂�
static u16 Manual_List_GetHeadPosFromY( MANUAL_LIST_WORK* work, u16 num_for_head_pos, u16 y )
{
  // ��ʂ�1�ԏ�(UP_HALF�͏���)�ɂ��鍀�ڂ̔ԍ�head_pos��Ԃ��B
  // num_for_head_pos�́A��ʂ�1�ԏ�(UP_HALF�͏���)�ɗ��邱�Ƃ��ł��鍀�ڂ̌��B
  // y�̓^�b�`���Ă�����W�B

  u16 head_pos;

  if( num_for_head_pos <= 1 )
  {
    head_pos = 0;
  }
  else
  {
    u16 range_min;  // range_min<= <range_max
    u16 range_max;
    u16 one_range;  // 1�̃p�l�����ɑ���������̕�

    range_min = SB_CURSOR_Y_MIN + SB_CURSOR_Y_OFFSET;
    range_max = SB_CURSOR_Y_MAX + SB_CURSOR_Y_OFFSET +1;

    one_range = ( range_max - range_min ) / num_for_head_pos;

    // y���͈͊O�ɂ���Ƃ�
    if( y<range_min )
    {
      head_pos = 0;
    }
    else if( range_max<=y )
    {
      head_pos = num_for_head_pos -1;
    }
    // y���͈͓��ɂ���Ƃ�
    else
    {
      // y�͉��Ԗڂ̋��ɓ����Ă��邩
      for( head_pos=0; head_pos<num_for_head_pos; head_pos++ )
      {
        //u16 curr_min = range_min + one_range * (head_pos +0);  // curr_min<= <curr_max 
        //u16 curr_max = range_min + one_range * (head_pos +1);
        u16 curr_min = range_min + ( range_max - range_min ) * (head_pos +0) / num_for_head_pos;  // �v�Z�̏��Ԃ����ւ��������Ȃ̂ŁA
        u16 curr_max = range_min + ( range_max - range_min ) * (head_pos +1) / num_for_head_pos;  // ��̃R�����g�A�E�g�Ɠ����Ӗ��ł��B
      
        if( curr_min<=y && y<curr_max )
        {
          break;
        }
      }
    }
  }

  if( head_pos >= num_for_head_pos ) head_pos = 0;
  return head_pos;
}

// head_pos����X�N���[���o�[�̃J�[�\����u��y���W�����߂�(SB_CURSOR_Y_OFFSET���l�����Ă���̂ŁA�߂�l��y�����̂܂�clwk�Ɏg���Ă��΂悢)
static u16 Manual_List_GetSbCursorYFromHeadPos( MANUAL_LIST_WORK* work, u16 num_for_head_pos, u16 head_pos )
{
  u16 y;

  {
    u16 range_min;  // range_min<= <range_max
    u16 range_max;
    u16 one_range;  // 1�̃p�l�����ɑ���������̕�

    range_min = SB_CURSOR_Y_MIN + SB_CURSOR_Y_OFFSET;
    range_max = SB_CURSOR_Y_MAX + SB_CURSOR_Y_OFFSET +1;

    one_range = ( range_max - range_min ) / num_for_head_pos;

    // �[�ɂ���Ƃ�
    if( head_pos == 0 )
    {
      y = range_min - SB_CURSOR_Y_OFFSET;  // �[�ɒu��
    }
    else if( head_pos == num_for_head_pos -1 )
    {
      y = range_max - SB_CURSOR_Y_OFFSET -1;  // �[�ɒu��
    }
    // �����ɂ���Ƃ�
    else
    {
      //u16 curr_min = range_min + one_range * (head_pos +0);  // curr_min<= <curr_max 
      //u16 curr_max = range_min + one_range * (head_pos +1);
      u16 curr_min = range_min + ( range_max - range_min ) * (head_pos +0) / num_for_head_pos;  // �v�Z�̏��Ԃ����ւ��������Ȃ̂ŁA
      u16 curr_max = range_min + ( range_max - range_min ) * (head_pos +1) / num_for_head_pos;  // ��̃R�����g�A�E�g�Ɠ����Ӗ��ł��B

      u16 curr_center = (curr_min + curr_max) /2;
      y = curr_center - SB_CURSOR_Y_OFFSET;  // �����ɒu��
    }
  }

  if( y<SB_CURSOR_Y_MIN )      y = SB_CURSOR_Y_MIN;
  else if( y>SB_CURSOR_Y_MAX ) y = SB_CURSOR_Y_MAX;
  return y;
}

