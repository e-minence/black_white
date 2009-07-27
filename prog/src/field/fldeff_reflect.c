//======================================================================
/**
 * @file  fldeff_reflect.c
 * @brief  �t�B�[���h �f�荞��
 * @author  kagaya
 * @data  05.07.13
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "fieldmap.h"
#include "field_effect.h"
#include "fldmmdl.h"

#include "fldeff_reflect.h"

//======================================================================
//  define
//======================================================================
#ifdef DEBUG_ONLY_FOR_kagaya
//#define DEBUG_REFLECT_CHECK
#endif

#define REF_SCALE_X_DEF (FX16_ONE*4)
#define REF_SCALE_Y_DEF (FX16_ONE*4)
#define REF_SCALE_X_UP (REF_SCALE_X_DEF+(FX16_ONE/4))
#define REF_SCALE_X_DOWN (REF_SCALE_X_DEF-(FX16_ONE/4))
#define REF_SCALE_X_SPEED (FX16_ONE/64)

#if 0 //dp
#define REF_OFFS_Z (-FX32_ONE*7)
#else //wb
#define REF_OFFS_Z (FX32_ONE*12)
#endif

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
///  FLDEFF_REFLECT�^
//--------------------------------------------------------------
typedef struct _TAG_FLDEFF_REFLECT FLDEFF_REFLECT;

//--------------------------------------------------------------
///  FLDEFF_REFLECT�\����
//--------------------------------------------------------------
struct _TAG_FLDEFF_REFLECT
{
  FLDEFF_CTRL *fectrl;
};

//--------------------------------------------------------------
/// TASKHEADER_REFLECT
//--------------------------------------------------------------
typedef struct
{
  FLDEFF_REFLECT *eff_reflect;
  REFLECT_TYPE type;
  MMDL *mmdl;
  MMDLSYS *mmdlsys;
  GFL_BBDACT_SYS *bbdactsys;
}TASKHEADER_REFLECT;

//--------------------------------------------------------------
/// TASKWORK_REFLECT
//--------------------------------------------------------------
typedef struct
{
  fx32 scale_x;
  fx32 scale_val_x;
  BOOL flag_initact;
  TASKHEADER_REFLECT head;
  MMDL_BLACTWORK_USER actWork;
  MMDL_CHECKSAME_DATA samedata;
}TASKWORK_REFLECT;

//======================================================================
//  �v���g�^�C�v
//======================================================================
static void reflect_InitResource( FLDEFF_REFLECT *reflect );
static void reflect_DeleteResource( FLDEFF_REFLECT *reflect );

static void reflectTask_UpdateBlAct( u16 actID, void *wk );

static const FLDEFF_TASK_HEADER data_reflectTaskHeader;

//======================================================================
//  �f�荞�݁@�V�X�e��
//======================================================================
//--------------------------------------------------------------
/**
 * �f�荞�� ������
 * @param  fectrl    FLDEFF_CTRL *
 * @param heapID HEAPID
 * @retval  void*  �G�t�F�N�g�g�p���[�N
 */
//--------------------------------------------------------------
void * FLDEFF_REFLECT_Init( FLDEFF_CTRL *fectrl, HEAPID heapID )
{
  FLDEFF_REFLECT *reflect;
  
  reflect = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLDEFF_REFLECT) );
  reflect->fectrl = fectrl;
  
  reflect_InitResource( reflect );
  return( reflect );
}

//--------------------------------------------------------------
/**
 * �f�荞�� �폜
 * @param fectrl FLDEFF_CTRL
 * @param  work  �G�t�F�N�g�g�p���[�N
 * @retval  nothing
 */
//--------------------------------------------------------------
void FLDEFF_REFLECT_Delete( FLDEFF_CTRL *fectrl, void *work )
{
  FLDEFF_REFLECT *reflect = work;
  reflect_DeleteResource( reflect );
  GFL_HEAP_FreeMemory( reflect );
}

//======================================================================
//  �f�荞�݁@���\�[�X
//======================================================================
//--------------------------------------------------------------
/**
 * �f�荞�݁@���\�[�X������
 * @param fectrl FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
static void reflect_InitResource( FLDEFF_REFLECT *reflect )
{
}

//--------------------------------------------------------------
/**
 * �f�荞�݁@���\�[�X�폜
 * @param fectrl FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
static void reflect_DeleteResource( FLDEFF_REFLECT *reflect )
{
}

//======================================================================
//  �f�荞�݁@�^�X�N
//======================================================================
//--------------------------------------------------------------
/**
 * ���샂�f���p�f�荞�݁@�ǉ�
 * @param mmdl MMDL
 * @param FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDEFF_REFLECT_SetMMdl( MMDLSYS *mmdlsys,
    MMDL *mmdl, FLDEFF_CTRL *fectrl, REFLECT_TYPE type )
{
  TASKHEADER_REFLECT head;

  head.type = type;
  head.mmdl = mmdl;
  head.mmdlsys = mmdlsys;
  head.bbdactsys = MMDL_BLACTCONT_GetBbdActSys(
      MMDLSYS_GetBlActCont(mmdlsys) );
  head.eff_reflect = FLDEFF_CTRL_GetEffectWork(
      fectrl, FLDEFF_PROCID_REFLECT );
   
  FLDEFF_CTRL_AddTask( fectrl, &data_reflectTaskHeader, NULL, 0, &head, 0 );
}

//--------------------------------------------------------------
/**
 * �f�荞�݃^�X�N�@������
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void reflectTask_Init( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_REFLECT *work = wk;
  const TASKHEADER_REFLECT *head;
  head = FLDEFF_TASK_GetAddPointer( task );
  
  work->head = *head;
  work->scale_x = REF_SCALE_X_DEF;
  work->scale_val_x = REF_SCALE_X_SPEED;
  
  if( work->head.type == REFLECT_TYPE_MIRROR ){ //�L�k����
    work->scale_val_x = 0;
  }
  
  MMDL_InitCheckSameData( head->mmdl, &work->samedata ); 
  
//  //������...�͐e�̃t���O�������^�C�~���O���Ԃɍ���Ȃ������l�����Ė���
//  FLDEFF_TASK_CallUpdate( task );
}

//--------------------------------------------------------------
/**
 * �f�荞�݃^�X�N�@�폜
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void reflectTask_Delete( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_REFLECT *work = wk;
  
  if( work->flag_initact ){
    MMDL_BLACTCONT_USER_DeleteActor( work->head.mmdlsys, &work->actWork );
  }
}

//--------------------------------------------------------------
/**
 * �f�荞�݃^�X�N�@�X�V
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void reflectTask_Update( FLDEFF_TASK *task, void *wk )
{
  u32 actID;
  TASKWORK_REFLECT *work = wk;
  
  if( MMDL_CheckSameData(work->head.mmdl,&work->samedata) == FALSE ||
    MMDL_CheckStatusBitReflect(work->head.mmdl) == FALSE ){
    FLDEFF_TASK_CallDelete( task );
    return;
  }
  
  actID = MMDL_CallDrawGetProc( work->head.mmdl, 0 );

  if( work->flag_initact == FALSE ){
    if( actID == MMDL_BLACTID_NULL ){ //���샂�f���r���{�[�h�ǉ��܂�
      return;
    }else{
      VecFx32 pos = {0,0,0};
      u16 code = MMDL_GetOBJCode( work->head.mmdl );
      MMDL_BLACTCONT_USER_AddActor( work->head.mmdlsys,
          code, &work->actWork, &pos,
          reflectTask_UpdateBlAct, work );
      work->flag_initact = TRUE;
    }
  }
  
  if( work->actWork.actID == MMDL_BLACTID_NULL ){ //�A�N�^�[�ǉ��܂�
    return;
  }
  
  work->scale_x += work->scale_val_x;
  
 	if( work->scale_x >= REF_SCALE_X_UP ){
		work->scale_x = REF_SCALE_X_UP;
		work->scale_val_x = -work->scale_val_x;
	}else if( work->scale_x <= REF_SCALE_X_DOWN ){
		work->scale_x = REF_SCALE_X_DOWN;
		work->scale_val_x = -work->scale_val_x;
	}
	 
  reflectTask_UpdateBlAct( work->actWork.actID, work );
} 

//--------------------------------------------------------------
/**
 * �f�荞�݃^�X�N�@�`��
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void reflectTask_Draw( FLDEFF_TASK *task, void *wk )
{
// TASKWORK_REFLECT *work = wk;
}

//--------------------------------------------------------------
/**
 * �f�荞�݃^�X�N ���샂�f���Ƃ̓���
 * @param actID �r���{�[�h�A�N�^�[ID
 * @param wk TASKWORK_REFLECT
 * @retval nothing
 */
//--------------------------------------------------------------
static void reflectTask_UpdateBlAct( u16 actID, void *wk )
{
  u16 ret;
  TASKWORK_REFLECT *work = wk;
  GFL_BBDACT_SYS *bbdactsys = work->head.bbdactsys;
  GFL_BBD_SYS *bbdsys = GFL_BBDACT_GetBBDSystem( bbdactsys );
  u32 m_actID = MMDL_CallDrawGetProc( work->head.mmdl, 0 );
  
  GF_ASSERT( actID != MMDL_BLACTID_NULL ); //�e���f���r���{�[�h�����B
  
  ret = GFL_BBDACT_GetDrawEnable( bbdactsys, m_actID );
  GFL_BBDACT_SetDrawEnable( bbdactsys, actID, ret );
  
  ret = GFL_BBDACT_GetAnimeEnable( bbdactsys, m_actID );
  GFL_BBDACT_SetAnimeEnable( bbdactsys, actID, ret );
  
  if( ret == TRUE ){
    ret = GFL_BBDACT_GetAnimeIdx( bbdactsys, m_actID );
    GFL_BBDACT_SetAnimeIdx( bbdactsys, actID, ret );
    ret = GFL_BBDACT_GetAnimeFrmIdx( bbdactsys, m_actID );
    GFL_BBDACT_SetAnimeFrmIdx( bbdactsys, actID, ret );
  }
  
  {
    fx32 x,y,z;
    VecFx32 pos;
#if 0 //dp
 	  fx32 offs[REFLECT_TYPE_MAX] = {
	    NUM_FX32(12),
		  NUM_FX32(16),
		  NUM_FX32(12),
	  };
#else
 	  fx32 offs[REFLECT_TYPE_MAX] = {
	    NUM_FX32(12*2)+NUM_FX32(1),
		  NUM_FX32(16*2)+NUM_FX32(1),
		  NUM_FX32(12*2)+NUM_FX32(1),
	  };
#endif
    MMDL_GetVectorDrawOffsetPos( work->head.mmdl, &pos );
    x = pos.x;
    z = -pos.z;
    
    MMDL_GetVectorPos( work->head.mmdl, &pos );
    pos.x += x;
    pos.z += z + REF_OFFS_Z;
    
	  if( MMDL_GetMapPosHeight(work->head.mmdl,&pos,&y) == FALSE ){
      pos.y = 0; //�����擾�G���[
    }else{
      pos.y -= offs[work->head.type];
    }
    
    GFL_BBD_SetObjectTrans( bbdsys, actID, &pos );
  }
  
  {
    BOOL flip = TRUE;
    int m_idx = GFL_BBDACT_GetBBDActIdxResIdx( bbdactsys, m_actID );
    int idx = GFL_BBDACT_GetBBDActIdxResIdx( bbdactsys, actID );
    fx16 sx = work->scale_x;
    fx16 sy = REF_SCALE_Y_DEF;
    GFL_BBD_SetObjectSiz( bbdsys, idx, &sx, &sy );
    GFL_BBD_SetObjectFlipT( bbdsys, idx, &flip );
    
    flip = GFL_BBD_GetObjectFlipS( bbdsys, m_idx );
    GFL_BBD_SetObjectFlipS( bbdsys, idx, &flip );
  }
  
#ifdef DEBUG_REFLECT_CHECK
  if( MMDL_GetOBJID(work->head.mmdl) == 0xff ){
    if( GFL_UI_KEY_GetCont() & PAD_BUTTON_A ){
      KAGAYA_Printf( "���@���T�C�Y 0x%x\n", work->scale_x );
    }
  }
#endif
}

//======================================================================
//  data
//======================================================================
//--------------------------------------------------------------
//  �f�荞�݃^�X�N�@�w�b�_�[
//--------------------------------------------------------------
static const FLDEFF_TASK_HEADER data_reflectTaskHeader =
{
  sizeof(TASKWORK_REFLECT),
  reflectTask_Init,
  reflectTask_Delete,
  reflectTask_Update,
  reflectTask_Draw,
};
