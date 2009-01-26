//******************************************************************************
/**
 *
 * @file	fieldobj_move.c
 * @brief	�t�B�[���h���샂�f�� ����n
 * @author	kagaya
 * @data	05.07.25
 *
 */
//******************************************************************************
#include "fldmmdl.h"

//==============================================================================
//	define
//==============================================================================
//#define DEBUG_FLDMMDL_MOVE_SUB_TEST	//�T�u����e�X�g
#ifdef DEBUG_FLDMMDL_MOVE_SUB_TEST
#define DEBUG_MOVE_SUB_TYPE (EV_TYPE_TRAINER_SPIN_MOVE_L)
#define DEBUG_MOVE_SUB_PARAM (2)
#endif

//--------------------------------------------------------------
///	�X�e�[�^�X�r�b�g
//--------------------------------------------------------------
///�ړ�������֎~����X�e�[�^�X�r�b�g
#define STA_BIT_MOVE_ERROR (FLDMMDL_STABIT_HEIGHT_GET_ERROR|FLDMMDL_STABIT_ATTR_GET_ERROR)

//--------------------------------------------------------------
///	�A�g���r���[�g�I�t�Z�b�g
//--------------------------------------------------------------
#define ATTROFFS_Y_NUMA 			(NUM_FX32(-12))				///<��Y�I�t�Z�b�g
#define ATTROFFS_Y_NUMA_DEEP		(NUM_FX32(-14))				///<�[����Y�I�t�Z�b�g
#define ATTROFFS_Y_YUKI		 		(NUM_FX32(-12))				///<��Y�I�t�Z�b�g
#define ATTROFFS_Y_YUKI_DEEP		(NUM_FX32(-14))				///<�[����Y�I�t�Z�b�g
#define ATTROFFS_Y_YUKI_DEEP_MOST	(NUM_FX32(-16))				///<�X�ɐ[����Y�I�t�Z�b�g

//==============================================================================
//	�v���g�^�C�v
//==============================================================================
static int fmmdl_MoveStartCheck( const FLDMMDL * fmmdl );
static void fmmdl_MoveBeforeHeight( FLDMMDL * fmmdl );
static void fmmdl_MoveBeforeAttr( FLDMMDL * fmmdl );
static void fmmdl_MoveStartFirst( FLDMMDL * fmmdl );
static void fmmdl_MoveStartSecond( FLDMMDL * fmmdl );
static void fmmdl_MoveEnd( FLDMMDL * fmmdl );

static void fmmdl_MapAttrProc_MoveStartFirst( FLDMMDL * fmmdl );
static void fmmdl_MapAttrProc_MoveStartSecond( FLDMMDL * fmmdl );
static void fmmdl_MapAttrProc_MoveStartJumpSecond( FLDMMDL * fmmdl );
static void fmmdl_MapAttrProc_MoveEnd( FLDMMDL * fmmdl );
static void fmmdl_MapAttrProc_MoveEndJump( FLDMMDL * fmmdl );

static void fmmdl_MapAttrHeight_02(
		FLDMMDL * fmmdl,MATR now, MATR old, const OBJCODE_STATE *state );

static void fmmdl_MapAttrGrassProc_0(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state );
static void fmmdl_MapAttrGrassProc_12(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state );

static void fmmdl_MapAttrFootMarkProc_1(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state );

static void fmmdl_MapAttrSplashProc_012(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state );
static void fmmdl_MapAttrSplashProc_Jump1(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state );

static void fmmdl_MapAttrShadowProc_0(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state );
static void fmmdl_MapAttrShadowProc_1(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state );
static void fmmdl_MapAttrShadowProc_2(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state );

static void fmmdl_MapAttrGroundSmokeProc_2(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state );

static void fmmdl_MapAttrLGrassProc_0(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state );
static void fmmdl_MapAttrLGrassProc_1(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state );

static void fmmdl_MapAttrNGrassProc_0(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state );
static void fmmdl_MapAttrNGrassProc_1(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state );

static void fmmdl_MapAttrPoolProc_1(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state );
static void fmmdl_MapAttrPoolProc_2(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state );

static void fmmdl_MapAttrSwampProc_1(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state );
static void fmmdl_MapAttrSwampProc_2(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state );

static void fmmdl_MapAttrReflect_01(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state );
static void fmmdl_MapAttrReflect_2(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state );

static void fmmdl_MapAttrBridgeProc_01(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state );

static void fmmdl_MapAttrSEProc_1(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state );
#if 0
static BOOL (* const DATA_HitCheckAttr_Now[DIR_4_MAX])( MATR attr );
static BOOL (* const DATA_HitCheckAttr_Next[DIR_4_MAX])( MATR attr );
#endif

//==============================================================================
//	�t�B�[���h���샂�f�� ����
//==============================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f�� ���쏉����
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_MoveInit( FLDMMDL * fmmdl )
{
#ifdef DEBUG_FLDMMDL_MOVE_SUB_TEST
	{
		if( FLDMMDL_MoveCodeGet(fmmdl) == MV_RT2 ){
			FLDMMDL_EventTypeSet( fmmdl, DEBUG_MOVE_SUB_TYPE );
			FLDMMDL_ParamSet( fmmdl, DEBUG_MOVE_SUB_PARAM, FLDMMDL_PARAM_1 );
		}
	}
#endif
	
	FLDMMDL_MoveInitProcCall( fmmdl );
	FLDMMDL_MoveSubProcInit( fmmdl );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f������
 * @param	fmmdl		FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_Move( FLDMMDL * fmmdl )
{
	if( FLDMMDL_FieldOBJSysStatusBitCheck(fmmdl,FLDMMDLSYS_STABIT_MOVE_PROC_STOP) ){
		return;
	}
	
	fmmdl_MoveBeforeHeight( fmmdl );
	fmmdl_MoveBeforeAttr( fmmdl );
	fmmdl_MoveStartFirst( fmmdl );
	
	if( FLDMMDL_StatusBit_Check(fmmdl,FLDMMDL_STABIT_ACMD) ){
		FLDMMDL_AcmdAction( fmmdl );
	}else if( FLDMMDL_MovePauseCheck(fmmdl) == FALSE ){
		if( fmmdl_MoveStartCheck(fmmdl) == TRUE ){
			if( FLDMMDL_MoveSub(fmmdl) == FALSE ){
				FLDMMDL_MoveProcCall( fmmdl );
			}
		}
	}
	
	fmmdl_MoveStartSecond( fmmdl );
	fmmdl_MoveEnd( fmmdl );
}

//�g���[�i�[�y�A����A�e���ړ����ł��q���A�g���r���[�g�n�ňړ��s�̏ꍇ
//���W�̂��ꂪ�������鎖������B
//�g���[�i�[�y�A����̎q�͐e��M�p���A�A�g���r���[�g�n�ŃG���[�������Ă�
//�e���ړ����ł���Έړ�������悤�ɂ���
#if 0
void FLDMMDL_Move( FLDMMDL * fmmdl )
{
	if( FLDMMDL_FieldOBJSysStatusBitCheck(fmmdl,FLDMMDLSYS_STABIT_MOVE_PROC_STOP) ){
		return;
	}
	
	fmmdl_MoveBeforeHeight( fmmdl );
	fmmdl_MoveBeforeAttr( fmmdl );
	fmmdl_MoveStartFirst( fmmdl );
	
	if( FLDMMDL_StatusBit_Check(fmmdl,FLDMMDL_STABIT_ACMD) ){
		FLDMMDL_AcmdAction( fmmdl );
	}else if( FLDMMDL_MovePauseCheck(fmmdl) == FALSE ){
		if( FLDMMDL_StatusBitCheck_Move(fmmdl) == TRUE ||
			FLDMMDL_StatusBit_Check(fmmdl,STA_BIT_MOVE_ERROR) == 0 ){
			if( FLDMMDL_MoveSub(fmmdl) == FALSE ){
				FLDMMDL_MoveProcCall( fmmdl );
			}
		}
	}
	
	fmmdl_MoveStartSecond( fmmdl );
	fmmdl_MoveEnd( fmmdl );
}
#endif

//--------------------------------------------------------------
/**
 * ����\���ǂ����`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @retval	int		TRUE=�\,FALSE=�s��
 */
//--------------------------------------------------------------
static int fmmdl_MoveStartCheck( const FLDMMDL * fmmdl )
{
	if( FLDMMDL_StatusBitCheck_Move(fmmdl) == TRUE ){
		return( TRUE );
	}
	
	if( FLDMMDL_StatusBit_Check(fmmdl,STA_BIT_MOVE_ERROR) == 0 ){
		return( TRUE );
	}else if( FLDMMDL_MoveCodeGet(fmmdl) == MV_TR_PAIR ){ //�e�̍s���ɏ]��
		return( TRUE );
	}
	
#if 0
	{	//�ړ��֎~�t���O���E�`�F�b�N
		u32 st = FLDMMDL_StatusBit_Get( fmmdl );
		
		//�����擾���Ȃ��ꍇ
		if( (st&FLDMMDL_STABIT_HEIGHT_GET_ERROR) &&
			(st&FLDMMDL_STABIT_HEIGHT_GET_OFF) == 0 ){
			return( FALSE );
		}
		
		//�A�g���r���[�g�擾���Ȃ��ꍇ
		if( (st&FLDMMDL_STABIT_ATTR_GET_ERROR) &&	
			FLDMMDL_MoveBitCheck_AttrGetOFF(fmmdl) == FALSE ){
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
static void fmmdl_MoveBeforeHeight( FLDMMDL * fmmdl )
{
	if( FLDMMDL_StatusBit_Check(fmmdl,FLDMMDL_STABIT_HEIGHT_GET_ERROR) ){
		FLDMMDL_VecPosNowHeightGetSet( fmmdl );
	}
}

//--------------------------------------------------------------
/**
 * �ړ��J�n���ɍs���A�g���r���[�g�擾
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_MoveBeforeAttr( FLDMMDL * fmmdl )
{
	if( FLDMMDL_StatusBit_Check(fmmdl,FLDMMDL_STABIT_ATTR_GET_ERROR) ){
		if( FLDMMDL_NowPosMapAttrSet(fmmdl) == TRUE ){
			FLDMMDL_StatusBitON_MoveStart( fmmdl );
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
static void fmmdl_MoveStartFirst( FLDMMDL * fmmdl )
{
	if( FLDMMDL_StatusBit_Check(fmmdl,FLDMMDL_STABIT_MOVE_START) ){
		fmmdl_MapAttrProc_MoveStartFirst( fmmdl );
	}
	
	FLDMMDL_StatusBit_OFF( fmmdl,
		FLDMMDL_STABIT_MOVE_START | FLDMMDL_STABIT_JUMP_START );
}

//--------------------------------------------------------------
/**
 * �ړ��J�n�Ŕ����@2nd
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_MoveStartSecond( FLDMMDL * fmmdl )
{
	if( FLDMMDL_StatusBit_Check(fmmdl,FLDMMDL_STABIT_JUMP_START) ){
		fmmdl_MapAttrProc_MoveStartJumpSecond( fmmdl );
	}else if( FLDMMDL_StatusBit_Check(fmmdl,FLDMMDL_STABIT_MOVE_START) ){
		fmmdl_MapAttrProc_MoveStartSecond( fmmdl );
	}
	
	FLDMMDL_StatusBit_OFF( fmmdl,
		FLDMMDL_STABIT_MOVE_START | FLDMMDL_STABIT_JUMP_START );
}

//--------------------------------------------------------------
/**
 * ����I���Ŕ���
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_MoveEnd( FLDMMDL * fmmdl )
{
	if( FLDMMDL_StatusBit_Check(fmmdl,FLDMMDL_STABIT_JUMP_END) ){
		fmmdl_MapAttrProc_MoveEndJump( fmmdl );
	}else if( FLDMMDL_StatusBit_Check(fmmdl,FLDMMDL_STABIT_MOVE_END) ){
		fmmdl_MapAttrProc_MoveEnd( fmmdl );
	}
	
	FLDMMDL_StatusBit_OFF( fmmdl,
		FLDMMDL_STABIT_MOVE_END | FLDMMDL_STABIT_JUMP_END );
}

//==============================================================================
//	�}�b�v�A�g���r���[�g
//==============================================================================
//--------------------------------------------------------------
/**
 * ����J�n�Ŕ�������A�g���r���[�g���� 1st
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_MapAttrProc_MoveStartFirst( FLDMMDL * fmmdl )
{
	FLDMMDL_NowPosMapAttrSet( fmmdl );
	
	if( FLDMMDL_StatusBit_DrawInitCompCheck(fmmdl) == TRUE ){
		MATR now = FLDMMDL_NowMapAttrGet( fmmdl );
		MATR old = FLDMMDL_OldMapAttrGet( fmmdl );
		const OBJCODE_STATE *state = FLDMMDL_OBJCodeDrawStateGet( fmmdl );
		fmmdl_MapAttrBridgeProc_01( fmmdl, now, old, state );
		fmmdl_MapAttrGrassProc_0( fmmdl, now, old, state );
		fmmdl_MapAttrSplashProc_012( fmmdl, now, old, state );
		fmmdl_MapAttrShadowProc_0( fmmdl, now, old, state );
		fmmdl_MapAttrHeight_02( fmmdl, now, old, state );
		fmmdl_MapAttrLGrassProc_0( fmmdl, now, old, state );
		fmmdl_MapAttrNGrassProc_0( fmmdl, now, old, state );
		fmmdl_MapAttrReflect_01( fmmdl, now, old, state );
	}
}

//--------------------------------------------------------------
/**
 * ����J�n�Ŕ�������A�g���r���[�g���� 2nd
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_MapAttrProc_MoveStartSecond( FLDMMDL * fmmdl )
{
	FLDMMDL_NowPosMapAttrSet( fmmdl );
	
	if( FLDMMDL_StatusBit_DrawInitCompCheck(fmmdl) == TRUE ){
		MATR now = FLDMMDL_NowMapAttrGet( fmmdl );
		MATR old = FLDMMDL_OldMapAttrGet( fmmdl );
		const OBJCODE_STATE *state = FLDMMDL_OBJCodeDrawStateGet( fmmdl );
		
		fmmdl_MapAttrBridgeProc_01( fmmdl, now, old, state );
		fmmdl_MapAttrGrassProc_12( fmmdl, now, old, state );
		fmmdl_MapAttrFootMarkProc_1( fmmdl, now, old, state );
		fmmdl_MapAttrSplashProc_012( fmmdl, now, old, state );
		fmmdl_MapAttrShadowProc_1( fmmdl, now, old, state );
		fmmdl_MapAttrLGrassProc_1( fmmdl, now, old, state );
		fmmdl_MapAttrNGrassProc_1( fmmdl, now, old, state );
		fmmdl_MapAttrPoolProc_1( fmmdl, now, old, state );
		fmmdl_MapAttrSwampProc_1( fmmdl, now, old, state );
		fmmdl_MapAttrReflect_01( fmmdl, now, old, state );
		
		fmmdl_MapAttrSEProc_1( fmmdl, now, old, state );				//�`��֌W�Ȃ�?
	}
}

//--------------------------------------------------------------
/**
 * ����J�n�Ŕ�������A�g���r���[�g���� 2nd Jump
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_MapAttrProc_MoveStartJumpSecond( FLDMMDL * fmmdl )
{
	FLDMMDL_NowPosMapAttrSet( fmmdl );
	
	if( FLDMMDL_StatusBit_DrawInitCompCheck(fmmdl) == TRUE ){
		MATR now = FLDMMDL_NowMapAttrGet( fmmdl );
		MATR old = FLDMMDL_OldMapAttrGet( fmmdl );
		const OBJCODE_STATE *state = FLDMMDL_OBJCodeDrawStateGet( fmmdl );
		
		fmmdl_MapAttrBridgeProc_01( fmmdl, now, old, state );
		fmmdl_MapAttrShadowProc_1( fmmdl, now, old, state );
		fmmdl_MapAttrReflect_01( fmmdl, now, old, state );
		fmmdl_MapAttrSplashProc_Jump1( fmmdl, now, old, state );
		fmmdl_MapAttrSEProc_1( fmmdl, now, old, state );				//�`��֌W�Ȃ�?
	}
}

//--------------------------------------------------------------
/**
 * ����I���Ŕ�������A�g���r���[�g�����@end
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_MapAttrProc_MoveEnd( FLDMMDL * fmmdl )
{
	FLDMMDL_NowPosMapAttrSet( fmmdl );
	
	if( FLDMMDL_StatusBit_DrawInitCompCheck(fmmdl) == TRUE ){
		MATR now = FLDMMDL_NowMapAttrGet( fmmdl );
		MATR old = FLDMMDL_OldMapAttrGet( fmmdl );
		const OBJCODE_STATE *state = FLDMMDL_OBJCodeDrawStateGet( fmmdl );
		
		//�I���@�A�g���r���[�g����
		fmmdl_MapAttrHeight_02( fmmdl, now, old, state );
		fmmdl_MapAttrPoolProc_2( fmmdl, now, old, state );
		fmmdl_MapAttrSwampProc_2( fmmdl, now, old, state );
		fmmdl_MapAttrSplashProc_012( fmmdl, now, old, state );
		fmmdl_MapAttrReflect_2( fmmdl, now, old, state );
		fmmdl_MapAttrShadowProc_2( fmmdl, now, old, state );
	}
}

//--------------------------------------------------------------
/**
 * ����I���Ŕ�������A�g���r���[�g�����@end jump
 * @param	fmmdl	FLDMMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_MapAttrProc_MoveEndJump( FLDMMDL * fmmdl )
{
	FLDMMDL_NowPosMapAttrSet( fmmdl );
	
	if( FLDMMDL_StatusBit_DrawInitCompCheck(fmmdl) == TRUE ){
		MATR now = FLDMMDL_NowMapAttrGet( fmmdl );
		MATR old = FLDMMDL_OldMapAttrGet( fmmdl );
		const OBJCODE_STATE *state = FLDMMDL_OBJCodeDrawStateGet( fmmdl );
		
		//�I���@�A�g���r���[�g����
		fmmdl_MapAttrHeight_02( fmmdl, now, old, state );
		fmmdl_MapAttrPoolProc_2( fmmdl, now, old, state );
		fmmdl_MapAttrSwampProc_2( fmmdl, now, old, state );
		fmmdl_MapAttrSplashProc_012( fmmdl, now, old, state );
		fmmdl_MapAttrReflect_2( fmmdl, now, old, state );
		fmmdl_MapAttrShadowProc_2( fmmdl, now, old, state );
		fmmdl_MapAttrGrassProc_12( fmmdl, now, old, state );
		fmmdl_MapAttrGroundSmokeProc_2( fmmdl, now, old, state );
	}
}

//==============================================================================
//	�A�g���r���[�g�ύ���
//==============================================================================
//--------------------------------------------------------------
/**
 * �A�g���r���[�g�ύ����@����J�n�A����I��
 * @param	fmmdl		FLDMMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	state		OBJCODE_STATE
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_MapAttrHeight_02(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state )
{
#if 0
	if( FLDMMDL_StatusBitCheck_AttrOffsOFF(fmmdl) == FALSE ){
		if( MATR_IsSwampDeep(now) == TRUE || MATR_IsSwampGrassDeep(now) == TRUE ){
			VecFx32 vec = { 0, ATTROFFS_Y_NUMA_DEEP, 0 };
			FLDMMDL_VecAttrOffsSet( fmmdl, &vec );
			return;
		}
		
		if( MATR_IsSwamp(now) == TRUE || MATR_IsSwampGrass(now) == TRUE ){
			VecFx32 vec = { 0, ATTROFFS_Y_NUMA, 0 };
			FLDMMDL_VecAttrOffsSet( fmmdl, &vec );
			return;
		}
		
		if( MATR_IsSnowDeepMost(now) == TRUE ){
			VecFx32 vec = { 0, ATTROFFS_Y_YUKI_DEEP_MOST, 0 };
			FLDMMDL_VecAttrOffsSet( fmmdl, &vec );
			return;
		}
		
		if( MATR_IsSnowDeep(now) == TRUE ){
			VecFx32 vec = { 0, ATTROFFS_Y_YUKI_DEEP, 0 };
			FLDMMDL_VecAttrOffsSet( fmmdl, &vec );
			return;
		}
		
		if( MATR_IsShallowSnow(now) == TRUE ){
			VecFx32 vec = { 0, ATTROFFS_Y_YUKI, 0 };
			FLDMMDL_VecAttrOffsSet( fmmdl, &vec );
			return;
		}
	}
	
	{
		VecFx32 vec = { 0, 0, 0 };
		FLDMMDL_VecAttrOffsSet( fmmdl, &vec );
	}
#endif
}

//==============================================================================
//	�A�g���r���[�g�@��
//==============================================================================
//--------------------------------------------------------------
/**
 * ���@����J�n 0
 * @param	fmmdl		FLDMMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	state		OBJCODE_STATE
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_MapAttrGrassProc_0(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state )
{
#if 0
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
 * @param	state		OBJCODE_STATE
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_MapAttrGrassProc_12(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state )
{
#if 0
	if( MATR_IsGrass(now) == TRUE ){
		FE_fmmdlGrass_Add( fmmdl, TRUE );
	}
#endif
}

//==============================================================================
//	�A�g���r���[�g�@����
//==============================================================================
//--------------------------------------------------------------
/**
 * ���Ձ@����J�n 1
 * @param	fmmdl	FLDMMDL *
 * @param	now		���݂̃A�g���r���[�g
 * @param	old		�ߋ��̃A�g���r���[�g
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_MapAttrFootMarkProc_1(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state )
{
#if 0
	if( state->type_footmark == FLDMMDL_FOOTMARK_NON ){
		return;
	}
	
	if( MATR_IsShadowOnSnow(old) == TRUE ){
		if( state->type_footmark == FLDMMDL_FOOTMARK_NORMAL ){
			FE_fmmdlFootMarkShadowSnow_Add( fmmdl );
		}else if( state->type_footmark == FLDMMDL_FOOTMARK_CYCLE ){
			FE_fmmdlFootMarkShadowSnowCycle_Add( fmmdl );
		}
	}
	
	if(	FLDMMDL_MapAttrKindCheck_Sand(fmmdl,old) == TRUE ){
		if( state->type_footmark == FLDMMDL_FOOTMARK_NORMAL ){
			FE_fmmdlFootMarkNormal_Add( fmmdl );
		}else if( state->type_footmark == FLDMMDL_FOOTMARK_CYCLE ){
			FE_fmmdlFootMarkCycle_Add( fmmdl );
		}
		return;
	}
	
	if( MATR_IsSnowDeep(old) == TRUE || MATR_IsSnowDeepMost(old) == TRUE ||
		MATR_IsShallowSnow(old) ){
		FE_fmmdlFootMarkSnowDeep_Add( fmmdl );
		return;
	}
	
	if( FLDMMDL_MapAttrKindCheck_MostShallowSnow(fmmdl,old) == TRUE ){
		FE_fmmdlFootMarkSnow_Add( fmmdl );
		return;
	}
#endif
}

//==============================================================================
//	�A�g���r���[�g�@����
//==============================================================================
//--------------------------------------------------------------
/**
 * ���򖗁@����J�n�I�� 012
 * @param	fmmdl	FLDMMDL *
 * @param	now		���݂̃A�g���r���[�g
 * @param	old		�ߋ��̃A�g���r���[�g
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_MapAttrSplashProc_012(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state )
{
#if 0
	if( MATR_IsShoal(now) == TRUE ){
		if( FLDMMDL_StatusBitCheck_ShoalEffectSet(fmmdl) == FALSE ){
			FE_fmmdlSplash_Add( fmmdl, TRUE );
			FLDMMDL_StatusBitSet_ShoalEffectSet( fmmdl, TRUE );
		}
	}else{
		FLDMMDL_StatusBitSet_ShoalEffectSet( fmmdl, FALSE );
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
static void fmmdl_MapAttrSplashProc_Jump1(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state )
{
	FLDMMDL_StatusBitSet_ShoalEffectSet( fmmdl, FALSE );
}

//==============================================================================
///	�A�g���r���[�g�@�e
//==============================================================================
//--------------------------------------------------------------
/**
 * �e�@����J�n 0
 * @param	fmmdl	FLDMMDL *
 * @param	now		���݂̃A�g���r���[�g
 * @param	old		�ߋ��̃A�g���r���[�g
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_MapAttrShadowProc_0(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state )
{
	fmmdl_MapAttrShadowProc_1( fmmdl, now, old, state );
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
static void fmmdl_MapAttrShadowProc_1(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state )
{
#if 0
	{
		const FLDMMDLSYS *fos = FLDMMDL_FieldOBJSysGet( fmmdl );
		
		if( FLDMMDLSYS_ShadowJoinCheck(fos) == FALSE ){
			return;
		}
	}
	
	if( state->type_shadow == FLDMMDL_SHADOW_NON ){
		return;
	}

	if( MATR_IsGrass(now) == TRUE ||
		MATR_IsLongGrass(now) == TRUE ||
		FLDMMDL_MapAttrKindCheck_Water(fmmdl,now) == TRUE ||
		MATR_IsPoolCheck(now) == TRUE ||
		MATR_IsShoal(now) == TRUE ||
		FLDMMDL_MapAttrKindCheck_Snow(fmmdl,now) == TRUE ||
		MATR_IsSwamp(now) == TRUE ||
		MATR_IsSwampGrass(now) == TRUE ||
		MATR_IsMirrorReflect(now) ){
		FLDMMDL_StatusBit_ON( fmmdl, FLDMMDL_STABIT_SHADOW_VANISH );
	}else{
		if( FLDMMDL_StatusBit_Check(fmmdl,FLDMMDL_STABIT_SHADOW_SET) == 0 ){
			FE_fmmdlShadow_Add( fmmdl );
			FLDMMDL_StatusBit_ON( fmmdl, FLDMMDL_STABIT_SHADOW_SET );
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
static void fmmdl_MapAttrShadowProc_2(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state )
{
#if 0
	{
		const FLDMMDLSYS *fos = FLDMMDL_FieldOBJSysGet( fmmdl );
		
		if( FLDMMDLSYS_ShadowJoinCheck(fos) == FALSE ){
			return;
		}
	}
	
	if( state->type_shadow == FLDMMDL_SHADOW_NON ){
		return;
	}
	
	if( MATR_IsGrass(now) == TRUE ||
		MATR_IsLongGrass(now) == TRUE ||
		FLDMMDL_MapAttrKindCheck_Water(fmmdl,now) == TRUE ||
		MATR_IsPoolCheck(now) == TRUE ||
		MATR_IsShoal(now) == TRUE ||
		FLDMMDL_MapAttrKindCheck_Snow(fmmdl,now) == TRUE ||
		MATR_IsSwamp(now) == TRUE ||
		MATR_IsSwampGrass(now) == TRUE ||
		MATR_IsMirrorReflect(now) ){
		FLDMMDL_StatusBit_ON( fmmdl, FLDMMDL_STABIT_SHADOW_VANISH );
	}else{
		FLDMMDL_StatusBit_OFF( fmmdl, FLDMMDL_STABIT_SHADOW_VANISH );
	}
#endif
}

//==============================================================================
//	�A�g���r���[�g�@�y��
//==============================================================================
//--------------------------------------------------------------
/**
 * �y���@����I�� 2 
 * @param	fmmdl	FLDMMDL *
 * @param	now		���݂̃A�g���r���[�g
 * @param	old		�ߋ��̃A�g���r���[�g
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_MapAttrGroundSmokeProc_2(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state )
{
#if 0
	if( FLDMMDL_MapAttrKindCheck_Water(fmmdl,now) == TRUE ||
		MATR_IsShoal(now) == TRUE ||
		MATR_IsIce(now) == TRUE ||
		MATR_IsSwamp(now) == TRUE ||
		MATR_IsSwampGrass(now) == TRUE ||
		FLDMMDL_MapAttrKindCheck_Snow(fmmdl,now) == TRUE ){
		return;
	}
	
	FE_fmmdlKemuri_Add( fmmdl );
#endif
}

//==============================================================================
//	�A�g���r���[�g�@������
//==============================================================================
//--------------------------------------------------------------
/**
 * �������@����J�n 0
 * @param	fmmdl		FLDMMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	state		OBJCODE_STATE
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_MapAttrLGrassProc_0(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state )
{
#if 0
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
 * @param	state		OBJCODE_STATE
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_MapAttrLGrassProc_1(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state )
{
#if 0
	if( MATR_IsLongGrass(now) == TRUE ){
		FE_fmmdlLGrass_Add( fmmdl, TRUE );
	}
#endif
}

//==============================================================================
//	�A�g���r���[�g�@����
//==============================================================================
//--------------------------------------------------------------
/**
 * �����@����J�n 0
 * @param	fmmdl		FLDMMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	state		OBJCODE_STATE
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_MapAttrNGrassProc_0(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state )
{
#if 0
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
 * @param	state		OBJCODE_STATE
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_MapAttrNGrassProc_1(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state )
{
#if 0
	if( MATR_IsSwampGrass(now) == TRUE ){
		FE_fmmdlNGrass_Add( fmmdl, TRUE );
	}
#endif
}

//==============================================================================
//	�A�g���r���[�g�@�����܂�
//==============================================================================
//--------------------------------------------------------------
/**
 * �����܂�@����J�n 1
 * @param	fmmdl	FLDMMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	state		OBJCODE_STATE
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_MapAttrPoolProc_1(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state )
{
#if 0
	if( MATR_IsPoolCheck(old) == TRUE ){
		FE_fmmdlRippleSet( fmmdl,
			FLDMMDL_OldPosGX_Get(fmmdl),
			FLDMMDL_OldPosGY_Get(fmmdl), 
			FLDMMDL_OldPosGZ_Get(fmmdl) );
	}
#endif
}

//--------------------------------------------------------------
/**
 * �����܂�@����I�� 2
 * @param	fmmdl	FLDMMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	state		OBJCODE_STATE
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_MapAttrPoolProc_2(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state )
{
#if 0
	if( MATR_IsPoolCheck(now) == TRUE ){
		FE_fmmdlRippleSet( fmmdl,
			FLDMMDL_NowPosGX_Get(fmmdl),
			FLDMMDL_NowPosGY_Get(fmmdl), 
			FLDMMDL_NowPosGZ_Get(fmmdl) );
	}
#endif
}

//==============================================================================
//	�A�g���r���[�g�@��
//==============================================================================
//--------------------------------------------------------------
/**
 * ���@����J�n 1
 * @param	fmmdl	FLDMMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	state		OBJCODE_STATE
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_MapAttrSwampProc_1(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state )
{
#if 0
	if( MATR_IsSwamp(old) == TRUE ){
		FE_fmmdlNRippleSet( fmmdl,
			FLDMMDL_OldPosGX_Get(fmmdl),
			FLDMMDL_OldPosGY_Get(fmmdl), 
			FLDMMDL_OldPosGZ_Get(fmmdl) );
	}
#endif
}

//--------------------------------------------------------------
/**
 * ���@����I�� 2
 * @param	fmmdl	FLDMMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	state		OBJCODE_STATE
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_MapAttrSwampProc_2(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state )
{
#if 0
	if( MATR_IsSwamp(now) == TRUE ){
		FE_fmmdlNRippleSet( fmmdl,
			FLDMMDL_NowPosGX_Get(fmmdl),
			FLDMMDL_NowPosGY_Get(fmmdl), 
			FLDMMDL_NowPosGZ_Get(fmmdl) );
	}
#endif
}

//==============================================================================
//	�A�g���r���[�g�@�f�肱��
//==============================================================================
//--------------------------------------------------------------
/**
 * �f�肱�݁@����J�n 01
 * @param	fmmdl	FLDMMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	state		OBJCODE_STATE
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_MapAttrReflect_01(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state )
{
#if 0
	if( state->type_reflect == FLDMMDL_REFLECT_NON ){
		return;
	}
	
	{
		if( FLDMMDL_StatusBitCheck_Reflect(fmmdl) == FALSE ){
			MATR hit = MATR_IsNotAttrGet();
			
			if( MATR_IsReflect(now) == TRUE ){
				hit = now;
			}else{
				MATR next = FLDMMDL_NextDirAttrGet( fmmdl, DIR_DOWN );
				
				if( MATR_IsReflect(next) == TRUE ){
					hit = next;
				}
			}
			
			if( hit != MATR_IsNotAttrGet() ){	//�f�荞�݃q�b�g
				REFTYPE type;
				FLDMMDL_StatusBitSet_Reflect( fmmdl, TRUE );
				
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

#if 0
static void fmmdl_MapAttrReflect_01(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state )
{
	if( state->type_reflect == FLDMMDL_REFLECT_NON ){
		return;
	}
	
	{
		MATR attr = FLDMMDL_NextDirAttrGet( fmmdl, DIR_DOWN );
		
		if( FLDMMDL_StatusBitCheck_Reflect(fmmdl) == FALSE ){
			if( MATR_IsReflect(attr) == TRUE || MATR_IsReflect(now) == TRUE ){
				REFTYPE type;
				FLDMMDL_StatusBitSet_Reflect( fmmdl, TRUE );
				
				if( MATR_IsMirrorReflect(attr) == TRUE ){ 	//����
					type = REFTYPE_MIRROR;
				}else if( MATR_IsPoolCheck(attr) == TRUE ){	//������
					type = REFTYPE_POOL;
				}else{										//�r
					type = REFTYPE_POND;
				}
				
				FE_fmmdlReflect_Add( fmmdl, type );
			}
		}
	}
}
#endif

//--------------------------------------------------------------
/**
 * �f�肱�݁@����I�� 2
 * @param	fmmdl	FLDMMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	state		OBJCODE_STATE
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_MapAttrReflect_2(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state )
{
#if 0
	if( state->type_reflect == FLDMMDL_REFLECT_NON ||
		FLDMMDL_StatusBitCheck_Reflect(fmmdl) == FALSE ){
		return;
	}
	
	{
		MATR attr = FLDMMDL_NextDirAttrGet( fmmdl, DIR_DOWN );
		
		if( MATR_IsReflect(attr) == FALSE ){
			FLDMMDL_StatusBitSet_Reflect( fmmdl, FALSE );
		}
	}
#endif
}

//==============================================================================
//	�A�g���r���[�g�@��
//==============================================================================
//--------------------------------------------------------------
/**
 * ���@����J�n 0
 * @param	fmmdl		FLDMMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	state		OBJCODE_STATE
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_MapAttrBridgeProc_01(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state )
{
#if 0
	if( MATR_IsBridgeFlag(now) == TRUE ){
		FLDMMDL_StatusBitSet_Bridge( fmmdl, TRUE );
	}else if( FLDMMDL_StatusBitCheck_Bridge(fmmdl) == TRUE ){
		if( MATR_IsBridge(now) == FALSE ){
			FLDMMDL_StatusBitSet_Bridge( fmmdl, FALSE );
		}
	}
#endif
}

//==============================================================================
//	�A�g���r���[�g�֘ASE
//==============================================================================
//--------------------------------------------------------------
/**
 * SE�@����J�n 1
 * @param	fmmdl	FLDMMDL *
 * @param	now		���݂̃A�g���r���[�g
 * @param	old		�ߋ��̃A�g���r���[�g
 * @param	state		OBJCODE_STATE
 * @retval	nothing
 */
//--------------------------------------------------------------
static void fmmdl_MapAttrSEProc_1(
		FLDMMDL * fmmdl, MATR now, MATR old, const OBJCODE_STATE *state )
{
#if 0
	if( MATR_IsSnow(now) ){
		Snd_SePlay( SEQ_SE_DP_YUKIASHI );
	}
#endif
}

//==============================================================================
//	���W�`�F�b�N
//==============================================================================
//--------------------------------------------------------------
/**
 * �w����W��Q���`�F�b�N
 * @param	fos		FLDMMDLSYS *
 * @param	x		�`�F�b�N����X���W	�O���b�h
 * @param	z		�`�F�b�N����Z���W	�O���b�h
 * @retval	u32		�q�b�g�r�b�g�BFLDMMDL_MOVE_HIT_BIT_LIM��
 */
//--------------------------------------------------------------
u32 FLDMMDL_PosHitCheck( const FLDMMDLSYS *fos, int x, int z )
{
	u32 ret;
	
	ret = FLDMMDL_MOVE_HIT_BIT_NON;
	
#if 0	
	if(	GetHitAttr(FLDMMDLSYS_FieldSysWorkGet(fos),x,z) == TRUE ){
		ret |= FLDMMDL_MOVE_HIT_BIT_ATTR;
	}
#endif
	
	if( FLDMMDL_SearchGridPos(fos,x,z,FALSE) != NULL ){
		ret |= FLDMMDL_MOVE_HIT_BIT_OBJ;
	}
	
	return( ret );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���ړ��`�F�b�N
 * @param	fmmdl	FLDMMDL *
 * @param	vec		���ݎ����W
 * @param	x		�ړ���X���W	�O���b�h
 * @param	y		�ړ���Y���W
 * @param	z		�ړ���Z���W	�O���b�h
 * @param	dir		�ړ����� DIR_UP��
 * @retval	u32		�q�b�g�r�b�g�BFLDMMDL_MOVE_HIT_BIT_LIM��
 */
//--------------------------------------------------------------
u32 FLDMMDL_MoveHitCheck(
		const FLDMMDL * fmmdl, const VecFx32 *vec, int x, int y, int z, int dir )
{
	u32 ret;
	
	ret = FLDMMDL_MOVE_HIT_BIT_NON;
	
	if( FLDMMDL_MoveHitCheckLimit(fmmdl,x,y,z) == TRUE ){
		ret |= FLDMMDL_MOVE_HIT_BIT_LIM;
	}
	
#if 0
	{
		s8 flag;
		FIELDSYS_WORK *fsys = FLDMMDL_FieldSysWorkGet( fmmdl );
		if( MPTL_CheckHitWall(fsys,vec,x,z,&flag) == TRUE ){
			ret |= FLDMMDL_MOVE_HIT_BIT_ATTR;
			
			if( flag != HIT_RES_EQUAL ){
				ret |= FLDMMDL_MOVE_HIT_BIT_HEIGHT;
			}
		}
	}
	
	if( FLDMMDL_MoveHitCheckAttr(fmmdl,x,z,dir) == TRUE ){
		ret |= FLDMMDL_MOVE_HIT_BIT_ATTR;
	}
	
	if( FLDMMDL_MoveHitCheckFellow(fmmdl,x,y,z) == TRUE ){
		ret |= FLDMMDL_MOVE_HIT_BIT_OBJ;
	}
#else
	{
		u32 attr;
		fx32 height;
		VecFx32 pos;
		
		pos.x = GRID_SIZE_FX32( x ) + GRID_HALF_FX32;
		pos.y = GRID_SIZE_FX32( y );
		pos.z = GRID_SIZE_FX32( z ) + GRID_HALF_FX32;
		
		if( FLDMMDL_GetMapAttr(fmmdl,&pos,&attr) == TRUE ){
			if( attr != 0 ){
				ret |= FLDMMDL_MOVE_HIT_BIT_ATTR;
			}
		}else{
			ret |= FLDMMDL_MOVE_HIT_BIT_ATTR;
		}
		
		if( FLDMMDL_GetMapHeight(fmmdl,&pos,&height) == TRUE ){
			fx32 diff = vec->y - height;
			if( diff < 0 ){ diff = -diff; }
			if( diff >= (FX32_ONE*20) ){
				ret |= FLDMMDL_MOVE_HIT_BIT_HEIGHT;
			}
		}else{
			ret |= FLDMMDL_MOVE_HIT_BIT_HEIGHT;
		}
	}
	
	if( FLDMMDL_MoveHitCheckFellow(fmmdl,x,y,z) == TRUE ){
		ret |= FLDMMDL_MOVE_HIT_BIT_OBJ;
	}
	
	{
		VecFx32 pos;
		const FLDMAPPER *pG3DMapper;

		pG3DMapper = FLDMMDLSYS_GetG3DMapper( FLDMMDL_GetFldMMdlSys(fmmdl) );
		pos.x = GRID_SIZE_FX32( x ) + GRID_HALF_FX32;
		pos.y = GRID_SIZE_FX32( y );
		pos.z = GRID_SIZE_FX32( z ) + GRID_HALF_FX32;
		
		if( FLDMAPPER_CheckOutRange(pG3DMapper,&pos) == TRUE ){
			ret |= FLDMMDL_MOVE_HIT_BIT_OUTRANGE;
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
 * @retval	u32		�q�b�g�r�b�g�BFLDMMDL_MOVE_HIT_BIT_LIM��
 */
//--------------------------------------------------------------
u32 FLDMMDL_MoveHitCheckNow( const FLDMMDL * fmmdl, int x, int y, int z, int dir )
{
	VecFx32 vec;
	
	FLDMMDL_VecPosGet( fmmdl, &vec );
	return( FLDMMDL_MoveHitCheck(fmmdl,&vec,x,y,z,dir) );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���ړ��`�F�b�N�@���݈ʒu+������
 * @param	fmmdl	FLDMMDL * 
 * @param	dir		�ړ���������BDIR_UP��
 * @retval	u32		�q�b�g�r�b�g�BFLDMMDL_MOVE_HIT_BIT_LIM��
 */
//--------------------------------------------------------------
u32 FLDMMDL_MoveHitCheckDir( const FLDMMDL * fmmdl, int dir )
{
	int x,y,z;
	
	x = FLDMMDL_NowPosGX_Get( fmmdl ) + FLDMMDL_DirAddValueGX( dir );
	y = FLDMMDL_NowPosGY_Get( fmmdl );
	z = FLDMMDL_NowPosGZ_Get( fmmdl ) + FLDMMDL_DirAddValueGZ( dir );
	
	return( FLDMMDL_MoveHitCheckNow(fmmdl,x,y,z,dir) );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f�����m�̏Փ˃`�F�b�N
 * @param	fmmdl	FLDMMDL * 
 * @param	x		�ړ���X���W	�O���b�h
 * @param	y		�ړ���X���W	�O���b�h
 * @param	z		�ړ���X���W	�O���b�h
 * @retval	int		TRUE=�Փ˃A��
 */
//--------------------------------------------------------------
int FLDMMDL_MoveHitCheckFellow( const FLDMMDL * fmmdl, int x, int y, int z )
{
	int max,hx,hz;
	const FLDMMDLSYS *sys;
	const FLDMMDL * fieldobj;
	
	sys = FLDMMDL_FieldOBJSysGet( fmmdl );
	fieldobj = FLDMMDLSYS_FieldOBJWorkGet( sys );
	max = FLDMMDLSYS_OBJMaxGet( sys );
	
	do{
		if( fieldobj != fmmdl ){
			if( FLDMMDL_StatusBit_Check(fieldobj,FLDMMDL_STABIT_USE) ){
				if( FLDMMDL_StatusBit_Check(fieldobj,FLDMMDL_STABIT_FELLOW_HIT_NON) == 0 ){
					hx = FLDMMDL_NowPosGX_Get( fieldobj );
					hz = FLDMMDL_NowPosGZ_Get( fieldobj );
					
					if( hx == x && hz == z ){
						int hy = FLDMMDL_NowPosGY_Get( fieldobj );
						int sy = hy - y;
						if( sy < 0 ){ sy = -sy; }
						if( sy < H_GRID_FELLOW_SIZE ){
							return( TRUE );
						}
					}
			
					hx = FLDMMDL_OldPosGX_Get( fieldobj );
					hz = FLDMMDL_OldPosGZ_Get( fieldobj );
			
					if( hx == x && hz == z ){
						int hy = FLDMMDL_NowPosGY_Get( fieldobj );
						int sy = hy - y;
						if( sy < 0 ){ sy = -sy; }
						if( sy < H_GRID_FELLOW_SIZE ){
							return( TRUE );
						}
					}
				}
			}
		}
		
		FLDMMDLSYS_FieldOBJWorkInc( &fieldobj );
		max--;
	}while( max );
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���ړ������`�F�b�N
 * @param	fmmdl	FLDMMDL * 
 * @param	x		�ړ���X���W	�O���b�h
 * @param	y		�ړ���Y���W	�O���b�h
 * @param	z		�ړ���Z���W	�O���b�h
 * @retval	int		TRUE=�����z��
 */
//--------------------------------------------------------------
int FLDMMDL_MoveHitCheckLimit( const FLDMMDL * fmmdl, int x, int y, int z )
{
	int init,limit,min,max;
	
	init = FLDMMDL_InitPosGX_Get( fmmdl );
	limit = FLDMMDL_MoveLimitXGet( fmmdl );
	
	if( limit != MOVE_LIMIT_NOT ){
		min = init - limit;
		max = init + limit;
	
		if( min > x || max < x ){
			return( TRUE );
		}
	}
	
	init = FLDMMDL_InitPosGZ_Get( fmmdl );
	limit = FLDMMDL_MoveLimitZGet( fmmdl );
	
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
 * @param	fmmdl	FLDMMDL * 
 * @param	x		�ړ���X���W	�O���b�h
 * @param	z		�ړ���Z���W	�O���b�h
 * @param	dir		�ړ����� DIR_UP��
 * @retval	int		TRUE=�ړ��s�A�g���r���[�g
 */
//--------------------------------------------------------------
int FLDMMDL_MoveHitCheckAttr(
	const FLDMMDL * fmmdl, int x, int z, int dir )
{
#if 0
	if( FLDMMDL_MoveBitCheck_AttrGetOFF(fmmdl) == FALSE ){
		FIELDSYS_WORK *fsys = FLDMMDL_FieldSysWorkGet( fmmdl );
		MATR now_attr = FLDMMDL_NowMapAttrGet( fmmdl );
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
	if( FLDMMDL_MoveBitCheck_AttrGetOFF(fmmdl) == FALSE ){
		u32 attr;
		VecFx32 pos;
		pos.x = GRID_SIZE_FX32( x );
		pos.y = 0;
		pos.z = GRID_SIZE_FX32( z );
		
		if( FLDMMDL_GetMapAttr(fmmdl,&pos,&attr) == TRUE ){
			if( attr == 0 ){
				return( FALSE );
			}
		}
	}
#endif
	return( TRUE );
}

#if 0
int FLDMMDL_MoveHitCheckAttr( const FLDMMDL * fmmdl, int x, int z, int dir )
{
	FIELDSYS_WORK *fsys = FLDMMDL_FieldSysWorkGet( fmmdl );
	MATR now_attr = FLDMMDL_NowMapAttrGet( fmmdl );
	MATR next_attr = GetAttributeLSB( fsys, x, z );
	
	#ifdef PM_DEBUG
	if( dir == DIR_NOT || dir >= DIR_4_MAX ){
		if( dir == DIR_NOT ){
			OS_Printf( "dir == DIR_NOT\n" );
		}else{
			OS_Printf( "dir ERROR dir=%d\n", dir );
		}
		
		OS_Printf( "OBJ ID = 0x%x,OBJ CODE = 0x%x,MOVE CODE = 0x%x,GX = 0x%x, GZ = 0x%x\n",
				FLDMMDL_OBJIDGet(fmmdl),
				FLDMMDL_OBJCodeGet(fmmdl),
				FLDMMDL_MoveCodeGet(fmmdl),
				FLDMMDL_NowPosGX_Get(fmmdl),
				FLDMMDL_NowPosGZ_Get(fmmdl) );
		
		GF_ASSERT( 0 );
	}
	#endif
	
	if( DATA_HitCheckAttr_Now[dir](now_attr) == TRUE ||
		DATA_HitCheckAttr_Next[dir](next_attr) == TRUE ){
		return( TRUE );
	}
	
	return( FALSE );
}
#endif

//--------------------------------------------------------------
///	���݈ʒu�A�g���r���[�g���画�肷��ړ�����A�g���r���[�g�`�F�b�N�֐�
//--------------------------------------------------------------
#if 0
static BOOL (* const DATA_HitCheckAttr_Now[DIR_4_MAX])( MATR attr ) =
{
	MATR_IsBadMoveUpCheck,
	MATR_IsBadMoveDownCheck,
	MATR_IsBadMoveLeftCheck,
	MATR_IsBadMoveRightCheck,
};

//--------------------------------------------------------------
///	�����ʒu�A�g���r���[�g���画�肷��ړ�����A�g���r���[�g�`�F�b�N�֐�
//--------------------------------------------------------------
static BOOL (* const DATA_HitCheckAttr_Next[DIR_4_MAX])( MATR attr ) =
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
 * @retval	int		TRUE=���A�g���r���[�g
 */
//--------------------------------------------------------------
int FLDMMDL_MapAttrKindCheck_Water( FLDMMDL * fmmdl, u32 attr )
{
#if 0
	if( MATR_IsBridgeWater(attr) ){
		if( FLDMMDL_StatusBitCheck_Bridge(fmmdl) == FALSE ){
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
int FLDMMDL_MapAttrKindCheck_Sand( FLDMMDL * fmmdl, u32 attr )
{
#if 0
	if( MATR_IsBridgeSand(attr) ){
		if( FLDMMDL_StatusBitCheck_Bridge(fmmdl) == FALSE ){
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
int FLDMMDL_MapAttrKindCheck_Snow( FLDMMDL * fmmdl, u32 attr )
{
#if 0
	if( MATR_IsBridgeSnowShallow(attr) ){
		if( FLDMMDL_StatusBitCheck_Bridge(fmmdl) == FALSE ){
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
int FLDMMDL_MapAttrKindCheck_MostShallowSnow( FLDMMDL * fmmdl, u32 attr )
{
#if 0
	if( MATR_IsBridgeSnowShallow(attr) ){
		if( FLDMMDL_StatusBitCheck_Bridge(fmmdl) == FALSE ){
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
int FLDMMDL_MapAttrKindCheck_Bridge( FLDMMDL * fmmdl, u32 attr )
{
	if( FLDMMDL_StatusBitCheck_Bridge(fmmdl) == TRUE ){
#if 0
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
int FLDMMDL_MapAttrKindCheck_BridgeV( FLDMMDL * fmmdl, u32 attr )
{
	if( FLDMMDL_StatusBitCheck_Bridge(fmmdl) == TRUE ){
#if 0
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
int FLDMMDL_MapAttrKindCheck_BridgeH( FLDMMDL * fmmdl, u32 attr )
{
	if( FLDMMDL_StatusBitCheck_Bridge(fmmdl) == TRUE ){
#if 0
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
 * @param
 * @retval
 */
//--------------------------------------------------------------
static BOOL fldmmdl_GetMapGridInfo(
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
 * @param
 * @retval
 */
//--------------------------------------------------------------
BOOL FLDMMDL_GetMapAttr(
	const FLDMMDL *fmmdl, const VecFx32 *pos, u32 *attr )
{
	FLDMAPPER_GRIDINFO GridInfo;
	*attr = 0;

	if( fldmmdl_GetMapGridInfo(fmmdl,pos,&GridInfo) == TRUE ){
		*attr = GridInfo.gridData[0].attr;
		return( TRUE );
	}
	
	return( FALSE );
}

//--------------------------------------------------------------
/**
 * �}�b�v�����擾
 * @param
 * @retval
 */
//--------------------------------------------------------------
BOOL FLDMMDL_GetMapHeight(
	const FLDMMDL *fmmdl, const VecFx32 *pos, fx32 *height )
{
	FLDMAPPER_GRIDINFO GridInfo;
	
	*height = 0;
	
	if( fldmmdl_GetMapGridInfo(fmmdl,pos,&GridInfo) == TRUE ){
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

//==============================================================================
//	�O���b�h���W�ړ��n
//==============================================================================
//--------------------------------------------------------------
///	4�����ʍ��W�����@X
//--------------------------------------------------------------
static const int DATA_GPosX_Dir4AddTbl[] =
{ 0, 0, -1, 1 };

//--------------------------------------------------------------
///	4�����ʍ��W�����@Y
//--------------------------------------------------------------
static const int DATA_GPosY_Dir4AddTbl[] =
{ 0, 0, 0, 0 };

//--------------------------------------------------------------
///	4�����ʍ��W�����@Z
//--------------------------------------------------------------
static const int DATA_GPosZ_Dir4AddTbl[] =
{ -1, 1, 0, 0 };

//--------------------------------------------------------------
/**
 * ��������X���������l���擾
 * @param	dir		�����BDIR_UP��
 * @retval	int		�O���b�h�P�ʂł̑����l
 */
//--------------------------------------------------------------
int FLDMMDL_DirAddValueGX( int dir )
{
	return( DATA_GPosX_Dir4AddTbl[dir] );
}

//--------------------------------------------------------------
/**
 * ��������Z���������l���擾
 * @param	dir		�����BDIR_UP��
 * @retval	int		�O���b�h�P�ʂł̑����l
 */
//--------------------------------------------------------------
int FLDMMDL_DirAddValueGZ( int dir )
{
	return( DATA_GPosZ_Dir4AddTbl[dir] );
}

//--------------------------------------------------------------
/**
 * �����Ō��ݍ��W���X�V
 * @param	fmmdl		FLDMMDL * 
 * @param	dir			�ړ�����
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_NowGPosDirAdd( FLDMMDL * fmmdl, int dir )
{
	FLDMMDL_OldPosGX_Set( fmmdl, FLDMMDL_NowPosGX_Get(fmmdl) );
	FLDMMDL_OldPosGY_Set( fmmdl, FLDMMDL_NowPosGY_Get(fmmdl) );
	FLDMMDL_OldPosGZ_Set( fmmdl, FLDMMDL_NowPosGZ_Get(fmmdl) );
	
	FLDMMDL_NowPosGX_Add( fmmdl, FLDMMDL_DirAddValueGX(dir) );
	FLDMMDL_NowPosGY_Add( fmmdl, 0 );
	FLDMMDL_NowPosGZ_Add( fmmdl, FLDMMDL_DirAddValueGZ(dir) );
}

//--------------------------------------------------------------
/**
 * ���ݍ��W�ŉߋ����W���X�V
 * @param	fmmdl		FLDMMDL * 
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_GPosUpdate( FLDMMDL * fmmdl )
{
	FLDMMDL_OldPosGX_Set( fmmdl, FLDMMDL_NowPosGX_Get(fmmdl) );
	FLDMMDL_OldPosGY_Set( fmmdl, FLDMMDL_NowPosGY_Get(fmmdl) );
	FLDMMDL_OldPosGZ_Set( fmmdl, FLDMMDL_NowPosGZ_Get(fmmdl) );
}

//--------------------------------------------------------------
/**
 * ���݂̃O���b�h���W����w�������̃A�g���r���[�g���擾
 * @param	fmmdl	FLDMMDL *
 * @param	dir		DIR_UP��
 * @retval	u32		MATR
 */
//--------------------------------------------------------------
u32 FLDMMDL_NextDirAttrGet( FLDMMDL * fmmdl, int dir )
{
	int gx = FLDMMDL_NowPosGX_Get( fmmdl ) + FLDMMDL_DirAddValueGX( dir );
	int gz = FLDMMDL_NowPosGZ_Get( fmmdl ) + FLDMMDL_DirAddValueGZ( dir );
#if 0
	FIELDSYS_WORK *fsys = FLDMMDL_FieldSysWorkGet( fmmdl );
	MATR attr = GetAttributeLSB( fsys, gx, gz );
#else
	MATR attr = 0;
#endif
	return( attr );
}

//==============================================================================
//	3D���W�n
//==============================================================================
//--------------------------------------------------------------
/**
 * �w��l�����Z����
 * @param	fmmdl		FLDMMDL * 
 * @param	val			�ړ���
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMMDL_VecPosAdd( FLDMMDL * fmmdl, const VecFx32 *val )
{
	VecFx32 vec;
	
	FLDMMDL_VecPosGet( fmmdl, &vec );
	
	vec.x += val->x;
	vec.y += val->y;
	vec.z += val->z;
	
	FLDMMDL_VecPosSet( fmmdl, &vec );
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
void FLDMMDL_VecPosDirAdd( FLDMMDL * fmmdl, int dir, fx32 val )
{
	VecFx32 vec;
	
	FLDMMDL_VecPosGet( fmmdl, &vec );
	
	switch( dir ){
	case DIR_UP:		vec.z -= val;	break;
	case DIR_DOWN:		vec.z += val;	break;
	case DIR_LEFT:		vec.x -= val;	break;
	case DIR_RIGHT:		vec.x += val;	break;
	}
	
	FLDMMDL_VecPosSet( fmmdl, &vec );
}

//--------------------------------------------------------------
/**
 * ���ݍ��W���獂���擾�������W�ɔ��f
 * FLDMMDL_STABIT_HEIGHT_GET_ERROR�̃Z�b�g�������čs��
 * @param	fmmdl		FLDMMDL * 
 * @retval	int		TRUE=��������ꂽ�BFALSE=���Ȃ��BFLDMMDL_STABIT_HEIGHT_GET_ERROR�Ŏ擾�\
 */
//--------------------------------------------------------------
int FLDMMDL_VecPosNowHeightGetSet( FLDMMDL * fmmdl )
{
	VecFx32 vec_pos,vec_pos_h;
	
	FLDMMDL_VecPosGet( fmmdl, &vec_pos );
	vec_pos_h = vec_pos;
	
	if( FLDMMDL_HeightOFFCheck(fmmdl) == TRUE ){
		FLDMMDL_StatusBit_OFF( fmmdl, FLDMMDL_STABIT_HEIGHT_GET_ERROR );
		return( FALSE );
	}
	
	{
#if 0
		int eflag = FLDMMDL_StatusBitCheck_HeightExpand( fmmdl );
		FIELDSYS_WORK *fsys = FLDMMDL_FieldSysWorkGet( fmmdl );
		int ret = FieldOBJTool_GetHeightExpand( fsys, &vec_pos_h, eflag );
		
		if( ret == TRUE ){
			vec_pos.y = vec_pos_h.y;
			FLDMMDL_VecPosSet( fmmdl, &vec_pos );
			FLDMMDL_OldPosGY_Set( fmmdl, FLDMMDL_NowPosGY_Get(fmmdl) );
			FLDMMDL_NowPosGY_Set( fmmdl, SIZE_H_GRID_FX32(vec_pos.y) );
			FLDMMDL_StatusBit_OFF( fmmdl, FLDMMDL_STABIT_HEIGHT_GET_ERROR );
		}else{
			FLDMMDL_StatusBit_ON( fmmdl, FLDMMDL_STABIT_HEIGHT_GET_ERROR );
		}
#else
		fx32 height;
		int ret = FLDMMDL_GetMapHeight( fmmdl, &vec_pos_h, &height );
		
		if( ret == TRUE ){
			vec_pos.y = height;
			FLDMMDL_VecPosSet( fmmdl, &vec_pos );
			FLDMMDL_StatusBit_OFF( fmmdl, FLDMMDL_STABIT_HEIGHT_GET_ERROR );
		}else{
			FLDMMDL_StatusBit_ON( fmmdl, FLDMMDL_STABIT_HEIGHT_GET_ERROR );
		}
#endif
		return( ret );
	}
}

//--------------------------------------------------------------
/**
 * ���ݍ��W�ŃA�g���r���[�g���f
 * FLDMMDL_STABIT_ATTR_GET_ERROR�̃Z�b�g�������čs��
 * @retval	int		TRUE=�擾�����BFALSE=���s�B
 */
//--------------------------------------------------------------
int FLDMMDL_NowPosMapAttrSet( FLDMMDL * fmmdl )
{
#if 0
	MATR old_attr = MATR_IsNotAttrGet();
	MATR now_attr = old_attr;
	
	if( FLDMMDL_MoveBitCheck_AttrGetOFF(fmmdl) == FALSE ){
		int gx = FLDMMDL_OldPosGX_Get( fmmdl );
		int gz = FLDMMDL_OldPosGZ_Get( fmmdl );
		FIELDSYS_WORK *fsys = FLDMMDL_FieldSysWorkGet( fmmdl );
		old_attr = GetAttributeLSB( fsys, gx, gz );
	
		gx = FLDMMDL_NowPosGX_Get( fmmdl );
		gz = FLDMMDL_NowPosGZ_Get( fmmdl );
		now_attr = GetAttributeLSB( fsys, gx, gz );
	}
	
	FLDMMDL_OldMapAttrSet( fmmdl, old_attr );
	FLDMMDL_NowMapAttrSet( fmmdl, now_attr );
	
	if( MATR_IsNotAttr(now_attr) == TRUE ){
		FLDMMDL_StatusBit_ON( fmmdl, FLDMMDL_STABIT_ATTR_GET_ERROR );
		return( FALSE );
	}
	
	FLDMMDL_StatusBit_OFF( fmmdl, FLDMMDL_STABIT_ATTR_GET_ERROR );
#endif
	return( TRUE );
}

#if 0
int FLDMMDL_NowPosMapAttrSet( FLDMMDL * fmmdl )
{
	int gx = FLDMMDL_OldPosGX_Get( fmmdl );
	int gz = FLDMMDL_OldPosGZ_Get( fmmdl );
	FIELDSYS_WORK *fsys = FLDMMDL_FieldSysWorkGet( fmmdl );
	MATR attr = GetAttributeLSB( fsys, gx, gz );
	FLDMMDL_OldMapAttrSet( fmmdl, attr );
	
	gx = FLDMMDL_NowPosGX_Get( fmmdl );
	gz = FLDMMDL_NowPosGZ_Get( fmmdl );
	attr = GetAttributeLSB( fsys, gx, gz );
	FLDMMDL_NowMapAttrSet( fmmdl, attr );
	
#if 0	
	{
		OS_Printf( "�A�g���r���[�g�擾�@ATTR=0x%x, X=0x%x, Z=0x%x, OBJCODE =0x%x�@",attr, gx, gz, FLDMMDL_OBJCodeGet(fmmdl) );
		
		if( MATR_IsNotAttr(attr) == TRUE ){
			OS_Printf( "�A�g���r���[�g�擾�c�ُ�\n" );
		}else{
			OS_Printf( "�A�g���r���[�g�擾�c����\n" );
		}
	}
#endif
	
	if( MATR_IsNotAttr(attr) == TRUE ){
		FLDMMDL_StatusBit_ON( fmmdl, FLDMMDL_STABIT_ATTR_GET_ERROR );
		return( FALSE );
	}
	
	FLDMMDL_StatusBit_OFF( fmmdl, FLDMMDL_STABIT_ATTR_GET_ERROR );
	return( TRUE );
}
#endif

//--------------------------------------------------------------
/**
 * �w��l���w������ɉ��Z����
 * @param	dir			�ړ�4�����BDIR_UP��
 * @param	vec			������Ώ�
 * @param	val			�ړ���(��
 * @retval	nothing
 */
//--------------------------------------------------------------
void FieldOBJTool_VecPosDirAdd( int dir, VecFx32 *vec, fx32 val )
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
void FieldOBJTool_GridCenterPosGet( int gx, int gz, VecFx32 *vec )
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
void FieldOBJTool_VectorGridPosGet( int *gx, int *gy, int *gz, const VecFx32 *vec )
{
	*gx = SIZE_GRID_FX32( vec->x );
	*gy = SIZE_GRID_FX32( vec->y );
	*gz = SIZE_GRID_FX32( vec->z );
}

//==============================================================================
//	�`�揉�����A���A����ɌĂ΂�铮�쏉�����֐�
//==============================================================================
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
void FLDMMDL_DrawInitAfterMoveProcCall( FLDMMDL * fmmdl )
{
	int code = FLDMMDL_MoveCodeGet( fmmdl );
	
	if( code == MV_HIDE_SNOW || code == MV_HIDE_SAND ||
		code == MV_HIDE_GRND || code == MV_HIDE_KUSA ){
		FLDMMDL_MoveProcCall( fmmdl );
	}
}

//==============================================================================
//	���̑��p�[�c
//==============================================================================
//--------------------------------------------------------------
//	�������]�e�[�u��
//--------------------------------------------------------------
static const int DATA_DirFlipTbl[] =
{ DIR_DOWN, DIR_UP, DIR_RIGHT, DIR_LEFT };

//--------------------------------------------------------------
/**
 * �^����ꂽ�����𔽓]����
 * @param	dir		DIR_UP��
 * @retval	int		dir�̔��]����
 */
//--------------------------------------------------------------
int FieldOBJTool_DirFlip( int dir )
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
int FieldOBJTool_DirRange( int ax, int az, int bx, int bz )
{
	if( ax > bx ){ return( DIR_LEFT ); }
	if( ax < bx ){ return( DIR_RIGHT ); }
	if( az > bz ){ return( DIR_UP ); }
	
	return( DIR_DOWN );
}

//--------------------------------------------------------------
/**
 * �����擾
 * @param	fsys	FIELDSYS_WORK *
 * @param	vec		�����擾����X,Y,Z�i�[�|�C���^�BY�͍����擾�ōX�V
 * @retval	int		TRUE=����擾�@FALSE=�����ݒ薳��
 */
//--------------------------------------------------------------
#if 0
int FieldOBJTool_GetHeight( FIELDSYS_WORK *fsys, VecFx32 *vec )
{
	fx32 y;
	HEIGHT_TYPE flag;
	
	y = GetHeightPack( fsys, vec->y, vec->x, vec->z, &flag );
	
	if( flag == HEIGHT_FAIL ){
		return( FALSE );
	}
	
	vec->y = y;
	return( TRUE );
}
#endif

//--------------------------------------------------------------
/**
 * �����擾
 * @param	fsys	FIELDSYS_WORK *
 * @param	vec		�����擾����X,Y,Z�i�[�|�C���^�BY�͍����擾�ōX�V
 * @param	eflag	TRUE=�g����������
 * @retval	int		TRUE=����擾�@FALSE=�����ݒ薳��
 */
//--------------------------------------------------------------
#if 0
int FieldOBJTool_GetHeightExpand( FIELDSYS_WORK *fsys, VecFx32 *vec, int eflag )
{
	fx32 y;
	HEIGHT_TYPE flag;
	
	y = GetHeightPack( fsys, vec->y, vec->x, vec->z, &flag );
	
	if( flag == HEIGHT_FAIL ){
		return( FALSE );
	}
	
	if( flag == HEIGHT_EXPAND && eflag == FALSE ){
		return( FALSE );
	}
	
	vec->y = y;
	return( TRUE );
}
#endif
