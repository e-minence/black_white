//============================================================================
/**
 *  @file   manual_touchbar.c
 *  @brief  �Q�[�����}�j���A��
 *  @author Koji Kawada
 *  @data   2010.04.26
 *  @note   
 *  ���W���[�����FMANUAL_TOUCHBAR
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

// �A�[�J�C�u
#include "arc_def.h"
#include "font/font.naix"
#include "message.naix"
#include "msg/msg_manual.h"
#include "msg/msg_manual_text.h"
#include "manual.naix"
#include "manual_image.naix"


// �T�E���h

// �I�[�o�[���C
FS_EXTERN_OVERLAY(ui_common);


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================
// �^�b�`�o�[�̃e�L�X�g
enum
{
  TEXT_DUMMY,
  TEXT_TB,
  TEXT_MAX,
};

static const u8 bmpwin_setup[TEXT_MAX][9] =
{
  // frmnum              posx  posy  sizx  sizy  palnum                dir                    x  y
  {  BG_FRAME_S_TB_TEXT,    0,    0,    1,    1, BG_PAL_POS_S_TEXT,    GFL_BMP_CHRAREA_GET_F, 0, 0 },  // TEXT_DUMMY
  {  BG_FRAME_S_TB_TEXT,    2,   21,   26,    3, BG_PAL_POS_S_TEXT,    GFL_BMP_CHRAREA_GET_F, 0, 4 },  // TEXT_TB
};

static const u8 bmpwin_type_setup[MANUAL_TOUCHBAR_TYPE_MAX][1] =
{
  //  x
  {   0 },
  {   0 },
  {   0 },
  {  32 },
};

#define TEXT_COLOR_L  (15)  // ������F
#define TEXT_COLOR_S  ( 2)  // �����e�F
#define TEXT_COLOR_B  ( 0)  // �����w�i�F(����)

#define TEXT_PAGE_LEN_MAX  (16)  // �y�[�W����������(��F8/8)�̕�����


// ���͏��
typedef enum
{
  INPUT_STATE_NONE,       // ���̓��̓A�j���[�V�������s���Ă��Ȃ��̂ŁA���͉\�B
  INPUT_STATE_NEXT_NONE,  // ���̃t���[����INPUT_STATE_NONE�ɂȂ�B���͓��͕s�\�B
  INPUT_STATE_TB,         // �^�b�`�o�[�̓��̓A�j���[�V�������s���Ă���Œ��Ȃ̂ŁA���͕s�\�B
  INPUT_STATE_BTN,        // �ǉ������{�^���̓��̓A�j���[�V�������s���Ă���Œ��Ȃ̂ŁA���͕s�\�B
}
INPUT_STATE;


// OBJ
enum
{
  OBJ_BTN_RES_NCG,  // �^�b�`�o�[�̃A�C�R���Ɠ������\�[�X(�����̂�ǂݍ��ނ̂̓C�������d�����Ȃ�)
  OBJ_BTN_RES_NCL,
  OBJ_BTN_RES_NCE,
  OBJ_RES_MAX,
};
enum
{
  OBJ_BTN_CELL_START,    // OBJ_BTN_CELL_START<= <OBJ_BTN_CELL_END
  OBJ_BTN_L_CELL       = OBJ_BTN_CELL_START,
  OBJ_BTN_R_CELL,
  OBJ_BTN_CELL_END,
  OBJ_CELL_MAX         = OBJ_BTN_CELL_END,

  OBJ_BTN_CELL_NUM     = OBJ_BTN_CELL_END - OBJ_BTN_CELL_START,
  BTN_PUSH_NONE        = 0xFF,
};

typedef struct
{
  GFL_CLWK_DATA  clwk_data;
  u8             touch_x_min;  // touch_x_min<= <=touch_x_max
  u8             touch_x_max;
  u8             touch_y_min;  // touch_y_min<= <=touch_y_max
  u8             touch_y_max;
  u32            push_anmseq;
  u32            repeat_key;
  u32            se;
  TOUCHBAR_ICON  icon;            
}
OBJ_BTN_DATA;

static const OBJ_BTN_DATA obj_btn_data[OBJ_BTN_CELL_NUM] =
{
    // pos_x,               pos_y,            anmseq,                           softpri,  bgpri,
  { {  TOUCHBAR_ICON_X_00,  TOUCHBAR_ICON_Y,  APP_COMMON_BARICON_CURSOR_LEFT,   0,        BG_FRAME_PRI_S_TB_BAR },
    TOUCHBAR_ICON_X_00, TOUCHBAR_ICON_X_00+TOUCHBAR_ICON_WIDTH, TOUCHBAR_ICON_Y, TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT,
    APP_COMMON_BARICON_CURSOR_LEFT_ON, (PAD_KEY_LEFT | PAD_BUTTON_L), MANUAL_SND_BTN_PUSH, TOUCHBAR_ICON_CUR_L },
  { {  TOUCHBAR_ICON_X_02,  TOUCHBAR_ICON_Y,  APP_COMMON_BARICON_CURSOR_RIGHT,  0,        BG_FRAME_PRI_S_TB_BAR },
    TOUCHBAR_ICON_X_02, TOUCHBAR_ICON_X_02+TOUCHBAR_ICON_WIDTH, TOUCHBAR_ICON_Y, TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT,
    APP_COMMON_BARICON_CURSOR_RIGHT_ON, (PAD_KEY_RIGHT | PAD_BUTTON_R), MANUAL_SND_BTN_PUSH, TOUCHBAR_ICON_CUR_R },
};

typedef enum
{
  BTN_STATE_MAIN,     // TOUCHBAR_SEQ_MAIN �ɍ��킹�Ă���
  BTN_STATE_ANM_TRG,  // TOUCHBAR_SEQ_ANM_TRG �ɍ��킹�Ă���
  BTN_STATE_ANM,      // TOUCHBAR_SEQ_ANM �ɍ��킹�Ă���
  BTN_STATE_TRG,      // TOUCHBAR_SEQ_TRG �ɍ��킹�Ă���
}
BTN_STATE;

#define CELL_IDX_TO_BTN_IDX(i)  (i-OBJ_BTN_CELL_START)
#define BTN_IDX_TO_CELL_IDX(i)  (i+OBJ_BTN_CELL_START)


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// ���[�N
//=====================================
struct _MANUAL_TOUCHBAR_WORK
{
  // ����
  MANUAL_COMMON_WORK*         cmn_wk;

  // �����ō쐬
  TOUCHBAR_WORK*              tb;

  // �^�C�v
  MANUAL_TOUCHBAR_TYPE        type;
  u8                          page_num;  // �y�[�W��(0�̂Ƃ��y�[�W�\���Ȃ�)
  u8                          page_no;   // ���݂̃y�[�W(0<=page_no<page_num)

  // �^�b�`�o�[�̃e�L�X�g
  GFL_BMPWIN*                 text_bmpwin[TEXT_MAX];
  BOOL                        text_trans[TEXT_MAX];  // �]������K�v������ꍇTRUE

  // ���͏��
  INPUT_STATE                 input_state;
  TOUCHBAR_ICON               touch_icon;            
  TOUCHBAR_ICON               trg_icon;            

  // OBJ
  u32                         obj_res[OBJ_RES_MAX];
  GFL_CLWK*                   obj_clwk[OBJ_CELL_MAX];
  u8                          btn_push;  // OBJ_BTN_L_CELL, OBJ_BTN_R_CELL, BTN_PUSH_NONE �̂ǂꂩ
  BTN_STATE                   btn_state;
  // VBlank��TCB
  GFL_TCB*                    vblank_tcb;
};


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// VBlank�֐�
static void Manual_Touchbar_VBlankFunc( GFL_TCB* tcb, void* wk );

// �^�b�`�o�[�̏���
static void Manual_Touchbar_TbPrepare( MANUAL_TOUCHBAR_WORK* work );
// �^�b�`�o�[�̓]��(VBlank�]���ł͂Ȃ�)
static void Manual_Touchbar_TbTrans( MANUAL_TOUCHBAR_WORK* work );

// �e�L�X�g
static void Manual_Touchbar_TextInit( MANUAL_TOUCHBAR_WORK* work );
static void Manual_Touchbar_TextExit( MANUAL_TOUCHBAR_WORK* work );
static void Manual_Touchbar_TextMain( MANUAL_TOUCHBAR_WORK* work );
static void Manual_Touchbar_TextPrintNone( MANUAL_TOUCHBAR_WORK* work );
static void Manual_Touchbar_TextPrintCategory( MANUAL_TOUCHBAR_WORK* work );
static void Manual_Touchbar_TextPrintTitle( MANUAL_TOUCHBAR_WORK* work );
static void Manual_Touchbar_TextPrintExplain( MANUAL_TOUCHBAR_WORK* work );

// OBJ
static void Manual_Touchbar_ObjInit( MANUAL_TOUCHBAR_WORK* work );
static void Manual_Touchbar_ObjExit( MANUAL_TOUCHBAR_WORK* work );
static void Manual_Touchbar_BtnMain( MANUAL_TOUCHBAR_WORK* work );

// ��Ԃ̏�����
static void Manual_Touchbar_StateInit( MANUAL_TOUCHBAR_WORK* work );


//=============================================================================
/**
*  �O�����J�֐���`
*/
//=============================================================================
// ����������
MANUAL_TOUCHBAR_WORK*  MANUAL_TOUCHBAR_Init(
    void*                    cmn_wk  // �L���X�g(MANUAL_COMMON_WORK*)���Ďg�p
)
{
  // ���[�N
  MANUAL_TOUCHBAR_WORK*  work;

  // ����
  MANUAL_COMMON_WORK*    cw = (MANUAL_COMMON_WORK*)cmn_wk;

  // �I�[�o�[���C�ǂݍ���
  GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common) );

  // ���[�N
  work  = GFL_HEAP_AllocClearMemory( cw->heap_id, sizeof(MANUAL_TOUCHBAR_WORK) );

  // ����
  work->cmn_wk = cw;

  // �����ō쐬
  Manual_Touchbar_TbPrepare( work );
  Manual_Touchbar_TbTrans( work );
  Manual_Touchbar_TextInit( work );
  Manual_Touchbar_ObjInit( work );

  // �^�C�v
  work->type = MANUAL_TOUCHBAR_TYPE_INIT;
  work->page_num = 0;
  work->page_no  = 0;
  MANUAL_TOUCHBAR_SetType( work, MANUAL_TOUCHBAR_TYPE_TOP );

  // ��Ԃ̏�����
  Manual_Touchbar_StateInit( work );

  // VBlank��TCB
  work->vblank_tcb = GFUser_VIntr_CreateTCB( Manual_Touchbar_VBlankFunc, work, 1 );

  return work;
}

// �I������
void  MANUAL_TOUCHBAR_Exit(
    MANUAL_TOUCHBAR_WORK*     work
)
{
  // VBlank��TCB
  GFL_TCB_DeleteTask( work->vblank_tcb );

  // �����ō쐬
  Manual_Touchbar_ObjExit( work );
  Manual_Touchbar_TextExit( work );
  // �^�b�`�o�[�j��
  TOUCHBAR_Exit( work->tb );

  // ���[�N
  GFL_HEAP_FreeMemory( work );

  // �I�[�o�[���C�j��
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common) );
}

// �又��
void  MANUAL_TOUCHBAR_Main(
    MANUAL_TOUCHBAR_WORK*     work
)
{
  // 1�t���[���������Ԃ����ɖ߂�
  work->touch_icon  = TOUCHBAR_SELECT_NONE;
  work->trg_icon    = TOUCHBAR_SELECT_NONE;

  // �e�L�X�g
  Manual_Touchbar_TextMain( work );

  // ���͏�Ԃ�ύX���ďI��
  if( work->input_state == INPUT_STATE_NEXT_NONE )
  {
    work->input_state = INPUT_STATE_NONE;
    return;
  }

  // �^�b�`�o�[�̓��͂𒲂ׂ�
  if(    work->input_state == INPUT_STATE_NONE
      || work->input_state == INPUT_STATE_TB )
  {
    // �^�b�`�o�[
    TOUCHBAR_Main( work->tb );

    if( work->input_state == INPUT_STATE_NONE )
    {
      TOUCHBAR_ICON icon = TOUCHBAR_GetTouch( work->tb );
      if( icon != TOUCHBAR_SELECT_NONE )
      {
        work->input_state = INPUT_STATE_TB;
        work->touch_icon  = icon;
      }
    }
    else
    {
      TOUCHBAR_ICON icon = TOUCHBAR_GetTrg( work->tb );
      if( icon != TOUCHBAR_SELECT_NONE )
      {
        work->input_state = INPUT_STATE_NEXT_NONE;
        work->trg_icon    = icon;
      }
    }
  }

  // �ǉ������{�^��
  if(    work->input_state == INPUT_STATE_NONE
      || work->input_state == INPUT_STATE_BTN )
  {
    // �{�^��
    Manual_Touchbar_BtnMain( work );
    
    if( work->input_state == INPUT_STATE_NONE )
    {
      if( work->btn_state == BTN_STATE_ANM_TRG )
      {
        work->input_state = INPUT_STATE_BTN;
        work->touch_icon  = obj_btn_data[CELL_IDX_TO_BTN_IDX(work->btn_push)].icon;
      }
    }
    else
    {
      if( work->btn_state == BTN_STATE_TRG )
      {
        work->input_state = INPUT_STATE_NEXT_NONE;
        work->trg_icon    = obj_btn_data[CELL_IDX_TO_BTN_IDX(work->btn_push)].icon;
      }
    }
  }
}

// �^�C�v�ݒ�
void MANUAL_TOUCHBAR_SetType(
    MANUAL_TOUCHBAR_WORK*     work,
    MANUAL_TOUCHBAR_TYPE      type
)
{
  if( work->type == type ) return;

  // �O�̂��\���ɂ���
  switch( work->type )
  {
  case MANUAL_TOUCHBAR_TYPE_INIT:      // �ŏ���1��̂�
    {
      // �������Ȃ�
    }
    break;
  case MANUAL_TOUCHBAR_TYPE_TOP:
    {
      TOUCHBAR_SetVisible( work->tb, TOUCHBAR_ICON_CLOSE, FALSE );
    }
    break;
  case MANUAL_TOUCHBAR_TYPE_CATEGORY:
    {
    }
    break;
  case MANUAL_TOUCHBAR_TYPE_TITLE:
    {
    }
    break;
  case MANUAL_TOUCHBAR_TYPE_EXPLAIN:
    {
      GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[OBJ_BTN_L_CELL], FALSE );
      GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[OBJ_BTN_R_CELL], FALSE );
/*
      TOUCHBAR_SetVisible( work->tb, TOUCHBAR_ICON_CUR_R, FALSE );
      TOUCHBAR_SetVisible( work->tb, TOUCHBAR_ICON_CUR_L, FALSE );
*/
    }
    break;
  }

  work->type = type;
 
  // ��Ԃ̏�����
  Manual_Touchbar_StateInit( work );

  // ���̂�\������
  switch( work->type )
  {
  case MANUAL_TOUCHBAR_TYPE_INIT:      // �ŏ���1��̂�
    {
      // ���蓾�Ȃ�
    }
    break;
  case MANUAL_TOUCHBAR_TYPE_TOP:
    {
      TOUCHBAR_SetVisible( work->tb, TOUCHBAR_ICON_CLOSE, TRUE );
      Manual_Touchbar_TextPrintNone( work );
    }
    break;
  case MANUAL_TOUCHBAR_TYPE_CATEGORY:
    {
      Manual_Touchbar_TextPrintCategory( work );
    }
    break;
  case MANUAL_TOUCHBAR_TYPE_TITLE:
    {
      Manual_Touchbar_TextPrintTitle( work );
    }
    break;
  case MANUAL_TOUCHBAR_TYPE_EXPLAIN:
    {
      // ���̊֐����Ă񂾒���ɌĂ΂��ł��낤
      // MANUAL_TOUCHBAR_SetPage
      // �ɔC����B
    }
    break;
  }
}

// �y�[�W�ݒ�(MANUAL_TOUCHBAR_TYPE_EXPLAIN�̂Ƃ��̂ݗL��)
void MANUAL_TOUCHBAR_SetPage(
    MANUAL_TOUCHBAR_WORK*     work,
    u8                        page_num,   // �y�[�W��(0�̂Ƃ��y�[�W�\���Ȃ�)
    u8                        page_no     // ���݂̃y�[�W(0<=page_no<page_num)
)
{
  work->page_num = page_num;
  work->page_no  = page_no;

  if( work->type != MANUAL_TOUCHBAR_TYPE_EXPLAIN ) return;  // MANUAL_TOUCHBAR_TYPE_EXPLAIN�łȂ��Ƃ��͖���

  if( work->page_num == 0 || work->page_num == 1 )
  {
    GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[OBJ_BTN_L_CELL], FALSE );
    GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[OBJ_BTN_R_CELL], FALSE );
/*
    TOUCHBAR_SetVisible( work->tb, TOUCHBAR_ICON_CUR_R, FALSE );
    TOUCHBAR_SetVisible( work->tb, TOUCHBAR_ICON_CUR_L, FALSE );
*/
    Manual_Touchbar_TextPrintNone( work );
  }
  else
  {
    if( work->page_no == 0 )
    {
      GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[OBJ_BTN_L_CELL], FALSE );
      GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[OBJ_BTN_R_CELL], TRUE );
/*
      TOUCHBAR_SetVisible( work->tb, TOUCHBAR_ICON_CUR_R, TRUE );
      TOUCHBAR_SetVisible( work->tb, TOUCHBAR_ICON_CUR_L, FALSE );
*/
    }
    else if( work->page_no == page_num -1 )
    {
      GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[OBJ_BTN_L_CELL], TRUE );
      GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[OBJ_BTN_R_CELL], FALSE );
/*
      TOUCHBAR_SetVisible( work->tb, TOUCHBAR_ICON_CUR_R, FALSE );
      TOUCHBAR_SetVisible( work->tb, TOUCHBAR_ICON_CUR_L, TRUE );
*/
    }
    else
    {
      GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[OBJ_BTN_L_CELL], TRUE );
      GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[OBJ_BTN_R_CELL], TRUE );
/*
      TOUCHBAR_SetVisible( work->tb, TOUCHBAR_ICON_CUR_R, TRUE );
      TOUCHBAR_SetVisible( work->tb, TOUCHBAR_ICON_CUR_L, TRUE );
*/
    }
    Manual_Touchbar_TextPrintExplain( work );
  }
}

// TOUCHBAR��킹�������̊֐�
TOUCHBAR_ICON MANUAL_TOUCHBAR_GetTrg(
    MANUAL_TOUCHBAR_WORK*     work
)
{
  return work->trg_icon;  // �Œ�ł�1�t���[���͓�����Ԃ�ۂ��Ă����Ȃ��ƁA
                          // ���̊֐����Ăяo�����^�C�~���O�ɂ���Ă͂��܂���Ԃ��擾�ł��Ȃ��B
/*
  if( work->input_state == INPUT_STATE_TB )
  {
    return TOUCHBAR_GetTrg( work->tb );
  }
  else if( work->input_state == INPUT_STATE_BTN )
  {
    if( work->btn_state == BTN_STATE_TRG )
    {
      return obj_btn_data[CELL_IDX_TO_BTN_IDX(work->btn_push)].icon;
    }
    else
    {
      return TOUCHBAR_SELECT_NONE;
    }
  }
  else
  {
    return TOUCHBAR_SELECT_NONE;
  }
*/
}
TOUCHBAR_ICON MANUAL_TOUCHBAR_GetTouch(
    MANUAL_TOUCHBAR_WORK*     work
)
{
  return work->touch_icon;  // �Œ�ł�1�t���[���͓�����Ԃ�ۂ��Ă����Ȃ��ƁA
                            // ���̊֐����Ăяo�����^�C�~���O�ɂ���Ă͂��܂���Ԃ��擾�ł��Ȃ��B
/*
  if( work->input_state == INPUT_STATE_TB )
  {
    return TOUCHBAR_GetTouch( work->tb );
  }
  else if( work->input_state == INPUT_STATE_BTN )
  {
    if( work->btn_state == BTN_STATE_TRG )
    {
      return obj_btn_data[CELL_IDX_TO_BTN_IDX(work->btn_push)].icon;
    }
    else
    {
      return TOUCHBAR_SELECT_NONE;
    }
  }
  else
  {
    return TOUCHBAR_SELECT_NONE;
  }
*/
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
static void Manual_Touchbar_VBlankFunc( GFL_TCB* tcb, void* wk )
{
  MANUAL_TOUCHBAR_WORK* work = (MANUAL_TOUCHBAR_WORK*)wk;

  // VBlank�]������Ȃ炱���ŁB
  //Manual_Touchbar_TbTrans( work );
}

// �^�b�`�o�[�̏���
static void Manual_Touchbar_TbPrepare( MANUAL_TOUCHBAR_WORK* work )
{
  // �^�b�`�o�[�̐ݒ�
  {
    // �A�C�R���̐ݒ�
    // �������
    TOUCHBAR_SETUP      tb_setup;

    TOUCHBAR_ITEM_ICON  tb_item_icon_tbl[] =
    {
      {
        TOUCHBAR_ICON_RETURN,
        { TOUCHBAR_ICON_X_07, TOUCHBAR_ICON_Y },
      },
      {
        TOUCHBAR_ICON_CLOSE,
        { TOUCHBAR_ICON_X_06, TOUCHBAR_ICON_Y },
      },
/*
      {
        TOUCHBAR_ICON_CUR_R,
        { TOUCHBAR_ICON_X_02, TOUCHBAR_ICON_Y },
      },
      {
        TOUCHBAR_ICON_CUR_L,
        { TOUCHBAR_ICON_X_00, TOUCHBAR_ICON_Y },
      },
*/
    };

    // �ݒ�\����
    // �����قǂ̑����{���\�[�X���������
    GFL_STD_MemClear( &tb_setup, sizeof(TOUCHBAR_SETUP) );
    tb_setup.p_item     = tb_item_icon_tbl;            //��̑����
    tb_setup.item_num   = NELEMS(tb_item_icon_tbl);    //�����������邩
    tb_setup.p_unit     = MANUAL_GRAPHIC_GetClunit( work->cmn_wk->graphic );  //OBJ�ǂݍ��݂̂��߂�CLUNIT
    tb_setup.bar_frm    = BG_FRAME_S_TB_BAR;           //BG�ǂݍ��݂̂��߂�BG��
    tb_setup.bg_plt     = BG_PAL_POS_S_TB_BAR;         //BG��گ�
    tb_setup.obj_plt    = OBJ_PAL_POS_S_TB_ICON;       //OBJ��گ�
    tb_setup.mapping    = APP_COMMON_MAPPING_32K;      //�}�b�s���O���[�h

    work->tb = TOUCHBAR_Init( &tb_setup, work->cmn_wk->heap_id );
  }

  // RETURN�ȊO�͔�\���ɂ��Ă���
  {
    TOUCHBAR_SetVisible( work->tb, TOUCHBAR_ICON_CLOSE, FALSE );
/*
    TOUCHBAR_SetVisible( work->tb, TOUCHBAR_ICON_CUR_R, FALSE );
    TOUCHBAR_SetVisible( work->tb, TOUCHBAR_ICON_CUR_L, FALSE );
*/
  }
}

// �^�b�`�o�[�̓]��(VBlank�]���ł͂Ȃ�)
static void Manual_Touchbar_TbTrans( MANUAL_TOUCHBAR_WORK* work )
{
}

// �e�L�X�g
static void Manual_Touchbar_TextInit( MANUAL_TOUCHBAR_WORK* work )
{
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
}
static void Manual_Touchbar_TextExit( MANUAL_TOUCHBAR_WORK* work )
{
  u8 i;
  PRINTSYS_QUE_Clear( work->cmn_wk->print_que_tb );
  for( i=0; i<TEXT_MAX; i++ )
  {
    work->text_trans[i] = FALSE;
    GFL_BMPWIN_Delete( work->text_bmpwin[i] );
  }
}
static void Manual_Touchbar_TextMain( MANUAL_TOUCHBAR_WORK* work )
{
  u8 i;
  for( i=0; i<TEXT_MAX; i++ )
  {
    if( work->text_trans[i] )
    {
      if( !PRINTSYS_QUE_IsExistTarget( work->cmn_wk->print_que_tb, GFL_BMPWIN_GetBmp(work->text_bmpwin[i]) ) )
      {
        GFL_BMPWIN_MakeTransWindow_VBlank( work->text_bmpwin[i] );
        work->text_trans[i] = FALSE;
      }
    }
  }
}
static void Manual_Touchbar_TextPrintNone( MANUAL_TOUCHBAR_WORK* work )
{
  PRINTSYS_QUE_Clear( work->cmn_wk->print_que_tb );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_TB]), 0 );
  GFL_BMPWIN_TransVramCharacter( work->text_bmpwin[TEXT_TB] );
  work->text_trans[TEXT_TB] = FALSE;
}
static void Manual_Touchbar_TextPrintCategory( MANUAL_TOUCHBAR_WORK* work )
{
  PRINTSYS_QUE_Clear( work->cmn_wk->print_que_tb );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_TB]), 0 );

  {
    STRBUF* strbuf = GFL_MSG_CreateString( work->cmn_wk->msgdata_system, manual_02_01 );
    PRINTSYS_PrintQueColor(
        work->cmn_wk->print_que_tb,
        GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_TB]),
        bmpwin_type_setup[MANUAL_TOUCHBAR_TYPE_CATEGORY][0], bmpwin_setup[TEXT_TB][8],
        strbuf,
        work->cmn_wk->font,
        PRINTSYS_LSB_Make(TEXT_COLOR_L,TEXT_COLOR_S,TEXT_COLOR_B) );
    GFL_STR_DeleteBuffer( strbuf );
    work->text_trans[TEXT_TB] = TRUE;
    // ���ɍς�ł��邩������Ȃ��̂ŁAMain��1�x�Ă�ł���
    Manual_Touchbar_TextMain( work );
  }
}
static void Manual_Touchbar_TextPrintTitle( MANUAL_TOUCHBAR_WORK* work )
{
  PRINTSYS_QUE_Clear( work->cmn_wk->print_que_tb );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_TB]), 0 );

  {
    STRBUF* strbuf = GFL_MSG_CreateString( work->cmn_wk->msgdata_system, manual_02_02 );
    PRINTSYS_PrintQueColor(
        work->cmn_wk->print_que_tb,
        GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_TB]),
        bmpwin_type_setup[MANUAL_TOUCHBAR_TYPE_TITLE][0], bmpwin_setup[TEXT_TB][8],
        strbuf,
        work->cmn_wk->font,
        PRINTSYS_LSB_Make(TEXT_COLOR_L,TEXT_COLOR_S,TEXT_COLOR_B) );
    GFL_STR_DeleteBuffer( strbuf );
    work->text_trans[TEXT_TB] = TRUE;
    // ���ɍς�ł��邩������Ȃ��̂ŁAMain��1�x�Ă�ł���
    Manual_Touchbar_TextMain( work );
  }
}
static void Manual_Touchbar_TextPrintExplain( MANUAL_TOUCHBAR_WORK* work )
{
  PRINTSYS_QUE_Clear( work->cmn_wk->print_que_tb );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_TB]), 0 );

  {
    WORDSET* wordset       = WORDSET_Create( work->cmn_wk->heap_id );
    STRBUF*  src_strbuf    = GFL_MSG_CreateString( work->cmn_wk->msgdata_system, manual_02_03 );
    STRBUF*  strbuf        = GFL_STR_CreateBuffer( TEXT_PAGE_LEN_MAX, work->cmn_wk->heap_id );
    WORDSET_RegisterNumber( wordset, 0, work->page_no +1, 1, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );  // 1�n�܂�ɂ��Ă���
    WORDSET_RegisterNumber( wordset, 1, work->page_num, 1, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
    WORDSET_ExpandStr( wordset, strbuf, src_strbuf );

    PRINTSYS_PrintQueColor(
        work->cmn_wk->print_que_tb,
        GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_TB]),
        bmpwin_type_setup[MANUAL_TOUCHBAR_TYPE_EXPLAIN][0], bmpwin_setup[TEXT_TB][8],
        strbuf,
        work->cmn_wk->font,
        PRINTSYS_LSB_Make(TEXT_COLOR_L,TEXT_COLOR_S,TEXT_COLOR_B) );
    GFL_STR_DeleteBuffer( strbuf );
    GFL_STR_DeleteBuffer( src_strbuf );
    WORDSET_Delete( wordset ); 
    work->text_trans[TEXT_TB] = TRUE;
    // ���ɍς�ł��邩������Ȃ��̂ŁAMain��1�x�Ă�ł���
    Manual_Touchbar_TextMain( work );
  }
}

// OBJ
static void Manual_Touchbar_ObjInit( MANUAL_TOUCHBAR_WORK* work )
{
  u8 cell_idx;

  
  // ���\�[�X�̓ǂݍ���
  ARCHANDLE* handle = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), work->cmn_wk->heap_id );

  // BTN 
  work->obj_res[OBJ_BTN_RES_NCL] = GFL_CLGRP_PLTT_RegisterEx(
      handle,
			APP_COMMON_GetBarIconPltArcIdx(),
      CLSYS_DRAW_SUB,
      OBJ_PAL_POS_S_BTN * 0x20,
      0,
      OBJ_PAL_NUM_S_BTN,
      work->cmn_wk->heap_id );
  work->obj_res[OBJ_BTN_RES_NCG] = GFL_CLGRP_CGR_Register(
      handle,
      APP_COMMON_GetBarIconCharArcIdx(),
      FALSE,
      CLSYS_DRAW_SUB,
      work->cmn_wk->heap_id );
  work->obj_res[OBJ_BTN_RES_NCE] = GFL_CLGRP_CELLANIM_Register(
      handle,
      APP_COMMON_GetBarIconCellArcIdx(APP_COMMON_MAPPING_32K),
      APP_COMMON_GetBarIconAnimeArcIdx(APP_COMMON_MAPPING_32K),
      work->cmn_wk->heap_id );

  GFL_ARC_CloseDataHandle( handle );


  // CLWK�쐬
  // BTN
  for( cell_idx=OBJ_BTN_CELL_START; cell_idx<OBJ_BTN_CELL_END; cell_idx++ )
  {
    work->obj_clwk[cell_idx] = GFL_CLACT_WK_Create(
        MANUAL_GRAPHIC_GetClunit( work->cmn_wk->graphic ),
        work->obj_res[OBJ_BTN_RES_NCG],
        work->obj_res[OBJ_BTN_RES_NCL],
        work->obj_res[OBJ_BTN_RES_NCE],
        &(obj_btn_data[CELL_IDX_TO_BTN_IDX(cell_idx)].clwk_data),
        CLSYS_DEFREND_SUB,
        work->cmn_wk->heap_id );
    GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[cell_idx], FALSE );
    GFL_CLACT_WK_SetAutoAnmFlag( work->obj_clwk[cell_idx], TRUE );
  }
}
static void Manual_Touchbar_ObjExit( MANUAL_TOUCHBAR_WORK* work )
{
  // CLWK�j��
  u8 i;
  for( i=0; i<OBJ_CELL_MAX; i++ )
  {
    GFL_CLACT_WK_Remove( work->obj_clwk[i] );
  }
  
  // ���\�[�X�j��
  // BTN
  GFL_CLGRP_CELLANIM_Release( work->obj_res[OBJ_BTN_RES_NCE] );
  GFL_CLGRP_CGR_Release( work->obj_res[OBJ_BTN_RES_NCG] );
  GFL_CLGRP_PLTT_Release( work->obj_res[OBJ_BTN_RES_NCL] );
}
static void Manual_Touchbar_BtnMain( MANUAL_TOUCHBAR_WORK* work )
{
  u32 x, y;

  switch( work->btn_state )
  {
  case BTN_STATE_MAIN:
    {
      BOOL b_input_done = FALSE;
      u8 btn_idx;
      for( btn_idx=0; btn_idx<OBJ_BTN_CELL_NUM; btn_idx++ )
      {
        if( !GFL_CLACT_WK_GetDrawEnable( work->obj_clwk[BTN_IDX_TO_CELL_IDX(btn_idx)] ) ) continue;  // �\�����Ă�����̂����L��

        // �L�[
        if( !b_input_done )
        {
          if( GFL_UI_KEY_GetRepeat() & obj_btn_data[btn_idx].repeat_key )
          {
            work->btn_push = BTN_IDX_TO_CELL_IDX(btn_idx);
            GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
            b_input_done = TRUE;
            break;
          }
        }
        // �^�b�`
        if( !b_input_done )
        {
          if( GFL_UI_TP_GetPointTrg( &x, &y ) )
          {
            if(    obj_btn_data[btn_idx].touch_x_min<=x && x<=obj_btn_data[btn_idx].touch_x_max
                && obj_btn_data[btn_idx].touch_y_min<=y && y<=obj_btn_data[btn_idx].touch_y_max )
            {
              work->btn_push = BTN_IDX_TO_CELL_IDX(btn_idx);
              GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
              b_input_done = TRUE;
              break;
            }
          }
        }
      }
      // ���͂���������
      if( b_input_done )
      {
        GFL_CLACT_WK_SetAnmSeq( work->obj_clwk[work->btn_push], obj_btn_data[CELL_IDX_TO_BTN_IDX(work->btn_push)].push_anmseq );
        if(obj_btn_data[CELL_IDX_TO_BTN_IDX(work->btn_push)].se) PMSND_PlaySE( obj_btn_data[CELL_IDX_TO_BTN_IDX(work->btn_push)].se );
        work->btn_state = BTN_STATE_ANM_TRG;
      }
    }
    break;
  case BTN_STATE_ANM_TRG:  //�A�j���O�̂PF�𓾂邽��
    {
      work->btn_state = BTN_STATE_ANM;
    }
    break;
  case BTN_STATE_ANM:
    {
      if( !GFL_CLACT_WK_CheckAnmActive( work->obj_clwk[work->btn_push] ) )
      {
        GFL_CLACT_WK_SetAnmSeq( work->obj_clwk[work->btn_push], obj_btn_data[CELL_IDX_TO_BTN_IDX(work->btn_push)].clwk_data.anmseq );
        work->btn_state = BTN_STATE_TRG;
      }
    }
    break;
  case BTN_STATE_TRG:  //�g���K��1�t���[�������`����
    {
      work->btn_state = BTN_STATE_MAIN;
      work->btn_push = BTN_PUSH_NONE;
    }
    break;
  }
}

// ��Ԃ̏�����
static void Manual_Touchbar_StateInit( MANUAL_TOUCHBAR_WORK* work )
{
  work->input_state             = INPUT_STATE_NONE;
  work->touch_icon              = TOUCHBAR_SELECT_NONE;            
  work->trg_icon                = TOUCHBAR_SELECT_NONE;            

  work->btn_push                = BTN_PUSH_NONE;
  work->btn_state               = BTN_STATE_MAIN;
}

