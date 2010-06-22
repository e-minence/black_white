//============================================================================
/**
 *  @file   manual_top.c
 *  @brief  �Q�[�����}�j���A��
 *  @author Koji Kawada
 *  @data   2010.04.26
 *  @note   
 *  ���W���[�����FMANUAL_TOP
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
#include "manual_top.h"

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
// APP_TASKMENU_WIN�̍���
typedef enum
{
  ATM_WIN_ITEM_CATEGORY,
  ATM_WIN_ITEM_ALL,
  ATM_WIN_ITEM_MAX,
}
ATM_WIN_ITEM;

#define ATM_PLATE_WIDTH (30)  // �L����
#define ATM_PLATE_X     (1)   // �L����
#define ATM_PLATE_Y     (3)   // �L����

// �^�b�`�̓����蔻��
static const GFL_UI_TP_HITTBL atm_tp_hittbl[ATM_WIN_ITEM_MAX +1] =
{
  {
    ( ATM_PLATE_Y + APP_TASKMENU_PLATE_HEIGHT*(ATM_WIN_ITEM_CATEGORY+0) ) *8  ,
    ( ATM_PLATE_Y + APP_TASKMENU_PLATE_HEIGHT*(ATM_WIN_ITEM_CATEGORY+1) ) *8 -1,
    ( ATM_PLATE_X ) *8,
    ( ATM_PLATE_X + ATM_PLATE_WIDTH ) *8 -1
  },  // rect
  {
    ( ATM_PLATE_Y + APP_TASKMENU_PLATE_HEIGHT*(ATM_WIN_ITEM_ALL+0) ) *8  ,
    ( ATM_PLATE_Y + APP_TASKMENU_PLATE_HEIGHT*(ATM_WIN_ITEM_ALL+1) ) *8 -1,
    ( ATM_PLATE_X ) *8,
    ( ATM_PLATE_X + ATM_PLATE_WIDTH ) *8 -1
  },  // rect
  { GFL_UI_TP_HIT_END, 0, 0, 0 },  // �e�[�u���I��
};

// ���͏��
typedef enum
{
  INPUT_STATE_NONE,  // ���̓��̓A�j���[�V�������s���Ă��Ȃ��̂ŁA���͉\�B
  INPUT_STATE_END,   // �������̓��͂��󂯕t���Ȃ��B
  INPUT_STATE_TB,    // �^�b�`�o�[�̓��̓A�j���[�V�������s���Ă���Œ��Ȃ̂ŁA���͕s�\�B
  INPUT_STATE_ATM,   // APP_TASKMENU_WIN�̓��̓A�j���[�V�������s���Ă���Œ��Ȃ̂ŁA���͕s�\�B
}
INPUT_STATE;

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
struct _MANUAL_TOP_WORK
{
  // �p�����[�^
  MANUAL_TOP_PARAM*           param;
  // ����
  MANUAL_COMMON_WORK*         cmn_wk;

  // �����ō쐬
  // APP_TASKMENU_WIN
  APP_TASKMENU_RES*           atm_res;
  APP_TASKMENU_WIN_WORK*      atm_win_wk[ATM_WIN_ITEM_MAX];

  // VBlank��TCB
  GFL_TCB*                    vblank_tcb;

  // ���͏��
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
static void Manual_Top_VBlankFunc( GFL_TCB* tcb, void* wk );

// ����
static void Manual_Top_Prepare( MANUAL_TOP_WORK* work );
static void Manual_Top_PrepareAppTaskmenuWin( MANUAL_TOP_WORK* work );
// �]��(VBlank�]���ł͂Ȃ�)
static void Manual_Top_Trans( MANUAL_TOP_WORK* work );
// �]��(VBlank�]��)
static void Manual_Top_TransVBlank( MANUAL_TOP_WORK* work );
// ��Еt��
static void Manual_Top_Finish( MANUAL_TOP_WORK* work );
static void Manual_Top_FinishAppTaskmenuWin( MANUAL_TOP_WORK* work );

// ����
static BOOL Manual_Top_InputAppTaskmenuWin( MANUAL_TOP_WORK* work );


//=============================================================================
/**
*  �O�����J�֐���`
*/
//=============================================================================
// ����������
MANUAL_TOP_WORK*  MANUAL_TOP_Init(
    MANUAL_TOP_PARAM*    param,
    MANUAL_COMMON_WORK*  cmn_wk
)
{
  // ���[�N
  MANUAL_TOP_WORK*  work  = GFL_HEAP_AllocClearMemory( cmn_wk->heap_id, sizeof(MANUAL_TOP_WORK) );

  // �p�����[�^
  work->param  = param;
  {
    // out
    work->param->result = MANUAL_TOP_RESULT_RETURN;
  }
  // ����
  work->cmn_wk = cmn_wk;

  // �����ō쐬
  Manual_Top_Prepare( work );
  //Manual_Top_Trans( work );
  Manual_Top_TransVBlank( work );

  // VBlank��TCB
  work->vblank_tcb = GFUser_VIntr_CreateTCB( Manual_Top_VBlankFunc, work, 1 );

  // ���͏��
  work->input_state = INPUT_STATE_NONE;

  // �I�����N�G�X�g
  work->end_req_count = END_REQ_COUNT_NO;
  
  // �}�j���A���^�b�`�o�[
  MANUAL_TOUCHBAR_SetType( work->cmn_wk->mtb_wk, MANUAL_TOUCHBAR_TYPE_TOP );

  return work;
}

// �I������
void  MANUAL_TOP_Exit(
    MANUAL_TOP_WORK*     work
)
{
  // VBlank��TCB
  GFL_TCB_DeleteTask( work->vblank_tcb );

  // �����ō쐬
  //Manual_Top_Finish( work );

  // ���[�N
  GFL_HEAP_FreeMemory( work );
}

// �又��
BOOL  MANUAL_TOP_Main(
    MANUAL_TOP_WORK*     work
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
      Manual_Top_Finish( work );
    }
    work->end_req_count--;
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
            work->param->result = MANUAL_TOP_RESULT_RETURN;
          }
          break;
        case TOUCHBAR_ICON_CLOSE:
          {
            work->param->result = MANUAL_TOP_RESULT_CLOSE;
          }
          break;
        }
        work->input_state = INPUT_STATE_END;
        //b_end = TRUE;
        work->end_req_count = END_REQ_COUNT_START_VAL;
      }
    }
  }

  // APP_TASKMENU_WIN�̓��͂𒲂ׂ�
  if(    work->input_state == INPUT_STATE_NONE
      || work->input_state == INPUT_STATE_ATM )
  {
    // APP_TASKMENU_WIN
    u8 i;
    for( i=0; i<ATM_WIN_ITEM_MAX; i++ )
    {
      APP_TASKMENU_WIN_Update( work->atm_win_wk[i] );
    }
      
    if( work->input_state == INPUT_STATE_NONE )
    {
      BOOL b_decide = Manual_Top_InputAppTaskmenuWin( work );
      if( b_decide ) work->input_state = INPUT_STATE_ATM;
    }
    else
    {
      BOOL b_finish = APP_TASKMENU_WIN_IsFinish( work->atm_win_wk[work->param->cursor_pos] );
      if( b_finish )
      {
        if( work->param->cursor_pos == ATM_WIN_ITEM_CATEGORY )
        {
          work->param->result = MANUAL_TOP_RESULT_CATEGORY;
        }
        else if( work->param->cursor_pos == ATM_WIN_ITEM_ALL )
        {
          work->param->result = MANUAL_TOP_RESULT_ALL;
        }
        work->input_state = INPUT_STATE_END;
        //b_end = TRUE;
        work->end_req_count = END_REQ_COUNT_START_VAL;
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
static void Manual_Top_VBlankFunc( GFL_TCB* tcb, void* wk )
{
  MANUAL_TOP_WORK* work = (MANUAL_TOP_WORK*)wk;

  // VBlank�]������Ȃ炱���ŁB
  //Manual_Top_Trans( work );
}

// ����
static void Manual_Top_Prepare( MANUAL_TOP_WORK* work )
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
      NARC_manual_bg1_NSCR,
      BG_FRAME_S_REAR,
      0,
      0,  // �S�]��
      FALSE,
      work->cmn_wk->heap_id );
*/

  GFL_ARCHDL_UTIL_TransVramScreenCharOfsVBlank(
      work->cmn_wk->handle_system,
      NARC_manual_bg1_NSCR,
      BG_FRAME_S_REAR,
      0,
      0,
      0,  // �S�]��
      FALSE,
      work->cmn_wk->heap_id );

  // APP_TASKMENU_WIN
  Manual_Top_PrepareAppTaskmenuWin( work );
}

static void Manual_Top_PrepareAppTaskmenuWin( MANUAL_TOP_WORK* work )
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

  // ���[�N
  {
    u8 i;

    // ���̐ݒ�
    APP_TASKMENU_ITEMWORK item[ATM_WIN_ITEM_MAX];

    item[ATM_WIN_ITEM_CATEGORY].str       = GFL_MSG_CreateString( work->cmn_wk->msgdata_system, manual_01_01 );
    item[ATM_WIN_ITEM_CATEGORY].msgColor  = APP_TASKMENU_ITEM_MSGCOLOR;
    item[ATM_WIN_ITEM_CATEGORY].type      = APP_TASKMENU_WIN_TYPE_NORMAL;

    item[ATM_WIN_ITEM_ALL].str            = GFL_MSG_CreateString( work->cmn_wk->msgdata_system, manual_01_02 );
    item[ATM_WIN_ITEM_ALL].msgColor       = APP_TASKMENU_ITEM_MSGCOLOR;
    item[ATM_WIN_ITEM_ALL].type           = APP_TASKMENU_WIN_TYPE_NORMAL;

    for( i=0; i<ATM_WIN_ITEM_MAX; i++ )
    {
      work->atm_win_wk[i] = APP_TASKMENU_WIN_Create(
          work->atm_res,
          &item[i],
          ATM_PLATE_X, ATM_PLATE_Y + APP_TASKMENU_PLATE_HEIGHT*i,
          ATM_PLATE_WIDTH,
          work->cmn_wk->heap_id );

      // ��������
      GFL_STR_DeleteBuffer( item[i].str );
    }
  }

  // �J�[�\���̕\��
  if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_KEY )
  {
    APP_TASKMENU_WIN_SetActive( work->atm_win_wk[work->param->cursor_pos], TRUE );
  }
}

// �]��(VBlank�]���ł͂Ȃ�)
static void Manual_Top_Trans( MANUAL_TOP_WORK* work )
{
  GFL_BG_LoadScreenReq( BG_FRAME_S_REAR );
}

// �]��(VBlank�]��)
static void Manual_Top_TransVBlank( MANUAL_TOP_WORK* work )
{
  GFL_BG_LoadScreenV_Req( BG_FRAME_S_REAR );
}

// ��Еt��
static void Manual_Top_Finish( MANUAL_TOP_WORK* work )
{
  Manual_Top_FinishAppTaskmenuWin( work );
}

static void Manual_Top_FinishAppTaskmenuWin( MANUAL_TOP_WORK* work )
{
  // APP_TASKMENU_WIN

  // �܂������I����Ă��Ȃ��e�L�X�g������Ƃ����Ȃ��̂ŁA�N���A���Ă���
  PRINTSYS_QUE_Clear( work->cmn_wk->print_que_main );
  {
    u8 i;
    // ���[�N
    for( i=0; i<ATM_WIN_ITEM_MAX; i++ )
    {
      APP_TASKMENU_WIN_Delete( work->atm_win_wk[i] );
    }
    // ���\�[�X
    APP_TASKMENU_RES_Delete( work->atm_res );
  }

  GFL_BG_LoadScreenV_Req( BG_FRAME_S_MAIN );
}

// ����
static BOOL Manual_Top_InputAppTaskmenuWin( MANUAL_TOP_WORK* work )
{
  // ���肵���Ƃ�TRUE��Ԃ�
  
  int tp_hit = GFL_UI_TP_HitTrg(atm_tp_hittbl);

  // �^�b�`or�L�[�̐؂�ւ��`�F�b�N
  // �L�[���͂̂Ƃ�
  if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_KEY )
  {
    if( ATM_WIN_ITEM_CATEGORY<=tp_hit && tp_hit<=ATM_WIN_ITEM_ALL )
    {
      // �^�b�`�ɕύX
      GFL_UI_SetTouchOrKey(GFL_APP_END_TOUCH);
      // �^�b�`�ɕύX�����ꍇ�́A���̂܂܏����𑱂���
      APP_TASKMENU_WIN_SetActive( work->atm_win_wk[work->param->cursor_pos], FALSE );
    }
  }
  // �^�b�`���͂̂Ƃ�
  else
  {
    if( GFL_UI_KEY_GetTrg() )
    {
      if( !( GFL_UI_KEY_GetTrg() & ( PAD_BUTTON_B | PAD_BUTTON_X ) ) )  // �^�b�`�o�[�ւ̓��͈ȊO�Ȃ�
      {
        // �L�[�ɕύX
        GFL_UI_SetTouchOrKey(GFL_APP_END_KEY);
        // �L�[�ɕύX�����ꍇ�́A�J�[�\����\�����āA�������I����
        APP_TASKMENU_WIN_SetActive( work->atm_win_wk[work->param->cursor_pos], TRUE );
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
      APP_TASKMENU_WIN_SetDecide( work->atm_win_wk[work->param->cursor_pos], TRUE );
      PMSND_PlaySE(MANUAL_SND_ATM_DECIDE);
      return TRUE;
    }
    if( GFL_UI_KEY_GetRepeat() & PAD_KEY_UP )  // �I�����ڂ̑�����ʂƑ�������킹�Ă��������̂�Repeat�ɂ���
    {
      APP_TASKMENU_WIN_SetActive( work->atm_win_wk[work->param->cursor_pos], FALSE );
      if( work->param->cursor_pos == ATM_WIN_ITEM_CATEGORY )
      {
        work->param->cursor_pos = ATM_WIN_ITEM_ALL;
      }
      else
      {
        work->param->cursor_pos = ATM_WIN_ITEM_CATEGORY;
      }
      APP_TASKMENU_WIN_SetActive( work->atm_win_wk[work->param->cursor_pos], TRUE );
      PMSND_PlaySE(MANUAL_SND_ATM_MOVE);
    }
    else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN )  // �I�����ڂ̑�����ʂƑ�������킹�Ă��������̂�Repeat�ɂ���
    {
      APP_TASKMENU_WIN_SetActive( work->atm_win_wk[work->param->cursor_pos], FALSE );
      if( work->param->cursor_pos == ATM_WIN_ITEM_ALL )
      {
        work->param->cursor_pos = ATM_WIN_ITEM_CATEGORY;
      }
      else
      {
        work->param->cursor_pos = ATM_WIN_ITEM_ALL;
      }
      APP_TASKMENU_WIN_SetActive( work->atm_win_wk[work->param->cursor_pos], TRUE );
      PMSND_PlaySE(MANUAL_SND_ATM_MOVE);
    }
    else if( GFL_UI_KEY_GetRepeat() & ( PAD_KEY_LEFT | PAD_BUTTON_L ) )
    {
      if( work->param->cursor_pos == ATM_WIN_ITEM_ALL )
      {
        APP_TASKMENU_WIN_SetActive( work->atm_win_wk[work->param->cursor_pos], FALSE );
        work->param->cursor_pos = ATM_WIN_ITEM_CATEGORY;
        APP_TASKMENU_WIN_SetActive( work->atm_win_wk[work->param->cursor_pos], TRUE );
        PMSND_PlaySE(MANUAL_SND_ATM_MOVE);
      }
    }
    else if( GFL_UI_KEY_GetRepeat() & ( PAD_KEY_RIGHT | PAD_BUTTON_R ) )
    {
      if( work->param->cursor_pos == ATM_WIN_ITEM_CATEGORY )
      {
        APP_TASKMENU_WIN_SetActive( work->atm_win_wk[work->param->cursor_pos], FALSE );
        work->param->cursor_pos = ATM_WIN_ITEM_ALL;
        APP_TASKMENU_WIN_SetActive( work->atm_win_wk[work->param->cursor_pos], TRUE );
        PMSND_PlaySE(MANUAL_SND_ATM_MOVE);
      }
    }
  }
  // �^�b�`���͎󂯕t��
  else
  {
    // �^�b�`�̂܂ܕύX�Ȃ��Ȃ̂ŁAGFL_UI_SetTouchOrKey�͕s�v�B
    if( ATM_WIN_ITEM_CATEGORY<=tp_hit && tp_hit<=ATM_WIN_ITEM_ALL )
    {
      // ����
      work->param->cursor_pos = tp_hit;
      APP_TASKMENU_WIN_SetActive( work->atm_win_wk[work->param->cursor_pos], TRUE );
      APP_TASKMENU_WIN_SetDecide( work->atm_win_wk[work->param->cursor_pos], TRUE );
      PMSND_PlaySE(MANUAL_SND_ATM_DECIDE);
      return TRUE;
    }
  }

  return FALSE;
}

