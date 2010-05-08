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

//#define FOOTMARK_OFFSPOS_Y (NUM_FX32(-8))
#define FOOTMARK_OFFSPOS_Y (NUM_FX32(-1))
#define FOOTMARK_OFFSPOS_Z_WALK_LR (NUM_FX32(-2))

#define FOOTMARK_VANISH_START_FRAME (16)			///<�_�ŊJ�n�t���[��(�ԗ�40)
#define FOOTMARK_VANISH_END_FRAME (28)				///<�_�ŏI���t���[��(�ԗ�56)

#if 0
#define FOOTMARK_BBDACT_SIZE_X (FX16_ONE*1+0x200)
#define FOOTMARK_BBDACT_SIZE_Y (FX16_ONE*1+0x200)
#else
#define FOOTMARK_BBDACT_SIZE_X (FX16_ONE*1+0x300)
#define FOOTMARK_BBDACT_SIZE_Y (FX16_ONE*1+0x300)
#endif

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
  GFL_BBDACT_SYS *bbdact_sys;
  u16 res_idx_tbl[FOOTMARK_ALL_MAX];
};

//--------------------------------------------------------------
/// TASKHEADER_FOOTMARK
//--------------------------------------------------------------
typedef struct
{
  FLDEFF_FOOTMARK *eff_fmark;
  u16 fmark_no;
}TASKHEADER_FOOTMARK;

//--------------------------------------------------------------
/// TASKWORK_FOOTMARK
//--------------------------------------------------------------
typedef struct
{
  TASKHEADER_FOOTMARK head;
  u16 act_id;
  u16 seq_no;
  u16 vanish;
  s16 alpha;
  int frame;
}TASKWORK_FOOTMARK;

//======================================================================
//  �v���g�^�C�v
//======================================================================
static void fmark_InitResource( FLDEFF_FOOTMARK *fmark );
static void fmark_DeleteResource( FLDEFF_FOOTMARK *fmark );
static int fmark_GetObject( FLDEFF_FOOTMARK *fmark,
    FOOTMARK_TYPE type, u16 now_dir, u16 old_dir );

static const FLDEFF_TASK_HEADER data_fmarkTaskHeader;
static const u16 data_FootMarkArcIdx[FOOTMARK_ALL_MAX];
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
	FIELDMAP_WORK *fieldmap;
  
  fmark = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLDEFF_FOOTMARK) );
  fmark->fectrl = fectrl;
  
  fieldmap = FLDEFF_CTRL_GetFieldMapWork( fmark->fectrl );
  fmark->bbdact_sys = FIELDMAP_GetEffBbdActSys( fieldmap );
  
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
  GFL_BBDACT_G3DRESDATA data;
  const u16 *idx = data_FootMarkArcIdx;
  ARCHANDLE *handle = FLDEFF_CTRL_GetArcHandleEffect( fmark->fectrl );
  
  data.texFmt = GFL_BBD_TEXFMT_PAL4;
  data.texSiz = GFL_BBD_TEXSIZ_16x16;
  data.celSizX = 16;
  data.celSizY = 16;
  data.dataCut = GFL_BBDACT_RESTYPE_DATACUT;
  
  for( i = 0; i < FOOTMARK_ALL_MAX; i++, idx++ )
  {
    data.g3dres = GFL_G3D_CreateResourceHandle( handle, *idx );
    fmark->res_idx_tbl[i] =
      GFL_BBDACT_AddResourceG3DResUnit( fmark->bbdact_sys, &data, 1 );
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
    GFL_BBDACT_RemoveResourceUnit(
        fmark->bbdact_sys, fmark->res_idx_tbl[i], 1 );
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
    FOOTMARK_TYPE type, u16 now_dir, u16 old_dir )
{
  int no = 0;
  switch( type ){
  case FOOTMARK_TYPE_HUMAN:
    no = FOOTMARK_WALK_UP + now_dir;
    break;
  case FOOTMARK_TYPE_CYCLE:
    if( old_dir != now_dir ){
      OS_Printf( "CYCLE OLD %d, NOW %d\n", old_dir, now_dir );
    }
    no = data_FootMarkCycleDirTbl[old_dir][now_dir];
    break;
  case FOOTMARK_TYPE_HUMAN_SNOW:
    no = FOOTMARK_SNOW_WALK_UP + now_dir;
    break;
  case FOOTMARK_TYPE_CYCLE_SNOW:
    no = data_FootMarkCycleDirTbl[old_dir][now_dir] - FOOTMARK_CYCLE_START;
    no += FOOTMARK_SNOW_CYCLE_START;
    break;
  case FOOTMARK_TYPE_DEEPSNOW:
    no = data_FootMarkCycleDirTbl[old_dir][now_dir] - FOOTMARK_CYCLE_START;
    no += FOOTMARK_DEEPSNOW_START;
    break;
  default:
    GF_ASSERT( 0 );
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
  u16 no;
  VecFx32 pos;
  FLDEFF_FOOTMARK *fmark;
  TASKHEADER_FOOTMARK head;
  
  fmark = FLDEFF_CTRL_GetEffectWork( fectrl, FLDEFF_PROCID_FOOTMARK );
  head.eff_fmark = fmark;
  
  no = fmark_GetObject( fmark, type,
      MMDL_GetDirDisp(mmdl), MMDL_GetDirDispOld(mmdl) );

  MMDL_GetVectorPos( mmdl, &pos ); //y
  MMDL_TOOL_GetCenterGridPos(
      MMDL_GetOldGridPosX(mmdl), MMDL_GetOldGridPosZ(mmdl), &pos );
  
  pos.y += FOOTMARK_OFFSPOS_Y;
  
  if( type == FOOTMARK_TYPE_HUMAN &&
      (no == FOOTMARK_WALK_LEFT || no == FOOTMARK_WALK_RIGHT) ){
    pos.z += FOOTMARK_OFFSPOS_Z_WALK_LR;
  }else if( type == FOOTMARK_TYPE_DEEPSNOW &&
      (no == FOOTMARK_WALK_LEFT || no == FOOTMARK_WALK_RIGHT) ){
    pos.z += FOOTMARK_OFFSPOS_Z_WALK_LR;
  }
  
  head.fmark_no = no;
  
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
  ARCHANDLE *handle;
  TASKWORK_FOOTMARK *work = wk;
  const TASKHEADER_FOOTMARK *head;
  GFL_BBDACT_ACTDATA actData;
  GFL_BBDACT_SYS *bbdact_sys;
  
  head = FLDEFF_TASK_GetAddPointer( task );
  work->head = *head;
  work->alpha = 31;
  
  actData.resID = work->head.eff_fmark->res_idx_tbl[work->head.fmark_no];
  actData.sizX = FOOTMARK_BBDACT_SIZE_X;
  actData.sizY = FOOTMARK_BBDACT_SIZE_Y;
  actData.alpha = work->alpha;
  actData.drawEnable = TRUE;
  actData.lightMask = GFL_BBD_LIGHTMASK_0;
  actData.func = NULL;
  actData.work = work;
  
  FLDEFF_TASK_GetPos( task, &actData.trans );
  
  bbdact_sys = work->head.eff_fmark->bbdact_sys;
  work->act_id = GFL_BBDACT_AddActEx(
      bbdact_sys, 0, &actData, 1, GFL_BBD_DRAWMODE_XZ_FLAT_BILLBORD );
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
  TASKWORK_FOOTMARK *work = wk;
  GFL_BBDACT_SYS *bbdact_sys = work->head.eff_fmark->bbdact_sys;
  GFL_BBDACT_RemoveAct( bbdact_sys, work->act_id, 1 );
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
    }else{
      u8 alpha = work->alpha;
      GFL_BBDACT_SYS *bbdact_sys = work->head.eff_fmark->bbdact_sys;
      GFL_BBD_SYS *bbdsys = GFL_BBDACT_GetBBDSystem( bbdact_sys );
      u16 obj_idx = GFL_BBDACT_GetBBDActIdxResIdx( bbdact_sys, work->act_id );
      GFL_BBD_SetObjectAlpha( bbdsys, obj_idx, &alpha );
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
#if 0 //�r���{�[�h�A�N�^�[�ɔC����
  TASKWORK_FOOTMARK *work = wk;
  
  if( work->vanish == FALSE ){
    GFL_G3D_OBJSTATUS status = {{0},{FX32_ONE,FX32_ONE,FX32_ONE},{0}};
    NNS_G3dMdlUseMdlAlpha( work->head.resMdl );
		NNS_G3dMdlSetMdlAlphaAll( work->head.resMdl, work->alpha );
    MTX_Identity33( &status.rotate );
    FLDEFF_TASK_GetPos( task, &status.trans );
    GFL_G3D_DRAW_DrawObjectCullingON( work->head.obj, &status );
  }
#endif
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
static const u16 data_FootMarkArcIdx[FOOTMARK_ALL_MAX] =
{
  //�ʏ�
	NARC_fldeff_f_mark_u_nsbtx,
	NARC_fldeff_f_mark_d_nsbtx,
	NARC_fldeff_f_mark_l_nsbtx,
	NARC_fldeff_f_mark_r_nsbtx,
	NARC_fldeff_c_mark_u_nsbtx,
	NARC_fldeff_c_mark_l_nsbtx,
	NARC_fldeff_c_mark_ul_nsbtx,
	NARC_fldeff_c_mark_ur_nsbtx,
	NARC_fldeff_c_mark_dl_nsbtx,
	NARC_fldeff_c_mark_dr_nsbtx,
  
  //��
	NARC_fldeff_nf_mark_u_nsbtx,
	NARC_fldeff_nf_mark_d_nsbtx,
	NARC_fldeff_nf_mark_l_nsbtx,
	NARC_fldeff_nf_mark_r_nsbtx,
	NARC_fldeff_sc_mark_u_nsbtx,
	NARC_fldeff_sc_mark_l_nsbtx,
	NARC_fldeff_sc_mark_ul_nsbtx,
	NARC_fldeff_sc_mark_ur_nsbtx,
	NARC_fldeff_sc_mark_dl_nsbtx,
	NARC_fldeff_sc_mark_dr_nsbtx,

  //�[����
	NARC_fldeff_nc_mark_u_nsbtx,
	NARC_fldeff_nc_mark_l_nsbtx,
	NARC_fldeff_nc_mark_ul_nsbtx,
	NARC_fldeff_nc_mark_ur_nsbtx,
	NARC_fldeff_nc_mark_dl_nsbtx,
	NARC_fldeff_nc_mark_dr_nsbtx,
};

//--------------------------------------------------------------
///	���]�ԑ��Ճe�[�u�� [�ߋ�����][���ݕ���]
//--------------------------------------------------------------
static const u8 data_FootMarkCycleDirTbl[DIR_MAX4][DIR_MAX4] =
{
	{FOOTMARK_CYCLE_UD,FOOTMARK_CYCLE_UD,FOOTMARK_CYCLE_DR,FOOTMARK_CYCLE_DL},
	{FOOTMARK_CYCLE_UD,FOOTMARK_CYCLE_UD,FOOTMARK_CYCLE_UR,FOOTMARK_CYCLE_UL},
	{FOOTMARK_CYCLE_UL,FOOTMARK_CYCLE_DL,FOOTMARK_CYCLE_LR,FOOTMARK_CYCLE_LR},
	{FOOTMARK_CYCLE_UR,FOOTMARK_CYCLE_DR,FOOTMARK_CYCLE_LR,FOOTMARK_CYCLE_LR},
};

#if 0
static const u8 data_FootMarkCycleDirTbl[DIR_MAX4][DIR_MAX4] =
{
	{FOOTMARK_CYCLE_UD,FOOTMARK_CYCLE_UD,FOOTMARK_CYCLE_UR,FOOTMARK_CYCLE_UL},
	{FOOTMARK_CYCLE_UD,FOOTMARK_CYCLE_UD,FOOTMARK_CYCLE_DR,FOOTMARK_CYCLE_DL},
	{FOOTMARK_CYCLE_DL,FOOTMARK_CYCLE_UL,FOOTMARK_CYCLE_LR,FOOTMARK_CYCLE_LR},
	{FOOTMARK_CYCLE_DR,FOOTMARK_CYCLE_UR,FOOTMARK_CYCLE_LR,FOOTMARK_CYCLE_LR},
};
#endif
