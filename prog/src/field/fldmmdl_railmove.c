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
static void MMdl_MapAttrProc_MoveStartFirst( MMDL * mmdl );
static void MMdl_MapAttrProc_MoveStartSecond( MMDL * mmdl );
static void MMdl_MapAttrProc_MoveStartJumpSecond( MMDL * mmdl );
static void MMdl_MapAttrProc_MoveEnd( MMDL * mmdl );
static void MMdl_MapAttrProc_MoveEndJump( MMDL * mmdl );

static void MMdl_MapAttrGrassProc_0(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );
static void MMdl_MapAttrGrassProc_12(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );

static void MMdl_MapAttrFootMarkProc_1(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );

static void MMdl_MapAttrShadowProc_0(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );
static void MMdl_MapAttrShadowProc_1(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );
static void MMdl_MapAttrShadowProc_2(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );

static void MMdl_MapAttrGroundSmokeProc_2(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );




static void MMdl_MapAttrReflect_01(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm );
static void MMdl_MapAttrReflect_2(
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
	MMDL_OnStatusBit( mmdl, MMDL_STABIT_MOVEPROC_INIT );
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
	
	if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_ACMD) ){
		MMDL_ActionAcmd( mmdl );
	}else if( MMDL_CheckMoveBitMoveProcPause(mmdl) == FALSE ){
		if( MMdl_CheckMoveStart(mmdl) == TRUE ){

			MMDL_CallMoveProc( mmdl );

		}
	}
	
	MMdl_ProcMoveStartSecond( mmdl );
	MMdl_ProcMoveEnd( mmdl );
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���[�����P�[�V�����̃A�g���r���[�g���擾����
 *
 *	@param	mmdl        ���f��
 *	@param	location    ���P�[�V����
 *
 *	@return �}�b�v�A�g���r���[�g
 */
//-----------------------------------------------------------------------------
MAPATTR MMDL_GetRailLocationAttr( const MMDL * mmdl, const RAIL_LOCATION* location )
{
  const MMDLSYS* mmdlsys = MMDL_GetMMdlSys( mmdl );
  const FLDNOGRID_MAPPER* pNOGRIDMapper = MMDLSYS_GetNOGRIDMapper( mmdlsys );

  return FLDNOGRID_MAPPER_GetAttr( pNOGRIDMapper, location );
}



//----------------------------------------------------------------------------
/**
 *	@brief  �A���O���x��������A���[���I�u�W�F�̕\��������Ԃ�
 *
 *	@param	mmdl        ���f��
 *	@param	angleYaw    �A���O��
 *
 *	@return ����
 */
//-----------------------------------------------------------------------------
u16 MMDL_RAIL_GetAngleYawToDirFour( MMDL * mmdl, u16 angleYaw )
{
  VecFx16 mmdl_way;
  u16 mmdl_yaw;
  s32 diff_yaw;
  u16 dir;
  static const u8 data_angle16_4[16] =
  {
    DIR_DOWN,
    DIR_RIGHT, DIR_RIGHT, DIR_RIGHT, DIR_RIGHT, DIR_RIGHT, DIR_RIGHT,
    DIR_UP, DIR_UP,
    DIR_LEFT, DIR_LEFT, DIR_LEFT, DIR_LEFT, DIR_LEFT, DIR_LEFT,
    DIR_DOWN,
  };

  dir = MMDL_GetDirDisp( mmdl );

  // �J���������Ɛi�s��������A�l���̕\�����������߂�
  MMDL_Rail_GetDirLineWay( mmdl, dir, &mmdl_way );
  mmdl_way.y = 0;
  VEC_Fx16Normalize( &mmdl_way, &mmdl_way );

  // ���ʕ���
  mmdl_yaw = FX_Atan2Idx( mmdl_way.x, mmdl_way.z );


  // �p�x�̍�����A�p�x�����߂�
  diff_yaw = (s32)mmdl_yaw - (s32)angleYaw;
  if( diff_yaw < 0 )
  {
    diff_yaw += 0x10000;  // �v���X�ɂ��Čv�Z
  }
  diff_yaw >>= 12;

  GF_ASSERT( diff_yaw < 16 );
  return data_angle16_4[ diff_yaw ];
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
	if( MMDL_CheckMoveBitMove(mmdl) == TRUE ){
		return( TRUE );
	}
	
	if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_ATTR_GET_ERROR) == 0 ){
		return( TRUE );
	}else if( MMDL_GetMoveCode(mmdl) == MV_TR_PAIR ){ //�e�̍s���ɏ]��
		return( TRUE );
	}
	
	
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
	if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_ATTR_GET_ERROR) )
  {
		if( MMdl_UpdateCurrentRailAttr(mmdl) == TRUE )
    {
			MMDL_OnMoveBitMoveStart( mmdl );
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
	if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_MOVE_START) ){
		MMdl_MapAttrProc_MoveStartFirst( mmdl );
	}
	
	MMDL_OffMoveBit( mmdl,
		MMDL_MOVEBIT_MOVE_START | MMDL_MOVEBIT_JUMP_START );
}

//----------------------------------------------------------------------------
/**
 * �ړ��J�n�Ŕ����@2nd
 *	@param	mmdl 
 */
//-----------------------------------------------------------------------------
static void MMdl_ProcMoveStartSecond( MMDL * mmdl )
{
	if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_JUMP_START) ){
		MMdl_MapAttrProc_MoveStartJumpSecond( mmdl );
	}else if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_MOVE_START) ){
		MMdl_MapAttrProc_MoveStartSecond( mmdl );
	}
	
	MMDL_OffMoveBit( mmdl,
		MMDL_MOVEBIT_MOVE_START | MMDL_MOVEBIT_JUMP_START );
}

//----------------------------------------------------------------------------
/**
 * ����I���Ŕ���
 *	@param	mmdl 
 */
//-----------------------------------------------------------------------------
static void MMdl_ProcMoveEnd( MMDL * mmdl )
{
	if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_JUMP_END) ){
		MMdl_MapAttrProc_MoveEndJump( mmdl );
	}else if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_MOVE_END) ){
		MMdl_MapAttrProc_MoveEnd( mmdl );
	}
	
	MMDL_OffMoveBit( mmdl,
		MMDL_MOVEBIT_MOVE_END | MMDL_MOVEBIT_JUMP_END );
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
	
  if( MMDL_CheckStatusBitAttrGetOFF(mmdl) == FALSE ){
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
		MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_ATTR_GET_ERROR );
		return( FALSE );
  }
  
	MMDL_OffMoveBit( mmdl, MMDL_MOVEBIT_ATTR_GET_ERROR );
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
	if( MMdl_UpdateCurrentRailAttr( mmdl ) )
  {
	
    if( MMDL_CheckCompletedDrawInit(mmdl) == TRUE ){
      MATR now = MMDL_GetMapAttr( mmdl );
      MATR old = MMDL_GetMapAttrOld( mmdl );
      const OBJCODE_PARAM *prm = MMDL_GetOBJCodeParam( mmdl );
      
      MMdl_MapAttrGrassProc_0( mmdl, now, old, prm );
      MMdl_MapAttrShadowProc_0( mmdl, now, old, prm );
      MMdl_MapAttrReflect_01( mmdl, now, old, prm );
    }
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
	if( MMdl_UpdateCurrentRailAttr( mmdl ) )
  {
	
    if( MMDL_CheckCompletedDrawInit(mmdl) == TRUE ){
      MATR now = MMDL_GetMapAttr( mmdl );
      MATR old = MMDL_GetMapAttrOld( mmdl );
      const OBJCODE_PARAM *prm = MMDL_GetOBJCodeParam( mmdl );

      MMdl_MapAttrGrassProc_12( mmdl, now, old, prm );
      MMdl_MapAttrFootMarkProc_1( mmdl, now, old, prm );
      MMdl_MapAttrShadowProc_1( mmdl, now, old, prm );
      MMdl_MapAttrReflect_01( mmdl, now, old, prm );
      
    }
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
	if( MMdl_UpdateCurrentRailAttr( mmdl ) )
  {
	
    if( MMDL_CheckCompletedDrawInit(mmdl) == TRUE ){
      MATR now = MMDL_GetMapAttr( mmdl );
      MATR old = MMDL_GetMapAttrOld( mmdl );
      const OBJCODE_PARAM *prm = MMDL_GetOBJCodeParam( mmdl );

      MMdl_MapAttrShadowProc_1( mmdl, now, old, prm );
      MMdl_MapAttrReflect_01( mmdl, now, old, prm );
    }
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
	if( MMdl_UpdateCurrentRailAttr( mmdl ) )
  {
	
    if( MMDL_CheckCompletedDrawInit(mmdl) == TRUE ){
      MATR now = MMDL_GetMapAttr( mmdl );
      MATR old = MMDL_GetMapAttrOld( mmdl );
      const OBJCODE_PARAM *prm = MMDL_GetOBJCodeParam( mmdl );
      
      //�I���@�A�g���r���[�g����
      MMdl_MapAttrReflect_2( mmdl, now, old, prm );
      MMdl_MapAttrShadowProc_2( mmdl, now, old, prm );
    }
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
	if( MMdl_UpdateCurrentRailAttr( mmdl ) )
  {
	
    if( MMDL_CheckCompletedDrawInit(mmdl) == TRUE ){
      MATR now = MMDL_GetMapAttr( mmdl );
      MATR old = MMDL_GetMapAttrOld( mmdl );
      const OBJCODE_PARAM *prm = MMDL_GetOBJCodeParam( mmdl );
      
      //�I���@�A�g���r���[�g����
      MMdl_MapAttrReflect_2( mmdl, now, old, prm );
      MMdl_MapAttrShadowProc_2( mmdl, now, old, prm );
      MMdl_MapAttrGrassProc_12( mmdl, now, old, prm );
      MMdl_MapAttrGroundSmokeProc_2( mmdl, now, old, prm );
    }
  }
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

  {
    MAPATTR_FLAG flag = MAPATTR_GetAttrFlag( now );
    
    if( (flag&MAPATTR_FLAGBIT_GRASS) ){
      FLDEFF_CTRL *fectrl = mmdl_GetFldEffCtrl( mmdl );
      FLDEFF_GRASS_SetMMdl( fectrl, mmdl, FALSE, FLDEFF_GRASS_SHORT );
    }
  }

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
  
  {
    if( now != MAPATTR_ERROR ){
      MAPATTR_FLAG flag = MAPATTR_GetAttrFlag( now );
    
      if( (flag&MAPATTR_FLAGBIT_GRASS) ){
        FLDEFF_CTRL *fectrl = mmdl_GetFldEffCtrl( mmdl );
        FLDEFF_GRASS_SetMMdl( fectrl, mmdl, TRUE, FLDEFF_GRASS_SHORT  );
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
 * @param	mmdl	MMDL *
 * @param	now		���݂̃A�g���r���[�g
 * @param	old		�ߋ��̃A�g���r���[�g
 * @retval	nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrFootMarkProc_1(
		MMDL * mmdl, MATR now, MATR old, const OBJCODE_PARAM *prm )
{
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
  {
  	const MMDLSYS *fos = MMDL_GetMMdlSys( mmdl );
    
		if( MMDLSYS_CheckJoinShadow(fos) == FALSE ){
			return;
		}
    
		if( MMDL_CheckMoveBit(mmdl,MMDL_MOVEBIT_SHADOW_SET) == 0 ){
      FLDEFF_CTRL *fectrl = mmdl_GetFldEffCtrl( mmdl );
      FLDEFF_SHADOW_SetMMdl( mmdl, fectrl );
			MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_SHADOW_SET );
    }
  }
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
  {
    FLDEFF_CTRL *fectrl = mmdl_GetFldEffCtrl( mmdl );
    FLDEFF_KEMURI_SetRailMMdl( mmdl, fectrl );
  }
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
  if( prm->reflect_type == MMDL_REFLECT_NON ){
    return;
  }
  
  if( MMDL_CheckMoveBitReflect(mmdl) == FALSE )
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
      
        MMDL_SetMoveBitReflect( mmdl, TRUE );
      
        type = REFLECT_TYPE_POND; //�{���̓A�g���r���[�g���ʂ��ă^�C�v����
        FLDEFF_REFLECT_SetMMdl( mmdlsys, mmdl, fectrl, type );
      }
    }
  }
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
  
	if( prm->reflect_type == MMDL_REFLECT_NON ||
		MMDL_CheckMoveBitReflect(mmdl) == FALSE ){
		return;
  }

  {
    MAPATTR_FLAG flag = MAPATTR_FLAGBIT_NONE;
		MAPATTR attr = MMDL_GetMapDirAttr( mmdl, DIR_DOWN );
    
    #ifdef DEBUG_REFLECT_CHECK
    attr = MAPATTR_FLAGBIT_REFLECT << 16;
    #endif
    
    if( attr == MAPATTR_ERROR ){
		  MMDL_SetMoveBitReflect( mmdl, FALSE );
    }else{
      flag = MAPATTR_GetAttrFlag( attr );

      if( (flag&MAPATTR_FLAGBIT_REFLECT) == 0 ){
		    MMDL_SetMoveBitReflect( mmdl, FALSE );
      }
    }
  }
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
