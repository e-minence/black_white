//============================================================================
/**
 *  @file   the_end.c
 *  @brief  �uTHE END�v�Əo�������̃A�v��
 *  @author Akito Mori
 *  @data   2010.04.30
 *  @note   
 *  ���W���[�����FTHE_END
 */
//============================================================================
// �C���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "print/gf_font.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "app/app_nogear_subscreen.h"

#include "the_end_graphic.h"
#include "demo/the_end.h"

// �A�[�J�C�u
#include "arc_def.h"
#include "font/font.naix"
#include "script_message.naix"
#include "staff_roll.naix"
#include "title/title_res.h"

// �T�E���h

// �I�[�o�[���C


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================
#define HEAP_SIZE              (0x30000)               ///< �q�[�v�T�C�Y

// ���C��BG�t���[��
#define BG_FRAME_M_REAR       (GFL_BG_FRAME1_M)
#define BG_FRAME_M_LOGO       (GFL_BG_FRAME2_M)

// ���C��BG�t���[���̃v���C�I���e�B
#define BG_FRAME_PRI_M_REAR    (1)
#define BG_FRAME_PRI_M_FRONT   (0)

// ���C��BG�p���b�g
// �{��
enum
{
  BG_PAL_NUM_M_GRA             = 0,  // �S�ē]��  // REAR, FRONT���ʃp���b�g
};
// �ʒu
enum
{
  BG_PAL_POS_M_GRA            =  0,  // REAR, FRONT���ʃp���b�g
};

// ���C��OBJ�p���b�g
// �{��
enum
{
  OBJ_PAL_NUM_M_        = 0,
};
// �ʒu
enum
{
  OBJ_PAL_POS_M_        = 0,
};

// ProcMain�̃V�[�P���X
enum
{
  SEQ_START          = 0,
  SEQ_FADE_IN,
  SEQ_MAIN,
  SEQ_FADE_OUT,
  SEQ_FADE_IN2,
  SEQ_MAIN2,
  SEQ_FADE_OUT2,
  SEQ_END,
};

// �t�F�[�h
#define FADE_IN_WAIT  (0)
#define FADE_OUT_WAIT (0)

#define THE_END_APPEAR_WAIT ( 240 )
#define LOGO_APPEAR_WAIT    ( 240 )

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
  HEAPID                heap_id;
  
  // �O���t�B�b�N�A�t�H���g�Ȃ�
  THE_END_GRAPHIC_WORK* graphic;
  GFL_FONT*             font;
  PRINT_QUE*            print_que;
  THE_END_PARAM*        param;
  // VBlank��TCB
  GFL_TCB*              vblank_tcb;
  int                   wait;
}
THE_END_WORK;


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// VBlank�֐�
static void TheEnd_VBlankFunc( GFL_TCB* tcb, void* wk );

// BG
static void TheEnd_BgInit( THE_END_WORK* work );
static void TheEnd_BgExit( THE_END_WORK* work );
static void TheEnd_LogoInit( THE_END_WORK* work );

static void StartBrightnessFade( int start, int end, int fadetype );


//=============================================================================
/**
*  PROC
*/
//=============================================================================
static GFL_PROC_RESULT TheEnd_ProcInit( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static GFL_PROC_RESULT TheEnd_ProcExit( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static GFL_PROC_RESULT TheEnd_ProcMain( GFL_PROC* proc, int* seq, void* pwk, void* mywk );

const GFL_PROC_DATA    THE_END_ProcData =
{
  TheEnd_ProcInit,
  TheEnd_ProcMain,
  TheEnd_ProcExit,
};


//=============================================================================
/**
*  �O�����J�֐���`
*/
//=============================================================================


//=============================================================================
/**
*  ���[�J���֐���`(PROC)
*/
//=============================================================================
//-------------------------------------
/// PROC ����������
//=====================================
static GFL_PROC_RESULT TheEnd_ProcInit( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  THE_END_WORK*  work;
  const u16 pal_white[]={0xffff};    //�����0�ԃp���b�g�ׂ��p�f�[�^

  // �q�[�v�A�p�����[�^�Ȃ�
  {
    GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_SUBWAY_MAP, HEAP_SIZE );
    work = GFL_PROC_AllocWork( proc, sizeof(THE_END_WORK), HEAPID_SUBWAY_MAP );
    GFL_STD_MemClear( work, sizeof(THE_END_WORK) );
    
    work->heap_id       = HEAPID_SUBWAY_MAP;
    work->param         = (THE_END_PARAM*)pwk;
  }

  // �O���t�B�b�N�A�t�H���g�Ȃ�
  {
    work->graphic       = THE_END_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, work->heap_id );
    work->font          = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, work->heap_id );
    work->print_que     = PRINTSYS_QUE_Create( work->heap_id );
  }

  // ���C��BG
  GFL_BG_SetPriority( BG_FRAME_M_REAR,   BG_FRAME_PRI_M_REAR  );
  GFL_BG_SetPriority( BG_FRAME_M_LOGO,  BG_FRAME_PRI_M_FRONT );

  // VBlank��TCB
  work->vblank_tcb = GFUser_VIntr_CreateTCB( TheEnd_VBlankFunc, work, 1 );

  // ����
  TheEnd_BgInit( work );
//  TheEnd_LogoInit(work);

  // �z���C�g�o�[�W�����ł͉���ʂ𔒂�����
  if(GetVersion()==VERSION_WHITE){
    GXS_LoadBGPltt( pal_white, 0, 2);
  }

  // �t�F�[�h�C��(��������������)
  StartBrightnessFade( 16, 0, FADE_IN_WAIT );

  // �ʐM�A�C�R��
  GFL_NET_WirelessIconEasy_HoldLCD( FALSE, work->heap_id );
  GFL_NET_ReloadIcon();

  return GFL_PROC_RES_FINISH;
}

//-------------------------------------
/// PROC �I������
//=====================================
static GFL_PROC_RESULT TheEnd_ProcExit( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  THE_END_WORK*     work     = (THE_END_WORK*)mywk;

  // �ʐM�A�C�R��
  GFL_NET_WirelessIconEasy_DefaultLCD();

  // �T�uBG
  APP_NOGEAR_SUBSCREEN_Exit();

  // �j��
  TheEnd_BgExit( work );

  // VBlank��TCB
  GFL_TCB_DeleteTask( work->vblank_tcb );

  // �O���t�B�b�N�A�t�H���g�Ȃ�
  {
    PRINTSYS_QUE_Clear( work->print_que );
    PRINTSYS_QUE_Delete( work->print_que );
    GFL_FONT_Delete( work->font );
    THE_END_GRAPHIC_Exit( work->graphic );
  }

  // �q�[�v�A�p�����[�^�Ȃ�
  {
    GFL_PROC_FreeWork( proc );
    GFL_HEAP_DeleteHeap( HEAPID_SUBWAY_MAP );
  }

  return GFL_PROC_RES_FINISH;
}

//-------------------------------------
/// PROC �又��
//=====================================
static GFL_PROC_RESULT TheEnd_ProcMain( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  THE_END_WORK*     work     = (THE_END_WORK*)mywk;

  switch(*seq)
  {
  case SEQ_START:
    {
      *seq = SEQ_FADE_IN2;
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
/*
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
*/
      if(++work->wait>THE_END_APPEAR_WAIT){
        b_end = TRUE;
      }

      if( b_end )
      {
        *seq = SEQ_FADE_OUT;
        // �t�F�[�h�A�E�g(�����遨��)
        StartBrightnessFade( 0, 16, FADE_OUT_WAIT );

      }
    }
    break;
  case SEQ_FADE_OUT:
    {
      if( !GFL_FADE_CheckFade() )
      {
        TheEnd_BgInit(work);
        // �t�F�[�h�C��(����������)
        StartBrightnessFade( 16, 0, FADE_IN_WAIT );
        *seq = SEQ_FADE_IN2;
      }
    }
    break;
  case SEQ_FADE_IN2:
    if( !GFL_FADE_CheckFade() )
    {
      work->wait = 0;
      *seq = SEQ_MAIN2;
    }
    break;
  case SEQ_MAIN2:
    if(++work->wait>THE_END_APPEAR_WAIT){
      *seq = SEQ_FADE_OUT2;
      // �t�F�[�h�A�E�g(�����遨��)
        StartBrightnessFade( 0, 16, FADE_OUT_WAIT );
    }
    break;
  case SEQ_FADE_OUT2:
    if( !GFL_FADE_CheckFade() )
    {
      *seq = SEQ_END;
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
  THE_END_GRAPHIC_2D_Draw( work->graphic );

  // 3D�`��
  //THE_END_GRAPHIC_3D_StartDraw( work->graphic );
  //THE_END_GRAPHIC_3D_EndDraw( work->graphic );

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
static void TheEnd_VBlankFunc( GFL_TCB* tcb, void* wk )
{
  THE_END_WORK* work = (THE_END_WORK*)wk;
}

//-------------------------------------
/// BG
//=====================================
static void TheEnd_BgInit( THE_END_WORK* work )
{
  ARCHANDLE* handle = GFL_ARC_OpenDataHandle( ARCID_STAFF_ROLL, work->heap_id );

  // ����
  GFL_ARCHDL_UTIL_TransVramPalette(
      handle,
      NARC_staff_roll_end_nclr,
      PALTYPE_MAIN_BG,
      BG_PAL_POS_M_GRA * 0x20,
      BG_PAL_NUM_M_GRA * 0x20,
      work->heap_id );

  GFL_ARCHDL_UTIL_TransVramBgCharacter(
      handle,
      NARC_staff_roll_end_ncgr,
      BG_FRAME_M_REAR,  // �L�����̈ʒu��BG_FRAME_M_FRONT�������ɂ��Ă���
      0,
      0,  // �S�]��
      FALSE,
      work->heap_id );

  // �Ђ炪�ȃ��[�h�Ȃ�u�����v�A�������[�h�ł́uTHE END�v
  if(work->param->mojiMode==MOJIMODE_HIRAGANA){
    // REAR
    GFL_ARCHDL_UTIL_TransVramScreen(
        handle,
        NARC_staff_roll_end_nscr,
        BG_FRAME_M_REAR,
        0,
        0,  // �S�]��
        FALSE,
        work->heap_id );
  }else{
    GFL_ARCHDL_UTIL_TransVramScreen(
        handle,
        NARC_staff_roll_end_e_nscr,
        BG_FRAME_M_REAR,
        0,
        0,  // �S�]��
        FALSE,
        work->heap_id );
    
  }

  // ���o�[�W�����̎��͔w�i���̍������ɂ���
  if(GetVersion()== VERSION_WHITE){
    GFL_BG_ChangeScreenPalette( BG_FRAME_M_REAR, 0, 0, 32, 24, 1 );
  }

  GFL_ARC_CloseDataHandle( handle );
  GFL_BG_LoadScreenReq( BG_FRAME_M_REAR );

  GFL_BG_SetVisible( BG_FRAME_M_REAR, VISIBLE_ON );
  GFL_BG_SetVisible( BG_FRAME_M_LOGO, VISIBLE_OFF );

}
static void TheEnd_BgExit( THE_END_WORK* work )
{
  // �������Ȃ�
}


//----------------------------------------------------------------------------------
/**
 * @brief �^�C�g�����S�O���t�B�b�N��ǂݍ���
 *
 * @param   work    
 */
//----------------------------------------------------------------------------------
static void TheEnd_LogoInit( THE_END_WORK* work )
{
  int version = GetVersion()-VERSION_WHITE;
  ARCHANDLE* handle = GFL_ARC_OpenDataHandle( TITLE_RES_ARCID, work->heap_id );

  // ����
  GFL_ARCHDL_UTIL_TransVramPalette(
      handle,
      TITLE_RES_LOGO_NCLR,
      PALTYPE_MAIN_BG,
      BG_PAL_POS_M_GRA * 0x20,
      BG_PAL_NUM_M_GRA * 0x20,
      work->heap_id );

  GFL_ARCHDL_UTIL_TransVramBgCharacter(
      handle,
      TITLE_RES_LOGO_NCGR,
      BG_FRAME_M_LOGO,  
      0,
      0,  // �S�]��
      TRUE,
      work->heap_id );

  GFL_ARCHDL_UTIL_TransVramScreen(
      handle,
      TITLE_RES_LOGO_NSCR,
      BG_FRAME_M_LOGO,
      0,
      0,  // �S�]��
      TRUE,
      work->heap_id );
   
  GFL_ARC_CloseDataHandle( handle );
  
  GFL_BG_SetVisible( BG_FRAME_M_LOGO, VISIBLE_ON );
  GFL_BG_SetVisible( BG_FRAME_M_REAR, VISIBLE_OFF );

}


//----------------------------------------------------------------------------------
/**
 * @brief �J�Z�b�g�o�[�W�����Ńu���b�N�A�E�g�ƃz���C�g�A�E�g��؂�ւ���
 *
 * @param   start     �t�F�[�h�X�^�[�g�l
 * @param   end       �t�F�[�h�I���l
 * @param   fadetype  �t�F�[�h�^�C�v
 */
//----------------------------------------------------------------------------------
static void StartBrightnessFade( int start, int end, int fadetype )
{
  if(GetVersion()==VERSION_BLACK){
    GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, start, end, fadetype );
  }else{
    GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_WHITEOUT, start, end, fadetype );
  }
}
