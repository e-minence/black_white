//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		event_fourkings.c
 *	@brief  �l�V�������@���o
 *	@author	tomoya takahashi
 *	@date		2009.11.21
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "gflib.h"

#include "system/ica_anime.h"
#include "system/ica_camera.h"

#include "sound/pm_sndsys.h"

#include "savedata/mystatus.h"

#include "arc/arc_def.h"
#include "arc/fourkings_scene.naix"

#include "field/field_const.h"

#include "event_fourkings.h"

#include "fieldmap.h"
#include "fldmmdl.h"
#include "field_camera.h"
#include "fieldmap_func.h"

#include "field_fk_sound_anime.h"

#include "field_sound.h"


#ifdef PM_DEBUG

#define DEBUG_FRAME_CONTROL // �t���[���̃R�}����Ȃǂ��s��

#endif

#ifdef DEBUG_FRAME_CONTROL

static BOOL s_DEBUG_FRAME_CONTROL_FLAG = 0;

#endif


//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
enum
{
  SEQ_INIT00,
  SEQ_INIT01,
  SEQ_INIT02,
  SEQ_UPDATE,
  SEQ_EXIT,
};


// �A�j��
enum
{
  HERO_ANIME_ICA,
  HERO_ANIME_ITP,

  HERO_ANIME_MAX,
};

// �X�g���[�~���O�Đ�
#define EV_CAMERA_ANIME_STREAMING_INTERVAL  ( 10 )
#define EV_SE_ANIME_STREAMING_INTERVAL  ( 10 )

// �e���\�[�X�h�c�擾�}�N��
#define EV_CAMERA_ANIME_GETDATA_ID( x )   ( NARC_fourkings_scene_c09_01_camera_bin + ((x)) )
#define EV_HERO_M_GETDATA_ID( x )   ( NARC_fourkings_scene_c09_01_m_nsbmd + ((x) * 6) )
#define EV_HERO_F_GETDATA_ID( x )   ( NARC_fourkings_scene_c09_01_f_nsbmd + ((x) * 6) )
#define EV_HERO_ANIME_GETDATA_ID( x )   ( NARC_fourkings_scene_c09_01_mf_nsbca + ((x) * 6) )
#define EV_HERO_M_ITPANIME_GETDATA_ID( x )   ( NARC_fourkings_scene_c09_01_m_nsbtp + ((x) * 6) )
#define EV_HERO_F_ITPANIME_GETDATA_ID( x )   ( NARC_fourkings_scene_c09_01_f_nsbtp + ((x) * 6) )
#define EV_SE_ANIME_GETDATA_ID( x )   ( NARC_fourkings_scene_c09_01_se_se + ((x)) )

// �A�j���[�V�����X�s�[�h
#define EV_CAMERA_ANIME_SPEED (FX32_ONE)

// ���@�f����o���ʒu
#define EV_HERO_WALK_END_GRID_X (15)
#define EV_HERO_WALK_END_GRID_Y (0)
#define EV_HERO_WALK_END_GRID_Z (16)
static const VecFx32 EV_DEMO_HERO_WALK_END_GRID[ FIELD_EVENT_FOURKINGS_MAX ] = 
{
  { EV_HERO_WALK_END_GRID_X, EV_HERO_WALK_END_GRID_Y, 13 }, // �S�[�X�g
  { EV_HERO_WALK_END_GRID_X, EV_HERO_WALK_END_GRID_Y, EV_HERO_WALK_END_GRID_Z },  // ����
  { EV_HERO_WALK_END_GRID_X, EV_HERO_WALK_END_GRID_Y, 18 },  // �����Ƃ�
  { EV_HERO_WALK_END_GRID_X-1, EV_HERO_WALK_END_GRID_Y, EV_HERO_WALK_END_GRID_Z }, // �G�X�p�[�����P�����
};

// �o�Ă������
static const u16 EV_DEMO_HERO_WALK_END_DIR[] = 
{
  DIR_UP,
  DIR_RIGHT,
  DIR_UP,
  DIR_UP,
};

// ���o�̒��S
//static const VecFx32 EV_DEMO_CenterVec = { GRID_TO_FX32(16), 0, GRID_TO_FX32(10) };
static const VecFx32 EV_DEMO_CenterVec = { GRID_TO_FX32(16), 0, GRID_TO_FX32(16) };



// �z�u�A�j���[�V�����̗L��
static const BOOL EV_DEMO_HAICHI_ANIME[ FIELD_EVENT_FOURKINGS_MAX ] = 
{
  FALSE,
  FALSE,
  TRUE,
  TRUE,
};
static const VecFx32 EV_DEMO_HAICHI_ANIME_POS[ FIELD_EVENT_FOURKINGS_MAX ] = 
{
  {0,0,0},
  {0,0,0},
  {GRID_TO_FX32(24), 0, GRID_TO_FX32(16)},
  {GRID_TO_FX32(16), 0, GRID_TO_FX32(16)},
};


// SE�A�j���[�V�����̗L��
static const BOOL EV_DEMO_SE_ANIME[ FIELD_EVENT_FOURKINGS_MAX ] = 
{
  TRUE,
  TRUE,
  TRUE,
  TRUE,
};
static const FIELD_FK_SOUND_ANIME_DATA EV_DEMO_SE_DATA[ FIELD_EVENT_FOURKINGS_MAX ] = 
{
  // 
  {
    {
      SEQ_SE_FLD_113, // ��������ʂɏo��
      SEQ_SE_FLD_114, // ���t��������
      SEQ_SE_FLD_115, // ���������U
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
    },
  },

  // 
  {
    {
      SEQ_SE_FLD_104, // �낤�����_��
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
    },
  },
  
  // 
  {
    {
      SEQ_SE_FLD_105, // ��ԏ��LOOP
      SEQ_SE_FLD_106, // �X�`�[��
      SEQ_SE_FLD_117, // ��Ԃɏ����
      SEQ_SE_FLD_118, // ������
      SEQ_SE_FLD_119, // �Q�[�g���J��
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
    },
  },

  // 
  {
    {
      SEQ_SE_FLD_110, // ��
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
    },
  },
};


// ����J����ID
static const u32 sc_EV_DEMO_CAMERA_ID_TBL[ FIELD_EVENT_FOURKINGS_MAX ] = 
{
  28,
  29,
  30,
  31
};

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	�J��������
//=====================================
typedef struct {
  ICA_ANIME* p_anime;
  GFL_G3D_CAMERA* p_dummy_camera;
  FIELD_CAMERA* p_camera;
  u16 camera_mode_save;
  u16 camera_area_save;
  VecFx32 camera_target_offset_save;
  VecFx32 camera_up_save;

  fx32 frame;

  u32 camera_type;
} EV_CIRCLEWALK_CAMERA;

//-------------------------------------
///	���@�_�~�[����
//=====================================
typedef struct {
  // ���\�[�X
  GFL_G3D_RES* p_mdlres;
  GFL_G3D_RES* p_anmres[HERO_ANIME_MAX];
  BOOL trans;

  // �Ǘ��I�u�W�F
  GFL_G3D_RND* p_rend;
  GFL_G3D_ANM* p_anm[HERO_ANIME_MAX];
  GFL_G3D_OBJ* p_obj;
  GFL_G3D_OBJSTATUS status;
  BOOL draw;

  // �l��
  const MYSTATUS* cp_mystatus;
  FIELD_PLAYER* p_player;
  MMDL* p_player_core;
  VecFx32 end_gridpos;
  u16 end_dir;
  
  int frame;
} EV_CIRCLEWALK_HERO;

//-------------------------------------
///	SE����
//=====================================
typedef struct {
  FIELD_FK_SOUND_ANIME* p_anime;
} EV_CIRCLEWALK_SE;



//-------------------------------------
///	�z�u�A�j���[�V�������[�N
//=====================================
typedef struct {
  
  // 
  FIELD_BMODEL* p_model;
  G3DMAPOBJST * p_objst;

  FIELD_BMODEL_MAN* p_modelman;

  
} EV_BMODEL_ANIME;


//-------------------------------------
///	DRAW_TASK���[�N
//=====================================
typedef struct 
{
  EV_CIRCLEWALK_HERO* p_hero;
} EV_CIRCLEWALK_DRAWWK;


//-------------------------------------
///	�C�x���g���[�N
//=====================================
typedef struct 
{
  FIELDMAP_WORK* p_fieldmap;
  GAMESYS_WORK* p_gsys;

  // �l�V���i���o�[
  u32 fourkings_no;
  
  // �J��������
  EV_CIRCLEWALK_CAMERA camera;

  // ���@�̃_�~�[����
  EV_CIRCLEWALK_HERO hero;

  // ���f���A�j���[�V����
  EV_BMODEL_ANIME bmodel;

  // SE
  EV_CIRCLEWALK_SE  se;

  // �`��^�X�N
  FLDMAPFUNC_WORK* p_drawtask;

  // VBlank TCB
  GFL_TCB * p_vintr;
} EV_CIRCLEWALK_WORK;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

static fx32 G3DANM_GetFrameMax( const GFL_G3D_ANM* cp_anm );

static void VBLANK_Update( GFL_TCB * p_tcb, void* p_work );

//-------------------------------------
///	�`��^�X�N
//=====================================
static void DRAW_Update( FLDMAPFUNC_WORK* p_taskwork, FIELDMAP_WORK* p_fieldmap , void* p_work );
static const FLDMAPFUNC_DATA sc_DRAWTASK_DATA = 
{
  0,
  sizeof(EV_CIRCLEWALK_DRAWWK),
  NULL,
  NULL,
  NULL,
  DRAW_Update,
};


//-------------------------------------
///	�J��������
//=====================================
static void EV_CAMERA_Init( EV_CIRCLEWALK_CAMERA* p_wk, FIELD_CAMERA* p_camera, HEAPID heapID, u32 fourkings_no );
static void EV_CAMERA_Exit( EV_CIRCLEWALK_CAMERA* p_wk );
static BOOL EV_CAMERA_Update( EV_CIRCLEWALK_CAMERA* p_wk );
static void EV_CAMERA_SetUpParam( EV_CIRCLEWALK_CAMERA* p_wk );


//-------------------------------------
/// �l�����쓮��
//=====================================
static void EV_HERO_Init0( EV_CIRCLEWALK_HERO* p_wk, FIELD_PLAYER * p_fld_player, const MYSTATUS* cp_mystatus, HEAPID heapID, u32 fourkings_no );
static void EV_HERO_Init1( EV_CIRCLEWALK_HERO* p_wk );
static void EV_HERO_Exit( EV_CIRCLEWALK_HERO* p_wk );
static BOOL EV_HERO_Update( EV_CIRCLEWALK_HERO* p_wk );
static void EV_HERO_Switch( EV_CIRCLEWALK_HERO* p_wk, BOOL sw );
static void EV_HERO_VBlank( EV_CIRCLEWALK_HERO* p_wk );
static void EV_HERO_Draw( EV_CIRCLEWALK_HERO* p_wk );


//-------------------------------------
///	�C�x���g�����֐��E�C�x���g�֐�
//=====================================
static GMEVENT* EVENT_SetUp_CircleWalk( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap, u32 fourkings_no );
static GMEVENT_RESULT EVENT_CircleWalk( GMEVENT* p_event, int* p_seq, void* p_wk );


//-------------------------------------
/// �z�u�I�u�W�F�̃A�j���[�V����
//=====================================
static void EV_BMODEL_Init( EV_BMODEL_ANIME* p_wk, FIELD_BMODEL_MAN * bmodel_man, u32 fourkings_no );
static void EV_BMODEL_Exit( EV_BMODEL_ANIME* p_wk );
static void EV_BMODEL_Start( EV_BMODEL_ANIME* p_wk );
static BOOL EV_BMODEL_IsEnd( const EV_BMODEL_ANIME* cp_wk );


//-------------------------------------
/// SE�A�j���[�V����
//=====================================
static void EV_SE_ANIME_Init( EV_CIRCLEWALK_SE* p_wk, u32 fourkings_no, HEAPID heapID );
static void EV_SE_ANIME_Exit( EV_CIRCLEWALK_SE* p_wk );
static BOOL EV_SE_ANIME_Update( EV_CIRCLEWALK_SE* p_wk );


//----------------------------------------------------------------------------
/**
 *	@brief  �����Ă����鉉�o
 *
 *	@param	p_parent          �e�̃C�x���g
 *	@param	p_gsys            �Q�[���f�[�^
 *	@param	p_fieldmap        �t�B�[���h�f�[�^
 *	@param	fourkins_no       �l�V���i���o�[
 */
//-----------------------------------------------------------------------------
GMEVENT* EVENT_FourKings_CircleWalk( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap, u32 fourkins_no )
{
  GF_ASSERT( FIELD_EVENT_FOURKINGS_MAX > fourkins_no );
  return EVENT_SetUp_CircleWalk(p_gsys, p_fieldmap, fourkins_no);
}



//----------------------------------------------------------------------------
/**
 *	@brief  �l�V�����������̒���J����ID���擾
 *
 *	@param	fourkins_no   �l�V���i���o�[
 *
 *	@return �J����ID
 */
//-----------------------------------------------------------------------------
u32 EVENT_FourKings_GetCameraID( u32 fourkins_no )
{
  GF_ASSERT( fourkins_no < FIELD_EVENT_FOURKINGS_MAX );
  return sc_EV_DEMO_CAMERA_ID_TBL[ fourkins_no ];
}



//-----------------------------------------------------------------------------
/**
 *        private�֐�
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief  �A�j���[�V�����̃t���[���ő吔���擾����
 *
 *	@param	cp_anm  �A�j���[�V�����I�u�W�F
 *
 *	@return �t���[���ő吔
 */
//-----------------------------------------------------------------------------
static fx32 G3DANM_GetFrameMax( const GFL_G3D_ANM* cp_anm )
{
  const NNSG3dAnmObj* cp_anm_core = GFL_G3D_ANIME_GetAnmObj( cp_anm ); 
  return NNS_G3dAnmObjGetNumFrame( cp_anm_core );
}


//----------------------------------------------------------------------------
/**
 *	@brief  VBLANK�^�X�N
 */
//-----------------------------------------------------------------------------
static void VBLANK_Update( GFL_TCB * p_tcb, void* p_work )
{
  EV_CIRCLEWALK_WORK * p_wk = p_work;
  
  // �e�N�X�`���]��
  EV_HERO_VBlank( &p_wk->hero );
}



//----------------------------------------------------------------------------
/**
 *	@brief  �l�V�������@�~�`�ړ��f���C�x���g�@����
 *
 *	@param	p_gsys        �Q�[���V�X�e��
 *	@param	fieldmap      �t�B�[���h�}�b�v
 *
 *	@return �C�x���g
 */
//-----------------------------------------------------------------------------
static GMEVENT* EVENT_SetUp_CircleWalk( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap, u32 fourkings_no )
{
  GMEVENT*              p_event;
  EV_CIRCLEWALK_WORK*   p_work;
  HEAPID heapID = FIELDMAP_GetHeapID( p_fieldmap );

  // �C�x���g����
  p_event = GMEVENT_Create( p_gsys, NULL, EVENT_CircleWalk, sizeof( EV_CIRCLEWALK_WORK ) );

  // �C�x���g���[�N��������
  p_work              = GMEVENT_GetEventWork( p_event );
  p_work->p_fieldmap  = p_fieldmap;
  p_work->p_gsys      = p_gsys;
  p_work->fourkings_no= fourkings_no;


  // VBlank�^�X�N�o�^
  p_work->p_vintr = GFUser_VIntr_CreateTCB( VBLANK_Update, p_work, 0 );


  // ���������C�x���g��Ԃ�
  return p_event;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �~�`�ړ��f���@�C�x���g
 *
 *	@param	p_event     �C�x���g���[�N
 *	@param	p_seq       �V�[�P���X
 *	@param	p_wk        ���[�N
 *
 *  @retval GMEVENT_RES_CONTINUE = 0, ///<�C�x���g�p����
 *  @retval GMEVENT_RES_FINISH,       ///<�C�x���g�I��
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_CircleWalk( GMEVENT* p_event, int* p_seq, void* p_wk )
{
  EV_CIRCLEWALK_WORK*   p_work = p_wk;
  HEAPID heapID = FIELDMAP_GetHeapID( p_work->p_fieldmap );

  switch( *p_seq )
  {
  case SEQ_INIT00:
    // �������c�ʂ̕\��
    OS_Printf( "init start heap free size 0x%x\n", GFI_HEAP_GetHeapFreeSize(heapID) );
    {
      GAMEDATA* p_gdata = GAMESYSTEM_GetGameData( p_work->p_gsys );
      MYSTATUS* p_mystatus = GAMEDATA_GetMyStatus( p_gdata );
      FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( p_work->p_fieldmap );

      EV_HERO_Init0( &p_work->hero, p_player, p_mystatus, heapID, p_work->fourkings_no );
    }
    (*p_seq)++;
    break;

  case SEQ_INIT01:
    {
      FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_work->p_fieldmap ); 
      FIELD_CAMERA_StopTraceRequest( p_camera );
    }
    (*p_seq)++;
    break;

  case SEQ_INIT02:
    {
      FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_work->p_fieldmap ); 

      if( FIELD_CAMERA_CheckTrace(p_camera) )
      {
        break;
      }

      EV_CAMERA_Init( &p_work->camera, p_camera, heapID, p_work->fourkings_no );
      
    }

    // SE
    {
      EV_SE_ANIME_Init( &p_work->se, p_work->fourkings_no, heapID );
    }
    
    // �z�u���f��
    {
      FLDMAPPER* p_mapper = FIELDMAP_GetFieldG3Dmapper( p_work->p_fieldmap );
      FIELD_BMODEL_MAN* p_bmodelman = FLDMAPPER_GetBuildModelManager( p_mapper );

      EV_BMODEL_Init( &p_work->bmodel, p_bmodelman, p_work->fourkings_no );
    }
    EV_HERO_Init1( &p_work->hero );

    // �`��J�n
    {
      EV_CIRCLEWALK_DRAWWK* p_drawwk;
      p_work->p_drawtask = FLDMAPFUNC_Create( FIELDMAP_GetFldmapFuncSys(p_work->p_fieldmap), &sc_DRAWTASK_DATA );
      p_drawwk = FLDMAPFUNC_GetFreeWork( p_work->p_drawtask );
      p_drawwk->p_hero = &p_work->hero;
    }

    // �`��X�C�b�`
    EV_HERO_Switch( &p_work->hero, TRUE );

    // �z�u�A�j���[�V�����J�n
    EV_BMODEL_Start( &p_work->bmodel ); 
    
    // �������c�ʂ̕\��
    OS_Printf( "init end heap free size 0x%x\n", GFI_HEAP_GetHeapFreeSize(heapID) );
    
    (*p_seq)++;
    break;

  case SEQ_UPDATE:
    {
      BOOL result1;
      BOOL result2;
      BOOL result3;
      BOOL result4;

#ifdef DEBUG_FRAME_CONTROL
      if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT )
      {
        s_DEBUG_FRAME_CONTROL_FLAG ^= TRUE;
      }
#endif

      result1 = EV_CAMERA_Update( &p_work->camera );
      result2 = EV_HERO_Update( &p_work->hero );
      result4 = EV_SE_ANIME_Update( &p_work->se );
      GF_ASSERT( (result1 == result2) );

      result3 = EV_BMODEL_IsEnd( &p_work->bmodel );

      if( result1 && result3 && result4 )
      {
        (*p_seq) ++;
      }
    }
    break;

  case SEQ_EXIT:
    // �`��X�C�b�`
    EV_HERO_Switch( &p_work->hero, FALSE );

    // �`��I��
    FLDMAPFUNC_Delete( p_work->p_drawtask );

    // TCB�j��
    GFL_TCB_DeleteTask( p_work->p_vintr );
    
    // �e�f���j��
    EV_HERO_Exit( &p_work->hero );
    EV_SE_ANIME_Exit( &p_work->se );
    EV_CAMERA_Exit( &p_work->camera );
    EV_BMODEL_Exit( &p_work->bmodel );
    return GMEVENT_RES_FINISH;

  default:
    GF_ASSERT(0);
    break;
  }

  return GMEVENT_RES_CONTINUE;
}



//----------------------------------------------------------------------------
/**
 *	@brief  �z�u���f���̃A�j���[�V����������
 *
 *	@param	p_wk              ���[�N
 *	@param	bmodel_man        �z�u�Ǘ��V�X�e��
 *	@param	fourkings_no      �l�V���i���o�[
 */
//-----------------------------------------------------------------------------
static void EV_BMODEL_Init( EV_BMODEL_ANIME* p_wk, FIELD_BMODEL_MAN * bmodel_man, u32 fourkings_no )
{
  if( EV_DEMO_HAICHI_ANIME[ fourkings_no ] )
  {
    p_wk->p_model = FIELD_BMODEL_Create_Search( bmodel_man, BM_SEARCH_ID_NULL, &EV_DEMO_HAICHI_ANIME_POS[fourkings_no] );
    p_wk->p_objst = FIELD_BMODEL_MAN_SearchObjStatusPos( bmodel_man, BM_SEARCH_ID_NULL, &EV_DEMO_HAICHI_ANIME_POS[fourkings_no] );
    p_wk->p_modelman = bmodel_man;
  }
  else
  {
    p_wk->p_model = NULL;
    p_wk->p_objst = NULL;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �j��
 *
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
static void EV_BMODEL_Exit( EV_BMODEL_ANIME* p_wk )
{
  if( p_wk->p_model )
  {
    FIELD_BMODEL_Delete( p_wk->p_model );

    // ��{���������Ă���
    G3DMAPOBJST_setAnime( p_wk->p_modelman, p_wk->p_objst, 0, BMANM_REQ_REVERSE_START );
    G3DMAPOBJST_setAnime( p_wk->p_modelman, p_wk->p_objst, 0, BMANM_REQ_STOP );
    
    p_wk->p_model = NULL;
    p_wk->p_objst = NULL;
    p_wk->p_modelman = NULL;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �A�j���[�V�����̊J�n
 *
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
static void EV_BMODEL_Start( EV_BMODEL_ANIME* p_wk )
{
  if( p_wk->p_model )
  {
    FIELD_BMODEL_SetAnime( p_wk->p_model, 0, BMANM_REQ_START );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �A�j���[�V�����̊����X
 *
 *	@param	cp_wk   ���[�N
 *
 *	@retval TRUE  ����
 *	@retval FALSE �r��
 */
//-----------------------------------------------------------------------------
static BOOL EV_BMODEL_IsEnd( const EV_BMODEL_ANIME* cp_wk )
{
  if( cp_wk->p_model )
  {
    return FIELD_BMODEL_WaitAnime( cp_wk->p_model, 0 );
  }
  return TRUE;
}



//----------------------------------------------------------------------------
/**
 *	@brief  �J�������[�N  ������
 *
 *	@param	p_wk        ���[�N
 *	@param	p_camera    �J����
 *	@param	heapID      �q�[�v�h�c
 *	@param  fourkins_no �l�V���i���o�[
 */
//-----------------------------------------------------------------------------
static void EV_CAMERA_Init( EV_CIRCLEWALK_CAMERA* p_wk, FIELD_CAMERA* p_camera, HEAPID heapID, u32 fourkings_no )
{
  p_wk->frame = 0;
  
  // �A�j���[�V�����ǂݍ���
  p_wk->p_anime = ICA_ANIME_CreateStreamingAlloc( heapID, ARCID_FOURKINGS_SCENE, EV_CAMERA_ANIME_GETDATA_ID(fourkings_no), EV_CAMERA_ANIME_STREAMING_INTERVAL );

  // �J�����쐬
  {
    VecFx32 target, pos;
    const GFL_G3D_CAMERA* cp_camera = FIELD_CAMERA_GetCameraPtr( p_camera );
    GFL_G3D_CAMERA_GetTarget( cp_camera, &target );
    GFL_G3D_CAMERA_GetPos( cp_camera, &pos );
    p_wk->p_dummy_camera = GFL_G3D_CAMERA_CreateDefault( &pos, &target, heapID );
  }

  // �f�[�^���f��t�B�[���h�J����
  p_wk->p_camera = p_camera;
  // ���n�w�胂�[�h
  p_wk->camera_mode_save = FIELD_CAMERA_GetMode( p_wk->p_camera );
  FIELD_CAMERA_ChangeMode( p_wk->p_camera, FIELD_CAMERA_MODE_DIRECT_POS );

  // �J�����G���A�ݒ�n�e�e
  p_wk->camera_area_save = FIELD_CAMERA_GetCameraAreaActive( p_wk->p_camera );
  FIELD_CAMERA_SetCameraAreaActive( p_wk->p_camera, FALSE );

  // �^�[�Q�b�g�̃o�C���h��OFF
  FIELD_CAMERA_FreeTarget( p_wk->p_camera );

  // �J�������W�̃o�C���h���Ȃ����`�F�b�N
  GF_ASSERT( FIELD_CAMERA_IsBindCamera(p_wk->p_camera) == FALSE );

  // �^�[�Q�b�g�I�t�Z�b�g���Z�[�u
  {
    VecFx32 clear = {0,0,0};
    FIELD_CAMERA_GetTargetOffset( p_wk->p_camera, &p_wk->camera_target_offset_save );
    FIELD_CAMERA_SetTargetOffset( p_wk->p_camera, &clear );
  }
  FIELD_CAMERA_GetCameraUp( p_wk->p_camera, &p_wk->camera_up_save );

  // �����p�����[�^�ݒ�
  EV_CAMERA_SetUpParam( p_wk );

  // ���A�J�����^�C�v
  p_wk->camera_type = sc_EV_DEMO_CAMERA_ID_TBL[ fourkings_no ];
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�������[�N  �j��
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void EV_CAMERA_Exit( EV_CIRCLEWALK_CAMERA* p_wk )
{
  // �^�[�Q�b�g�̃o�C���h��ON
  FIELD_CAMERA_BindDefaultTarget( p_wk->p_camera );
  FIELD_CAMERA_SetTargetPos( p_wk->p_camera, FIELD_CAMERA_GetWatchTarget(p_wk->p_camera) );
  
  // �J���������ɖ߂�
  FIELD_CAMERA_SetTargetOffset( p_wk->p_camera, &p_wk->camera_target_offset_save );
  FIELD_CAMERA_ChangeMode( p_wk->p_camera, p_wk->camera_mode_save );
  FIELD_CAMERA_SetCameraAreaActive( p_wk->p_camera, p_wk->camera_area_save );
  FIELD_CAMERA_SetCameraUp( p_wk->p_camera, &p_wk->camera_up_save );

  // ����J�������w��
  FIELD_CAMERA_SetCameraType( p_wk->p_camera, p_wk->camera_type );

  // �g���[�X���̏�����
  FIELD_CAMERA_RestartTrace( p_wk->p_camera );

  // �_�~�[�J�����j��
  GFL_G3D_CAMERA_Delete( p_wk->p_dummy_camera );

  // �A�j���[�V�����j��
  ICA_ANIME_Delete( p_wk->p_anime );

  // ���[�N�N���A
  GFL_STD_MemClear( p_wk, sizeof(EV_CIRCLEWALK_CAMERA) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J����  �A�b�v�f�[�g
 *
 *	@param	p_wk  ���[�N
 *
 *	@retval TRUE    �J�����A�j���[�V��������
 *	@retval FALSE   �J�����A�j���[�V�����r��
 */
//-----------------------------------------------------------------------------
static BOOL EV_CAMERA_Update( EV_CIRCLEWALK_CAMERA* p_wk )
{
  BOOL result;
  fx32 max_frame;

  max_frame = ICA_ANIME_GetMaxFrame( p_wk->p_anime )<<FX32_SHIFT;

#ifndef DEBUG_FRAME_CONTROL
  // ���łɊ������Ă��Ȃ����H
  if( max_frame <= (p_wk->frame + EV_CAMERA_ANIME_SPEED) )
  {
    result = TRUE;
    p_wk->frame = max_frame - FX32_ONE;
  }
  else
  {
    result = FALSE;
    p_wk->frame += EV_CAMERA_ANIME_SPEED;
  }
#else

  if( s_DEBUG_FRAME_CONTROL_FLAG == FALSE )
  {
    // ���łɊ������Ă��Ȃ����H
    if( max_frame <= (p_wk->frame + EV_CAMERA_ANIME_SPEED) )
    {
      result = TRUE;
      p_wk->frame = max_frame - FX32_ONE;
    }
    else
    {
      result = FALSE;
      p_wk->frame += EV_CAMERA_ANIME_SPEED;
    }
  }
  else
  {
    result = FALSE;
    if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_L )
    {
      if( (p_wk->frame - EV_CAMERA_ANIME_SPEED) >= 0 )
      {
        p_wk->frame -= EV_CAMERA_ANIME_SPEED;
      }
    }
    if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_R )
    {
      if( (p_wk->frame + EV_CAMERA_ANIME_SPEED) < max_frame )
      {
        p_wk->frame += EV_CAMERA_ANIME_SPEED;
      }
    }
  }
#endif


  // �A�j���[�V������i�߁A�J�������W��ݒ�
  ICA_ANIME_SetAnimeFrame( p_wk->p_anime, p_wk->frame );

  EV_CAMERA_SetUpParam( p_wk );
  
  return result;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �p�����[�^�ݒ�
 *
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
static void EV_CAMERA_SetUpParam( EV_CIRCLEWALK_CAMERA* p_wk )
{
  VecFx32 target, pos, up;
  
  // �J�����p�����[�^���擾
  ICA_CAMERA_SetCameraStatus( p_wk->p_dummy_camera, p_wk->p_anime );
  GFL_G3D_CAMERA_GetPos( p_wk->p_dummy_camera, &pos ); 
  GFL_G3D_CAMERA_GetTarget( p_wk->p_dummy_camera, &target ); 
  GFL_G3D_CAMERA_GetCamUp( p_wk->p_dummy_camera, &up );

  // ���S�����s�ړ�
  VEC_Add( &pos, &EV_DEMO_CenterVec, &pos );
  VEC_Add( &target, &EV_DEMO_CenterVec, &target );

  // ���W���t�B�[���h�J�����ɐݒ�
  FIELD_CAMERA_SetCameraPos( p_wk->p_camera, &pos ); 
  FIELD_CAMERA_SetTargetPos( p_wk->p_camera, &target ); 
  FIELD_CAMERA_SetCameraUp( p_wk->p_camera, &up ); 
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���@�_�~�[  ������
 *
 *	@param	p_wk          ���[�N
 *	@param	cp_mystatus   ���@���
 *	@param	heapID        �q�[�v�h�c
 *	@param	fourkings_no  �l�V���i���o�[
 */
//-----------------------------------------------------------------------------
static void EV_HERO_Init0( EV_CIRCLEWALK_HERO* p_wk, FIELD_PLAYER * p_fld_player, const MYSTATUS* cp_mystatus, HEAPID heapID, u32 fourkings_no )
{
  u32 sex;
  ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_FOURKINGS_SCENE, GFL_HEAP_LOWID(heapID) );
  BOOL result;

  // �t���[���̏�����
  p_wk->frame = 0;

  // �ŏ��͔�\��
  p_wk->draw = FALSE;

  //  �I����o���ʒu
  p_wk->end_gridpos = EV_DEMO_HERO_WALK_END_GRID[ fourkings_no ];
  p_wk->end_dir = EV_DEMO_HERO_WALK_END_DIR[ fourkings_no ];
  
  //�@�ۑ�
  p_wk->cp_mystatus   = cp_mystatus;
  p_wk->p_player      = p_fld_player;
  p_wk->p_player_core = FIELD_PLAYER_GetMMdl( p_wk->p_player );

  // ���W
  p_wk->status.trans = EV_DEMO_CenterVec;
  VEC_Set( &p_wk->status.scale, FX32_ONE, FX32_ONE, FX32_ONE );
  MTX_Identity33( &p_wk->status.rotate );

  // ���ʎ擾
  sex = MyStatus_GetMySex( cp_mystatus );
  GF_ASSERT( sex != PM_NEUTRAL );
  
  // �R�c���f���̓ǂݍ���
  if( sex == PM_MALE )
  {
    p_wk->p_mdlres = GFL_G3D_CreateResourceHandle( p_handle, EV_HERO_M_GETDATA_ID(fourkings_no) );
    p_wk->p_anmres[HERO_ANIME_ITP] = GFL_G3D_CreateResourceHandle( p_handle, EV_HERO_M_ITPANIME_GETDATA_ID(fourkings_no) );
  }
  else
  {
    p_wk->p_mdlres = GFL_G3D_CreateResourceHandle( p_handle, EV_HERO_F_GETDATA_ID(fourkings_no) );
    p_wk->p_anmres[HERO_ANIME_ITP] = GFL_G3D_CreateResourceHandle( p_handle, EV_HERO_F_ITPANIME_GETDATA_ID(fourkings_no) );
  }
  p_wk->p_anmres[HERO_ANIME_ICA] = GFL_G3D_CreateResourceHandle( p_handle, EV_HERO_ANIME_GETDATA_ID(fourkings_no) );


  // VRAMKEY�̎擾
  result = GFL_G3D_AllocVramTexture( p_wk->p_mdlres );
  GF_ASSERT( result );
  p_wk->trans = TRUE;

  GFL_ARC_CloseDataHandle( p_handle );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �q�[���[�������@2�t���[����
 *
 *	@param	p_wk          ���[�N
 */
//-----------------------------------------------------------------------------
static void EV_HERO_Init1( EV_CIRCLEWALK_HERO* p_wk )
{
  // �����_�[�I�u�W�F����
  p_wk->p_rend = GFL_G3D_RENDER_Create( p_wk->p_mdlres, 0, p_wk->p_mdlres );

  // �A�j���[�V�����I�u�W�F����
  p_wk->p_anm[HERO_ANIME_ICA] = GFL_G3D_ANIME_Create( p_wk->p_rend, p_wk->p_anmres[HERO_ANIME_ICA], 0 );
  p_wk->p_anm[HERO_ANIME_ITP] = GFL_G3D_ANIME_Create( p_wk->p_rend, p_wk->p_anmres[HERO_ANIME_ITP], 0 );

  // �\���I�u�W�F����
  p_wk->p_obj = GFL_G3D_OBJECT_Create( p_wk->p_rend, p_wk->p_anm, HERO_ANIME_MAX );

  {
    int frame = 0;
    int i;
    for( i=0; i<HERO_ANIME_MAX; i++ )
    {
      GFL_G3D_OBJECT_SetAnimeFrame( p_wk->p_obj, i, &frame );
      GFL_G3D_OBJECT_EnableAnime( p_wk->p_obj, i );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���@�_�~�[�@�j��
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void EV_HERO_Exit( EV_CIRCLEWALK_HERO* p_wk )
{
  // �S���j��
  GFL_G3D_OBJECT_Delete( p_wk->p_obj );
  GFL_G3D_ANIME_Delete( p_wk->p_anm[HERO_ANIME_ICA] );
  GFL_G3D_ANIME_Delete( p_wk->p_anm[HERO_ANIME_ITP] );
  GFL_G3D_RENDER_Delete( p_wk->p_rend );

  //���\�[�X
  GFL_G3D_FreeVramTexture( p_wk->p_mdlres );
  GFL_G3D_DeleteResource( p_wk->p_mdlres );
  GFL_G3D_DeleteResource( p_wk->p_anmres[HERO_ANIME_ICA] );
  GFL_G3D_DeleteResource( p_wk->p_anmres[HERO_ANIME_ITP] );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���@�_�~�[�@�f���X�V
 *
 *	@param	p_wk  ���[�N
 *
 *	@retval TRUE    �f������
 *	@retval FALSE   �f���r��
 */
//-----------------------------------------------------------------------------
static BOOL EV_HERO_Update( EV_CIRCLEWALK_HERO* p_wk )
{
  BOOL result;
  fx32 max_frame;

  max_frame = G3DANM_GetFrameMax( p_wk->p_anm[HERO_ANIME_ICA] );

#ifndef DEBUG_FRAME_CONTROL
  // ���łɊ������Ă��Ȃ����H
  if( max_frame <= (p_wk->frame + EV_CAMERA_ANIME_SPEED) )
  {
    result = TRUE;
    p_wk->frame = max_frame - FX32_ONE;
  }
  else
  {
    result = FALSE;
    p_wk->frame += EV_CAMERA_ANIME_SPEED;
  }
#else

  if( s_DEBUG_FRAME_CONTROL_FLAG == FALSE )
  {
    // ���łɊ������Ă��Ȃ����H
    if( max_frame <= (p_wk->frame + EV_CAMERA_ANIME_SPEED) )
    {
      result = TRUE;
      p_wk->frame = max_frame - FX32_ONE;
    }
    else
    {
      result = FALSE;
      p_wk->frame += EV_CAMERA_ANIME_SPEED;
    }
  }
  else
  {
    result = FALSE;
    if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_L )
    {
      if( (p_wk->frame - EV_CAMERA_ANIME_SPEED) >= 0 )
      {
        p_wk->frame -= EV_CAMERA_ANIME_SPEED;
      }
    }
    if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_R )
    {
      if( (p_wk->frame + EV_CAMERA_ANIME_SPEED) < max_frame )
      {
        p_wk->frame += EV_CAMERA_ANIME_SPEED;
      }
    }
  }
#endif

  //�@�A�j���[�V�����������߂�
  GFL_G3D_OBJECT_SetAnimeFrame( p_wk->p_obj, HERO_ANIME_ICA, &p_wk->frame );
  GFL_G3D_OBJECT_SetAnimeFrame( p_wk->p_obj, HERO_ANIME_ITP, &p_wk->frame );

  return result;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �_�~�[�Ǝ��@�̕\�����X�C�b�`
 *
 *	@param	p_wk  ���[�N
 *	@param	sw    �X�C�b�`    TRUE:�_�~�[��\��
 */
//-----------------------------------------------------------------------------
static void EV_HERO_Switch( EV_CIRCLEWALK_HERO* p_wk, BOOL sw )
{
  if(sw)
  {
    p_wk->draw = TRUE;
    MMDL_SetStatusBitVanish( p_wk->p_player_core, TRUE );
  }
  else
  {
    VecFx32 vecpos;
    
    p_wk->draw = FALSE;
    MMDL_SetStatusBitVanish( p_wk->p_player_core, FALSE );

    MMDL_InitGridPosition(
      p_wk->p_player_core, 
      p_wk->end_gridpos.x, p_wk->end_gridpos.y, p_wk->end_gridpos.z, 
      p_wk->end_dir );

    MMDL_UpdateCurrentHeight( p_wk->p_player_core );

    MMDL_GetVectorPos( p_wk->p_player_core, &vecpos );
    FIELD_PLAYER_SetPos( p_wk->p_player, &vecpos );

    // �ʂ肱��OFF
    MMDL_SetMoveBitReflect( p_wk->p_player_core, FALSE );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���@�_�~�[�@VBlank
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void EV_HERO_VBlank( EV_CIRCLEWALK_HERO* p_wk )
{
  if( p_wk->trans )
  {
    GFL_G3D_TransOnlyTexture( p_wk->p_mdlres );
    p_wk->trans = FALSE;
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  ��l���̕\��
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void EV_HERO_Draw( EV_CIRCLEWALK_HERO* p_wk )
{
  GFL_G3D_DRAW_DrawObject(  p_wk->p_obj, &p_wk->status );
}




//----------------------------------------------------------------------------
/**
 *	@brief  �`��A�b�v�f�[�g
 *
 *	@param	p_taskwork    �^�X�N���[�N
 *	@param	p_fieldmap    �t�B�[���h�}�b�v
 *	@param	p_work        ���[�N
 */
//-----------------------------------------------------------------------------
static void DRAW_Update( FLDMAPFUNC_WORK* p_taskwork, FIELDMAP_WORK* p_fieldmap , void* p_work )
{
  EV_CIRCLEWALK_DRAWWK* p_wk = p_work; 

  EV_HERO_Draw( p_wk->p_hero );
}



//----------------------------------------------------------------------------
/**
 *	@brief  SE�Đ��A�j���[�V�����̏�����
 *
 *	@param	p_wk            ���[�N
 *	@param	fourkings_no    �l�V��ID
 *	@param	heapID          �q�[�vID
 */
//-----------------------------------------------------------------------------
static void EV_SE_ANIME_Init( EV_CIRCLEWALK_SE* p_wk, u32 fourkings_no, HEAPID heapID )
{
  if( EV_DEMO_SE_ANIME[fourkings_no] )
  {
    // �A�j���[�V�����ǂݍ���
    p_wk->p_anime = FIELD_FK_SOUND_ANIME_Create( &EV_DEMO_SE_DATA[ fourkings_no ],ARCID_FOURKINGS_SCENE, EV_SE_ANIME_GETDATA_ID(fourkings_no), EV_SE_ANIME_STREAMING_INTERVAL, heapID );
  }
  else
  {
    GFL_STD_MemClear( p_wk, sizeof(EV_CIRCLEWALK_SE) );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  SE�Đ��A�j���[�V�����̔j��
 *
 *	@param	p_wk        ���[�N
 */ 
//-----------------------------------------------------------------------------
static void EV_SE_ANIME_Exit( EV_CIRCLEWALK_SE* p_wk )
{
  if( p_wk->p_anime )
  {
    FIELD_FK_SOUND_ANIME_Delete( p_wk->p_anime );
  }
  GFL_STD_MemClear( p_wk, sizeof(EV_CIRCLEWALK_SE) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  SE�Đ��A�j���[�V�����̃A�b�v�f�[�g
 *
 *	@param	p_wk  ���[�N
 *
 *	@retval TRUE  ����
 *	@retval FALSE �Đ���
 */
//-----------------------------------------------------------------------------
static BOOL EV_SE_ANIME_Update( EV_CIRCLEWALK_SE* p_wk )
{
  BOOL result;

  result = TRUE;
  if( p_wk->p_anime )
  {
    result = FIELD_FK_SOUND_ANIME_Update( p_wk->p_anime );  
  }
  return result;
}








//-----------------------------------------------------------------------------
/**
 *      �����p�̃V�X�e��
 */
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
#define GHOST_SPARK_SOUND_ANM_INDEX (1)

// �ʒu
static const VecFx32 sc_GHOST_SPARK_SARCH_POS = 
{
  FX32_CONST(256), 0, FX32_CONST(240)
};

// ���̃^�C�~���O
//static fx32 sc_GHOST_SPARK_SOUND_TIMING[] = {
static const fx32 sc_GHOST_SPARK_SOUND_TIMING[] = {
  FX32_CONST(6), 
};

#define GHOST_SPARK_SOUND_TIMING_TBL_MAX  (NELEMS(sc_GHOST_SPARK_SOUND_TIMING))


// �T�E���hID
#define GHOST_SND_SE_SPARK      ( SEQ_SE_FLD_33 )
#define GHOST_SND_SE_SPARK_SML  ( SEQ_SE_FLD_34 )

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	SPARK SOUND
//=====================================
typedef struct 
{
  FIELD_BMODEL_MAN* p_man;
  const G3DMAPOBJST* cp_spark_obj;

  fx32 next_frame;
  int tbl_index;
} GHOST_SPARK_SOUND;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

static void GHOST_SPARK_SOUND_Create(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work );
static void GHOST_SPARK_SOUND_Delete(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work );
static void GHOST_SPARK_SOUND_Update(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work );


static const FLDMAPFUNC_DATA sc_FLDMAPFUNC_GHOST_SPARK_SOUND = 
{
  128,
  sizeof(GHOST_SPARK_SOUND),
  GHOST_SPARK_SOUND_Create,
  GHOST_SPARK_SOUND_Delete,
  GHOST_SPARK_SOUND_Update,
  NULL,
};


//----------------------------------------------------------------------------
/**
 *	@brief  �l�V���@�S�[�X�g�����@�����@�N��
 *
 *	@param	p_fieldmap 
 */
//-----------------------------------------------------------------------------
void FIELDMAPFUNC_FourkingsGhostSound( FIELDMAP_WORK* p_fieldmap )
{
  FLDMAPFUNC_WORK * p_funcwk;
  FLDMAPFUNC_SYS* p_funcsys = FIELDMAP_GetFldmapFuncSys( p_fieldmap );

  // 
  p_funcwk = FLDMAPFUNC_Create(p_funcsys, &sc_FLDMAPFUNC_GHOST_SPARK_SOUND);
}



//----------------------------------------------------------------------------
/**
 *	@brief  �S�[�X�g�@�����@�Ǘ�
 */
//-----------------------------------------------------------------------------
static void GHOST_SPARK_SOUND_Create(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work )
{
  GHOST_SPARK_SOUND* p_wk = p_work;
  FLDMAPPER* p_mapper = FIELDMAP_GetFieldG3Dmapper( p_fieldmap );
  FIELD_BMODEL_MAN* p_model_man = FLDMAPPER_GetBuildModelManager( p_mapper );
  fx32 start_frame;
  int  i;

  // ����z�u�I�u�W�F
  p_wk->cp_spark_obj  = FIELD_BMODEL_MAN_SearchObjStatusPos( p_model_man, BM_SEARCH_ID_NULL, &sc_GHOST_SPARK_SARCH_POS );
  p_wk->p_man         = p_model_man;

  GF_ASSERT(p_wk->cp_spark_obj);
  GF_ASSERT(p_wk->p_man);
  
  start_frame = G3DMAPOBJST_getAnimeFrame( p_wk->p_man, p_wk->cp_spark_obj, GHOST_SPARK_SOUND_ANM_INDEX );
  
  // �J�n���̉������t���[����ݒ�
  p_wk->next_frame  = sc_GHOST_SPARK_SOUND_TIMING[0];
  p_wk->tbl_index   = 0;
  for( i=0; i<GHOST_SPARK_SOUND_TIMING_TBL_MAX; i++ )
  {
    if( start_frame < sc_GHOST_SPARK_SOUND_TIMING[i] )
    {
      p_wk->next_frame  = sc_GHOST_SPARK_SOUND_TIMING[i];
      p_wk->tbl_index   = i;
      break;
    }
  }

  // ���[�v�Đ��J�n
  PMSND_PlaySE( SEQ_SE_FLD_111 );
  PMSND_PlaySE( SEQ_SE_FLD_112 );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �S�[�X�g�@�����@�j��
 */
//-----------------------------------------------------------------------------
static void GHOST_SPARK_SOUND_Delete(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work )
{
  // ���[�vSE�̒�~
  PMSND_StopSE_byPlayerID( PMSND_GetSE_DefaultPlayerID(SEQ_SE_FLD_111) );
  PMSND_StopSE_byPlayerID( PMSND_GetSE_DefaultPlayerID(SEQ_SE_FLD_112) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �S�[�X�g�@�����@�X�V
 */
//-----------------------------------------------------------------------------
static void GHOST_SPARK_SOUND_Update(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work )
{
  GHOST_SPARK_SOUND* p_wk = p_work;
  fx32 now_frame;

  now_frame = G3DMAPOBJST_getAnimeFrame( p_wk->p_man, p_wk->cp_spark_obj, GHOST_SPARK_SOUND_ANM_INDEX );

  // ���܂����t���[���ŁA����炷�B
  if( p_wk->next_frame == now_frame )
  {
    p_wk->tbl_index = (p_wk->tbl_index + 1) % GHOST_SPARK_SOUND_TIMING_TBL_MAX;

    p_wk->next_frame = sc_GHOST_SPARK_SOUND_TIMING[ p_wk->tbl_index ];
    PMSND_PlaySE( GHOST_SND_SE_SPARK );
  }

  /*
#ifdef PM_DEBUG
  if( GFL_UI_KEY_GetTrg() & PAD_KEY_LEFT  )
  {
    sc_GHOST_SPARK_SOUND_TIMING[0] -= FX32_ONE;
    TOMOYA_Printf( "frame %d\n", FX_Whole(sc_GHOST_SPARK_SOUND_TIMING[0]) );
  }
  else if( GFL_UI_KEY_GetTrg() & PAD_KEY_RIGHT  )
  {
    sc_GHOST_SPARK_SOUND_TIMING[0] += FX32_ONE;
    TOMOYA_Printf( "frame %d\n", FX_Whole(sc_GHOST_SPARK_SOUND_TIMING[0]) );
  }
#endif
//*/
}




//-----------------------------------------------------------------------------
/**
 *    �����ρ[����
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	ESPERT SOUND
//=====================================
typedef struct 
{
  FIELD_BMODEL_MAN* p_man;
  const G3DMAPOBJST* cp_obj;
} ESPERT_SOUND;

// �G�X�p�[�������̈ʒu
static const VecFx32 sc_ESPERT_STAR_SARCH_POS = 
{
  FX32_CONST(128), 0, FX32_CONST(255)
};

#define ESPERT_SE_START_TIMING_MAX  (2)
static const sc_ESPERT_SE_START_TIMING[ ESPERT_SE_START_TIMING_MAX ] = 
{
  FX32_CONST(90), FX32_CONST(213),
};
#define ESPERT_BMODEL_ANIME_IDX  ( 0 )


static void ESPERT_SOUND_Create(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work );
static void ESPERT_SOUND_Delete(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work );
static void ESPERT_SOUND_Update(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work );


static const FLDMAPFUNC_DATA sc_FLDMAPFUNC_ESPERT_SOUND = 
{
  128,
  sizeof(ESPERT_SOUND),
  ESPERT_SOUND_Create,
  ESPERT_SOUND_Delete,
  ESPERT_SOUND_Update,
  NULL,
};


//----------------------------------------------------------------------------
/**
 *	@brief  �G�X�p�[�����̃T�E���h�V�X�e���J�n
 *
 *	@param	p_fieldmap 
 */
//-----------------------------------------------------------------------------
void FIELDMAPFUNC_FourkingsEspertSound( FIELDMAP_WORK* p_fieldmap )
{
  FLDMAPFUNC_WORK * p_funcwk;
  FLDMAPFUNC_SYS* p_funcsys = FIELDMAP_GetFldmapFuncSys( p_fieldmap );

  // 
  p_funcwk = FLDMAPFUNC_Create(p_funcsys, &sc_FLDMAPFUNC_ESPERT_SOUND);
}


//----------------------------------------------------------------------------
/**
 *	@brief  �����ρ[SE�V�X�e��  ����
 */
//-----------------------------------------------------------------------------
static void ESPERT_SOUND_Create(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work )
{
  ESPERT_SOUND* p_wk = p_work;
  FLDMAPPER* p_mapper = FIELDMAP_GetFieldG3Dmapper( p_fieldmap );
  FIELD_BMODEL_MAN* p_model_man = FLDMAPPER_GetBuildModelManager( p_mapper );
  fx32 start_frame;
  int  i;

  // ����z�u�I�u�W�F
  p_wk->cp_obj  = FIELD_BMODEL_MAN_SearchObjStatusPos( p_model_man, BM_SEARCH_ID_NULL, &sc_ESPERT_STAR_SARCH_POS );
  p_wk->p_man         = p_model_man;

  GF_ASSERT(p_wk->cp_obj);
  GF_ASSERT(p_wk->p_man);
  
}


//----------------------------------------------------------------------------
/**
 *	@brief  �����ρ[SE�V�X�e��  �j��
 */
//-----------------------------------------------------------------------------
static void ESPERT_SOUND_Delete(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work )
{
  ESPERT_SOUND* p_wk = p_work;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����ρ[SE�V�X�e��  �X�V
 */
//-----------------------------------------------------------------------------
static void ESPERT_SOUND_Update(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work )
{
  ESPERT_SOUND* p_wk = p_work;
  fx32 frame;
  int i;

  frame = G3DMAPOBJST_getAnimeFrame( p_wk->p_man, p_wk->cp_obj, ESPERT_BMODEL_ANIME_IDX );

  for( i=0; i<ESPERT_SE_START_TIMING_MAX; i++ )
  {
    if(frame == sc_ESPERT_SE_START_TIMING[i])
    {
      PMSND_PlaySE( SEQ_SE_FLD_108 );
      break;
    }
  }

#if 0
  if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_A )
  {
    sc_ESPERT_SE_START_TIMING[0] += FX32_HALF;
    TOMOYA_Printf( "start timing 0 0x%x\n", sc_ESPERT_SE_START_TIMING[0] );
  }
  else if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_B )
  {
    sc_ESPERT_SE_START_TIMING[0] -= FX32_HALF;
    TOMOYA_Printf( "start timing 0 0x%x\n", sc_ESPERT_SE_START_TIMING[0] );
  }

  if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_X )
  {
    sc_ESPERT_SE_START_TIMING[1] += FX32_HALF;
    TOMOYA_Printf( "start timing 1 0x%x\n", sc_ESPERT_SE_START_TIMING[1] );
  }
  else if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_Y )
  {
    sc_ESPERT_SE_START_TIMING[1] -= FX32_HALF;
    TOMOYA_Printf( "start timing 1 0x%x\n", sc_ESPERT_SE_START_TIMING[1] );
  }
#endif
}

//-----------------------------------------------------------------------------
/**
 *    ������
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	BAD SOUND
//=====================================
typedef struct 
{
  int dummy;
} BAD_SOUND;


static void BAD_SOUND_Create(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work );
static void BAD_SOUND_Delete(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work );


static const FLDMAPFUNC_DATA sc_FLDMAPFUNC_BAD_SOUND = 
{
  128,
  sizeof(BAD_SOUND),
  BAD_SOUND_Create,
  BAD_SOUND_Delete,
  NULL,
  NULL,
};


//----------------------------------------------------------------------------
/**
 *	@brief  �������̃T�E���h�V�X�e���J�n
 *
 *	@param	p_fieldmap 
 */
//-----------------------------------------------------------------------------
void FIELDMAPFUNC_FourkingsBadSound( FIELDMAP_WORK* p_fieldmap )
{
  FLDMAPFUNC_WORK * p_funcwk;
  FLDMAPFUNC_SYS* p_funcsys = FIELDMAP_GetFldmapFuncSys( p_fieldmap );

  // 
  p_funcwk = FLDMAPFUNC_Create(p_funcsys, &sc_FLDMAPFUNC_BAD_SOUND);
}


//----------------------------------------------------------------------------
/**
 *	@brief  ��SE�V�X�e��  ����
 */
//-----------------------------------------------------------------------------
static void BAD_SOUND_Create(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work )
{
  // ���[�v�Đ��J�n
  PMSND_PlaySE( SEQ_SE_FLD_116 );
}


//----------------------------------------------------------------------------
/**
 *	@brief  ��SE�V�X�e��  �j��
 */
//-----------------------------------------------------------------------------
static void BAD_SOUND_Delete(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work )
{
  // ���[�vSE�̒�~
  PMSND_StopSE_byPlayerID( PMSND_GetSE_DefaultPlayerID(SEQ_SE_FLD_116) );
}

