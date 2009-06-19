//======================================================================
/**
 * @file	fldeff_namipoke.c
 * @brief	�t�B�[���h �g���|�P����
 * @author	kagaya
 * @data	05.07.13
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "fieldmap.h"
#include "field_effect.h"
#include "fldmmdl.h"

#include "fldeff_namipoke.h"

//======================================================================
//  define
//======================================================================
#define NAMIPOKE_SHAKE_VALUE (0x0400) ///<�����h�ꕝ
#define NAMIPOKE_SHAKE_MAX (FX32_ONE*4) ///<�h�ꕝ�ő�

#define NAMIPOKE_RIDE_Y_OFFSET (FX32_ONE*7) ///<�^�]��\���I�t�Z�b�gY
#define NAMIPOKE_RIDE_Z_OFFSET (FX32_ONE*4) ///<�^�]��\���I�t�Z�b�gZ

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
///	FLDEFF_NAMIPOKE�^
//--------------------------------------------------------------
typedef struct _TAG_FLDEFF_NAMIPOKE FLDEFF_NAMIPOKE;

//--------------------------------------------------------------
///	FLDEFF_NAMIPOKE�\����
//--------------------------------------------------------------
struct _TAG_FLDEFF_NAMIPOKE
{
	FLDEFF_CTRL *fectrl;
  GFL_G3D_RES *g3d_res_mdl;
};

//--------------------------------------------------------------
/// TASKHEADER_NAMIPOKE
//--------------------------------------------------------------
typedef struct
{
  FLDEFF_NAMIPOKE *eff_namipoke;
  MMDL *mmdl;
  u16 obj_id;
  u16 zone_id;
  u16 dir;
  u16 dmy;
  VecFx32 pos;
}TASKHEADER_NAMIPOKE;

//--------------------------------------------------------------
/// TASKWORK_NAMIPOKE
//--------------------------------------------------------------
typedef struct
{
  int seq_no;
  
  BOOL joint;
  
  u16 dir;
  fx32 shake_offs;
  fx32 shake_value;
  
  TASKHEADER_NAMIPOKE head;

  GFL_G3D_OBJ *obj;
  GFL_G3D_RND *obj_rnd;
}TASKWORK_NAMIPOKE;

//======================================================================
//	�v���g�^�C�v
//======================================================================
static void namipoke_InitResource( FLDEFF_NAMIPOKE *namipoke );
static void namipoke_DeleteResource( FLDEFF_NAMIPOKE *namipoke );

static const FLDEFF_TASK_HEADER DATA_namipokeTaskHeader;

//======================================================================
//	�g���|�P�����G�t�F�N�g�@�V�X�e��
//======================================================================
//--------------------------------------------------------------
/**
 * �g���|�P�����G�t�F�N�g ������
 * @param	fectrl		FLDEFF_CTRL *
 * @param heapID HEAPID
 * @retval	void*	�G�t�F�N�g�g�p���[�N
 */
//--------------------------------------------------------------
void * FLDEFF_NAMIPOKE_Init( FLDEFF_CTRL *fectrl, HEAPID heapID )
{
	FLDEFF_NAMIPOKE *namipoke;
	
	namipoke = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLDEFF_NAMIPOKE) );
	namipoke->fectrl = fectrl;
  
	namipoke_InitResource( namipoke );
	return( namipoke );
}

//--------------------------------------------------------------
/**
 * �g���|�P�����G�t�F�N�g �폜
 * @param fectrl FLDEFF_CTRL
 * @param	work	�G�t�F�N�g�g�p���[�N
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDEFF_NAMIPOKE_Delete( FLDEFF_CTRL *fectrl, void *work )
{
	FLDEFF_NAMIPOKE *namipoke = work;
  namipoke_DeleteResource( namipoke );
  GFL_HEAP_FreeMemory( namipoke );
}

//======================================================================
//	�g���|�P�����G�t�F�N�g�@���\�[�X
//======================================================================
//--------------------------------------------------------------
/**
 * �g���|�P�����G�t�F�N�g�@���\�[�X������
 * @param fectrl FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
static void namipoke_InitResource( FLDEFF_NAMIPOKE *namipoke )
{
  BOOL ret;
  ARCHANDLE *handle;
  
  handle = FLDEFF_CTRL_GetArcHandleEffect( namipoke->fectrl );
  
  namipoke->g3d_res_mdl	=
    GFL_G3D_CreateResourceHandle( handle, NARC_fldeff_sea_ride_nsbmd );
  ret = GFL_G3D_TransVramTexture( namipoke->g3d_res_mdl );
  GF_ASSERT( ret );
}

//--------------------------------------------------------------
/**
 * �g���|�P�����G�t�F�N�g�@���\�[�X�폜
 * @param fectrl FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
static void namipoke_DeleteResource( FLDEFF_NAMIPOKE *namipoke )
{
 	GFL_G3D_DeleteResource( namipoke->g3d_res_mdl );
}

//======================================================================
//	�g���|�P�����G�t�F�N�g�@�^�X�N
//======================================================================
//--------------------------------------------------------------
/**
 * ���샂�f���p�g���|�P�����G�t�F�N�g�@�ǉ�
 * @param FLDEFF_CTRL*
 * @param pos �\�����W
 * @param mmdl MMDL �|�P�����ɏ�铮�샂�f��
 * @param joint ���샂�f���Ƃ̓��� OFF=�W���C���g���Ȃ�
 * @retval nothing
 */
//--------------------------------------------------------------
FLDEFF_TASK * FLDEFF_NAMIPOKE_SetMMdl(
    FLDEFF_CTRL *fectrl, u16 dir, const VecFx32 *pos, MMDL *mmdl, BOOL joint )
{
  fx32 h;
  FLDEFF_TASK *task;
  TASKHEADER_NAMIPOKE head;
  FLDEFF_NAMIPOKE *namipoke;
  
  namipoke = FLDEFF_CTRL_GetEffectWork( fectrl, FLDEFF_PROCID_NAMIPOKE );
  head.eff_namipoke = namipoke;
  head.mmdl = mmdl;
  head.obj_id = MMDL_GetOBJID( mmdl );
  head.zone_id = MMDL_GetZoneID( mmdl );
  head.dir = dir;
  head.pos = *pos;
  
  task = FLDEFF_CTRL_AddTask(
      fectrl, &DATA_namipokeTaskHeader, &head.pos, joint, &head, 0 );
  return( task );
}

//--------------------------------------------------------------
/**
 * ���샂�f���p�g���|�P�����G�t�F�N�g�@�W���C���g�t���O�Z�b�g
 * @param task FLDEFF_TASK*
 * @param flag TRUE=�ڑ�,FALSE=��ڑ�
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDEFF_NAMIPOKE_SetJointFlag( FLDEFF_TASK *task, BOOL flag )
{
  TASKWORK_NAMIPOKE *work = FLDEFF_TASK_GetWork( task );
  work->joint = flag;
}

//--------------------------------------------------------------
/**
 * �g���|�P�����G�t�F�N�g�^�X�N�@������
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void namipokeTask_Init( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_NAMIPOKE *work = wk;
  const TASKHEADER_NAMIPOKE *head;
  
  head = FLDEFF_TASK_GetAddPointer( task );
  work->head = *head;
  FLDEFF_TASK_SetPos( task, &head->pos );
  
  work->dir = head->dir;
  
  work->obj_rnd =
    GFL_G3D_RENDER_Create(
        work->head.eff_namipoke->g3d_res_mdl, 0,
        work->head.eff_namipoke->g3d_res_mdl );
  
  work->obj = GFL_G3D_OBJECT_Create( work->obj_rnd, NULL, 0 );
  work->joint = FLDEFF_TASK_GetAddParam( task );
  work->shake_offs = FX32_ONE;
  work->shake_value = NAMIPOKE_SHAKE_VALUE;
  
  FLDEFF_TASK_CallUpdate( task ); //������
}

//--------------------------------------------------------------
/**
 * �g���|�P�����G�t�F�N�g�^�X�N�@�폜
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void namipokeTask_Delete( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_NAMIPOKE *work = wk;
  GFL_G3D_OBJECT_Delete( work->obj );
	GFL_G3D_RENDER_Delete( work->obj_rnd );
}

//--------------------------------------------------------------
/**
 * �g���|�P�����G�t�F�N�g�^�X�N�@�X�V
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void namipokeTask_Update( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_NAMIPOKE *work = wk;
  
  if( work->joint == FALSE ){ //�ڑ����Ȃ�
    return;
  }
 	
  { //����
    work->dir = MMDL_GetDirDisp( work->head.mmdl );
  }
  
  { //�h��
		work->shake_offs += work->shake_value;
		
		if( work->shake_offs >= NAMIPOKE_SHAKE_MAX ){
			work->shake_offs = NAMIPOKE_SHAKE_MAX;
			work->shake_value = -work->shake_value;
		}else if( work->shake_offs <= FX32_ONE ){
			work->shake_offs = FX32_ONE;
			work->shake_value = -work->shake_value;
		}
	}
  
  { //�^�]��ɗh���ǉ�
    VecFx32 offs;
    offs.x = 0;
    offs.y = work->shake_offs + NAMIPOKE_RIDE_Y_OFFSET;
    offs.z = NAMIPOKE_RIDE_Z_OFFSET;
    MMDL_SetVectorOuterDrawOffsetPos( work->head.mmdl,  &offs );
  }

  { //���W
    VecFx32 pos;
    MMDL_GetVectorPos( work->head.mmdl, &pos );
    pos.y += work->shake_offs - FX32_ONE;
    FLDEFF_TASK_SetPos( task, &pos );
  }
}

//--------------------------------------------------------------
/**
 * �g���|�P�����G�t�F�N�g�^�X�N�@�`��
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void namipokeTask_Draw( FLDEFF_TASK *task, void *wk )
{
  VecFx32 pos;
  TASKWORK_NAMIPOKE *work = wk;
  GFL_G3D_OBJSTATUS status = {{0},{FX32_ONE,FX32_ONE,FX32_ONE},{0}};
  u16 rtbl[DIR_MAX4][3] = {{0,0x8000,0},{0,0,0},{0,0xc000,0},{0,0x4000,0}};
  u16 *rot = rtbl[work->dir];
  GFL_CALC3D_MTX_CreateRot( rot[0], rot[1], rot[2], &status.rotate );
  FLDEFF_TASK_GetPos( task, &status.trans );
  GFL_G3D_DRAW_DrawObjectCullingON( work->obj, &status );
}

//--------------------------------------------------------------
//  �g���|�P�����G�t�F�N�g�^�X�N�@�w�b�_�[
//--------------------------------------------------------------
static const FLDEFF_TASK_HEADER DATA_namipokeTaskHeader =
{
  sizeof(TASKWORK_NAMIPOKE),
  namipokeTask_Init,
  namipokeTask_Delete,
  namipokeTask_Update,
  namipokeTask_Draw,
};
