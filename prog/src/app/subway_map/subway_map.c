//============================================================================
/**
 *  @file   subway_map.c
 *  @brief  �n���S�H���}
 *  @author Koji Kawada
 *  @data   2010.03.15
 *  @note   
 *  ���W���[�����FSUBWAY_MAP
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

#include "subway_map_graphic.h"
#include "app/subway_map.h"

// �A�[�J�C�u
#include "arc_def.h"
#include "font/font.naix"
#include "script_message.naix"
#include "subway_map_gra.naix"

// �T�E���h

// �I�[�o�[���C


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================
#define HEAP_SIZE              (0x30000)               ///< �q�[�v�T�C�Y

// ���C��BG�t���[��
#define BG_FRAME_M_REAR        (GFL_BG_FRAME1_M)
#define BG_FRAME_M_FRONT       (GFL_BG_FRAME2_M)

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
  SEQ_END,
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
  SUBWAY_MAP_PARAM*   param;
  
  // �O���t�B�b�N�A�t�H���g�Ȃ�
  SUBWAY_MAP_GRAPHIC_WORK*      graphic;
  GFL_FONT*                             font;
  PRINT_QUE*                            print_que;

  // VBlank��TCB
  GFL_TCB*                    vblank_tcb;
}
SUBWAY_MAP_WORK;


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// VBlank�֐�
static void Subway_Map_VBlankFunc( GFL_TCB* tcb, void* wk );

// BG
static void Subway_Map_BgInit( SUBWAY_MAP_WORK* work );
static void Subway_Map_BgExit( SUBWAY_MAP_WORK* work );


//=============================================================================
/**
*  PROC
*/
//=============================================================================
static GFL_PROC_RESULT Subway_Map_ProcInit( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static GFL_PROC_RESULT Subway_Map_ProcExit( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static GFL_PROC_RESULT Subway_Map_ProcMain( GFL_PROC* proc, int* seq, void* pwk, void* mywk );

const GFL_PROC_DATA    SUBWAY_MAP_ProcData =
{
  Subway_Map_ProcInit,
  Subway_Map_ProcMain,
  Subway_Map_ProcExit,
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
 *  @param[in]       mystatus      ������ԃf�[�^(���ʂ��g�p)
 *
 *  @retval          SUBWAY_MAP_PARAM
 */
//------------------------------------------------------------------
SUBWAY_MAP_PARAM*  SUBWAY_MAP_AllocParam(
                                HEAPID               heap_id,
                                const MYSTATUS*      mystatus
                           )
{
  SUBWAY_MAP_PARAM* param = GFL_HEAP_AllocMemory( heap_id, sizeof( SUBWAY_MAP_PARAM ) );
  SUBWAY_MAP_InitParam(
      param,
      mystatus );
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
void            SUBWAY_MAP_FreeParam(
                            SUBWAY_MAP_PARAM*  param )
{
  GFL_HEAP_FreeMemory( param );
}

//------------------------------------------------------------------
/**
 *  @brief           �p�����[�^��ݒ肷��
 *
 *  @param[in,out]   param         SUBWAY_MAP_PARAM
 *  @param[in]       mystatus      ������ԃf�[�^(���ʂ��g�p)
 *
 *  @retval          
 */
//------------------------------------------------------------------
void  SUBWAY_MAP_InitParam(
                  SUBWAY_MAP_PARAM*      param,
                  const MYSTATUS*                mystatus
                         )
{
  param->mystatus          = mystatus;
}


//=============================================================================
/**
*  ���[�J���֐���`(PROC)
*/
//=============================================================================
//-------------------------------------
/// PROC ����������
//=====================================
static GFL_PROC_RESULT Subway_Map_ProcInit( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  SUBWAY_MAP_WORK*     work;

  // �q�[�v�A�p�����[�^�Ȃ�
  {
    GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_SUBWAY_MAP, HEAP_SIZE );
    work = GFL_PROC_AllocWork( proc, sizeof(SUBWAY_MAP_WORK), HEAPID_SUBWAY_MAP );
    GFL_STD_MemClear( work, sizeof(SUBWAY_MAP_WORK) );
    
    work->heap_id       = HEAPID_SUBWAY_MAP;
    work->param         = (SUBWAY_MAP_PARAM*)pwk;
  }

  // �O���t�B�b�N�A�t�H���g�Ȃ�
  {
    work->graphic       = SUBWAY_MAP_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, work->heap_id );
    work->font          = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, work->heap_id );
    work->print_que     = PRINTSYS_QUE_Create( work->heap_id );
  }

  // ���C��BG
  GFL_BG_SetPriority( BG_FRAME_M_REAR,   BG_FRAME_PRI_M_REAR  );
  GFL_BG_SetPriority( BG_FRAME_M_FRONT,  BG_FRAME_PRI_M_FRONT );

  // VBlank��TCB
  work->vblank_tcb = GFUser_VIntr_CreateTCB( Subway_Map_VBlankFunc, work, 1 );

  // ����
  Subway_Map_BgInit( work );

  // �T�uBG
  APP_NOGEAR_SUBSCREEN_Init();
  APP_NOGEAR_SUBSCREEN_Trans( work->heap_id, work->param->mystatus->sex );  // PM_MALE or PM_FEMALE  // include/pm_version.h

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
static GFL_PROC_RESULT Subway_Map_ProcExit( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  SUBWAY_MAP_WORK*     work     = (SUBWAY_MAP_WORK*)mywk;

  // �ʐM�A�C�R��
  GFL_NET_WirelessIconEasy_DefaultLCD();

  // �T�uBG
  APP_NOGEAR_SUBSCREEN_Exit();

  // �j��
  Subway_Map_BgExit( work );

  // VBlank��TCB
  GFL_TCB_DeleteTask( work->vblank_tcb );

  // �O���t�B�b�N�A�t�H���g�Ȃ�
  {
    PRINTSYS_QUE_Clear( work->print_que );
    PRINTSYS_QUE_Delete( work->print_que );
    GFL_FONT_Delete( work->font );
    SUBWAY_MAP_GRAPHIC_Exit( work->graphic );
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
static GFL_PROC_RESULT Subway_Map_ProcMain( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  SUBWAY_MAP_WORK*     work     = (SUBWAY_MAP_WORK*)mywk;

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
  SUBWAY_MAP_GRAPHIC_2D_Draw( work->graphic );

  // 3D�`��
  //SUBWAY_MAP_GRAPHIC_3D_StartDraw( work->graphic );
  //SUBWAY_MAP_GRAPHIC_3D_EndDraw( work->graphic );

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
static void Subway_Map_VBlankFunc( GFL_TCB* tcb, void* wk )
{
  SUBWAY_MAP_WORK* work = (SUBWAY_MAP_WORK*)wk;
}

//-------------------------------------
/// BG
//=====================================
static void Subway_Map_BgInit( SUBWAY_MAP_WORK* work )
{
  ARCHANDLE* handle = GFL_ARC_OpenDataHandle( ARCID_SUBWAY_MAP_GRA, work->heap_id );

  // ����
  GFL_ARCHDL_UTIL_TransVramPalette(
      handle,
      NARC_subway_map_gra_subwaymap_NCLR,
      PALTYPE_MAIN_BG,
      BG_PAL_POS_M_GRA * 0x20,
      BG_PAL_NUM_M_GRA * 0x20,
      work->heap_id );

  GFL_ARCHDL_UTIL_TransVramBgCharacter(
      handle,
      NARC_subway_map_gra_subwaymap_NCGR,
      BG_FRAME_M_REAR,  // �L�����̈ʒu��BG_FRAME_M_FRONT�������ɂ��Ă���
			0,
      0,  // �S�]��
      FALSE,
      work->heap_id );

  // REAR
  GFL_ARCHDL_UTIL_TransVramScreen(
      handle,
      NARC_subway_map_gra_subwaymap_base_NSCR,
      BG_FRAME_M_REAR,
      0,
      0,  // �S�]��
      FALSE,
      work->heap_id );

  // FRONT
  GFL_ARCHDL_UTIL_TransVramScreen(
      handle,
      NARC_subway_map_gra_subwaymap_root_NSCR,
      BG_FRAME_M_FRONT,
      0,
      0,  // �S�]��
      FALSE,
      work->heap_id );

  GFL_ARC_CloseDataHandle( handle );

  GFL_BG_LoadScreenReq( BG_FRAME_M_REAR );
  GFL_BG_LoadScreenReq( BG_FRAME_M_FRONT );
}
static void Subway_Map_BgExit( SUBWAY_MAP_WORK* work )
{
  // �������Ȃ�
}

