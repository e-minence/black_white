//======================================================================
/**
 * @file  fldeff_footmark.c
 * @brief  �t�B�[���h ����
 * @author  kagaya
 * @date  05.07.13
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "fieldmap.h"
#include "field_effect.h"
#include "fldmmdl.h"

#include "fldeff_footmark.h"

//======================================================================
//  define
//======================================================================
//--------------------------------------------------------------
///  ���Վ���
//--------------------------------------------------------------
enum
{
  FOOTMARK_WALK_UP = 0,              ///<���Ձ@��
  FOOTMARK_WALK_DOWN,                ///<���Ձ@��
  FOOTMARK_WALK_LEFT,                ///<���Ձ@��
  FOOTMARK_WALK_RIGHT,              ///<���Ձ@�E
  FOOTMARK_CYCLE_UD,                ///<���]�ԑ��� �㉺
  FOOTMARK_CYCLE_LR,                ///<���]�ԑ��� ���E
  FOOTMARK_CYCLE_UL,                ///<���]�ԑ��� �㍶
  FOOTMARK_CYCLE_UR,                ///<���]�ԑ��� ��E
  FOOTMARK_CYCLE_DL,                ///<���]�ԑ��� ����
  FOOTMARK_CYCLE_DR,                ///<���]�ԑ��� ���E
  FOOTMARK_MAX,                    ///<�n�ʑ��Սő吔
  
  FOOTMARK_SNOW_WALK_UP = FOOTMARK_MAX,   ///<���Ձ@��
  FOOTMARK_SNOW_WALK_DOWN,                ///<���Ձ@��
  FOOTMARK_SNOW_WALK_LEFT,                ///<���Ձ@��
  FOOTMARK_SNOW_WALK_RIGHT,              ///<���Ձ@�E
  FOOTMARK_SNOW_CYCLE_UD,                ///<���]�ԑ��� �㉺
  FOOTMARK_SNOW_CYCLE_LR,                ///<���]�ԑ��� ���E
  FOOTMARK_SNOW_CYCLE_UL,                ///<���]�ԑ��� �㍶
  FOOTMARK_SNOW_CYCLE_UR,                ///<���]�ԑ��� ��E
  FOOTMARK_SNOW_CYCLE_DL,                ///<���]�ԑ��� ����
  FOOTMARK_SNOW_CYCLE_DR,                ///<���]�ԑ��� ���E
  FOOTMARK_SNOW_MAX,                  ///<�ᑫ�Սő吔
  
  FOOTMARK_DEEPSNOW_UD = FOOTMARK_SNOW_MAX, //�[����@�㉺
  FOOTMARK_DEEPSNOW_LR, //�[����@���E
  FOOTMARK_DEEPSNOW_UL, //�[����@�㍶
  FOOTMARK_DEEPSNOW_UR, //�[����@��E
  FOOTMARK_DEEPSNOW_DL, //�[����@����
  FOOTMARK_DEEPSNOW_DR, //�[����@���E
  FOOTMARK_DEEPSNOW_MAX,
  
  FOOTMARK_ALL_MAX = FOOTMARK_DEEPSNOW_MAX, ///<�S���Սő吔
};

///�� ���Ճ��\�[�X�ʒu
#define FOOTMARK_SNOW_START (FOOTMARK_SNOW_WALK_UP)
///���]�ԑ��Ճ��\�[�X�ʒu
#define FOOTMARK_CYCLE_START (FOOTMARK_CYCLE_UD)
///��@���]�ԑ��Ճ��\�[�X�ʒu
#define FOOTMARK_SNOW_CYCLE_START (FOOTMARK_SNOW_CYCLE_UD)
///�[���� ���\�[�X�ʒu
#define FOOTMARK_DEEPSNOW_START (FOOTMARK_DEEPSNOW_UD)

#define FOOTMARK_OFFSPOS_Y (NUM_FX32(-8))
#define FOOTMARK_OFFSPOS_Z (NUM_FX32(1))
#define FOOTMARK_OFFSPOS_Z_WALK_LR (NUM_FX32(-2))

#define FOOTMARK_VANISH_START_FRAME (16)			///<�_�ŊJ�n�t���[��(�ԗ�40)
#define FOOTMARK_VANISH_END_FRAME (28)				///<�_�ŏI���t���[��(�ԗ�56)

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
///  FLDEFF_FOOTMARK�^
//--------------------------------------------------------------
typedef struct _TAG_FLDEFF_FOOTMARK FLDEFF_FOOTMARK;

//--------------------------------------------------------------
///  FLDEFF_FOOTMARK�\����
//--------------------------------------------------------------
struct _TAG_FLDEFF_FOOTMARK
{
  FLDEFF_CTRL *fectrl;
  
  GFL_G3D_RES *g3d_res_mdl[FOOTMARK_ALL_MAX];
  GFL_G3D_RND *g3d_rnd[FOOTMARK_ALL_MAX];
  GFL_G3D_OBJ *g3d_obj[FOOTMARK_ALL_MAX];
};

//--------------------------------------------------------------
/// TASKHEADER_FOOTMARK
//--------------------------------------------------------------
typedef struct
{
  FLDEFF_FOOTMARK *eff_fmark;
  GFL_G3D_OBJ *obj;
  NNSG3dResMdl *resMdl;
}TASKHEADER_FOOTMARK;

//--------------------------------------------------------------
/// TASKWORK_FOOTMARK
//--------------------------------------------------------------
typedef struct
{
  TASKHEADER_FOOTMARK head;
  int seq_no;
  int vanish;
  int frame;
  int alpha;
}TASKWORK_FOOTMARK;

//======================================================================
//  �v���g�^�C�v
//======================================================================
static void fmark_InitResource( FLDEFF_FOOTMARK *fmark );
static void fmark_DeleteResource( FLDEFF_FOOTMARK *fmark );
static int fmark_GetObject( FLDEFF_FOOTMARK *fmark,
    FOOTMARK_TYPE type, u16 now_dir, u16 old_dir, GFL_G3D_OBJ **outobj );

static const FLDEFF_TASK_HEADER data_fmarkTaskHeader;
static const u32 data_FootMarkArcIdx[FOOTMARK_ALL_MAX];
static const u8 data_FootMarkCycleDirTbl[DIR_MAX4][DIR_MAX4];

//======================================================================
//  ���Ձ@�V�X�e��
//======================================================================
//--------------------------------------------------------------
/**
 * ���� ������
 * @param  fectrl    FLDEFF_CTRL *
 * @param heapID HEAPID
 * @retval  void*  �G�t�F�N�g�g�p���[�N
 */
//--------------------------------------------------------------
void * FLDEFF_FOOTMARK_Init( FLDEFF_CTRL *fectrl, HEAPID heapID )
{
  FLDEFF_FOOTMARK *fmark;
  
  fmark = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLDEFF_FOOTMARK) );
  fmark->fectrl = fectrl;
  
  fmark_InitResource( fmark );
  return( fmark );
}

//--------------------------------------------------------------
/**
 * ���� �폜
 * @param fectrl FLDEFF_CTRL
 * @param  work  �G�t�F�N�g�g�p���[�N
 * @retval  nothing
 */
//--------------------------------------------------------------
void FLDEFF_FOOTMARK_Delete( FLDEFF_CTRL *fectrl, void *work )
{
  FLDEFF_FOOTMARK *fmark = work;
  fmark_DeleteResource( fmark );
  GFL_HEAP_FreeMemory( fmark );
}

//======================================================================
//  ���Ձ@���\�[�X
//======================================================================
//--------------------------------------------------------------
/**
 * ���Ձ@���\�[�X������
 * @param fectrl FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
static void fmark_InitResource( FLDEFF_FOOTMARK *fmark )
{
  int i;
  NNSG3dResMdl *pMdl;
  NNSG3dRenderObj *rnd;
  const u32 *idx = data_FootMarkArcIdx;
  ARCHANDLE *handle = FLDEFF_CTRL_GetArcHandleEffect( fmark->fectrl );
  
  for( i = 0; i < FOOTMARK_ALL_MAX; i++, idx++ )
  {
    fmark->g3d_res_mdl[i] =
      GFL_G3D_CreateResourceHandle( handle, *idx );
      GFL_G3D_TransVramTexture( fmark->g3d_res_mdl[i] );
    fmark->g3d_rnd[i] =
      GFL_G3D_RENDER_Create(
          fmark->g3d_res_mdl[i], 0, fmark->g3d_res_mdl[i] );
  
    rnd = GFL_G3D_RENDER_GetRenderObj( fmark->g3d_rnd[i] );
    pMdl = NNS_G3dRenderObjGetResMdl( rnd );
#if 0
    NNS_G3dMdlUseGlbDiff( pMdl );
    NNS_G3dMdlUseGlbAmb( pMdl );
    NNS_G3dMdlUseGlbSpec( pMdl );
#endif    
    fmark->g3d_obj[i] = 
      GFL_G3D_OBJECT_Create( fmark->g3d_rnd[i], NULL, 0 );
  }
}

//--------------------------------------------------------------
/**
 * ���Ձ@���\�[�X�폜
 * @param fectrl FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
static void fmark_DeleteResource( FLDEFF_FOOTMARK *fmark )
{
  int i;
  
  for( i = 0; i < FOOTMARK_ALL_MAX; i++ )
  {
    GFL_G3D_OBJECT_Delete( fmark->g3d_obj[i] );
    GFL_G3D_RENDER_Delete( fmark->g3d_rnd[i] );
    GFL_G3D_DeleteResource( fmark->g3d_res_mdl[i] );
  }
}
  
//--------------------------------------------------------------
/**
 * ���Ձ@�w��^�C�v��OBJ�擾
 * @param fmark FLDEFF_FOOTMARK *fmark
 * @param type FOOTMARK_TYPE
 * @param now_dir ���ݕ���
 * @param old_dir �ߋ�����
 * @param outobj GFL_G3D_OBJ�i�[��
 * @retval int FOOTMARK_WALK_UP��
 */
//--------------------------------------------------------------
static int fmark_GetObject( FLDEFF_FOOTMARK *fmark,
    FOOTMARK_TYPE type, u16 now_dir, u16 old_dir, GFL_G3D_OBJ **outobj )
{
  int no = 0;
  switch( type ){
  case FOOTMARK_TYPE_HUMAN:
    no = FOOTMARK_WALK_UP + now_dir;
    *outobj = fmark->g3d_obj[no];
    break;
  case FOOTMARK_TYPE_CYCLE:
    no = data_FootMarkCycleDirTbl[old_dir][now_dir];
    *outobj = fmark->g3d_obj[no];
    break;
  case FOOTMARK_TYPE_HUMAN_SNOW:
    no = FOOTMARK_SNOW_WALK_UP + now_dir;
    *outobj = fmark->g3d_obj[no];
    break;
  case FOOTMARK_TYPE_CYCLE_SNOW:
    no = data_FootMarkCycleDirTbl[old_dir][now_dir] - FOOTMARK_CYCLE_START;
    no += FOOTMARK_SNOW_CYCLE_START;
    *outobj = fmark->g3d_obj[no];
    break;
  case FOOTMARK_TYPE_DEEPSNOW:
    no = data_FootMarkCycleDirTbl[old_dir][now_dir] - FOOTMARK_CYCLE_START;
    no += FOOTMARK_DEEPSNOW_START;
    *outobj = fmark->g3d_obj[no];
    break;
  default:
    GF_ASSERT( 0 );
    *outobj = fmark->g3d_obj[no];
  }
  return( no );
}

//======================================================================
//  ���Ձ@�^�X�N
//======================================================================
//--------------------------------------------------------------
/**
 * ���샂�f���p���Ձ@�ǉ�
 * @param mmdl MMDL
 * @param FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDEFF_FOOTMARK_SetMMdl(
    MMDL *mmdl, FLDEFF_CTRL *fectrl, FOOTMARK_TYPE type )
{
  int no;
  VecFx32 pos;
  FLDEFF_FOOTMARK *fmark;
  TASKHEADER_FOOTMARK head;
  
  fmark = FLDEFF_CTRL_GetEffectWork( fectrl, FLDEFF_PROCID_FOOTMARK );
  head.eff_fmark = fmark;
  no = fmark_GetObject( fmark, type,
      MMDL_GetDirDisp(mmdl), MMDL_GetDirDispOld(mmdl), &head.obj );
  
  {
    GFL_G3D_RND *g3drnd = GFL_G3D_OBJECT_GetG3Drnd( head.obj );
    NNSG3dRenderObj *rnd = GFL_G3D_RENDER_GetRenderObj( g3drnd );
    head.resMdl = NNS_G3dRenderObjGetResMdl( rnd );
  }

  MMDL_GetVectorPos( mmdl, &pos ); //y
  MMDL_TOOL_GetCenterGridPos(
      MMDL_GetOldGridPosX(mmdl), MMDL_GetOldGridPosZ(mmdl), &pos );
  
  switch( type ){
  case FOOTMARK_TYPE_HUMAN:
  case FOOTMARK_TYPE_CYCLE:
    pos.y += FOOTMARK_OFFSPOS_Y;
    
    if( type == FOOTMARK_TYPE_HUMAN &&
        (no == FOOTMARK_WALK_LEFT || no == FOOTMARK_WALK_RIGHT) ){
      pos.z += FOOTMARK_OFFSPOS_Z_WALK_LR;
    }else{
      pos.z += FOOTMARK_OFFSPOS_Z;
    }
    break;
  case FOOTMARK_TYPE_DEEPSNOW:
    pos.y += NUM_FX32( -8 );
    pos.z += NUM_FX32( -4 );
    break;
  }
  
  FLDEFF_CTRL_AddTask(
      fectrl, &data_fmarkTaskHeader, &pos, 0, &head, 0 );
}

//--------------------------------------------------------------
/**
 * ���Ճ^�X�N�@������
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void fmarkTask_Init( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_FOOTMARK *work = wk;
  const TASKHEADER_FOOTMARK *head;
  head = FLDEFF_TASK_GetAddPointer( task );
  work->head = *head;
  work->alpha = 31;
}

//--------------------------------------------------------------
/**
 * ���Ճ^�X�N�@�폜
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void fmarkTask_Delete( FLDEFF_TASK *task, void *wk )
{
}

//--------------------------------------------------------------
/**
 * ���Ճ^�X�N�@�X�V
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void fmarkTask_Update( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_FOOTMARK *work = wk;
  
  switch( work->seq_no ){
  case 0:
    work->frame++;
    if( work->frame >= FOOTMARK_VANISH_START_FRAME ){
      work->seq_no++;
    }
    break;
  case 1:
    work->alpha -= 2;
    if( work->alpha < 0 ){
      FLDEFF_TASK_CallDelete( task );
    }
  }
} 

//--------------------------------------------------------------
/**
 * ���Ճ^�X�N�@�`��
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void fmarkTask_Draw( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_FOOTMARK *work = wk;
  
  if( work->vanish == FALSE ){
    GFL_G3D_OBJSTATUS status = {{0},{FX32_ONE,FX32_ONE,FX32_ONE},{0}};
    NNS_G3dMdlUseMdlAlpha( work->head.resMdl );
		NNS_G3dMdlSetMdlAlphaAll( work->head.resMdl, work->alpha );
    MTX_Identity33( &status.rotate );
    FLDEFF_TASK_GetPos( task, &status.trans );
    GFL_G3D_DRAW_DrawObjectCullingON( work->head.obj, &status );
  }
}

//--------------------------------------------------------------
//  ���Ճ^�X�N�@�w�b�_�[
//--------------------------------------------------------------
static const FLDEFF_TASK_HEADER data_fmarkTaskHeader =
{
  sizeof(TASKWORK_FOOTMARK),
  fmarkTask_Init,
  fmarkTask_Delete,
  fmarkTask_Update,
  fmarkTask_Draw,
};

//======================================================================
//  data
//======================================================================
//--------------------------------------------------------------
///	����imd�e�[�u���@���т�FOOTMARK_WALK_UP���̒l�Ɉ�v
//--------------------------------------------------------------
static const u32 data_FootMarkArcIdx[FOOTMARK_ALL_MAX] =
{
  //�ʏ�
	NARC_fldeff_f_mark_u_nsbmd,
	NARC_fldeff_f_mark_d_nsbmd,
	NARC_fldeff_f_mark_l_nsbmd,
	NARC_fldeff_f_mark_r_nsbmd,
	NARC_fldeff_c_mark_u_nsbmd,
	NARC_fldeff_c_mark_l_nsbmd,
	NARC_fldeff_c_mark_ul_nsbmd,
	NARC_fldeff_c_mark_ur_nsbmd,
	NARC_fldeff_c_mark_dl_nsbmd,
	NARC_fldeff_c_mark_dr_nsbmd,
  
  //��
	NARC_fldeff_nf_mark_u_nsbmd,
	NARC_fldeff_nf_mark_d_nsbmd,
	NARC_fldeff_nf_mark_l_nsbmd,
	NARC_fldeff_nf_mark_r_nsbmd,
	NARC_fldeff_sc_mark_u_nsbmd,
	NARC_fldeff_sc_mark_l_nsbmd,
	NARC_fldeff_sc_mark_ul_nsbmd,
	NARC_fldeff_sc_mark_ur_nsbmd,
	NARC_fldeff_sc_mark_dl_nsbmd,
	NARC_fldeff_sc_mark_dr_nsbmd,

  //�[����
	NARC_fldeff_nc_mark_u_nsbmd,
	NARC_fldeff_nc_mark_l_nsbmd,
	NARC_fldeff_nc_mark_ul_nsbmd,
	NARC_fldeff_nc_mark_ur_nsbmd,
	NARC_fldeff_nc_mark_dl_nsbmd,
	NARC_fldeff_nc_mark_dr_nsbmd,
};

//--------------------------------------------------------------
///	���]�ԑ��Ճe�[�u�� [�ߋ�����][���ݕ���]
//--------------------------------------------------------------
static const u8 data_FootMarkCycleDirTbl[DIR_MAX4][DIR_MAX4] =
{
	{FOOTMARK_CYCLE_UD,FOOTMARK_CYCLE_UD,FOOTMARK_CYCLE_UR,FOOTMARK_CYCLE_UL},
	{FOOTMARK_CYCLE_UD,FOOTMARK_CYCLE_UD,FOOTMARK_CYCLE_DR,FOOTMARK_CYCLE_DL},
	{FOOTMARK_CYCLE_DL,FOOTMARK_CYCLE_UL,FOOTMARK_CYCLE_LR,FOOTMARK_CYCLE_LR},
	{FOOTMARK_CYCLE_DR,FOOTMARK_CYCLE_UR,FOOTMARK_CYCLE_LR,FOOTMARK_CYCLE_LR},
};
