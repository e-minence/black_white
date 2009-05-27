//======================================================================
/**
 *
 * @file	fieldobj_move.c
 * @brief	�t�B�[���h���샂�f�� ����n
 * @author	kagaya
 * @data	05.07.25
 *
 */
//======================================================================
#include "fldmmdl.h"

#include "fieldmap.h"

#include "fldeff_shadow.h"
#include "fldeff_kemuri.h"

//======================================================================
//	define
//======================================================================
//--------------------------------------------------------------
///	�X�e�[�^�X�r�b�g
//--------------------------------------------------------------
///�ړ�������֎~����X�e�[�^�X�r�b�g
#define STA_BIT_MOVE_ERROR (FLDMMDL_STABIT_HEIGHT_GET_ERROR|FLDMMDL_STABIT_ATTR_GET_ERROR)

//--------------------------------------------------------------
///	�A�g���r���[�g�I�t�Z�b�g
//--------------------------------------------------------------
#define ATTROFFS_Y_NUMA (NUM_FX32(-12))///<��Y�I�t�Z�b�g
#define ATTROFFS_Y_NUMA_DEEP (NUM_FX32(-14))///<�[����Y�I�t�Z�b�g
#define ATTROFFS_Y_YUKI (NUM_FX32(-12))///<��Y�I�t�Z�b�g
#define ATTROFFS_Y_YUKI_DEEP (NUM_FX32(-14))///<�[����Y�I�t�Z�b�g
#define ATTROFFS_Y_YUKI_DEEP_MOST (NUM_FX32(-16))///<�X�ɐ[����Y�I�t�Z�b�g

//======================================================================
//	struct
//======================================================================

//======================================================================
//	proto
//======================================================================
static int FldMMdl_CheckMoveStart( const FLDMMDL * fmmdl );
static void FldMMdl_GetHeightMoveBefore( FLDMMDL * fmmdl );
static void FldMMdl_GetAttrMoveBefore( FLDMMDL * fmmdl );
static void FldMMdl_ProcMoveStartFirst( FLDMMDL * fmmdl );
static void FldMMdl_ProcMoveStartSecond( FLDMMDL * fmmdl );
static void FldMMdl_ProcMoveEnd( FLDMMDL * fmmdl );

static void FldMMdl_MapAttrProc_MoveStartFirst( FLDMMDL * fmmdl );
static void FldMMdl_MapAttrProc_MoveStartSecond( FLDMMDL * fmmdl );
static void FldMMdl_MapAttrProc_MoveStartJumpSecond( FLDMMDL * fmmdl );
static void FldMMdl_MapAttrProc_MoveEnd( FLDMMDL * fmmdl );
static void FldMMdl_MapAttrProc_MoveEndJump( FLDMMDL * fmmdl );

static void FldMMdl_MapAttrHeight_02(
		FLDMMDL * fmmdl,MATR now, MATR old, const OBJCODE_PARAM *prm );

static void FldMMdl_MapAttrGrassProc_0(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );
static void FldMMdl_MapAttrGrassProc_12(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );

static void FldMMdl_MapAttrFootMarkProc_1(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );

static void FldMMdl_MapAttrSplashProc_012(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );
static void FldMMdl_MapAttrSplashProc_Jump1(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );

static void FldMMdl_MapAttrShadowProc_0(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );
static void FldMMdl_MapAttrShadowProc_1(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );
static void FldMMdl_MapAttrShadowProc_2(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );

static void FldMMdl_MapAttrGroundSmokeProc_2(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );

static void FldMMdl_MapAttrLGrassProc_0(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );
static void FldMMdl_MapAttrLGrassProc_1(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );

static void FldMMdl_MapAttrNGrassProc_0(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );
static void FldMMdl_MapAttrNGrassProc_1(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );

static void FldMMdl_MapAttrPoolProc_1(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );
static void FldMMdl_MapAttrPoolProc_2(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );

static void FldMMdl_MapAttrSwampProc_1(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );
static void FldMMdl_MapAttrSwampProc_2(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );

static void FldMMdl_MapAttrReflect_01(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );
static void FldMMdl_MapAttrReflect_2(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );

static void FldMMdl_MapAttrBridgeProc_01(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );

static void FldMMdl_MapAttrSEProc_1(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );

static u32 FldMMdl_HitCheckMoveCurrent(
	const FLDMMDL * fmmdl, s16 x, s16 y, s16 z, u16 dir );
static BOOL FldMMdl_HitCheckMoveAttr(
	const FLDMMDL * fmmdl, s16 x, s16 z, u16 dir );

#ifndef FLDMMDL_PL_NULL
static BOOL (* const DATA_HitCheckAttr_Now[DIR_MAX4])( MATR attr );
static BOOL (* const DATA_HitCheckAttr_Next[DIR_MAX4])( MATR attr );
#endif

static BOOL FldMMdl_GetMapGridInfo(
	const FLDMMDL *fmmdl, const VecFx32 *pos,
	FLDMAPPER_GRIDINFO *pGridInfo );

//======================================================================
//	�t�B�[���h���샂�f�� ����
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f�� ���쏉����
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_InitMoveProc( FLDMMDL * fmmdl )
{
	FLDMMDL_CallMoveInitProc( fmmdl );
	FLDMMDL_MoveSubProcInit( fmmdl );
	FLDMMDL_OnMoveBit( fmmdl, FLDMMDL_MOVEBIT_MOVEPROC_INIT );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f������
 * @param	fmmdl		FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_UpdateMove( FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckFldMMdlSysStatusBit(
		fmmdl,FLDMMDLSYS_STABIT_MOVE_PROC_STOP) ){
		return;
	}
	
	FldMMdl_GetHeightMoveBefore( fmmdl );
	FldMMdl_GetAttrMoveBefore( fmmdl );
	FldMMdl_ProcMoveStartFirst( fmmdl );
	
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_ACMD) ){
		FLDMMDL_ActionAcmd( fmmdl );
	}else if( FLDMMDL_CheckStatusBitMoveProcPause(fmmdl) == FALSE ){
		if( FldMMdl_CheckMoveStart(fmmdl) == TRUE ){
			if( FLDMMDL_MoveSub(fmmdl) == FALSE ){
				FLDMMDL_CallMoveProc( fmmdl );
			}
		}
	}
	
	FldMMdl_ProcMoveStartSecond( fmmdl );
	FldMMdl_ProcMoveEnd( fmmdl );
}

//--------------------------------------------------------------
/**
 * ����\���ǂ����`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	int		TRUE=�\,FALSE=�s��
 */
//--------------------------------------------------------------
static int FldMMdl_CheckMoveStart( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckStatusBitMove(fmmdl) == TRUE ){
		return( TRUE );
	}
	
	if( FLDMMDL_CheckStatusBit(fmmdl,STA_BIT_MOVE_ERROR) == 0 ){
		return( TRUE );
	}else if( FLDMMDL_GetMoveCode(fmmdl) == MV_TR_PAIR ){ //�e�̍s���ɏ]��
		return( TRUE );
	}
	
	#ifndef FLDMMDL_PL_NULL
	{	//�ړ��֎~�t���O���E�`�F�b�N
		u32 st = FLDMMDL_GetStatusBit( fmmdl );
		
		//�����擾���Ȃ��ꍇ
		if( (st&FLDMMDL_STABIT_HEIGHT_GET_ERROR) &&
			(st&FLDMMDL_STABIT_HEIGHT_GET_OFF) == 0 ){
			return( FALSE );
		}
		
		//�A�g���r���[�g�擾���Ȃ��ꍇ
		if( (st&FLDMMDL_STABIT_ATTR_GET_ERROR) &&	
			FLDMMDL_CheckMoveBitAttrGetOFF(fmmdl) == FALSE ){
			return( FALSE );
		}
	}
	#endif
	
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * �ړ��J�n���ɍs�������擾
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_GetHeightMoveBefore( FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_HEIGHT_GET_ERROR) ){
		FLDMMDL_UpdateCurrentHeight( fmmdl );
	}
}

//--------------------------------------------------------------
/**
 * �ړ��J�n���ɍs���A�g���r���[�g�擾
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_GetAttrMoveBefore( FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_ATTR_GET_ERROR) ){
		if( FLDMMDL_UpdateCurrentMapAttr(fmmdl) == TRUE ){
			FLDMMDL_OnStatusBitMoveStart( fmmdl );
		}
	}
}

//--------------------------------------------------------------
/**
 * �ړ��J�n�Ŕ����@1st
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_ProcMoveStartFirst( FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_MOVE_START) ){
		FldMMdl_MapAttrProc_MoveStartFirst( fmmdl );
	}
	
	FLDMMDL_OffStatusBit( fmmdl,
		FLDMMDL_STABIT_MOVE_START | FLDMMDL_STABIT_JUMP_START );
}

//--------------------------------------------------------------
/**
 * �ړ��J�n�Ŕ����@2nd
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_ProcMoveStartSecond( FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_JUMP_START) ){
		FldMMdl_MapAttrProc_MoveStartJumpSecond( fmmdl );
	}else if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_MOVE_START) ){
		FldMMdl_MapAttrProc_MoveStartSecond( fmmdl );
	}
	
	FLDMMDL_OffStatusBit( fmmdl,
		FLDMMDL_STABIT_MOVE_START | FLDMMDL_STABIT_JUMP_START );
}

//--------------------------------------------------------------
/**
 * ����I���Ŕ���
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_ProcMoveEnd( FLDMMDL * fmmdl )
{
	if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_JUMP_END) ){
		FldMMdl_MapAttrProc_MoveEndJump( fmmdl );
	}else if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_MOVE_END) ){
		FldMMdl_MapAttrProc_MoveEnd( fmmdl );
	}
	
	FLDMMDL_OffStatusBit( fmmdl,
		FLDMMDL_STABIT_MOVE_END | FLDMMDL_STABIT_JUMP_END );
}

//======================================================================
//	�}�b�v�A�g���r���[�g
//======================================================================
//--------------------------------------------------------------
/**
 * ����J�n�Ŕ�������A�g���r���[�g���� 1st
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_MapAttrProc_MoveStartFirst( FLDMMDL * fmmdl )
{
	FLDMMDL_UpdateCurrentMapAttr( fmmdl );
	
	if( FLDMMDL_CheckCompletedDrawInit(fmmdl) == TRUE ){
		MATR now = FLDMMDL_GetMapAttr( fmmdl );
		MATR old = FLDMMDL_GetMapAttrOld( fmmdl );
		const OBJCODE_PARAM *prm =
			FLDMMDL_GetOBJCodeParam( fmmdl, FLDMMDL_GetOBJCode(fmmdl) );
		
		FldMMdl_MapAttrBridgeProc_01( fmmdl, now, old, prm );
		FldMMdl_MapAttrGrassProc_0( fmmdl, now, old, prm );
		FldMMdl_MapAttrSplashProc_012( fmmdl, now, old, prm );
		FldMMdl_MapAttrShadowProc_0( fmmdl, now, old, prm );
		FldMMdl_MapAttrHeight_02( fmmdl, now, old, prm );
		FldMMdl_MapAttrLGrassProc_0( fmmdl, now, old, prm );
		FldMMdl_MapAttrNGrassProc_0( fmmdl, now, old, prm );
		FldMMdl_MapAttrReflect_01( fmmdl, now, old, prm );
	}
}

//--------------------------------------------------------------
/**
 * ����J�n�Ŕ�������A�g���r���[�g���� 2nd
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_MapAttrProc_MoveStartSecond( FLDMMDL * fmmdl )
{
	FLDMMDL_UpdateCurrentMapAttr( fmmdl );
	
	if( FLDMMDL_CheckCompletedDrawInit(fmmdl) == TRUE ){
		MATR now = FLDMMDL_GetMapAttr( fmmdl );
		MATR old = FLDMMDL_GetMapAttrOld( fmmdl );
		const OBJCODE_PARAM *prm =
			FLDMMDL_GetOBJCodeParam( fmmdl, FLDMMDL_GetOBJCode(fmmdl) );

		FldMMdl_MapAttrBridgeProc_01( fmmdl, now, old, prm );
		FldMMdl_MapAttrGrassProc_12( fmmdl, now, old, prm );
		FldMMdl_MapAttrFootMarkProc_1( fmmdl, now, old, prm );
		FldMMdl_MapAttrSplashProc_012( fmmdl, now, old, prm );
		FldMMdl_MapAttrShadowProc_1( fmmdl, now, old, prm );
		FldMMdl_MapAttrLGrassProc_1( fmmdl, now, old, prm );
		FldMMdl_MapAttrNGrassProc_1( fmmdl, now, old, prm );
		FldMMdl_MapAttrPoolProc_1( fmmdl, now, old, prm );
		FldMMdl_MapAttrSwampProc_1( fmmdl, now, old, prm );
		FldMMdl_MapAttrReflect_01( fmmdl, now, old, prm );
		
		FldMMdl_MapAttrSEProc_1( fmmdl, now, old, prm ); //�`��֌W�Ȃ�?
	}
}

//--------------------------------------------------------------
/**
 * ����J�n�Ŕ�������A�g���r���[�g���� 2nd Jump
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_MapAttrProc_MoveStartJumpSecond( FLDMMDL * fmmdl )
{
	FLDMMDL_UpdateCurrentMapAttr( fmmdl );
	
	if( FLDMMDL_CheckCompletedDrawInit(fmmdl) == TRUE ){
		MATR now = FLDMMDL_GetMapAttr( fmmdl );
		MATR old = FLDMMDL_GetMapAttrOld( fmmdl );
		const OBJCODE_PARAM *prm =
			FLDMMDL_GetOBJCodeParam( fmmdl, FLDMMDL_GetOBJCode(fmmdl) );

		FldMMdl_MapAttrBridgeProc_01( fmmdl, now, old, prm );
		FldMMdl_MapAttrShadowProc_1( fmmdl, now, old, prm );
		FldMMdl_MapAttrReflect_01( fmmdl, now, old, prm );
		FldMMdl_MapAttrSplashProc_Jump1( fmmdl, now, old, prm );
		FldMMdl_MapAttrSEProc_1( fmmdl, now, old, prm );//�`��֌W�Ȃ�?
	}
}

//--------------------------------------------------------------
/**
 * ����I���Ŕ�������A�g���r���[�g�����@end
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_MapAttrProc_MoveEnd( FLDMMDL * fmmdl )
{
	FLDMMDL_UpdateCurrentMapAttr( fmmdl );
	
	if( FLDMMDL_CheckCompletedDrawInit(fmmdl) == TRUE ){
		MATR now = FLDMMDL_GetMapAttr( fmmdl );
		MATR old = FLDMMDL_GetMapAttrOld( fmmdl );
		const OBJCODE_PARAM *prm =
			FLDMMDL_GetOBJCodeParam( fmmdl, FLDMMDL_GetOBJCode(fmmdl) );
		
		//�I���@�A�g���r���[�g����
		FldMMdl_MapAttrHeight_02( fmmdl, now, old, prm );
		FldMMdl_MapAttrPoolProc_2( fmmdl, now, old, prm );
		FldMMdl_MapAttrSwampProc_2( fmmdl, now, old, prm );
		FldMMdl_MapAttrSplashProc_012( fmmdl, now, old, prm );
		FldMMdl_MapAttrReflect_2( fmmdl, now, old, prm );
		FldMMdl_MapAttrShadowProc_2( fmmdl, now, old, prm );
	}
}

//--------------------------------------------------------------
/**
 * ����I���Ŕ�������A�g���r���[�g�����@end jump
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_MapAttrProc_MoveEndJump( FLDMMDL * fmmdl )
{
	FLDMMDL_UpdateCurrentMapAttr( fmmdl );
	
	if( FLDMMDL_CheckCompletedDrawInit(fmmdl) == TRUE ){
		MATR now = FLDMMDL_GetMapAttr( fmmdl );
		MATR old = FLDMMDL_GetMapAttrOld( fmmdl );
		const OBJCODE_PARAM *prm =
			FLDMMDL_GetOBJCodeParam( fmmdl, FLDMMDL_GetOBJCode(fmmdl) );
		
		//�I���@�A�g���r���[�g����
		FldMMdl_MapAttrHeight_02( fmmdl, now, old, prm );
		FldMMdl_MapAttrPoolProc_2( fmmdl, now, old, prm );
		FldMMdl_MapAttrSwampProc_2( fmmdl, now, old, prm );
		FldMMdl_MapAttrSplashProc_012( fmmdl, now, old, prm );
		FldMMdl_MapAttrReflect_2( fmmdl, now, old, prm );
		FldMMdl_MapAttrShadowProc_2( fmmdl, now, old, prm );
		FldMMdl_MapAttrGrassProc_12( fmmdl, now, old, prm );
		FldMMdl_MapAttrGroundSmokeProc_2( fmmdl, now, old, prm );
	}
}

//======================================================================
//	�A�g���r���[�g�ύ���
//======================================================================
//--------------------------------------------------------------
/**
 * �A�g���r���[�g�ύ����@����J�n�A����I��
 * @param	fmmdl		FLDMMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_MapAttrHeight_02(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
	#ifndef FLDMMDL_PL_NULL
	if( FLDMMDL_CheckStatusBitAttrOffsetOFF(fmmdl) == FALSE ){
		if( MATR_IsSwampDeep(now) == TRUE ||
			MATR_IsSwampGrassDeep(now) == TRUE ){
			VecFx32 vec = { 0, ATTROFFS_Y_NUMA_DEEP, 0 };
			FLDMMDL_SetVectorAttrDrawOffsetPos( fmmdl, &vec );
			return;
		}
		
		if( MATR_IsSwamp(now) == TRUE || MATR_IsSwampGrass(now) == TRUE ){
			VecFx32 vec = { 0, ATTROFFS_Y_NUMA, 0 };
			FLDMMDL_SetVectorAttrDrawOffsetPos( fmmdl, &vec );
			return;
		}
		
		if( MATR_IsSnowDeepMost(now) == TRUE ){
			VecFx32 vec = { 0, ATTROFFS_Y_YUKI_DEEP_MOST, 0 };
			FLDMMDL_SetVectorAttrDrawOffsetPos( fmmdl, &vec );
			return;
		}
		
		if( MATR_IsSnowDeep(now) == TRUE ){
			VecFx32 vec = { 0, ATTROFFS_Y_YUKI_DEEP, 0 };
			FLDMMDL_SetVectorAttrDrawOffsetPos( fmmdl, &vec );
			return;
		}
		
		if( MATR_IsShallowSnow(now) == TRUE ){
			VecFx32 vec = { 0, ATTROFFS_Y_YUKI, 0 };
			FLDMMDL_SetVectorAttrDrawOffsetPos( fmmdl, &vec );
			return;
		}
	}
	
	{
		VecFx32 vec = { 0, 0, 0 };
		FLDMMDL_SetVectorAttrDrawOffsetPos( fmmdl, &vec );
	}
	#endif
}

//======================================================================
//	�A�g���r���[�g�@��
//======================================================================
//--------------------------------------------------------------
/**
 * ���@����J�n 0
 * @param	fmmdl		FLDMMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_MapAttrGrassProc_0(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
	#ifndef FLDMMDL_PL_NULL
	if( MATR_IsGrass(now) == TRUE ){
		FE_fmmdlGrass_Add( fmmdl, FALSE );
	}
	#endif
}

//--------------------------------------------------------------
/**
 * ���@���� 1,2
 * @param	fmmdl	FLDMMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_MapAttrGrassProc_12(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
	#ifndef FLDMMDL_PL_NULL
	if( MATR_IsGrass(now) == TRUE ){
		FE_fmmdlGrass_Add( fmmdl, TRUE );
	}
	#endif
}

//======================================================================
//	�A�g���r���[�g�@����
//======================================================================
//--------------------------------------------------------------
/**
 * ���Ձ@����J�n 1
 * @param	fmmdl	FLDMMDL *
 * @param	now		���݂̃A�g���r���[�g
 * @param	old		�ߋ��̃A�g���r���[�g
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_MapAttrFootMarkProc_1(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
	#ifndef FLDMMDL_PL_NULL
	if( prm->footmark_type == FLDMMDL_FOOTMARK_NON ){
		return;
	}
	
	if( MATR_IsShadowOnSnow(old) == TRUE ){
		if( prm->footmark_type == FLDMMDL_FOOTMARK_NORMAL ){
			FE_fmmdlFootMarkShadowSnow_Add( fmmdl );
		}else if( prm->footmark_type == FLDMMDL_FOOTMARK_CYCLE ){
			FE_fmmdlFootMarkShadowSnowCycle_Add( fmmdl );
		}
	}
	
	if(	FldMMdl_CheckMapAttrKind_Sand(fmmdl,old) == TRUE ){
		if( prm->footmark_type == FLDMMDL_FOOTMARK_NORMAL ){
			FE_fmmdlFootMarkNormal_Add( fmmdl );
		}else if( prm->footmark_type == FLDMMDL_FOOTMARK_CYCLE ){
			FE_fmmdlFootMarkCycle_Add( fmmdl );
		}
		return;
	}
	
	if( MATR_IsSnowDeep(old) == TRUE || MATR_IsSnowDeepMost(old) == TRUE ||
		MATR_IsShallowSnow(old) ){
		FE_fmmdlFootMarkSnowDeep_Add( fmmdl );
		return;
	}
	
	if( FldMMdl_CheckMapAttrKind_MostShallowSnow(fmmdl,old) == TRUE ){
		FE_fmmdlFootMarkSnow_Add( fmmdl );
		return;
	}
	#endif
}

//======================================================================
//	�A�g���r���[�g�@����
//======================================================================
//--------------------------------------------------------------
/**
 * ���򖗁@����J�n�I�� 012
 * @param	fmmdl	FLDMMDL *
 * @param	now		���݂̃A�g���r���[�g
 * @param	old		�ߋ��̃A�g���r���[�g
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_MapAttrSplashProc_012(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
	#ifndef FLDMMDL_PL_NULL
	if( MATR_IsShoal(now) == TRUE ){
		if( FLDMMDL_CheckStatusBitShoalEffect(fmmdl) == FALSE ){
			FE_fmmdlSplash_Add( fmmdl, TRUE );
			FLDMMDL_SetStatusBitShoalEffect( fmmdl, TRUE );
		}
	}else{
		FLDMMDL_SetStatusBitShoalEffect( fmmdl, FALSE );
	}
	#endif
}

//--------------------------------------------------------------
/**
 * ���򖗁@�W�����v����J�n 1
 * @param	fmmdl	FLDMMDL *
 * @param	now		���݂̃A�g���r���[�g
 * @param	old		�ߋ��̃A�g���r���[�g
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_MapAttrSplashProc_Jump1(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
	FLDMMDL_SetStatusBitShoalEffect( fmmdl, FALSE );
}

//======================================================================
//	�A�g���r���[�g�@�e
//======================================================================
//--------------------------------------------------------------
/**
 * �e�@����J�n 0
 * @param	fmmdl	FLDMMDL *
 * @param	now		���݂̃A�g���r���[�g
 * @param	old		�ߋ��̃A�g���r���[�g
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_MapAttrShadowProc_0(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
	FldMMdl_MapAttrShadowProc_1( fmmdl, now, old, prm );
}

//--------------------------------------------------------------
/**
 * �e�@����J�n 1
 * @param	fmmdl	FLDMMDL *
 * @param	now		���݂̃A�g���r���[�g
 * @param	old		�ߋ��̃A�g���r���[�g
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_MapAttrShadowProc_1(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
	#ifndef FLDMMDL_PL_NULL
	{
		const FLDMMDLSYS *fos = FLDMMDL_GetFldMMdlSys( fmmdl );
		
		if( FLDMMDLSYS_CheckJoinShadow(fos) == FALSE ){
			return;
		}
	}
	
	if( prm->shadow_type == FLDMMDL_SHADOW_NON ){
		return;
	}

	if( MATR_IsGrass(now) == TRUE ||
		MATR_IsLongGrass(now) == TRUE ||
		FldMMdl_CheckMapAttrKind_Water(fmmdl,now) == TRUE ||
		MATR_IsPoolCheck(now) == TRUE ||
		MATR_IsShoal(now) == TRUE ||
		FldMMdl_CheckMapAttrKind_Snow(fmmdl,now) == TRUE ||
		MATR_IsSwamp(now) == TRUE ||
		MATR_IsSwampGrass(now) == TRUE ||
		MATR_IsMirrorReflect(now) ){
		FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_SHADOW_VANISH );
	}else{
		if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_SHADOW_SET) == 0 ){
			FE_fmmdlShadow_Add( fmmdl );
			FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_SHADOW_SET );
		}
	}
	#else
  {
  	const FLDMMDLSYS *fos = FLDMMDL_GetFldMMdlSys( fmmdl );
    
		if( FLDMMDLSYS_CheckJoinShadow(fos) == FALSE ){
			return;
		}
    
		if( FLDMMDL_CheckStatusBit(fmmdl,FLDMMDL_STABIT_SHADOW_SET) == 0 ){
      FLDMMDLSYS *fos = (FLDMMDLSYS*)FLDMMDL_GetFldMMdlSys( fmmdl );
      FIELDMAP_WORK *fieldMapWork = FLDMMDLSYS_GetFieldMapWork( fos );
      FLDEFF_CTRL *fectrl = FIELDMAP_GetFldEffCtrl( fieldMapWork );
      FLDEFF_SHADOW_SetFldMMdl( fmmdl, fectrl );
			FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_SHADOW_SET );
    }
  }
  #endif
}

//--------------------------------------------------------------
/**
 * �e�@����I�� 2
 * @param	fmmdl	FLDMMDL *
 * @param	now		���݂̃A�g���r���[�g
 * @param	old		�ߋ��̃A�g���r���[�g
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_MapAttrShadowProc_2(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
	#ifndef FLDMMDL_PL_NULL
	{
		const FLDMMDLSYS *fos = FLDMMDL_GetFldMMdlSys( fmmdl );
		
		if( FLDMMDLSYS_CheckJoinShadow(fos) == FALSE ){
			return;
		}
	}
	
	if( prm->shadow_type == FLDMMDL_SHADOW_NON ){
		return;
	}
	
	if( MATR_IsGrass(now) == TRUE ||
		MATR_IsLongGrass(now) == TRUE ||
		FldMMdl_CheckMapAttrKind_Water(fmmdl,now) == TRUE ||
		MATR_IsPoolCheck(now) == TRUE ||
		MATR_IsShoal(now) == TRUE ||
		FldMMdl_CheckMapAttrKind_Snow(fmmdl,now) == TRUE ||
		MATR_IsSwamp(now) == TRUE ||
		MATR_IsSwampGrass(now) == TRUE ||
		MATR_IsMirrorReflect(now) ){
		FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_SHADOW_VANISH );
	}else{
		FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_SHADOW_VANISH );
	}
	#endif
}

//======================================================================
//	�A�g���r���[�g�@�y��
//======================================================================
//--------------------------------------------------------------
/**
 * �y���@����I�� 2 
 * @param	fmmdl	FLDMMDL *
 * @param	now		���݂̃A�g���r���[�g
 * @param	old		�ߋ��̃A�g���r���[�g
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_MapAttrGroundSmokeProc_2(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
	#ifndef FLDMMDL_PL_NULL
	if( FldMMdl_CheckMapAttrKind_Water(fmmdl,now) == TRUE ||
		MATR_IsShoal(now) == TRUE ||
		MATR_IsIce(now) == TRUE ||
		MATR_IsSwamp(now) == TRUE ||
		MATR_IsSwampGrass(now) == TRUE ||
		FldMMdl_CheckMapAttrKind_Snow(fmmdl,now) == TRUE ){
		return;
	}
	
	FE_fmmdlKemuri_Add( fmmdl );
	#else
  {
    FLDMMDLSYS *fos = (FLDMMDLSYS*)FLDMMDL_GetFldMMdlSys( fmmdl );
    FIELDMAP_WORK *fieldMapWork = FLDMMDLSYS_GetFieldMapWork( fos );
    FLDEFF_CTRL *fectrl = FIELDMAP_GetFldEffCtrl( fieldMapWork );
    FLDEFF_KEMURI_SetFldMMdl( fmmdl, fectrl );
  }
  #endif
}

//======================================================================
//	�A�g���r���[�g�@������
//======================================================================
//--------------------------------------------------------------
/**
 * �������@����J�n 0
 * @param	fmmdl		FLDMMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_MapAttrLGrassProc_0(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
	#ifndef FLDMMDL_PL_NULL
	if( MATR_IsLongGrass(now) == TRUE ){
		FE_fmmdlLGrass_Add( fmmdl, FALSE );
	}
	#endif
}

//--------------------------------------------------------------
/**
 * �������@����J�n 1
 * @param	fmmdl	FLDMMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_MapAttrLGrassProc_1(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
	#ifndef FLDMMDL_PL_NULL
	if( MATR_IsLongGrass(now) == TRUE ){
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
 * @param	fmmdl		FLDMMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_MapAttrNGrassProc_0(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
	#ifndef FLDMMDL_PL_NULL
	if( MATR_IsSwampGrass(now) == TRUE ){
		FE_fmmdlNGrass_Add( fmmdl, FALSE );
	}
	#endif
}

//--------------------------------------------------------------
/**
 * �����@����J�n 1
 * @param	fmmdl	FLDMMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_MapAttrNGrassProc_1(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
	#ifndef FLDMMDL_PL_NULL
	if( MATR_IsSwampGrass(now) == TRUE ){
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
 * @param	fmmdl	FLDMMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_MapAttrPoolProc_1(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
	#ifndef FLDMMDL_PL_NULL
	if( MATR_IsPoolCheck(old) == TRUE ){
		FE_fmmdlRippleSet( fmmdl,
			FLDMMDL_GetOldGridPosX(fmmdl),
			FLDMMDL_GetOldGridPosY(fmmdl), 
			FLDMMDL_GetOldGridPosZ(fmmdl) );
	}
	#endif
}

//--------------------------------------------------------------
/**
 * �����܂�@����I�� 2
 * @param	fmmdl	FLDMMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_MapAttrPoolProc_2(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
	#ifndef FLDMMDL_PL_NULL
	if( MATR_IsPoolCheck(now) == TRUE ){
		FE_fmmdlRippleSet( fmmdl,
			FLDMMDL_GetGridPosX(fmmdl),
			FLDMMDL_GetGridPosY(fmmdl), 
			FLDMMDL_GetGridPosZ(fmmdl) );
	}
	#endif
}

//======================================================================
//	�A�g���r���[�g�@��
//======================================================================
//--------------------------------------------------------------
/**
 * ���@����J�n 1
 * @param	fmmdl	FLDMMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_MapAttrSwampProc_1(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
	#ifndef FLDMMDL_PL_NULL
	if( MATR_IsSwamp(old) == TRUE ){
		FE_fmmdlNRippleSet( fmmdl,
			FLDMMDL_GetOldGridPosX(fmmdl),
			FLDMMDL_GetOldGridPosY(fmmdl), 
			FLDMMDL_GetOldGridPosZ(fmmdl) );
	}
	#endif
}

//--------------------------------------------------------------
/**
 * ���@����I�� 2
 * @param	fmmdl	FLDMMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_MapAttrSwampProc_2(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
	#ifndef FLDMMDL_PL_NULL
	if( MATR_IsSwamp(now) == TRUE ){
		FE_fmmdlNRippleSet( fmmdl,
			FLDMMDL_GetGridPosX(fmmdl),
			FLDMMDL_GetGridPosY(fmmdl), 
			FLDMMDL_GetGridPosZ(fmmdl) );
	}
	#endif
}

//======================================================================
//	�A�g���r���[�g�@�f�肱��
//======================================================================
//--------------------------------------------------------------
/**
 * �f�肱�݁@����J�n 01
 * @param	fmmdl	FLDMMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_MapAttrReflect_01(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
	#ifndef FLDMMDL_PL_NULL
	if( prm->reflect_type == FLDMMDL_REFLECT_NON ){
		return;
	}
	
	{
		if( FLDMMDL_CheckStatusBitReflect(fmmdl) == FALSE ){
			MATR hit = MATR_IsNotAttrGet();
			
			if( MATR_IsReflect(now) == TRUE ){
				hit = now;
			}else{
				MATR next = FLDMMDL_GetMapDirAttr( fmmdl, DIR_DOWN );
				
				if( MATR_IsReflect(next) == TRUE ){
					hit = next;
				}
			}
			
			if( hit != MATR_IsNotAttrGet() ){	//�f�荞�݃q�b�g
				REFTYPE type;
				FLDMMDL_SetStatusBitReflect( fmmdl, TRUE );
				
				if( MATR_IsMirrorReflect(hit) == TRUE ){ 	//����
					type = REFTYPE_MIRROR;
				}else if( MATR_IsPoolCheck(hit) == TRUE ){	//������
					type = REFTYPE_POOL;
				}else{										//�r
					type = REFTYPE_POND;
				}
				
				FE_fmmdlReflect_Add( fmmdl, type );
			}
		}
	}
	#endif
}

//--------------------------------------------------------------
/**
 * �f�肱�݁@����I�� 2
 * @param	fmmdl	FLDMMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_MapAttrReflect_2(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
	#ifndef FLDMMDL_PL_NULL
	if( prm->reflect_type == FLDMMDL_REFLECT_NON ||
		FLDMMDL_CheckStatusBitReflect(fmmdl) == FALSE ){
		return;
	}
	
	{
		MATR attr = FLDMMDL_GetMapDirAttr( fmmdl, DIR_DOWN );
		
		if( MATR_IsReflect(attr) == FALSE ){
			FLDMMDL_SetStatusBitReflect( fmmdl, FALSE );
		}
	}
	#endif
}

//======================================================================
//	�A�g���r���[�g�@��
//======================================================================
//--------------------------------------------------------------
/**
 * ���@����J�n 0
 * @param	fmmdl		FLDMMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_MapAttrBridgeProc_01(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
	#ifndef FLDMMDL_PL_NULL
	if( MATR_IsBridgeFlag(now) == TRUE ){
		FLDMMDL_SetStatusBitBridge( fmmdl, TRUE );
	}else if( FLDMMDL_CheckStatusBitBridge(fmmdl) == TRUE ){
		if( MATR_IsBridge(now) == FALSE ){
			FLDMMDL_SetStatusBitBridge( fmmdl, FALSE );
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
 * @param	fmmdl	FLDMMDL *
 * @param	now		���݂̃A�g���r���[�g
 * @param	old		�ߋ��̃A�g���r���[�g
 * @param	param		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMMdl_MapAttrSEProc_1(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
	#ifndef FLDMMDL_PL_NULL
	if( MATR_IsSnow(now) ){
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
 * @param	fos		FLDMMDLSYS *
 * @param	x		�`�F�b�N����X���W	�O���b�h
 * @param	z		�`�F�b�N����Z���W	�O���b�h
 * @retval	u32		�q�b�g�r�b�g�BFLDMMDL_MOVEHITBIT_LIM��
 */
//--------------------------------------------------------------
#ifndef FLDMMDL_PL_NULL
u32 FLDMMDL_PosHitCheck( const FLDMMDLSYS *fos, int x, int z )
{
	u32 ret;
	
	ret = FLDMMDL_MOVEHITBIT_NON;
	
	if(	GetHitAttr(FLDMMDLSYS_FieldSysWorkGet(fos),x,z) == TRUE ){
		ret |= FLDMMDL_MOVEHITBIT_ATTR;
	}
	
	if( FLDMMDLSYS_SearchGridPos(fos,x,z,FALSE) != NULL ){
		ret |= FLDMMDL_MOVEHITBIT_OBJ;
	}
	
	return( ret );
}
#endif

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���ړ��`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @param	vec		���ݎ����W
 * @param	x		�ړ���X���W	�O���b�h
 * @param	y		�ړ���Y���W
 * @param	z		�ړ���Z���W	�O���b�h
 * @param	dir		�ړ����� DIR_UP��
 * @retval	u32		�q�b�g�r�b�g�BFLDMMDL_MOVEHITBIT_LIM��
 */
//--------------------------------------------------------------
u32 FLDMMDL_HitCheckMove(
	const FLDMMDL *fmmdl, const VecFx32 *vec, s16 x, s16 y, s16 z, u16 dir )
{
	u32 ret;
	
	ret = FLDMMDL_MOVEHITBIT_NON;
	
	if( FLDMMDL_HitCheckMoveLimit(fmmdl,x,y,z) == TRUE ){
		ret |= FLDMMDL_MOVEHITBIT_LIM;
	}
	
	#ifndef FLDMMDL_PL_NULL
	{
		s8 flag;
		FIELDSYS_WORK *fsys = FLDMMDL_FieldSysWorkGet( fmmdl );
		if( MPTL_CheckHitWall(fsys,vec,x,z,&flag) == TRUE ){
			ret |= FLDMMDL_MOVEHITBIT_ATTR;
			
			if( flag != HIT_RES_EQUAL ){
				ret |= FLDMMDL_MOVEHITBIT_HEIGHT;
			}
		}
	}
	
	if( FldMMdl_HitCheckMoveAttr(fmmdl,x,z,dir) == TRUE ){
		ret |= FLDMMDL_MOVEHITBIT_ATTR;
	}
	
	if( FLDMMDL_HitCheckMoveFellow(fmmdl,x,y,z) == TRUE ){
		ret |= FLDMMDL_MOVEHITBIT_OBJ;
	}
	#else
	{
		u32 attr;
		fx32 height;
		VecFx32 pos;
		
		pos.x = GRID_SIZE_FX32( x ) + GRID_HALF_FX32;
		pos.y = GRID_SIZE_FX32( y );
		pos.z = GRID_SIZE_FX32( z ) + GRID_HALF_FX32;
		
		if( FldMMdl_HitCheckMoveAttr(fmmdl,x,z,dir) == TRUE ){
			ret |= FLDMMDL_MOVEHITBIT_ATTR;
		}

		if( FLDMMDL_GetMapPosHeight(fmmdl,&pos,&height) == TRUE ){
			fx32 diff = vec->y - height;
			if( diff < 0 ){ diff = -diff; }
			if( diff >= (FX32_ONE*20) ){
				ret |= FLDMMDL_MOVEHITBIT_HEIGHT;
			}
		}else{
			ret |= FLDMMDL_MOVEHITBIT_HEIGHT;
		}
	}
	
	if( FLDMMDL_HitCheckMoveFellow(fmmdl,x,y,z) == TRUE ){
		ret |= FLDMMDL_MOVEHITBIT_OBJ;
	}
	
	{
		VecFx32 pos;
		const FLDMAPPER *pG3DMapper;
		
		pG3DMapper = FLDMMDLSYS_GetG3DMapper( FLDMMDL_GetFldMMdlSys(fmmdl) );
		pos.x = GRID_SIZE_FX32( x ) + GRID_HALF_FX32;
		pos.y = GRID_SIZE_FX32( y );
		pos.z = GRID_SIZE_FX32( z ) + GRID_HALF_FX32;
		
		if( FLDMAPPER_CheckOutRange(pG3DMapper,&pos) == TRUE ){
			ret |= FLDMMDL_MOVEHITBIT_OUTRANGE;
		}
	}
	#endif

	return( ret );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���ړ��`�F�b�N�@���݈ʒu���画��
 * @param	fmmdl	FLDMMDL * 
 * @param	x		�ړ���X���W	�O���b�h
 * @param	y		�ړ���X���W
 * @param	z		�ړ���X���W	�O���b�h
 * @param	dir		�ړ����� DIR_UP��
 * @retval	u32		�q�b�g�r�b�g�BFLDMMDL_MOVEHITBIT_LIM��
 */
//--------------------------------------------------------------
static u32 FldMMdl_HitCheckMoveCurrent(
	const FLDMMDL * fmmdl, s16 x, s16 y, s16 z, u16 dir )
{
	VecFx32 vec;
	FLDMMDL_GetVectorPos( fmmdl, &vec );
	return( FLDMMDL_HitCheckMove(fmmdl,&vec,x,y,z,dir) );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���ړ��`�F�b�N�@���݈ʒu+������
 * @param	fmmdl	FLDMMDL * 
 * @param	dir		�ړ���������BDIR_UP��
 * @retval	u32		�q�b�g�r�b�g�BFLDMMDL_MOVEHITBIT_LIM��
 */
//--------------------------------------------------------------
u32 FLDMMDL_HitCheckMoveDir( const FLDMMDL * fmmdl, u16 dir )
{
	s16 x,y,z;
	x = FLDMMDL_GetGridPosX( fmmdl ) + FLDMMDL_TOOL_GetDirAddValueGridX( dir );
	y = FLDMMDL_GetHeightGrid( fmmdl );
	z = FLDMMDL_GetGridPosZ( fmmdl ) + FLDMMDL_TOOL_GetDirAddValueGridZ( dir );
	return( FldMMdl_HitCheckMoveCurrent(fmmdl,x,y,z,dir) );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f�����m�̏Փ˃`�F�b�N
 * @param	fmmdl	FLDMMDL * 
 * @param	x		�ړ���X���W	�O���b�h
 * @param	y		�ړ���X���W	�O���b�h
 * @param	z		�ړ���X���W	�O���b�h
 * @retval	BOOL	TRUE=�Փ˃A��
 */
//--------------------------------------------------------------
BOOL FLDMMDL_HitCheckMoveFellow( const FLDMMDL * fmmdl, s16 x, s16 y, s16 z )
{
	u32 no=0;
	int hx,hz;
	FLDMMDL *cmmdl;
	const FLDMMDLSYS *sys;
	
	sys = FLDMMDL_GetFldMMdlSys( fmmdl );
	
	while( FLDMMDLSYS_SearchUseFldMMdl(sys,&cmmdl,&no) == TRUE ){
		if( cmmdl != fmmdl ){
			if( FLDMMDL_CheckStatusBit(
				cmmdl,FLDMMDL_STABIT_FELLOW_HIT_NON) == 0 ){
				hx = FLDMMDL_GetGridPosX( cmmdl );
				hz = FLDMMDL_GetGridPosZ( cmmdl );
				#if 0
				if( hx == x && hz == z ){
					int hy = FLDMMDL_GetHeightGrid( cmmdl );
					int sy = hy - y;
					if( sy < 0 ){ sy = -sy; }
					if( sy < H_GRID_FELLOW_SIZE ){
						return( TRUE );
					}
				}
			
				hx = FLDMMDL_GetOldGridPosX( cmmdl );
				hz = FLDMMDL_GetOldGridPosZ( cmmdl );
			
				if( hx == x && hz == z ){
					int hy = FLDMMDL_GetHeightGrid( cmmdl );
					int sy = hy - y;
					if( sy < 0 ){ sy = -sy; }
					if( sy < H_GRID_FELLOW_SIZE ){
						return( TRUE );
					}
				}
				#else
				{
					BOOL debug = FALSE;
					if( FLDMMDL_GetOBJID(fmmdl) == FLDMMDL_ID_PLAYER ){
						debug = TRUE;
					}
					if( hx == x && hz == z ){
						int hy = FLDMMDL_GetHeightGrid( cmmdl );
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
				
					hx = FLDMMDL_GetOldGridPosX( cmmdl );
					hz = FLDMMDL_GetOldGridPosZ( cmmdl );
				
					if( hx == x && hz == z ){
						int hy = FLDMMDL_GetHeightGrid( cmmdl );
						int sy = hy - y;
						if( sy < 0 ){ sy = -sy; }
						if( sy < H_GRID_FELLOW_SIZE ){
							return( TRUE );
						}
					}
				}
				#endif
			}
		}
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���ړ������`�F�b�N
 * @param	fmmdl	FLDMMDL * 
 * @param	x		�ړ���X���W	�O���b�h
 * @param	y		�ړ���Y���W	�O���b�h
 * @param	z		�ړ���Z���W	�O���b�h
 * @retval	BOOL	TRUE=�����z��
 */
//--------------------------------------------------------------
BOOL FLDMMDL_HitCheckMoveLimit( const FLDMMDL * fmmdl, s16 x, s16 y, s16 z )
{
	s16 init,limit,min,max;
	
	init = FLDMMDL_GetInitGridPosX( fmmdl );
	limit = FLDMMDL_GetMoveLimitX( fmmdl );
	
	if( limit != MOVE_LIMIT_NOT ){
		min = init - limit;
		max = init + limit;
	
		if( min > x || max < x ){
			return( TRUE );
		}
	}
	
	init = FLDMMDL_GetInitGridPosZ( fmmdl );
	limit = FLDMMDL_GetMoveLimitZ( fmmdl );
	
	if( limit != MOVE_LIMIT_NOT ){
		min = init - limit;
		max = init + limit;
	
		if( min > z || max < z ){
			return( TRUE );
		}
	}
	
	return( FALSE );
}

#define MAP_ATTR_FLAG_HITCH (1<<0)

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���A�g���r���[�g�q�b�g�`�F�b�N
 * @param	fmmdl	FLDMMDL * 
 * @param	x		�ړ���X���W	�O���b�h
 * @param	z		�ړ���Z���W	�O���b�h
 * @param	dir		�ړ����� DIR_UP��
 * @retval	int		TRUE=�ړ��s�A�g���r���[�g
 */
//--------------------------------------------------------------
static BOOL FldMMdl_HitCheckMoveAttr(
	const FLDMMDL * fmmdl, s16 x, s16 z, u16 dir )
{
	#ifndef FLDMMDL_PL_NULL
	if( FLDMMDL_CheckMoveBitAttrGetOFF(fmmdl) == FALSE ){
		FIELDSYS_WORK *fsys = FLDMMDL_FieldSysWorkGet( fmmdl );
		MATR now_attr = FLDMMDL_GetMapAttr( fmmdl );
		MATR next_attr = GetAttributeLSB( fsys, x, z );
		
		if( next_attr == MATR_IsNotAttrGet() ){ //�A�g���r���[�g�擾���s
			return( TRUE );
		}
		
		if( DATA_HitCheckAttr_Now[dir](now_attr) == TRUE ||
			DATA_HitCheckAttr_Next[dir](next_attr) == TRUE ){
			return( TRUE );
		}
	}
	#else
	if( FLDMMDL_CheckMoveBitAttrGetOFF(fmmdl) == FALSE ){
		u32 attr;
		VecFx32 pos;
		pos.x = GRID_SIZE_FX32( x );
		pos.y = 0;
		pos.z = GRID_SIZE_FX32( z );
		
		if( FLDMMDL_GetMapPosAttr(fmmdl,&pos,&attr) == TRUE ){
#if 0
			if( attr == 0 ){
				return( FALSE );
			}
#else
			u16 val = attr & 0xffff;
			u16 flag = (attr&0xffff0000) >> 16;
			
			if( (flag & MAP_ATTR_FLAG_HITCH) == 0 ){ //�ړ��\�t���O
				return( FALSE );	//�ړ��\�A�g���r���[�g
			}
#endif
		}
	}
	#endif
	
	return( TRUE ); //�ړ��s�A�g���r���[�g
}

//--------------------------------------------------------------
///	���݈ʒu�A�g���r���[�g���画�肷��ړ�����A�g���r���[�g�`�F�b�N�֐�
//--------------------------------------------------------------
#ifndef FLDMMDL_PL_NULL
static BOOL (* const DATA_HitCheckAttr_Now[DIR_MAX4])( MATR attr ) =
{
	MATR_IsBadMoveUpCheck,
	MATR_IsBadMoveDownCheck,
	MATR_IsBadMoveLeftCheck,
	MATR_IsBadMoveRightCheck,
};

//--------------------------------------------------------------
///	�����ʒu�A�g���r���[�g���画�肷��ړ�����A�g���r���[�g�`�F�b�N�֐�
//--------------------------------------------------------------
static BOOL (* const DATA_HitCheckAttr_Next[DIR_MAX4])( MATR attr ) =
{
	MATR_IsBadMoveDownCheck,
	MATR_IsBadMoveUpCheck,
	MATR_IsBadMoveRightCheck,
	MATR_IsBadMoveLeftCheck,
};
#endif

//--------------------------------------------------------------
/**
 * �����l���������A�g���r���[�g�`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @param	attr	�A�g���r���[�g
 * @retval	BOOL	TRUE=���A�g���r���[�g
 */
//--------------------------------------------------------------
static BOOL FldMMdl_CheckMapAttrKind_Water( FLDMMDL * fmmdl, u32 attr )
{
	#ifndef FLDMMDL_PL_NULL
	if( MATR_IsBridgeWater(attr) ){
		if( FLDMMDL_CheckStatusBitBridge(fmmdl) == FALSE ){
			return( TRUE );
		}
	}else if( MATR_IsWater(attr) ){
		return( TRUE );
	}
	#endif
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �����l���������A�g���r���[�g�`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @param	attr	�A�g���r���[�g
 * @retval	int		TRUE=���A�g���r���[�g
 */
//--------------------------------------------------------------
static BOOL FldMMdl_CheckMapAttrKind_Sand( FLDMMDL * fmmdl, u32 attr )
{
	#ifndef FLDMMDL_PL_NULL
	if( MATR_IsBridgeSand(attr) ){
		if( FLDMMDL_CheckStatusBitBridge(fmmdl) == FALSE ){
			return( TRUE );
		}
	}else if( MATR_IsSand(attr) ){
		return( TRUE );
	}
	#endif
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �����l��������A�g���r���[�g�`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @param	attr	�A�g���r���[�g
 * @retval	int		TRUE=���A�g���r���[�g
 */
//--------------------------------------------------------------
static BOOL FldMMdl_CheckMapAttrKind_Snow( FLDMMDL * fmmdl, u32 attr )
{
	#ifndef FLDMMDL_PL_NULL
	if( MATR_IsBridgeSnowShallow(attr) ){
		if( FLDMMDL_CheckStatusBitBridge(fmmdl) == FALSE ){
			return( TRUE );
		}
	}else if( MATR_IsSnow(attr) ){
		return( TRUE );
	}
	#endif
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �����l�������󂢐�A�g���r���[�g�`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @param	attr	�A�g���r���[�g
 * @retval	int		TRUE=���A�g���r���[�g
 */
//--------------------------------------------------------------
static BOOL FldMMdl_CheckMapAttrKind_MostShallowSnow(
		FLDMMDL * fmmdl, u32 attr )
{
	#ifndef FLDMMDL_PL_NULL
	if( MATR_IsBridgeSnowShallow(attr) ){
		if( FLDMMDL_CheckStatusBitBridge(fmmdl) == FALSE ){
			return( TRUE );
		}
	}else if( MATR_IsMostShallowSnow(attr) ){
		return( TRUE );
	}
	#endif	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �����l���������A�g���r���[�g�`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @param	attr	�A�g���r���[�g
 * @retval	int		TRUE=���A�g���r���[�g
 */
//--------------------------------------------------------------
static BOOL FldMMdl_CheckMapAttrKind_Bridge( FLDMMDL * fmmdl, u32 attr )
{
	if( FLDMMDL_CheckStatusBitBridge(fmmdl) == TRUE ){
		#ifndef FLDMMDL_PL_NULL
		if( MATR_IsBridge(attr) == TRUE ){
			return( TRUE );
		}
		#endif
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �����l��������{���c�A�g���r���[�g�`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @param	attr	�A�g���r���[�g
 * @retval	int		TRUE=���A�g���r���[�g
 */
//--------------------------------------------------------------
static BOOL FldMMdl_CheckMapAttrKind_BridgeV( FLDMMDL * fmmdl, u32 attr )
{
	if( FLDMMDL_CheckStatusBitBridge(fmmdl) == TRUE ){
	#ifndef FLDMMDL_PL_NULL
		if( MATR_IsBridgeV(attr) == TRUE ){
			return( TRUE );
		}
	#endif
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �����l��������{�����A�g���r���[�g�`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @param	attr	�A�g���r���[�g
 * @retval	int		TRUE=���A�g���r���[�g
 */
//--------------------------------------------------------------
static BOOL FldMMdl_CheckMapAttrKind_BridgeH( FLDMMDL * fmmdl, u32 attr )
{
	if( FLDMMDL_CheckStatusBitBridge(fmmdl) == TRUE ){
	#ifndef FLDMMDL_PL_NULL
		if( MATR_IsBridgeH(attr) == TRUE ){
			return( TRUE );
		}
	#endif
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �}�b�v�O���b�h�����擾
 * @param	fmmdl	FLDMMDL
 * @param	pos	�擾������W
 * @param	pGridInfo ���i�[��
 * @retval	BOOL FALSE=�}�b�v��񂪖���
 */
//--------------------------------------------------------------
static BOOL FldMMdl_GetMapGridInfo(
	const FLDMMDL *fmmdl, const VecFx32 *pos, FLDMAPPER_GRIDINFO *pGridInfo )
{
	const FLDMAPPER *pG3DMapper =
		FLDMMDLSYS_GetG3DMapper( FLDMMDL_GetFldMMdlSys(fmmdl) );
	
	if( FLDMAPPER_GetGridInfo(pG3DMapper,pos,pGridInfo) == TRUE ){
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �}�b�v�A�g���r���[�g�擾
 * @param	fmmdl	FLDMMDL
 * @param	pos	�擾������W
 * @param	attr	�A�g���r���[�g�i�[��
 * @retval	BOOL	FALSE=�A�g���r���[�g�����݂��Ȃ��B
 */
//--------------------------------------------------------------
BOOL FLDMMDL_GetMapPosAttr(
	const FLDMMDL *fmmdl, const VecFx32 *pos, u32 *attr )
{
	FLDMAPPER_GRIDINFO GridInfo;
	*attr = 0;
	
	if( FldMMdl_GetMapGridInfo(fmmdl,pos,&GridInfo) == TRUE ){
		*attr = GridInfo.gridData[0].attr;
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �}�b�v�����擾
 * @param	fmmdl	FLDMMDL
 * @param	pos	�擾������W
 * @param	attr	�����i�[��
 * @retval	BOOL	FALSE=���������݂��Ȃ��B
 */
//--------------------------------------------------------------
BOOL FLDMMDL_GetMapPosHeight(
	const FLDMMDL *fmmdl, const VecFx32 *pos, fx32 *height )
{
	FLDMAPPER_GRIDINFO GridInfo;
	
	*height = 0;
	
	if( FldMMdl_GetMapGridInfo(fmmdl,pos,&GridInfo) == TRUE ){
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
	
	return( FALSE );
}

//======================================================================
//	�O���b�h���W�ړ��n
//======================================================================
//--------------------------------------------------------------
/**
 * �����Ō��ݍ��W���X�V�B�����͍X�V����Ȃ��B
 * @param	fmmdl		FLDMMDL * 
 * @param	dir			�ړ�����
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_UpdateGridPosDir( FLDMMDL * fmmdl, u16 dir )
{
	FLDMMDL_SetOldGridPosX( fmmdl, FLDMMDL_GetGridPosX(fmmdl) );
	FLDMMDL_SetOldGridPosY( fmmdl, FLDMMDL_GetGridPosY(fmmdl) );
	FLDMMDL_SetOldGridPosZ( fmmdl, FLDMMDL_GetGridPosZ(fmmdl) );
	
	FLDMMDL_AddGridPosX( fmmdl, FLDMMDL_TOOL_GetDirAddValueGridX(dir) );
	FLDMMDL_AddGridPosY( fmmdl, 0 );
	FLDMMDL_AddGridPosZ( fmmdl, FLDMMDL_TOOL_GetDirAddValueGridZ(dir) );
}

//--------------------------------------------------------------
/**
 * ���ݍ��W�ŉߋ����W���X�V
 * @param	fmmdl		FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_UpdateGridPosCurrent( FLDMMDL * fmmdl )
{
	FLDMMDL_SetOldGridPosX( fmmdl, FLDMMDL_GetGridPosX(fmmdl) );
	FLDMMDL_SetOldGridPosY( fmmdl, FLDMMDL_GetHeightGrid(fmmdl) );
	FLDMMDL_SetOldGridPosZ( fmmdl, FLDMMDL_GetGridPosZ(fmmdl) );
}

//--------------------------------------------------------------
/**
 * ���݂̃O���b�h���W����w�������̃A�g���r���[�g���擾
 * @param	fmmdl	FLDMMDL *
 * @param	dir		DIR_UP��
 * @retval	u32		MATR
 */
//--------------------------------------------------------------
u32 FLDMMDL_GetMapDirAttr( FLDMMDL * fmmdl, u16 dir )
{
	#ifndef FLDMMDL_PL_NULL
	int gx,gz;
	gx = FLDMMDL_GetGridPosX( fmmdl ) + FLDMMDL_TOOL_GetDirAddValueGridX( dir );
	gz = FLDMMDL_GetGridPosZ( fmmdl ) + FLDMMDL_TOOL_GetDirAddValueGridZ( dir );
	FIELDSYS_WORK *fsys = FLDMMDL_FieldSysWorkGet( fmmdl );
	MATR attr = GetAttributeLSB( fsys, gx, gz );
	#else
	MATR attr = 0;
	#endif

	return( attr );
}

//======================================================================
//	3D���W�n
//======================================================================
//--------------------------------------------------------------
/**
 * �w��l�����Z����
 * @param	fmmdl		FLDMMDL * 
 * @param	val			�ړ���
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_AddVectorPos( FLDMMDL * fmmdl, const VecFx32 *val )
{
	VecFx32 vec;
	FLDMMDL_GetVectorPos( fmmdl, &vec );
	vec.x += val->x;
	vec.y += val->y;
	vec.z += val->z;
	FLDMMDL_SetVectorPos( fmmdl, &vec );
}

//--------------------------------------------------------------
/**
 * �w��l���w������ɉ��Z����
 * @param	fmmdl		FLDMMDL * 
 * @param	dir			�ړ�4�����BDIR_UP��
 * @param	val			�ړ���(��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_AddVectorPosDir( FLDMMDL *fmmdl, u16 dir, fx32 val )
{
	VecFx32 vec;
	FLDMMDL_GetVectorPos( fmmdl, &vec );
	switch( dir ){
	case DIR_UP:		vec.z -= val;	break;
	case DIR_DOWN:		vec.z += val;	break;
	case DIR_LEFT:		vec.x -= val;	break;
	case DIR_RIGHT:		vec.x += val;	break;
	}
	FLDMMDL_SetVectorPos( fmmdl, &vec );
}

//--------------------------------------------------------------
/**
 * ���ݍ��W���獂���擾�������W�ɔ��f
 * FLDMMDL_STABIT_HEIGHT_GET_ERROR�̃Z�b�g�������čs��
 * @param	fmmdl		FLDMMDL * 
 * @retval	BOOL	TRUE=��������ꂽ�BFALSE=���Ȃ��B
 * FLDMMDL_STABIT_HEIGHT_GET_ERROR�Ŏ擾�\
 */
//--------------------------------------------------------------
BOOL FLDMMDL_UpdateCurrentHeight( FLDMMDL * fmmdl )
{
	VecFx32 vec_pos,vec_pos_h;
	
	FLDMMDL_GetVectorPos( fmmdl, &vec_pos );
	vec_pos_h = vec_pos;
	
	if( FLDMMDL_CheckStatusBitHeightGetOFF(fmmdl) == TRUE ){
		FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_HEIGHT_GET_ERROR );
		return( FALSE );
	}
	
	{
	#ifndef FLDMMDL_PL_NULL
		int eflag = FLDMMDL_CheckStatusBitHeightExpand( fmmdl );
		FIELDSYS_WORK *fsys = FLDMMDL_FieldSysWorkGet( fmmdl );
		int ret = FieldOBJTool_GetHeightExpand( fsys, &vec_pos_h, eflag );
		
		if( ret == TRUE ){
			vec_pos.y = vec_pos_h.y;
			FLDMMDL_SetVectorPos( fmmdl, &vec_pos );
			FLDMMDL_SetOldGridPosY( fmmdl, FLDMMDL_GetHeightGrid(fmmdl) );
			FLDMMDL_SetGridPosY( fmmdl, SIZE_H_GRID_FX32(vec_pos.y) );
			FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_HEIGHT_GET_ERROR );
		}else{
			FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_HEIGHT_GET_ERROR );
		}
	#else
		fx32 height;
		int ret = FLDMMDL_GetMapPosHeight( fmmdl, &vec_pos_h, &height );
		
		if( ret == TRUE ){
			vec_pos.y = height;
			FLDMMDL_SetVectorPos( fmmdl, &vec_pos );
			FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_HEIGHT_GET_ERROR );
		}else{
			FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_HEIGHT_GET_ERROR );
		}
	#endif
		return( ret );
	}
}

//--------------------------------------------------------------
/**
 * ���ݍ��W�ŃA�g���r���[�g���f
 * FLDMMDL_STABIT_ATTR_GET_ERROR�̃Z�b�g�������čs��
 * @retval	BOOL TRUE=�擾�����BFALSE=���s�B
 */
//--------------------------------------------------------------
BOOL FLDMMDL_UpdateCurrentMapAttr( FLDMMDL * fmmdl )
{
	#ifndef FLDMMDL_PL_NULL
	MATR old_attr = MATR_IsNotAttrGet();
	MATR now_attr = old_attr;
	
	if( FLDMMDL_CheckMoveBitAttrGetOFF(fmmdl) == FALSE ){
		int gx = FLDMMDL_GetOldGridPosX( fmmdl );
		int gz = FLDMMDL_GetOldGridPosZ( fmmdl );
		FIELDSYS_WORK *fsys = FLDMMDL_FieldSysWorkGet( fmmdl );
		old_attr = GetAttributeLSB( fsys, gx, gz );
	
		gx = FLDMMDL_GetGridPosX( fmmdl );
		gz = FLDMMDL_GetGridPosZ( fmmdl );
		now_attr = GetAttributeLSB( fsys, gx, gz );
	}
	
	FLDMMDL_SetMapAttrOld( fmmdl, old_attr );
	FLDMMDL_SetMapAttr( fmmdl, now_attr );
	
	if( MATR_IsNotAttr(now_attr) == TRUE ){
		FLDMMDL_OnStatusBit( fmmdl, FLDMMDL_STABIT_ATTR_GET_ERROR );
		return( FALSE );
	}
	
	FLDMMDL_OffStatusBit( fmmdl, FLDMMDL_STABIT_ATTR_GET_ERROR );
	#endif
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
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_CallMoveProcAfterDrawInit( FLDMMDL * fmmdl )
{
	u16 code = FLDMMDL_GetMoveCode( fmmdl );
	if( code == MV_HIDE_SNOW || code == MV_HIDE_SAND ||
		code == MV_HIDE_GRND || code == MV_HIDE_KUSA ){
		FLDMMDL_CallMoveProc( fmmdl );
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
s16 FLDMMDL_TOOL_GetDirAddValueGridX( u16 dir )
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
s16 FLDMMDL_TOOL_GetDirAddValueGridZ( u16 dir )
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
void FLDMMDL_TOOL_AddDirVector( u16 dir, VecFx32 *vec, fx32 val )
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
 * �w�肳�ꂽ�O���b�h���W�̒��S�ʒu�������W�Ŏ擾
 * @param	gx	�O���b�hX���W	
 * @param	gz	�O���b�hZ���W
 * @param	vec	���W�i�[��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_TOOL_GetCenterGridPos( s16 gx, s16 gz, VecFx32 *vec )
{
	vec->x = GRID_SIZE_FX32( gx ) + GRID_HALF_FX32;
	vec->z = GRID_SIZE_FX32( gz ) + GRID_HALF_FX32;
}

//--------------------------------------------------------------
/**
 * �w�肳�ꂽ�����W���O���b�h�P�ʂɒ���
 * @param	gx	�O���b�hX���W�i�[��
 * @param	gy	�O���b�hY���W�i�[��
 * @param	gz	�O���b�hZ���W�i�[��
 * @param	vec	�ϊ�������W
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_TOOL_GetVectorPosGrid(
	s16 *gx, s16 *gy, s16 *gz, const VecFx32 *vec )
{
	*gx = SIZE_GRID_FX32( vec->x );
	*gy = SIZE_GRID_FX32( vec->y );
	*gz = SIZE_GRID_FX32( vec->z );
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
u16 FLDMMDL_TOOL_FlipDir( u16 dir )
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
u16 FLDMMDL_TOOL_GetRangeDir( int ax, int az, int bx, int bz )
{
	if( ax > bx ){ return( DIR_LEFT ); }
	if( ax < bx ){ return( DIR_RIGHT ); }
	if( az > bz ){ return( DIR_UP ); }
	return( DIR_DOWN );
}
