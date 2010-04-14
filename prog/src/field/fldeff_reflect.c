//======================================================================
/**
 * @file  fldeff_reflect.c
 * @brief  �t�B�[���h �f�荞��
 * @author  kagaya
 * @date  05.07.13
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

#define REF_SCALE_X_UP (FX16_ONE/4)
#define REF_SCALE_X_DOWN (-FX16_ONE/4)
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
  fx32 scale_x_org;
  fx32 scale_y_org;
  fx32 scale_x;
  fx32 scale_val_x;
  u8 flag_initact;
  u8 flag_initfunc;
  u8 padding[2];
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
static void reflectBlAct_Update(
    GFL_BBDACT_SYS *bbdactsys, int actID, void *wk );

static const FLDEFF_TASK_HEADER data_reflectTaskHeader;
static const fx32 data_offsetY[REFLECT_TYPE_MAX];
static const fx32 data_offsetZ[MMDL_BLACT_MDLSIZE_MAX];

#ifdef PM_DEBUG
static BOOL debug_CheckMMdl( const MMDL *mmdl );
#endif

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
   
#ifdef PM_DEBUG
  debug_CheckMMdl( mmdl );
#endif

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
  work->scale_x_org = 0;
  work->scale_x = 0;
  work->scale_val_x = REF_SCALE_X_SPEED;
  
  if( work->head.type == REFLECT_TYPE_MIRROR ){ //�L�k����
    work->scale_val_x = 0;
  }
  
  MMDL_InitCheckSameData( head->mmdl, &work->samedata ); 
  
#ifdef PM_DEBUG
  debug_CheckMMdl( work->head.mmdl );
#endif
  
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
    MMDL_CheckMoveBitReflect(work->head.mmdl) == FALSE ){
    FLDEFF_TASK_CallDelete( task );
    return;
  }
  
#ifdef PM_DEBUG
  if( debug_CheckMMdl(work->head.mmdl) == TRUE ){
    return;
  }
#endif

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
  
#if 0  
  if( work->flag_initfunc == FALSE ){
    GFL_BBDACT_SetFunc( work->head.bbdactsys,
        work->actWork.actID, reflectBlAct_Update );
    work->flag_initfunc = TRUE;
  }
#else
  if( work->flag_initfunc == FALSE ){
    u16 size_x,size_y;
    MMDL_BLACTCONT_GetMMdlDrawSize( work->head.mmdl, &size_x, &size_y );
    work->scale_x_org = size_x;
    work->scale_y_org = size_y;
    work->scale_x = work->scale_x_org;
    work->flag_initfunc = TRUE;
  }
#endif
  
  work->scale_x += work->scale_val_x;
  
 	if( work->scale_x >= (work->scale_x_org+REF_SCALE_X_UP) ){
		work->scale_x = work->scale_x_org + REF_SCALE_X_UP;
		work->scale_val_x = -work->scale_val_x;
	}else if( work->scale_x <= (work->scale_x_org+REF_SCALE_X_DOWN) ){
		work->scale_x = work->scale_x_org + REF_SCALE_X_DOWN;
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
  
#if 0  
  ret = GFL_BBDACT_GetAnimeEnable( bbdactsys, m_actID );
  GFL_BBDACT_SetAnimeEnable( bbdactsys, actID, ret );
  
  if( ret == TRUE ){
    ret = GFL_BBDACT_GetAnimeIdx( bbdactsys, m_actID );
    GFL_BBDACT_SetAnimeIdx( bbdactsys, actID, ret );
    ret = GFL_BBDACT_GetAnimeFrmIdx( bbdactsys, m_actID );
    GFL_BBDACT_SetAnimeFrmIdx( bbdactsys, actID, ret );
  }
#else
  GFL_BBDACT_SetAnimeEnable( bbdactsys, actID, FALSE );
#endif
  
  {
    fx32 x,y,z;
    VecFx32 pos;
    fx32 offs;
    const MMDL *mmdl;
    const OBJCODE_PARAM *param;
    
    mmdl = work->head.mmdl;
    param = MMDL_GetOBJCodeParam( mmdl, MMDL_GetOBJCode(mmdl) );
    
    MMDL_GetVectorDrawOffsetPos( mmdl, &pos );
    x = pos.x;
    z = -pos.z;
    
    offs = data_offsetZ[param->mdl_size];
    
    MMDL_GetVectorPos( mmdl, &pos );
    pos.x += x;
    pos.z += z + offs;
    
    offs = data_offsetY[work->head.type];

	  if( MMDL_GetMapPosHeight(mmdl,&pos,&y) == FALSE ){ 
      //�����擾�G���[
      #if 1
      pos.y = 0;
      #else
      pos.y -= offs[0];
      #endif
    }else{
      pos.y -= offs;
    }
    
    GFL_BBD_SetObjectTrans( bbdsys, actID, &pos );
  }
  
  {
    BOOL flip = TRUE;
    int m_idx = GFL_BBDACT_GetBBDActIdxResIdx( bbdactsys, m_actID );
    int idx = GFL_BBDACT_GetBBDActIdxResIdx( bbdactsys, actID );
    fx16 sx = work->scale_x;
    fx16 sy = work->scale_y_org;
    
    {
      u16 res_idx = 0;
      u16 cell_idx = 0;
      GFL_BBD_GetObjectResIdx( bbdsys, m_idx, &res_idx );
      GFL_BBD_SetObjectResIdx( bbdsys, idx, &res_idx );
      
			GFL_BBD_GetObjectCelIdx( bbdsys, m_idx, &cell_idx );
			GFL_BBD_SetObjectCelIdx( bbdsys, idx, &cell_idx );
    }
    
    GFL_BBD_SetObjectSiz( bbdsys, idx, &sx, &sy );
    GFL_BBD_SetObjectFlipT( bbdsys, idx, &flip );
    
    flip = GFL_BBD_GetObjectFlipS( bbdsys, m_idx ); //��Flip�󂯌p��
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

static void reflectBlAct_Update(
    GFL_BBDACT_SYS *bbdactsys, int actID, void *wk )
{
  reflectTask_UpdateBlAct( actID, wk );
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

//--------------------------------------------------------------
/// �f�荞�ݕ\���ʒu
//--------------------------------------------------------------
static const fx32 data_offsetY[REFLECT_TYPE_MAX] =
{
  NUM_FX32(12*2)+NUM_FX32(1),
  NUM_FX32(16*2)+NUM_FX32(1),
  NUM_FX32(12*2)+NUM_FX32(1),
};

static const fx32 data_offsetZ[MMDL_BLACT_MDLSIZE_MAX] =
{
  REF_OFFS_Z, //32
  REF_OFFS_Z/2, //16
  REF_OFFS_Z*2+FX32_ONE*1, //64
};

//======================================================================
//  define
//======================================================================
#ifdef PM_DEBUG
//--------------------------------------------------------------
//  ���̂��f�荞�ݑΏۈȊO��OBJ�ɑ΂���
//  �f�荞�݂��������錻�ۂ��N���Ă���A���ݒ�����
//  ����܂ł̎b��Ώ�
//  ���C�x���g�f�[�^���݂�������Ȃ��B
//�@�ЂƂ܂��`�F�b�N�Ƃ��Ă��̕����̓f�o�b�O�p�ɋ@�\�����Ă����B
//--------------------------------------------------------------
static BOOL debug_CheckMMdl( const MMDL *mmdl )
{
  u16 code = MMDL_GetOBJCode( mmdl );
  const OBJCODE_PARAM *prm = MMDL_GetOBJCodeParam( mmdl, code );
    
  if( prm->draw_type != MMDL_DRAWTYPE_BLACT ){
    GF_ASSERT( 0 && "FLDEFF REFLECT NOT REFLECT OBJ" );
    return( TRUE );
  }
  
  if( prm->reflect_type == MMDL_REFLECT_NON ){
    GF_ASSERT( 0 && "FLDEFF REFLECT NOT REFLECT OBJ" );
    return( TRUE );
  }
  return( FALSE );
}
#endif

