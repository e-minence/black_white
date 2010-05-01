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
#include "app/manual.h"

// �A�[�J�C�u
#include "arc_def.h"
#include "font/font.naix"
#include "message.naix"
//#include "msg/msg_????.h"
//#include "manual_gra????.naix"

// �T�E���h

// �I�[�o�[���C


#include "app/app_nogear_subscreen.h"


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================
#define HEAP_SIZE              (0x30000)               ///< �q�[�v�T�C�Y

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
  SEQ_FADE_OUT,
  SEQ_END,
};

// �e�L�X�g
enum
{
  TEXT_DUMMY,
  TEXT_MAX,
};

// BG_PAL_POS_M_TEXT�̊��蓖��
#define TEXT_PAL_POS      (BG_PAL_POS_M_TEXT)
#define TEXT_COLOR_L      (1)  // ������F
#define TEXT_COLOR_S      (2)  // �����e�F
#define TEXT_COLOR_B      (0)  // �����w�i�F(����)

static const u8 bmpwin_setup[TEXT_MAX][9] =
{
  // frmnum           posx  posy  sizx  sizy  palnum          dir                    x  y (x,y�͖������ăZ���^�����O���邱�Ƃ�����)
  {  BG_FRAME_M_TEXT,    0,    0,    1,    1, TEXT_PAL_POS,   GFL_BMP_CHRAREA_GET_F, 0, 0 },
};

// �t�F�[�h
#define FADE_IN_WAIT  (0)
#define FADE_OUT_WAIT (0)


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
  
  // �O���t�B�b�N�A�t�H���g�Ȃ�
  MANUAL_GRAPHIC_WORK*        graphic;
  GFL_FONT*                   font;
  PRINT_QUE*                  print_que;

  // VBlank��TCB
  GFL_TCB*                    vblank_tcb;

  // �e�L�X�g
  GFL_MSGDATA*                msgdata;
  GFL_BMPWIN*                 text_bmpwin[TEXT_MAX];
  BOOL                        text_trans[TEXT_MAX];  // bmpwin�̓]���̕K�v������ꍇTRUE
}
MANUAL_WORK;


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// VBlank�֐�
static void Manual_VBlankFunc( GFL_TCB* tcb, void* wk );


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
  }

  // �O���t�B�b�N�A�t�H���g�Ȃ�
  {
    work->graphic       = MANUAL_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, work->heap_id );
    work->font          = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, work->heap_id );
    work->print_que     = PRINTSYS_QUE_Create( work->heap_id );
  }

  // ���C��BG
  GFL_BG_SetPriority( BG_FRAME_M_FRONT,  BG_FRAME_PRI_M_FRONT );
  GFL_BG_SetPriority( BG_FRAME_M_TEXT,   BG_FRAME_PRI_M_TEXT  );

  // VBlank��TCB
  work->vblank_tcb = GFUser_VIntr_CreateTCB( Manual_VBlankFunc, work, 1 );

  // �T�uBG
  {
    const MYSTATUS*  mystatus  = GAMEDATA_GetMyStatus( work->param->gamedata );
    APP_NOGEAR_SUBSCREEN_Init();
    APP_NOGEAR_SUBSCREEN_Trans( work->heap_id, mystatus->sex );  // PM_MALE or PM_FEMALE  // include/pm_version.h
  }

  // �t�F�[�h�C��(����������)
  GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, FADE_IN_WAIT );

  // �ʐM�A�C�R��
  GFL_NET_WirelessIconEasy_HoldLCD( FALSE, work->heap_id );
  GFL_NET_ReloadIcon();

  return GFL_PROC_RES_FINISH;
}

//-------------------------------------
/// PROC �I������
//=====================================
static GFL_PROC_RESULT Manual_ProcExit( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  MANUAL_WORK* work = (MANUAL_WORK*)mywk;

  // �ʐM�A�C�R��
  GFL_NET_WirelessIconEasy_DefaultLCD();

  // �T�uBG
  APP_NOGEAR_SUBSCREEN_Exit();

  // VBlank��TCB
  GFL_TCB_DeleteTask( work->vblank_tcb );

  // �O���t�B�b�N�A�t�H���g�Ȃ�
  {
    PRINTSYS_QUE_Clear( work->print_que );
    PRINTSYS_QUE_Delete( work->print_que );
    GFL_FONT_Delete( work->font );
    MANUAL_GRAPHIC_Exit( work->graphic );
  }

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

  PRINTSYS_QUE_Main( work->print_que );

  // 2D�`��
  MANUAL_GRAPHIC_2D_Draw( work->graphic );

  // 3D�`��
  //MANUAL_GRAPHIC_3D_StartDraw( work->graphic );
  //MANUAL_GRAPHIC_3D_EndDraw( work->graphic );

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

