//======================================================================
/**
 * @file	fldeff_shadow.c
 * @brief	�t�B�[���hOBJ�e
 * @author	kagaya
 * @data	05.07.13
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
typedef struct _TAG_FLDEFF_SHADOW
{
	int time_seq_no;
	int time_zone;
	int next_time_zone;
	int frame;
	fx32 alpha;
	VecFx32 scale;
	FLDEFF_CTRL *fectrl;
  
  GFL_G3D_RES *g3d_res;
  GFL_G3D_RND *g3d_rnd;
  GFL_G3D_OBJ *g3d_obj;

#ifndef DEBUG_SHADOW_PL_NON
	TCB_PTR tcb_time_proc;
	FRO_MDL rmdl[SHADOW_MAX];
	FRO_OBJ robj[SHADOW_MAX];
#endif
}FE_SHADOW;

#define FE_SHADOW_SIZE (sizeof(FE_SHADOW)) ///<FE_SHADOW�T�C�Y

//--------------------------------------------------------------
///	SHADOW_ADD_H�\����
//--------------------------------------------------------------
typedef struct
{
	FLDEFF_CTRL *fectrl;								///<FLDEFF_CTRL *
	FLDEFF_SHADOW *shadow;						///<FLDEFF_SHADOW
	FLDMMDL * fmmdl;						///<�e�̑Ώ�FLDMMDL *
}SHADOW_ADD_H;

#define SHADOW_ADD_H_SIZE (sizeof(SHADOW_ADD_H)) ///<SHADOW_ADD_H�T�C�Y

//--------------------------------------------------------------
///	SHADOW_WORK�\����
//--------------------------------------------------------------
typedef struct
{
	int obj_code;								///<�e�Ώ�OBJ�R�[�h
	int obj_id;									///<�e�Ώ�OBJID
	int zone_id;								///<�e�Ώۃ]�[��ID
	int vanish_sw;								///<��\��SW
	int type;									///<SHADOW_BLACK��
	SHADOW_ADD_H head;							///<�ǉ�����SHADOW_ADD_H
}SHADOW_WORK;

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

//======================================================================
//	�e�@���\�[�X
//======================================================================
//--------------------------------------------------------------
/**
 * �e�@���\�[�X������
 * @param fectrl FLDEFF_CTRL*
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
  sd->g3d_obj =
    GFL_G3D_OBJECT_Create( sd->g3d_rnd, NULL, 0 );
}

//--------------------------------------------------------------
/**
 * �e�@���\�[�X�폜
 * @param fectrl FLDEFF_CTRL*
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
/// SHADOW_TASKHEADER
//--------------------------------------------------------------
typedef struct
{
  FLDEFF_SHADOW *eff_shadow;
  FLDMMDL *fmmdl;
}SHADOW_TASKHEADER;

//--------------------------------------------------------------
/// SHADOW_TASKWORK
//--------------------------------------------------------------
typedef struct
{
  FLDEFF_SHADOW *eff_shadow;
  FLDMMDL *fmmdl;
  u16 obj_id;
  u16 zone_id;
}TASKWORK_SHADOW;

static const FLDEFF_TASK_HEADER DATA_shadowTaskHeader;

//--------------------------------------------------------------
/**
 * ���샂�f���p�e�@�ǉ�
 * @param fmmdl FLDMMDL
 * @param FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDEFF_SHADOW_SetFldMMdl( FLDMMDL *fmmdl, FLDEFF_CTRL *fectrl )
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
  work->obj_id = FLDMMDL_GetOBJID( work->fmmdl );
  work->zone_id = FLDMMDL_GetZoneID( work->fmmdl );
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
  
  if( FLDMMDL_CheckSameID(work->fmmdl,work->obj_id,work->zone_id) == FALSE ){
    FLDEFF_TASK_CallDelete( task );
    return;
  }
  
  FLDMMDL_GetVectorPos( work->fmmdl, &pos );
//  pos.x += NUM_FX32(-1)/2;
  pos.x += NUM_FX32(1) / 4;
  pos.y += NUM_FX32(-2);
  pos.z += NUM_FX32(6)+0x800;
  FLDEFF_TASK_SetPos( task, &pos );
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
  VecFx32 pos;
  TASKWORK_SHADOW *work = wk;
  GFL_G3D_OBJ *obj = work->eff_shadow->g3d_obj;
  GFL_G3D_OBJSTATUS status = {{0},{FX32_ONE,FX32_ONE,0xc00},{0}};
  
  MTX_Identity33( &status.rotate );
  FLDEFF_TASK_GetPos( task, &status.trans );
  GFL_G3D_DRAW_DrawObjectCullingON( obj, &status );
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

#ifndef DEBUG_SHADOW_PL_NON
//--------------------------------------------------------------
/**
 * �t�B�[���hOBJ�p�e�ǉ�
 * @param	fmmdl		FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FE_fmmdlShadow_Add( FLDMMDL * fmmdl )
{
	int param,pri;
	SHADOW_ADD_H head;
	FLDEFF_CTRL *fectrl;
	VecFx32 mtx;
	
	fectrl = FE_FieldOBJ_FLDEFF_CTRL_Get( fmmdl );
	
	head.fectrl = fectrl;
	head.shadow = FE_EffectWorkGet( fectrl, FE_FLD_SHADOW );
	head.fmmdl = fmmdl;
	FieldOBJ_VecPosGet( fmmdl, &mtx );
	
	param = SHADOW_BLACK;
	pri = FieldOBJ_TCBPriGet( fmmdl, fmmdl_TCBPRI_OFFS_AFTER );
	FE_EoaAddNpp( fectrl, &DATA_EoaH_Shadow, &mtx, param, &head, pri );
}

//--------------------------------------------------------------
/**
 * EOA �e�@������
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	int		TRUE=����I���BFALSE=�ُ�I��
 */
//--------------------------------------------------------------
static int EoaShadow_Init( EOA_PTR eoa, void *wk )
{
	SHADOW_WORK *work;
	const SHADOW_ADD_H *head;
	
	work = wk;
	head = EOA_AddPtrGet( eoa );
	work->head = *head;
	
	work->type = EOA_AddParamGet( eoa );
	work->obj_code = FieldOBJ_OBJCodeGet( work->head.fmmdl );
	work->obj_id = FieldOBJ_OBJIDGet( work->head.fmmdl );
	
	if( FieldOBJ_StatusBitCheck_Alies(work->head.fmmdl) == TRUE ){
		work->zone_id = FieldOBJ_ZoneIDGetAlies( work->head.fmmdl );
	}else{
		work->zone_id = FieldOBJ_ZoneIDGet( work->head.fmmdl );
	}
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * EOA �e�@�폜
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaShadow_Delete( EOA_PTR eoa, void *wk )
{
}

//--------------------------------------------------------------
/**
 * EOA �e�@����
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaShadow_Move( EOA_PTR eoa, void *wk )
{
	SHADOW_WORK *work;
	FLDMMDL * fmmdl;
	
	work = wk;
	fmmdl = work->head.fmmdl;
	
	if( FieldOBJ_CheckSameIDOBJCodeIn(
		fmmdl,work->obj_code,work->obj_id,work->zone_id) == FALSE ){
		FE_EoaDelete( eoa );										//����ł͂Ȃ�
		return;
	}
	
	if( FieldOBJ_FieldOBJSysStatusBitCheck(
        fmmdl,fmmdlSYS_STA_BIT_SHADOW_JOIN_NOT) ){
		FE_EoaDelete( eoa );
		return;
	}
	
	work->vanish_sw = FALSE;
	
	if( FieldOBJ_StatusBit_CheckEasy(fmmdl,
		fmmdl_STA_BIT_VANISH|fmmdl_STA_BIT_SHADOW_VANISH) == TRUE ){
		work->vanish_sw = TRUE;									//��\��
		return;
	}
	
	{
		VecFx32 vec;
		
		FieldOBJ_VecPosGet( fmmdl, &vec );
		EOA_MatrixSet( eoa, &vec );
	}
}

//--------------------------------------------------------------
/**
 * EOA �e�@�`��
 * @param	eoa		EOA_PTR
 * @param	wk		eoa work *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void EoaShadow_Draw( EOA_PTR eoa, void *wk )
{
	SHADOW_WORK *work = wk;
	
	if( work->vanish_sw == FALSE ){
		VecFx32 pos,scale;
		MtxFx33 rot = { FX32_ONE, 0,0,0, FX32_ONE, 0,0,0,FX32_ONE};
		FRO_OBJ *robj = &work->head.shadow->robj[SHADOW_BLACK];
		
		Shadow_TimeScaleGet( work->head.shadow, &scale );
		EOA_MatrixGet( eoa, &pos );
		pos.x += NUM_FX32(-1) / 2;
		pos.y += NUM_FX32(-4);
		pos.z += NUM_FX32(1);
		FRO_OBJ_Draw( robj, &pos, &scale, &rot );
	}
}

//--------------------------------------------------------------
///	�eEOA_H
//--------------------------------------------------------------
static const EOA_H_NPP DATA_EoaH_Shadow =
{
	SHADOW_WORK_SIZE,
	EoaShadow_Init,
	EoaShadow_Delete,
	EoaShadow_Move,
	EoaShadow_Draw,
};
#endif

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
