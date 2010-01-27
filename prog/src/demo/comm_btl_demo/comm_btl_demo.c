//=============================================================================
/**
 *
 *	@file		comm_btl_demo.c
 *	@brief  �ʐM�o�g���f��
 *	@author	hosaka genya
 *	@data		2009.09.29
 *
 */
//=============================================================================
//�K���K�v�ȃC���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

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
#include "comm_btl_demo.naix"	// �A�[�J�C�u

#include "comm_btl_demo_res.cdat"

FS_EXTERN_OVERLAY(ui_common);

//=============================================================================
/**
 *								�萔��`
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
  OBJ_ANM_ID_BALL_NORMAL = 10,  ///< �{�[���F�ʏ�
  OBJ_ANM_ID_BALL_SICK = 11,    ///< �{�[���F��Ԉُ�
  OBJ_ANM_ID_BALL_DOWN = 12,    ///< �{�[���F�m��
  OBJ_ANM_ID_BALL_NULL = 13,    ///< �{�[���F��
};

enum
{ 
  // �{�[��
  NORMAL_POSID0_BALL_PX_BASE = 128+8,
  NORMAL_POSID0_BALL_PX_OFS = 16,
  NORMAL_POSID0_BALL_PY = 7*8,
  NORMAL_POSID1_BALL_PY = 21*8,
  // �g���[�i�[��
  TRNAME_CSX = 10,
  TRNAME_CSY = 2,
  NORMAL_POSID0_TRNAME_CPX = 16,
  NORMAL_POSID0_TRNAME_CPY = 3,
  NORMAL_POSID1_TRNAME_CPX = 4,
  NORMAL_POSID1_TRNAME_CPY = 17,
  NORMAL_POSID0_TRNAME_PTC_PX = 0x800,
  NORMAL_POSID0_TRNAME_PTC_PY = 0x2800,
  NORMAL_POSID1_TRNAME_PTC_PX = 0xffffe000, 
  NORMAL_POSID1_TRNAME_PTC_PY = 0xffffe000,
};

//-------------------------------------
///	�t���[��
//=====================================
enum
{	
	BG_FRAME_TEXT_M	= GFL_BG_FRAME1_M,
	BG_FRAME_BAR_M	= GFL_BG_FRAME2_M,
	BG_FRAME_BACK_M	= GFL_BG_FRAME3_M,
	BG_FRAME_BACK_S	= GFL_BG_FRAME2_S,
  BG_FRAME_TEXT_S = GFL_BG_FRAME0_S, 
};

//-------------------------------------
///	�p���b�g
//=====================================
enum
{	
	//���C��BG
	PLTID_BG_BACK_M				= 0,
	PLTID_BG_TEXT_M				= 10,
	PLTID_BG_TASKMENU_M		= 11,
	PLTID_BG_TOUCHBAR_M		= 13,
	PLTID_BG_INFOWIN_M		= 15,
	//�T�uBG
	PLTID_BG_BACK_S				=	0,

  // Main OBJ
  PLTID_OBJ_COMMON_M = 0,
};

//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================

//--------------------------------------------------------------
///	BG�Ǘ����[�N
//==============================================================
typedef struct 
{
	int dummy;
} COMM_BTL_DEMO_BG_WORK;

//--------------------------------------------------------------
///	OBJ���[�N
//==============================================================
typedef struct {
  //[IN]
  COMM_BTL_DEMO_GRAPHIC_WORK* graphic;
  HEAPID  heapID;
  //[PRIVATE]
  UI_EASY_CLWK_RES clres_common;
} COMM_BTL_DEMO_OBJ_WORK;

//--------------------------------------------------------------
///	G3D�Ǘ����[�N
//==============================================================
typedef struct {
  //[IN]
  COMM_BTL_DEMO_GRAPHIC_WORK* graphic;
  HEAPID  heapID;
  //[PRIVATE]
  GFL_G3D_UTIL*   g3d_util;
  u16 anm_unit_idx;   ///< �A�j���[�V����������UNIT��IDX
  u8 padding[2];
  
  GFL_PTC_PTR     ptc;
  u8              spa_work[ PARTICLE_LIB_HEAP_SIZE ];
  u8  spa_num;

} COMM_BTL_DEMO_G3D_WORK;

#define BALL_CLWK_MAX ( 6 )

//--------------------------------------------------------------
///	�{�[��UNIT���[�N
//==============================================================
typedef struct {
  // [IN]
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
///	�g���[�i�[���j�b�g
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
  GFL_BMPWIN* win_name;
  BALL_UNIT ball; // �{�[���Ǘ����[�N
  u32 timer;
} TRAINER_UNIT;

//--------------------------------------------------------------
///	���C�����[�N
//==============================================================
typedef struct 
{
  HEAPID heapID;
  COMM_BTL_DEMO_PARAM * pwk;

	COMM_BTL_DEMO_BG_WORK		wk_bg;
  COMM_BTL_DEMO_OBJ_WORK  wk_obj;
  COMM_BTL_DEMO_G3D_WORK  wk_g3d;

  // �g���[�i�[���j�b�g
  TRAINER_UNIT  trainer_unit[ COMM_BTL_DEMO_TRDATA_MAX ];

	//�`��ݒ�
	COMM_BTL_DEMO_GRAPHIC_WORK	*graphic;

	//�t�H���g
	GFL_FONT						*font;

	//�v�����g�L���[
	PRINT_QUE						*print_que;
	GFL_MSGDATA					*msg;
  
  // �V�[���R���g���[��
  UI_SCENE_CNT_PTR  cntScene;

  u8 type;
  int timer;  ///< �f���N�����ԃJ�E���^

} COMM_BTL_DEMO_MAIN_WORK;


//=============================================================================
/**
 *							�V�[����`
 */
//=============================================================================

// �m�[�}���J�n
static BOOL SceneStartDemo_Init( UI_SCENE_CNT_PTR cnt, void* work );
static BOOL SceneStartDemo_Main( UI_SCENE_CNT_PTR cnt, void* work );
static BOOL SceneStartDemo_End( UI_SCENE_CNT_PTR cnt, void* work );

// �m�[�}���I��
static BOOL SceneEndDemo_Init( UI_SCENE_CNT_PTR cnt, void* work );
static BOOL SceneEndDemo_Main( UI_SCENE_CNT_PTR cnt, void* work );
static BOOL SceneEndDemo_End( UI_SCENE_CNT_PTR cnt, void* work );


//--------------------------------------------------------------
///	SceneID
//==============================================================
typedef enum
{ 
  CBD_SCENE_ID_NORMAL_START = 0,  ///< �o�g���J�n�f��
  CBD_SCENE_ID_NORMAL_END,        ///< �o�g���I���f��

  CBD_SCENE_ID_MAX,
} CBD_SCENE_ID;

//--------------------------------------------------------------
///	SceneFunc
//==============================================================
static const UI_SCENE_FUNC_SET c_scene_func_tbl[ CBD_SCENE_ID_MAX ] = 
{
  // CBD_SCENE_ID_NORMAL_START
  {
    SceneStartDemo_Init,
    NULL,
    SceneStartDemo_Main,
    NULL,
    SceneStartDemo_End,
  },
  // CBD_SCENE_ID_NORMAL_END
  {
    SceneEndDemo_Init,
    NULL,
    SceneEndDemo_Main,
    NULL,
    SceneEndDemo_End,
  },
};


//=============================================================================
/**
 *							�v���g�^�C�v�錾
 */
//=============================================================================
static CBD_SCENE_ID calc_first_scene( COMM_BTL_DEMO_PARAM* pwk );
BOOL type_is_normal( u8 type );
BOOL type_is_start( u8 type );

static void BALL_UNIT_Init( BALL_UNIT* unit, const POKEPARTY* party, u8 type, u8 posid, COMM_BTL_DEMO_OBJ_WORK* obj );
static void BALL_UNIT_Exit( BALL_UNIT* unit );
static void BALL_UNIT_SetStart( BALL_UNIT* unit );
static void BALL_UNIT_Main( BALL_UNIT* unit );

static void TRAINER_UNIT_Init( TRAINER_UNIT* unit, u8 type, u8 posid, const COMM_BTL_DEMO_TRAINER_DATA* data, COMM_BTL_DEMO_OBJ_WORK* obj, COMM_BTL_DEMO_G3D_WORK* g3d, GFL_FONT* font );
static void TRAINER_UNIT_Main( TRAINER_UNIT* unit );
static void TRAINER_UNIT_Exit( TRAINER_UNIT* unit );
static void TRAINER_UNIT_DrawTrainerName( TRAINER_UNIT* unit, GFL_FONT *font );

static void TRAINER_UNIT_CNT_Init( COMM_BTL_DEMO_MAIN_WORK* wk );
static void TRAINER_UNIT_CNT_Exit( COMM_BTL_DEMO_MAIN_WORK* wk );
static void TRAINER_UNIT_CNT_Main( COMM_BTL_DEMO_MAIN_WORK* wk );
static void TRAINER_UNIT_CNT_BallSetStart( COMM_BTL_DEMO_MAIN_WORK* wk );

static void OBJ_Init( COMM_BTL_DEMO_OBJ_WORK* wk, COMM_BTL_DEMO_GRAPHIC_WORK* graphic, HEAPID heapID );
static void OBJ_Exit( COMM_BTL_DEMO_OBJ_WORK* wk );
static GFL_CLWK* OBJ_CreateCLWK( COMM_BTL_DEMO_OBJ_WORK* wk, s16 px, s16 py, u16 anim );

static void G3D_Init( COMM_BTL_DEMO_G3D_WORK* g3d, COMM_BTL_DEMO_GRAPHIC_WORK* graphic, HEAPID heapID );
static void G3D_End( COMM_BTL_DEMO_G3D_WORK* g3d );
static void G3D_Main( COMM_BTL_DEMO_G3D_WORK * g3d );
static void G3D_PTC_Setup( COMM_BTL_DEMO_G3D_WORK* g3d, int spa_idx );
static void G3D_PTC_Delete( COMM_BTL_DEMO_G3D_WORK* g3d );
static void G3D_PTC_CreateEmitter( COMM_BTL_DEMO_G3D_WORK * g3d, u16 idx, const VecFx32* pos );
static void G3D_AnimeSet( COMM_BTL_DEMO_G3D_WORK* g3d, u16 demo_id );
static void G3D_AnimeDel( COMM_BTL_DEMO_G3D_WORK* g3d );
static BOOL G3D_AnimeMain( COMM_BTL_DEMO_G3D_WORK* g3d );

  //-------------------------------------
///	PROC
//=====================================
static GFL_PROC_RESULT CommBtlDemoProc_Init( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
static GFL_PROC_RESULT CommBtlDemoProc_Main( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
static GFL_PROC_RESULT CommBtlDemoProc_Exit( GFL_PROC *proc, int *seq, void *pwk, void *mywk );

//-------------------------------------
///	�ėp�������[�e�B���e�B
//=====================================
static void CommBtlDemo_BG_LoadResource( COMM_BTL_DEMO_BG_WORK* wk, HEAPID heapID );

//=============================================================================
/**
 *								�O�����J
 */
//=============================================================================
const GFL_PROC_DATA CommBtlDemoProcData = 
{
	CommBtlDemoProc_Init,
	CommBtlDemoProc_Main,
	CommBtlDemoProc_Exit,
};

//@TODO hosaka�̂�
#ifdef PM_DEBUG
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
    prm->trainer_data[i].trsex = (i!=0) ? PM_MALE : PM_FEMALE; //@TODO

    // �g���[�i�[��
    {
      //�I�[�R�[�h��ǉ����Ă���STRBUF�ɕϊ�
      STRCODE debugname[32] = L"�Ƃ���";
      
      debugname[3] = GFL_STR_GetEOMCode();

      prm->trainer_data[i].str_trname = GFL_STR_CreateBuffer( sizeof(STRCODE)*10, HEAPID_COMM_BTL_DEMO );
      GFL_STR_SetStringCode( prm->trainer_data[i].str_trname, debugname );
    }
    
    // �f�o�b�O�|�P�p�[�e�B�[
    {
      POKEPARTY *party;
      int poke_cnt;
      int p;

      party = PokeParty_AllocPartyWork( HEAPID_COMM_BTL_DEMO );
      Debug_PokeParty_MakeParty( party );
      
      prm->trainer_data[i].party = party;
    
      poke_cnt = PokeParty_GetPokeCount( prm->trainer_data[i].party );

      for( p=0; p<poke_cnt; p++ )
      {
        POKEMON_PARAM* pp = PokeParty_GetMemberPointer( party, p );
        if( GFUser_GetPublicRand(2) == 0 )
        {
          PP_SetSick( pp, POKESICK_DOKU );
        }
        HOSAKA_Printf("poke [%d] condition=%d \n",p, PP_Get( pp, ID_PARA_condition, NULL ) );
      }

      HOSAKA_Printf("[%d] server_version=%d trsex=%d poke_cnt=%d \n",i, 
          prm->trainer_data[i].server_version,
          prm->trainer_data[i].trsex,
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
    GFL_STR_DeleteBuffer( prm->trainer_data[i].str_trname );
    GFL_HEAP_FreeMemory( (POKEPARTY*)prm->trainer_data[i].party );
  }
}
#endif

//=============================================================================
/**
 *								PROC
 */
//=============================================================================
//-----------------------------------------------------------------------------
/**
 *	@brief  PROC ����������
 *
 *	@param	GFL_PROC *proc�v���Z�X�V�X�e��
 *	@param	*seq					�V�[�P���X
 *	@param	*pwk					COMM_BTL_DEMO_PARAM
 *	@param	*mywk					PROC���[�N
 *
 *	@retval	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT CommBtlDemoProc_Init( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{
	COMM_BTL_DEMO_MAIN_WORK *wk;

	//�I�[�o�[���C�ǂݍ���
	GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common));

	//�q�[�v�쐬
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_COMM_BTL_DEMO, COMM_BTL_DEMO_HEAP_SIZE );
  wk = GFL_PROC_AllocWork( proc, sizeof(COMM_BTL_DEMO_MAIN_WORK), HEAPID_COMM_BTL_DEMO );
  GFL_STD_MemClear( wk, sizeof(COMM_BTL_DEMO_MAIN_WORK) );
	
#ifdef PM_DEBUG
  debug_param( pwk );
#endif

  // ������
  wk->heapID = HEAPID_COMM_BTL_DEMO;
  wk->pwk = pwk;
  wk->type = wk->pwk->type;
	
	//�`��ݒ菉����
	wk->graphic	= COMM_BTL_DEMO_GRAPHIC_Init( GX_DISP_SELECT_SUB_MAIN, wk->heapID );

	//�t�H���g�쐬
	wk->font			= GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
												GFL_FONT_LOADTYPE_FILE, FALSE, wk->heapID );

	//���b�Z�[�W
	wk->msg = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
			NARC_message_mictest_dat, wk->heapID );

	//PRINT_QUE�쐬
	wk->print_que		= PRINTSYS_QUE_Create( wk->heapID );
  
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

  // �t�F�[�h�C�� ���N�G�X�g
  GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 2 );

  return GFL_PROC_RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  PROC �I������
 *
 *	@param	GFL_PROC *procp���Z�X�V�X�e��
 *	@param	*seq					�V�[�P���X
 *	@param	*pwk					COMM_BTL_DEMO_PARAM
 *	@param	*mywk					PROC���[�N
 *
 *	@retval	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT CommBtlDemoProc_Exit( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{ 
	COMM_BTL_DEMO_MAIN_WORK* wk = mywk;

#ifdef DEBUG_ONLY_FOR_genya_hosaka
  debug_param_del( pwk );
#endif
  
  // �t�F�[�h���͏������Ȃ�
  if( GFL_FADE_CheckFade() == TRUE )
  {
    return GFL_PROC_RES_CONTINUE;
  }
  
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
 *	@brief  PROC �又��
 *
 *	@param	GFL_PROC *proc�v���Z�X�V�X�e��
 *	@param	*seq					�V�[�P���X
 *	@param	*pwk					COMM_BTL_DEMO_PARAM
 *	@param	*mywk					PROC���[�N
 *
 *	@retval	�I���R�[�h
 */
//-----------------------------------------------------------------------------
#include "ui/debug_code/g3d/camera_test.c"
static GFL_PROC_RESULT CommBtlDemoProc_Main( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{ 
	COMM_BTL_DEMO_MAIN_WORK* wk = mywk;

  //@TODO �J�����e�X�g
#ifdef PM_DEBUG
#if 0
  {
    GFL_G3D_CAMERA* p_camera = COMM_BTL_DEMO_GRAPHIC_GetCamera( wk->graphic );
    debug_camera_test( p_camera );
  }
#endif
#endif

  G3D_Main( &wk->wk_g3d );

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
  
  // �t�F�[�h���͏������Ȃ�
  if( GFL_FADE_CheckFade() == TRUE )
  {
    return GFL_PROC_RES_CONTINUE;
  }
	
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
 *								static�֐�
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  BG�Ǘ����W���[�� ���\�[�X�ǂݍ���
 *
 *	@param	COMM_BTL_DEMO_BG_WORK* wk BG�Ǘ����[�N
 *	@param	heapID  �q�[�vID 
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void CommBtlDemo_BG_LoadResource( COMM_BTL_DEMO_BG_WORK* wk, HEAPID heapID )
{
	ARCHANDLE	*handle;

	handle	= GFL_ARC_OpenDataHandle( ARCID_COMM_BTL_DEMO_GRA, heapID );

	// �㉺��ʂa�f�p���b�g�]��
	GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_comm_btl_demo_bg_base_u_NCLR, PALTYPE_MAIN_BG, PLTID_BG_BACK_M, 0x0, heapID );
	GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_comm_btl_demo_bg_base_d_NCLR, PALTYPE_SUB_BG, PLTID_BG_BACK_S, 0x0, heapID );
  
  // ��b�t�H���g�p���b�g�]��
	GFL_ARC_UTIL_TransVramPalette( ARCID_FONT,
      NARC_font_default_nclr,
      PALTYPE_MAIN_BG,
      0x20*PLTID_BG_TEXT_M, 0x20, heapID );
	
	
  //	----- ����� -----
	GFL_ARCHDL_UTIL_TransVramBgCharacter(	handle, NARC_comm_btl_demo_bg_base_d_NCGR,
						BG_FRAME_BACK_S, 0, 0, 0, heapID );
	GFL_ARCHDL_UTIL_TransVramScreen(	handle, NARC_comm_btl_demo_bg_base_d_NSCR,
						BG_FRAME_BACK_S, 0, 0, 0, heapID );	

	//	----- ���� -----
	GFL_ARCHDL_UTIL_TransVramBgCharacter(	handle, NARC_comm_btl_demo_bg_base_u_NCGR,
						BG_FRAME_BACK_M, 0, 0, 0, heapID );
	GFL_ARCHDL_UTIL_TransVramScreen(	handle, NARC_comm_btl_demo_bg_base_u_NSCR,
						BG_FRAME_BACK_M, 0, 0, 0, heapID );		

	GFL_ARC_CloseDataHandle( handle );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �o�g���O�f�� ����������
 *
 *	@param	UI_SCENE_CNT_PTR cnt
 *	@param	work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneStartDemo_Init( UI_SCENE_CNT_PTR cnt, void* work )
{
  COMM_BTL_DEMO_MAIN_WORK* wk = work;
  
  // �g���C�i�[���j�b�g������
  TRAINER_UNIT_CNT_Init( wk );

#ifdef DEBUG_ONLY_FOR_genya_hosaka
  GFL_BG_ClearScreen( BG_FRAME_TEXT_M ); // �e�L�X�g�ʂ��������Ă���
    // �t�F�[�h�A�E�g ���N�G�X�g
  GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_WHITEOUT, 16, 0, 1 );
#endif

  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �o�g���O�f�� �又��
 *
 *	@param	UI_SCENE_CNT_PTR cnt
 *	@param	work 
 *
 *	@retval
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
    G3D_PTC_Setup( &wk->wk_g3d, NARC_comm_btl_demo_vs_demo01_spa );

    if( type_is_normal(wk->type) )
    {
      G3D_AnimeSet( &wk->wk_g3d, DEMO_ID_01_A );
    }
    else
    {
      G3D_AnimeSet( &wk->wk_g3d, DEMO_ID_02_A );
    }
    
    // �{�[���A�j���J�n
    TRAINER_UNIT_CNT_BallSetStart( wk );

    UI_SCENE_CNT_IncSubSeq( cnt );
    break;
  case 1:
    if( G3D_AnimeMain( &wk->wk_g3d ) == FALSE )
    {
      G3D_AnimeDel( &wk->wk_g3d );
      G3D_AnimeSet( &wk->wk_g3d, DEMO_ID_DRAW );

      // ptc���[�N�����uVS�v
      G3D_PTC_Delete( &wk->wk_g3d );
      G3D_PTC_Setup( &wk->wk_g3d, NARC_comm_btl_demo_vs_demo02_spa );

      UI_SCENE_CNT_IncSubSeq( cnt );
    }
    break;
  case 2:
    {
      // �uVS�v�p�[�e�B�N���\��
      {
        int i;
        for( i=0; i<wk->wk_g3d.spa_num; i++ )
        {
          G3D_PTC_CreateEmitter( &wk->wk_g3d, i, &(VecFx32){0,0,-100} );
        }
      }

      wk->timer = 0;
      UI_SCENE_CNT_IncSubSeq( cnt );
    }
    break;
  case 3:
    //@TODO �^�C�~���O
    if( wk->timer++ == 120+15 )
    {
      // �t�F�[�h�A�E�g ���N�G�X�g
      GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_WHITEOUT, 0, 16, 2 );
    }
    
    // �A�j���Đ�
    if( G3D_AnimeMain( &wk->wk_g3d ) == FALSE )
    {
      G3D_AnimeDel( &wk->wk_g3d );
      return TRUE;
    }
    break;
  default : GF_ASSERT(0);
  }

  return FALSE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �o�g���O�f�� �I������
 *
 *	@param	UI_SCENE_CNT_PTR cnt
 *	@param	work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneStartDemo_End( UI_SCENE_CNT_PTR cnt, void* work )
{ 
  COMM_BTL_DEMO_MAIN_WORK* wk = work;
  
  // �g���[�i�[�J��
  TRAINER_UNIT_CNT_Exit( wk );

  // PTC���[�N�J��
  G3D_PTC_Delete( &wk->wk_g3d );
  
  // �I��
  UI_SCENE_CNT_SetNextScene( cnt, UI_SCENE_ID_END );

// @TODO �ۍ�̂݃��[�v
#ifdef DEBUG_ONLY_FOR_genya_hosaka
  if( (GFL_UI_KEY_GetCont() & PAD_BUTTON_START) == FALSE )
  {
    UI_SCENE_CNT_SetNextScene( cnt, CBD_SCENE_ID_NORMAL_START );
  }
#endif

  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �o�g����f�� ����������
 *
 *	@param	UI_SCENE_CNT_PTR cnt
 *	@param	*work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneEndDemo_Init( UI_SCENE_CNT_PTR cnt, void *work )
{
  COMM_BTL_DEMO_MAIN_WORK* wk = work;

  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �o�g����f�� �又��
 *
 *	@param	UI_SCENE_CNT_PTR cnt
 *	@param	work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneEndDemo_Main( UI_SCENE_CNT_PTR cnt, void* work )
{
  // �I��
  UI_SCENE_CNT_SetNextScene( cnt, UI_SCENE_ID_END );
  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �o�g����f�� �又��
 *
 *	@param	UI_SCENE_CNT_PTR cnt
 *	@param	work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneEndDemo_End( UI_SCENE_CNT_PTR cnt, void* work )
{
  COMM_BTL_DEMO_MAIN_WORK* wk = work;

  return TRUE;
}


//-----------------------------------------------------------------------------
/**
 *	@brief  �ŏ��̃V�[���𔻒�
 *
 *	@param	COMM_BTL_DEMO_MAIN_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static CBD_SCENE_ID calc_first_scene( COMM_BTL_DEMO_PARAM* pwk )
{
  switch( pwk->type )
  {
  case COMM_BTL_DEMO_TYPE_NORMAL_START:
  case COMM_BTL_DEMO_TYPE_MULTI_START:
    return CBD_SCENE_ID_NORMAL_START;

  case COMM_BTL_DEMO_TYPE_NORMAL_END:
  case COMM_BTL_DEMO_TYPE_MULTI_END:
    return CBD_SCENE_ID_NORMAL_END;

  default : GF_ASSERT_MSG( 0 , "demo type=%d ", pwk->type);
  }

  return CBD_SCENE_ID_NORMAL_START;
}

//-----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	u8 type 
 *
 *	@retval
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
 *	@brief  �^�C�v���X�^�[�g���G���h���擾
 *
 *	@param	u8 type 
 *
 *	@retval
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
 *	@brief  �|�P�p������{�[��OBJ�̃A�j��ID���擾
 *
 *	@param	POKEMON_PARAM* pp 
 *
 *	@retval
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
 *	@brief  �{�[�����j�b�g������
 *
 *	@param	BALL_UNIT* unit
 *	@param	POKEPARTY* party
 *	@param	type
 *	@param	posid
 *	@param	obj 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void BALL_UNIT_Init( BALL_UNIT* unit, const POKEPARTY* party, u8 type, u8 posid, COMM_BTL_DEMO_OBJ_WORK* obj )
{
  int i;
  int is_normal = type_is_normal(type);

  GF_ASSERT(unit);
  GF_ASSERT(party);
  GF_ASSERT(obj);

  unit->num = PokeParty_GetPokeCount( party );
  unit->max = PokeParty_GetPokeCountMax( party );
  unit->type = type;
  unit->posid = posid;
  unit->timer = 0;

  HOSAKA_Printf("max=%d pokenum=%d\n", unit->max, unit->num);

  for( i=0; i<unit->max; i++ )
  {
    s16 px;
    s16 py;
    int anm;
  
    if( type_is_normal(type) )
    {
      // �m�[�}��
      px = NORMAL_POSID0_BALL_PX_BASE + (i) * NORMAL_POSID0_BALL_PX_OFS;
      py= (posid==0) ? NORMAL_POSID0_BALL_PY : NORMAL_POSID1_BALL_PY;

      if( posid == 1 ){ px *= -1; }
    }
    else
    {
      //@TODO posid �ɂ����W����
      // �}���`
      px = NORMAL_POSID0_BALL_PX_BASE + (i) * NORMAL_POSID0_BALL_PX_OFS;
      py= (posid==0) ? NORMAL_POSID0_BALL_PY : NORMAL_POSID1_BALL_PY;

      if( posid == 1 ){ px *= -1; }
    }

    // �J�n�f��
    if( type_is_start(type) )
    {
      // ��Ԏ擾
      POKEMON_PARAM* pp = NULL;
      
      // �|�P���������݂���Ԃ�POKEMON_PARAM���擾
      if( i < unit->num )
      {
        pp  = PokeParty_GetMemberPointer( party, i );
      }

      anm = PokeParaToBallAnim( pp );

    }
    else
    // �I���f��
    {
      // �S�Ēʏ��Ԃŏ�����
      anm = OBJ_ANM_ID_BALL_NORMAL;
    }
      
    // CLWK����
    unit->clwk[i] = OBJ_CreateCLWK( obj, px, py, anm );
    GFL_CLACT_WK_SetDrawEnable( unit->clwk[i] , FALSE );
  }
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �{�[�����j�b�g�J��
 *
 *	@param	BALL_UNIT* unit 
 *
 *	@retval
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
 *	@brief  �A�j���J�n
 *
 *	@param	BALL_UNIT* unit 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void BALL_UNIT_SetStart( BALL_UNIT* unit )
{
  unit->is_start = TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �{�[���又��
 *
 *	@param	BALL_UNIT* unit 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void BALL_UNIT_Main( BALL_UNIT* unit )
{
  const int OPEN_SYNC = 12;

  if( unit->is_start == FALSE )
  {
    return;
  }

  switch( unit->type )
  {
  case COMM_BTL_DEMO_TYPE_NORMAL_START :
    if( unit->timer >= OPEN_SYNC )
    {
      int id = unit->timer - OPEN_SYNC;

      HOSAKA_Printf("unit->timer=%d ",unit->timer);
      HOSAKA_Printf("ball open id=%d\n", id);

      if( id < BALL_CLWK_MAX )
      {
        // �J��
        GFL_CLACT_WK_SetDrawEnable( unit->clwk[id] , TRUE );
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
    break;
  case COMM_BTL_DEMO_TYPE_NORMAL_END :
    break;
  case COMM_BTL_DEMO_TYPE_MULTI_START :
    //@TODO �m�[�}���ƈꏏ�ł�����\��������
    break;
  case COMM_BTL_DEMO_TYPE_MULTI_END :
    break;
  default : GF_ASSERT(0);
  }

  unit->timer++;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �g���[�i�[���pBMPWIN����
 *
 *	@param	u8 type
 *	@param	posid 
 *
 *	@retval
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
    //@TODO �}���`
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
 *	@brief  �g���[�i�[���j�b�g ������
 *
 *	@param	TRAINER_UNIT* unit
 *	@param	posid
 *	@param	COMM_BTL_DEMO_TRAINER_DATA* data
 *	@param	obj 
 *
 *	@retval
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
  BALL_UNIT_Init( &unit->ball, data->party, type, posid, unit->obj );

  // �g���[�i�[��������
  unit->win_name = TRAINERNAME_WIN_Create( type, posid );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �g���[�i�[���j�b�g �又��
 *
 *	@param	TRAINER_UNIT* unit 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void TRAINER_UNIT_Main( TRAINER_UNIT* unit )
{
  GF_ASSERT( unit->g3d );

  // �{�[���又��
  BALL_UNIT_Main( &unit->ball );

  //@TODO �^�C�~���O�F�}�W�b�N�i���o�[
  // �g���[�i�[���\��
  if( unit->timer == 45 ) 
  { 
    // �g���[�i�[���\��
    TRAINER_UNIT_DrawTrainerName( unit, unit->font );

    // �p�[�e�B�N���\��
    {
      int p;
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
        //@TODO �}���`
      }

      for( p=0; p<unit->g3d->spa_num; p++ )
      {
        G3D_PTC_CreateEmitter( unit->g3d, p, &(VecFx32){fx,fy,-100} );
      }
    }
  
  }  

//  HOSAKA_Printf("unit timer=%d \n", unit->timer);

  unit->timer++;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �g���C�i�[���j�b�g �㏈��
 *
 *	@param	TRAINER_UNIT* unit 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void TRAINER_UNIT_Exit( TRAINER_UNIT* unit )
{
  // �{�[���J��
  BALL_UNIT_Exit( &unit->ball );
  // �g���[�i�[��BMPWIN���
  GFL_BMPWIN_Delete( unit->win_name );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �g���[�i�[���j�b�g �g���[�i�[�������A�\��
 *
 *	@param	TRAINER_UNIT* unit 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void TRAINER_UNIT_DrawTrainerName( TRAINER_UNIT* unit, GFL_FONT *font )
{
  // �������]��

  GFL_FONTSYS_SetColor( 0xf, 0x1, 0x2 );

#if 0
  if( unit->data->trsex == PM_MALE )
  {
    // �j����
    GFL_FONTSYS_SetColor( 0xf, 0x5, 0x6 );
  }
  else
  {
    // ������
    GFL_FONTSYS_SetColor( 0xf, 0x3, 0x4 );
  }
#endif

  PRINTSYS_Print( GFL_BMPWIN_GetBmp(unit->win_name), 0, 0, unit->data->str_trname, font );
  GFL_BMPWIN_MakeTransWindow_VBlank( unit->win_name );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �g���[�i�[���j�b�g �������R���g���[���[
 *
 *	@param	COMM_BTL_DEMO_MAIN_WORK* wk 
 *
 *	@retval
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
          wk->font
          );
  }
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �g���[�i�[���j�b�g �㏈���R���g���[���[
 *
 *	@param	COMM_BTL_DEMO_MAIN_WORK* wk 
 *
 *	@retval
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
 *	@brief  �g���[�i�[���j�b�g �又���R���g���[���[
 *
 *	@param	COMM_BTL_DEMO_MAIN_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void TRAINER_UNIT_CNT_Main( COMM_BTL_DEMO_MAIN_WORK* wk )
{
  int i;
  int max = (type_is_normal(wk->type) ? TRAINER_CNT_NORMAL : TRAINER_CNT_MULTI ); 

#if 0
  // �p�[�e�B�N�����W����
  {
    static fx32 fx=0;
    static fx32 fy=0;
    
    if( GFL_UI_KEY_GetTrg() & PAD_KEY_UP )  { fy+=FX32_ONE/2; }else
    if( GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN ){ fy-=FX32_ONE/2; }else
    if( GFL_UI_KEY_GetTrg() & PAD_KEY_LEFT ){ fx-=FX32_ONE/2; }else
    if( GFL_UI_KEY_GetTrg() & PAD_KEY_RIGHT ){ fx+=FX32_ONE/2; }else
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
    {
      if( wk->wk_g3d.ptc != NULL )
      {
        int p;
        HOSAKA_Printf("{0x%x,0x%x}\n", fx, fy);
    
        for( p=0; p<wk->wk_g3d.spa_num; p++ )
        {
          G3D_PTC_CreateEmitter( &wk->wk_g3d, p, &(VecFx32){fx,fy,-100} );
        }
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
 *	@brief  �g���[�i�[���j�b�g �{�[���J�n�R���g���[���[
 *
 *	@param	COMM_BTL_DEMO_MAIN_WORK* wk 
 *
 *	@retval
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
 *	@brief  OBJ������
 *
 *	@param	COMM_BTL_DEMO_OBJ_WORK* wk 
 *
 *	@retval
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
 *	@brief  OBJ�J��
 *
 *	@param	COMM_BTL_DEMO_OBJ_WORK* wk 
 *
 *	@retval
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
 *	@brief  CLWK����
 *
 *	@param	COMM_BTL_DEMO_OBJ_WORK* wk
 *	@param	px
 *	@param	py
 *	@param	anim 
 *
 *	@retval
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
 *	@brief  ������
 *
 *	@param	COMM_BTL_DEMO_G3D_WORK* g3d 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void G3D_Init( COMM_BTL_DEMO_G3D_WORK* g3d, COMM_BTL_DEMO_GRAPHIC_WORK* graphic, HEAPID heapID )
{
  int i;
  
  GF_ASSERT( graphic );

  // �u�����h�w��
  G2_SetBlendAlpha( GX_PLANEMASK_BG0, GX_PLANEMASK_OBJ|GX_PLANEMASK_BG1|GX_PLANEMASK_BG2|GX_PLANEMASK_BG3|GX_PLANEMASK_BG0, 16, 0 );
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
 *	@brief  �I������
 *
 *	@param	COMM_BTL_DEMO_G3D_WORK* g3d 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void G3D_End( COMM_BTL_DEMO_G3D_WORK* g3d )
{ 
  int i;

  GF_ASSERT( g3d );
  GF_ASSERT( g3d->g3d_util );
  
  // PTC�J��
  GFL_PTC_Exit();

  // 3D�Ǘ����[�e�B���e�B�[�̔j��
  GFL_G3D_UTIL_Delete( g3d->g3d_util );
}


//-----------------------------------------------------------------------------
/**
 *	@brief  3D�֘A �又��
 *
 *	@param	COMM_BTL_DEMO_G3D_WORK * g3d 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void G3D_Main( COMM_BTL_DEMO_G3D_WORK * g3d )
{
  GF_ASSERT( g3d );

  if( g3d->ptc )
  {
    GFL_PTC_Main();
  }
}

static VecFx32 sc_camera_eye = { 0, 0, FX32_CONST(70), };
static VecFx32 sc_camera_up = { 0, FX32_ONE, 0 };
static VecFx32 sc_camera_at = { 0, 0, -FX32_ONE };

//-----------------------------------------------------------------------------
/**
 *	@brief  PTC���\�[�X���[�h
 *
 *	@param	COMM_BTL_DEMO_G3D_WORK* g3d
 *	@param	spa_idx 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void G3D_PTC_Setup( COMM_BTL_DEMO_G3D_WORK* g3d, int spa_idx )
{
  GF_ASSERT( g3d->ptc == NULL );

  // PTC���[�N����
  g3d->ptc = GFL_PTC_Create( g3d->spa_work, sizeof(g3d->spa_work), TRUE, g3d->heapID );
  
  /* �p�[�e�B�N���̃J�����𐳎ˉe�ɐݒ� */	
  {
    GFL_G3D_PROJECTION projection; 
    projection.type = GFL_G3D_PRJORTH;
    projection.param1 = FX32_CONST(4); 
    projection.param2 = -FX32_CONST(4); 
    projection.param3 = -FX32_CONST(3); 
    projection.param4 = FX32_CONST(3);  
    projection.near = FX32_ONE * 1;
    projection.far  = FX32_CONST( 1024 );	// ���ˉe�Ȃ̂Ŋ֌W�Ȃ����A�O�̂��߃N���b�v��far��ݒ�
    projection.scaleW = FX32_ONE;//0
    GFL_PTC_PersonalCameraDelete( g3d->ptc );
    GFL_PTC_PersonalCameraCreate( g3d->ptc, &projection, DEFAULT_PERSP_WAY, &sc_camera_eye, &sc_camera_up, &sc_camera_at, g3d->heapID );
  }

  // ���\�[�X�W�J
  {
    void *res;

    res = GFL_PTC_LoadArcResource( ARCID_COMM_BTL_DEMO_GRA, spa_idx, g3d->heapID );
    g3d->spa_num = GFL_PTC_GetResNum( res );
    HOSAKA_Printf("load spa_idx=%d num=%d \n", spa_idx, g3d->spa_num );
    GFL_PTC_SetResource( g3d->ptc, res, TRUE, NULL );
  }

}

//-----------------------------------------------------------------------------
/**
 *	@brief  PTC���[�N�폜
 *
 *	@param	COMM_BTL_DEMO_G3D_WORK* g3d 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void G3D_PTC_Delete( COMM_BTL_DEMO_G3D_WORK* g3d )
{
  GF_ASSERT( g3d->ptc );

  GFL_PTC_Delete( g3d->ptc );
  g3d->ptc = NULL;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �G�~�b�^����
 *
 *	@param	COMM_BTL_DEMO_G3D_WORK * g3d
 *	@param	idx
 *	@param	VecFx32* pos 
 *
 *	@retval
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
 *	@brief  �A�j���[�V����������
 *
 *	@param	COMM_BTL_DEMO_G3D_WORK* g3d
 *	@param	demo_id 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void G3D_AnimeSet( COMM_BTL_DEMO_G3D_WORK* g3d, u16 demo_id )
{
  int i;
  
  // ���j�b�g����
  g3d->anm_unit_idx = GFL_G3D_UTIL_AddUnit( g3d->g3d_util, &sc_setup[ demo_id ] );
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

    //@TODO �j������e�N�X�`���̃p���b�g��ݒ�
    {
      GFL_G3D_RND* rnd = GFL_G3D_OBJECT_GetG3Drnd(obj);
//    NNSG3dRenderObj* renderobj = GFL_G3D_RENDER_GetRenderObj( GFL_G3D_OBJECT_GetG3Drnd(g3Dobj) );
//    GFL_G3D_RES* tex = GFL_G3D_RENDER_GetG3DresTex(rnd);
//    GFL_G3D_RES* mdl = GFL_G3D_RENDER_GetG3DresMdl(rnd);

//    GFL_G3D_UTIL_SetObjAnotherUnitAnime

//BOOL GFL_G3D_RENDER_SetTexture( GFL_G3D_RND* g3Drnd, const int mdlidx, const GFL_G3D_RES* tex )
    }
  }
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �A�j���[�V�����폜
 *
 *	@param	COMM_BTL_DEMO_G3D_WORK* g3d 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void G3D_AnimeDel( COMM_BTL_DEMO_G3D_WORK* g3d )
{
  GF_ASSERT( g3d );

  HOSAKA_Printf("del unit_idx=%d\n", g3d->anm_unit_idx );

  // ���j�b�g�폜
  GFL_G3D_UTIL_DelUnit( g3d->g3d_util, g3d->anm_unit_idx );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �`�揈��
 *
 *	@param	COMM_BTL_DEMO_G3D_WORK* g3d 
 *
 *	@retval TRUE:���[�v�Đ���, FALSE: �Đ��I��
 */
//-----------------------------------------------------------------------------
static BOOL G3D_AnimeMain( COMM_BTL_DEMO_G3D_WORK* g3d )
{ 
  GFL_G3D_OBJSTATUS status;
  BOOL is_loop;

  GF_ASSERT( g3d );
  GF_ASSERT( g3d->g3d_util );
  
  // �X�e�[�^�X������
  VEC_Set( &status.trans, 0, 0, 0 );
  VEC_Set( &status.scale, FX32_ONE, FX32_ONE, FX32_ONE );
  MTX_Identity33( &status.rotate );

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
    
    // �A�j���[�V�����X�V
    for( i=0; i<anime_count; i++ )
    {
      static int speed = 1;
#ifdef DEBUG_ONLY_FOR_genya_hosaka
      if( PAD_BUTTON_SELECT & GFL_UI_KEY_GetTrg() )
      {
        speed ^= 1;
      }
#endif
//    GFL_G3D_OBJECT_SetAnimeFrame( obj, i, &frame );
      is_loop = GFL_G3D_OBJECT_LoopAnimeFrame( obj, i, speed * FX32_ONE );
    }

    //3D�`��
    COMM_BTL_DEMO_GRAPHIC_3D_StartDraw( g3d->graphic );
    
    // �`��
    GFL_G3D_DRAW_DrawObject( obj, &status );
    
    COMM_BTL_DEMO_GRAPHIC_3D_EndDraw( g3d->graphic );
  }

  return is_loop;
}

