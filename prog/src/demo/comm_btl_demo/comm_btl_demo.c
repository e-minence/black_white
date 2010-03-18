//=============================================================================
/**
 *
 *  @file   comm_btl_demo.c
 *  @brief  �ʐM�o�g���f��
 *  @author hosaka genya
 *  @data   2009.09.29
 *
 */
//=============================================================================
//�K���K�v�ȃC���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

//SE
#include "sound/pm_sndsys.h"

#include "poke_tool/pokeparty.h" // for POKEPARTY

//FONT
#include "print/gf_font.h"
#include "font/font.naix"

//PRINT_QUE
#include "print/printsys.h"

#include "print/wordset.h"

//�Ȉ�CLWK�ǂݍ��݁��J�����[�e�B���e�B�[
#include "ui/ui_easy_clwk.h"

//�V�[���R���g���[���[
#include "ui/ui_scene.h"

//�`��ݒ�
#include "comm_btl_demo_graphic.h"

//�A�v�����ʑf��
#include "app/app_menu_common.h"

//�A�[�J�C�u
#include "arc_def.h"

//�O�����J
#include "demo/comm_btl_demo.h"

#include "message.naix"
#include "comm_btl_demo.naix" // �A�[�J�C�u

#include "comm_btl_demo_res.cdat"

#define STR_TRNAME_SIZE (16)

FS_EXTERN_OVERLAY(ui_common);

#ifdef PM_DEBUG

#define DEBUG_LOOP
static BOOL g_debug_loop = FALSE;

// �A�v�����Ńf�o�b�O�p�̃p�����[�^���Z�b�g
//#define DEBUG_SET_PARAM

#endif // PM_DEBUG

//=============================================================================
/**
 *                �萔��`
 */
//=============================================================================
enum
{ 
  //@TODO �f�J�߂ɂƂ��Ă���
  COMM_BTL_DEMO_HEAP_SIZE = 0x70000,  ///< �q�[�v�T�C�Y

  TRAINER_CNT_NORMAL = 2,
  TRAINER_CNT_MULTI = 4,
};

enum
{ 
  SE_BALL_IN = SEQ_SE_SYS_65,   ///< �{�[�����o��
  SE_VS_IN   = SEQ_SE_SYS_66,   ///< VS�\��
  SE_WIN_IN  = SEQ_SE_SYS_67,   ///< WIN�\��
};

//--------------------------------------------------------------
/// ���U���g�\���ʒu
//==============================================================
typedef enum
{ 
  RESULT_POS_ME = 0,
  RESULT_POS_YOU,
  RESULT_POS_DRAW,
  RESULT_POS_MAX,
} RESULT_POS;

enum
{ 
  OBJ_ANM_ID_RSLT_WIN   = 7,    ///< ���sOBJ�FWIN
  OBJ_ANM_ID_RSLT_LOSE  = 8,    ///< ���sOBJ�FLOSE
  OBJ_ANM_ID_RSLT_DRAW  = 9,    ///< ���sOBJ�FDRAW
  OBJ_ANM_ID_BALL_NORMAL = 10,  ///< �{�[���F�ʏ�
  OBJ_ANM_ID_BALL_SICK = 11,    ///< �{�[���F��Ԉُ�
  OBJ_ANM_ID_BALL_DOWN = 12,    ///< �{�[���F�m��
  OBJ_ANM_ID_BALL_NULL = 13,    ///< �{�[���F��
};

enum
{ 
  RESULT_PTC_SYNC = 180, // WIN���Ƀp�[�e�B�N�����o���Ԋu

  DRAW_OPEN_SYNC = 30, //DRAW�p�A�j���Đ�����uDRAW�v��\������܂ł�SYNC

  WIN_OPEN_SYNC   = 15,
  LOSE_OPEN_SYNC  = WIN_OPEN_SYNC+30,

  START_DEMO_VS_OPEN_SYNC = 15, // VS�o�����̃E�F�C�g
  START_DEMO_VS_OPEN_WAIT_SYNC = START_DEMO_VS_OPEN_SYNC+10, // VS�o�����̃E�F�C�g
  START_DEMO_FADEOUT_SYNC = 160, //�uVS�v�\���J�n����̃E�F�C�g
  START_DEMO_FINISH_SYNC = START_DEMO_FADEOUT_SYNC + 8,

  PTC_WIN_WAIT_SYNC = 120,

  END_DEMO_FADEOUT_SYNC = 120,
  END_DEMO_END_SYNC = END_DEMO_FADEOUT_SYNC + 8,

  // �c��|�P��������\������SYNC
  NORMAL_POKENUM_OPEN_SYNC = 30,  ///<
  MULTI_POKENUM_OPEN_SYNC  = 30,  ///< 
  MULTI_POKENUM_OPEN_SYNC2 = 40,  ///<

  // �{�[��
  BALL_PX_OFS = 16,
  NORMAL_BALL_PX_BASE = 128+8,

  NORMAL_POSID0_BALL_PY = 7*8,
  NORMAL_POSID1_BALL_PY = 21*8,

  MULTI_POSID0_BALL_PX_BASE = 10*8,
  MULTI_POSID1_BALL_PX_BASE = 18*8,
  MULTI_POSID2_BALL_PX_BASE = 16*8,
  MULTI_POSID3_BALL_PX_BASE = 24*8,

  MULTI_POSID0_BALL_PY = 17*8,
  MULTI_POSID1_BALL_PY = 22*8,
  MULTI_POSID2_BALL_PY = 6*8,
  MULTI_POSID3_BALL_PY = 11*8,

  // �g���[�i�[��
  TRNAME_CSX = 10,
  TRNAME_CSY = 3,
  TRNAME_OPEN_SYNC = 45, ///< �\���J�nSYNC

  NORMAL_POSID0_TRNAME_CPX = 16,
  NORMAL_POSID0_TRNAME_CPY = 3,

  NORMAL_POSID1_TRNAME_CPX = 4,
  NORMAL_POSID1_TRNAME_CPY = 17,
  
  NORMAL_POSID0_TRNAME_PTC_PX = 0xffffd800, 
  NORMAL_POSID0_TRNAME_PTC_PY = 0xffffe800,

  NORMAL_POSID1_TRNAME_PTC_PX = 0x800,
  NORMAL_POSID1_TRNAME_PTC_PY = 0x2000,

  MULTI_POSID0_TRNAME_PTC_PX = 0xffffe000,
  MULTI_POSID0_TRNAME_PTC_PY = 0xfffff800,

  MULTI_POSID1_TRNAME_PTC_PX = 0x0,
  MULTI_POSID1_TRNAME_PTC_PY = 0xffffe000,

  MULTI_POSID2_TRNAME_PTC_PX = 0x400,
  MULTI_POSID2_TRNAME_PTC_PY = 0x2000,

  MULTI_POSID3_TRNAME_PTC_PX = 0x2400,
  MULTI_POSID3_TRNAME_PTC_PY = 0x1000,

  MULTI_POSID0_TRNAME_CPX = 5,
  MULTI_POSID0_TRNAME_CPY = 13,

  MULTI_POSID1_TRNAME_CPX = 5+8,
  MULTI_POSID1_TRNAME_CPY = 19,

  MULTI_POSID2_TRNAME_CPX = 15,
  MULTI_POSID2_TRNAME_CPY = 1,

  MULTI_POSID3_TRNAME_CPX = 15+8,
  MULTI_POSID3_TRNAME_CPY = 7,

  BALL_PTC_DIFF_X = 0x800,

  NORMAL_POSID0_BALL_PTC_BASE_X = 0x300,
  NORMAL_POSID0_BALL_PTC_BASE_Y = 0x1300,

  NORMAL_POSID1_BALL_PTC_BASE_X = 0xfffffc00,
  NORMAL_POSID1_BALL_PTC_BASE_Y = 0xffffdb00,
  
  MULTI_POSID0_BALL_PTC_BASE_X = 0xffffe800,
  MULTI_POSID0_BALL_PTC_BASE_Y = 0xffffeb00,
  
  MULTI_POSID1_BALL_PTC_BASE_X = 0x800,
  MULTI_POSID1_BALL_PTC_BASE_Y = 0xffffd800,

  MULTI_POSID2_BALL_PTC_BASE_X = 0xffffff00,
  MULTI_POSID2_BALL_PTC_BASE_Y = 0x1800,

  MULTI_POSID3_BALL_PTC_BASE_X = 0x1f00,
  MULTI_POSID3_BALL_PTC_BASE_Y = 0x300,

};

//-------------------------------------
/// �t���[��
//=====================================
enum
{ 
  BG_FRAME_TEXT_M = GFL_BG_FRAME1_M,
  BG_FRAME_BAR_M  = GFL_BG_FRAME2_M,
  BG_FRAME_BACK_M = GFL_BG_FRAME3_M,
  BG_FRAME_BACK_S = GFL_BG_FRAME2_S,
  BG_FRAME_TEXT_S = GFL_BG_FRAME0_S, 
};

//-------------------------------------
/// �p���b�g
//=====================================
enum
{ 
  //���C��BG
  PLTID_BG_BACK_M       = 0,
  PLTID_BG_TEXT_M       = 1,
  PLTID_BG_TASKMENU_M   = 11,
  PLTID_BG_TOUCHBAR_M   = 13,
  PLTID_BG_INFOWIN_M    = 15,
  //�T�uBG
  PLTID_BG_BACK_S       = 0,

  // Main OBJ
  PLTID_OBJ_COMMON_M = 0,
};

//=============================================================================
/**
 *                �\���̒�`
 */
//=============================================================================

//--------------------------------------------------------------
/// BG�Ǘ����[�N
//==============================================================
typedef struct 
{
  int dummy;
} COMM_BTL_DEMO_BG_WORK;

//--------------------------------------------------------------
/// OBJ���[�N
//==============================================================
typedef struct {
  //[IN]
  COMM_BTL_DEMO_GRAPHIC_WORK* graphic;
  HEAPID  heapID;
  //[PRIVATE]
  UI_EASY_CLWK_RES clres_common;
} COMM_BTL_DEMO_OBJ_WORK;

//--------------------------------------------------------------
/// G3D�Ǘ����[�N
//==============================================================
typedef struct {
  //[IN]
  COMM_BTL_DEMO_GRAPHIC_WORK* graphic;
  HEAPID  heapID;
  //[PRIVATE]
  GFL_G3D_UTIL*   g3d_util;
  u16 anm_unit_idx;   ///< �A�j���[�V����������UNIT��IDX
  u8 is_end;
  u8 is_add;
  
  GFL_PTC_PTR ptc;
  u8          spa_work[ PARTICLE_LIB_HEAP_SIZE ];
  u8          spa_num;

} COMM_BTL_DEMO_G3D_WORK;

#define BALL_CLWK_MAX ( 6 )

//--------------------------------------------------------------
/// �{�[��UNIT���[�N
//==============================================================
typedef struct {
  //[IN]
  const POKEPARTY* party;
  COMM_BTL_DEMO_G3D_WORK* g3d;
  u8 type;
  u8 posid;
  u8 num;
  u8 max;
  //[PRIVATE]
  GFL_CLWK* clwk[ BALL_CLWK_MAX ];
  u32 timer;
  u8 is_start;
  u8 padding[3];
} BALL_UNIT;

//--------------------------------------------------------------
/// �g���[�i�[���j�b�g
//==============================================================
typedef struct {
  // [IN]
  const COMM_BTL_DEMO_TRAINER_DATA* data;
  COMM_BTL_DEMO_OBJ_WORK* obj;
  COMM_BTL_DEMO_G3D_WORK* g3d;
  GFL_FONT* font;
  u8 type;
  u8 posid;
  u8 padding[2];
  // [PRIVATE]
  int trsex; // PM_MALE or PM_FEMALE
  STRBUF* str_trname;
  GFL_BMPWIN* win_name;   // �g���[�i�[��
  BALL_UNIT ball;         // �{�[���Ǘ����[�N
  u32 timer;
} TRAINER_UNIT;

//--------------------------------------------------------------
/// ���s���j�b�g
//==============================================================
typedef struct {
  // [IN]
  COMM_BTL_DEMO_RESULT result;
  COMM_BTL_DEMO_OBJ_WORK* obj;
  COMM_BTL_DEMO_G3D_WORK* g3d;
  u16 pokenum_me;
  u16 pokenum_you;
  // [PRIVATE]
  GFL_CLWK* clwk_pokenum[2]; // �|�P�������\���i��f���p�j
  GFL_CLWK* clwk[ RESULT_POS_MAX ];
  u16 timer;
  u8 type;
  u8 padding;
} RESULT_UNIT;


//--------------------------------------------------------------
/// ���C�����[�N
//==============================================================
typedef struct 
{
  HEAPID heapID;
  COMM_BTL_DEMO_PARAM * pwk;

  COMM_BTL_DEMO_BG_WORK   wk_bg;
  COMM_BTL_DEMO_OBJ_WORK  wk_obj;
  COMM_BTL_DEMO_G3D_WORK  wk_g3d;

  // �g���[�i�[���j�b�g
  TRAINER_UNIT  trainer_unit[ COMM_BTL_DEMO_TRDATA_MAX ];

  RESULT_UNIT result_unit;

  //�`��ݒ�
  COMM_BTL_DEMO_GRAPHIC_WORK  *graphic;

  //�t�H���g
  GFL_FONT            *font;

  //�v�����g�L���[
  PRINT_QUE           *print_que;
  GFL_MSGDATA         *msg;
  
  // �V�[���R���g���[��
  UI_SCENE_CNT_PTR  cntScene;

  u8 type;
  COMM_BTL_DEMO_RESULT result;
  int timer;  ///< �f���N�����ԃJ�E���^

} COMM_BTL_DEMO_MAIN_WORK;


//=============================================================================
/**
 *              �V�[����`
 */
//=============================================================================

// �J�n�f��
static BOOL SceneStartDemo_Init( UI_SCENE_CNT_PTR cnt, void* work );
static BOOL SceneStartDemo_Main( UI_SCENE_CNT_PTR cnt, void* work );
static BOOL SceneStartDemo_End( UI_SCENE_CNT_PTR cnt, void* work );

// �I���f��
static BOOL SceneEndDemo_Init( UI_SCENE_CNT_PTR cnt, void* work );
static BOOL SceneEndDemo_Main( UI_SCENE_CNT_PTR cnt, void* work );
static BOOL SceneEndDemo_End( UI_SCENE_CNT_PTR cnt, void* work );

// �I�����������f���i�I���f�����番��j
static BOOL SceneEndDemoDraw_Main( UI_SCENE_CNT_PTR cnt, void* work );
static BOOL SceneEndDemoDraw_End( UI_SCENE_CNT_PTR cnt, void* work );

//--------------------------------------------------------------
/// SceneID
//==============================================================
typedef enum
{ 
  CBD_SCENE_ID_START_DEMO = 0,  ///< �o�g���J�n�f��
  CBD_SCENE_ID_END_DEMO,        ///< �o�g���I���f��
  CBD_SCENE_ID_END_DEMO_DRAW,   ///< �o�g���I���f�� ��������

  CBD_SCENE_ID_MAX,
} CBD_SCENE_ID;

//--------------------------------------------------------------
/// SceneFunc
//==============================================================
static const UI_SCENE_FUNC_SET c_scene_func_tbl[ CBD_SCENE_ID_MAX ] = 
{
  // CBD_SCENE_ID_START_DEMO
  {
    SceneStartDemo_Init,
    NULL,
    SceneStartDemo_Main,
    NULL,
    SceneStartDemo_End,
  },
  // CBD_SCENE_ID_END_DEMO
  {
    SceneEndDemo_Init,
    NULL,
    SceneEndDemo_Main,
    NULL,
    SceneEndDemo_End,
  },
  // CBD_SCENE_ID_END_DEMO_DRAW
  {
    NULL,
    NULL,
    SceneEndDemoDraw_Main,
    NULL,
    SceneEndDemoDraw_End,
  },
};


//=============================================================================
/**
 *              �v���g�^�C�v�錾
 */
//=============================================================================
static CBD_SCENE_ID calc_first_scene( COMM_BTL_DEMO_PARAM* pwk );
BOOL type_is_normal( u8 type );
BOOL type_is_start( u8 type );

static void BALL_UNIT_Init( BALL_UNIT* unit, const POKEPARTY* party, u8 type, u8 posid, COMM_BTL_DEMO_OBJ_WORK* obj, COMM_BTL_DEMO_G3D_WORK* g3d );
static void BALL_UNIT_Exit( BALL_UNIT* unit );
static void BALL_UNIT_Main( BALL_UNIT* unit );
static void BALL_UNIT_SetStart( BALL_UNIT* unit );
static void BALL_UNIT_SetPartyCondition( BALL_UNIT* unit, const POKEPARTY* party, int id );

static void TRAINER_UNIT_Init( TRAINER_UNIT* unit, u8 type, u8 posid, const COMM_BTL_DEMO_TRAINER_DATA* data, COMM_BTL_DEMO_OBJ_WORK* obj, COMM_BTL_DEMO_G3D_WORK* g3d, GFL_FONT* font );
static void TRAINER_UNIT_Exit( TRAINER_UNIT* unit );
static void TRAINER_UNIT_Main( TRAINER_UNIT* unit );
static void TRAINER_UNIT_DrawTrainerName( TRAINER_UNIT* unit, GFL_FONT *font );

static void TRAINER_UNIT_CNT_Init( COMM_BTL_DEMO_MAIN_WORK* wk );
static void TRAINER_UNIT_CNT_Exit( COMM_BTL_DEMO_MAIN_WORK* wk );
static void TRAINER_UNIT_CNT_Main( COMM_BTL_DEMO_MAIN_WORK* wk );
static void TRAINER_UNIT_CNT_BallSetStart( COMM_BTL_DEMO_MAIN_WORK* wk );

static void RESULT_UNIT_Init( RESULT_UNIT* unit, COMM_BTL_DEMO_OBJ_WORK* obj, COMM_BTL_DEMO_G3D_WORK* g3d, u16 pokenum_me, u16 pokenum_you, u8 type );
static void RESULT_UNIT_Exit( RESULT_UNIT* unit );
static void RESULT_UNIT_Main( RESULT_UNIT* unit );
static void RESULT_UNIT_SetOpen( RESULT_UNIT* unit, RESULT_POS pos, COMM_BTL_DEMO_RESULT result );

static void OBJ_Init( COMM_BTL_DEMO_OBJ_WORK* wk, COMM_BTL_DEMO_GRAPHIC_WORK* graphic, HEAPID heapID );
static void OBJ_Exit( COMM_BTL_DEMO_OBJ_WORK* wk );
static GFL_CLWK* OBJ_CreateCLWK( COMM_BTL_DEMO_OBJ_WORK* wk, s16 px, s16 py, u16 anim );
static GFL_CLWK* OBJ_CreatePokeNum( COMM_BTL_DEMO_OBJ_WORK* obj, u8 posid, u8 pokenum );

static void G3D_Init( COMM_BTL_DEMO_G3D_WORK* g3d, COMM_BTL_DEMO_GRAPHIC_WORK* graphic, HEAPID heapID );
static void G3D_End( COMM_BTL_DEMO_G3D_WORK* g3d );
static void G3D_Main( COMM_BTL_DEMO_G3D_WORK * g3d );
static void G3D_PTC_Setup( COMM_BTL_DEMO_G3D_WORK* g3d, int spa_idx );
static void G3D_PTC_Delete( COMM_BTL_DEMO_G3D_WORK* g3d );
static void G3D_PTC_Exit( COMM_BTL_DEMO_G3D_WORK* g3d );
static void G3D_PTC_CreateEmitter( COMM_BTL_DEMO_G3D_WORK * g3d, u16 idx, const VecFx32* pos );
static void G3D_PTC_CreateEmitterAll( COMM_BTL_DEMO_G3D_WORK* g3d, const VecFx32* pos );
static BOOL G3D_OBJ_PaletteReBind( GFL_G3D_OBJ* obj, int mat_idx, int pal_idx );
static void G3D_AnimeSet( COMM_BTL_DEMO_G3D_WORK* g3d, u16 demo_id );
static void G3D_AnimeDel( COMM_BTL_DEMO_G3D_WORK* g3d );
static void G3D_AnimeExit( COMM_BTL_DEMO_G3D_WORK* g3d );
static BOOL G3D_AnimeMain( COMM_BTL_DEMO_G3D_WORK* g3d );

  //-------------------------------------
/// PROC
//=====================================
static GFL_PROC_RESULT CommBtlDemoProc_Init( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
static GFL_PROC_RESULT CommBtlDemoProc_Main( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
static GFL_PROC_RESULT CommBtlDemoProc_Exit( GFL_PROC *proc, int *seq, void *pwk, void *mywk );

//-------------------------------------
/// �ėp�������[�e�B���e�B
//=====================================
static void CommBtlDemo_BG_LoadResource( COMM_BTL_DEMO_BG_WORK* wk, HEAPID heapID );

//=============================================================================
/**
 *                �O�����J
 */
//=============================================================================
const GFL_PROC_DATA CommBtlDemoProcData = 
{
  CommBtlDemoProc_Init,
  CommBtlDemoProc_Main,
  CommBtlDemoProc_Exit,
};

#ifdef DEBUG_SET_PARAM
// ���[�N����
static void debug_param( COMM_BTL_DEMO_PARAM* prm )
{ 
  int i;

  HOSAKA_Printf("in param type = %d \n", prm->type);
  
  prm->result = GFUser_GetPublicRand( COMM_BTL_DEMO_RESULT_MAX );

  HOSAKA_Printf( "result = %d \n", prm->result );

  for( i=0; i<COMM_BTL_DEMO_TRDATA_MAX; i++ )
  {
    prm->trainer_data[i].server_version = GFUser_GetPublicRand(2);
    prm->trainer_data[i].mystatus = SaveData_GetMyStatus( SaveControl_GetPointer() );

#if 0
    prm->trainer_data[i].trsex = (i!=0) ? PM_MALE : PM_FEMALE; 
    // �g���[�i�[��
    {
      //�I�[�R�[�h��ǉ����Ă���STRBUF�ɕϊ�
      STRCODE debugname[32] = L"�Ƃ���";
      
      debugname[3] = GFL_STR_GetEOMCode();

      prm->trainer_data[i].str_trname = GFL_STR_CreateBuffer( sizeof(STRCODE)*10, HEAPID_COMM_BTL_DEMO );
      GFL_STR_SetStringCode( prm->trainer_data[i].str_trname, debugname );
    }
#endif
    
    // �f�o�b�O�|�P�p�[�e�B�[
    {
      POKEPARTY *party;
      int poke_cnt;
      int p;

      party = PokeParty_AllocPartyWork( HEAPID_COMM_BTL_DEMO );

      if( type_is_normal(prm->type) )
      {
        Debug_PokeParty_MakeParty( party );
      }
      else
      {
        static const int pokemax=3;
        PokeParty_Init(party, pokemax);
        for (p = 0; p < pokemax; p++) 
        {
          POKEMON_PARAM* pp = GFL_HEAP_AllocMemoryLo( HEAPID_COMM_BTL_DEMO , POKETOOL_GetWorkSize() );
          PP_Clear(pp);
          PP_Setup( pp, 392+p, 99, 0 );
          PokeParty_Add(party, pp);
          GFL_HEAP_FreeMemory(pp);
        }
      }
      
      prm->trainer_data[i].party = party;

      poke_cnt = PokeParty_GetPokeCount( prm->trainer_data[i].party );

      if( prm->type == COMM_BTL_DEMO_TYPE_NORMAL_END || prm->type == COMM_BTL_DEMO_TYPE_MULTI_END )
      {
        // �ΐ��̃|�P�����̏�Ԉُ�
        for( p=0; p<poke_cnt; p++ )
        {
          POKEMON_PARAM* pp = PokeParty_GetMemberPointer( party, p );
          switch( GFUser_GetPublicRand(3) )
          {
          case 0: PP_SetSick( pp, POKESICK_DOKU ); break; // ��
          case 1: PP_Put(pp, ID_PARA_hp, 0 ); break; // �m��
          }
          HOSAKA_Printf("poke [%d] condition=%d \n",p, PP_Get( pp, ID_PARA_condition, NULL ) );
        }
      }

      HOSAKA_Printf("[%d] server_version=%d poke_cnt=%d \n",i, 
          prm->trainer_data[i].server_version,
          poke_cnt );
    }
  }
}
// ���[�N�J��
static void debug_param_del( COMM_BTL_DEMO_PARAM* prm )
{
  int i;

  for( i=0; i<COMM_BTL_DEMO_TRDATA_MAX; i++ )
  {
    GFL_HEAP_FreeMemory( (POKEPARTY*)prm->trainer_data[i].party );
  }
}
#endif

//=============================================================================
/**
 *                PROC
 */
//=============================================================================
//-----------------------------------------------------------------------------
/**
 *  @brief  PROC ����������
 *
 *  @param  GFL_PROC *proc�v���Z�X�V�X�e��
 *  @param  *seq          �V�[�P���X
 *  @param  *pwk          COMM_BTL_DEMO_PARAM
 *  @param  *mywk         PROC���[�N
 *
 *  @retval �I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT CommBtlDemoProc_Init( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{
  COMM_BTL_DEMO_MAIN_WORK *wk;
  COMM_BTL_DEMO_PARAM * prm;

  prm = pwk;

#ifdef DEBUG_LOOP
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_START )
  {
    g_debug_loop = TRUE;
  }
  else
  {
    g_debug_loop = FALSE;
  }
#endif
  
  // �t�F�[�h�҂�
  if( (*seq) == 1 && GFL_FADE_CheckFade() == TRUE )
  {
    return GFL_PROC_RES_FINISH;
  }

  GF_ASSERT( prm );
  GF_ASSERT( (*seq) == 0 );

  //�I�[�o�[���C�ǂݍ���
  GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common));

  //�q�[�v�쐬
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_COMM_BTL_DEMO, COMM_BTL_DEMO_HEAP_SIZE );
  wk = GFL_PROC_AllocWork( proc, sizeof(COMM_BTL_DEMO_MAIN_WORK), HEAPID_COMM_BTL_DEMO );
  GFL_STD_MemClear( wk, sizeof(COMM_BTL_DEMO_MAIN_WORK) );
  
#ifdef DEBUG_SET_PARAM
  debug_param( prm );
#endif
  
  GF_ASSERT( prm->type > COMM_BTL_DEMO_TYPE_NULL &&
             prm->type < COMM_BTL_DEMO_TYPE_MAX );

  // ������
  wk->heapID = HEAPID_COMM_BTL_DEMO;
  wk->pwk = prm;
  wk->type = wk->pwk->type;
  wk->result = wk->pwk->result;
  
  //�`��ݒ菉����
  wk->graphic = COMM_BTL_DEMO_GRAPHIC_Init( GX_DISP_SELECT_SUB_MAIN, wk->heapID );

  //�t�H���g�쐬
  wk->font      = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
                        GFL_FONT_LOADTYPE_FILE, FALSE, wk->heapID );

  //���b�Z�[�W
  wk->msg = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
      NARC_message_mictest_dat, wk->heapID );

  //PRINT_QUE�쐬
  wk->print_que   = PRINTSYS_QUE_Create( wk->heapID );
  
  // �V�[���R���g���[���쐬
  wk->cntScene = UI_SCENE_CNT_Create( 
      wk->heapID, c_scene_func_tbl, CBD_SCENE_ID_MAX, 
      calc_first_scene(pwk), wk );

  //BG���\�[�X�ǂݍ���
  CommBtlDemo_BG_LoadResource( &wk->wk_bg, wk->heapID );

  // G3D�֘A������
  G3D_Init( &wk->wk_g3d, wk->graphic, wk->heapID );

  // OBJ������
  OBJ_Init( &wk->wk_obj, wk->graphic, wk->heapID );

  // ���C����ʂ���ɕ\��
  GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);

  // �t�F�[�h�C�� ���N�G�X�g
  GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 1 );

  (*seq) = 1; // �t�F�[�hSEQ��
    
  return GFL_PROC_RES_CONTINUE;

}

//-----------------------------------------------------------------------------
/**
 *  @brief  PROC �I������
 *
 *  @param  GFL_PROC *procp���Z�X�V�X�e��
 *  @param  *seq          �V�[�P���X
 *  @param  *pwk          COMM_BTL_DEMO_PARAM
 *  @param  *mywk         PROC���[�N
 *
 *  @retval �I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT CommBtlDemoProc_Exit( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{ 
  COMM_BTL_DEMO_MAIN_WORK* wk = mywk;

#ifdef DEBUG_SET_PARAM
  debug_param_del( pwk );
#endif
  
  // �V�[���R���g�[���폜
  UI_SCENE_CNT_Delete( wk->cntScene );

  //���b�Z�[�W�j��
  GFL_MSG_Delete( wk->msg );

  //PRINT_QUE
  PRINTSYS_QUE_Delete( wk->print_que );

  //FONT
  GFL_FONT_Delete( wk->font );
  
  // OBJ�J��
  OBJ_Exit( &wk->wk_obj );

  // 3D�V�X�e���J��
  G3D_End( &wk->wk_g3d );

  //�`��ݒ�j��
  COMM_BTL_DEMO_GRAPHIC_Exit( wk->graphic );

  //PROC�p���������
  GFL_PROC_FreeWork( proc );
  GFL_HEAP_DeleteHeap( wk->heapID );

  //�I�[�o�[���C�j��
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common));

  return GFL_PROC_RES_FINISH;
}
//-----------------------------------------------------------------------------
/**
 *  @brief  PROC �又��
 *
 *  @param  GFL_PROC *proc�v���Z�X�V�X�e��
 *  @param  *seq          �V�[�P���X
 *  @param  *pwk          COMM_BTL_DEMO_PARAM
 *  @param  *mywk         PROC���[�N
 *
 *  @retval �I���R�[�h
 */
//-----------------------------------------------------------------------------
#include "ui/debug_code/g3d/camera_test.c"
static GFL_PROC_RESULT CommBtlDemoProc_Main( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{ 
  COMM_BTL_DEMO_MAIN_WORK* wk = mywk;
        
#if 0
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
  {
    HOSAKA_Printf("aaa\n");
    PMSND_PlaySE( SE_WIN_IN );
  }
#endif

#ifdef PM_DEBUG
#if 0
  {
    // �J�����e�X�g
    GFL_G3D_CAMERA* p_camera = COMM_BTL_DEMO_GRAPHIC_GetCamera( wk->graphic );
    debug_camera_test( p_camera );
  }
#endif
#endif

  G3D_Main( &wk->wk_g3d );
  
  // SCENE
  if( UI_SCENE_CNT_Main( wk->cntScene ) )
  {
    return GFL_PROC_RES_FINISH;
  }

  //PRINT_QUE
  PRINTSYS_QUE_Main( wk->print_que );

  //2D�`��
  COMM_BTL_DEMO_GRAPHIC_2D_Draw( wk->graphic );

  return GFL_PROC_RES_CONTINUE;
}

//=============================================================================
/**
 *                static�֐�
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *  @brief  BG�Ǘ����W���[�� ���\�[�X�ǂݍ���
 *
 *  @param  COMM_BTL_DEMO_BG_WORK* wk BG�Ǘ����[�N
 *  @param  heapID  �q�[�vID 
 *
 *  @retval none
 */
//-----------------------------------------------------------------------------
static void CommBtlDemo_BG_LoadResource( COMM_BTL_DEMO_BG_WORK* wk, HEAPID heapID )
{
  ARCHANDLE *handle;

  handle  = GFL_ARC_OpenDataHandle( ARCID_COMM_BTL_DEMO_GRA, heapID );

  // �㉺��ʂa�f�p���b�g�]��
  GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_comm_btl_demo_bg_base_u_NCLR, PALTYPE_SUB_BG, PLTID_BG_BACK_S, 0x0, heapID );
  GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_comm_btl_demo_bg_base_d_NCLR, PALTYPE_MAIN_BG, PLTID_BG_BACK_M, 0x0, heapID );
  
#if 0 
  // ��b�t�H���g�p���b�g�]��
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT,
      NARC_font_default_nclr,
      PALTYPE_MAIN_BG,
      0x20*PLTID_BG_TEXT_M, 0x20, heapID );
#endif
  
  //  ----- ����� -----
  GFL_ARCHDL_UTIL_TransVramBgCharacter( handle, NARC_comm_btl_demo_bg_base_d_NCGR,
            BG_FRAME_BACK_M, 0, 0, 0, heapID );
  GFL_ARCHDL_UTIL_TransVramScreen(  handle, NARC_comm_btl_demo_bg_base_d_NSCR,
            BG_FRAME_BACK_M, 0, 0, 0, heapID ); 

  //  ----- ���� -----
  GFL_ARCHDL_UTIL_TransVramBgCharacter( handle, NARC_comm_btl_demo_bg_base_u_NCGR,
            BG_FRAME_BACK_S, 0, 0, 0, heapID );
  GFL_ARCHDL_UTIL_TransVramScreen(  handle, NARC_comm_btl_demo_bg_base_u_NSCR,
            BG_FRAME_BACK_S, 0, 0, 0, heapID );   

  GFL_ARC_CloseDataHandle( handle );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ���ʊJ������
 *
 *  @param  UI_SCENE_CNT_PTR cnt
 *  @param  wk 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void _SceneEndDemoEnd( UI_SCENE_CNT_PTR cnt, COMM_BTL_DEMO_MAIN_WORK* wk )
{
  // �g���[�i�[�J��
  TRAINER_UNIT_CNT_Exit( wk );

  // ���s���j�b�g�J��
  RESULT_UNIT_Exit( &wk->result_unit );
  
  // �I��
  UI_SCENE_CNT_SetNextScene( cnt, UI_SCENE_ID_END );

#ifdef DEBUG_LOOP
  if( g_debug_loop )
  {
    if( (GFL_UI_KEY_GetCont() & PAD_BUTTON_START) == FALSE ) // �X�^�[�g�����Ȃ���ŏI��
    {
      wk->result = GFUser_GetPublicRand(3); // ���s��ύX
      UI_SCENE_CNT_SetNextScene( cnt, CBD_SCENE_ID_END_DEMO );
    }
  }
#endif

}

//-----------------------------------------------------------------------------
/**
 *  @brief  �o�g���O�f�� ����������
 *
 *  @param  UI_SCENE_CNT_PTR cnt
 *  @param  work 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneStartDemo_Init( UI_SCENE_CNT_PTR cnt, void* work )
{
  COMM_BTL_DEMO_MAIN_WORK* wk = work;
  
  // �g���[�i�[���j�b�g������
  TRAINER_UNIT_CNT_Init( wk );

#ifdef DEBUG_LOOP
  if( g_debug_loop )
  {
    GFL_BG_ClearScreen( BG_FRAME_TEXT_M ); // �e�L�X�g�ʂ��������Ă���
      // �t�F�[�h�A�E�g ���N�G�X�g
    GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_WHITEOUT, 16, 0, -8 );
  }
#endif

  return TRUE;
}

// �g���[�i�[�̐��ʂɂ���ă}�e���A���̃p���b�g�̊֘A�t����ύX
static void _start_demo_init_rebind_normal( COMM_BTL_DEMO_MAIN_WORK* wk )
{
  int i;
  COMM_BTL_DEMO_G3D_WORK* g3d = &wk->wk_g3d;
  GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( g3d->g3d_util, g3d->anm_unit_idx );

  for( i=0; i<2; i++ )
  {
    int trsex;
    int pal;

    trsex = wk->trainer_unit[i].trsex;

    if( trsex == PM_MALE )
    {
      pal = 1;
    }
    else
    {
      pal = 2;
    }

    G3D_OBJ_PaletteReBind( obj, 2-i, pal );
  }
}

static void _start_demo_init_rebind_multi( COMM_BTL_DEMO_MAIN_WORK* wk )
{
  int i;
  COMM_BTL_DEMO_G3D_WORK* g3d = &wk->wk_g3d;
  GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( g3d->g3d_util, g3d->anm_unit_idx );

  for( i=0; i<4; i++ )
  {
    int trsex;
    int pal;

    trsex = wk->trainer_unit[i].trsex;

    if( trsex == PM_MALE )
    {
      pal = 1;
    }
    else
    {
      pal = 3;
    }

    // @TODO �}�e���A��ID
    G3D_OBJ_PaletteReBind( obj, 1+i, pal );
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �o�g���O�f�� �又��
 *
 *  @param  UI_SCENE_CNT_PTR cnt
 *  @param  work 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneStartDemo_Main( UI_SCENE_CNT_PTR cnt, void* work )
{ 
  COMM_BTL_DEMO_MAIN_WORK* wk = work;
  u8 seq = UI_SCENE_CNT_GetSubSeq( cnt );

  TRAINER_UNIT_CNT_Main( wk );

  switch( seq )
  {
  case 0:
    //�u�����X�^�[�{�[���v�pPTC���[�h
    G3D_PTC_Setup( &wk->wk_g3d, NARC_comm_btl_demo_vs_demo04_spa );

    // �J�n���\�[�X�I��
    if( type_is_normal(wk->type) )
    {
      G3D_AnimeSet( &wk->wk_g3d, DEMO_ID_01_A );
      _start_demo_init_rebind_normal(wk);
    }
    else
    {
      G3D_AnimeSet( &wk->wk_g3d, DEMO_ID_02_A );
      _start_demo_init_rebind_multi(wk);
    }
    
    // �{�[���A�j���J�n
    TRAINER_UNIT_CNT_BallSetStart( wk );

    UI_SCENE_CNT_IncSubSeq( cnt );
    break;
  case 1:
    if( G3D_AnimeMain( &wk->wk_g3d ) == FALSE )
    {
      // 3D�J�n�A�j��
      G3D_AnimeSet( &wk->wk_g3d, DEMO_ID_START );
      
      //�uVS�v�pPTC���[�h
      G3D_PTC_Setup( &wk->wk_g3d, NARC_comm_btl_demo_vs_demo01_spa );
      
      wk->timer = 0;

      UI_SCENE_CNT_IncSubSeq( cnt );
    }
    break;
  case 2:
    // �A�j���Đ�
    G3D_AnimeMain( &wk->wk_g3d );

    if( wk->timer == START_DEMO_VS_OPEN_SYNC )
    {
      PMSND_PlaySE( SE_VS_IN );
      // �uVS�v�o���p�[�e�B�N��
      G3D_PTC_CreateEmitter( &wk->wk_g3d, 0, &(VecFx32){0,0,-100} );
      G3D_PTC_CreateEmitter( &wk->wk_g3d, 1, &(VecFx32){0,0,-100} );
      G3D_PTC_CreateEmitter( &wk->wk_g3d, 2, &(VecFx32){0,0,-100} );
    }
    else if( wk->timer == START_DEMO_VS_OPEN_WAIT_SYNC )
    {
      // �uVS�v�z���C�g�A�E�g�p�[�e�B�N���\��
      G3D_PTC_CreateEmitter( &wk->wk_g3d, 3, &(VecFx32){0,0,-100} );
    }
    else if( wk->timer == START_DEMO_FADEOUT_SYNC )
    {
      // �t�F�[�h�A�E�g ���N�G�X�g
      GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_WHITEOUT, 0, 16, -3 );
    }
    else if( wk->timer == START_DEMO_FINISH_SYNC )
    {
      // �I��
      return TRUE;
    }
    wk->timer++;
    break;
    
  default : GF_ASSERT(0);
  }

  return FALSE;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �o�g���O�f�� �I������
 *
 *  @param  UI_SCENE_CNT_PTR cnt
 *  @param  work 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneStartDemo_End( UI_SCENE_CNT_PTR cnt, void* work )
{ 
  COMM_BTL_DEMO_MAIN_WORK* wk = work;
      
  // �g���[�i�[�J��
  TRAINER_UNIT_CNT_Exit( wk );
  
  // �I��
  UI_SCENE_CNT_SetNextScene( cnt, UI_SCENE_ID_END );

#ifdef DEBUG_LOOP
  if( g_debug_loop )
  {
    // �X�^�[�g�������ςŏI��
    if( (GFL_UI_KEY_GetCont() & PAD_BUTTON_START) == FALSE )
    {
      UI_SCENE_CNT_SetNextScene( cnt, CBD_SCENE_ID_START_DEMO );
    }
  }
#endif

  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �o�g����f�� ����������
 *
 *  @param  UI_SCENE_CNT_PTR cnt
 *  @param  *work 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneEndDemo_Init( UI_SCENE_CNT_PTR cnt, void *work )
{
  COMM_BTL_DEMO_MAIN_WORK* wk = work;
  
  // �g���[�i�[���j�b�g������
  TRAINER_UNIT_CNT_Init( wk );

  // ���s���j�b�g����
  {
    u16 me,you;

    if( type_is_normal(wk->type) )
    {
      me  = PokeParty_GetPokeCountBattleEnable( wk->pwk->trainer_data[ COMM_BTL_DEMO_TRDATA_A ].party );
      you = PokeParty_GetPokeCountBattleEnable( wk->pwk->trainer_data[ COMM_BTL_DEMO_TRDATA_B ].party );
    }
    else
    {
      me  = PokeParty_GetPokeCountBattleEnable( wk->pwk->trainer_data[ COMM_BTL_DEMO_TRDATA_A ].party );
      me += PokeParty_GetPokeCountBattleEnable( wk->pwk->trainer_data[ COMM_BTL_DEMO_TRDATA_B ].party );
      you  = PokeParty_GetPokeCountBattleEnable( wk->pwk->trainer_data[ COMM_BTL_DEMO_TRDATA_C ].party );
      you += PokeParty_GetPokeCountBattleEnable( wk->pwk->trainer_data[ COMM_BTL_DEMO_TRDATA_D ].party );
    }

    RESULT_UNIT_Init( &wk->result_unit, &wk->wk_obj, &wk->wk_g3d, me, you, wk->type );
  }

#ifdef DEBUG_LOOP
  if( g_debug_loop )
  {
      // �t�F�[�h�A�E�g ���N�G�X�g
    GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_WHITEOUT, 16, 0, -8 );
  }
#endif

  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �o�g����f�� �又��
 *
 *  @param  UI_SCENE_CNT_PTR cnt
 *  @param  work 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneEndDemo_Main( UI_SCENE_CNT_PTR cnt, void* work )
{
  COMM_BTL_DEMO_MAIN_WORK* wk = work;
  u8 seq = UI_SCENE_CNT_GetSubSeq( cnt );

  TRAINER_UNIT_CNT_Main( wk );
  RESULT_UNIT_Main( &wk->result_unit );

  switch( seq )
  {
  case 0:
    // �J�n���\�[�X�I��
    if( type_is_normal(wk->type) )
    {
      G3D_AnimeSet( &wk->wk_g3d, DEMO_ID_01_B );
    }
    else
    {
      G3D_AnimeSet( &wk->wk_g3d, DEMO_ID_02_B );
    }
    //�u�����X�^�[�{�[���v�pPTC���[�h
    G3D_PTC_Setup( &wk->wk_g3d, NARC_comm_btl_demo_vs_demo04_spa );
    
    // �{�[���A�j���J�n
    TRAINER_UNIT_CNT_BallSetStart( wk );

    UI_SCENE_CNT_IncSubSeq( cnt );
    break;
  case 1:
    if( G3D_AnimeMain( &wk->wk_g3d ) == FALSE )
    {
      // ���������͕ʃV�[�P���X��
      if( wk->result == COMM_BTL_DEMO_RESULT_DRAW )
      {
        return TRUE;
      }
    
      wk->timer = 0;
      
      //�uWIN�v�pPTC���[�h
      G3D_PTC_Setup( &wk->wk_g3d, NARC_comm_btl_demo_vs_demo03_spa );

      UI_SCENE_CNT_IncSubSeq( cnt );
    }
    break;
  case 2:
    {
      // WIN�\��
      if( wk->timer == WIN_OPEN_SYNC )
      {
        int pos;
        if( wk->result == COMM_BTL_DEMO_RESULT_WIN )
        {
          pos = RESULT_POS_ME;
        }
        else
        {
          pos = RESULT_POS_YOU;
        }

        PMSND_PlaySE( SE_WIN_IN );
        RESULT_UNIT_SetOpen( &wk->result_unit, pos, COMM_BTL_DEMO_RESULT_WIN );
      }
      // LOSE�\��
      else if( wk->timer == LOSE_OPEN_SYNC )
      {
        int pos;
        if( wk->result == COMM_BTL_DEMO_RESULT_LOSE )
        {
          pos = RESULT_POS_ME;
        }
        else
        {
          pos = RESULT_POS_YOU;
        }

        RESULT_UNIT_SetOpen( &wk->result_unit, pos, COMM_BTL_DEMO_RESULT_LOSE );
      }
      else if( wk->timer == END_DEMO_FADEOUT_SYNC )
      {
        // �t�F�[�h�A�E�g ���N�G�X�g
        GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, -3 );
      }
      else if( wk->timer == END_DEMO_END_SYNC )
      {
        return TRUE;
      }

      // �p�[�e�B�N�������I�ɕ\��
      if( wk->timer % RESULT_PTC_SYNC == 0 ) 
      {
        int pos;
        s16 px, py;

        // ����������POS����
        if( wk->result == COMM_BTL_DEMO_RESULT_LOSE )
        {
          px = 0xffffd800;
          py = 0x2000;
        }
        else 
        {
          px = 0x2800;
          py = 0xffffe800;
        }

        //�uWIN�v�p�[�e�B�N���\��
        G3D_PTC_CreateEmitter( &wk->wk_g3d, 0, &(VecFx32){px,py,-100} );
        G3D_PTC_CreateEmitter( &wk->wk_g3d, 1, &(VecFx32){px,py,-100} );
        G3D_PTC_CreateEmitter( &wk->wk_g3d, 2, &(VecFx32){px,py,-100} );
        G3D_PTC_CreateEmitter( &wk->wk_g3d, 3, &(VecFx32){px,py,-100} );
        G3D_PTC_CreateEmitter( &wk->wk_g3d, 4, &(VecFx32){px,py,-100} );
      }
    
      wk->timer++;
    }
    break;

  default : GF_ASSERT(0);
  }
  
  return FALSE;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �o�g����f�� �又��
 *
 *  @param  UI_SCENE_CNT_PTR cnt
 *  @param  work 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneEndDemo_End( UI_SCENE_CNT_PTR cnt, void* work )
{
  COMM_BTL_DEMO_MAIN_WORK* wk = work;
      
  // ���������͊J�������ɕʃV�[�P���X��
  if( wk->result == COMM_BTL_DEMO_RESULT_DRAW )
  {
    UI_SCENE_CNT_SetNextScene( cnt, CBD_SCENE_ID_END_DEMO_DRAW );
    return TRUE;
  }
  
  _SceneEndDemoEnd( cnt, wk );

  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �o�g����f�� �������� �又��
 *
 *  @param  UI_SCENE_CNT_PTR cnt
 *  @param  work 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneEndDemoDraw_Main( UI_SCENE_CNT_PTR cnt, void* work )
{
  COMM_BTL_DEMO_MAIN_WORK* wk = work;
  u8 seq = UI_SCENE_CNT_GetSubSeq( cnt );

  TRAINER_UNIT_CNT_Main( wk );
  RESULT_UNIT_Main( &wk->result_unit );
    
  switch( seq )
  {
  case 0:
    G3D_AnimeSet( &wk->wk_g3d, DEMO_ID_DRAW );
    wk->timer=0;
    UI_SCENE_CNT_IncSubSeq( cnt );
    break;
  case 1:
    
    // DRAW�����\��
    if( wk->timer++ == DRAW_OPEN_SYNC )
    {
      RESULT_UNIT_SetOpen( &wk->result_unit, RESULT_POS_DRAW, COMM_BTL_DEMO_RESULT_DRAW );
    }

    if( G3D_AnimeMain( &wk->wk_g3d ) == FALSE )
    {
      // �t�F�[�h�A�E�g ���N�G�X�g
      GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, -3 );
      return TRUE;
    }
    break;
  case 2:
    break;
  }

  return FALSE;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �o�g����f�� �������� �又��
 *
 *  @param  UI_SCENE_CNT_PTR cnt
 *  @param  work 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneEndDemoDraw_End( UI_SCENE_CNT_PTR cnt, void* work )
{
  COMM_BTL_DEMO_MAIN_WORK* wk = work;

  // �t�F�[�h�҂�
  if( GFL_FADE_CheckFade() == TRUE )
  {
    return FALSE;
  }

  _SceneEndDemoEnd( cnt, wk );

  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �ŏ��̃V�[���𔻒�
 *
 *  @param  COMM_BTL_DEMO_MAIN_WORK* wk 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static CBD_SCENE_ID calc_first_scene( COMM_BTL_DEMO_PARAM* pwk )
{
  switch( pwk->type )
  {
  case COMM_BTL_DEMO_TYPE_NORMAL_START:
  case COMM_BTL_DEMO_TYPE_MULTI_START:
    return CBD_SCENE_ID_START_DEMO;

  case COMM_BTL_DEMO_TYPE_NORMAL_END:
  case COMM_BTL_DEMO_TYPE_MULTI_END:
    return CBD_SCENE_ID_END_DEMO;

  default : GF_ASSERT_MSG( 0 , "demo type=%d ", pwk->type);
  }

  return CBD_SCENE_ID_START_DEMO;
}

//-----------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  u8 type 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
BOOL type_is_normal( u8 type )
{
  switch( type )
  {
  case COMM_BTL_DEMO_TYPE_NORMAL_START:
    return TRUE;

  case COMM_BTL_DEMO_TYPE_NORMAL_END:
    return TRUE;

  case COMM_BTL_DEMO_TYPE_MULTI_START:
    return FALSE;

  case COMM_BTL_DEMO_TYPE_MULTI_END:
    return FALSE;

  default : GF_ASSERT(0);
  }

  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �^�C�v���X�^�[�g���G���h���擾
 *
 *  @param  u8 type 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
BOOL type_is_start( u8 type )
{
  switch( type )
  {
  case COMM_BTL_DEMO_TYPE_NORMAL_START:
    return TRUE;

  case COMM_BTL_DEMO_TYPE_NORMAL_END:
    return FALSE;

  case COMM_BTL_DEMO_TYPE_MULTI_START:
    return TRUE;

  case COMM_BTL_DEMO_TYPE_MULTI_END:
    return FALSE;

  default : GF_ASSERT(0);
  }
  
  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �|�P�p������{�[��OBJ�̃A�j��ID���擾
 *
 *  @param  POKEMON_PARAM* pp 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static u32 PokeParaToBallAnim( POKEMON_PARAM* pp )
{
  if( pp == NULL )
  {
    return OBJ_ANM_ID_BALL_NULL; // �{�[���Ȃ�
  }

  if( PP_Get( pp, ID_PARA_hp, NULL ) == 0 )
  {
    return OBJ_ANM_ID_BALL_DOWN; // �m��
  }
  else
  {
    // ��Ԉُ�
    u32 prm = PP_Get( pp, ID_PARA_condition, NULL );

    if( prm == POKESICK_NULL )
    {
      return OBJ_ANM_ID_BALL_NORMAL; // �ʏ�
    }
    else
    {
      return OBJ_ANM_ID_BALL_SICK; // ��Ԉُ�
    }
  }

  GF_ASSERT(0);

  return OBJ_ANM_ID_BALL_NORMAL; // ��΂����ɂ͗��Ȃ�
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �{�[�����j�b�g������
 *
 *  @param  BALL_UNIT* unit
 *  @param  POKEPARTY* party
 *  @param  type
 *  @param  posid
 *  @param  obj 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void BALL_UNIT_Init( BALL_UNIT* unit, const POKEPARTY* party, u8 type, u8 posid, COMM_BTL_DEMO_OBJ_WORK* obj, COMM_BTL_DEMO_G3D_WORK* g3d )
{
  int i;
  int clwk_id;

  GF_ASSERT(unit);
  GF_ASSERT(party);
  GF_ASSERT(obj);

  unit->type = type;
  unit->posid = posid;
  unit->party = party;
  unit->num = PokeParty_GetPokeCount( party );
  unit->max = ( type_is_normal(type) ? 6 : 3 );
  unit->timer = 0;
  unit->g3d = g3d;

  HOSAKA_Printf("max=%d pokenum=%d\n", unit->max, unit->num);

  for( i=0; i<unit->max; i++ )
  {
    s16 px;
    s16 py;
    int anm;
  
    if( type_is_normal(type) )
    {
      // �m�[�}��
      px = NORMAL_BALL_PX_BASE + (i) * BALL_PX_OFS;
      py = (posid==0) ? NORMAL_POSID0_BALL_PY : NORMAL_POSID1_BALL_PY;

      if( posid == 1 ){ px *= -1; }
    }
    else
    {
      // �}���`
      switch( posid )
      { 
      case 0:
        px = MULTI_POSID0_BALL_PX_BASE + (i) * BALL_PX_OFS * -1;
        py = MULTI_POSID0_BALL_PY;
        break;
      case 1:
        px = MULTI_POSID1_BALL_PX_BASE + (i) * BALL_PX_OFS * -1;
        py = MULTI_POSID1_BALL_PY;
        break;
      case 2:
        px = MULTI_POSID2_BALL_PX_BASE + (i) * BALL_PX_OFS;
        py = MULTI_POSID2_BALL_PY;
        break;
      case 3:
        px = MULTI_POSID3_BALL_PX_BASE + (i) * BALL_PX_OFS;
        py = MULTI_POSID3_BALL_PY;
        break;
      }
    }

    if( type_is_start(type) )
    {
      clwk_id = i;
    }
    else
    // �I���f��
    {
      // ���Ԕ��]
      clwk_id = (unit->max-1) - i;
    }

    GF_ASSERT( clwk_id < unit->max );
    
    {
      // ��Ԏ擾
      POKEMON_PARAM* pp = NULL;
      
      // �|�P���������݂���Ԃ�POKEMON_PARAM���擾
      if( clwk_id < unit->num )
      {
        pp  = PokeParty_GetMemberPointer( party, clwk_id );
      }

      anm = PokeParaToBallAnim( pp );

      // �I���f���͋�ȊO�ʏ��Ԃŏ㏑��
      if( type_is_start(type) == FALSE )
      {
        if( anm != OBJ_ANM_ID_BALL_NULL )
        {
          anm = OBJ_ANM_ID_BALL_NORMAL;
        }
      }
    }
      
    // CLWK����
    unit->clwk[clwk_id] = OBJ_CreateCLWK( obj, px, py, anm );
    
    // �J�n�f�� ��\���ɂ��Ă���
    if( type_is_start(type) )
    {
      GFL_CLACT_WK_SetDrawEnable( unit->clwk[clwk_id] , FALSE );
    }
    // �I���f��
    else
    {
      // �I���f���͍ŏ�����\��
      GFL_CLACT_WK_SetDrawEnable( unit->clwk[clwk_id] , TRUE );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �{�[�����j�b�g�J��
 *
 *  @param  BALL_UNIT* unit 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void BALL_UNIT_Exit( BALL_UNIT* unit )
{
  int i;

  GF_ASSERT(unit);

  for( i=0; i<unit->max; i++ )
  {
    GFL_CLACT_WK_Remove( unit->clwk[i] );
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  BALL_UNIT* unit 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void _ball_open( BALL_UNIT* unit, int start_sync )
{
  fx32 fx;
  fx32 fy;
  int id = unit->timer - start_sync;

  GF_ASSERT( id <= unit->max && id >= 0 );

  HOSAKA_Printf("unit->timer=%d ",unit->timer);
  HOSAKA_Printf("ball open id=%d\n", id);

  if( id < unit->max )
  {
    if( id == 0 ){ PMSND_PlaySE( SE_BALL_IN ); } // �{�[��

    // �p�[�e�B�N���\��
    if( type_is_normal(unit->type) )
    {
      if( unit->posid == 0 )
      {
        fx = NORMAL_POSID0_BALL_PTC_BASE_X + id * BALL_PTC_DIFF_X;
        fy = NORMAL_POSID0_BALL_PTC_BASE_Y;
      }
      else if( unit->posid == 1 )
      {
        fx = NORMAL_POSID1_BALL_PTC_BASE_X - id * BALL_PTC_DIFF_X;
        fy = NORMAL_POSID1_BALL_PTC_BASE_Y;
      }
    }
    else
    {
      if( unit->posid == 0 )
      {
        fx = MULTI_POSID0_BALL_PTC_BASE_X - id * BALL_PTC_DIFF_X;
        fy = MULTI_POSID0_BALL_PTC_BASE_Y;
      }
      else if( unit->posid == 1 )
      {
        fx = MULTI_POSID1_BALL_PTC_BASE_X - id * BALL_PTC_DIFF_X;
        fy = MULTI_POSID1_BALL_PTC_BASE_Y;
      }
      else if( unit->posid == 2 )
      {
        fx = MULTI_POSID2_BALL_PTC_BASE_X + id * BALL_PTC_DIFF_X;
        fy = MULTI_POSID2_BALL_PTC_BASE_Y;
      }
      else if( unit->posid == 3 )
      {
        fx = MULTI_POSID3_BALL_PTC_BASE_X + id * BALL_PTC_DIFF_X;
        fy = MULTI_POSID3_BALL_PTC_BASE_Y;
      }
    }
        
    G3D_PTC_CreateEmitterAll( unit->g3d, &(VecFx32){fx,fy,-100} );

    if( type_is_start(unit->type) )
    {
      // �J��
      GFL_CLACT_WK_SetDrawEnable( unit->clwk[id] , TRUE );
    }
    else
    {
      // �o�g����̏�Ԃ𔽉f
      BALL_UNIT_SetPartyCondition( unit, unit->party, id );
    }
  }
  else
  {
    int i;

    // �v���C�I���e�B�������Ă���
    for( i=0; i<unit->max; i++ )
    {
      GFL_CLACT_WK_SetBgPri( unit->clwk[i], 1 );
    }

    unit->is_start = FALSE; // �I��
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �{�[���又��
 *
 *  @param  BALL_UNIT* unit 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void BALL_UNIT_Main( BALL_UNIT* unit )
{
  const int NORMAL_OPEN_SYNC = 12;

  const int MULTI_OPEN_SYNC = 15; // �}���`�̂Ƃ�
  const int MULTI_OPEN_SYNC2 = MULTI_OPEN_SYNC+4; // �}���`�̂Ƃ��̌㑱

  const int AFTER_NORMAL_OPEN_SYNC = 8;
  const int AFTER_MULTI_OPEN_SYNC = 8+3;
  const int AFTER_MULTI_OPEN_SYNC2 = AFTER_MULTI_OPEN_SYNC+4;

  int start_sync;

  // ���N�G�X�g���Ȃ���Ώ������Ȃ�
  if( unit->is_start == FALSE )
  {
    return;
  }

  // �J�n�^�C�~���O����
  switch( unit->type )
  {
  case COMM_BTL_DEMO_TYPE_NORMAL_START :
      start_sync = NORMAL_OPEN_SYNC;
      break;
  case COMM_BTL_DEMO_TYPE_MULTI_START :
      if( unit->posid==0 || unit->posid == 2 )
      {
        start_sync = MULTI_OPEN_SYNC;
      }
      else if( unit->posid==1 || unit->posid == 3 )
      {
        start_sync = MULTI_OPEN_SYNC2;
      }
      break;
  case COMM_BTL_DEMO_TYPE_NORMAL_END :
      start_sync = AFTER_NORMAL_OPEN_SYNC;
      break;
  case COMM_BTL_DEMO_TYPE_MULTI_END :
      if( unit->posid==0 || unit->posid == 2 )
      {
        start_sync = AFTER_MULTI_OPEN_SYNC;
      }
      else if( unit->posid==1 || unit->posid == 3 )
      {
        start_sync = AFTER_MULTI_OPEN_SYNC2;
      }
      break;
  default : GF_ASSERT(0);
  }
  
  if( unit->timer >= start_sync )
  {
    _ball_open( unit, start_sync );
  }

  unit->timer++;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �A�j���J�n
 *
 *  @param  BALL_UNIT* unit 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void BALL_UNIT_SetStart( BALL_UNIT* unit )
{
  unit->is_start = TRUE;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �|�P�����̏�Ԃ𔽉f
 *
 *  @param  BALL_UNIT* unit
 *  @param  id 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void BALL_UNIT_SetPartyCondition( BALL_UNIT* unit, const POKEPARTY* party, int id )
{
  int num;
  int anm;

  POKEMON_PARAM* pp = NULL;

  num = PokeParty_GetPokeCount( party );

  GF_ASSERT( id <= unit->max && id >= 0 );

  // �|�P���������݂���Ԃ�POKEMON_PARAM���擾
  if( id < num )
  {
    pp  = PokeParty_GetMemberPointer( unit->party, id );
  }

  anm = PokeParaToBallAnim( pp );

  GFL_CLACT_WK_SetAnmSeqDiff( unit->clwk[id], anm );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �g���[�i�[���pBMPWIN����
 *
 *  @param  u8 type
 *  @param  posid 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static GFL_BMPWIN* TRAINERNAME_WIN_Create( u8 type, u8 posid )
{
  GFL_BMPWIN* win;
  s16 px, py;

  if( type_is_normal(type) )
  {
    if( posid == 0 )
    {
      px = NORMAL_POSID0_TRNAME_CPX;
      py = NORMAL_POSID0_TRNAME_CPY;
    }
    else
    {
      px = NORMAL_POSID1_TRNAME_CPX;
      py = NORMAL_POSID1_TRNAME_CPY;
    }
  }
  else
  {
    switch( posid )
    {
    case 0:
      px = MULTI_POSID0_TRNAME_CPX;
      py = MULTI_POSID0_TRNAME_CPY;
      break;
    case 1:
      px = MULTI_POSID1_TRNAME_CPX;
      py = MULTI_POSID1_TRNAME_CPY;
      break;
    case 2:
      px = MULTI_POSID2_TRNAME_CPX;
      py = MULTI_POSID2_TRNAME_CPY;
      break;
    case 3:
      px = MULTI_POSID3_TRNAME_CPX;
      py = MULTI_POSID3_TRNAME_CPY;
      break;
    }
  }

  win = GFL_BMPWIN_Create( 
      BG_FRAME_TEXT_M,        // BG�t���[��
      px, py,                 // �\�����W 
      TRNAME_CSX, TRNAME_CSY, // �\���T�C�Y
      PLTID_BG_TEXT_M,        // �p���b�g
      GFL_BMP_CHRAREA_GET_B   // �L�����擾����
      );

  return win;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �g���[�i�[���j�b�g ������
 *
 *  @param  TRAINER_UNIT* unit
 *  @param  posid
 *  @param  COMM_BTL_DEMO_TRAINER_DATA* data
 *  @param  obj 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void TRAINER_UNIT_Init( TRAINER_UNIT* unit, u8 type, u8 posid, const COMM_BTL_DEMO_TRAINER_DATA* data, COMM_BTL_DEMO_OBJ_WORK* obj, COMM_BTL_DEMO_G3D_WORK* g3d, GFL_FONT* font )
{
  u8 num;

  GF_ASSERT( obj );

  // �����o������
  unit->data = data;
  unit->obj = obj;
  unit->g3d = g3d;
  unit->type = type;
  unit->posid = posid;
  unit->font = font;
  unit->timer = 0;

  // �{�[��������
  BALL_UNIT_Init( &unit->ball, data->party, type, posid, obj, g3d );

  // �g���[�i�[�� ����
  unit->str_trname = GFL_STR_CreateBuffer( STR_TRNAME_SIZE, HEAPID_COMM_BTL_DEMO );
  MyStatus_CopyNameString( unit->data->mystatus, unit->str_trname );
  
  // �g���[�i�[���ʎ擾
  unit->trsex = MyStatus_GetMySex( unit->data->mystatus );
  
    // �g���[�i�[��BMPWIN������
  unit->win_name = TRAINERNAME_WIN_Create( type, posid );

  // �I���f���̏�����
  if( type_is_start(unit->type) == FALSE )
  {
    // �ŏ�����\��
    TRAINER_UNIT_DrawTrainerName( unit, unit->font );
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �g���[�i�[���j�b�g 
 *
 *  @param  TRAINER_UNIT* unit 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void _trainer_unit_main_start( TRAINER_UNIT* unit )
{
  // �g���[�i�[���\��
  if( unit->timer == TRNAME_OPEN_SYNC ) 
  { 
    // �g���[�i�[���\��
    TRAINER_UNIT_DrawTrainerName( unit, unit->font );

#if 0
    // �p�[�e�B�N���\��
    {
      fx32 fx, fy;

      if( type_is_normal(unit->type) )
      {
        if( unit->posid == 0 )
        {
          fx = NORMAL_POSID0_TRNAME_PTC_PX;
          fy = NORMAL_POSID0_TRNAME_PTC_PY;
        }
        else
        {
          fx = NORMAL_POSID1_TRNAME_PTC_PX;
          fy = NORMAL_POSID1_TRNAME_PTC_PY;
        }
      }
      else
      {
        switch( unit->posid )
        { 
        case 0:
          fx = MULTI_POSID0_TRNAME_PTC_PX;
          fy = MULTI_POSID0_TRNAME_PTC_PY;
          break;
        case 1:
          fx = MULTI_POSID1_TRNAME_PTC_PX;
          fy = MULTI_POSID1_TRNAME_PTC_PY;
          break;
        case 2:
          fx = MULTI_POSID2_TRNAME_PTC_PX;
          fy = MULTI_POSID2_TRNAME_PTC_PY;
          break;
        case 3:
          fx = MULTI_POSID3_TRNAME_PTC_PX;
          fy = MULTI_POSID3_TRNAME_PTC_PY;
          break;
        };
      }

      // ���O�\��
      G3D_PTC_CreateEmitterAll( unit->g3d, &(VecFx32){fx,fy,-100} );
    }
#endif
  }  
}

//-----------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  TRAINER_UNIT* unit 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
#if 0
static void _trainer_unit_main_end( TRAINER_UNIT* unit )
{
  int sync;

  if( type_is_normal(unit->type) )
  {
    sync = NORMAL_POKENUM_OPEN_SYNC;
  }
  else
  {
    if( unit->posid == 0 || unit->posid == 2 )
    {
      sync = MULTI_POKENUM_OPEN_SYNC;
    }
    else
    {
      sync = MULTI_POKENUM_OPEN_SYNC2;
    }
  }

  if( unit->timer == sync )
  {
    GFL_CLACT_WK_SetDrawEnable( unit->clwk_pokenum , TRUE );
    GFL_CLACT_WK_SetAutoAnmFlag( unit->clwk_pokenum , TRUE );
  }
}
#endif

//-----------------------------------------------------------------------------
/**
 *  @brief  �g���[�i�[���j�b�g �㏈��
 *
 *  @param  TRAINER_UNIT* unit 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void TRAINER_UNIT_Exit( TRAINER_UNIT* unit )
{
  // �g���[�i�[�� �폜
  GFL_STR_DeleteBuffer( unit->str_trname );

  // �{�[���J��
  BALL_UNIT_Exit( &unit->ball );

  // �g���[�i�[��BMPWIN���
  GFL_BMPWIN_Delete( unit->win_name );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �g���[�i�[���j�b�g �又��
 *
 *  @param  TRAINER_UNIT* unit 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void TRAINER_UNIT_Main( TRAINER_UNIT* unit )
{
  GF_ASSERT( unit->g3d );

  // �{�[���又��
  BALL_UNIT_Main( &unit->ball );

  if( type_is_start(unit->type) )
  {
    // �J�n�f��
    _trainer_unit_main_start( unit );
  }
  else
  {
    // �I���f��
//  _trainer_unit_main_end( unit );
  }

//  HOSAKA_Printf("unit timer=%d \n", unit->timer);

  unit->timer++;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �g���[�i�[���j�b�g �g���[�i�[�������A�\��
 *
 *  @param  TRAINER_UNIT* unit 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void TRAINER_UNIT_DrawTrainerName( TRAINER_UNIT* unit, GFL_FONT *font )
{
  GF_ASSERT( unit );
  GF_ASSERT( unit->str_trname );

  if( unit->trsex == PM_MALE )
  {
    // �j����
    GFL_FONTSYS_SetColor( 0x6, 0x6, 0x6 );
  }
  else
  {
    // ������
    GFL_FONTSYS_SetColor( 0x4, 0x4, 0x4 );
  }

  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(unit->win_name) , 0 );

#if 0
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(unit->win_name), 0, 0, unit->str_trname, font );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(unit->win_name), 0, 1, unit->str_trname, font );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(unit->win_name), 0, 2, unit->str_trname, font );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(unit->win_name), 1, 0, unit->str_trname, font );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(unit->win_name), 1, 2, unit->str_trname, font );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(unit->win_name), 2, 0, unit->str_trname, font );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(unit->win_name), 2, 1, unit->str_trname, font );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(unit->win_name), 2, 2, unit->str_trname, font );
#endif
  
  if( unit->trsex == PM_MALE )
  {
    // �j����
    GFL_FONTSYS_SetColor( 0x7, 0x5, 0x5 );
  }
  else
  {
    // ������
    GFL_FONTSYS_SetColor( 0x7, 0x3, 0x3 );
  }
    
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(unit->win_name), 1, 1, unit->str_trname, font );
  GFL_BMPWIN_MakeTransWindow_VBlank( unit->win_name );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �g���[�i�[���j�b�g �������R���g���[���[
 *
 *  @param  COMM_BTL_DEMO_MAIN_WORK* wk 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void TRAINER_UNIT_CNT_Init( COMM_BTL_DEMO_MAIN_WORK* wk )
{
  int i;
  int max = (type_is_normal(wk->type) ? TRAINER_CNT_NORMAL : TRAINER_CNT_MULTI ); 

  for( i=0; i<max; i++ )
  {
    TRAINER_UNIT_Init( &wk->trainer_unit[i],
          wk->type, i,
          &wk->pwk->trainer_data[i], 
          &wk->wk_obj,
          &wk->wk_g3d,
          wk->font );
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �g���[�i�[���j�b�g �㏈���R���g���[���[
 *
 *  @param  COMM_BTL_DEMO_MAIN_WORK* wk 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void TRAINER_UNIT_CNT_Exit( COMM_BTL_DEMO_MAIN_WORK* wk )
{
  int i;
  int max = (type_is_normal(wk->type) ? TRAINER_CNT_NORMAL : TRAINER_CNT_MULTI ); 

  for( i=0; i<max; i++ )
  {
    TRAINER_UNIT_Exit( &wk->trainer_unit[i] );
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �g���[�i�[���j�b�g �又���R���g���[���[
 *
 *  @param  COMM_BTL_DEMO_MAIN_WORK* wk 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void TRAINER_UNIT_CNT_Main( COMM_BTL_DEMO_MAIN_WORK* wk )
{
  int i;
  int max = (type_is_normal(wk->type) ? TRAINER_CNT_NORMAL : TRAINER_CNT_MULTI ); 

#if DEBUG_ONLY_FOR_genya_hosaka
  //@TODO 
  // �p�[�e�B�N�����W����
  {
    static fx32 fx=0;
    static fx32 fy=0;
    static fx32 num = FX32_ONE;
    
    if( GFL_UI_KEY_GetTrg() & PAD_KEY_UP )  {  fy += num; }else
    if( GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN ){  fy -= num; }else
    if( GFL_UI_KEY_GetTrg() & PAD_KEY_LEFT ){  fx -= num; }else
    if( GFL_UI_KEY_GetTrg() & PAD_KEY_RIGHT ){ fx += num; }else
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X ){ num += 0x100; HOSAKA_Printf("num=0x%x\n",num); }else
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y ){ num -= 0x100; HOSAKA_Printf("num=0x%x\n",num); }else
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
    {
      if( wk->wk_g3d.ptc != NULL )
      {
        HOSAKA_Printf("{0x%x,0x%x}\n", fx, fy);
        G3D_PTC_CreateEmitterAll( &wk->wk_g3d, &(VecFx32){fx,fy,-100} );
      }
    }
  }
#endif

  for( i=0; i<max; i++ )
  {
    TRAINER_UNIT_Main( &wk->trainer_unit[i] );
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �g���[�i�[���j�b�g �{�[���J�n�R���g���[���[
 *
 *  @param  COMM_BTL_DEMO_MAIN_WORK* wk 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void TRAINER_UNIT_CNT_BallSetStart( COMM_BTL_DEMO_MAIN_WORK* wk )
{
  int i;
  int max = (type_is_normal(wk->type) ? TRAINER_CNT_NORMAL : TRAINER_CNT_MULTI ); 

  for( i=0; i<max; i++ )
  {
    BALL_UNIT_SetStart( &wk->trainer_unit[i].ball );
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ���s���j�b�g����
 *
 *  @param  RESULT_UNIT* unit
 *  @param  obj
 *  @param  g3d 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void RESULT_UNIT_Init( RESULT_UNIT* unit, COMM_BTL_DEMO_OBJ_WORK* obj, COMM_BTL_DEMO_G3D_WORK* g3d, u16 pokenum_me, u16 pokenum_you, u8 type )
{
  int i;

  // �����o������
  unit->obj = obj;
  unit->g3d = g3d;
  unit->type = type;
  unit->pokenum_me = pokenum_me;
  unit->pokenum_you = pokenum_you;

  unit->clwk_pokenum[0] = OBJ_CreatePokeNum( obj, 0, unit->pokenum_me );
  unit->clwk_pokenum[1] = OBJ_CreatePokeNum( obj, 1, unit->pokenum_you );

  //-----------------------------------------------------
  // ���s����CLWK����
  //-----------------------------------------------------

  //@TODO MN
  for( i=0; i<COMM_BTL_DEMO_RESULT_MAX; i++ )
  {
    s16 px, py;

    switch( i )
    {
    case RESULT_POS_ME : 
      px = GX_LCD_SIZE_X-50;
      py = 19*8;
      break;
    case RESULT_POS_YOU : 
      px = 50;
      py = 8*5;
      break;
    case RESULT_POS_DRAW : 
      px = GX_LCD_SIZE_X/2;
      py = GX_LCD_SIZE_Y/2;
      break;
    default : GF_ASSERT(0);
    }
  
    unit->clwk[i]= OBJ_CreateCLWK( obj, px, py, OBJ_ANM_ID_RSLT_WIN );
    GFL_CLACT_WK_SetDrawEnable( unit->clwk[i], FALSE );
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ���s���j�b�g �J��
 *
 *  @param  RESULT_UNIT* unit 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void RESULT_UNIT_Exit( RESULT_UNIT* unit )
{
  int i;

  GF_ASSERT( unit );
  GF_ASSERT( unit->clwk );

  for( i=0; i<COMM_BTL_DEMO_RESULT_MAX; i++ )
  {
    GFL_CLACT_WK_Remove( unit->clwk[i] );
  }

  for( i=0; i<2; i++ )
  {
    GFL_CLACT_WK_Remove( unit->clwk_pokenum[i] );
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ���U���g���j�b�g �又��
 *
 *  @param  RESULT_UNIT* unit 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void RESULT_UNIT_Main( RESULT_UNIT* unit )
{
  int sync;

  if( type_is_normal(unit->type) )
  {
    sync = NORMAL_POKENUM_OPEN_SYNC;
  }
  else
  {
    sync = MULTI_POKENUM_OPEN_SYNC;
  }

  if( unit->timer == sync )
  {
    GFL_CLACT_WK_SetDrawEnable( unit->clwk_pokenum[0] , TRUE );
    GFL_CLACT_WK_SetAutoAnmFlag( unit->clwk_pokenum[0] , TRUE );
    GFL_CLACT_WK_SetDrawEnable( unit->clwk_pokenum[1] , TRUE );
    GFL_CLACT_WK_SetAutoAnmFlag( unit->clwk_pokenum[1] , TRUE );
  }

  unit->timer++;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ���U���g�A�j���J�n
 *
 *  @param  RESULT_UNIT* unit
 *  @param  pos
 *  @param  result 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void RESULT_UNIT_SetOpen( RESULT_UNIT* unit, RESULT_POS pos, COMM_BTL_DEMO_RESULT result )
{
  int anm;
  
  GF_ASSERT( pos < RESULT_POS_MAX );

  switch( result )
  {
  case COMM_BTL_DEMO_RESULT_WIN :
    anm = OBJ_ANM_ID_RSLT_WIN;
    break;
  case COMM_BTL_DEMO_RESULT_LOSE :
    anm = OBJ_ANM_ID_RSLT_LOSE;
    break;
  case COMM_BTL_DEMO_RESULT_DRAW:
    anm = OBJ_ANM_ID_RSLT_DRAW;
    break;
  default : GF_ASSERT(0);
  }

  GFL_CLACT_WK_SetDrawEnable( unit->clwk[pos], TRUE );
  GFL_CLACT_WK_SetAutoAnmFlag( unit->clwk[pos], TRUE );
  GFL_CLACT_WK_SetAnmSeqDiff( unit->clwk[pos], anm );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  OBJ������
 *
 *  @param  COMM_BTL_DEMO_OBJ_WORK* wk 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void OBJ_Init( COMM_BTL_DEMO_OBJ_WORK* wk, COMM_BTL_DEMO_GRAPHIC_WORK* graphic, HEAPID heapID )
{
  UI_EASY_CLWK_RES_PARAM prm;
  GFL_CLUNIT* unit;

  
  GF_ASSERT( wk );
  GF_ASSERT( graphic );

  // �����o������
  wk->graphic = graphic;
  wk->heapID = heapID;

  // ���\�[�X�W�J
  prm.draw_type = CLSYS_DRAW_MAIN;
  prm.comp_flg  = UI_EASY_CLWK_RES_COMP_NONE;
  prm.arc_id    = ARCID_COMM_BTL_DEMO_GRA;
  prm.pltt_id   = NARC_comm_btl_demo_line_num_full_NCLR;
  prm.ncg_id    = NARC_comm_btl_demo_line_num_full_NCGR;
  prm.cell_id   = NARC_comm_btl_demo_line_num_full_NCER;
  prm.anm_id    = NARC_comm_btl_demo_line_num_full_NANR;
  prm.pltt_line = PLTID_OBJ_COMMON_M;
  prm.pltt_src_ofs = 0;
  prm.pltt_src_num = 4;
  
  unit = COMM_BTL_DEMO_GRAPHIC_GetClunit( wk->graphic );
  UI_EASY_CLWK_LoadResource( &wk->clres_common, &prm, unit, heapID );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  OBJ�J��
 *
 *  @param  COMM_BTL_DEMO_OBJ_WORK* wk 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void OBJ_Exit( COMM_BTL_DEMO_OBJ_WORK* wk )
{
  GF_ASSERT( wk );
  GF_ASSERT( wk->graphic );

  UI_EASY_CLWK_UnLoadResource( &wk->clres_common );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  CLWK����
 *
 *  @param  COMM_BTL_DEMO_OBJ_WORK* wk
 *  @param  px
 *  @param  py
 *  @param  anim 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static GFL_CLWK* OBJ_CreateCLWK( COMM_BTL_DEMO_OBJ_WORK* wk, s16 px, s16 py, u16 anim )
{
  GFL_CLUNIT* unit;

  GF_ASSERT( wk );
  GF_ASSERT( wk->graphic );
  
  unit = COMM_BTL_DEMO_GRAPHIC_GetClunit( wk->graphic );

  return UI_EASY_CLWK_CreateCLWK( &wk->clres_common, unit, px, py, anim, wk->heapID );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �c��|�P������OBJ�𐶐�
 *
 *  @param  TRAINER_UNIT* unit 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static GFL_CLWK* OBJ_CreatePokeNum( COMM_BTL_DEMO_OBJ_WORK* obj, u8 posid, u8 pokenum )
{
  GFL_CLWK* clwk;
  s16 px, py;
  int anm = 0;

  // ���W
  {
    //@TODO MN
    if( posid == 0 )
    {
      px = GX_LCD_SIZE_X - 50;
      py = NORMAL_POSID1_BALL_PY-16;
    }
    else
    {
      px = 50;
      py = NORMAL_POSID0_BALL_PY-16;
    }
  }
#if 0
  else
  {
    switch( posid )
    {
    case 0:
      px = GX_LCD_SIZE_X-50;
      py = MULTI_POSID0_BALL_PY-16;
      break;
    case 1:
      px = GX_LCD_SIZE_X-50+20;
      py = MULTI_POSID1_BALL_PY-16;
      break;
    case 2:
      px = 50;
      py = MULTI_POSID2_BALL_PY-16;
      break;
    case 3:
      px = 50+20;
      py = MULTI_POSID3_BALL_PY-16;
      break;
    default : GF_ASSERT(0);
    }
  }
#endif
  
  anm = pokenum;

  HOSAKA_Printf("after pokenum = %d\n", anm);

  clwk = OBJ_CreateCLWK( obj, px, py, anm );
  GFL_CLACT_WK_SetDrawEnable( clwk , FALSE );
  GFL_CLACT_WK_SetSoftPri( clwk, 1 ); ///< ��i�K�����Ă���

  return clwk;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ������
 *
 *  @param  COMM_BTL_DEMO_G3D_WORK* g3d 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void G3D_Init( COMM_BTL_DEMO_G3D_WORK* g3d, COMM_BTL_DEMO_GRAPHIC_WORK* graphic, HEAPID heapID )
{
  int i;
  
  GF_ASSERT( graphic );

  // �u�����h�w��
  G2_SetBlendAlpha( GX_PLANEMASK_BG0, GX_PLANEMASK_OBJ|GX_PLANEMASK_BG1|GX_PLANEMASK_BG2|GX_PLANEMASK_BG3|GX_PLANEMASK_BG0, 0, 0 );
//  G2_SetBlendAlpha( GX_PLANEMASK_BG0, GX_PLANEMASK_BG3|GX_PLANEMASK_BG2|GX_PLANEMASK_BG1|GX_PLANEMASK_BG0, 0, 0 );
  
  // �����o������
  g3d->graphic = graphic;
  g3d->heapID = heapID;

  // 3D�Ǘ����[�e�B���e�B�[�̐���
  g3d->g3d_util = GFL_G3D_UTIL_Create( 16, 20, heapID );

  // PTC SYSTEM
  GFL_PTC_Init( heapID );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �I������
 *
 *  @param  COMM_BTL_DEMO_G3D_WORK* g3d 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void G3D_End( COMM_BTL_DEMO_G3D_WORK* g3d )
{ 
  int i;

  GF_ASSERT( g3d );
  GF_ASSERT( g3d->g3d_util );

  // �A�j���J��
  G3D_AnimeExit( g3d );
  
  // PTC�J��
  G3D_PTC_Exit( g3d );

  // 3D�Ǘ����[�e�B���e�B�[�̔j��
  GFL_G3D_UTIL_Delete( g3d->g3d_util );
}


//-----------------------------------------------------------------------------
/**
 *  @brief  3D�֘A �又��
 *
 *  @param  COMM_BTL_DEMO_G3D_WORK * g3d 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void G3D_Main( COMM_BTL_DEMO_G3D_WORK * g3d )
{
  GF_ASSERT( g3d );

  //3D�`��
  COMM_BTL_DEMO_GRAPHIC_3D_StartDraw( g3d->graphic );

  if( g3d->ptc )
  {
    GFL_PTC_Main();
  }
  
  // �A�j��������ꍇ�͕`��
  if( g3d->is_add )
  {
    GFL_G3D_OBJSTATUS status;
    GFL_G3D_OBJ* obj;
    
    // �X�e�[�^�X������
    VEC_Set( &status.trans, 0, 0, 0 );
    VEC_Set( &status.scale, FX32_ONE, FX32_ONE, FX32_ONE );
    MTX_Identity33( &status.rotate );
    
    obj = GFL_G3D_UTIL_GetObjHandle( g3d->g3d_util, g3d->anm_unit_idx );
    
    GFL_G3D_DRAW_DrawObject( obj, &status );
  }
  
  COMM_BTL_DEMO_GRAPHIC_3D_EndDraw( g3d->graphic );
}

static VecFx32 sc_camera_eye = { 0, 0, FX32_CONST(128), };
static VecFx32 sc_camera_up = { 0, FX32_ONE, 0 };
static VecFx32 sc_camera_at = { 0, 0, 0 };

//-----------------------------------------------------------------------------
/**
 *  @brief  PTC���\�[�X���[�h
 *
 *  @param  COMM_BTL_DEMO_G3D_WORK* g3d
 *  @param  spa_idx 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void G3D_PTC_Setup( COMM_BTL_DEMO_G3D_WORK* g3d, int spa_idx )
{
  if( g3d->ptc )
  {
    G3D_PTC_Delete( g3d );
  }

  // PTC���[�N����
  g3d->ptc = GFL_PTC_Create( g3d->spa_work, sizeof(g3d->spa_work), TRUE, g3d->heapID );
  
  /* �p�[�e�B�N���̃J�����𐳎ˉe�ɐݒ� */  
  {
    GFL_G3D_PROJECTION projection; 
    projection.type = GFL_G3D_PRJORTH;
    projection.param1 = FX32_ONE*3.0f;
    projection.param2 = -(FX32_ONE*3.0f);
    projection.param3 = -(FX32_ONE*4.0f);
    projection.param4 = FX32_ONE*4.0f;  
#if 0
    projection.param1 = FX32_ONE*48.0f;
    projection.param2 = -(FX32_ONE*48.0f);
    projection.param3 = -(FX32_ONE*64.0f);
    projection.param4 = FX32_ONE*64.0f;  
#endif
    projection.near = FX32_CONST( 1 );
    projection.far  = FX32_CONST( 1024 ); // ���ˉe�Ȃ̂Ŋ֌W�Ȃ����A�O�̂��߃N���b�v��far��ݒ�
    projection.scaleW = 0;
    GFL_PTC_PersonalCameraDelete( g3d->ptc );
    GFL_PTC_PersonalCameraCreate( g3d->ptc, &projection, DEFAULT_PERSP_WAY, &sc_camera_eye, &sc_camera_up, &sc_camera_at, g3d->heapID );
  }

  // ���\�[�X�W�J
  {
    void *res;

    res = GFL_PTC_LoadArcResource( ARCID_COMM_BTL_DEMO_GRA, spa_idx, g3d->heapID );
    g3d->spa_num = GFL_PTC_GetResNum( res );
    OS_Printf("load spa_idx=%d num=%d \n", spa_idx, g3d->spa_num );
    GFL_PTC_SetResource( g3d->ptc, res, TRUE, NULL );
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief  PTC���[�N�폜
 *
 *  @param  COMM_BTL_DEMO_G3D_WORK* g3d 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void G3D_PTC_Delete( COMM_BTL_DEMO_G3D_WORK* g3d )
{
  GF_ASSERT( g3d->ptc );

  GFL_PTC_DeleteEmitterAll( g3d->ptc );
  GFL_PTC_Delete( g3d->ptc );
  g3d->ptc = NULL;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  PTC�V�X�e���I��
 *
 *  @param  COMM_BTL_DEMO_G3D_WORK* g3d 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void G3D_PTC_Exit( COMM_BTL_DEMO_G3D_WORK* g3d )
{
  if( g3d->ptc )
  {
    GFL_PTC_Delete( g3d->ptc );
  }

  GFL_PTC_Exit();
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �G�~�b�^����
 *
 *  @param  COMM_BTL_DEMO_G3D_WORK * g3d
 *  @param  idx
 *  @param  VecFx32* pos 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void G3D_PTC_CreateEmitter( COMM_BTL_DEMO_G3D_WORK * g3d, u16 idx, const VecFx32* pos )
{
  GFL_EMIT_PTR emit;
  
  GF_ASSERT(g3d);
  GF_ASSERT(g3d->ptc);

  emit = GFL_PTC_CreateEmitter( g3d->ptc, idx, pos );

  HOSAKA_Printf("emit=%d \n", emit);
}

//-----------------------------------------------------------------------------
/**
 *  @brief  SPA�̒���S�čĐ�
 *
 *  @param  COMM_BTL_DEMO_G3D_WORK* g3d
 *  @param  VecFx32* pos 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void G3D_PTC_CreateEmitterAll( COMM_BTL_DEMO_G3D_WORK* g3d, const VecFx32* pos )
{
  int i;

  for( i=0; i<g3d->spa_num; i++ )
  {
    G3D_PTC_CreateEmitter( g3d, i, pos );
  }
}

//-----------------------------------------------------------------------------
/** 
 *  @brief  imd���}�e���A���̃p���b�g�̌��т���ύX
 *
 *  @param  GFL_G3D_OBJ* obj G3D_OBJ
 *  @param  mat_idx �}�e���A�� �C���f�b�N�X
 *  @param  pal_idx �p���b�g �C���f�b�N�X
 *
 *  @retval TRUE:���� FALSE:���s
 */
//-----------------------------------------------------------------------------
static BOOL G3D_OBJ_PaletteReBind( GFL_G3D_OBJ* obj, int mat_idx, int pal_idx )
{
  GFL_G3D_RND* rnd = GFL_G3D_OBJECT_GetG3Drnd(obj);
  GFL_G3D_RES* restex = GFL_G3D_RENDER_GetG3DresTex(rnd);

  NNSG3dRenderObj* rndobj = GFL_G3D_RENDER_GetRenderObj(rnd);
  NNSG3dResMdl* mdl = NNS_G3dRenderObjGetResMdl(rndobj);
  NNSG3dResTex* tex = GFL_G3D_GetResTex(restex);
    
  BOOL result = TRUE;

  // �����[�X
  NNS_G3dReleaseMdlPltt(mdl);

  // ���o�C���h
  result &= NNS_G3dForceBindMdlPltt( mdl, tex, mat_idx, pal_idx );

  return result;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �A�j���[�V����������
 *
 *  @param  COMM_BTL_DEMO_G3D_WORK* g3d
 *  @param  demo_id 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void G3D_AnimeSet( COMM_BTL_DEMO_G3D_WORK* g3d, u16 demo_id )
{
  int i;

  // �폜����Ă��Ȃ���Ώ���
  if( g3d->is_add )
  {
    G3D_AnimeDel( g3d );
  }
  
  // ���j�b�g����
  g3d->anm_unit_idx = GFL_G3D_UTIL_AddUnit( g3d->g3d_util, &sc_setup[ demo_id ] );
  g3d->is_add = TRUE;
  HOSAKA_Printf("demo_id=%d add unit idx=%d ",demo_id, g3d->anm_unit_idx );
  
  {
    GFL_G3D_OBJ* obj;
    int anime_count;
    
    obj         = GFL_G3D_UTIL_GetObjHandle( g3d->g3d_util, g3d->anm_unit_idx );
    anime_count = GFL_G3D_OBJECT_GetAnimeCount( obj );

    HOSAKA_Printf("anime_count=%d \n", anime_count);

    // �A�j���[�V�����L����
    for( i=0; i<anime_count; i++ )
    {
      GFL_G3D_OBJECT_EnableAnime( obj, i );
    }

    HOSAKA_Printf("obj_count=%d resCount=%d resIdx=%d \n",
      GFL_G3D_UTIL_GetObjCount(g3d->g3d_util),
      GFL_G3D_UTIL_GetUnitResCount(g3d->g3d_util,0),
      GFL_G3D_UTIL_GetUnitResIdx(g3d->g3d_util, 0)
    );
  }
}

#if 0
//-----------------------------------------------------------------------------
/**
 *  @brief  �j������e�N�X�`�����\�[�X�̌��т���ύX
 *
 *  @param  COMM_BTL_DEMO_G3D_WORK* g3d
 *  @param  trsex 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void G3D_AnimeSetTrSex( COMM_BTL_DEMO_G3D_WORK* g3d, u8 trsex, u8 is_normal )
{
  GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( g3d->g3d_util, g3d->anm_unit_idx );
  BOOL result = TRUE;

  // �f�o�b�O����
  {
    static int mat = 0;
    static int pal = 0;

    if( GFL_UI_KEY_GetTrg() & PAD_KEY_UP )
    {
      mat++;
      HOSAKA_Printf("mat=%d pal=%d \n", mat, pal);
    }
    else if( GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN )
    {
      mat--;
      HOSAKA_Printf("mat=%d pal=%d \n", mat, pal);
    }
    else if( GFL_UI_KEY_GetTrg() & PAD_KEY_RIGHT )
    {
      pal++;
      HOSAKA_Printf("mat=%d pal=%d \n", mat, pal);
    }
    else if( GFL_UI_KEY_GetTrg() & PAD_KEY_LEFT )
    {
      pal--;
      HOSAKA_Printf("mat=%d pal=%d \n", mat, pal);
    }
    else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X )
    {
      BOOL result = TRUE;
      // �����[�X
      NNS_G3dReleaseMdlPltt(mdl);
      // ���o�C���h
      result &= NNS_G3dForceBindMdlPltt( mdl, tex, mat, pal );
      HOSAKA_Printf("bind mat=%d pal=%d \n", mat, pal);
      if( result == FALSE )
      {
        HOSAKA_Printf("failed.\n");
      }
    }
  }
}
#endif

//-----------------------------------------------------------------------------
/**
 *  @brief  �A�j���[�V�����폜
 *
 *  @param  COMM_BTL_DEMO_G3D_WORK* g3d 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void G3D_AnimeDel( COMM_BTL_DEMO_G3D_WORK* g3d )
{
  GF_ASSERT( g3d );

  HOSAKA_Printf("del unit_idx=%d\n", g3d->anm_unit_idx );

  // ���j�b�g�폜
  GFL_G3D_UTIL_DelUnit( g3d->g3d_util, g3d->anm_unit_idx );

  g3d->is_add = FALSE;
  g3d->is_end = FALSE;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �A�j���[�V�����V�X�e���I��
 *
 *  @param  COMM_BTL_DEMO_G3D_WORK* g3d 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void G3D_AnimeExit( COMM_BTL_DEMO_G3D_WORK* g3d )
{
  // �ǉ����ꂽ�܂܂Ȃ����
  if( g3d->is_add )
  {
    G3D_AnimeDel( g3d );
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �`�揈��
 *
 *  @param  COMM_BTL_DEMO_G3D_WORK* g3d 
 *
 *  @retval TRUE:���[�v�Đ���, FALSE: �Đ��I��
 */
//-----------------------------------------------------------------------------
static BOOL G3D_AnimeMain( COMM_BTL_DEMO_G3D_WORK* g3d )
{ 
  BOOL is_loop = TRUE;

  GF_ASSERT( g3d );
  GF_ASSERT( g3d->g3d_util );
  GF_ASSERT( g3d->is_add == TRUE );

  // �Đ��I����͂Ȃɂ�����RETURN
  if( g3d->is_end )
  {
    return FALSE;
  }

#if 0
    {
      static int frame = 0;

      // �A�j���[�V�����t���[������
      if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X )
      {
        frame+= FX32_ONE;
        HOSAKA_Printf("frame=%d\n",frame);
      }
      else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y )
      {
        frame-= FX32_ONE;
        HOSAKA_Printf("frame=%d\n",frame);
      }
      
      GFL_G3D_OBJECT_GetAnimeFrame( obj, 0, &frame );
      HOSAKA_Printf("frame=%d \n", frame>>FX32_SHIFT );
    }
#endif
  
  {
    int i;
    GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( g3d->g3d_util, g3d->anm_unit_idx );
    int anime_count = GFL_G3D_OBJECT_GetAnimeCount( obj );
    static BOOL speed = 1;  ///< �X�s�[�h

#ifdef DEBUG_ONLY_FOR_genya_hosaka
    if( PAD_BUTTON_SELECT & GFL_UI_KEY_GetTrg() )
    {
      speed ^= 1;
      HOSAKA_Printf("set anime speed=%d \n", speed );
    }
#endif
    
    // �A�j���[�V�����X�V
    for( i=0; i<anime_count; i++ )
    {
//    GFL_G3D_OBJECT_SetAnimeFrame( obj, i, &frame );
      is_loop = GFL_G3D_OBJECT_LoopAnimeFrame( obj, i, speed * FX32_ONE );

      if( is_loop == FALSE )
      {
        g3d->is_end = TRUE;
      }
    }
  }

  return is_loop;
}

  // @TODO �ėp��
#if 0
  // �f�o�b�Ofovy
  {
    static fx32 fovy = 40;
    
    GFL_G3D_CAMERA* p_camera = COMM_BTL_DEMO_GRAPHIC_GetCamera( wk->graphic );

    debug_camera_test( p_camera );

    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
    {
      fovy += 1;
      GFL_G3D_CAMERA_SetfovyCos( p_camera, FX_SinIdx( fovy/2 * PERSPWAY_COEFFICIENT ) );
      GFL_G3D_CAMERA_SetfovySin( p_camera, FX_CosIdx( fovy/2 * PERSPWAY_COEFFICIENT ) );
      HOSAKA_Printf("fovy = %d \n", fovy);
    }
    else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B )
    {
      fovy -= 1;
      GFL_G3D_CAMERA_SetfovyCos( p_camera, FX_SinIdx( fovy/2 * PERSPWAY_COEFFICIENT ) );
      GFL_G3D_CAMERA_SetfovySin( p_camera, FX_CosIdx( fovy/2 * PERSPWAY_COEFFICIENT ) );
      HOSAKA_Printf("fovy = %d \n", fovy);
    }
  }
#endif

