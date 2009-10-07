//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		fldmmdl_railmove.c
 *	@brief  ���샂�f��  ���[������n
 *	@author	 tomoya takahashi
 *	@date		2009.08.24
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "fldmmdl.h"

#include "fieldmap.h"
#include "map_attr.h"

#include "fldeff_shadow.h"
#include "fldeff_kemuri.h"
#include "fldeff_grass.h"
#include "fldeff_footmark.h"
#include "fldeff_reflect.h"

//#define MMDL_RAIL_NULL  // fldmmdl_move���玝���Ă����A����g�ݍ��މ\�������鏈��

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

static int MMdl_CheckMoveStart( const MMDL * mmdl );
static void MMdl_GetAttrMoveBefore( MMDL * mmdl );
static void MMdl_ProcMoveStartFirst( MMDL * mmdl );
static void MMdl_ProcMoveStartSecond( MMDL * mmdl );
static void MMdl_ProcMoveEnd( MMDL * mmdl );

//�A�g���r���[�g�X�V
static BOOL MMdl_UpdateCurrentRailAttr( MMDL * mmdl ); 


// �A�g���r���[�g�֌W
// @TODO �`�����ŁA���g�͖������@090826 tomoya
static void MMdl_MapAttrProc_MoveStartFirst( MMDL * mmdl );
static void MMdl_MapAttrProc_MoveStartSecond( MMDL * mmdl );
static void MMdl_MapAttrProc_MoveStartJumpSecond( MMDL * mmdl );
static void MMdl_MapAttrProc_MoveEnd( MMDL * mmdl );
static void MMdl_MapAttrProc_MoveEndJump( MMDL * mmdl );

static void MMdl_MapAttrHeight_02(
		MMDL * mmdl,MATR now, MATR old, const OBJCODE_PARAM *prm );

static void MMdl_MapAttrGrassProc_0(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );
static void MMdl_MapAttrGrassProc_12(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );

static void MMdl_MapAttrFootMarkProc_1(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );

static void MMdl_MapAttrSplashProc_012(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );
static void MMdl_MapAttrSplashProc_Jump1(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );

static void MMdl_MapAttrShadowProc_0(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );
static void MMdl_MapAttrShadowProc_1(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );
static void MMdl_MapAttrShadowProc_2(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );

static void MMdl_MapAttrGroundSmokeProc_2(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );

static void MMdl_MapAttrLGrassProc_0(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );
static void MMdl_MapAttrLGrassProc_1(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );

static void MMdl_MapAttrNGrassProc_0(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );
static void MMdl_MapAttrNGrassProc_1(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );

static void MMdl_MapAttrPoolProc_1(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );
static void MMdl_MapAttrPoolProc_2(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );

static void MMdl_MapAttrSwampProc_1(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );
static void MMdl_MapAttrSwampProc_2(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );

static void MMdl_MapAttrReflect_01(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );
static void MMdl_MapAttrReflect_2(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );

static void MMdl_MapAttrBridgeProc_01(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );

static void MMdl_MapAttrSEProc_1(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );





static FLDEFF_CTRL * mmdl_GetFldEffCtrl( MMDL *mmdl );



//----------------------------------------------------------------------------
/**
 *	@brief  �t�B�[���h���샂�f���@���[�����쏉����
 *
 *	@param	mmdl 
 */
//-----------------------------------------------------------------------------
void MMDL_InitRailMoveProc( MMDL * mmdl )
{
	MMDL_CallMoveInitProc( mmdl );
#ifndef MMDL_RAIL_NULL 
//	MMDL_MoveSubProcInit( mmdl );
#endif
	MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_MOVEPROC_INIT );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t�B�[���h���샂�f��  ���[������@�X�V����
 *
 *	@param	mmdl 
 */
//-----------------------------------------------------------------------------
void MMDL_UpdateRailMove( MMDL * mmdl )
{
	if( MMDL_CheckMMdlSysStatusBit(
		mmdl,MMDLSYS_STABIT_MOVE_PROC_STOP) ){
		return;
	}
	
	MMdl_GetAttrMoveBefore( mmdl );
	MMdl_ProcMoveStartFirst( mmdl );
	
	if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_ACMD) ){
		MMDL_ActionAcmd( mmdl );
	}else if( MMDL_CheckStatusBitMoveProcPause(mmdl) == FALSE ){
		if( MMdl_CheckMoveStart(mmdl) == TRUE ){

			MMDL_CallMoveProc( mmdl );

#ifndef MMDL_RAIL_NULL
//			if( MMDL_MoveSub(mmdl) == FALSE ){
//				MMDL_CallMoveProc( mmdl );
//			}
#endif
		}
	}
	
	MMdl_ProcMoveStartSecond( mmdl );
	MMdl_ProcMoveEnd( mmdl );
}








//-----------------------------------------------------------------------------
/**
 *      private�֐�
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  ����\�@�`�F�b�N
 *
 *	@param	mmdl 
 *
 *	@retval TRUE  �\
 *	@retval FALSE �s�\
 */
//-----------------------------------------------------------------------------
static int MMdl_CheckMoveStart( const MMDL * mmdl )
{
	if( MMDL_CheckStatusBitMove(mmdl) == TRUE ){
		return( TRUE );
	}
	
	if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_ATTR_GET_ERROR) == 0 ){
		return( TRUE );
	}else if( MMDL_GetMoveCode(mmdl) == MV_TR_PAIR ){ //�e�̍s���ɏ]��
		return( TRUE );
	}
	
#ifndef MMDL_RAIL_NULL

  #ifndef MMDL_PL_NULL
	{	//�ړ��֎~�t���O���E�`�F�b�N
		u32 st = MMDL_GetStatusBit( mmdl );
		
		//�����擾���Ȃ��ꍇ
		if( (st&MMDL_STABIT_HEIGHT_GET_ERROR) &&
			(st&MMDL_STABIT_HEIGHT_GET_OFF) == 0 ){
			return( FALSE );
		}
		
		//�A�g���r���[�g�擾���Ȃ��ꍇ
		if( (st&MMDL_STABIT_ATTR_GET_ERROR) &&	
			MMDL_CheckMoveBitAttrGetOFF(mmdl) == FALSE ){
			return( FALSE );
		}
	}
  #endif

#endif
	
	return( TRUE );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[���A�g���r���[�g�擾
 *
 *	@param	mmdl 
 */
//-----------------------------------------------------------------------------
static void MMdl_GetAttrMoveBefore( MMDL * mmdl )
{
	if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_ATTR_GET_ERROR) )
  {
		if( MMdl_UpdateCurrentRailAttr(mmdl) == TRUE )
    {
			MMDL_OnStatusBitMoveStart( mmdl );
    }
  }
}

//----------------------------------------------------------------------------
/**
 * �ړ��J�n�Ŕ����@1st
 *	@param	mmdl 
 */
//-----------------------------------------------------------------------------
static void MMdl_ProcMoveStartFirst( MMDL * mmdl )
{
	if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_MOVE_START) ){
		MMdl_MapAttrProc_MoveStartFirst( mmdl );
	}
	
	MMDL_OffStatusBit( mmdl,
		MMDL_STABIT_MOVE_START | MMDL_STABIT_JUMP_START );
}

//----------------------------------------------------------------------------
/**
 * �ړ��J�n�Ŕ����@2nd
 *	@param	mmdl 
 */
//-----------------------------------------------------------------------------
static void MMdl_ProcMoveStartSecond( MMDL * mmdl )
{
	if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_JUMP_START) ){
		MMdl_MapAttrProc_MoveStartJumpSecond( mmdl );
	}else if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_MOVE_START) ){
		MMdl_MapAttrProc_MoveStartSecond( mmdl );
	}
	
	MMDL_OffStatusBit( mmdl,
		MMDL_STABIT_MOVE_START | MMDL_STABIT_JUMP_START );
}

//----------------------------------------------------------------------------
/**
 * ����I���Ŕ���
 *	@param	mmdl 
 */
//-----------------------------------------------------------------------------
static void MMdl_ProcMoveEnd( MMDL * mmdl )
{
	if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_JUMP_END) ){
		MMdl_MapAttrProc_MoveEndJump( mmdl );
	}else if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_MOVE_END) ){
		MMdl_MapAttrProc_MoveEnd( mmdl );
	}
	
	MMDL_OffStatusBit( mmdl,
		MMDL_STABIT_MOVE_END | MMDL_STABIT_JUMP_END );
}




//----------------------------------------------------------------------------
/**
 *	@brief  ���ݍ��W�ŃA�g���r���[�g���f
 *          MMDL_STABIT_ATTR_GET_ERROR�̃Z�b�g�������čs��
 *
 *	@param	mmdl
 *
 *	@retval TRUE  �擾����
 *	@retval FALSE �擾���s
 */
//-----------------------------------------------------------------------------
static BOOL MMdl_UpdateCurrentRailAttr( MMDL * mmdl )
{
	MAPATTR old_attr = MAPATTR_ERROR;
	MAPATTR now_attr = old_attr;
	
  if( MMDL_CheckMoveBitAttrGetOFF(mmdl) == FALSE ){
    // ���[������A���ƑO��LOCATION������悤�ɂ���
    RAIL_LOCATION old;
    RAIL_LOCATION now;
    const MMDLSYS* mmdlsys = MMDL_GetMMdlSys( mmdl );
    const FLDNOGRID_MAPPER* pNOGRIDMapper = MMDLSYS_GetNOGRIDMapper( mmdlsys );

    MMDL_GetRailLocation( mmdl, &now );
    MMDL_GetOldRailLocation( mmdl, &old );

    old_attr = FLDNOGRID_MAPPER_GetAttr( pNOGRIDMapper, &old );
    now_attr = FLDNOGRID_MAPPER_GetAttr( pNOGRIDMapper, &now );
  }
  
	MMDL_SetMapAttrOld( mmdl, old_attr );
	MMDL_SetMapAttr( mmdl, now_attr );
  
  if( now_attr == MAPATTR_ERROR ){
		MMDL_OnStatusBit( mmdl, MMDL_STABIT_ATTR_GET_ERROR );
		return( FALSE );
  }
  
	MMDL_OffStatusBit( mmdl, MMDL_STABIT_ATTR_GET_ERROR );
	return( TRUE );
}





//======================================================================
//	�}�b�v�A�g���r���[�g
//======================================================================
//--------------------------------------------------------------
/**
 * ����J�n�Ŕ�������A�g���r���[�g���� 1st
 * @param	mmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrProc_MoveStartFirst( MMDL * mmdl )
{
	MMdl_UpdateCurrentRailAttr( mmdl );
	
	if( MMDL_CheckCompletedDrawInit(mmdl) == TRUE ){
		MATR now = MMDL_GetMapAttr( mmdl );
		MATR old = MMDL_GetMapAttrOld( mmdl );
		const OBJCODE_PARAM *prm =
			MMDL_GetOBJCodeParam( mmdl, MMDL_GetOBJCode(mmdl) );
		
		MMdl_MapAttrBridgeProc_01( mmdl, now, old, prm );
		MMdl_MapAttrGrassProc_0( mmdl, now, old, prm );
		MMdl_MapAttrSplashProc_012( mmdl, now, old, prm );
		MMdl_MapAttrShadowProc_0( mmdl, now, old, prm );
		MMdl_MapAttrHeight_02( mmdl, now, old, prm );
		MMdl_MapAttrLGrassProc_0( mmdl, now, old, prm );
		MMdl_MapAttrNGrassProc_0( mmdl, now, old, prm );
		MMdl_MapAttrReflect_01( mmdl, now, old, prm );
	}
}

//--------------------------------------------------------------
/**
 * ����J�n�Ŕ�������A�g���r���[�g���� 2nd
 * @param	mmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrProc_MoveStartSecond( MMDL * mmdl )
{
	MMdl_UpdateCurrentRailAttr( mmdl );
	
	if( MMDL_CheckCompletedDrawInit(mmdl) == TRUE ){
		MATR now = MMDL_GetMapAttr( mmdl );
		MATR old = MMDL_GetMapAttrOld( mmdl );
		const OBJCODE_PARAM *prm =
			MMDL_GetOBJCodeParam( mmdl, MMDL_GetOBJCode(mmdl) );

		MMdl_MapAttrBridgeProc_01( mmdl, now, old, prm );
		MMdl_MapAttrGrassProc_12( mmdl, now, old, prm );
		MMdl_MapAttrFootMarkProc_1( mmdl, now, old, prm );
		MMdl_MapAttrSplashProc_012( mmdl, now, old, prm );
		MMdl_MapAttrShadowProc_1( mmdl, now, old, prm );
		MMdl_MapAttrLGrassProc_1( mmdl, now, old, prm );
		MMdl_MapAttrNGrassProc_1( mmdl, now, old, prm );
		MMdl_MapAttrPoolProc_1( mmdl, now, old, prm );
		MMdl_MapAttrSwampProc_1( mmdl, now, old, prm );
		MMdl_MapAttrReflect_01( mmdl, now, old, prm );
		
		MMdl_MapAttrSEProc_1( mmdl, now, old, prm ); //�`��֌W�Ȃ�?
	}
}

//--------------------------------------------------------------
/**
 * ����J�n�Ŕ�������A�g���r���[�g���� 2nd Jump
 * @param	mmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrProc_MoveStartJumpSecond( MMDL * mmdl )
{
	MMdl_UpdateCurrentRailAttr( mmdl );
	
	if( MMDL_CheckCompletedDrawInit(mmdl) == TRUE ){
		MATR now = MMDL_GetMapAttr( mmdl );
		MATR old = MMDL_GetMapAttrOld( mmdl );
		const OBJCODE_PARAM *prm =
			MMDL_GetOBJCodeParam( mmdl, MMDL_GetOBJCode(mmdl) );

		MMdl_MapAttrBridgeProc_01( mmdl, now, old, prm );
		MMdl_MapAttrShadowProc_1( mmdl, now, old, prm );
		MMdl_MapAttrReflect_01( mmdl, now, old, prm );
		MMdl_MapAttrSplashProc_Jump1( mmdl, now, old, prm );
		MMdl_MapAttrSEProc_1( mmdl, now, old, prm );//�`��֌W�Ȃ�?
	}
}

//--------------------------------------------------------------
/**
 * ����I���Ŕ�������A�g���r���[�g�����@end
 * @param	mmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrProc_MoveEnd( MMDL * mmdl )
{
	MMdl_UpdateCurrentRailAttr( mmdl );
	
	if( MMDL_CheckCompletedDrawInit(mmdl) == TRUE ){
		MATR now = MMDL_GetMapAttr( mmdl );
		MATR old = MMDL_GetMapAttrOld( mmdl );
		const OBJCODE_PARAM *prm =
			MMDL_GetOBJCodeParam( mmdl, MMDL_GetOBJCode(mmdl) );
		
		//�I���@�A�g���r���[�g����
		MMdl_MapAttrHeight_02( mmdl, now, old, prm );
		MMdl_MapAttrPoolProc_2( mmdl, now, old, prm );
		MMdl_MapAttrSwampProc_2( mmdl, now, old, prm );
		MMdl_MapAttrSplashProc_012( mmdl, now, old, prm );
		MMdl_MapAttrReflect_2( mmdl, now, old, prm );
		MMdl_MapAttrShadowProc_2( mmdl, now, old, prm );
	}
}

//--------------------------------------------------------------
/**
 * ����I���Ŕ�������A�g���r���[�g�����@end jump
 * @param	mmdl	MMDL *
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrProc_MoveEndJump( MMDL * mmdl )
{
	MMdl_UpdateCurrentRailAttr( mmdl );
	
	if( MMDL_CheckCompletedDrawInit(mmdl) == TRUE ){
		MATR now = MMDL_GetMapAttr( mmdl );
		MATR old = MMDL_GetMapAttrOld( mmdl );
		const OBJCODE_PARAM *prm =
			MMDL_GetOBJCodeParam( mmdl, MMDL_GetOBJCode(mmdl) );
		
		//�I���@�A�g���r���[�g����
		MMdl_MapAttrHeight_02( mmdl, now, old, prm );
		MMdl_MapAttrPoolProc_2( mmdl, now, old, prm );
		MMdl_MapAttrSwampProc_2( mmdl, now, old, prm );
		MMdl_MapAttrSplashProc_012( mmdl, now, old, prm );
		MMdl_MapAttrReflect_2( mmdl, now, old, prm );
		MMdl_MapAttrShadowProc_2( mmdl, now, old, prm );
		MMdl_MapAttrGrassProc_12( mmdl, now, old, prm );
		MMdl_MapAttrGroundSmokeProc_2( mmdl, now, old, prm );
	}
}

//======================================================================
//	�A�g���r���[�g�ύ���
//======================================================================
//--------------------------------------------------------------
/**
 * �A�g���r���[�g�ύ����@����J�n�A����I��
 * @param	mmdl		MMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrHeight_02(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
#ifndef MMDL_RAIL_NULL 
  
	#ifndef MMDL_PL_NULL
	if( MMDL_CheckStatusBitAttrOffsetOFF(mmdl) == FALSE ){
		if( MATR_IsSwampDeep(now) == TRUE ||
			MATR_IsSwampGrassDeep(now) == TRUE ){
			VecFx32 vec = { 0, ATTROFFS_Y_NUMA_DEEP, 0 };
			MMDL_SetVectorAttrDrawOffsetPos( mmdl, &vec );
			return;
		}
		
		if( MATR_IsSwamp(now) == TRUE || MATR_IsSwampGrass(now) == TRUE ){
			VecFx32 vec = { 0, ATTROFFS_Y_NUMA, 0 };
			MMDL_SetVectorAttrDrawOffsetPos( mmdl, &vec );
			return;
		}
		
		if( MATR_IsSnowDeepMost(now) == TRUE ){
			VecFx32 vec = { 0, ATTROFFS_Y_YUKI_DEEP_MOST, 0 };
			MMDL_SetVectorAttrDrawOffsetPos( mmdl, &vec );
			return;
		}
		
		if( MATR_IsSnowDeep(now) == TRUE ){
			VecFx32 vec = { 0, ATTROFFS_Y_YUKI_DEEP, 0 };
			MMDL_SetVectorAttrDrawOffsetPos( mmdl, &vec );
			return;
		}
		
		if( MATR_IsShallowSnow(now) == TRUE ){
			VecFx32 vec = { 0, ATTROFFS_Y_YUKI, 0 };
			MMDL_SetVectorAttrDrawOffsetPos( mmdl, &vec );
			return;
		}
	}
	
	{
		VecFx32 vec = { 0, 0, 0 };
		MMDL_SetVectorAttrDrawOffsetPos( mmdl, &vec );
	}
	#endif

#endif
}

//======================================================================
//	�A�g���r���[�g�@��
//======================================================================
//--------------------------------------------------------------
/**
 * ���@����J�n 0
 * @param	mmdl		MMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrGrassProc_0(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
#ifndef MMDL_RAIL_NULL 

	#ifndef MMDL_PL_NULL
	if( MATR_IsGrass(now) == TRUE ){
		FE_mmdlGrass_Add( mmdl, FALSE );
	}
	#else
  {
    MAPATTR_FLAG flag = MAPATTR_GetAttrFlag( now );
    
    if( (flag&MAPATTR_FLAGBIT_GRASS) ){
      FLDEFF_CTRL *fectrl = mmdl_GetFldEffCtrl( mmdl );
      FLDEFF_GRASS_SetMMdl( fectrl, mmdl, FALSE );
    }
  }
  #endif

#endif
}

//--------------------------------------------------------------
/**
 * ���@���� 1,2
 * @param	mmdl	MMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrGrassProc_12(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
#ifndef MMDL_RAIL_NULL 
  
	#ifndef MMDL_PL_NULL
	if( MATR_IsGrass(now) == TRUE ){
		FE_mmdlGrass_Add( mmdl, TRUE );
	}
	#else
  {
    if( now != MAPATTR_ERROR ){
      MAPATTR_FLAG flag = MAPATTR_GetAttrFlag( now );
    
      if( (flag&MAPATTR_FLAGBIT_GRASS) ){
        FLDEFF_CTRL *fectrl = mmdl_GetFldEffCtrl( mmdl );
        FLDEFF_GRASS_SetMMdl( fectrl, mmdl, TRUE );
      }
    }
  }
  #endif

#endif
}

//======================================================================
//	�A�g���r���[�g�@����
//======================================================================
//--------------------------------------------------------------
/**
 * ���Ձ@����J�n 1
 * @param	mmdl	MMDL *
 * @param	now		���݂̃A�g���r���[�g
 * @param	old		�ߋ��̃A�g���r���[�g
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrFootMarkProc_1(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
#ifndef MMDL_RAIL_NULL 

	#ifndef MMDL_PL_NULL
	if( prm->footmark_type == MMDL_FOOTMARK_NON ){
		return;
	}
	
	if( MATR_IsShadowOnSnow(old) == TRUE ){
		if( prm->footmark_type == MMDL_FOOTMARK_NORMAL ){
			FE_mmdlFootMarkShadowSnow_Add( mmdl );
		}else if( prm->footmark_type == MMDL_FOOTMARK_CYCLE ){
			FE_mmdlFootMarkShadowSnowCycle_Add( mmdl );
		}
	}
	
	if(	MMdl_CheckMapAttrKind_Sand(mmdl,old) == TRUE ){
		if( prm->footmark_type == MMDL_FOOTMARK_NORMAL ){
			FE_mmdlFootMarkNormal_Add( mmdl );
		}else if( prm->footmark_type == MMDL_FOOTMARK_CYCLE ){
			FE_mmdlFootMarkCycle_Add( mmdl );
		}
		return;
	}
	
	if( MATR_IsSnowDeep(old) == TRUE || MATR_IsSnowDeepMost(old) == TRUE ||
		MATR_IsShallowSnow(old) ){
		FE_mmdlFootMarkSnowDeep_Add( mmdl );
		return;
	}
	
	if( MMdl_CheckMapAttrKind_MostShallowSnow(mmdl,old) == TRUE ){
		FE_mmdlFootMarkSnow_Add( mmdl );
		return;
	}
	#else
  if( old != MAPATTR_ERROR ){
    MAPATTR_FLAG flag = MAPATTR_GetAttrFlag( old );
  
    if( (flag & MAPATTR_FLAGBIT_FOOTMARK) ){
      FLDEFF_CTRL *fectrl = mmdl_GetFldEffCtrl( mmdl );
      FOOTMARK_TYPE type = FOOTMARK_TYPE_HUMAN;
    
      if( prm->footmark_type == MMDL_FOOTMARK_CYCLE ){
        type = FOOTMARK_TYPE_CYCLE;
      }

      FLDEFF_FOOTMARK_SetMMdl( mmdl, fectrl, type );
    }
  }
  #endif

#endif
}

//======================================================================
//	�A�g���r���[�g�@����
//======================================================================
//--------------------------------------------------------------
/**
 * ���򖗁@����J�n�I�� 012
 * @param	mmdl	MMDL *
 * @param	now		���݂̃A�g���r���[�g
 * @param	old		�ߋ��̃A�g���r���[�g
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrSplashProc_012(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
#ifndef MMDL_RAIL_NULL 

	#ifndef MMDL_PL_NULL
	if( MATR_IsShoal(now) == TRUE ){
		if( MMDL_CheckStatusBitShoalEffect(mmdl) == FALSE ){
			FE_mmdlSplash_Add( mmdl, TRUE );
			MMDL_SetStatusBitShoalEffect( mmdl, TRUE );
		}
	}else{
		MMDL_SetStatusBitShoalEffect( mmdl, FALSE );
	}
	#endif

#endif
}

//--------------------------------------------------------------
/**
 * ���򖗁@�W�����v����J�n 1
 * @param	mmdl	MMDL *
 * @param	now		���݂̃A�g���r���[�g
 * @param	old		�ߋ��̃A�g���r���[�g
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrSplashProc_Jump1(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
#ifndef MMDL_RAIL_NULL 
	MMDL_SetStatusBitShoalEffect( mmdl, FALSE );
#endif
}

//======================================================================
//	�A�g���r���[�g�@�e
//======================================================================
//--------------------------------------------------------------
/**
 * �e�@����J�n 0
 * @param	mmdl	MMDL *
 * @param	now		���݂̃A�g���r���[�g
 * @param	old		�ߋ��̃A�g���r���[�g
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrShadowProc_0(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
	MMdl_MapAttrShadowProc_1( mmdl, now, old, prm );
}

//--------------------------------------------------------------
/**
 * �e�@����J�n 1
 * @param	mmdl	MMDL *
 * @param	now		���݂̃A�g���r���[�g
 * @param	old		�ߋ��̃A�g���r���[�g
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrShadowProc_1(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
#ifndef MMDL_RAIL_NULL  

	#ifndef MMDL_PL_NULL
	{
		const MMDLSYS *fos = MMDL_GetMMdlSys( mmdl );
		
		if( MMDLSYS_CheckJoinShadow(fos) == FALSE ){
			return;
		}
	}
	
	if( prm->shadow_type == MMDL_SHADOW_NON ){
		return;
	}

	if( MATR_IsGrass(now) == TRUE ||
		MATR_IsLongGrass(now) == TRUE ||
		MMdl_CheckMapAttrKind_Water(mmdl,now) == TRUE ||
		MATR_IsPoolCheck(now) == TRUE ||
		MATR_IsShoal(now) == TRUE ||
		MMdl_CheckMapAttrKind_Snow(mmdl,now) == TRUE ||
		MATR_IsSwamp(now) == TRUE ||
		MATR_IsSwampGrass(now) == TRUE ||
		MATR_IsMirrorReflect(now) ){
		MMDL_OnStatusBit( mmdl, MMDL_STABIT_SHADOW_VANISH );
	}else{
		if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_SHADOW_SET) == 0 ){
			FE_mmdlShadow_Add( mmdl );
			MMDL_OnStatusBit( mmdl, MMDL_STABIT_SHADOW_SET );
		}
	}
	#else
  {
  	const MMDLSYS *fos = MMDL_GetMMdlSys( mmdl );
    
		if( MMDLSYS_CheckJoinShadow(fos) == FALSE ){
			return;
		}
    
		if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_SHADOW_SET) == 0 ){
      FLDEFF_CTRL *fectrl = mmdl_GetFldEffCtrl( mmdl );
      FLDEFF_SHADOW_SetMMdl( mmdl, fectrl );
			MMDL_OnStatusBit( mmdl, MMDL_STABIT_SHADOW_SET );
    }
  }
  #endif

#endif
}

//--------------------------------------------------------------
/**
 * �e�@����I�� 2
 * @param	mmdl	MMDL *
 * @param	now		���݂̃A�g���r���[�g
 * @param	old		�ߋ��̃A�g���r���[�g
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrShadowProc_2(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
#ifndef MMDL_RAIL_NULL  
  
	#ifndef MMDL_PL_NULL
	{
		const MMDLSYS *fos = MMDL_GetMMdlSys( mmdl );
		
		if( MMDLSYS_CheckJoinShadow(fos) == FALSE ){
			return;
		}
	}
	
	if( prm->shadow_type == MMDL_SHADOW_NON ){
		return;
	}
	
	if( MATR_IsGrass(now) == TRUE ||
		MATR_IsLongGrass(now) == TRUE ||
		MMdl_CheckMapAttrKind_Water(mmdl,now) == TRUE ||
		MATR_IsPoolCheck(now) == TRUE ||
		MATR_IsShoal(now) == TRUE ||
		MMdl_CheckMapAttrKind_Snow(mmdl,now) == TRUE ||
		MATR_IsSwamp(now) == TRUE ||
		MATR_IsSwampGrass(now) == TRUE ||
		MATR_IsMirrorReflect(now) ){
		MMDL_OnStatusBit( mmdl, MMDL_STABIT_SHADOW_VANISH );
	}else{
		MMDL_OffStatusBit( mmdl, MMDL_STABIT_SHADOW_VANISH );
	}
	#endif

#endif
}

//======================================================================
//	�A�g���r���[�g�@�y��
//======================================================================
//--------------------------------------------------------------
/**
 * �y���@����I�� 2 
 * @param	mmdl	MMDL *
 * @param	now		���݂̃A�g���r���[�g
 * @param	old		�ߋ��̃A�g���r���[�g
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrGroundSmokeProc_2(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
#ifndef MMDL_RAIL_NULL 

	#ifndef MMDL_PL_NULL
	if( MMdl_CheckMapAttrKind_Water(mmdl,now) == TRUE ||
		MATR_IsShoal(now) == TRUE ||
		MATR_IsIce(now) == TRUE ||
		MATR_IsSwamp(now) == TRUE ||
		MATR_IsSwampGrass(now) == TRUE ||
		MMdl_CheckMapAttrKind_Snow(mmdl,now) == TRUE ){
		return;
	}
	
	FE_mmdlKemuri_Add( mmdl );
	#else
  {
    FLDEFF_CTRL *fectrl = mmdl_GetFldEffCtrl( mmdl );
    FLDEFF_KEMURI_SetMMdl( mmdl, fectrl );
  }
  #endif

#endif
}


//======================================================================
//	�A�g���r���[�g�@������
//======================================================================
//--------------------------------------------------------------
/**
 * �������@����J�n 0
 * @param	mmdl		MMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrLGrassProc_0(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
#ifndef MMDL_RAIL_NULL 
  
	#ifndef MMDL_PL_NULL
	if( MATR_IsLongGrass(now) == TRUE ){
		FE_mmdlLGrass_Add( mmdl, FALSE );
	}
	#endif

#endif
}

//--------------------------------------------------------------
/**
 * �������@����J�n 1
 * @param	mmdl	MMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrLGrassProc_1(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
#ifndef MMDL_RAIL_NULL 
  
	#ifndef MMDL_PL_NULL
	if( MATR_IsLongGrass(now) == TRUE ){
		FE_mmdlLGrass_Add( mmdl, TRUE );
	}
	#endif

#endif
}

//======================================================================
//	�A�g���r���[�g�@����
//======================================================================
//--------------------------------------------------------------
/**
 * �����@����J�n 0
 * @param	mmdl		MMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrNGrassProc_0(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
#ifndef MMDL_RAIL_NULL 
  
	#ifndef MMDL_PL_NULL
	if( MATR_IsSwampGrass(now) == TRUE ){
		FE_mmdlNGrass_Add( mmdl, FALSE );
	}
	#endif

#endif
}

//--------------------------------------------------------------
/**
 * �����@����J�n 1
 * @param	mmdl	MMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrNGrassProc_1(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
#ifndef MMDL_RAIL_NULL 
  
	#ifndef MMDL_PL_NULL
	if( MATR_IsSwampGrass(now) == TRUE ){
		FE_mmdlNGrass_Add( mmdl, TRUE );
	}
	#endif

#endif
}

//======================================================================
//	�A�g���r���[�g�@�����܂�
//======================================================================
//--------------------------------------------------------------
/**
 * �����܂�@����J�n 1
 * @param	mmdl	MMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrPoolProc_1(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
#ifndef MMDL_RAIL_NULL 
  
	#ifndef MMDL_PL_NULL
	if( MATR_IsPoolCheck(old) == TRUE ){
		FE_mmdlRippleSet( mmdl,
			MMDL_GetOldGridPosX(mmdl),
			MMDL_GetOldGridPosY(mmdl), 
			MMDL_GetOldGridPosZ(mmdl) );
	}
	#endif

#endif
}

//--------------------------------------------------------------
/**
 * �����܂�@����I�� 2
 * @param	mmdl	MMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrPoolProc_2(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
#ifndef MMDL_RAIL_NULL 
  
	#ifndef MMDL_PL_NULL
	if( MATR_IsPoolCheck(now) == TRUE ){
		FE_mmdlRippleSet( mmdl,
			MMDL_GetGridPosX(mmdl),
			MMDL_GetGridPosY(mmdl), 
			MMDL_GetGridPosZ(mmdl) );
	}
	#endif

#endif
}

//======================================================================
//	�A�g���r���[�g�@��
//======================================================================
//--------------------------------------------------------------
/**
 * ���@����J�n 1
 * @param	mmdl	MMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrSwampProc_1(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
#ifndef MMDL_RAIL_NULL 
  
	#ifndef MMDL_PL_NULL
	if( MATR_IsSwamp(old) == TRUE ){
		FE_mmdlNRippleSet( mmdl,
			MMDL_GetOldGridPosX(mmdl),
			MMDL_GetOldGridPosY(mmdl), 
			MMDL_GetOldGridPosZ(mmdl) );
	}
	#endif

#endif
}

//--------------------------------------------------------------
/**
 * ���@����I�� 2
 * @param	mmdl	MMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrSwampProc_2(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
#ifndef MMDL_RAIL_NULL 
  
	#ifndef MMDL_PL_NULL
	if( MATR_IsSwamp(now) == TRUE ){
		FE_mmdlNRippleSet( mmdl,
			MMDL_GetGridPosX(mmdl),
			MMDL_GetGridPosY(mmdl), 
			MMDL_GetGridPosZ(mmdl) );
	}
	#endif

#endif
}

//======================================================================
//	�A�g���r���[�g�@�f�肱��
//======================================================================
//--------------------------------------------------------------
/**
 * �f�肱�݁@����J�n 01
 * @param	mmdl	MMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrReflect_01(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
#ifndef MMDL_RAIL_NULL 
  
	#ifndef MMDL_PL_NULL
	if( prm->reflect_type == MMDL_REFLECT_NON ){
		return;
	}
	
	{
		if( MMDL_CheckStatusBitReflect(mmdl) == FALSE ){
			MATR hit = MATR_IsNotAttrGet();
			
			if( MATR_IsReflect(now) == TRUE ){
				hit = now;
			}else{
				MATR next = MMDL_GetMapDirAttr( mmdl, DIR_DOWN );
				
				if( MATR_IsReflect(next) == TRUE ){
					hit = next;
				}
			}
			
			if( hit != MATR_IsNotAttrGet() ){	//�f�荞�݃q�b�g
				REFTYPE type;
				MMDL_SetStatusBitReflect( mmdl, TRUE );
				
				if( MATR_IsMirrorReflect(hit) == TRUE ){ 	//����
					type = REFTYPE_MIRROR;
				}else if( MATR_IsPoolCheck(hit) == TRUE ){	//������
					type = REFTYPE_POOL;
				}else{										//�r
					type = REFTYPE_POND;
				}
				
				FE_mmdlReflect_Add( mmdl, type );
			}
		}
	}
	#else //wb
  if( prm->reflect_type == MMDL_REFLECT_NON ){
    return;
  }
  
  if( MMDL_CheckStatusBitReflect(mmdl) == FALSE )
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
        MAPATTR next = MMDL_GetMapDirAttr( mmdl, DIR_DOWN );
        flag = MAPATTR_GetAttrFlag( next );
      
        if( (flag&MAPATTR_FLAGBIT_REFLECT) )
        {
          hit_attr = next;
        }
      }
    
      if( hit_attr != MAPATTR_ERROR )
      {
        REFLECT_TYPE type;
        FLDEFF_CTRL *fectrl = mmdl_GetFldEffCtrl( mmdl );
        MMDLSYS *mmdlsys = (MMDLSYS*)MMDL_GetMMdlSys( mmdl );
      
        MMDL_SetStatusBitReflect( mmdl, TRUE );
      
        type = REFLECT_TYPE_POND; //�{���̓A�g���r���[�g���ʂ��ă^�C�v����
        FLDEFF_REFLECT_SetMMdl( mmdlsys, mmdl, fectrl, type );
      }
    }
  }
  #endif

#endif
}

//--------------------------------------------------------------
/**
 * �f�肱�݁@����I�� 2
 * @param	mmdl	MMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrReflect_2(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
#ifndef MMDL_RAIL_NULL 
  
	#ifndef MMDL_PL_NULL
	if( prm->reflect_type == MMDL_REFLECT_NON ||
		MMDL_CheckStatusBitReflect(mmdl) == FALSE ){
		return;
	}
	
	{
		MATR attr = MMDL_GetMapDirAttr( mmdl, DIR_DOWN );
		
		if( MATR_IsReflect(attr) == FALSE ){
			MMDL_SetStatusBitReflect( mmdl, FALSE );
		}
	}
	#else //wb
  
	if( prm->reflect_type == MMDL_REFLECT_NON ||
		MMDL_CheckStatusBitReflect(mmdl) == FALSE ){
		return;
  }

  {
    MAPATTR_FLAG flag = MAPATTR_FLAGBIT_NONE;
		MAPATTR attr = MMDL_GetMapDirAttr( mmdl, DIR_DOWN );
    
    #ifdef DEBUG_REFLECT_CHECK
    attr = MAPATTR_FLAGBIT_REFLECT << 16;
    #endif
    
    if( attr == MAPATTR_ERROR ){
		  MMDL_SetStatusBitReflect( mmdl, FALSE );
    }else{
      flag = MAPATTR_GetAttrFlag( attr );

      if( (flag&MAPATTR_FLAGBIT_REFLECT) == 0 ){
		    MMDL_SetStatusBitReflect( mmdl, FALSE );
      }
    }
  }
  #endif

#endif
}

//======================================================================
//	�A�g���r���[�g�@��
//======================================================================
//--------------------------------------------------------------
/**
 * ���@����J�n 0
 * @param	mmdl		MMDL *
 * @param	now			���݂̃A�g���r���[�g
 * @param	old			�ߋ��̃A�g���r���[�g
 * @param	prm		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrBridgeProc_01(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
#ifndef MMDL_RAIL_NULL 
  
	#ifndef MMDL_PL_NULL
	if( MATR_IsBridgeFlag(now) == TRUE ){
		MMDL_SetStatusBitBridge( mmdl, TRUE );
	}else if( MMDL_CheckStatusBitBridge(mmdl) == TRUE ){
		if( MATR_IsBridge(now) == FALSE ){
			MMDL_SetStatusBitBridge( mmdl, FALSE );
		}
	}
	#endif

#endif
}

//======================================================================
//	�A�g���r���[�g�֘ASE
//======================================================================
//--------------------------------------------------------------
/**
 * SE�@����J�n 1
 * @param	mmdl	MMDL *
 * @param	now		���݂̃A�g���r���[�g
 * @param	old		�ߋ��̃A�g���r���[�g
 * @param	param		OBJCODE_PARAM
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrSEProc_1(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
#ifndef MMDL_RAIL_NULL 
  
	#ifndef MMDL_PL_NULL
	if( MATR_IsSnow(now) ){
		Snd_SePlay( SEQ_SE_DP_YUKIASHI );
	}
	#endif

#endif
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
