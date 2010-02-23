//============================================================================
/**
 *  @file   egg_demo.c
 *  @brief  �^�}�S�z���f��
 *  @author Koji Kawada
 *  @data   2010.01.25
 *  @note   
 *  ���W���[�����FEGG_DEMO
 */
//============================================================================
#define HEAPID_EGG_DEMO (HEAPID_SHINKA_DEMO)


// �C���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "gamesystem/gamedata_def.h"
#include "gamesystem/msgspeed.h"
#include "system/palanm.h"
#include "system/bmp_winframe.h"
#include "system/bmp_menu.h"
#include "print/gf_font.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "poke_tool/poke_tool.h"
#include "sound/pm_sndsys.h"
#include "app/name_input.h"

#include "egg_demo_graphic.h"
#include "egg_demo_view.h"
#include "demo/egg_demo.h"


// �A�[�J�C�u
#include "arc_def.h"
#include "font/font.naix"
#include "script_message.naix"
#include "msg/script/msg_egg_event.h"
// �T�E���h
#include "sound/wb_sound_data.sadl"


// �I�[�o�[���C


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================
#define HEAP_SIZE              (0x50000)               ///< �q�[�v�T�C�Y

// BG�t���[��
#define BG_FRAME_M_BACK              (GFL_BG_FRAME2_M)        // �v���C�I���e�B2
#define BG_FRAME_M_POKEMON           (GFL_BG_FRAME0_M)        // �v���C�I���e�B1
#define BG_FRAME_M_TEXT              (GFL_BG_FRAME1_M)        // �v���C�I���e�B0

#define BG_FRAME_S_BACK              (GFL_BG_FRAME0_S)        // �v���C�I���e�B0

// BG�p���b�g
// �{��
enum
{
  BG_PAL_NUM_M_BACKDROP      = 1,
  BG_PAL_NUM_M_TEXT_FONT     = 1,
  BG_PAL_NUM_M_TEXT_FRAME    = 1,
  BG_PAL_NUM_M_TM            = 1,  // �g�p����
  BG_PAL_NUM_M_BACK          = 1,
};
// �ʒu
enum
{
  BG_PAL_POS_M_BACKDROP      = 0,                                                         // 0
  BG_PAL_POS_M_TEXT_FONT     = BG_PAL_POS_M_BACKDROP    + BG_PAL_NUM_M_BACKDROP        ,  // 1
  BG_PAL_POS_M_TEXT_FRAME    = BG_PAL_POS_M_TEXT_FONT   + BG_PAL_NUM_M_TEXT_FONT       ,  // 2
  BG_PAL_POS_M_TM            = BG_PAL_POS_M_TEXT_FRAME  + BG_PAL_NUM_M_TEXT_FRAME      ,  // 3  // �g�p����
  BG_PAL_POS_M_BACK          = BG_PAL_POS_M_TM          + BG_PAL_NUM_M_TM              ,  // 4
  BG_PAL_POS_M_MAX           = BG_PAL_POS_M_BACK        + BG_PAL_NUM_M_BACK            ,  // 5  // ���������
};
// �{��
enum
{
  BG_PAL_NUM_S_BACK          = 1,
};
// �ʒu
enum
{
  BG_PAL_POS_S_BACK          = 0                                                      ,  // 0
  BG_PAL_POS_S_MAX           = BG_PAL_POS_S_BACK        + BG_PAL_NUM_S_BACK           ,  // 1  // ���������
};

// OBJ�p���b�g
// �{��
enum
{
  OBJ_PAL_NUM_M_             = 1,
};
// �ʒu
enum
{
  OBJ_PAL_POS_M_MAX          = 0                                                      ,       // ���������
};
// �{��
enum
{
  OBJ_PAL_NUM_S_             = 1,
};
// �ʒu
enum
{
  OBJ_PAL_POS_S_MAX          = 0                                                      ,      // ���������
};

// TEXT
#define TEXT_WININ_BACK_COLOR        (15)

// �t�F�[�h
#define FADE_IN_WAIT           (2)         ///< �t�F�[�h�C���̃X�s�[�h
#define FADE_OUT_WAIT          (2)         ///< �t�F�[�h�A�E�g�̃X�s�[�h

#define NAMEIN_FADE_OUT_WAIT   (0)         ///< ���O���͂ֈڍs���邽�߂̃t�F�[�h�A�E�g�̃X�s�[�h

// ������΂����o�̂��߂̃p���b�g�t�F�[�h
#define PFADE_TCBSYS_TASK_MAX  (8)
#define PFADE_WAIT_TO_WHITE    (2)
#define PFADE_WAIT_FROM_WHITE  (4)

// �㉺�ɍ��т�\�����邽�߂�wnd
#define WND_UP_Y_APPEAR   ( 20)
#define WND_DOWN_Y_APPEAR (130)

// �͂��E�������E�B���h�E
typedef enum
{
  TM_RESULT_NO,
  TM_RESULT_YES,
  TM_RESULT_SEL,
}
TM_RESULT;

// ������
#define STRBUF_LENGTH                (256)  // ���̕������ő���邩buflen.h�ŗv�m�F
#define NAMEIN_STRBUF_LENGTH         ( 32)  // ���̕������ő���邩buflen.h�ŗv�m�F

// �����X�e�b�v
typedef enum
{
  TRUNK_STEP_FADE_IN,
  TRUNK_STEP_SOUND_INTRO,
  TRUNK_STEP_DEMO_EGG,
  TRUNK_STEP_DEMO_TO_WHITE,
  TRUNK_STEP_DEMO_BRIGHT_TO_WHITE,
  TRUNK_STEP_DEMO_WHITE,
  TRUNK_STEP_DEMO_BRIGHT_WHITE,
  TRUNK_STEP_DEMO_MON,
  TRUNK_STEP_TEXT_0,
  TRUNK_STEP_TEXT_1,
  TRUNK_STEP_TM,
  TRUNK_STEP_NAMEIN_FADE_OUT,
  TRUNK_STEP_NAMEIN,
  TRUNK_STEP_NAMEIN_BLACK,
  TRUNK_STEP_NAMEIN_END,
  TRUNK_STEP_FADE_OUT,
  TRUNK_STEP_END,
}
TRUNK_STEP;

// �T�E���h�X�e�b�v
typedef enum
{
  SOUND_STEP_WAIT,
  SOUND_STEP_INTRO,
  SOUND_STEP_HATCH,
  SOUND_STEP_CONGRATULATE,
}
SOUND_STEP;

// TEXT�̃X�e�b�v
typedef enum
{
  TEXT_STEP_WAIT,
}
TEXT_STEP;

// �͂��E�������E�B���h�E�̃X�e�b�v
typedef enum
{
  TM_STEP_WAIT,
  TM_STEP_SELECT,
  TM_STEP_END,
}
TM_STEP;

// ������΂����o�̂��߂̃p���b�g�t�F�[�h�̃X�e�b�v
typedef enum
{
  PFADE_STEP_WAIT,
}
PFADE_STEP;

// �㉺�ɍ��т�\�����邽�߂�wnd�̃X�e�b�v
typedef enum
{
  WND_STEP_WAIT,
}
WND_STEP;


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
  // �O���t�B�b�N�A�t�H���g�Ȃ�
  HEAPID                      heap_id;
  EGG_DEMO_GRAPHIC_WORK*      graphic;
  GFL_FONT*                   font;
  PRINT_QUE*                  print_que;

  // �X�e�b�v
  TRUNK_STEP                  trunk_step;
  SOUND_STEP                  sound_step;
  TEXT_STEP                   text_step;
  TM_STEP                     tm_step;
  PFADE_STEP                  pfade_step;
  WND_STEP                    wnd_step;

  // VBlank��TCB
  GFL_TCB*                    vblank_tcb;

  // �P��F�A�P��L������BG
  GFL_ARCUTIL_TRANSINFO       sim_transinfo;  // ARCUTIL�͎g�p���Ă��Ȃ����A�ʒu�ƃT�C�Y���ЂƂ܂Ƃ߂ɂ����������̂ŁB

  // TEXT
  PRINT_STREAM*               text_stream;
  GFL_TCBLSYS*                text_tcblsys;
  GFL_BMPWIN*                 text_winin_bmpwin;
  GFL_BMPWIN*                 text_dummy_bmpwin;      ///< 0�Ԃ̃L�����N�^�[��1x1�ł����Ă���
  GFL_ARCUTIL_TRANSINFO       text_winfrm_tinfo;
  GFL_MSGDATA*                text_msgdata_egg;
  STRBUF*                     text_strbuf;

  // �͂��E�������E�B���h�E
  BMPWIN_YESNO_DAT            tm_dat;
  BMPMENU_WORK*               tm_wk;
  TM_RESULT                   tm_result;

  // ������΂����o�̂��߂̃p���b�g�t�F�[�h
  GFL_TCBSYS*                 pfade_tcbsys;
  void*                       pfade_tcbsys_wk;
  PALETTE_FADE_PTR            pfade_ptr;

  // �㉺�ɍ��т�\�����邽�߂�wnd
  u8                          wnd_up_y;    // wnd_up_y <= ������s�N�Z�� < wnd_down_y
  u8                          wnd_down_y;  //        0 <= ������s�N�Z�� < 192
  s16                         wnd_appear_speed;    // 1��̈ړ��ŉ��s�N�Z���ړ����邩(0=�ړ����Ȃ��A+=���т������Ă���A-=���т��͂��Ă���)
  u8                          wnd_appear_wait;     // 1��̈ړ�������̂ɉ��t���[���҂�(0=���t���[���ړ��A1=1�t���[�������Ɉړ��A2=3�t���[����1��ړ�)
  u8                          wnd_count;

  // �^�}�S�z���f���̉��o
  EGG_DEMO_VIEW_WORK*         view;

  // ���O����
  NAMEIN_PARAM*               namein_param;

  // ���[�J��PROC�V�X�e��
  GFL_PROCSYS*  local_procsys;
}
EGG_DEMO_WORK;


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// VBlank�֐�
static void Egg_Demo_VBlankFunc( GFL_TCB* tcb, void* wk );

// �����������A�I������(PROC�ύX�O��ɍs�����Ƃ��ł������)
static void Egg_Demo_Init( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static void Egg_Demo_Exit( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );

// �T�E���h
static void Egg_Demo_SoundInit( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static void Egg_Demo_SoundExit( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static void Egg_Demo_SoundMain( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static void Egg_Demo_SoundPlayIntro( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static BOOL Egg_Demo_SoundCheckPlayIntro( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static void Egg_Demo_SoundPlayHatch( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static void Egg_Demo_SoundPushHatch( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static void Egg_Demo_SoundPopHatch( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static void Egg_Demo_SoundPlayCongratulate( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static BOOL Egg_Demo_SoundCheckPlayCongratulate( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static void Egg_Demo_SoundFadeOutHatch( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static BOOL Egg_Demo_SoundCheckFadeOutHatch( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );

// �P��F�A�P��L������BG
static void Egg_Demo_CreateSimpleBG( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static void Egg_Demo_DeleteSimpleBG( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );

// TEXT
static void Egg_Demo_TextInit( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static void Egg_Demo_TextExit( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static void Egg_Demo_TextMain( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static void Egg_Demo_TextShowWindow( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static void Egg_Demo_TextMakeStream( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work,
                                     u32 str_id );
static BOOL Egg_Demo_TextWaitStream( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );

// �͂��E�������E�B���h�E
static void Egg_Demo_TmInit( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static void Egg_Demo_TmExit( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static void Egg_Demo_TmMain( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static void Egg_Demo_TmStartSelect(  EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static TM_RESULT Egg_Demo_TmGetResult( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );

// ������΂����o�̂��߂̃p���b�g�t�F�[�h
static void Egg_Demo_PFadeZero( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static void Egg_Demo_PFadeInit( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );  // �p���b�g������������̂ŁA�S�p���b�g�̗p�ӂ��ς�ł���ĂԂ���
static void Egg_Demo_PFadeExit( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static void Egg_Demo_PFadeMain( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static void Egg_Demo_PFadeToWhite( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static void Egg_Demo_PFadeFromWhite( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static BOOL Egg_Demo_PFadeCheck( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );  // TRUE�������AFALSE�I��

// �㉺�ɍ��т�\�����邽�߂�wnd
static void Egg_Demo_WndInit( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static void Egg_Demo_WndExit( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static void Egg_Demo_WndMain( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static void Egg_Demo_WndAppear( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static void Egg_Demo_WndDisappear( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );


//=============================================================================
/**
*  PROC
*/
//=============================================================================
static GFL_PROC_RESULT Egg_Demo_ProcInit( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static GFL_PROC_RESULT Egg_Demo_ProcExit( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static GFL_PROC_RESULT Egg_Demo_ProcMain( GFL_PROC* proc, int* seq, void* pwk, void* mywk );

const GFL_PROC_DATA    EGG_DEMO_ProcData =
{
  Egg_Demo_ProcInit,
  Egg_Demo_ProcMain,
  Egg_Demo_ProcExit,
};


//=============================================================================
/**
*  �O�����J�֐���`
*/
//=============================================================================
//------------------------------------------------------------------
/**
 *  @brief       �^�}�S�z���f���p�p�����[�^����
 *
 *  @param[in]   heap_id       �q�[�vID
 *  @param[in]   gamedata      GAMEDATA
 *  @param[in]   pp            POKEMON_PARAM
 *
 *  @retval      EGG_DEMO_PARAM
 */
//------------------------------------------------------------------
EGG_DEMO_PARAM*  EGG_DEMO_AllocParam(
                            HEAPID           heap_id,
                            GAMEDATA*        gamedata,
                            POKEMON_PARAM*   pp )
{
  EGG_DEMO_PARAM* param = GFL_HEAP_AllocMemory( heap_id, sizeof( EGG_DEMO_PARAM ) );
  EGG_DEMO_InitParam( param, gamedata, pp );
  return param;
}

//------------------------------------------------------------------
/**
 *  @brief           �^�}�S�z���f���p�p�����[�^���
 *
 *  @param[in,out]   param      EGG_DEMO_AllocParam�Ő�����������
 *
 *  @retval          
 */
//------------------------------------------------------------------
void             EGG_DEMO_FreeParam(
                            EGG_DEMO_PARAM*  param )
{
  GFL_HEAP_FreeMemory( param );
}

//------------------------------------------------------------------
/**
 *  @brief           �^�}�S�z���f���p�p�����[�^��ݒ肷��
 *
 *  @param[in,out]   param      EGG_DEMO_PARAM
 *  @param[in]       gamedata   GAMEDATA
 *  @param[in]       pp         POKEMON_PARAM
 *
 *  @retval          
 */
//------------------------------------------------------------------
void             EGG_DEMO_InitParam(
                            EGG_DEMO_PARAM*  param,
                            GAMEDATA*        gamedata,
                            POKEMON_PARAM*   pp )
{
  param->gamedata    = gamedata;
  param->pp          = pp;
}


//=============================================================================
/**
*  ���[�J���֐���`(PROC)
*/
//=============================================================================
//-------------------------------------
/// PROC ����������
//=====================================
static GFL_PROC_RESULT Egg_Demo_ProcInit( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  EGG_DEMO_PARAM*    param    = (EGG_DEMO_PARAM*)pwk;
  EGG_DEMO_WORK*     work;

  // �q�[�v
  {
    GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_EGG_DEMO, HEAP_SIZE );
    work = GFL_PROC_AllocWork( proc, sizeof(EGG_DEMO_WORK), HEAPID_EGG_DEMO );
    GFL_STD_MemClear( work, sizeof(EGG_DEMO_WORK) );
    
    work->heap_id       = HEAPID_EGG_DEMO;
  }

  // �X�e�b�v
  {
    work->trunk_step    = TRUNK_STEP_FADE_IN;
    work->sound_step    = SOUND_STEP_WAIT;
    work->text_step     = TEXT_STEP_WAIT;
    work->tm_step       = TM_STEP_WAIT;
    work->pfade_step    = PFADE_STEP_WAIT;
    work->wnd_step      = WND_STEP_WAIT;
  }

  Egg_Demo_Init( param, work );

  // �T�E���h
  Egg_Demo_SoundInit( param, work );
  
  // �t�F�[�h�C��(����������)
  GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, FADE_IN_WAIT );

  // ���[�J��PROC�V�X�e�����쐬
  work->local_procsys = GFL_PROC_LOCAL_boot( work->heap_id );

  return GFL_PROC_RES_FINISH;
}

//-------------------------------------
/// PROC �I������
//=====================================
static GFL_PROC_RESULT Egg_Demo_ProcExit( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  EGG_DEMO_PARAM*    param    = (EGG_DEMO_PARAM*)pwk;
  EGG_DEMO_WORK*     work     = (EGG_DEMO_WORK*)mywk;

  // ���[�J��PROC�V�X�e����j��
  GFL_PROC_LOCAL_Exit( work->local_procsys ); 

  // �T�E���h
  Egg_Demo_SoundExit( param, work );

  if( work->trunk_step != TRUNK_STEP_NAMEIN_END )
    Egg_Demo_Exit( param, work );

  // �q�[�v
  {
    GFL_PROC_FreeWork( proc );
    GFL_HEAP_DeleteHeap( HEAPID_EGG_DEMO );
  }

  return GFL_PROC_RES_FINISH;
}

//-------------------------------------
/// PROC �又��
//=====================================
static GFL_PROC_RESULT Egg_Demo_ProcMain( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  EGG_DEMO_PARAM*    param    = (EGG_DEMO_PARAM*)pwk;
  EGG_DEMO_WORK*     work     = (EGG_DEMO_WORK*)mywk;

  int key_trg = GFL_UI_KEY_GetTrg();
  
  // ���[�J��PROC�̍X�V����
  GFL_PROC_MAIN_STATUS  local_proc_status   =  GFL_PROC_LOCAL_Main( work->local_procsys );
  if( local_proc_status == GFL_PROC_MAIN_VALID ) return GFL_PROC_RES_CONTINUE;

  switch( work->trunk_step )
  {
  case TRUNK_STEP_FADE_IN:
    {
      if( !GFL_FADE_CheckFade() )
      {
        // ����
        work->trunk_step = TRUNK_STEP_SOUND_INTRO;

        Egg_Demo_SoundPlayIntro( param, work );
      }
    }
    break;
  case TRUNK_STEP_SOUND_INTRO:
    {
      if( !Egg_Demo_SoundCheckPlayIntro( param, work ) )
      {
        // ����
        work->trunk_step = TRUNK_STEP_DEMO_EGG;

        // �^�}�S�z���f���̉��o
        EGG_DEMO_VIEW_Start( work->view );
        
        Egg_Demo_SoundPlayHatch( param, work );
      }
    }
    break;
  case TRUNK_STEP_DEMO_EGG:
    {
      // �^�}�S�z���f���̉��o
      if( EGG_DEMO_VIEW_White( work->view ) )
      {
        // ����
        work->trunk_step = TRUNK_STEP_DEMO_TO_WHITE;

        Egg_Demo_WndDisappear( param, work );
        Egg_Demo_PFadeToWhite( param, work );

        Egg_Demo_SoundPushHatch( param, work );
      }
    }
    break;
  case TRUNK_STEP_DEMO_TO_WHITE:
    {
      if( !Egg_Demo_PFadeCheck( param, work ) )
      {
        // ����
        work->trunk_step = TRUNK_STEP_DEMO_BRIGHT_TO_WHITE;

        // �t�F�[�h�A�E�g(�����遨��)
        GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_WHITEOUT_MAIN, 16, 16, 0 );
      }
    }
    break;
  case TRUNK_STEP_DEMO_BRIGHT_TO_WHITE:
    {
      if( !GFL_FADE_CheckFade() )
      {
        // ����
        work->trunk_step = TRUNK_STEP_DEMO_WHITE;

        Egg_Demo_TextShowWindow( param, work );
        
        // �^�}�S�z��
        PP_Put( param->pp, ID_PARA_tamago_flag, 0 );

        // �^�}�S�z���f���̉��o
        EGG_DEMO_VIEW_Hatch( work->view, param->pp );
      }
    }
    break;
  case TRUNK_STEP_DEMO_WHITE:
    {
      // �^�}�S�z���f���̉��o
      if( EGG_DEMO_VIEW_IsReady( work->view ) )
      {
        // ����
        work->trunk_step = TRUNK_STEP_DEMO_BRIGHT_WHITE;

        Egg_Demo_PFadeFromWhite( param, work );
      }
    }
    break;
  case TRUNK_STEP_DEMO_BRIGHT_WHITE:
    {
      {
        // ����
        work->trunk_step = TRUNK_STEP_DEMO_MON;

        // �t�F�[�h�C��(����������)
        GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_WHITEOUT_MAIN, 16, 0, PFADE_WAIT_FROM_WHITE );
        
        // �^�}�S�z���f���̉��o
        EGG_DEMO_VIEW_MonStart( work->view );
      }
    }
    break;
  case TRUNK_STEP_DEMO_MON:
    {
      // �^�}�S�z���f���̉��o
      if( EGG_DEMO_VIEW_IsEnd( work->view ) )
      {
        // ����
        work->trunk_step = TRUNK_STEP_TEXT_0;

        Egg_Demo_TextMakeStream( param, work, egg_evemt_02 );
        
        Egg_Demo_SoundPlayCongratulate( param, work );
      }
    }
    break;
  case TRUNK_STEP_TEXT_0:
    {
      if(    Egg_Demo_TextWaitStream( param, work )
          && (!Egg_Demo_SoundCheckPlayCongratulate( param, work )) )
      {
        // ����
        work->trunk_step = TRUNK_STEP_TEXT_1;
        
        Egg_Demo_TextMakeStream( param, work, egg_evemt_03 );
      }
    }
  break;
  case TRUNK_STEP_TEXT_1:
    {
      if( Egg_Demo_TextWaitStream( param, work ) )
      {
        // ����
        work->trunk_step = TRUNK_STEP_TM;

        Egg_Demo_TmStartSelect( param, work );
      }
    }
    break;
  case TRUNK_STEP_TM:
    {
      TM_RESULT tm_result = Egg_Demo_TmGetResult( param, work );
      if( tm_result == TM_RESULT_NO )
      {
        // ����
        work->trunk_step = TRUNK_STEP_FADE_OUT;

        // �t�F�[�h�A�E�g(�����遨��)
        GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, FADE_OUT_WAIT );

        Egg_Demo_SoundFadeOutHatch( param, work );
      }
      else if( tm_result == TM_RESULT_YES )
      {
        // ����
        work->trunk_step = TRUNK_STEP_NAMEIN_FADE_OUT;

        // �t�F�[�h�A�E�g(�����遨��)
        GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, NAMEIN_FADE_OUT_WAIT );
      }
    }
    break;
  case TRUNK_STEP_NAMEIN_FADE_OUT:
    {
      if( !GFL_FADE_CheckFade() )
      {
        Egg_Demo_Exit( param, work );

        // ����
        work->trunk_step = TRUNK_STEP_NAMEIN;

        // ���O���͂֍s��
        GFL_OVERLAY_Load( FS_OVERLAY_ID(namein) );
        work->namein_param = NAMEIN_AllocParamPokemonByPP( work->heap_id, param->pp, NAMEIN_POKEMON_LENGTH, NULL );
        // ���[�J��PROC�Ăяo��
        GFL_PROC_LOCAL_CallProc( work->local_procsys, NO_OVERLAY_ID, &NameInputProcData, work->namein_param );
      }
    }
    break;
  case TRUNK_STEP_NAMEIN:
    {
      // ���[�J��PROC���I������̂�҂�  // ����Main�̍ŏ���GFL_PROC_MAIN_VALID�Ȃ�return���Ă���̂ŁA�����ł͔��肵�Ȃ��Ă��悢���O�̂���
      if( local_proc_status != GFL_PROC_MAIN_VALID )
      {
        // ���O���͂���߂�
        if( !NAMEIN_IsCancel( work->namein_param ) )
        {
          STRBUF* strbuf = GFL_STR_CreateBuffer( NAMEIN_STRBUF_LENGTH, work->heap_id );
          NAMEIN_CopyStr( work->namein_param, strbuf );
          PP_Put( param->pp, ID_PARA_nickname, (u32)strbuf );
          GFL_STR_DeleteBuffer( strbuf );
        }
        NAMEIN_FreeParam( work->namein_param );
        GFL_OVERLAY_Unload( FS_OVERLAY_ID(namein) );

        // ����
        work->trunk_step = TRUNK_STEP_NAMEIN_BLACK;
     
        Egg_Demo_SoundFadeOutHatch( param, work );
      }
      else
      {
        return GFL_PROC_RES_CONTINUE;
      }
    }
    break;
  case TRUNK_STEP_NAMEIN_BLACK:
    {
      if( Egg_Demo_SoundCheckFadeOutHatch( param, work ) )
      {
        // ����
        work->trunk_step = TRUNK_STEP_NAMEIN_END;
 
        return GFL_PROC_RES_FINISH;
      }
    }
    break;
  case TRUNK_STEP_FADE_OUT:
    {
      if( !GFL_FADE_CheckFade() )
      {
        // ����
        work->trunk_step = TRUNK_STEP_END;

        return GFL_PROC_RES_FINISH;
      }
    }
    break;
  default:
    break;
  }

  if(    work->trunk_step != TRUNK_STEP_NAMEIN
      && work->trunk_step != TRUNK_STEP_NAMEIN_BLACK
      && work->trunk_step != TRUNK_STEP_NAMEIN_END
      && work->trunk_step != TRUNK_STEP_END )
  {
    // TEXT
    Egg_Demo_TextMain( param, work );
    // �͂��E�������E�B���h�E
    Egg_Demo_TmMain( param, work );
    // �㉺�ɍ��т�\�����邽�߂�wnd
    Egg_Demo_WndMain( param, work );

    // ������΂����o�̂��߂̃p���b�g�t�F�[�h
    Egg_Demo_PFadeMain( param, work );

    // �^�}�S�z���f���̉��o
    EGG_DEMO_VIEW_Main( work->view );

    // �T�E���h
    Egg_Demo_SoundMain( param, work );

    PRINTSYS_QUE_Main( work->print_que );

    // 2D�`��
    EGG_DEMO_GRAPHIC_2D_Draw( work->graphic );
    // 3D�`��
    EGG_DEMO_GRAPHIC_3D_StartDraw( work->graphic );
    EGG_DEMO_VIEW_Draw( work->view );  // �^�}�S�z���f���̉��o
    EGG_DEMO_GRAPHIC_3D_EndDraw( work->graphic );
  }

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
static void Egg_Demo_VBlankFunc( GFL_TCB* tcb, void* wk )
{
  EGG_DEMO_WORK* work = (EGG_DEMO_WORK*)wk;

  // ������΂����o�̂��߂̃p���b�g�t�F�[�h
  if( work->pfade_ptr ) PaletteFadeTrans( work->pfade_ptr );
}

//-------------------------------------
/// �����������A�I������(PROC�ύX�O��ɍs�����Ƃ��ł������)
//=====================================
static void Egg_Demo_Init( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  // ��������ɍs��������
  {
    // ��\��
    u8 i;
    for(i=GFL_BG_FRAME0_M; i<=GFL_BG_FRAME3_S; i++)
    {
      GFL_BG_SetVisible( i, VISIBLE_OFF );
    }
  } 

  // �O���t�B�b�N�A�t�H���g�Ȃ�
  {
    work->graphic       = EGG_DEMO_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, work->heap_id );
    work->font          = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, work->heap_id );
    work->print_que     = PRINTSYS_QUE_Create( work->heap_id );
  }

  // VBlank��TCB
  {
    work->vblank_tcb = GFUser_VIntr_CreateTCB( Egg_Demo_VBlankFunc, work, 1 );
  }

  // �P��F�A�P��L������BG
  Egg_Demo_CreateSimpleBG( param, work );

  // TEXT
  Egg_Demo_TextInit( param, work );
  // �͂��E�������E�B���h�E
  Egg_Demo_TmInit( param, work );
  // �㉺�ɍ��т�\�����邽�߂�wnd
  Egg_Demo_WndInit( param, work );

  // �v���C�I���e�B�A�\���A�w�i�F�Ȃ�
  {
    GFL_BG_SetPriority( BG_FRAME_M_BACK              , 2 );
    GFL_BG_SetPriority( BG_FRAME_M_POKEMON           , 1 );
    GFL_BG_SetPriority( BG_FRAME_M_TEXT              , 0 );  // �őO��

    GFL_BG_SetPriority( BG_FRAME_S_BACK              , 0 );  // �őO��

    GFL_BG_SetVisible( BG_FRAME_M_BACK               , VISIBLE_ON );
    GFL_BG_SetVisible( BG_FRAME_M_POKEMON            , VISIBLE_ON );
    GFL_BG_SetVisible( BG_FRAME_M_TEXT               , VISIBLE_ON );
  
    GFL_BG_SetVisible( BG_FRAME_S_BACK               , VISIBLE_ON );

    // �p�[�e�B�N���Ή�
    G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG0,
                      GX_BLEND_PLANEMASK_BD | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG1,
                      0, 0 );

    GFL_BG_SetBackGroundColor( GFL_BG_FRAME0_M, 0x0000 );
    GFL_BG_SetBackGroundColor( GFL_BG_FRAME0_S, 0x0000 );
  }

  // ������΂����o�̂��߂̃p���b�g�t�F�[�h
  Egg_Demo_PFadeInit( param, work );  // �p���b�g������������̂ŁA�S�p���b�g�̗p�ӂ��ς�ł���ĂԂ���

  // �^�}�S�z���f���̉��o
  work->view = EGG_DEMO_VIEW_Init( work->heap_id, param->pp );
}
static void Egg_Demo_Exit( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  // �^�}�S�z���f���̉��o
  EGG_DEMO_VIEW_Exit( work->view );

  // ������΂����o�̂��߂̃p���b�g�t�F�[�h
  Egg_Demo_PFadeExit( param, work );

  // �㉺�ɍ��т�\�����邽�߂�wnd
  Egg_Demo_WndExit( param, work );
  // �͂��E�������E�B���h�E
  Egg_Demo_TmExit( param, work );
  // TEXT
  Egg_Demo_TextExit( param, work );

  // �P��F�A�P��L������BG
  Egg_Demo_DeleteSimpleBG( param, work );

  // VBlank��TCB
  GFL_TCB_DeleteTask( work->vblank_tcb );

  // �O���t�B�b�N�A�t�H���g�Ȃ�
  {
    PRINTSYS_QUE_Delete( work->print_que );
    GFL_FONT_Delete( work->font );
    EGG_DEMO_GRAPHIC_Exit( work->graphic );
  }
}

//-------------------------------------
/// �T�E���h
//=====================================
static void Egg_Demo_SoundInit( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  PMSND_PushBGM();
}
static void Egg_Demo_SoundExit( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  PMSND_StopBGM();
  PMSND_PopBGM();
}
static void Egg_Demo_SoundMain( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  switch( work->sound_step )
  {
  case SOUND_STEP_WAIT:
    {
    }
    break;
  case SOUND_STEP_INTRO:
    {
      if( !PMSND_CheckPlayBGM() )
      {
        work->sound_step = SOUND_STEP_WAIT;
      }
    }
    break;
  case SOUND_STEP_HATCH:
    {
    }
    break;
  case SOUND_STEP_CONGRATULATE:
    {
      if( !PMSND_CheckPlayBGM() )
      {
        Egg_Demo_SoundPopHatch( param, work );
        work->sound_step = SOUND_STEP_HATCH;
      }
    }
    break;
  }
}
static void Egg_Demo_SoundPlayIntro( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  PMSND_PlayBGM(SEQ_BGM_SHINKA);
  work->sound_step = SOUND_STEP_INTRO;
}
static BOOL Egg_Demo_SoundCheckPlayIntro( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  if( work->sound_step == SOUND_STEP_INTRO ) return TRUE;
  else                                       return FALSE;
}
static void Egg_Demo_SoundPlayHatch( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  PMSND_PlayBGM(SEQ_BGM_KOUKAN);
  work->sound_step = SOUND_STEP_HATCH;
}
static void Egg_Demo_SoundPushHatch( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  PMSND_PushBGM();
}
static void Egg_Demo_SoundPopHatch( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  PMSND_PopBGM();
}
static void Egg_Demo_SoundPlayCongratulate( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  PMSND_PlayBGM(SEQ_ME_SHINKAOME);
  work->sound_step = SOUND_STEP_CONGRATULATE;
}
static BOOL Egg_Demo_SoundCheckPlayCongratulate( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  return ( work->sound_step == SOUND_STEP_CONGRATULATE );
}
static void Egg_Demo_SoundFadeOutHatch( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  PMSND_FadeOutBGM( 16 * FADE_OUT_WAIT / 2 );
}
static BOOL Egg_Demo_SoundCheckFadeOutHatch( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  return PMSND_CheckFadeOnBGM();
}

//-------------------------------------
/// �P��F�A�P��L������BG
//=====================================
static void Egg_Demo_CreateSimpleBG( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  // �p���b�g�̍쐬���]��
  {
    u16* pal = GFL_HEAP_AllocClearMemory( work->heap_id, sizeof(u16) * 0x10 );
    pal[0x00] = 0x0000;  // ����
    pal[0x01] = 0x0000;  // ��
    pal[0x02] = 0x7fff;  // ��
    pal[0x03] = 0x2108;  // �D

    GFL_BG_LoadPalette( BG_FRAME_M_BACK, pal, 0x20, BG_PAL_POS_M_BACK*0x20 );
    GFL_HEAP_FreeMemory( pal );
  }

  // �L�����̍쐬���]��
  {
    u32 sim_bmp_pos;
    u32 sim_bmp_size;

    GFL_BMP_DATA* sim_bmp = GFL_BMP_Create( 1, 1, GFL_BMP_16_COLOR, work->heap_id );
    GFL_BMP_Fill( sim_bmp, 0, 0, 8, 8, 0x03 );

    sim_bmp_size = GFL_BMP_GetBmpDataSize(sim_bmp);
    sim_bmp_pos = GFL_BG_LoadCharacterAreaMan(
                    BG_FRAME_M_BACK,
                    GFL_BMP_GetCharacterAdrs(sim_bmp),
                    sim_bmp_size );

    GF_ASSERT_MSG( sim_bmp_pos != AREAMAN_POS_NOTFOUND, "EGG_DEMO : BG�L�����̈悪����܂���ł����B\n" );  // gflib��arc_util.c��
    GF_ASSERT_MSG( sim_bmp_pos < 0xffff, "EGG_DEMO : BG�L�����̈ʒu���悭����܂���B\n" );                 // _TransVramBgCharacterAreaMan
    GF_ASSERT_MSG( sim_bmp_size < 0xffff, "EGG_DEMO : BG�L�����̃T�C�Y���悭����܂���B\n" );              // ���Q�l�ɂ����B

    work->sim_transinfo = GFL_ARCUTIL_TRANSINFO_Make( sim_bmp_pos, sim_bmp_size );
    
    GFL_BMP_Delete( sim_bmp );
  }

  // �X�N���[���̍쐬���]��
  {
    u16* sim_scr = GFL_HEAP_AllocClearMemory( work->heap_id, sizeof(u16) * 32*24 );
    u8 i, j;
    u16 h = 0;
    for(i=0; i<32; i++)
    {
      for(j=0; j<24; j++)
      {
        u16 chara_name = GFL_ARCUTIL_TRANSINFO_GetPos(work->sim_transinfo);
        u16 flip_h     = 0;
        u16 flip_v     = 0;
        u16 pal        = BG_PAL_POS_M_BACK;
        sim_scr[h] = ( pal << 12 ) | ( flip_v << 11 ) | ( flip_h << 10 ) | ( chara_name << 0 );
        
        h++;
      }
    }

    GFL_BG_WriteScreen( BG_FRAME_M_BACK, sim_scr, 0, 0, 32, 24 );
    GFL_BG_LoadScreenReq( BG_FRAME_M_BACK );

    GFL_HEAP_FreeMemory( sim_scr );
  }
}
static void Egg_Demo_DeleteSimpleBG( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  GFL_BG_FreeCharacterArea( BG_FRAME_M_BACK,
                            GFL_ARCUTIL_TRANSINFO_GetPos(work->sim_transinfo),
                            GFL_ARCUTIL_TRANSINFO_GetSize(work->sim_transinfo) );
}

//-------------------------------------
/// TEXT
//=====================================
static void Egg_Demo_TextInit( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  // �p���b�g
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG,
      BG_PAL_POS_M_TEXT_FONT * 0x20, BG_PAL_NUM_M_TEXT_FONT * 0x20, work->heap_id );

  // BG�t���[���̃X�N���[���̋󂢂Ă���ӏ��ɉ����\��������Ȃ��悤�ɂ��Ă���
  work->text_dummy_bmpwin = GFL_BMPWIN_Create( BG_FRAME_M_TEXT, 0, 0, 1, 1,
                                BG_PAL_POS_M_TEXT_FONT, GFL_BMP_CHRAREA_GET_F );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_dummy_bmpwin), 0 );
  GFL_BMPWIN_TransVramCharacter(work->text_dummy_bmpwin);

  // �E�B���h�E��
  work->text_winin_bmpwin = GFL_BMPWIN_Create( BG_FRAME_M_TEXT, 1, 19, 30, 4,
                                 BG_PAL_POS_M_TEXT_FONT, GFL_BMP_CHRAREA_GET_F );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_winin_bmpwin), TEXT_WININ_BACK_COLOR );
  GFL_BMPWIN_TransVramCharacter(work->text_winin_bmpwin);
    
  // �E�B���h�E�g
  work->text_winfrm_tinfo = BmpWinFrame_GraphicSetAreaMan( BG_FRAME_M_TEXT,
                                BG_PAL_POS_M_TEXT_FRAME,
                                MENU_TYPE_SYSTEM, work->heap_id );
  
  // �E�B���h�E���A�E�B���h�E�g�ǂ���̕\�����܂��s��Ȃ��̂ŁA�������ł͂����܂ŁB

  // ���b�Z�[�W
  work->text_msgdata_egg     = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_SCRIPT_MESSAGE, NARC_script_message_egg_event_dat, work->heap_id );

  // TCBL�A�t�H���g�J���[�A�]���Ȃ�
  work->text_tcblsys = GFL_TCBL_Init( work->heap_id, work->heap_id, 1, 0 );
  GFL_FONTSYS_SetColor( 1, 2, TEXT_WININ_BACK_COLOR );
  GFL_BG_LoadScreenV_Req( BG_FRAME_M_TEXT );

  // NULL������
  work->text_stream       = NULL;
  work->text_strbuf       = NULL;
}
static void Egg_Demo_TextExit( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  if( work->text_stream ) PRINTSYS_PrintStreamDelete( work->text_stream );
  if( work->text_strbuf ) GFL_STR_DeleteBuffer( work->text_strbuf );
  GFL_TCBL_Exit( work->text_tcblsys );
  GFL_MSG_Delete( work->text_msgdata_egg );
  GFL_BG_FreeCharacterArea( BG_FRAME_M_TEXT,
      GFL_ARCUTIL_TRANSINFO_GetPos(work->text_winfrm_tinfo),
      GFL_ARCUTIL_TRANSINFO_GetSize(work->text_winfrm_tinfo) );
  GFL_BMPWIN_Delete( work->text_winin_bmpwin );
  GFL_BMPWIN_Delete( work->text_dummy_bmpwin );
}
static void Egg_Demo_TextMain( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  switch( work->text_step )
  {
  case TEXT_STEP_WAIT:
    {
    }
    break;
  }

  GFL_TCBL_Main( work->text_tcblsys );
}
static void Egg_Demo_TextShowWindow( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  BmpWinFrame_Write( work->text_winin_bmpwin, WINDOW_TRANS_ON_V,
      GFL_ARCUTIL_TRANSINFO_GetPos(work->text_winfrm_tinfo),
      BG_PAL_POS_M_TEXT_FRAME );
  GFL_BMPWIN_MakeTransWindow_VBlank( work->text_winin_bmpwin );
}
static void Egg_Demo_TextMakeStream( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work,
                                     u32 str_id )
{
  // ��U����
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_winin_bmpwin), TEXT_WININ_BACK_COLOR );
  
  // �O�̂�����
  if( work->text_stream ) PRINTSYS_PrintStreamDelete( work->text_stream );  // ���̊֐��Ń^�X�N���폜���Ă����̂ŁA�����ɓ����^�X�N��1�ōς�
  if( work->text_strbuf ) GFL_STR_DeleteBuffer( work->text_strbuf );

  // ������쐬
  {
    STRBUF*   src_strbuf  = GFL_MSG_CreateString( work->text_msgdata_egg, str_id );
    WORDSET*  wordset     = WORDSET_Create( work->heap_id );
    WORDSET_RegisterPokeMonsName( wordset, 0, param->pp );
    work->text_strbuf = GFL_STR_CreateBuffer( STRBUF_LENGTH, work->heap_id );
    WORDSET_ExpandStr( wordset, work->text_strbuf, src_strbuf );
    WORDSET_Delete( wordset );
    GFL_STR_DeleteBuffer( src_strbuf );
  }
  
  // �X�g���[���J�n
  work->text_stream = PRINTSYS_PrintStream(
                          work->text_winin_bmpwin,
                          0, 0,
                          work->text_strbuf,
                          work->font, MSGSPEED_GetWait(),
                          work->text_tcblsys, 2,
                          work->heap_id,
                          TEXT_WININ_BACK_COLOR );
}
static BOOL Egg_Demo_TextWaitStream( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  BOOL ret = FALSE;

  switch( PRINTSYS_PrintStreamGetState( work->text_stream ) )
  { 
  case PRINTSTREAM_STATE_PAUSE:
    if( ( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A ) || GFL_UI_TP_GetTrg() )
    { 
      PRINTSYS_PrintStreamReleasePause( work->text_stream );
    }
    break;
  case PRINTSTREAM_STATE_DONE:
    {
      ret = TRUE;
    }
    break;
  }
  
  return ret;
}

//-------------------------------------
/// �͂��E�������E�B���h�E
//=====================================
static void Egg_Demo_TmInit( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  // ���̊֐���Egg_Demo_TextInit���ς��
  // BG_PAL_POS_M_TEXT_FONT���ݒ肳��Ă����ԂŌĂԂ���
  // GFL_FONTSYS_SetColor�̐ݒ���K�v��������Ȃ�

  u32 size = GFL_BG_1CHRDATASIZ * ( 7*4 +12 );  // system/bmp_menu.c��BmpMenu_YesNoSelectInit���Q�l�ɂ����T�C�Y + �����傫�߂�
  u32 pos = GFL_BG_AllocCharacterArea( BG_FRAME_M_TEXT, size, GFL_BG_CHRAREA_GET_B );
  GF_ASSERT_MSG( pos != AREAMAN_POS_NOTFOUND, "EGG_DEMO : BG�L�����̈悪����܂���ł����B\n" );
  GFL_BG_FreeCharacterArea( BG_FRAME_M_TEXT, pos, size );

  work->tm_dat.frmnum    = BG_FRAME_M_TEXT;
  work->tm_dat.pos_x     = 24;
  work->tm_dat.pos_y     = 13;
  work->tm_dat.palnum    = BG_PAL_POS_M_TEXT_FONT;//BG_PAL_POS_M_TM;
  work->tm_dat.chrnum    = pos;  // �g���Ă��Ȃ��悤��

  work->tm_result = TM_RESULT_SEL;
}
static void Egg_Demo_TmExit( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  // BmpMenu_YesNoSelectMain��������Ă����̂ŁA�������Ȃ��Ă悢
}
static void Egg_Demo_TmMain( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  switch( work->tm_step )
  {
  case TM_STEP_WAIT:
    {
    }
    break;
  case TM_STEP_SELECT:
    {
      u32 ret = BmpMenu_YesNoSelectMain( work->tm_wk );
      if( ret != BMPMENU_NULL )
      {
        if( ret == 0 )
          work->tm_result = TM_RESULT_YES;
        else //if( ret == BMPMENU_CANCEL )
          work->tm_result = TM_RESULT_NO;

        work->tm_step = TM_STEP_END;
      }
    }
    break;
  }
}
static void Egg_Demo_TmStartSelect(  EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  // ���̊֐���Egg_Demo_TextInit���ς��
  // work->text_winfrm_tinfo���ݒ肳��Ă����ԂŌĂԂ���

  work->tm_wk = BmpMenu_YesNoSelectInit(
                    &work->tm_dat,
                    GFL_ARCUTIL_TRANSINFO_GetPos(work->text_winfrm_tinfo),
                    BG_PAL_POS_M_TEXT_FRAME,
                    0,
                    work->heap_id );

  work->tm_result = TM_RESULT_SEL;
  work->tm_step = TM_STEP_SELECT;
}
static TM_RESULT Egg_Demo_TmGetResult( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  return work->tm_result;
}

//-------------------------------------
/// ������΂����o�̂��߂̃p���b�g�t�F�[�h
//=====================================
static void Egg_Demo_PFadeZero( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  work->pfade_tcbsys       = NULL;
  work->pfade_tcbsys_wk    = NULL;
  work->pfade_ptr          = NULL;
}
static void Egg_Demo_PFadeInit( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )  // �p���b�g������������̂ŁA�S�p���b�g�̗p�ӂ��ς�ł���ĂԂ���
{
  Egg_Demo_PFadeZero( param, work );

  // �^�X�N
  work->pfade_tcbsys_wk = GFL_HEAP_AllocClearMemory( work->heap_id, GFL_TCB_CalcSystemWorkSize(PFADE_TCBSYS_TASK_MAX) );
  work->pfade_tcbsys = GFL_TCB_Init( PFADE_TCBSYS_TASK_MAX, work->pfade_tcbsys_wk );

  // �p���b�g�t�F�[�h
  work->pfade_ptr = PaletteFadeInit( work->heap_id );
  PaletteTrans_AutoSet( work->pfade_ptr, TRUE );
  PaletteFadeWorkAllocSet( work->pfade_ptr, FADE_MAIN_BG, 0x1e0, work->heap_id );
  PaletteFadeWorkAllocSet( work->pfade_ptr, FADE_MAIN_OBJ, 0x1e0, work->heap_id );

  // ����VRAM�ɂ���p���b�g���󂳂Ȃ��悤�ɁAVRAM����p���b�g���e���R�s�[����
  PaletteWorkSet_VramCopy( work->pfade_ptr, FADE_MAIN_BG, 0, 0x1e0 );
  PaletteWorkSet_VramCopy( work->pfade_ptr, FADE_MAIN_OBJ, 0, 0x1e0 );
}
static void Egg_Demo_PFadeExit( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  // �p���b�g�t�F�[�h
  PaletteFadeWorkAllocFree( work->pfade_ptr, FADE_MAIN_BG );
  PaletteFadeWorkAllocFree( work->pfade_ptr, FADE_MAIN_OBJ );
  PaletteFadeFree( work->pfade_ptr );

  // �^�X�N
  GFL_TCB_Exit( work->pfade_tcbsys );
  GFL_HEAP_FreeMemory( work->pfade_tcbsys_wk );

  Egg_Demo_PFadeZero( param, work );
}
static void Egg_Demo_PFadeMain( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  // �^�X�N
  if( work->pfade_tcbsys ) GFL_TCB_Main( work->pfade_tcbsys );

  // �X�e�b�v
  switch( work->pfade_step )
  {
  case PFADE_STEP_WAIT:
    {
    }
    break;
  }
}
static void Egg_Demo_PFadeToWhite( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  // ������΂�
  u16 req_bit = (1<<BG_PAL_POS_M_BACKDROP) | (1<<BG_PAL_POS_M_BACK);
  PaletteFadeReq(
    work->pfade_ptr, PF_BIT_MAIN_BG, req_bit, PFADE_WAIT_TO_WHITE, 0, 16, 0x7fff, work->pfade_tcbsys
  );
}
static void Egg_Demo_PFadeFromWhite( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  // ������߂�
  u16 req_bit = (1<<BG_PAL_POS_M_BACKDROP) | (1<<BG_PAL_POS_M_BACK);
//  PaletteFadeReq(
//    work->pfade_ptr, PF_BIT_MAIN_BG, req_bit, PFADE_WAIT_FROM_WHITE, 16, 0, 0x7fff, work->pfade_tcbsys
//  );
  PaletteFadeReq(
    work->pfade_ptr, PF_BIT_MAIN_BG, req_bit, 0, 0, 0, 0x7fff, work->pfade_tcbsys
  );
}
static BOOL Egg_Demo_PFadeCheck( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )  // TRUE�������AFALSE�I��
{
  if( PaletteFadeCheck(work->pfade_ptr) == 0 )
    return FALSE;
  else
    return TRUE;
}
 

//-------------------------------------
/// �㉺�ɍ��т�\�����邽�߂�wnd
//=====================================
static void Egg_Demo_WndInit( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  work->wnd_up_y     = WND_UP_Y_APPEAR;
  work->wnd_down_y   = WND_DOWN_Y_APPEAR;
  //work->wnd_up_y     = 0;
  //work->wnd_down_y   = 192;

  GX_SetVisibleWnd( GX_WNDMASK_W0 | GX_WNDMASK_W1 );

  G2_SetWnd0Position(   0, work->wnd_up_y,      128, work->wnd_down_y );
  G2_SetWnd1Position( 128, work->wnd_up_y, 0/*256*/, work->wnd_down_y );

  G2_SetWnd0InsidePlane(
    GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2 | GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ,
    TRUE );
  G2_SetWnd1InsidePlane(
    GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2 | GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ,
    TRUE );

  G2_SetWndOutsidePlane(
    GX_WND_PLANEMASK_BG1,
    TRUE );

  work->wnd_appear_speed  = 0;
  work->wnd_appear_wait   = 0;
  work->wnd_count         = 0;
}
static void Egg_Demo_WndExit( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  GX_SetVisibleWnd( GX_WNDMASK_NONE );
}
static void Egg_Demo_WndMain( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  if( work->wnd_appear_speed != 0 )
  {
    work->wnd_count++;
    if( work->wnd_count >= work->wnd_appear_wait )
    {
      BOOL b_up_y_move  = FALSE;
      s16  down_y       = work->wnd_down_y;
      s16  up_y         = work->wnd_up_y;

      down_y -= work->wnd_appear_speed;
      if( work->wnd_appear_speed > 0 )
      {
        if( down_y < WND_DOWN_Y_APPEAR + WND_UP_Y_APPEAR ) b_up_y_move = TRUE;
      }
      else
      {
        b_up_y_move = TRUE;
      }
      if( b_up_y_move ) up_y += work->wnd_appear_speed;

      if( down_y < WND_DOWN_Y_APPEAR ) work->wnd_down_y = WND_DOWN_Y_APPEAR;
      else if( down_y > 192 )          work->wnd_down_y = 192;
      else                             work->wnd_down_y = (u8)down_y;

      if( up_y < 0 )                    work->wnd_up_y = 0;
      else if( up_y > WND_UP_Y_APPEAR ) work->wnd_up_y = WND_UP_Y_APPEAR;
      else                              work->wnd_up_y = (u8)up_y;

      G2_SetWnd0Position(   0, work->wnd_up_y,      128, work->wnd_down_y );
      G2_SetWnd1Position( 128, work->wnd_up_y, 0/*256*/, work->wnd_down_y );

      {
        if( work->wnd_appear_speed > 0 )
        {
          if( work->wnd_up_y == WND_UP_Y_APPEAR && work->wnd_down_y == WND_DOWN_Y_APPEAR )
            work->wnd_appear_speed = 0;
        }
        else
        {
          if( work->wnd_up_y == 0 && work->wnd_down_y == 192 )
            work->wnd_appear_speed = 0;
        }
      }

      work->wnd_count = 0;
    }
  }
}
static void Egg_Demo_WndAppear( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  work->wnd_appear_speed = 1;
}
static void Egg_Demo_WndDisappear( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  work->wnd_appear_speed = -2;
}
