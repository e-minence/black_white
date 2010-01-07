//======================================================================
/**
 * @file	fldeff_shadow.c
 * @brief	�t�B�[���hOBJ�e
 * @author	kagaya
 * @date	05.07.13
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "fieldmap.h"
#include "field_effect.h"
#include "fldmmdl.h"
#include "fldeff_shadow.h"

//#include "..\..\include\system\timezone.h"
//#include "..\..\include\system\pm_rtc.h"

//======================================================================
//	define
//======================================================================
#define DEBUG_SHADOW_PL_NON
//#define DEBUG_SHADOW_WRITE_OFF	//��`�ŉe�`��OFF
//#define SHADOW_DRAW_Z_OFFSET (FX32_ONE*(3))
//#define SHADOW_DRAW_Z_OFFSET (FX32_ONE*(5))			///<�e�`��I�t�Z�b�gZ��
#define SHADOW_DRAW_Z_OFFSET (FX32_ONE*(3))				

#define SHADOW_SCALE_SPEED (0x0010)
#define SHADOW_ALPHA_SPEED (0x0200)

/*
#define	TIMEZONE_MORNING	(0)
#define	TIMEZONE_NOON		(1)
#define	TIMEZONE_EVENING	(2)
#define TIMEZONE_NIGHT		(3)
#define TIMEZONE_MIDNIGHT	(4)
*/

//======================================================================
//	struct
//======================================================================
//--------------------------------------------------------------
///	FE_SHADOW�^
//--------------------------------------------------------------
typedef struct _TAG_FLDEFF_SHADOW FLDEFF_SHADOW;

//--------------------------------------------------------------
///	FE_SHADOW�\����
//--------------------------------------------------------------
struct _TAG_FLDEFF_SHADOW
{
#ifndef DEBUG_SHADOW_PL_NON
	int time_seq_no;
	int time_zone;
	int next_time_zone;
	int frame;
	fx32 alpha;
	VecFx32 scale;
#endif
	FLDEFF_CTRL *fectrl;

  GFL_G3D_RES *g3d_res;
  GFL_G3D_RND *g3d_rnd;
  GFL_G3D_OBJ *g3d_obj;
  
  VecFx32 scale;
};

//--------------------------------------------------------------
///	SHADOW_ADD_H�\����
//--------------------------------------------------------------
typedef struct
{
	FLDEFF_CTRL *fectrl;			///<FLDEFF_CTRL *
	FLDEFF_SHADOW *shadow;		///<FLDEFF_SHADOW
	MMDL *fmmdl;						///<�e�̑Ώ�MMDL *
}SHADOW_ADD_H;

//--------------------------------------------------------------
/// SHADOW_TASKHEADER
//--------------------------------------------------------------
typedef struct
{
  FLDEFF_SHADOW *eff_shadow;
  MMDL *fmmdl;
}SHADOW_TASKHEADER;

//--------------------------------------------------------------
/// TASKWORK_SHADOW
//--------------------------------------------------------------
typedef struct
{
  u32 vanish_flag;
  FLDEFF_SHADOW *eff_shadow;
  MMDL *fmmdl;
  MMDL_CHECKSAME_DATA samedata;
}TASKWORK_SHADOW;

//======================================================================
//	�v���g�^�C�v
//======================================================================
#ifndef DEBUG_SHADOW_PL_NON
static void Shadow_TimeProcAdd( FLDEFF_SHADOW *sd );
static void Shadow_TimeProcDelete( FLDEFF_SHADOW *sd );
static void Shadow_TimeScaleGet( FLDEFF_SHADOW *sd, VecFx32 *scale );
static int Shadow_TimeAlpha031( int alpha );
static void Shadow_TimeAlphaScaleSet( FLDEFF_SHADOW *sd, int zone );
static void Shadow_ValueAdd( fx32 *val0, fx32 val1, fx32 add );
static void Shadow_TimeProc( TCB_PTR tcb, void *wk );

static void Shadow_GraphicInit( FLDEFF_SHADOW *sd );
static void Shadow_GraphicDelete( FLDEFF_SHADOW *sd );
static void Shadow_GraphicAlphaSet( FLDEFF_SHADOW *sd, int alpha );

static const EOA_H_NPP DATA_EoaH_Shadow;
static const EOA_H_NPP DATA_EoaH_ShadowType;

const VecFx32 DATA_ShadowTimeScaleTbl[];
const fx32 DATA_ShadowTimeAlphaTbl[];
static const DATA_ShadowArcIdx[SHADOW_MAX];
#endif

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
  
  handle = FLDEFF_CTRL_GetArcHandleEffect( sd->fectrl );
  
  sd->g3d_res	=
    GFL_G3D_CreateResourceHandle( handle, NARC_fldeff_kage_nsbmd );
  GFL_G3D_TransVramTexture( sd->g3d_res );
  
  sd->g3d_rnd =
    GFL_G3D_RENDER_Create( sd->g3d_res, 0, sd->g3d_res );
  
  {
    NNSG3dRenderObj *rnd = GFL_G3D_RENDER_GetRenderObj( sd->g3d_rnd );
	  NNSG3dResMdl *pMdl = NNS_G3dRenderObjGetResMdl( rnd );
   	NNS_G3dMdlUseGlbDiff( pMdl );
	  NNS_G3dMdlUseGlbAmb( pMdl );
	  NNS_G3dMdlUseGlbSpec( pMdl );
  	NNS_G3dMdlUseGlbEmi( pMdl );
  }
  
  sd->g3d_obj =
    GFL_G3D_OBJECT_Create( sd->g3d_rnd, NULL, 0 );
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
  GFL_G3D_OBJECT_Delete( sd->g3d_obj );
	GFL_G3D_RENDER_Delete( sd->g3d_rnd );
 	GFL_G3D_DeleteResource( sd->g3d_res );
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
// TASKWORK_SHADOW *work = wk;
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
  
  if( MMDL_CheckStatusBitVanish(work->fmmdl) == TRUE ||
      MMDL_CheckMoveBit(work->fmmdl,MMDL_MOVEBIT_SHADOW_VANISH) ){
    work->vanish_flag = TRUE;
  }else{
    work->vanish_flag = FALSE;
    MMDL_GetVectorPos( work->fmmdl, &pos );
//  pos.x += NUM_FX32(1) / 8;   
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
  
  if( work->vanish_flag == FALSE ){
    VecFx32 pos;
    GFL_G3D_OBJ *obj = work->eff_shadow->g3d_obj;
    GFL_G3D_OBJSTATUS status = {{0},{FX32_ONE,FX32_ONE,0xc00},{0}};
    
    #if 0
    MTX_Identity33( &status.rotate );
    #else //��]�\��
    {
      u16 camera_yaw = MMDLSYS_GetTargetCameraAngleYaw( MMDL_GetMMdlSys( work->fmmdl ) );
      GFL_CALC3D_MTX_CreateRot(
          0, camera_yaw, 0, &status.rotate );
      status.scale = work->eff_shadow->scale;
    }
    #endif
    
    FLDEFF_TASK_GetPos( task, &status.trans );
    GFL_G3D_DRAW_DrawObjectCullingON( obj, &status );
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

//======================================================================
//	data
//======================================================================
#ifndef DEBUG_SHADOW_PL_NON
//--------------------------------------------------------------
///	���ԑѕʊg�嗦
//--------------------------------------------------------------
static const VecFx32 DATA_ShadowTimeScaleTbl[] =
{
	{ FX32_ONE, FX32_ONE, FX32_ONE },	//TIMEZONE_MORNING
	{ FX32_ONE+(FX32_ONE/4), FX32_ONE, FX32_ONE+(FX32_ONE/4)},	//TIMEZONE_NOON
	{ FX32_ONE+(FX32_ONE/4), FX32_ONE, FX32_ONE },	//TIMEZONE_EVENING
	{ FX32_ONE+(FX32_ONE/8), FX32_ONE, FX32_ONE },	//TIMEZONE_NIGHT
	{ FX32_ONE-(FX32_ONE/8), FX32_ONE, FX32_ONE-(FX32_ONE/8) },	//TIMEZONE_MIDNIGHT
};

//--------------------------------------------------------------
///	���ԑѕʔ������Z�x
//--------------------------------------------------------------
static const fx32 DATA_ShadowTimeAlphaTbl[] =
{
	FX32_ONE*14, //TIMEZONE_MORNING
	FX32_ONE*18,	//TIMEZONE_NOON
	FX32_ONE*18,	//TIMEZONE_EVENING
	FX32_ONE*8,	//TIMEZONE_NIGHT
	FX32_ONE*4,	//TIMEZONE_MIDNIGHT
};

//--------------------------------------------------------------
///	�e��ޕʃA�[�J�C�u�C���f�b�N�X
//--------------------------------------------------------------
static const DATA_ShadowArcIdx[SHADOW_MAX] =
{
	NARC_fldeff_kage_nsbmd,
	NARC_fldeff_red_mark_nsbmd,
	NARC_fldeff_blue_mark_nsbmd,
};
#endif
