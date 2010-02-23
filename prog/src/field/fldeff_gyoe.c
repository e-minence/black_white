//======================================================================
/**
 * @file	fldeff_gyoe.c
 * @brief	�t�B�[���h �т�����}�[�N�G�t�F�N�g
 * @author	kagaya
 * @date	05.07.13
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "fieldmap.h"
#include "field_effect.h"
#include "fldmmdl.h"

#include "fldeff_gyoe.h"

#include "sound/pm_sndsys.h"

//======================================================================
//  define
//======================================================================
#define GYOE_FLDOBJ_Y_OFFSET (NUM_FX32(32)) ///<�t�B�[���hOBJ�����Y�I�t�Z�b�g
#define GYOE_FLDOBJ_Z_OFFSET (0x1000) ///<�t�B�[���hOBJ�����Z�I�t�Z�b�g
#define GYOE_FLDOBJ_Y_MOVE_START (0x6000) ///<�M���G�[����
#define GYOE_END_FRAME (30) ///<�M���G�[�I���t���[��	

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
///	FLDEFF_GYOE�^
//--------------------------------------------------------------
typedef struct _TAG_FLDEFF_GYOE FLDEFF_GYOE;

//--------------------------------------------------------------
///	FLDEFF_GYOE�\����
//--------------------------------------------------------------
struct _TAG_FLDEFF_GYOE
{
	FLDEFF_CTRL *fectrl;
  GFL_G3D_RES *g3d_res_mdl[FLDEFF_GYOETYPE_MAX];
  GFL_G3D_RND *g3d_rnd[FLDEFF_GYOETYPE_MAX];
  GFL_G3D_OBJ *g3d_obj[FLDEFF_GYOETYPE_MAX];
};

//--------------------------------------------------------------
/// TASKHEADER_GYOE
//--------------------------------------------------------------
typedef struct
{
  FLDEFF_GYOETYPE type;
  BOOL se_play;
  
  FLDEFF_GYOE *eff_gyoe;
  const MMDL *mmdl;
  MMDL_CHECKSAME_DATA samedata;
}TASKHEADER_GYOE;

//--------------------------------------------------------------
/// TASKWORK_GYOE
//--------------------------------------------------------------
typedef struct
{
  TASKHEADER_GYOE head;
  
  int seq_no;
  int frame;
  BOOL end_flag;
  fx32 offs_y;
  fx32 move_y;
}TASKWORK_GYOE;

//======================================================================
//	�v���g�^�C�v
//======================================================================
static void gyoe_InitResource( FLDEFF_GYOE *gyoe );
static void gyoe_DeleteResource( FLDEFF_GYOE *gyoe );

static const FLDEFF_TASK_HEADER DATA_gyoeTaskHeader;
static const FLDEFF_TASK_HEADER DATA_gyoeTaskHeader_only;
static const u32 data_ArcIdxTbl[FLDEFF_GYOETYPE_MAX];

//======================================================================
//	�т�����}�[�N�G�t�F�N�g�@�V�X�e��
//======================================================================
//--------------------------------------------------------------
/**
 * �т�����}�[�N�G�t�F�N�g ������
 * @param	fectrl		FLDEFF_CTRL *
 * @param heapID HEAPID
 * @retval	void*	�G�t�F�N�g�g�p���[�N
 */
//--------------------------------------------------------------
void * FLDEFF_GYOE_Init( FLDEFF_CTRL *fectrl, HEAPID heapID )
{
	FLDEFF_GYOE *gyoe;
	
	gyoe = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLDEFF_GYOE) );
	gyoe->fectrl = fectrl;
	gyoe_InitResource( gyoe );
	return( gyoe );
}

//--------------------------------------------------------------
/**
 * �т�����}�[�N�G�t�F�N�g �폜
 * @param fectrl FLDEFF_CTRL
 * @param	work	�G�t�F�N�g�g�p���[�N
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDEFF_GYOE_Delete( FLDEFF_CTRL *fectrl, void *work )
{
	FLDEFF_GYOE *gyoe = work;
  gyoe_DeleteResource( gyoe );
  GFL_HEAP_FreeMemory( gyoe );
}

//======================================================================
//	�т�����}�[�N�G�t�F�N�g�@���\�[�X
//======================================================================
//--------------------------------------------------------------
/**
 * �т�����}�[�N�G�t�F�N�g�@���\�[�X������
 * @param fectrl FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
static void gyoe_InitResource( FLDEFF_GYOE *gyoe )
{
  int i;
  BOOL ret;
  ARCHANDLE *handle;
  
  handle = FLDEFF_CTRL_GetArcHandleEffect( gyoe->fectrl );
  
  for( i = 0; i < FLDEFF_GYOETYPE_MAX; i++ ){
    gyoe->g3d_res_mdl[i]	=
      GFL_G3D_CreateResourceHandle( handle, data_ArcIdxTbl[i] );
    ret = GFL_G3D_TransVramTexture( gyoe->g3d_res_mdl[i] );
    GF_ASSERT( ret );
  
    gyoe->g3d_rnd[i] = GFL_G3D_RENDER_Create(
        gyoe->g3d_res_mdl[i], 0, gyoe->g3d_res_mdl[i] );
  
    gyoe->g3d_obj[i] = GFL_G3D_OBJECT_Create( gyoe->g3d_rnd[i], NULL, 0 );
  }
}

//--------------------------------------------------------------
/**
 * �т�����}�[�N�G�t�F�N�g�@���\�[�X�폜
 * @param fectrl FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
static void gyoe_DeleteResource( FLDEFF_GYOE *gyoe )
{
  int i;

  for( i = 0; i < FLDEFF_GYOETYPE_MAX; i++ ){
    GFL_G3D_OBJECT_Delete( gyoe->g3d_obj[i] );
	  GFL_G3D_RENDER_Delete( gyoe->g3d_rnd[i] );
 	  GFL_G3D_DeleteResource( gyoe->g3d_res_mdl[i] );
  }
}

//======================================================================
//	�т�����}�[�N�G�t�F�N�g�@�^�X�N
//======================================================================
//--------------------------------------------------------------
/**
 * ���샂�f���p�т�����}�[�N�G�t�F�N�g�@�ǉ�
 * @param mmdl MMDL
 * @param FLDEFF_CTRL*
 * @retval FLDEFF_TASK*
 */
//--------------------------------------------------------------
FLDEFF_TASK * FLDEFF_GYOE_SetMMdl( FLDEFF_CTRL *fectrl,
    const MMDL *mmdl, FLDEFF_GYOETYPE type, BOOL se_play )
{
  FLDEFF_GYOE *gyoe;
  TASKHEADER_GYOE head;
  
  head.type = type;
  head.se_play = se_play;
  
  head.eff_gyoe = FLDEFF_CTRL_GetEffectWork( fectrl, FLDEFF_PROCID_GYOE );
  head.mmdl = mmdl;
  MMDL_InitCheckSameData( mmdl, &head.samedata );
  
  if( se_play == TRUE ){
    u32 idx = 0;

    switch( type ){
    case FLDEFF_GYOETYPE_GYOE:
      idx = SEQ_SE_FLD_07;
      break;
    case FLDEFF_GYOETYPE_HATE:
      idx = SEQ_SE_SYS_62;
      break;
    case FLDEFF_GYOETYPE_ONPU:
      idx = SEQ_SE_SYS_63;
      break;
    case FLDEFF_GYOETYPE_TEN:
      idx = SEQ_SE_SYS_64;
      break;
    }

    PMSND_PlaySE( idx );
  }
  
  return( FLDEFF_CTRL_AddTask(fectrl,&DATA_gyoeTaskHeader,NULL,0,&head,0) );
}

//--------------------------------------------------------------
/**
 * ���샂�f���p�т�����}�[�N�G�t�F�N�g�@�ǉ��@���샂�f���ˑ�������
 * @param mmdl MMDL
 * @param FLDEFF_CTRL*
 * @retval FLDEFF_TASK*
 * @note ���o�I��or���샂�f�����S���ɏI�����܂��B
 */
//--------------------------------------------------------------
FLDEFF_TASK * FLDEFF_GYOE_SetMMdlNonDepend( FLDEFF_CTRL *fectrl,
    const MMDL *mmdl, FLDEFF_GYOETYPE type, BOOL se_play )
{
  FLDEFF_GYOE *gyoe;
  TASKHEADER_GYOE head;
  
  head.type = type;
  head.se_play = se_play;
  
  head.eff_gyoe = FLDEFF_CTRL_GetEffectWork( fectrl, FLDEFF_PROCID_GYOE );
  head.mmdl = mmdl;
  MMDL_InitCheckSameData( mmdl, &head.samedata );
  
  if( se_play == TRUE ){
    PMSND_PlaySE( SEQ_SE_FLD_07 );
  }
  
  return( FLDEFF_CTRL_AddTask(fectrl,&DATA_gyoeTaskHeader_only,NULL,0,&head,0) );
}


//--------------------------------------------------------------
/**
 * ���샂�f���p�т�����}�[�N�G�t�F�N�g�@�I���`�F�b�N
 * @param task FLDEFF_TASK
 * @retval
 */
//--------------------------------------------------------------
BOOL FLDEFF_GYOE_CheckEnd( FLDEFF_TASK *task )
{
  TASKWORK_GYOE *work = FLDEFF_TASK_GetWork( task );
  return( work->end_flag );
}

//--------------------------------------------------------------
/**
 * �т�����}�[�N�G�t�F�N�g�^�X�N�@������
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void gyoeTask_Init( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_GYOE *work = wk;
  const TASKHEADER_GYOE *head;
  
  head = FLDEFF_TASK_GetAddPointer( task );
  work->head = *head;
  work->move_y = GYOE_FLDOBJ_Y_MOVE_START;
  FLDEFF_TASK_CallUpdate( task ); //������
}

//--------------------------------------------------------------
/**
 * �т�����}�[�N�G�t�F�N�g�^�X�N�@�폜
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void gyoeTask_Delete( FLDEFF_TASK *task, void *wk )
{
//  TASKWORK_GYOE *work = wk;
}

//--------------------------------------------------------------
/**
 * �т�����}�[�N�G�t�F�N�g�^�X�N�@�X�V
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void gyoeTask_Update( FLDEFF_TASK *task, void *wk )
{
  VecFx32 pos;
  TASKWORK_GYOE *work = wk;
  
  if( MMDL_CheckSameData(work->head.mmdl,&work->head.samedata) == FALSE ){
    GF_ASSERT( 0 ); //�}�[�N�\�����ɏ���
    work->end_flag = TRUE;
    return;
  }
    
  switch( work->seq_no ){
  case 0:
    work->offs_y += work->move_y;
    if( work->offs_y ){
      work->move_y += -0x2000;
    }else{
      work->move_y = 0;
      work->seq_no++;
    }
    break;
  case 1:
    work->frame++;
    
    if( work->frame >= GYOE_END_FRAME ){
      work->seq_no++;
      work->end_flag = TRUE;
    }
  }
  
  MMDL_GetVectorPos( work->head.mmdl, &pos );
  pos.y += GYOE_FLDOBJ_Y_OFFSET + work->offs_y;

  // GRID��RAIL�ŃI�t�Z�b�g�̑�������ύX
  if( MMDL_CheckStatusBit( work->head.mmdl, MMDL_STABIT_RAIL_MOVE ) == FALSE ){
    // GRID
    pos.z += GYOE_FLDOBJ_Z_OFFSET;
  }else{
    VecFx16 way;
    MMDL_Rail_GetFrontWay( work->head.mmdl, &way );
    way.y = 0;
    VEC_Fx16Normalize( &way, &way );
    // RAIL
    pos.x += FX_Mul( GYOE_FLDOBJ_Z_OFFSET, way.x );
    pos.z += FX_Mul( GYOE_FLDOBJ_Z_OFFSET, way.z );
  }
  
  FLDEFF_TASK_SetPos( task, &pos );
}

//--------------------------------------------------------------
/**
 * �т�����}�[�N�G�t�F�N�g�^�X�N�@�X�V�@���샂�f���ˑ�������
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void gyoeTask_Update_only( FLDEFF_TASK *task, void *wk )
{
  VecFx32 pos;
  TASKWORK_GYOE *work = wk;
  
  if( work->end_flag == TRUE ||
      MMDL_CheckSameData(work->head.mmdl,&work->head.samedata) == FALSE ){
    FLDEFF_TASK_CallDelete( task ); //�A�j���I�� or ���샂�f�����S�@�폜
    return;
  }
  
  switch( work->seq_no ){
  case 0:
    work->offs_y += work->move_y;
    if( work->offs_y ){
      work->move_y += -0x2000;
    }else{
      work->move_y = 0;
      work->seq_no++;
    }
    break;
  case 1:
    work->frame++;
    
    if( work->frame >= GYOE_END_FRAME ){
      work->seq_no++;
      work->end_flag = TRUE;
    }
  }
  
  MMDL_GetVectorPos( work->head.mmdl, &pos );
  pos.y += GYOE_FLDOBJ_Y_OFFSET + work->offs_y;
  // GRID��RAIL�ŃI�t�Z�b�g�̑�������ύX
  if( MMDL_CheckStatusBit( work->head.mmdl, MMDL_STABIT_RAIL_MOVE ) == FALSE ){
    // GRID
    pos.z += GYOE_FLDOBJ_Z_OFFSET;
  }else{
    VecFx16 way;
    MMDL_Rail_GetFrontWay( work->head.mmdl, &way );
    way.y = 0;
    VEC_Fx16Normalize( &way, &way );
    // RAIL
    pos.x += FX_Mul( GYOE_FLDOBJ_Z_OFFSET, way.x );
    pos.z += FX_Mul( GYOE_FLDOBJ_Z_OFFSET, way.z );
  }
  FLDEFF_TASK_SetPos( task, &pos );
}

//--------------------------------------------------------------
/**
 * �т�����}�[�N�G�t�F�N�g�^�X�N�@�`��
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void gyoeTask_Draw( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_GYOE *work = wk;
  GFL_G3D_OBJSTATUS status = {{0},{FX32_ONE,FX32_ONE,FX32_ONE},{0}};
  MTX_Identity33( &status.rotate );
  FLDEFF_TASK_GetPos( task, &status.trans );
  GFL_G3D_DRAW_DrawObjectCullingON(
      work->head.eff_gyoe->g3d_obj[work->head.type], &status );
}

//--------------------------------------------------------------
//  �т�����}�[�N�G�t�F�N�g�^�X�N�@�w�b�_�[
//--------------------------------------------------------------
static const FLDEFF_TASK_HEADER DATA_gyoeTaskHeader =
{
  sizeof(TASKWORK_GYOE),
  gyoeTask_Init,
  gyoeTask_Delete,
  gyoeTask_Update,
  gyoeTask_Draw,
};

static const FLDEFF_TASK_HEADER DATA_gyoeTaskHeader_only =
{
  sizeof(TASKWORK_GYOE),
  gyoeTask_Init,
  gyoeTask_Delete,
  gyoeTask_Update_only,
  gyoeTask_Draw,
};

//======================================================================
//  
//======================================================================
//--------------------------------------------------------------
//  �e�A�[�J�C�u�C���f�b�N�X
//--------------------------------------------------------------
static const u32 data_ArcIdxTbl[FLDEFF_GYOETYPE_MAX] =
{
  NARC_fldeff_sisen_ef_nsbmd,
  NARC_fldeff_mark_hate_nsbmd,
  NARC_fldeff_mark_onpu_nsbmd,
  NARC_fldeff_mark_ten_nsbmd,
};
