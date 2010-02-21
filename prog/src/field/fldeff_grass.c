//======================================================================
/**
 * @file	fldeff_grass.c
 * @brief	�t�B�[���h ���G�t�F�N�g
 * @author	kagaya
 * @date	05.07.13
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "fieldmap.h"
#include "field_effect.h"
#include "fldmmdl.h"

#include "fldeff_grass.h"

#include "include/gamesystem/pm_season.h"

//======================================================================
//  define
//======================================================================
#define GRASS_SHAKE_FRAME (FX32_ONE*12)

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
///	FLDEFF_GRASS�^
//--------------------------------------------------------------
typedef struct _TAG_FLDEFF_GRASS FLDEFF_GRASS;

//--------------------------------------------------------------
///	FLDEFF_GRASS�\����
//--------------------------------------------------------------
struct _TAG_FLDEFF_GRASS
{
	FLDEFF_CTRL *fectrl;
  FLD_G3DOBJ_RESIDX res_idx_tbl[FLDEFF_GRASS_MAX];
};

//--------------------------------------------------------------
/// TASKHEADER_GRASS
//--------------------------------------------------------------
typedef struct
{
  FLDEFF_GRASS *eff_grass;
  FLD_G3DOBJ_CTRL *obj_ctrl;
  MMDL *fmmdl;
  int init_gx;
  int init_gz;
  u16 obj_id;
  u16 zone_id;
  VecFx32 pos;
  FLDEFF_GRASSTYPE type;
}TASKHEADER_GRASS;

//--------------------------------------------------------------
/// TASKWORK_GRASS
//--------------------------------------------------------------
typedef struct
{
  int seq_no;
  TASKHEADER_GRASS head;
  MMDL_CHECKSAME_DATA samedata;
  FLD_G3DOBJ_OBJIDX obj_idx;
}TASKWORK_GRASS;

//--------------------------------------------------------------
/// GRASS_ARCIDX
//--------------------------------------------------------------
typedef struct
{
  u16 idx_mdl;
  u16 idx_anm;
}GRASS_ARCIDX;

//======================================================================
//	�v���g�^�C�v
//======================================================================
static void grass_InitResource( FLDEFF_GRASS *grass );
static void grass_DeleteResource( FLDEFF_GRASS *grass );

static const FLDEFF_TASK_HEADER DATA_grassTaskHeader;

static const GRASS_ARCIDX data_ArcIdxTbl[FLDEFF_GRASS_MAX][PMSEASON_TOTAL];

//======================================================================
//	���G�t�F�N�g�@�V�X�e��
//======================================================================
//--------------------------------------------------------------
/**
 * ���G�t�F�N�g ������
 * @param	fectrl		FLDEFF_CTRL *
 * @param heapID HEAPID
 * @retval	void*	�G�t�F�N�g�g�p���[�N
 */
//--------------------------------------------------------------
void * FLDEFF_GRASS_Init( FLDEFF_CTRL *fectrl, HEAPID heapID )
{
	FLDEFF_GRASS *grass;
	
	grass = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLDEFF_GRASS) );
	grass->fectrl = fectrl;
  
	grass_InitResource( grass );
	return( grass );
}

//--------------------------------------------------------------
/**
 * ���G�t�F�N�g �폜
 * @param fectrl FLDEFF_CTRL
 * @param	work	�G�t�F�N�g�g�p���[�N
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDEFF_GRASS_Delete( FLDEFF_CTRL *fectrl, void *work )
{
	FLDEFF_GRASS *grass = work;
  grass_DeleteResource( grass );
  GFL_HEAP_FreeMemory( grass );
}

//======================================================================
//	���G�t�F�N�g�@���\�[�X
//======================================================================
//--------------------------------------------------------------
/**
 * ���G�t�F�N�g�@���\�[�X������
 * @param fectrl FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
static void grass_InitResource( FLDEFF_GRASS *grass )
{
  BOOL ret;
  int i,season;
  ARCHANDLE *handle;
  GAMESYS_WORK *gsys;
  FIELDMAP_WORK *fieldmap;
  FLD_G3DOBJ_CTRL *obj_ctrl;
  FLD_G3DOBJ_RES_HEADER head;

  obj_ctrl = FLDEFF_CTRL_GetFldG3dOBJCtrl( grass->fectrl );
  handle = FLDEFF_CTRL_GetArcHandleEffect( grass->fectrl );
  fieldmap = FLDEFF_CTRL_GetFieldMapWork( grass->fectrl );
  gsys = FIELDMAP_GetGameSysWork( fieldmap );
  season = GAMEDATA_GetSeasonID( GAMESYSTEM_GetGameData(gsys) );
  
  for( i = 0; i < FLDEFF_GRASS_MAX; i++ ){
    FLD_G3DOBJ_RES_HEADER_Init( &head );
    FLD_G3DOBJ_RES_HEADER_SetMdl(
        &head, handle, data_ArcIdxTbl[i][season].idx_mdl );
    FLD_G3DOBJ_RES_HEADER_SetAnmArcHandle( &head, handle );
    FLD_G3DOBJ_RES_HEADER_SetAnmArcIdx(
        &head, data_ArcIdxTbl[i][season].idx_anm );
    grass->res_idx_tbl[i] =
      FLD_G3DOBJ_CTRL_CreateResource( obj_ctrl, &head, FALSE );
  }
}

//--------------------------------------------------------------
/**
 * ���G�t�F�N�g�@���\�[�X�폜
 * @param fectrl FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
static void grass_DeleteResource( FLDEFF_GRASS *grass )
{
  int i;
  FLD_G3DOBJ_CTRL *obj_ctrl;
  
  obj_ctrl = FLDEFF_CTRL_GetFldG3dOBJCtrl( grass->fectrl );
  
  for( i = 0; i < FLDEFF_GRASS_MAX; i++ ){
    FLD_G3DOBJ_CTRL_DeleteResource( obj_ctrl, grass->res_idx_tbl[i] );
  }
}

//======================================================================
//	���G�t�F�N�g�@�^�X�N
//======================================================================
//--------------------------------------------------------------
/**
 * ���샂�f���p���G�t�F�N�g�@�ǉ�
 * @param fmmdl MMDL
 * @param FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDEFF_GRASS_SetMMdl( FLDEFF_CTRL *fectrl,
    MMDL *fmmdl, BOOL anm, FLDEFF_GRASSTYPE type )
{
  fx32 h;
  VecFx32 pos;
  FLDEFF_GRASS *grass;
  TASKHEADER_GRASS head;
  
  grass = FLDEFF_CTRL_GetEffectWork( fectrl, FLDEFF_PROCID_GRASS );
  head.eff_grass = grass;
  head.obj_ctrl = FLDEFF_CTRL_GetFldG3dOBJCtrl( fectrl );
  head.fmmdl = fmmdl;
  head.type = type;
  head.obj_id = MMDL_GetOBJID( fmmdl );
  head.zone_id = MMDL_GetZoneID( fmmdl );
  head.init_gx = MMDL_GetGridPosX( fmmdl );
  head.init_gz = MMDL_GetGridPosZ( fmmdl );
  MMDL_TOOL_GetCenterGridPos( head.init_gx, head.init_gz, &pos );
  pos.y = MMDL_GetVectorPosY( fmmdl );
  
  // ���[������̐ݒ�
  if( MMDL_CheckStatusBit( fmmdl, MMDL_STABIT_RAIL_MOVE ) )
  {
    pos.y += FX32_CONST(4);  
  }
  // �O���b�h����̐ݒ�
  else
  {
    MMDL_GetMapPosHeight( fmmdl, &pos, &h );
    pos.y = h;
    
    pos.y += NUM_FX32(4);
    pos.z += NUM_FX32(12);
  }
  
  head.pos = pos;

  FLDEFF_CTRL_AddTask(
      fectrl, &DATA_grassTaskHeader, NULL, anm, &head, 0 );
}

//--------------------------------------------------------------
/**
 * ���G�t�F�N�g�^�X�N�@������
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void grassTask_Init( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_GRASS *work = wk;
  const TASKHEADER_GRASS *head;
  
  head = FLDEFF_TASK_GetAddPointer( task );
  work->head = *head;
  MMDL_InitCheckSameData( head->fmmdl, &work->samedata );
  FLDEFF_TASK_SetPos( task, &head->pos );
  
  work->obj_idx = FLD_G3DOBJ_CTRL_AddObject( work->head.obj_ctrl,
      work->head.eff_grass->res_idx_tbl[work->head.type], 0, &head->pos );
  
  if( FLDEFF_TASK_GetAddParam(task) == FALSE ){ //�A�j������
    work->seq_no = 1;
  }
  
  FLDEFF_TASK_CallUpdate( task ); //������
}

//--------------------------------------------------------------
/**
 * ���G�t�F�N�g�^�X�N�@�폜
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void grassTask_Delete( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_GRASS *work = wk;
  FLD_G3DOBJ_CTRL_DeleteObject( work->head.obj_ctrl, work->obj_idx );
}

//--------------------------------------------------------------
/**
 * ���G�t�F�N�g�^�X�N�@�X�V
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void grassTask_Update( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_GRASS *work = wk;
  FLD_G3DOBJ_CTRL *obj_ctrl = work->head.obj_ctrl;
  
  switch( work->seq_no ){
  case 0: //�A�j���I���҂�
    if( FLD_G3DOBJ_CTRL_AnimeObject(
          work->head.obj_ctrl,work->obj_idx,FX32_ONE) == FALSE ){
      work->seq_no++;
    }
    break;
  case 1: //�A�j���I�� or �h�ꖳ��
    FLD_G3DOBJ_CTRL_SetAnimeFrame(
        obj_ctrl, work->obj_idx, 0, GRASS_SHAKE_FRAME );
    work->seq_no++;
  case 2:
    if( MMDL_CheckSameData(work->head.fmmdl,&work->samedata) == FALSE ){
      FLDEFF_TASK_CallDelete( task );
      return;
    }
    
    {
      int gx = MMDL_GetGridPosX( work->head.fmmdl );
      int gz = MMDL_GetGridPosZ( work->head.fmmdl );
      if( work->head.init_gx != gx || work->head.init_gz != gz ){
         FLDEFF_TASK_CallDelete( task );
         return;
      }
    }
    break;
  }
}

//--------------------------------------------------------------
/**
 * ���G�t�F�N�g�^�X�N�@�`��
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void grassTask_Draw( FLDEFF_TASK *task, void *wk )
{
#if 0 //field_g3dobj�𗘗p����
  VecFx32 pos;
  TASKWORK_GRASS *work = wk;
  GFL_G3D_OBJSTATUS status = {{0},{FX32_ONE,FX32_ONE,FX32_ONE},{0}};
  
  MTX_Identity33( &status.rotate );
  FLDEFF_TASK_GetPos( task, &status.trans );
  GFL_G3D_DRAW_DrawObjectCullingON( work->obj, &status );
#endif
}

//--------------------------------------------------------------
//  ���G�t�F�N�g�^�X�N�@�w�b�_�[
//--------------------------------------------------------------
static const FLDEFF_TASK_HEADER DATA_grassTaskHeader =
{
  sizeof(TASKWORK_GRASS),
  grassTask_Init,
  grassTask_Delete,
  grassTask_Update,
  grassTask_Draw,
};

//======================================================================
//  data
//======================================================================
static const GRASS_ARCIDX data_ArcIdxTbl[FLDEFF_GRASS_MAX][PMSEASON_TOTAL] =
{
  { //�Z����
    { NARC_fldeff_kusaeff_sp_nsbmd, NARC_fldeff_kusaeff_sp_nsbtp },
    { NARC_fldeff_kusaeff_sm_nsbmd, NARC_fldeff_kusaeff_sm_nsbtp },
    { NARC_fldeff_kusaeff_at_nsbmd, NARC_fldeff_kusaeff_at_nsbtp },
    { NARC_fldeff_kusaeff_wt_nsbmd, NARC_fldeff_kusaeff_wt_nsbtp },
  },
  { //�Z���� ��
    { NARC_fldeff_kusaeff_sp2_nsbmd, NARC_fldeff_kusaeff_sp2_nsbtp },
    { NARC_fldeff_kusaeff_sm2_nsbmd, NARC_fldeff_kusaeff_sm2_nsbtp },
    { NARC_fldeff_kusaeff_at2_nsbmd, NARC_fldeff_kusaeff_at2_nsbtp },
    { NARC_fldeff_kusaeff_wt2_nsbmd, NARC_fldeff_kusaeff_wt2_nsbtp },
  },
  { //������
    { NARC_fldeff_lgrass_sp_nsbmd, NARC_fldeff_lgrass_sp_nsbtp },
    { NARC_fldeff_lgrass_sm_nsbmd, NARC_fldeff_lgrass_sm_nsbtp },
    { NARC_fldeff_lgrass_at_nsbmd, NARC_fldeff_lgrass_at_nsbtp },
    { NARC_fldeff_lgrass_wt_nsbmd, NARC_fldeff_lgrass_wt_nsbtp },
  },
  { //������ ��
    { NARC_fldeff_lgrass_sp2_nsbmd, NARC_fldeff_lgrass_sp2_nsbtp },
    { NARC_fldeff_lgrass_sm2_nsbmd, NARC_fldeff_lgrass_sm2_nsbtp },
    { NARC_fldeff_lgrass_at2_nsbmd, NARC_fldeff_lgrass_at2_nsbtp },
    { NARC_fldeff_lgrass_wt2_nsbmd, NARC_fldeff_lgrass_wt2_nsbtp },
  },
  { //����n�� �P�V�[�Y���̂�
    { NARC_fldeff_kusaeff_sn_nsbmd, NARC_fldeff_kusaeff_sn_nsbtp },
    { NARC_fldeff_kusaeff_sn_nsbmd, NARC_fldeff_kusaeff_sn_nsbtp },
    { NARC_fldeff_kusaeff_sn_nsbmd, NARC_fldeff_kusaeff_sn_nsbtp },
    { NARC_fldeff_kusaeff_sn_nsbmd, NARC_fldeff_kusaeff_sn_nsbtp },
  },
  { //����n�� �� �P�V�[�Y���̂�
    { NARC_fldeff_kusaeff_sn2_nsbmd, NARC_fldeff_kusaeff_sn2_nsbtp },
    { NARC_fldeff_kusaeff_sn2_nsbmd, NARC_fldeff_kusaeff_sn2_nsbtp },
    { NARC_fldeff_kusaeff_sn2_nsbmd, NARC_fldeff_kusaeff_sn2_nsbtp },
    { NARC_fldeff_kusaeff_sn2_nsbmd, NARC_fldeff_kusaeff_sn2_nsbtp },
  },
};
