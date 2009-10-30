//======================================================================
/**
 *
 * @file	fieldobj_move.c
 * @brief	���샂�f�� ����n
 * @author	kagaya
 * @date	05.07.25
 *
 */
//======================================================================
#include "fldmmdl.h"

#include "fieldmap.h"

#include "fldeff_shadow.h"
#include "fldeff_kemuri.h"
#include "fldeff_grass.h"
#include "fldeff_footmark.h"
#include "fldeff_reflect.h"
#include "fldeff_ripple.h"
#include "fldeff_splash.h"

//======================================================================
//	define
//======================================================================
//--------------------------------------------------------------
//  debug
//--------------------------------------------------------------
#ifdef DEBUG_ONLY_FOR_kagaya
//#define DEBUG_REFLECT_CHECK //��`�ŉf�荞�݃`�F�b�N
#endif

//--------------------------------------------------------------
///	�X�e�[�^�X�r�b�g
//--------------------------------------------------------------
///�ړ�������֎~����X�e�[�^�X�r�b�g
#define STA_BIT_MOVE_ERROR (MMDL_STABIT_HEIGHT_GET_ERROR|MMDL_STABIT_ATTR_GET_ERROR)

//--------------------------------------------------------------
///	�A�g���r���[�g�I�t�Z�b�g
//--------------------------------------------------------------
#define ATTROFFS_Y_NUMA (NUM_FX32(-12))///<��Y�I�t�Z�b�g
#define ATTROFFS_Y_NUMA_DEEP (NUM_FX32(-14))///<�[����Y�I�t�Z�b�g
#define ATTROFFS_Y_YUKI (NUM_FX32(-12))///<��Y�I�t�Z�b�g
#define ATTROFFS_Y_YUKI_DEEP (NUM_FX32(-14))///<�[����Y�I�t�Z�b�g
#define ATTROFFS_Y_YUKI_DEEP_MOST (NUM_FX32(-16))///<�X�ɐ[����Y�I�t�Z�b�g

///���������ɂ��ړ�����
#define HEIGHT_DIFF_COLLISION (FX32_ONE*(20))

//======================================================================
//	struct
//======================================================================

//======================================================================
//	proto
//======================================================================
static int MMdl_CheckMoveStart( const MMDL * fmmdl );
static void MMdl_GetHeightMoveBefore( MMDL * fmmdl );
static void MMdl_GetAttrMoveBefore( MMDL * fmmdl );
static void MMdl_ProcMoveStartFirst( MMDL * fmmdl );
static void MMdl_ProcMoveStartSecond( MMDL * fmmdl );
static void MMdl_ProcMoveEnd( MMDL * fmmdl );

static void MMdl_MapAttrProc_MoveStartFirst( MMDL * fmmdl );
static void MMdl_MapAttrProc_MoveStartSecond( MMDL * fmmdl );
static void MMdl_MapAttrProc_MoveStartJumpSecond( MMDL * fmmdl );
static void MMdl_MapAttrProc_MoveEnd( MMDL * fmmdl );
static void MMdl_MapAttrProc_MoveEndJump( MMDL * fmmdl );

static void MMdl_MapAttrHeight_02(
		MMDL * fmmdl,MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm );

static void MMdl_MapAttrGrassProc_0(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm );
static void MMdl_MapAttrGrassProc_12(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm );

static void MMdl_MapAttrFootMarkProc_1(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm );

static void MMdl_MapAttrSplashProc_012(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm );
static void MMdl_MapAttrSplashProc_Jump1(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm );

static void MMdl_MapAttrShadowProc_0(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm );
static void MMdl_MapAttrShadowProc_1(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm );
static void MMdl_MapAttrShadowProc_2(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm );

static void MMdl_MapAttrGroundSmokeProc_2(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm );

static void MMdl_MapAttrLGrassProc_0(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm );
static void MMdl_MapAttrLGrassProc_1(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm );

static void MMdl_MapAttrNGrassProc_0(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm );
static void MMdl_MapAttrNGrassProc_1(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm );

static void MMdl_MapAttrPoolProc_1(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm );
static void MMdl_MapAttrPoolProc_2(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm );

static void MMdl_MapAttrSwampProc_1(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm );
static void MMdl_MapAttrSwampProc_2(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm );

static void MMdl_MapAttrReflect_01(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm );
static void MMdl_MapAttrReflect_2(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm );

static void MMdl_MapAttrBridgeProc_01(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm );

static void MMdl_MapAttrSEProc_1(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm );

static BOOL MMdl_HitCheckMoveAttr(
	const MMDL * fmmdl, const VecFx32 pos );

#ifndef MMDL_PL_NULL
static BOOL (* const DATA_HitCheckAttr_Now[DIR_MAX4])( MAPATTR attr );
static BOOL (* const DATA_HitCheckAttr_Next[DIR_MAX4])( MAPATTR attr );
#endif

static BOOL MMdl_GetMapGridInfo(
	const MMDL *fmmdl, const VecFx32 *pos,
	FLDMAPPER_GRIDINFO *pGridInfo );

static FLDEFF_CTRL * mmdl_GetFldEffCtrl( MMDL *mmdl );

//======================================================================
//	�t�B�[���h���샂�f�� ����
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f�� ���쏉����
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_InitMoveProc( MMDL * fmmdl )
{
	MMDL_CallMoveInitProc( fmmdl );
	MMDL_MoveSubProcInit( fmmdl );
	MMDL_OnMoveBit( fmmdl, MMDL_MOVEBIT_MOVEPROC_INIT );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f������
 * @param	fmmdl		MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_UpdateMove( MMDL * fmmdl )
{
	if( MMDL_CheckMMdlSysStatusBit(
		fmmdl,MMDLSYS_STABIT_MOVE_PROC_STOP) ){
		return;
	}
	
	MMdl_GetHeightMoveBefore( fmmdl );
	MMdl_GetAttrMoveBefore( fmmdl );
	MMdl_ProcMoveStartFirst( fmmdl );
	
	if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_ACMD) ){
		MMDL_ActionAcmd( fmmdl );
	}else if( MMDL_CheckStatusBitMoveProcPause(fmmdl) == FALSE ){
		if( MMdl_CheckMoveStart(fmmdl) == TRUE ){
			if( MMDL_MoveSub(fmmdl) == FALSE ){
				MMDL_CallMoveProc( fmmdl );
			}
		}
	}
	
	MMdl_ProcMoveStartSecond( fmmdl );
	MMdl_ProcMoveEnd( fmmdl );
}

//--------------------------------------------------------------
/**
 * ����\���ǂ����`�F�b�N
 * @param	fmmdl	MMDL *
 * @retval	int		TRUE=�\,FALSE=�s��
 */
//--------------------------------------------------------------
static int MMdl_CheckMoveStart( const MMDL * fmmdl )
{
	if( MMDL_CheckStatusBitMove(fmmdl) == TRUE ){
		return( TRUE );
	}
	
	if( MMDL_CheckStatusBit(fmmdl,STA_BIT_MOVE_ERROR) == 0 ){
		return( TRUE );
	}else if( MMDL_GetMoveCode(fmmdl) == MV_TR_PAIR ){ //�e�̍s���ɏ]��
		return( TRUE );
	}
	
	#ifndef MMDL_PL_NULL
	{	//�ړ��֎~�t���O���E�`�F�b�N
		u32 st = MMDL_GetStatusBit( fmmdl );
		
		//�����擾���Ȃ��ꍇ
		if( (st&MMDL_STABIT_HEIGHT_GET_ERROR) &&
			(st&MMDL_STABIT_HEIGHT_GET_OFF) == 0 ){
			return( FALSE );
		}
		
		//�A�g���r���[�g�擾���Ȃ��ꍇ
		if( (st&MMDL_STABIT_ATTR_GET_ERROR) &&	
			MMDL_CheckMoveBitAttrGetOFF(fmmdl) == FALSE ){
			return( FALSE );
		}
	}
	#endif
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * �ړ��J�n���ɍs�������擾
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_GetHeightMoveBefore( MMDL * fmmdl )
{
	if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_HEIGHT_GET_ERROR) ){
		MMDL_UpdateCurrentHeight( fmmdl );
	}
}

//--------------------------------------------------------------
/**
 * �ړ��J�n���ɍs���A�g���r���[�g�擾
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_GetAttrMoveBefore( MMDL * fmmdl )
{
	if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_ATTR_GET_ERROR) ){
		if( MMDL_UpdateCurrentMapAttr(fmmdl) == TRUE ){
			MMDL_OnStatusBitMoveStart( fmmdl );
		}
	}
}

//--------------------------------------------------------------
/**
 * �ړ��J�n�Ŕ����@1st
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_ProcMoveStartFirst( MMDL * fmmdl )
{
	if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_MOVE_START) ){
		MMdl_MapAttrProc_MoveStartFirst( fmmdl );
	}
	
	MMDL_OffStatusBit( fmmdl,
		MMDL_STABIT_MOVE_START | MMDL_STABIT_JUMP_START );
}

//--------------------------------------------------------------
/**
 * �ړ��J�n�Ŕ����@2nd
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_ProcMoveStartSecond( MMDL * fmmdl )
{
	if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_JUMP_START) ){
		MMdl_MapAttrProc_MoveStartJumpSecond( fmmdl );
	}else if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_MOVE_START) ){
		MMdl_MapAttrProc_MoveStartSecond( fmmdl );
	}
	
	MMDL_OffStatusBit( fmmdl,
		MMDL_STABIT_MOVE_START | MMDL_STABIT_JUMP_START );
}

//--------------------------------------------------------------
/**
 * ����I���Ŕ���
 * @param	fmmdl	MMDL *GX_WNDMASK_NONE
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_ProcMoveEnd( MMDL * fmmdl )
{
	if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_JUMP_END) ){
		MMdl_MapAttrProc_MoveEndJump( fmmdl );
	}else if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_MOVE_END) ){
		MMdl_MapAttrProc_MoveEnd( fmmdl );
	}
	
	MMDL_OffStatusBit( fmmdl,
		MMDL_STABIT_MOVE_END | MMDL_STABIT_JUMP_END );
}

//======================================================================
//	�}�b�v�A�g���r���[�g
//======================================================================
//--------------------------------------------------------------
/**
 * ����J�n�Ŕ�������A�g���r���[�g���� 1st
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrProc_MoveStartFirst( MMDL * fmmdl )
{
	MMDL_UpdateCurrentMapAttr( fmmdl );
	
	if( MMDL_CheckCompletedDrawInit(fmmdl) == TRUE ){
		MAPATTR now = MMDL_GetMapAttr( fmmdl );
		MAPATTR old = MMDL_GetMapAttrOld( fmmdl );
		const OBJCODE_PARAM *prm =
			MMDL_GetOBJCodeParam( fmmdl, MMDL_GetOBJCode(fmmdl) );
		
		MMdl_MapAttrBridgeProc_01( fmmdl, now, old, prm );
		MMdl_MapAttrGrassProc_0( fmmdl, now, old, prm );
		MMdl_MapAttrSplashProc_012( fmmdl, now, old, prm );
		MMdl_MapAttrShadowProc_0( fmmdl, now, old, prm );
		MMdl_MapAttrHeight_02( fmmdl, now, old, prm );
		MMdl_MapAttrLGrassProc_0( fmmdl, now, old, prm );
		MMdl_MapAttrNGrassProc_0( fmmdl, now, old, prm );
		MMdl_MapAttrReflect_01( fmmdl, now, old, prm );
	}
}

//--------------------------------------------------------------
/**
 * ����J�n�Ŕ�������A�g���r���[�g���� 2nd
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrProc_MoveStartSecond( MMDL * fmmdl )
{
	MMDL_UpdateCurrentMapAttr( fmmdl );
	
	if( MMDL_CheckCompletedDrawInit(fmmdl) == TRUE ){
		MAPATTR now = MMDL_GetMapAttr( fmmdl );
		MAPATTR old = MMDL_GetMapAttrOld( fmmdl );
		const OBJCODE_PARAM *prm =
			MMDL_GetOBJCodeParam( fmmdl, MMDL_GetOBJCode(fmmdl) );

		MMdl_MapAttrBridgeProc_01( fmmdl, now, old, prm );
		MMdl_MapAttrGrassProc_12( fmmdl, now, old, prm );
		MMdl_MapAttrFootMarkProc_1( fmmdl, now, old, prm );
		MMdl_MapAttrSplashProc_012( fmmdl, now, old, prm );
		MMdl_MapAttrShadowProc_1( fmmdl, now, old, prm );
		MMdl_MapAttrLGrassProc_1( fmmdl, now, old, prm );
		MMdl_MapAttrNGrassProc_1( fmmdl, now, old, prm );
		MMdl_MapAttrPoolProc_1( fmmdl, now, old, prm );
		MMdl_MapAttrSwampProc_1( fmmdl, now, old, prm );
		MMdl_MapAttrReflect_01( fmmdl, now, old, prm );
		
		MMdl_MapAttrSEProc_1( fmmdl, now, old, prm ); //�`��֌W�Ȃ�?
	}
}

//--------------------------------------------------------------
/**
 * ����J�n�Ŕ�������A�g���r���[�g���� 2nd Jump
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrProc_MoveStartJumpSecond( MMDL * fmmdl )
{
	MMDL_UpdateCurrentMapAttr( fmmdl );
	
	if( MMDL_CheckCompletedDrawInit(fmmdl) == TRUE ){
		MAPATTR now = MMDL_GetMapAttr( fmmdl );
		MAPATTR old = MMDL_GetMapAttrOld( fmmdl );
		const OBJCODE_PARAM *prm =
			MMDL_GetOBJCodeParam( fmmdl, MMDL_GetOBJCode(fmmdl) );

		MMdl_MapAttrBridgeProc_01( fmmdl, now, old, prm );
		MMdl_MapAttrShadowProc_1( fmmdl, now, old, prm );
		MMdl_MapAttrReflect_01( fmmdl, now, old, prm );
		MMdl_MapAttrSplashProc_Jump1( fmmdl, now, old, prm );
		MMdl_MapAttrSEProc_1( fmmdl, now, old, prm );//�`��֌W�Ȃ�?
	}
}

//--------------------------------------------------------------
/**
 * ����I���Ŕ�������A�g���r���[�g�����@end
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrProc_MoveEnd( MMDL * fmmdl )
{
	MMDL_UpdateCurrentMapAttr( fmmdl );
	
	if( MMDL_CheckCompletedDrawInit(fmmdl) == TRUE ){
		MAPATTR now = MMDL_GetMapAttr( fmmdl );
		MAPATTR old = MMDL_GetMapAttrOld( fmmdl );
		const OBJCODE_PARAM *prm =
			MMDL_GetOBJCodeParam( fmmdl, MMDL_GetOBJCode(fmmdl) );
		
		//�I���@�A�g���r���[�g����
		MMdl_MapAttrHeight_02( fmmdl, now, old, prm );
		MMdl_MapAttrPoolProc_2( fmmdl, now, old, prm );
		MMdl_MapAttrSwampProc_2( fmmdl, now, old, prm );
		MMdl_MapAttrSplashProc_012( fmmdl, now, old, prm );
		MMdl_MapAttrReflect_2( fmmdl, now, old, prm );
		MMdl_MapAttrShadowProc_2( fmmdl, now, old, prm );
	}
}

//--------------------------------------------------------------
/**
 * ����I���Ŕ�������A�g���r���[�g�����@end jump
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrProc_MoveEndJump( MMDL * fmmdl )
{
	MMDL_UpdateCurrentMapAttr( fmmdl );
	
	if( MMDL_CheckCompletedDrawInit(fmmdl) == TRUE ){
		MAPATTR now = MMDL_GetMapAttr( fmmdl );
		MAPATTR old = MMDL_GetMapAttrOld( fmmdl );
		const OBJCODE_PARAM *prm =
			MMDL_GetOBJCodeParam( fmmdl, MMDL_GetOBJCode(fmmdl) );
		
		//�I���@�A�g���r���[�g����
		MMdl_MapAttrHeight_02( fmmdl, now, old, prm );
		MMdl_MapAttrPoolProc_2( fmmdl, now, old, prm );
		MMdl_MapAttrSwampProc_2( fmmdl, now, old, prm );
		MMdl_MapAttrSplashProc_012( fmmdl, now, old, prm );
		MMdl_MapAttrReflect_2( fmmdl, now, old, prm );
		MMdl_MapAttrShadowProc_2( fmmdl, now, old, prm );
		MMdl_MapAttrGrassProc_12( fmmdl, now, old, prm );
		MMdl_MapAttrGroundSmokeProc_2( fmmdl, now, old, prm );
	}
}

//======================================================================
//	�A�g���r���[�g�ύ���
//======================================================================
//--------------------------------------------------------------
/**
 * �A�g���r���[�g�ύ����@����J�n�A����I��
 * @param	fmmdl		MMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrHeight_02(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm )
{
	#ifndef MMDL_PL_NULL
	if( MMDL_CheckStatusBitAttrOffsetOFF(fmmdl) == FALSE ){
		if( MAPATTR_IsSwampDeep(now) == TRUE ||
			MAPATTR_IsSwampGrassDeep(now) == TRUE ){
			VecFx32 vec = { 0, ATTROFFS_Y_NUMA_DEEP, 0 };
			MMDL_SetVectorAttrDrawOffsetPos( fmmdl, &vec );
			return;
		}
		
		if( MAPATTR_IsSwamp(now) == TRUE || MAPATTR_IsSwampGrass(now) == TRUE ){
			VecFx32 vec = { 0, ATTROFFS_Y_NUMA, 0 };
			MMDL_SetVectorAttrDrawOffsetPos( fmmdl, &vec );
			return;
		}
		
		if( MAPATTR_IsSnowDeepMost(now) == TRUE ){
			VecFx32 vec = { 0, ATTROFFS_Y_YUKI_DEEP_MOST, 0 };
			MMDL_SetVectorAttrDrawOffsetPos( fmmdl, &vec );
			return;
		}
		
		if( MAPATTR_IsSnowDeep(now) == TRUE ){
			VecFx32 vec = { 0, ATTROFFS_Y_YUKI_DEEP, 0 };
			MMDL_SetVectorAttrDrawOffsetPos( fmmdl, &vec );
			return;
		}
		
		if( MAPATTR_IsShallowSnow(now) == TRUE ){
			VecFx32 vec = { 0, ATTROFFS_Y_YUKI, 0 };
			MMDL_SetVectorAttrDrawOffsetPos( fmmdl, &vec );
			return;
		}
	}
	
	{
		VecFx32 vec = { 0, 0, 0 };
		MMDL_SetVectorAttrDrawOffsetPos( fmmdl, &vec );
	}
	#endif
}

//======================================================================
//	�A�g���r���[�g�@��
//======================================================================
//--------------------------------------------------------------
/**
 * ���@����J�n 0
 * @param	fmmdl		MMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrGrassProc_0(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm )
{
  {
    MAPATTR_FLAG flag = MAPATTR_GetAttrFlag( now );
    
    if( (flag&MAPATTR_FLAGBIT_GRASS) ){
      FLDEFF_CTRL *fectrl = mmdl_GetFldEffCtrl( fmmdl );
      FLDEFF_GRASS_SetMMdl( fectrl, fmmdl, FALSE );
    }
  }
}

//--------------------------------------------------------------
/**
 * ���@���� 1,2
 * @param	fmmdl	MMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrGrassProc_12(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm )
{
  {
    if( now != MAPATTR_ERROR ){
      MAPATTR_FLAG flag = MAPATTR_GetAttrFlag( now );
    
      if( (flag&MAPATTR_FLAGBIT_GRASS) ){
        FLDEFF_CTRL *fectrl = mmdl_GetFldEffCtrl( fmmdl );
        FLDEFF_GRASS_SetMMdl( fectrl, fmmdl, TRUE );
      }
    }
  }
}

//======================================================================
//	�A�g���r���[�g�@����
//======================================================================
//--------------------------------------------------------------
/**
 * ���Ձ@����J�n 1
 * @param	fmmdl	MMDL *
 * @param	now		���݂̃A�g���r���[�g
 * @param	old		�ߋ��̃A�g���r���[�g
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrFootMarkProc_1(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm )
{
  if( old != MAPATTR_ERROR ){
    MAPATTR_FLAG flag = MAPATTR_GetAttrFlag( old );
    
    if( (flag & MAPATTR_FLAGBIT_FOOTMARK) ){
      FOOTMARK_TYPE type;
      FLDEFF_CTRL *fectrl = mmdl_GetFldEffCtrl( fmmdl );
      MAPATTR_VALUE val = MAPATTR_GetAttrValue( old );

      if( MAPATTR_VALUE_CheckSnowType(val) == FALSE ){
        type = FOOTMARK_TYPE_HUMAN;
        if( prm->footmark_type == MMDL_FOOTMARK_CYCLE ){
          type = FOOTMARK_TYPE_CYCLE;
        }
      }else{
        type = FOOTMARK_TYPE_HUMAN_SNOW;
        if( prm->footmark_type == MMDL_FOOTMARK_CYCLE ){
          type = FOOTMARK_TYPE_CYCLE_SNOW;
        }
      }
      
      FLDEFF_FOOTMARK_SetMMdl( fmmdl, fectrl, type );
    }
  }
}

//======================================================================
//	�A�g���r���[�g�@����
//======================================================================
//--------------------------------------------------------------
/**
 * ���򖗁@����J�n�I�� 012
 * @param	fmmdl	MMDL *
 * @param	now		���݂̃A�g���r���[�g
 * @param	old		�ߋ��̃A�g���r���[�g
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrSplashProc_012(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm )
{
  MAPATTR_VALUE val = MAPATTR_GetAttrValue( now );
  
  if( MAPATTR_VALUE_CheckShoal(val) == TRUE ){
		if( MMDL_CheckStatusBitShoalEffect(fmmdl) == FALSE ){
      FLDEFF_CTRL *fectrl = mmdl_GetFldEffCtrl( fmmdl );
      FLDEFF_SPLASH_SetMMdl( fectrl, fmmdl, TRUE );
			MMDL_SetStatusBitShoalEffect( fmmdl, TRUE );
		}
	}else{
		MMDL_SetStatusBitShoalEffect( fmmdl, FALSE );
	}
}

//--------------------------------------------------------------
/**
 * ���򖗁@�W�����v����J�n 1
 * @param	fmmdl	MMDL *
 * @param	now		���݂̃A�g���r���[�g
 * @param	old		�ߋ��̃A�g���r���[�g
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrSplashProc_Jump1(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm )
{
	MMDL_SetStatusBitShoalEffect( fmmdl, FALSE );
}

//======================================================================
//	�A�g���r���[�g�@�e
//======================================================================
//--------------------------------------------------------------
/**
 * �e�@����J�n 0
 * @param	fmmdl	MMDL *
 * @param	now		���݂̃A�g���r���[�g
 * @param	old		�ߋ��̃A�g���r���[�g
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrShadowProc_0(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm )
{
	MMdl_MapAttrShadowProc_1( fmmdl, now, old, prm );
}

//--------------------------------------------------------------
/**
 * �e�@����J�n 1
 * @param	fmmdl	MMDL *
 * @param	now		���݂̃A�g���r���[�g
 * @param	old		�ߋ��̃A�g���r���[�g
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrShadowProc_1(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm )
{
  {
  	const MMDLSYS *fos = MMDL_GetMMdlSys( fmmdl );
    
		if( MMDLSYS_CheckJoinShadow(fos) == FALSE ){
			return;
		}
    
		if( MMDL_CheckStatusBit(fmmdl,MMDL_STABIT_SHADOW_SET) == 0 ){
      FLDEFF_CTRL *fectrl = mmdl_GetFldEffCtrl( fmmdl );
      FLDEFF_SHADOW_SetMMdl( fmmdl, fectrl );
			MMDL_OnStatusBit( fmmdl, MMDL_STABIT_SHADOW_SET );
    }
  }
}

//--------------------------------------------------------------
/**
 * �e�@����I�� 2
 * @param	fmmdl	MMDL *
 * @param	now		���݂̃A�g���r���[�g
 * @param	old		�ߋ��̃A�g���r���[�g
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrShadowProc_2(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm )
{
	#ifndef MMDL_PL_NULL
	{
		const MMDLSYS *fos = MMDL_GetMMdlSys( fmmdl );
		
		if( MMDLSYS_CheckJoinShadow(fos) == FALSE ){
			return;
		}
	}
	
	if( prm->shadow_type == MMDL_SHADOW_NON ){
		return;
	}
	
	if( MAPATTR_IsGrass(now) == TRUE ||
		MAPATTR_IsLongGrass(now) == TRUE ||
		MMdl_CheckMapAttrKind_Water(fmmdl,now) == TRUE ||
		MAPATTR_IsPoolCheck(now) == TRUE ||
		MAPATTR_IsShoal(now) == TRUE ||
		MMdl_CheckMapAttrKind_Snow(fmmdl,now) == TRUE ||
		MAPATTR_IsSwamp(now) == TRUE ||
		MAPATTR_IsSwampGrass(now) == TRUE ||
		MAPATTR_IsMirrorReflect(now) ){
		MMDL_OnStatusBit( fmmdl, MMDL_STABIT_SHADOW_VANISH );
	}else{
		MMDL_OffStatusBit( fmmdl, MMDL_STABIT_SHADOW_VANISH );
	}
	#endif
}

//======================================================================
//	�A�g���r���[�g�@�y��
//======================================================================
//--------------------------------------------------------------
/**
 * �y���@����I�� 2 
 * @param	fmmdl	MMDL *
 * @param	now		���݂̃A�g���r���[�g
 * @param	old		�ߋ��̃A�g���r���[�g
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrGroundSmokeProc_2(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm )
{
  {
    FLDEFF_CTRL *fectrl = mmdl_GetFldEffCtrl( fmmdl );
    FLDEFF_KEMURI_SetMMdl( fmmdl, fectrl );
  }
}

//======================================================================
//	�A�g���r���[�g�@������
//======================================================================
//--------------------------------------------------------------
/**
 * �������@����J�n 0
 * @param	fmmdl		MMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrLGrassProc_0(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm )
{
	#ifndef MMDL_PL_NULL
	if( MAPATTR_IsLongGrass(now) == TRUE ){
		FE_fmmdlLGrass_Add( fmmdl, FALSE );
	}
	#endif
}

//--------------------------------------------------------------
/**
 * �������@����J�n 1
 * @param	fmmdl	MMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrLGrassProc_1(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm )
{
	#ifndef MMDL_PL_NULL
	if( MAPATTR_IsLongGrass(now) == TRUE ){
		FE_fmmdlLGrass_Add( fmmdl, TRUE );
	}
	#endif
}

//======================================================================
//	�A�g���r���[�g�@����
//======================================================================
//--------------------------------------------------------------
/**
 * �����@����J�n 0
 * @param	fmmdl		MMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrNGrassProc_0(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm )
{
	#ifndef MMDL_PL_NULL
	if( MAPATTR_IsSwampGrass(now) == TRUE ){
		FE_fmmdlNGrass_Add( fmmdl, FALSE );
	}
	#endif
}

//--------------------------------------------------------------
/**
 * �����@����J�n 1
 * @param	fmmdl	MMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrNGrassProc_1(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm )
{
	#ifndef MMDL_PL_NULL
	if( MAPATTR_IsSwampGrass(now) == TRUE ){
		FE_fmmdlNGrass_Add( fmmdl, TRUE );
	}
	#endif
}

//======================================================================
//	�A�g���r���[�g�@�����܂�
//======================================================================
//--------------------------------------------------------------
/**
 * �����܂�@����J�n 1
 * @param	fmmdl	MMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrPoolProc_1(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm )
{
  MAPATTR_VALUE val = MAPATTR_GetAttrValue( old );
  
	if( MAPATTR_VALUE_CheckPool(val) == TRUE ){
		FLDEFF_RIPPLE_Set( 
      mmdl_GetFldEffCtrl(fmmdl),
			MMDL_GetOldGridPosX(fmmdl),
			MMDL_GetOldGridPosZ(fmmdl),
			MMDL_GetVectorPosY(fmmdl) );
	}
}

//--------------------------------------------------------------
/**
 * �����܂�@����I�� 2
 * @param	fmmdl	MMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrPoolProc_2(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm )
{
  MAPATTR_VALUE val = MAPATTR_GetAttrValue( now );

	if( MAPATTR_VALUE_CheckPool(val) == TRUE ){
		FLDEFF_RIPPLE_Set( 
      mmdl_GetFldEffCtrl(fmmdl),
			MMDL_GetGridPosX(fmmdl),
			MMDL_GetGridPosZ(fmmdl),
			MMDL_GetVectorPosY(fmmdl) );
	}
}

//======================================================================
//	�A�g���r���[�g�@��
//======================================================================
//--------------------------------------------------------------
/**
 * ���@����J�n 1
 * @param	fmmdl	MMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrSwampProc_1(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm )
{
	#ifndef MMDL_PL_NULL
	if( MAPATTR_IsSwamp(old) == TRUE ){
		FE_fmmdlNRippleSet( fmmdl,
			MMDL_GetOldGridPosX(fmmdl),
			MMDL_GetOldGridPosY(fmmdl), 
			MMDL_GetOldGridPosZ(fmmdl) );
	}
	#endif
}

//--------------------------------------------------------------
/**
 * ���@����I�� 2
 * @param	fmmdl	MMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrSwampProc_2(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm )
{
	#ifndef MMDL_PL_NULL
	if( MAPATTR_IsSwamp(now) == TRUE ){
		FE_fmmdlNRippleSet( fmmdl,
			MMDL_GetGridPosX(fmmdl),
			MMDL_GetGridPosY(fmmdl), 
			MMDL_GetGridPosZ(fmmdl) );
	}
	#endif
}

//======================================================================
//	�A�g���r���[�g�@�f�肱��
//======================================================================
//--------------------------------------------------------------
/**
 * �f�肱�݁@����J�n 01
 * @param	fmmdl	MMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrReflect_01(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm )
{
  if( prm->reflect_type == MMDL_REFLECT_NON ){
    return;
  }
  
  if( MMDL_CheckStatusBitReflect(fmmdl) == FALSE )
  {
    MAPATTR_FLAG flag;
    MAPATTR hit_attr = MAPATTR_ERROR;
    
    #ifdef DEBUG_REFLECT_CHECK
    now = MAPATTR_FLAGBIT_REFLECT << 16;
    #endif
    
    if( now != MAPATTR_ERROR ){
      MAPATTR_FLAG flag = MAPATTR_GetAttrFlag( now );
    
      if( (flag&MAPATTR_FLAGBIT_REFLECT) )
      {
        hit_attr = now;
        #ifdef DEBUG_REFLECT_CHECK
        hit_attr = 0;
        #endif
      }
      else
      {
        MAPATTR next = MMDL_GetMapDirAttr( fmmdl, DIR_DOWN );
        flag = MAPATTR_GetAttrFlag( next );
      
        if( (flag&MAPATTR_FLAGBIT_REFLECT) )
        {
          hit_attr = next;
        }
      }
    
      if( hit_attr != MAPATTR_ERROR )
      {
        REFLECT_TYPE type;
        FLDEFF_CTRL *fectrl = mmdl_GetFldEffCtrl( fmmdl );
        MMDLSYS *fmmdlsys = (MMDLSYS*)MMDL_GetMMdlSys( fmmdl );
      
        MMDL_SetStatusBitReflect( fmmdl, TRUE );
      
        type = REFLECT_TYPE_POND; //�{���̓A�g���r���[�g���ʂ��ă^�C�v����
        FLDEFF_REFLECT_SetMMdl( fmmdlsys, fmmdl, fectrl, type );
      }
    }
  }
}

//--------------------------------------------------------------
/**
 * �f�肱�݁@����I�� 2
 * @param	fmmdl	MMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrReflect_2(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm )
{
	if( prm->reflect_type == MMDL_REFLECT_NON ||
		MMDL_CheckStatusBitReflect(fmmdl) == FALSE ){
		return;
  }

  {
    MAPATTR_FLAG flag = MAPATTR_FLAGBIT_NONE;
		MAPATTR attr = MMDL_GetMapDirAttr( fmmdl, DIR_DOWN );
    
    #ifdef DEBUG_REFLECT_CHECK
    attr = MAPATTR_FLAGBIT_REFLECT << 16;
    #endif
    
    if( attr == MAPATTR_ERROR ){
		  MMDL_SetStatusBitReflect( fmmdl, FALSE );
    }else{
      flag = MAPATTR_GetAttrFlag( attr );

      if( (flag&MAPATTR_FLAGBIT_REFLECT) == 0 ){
		    MMDL_SetStatusBitReflect( fmmdl, FALSE );
      }
    }
  }
}

//======================================================================
//	�A�g���r���[�g�@��
//======================================================================
//--------------------------------------------------------------
/**
 * ���@����J�n 0
 * @param	fmmdl		MMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrBridgeProc_01(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm )
{
	#ifndef MMDL_PL_NULL
	if( MAPATTR_IsBridgeFlag(now) == TRUE ){
		MMDL_SetStatusBitBridge( fmmdl, TRUE );
	}else if( MMDL_CheckStatusBitBridge(fmmdl) == TRUE ){
		if( MAPATTR_IsBridge(now) == FALSE ){
			MMDL_SetStatusBitBridge( fmmdl, FALSE );
		}
	}
	#endif
}

//======================================================================
//	�A�g���r���[�g�֘ASE
//======================================================================
//--------------------------------------------------------------
/**
 * SE�@����J�n 1
 * @param	fmmdl	MMDL *
 * @param	now		���݂̃A�g���r���[�g
 * @param	old		�ߋ��̃A�g���r���[�g
 * @param	param		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrSEProc_1(
		MMDL * fmmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm )
{
	#ifndef MMDL_PL_NULL
	if( MAPATTR_IsSnow(now) ){
		Snd_SePlay( SEQ_SE_DP_YUKIASHI );
	}
	#endif
}

//======================================================================
//	�ړ��`�F�b�N
//======================================================================
//--------------------------------------------------------------
/**
 * �w����W��Q���`�F�b�N
 * @param	fos		MMDLSYS *
 * @param	x		�`�F�b�N����X���W	�O���b�h
 * @param	z		�`�F�b�N����Z���W	�O���b�h
 * @retval	u32		�q�b�g�r�b�g�BMMDL_MOVEHITBIT_LIM��
 */
//--------------------------------------------------------------
#ifndef MMDL_PL_NULL
u32 MMDL_PosHitCheck( const MMDLSYS *fos, int x, int z )
{
	u32 ret;
	
	ret = MMDL_MOVEHITBIT_NON;
	
	if(	GetHitAttr(MMDLSYS_FieldSysWorkGet(fos),x,z) == TRUE ){
		ret |= MMDL_MOVEHITBIT_ATTR;
	}
	
	if( MMDLSYS_SearchGridPos(fos,x,z,FALSE) != NULL ){
		ret |= MMDL_MOVEHITBIT_OBJ;
	}
	
	return( ret );
}
#endif

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���ړ��`�F�b�N
 * @param	fmmdl	MMDL *
 * @param	vec		���ݎ����W
 * @param	x		�ړ���X���W	�O���b�h
 * @param	y		�ړ���Y���W
 * @param	z		�ړ���Z���W	�O���b�h
 * @param	dir		�ړ����� DIR_UP��
 * @retval	u32		�q�b�g�r�b�g�BMMDL_MOVEHITBIT_LIM��
 */
//--------------------------------------------------------------
u32 MMDL_HitCheckMove(
	const MMDL *fmmdl, const VecFx32 *vec, s16 x, s16 y, s16 z, u16 dir )
{
	u32 ret;
	VecFx32 pos;
	
	ret = MMDL_MOVEHITBIT_NON;
	
	pos.x = GRID_SIZE_FX32( x ) + GRID_HALF_FX32;
	pos.y = vec->y; //GRID_SIZE_FX32( y );
	pos.z = GRID_SIZE_FX32( z ) + GRID_HALF_FX32;
		
	if( MMDL_HitCheckMoveLimit(fmmdl,x,z) == TRUE ){
		ret |= MMDL_MOVEHITBIT_LIM;
	}
	
	{
		u32 attr;
		fx32 height;
		
		if( MMdl_HitCheckMoveAttr(fmmdl,pos) == TRUE ){
			ret |= MMDL_MOVEHITBIT_ATTR;
		}

		if( MMDL_GetMapPosHeight(fmmdl,&pos,&height) == TRUE ){
			fx32 diff = vec->y - height;
			if( diff < 0 ){ diff = -diff; }
			if( diff >= HEIGHT_DIFF_COLLISION ){
				ret |= MMDL_MOVEHITBIT_HEIGHT;
			}
		}else{
			ret |= MMDL_MOVEHITBIT_HEIGHT;
		}
	}
	
	if( MMDL_HitCheckMoveFellow(fmmdl,x,y,z) == TRUE ){
		ret |= MMDL_MOVEHITBIT_OBJ;
	}
	
	{
		const FLDMAPPER *pG3DMapper;
		
		pG3DMapper = MMDLSYS_GetG3DMapper( MMDL_GetMMdlSys(fmmdl) );
		
		//pos->y��CheckOutRange()���ł͖��]��<090916���_>
		if( FLDMAPPER_CheckOutRange(pG3DMapper,&pos) == TRUE ){
			ret |= MMDL_MOVEHITBIT_OUTRANGE;
		}
	}

	return( ret );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���ړ��`�F�b�N�@���݈ʒu���画��
 * @param	fmmdl	MMDL * 
 * @param	x		�ړ���X���W	�O���b�h
 * @param	y		�ړ���Y���W
 * @param	z		�ړ���Z���W	�O���b�h
 * @param	dir		�ړ����� DIR_UP��
 * @retval	u32		�q�b�g�r�b�g�BMMDL_MOVEHITBIT_LIM��
 */
//--------------------------------------------------------------
u32 MMDL_HitCheckMoveCurrent(
	const MMDL * fmmdl, s16 x, s16 y, s16 z, u16 dir )
{
	VecFx32 vec;
	MMDL_GetVectorPos( fmmdl, &vec );
	return( MMDL_HitCheckMove(fmmdl,&vec,x,y,z,dir) );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���ړ��`�F�b�N�@���݈ʒu+������
 * @param	fmmdl	MMDL * 
 * @param	dir		�ړ���������BDIR_UP��
 * @retval	u32		�q�b�g�r�b�g�BMMDL_MOVEHITBIT_LIM��
 */
//--------------------------------------------------------------
u32 MMDL_HitCheckMoveDir( const MMDL * fmmdl, u16 dir )
{
	s16 x,y,z;
	x = MMDL_GetGridPosX( fmmdl ) + MMDL_TOOL_GetDirAddValueGridX( dir );
	y = MMDL_GetHeightGrid( fmmdl );
	z = MMDL_GetGridPosZ( fmmdl ) + MMDL_TOOL_GetDirAddValueGridZ( dir );
	return( MMDL_HitCheckMoveCurrent(fmmdl,x,y,z,dir) );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f�����m�̏Փ˃`�F�b�N
 * @param	fmmdl	MMDL * 
 * @param	x		�ړ���X���W	�O���b�h
 * @param	y		�ړ���X���W	�O���b�h
 * @param	z		�ړ���X���W	�O���b�h
 * @retval	BOOL	TRUE=�Փ˃A��
 */
//--------------------------------------------------------------
BOOL MMDL_HitCheckMoveFellow( const MMDL * fmmdl, s16 x, s16 y, s16 z )
{
	u32 no=0;
	int hx,hz;
	MMDL *cmmdl;
	const MMDLSYS *sys;
	
	sys = MMDL_GetMMdlSys( fmmdl );
	
	while( MMDLSYS_SearchUseMMdl(sys,&cmmdl,&no) == TRUE ){
		if( cmmdl != fmmdl ){
			if( MMDL_CheckStatusBit(
				cmmdl,MMDL_STABIT_FELLOW_HIT_NON) == 0 ){
				hx = MMDL_GetGridPosX( cmmdl );
				hz = MMDL_GetGridPosZ( cmmdl );
				{
					BOOL debug = FALSE;
					if( MMDL_GetOBJID(fmmdl) == MMDL_ID_PLAYER ){
						debug = TRUE;
					}
					if( hx == x && hz == z ){
						int hy = MMDL_GetHeightGrid( cmmdl );
						int sy = hy - y;
						if( sy < 0 ){ sy = -sy; }
						#ifdef DEBUG_ONLY_FOR_kagaya
						if( debug ){
							OS_Printf(
								"HERO %d, %d, %d NPC %d %d %d\n",
								x, y, z, hx, hy, hz );
						}
						#endif
						if( sy < H_GRID_FELLOW_SIZE ){
							return( TRUE );
						}
					}
				
					hx = MMDL_GetOldGridPosX( cmmdl );
					hz = MMDL_GetOldGridPosZ( cmmdl );
				
					if( hx == x && hz == z ){
						int hy = MMDL_GetHeightGrid( cmmdl );
						int sy = hy - y;
						if( sy < 0 ){ sy = -sy; }
						if( sy < H_GRID_FELLOW_SIZE ){
							return( TRUE );
						}
					}
				}
			}
		}
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���ړ������`�F�b�N
 * @param	fmmdl	MMDL * 
 * @param	x		�ړ���X���W	�O���b�h
 * @param	y		�ړ���Y���W	�O���b�h
 * @param	z		�ړ���Z���W	�O���b�h
 * @retval	BOOL	TRUE=�����z��
 */
//--------------------------------------------------------------
BOOL MMDL_HitCheckMoveLimit( const MMDL * fmmdl, s16 x, s16 z )
{
	s16 init,limit,min,max;
	
	init = MMDL_GetInitGridPosX( fmmdl );
	limit = MMDL_GetMoveLimitX( fmmdl );
	
	if( limit != MOVE_LIMIT_NOT ){
		min = init - limit;
		max = init + limit;
	
		if( min > x || max < x ){
			return( TRUE );
		}
	}
	
	init = MMDL_GetInitGridPosZ( fmmdl );
	limit = MMDL_GetMoveLimitZ( fmmdl );
	
	if( limit != MOVE_LIMIT_NOT ){
		min = init - limit;
		max = init + limit;
	
		if( min > z || max < z ){
			return( TRUE );
		}
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���A�g���r���[�g�q�b�g�`�F�b�N
 * @param	fmmdl	MMDL * 
 * @param	pos		�ړ���X���W,���݈ʒu��Y,�ړ���Z���W
 * @param	dir		�ړ����� DIR_UP��
 * @retval	int		TRUE=�ړ��s�A�g���r���[�g
 */
//--------------------------------------------------------------
static BOOL MMdl_HitCheckMoveAttr(
	const MMDL * fmmdl, const VecFx32 pos )
{
	if( MMDL_CheckMoveBitAttrGetOFF(fmmdl) == FALSE ){
		MAPATTR attr;
		
		if( MMDL_GetMapPosAttr(fmmdl,&pos,&attr) == TRUE ){
      return MAPATTR_GetHitchFlag(attr);
		}
	}
	
	return( TRUE ); //�ړ��s�A�g���r���[�g
}

//--------------------------------------------------------------
///	���݈ʒu�A�g���r���[�g���画�肷��ړ�����A�g���r���[�g�`�F�b�N�֐�
//--------------------------------------------------------------
#ifndef MMDL_PL_NULL
static BOOL (* const DATA_HitCheckAttr_Now[DIR_MAX4])( MAPATTR attr ) =
{
	MAPATTR_IsBadMoveUpCheck,
	MAPATTR_IsBadMoveDownCheck,
	MAPATTR_IsBadMoveLeftCheck,
	MAPATTR_IsBadMoveRightCheck,
};

//--------------------------------------------------------------
///	�����ʒu�A�g���r���[�g���画�肷��ړ�����A�g���r���[�g�`�F�b�N�֐�
//--------------------------------------------------------------
static BOOL (* const DATA_HitCheckAttr_Next[DIR_MAX4])( MAPATTR attr ) =
{
	MAPATTR_IsBadMoveDownCheck,
	MAPATTR_IsBadMoveUpCheck,
	MAPATTR_IsBadMoveRightCheck,
	MAPATTR_IsBadMoveLeftCheck,
};
#endif

//--------------------------------------------------------------
/**
 * �����l���������A�g���r���[�g�`�F�b�N
 * @param	fmmdl	MMDL *
 * @param	attr	�A�g���r���[�g
 * @retval	BOOL	TRUE=���A�g���r���[�g
 */
//--------------------------------------------------------------
static BOOL MMdl_CheckMapAttrKind_Water( MMDL * fmmdl, u32 attr )
{
	#ifndef MMDL_PL_NULL
	if( MAPATTR_IsBridgeWater(attr) ){
		if( MMDL_CheckStatusBitBridge(fmmdl) == FALSE ){
			return( TRUE );
		}
	}else if( MAPATTR_IsWater(attr) ){
		return( TRUE );
	}
	#endif
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �����l���������A�g���r���[�g�`�F�b�N
 * @param	fmmdl	MMDL *
 * @param	attr	�A�g���r���[�g
 * @retval	int		TRUE=���A�g���r���[�g
 */
//--------------------------------------------------------------
static BOOL MMdl_CheckMapAttrKind_Sand( MMDL * fmmdl, u32 attr )
{
	#ifndef MMDL_PL_NULL
	if( MAPATTR_IsBridgeSand(attr) ){
		if( MMDL_CheckStatusBitBridge(fmmdl) == FALSE ){
			return( TRUE );
		}
	}else if( MAPATTR_IsSand(attr) ){
		return( TRUE );
	}
	#endif
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �����l��������A�g���r���[�g�`�F�b�N
 * @param	fmmdl	MMDL *
 * @param	attr	�A�g���r���[�g
 * @retval	int		TRUE=���A�g���r���[�g
 */
//--------------------------------------------------------------
static BOOL MMdl_CheckMapAttrKind_Snow( MMDL * fmmdl, u32 attr )
{
	#ifndef MMDL_PL_NULL
	if( MAPATTR_IsBridgeSnowShallow(attr) ){
		if( MMDL_CheckStatusBitBridge(fmmdl) == FALSE ){
			return( TRUE );
		}
	}else if( MAPATTR_IsSnow(attr) ){
		return( TRUE );
	}
	#endif
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �����l�������󂢐�A�g���r���[�g�`�F�b�N
 * @param	fmmdl	MMDL *
 * @param	attr	�A�g���r���[�g
 * @retval	int		TRUE=���A�g���r���[�g
 */
//--------------------------------------------------------------
static BOOL MMdl_CheckMapAttrKind_MostShallowSnow(
		MMDL * fmmdl, u32 attr )
{
	#ifndef MMDL_PL_NULL
	if( MAPATTR_IsBridgeSnowShallow(attr) ){
		if( MMDL_CheckStatusBitBridge(fmmdl) == FALSE ){
			return( TRUE );
		}
	}else if( MAPATTR_IsMostShallowSnow(attr) ){
		return( TRUE );
	}
	#endif	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �����l���������A�g���r���[�g�`�F�b�N
 * @param	fmmdl	MMDL *
 * @param	attr	�A�g���r���[�g
 * @retval	int		TRUE=���A�g���r���[�g
 */
//--------------------------------------------------------------
static BOOL MMdl_CheckMapAttrKind_Bridge( MMDL * fmmdl, u32 attr )
{
	if( MMDL_CheckStatusBitBridge(fmmdl) == TRUE ){
		#ifndef MMDL_PL_NULL
		if( MAPATTR_IsBridge(attr) == TRUE ){
			return( TRUE );
		}
		#endif
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �����l��������{���c�A�g���r���[�g�`�F�b�N
 * @param	fmmdl	MMDL *
 * @param	attr	�A�g���r���[�g
 * @retval	int		TRUE=���A�g���r���[�g
 */
//--------------------------------------------------------------
static BOOL MMdl_CheckMapAttrKind_BridgeV( MMDL * fmmdl, u32 attr )
{
	if( MMDL_CheckStatusBitBridge(fmmdl) == TRUE ){
	#ifndef MMDL_PL_NULL
		if( MAPATTR_IsBridgeV(attr) == TRUE ){
			return( TRUE );
		}
	#endif
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �����l��������{�����A�g���r���[�g�`�F�b�N
 * @param	fmmdl	MMDL *
 * @param	attr	�A�g���r���[�g
 * @retval	int		TRUE=���A�g���r���[�g
 */
//--------------------------------------------------------------
static BOOL MMdl_CheckMapAttrKind_BridgeH( MMDL * fmmdl, u32 attr )
{
	if( MMDL_CheckStatusBitBridge(fmmdl) == TRUE ){
	#ifndef MMDL_PL_NULL
		if( MAPATTR_IsBridgeH(attr) == TRUE ){
			return( TRUE );
		}
	#endif
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �}�b�v�O���b�h�����擾
 * @param	fmmdl	MMDL
 * @param	pos	�擾������W
 * @param	pGridInfo ���i�[��
 * @retval	BOOL FALSE=�}�b�v��񂪖���
 */
//--------------------------------------------------------------
static BOOL MMdl_GetMapGridInfo(
	const MMDL *fmmdl, const VecFx32 *pos, FLDMAPPER_GRIDINFO *pGridInfo )
{
	const FLDMAPPER *pG3DMapper =
		MMDLSYS_GetG3DMapper( MMDL_GetMMdlSys(fmmdl) );
	
	if( FLDMAPPER_GetGridInfo(pG3DMapper,pos,pGridInfo) == TRUE ){
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �}�b�v�O���b�h���擾
 * @param	fmmdl	MMDL
 * @param	pos     	�擾������W(x,z�Ɏ擾�������ʒu���W�Ay�ɂ͌��݂�height���i�[����Ă��邱��)
 * @param	pGridData	�O���b�h�A�g���r���[�g�f�[�^�i�[��
 * @retval	0���� ��񂪎擾�ł��Ȃ�
 *
 *  @li GridInfo�Ɏ擾���ꂽ�S�K�w���T�[�`���A���݂�Y�ɍł��߂�height,attr��Ԃ��܂�
 *  @li GridInfo�̔z��0�̕����v���C�I���e�B�������ł�
 */
//--------------------------------------------------------------
static int MMdl_GetMapPosGridData(
	const MMDL *fmmdl, const VecFx32 *pos, FLDMAPPER_GRIDINFODATA* pGridData)
{
	const FLDMAPPER *pG3DMapper =
		MMDLSYS_GetG3DMapper( MMDL_GetMMdlSys(fmmdl) );
	
	return FLDMAPPER_GetGridData(pG3DMapper,pos,pGridData);
}

//--------------------------------------------------------------
/**
 * �}�b�v�A�g���r���[�g�擾
 * @param	fmmdl	MMDL
 * @param	pos	�擾������W(x,z�Ɏ擾�������ʒu���W�Ay�ɂ͌��݂�height���i�[����Ă��邱��)
 * @param	attr	�A�g���r���[�g�i�[��
 * @retval	BOOL	FALSE=�A�g���r���[�g�����݂��Ȃ��B
 */
//--------------------------------------------------------------
BOOL MMDL_GetMapPosAttr(
	const MMDL *fmmdl, const VecFx32 *pos, u32 *attr )
{
	FLDMAPPER_GRIDINFODATA gridData;
	*attr = 0;
	
	if( MMdl_GetMapPosGridData(fmmdl,pos,&gridData) == TRUE ){
		*attr = gridData.attr;
		return( TRUE );
	}
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �}�b�v�����擾
 * @param	fmmdl	MMDL
 * @param	pos	�擾������W
 * @param	height	�����i�[��
 * @retval	BOOL	FALSE=���������݂��Ȃ��B
 */
//--------------------------------------------------------------
BOOL MMDL_GetMapPosHeight(
	const MMDL *fmmdl, const VecFx32 *pos, fx32 *height )
{
#if 1
	FLDMAPPER_GRIDINFODATA gridData;
	
  *height = 0;
	if( MMdl_GetMapPosGridData(fmmdl,pos,&gridData) == TRUE ){
		*height = gridData.height;
		return( TRUE );
	}
#else
	FLDMAPPER_GRIDINFO GridInfo;
  *height = 0;

	if( MMdl_GetMapGridInfo(fmmdl,pos,&GridInfo) == TRUE ){
		if( GridInfo.count ){
			int		i = 0;
			fx32	h_tmp,diff1,diff2;
			
			*height = GridInfo.gridData[i].height;
			i++;
			
			while( i < GridInfo.count ){
				h_tmp = GridInfo.gridData[i].height;
				diff1 = *height - pos->y;
				diff2 = h_tmp - pos->y;
				
				if( FX_Mul(diff2,diff2) < FX_Mul(diff1,diff1) ){
					*height = h_tmp;
				}
				i++;
			}
			
			return( TRUE );
		}
	}
#endif
	return( FALSE );
}

//======================================================================
//	�O���b�h���W�ړ��n
//======================================================================
//--------------------------------------------------------------
/**
 * �����Ō��ݍ��W���X�V�B�����͍X�V����Ȃ��B
 * @param	fmmdl		MMDL * 
 * @param	dir			�ړ�����
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_UpdateGridPosDir( MMDL * fmmdl, u16 dir )
{
	MMDL_SetOldGridPosX( fmmdl, MMDL_GetGridPosX(fmmdl) );
	MMDL_SetOldGridPosY( fmmdl, MMDL_GetGridPosY(fmmdl) );
	MMDL_SetOldGridPosZ( fmmdl, MMDL_GetGridPosZ(fmmdl) );
	
	MMDL_AddGridPosX( fmmdl, MMDL_TOOL_GetDirAddValueGridX(dir) );
	MMDL_AddGridPosY( fmmdl, 0 );
	MMDL_AddGridPosZ( fmmdl, MMDL_TOOL_GetDirAddValueGridZ(dir) );
}

//--------------------------------------------------------------
/**
 * ���ݍ��W�ŉߋ����W���X�V
 * @param	fmmdl		MMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_UpdateGridPosCurrent( MMDL * fmmdl )
{
	MMDL_SetOldGridPosX( fmmdl, MMDL_GetGridPosX(fmmdl) );
	MMDL_SetOldGridPosY( fmmdl, MMDL_GetHeightGrid(fmmdl) );
	MMDL_SetOldGridPosZ( fmmdl, MMDL_GetGridPosZ(fmmdl) );
}

//--------------------------------------------------------------
/**
 * ���݂̃O���b�h���W����w�������̃A�g���r���[�g���擾
 * @param	fmmdl	MMDL *
 * @param	dir		DIR_UP��
 * @retval	u32		MAPATTR
 */
//--------------------------------------------------------------
MAPATTR MMDL_GetMapDirAttr( MMDL * fmmdl, u16 dir )
{
	MAPATTR attr = MAPATTR_ERROR;
  VecFx32 pos;
  const FLDMAPPER *pG3DMapper;
  
  MMDL_GetVectorPos( fmmdl, &pos );
  MMDL_TOOL_AddDirVector( dir, &pos, GRID_FX32 );
  MMDL_GetMapPosAttr( fmmdl, &pos, &attr );
  
	return( attr );
}

//======================================================================
//	3D���W�n
//======================================================================
//--------------------------------------------------------------
/**
 * �w��l�����Z����
 * @param	fmmdl		MMDL * 
 * @param	val			�ړ���
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_AddVectorPos( MMDL * fmmdl, const VecFx32 *val )
{
	VecFx32 vec;
	MMDL_GetVectorPos( fmmdl, &vec );
	vec.x += val->x;
	vec.y += val->y;
	vec.z += val->z;
	MMDL_SetVectorPos( fmmdl, &vec );
}

//--------------------------------------------------------------
/**
 * �w��l���w������ɉ��Z����
 * @param	fmmdl		MMDL * 
 * @param	dir			�ړ�4�����BDIR_UP��
 * @param	val			�ړ���(��
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_AddVectorPosDir( MMDL *fmmdl, u16 dir, fx32 val )
{
	VecFx32 vec;
	MMDL_GetVectorPos( fmmdl, &vec );
	switch( dir ){
	case DIR_UP:		vec.z -= val;	break;
	case DIR_DOWN:		vec.z += val;	break;
	case DIR_LEFT:		vec.x -= val;	break;
	case DIR_RIGHT:		vec.x += val;	break;
	}
	MMDL_SetVectorPos( fmmdl, &vec );
}

//--------------------------------------------------------------
/**
 * ���ݍ��W���獂���擾�������W�ɔ��f
 * MMDL_STABIT_HEIGHT_GET_ERROR�̃Z�b�g�������čs��
 * @param	fmmdl		MMDL * 
 * @retval	BOOL	TRUE=��������ꂽ�BFALSE=���Ȃ��B
 * MMDL_STABIT_HEIGHT_GET_ERROR�Ŏ擾�\
 */
//--------------------------------------------------------------
BOOL MMDL_UpdateCurrentHeight( MMDL * fmmdl )
{
	VecFx32 vec_pos,vec_pos_h;
	
	MMDL_GetVectorPos( fmmdl, &vec_pos );
	vec_pos_h = vec_pos;
	
	if( MMDL_CheckStatusBitHeightGetOFF(fmmdl) == TRUE ){
		MMDL_OffStatusBit( fmmdl, MMDL_STABIT_HEIGHT_GET_ERROR );
		return( FALSE );
	}
	
	{
		fx32 height;
		int ret = MMDL_GetMapPosHeight( fmmdl, &vec_pos_h, &height );
		
		if( ret == TRUE ){
			vec_pos.y = height;
			MMDL_SetVectorPos( fmmdl, &vec_pos );
			MMDL_OffStatusBit( fmmdl, MMDL_STABIT_HEIGHT_GET_ERROR );
		}else{
			MMDL_OnStatusBit( fmmdl, MMDL_STABIT_HEIGHT_GET_ERROR );
		}
		return( ret );
	}
}

//--------------------------------------------------------------
/**
 * ���ݍ��W�ŃA�g���r���[�g���f
 * MMDL_STABIT_ATTR_GET_ERROR�̃Z�b�g�������čs��
 * @retval	BOOL TRUE=�擾�����BFALSE=���s�B
 */
//--------------------------------------------------------------
BOOL MMDL_UpdateCurrentMapAttr( MMDL * fmmdl )
{
	MAPATTR old_attr = MAPATTR_ERROR;
	MAPATTR now_attr = old_attr;
	
  if( MMDL_CheckMoveBitAttrGetOFF(fmmdl) == FALSE ){
    VecFx32 pos;
		int gx = MMDL_GetOldGridPosX( fmmdl );
		int gy = MMDL_GetOldGridPosY( fmmdl );
		int gz = MMDL_GetOldGridPosZ( fmmdl );
    
    pos.x = GRID_SIZE_FX32( gx );
    pos.y = GRID_SIZE_FX32( gy );
    pos.z = GRID_SIZE_FX32( gz );
    MMDL_GetMapPosAttr( fmmdl, &pos, &old_attr );
    
    gx = MMDL_GetGridPosX( fmmdl );
    gz = MMDL_GetGridPosZ( fmmdl );
    MMDL_TOOL_GetCenterGridPos( gx, gz, &pos );
    pos.y = MMDL_GetVectorPosY( fmmdl );
    MMDL_GetMapPosAttr( fmmdl, &pos, &now_attr );
  }
  
	MMDL_SetMapAttrOld( fmmdl, old_attr );
	MMDL_SetMapAttr( fmmdl, now_attr );
  
  if( now_attr == MAPATTR_ERROR ){
		MMDL_OnStatusBit( fmmdl, MMDL_STABIT_ATTR_GET_ERROR );
		return( FALSE );
  }
  
	MMDL_OffStatusBit( fmmdl, MMDL_STABIT_ATTR_GET_ERROR );
	return( TRUE );
}

//======================================================================
//	�`�揉�����A���A����ɌĂ΂�铮�쏉�����֐�
//======================================================================
//--------------------------------------------------------------
/**
 * �`�揉�����A���A����ɌĂ΂�铮�쏉�����֐��B
 * �B�ꖪ�n����͓���֐����ŃG�t�F�N�g�n�̃Z�b�g���s���Ă���B
 * �C�x���g���̃|�[�Y�����������ォ�甭�����Ă���ꍇ�A
 * ����֐����Ă΂�Ȃ��ׁA�`�揉�������ɂ��̊֐����ĂԎ��ő΍�B
 * @param	fmmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_CallMoveProcAfterDrawInit( MMDL * fmmdl )
{
	u16 code = MMDL_GetMoveCode( fmmdl );
	if( code == MV_HIDE_SNOW || code == MV_HIDE_SAND ||
		code == MV_HIDE_GRND || code == MV_HIDE_KUSA ){
		MMDL_CallMoveProc( fmmdl );
	}
}

//======================================================================
//	����c�[��
//======================================================================
//--------------------------------------------------------------
///	4�����ʍ��W�����@X
//--------------------------------------------------------------
static const int DATA_DirGridValueX[] =
{ 0, 0, -1, 1 };

//--------------------------------------------------------------
///	4�����ʍ��W�����@Y
//--------------------------------------------------------------
static const int DATA_DirGridValueY[] =
{ 0, 0, 0, 0 };

//--------------------------------------------------------------
///	4�����ʍ��W�����@Z
//--------------------------------------------------------------
static const int DATA_DirGridValueZ[] =
{ -1, 1, 0, 0 };

//--------------------------------------------------------------
/**
 * ��������X���������l���擾
 * @param	dir		�����BDIR_UP��
 * @retval	s16 �O���b�h�P�ʂł̑����l
 */
//--------------------------------------------------------------
s16 MMDL_TOOL_GetDirAddValueGridX( u16 dir )
{
	return( (s16)DATA_DirGridValueX[dir] );
}

//--------------------------------------------------------------
/**
 * ��������Z���������l���擾
 * @param	dir		�����BDIR_UP��
 * @retval	int		�O���b�h�P�ʂł̑����l
 */
//--------------------------------------------------------------
s16 MMDL_TOOL_GetDirAddValueGridZ( u16 dir )
{
	return( (s16)DATA_DirGridValueZ[dir] );
}

//--------------------------------------------------------------
/**
 * �w��l���w������ɉ��Z����
 * @param	dir			�ړ�4�����BDIR_UP��
 * @param	vec			������Ώ�
 * @param	val			�ړ���(��
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_TOOL_AddDirVector( u16 dir, VecFx32 *vec, fx32 val )
{
	switch( dir ){
	case DIR_UP:		vec->z -= val;	break;
	case DIR_DOWN:		vec->z += val;	break;
	case DIR_LEFT:		vec->x -= val;	break;
	case DIR_RIGHT:		vec->x += val;	break;
	}
}

//--------------------------------------------------------------
/**
 * �w��l���w������ɉ��Z����@�O���b�h
 * @param	dir			�ړ�4�����BDIR_UP��
 * @param	gx      ������Ώہ@�O���b�hx
 * @param gz      ������Ώہ@�O���b�hz
 * @param	val			�ړ���(��
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_TOOL_AddDirGrid( u16 dir, s16 *gx, s16 *gz, s16 val )
{
	switch( dir ){
	case DIR_UP: *gz -= val; break;
	case DIR_DOWN: *gz += val; break;
	case DIR_LEFT: *gx -= val; break;
	case DIR_RIGHT: *gx += val; break;
	}
}

//--------------------------------------------------------------
/**
 * �w�肳�ꂽ�O���b�h���W�̒��S�ʒu�������W�Ŏ擾
 * @param	gx	�O���b�hX���W	
 * @param	gz	�O���b�hZ���W
 * @param	vec	���W�i�[��
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_TOOL_GetCenterGridPos( s16 gx, s16 gz, VecFx32 *vec )
{
	vec->x = GRID_SIZE_FX32( gx ) + GRID_HALF_FX32;
	vec->z = GRID_SIZE_FX32( gz ) + GRID_HALF_FX32;
}

//--------------------------------------------------------------
/**
 * �w�肳�ꂽ�����W���O���b�h�P�ʂɕύX
 * @param	gx	�O���b�hX���W�i�[��
 * @param	gy	�O���b�hY���W�i�[��
 * @param	gz	�O���b�hZ���W�i�[��
 * @param	vec	�ϊ�������W
 * @retval	nothing
 */
//--------------------------------------------------------------
void MMDL_TOOL_VectorPosToGridPos(
	s16 *gx, s16 *gy, s16 *gz, const VecFx32 *vec )
{
	*gx = SIZE_GRID_FX32( vec->x );
	*gy = SIZE_GRID_FX32( vec->y );
	*gz = SIZE_GRID_FX32( vec->z );
}

//--------------------------------------------------------------
/**
 * �w�肳�ꂽ�O���b�h���W�������W�ɕύX
 * @param	gx	�O���b�hX���W
 * @param	gy	�O���b�hY���W
 * @param	gz	�O���b�hZ���W
 * @param	vec	���W�ϊ���
 * @retval nothing
 */
//--------------------------------------------------------------
void MMDL_TOOL_GridPosToVectorPos(
    const s16 gx, const s16 gy, const s16 gz, VecFx32 *pos )
{
  pos->x = GRID_SIZE_FX32( gx );
  pos->y = GRID_SIZE_FX32( gy );
  pos->z = GRID_SIZE_FX32( gz );
}

//--------------------------------------------------------------
//	�������]�e�[�u��
//--------------------------------------------------------------
static const u32 DATA_DirFlipTbl[] =
{ DIR_DOWN, DIR_UP, DIR_RIGHT, DIR_LEFT };

//--------------------------------------------------------------
/**
 * �^����ꂽ�����𔽓]����
 * @param	dir		DIR_UP��
 * @retval	int		dir�̔��]����
 */
//--------------------------------------------------------------
u16 MMDL_TOOL_FlipDir( u16 dir )
{
	return( DATA_DirFlipTbl[dir] );
}

//--------------------------------------------------------------
/**
 * �����ԕ������擾
 * @param	ax	�Ώ�A X���W
 * @param	az	�Ώ�A Z���W
 * @param	bx	�Ώ�B X���W
 * @param	bz	�Ώ�B Z���W
 * @retval	int	�Ώ�A����Ώ�B�ւ̕��� DIR_UP��
 */
//--------------------------------------------------------------
u16 MMDL_TOOL_GetRangeDir( int ax, int az, int bx, int bz )
{
	if( ax > bx ){ return( DIR_LEFT ); }
	if( ax < bx ){ return( DIR_RIGHT ); }
	if( az > bz ){ return( DIR_UP ); }
	return( DIR_DOWN );
}

//--------------------------------------------------------------
/**
 * FLDEFF_CTRL�擾
 * @param mmdl  MMDL*
 * @retval FLDEFF_CTRL*
 */
//--------------------------------------------------------------
static FLDEFF_CTRL * mmdl_GetFldEffCtrl( MMDL *mmdl )
{
  const MMDLSYS *mmdlsys = MMDL_GetMMdlSys( mmdl );
  FIELDMAP_WORK *fieldMapWork = MMDLSYS_GetFieldMapWork( (MMDLSYS*)mmdlsys );
  return( FIELDMAP_GetFldEffCtrl(fieldMapWork) );
}

static const u8 data_angle8_4[8] = { 0, 1, 1, 2, 2, 3, 3, 0 };

static const u8 data_angleChange360_4[4][4] =
{
  {DIR_UP,DIR_DOWN,DIR_LEFT,DIR_RIGHT}, //0
  {DIR_RIGHT,DIR_LEFT,DIR_UP,DIR_DOWN}, //1
  {DIR_DOWN,DIR_UP,DIR_RIGHT,DIR_LEFT}, //2
  {DIR_LEFT,DIR_RIGHT,DIR_DOWN,DIR_UP}, //3
};

//--------------------------------------------------------------
/**
 * �J�������p�x����\������S�������擾
 * @param dir �p�x0���̕����B
 * @param angleYaw �J�����p�x
 * @retval u16 DIR_UP��
 */
//--------------------------------------------------------------
u16 MMDL_TOOL_GetAngleYawToDirFour( u16 dir, u16 angleYaw )
{
  angleYaw /= 0x2000; // /8
  angleYaw = data_angle8_4[angleYaw];
  angleYaw = data_angleChange360_4[angleYaw][dir];
  GF_ASSERT( angleYaw < DIR_MAX4 );
  return( angleYaw );
}

