//======================================================================
/**
 * @file	fldeff_shadow.c
 * @brief	�t�B�[���hOBJ�e
 * @author	kagaya
 * @date	05.07.13
 *
 * 2010.04.14 tamada  �������׌y���̂��߁AG3DOBJ�łȂ��r���{�[�h�`��ɐ؂�ւ�
 * 2010.04.22 tamada  DP����̎��ԑѐ���ȂǃR�����g�������폜
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "fieldmap.h"
#include "field_effect.h"
#include "fldmmdl.h"
#include "fldeff_shadow.h"

#include "debug/debug_flg.h" //DEBUG_FLG_�`
//======================================================================
//	define
//======================================================================


//======================================================================
//	struct
//======================================================================
//--------------------------------------------------------------
///	FE_SHADOW�^
//--------------------------------------------------------------
typedef struct _TAG_FLDEFF_SHADOW FLDEFF_SHADOW;

//--------------------------------------------------------------
///	�e�G�t�F�N�g�S�̗p�̃��[�N��`
//--------------------------------------------------------------
struct _TAG_FLDEFF_SHADOW
{
	FLDEFF_CTRL *fectrl;

  VecFx32 scale;

  GFL_BBDACT_SYS *bbdactsys;
  GFL_BBD_SYS *bbdsys;
  int resIdx;
};

//--------------------------------------------------------------
/// �e�G�t�F�N�g�^�X�N�o�^�p�f�[�^��`
//--------------------------------------------------------------
typedef struct
{
  FLDEFF_SHADOW *eff_shadow;  ///<�e�G�t�F�N�g�S�̃��[�N�ւ̃|�C���^
  MMDL *fmmdl;                ///<�e���ǐ�����Ώۂ̓��샂�f���ւ̃|�C���^
}SHADOW_TASKHEADER;

//--------------------------------------------------------------
/// �ʂ̉e�G�t�F�N�g���Ƃ̃��[�N��`
//--------------------------------------------------------------
typedef struct
{
  u32 vanish_flag;              ///<�\������t���O
  FLDEFF_SHADOW *eff_shadow;    ///<�e�G�t�F�N�g�S�̃��[�N�ւ̃|�C���^
  MMDL *fmmdl;                  ///<�e���ǐ�����Ώۂ̓��샂�f���ւ̃|�C���^
  MMDL_CHECKSAME_DATA samedata; ///<���샂�f�����ꐫ�`�F�b�N�p���[�N
  int bbdUnitIdx;               ///<�\�����Ă���BBDACT�̃��j�b�g�C���f�b�N�X
}TASKWORK_SHADOW;

//======================================================================
//	�v���g�^�C�v
//======================================================================
static void shadow_InitResource( FLDEFF_SHADOW *sd );
static void shadow_DeleteResource( FLDEFF_SHADOW *sd );

static const FLDEFF_TASK_HEADER DATA_shadowTaskHeader;

//======================================================================
//	�e�@�V�X�e��
//======================================================================
//--------------------------------------------------------------
/**
 * �e������
 * @param	fectrl		FLDEFF_CTRL *
 * @param heapID HEAPID
 * @retval	void*	�G�t�F�N�g�g�p���[�N
 */
//--------------------------------------------------------------
void * FLDEFF_SHADOW_Init( FLDEFF_CTRL *fectrl, HEAPID heapID )
{
	FLDEFF_SHADOW *sd;
	
	sd = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLDEFF_SHADOW) );
	sd->fectrl = fectrl;
  
  { //wb
    VecFx32 scale = {FX32_ONE,FX32_ONE,0xc00};
    sd->scale = scale;
  }
  
  {
    FIELDMAP_WORK * fieldmap = FLDEFF_CTRL_GetFieldMapWork( fectrl );
    sd->bbdactsys = FIELDMAP_GetSubBbdActSys( fieldmap );
    sd->bbdsys = GFL_BBDACT_GetBBDSystem( sd->bbdactsys );
  }

	shadow_InitResource( sd );
	return( sd );
}

//--------------------------------------------------------------
/**
 * �e�폜
 * @param fectrl FLDEFF_CTRL
 * @param	work	�G�t�F�N�g�g�p���[�N
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDEFF_SHADOW_Delete( FLDEFF_CTRL *fectrl, void *work )
{
	FLDEFF_SHADOW *sd = work;
  shadow_DeleteResource( sd );
  GFL_HEAP_FreeMemory( sd );
}

//--------------------------------------------------------------
/**
 * �e�@�O���[�o���k�ڒl���w��
 * @param fectrl FLDEFF_CTRL
 * @pram scale �k�ڒl
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDEFF_SHADOW_SetGlobalScale( FLDEFF_CTRL *fectrl, const VecFx32 *scale )
{
	FLDEFF_SHADOW *sd = FLDEFF_CTRL_GetEffectWork(
      fectrl, FLDEFF_PROCID_SHADOW  );
  sd->scale = *scale;
}

//======================================================================
//	�e�@���\�[�X
//======================================================================
//--------------------------------------------------------------
/**
 * �e�@���\�[�X������
 * @param sd FLDEFF_SHADOW
 * @retval nothing
 */
//--------------------------------------------------------------
static void shadow_InitResource( FLDEFF_SHADOW *sd )
{
  ARCHANDLE *handle;
  GFL_G3D_RES *g3d_res;
  
  handle = FLDEFF_CTRL_GetArcHandleEffect( sd->fectrl );
  
  g3d_res	= GFL_G3D_CreateResourceHandle( handle, NARC_fldeff_kage_nsbmd );
  {
    GFL_BBDACT_G3DRESDATA data;
    data.g3dres = g3d_res;
    data.texFmt = GFL_BBD_TEXFMT_PAL4;
    data.texSiz = GFL_BBD_TEXSIZ_16x16;
    data.celSizX = 16;
    data.celSizY = 16;
    data.dataCut = GFL_BBDACT_RESTYPE_DATACUT;

    sd->resIdx = GFL_BBDACT_AddResourceG3DResUnit( sd->bbdactsys, &data, 1 );
  }
}

//--------------------------------------------------------------
/**
 * �e�@���\�[�X�폜
 * @param sd FLDEFF_SHADOW
 * @retval nothing
 */
//--------------------------------------------------------------
static void shadow_DeleteResource( FLDEFF_SHADOW *sd )
{
  GFL_BBDACT_RemoveResourceUnit( sd->bbdactsys, sd->resIdx, 1 );
}

//======================================================================
//	�e�@�^�X�N
//======================================================================
//--------------------------------------------------------------
/**
 * ���샂�f���p�e�@�ǉ�
 * @param fmmdl MMDL
 * @param FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDEFF_SHADOW_SetMMdl( MMDL *fmmdl, FLDEFF_CTRL *fectrl )
{
  FLDEFF_SHADOW *sd;
  SHADOW_TASKHEADER head;
  
  sd = FLDEFF_CTRL_GetEffectWork( fectrl, FLDEFF_PROCID_SHADOW );
  head.eff_shadow = sd;
  head.fmmdl = fmmdl;
  
  FLDEFF_CTRL_AddTask(
      fectrl, &DATA_shadowTaskHeader, NULL, 0, &head, 0 );
}

//--------------------------------------------------------------
/**
 * �e�^�X�N�@������
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void shadowTask_Init( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_SHADOW *work = wk;
  const SHADOW_TASKHEADER *head;
  
  head = FLDEFF_TASK_GetAddPointer( task );
  work->eff_shadow = head->eff_shadow;
  work->fmmdl = head->fmmdl;
  MMDL_InitCheckSameData( head->fmmdl, &work->samedata );

  {
    GFL_BBDACT_ACTDATA actData;
    actData.resID = 0;  //ResUnit���I�t�Z�b�g�w��
    actData.sizX = FX32_ONE;
    actData.sizY = FX32_ONE;
    actData.alpha = 19;
    actData.drawEnable = TRUE;
    actData.lightMask = GFL_BBD_LIGHTMASK_0;
    actData.trans = (VecFx32){ 0, 0, 0 };
    actData.func = NULL;
    actData.work = work;
    work->bbdUnitIdx = GFL_BBDACT_AddActEx(
        work->eff_shadow->bbdactsys, work->eff_shadow->resIdx,
        &actData, 1, GFL_BBD_DRAWMODE_XZ_FLAT_BILLBORD );
  }
  FLDEFF_TASK_CallUpdate( task ); //���A���W�𔽉f����
}

//--------------------------------------------------------------
/**
 * �e�^�X�N�@�폜
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void shadowTask_Delete( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_SHADOW *work = wk;
  GFL_BBDACT_RemoveAct( work->eff_shadow->bbdactsys, work->bbdUnitIdx, 1 );
}

//--------------------------------------------------------------
/**
 * �e�^�X�N�@�X�V
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void shadowTask_Update( FLDEFF_TASK *task, void *wk )
{
  VecFx32 pos;
  TASKWORK_SHADOW *work = wk;
  
  if( MMDL_CheckSameData(work->fmmdl,&work->samedata) == FALSE ){
    FLDEFF_TASK_CallDelete( task );
    return;
  }else{
    const MMDLSYS *mmdlsys = MMDL_GetMMdlSys( work->fmmdl );
    
    if( MMDLSYS_CheckJoinShadow(mmdlsys) == FALSE ){
      FLDEFF_TASK_CallDelete( task );
      return;
    }
  }

#ifdef PM_DEBUG  
  if( DEBUG_FLG_GetFlg(DEBUG_FLG_DisableTrainerEye) ||
      MMDL_CheckStatusBitVanish(work->fmmdl) == TRUE ||
      MMDL_CheckMoveBit(work->fmmdl,MMDL_MOVEBIT_SHADOW_VANISH) ){
    work->vanish_flag = TRUE;
  }
#else
  if( MMDL_CheckStatusBitVanish(work->fmmdl) == TRUE ||
      MMDL_CheckMoveBit(work->fmmdl,MMDL_MOVEBIT_SHADOW_VANISH) ){
    work->vanish_flag = TRUE;
  }
#endif
  else
  {
    work->vanish_flag = FALSE;
    MMDL_GetVectorPos( work->fmmdl, &pos );
    pos.y += NUM_FX32(-4);
#ifdef MMDL_BBD_DRAW_OFFS_Z_USE 
    pos.z += NUM_FX32(2)+0x800;
#endif
    FLDEFF_TASK_SetPos( task, &pos );
  }
}

//--------------------------------------------------------------
/**
 * �e�^�X�N�@�`��
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void shadowTask_Draw( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_SHADOW *work = wk;
  int bbdIdx = work->bbdUnitIdx + 0;  //BBD��INDEX == BBDACT��UNITIDX + Unit��Idx
  
  if( work->vanish_flag == FALSE ){
    int flag = TRUE;
    VecFx32 pos;
    MMDL_GetVectorPos( work->fmmdl, &pos );

    GFL_BBD_SetObjectDrawEnable( work->eff_shadow->bbdsys, bbdIdx, &flag );
    pos.y += FX32_CONST(+1);
    GFL_BBD_SetObjectTrans( work->eff_shadow->bbdsys, bbdIdx, &pos );
  } else {
    int flag = FALSE;
    GFL_BBD_SetObjectDrawEnable( work->eff_shadow->bbdsys, bbdIdx, &flag );
  }
}

//--------------------------------------------------------------
//  �e�^�X�N�@�w�b�_�[
//--------------------------------------------------------------
static const FLDEFF_TASK_HEADER DATA_shadowTaskHeader =
{
  sizeof(TASKWORK_SHADOW),
  shadowTask_Init,
  shadowTask_Delete,
  shadowTask_Update,
  shadowTask_Draw,
};

