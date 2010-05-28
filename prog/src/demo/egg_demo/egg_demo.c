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
// �C���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "gamesystem/game_data.h"
#include "savedata/record.h"
#include "gamesystem/msgspeed.h"
#include "system/palanm.h"
#include "system/bmp_winframe.h"
#include "system/bmp_menu.h"
#include "print/gf_font.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "poke_tool/poke_tool.h"
#include "sound/pm_sndsys.h"
#include "../../field/field_sound.h"
#include "app/name_input.h"
#include "poke_tool/poke_memo.h"
#include "field/zonedata.h"  //ZONEDATA_GetPlaceNameID

#include "egg_demo_graphic.h"
#include "egg_demo_view.h"
#include "demo/egg_demo.h"


// �A�[�J�C�u
#include "arc_def.h"
#include "font/font.naix"
#include "script_message.naix"
#include "msg/script/msg_egg_event.h"
#include "egg_demo_particle.naix"
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
#define BG_FRAME_M_POKEMON           (GFL_BG_FRAME0_M)        // �v���C�I���e�B2
#define BG_FRAME_M_BELT              (GFL_BG_FRAME2_M)        // �v���C�I���e�B1
#define BG_FRAME_M_TEXT              (GFL_BG_FRAME1_M)        // �v���C�I���e�B0

#define BG_FRAME_S_BACK              (GFL_BG_FRAME0_S)        // �v���C�I���e�B0

// BG�t���[���̃v���C�I���e�B
#define BG_FRAME_PRI_M_POKEMON    (2)
#define BG_FRAME_PRI_M_BELT       (1)
#define BG_FRAME_PRI_M_TEXT       (0)

#define BG_FRAME_PRI_S_BACK       (0)

// BG�p���b�g
// �{��
enum
{
  BG_PAL_NUM_M_BELT          = 1,
  BG_PAL_NUM_M_TEXT_FONT     = 1,
  BG_PAL_NUM_M_TEXT_FRAME    = 1,
};
// �ʒu
enum
{
  BG_PAL_POS_M_BELT          = 0,
  BG_PAL_POS_M_TEXT_FONT     = 1,
  BG_PAL_POS_M_TEXT_FRAME    = 2,
  BG_PAL_POS_M_MAX           = 3,  // ���������
};
// �{��
enum
{
  BG_PAL_NUM_S_              = 0,
};
// �ʒu
enum
{
  BG_PAL_POS_S_              = 0,
  BG_PAL_POS_S_MAX           = 0,  // ���������
};

// OBJ�p���b�g
// �{��
enum
{
  OBJ_PAL_NUM_M_BELT         = 1,
};
// �ʒu
enum
{
  OBJ_PAL_POS_M_BELT         = 0,
  OBJ_PAL_POS_M_MAX          = 1,       // ���������
};
// �{��
enum
{
  OBJ_PAL_NUM_S_             = 0,
};
// �ʒu
enum
{
  OBJ_PAL_POS_S_             = 0, 
  OBJ_PAL_POS_S_MAX          = 0,      // ���������
};

// OBJ��BG�v���C�I���e�B
#define OBJ_BG_PRI_M_BELT   (BG_FRAME_PRI_M_BELT)

// OBJ
enum
{
  OBJ_CELL_BELT_UP_01,
  OBJ_CELL_BELT_UP_02,
  OBJ_CELL_BELT_UP_03,
  OBJ_CELL_BELT_DOWN_01,
  OBJ_CELL_BELT_DOWN_02,
  OBJ_CELL_BELT_DOWN_03,
  OBJ_CELL_MAX,
};
enum
{
  OBJ_RES_BELT_NCG,
  OBJ_RES_BELT_NCL,
  OBJ_RES_BELT_NCE,
  OBJ_RES_MAX,
};
static const GFL_CLWK_DATA obj_cell_data[OBJ_CELL_MAX] =
{
  { 128, 96, 0, 0, OBJ_BG_PRI_M_BELT },
  { 128, 96, 1, 0, OBJ_BG_PRI_M_BELT },
  { 128, 96, 2, 0, OBJ_BG_PRI_M_BELT },
  { 128, 96, 3, 0, OBJ_BG_PRI_M_BELT },
  { 128, 96, 4, 0, OBJ_BG_PRI_M_BELT },
  { 128, 96, 5, 0, OBJ_BG_PRI_M_BELT },
};


// TEXT
#define TEXT_WININ_BACK_COLOR        (15)

// �t�F�[�h
#define FADE_IN_WAIT            (-16)       ///< �t�F�[�h�C���̃X�s�[�h  // BG��OBJ��BELT���������Ă���̂Ńt�F�[�h�C���͈�u�ōs���̂Ń}�C�i�X
#define FADE_OUT_WAIT           (2)         ///< �t�F�[�h�A�E�g�̃X�s�[�h
#define FROM_WHITE_FADE_IN_WAIT (2)         ///< ������΂����t�F�[�h����A�������ԂɂȂ�Ƃ��̃X�s�[�h
#define NAMEIN_FADE_OUT_WAIT    (0)         ///< ���O���͂ֈڍs���邽�߂̃t�F�[�h�A�E�g�̃X�s�[�h

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
  TRUNK_STEP_FADE_IN_START,
  TRUNK_STEP_FADE_IN,
  TRUNK_STEP_BELT_OPEN,
  TRUNK_STEP_SOUND_INTRO,
  TRUNK_STEP_DEMO_EGG,
  TRUNK_STEP_DEMO_HATCH,
  TRUNK_STEP_DEMO_READY,
  TRUNK_STEP_DEMO_MON,
  TRUNK_STEP_TEXT_0_0,
  TRUNK_STEP_TEXT_0_1,
  TRUNK_STEP_TEXT_0,
  TRUNK_STEP_TEXT_1,
  TRUNK_STEP_TM,
  TRUNK_STEP_NAMEIN_FADE_OUT,
  TRUNK_STEP_NAMEIN,
  TRUNK_STEP_NAMEIN_BLACK,
  TRUNK_STEP_NAMEIN_END,
  TRUNK_STEP_SOUND_FADE_OUT,
  TRUNK_STEP_FADE_OUT,
  TRUNK_STEP_END,
}
TRUNK_STEP;

// �T�E���h�X�e�b�v
typedef enum
{
  SOUND_STEP_WAIT,
  SOUND_STEP_FIELD_FADE_OUT,
  SOUND_STEP_INTRO,
  SOUND_STEP_HATCH,
  SOUND_STEP_CONGRATULATE_LOAD,
  SOUND_STEP_CONGRATULATE,
  SOUND_STEP_HATCH_FADE_OUT,
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
  u8                          trunk_wait;
  SOUND_STEP                  sound_step;
  TEXT_STEP                   text_step;
  TM_STEP                     tm_step;

  u32                         sound_div_seq;  // BGM�̕������[�h�̃V�[�P���X

  // VBlank��TCB
  GFL_TCB*                    vblank_tcb;

  // OBJ
  u32                         obj_res[OBJ_RES_MAX];
  GFL_CLWK*                   obj_clwk[OBJ_CELL_MAX]; 

  // TEXT
  PRINT_STREAM*               text_stream;
  GFL_TCBLSYS*                text_tcblsys;
  GFL_BMPWIN*                 text_winin_bmpwin;
  GFL_BMPWIN*                 text_dummy_bmpwin;      ///< 0�Ԃ̃L�����N�^�[��1x1�ł����Ă���
  GFL_ARCUTIL_TRANSINFO       text_winfrm_tinfo;
  GFL_MSGDATA*                text_msgdata_egg;
  STRBUF*                     text_strbuf;
  BOOL                        text_show_vblank_req;  // TRUE�̂Ƃ��e�L�X�g��\������BG�ʂ�\������

  // �͂��E�������E�B���h�E
  BMPWIN_YESNO_DAT            tm_dat;
  BMPMENU_WORK*               tm_wk;
  TM_RESULT                   tm_result;

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

// BG
static void Egg_Demo_BgInit( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static void Egg_Demo_BgExit( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );

// OBJ
static void Egg_Demo_ObjInit( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static void Egg_Demo_ObjExit( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static void Egg_Demo_ObjStartAnime( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static BOOL Egg_Demo_ObjIsEndAnime( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );

// �T�E���h
static void Egg_Demo_SoundInit( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static void Egg_Demo_SoundExit( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static void Egg_Demo_SoundMain( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static BOOL Egg_Demo_SoundCheckFadeOutField( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static void Egg_Demo_SoundPlayIntro( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static BOOL Egg_Demo_SoundCheckPlayIntro( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static void Egg_Demo_SoundPlayHatch( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static void Egg_Demo_SoundPushHatch( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static void Egg_Demo_SoundPopHatch( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static void Egg_Demo_SoundPlayCongratulate( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static BOOL Egg_Demo_SoundCheckPlayCongratulate( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static void Egg_Demo_SoundFadeOutHatch( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static BOOL Egg_Demo_SoundCheckFadeOutHatch( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );
static void Egg_Demo_SoundFadeInField( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work );

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

#if 0

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

#endif


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
    work->trunk_step    = TRUNK_STEP_FADE_IN_START;
    work->trunk_wait    = 5;  // �ŏ���ʂ������̂ŁA���΂��^���Â̂܂ܑ҂�
    work->sound_step    = SOUND_STEP_WAIT;
    work->text_step     = TEXT_STEP_WAIT;
    work->tm_step       = TM_STEP_WAIT;
  }

  Egg_Demo_Init( param, work );

  // �T�E���h
  Egg_Demo_SoundInit( param, work );
  
  // �t�F�[�h�C��(������)
  GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 16, 0 );

  // ���[�J��PROC�V�X�e�����쐬
  work->local_procsys = GFL_PROC_LOCAL_boot( work->heap_id );

  // �ʐM�A�C�R��
  GFL_NET_WirelessIconEasy_HoldLCD( FALSE, work->heap_id );
  GFL_NET_ReloadIcon();

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

  // �ʐM�A�C�R��
  GFL_NET_WirelessIconEasy_DefaultLCD();

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
  case TRUNK_STEP_FADE_IN_START:
    {
      if( work->trunk_wait == 0 )
      {
        // ����
        work->trunk_step = TRUNK_STEP_FADE_IN;

        // �t�F�[�h�C��(����������)
        GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, FADE_IN_WAIT );
      }
      else
      {
        work->trunk_wait--;
      }
    }
    break;
  case TRUNK_STEP_FADE_IN:
    {
      if( !GFL_FADE_CheckFade() )
      {
        // ����
        work->trunk_step = TRUNK_STEP_BELT_OPEN;

        Egg_Demo_ObjStartAnime( param, work );
      }
    }
    break;
  case TRUNK_STEP_BELT_OPEN:
    {
      if(    ( !Egg_Demo_SoundCheckFadeOutField( param, work ) )
          && Egg_Demo_ObjIsEndAnime( param, work ) )
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
        work->trunk_step = TRUNK_STEP_DEMO_HATCH;

        Egg_Demo_SoundPushHatch( param, work );
      }
    }
    break;
  case TRUNK_STEP_DEMO_HATCH:
    {
      // ����
      work->trunk_step = TRUNK_STEP_DEMO_READY;

      // �^�}�S�z��
      {
        //PP_Put( param->pp, ID_PARA_tamago_flag, 0 );  // �^�}�S�t���O�𗎂Ƃ������ł͕s���S�ȏ������ɂȂ��Ă��܂��̂ŁAPP_Birth���ĂԁB
        PLAYER_WORK* player_wk = GAMEDATA_GetMyPlayerWork( param->gamedata );
        PP_Birth(  // PP_Birth�̓g���[�i�[�����̐ݒ�����Ă����̂ŁA���ɂ���g���[�i�[�����̐ݒ�̓R�����g�A�E�g�����B
            param->pp,
            GAMEDATA_GetMyStatus( param->gamedata ),
            ZONEDATA_GetPlaceNameID( PLAYERWORK_getZoneID( player_wk ) ),
            work->heap_id );
      }

      // �}�ӓo�^�i�߂܂����j
      {
        ZUKAN_SAVEDATA* zukan_savedata = GAMEDATA_GetZukanSave( param->gamedata );
        ZUKANSAVE_SetPokeSee( zukan_savedata, param->pp );  // ����
        ZUKANSAVE_SetPokeGet( zukan_savedata, param->pp );  // �߂܂���
      }

/*
      {
        // �g���[�i�[����
        PLAYER_WORK* player_wk = GAMEDATA_GetMyPlayerWork( param->gamedata );
        POKE_MEMO_SetTrainerMemoPP(
            param->pp,
            POKE_MEMO_INCUBATION,
            GAMEDATA_GetMyStatus( param->gamedata ),
            ZONEDATA_GetPlaceNameID( PLAYERWORK_getZoneID( player_wk ) ),
            work->heap_id );
      }
*/

      // �^�}�S�z���f���̉��o
      EGG_DEMO_VIEW_Hatch( work->view, param->pp );
    }
    break;
  case TRUNK_STEP_DEMO_READY:
    {
      // �^�}�S�z���f���̉��o
      if( EGG_DEMO_VIEW_IsReady( work->view ) )
      {
        // ����
        work->trunk_step = TRUNK_STEP_DEMO_MON;
        
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
        //work->trunk_step = TRUNK_STEP_TEXT_0;
        work->trunk_step = TRUNK_STEP_TEXT_0_0;

        Egg_Demo_TextShowWindow( param, work );
        //Egg_Demo_TextMakeStream( param, work, egg_evemt_02 );
        
        Egg_Demo_SoundPlayCongratulate( param, work );
      }
    }
    break;
  
  case TRUNK_STEP_TEXT_0_0:
    {
      work->text_show_vblank_req = TRUE;
      // ����
      work->trunk_step = TRUNK_STEP_TEXT_0_1;
    }
    break;
  case TRUNK_STEP_TEXT_0_1:
    {
      Egg_Demo_TextMakeStream( param, work, egg_evemt_02 );
      // ����
      work->trunk_step = TRUNK_STEP_TEXT_0;
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
        work->trunk_step = TRUNK_STEP_SOUND_FADE_OUT;

        // �t�F�[�h�A�E�g(�����遨��)
        GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, FADE_OUT_WAIT );

        Egg_Demo_SoundFadeOutHatch( param, work );

        // �^�b�`or�L�[
        GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );  // �͂��E�������E�B���h�E�ɉ����ďI���̂ŁA�K���L�[�ŏI��
      }
      else if( tm_result == TM_RESULT_YES )
      {
        // ����
        work->trunk_step = TRUNK_STEP_NAMEIN_FADE_OUT;

        // �t�F�[�h�A�E�g(�����遨��)
        GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, NAMEIN_FADE_OUT_WAIT );

        // �^�b�`or�L�[
        GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );  // �͂��E�������E�B���h�E�ɉ����ďI���̂ŁA�K���L�[�ŏI��
      }
    }
    break;
  case TRUNK_STEP_NAMEIN_FADE_OUT:
    {
      if( !GFL_FADE_CheckFade() )
      {
        MISC *misc  = SaveData_GetMisc( GAMEDATA_GetSaveControlWork( param->gamedata ) );

        Egg_Demo_Exit( param, work );

        // ����
        work->trunk_step = TRUNK_STEP_NAMEIN;

        // ���O���͂֍s��
        work->namein_param = NAMEIN_AllocParamPokemonByPP( work->heap_id, param->pp, NAMEIN_POKEMON_LENGTH, NULL, misc );
        // ���[�J��PROC�Ăяo��
        GFL_PROC_LOCAL_CallProc( work->local_procsys, FS_OVERLAY_ID(namein), &NameInputProcData, work->namein_param );
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

          // �|�P�����Ƀj�b�N�l�[����t������
          {
            RECORD* rec = GAMEDATA_GetRecordPtr(param->gamedata); 
            RECORD_Inc(rec, RECID_NICKNAME);
          }
        }
        NAMEIN_FreeParam( work->namein_param );

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
      if( !Egg_Demo_SoundCheckFadeOutHatch( param, work ) )
      {
        // ����
        work->trunk_step = TRUNK_STEP_NAMEIN_END;
 
        Egg_Demo_SoundFadeInField( param, work );
        
        return GFL_PROC_RES_FINISH;
      }
    }
    break;
  case TRUNK_STEP_SOUND_FADE_OUT:
    {
      if( !Egg_Demo_SoundCheckFadeOutHatch( param, work ) )
      {
        // ����
        work->trunk_step = TRUNK_STEP_FADE_OUT;
        
        Egg_Demo_SoundFadeInField( param, work );
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

  // �T�E���h
  Egg_Demo_SoundMain( param, work );
  
  if(    work->trunk_step != TRUNK_STEP_NAMEIN
      && work->trunk_step != TRUNK_STEP_NAMEIN_BLACK
      && work->trunk_step != TRUNK_STEP_NAMEIN_END
      && work->trunk_step != TRUNK_STEP_END )
  {
    // TEXT
    Egg_Demo_TextMain( param, work );
    // �͂��E�������E�B���h�E
    Egg_Demo_TmMain( param, work );

    // �^�}�S�z���f���̉��o
    EGG_DEMO_VIEW_Main( work->view );

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

  // �e�L�X�g��\������BG��
  if( work->text_show_vblank_req )
  {
    GFL_BG_SetVisible( BG_FRAME_M_TEXT               , VISIBLE_ON );
    work->text_show_vblank_req = FALSE;
  }
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

  // BG
  Egg_Demo_BgInit( param, work );
  // OBJ
  Egg_Demo_ObjInit( param, work );
  // TEXT
  Egg_Demo_TextInit( param, work );
  // �͂��E�������E�B���h�E
  Egg_Demo_TmInit( param, work );

  // �v���C�I���e�B�A�\���A�w�i�F�Ȃ�
  {
    GFL_BG_SetPriority( BG_FRAME_M_POKEMON           , BG_FRAME_PRI_M_POKEMON );
    GFL_BG_SetPriority( BG_FRAME_M_BELT              , BG_FRAME_PRI_M_BELT );
    GFL_BG_SetPriority( BG_FRAME_M_TEXT              , BG_FRAME_PRI_M_TEXT );  // �őO��

    GFL_BG_SetPriority( BG_FRAME_S_BACK              , 0 );  // �őO��

    GFL_BG_SetVisible( BG_FRAME_M_POKEMON            , VISIBLE_ON );
    GFL_BG_SetVisible( BG_FRAME_M_BELT               , VISIBLE_ON );
    //GFL_BG_SetVisible( BG_FRAME_M_TEXT               , VISIBLE_ON );  // �e�L�X�g��\������BG��
  
    GFL_BG_SetVisible( BG_FRAME_S_BACK               , VISIBLE_ON );

    // �p�[�e�B�N���Ή�
    G2_SetBlendAlpha( /*GX_BLEND_PLANEMASK_NONE,*/GX_BLEND_PLANEMASK_BG0,
                      GX_BLEND_PLANEMASK_BD | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG0,
                      0, 0 );

    GFL_BG_SetBackGroundColor( GFL_BG_FRAME0_M, 0x4210 );
    GFL_BG_SetBackGroundColor( GFL_BG_FRAME0_S, 0x0000 );
  }

  // �^�}�S�z���f���̉��o
  work->view = EGG_DEMO_VIEW_Init( work->heap_id, param->pp );
}
static void Egg_Demo_Exit( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  // �^�}�S�z���f���̉��o
  EGG_DEMO_VIEW_Exit( work->view );

  // �͂��E�������E�B���h�E
  Egg_Demo_TmExit( param, work );
  // TEXT
  Egg_Demo_TextExit( param, work );
  // OBJ
  Egg_Demo_ObjExit( param, work );
  // BG
  Egg_Demo_BgExit( param, work );

  // VBlank��TCB
  GFL_TCB_DeleteTask( work->vblank_tcb );

  // �O���t�B�b�N�A�t�H���g�Ȃ�
  {
    PRINTSYS_QUE_Clear( work->print_que );
    PRINTSYS_QUE_Delete( work->print_que );
    GFL_FONT_Delete( work->font );
    EGG_DEMO_GRAPHIC_Exit( work->graphic );
  }
}

//-------------------------------------
/// BG
//=====================================
static void Egg_Demo_BgInit( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  ARCHANDLE* handle = GFL_ARC_OpenDataHandle( ARCID_EGG_DEMO, work->heap_id );

  // BELT
  GFL_ARCHDL_UTIL_TransVramPalette(
      handle,
      NARC_egg_demo_particle_demo_egg_NCLR,
      PALTYPE_MAIN_BG,
      BG_PAL_POS_M_BELT * 0x20,
      BG_PAL_NUM_M_BELT * 0x20,
      work->heap_id );

  GFL_ARCHDL_UTIL_TransVramBgCharacter(
      handle,
      NARC_egg_demo_particle_demo_egg_bg_NCGR,
      BG_FRAME_M_BELT,
			0,
      0,  // �S�]��
      FALSE,
      work->heap_id );

  GFL_ARCHDL_UTIL_TransVramScreen(
      handle,
      NARC_egg_demo_particle_demo_egg_NSCR,
      BG_FRAME_M_BELT,
      0,
      0,  // �S�]��
      FALSE,
      work->heap_id );

  GFL_ARC_CloseDataHandle( handle );

  GFL_BG_LoadScreenReq( BG_FRAME_M_BELT );
}
static void Egg_Demo_BgExit( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  // �������Ȃ�
}

//-------------------------------------
/// OBJ
//=====================================
static void Egg_Demo_ObjInit( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  u8 i;

  // ���\�[�X�ǂݍ���
  ARCHANDLE* handle = GFL_ARC_OpenDataHandle( ARCID_EGG_DEMO, work->heap_id );

  work->obj_res[OBJ_RES_BELT_NCL] = GFL_CLGRP_PLTT_RegisterEx( 
          handle,
          NARC_egg_demo_particle_demo_egg_NCLR,
          CLSYS_DRAW_MAIN,
          OBJ_PAL_POS_M_BELT * 0x20,
          0,
          OBJ_PAL_NUM_M_BELT,
          work->heap_id );
      
  work->obj_res[OBJ_RES_BELT_NCG] = GFL_CLGRP_CGR_Register(
          handle,
          NARC_egg_demo_particle_demo_egg_NCGR,
          FALSE,
          CLSYS_DRAW_MAIN,
          work->heap_id );

  work->obj_res[OBJ_RES_BELT_NCE] = GFL_CLGRP_CELLANIM_Register(
          handle,
          NARC_egg_demo_particle_demo_egg_NCER,
          NARC_egg_demo_particle_demo_egg_NANR,
          work->heap_id );
  
  GFL_ARC_CloseDataHandle( handle );

  // CLWK�쐬
  for( i=0; i<OBJ_CELL_MAX; i++ )
  {
    work->obj_clwk[i] = GFL_CLACT_WK_Create(
        EGG_DEMO_GRAPHIC_GetClunit( work->graphic ),
        work->obj_res[OBJ_RES_BELT_NCG],
        work->obj_res[OBJ_RES_BELT_NCL],
        work->obj_res[OBJ_RES_BELT_NCE],
        &obj_cell_data[i],
        CLSYS_DEFREND_MAIN,
        work->heap_id );
    GFL_CLACT_WK_SetAutoAnmFlag( work->obj_clwk[i], FALSE );
  }
}
static void Egg_Demo_ObjExit( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  // CLWK�j��
  u8 i;
  for( i=0; i<OBJ_CELL_MAX; i++ )
  {
    GFL_CLACT_WK_Remove( work->obj_clwk[i] );
  }

  // ���\�[�X�j��
  GFL_CLGRP_CELLANIM_Release( work->obj_res[OBJ_RES_BELT_NCE] );
  GFL_CLGRP_CGR_Release( work->obj_res[OBJ_RES_BELT_NCG] );
  GFL_CLGRP_PLTT_Release( work->obj_res[OBJ_RES_BELT_NCL] );
}
static void Egg_Demo_ObjStartAnime( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  u8 i;
  for( i=0; i<OBJ_CELL_MAX; i++ )
  {
    GFL_CLACT_WK_SetAnmIndex( work->obj_clwk[i], 0 );
    GFL_CLACT_WK_SetAutoAnmFlag( work->obj_clwk[i], TRUE );
  }
}
static BOOL Egg_Demo_ObjIsEndAnime( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  BOOL b_end = TRUE;
  u8 i;
  for( i=0; i<OBJ_CELL_MAX; i++ )
  {
    if( GFL_CLACT_WK_CheckAnmActive( work->obj_clwk[i] ) )
    {
      b_end = FALSE;
      break;
    }
  }
  return b_end;
}

//-------------------------------------
/// �T�E���h
//=====================================
static void Egg_Demo_SoundInit( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  PMSND_FadeOutBGM( FSND_FADE_SHORT );
  work->sound_step = SOUND_STEP_FIELD_FADE_OUT;
}
static void Egg_Demo_SoundExit( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  // �������Ȃ�
}
static void Egg_Demo_SoundMain( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  switch( work->sound_step )
  {
  case SOUND_STEP_WAIT:
    {
    }
    break;
  case SOUND_STEP_FIELD_FADE_OUT:
    {
      if( !PMSND_CheckFadeOnBGM() )
      {
        PMSND_PauseBGM( TRUE );
        PMSND_PushBGM();
        work->sound_step = SOUND_STEP_WAIT;
      }
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
  case SOUND_STEP_CONGRATULATE_LOAD:
    {
      // BGM�̕������[�h�𗘗p���Ă݂�
      BOOL play_start = PMSND_PlayBGMdiv( SEQ_ME_SHINKAOME, &(work->sound_div_seq), FALSE );
      if( play_start )
      {
        work->sound_step = SOUND_STEP_CONGRATULATE;
      }
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
  case SOUND_STEP_HATCH_FADE_OUT:
    {
      if( !PMSND_CheckFadeOnBGM() )
      {
        PMSND_StopBGM();
        work->sound_step = SOUND_STEP_WAIT;
      }
    }
    break;
  }
}
static BOOL Egg_Demo_SoundCheckFadeOutField( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  if( work->sound_step == SOUND_STEP_FIELD_FADE_OUT ) return TRUE;
  else                                                return FALSE;
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
  // �����̓t�F�[�h���Ȃ��Ă���
  PMSND_PauseBGM( TRUE );
  PMSND_PushBGM();
}
static void Egg_Demo_SoundPopHatch( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  PMSND_PopBGM();
  PMSND_PauseBGM( FALSE );
  PMSND_FadeInBGM( FSND_FADE_FAST );
}
static void Egg_Demo_SoundPlayCongratulate( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
/*
  PMSND_PlayBGM(SEQ_ME_SHINKAOME);
  work->sound_step = SOUND_STEP_CONGRATULATE;
*/
  // BGM�̕������[�h�𗘗p���Ă݂�
  work->sound_div_seq = 0;
  PMSND_PlayBGMdiv( SEQ_ME_SHINKAOME, &(work->sound_div_seq), TRUE );
  work->sound_step = SOUND_STEP_CONGRATULATE_LOAD;
}
static BOOL Egg_Demo_SoundCheckPlayCongratulate( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  return ( work->sound_step == SOUND_STEP_CONGRATULATE_LOAD || work->sound_step == SOUND_STEP_CONGRATULATE );
}
static void Egg_Demo_SoundFadeOutHatch( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  PMSND_FadeOutBGM( FSND_FADE_NORMAL );
  work->sound_step = SOUND_STEP_HATCH_FADE_OUT;
}
static BOOL Egg_Demo_SoundCheckFadeOutHatch( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  if( work->sound_step == SOUND_STEP_HATCH_FADE_OUT ) return TRUE;
  else                                                return FALSE;
}
static void Egg_Demo_SoundFadeInField( EGG_DEMO_PARAM* param, EGG_DEMO_WORK* work )
{
  // �t�B�[���hBGM
  PMSND_PopBGM();
  PMSND_PauseBGM( FALSE );
  PMSND_FadeInBGM( FSND_FADE_NORMAL );
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

  // �e�L�X�g��\������BG��
  work->text_show_vblank_req = FALSE;
  GFL_BG_SetVisible( BG_FRAME_M_TEXT               , VISIBLE_OFF );
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
/*
  BmpWinFrame_Write( work->text_winin_bmpwin, WINDOW_TRANS_ON_V,
      GFL_ARCUTIL_TRANSINFO_GetPos(work->text_winfrm_tinfo),
      BG_PAL_POS_M_TEXT_FRAME );
  GFL_BMPWIN_MakeTransWindow_VBlank( work->text_winin_bmpwin );
*/

  BmpWinFrame_Write( work->text_winin_bmpwin, WINDOW_TRANS_ON,
      GFL_ARCUTIL_TRANSINFO_GetPos(work->text_winfrm_tinfo),
      BG_PAL_POS_M_TEXT_FRAME );
  GFL_BMPWIN_MakeTransWindow( work->text_winin_bmpwin );
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
  case PRINTSTREAM_STATE_RUNNING:
    if( ( GFL_UI_KEY_GetCont() & ( PAD_BUTTON_A | PAD_BUTTON_B ) ) || GFL_UI_TP_GetCont() )
    {
      PRINTSYS_PrintStreamShortWait( work->text_stream, 0 );
    }
    break;
  case PRINTSTREAM_STATE_PAUSE:
    if( ( GFL_UI_KEY_GetTrg() & ( PAD_BUTTON_A | PAD_BUTTON_B ) ) || GFL_UI_TP_GetTrg() )
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
  work->tm_dat.palnum    = BG_PAL_POS_M_TEXT_FONT;
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

