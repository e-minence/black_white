//============================================================================
/**
 *  @file   d_test.c
 *  @brief  �e�X�g
 *  @author Koji Kawada
 *  @data   2010.03.31
 *  @note   
 *  ���W���[�����FD_KAWADA_TEST, D_TEST
 */
//============================================================================
#define DEBUG_KAWADA  // ���ꂪ��`����Ă���Ƃ��A�f�o�b�O�o�͂��������肷��


#define HEAPID_D_TEST (HEAPID_SHINKA_DEMO)


// �C���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "print/gf_font.h"
#include "print/printsys.h"


// �o�g���̉����
#include "field/zonedata.h"
#include "tr_tool/trtype_def.h"
#include "battle/btlv/btlv_mcss.h"
#include "battle/btlv/btlv_input.h"
#include "battle/btlv/btlv_effect.h"


#include "d_test_graphic.h"
#include "d_test.h"

// �A�[�J�C�u
#include "arc_def.h"
#include "font/font.naix"
#include "message.naix"
#include "msg/msg_yesnomenu.h"

// �T�E���h

// �I�[�o�[���C


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================
#define HEAP_SIZE              (0xc0000)               ///< �q�[�v�T�C�Y

// ���C��BG�t���[��
#define BG_FRAME_M_FRONT       (GFL_BG_FRAME1_M)
#define BG_FRAME_M_TEXT        (GFL_BG_FRAME2_M)

// ���C��BG�t���[���̃v���C�I���e�B
#define BG_FRAME_PRI_M_FRONT   (1)
#define BG_FRAME_PRI_M_TEXT    (0)

// ���C��BG�p���b�g
// �{��
enum
{
  BG_PAL_NUM_M_GRA_FRONT     = 15,
  BG_PAL_NUM_M_TEXT          = 1,
};
// �ʒu
enum
{
  BG_PAL_POS_M_GRA_FRONT    =  0,
  BG_PAL_POS_M_TEXT         = 15,
};

// ���C��OBJ�p���b�g
// �{��
enum
{
  OBJ_PAL_NUM_M_BALL        = 2,
  OBJ_PAL_NUM_M_GF          = 1,
};
// �ʒu
enum
{
  OBJ_PAL_POS_M_BALL        = 0,
  OBJ_PAL_POS_M_GF          = 2,
};

// ProcMain�̃V�[�P���X
enum
{
  SEQ_START          = 0,
  SEQ_FADE_IN,
  SEQ_MAIN,


  // �o�g���̉����
  SEQ_BTLV,
 

  SEQ_FADE_OUT,
  SEQ_END,
};

// �e�L�X�g
enum
{
  TEXT_DUMMY,
  TEXT_BLACK,
  TEXT_NAME,
  TEXT_MAIN,
  TEXT_STAFF,
  TEXT_MAX,
};

// BG_PAL_POS_M_TEXT�̊��蓖��
#define TEXT_PAL_POS      (BG_PAL_POS_M_TEXT)
#define TEXT_COLOR_L      (1)  // ������F
#define TEXT_COLOR_S      (2)  // �����e�F
#define TEXT_COLOR_B      (0)  // �����w�i�F(����)
#define TEXT_COLOR_BLACK  (3)  // �X�N���[������ۂ̋󂫗̈�����ɂ��Ă���

// �����̐F
#define TEXT_COLOR_VALUE_L   (0x294B)
#define TEXT_COLOR_VALUE_S   (0x4A74)

static const u8 bmpwin_setup[TEXT_MAX][9] =
{
  // frmnum           posx  posy  sizx  sizy  palnum                dir                    x  y (x,y�͖������ăZ���^�����O���邱�Ƃ�����)
  {  BG_FRAME_M_TEXT,    0,    0,    1,    1, TEXT_PAL_POS,         GFL_BMP_CHRAREA_GET_F, 0, 0 },
  {  BG_FRAME_M_TEXT,    0,   24,    1,    1, TEXT_PAL_POS,         GFL_BMP_CHRAREA_GET_F, 0, 0 },
  {  BG_FRAME_M_TEXT,    1,    3,   30,    3, TEXT_PAL_POS,         GFL_BMP_CHRAREA_GET_F, 0, 0 },
  {  BG_FRAME_M_TEXT,    1,    7,   30,    9, TEXT_PAL_POS,         GFL_BMP_CHRAREA_GET_F, 0, 0 },
  {  BG_FRAME_M_TEXT,    1,   17,   30,    5, TEXT_PAL_POS,         GFL_BMP_CHRAREA_GET_F, 0, 0 },
};

// �t�F�[�h
#define FADE_IN_WAIT  (0)
#define FADE_OUT_WAIT (0)

// ������
#define TEXT_NAME_LEN_MAX    (32)  // EOM���܂߂�������

// �X�N���[��
#define SCROLL_START_POS_Y  (64)
#define SCROLL_WAIT         ( 0)  // 0�ȏ�̒l�B0�Ŗ��t���[���ړ��B1�ő҂��A�ړ��A�҂��A�ړ��B2�ő҂��A�҂��A�ړ��B
#define SCROLL_VALUE        ( 2)  // �ړ�����Ƃ��̈ړ��ʁB


// OBJ
enum
{
  OBJ_BALL_RES_NCG,
  OBJ_BALL_RES_NCL,
  OBJ_BALL_RES_NCE,
  OBJ_GF_RES_NCG,
  OBJ_GF_RES_NCL,
  OBJ_GF_RES_NCE,
  OBJ_RES_MAX,
};
// CELL
enum
{
  OBJ_BALL_CELL,
  OBJ_GF_CELL,
  OBJ_CELL_MAX,
};
#define OBJ_BALL_CELL_ANMSEQ  (1)
#define OBJ_GF_CELL_ANMSEQ    (0)
static const GFL_CLWK_DATA obj_cell_data[OBJ_CELL_MAX] =
{
  // pos_x, pos_y, anmseq,               softpri, bgpri
  {   14,    11,   OBJ_BALL_CELL_ANMSEQ, 0,       BG_FRAME_PRI_M_FRONT },
  {  175,   136,   OBJ_GF_CELL_ANMSEQ,   0,       BG_FRAME_PRI_M_FRONT },
};


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
  D_KAWADA_TEST_PARAM*        param;
  
  // �O���t�B�b�N�A�t�H���g�Ȃ�
  D_TEST_GRAPHIC_WORK*        graphic;
  GFL_FONT*                   font;
  PRINT_QUE*                  print_que;

  // VBlank��TCB
  GFL_TCB*                    vblank_tcb;

  // �T�u�V�[�P���X
  int                         sub_seq;
  int                         sub_main_step;


  // �o�g���̉����
  GFL_MSGDATA*                btlv_msgdata;
  u8                          btlv_cursor_flag;
  BTLV_INPUT_WORK*            btlv_input_work;


}
D_TEST_WORK;


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// VBlank�֐�
static void D_Test_VBlankFunc( GFL_TCB* tcb, void* wk );

// �o�g���̉����
static void D_Test_BtlvInit( D_TEST_WORK* work );
static void D_Test_BtlvExit( D_TEST_WORK* work );
static void D_Test_BtlvMain( D_TEST_WORK* work );


//=============================================================================
/**
*  PROC
*/
//=============================================================================
static GFL_PROC_RESULT D_Test_ProcInit( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static GFL_PROC_RESULT D_Test_ProcExit( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static GFL_PROC_RESULT D_Test_ProcMain( GFL_PROC* proc, int* seq, void* pwk, void* mywk );

const GFL_PROC_DATA    D_KAWADA_TEST_ProcData =
{
  D_Test_ProcInit,
  D_Test_ProcMain,
  D_Test_ProcExit,
};


//=============================================================================
/**
*  �O�����J�֐���`
*/
//=============================================================================
//------------------------------------------------------------------
/**
 *  @brief           �p�����[�^����
 *
 *  @param[in]       heap_id       �q�[�vID
 *  @param[in]       dummy
 *
 *  @retval          D_KAWADA_TEST_PARAM
 */
//------------------------------------------------------------------
D_KAWADA_TEST_PARAM*  D_KAWADA_TEST_AllocParam(
                                HEAPID               heap_id,
                                u32                  dummy
                           )
{
  D_KAWADA_TEST_PARAM* param = GFL_HEAP_AllocMemory( heap_id, sizeof( D_KAWADA_TEST_PARAM ) );
  D_KAWADA_TEST_InitParam(
      param,
      dummy );
  return param;
}

//------------------------------------------------------------------
/**
 *  @brief           �p�����[�^���
 *
 *  @param[in,out]   param      �p�����[�^�����Ő�����������
 *
 *  @retval          
 */
//------------------------------------------------------------------
void            D_KAWADA_TEST_FreeParam(
                            D_KAWADA_TEST_PARAM*  param )
{
  GFL_HEAP_FreeMemory( param );
}

//------------------------------------------------------------------
/**
 *  @brief           �p�����[�^��ݒ肷��
 *
 *  @param[in,out]   param         D_KAWADA_TEST_PARAM
 *  @param[in]       dummy
 *
 *  @retval          
 */
//------------------------------------------------------------------
void  D_KAWADA_TEST_InitParam(
                  D_KAWADA_TEST_PARAM*      param,
                  u32                       dummy
                         )
{
  param->dummy             = dummy;
}


//=============================================================================
/**
*  ���[�J���֐���`(PROC)
*/
//=============================================================================
//-------------------------------------
/// PROC ����������
//=====================================
static GFL_PROC_RESULT D_Test_ProcInit( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  D_TEST_WORK*     work;

#ifdef DEBUG_KAWADA
  {
    OS_Printf( "D_Test_ProcInit\n" );
  }
#endif

  // �q�[�v�A�p�����[�^�Ȃ�
  {
    GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_D_TEST, HEAP_SIZE );
    work = GFL_PROC_AllocWork( proc, sizeof(D_TEST_WORK), HEAPID_D_TEST );
    GFL_STD_MemClear( work, sizeof(D_TEST_WORK) );
    
    work->heap_id       = HEAPID_D_TEST;
    work->param         = (D_KAWADA_TEST_PARAM*)pwk;
  }

  // �O���t�B�b�N�A�t�H���g�Ȃ�
  {
    work->graphic       = D_TEST_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, work->heap_id );
    work->font          = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, work->heap_id );
    work->print_que     = PRINTSYS_QUE_Create( work->heap_id );
  }

  // VBlank��TCB
  work->vblank_tcb = GFUser_VIntr_CreateTCB( D_Test_VBlankFunc, work, 1 );

  // �T�u�V�[�P���X
  work->sub_seq = 0;

  // �t�F�[�h�C��(����������)
  GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, FADE_IN_WAIT );

  return GFL_PROC_RES_FINISH;
}

//-------------------------------------
/// PROC �I������
//=====================================
static GFL_PROC_RESULT D_Test_ProcExit( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  D_TEST_WORK*     work     = (D_TEST_WORK*)mywk;

  // VBlank��TCB
  GFL_TCB_DeleteTask( work->vblank_tcb );

  // �O���t�B�b�N�A�t�H���g�Ȃ�
  {
    PRINTSYS_QUE_Delete( work->print_que );
    GFL_FONT_Delete( work->font );
    D_TEST_GRAPHIC_Exit( work->graphic );
  }

  // �q�[�v�A�p�����[�^�Ȃ�
  {
    GFL_PROC_FreeWork( proc );
    GFL_HEAP_DeleteHeap( HEAPID_D_TEST );
  }

#ifdef DEBUG_KAWADA
  {
    OS_Printf( "D_Test_ProcExit\n" );
  }
#endif

  return GFL_PROC_RES_FINISH;
}

//-------------------------------------
/// PROC �又��
//=====================================
static GFL_PROC_RESULT D_Test_ProcMain( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  D_TEST_WORK*     work     = (D_TEST_WORK*)mywk;

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
      if( work->sub_seq == 0 )
      {
        *seq = SEQ_BTLV;
        work->sub_seq = 0;
      }
      else
      {
        BOOL b_end = FALSE;
        u32 x, y;
        if( GFL_UI_KEY_GetTrg() & ( PAD_BUTTON_A | PAD_BUTTON_B ) )
        {
          GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
          b_end = TRUE;
        }
        else if( GFL_UI_TP_GetPointTrg( &x, &y ) )
        {
          GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
          b_end = TRUE;
        }

        if( b_end )
        {
          *seq = SEQ_FADE_OUT;
          // �t�F�[�h�A�E�g(�����遨��)
          GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, FADE_OUT_WAIT );
        }
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


  // �o�g���̉����
  case SEQ_BTLV:
    {
      switch(work->sub_seq)
      {
      case 0:
        {
          D_Test_BtlvInit( work );
          work->sub_seq = 1;
        }
        break;
      case 1:
        {
          D_Test_BtlvMain( work );
          if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_START )
          {
            GFL_UI_SetTouchOrKey( GFL_APP_KTST_KEY );
            work->sub_seq = 2;
          } 
        }
        break;
      case 2:
        {
          D_Test_BtlvExit( work );
          *seq = SEQ_MAIN;
          work->sub_seq = 1;
        }
        break;
      }
    }
    break;


  // 
//  case SEQ_:
//    {
//    }
//    break;

  }

  PRINTSYS_QUE_Main( work->print_que );

  // 2D�`��
  D_TEST_GRAPHIC_2D_Draw( work->graphic );

  // 3D�`��
  //D_TEST_GRAPHIC_3D_StartDraw( work->graphic );
  //D_TEST_GRAPHIC_3D_EndDraw( work->graphic );

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
static void D_Test_VBlankFunc( GFL_TCB* tcb, void* wk )
{
  D_TEST_WORK* work = (D_TEST_WORK*)wk;
}


//-------------------------------------
/// �o�g���̉����
//=====================================
FS_EXTERN_OVERLAY(battle_view);
#include "../../battle/btlv/data/yesno_sel.cdat"
static void D_Test_BtlvInit( D_TEST_WORK* work )
{
#ifdef DEBUG_KAWADA
  {
    OS_Printf( "D_Test_BtlvInit\n" );
  }
#endif

  ZONEDATA_Open( work->heap_id );
  work->btlv_msgdata = GFL_MSG_Create(
                           GFL_MSG_LOAD_NORMAL,
                           ARCID_MESSAGE,
                           NARC_message_yesnomenu_dat,
                           work->heap_id );
  work->btlv_cursor_flag = ( GFL_UI_CheckTouchOrKey() == GFL_APP_KTST_KEY ) ? ( 1 ) : ( 0 );

  GFL_OVERLAY_Load(FS_OVERLAY_ID(battle_view));
  {
    BTL_FIELD_SITUATION bfs = { 
      0, 0, 0, 0, 0
    };
    u16 tr_type[] = { 
      TRTYPE_HERO, TRTYPE_HERO, 0xffff, 0xffff,
    };
    BTLV_EFFECT_SETUP_PARAM* besp = BTLV_EFFECT_MakeSetUpParam( BTL_RULE_SINGLE, &bfs, FALSE, tr_type, NULL, work->heap_id );
    BTLV_EFFECT_Init( besp, work->font, work->heap_id );
    GFL_HEAP_FreeMemory( besp );
  }
  work->btlv_input_work = BTLV_INPUT_InitEx( BTLV_INPUT_TYPE_SINGLE, BTLV_EFFECT_GetPfd(), work->font, &work->btlv_cursor_flag, work->heap_id );

  work->sub_main_step = 0;
}
static void D_Test_BtlvExit( D_TEST_WORK* work )
{
  BTLV_INPUT_Exit( work->btlv_input_work );
  BTLV_EFFECT_Exit();
  GFL_OVERLAY_Unload(FS_OVERLAY_ID(battle_view));
 
  GFL_UI_SetTouchOrKey( ( work->btlv_cursor_flag ) ? ( GFL_APP_KTST_KEY ) : ( GFL_APP_KTST_TOUCH ) );
  GFL_MSG_Delete( work->btlv_msgdata );
  ZONEDATA_Close();

#ifdef DEBUG_KAWADA
  {
    OS_Printf( "D_Test_BtlvExit\n" );
  }
#endif
}
static void D_Test_BtlvMain( D_TEST_WORK* work )
{
  if( work->sub_main_step == 0 )
  {
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_L )
    {
      BTLV_INPUT_YESNO_PARAM param;
      param.yes_msg = GFL_MSG_CreateString( work->btlv_msgdata, msgid_yesno_yes );
      param.no_msg = GFL_MSG_CreateString( work->btlv_msgdata, msgid_yesno_no );
      BTLV_INPUT_CreateScreen( work->btlv_input_work, BTLV_INPUT_SCRTYPE_YES_NO, &param );
      GFL_STR_DeleteBuffer( param.yes_msg );
      GFL_STR_DeleteBuffer( param.no_msg );
      work->sub_main_step++;
    }
    else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_R )
    {
      BTLV_INPUT_CreateScreen( work->btlv_input_work, BTLV_INPUT_SCRTYPE_STANDBY, NULL );
    }
  }
  else if( work->sub_main_step == 1 )
  {
    int input = BTLV_INPUT_CheckInput( work->btlv_input_work, &YesNoTouchData, YesNoKeyData );
    if( input != GFL_UI_TP_HIT_NONE )
    {
      BTLV_EFFECT_Stop();  // BTLV_INPUT_CheckInput�̌��ʂ�GFL_UI_TP_HIT_NONE�̂Ƃ��ABTLV_EFFECT_Add���Ă���̂ŁB
                           // BTLV_EFFECT_Main���񂵂Ă���Ή������邪�A�񂵂Ă��Ȃ��Ƃ��͉�����ꂸ�Ɏc���Ă��܂��B
      BTLV_INPUT_CreateScreen( work->btlv_input_work, BTLV_INPUT_SCRTYPE_STANDBY, NULL );
      work->sub_main_step = 0;
    }
  }

  //BTLV_EFFECT_Main();  // Init�͕K�v�����AMain�͂Ȃ��Ă����v�݂����BMain���񂷂Ə��ʂɂ��ڂ񂪕\������Ă��܂��B
  BTLV_INPUT_Main( work->btlv_input_work );
}


//-------------------------------------
/// 
//=====================================


