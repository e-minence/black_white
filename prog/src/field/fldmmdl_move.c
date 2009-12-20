//======================================================================
/**
 *
 * @file  fieldobj_move.c
 * @brief  ���샂�f�� ����n
 * @author  kagaya
 * @date  05.07.25
 *
 */
//======================================================================
#include "fldmmdl.h"

#include "include/gamesystem/pm_season.h"

#include "fieldmap.h"

#include "fldeff_shadow.h"
#include "fldeff_kemuri.h"
#include "fldeff_grass.h"
#include "fldeff_footmark.h"
#include "fldeff_reflect.h"
#include "fldeff_ripple.h"
#include "fldeff_splash.h"
#include "fldeff_d06denki.h"

#include "sound/pm_sndsys.h"

//======================================================================
//  define
//======================================================================
//--------------------------------------------------------------
//  debug
//--------------------------------------------------------------
#ifdef DEBUG_ONLY_FOR_kagaya
//#define DEBUG_REFLECT_CHECK //��`�ŉf�荞�݃`�F�b�N
#endif

//--------------------------------------------------------------
///  �X�e�[�^�X�r�b�g
//--------------------------------------------------------------
///�ړ�������֎~����X�e�[�^�X�r�b�g
#define STA_BIT_MOVE_ERROR (MMDL_STABIT_HEIGHT_GET_ERROR|MMDL_STABIT_ATTR_GET_ERROR)

//--------------------------------------------------------------
///  �A�g���r���[�g�I�t�Z�b�g
//--------------------------------------------------------------
#define ATTROFFS_Y_NUMA (NUM_FX32(-12))///<��Y�I�t�Z�b�g
#define ATTROFFS_Y_NUMA_DEEP (NUM_FX32(-14))///<�[����Y�I�t�Z�b�g
#define ATTROFFS_Y_YUKI (NUM_FX32(-12))///<��Y�I�t�Z�b�g
#define ATTROFFS_Y_YUKI_DEEP (NUM_FX32(-14))///<�[����Y�I�t�Z�b�g
#define ATTROFFS_Y_YUKI_DEEP_MOST (NUM_FX32(-16))///<�X�ɐ[����Y�I�t�Z�b�g
#define ATTROFFS_Y_DESERT_DEEP (NUM_FX32(-6)) ///<�[������Y�I�t�Z�b�g

///���������ɂ��ړ�����
#define HEIGHT_DIFF_COLLISION (FX32_ONE*(20))


//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
/// ATTRDATA
//--------------------------------------------------------------
typedef struct
{
  MAPATTR attr_old;
  MAPATTR attr_now;
  MAPATTR_VALUE attr_val_old;
  MAPATTR_VALUE attr_val_now;
  MAPATTR_FLAG attr_flag_old;
  MAPATTR_FLAG attr_flag_now;
  
  const OBJCODE_PARAM *objcode_prm;
  FLDEFF_CTRL *fectrl;
  
  u8 season;
  u8 padding[3];
}ATTRDATA;

//======================================================================
//  proto
//======================================================================
static int MMdl_CheckMoveStart( const MMDL * mmdl );
static void MMdl_GetHeightMoveBefore( MMDL * mmdl );
static void MMdl_GetAttrMoveBefore( MMDL * mmdl );
static void MMdl_ProcMoveStartFirst( MMDL * mmdl );
static void MMdl_ProcMoveStartSecond( MMDL * mmdl );
static void MMdl_ProcMoveEnd( MMDL * mmdl );

static void mmdl_InitAttrData( const MMDL *mmdl, ATTRDATA *data );

static void MMdl_MapAttrProc_MoveStartFirst( MMDL * mmdl );
static void MMdl_MapAttrProc_MoveStartSecond( MMDL * mmdl );
static void MMdl_MapAttrProc_MoveStartJumpSecond( MMDL * mmdl );
static void MMdl_MapAttrProc_MoveEnd( MMDL * mmdl );
static void MMdl_MapAttrProc_MoveEndJump( MMDL * mmdl );

static void MMdl_MapAttrHeight_02( MMDL *mmdl, ATTRDATA *data );

static void MMdl_MapAttrGrassProc_0( MMDL *mmdl, ATTRDATA *data );
static void MMdl_MapAttrGrassProc_12( MMDL *mmdl, ATTRDATA *data );

static void MMdl_MapAttrFootMarkProc_1( MMDL *mmdl, ATTRDATA *data );

static void MMdl_MapAttrSplashProc_0( MMDL *mmdl, ATTRDATA *data );
static void MMdl_MapAttrSplashProc_12( MMDL *mmdl, ATTRDATA *data );
static void MMdl_MapAttrSplashProc_Jump1( MMDL *mmdl, ATTRDATA *data );

static void MMdl_MapAttrShadowProc_0( MMDL *mmdl, ATTRDATA *data );
static void MMdl_MapAttrShadowProc_1( MMDL *mmdl, ATTRDATA *data );
static void MMdl_MapAttrShadowProc_2( MMDL *mmdl, ATTRDATA *data );

static void MMdl_MapAttrGroundSmokeProc_2( MMDL *mmdl, ATTRDATA *data );

static void MMdl_MapAttrLGrassProc_0( MMDL *mmdl, ATTRDATA *data );
static void MMdl_MapAttrLGrassProc_1( MMDL *mmdl, ATTRDATA *data );

static void MMdl_MapAttrNGrassProc_0( MMDL *mmdl, ATTRDATA *data );
static void MMdl_MapAttrNGrassProc_1( MMDL *mmdl, ATTRDATA *data );

static void MMdl_MapAttrPoolProc_1( MMDL *mmdl, ATTRDATA *data );
static void MMdl_MapAttrPoolProc_2( MMDL *mmdl, ATTRDATA *data );

static void MMdl_MapAttrSwampProc_1( MMDL *mmdl, ATTRDATA *data );
static void MMdl_MapAttrSwampProc_2( MMDL *mmdl, ATTRDATA *data );

static void MMdl_MapAttrReflect_01( MMDL *mmdl, ATTRDATA *data );
static void MMdl_MapAttrReflect_2( MMDL *mmdl, ATTRDATA *data );

static void MMdl_MapAttrBridgeProc_01( MMDL *mmdl, ATTRDATA *data );

static void MMdl_MapAttrBiriBiri_2( MMDL *mmdl, ATTRDATA *data );

static void MMdl_MapAttrSEProc_1( MMDL *mmdl, ATTRDATA *data );

static BOOL MMdl_HitCheckMoveAttr(
  const MMDL * mmdl, const VecFx32 pos );

#ifndef MMDL_PL_NULL
static BOOL (* const DATA_HitCheckAttr_Now[DIR_MAX4])( MAPATTR attr );
static BOOL (* const DATA_HitCheckAttr_Next[DIR_MAX4])( MAPATTR attr );
#endif

static BOOL MMdl_GetMapGridInfo(
  const MMDL *mmdl, const VecFx32 *pos,
  FLDMAPPER_GRIDINFO *pGridInfo );

//======================================================================
//  �t�B�[���h���샂�f�� ����
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f�� ���쏉����
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_InitMoveProc( MMDL * mmdl )
{
  MMDL_CallMoveInitProc( mmdl );
  MMDL_MoveSubProcInit( mmdl );
  MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_MOVEPROC_INIT );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f������
 * @param  mmdl    MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_UpdateMove( MMDL * mmdl )
{
  if( MMDL_CheckMMdlSysStatusBit(
    mmdl,MMDLSYS_STABIT_MOVE_PROC_STOP) ){
    return;
  }
  
  MMdl_GetHeightMoveBefore( mmdl );
  MMdl_GetAttrMoveBefore( mmdl );
  MMdl_ProcMoveStartFirst( mmdl );
  
  if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_ACMD) ){
    MMDL_ActionAcmd( mmdl );
  }else if( MMDL_CheckStatusBitMoveProcPause(mmdl) == FALSE ){
    if( MMdl_CheckMoveStart(mmdl) == TRUE ){
      if( MMDL_MoveSub(mmdl) == FALSE ){
        MMDL_CallMoveProc( mmdl );
      }
    }
  }
  
  MMdl_ProcMoveStartSecond( mmdl );
  MMdl_ProcMoveEnd( mmdl );
}

//--------------------------------------------------------------
/**
 * ����\���ǂ����`�F�b�N
 * @param  mmdl  MMDL *
 * @retval  int    TRUE=�\,FALSE=�s��
 */
//--------------------------------------------------------------
static int MMdl_CheckMoveStart( const MMDL * mmdl )
{
  if( MMDL_CheckStatusBitMove(mmdl) == TRUE ){
    return( TRUE );
  }
  
  if( MMDL_CheckStatusBit(mmdl,STA_BIT_MOVE_ERROR) == 0 ){
    return( TRUE );
  }else if( MMDL_GetMoveCode(mmdl) == MV_TR_PAIR ){ //�e�̍s���ɏ]��
    return( TRUE );
  }
  
  #ifndef MMDL_PL_NULL
  {  //�ړ��֎~�t���O���E�`�F�b�N
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
  
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * �ړ��J�n���ɍs�������擾
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_GetHeightMoveBefore( MMDL * mmdl )
{
  if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_HEIGHT_GET_ERROR) ){
    MMDL_UpdateCurrentHeight( mmdl );
  }
}

//--------------------------------------------------------------
/**
 * �ړ��J�n���ɍs���A�g���r���[�g�擾
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_GetAttrMoveBefore( MMDL * mmdl )
{
  if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_ATTR_GET_ERROR) ){
    if( MMDL_UpdateCurrentMapAttr(mmdl) == TRUE ){
      MMDL_OnStatusBitMoveStart( mmdl );
    }
  }
}

//--------------------------------------------------------------
/**
 * �ړ��J�n�Ŕ����@1st
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_ProcMoveStartFirst( MMDL * mmdl )
{
  if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_MOVE_START) ){
    MMdl_MapAttrProc_MoveStartFirst( mmdl );
  }
  
  MMDL_OffStatusBit( mmdl,
    MMDL_STABIT_MOVE_START | MMDL_STABIT_JUMP_START );
}

//--------------------------------------------------------------
/**
 * �ړ��J�n�Ŕ����@2nd
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
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

//--------------------------------------------------------------
/**
 * ����I���Ŕ���
 * @param  mmdl  MMDL *GX_WNDMASK_NONE
 * @retval  nothing
 */
//--------------------------------------------------------------
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

//======================================================================
//  �}�b�v�A�g���r���[�g
//======================================================================
//--------------------------------------------------------------
/**
 * �A�g���r���[�g�f�[�^������
 * @param mmdl MMDL
 * @param data ATTRDATA
 * @retval nothing
 */
//--------------------------------------------------------------
static void mmdl_InitAttrData( const MMDL *mmdl, ATTRDATA *data )
{
  data->attr_old = MMDL_GetMapAttrOld( mmdl );
  data->attr_now = MMDL_GetMapAttr( mmdl );
  data->attr_val_old = MAPATTR_GetAttrValue( data->attr_old );
  data->attr_val_now = MAPATTR_GetAttrValue( data->attr_now );
  data->attr_flag_old = MAPATTR_GetAttrFlag( data->attr_old );
  data->attr_flag_now = MAPATTR_GetAttrFlag( data->attr_now );
  
  data->objcode_prm =
      MMDL_GetOBJCodeParam( mmdl, MMDL_GetOBJCode(mmdl) );
   
  {
    MMDLSYS *mmdlsys = MMDL_GetMMdlSys( mmdl );
    FIELDMAP_WORK *fieldmap = MMDLSYS_GetFieldMapWork( mmdlsys );
    data->fectrl = FIELDMAP_GetFldEffCtrl( fieldmap );
    data->season = GAMEDATA_GetSeasonID( MMDLSYS_GetGameData(mmdlsys) );
  }
}

//--------------------------------------------------------------
/**
 * ����J�n�Ŕ�������A�g���r���[�g���� 1st
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrProc_MoveStartFirst( MMDL * mmdl )
{
  MMDL_UpdateCurrentMapAttr( mmdl );
  
  if( MMDL_CheckCompletedDrawInit(mmdl) == TRUE ){
    ATTRDATA data;
    mmdl_InitAttrData( mmdl, &data );
    
    MMdl_MapAttrBridgeProc_01( mmdl, &data );
    MMdl_MapAttrGrassProc_0( mmdl, &data );
    MMdl_MapAttrSplashProc_0( mmdl, &data );
    MMdl_MapAttrShadowProc_0( mmdl, &data );
    MMdl_MapAttrHeight_02( mmdl, &data );
    MMdl_MapAttrLGrassProc_0( mmdl, &data );
    MMdl_MapAttrNGrassProc_0( mmdl, &data );
    MMdl_MapAttrReflect_01( mmdl, &data );
  }
}

//--------------------------------------------------------------
/**
 * ����J�n�Ŕ�������A�g���r���[�g���� 2nd
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrProc_MoveStartSecond( MMDL * mmdl )
{
  MMDL_UpdateCurrentMapAttr( mmdl );
  
  if( MMDL_CheckCompletedDrawInit(mmdl) == TRUE ){
    ATTRDATA data;
    mmdl_InitAttrData( mmdl, &data );
    
    MMdl_MapAttrBridgeProc_01( mmdl, &data );
    MMdl_MapAttrGrassProc_12( mmdl, &data );
    MMdl_MapAttrFootMarkProc_1( mmdl, &data );
    MMdl_MapAttrSplashProc_12( mmdl, &data );
    MMdl_MapAttrShadowProc_1( mmdl, &data );
    MMdl_MapAttrLGrassProc_1( mmdl, &data );
    MMdl_MapAttrNGrassProc_1( mmdl, &data );
    MMdl_MapAttrPoolProc_1( mmdl, &data );
    MMdl_MapAttrSwampProc_1( mmdl, &data );
    MMdl_MapAttrReflect_01( mmdl, &data );
    
    MMdl_MapAttrSEProc_1( mmdl, &data ); //�`��֌W�Ȃ�?
  }
}

//--------------------------------------------------------------
/**
 * ����J�n�Ŕ�������A�g���r���[�g���� 2nd Jump
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrProc_MoveStartJumpSecond( MMDL * mmdl )
{
  MMDL_UpdateCurrentMapAttr( mmdl );
  
  if( MMDL_CheckCompletedDrawInit(mmdl) == TRUE ){
    ATTRDATA data;
    mmdl_InitAttrData( mmdl, &data );
    
    MMdl_MapAttrBridgeProc_01( mmdl, &data );
    MMdl_MapAttrShadowProc_1( mmdl, &data );
    MMdl_MapAttrReflect_01( mmdl, &data );
    MMdl_MapAttrSplashProc_Jump1( mmdl, &data );

    MMdl_MapAttrSEProc_1( mmdl, &data );//�`��֌W�Ȃ�?
  }
}

//--------------------------------------------------------------
/**
 * ����I���Ŕ�������A�g���r���[�g�����@end
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrProc_MoveEnd( MMDL * mmdl )
{
  MMDL_UpdateCurrentMapAttr( mmdl );
  
  if( MMDL_CheckCompletedDrawInit(mmdl) == TRUE ){
    ATTRDATA data;
    mmdl_InitAttrData( mmdl, &data );
    
    //�I���@�A�g���r���[�g����
    MMdl_MapAttrHeight_02( mmdl, &data );
    MMdl_MapAttrPoolProc_2( mmdl, &data );
    MMdl_MapAttrSwampProc_2( mmdl, &data );
    MMdl_MapAttrSplashProc_12( mmdl, &data );
    MMdl_MapAttrReflect_2( mmdl, &data );
    MMdl_MapAttrShadowProc_2( mmdl, &data );
    MMdl_MapAttrBiriBiri_2( mmdl, &data );
  }
}

//--------------------------------------------------------------
/**
 * ����I���Ŕ�������A�g���r���[�g�����@end jump
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrProc_MoveEndJump( MMDL * mmdl )
{
  MMDL_UpdateCurrentMapAttr( mmdl );
  
  if( MMDL_CheckCompletedDrawInit(mmdl) == TRUE ){
    ATTRDATA data;
    mmdl_InitAttrData( mmdl, &data );
    
    //�I���@�A�g���r���[�g����
    MMdl_MapAttrHeight_02( mmdl, &data );
    MMdl_MapAttrPoolProc_2( mmdl, &data );
    MMdl_MapAttrSwampProc_2( mmdl, &data );
    MMdl_MapAttrSplashProc_12( mmdl, &data );
    MMdl_MapAttrReflect_2( mmdl, &data );
    MMdl_MapAttrShadowProc_2( mmdl, &data );
    MMdl_MapAttrGrassProc_12( mmdl, &data );
    MMdl_MapAttrGroundSmokeProc_2( mmdl, &data );
    MMdl_MapAttrBiriBiri_2( mmdl, &data );
  }
}

//======================================================================
//  �A�g���r���[�g�ύ���
//======================================================================
//--------------------------------------------------------------
/**
 * �A�g���r���[�g�ύ����@����J�n�A����I��
 * @param  mmdl    MMDL *
 * @param  now      ���݂̃A�g���r���[�g
 * @param  old      �ߋ��̃A�g���r���[�g
 * @param  prm    OBJCODE_PARAM
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrHeight_02( MMDL *mmdl, ATTRDATA *data )
{
  if( MMDL_CheckStatusBitAttrOffsetOFF(mmdl) == FALSE ){
    if( MAPATTR_VALUE_CheckDesertDeep(data->attr_val_now) ){
      VecFx32 offs = { 0, ATTROFFS_Y_DESERT_DEEP, 0 };
      MMDL_SetVectorAttrDrawOffsetPos( mmdl, &offs );
      return;
    }
  }
  
  {
    VecFx32 vec = { 0, 0, 0 };
    MMDL_SetVectorAttrDrawOffsetPos( mmdl, &vec );
  }
}

#ifndef MMDL_PL_NULL
static void MMdl_MapAttrHeight_02(
    MMDL * mmdl, MAPATTR now, MAPATTR old, const OBJCODE_PARAM *prm )
{
  if( MMDL_CheckStatusBitAttrOffsetOFF(mmdl) == FALSE ){
    if( MAPATTR_IsSwampDeep(now) == TRUE ||
      MAPATTR_IsSwampGrassDeep(now) == TRUE ){
      VecFx32 vec = { 0, ATTROFFS_Y_NUMA_DEEP, 0 };
      MMDL_SetVectorAttrDrawOffsetPos( mmdl, &vec );
      return;
    }
    
    if( MAPATTR_IsSwamp(now) == TRUE || MAPATTR_IsSwampGrass(now) == TRUE ){
      VecFx32 vec = { 0, ATTROFFS_Y_NUMA, 0 };
      MMDL_SetVectorAttrDrawOffsetPos( mmdl, &vec );
      return;
    }
    
    if( MAPATTR_IsSnowDeepMost(now) == TRUE ){
      VecFx32 vec = { 0, ATTROFFS_Y_YUKI_DEEP_MOST, 0 };
      MMDL_SetVectorAttrDrawOffsetPos( mmdl, &vec );
      return;
    }
    
    if( MAPATTR_IsSnowDeep(now) == TRUE ){
      VecFx32 vec = { 0, ATTROFFS_Y_YUKI_DEEP, 0 };
      MMDL_SetVectorAttrDrawOffsetPos( mmdl, &vec );
      return;
    }
    
    if( MAPATTR_IsShallowSnow(now) == TRUE ){
      VecFx32 vec = { 0, ATTROFFS_Y_YUKI, 0 };
      MMDL_SetVectorAttrDrawOffsetPos( mmdl, &vec );
      return;
    }
  }
  
  {
    VecFx32 vec = { 0, 0, 0 };
    MMDL_SetVectorAttrDrawOffsetPos( mmdl, &vec );
  }
}
#endif

//======================================================================
//  �A�g���r���[�g�@��
//======================================================================
//--------------------------------------------------------------
/**
 * ���@����J�n 0
 * @param  mmdl    MMDL *
 * @param  now      ���݂̃A�g���r���[�g
 * @param  old      �ߋ��̃A�g���r���[�g
 * @param  prm    OBJCODE_PARAM
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrGrassProc_0( MMDL *mmdl, ATTRDATA *data )
{
  if( (data->attr_flag_now & MAPATTR_FLAGBIT_GRASS) ){
    if( MAPATTR_VALUE_CheckLongGrass(data->attr_val_now) ){
      FLDEFF_GRASS_SetMMdl( data->fectrl, mmdl, FALSE, FLDEFF_GRASS_LONG );
    }else if( MAPATTR_VALUE_CheckSnowGrass(data->attr_val_now) ){
      FLDEFF_GRASS_SetMMdl( data->fectrl, mmdl, FALSE, FLDEFF_GRASS_SNOW );
    }else{
      FLDEFF_GRASS_SetMMdl( data->fectrl, mmdl, FALSE, FLDEFF_GRASS_SHORT );
    }
  }
}

//--------------------------------------------------------------
/**
 * ���@���� 1,2
 * @param  mmdl  MMDL *
 * @param  now      ���݂̃A�g���r���[�g
 * @param  old      �ߋ��̃A�g���r���[�g
 * @param  prm    OBJCODE_PARAM
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrGrassProc_12( MMDL *mmdl, ATTRDATA *data )
{
  if( data->attr_now != MAPATTR_ERROR &&
      (data->attr_flag_now & MAPATTR_FLAGBIT_GRASS) )
  {
    if( MAPATTR_VALUE_CheckLongGrass(data->attr_val_now) ){
      FLDEFF_GRASS_SetMMdl( data->fectrl, mmdl, TRUE, FLDEFF_GRASS_LONG );
    }else if( MAPATTR_VALUE_CheckSnowGrass(data->attr_val_now) ){
      FLDEFF_GRASS_SetMMdl( data->fectrl, mmdl, TRUE, FLDEFF_GRASS_SNOW );
    }else{
      FLDEFF_GRASS_SetMMdl( data->fectrl, mmdl, TRUE, FLDEFF_GRASS_SHORT );
    }
  }
}

//======================================================================
//  �A�g���r���[�g�@����
//======================================================================
//--------------------------------------------------------------
/**
 * ���Ձ@����J�n 1
 * @param  mmdl  MMDL *
 * @param  now    ���݂̃A�g���r���[�g
 * @param  old    �ߋ��̃A�g���r���[�g
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrFootMarkProc_1( MMDL *mmdl, ATTRDATA *data )
{
  if( data->attr_old != MAPATTR_ERROR &&
      (data->attr_flag_old & MAPATTR_FLAGBIT_FOOTMARK) )
  {
    FOOTMARK_TYPE type;
    
    if( MAPATTR_VALUE_CheckSnowType(data->attr_val_old) == TRUE ||
        (data->season == PMSEASON_WINTER &&
          (MAPATTR_VALUE_CheckSeasonGround1(data->attr_val_old) ||
          MAPATTR_VALUE_CheckSeasonGround2(data->attr_val_old))) )
    {
      type = FOOTMARK_TYPE_HUMAN_SNOW;
      
      if( data->objcode_prm->footmark_type == MMDL_FOOTMARK_CYCLE )
      {
        type = FOOTMARK_TYPE_CYCLE_SNOW;
      }
    }
    else if( MAPATTR_VALUE_CheckDesertDeep(data->attr_val_old) == TRUE )
    {
      type = FOOTMARK_TYPE_DEEPSNOW;       
      PMSND_PlaySE( SEQ_SE_FLD_91 );
    }
    else
    {
      type = FOOTMARK_TYPE_HUMAN;
      
      if( data->objcode_prm->footmark_type == MMDL_FOOTMARK_CYCLE )
      {
        type = FOOTMARK_TYPE_CYCLE;
      }
    }
     
    FLDEFF_FOOTMARK_SetMMdl( mmdl, data->fectrl, type );
  }
}

//======================================================================
//  �A�g���r���[�g�@����
//======================================================================
//--------------------------------------------------------------
/**
 * ���򖗁@����J�n 0
 * @param  mmdl  MMDL *
 * @param  now    ���݂̃A�g���r���[�g
 * @param  old    �ߋ��̃A�g���r���[�g
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrSplashProc_0( MMDL *mmdl, ATTRDATA *data )
{
  if( MAPATTR_VALUE_CheckShoal(data->attr_val_now) == TRUE ){
    if( MMDL_CheckStatusBitShoalEffect(mmdl) == FALSE ){
      FLDEFF_SPLASH_SetMMdl( data->fectrl, mmdl, TRUE );
      MMDL_SetStatusBitShoalEffect( mmdl, TRUE );
    }
  }else{
    MMDL_SetStatusBitShoalEffect( mmdl, FALSE );
  }
}

//--------------------------------------------------------------
/**
 * ���򖗁@����J�n�I�� 12
 * @param  mmdl  MMDL *
 * @param  now    ���݂̃A�g���r���[�g
 * @param  old    �ߋ��̃A�g���r���[�g
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrSplashProc_12( MMDL *mmdl, ATTRDATA *data )
{
  if( MAPATTR_VALUE_CheckShoal(data->attr_val_now) == TRUE ){
    if( MMDL_CheckStatusBitShoalEffect(mmdl) == FALSE ){
      FLDEFF_SPLASH_SetMMdl( data->fectrl, mmdl, TRUE );
      MMDL_SetStatusBitShoalEffect( mmdl, TRUE );
    }
  }else if( MAPATTR_VALUE_CheckMarsh(data->attr_val_now) == TRUE ){
    FLDEFF_SPLASH_SetMMdl( data->fectrl, mmdl, FALSE );
  }else{
    MMDL_SetStatusBitShoalEffect( mmdl, FALSE );
  }
}

//--------------------------------------------------------------
/**
 * ���򖗁@�W�����v����J�n 1
 * @param  mmdl  MMDL *
 * @param  now    ���݂̃A�g���r���[�g
 * @param  old    �ߋ��̃A�g���r���[�g
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrSplashProc_Jump1( MMDL *mmdl, ATTRDATA *data )
{
  MMDL_SetStatusBitShoalEffect( mmdl, FALSE );
}

//======================================================================
//  �A�g���r���[�g�@�e
//======================================================================
//--------------------------------------------------------------
/**
 * �e�@����J�n 0
 * @param  mmdl  MMDL *
 * @param  now    ���݂̃A�g���r���[�g
 * @param  old    �ߋ��̃A�g���r���[�g
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrShadowProc_0( MMDL *mmdl, ATTRDATA *data )
{
  MMdl_MapAttrShadowProc_1( mmdl, data );
}

//--------------------------------------------------------------
/**
 * �e�@����J�n 1
 * @param  mmdl  MMDL *
 * @param  now    ���݂̃A�g���r���[�g
 * @param  old    �ߋ��̃A�g���r���[�g
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrShadowProc_1( MMDL *mmdl, ATTRDATA *data )
{
  const MMDLSYS *fos = MMDL_GetMMdlSys( mmdl );
    
  if( MMDLSYS_CheckJoinShadow(fos) == FALSE ){
    return;
  }

  if( (data->attr_flag_now & MAPATTR_FLAGBIT_SHADOW) &&
      data->objcode_prm->shadow_type == MMDL_SHADOW_ON ){
    if( MMDL_CheckStatusBit(mmdl,MMDL_STABIT_SHADOW_SET) == 0 ){
      FLDEFF_SHADOW_SetMMdl( mmdl, data->fectrl );
      MMDL_OnStatusBit( mmdl, MMDL_STABIT_SHADOW_SET );
    }
  }
}

//--------------------------------------------------------------
/**
 * �e�@����I�� 2
 * @param  mmdl  MMDL *
 * @param  now    ���݂̃A�g���r���[�g
 * @param  old    �ߋ��̃A�g���r���[�g
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrShadowProc_2( MMDL *mmdl, ATTRDATA *data )
{
  const MMDLSYS *fos = MMDL_GetMMdlSys( mmdl );
    
  if( MMDLSYS_CheckJoinShadow(fos) == FALSE ){
    return;
  }
  
  if( data->objcode_prm->shadow_type == MMDL_SHADOW_NON ){
    return;
  }
  
  if( (data->attr_flag_now & MAPATTR_FLAGBIT_SHADOW) == 0 ){
    MMDL_OnStatusBit( mmdl, MMDL_STABIT_SHADOW_VANISH );
  }else{
    MMDL_OffStatusBit( mmdl, MMDL_STABIT_SHADOW_VANISH );
  }
}

//======================================================================
//  �A�g���r���[�g�@�y��
//======================================================================
//--------------------------------------------------------------
/**
 * �y���@����I�� 2 
 * @param  mmdl  MMDL *
 * @param  now    ���݂̃A�g���r���[�g
 * @param  old    �ߋ��̃A�g���r���[�g
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrGroundSmokeProc_2( MMDL *mmdl, ATTRDATA *data )
{
  FLDEFF_KEMURI_SetMMdl( mmdl, data->fectrl );
}

//======================================================================
//  �A�g���r���[�g�@������
//======================================================================
//--------------------------------------------------------------
/**
 * �������@����J�n 0
 * @param  mmdl    MMDL *
 * @param  now      ���݂̃A�g���r���[�g
 * @param  old      �ߋ��̃A�g���r���[�g
 * @param  prm    OBJCODE_PARAM
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrLGrassProc_0( MMDL *mmdl, ATTRDATA *data )
{
  #ifndef MMDL_PL_NULL
  if( MAPATTR_IsLongGrass(now) == TRUE ){
    FE_mmdlLGrass_Add( mmdl, FALSE );
  }
  #endif
}

//--------------------------------------------------------------
/**
 * �������@����J�n 1
 * @param  mmdl  MMDL *
 * @param  now      ���݂̃A�g���r���[�g
 * @param  old      �ߋ��̃A�g���r���[�g
 * @param  prm    OBJCODE_PARAM
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrLGrassProc_1( MMDL *mmdl, ATTRDATA *data )
{
  #ifndef MMDL_PL_NULL
  if( MAPATTR_IsLongGrass(now) == TRUE ){
    FE_mmdlLGrass_Add( mmdl, TRUE );
  }
  #endif
}

//======================================================================
//  �A�g���r���[�g�@����
//======================================================================
//--------------------------------------------------------------
/**
 * �����@����J�n 0
 * @param  mmdl    MMDL *
 * @param  now      ���݂̃A�g���r���[�g
 * @param  old      �ߋ��̃A�g���r���[�g
 * @param  prm    OBJCODE_PARAM
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrNGrassProc_0( MMDL *mmdl, ATTRDATA *data )
{
  #ifndef MMDL_PL_NULL
  if( MAPATTR_IsSwampGrass(now) == TRUE ){
    FE_mmdlNGrass_Add( mmdl, FALSE );
  }
  #endif
}

//--------------------------------------------------------------
/**
 * �����@����J�n 1
 * @param  mmdl  MMDL *
 * @param  now      ���݂̃A�g���r���[�g
 * @param  old      �ߋ��̃A�g���r���[�g
 * @param  prm    OBJCODE_PARAM
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrNGrassProc_1( MMDL *mmdl, ATTRDATA *data )
{
  #ifndef MMDL_PL_NULL
  if( MAPATTR_IsSwampGrass(now) == TRUE ){
    FE_mmdlNGrass_Add( mmdl, TRUE );
  }
  #endif
}

//======================================================================
//  �A�g���r���[�g�@�����܂�
//======================================================================
//--------------------------------------------------------------
/**
 * �����܂�@����J�n 1
 * @param  mmdl  MMDL *
 * @param  now      ���݂̃A�g���r���[�g
 * @param  old      �ߋ��̃A�g���r���[�g
 * @param  prm    OBJCODE_PARAM
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrPoolProc_1( MMDL *mmdl, ATTRDATA *data )
{
  if( MAPATTR_VALUE_CheckPool(data->attr_val_old) == TRUE ){
    FLDEFF_RIPPLE_Set( data->fectrl,
      MMDL_GetOldGridPosX(mmdl),
      MMDL_GetOldGridPosZ(mmdl),
      MMDL_GetVectorPosY(mmdl) );
  }
}

//--------------------------------------------------------------
/**
 * �����܂�@����I�� 2
 * @param  mmdl  MMDL *
 * @param  now      ���݂̃A�g���r���[�g
 * @param  old      �ߋ��̃A�g���r���[�g
 * @param  prm    OBJCODE_PARAM
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrPoolProc_2( MMDL *mmdl, ATTRDATA *data )
{
  if( MAPATTR_VALUE_CheckPool(data->attr_val_now) == TRUE ){
    FLDEFF_RIPPLE_Set( data->fectrl,
      MMDL_GetGridPosX(mmdl),
      MMDL_GetGridPosZ(mmdl),
      MMDL_GetVectorPosY(mmdl) );
  }
}

//======================================================================
//  �A�g���r���[�g�@��
//======================================================================
//--------------------------------------------------------------
/**
 * ���@����J�n 1
 * @param  mmdl  MMDL *
 * @param  now      ���݂̃A�g���r���[�g
 * @param  old      �ߋ��̃A�g���r���[�g
 * @param  prm    OBJCODE_PARAM
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrSwampProc_1( MMDL *mmdl, ATTRDATA *data )
{
  #ifndef MMDL_PL_NULL
  if( MAPATTR_IsSwamp(old) == TRUE ){
    FE_mmdlNRippleSet( mmdl,
      MMDL_GetOldGridPosX(mmdl),
      MMDL_GetOldGridPosY(mmdl), 
      MMDL_GetOldGridPosZ(mmdl) );
  }
  #endif
}

//--------------------------------------------------------------
/**
 * ���@����I�� 2
 * @param  mmdl  MMDL *
 * @param  now      ���݂̃A�g���r���[�g
 * @param  old      �ߋ��̃A�g���r���[�g
 * @param  prm    OBJCODE_PARAM
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrSwampProc_2( MMDL *mmdl, ATTRDATA *data )
{
  #ifndef MMDL_PL_NULL
  if( MAPATTR_IsSwamp(now) == TRUE ){
    FE_mmdlNRippleSet( mmdl,
      MMDL_GetGridPosX(mmdl),
      MMDL_GetGridPosY(mmdl), 
      MMDL_GetGridPosZ(mmdl) );
  }
  #endif
}

//======================================================================
//  �A�g���r���[�g�@�f�肱��
//======================================================================
//--------------------------------------------------------------
/**
 * �f�肱�݁@����J�n 01
 * @param  mmdl  MMDL *
 * @param  now      ���݂̃A�g���r���[�g
 * @param  old      �ߋ��̃A�g���r���[�g
 * @param  prm    OBJCODE_PARAM
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrReflect_01( MMDL *mmdl, ATTRDATA *data )
{
  if( data->objcode_prm->reflect_type == MMDL_REFLECT_NON ){
    return;
  }
  
  if( MMDL_CheckStatusBitReflect(mmdl) == FALSE )
  {
    MAPATTR_FLAG flag = MAPATTR_FLAGBIT_NONE;
    MAPATTR hit_attr = MAPATTR_ERROR;
    
    #ifdef DEBUG_REFLECT_CHECK
    now = MAPATTR_FLAGBIT_REFLECT << 16;
    #endif
    
    if( data->attr_now != MAPATTR_ERROR )
    {
      if( (data->attr_flag_now & MAPATTR_FLAGBIT_REFLECT) )
      {
        hit_attr = data->attr_now;
        #ifdef DEBUG_REFLECT_CHECK
        hit_attr = 0;
        #endif
      }
      else
      {
        MAPATTR next = MMDL_GetMapDirAttr( mmdl, DIR_DOWN );
        flag = MAPATTR_GetAttrFlag( next );
      
        if( (flag & MAPATTR_FLAGBIT_REFLECT) )
        {
          hit_attr = next;
        }
      }
      
      if( hit_attr != MAPATTR_ERROR )
      {
        REFLECT_TYPE type;
        MMDLSYS *mmdlsys = (MMDLSYS*)MMDL_GetMMdlSys( mmdl );
        
        MMDL_SetStatusBitReflect( mmdl, TRUE );
         
        type = REFLECT_TYPE_POND; //�{���̓A�g���r���[�g���ʂ��ă^�C�v����
        FLDEFF_REFLECT_SetMMdl( mmdlsys, mmdl, data->fectrl, type );
      }
    }
  }
}

//--------------------------------------------------------------
/**
 * �f�肱�݁@����I�� 2
 * @param  mmdl  MMDL *
 * @param  now      ���݂̃A�g���r���[�g
 * @param  old      �ߋ��̃A�g���r���[�g
 * @param  prm    OBJCODE_PARAM
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrReflect_2( MMDL *mmdl, ATTRDATA *data )
{
  if( data->objcode_prm->reflect_type == MMDL_REFLECT_NON ||
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
      
      if( (flag & MAPATTR_FLAGBIT_REFLECT) == 0 ){
        MMDL_SetStatusBitReflect( mmdl, FALSE );
      }
    }
  }
}

//======================================================================
//  �A�g���r���[�g�@��
//======================================================================
//--------------------------------------------------------------
/**
 * ���@����J�n 0
 * @param  mmdl    MMDL *
 * @param  now      ���݂̃A�g���r���[�g
 * @param  old      �ߋ��̃A�g���r���[�g
 * @param  prm    OBJCODE_PARAM
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrBridgeProc_01( MMDL *mmdl, ATTRDATA *data )
{
  #ifndef MMDL_PL_NULL
  if( MAPATTR_IsBridgeFlag(now) == TRUE ){
    MMDL_SetStatusBitBridge( mmdl, TRUE );
  }else if( MMDL_CheckStatusBitBridge(mmdl) == TRUE ){
    if( MAPATTR_IsBridge(now) == FALSE ){
      MMDL_SetStatusBitBridge( mmdl, FALSE );
    }
  }
  #endif
}

//======================================================================
//  �A�g���r���[�g�@�r���r����
//======================================================================
//--------------------------------------------------------------
/**
 * �r���r�����@����I�� 2
 * @param  mmdl    MMDL *
 * @param  now      ���݂̃A�g���r���[�g
 * @param  old      �ߋ��̃A�g���r���[�g
 * @param  prm    OBJCODE_PARAM
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrBiriBiri_2( MMDL *mmdl, ATTRDATA *data )
{
  if( MAPATTR_VALUE_CheckElecFloor(data->attr_val_now) == TRUE ){
    FLDEFF_D06DENKI_BIRIBIRI_SetMMdl( mmdl, data->fectrl );
  }
}

//======================================================================
//  �A�g���r���[�g�֘ASE
//======================================================================
//--------------------------------------------------------------
/**
 * SE�@����J�n 1
 * @param  mmdl  MMDL *
 * @param  now    ���݂̃A�g���r���[�g
 * @param  old    �ߋ��̃A�g���r���[�g
 * @param  param    OBJCODE_PARAM
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_MapAttrSEProc_1( MMDL *mmdl, ATTRDATA *data )
{
  #ifndef MMDL_PL_NULL
  if( MAPATTR_IsSnow(now) ){
    Snd_SePlay( SEQ_SE_DP_YUKIASHI );
  }
  #endif
}

//======================================================================
//  �ړ��`�F�b�N
//======================================================================
//--------------------------------------------------------------
/**
 * �w����W��Q���`�F�b�N
 * @param  fos    MMDLSYS *
 * @param  x    �`�F�b�N����X���W  �O���b�h
 * @param  z    �`�F�b�N����Z���W  �O���b�h
 * @retval  u32    �q�b�g�r�b�g�BMMDL_MOVEHITBIT_LIM��
 */
//--------------------------------------------------------------
#ifndef MMDL_PL_NULL
u32 MMDL_PosHitCheck( const MMDLSYS *fos, int x, int z )
{
  u32 ret;
  
  ret = MMDL_MOVEHITBIT_NON;
  
  if(  GetHitAttr(MMDLSYS_FieldSysWorkGet(fos),x,z) == TRUE ){
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
 * @param  mmdl  MMDL *
 * @param  vec    ���ݎ����W
 * @param  x    �ړ���X���W  �O���b�h
 * @param  y    �ړ���Y���W
 * @param  z    �ړ���Z���W  �O���b�h
 * @param  dir    �ړ����� DIR_UP��
 * @retval  u32    �q�b�g�r�b�g�BMMDL_MOVEHITBIT_LIM��
 */
//--------------------------------------------------------------
u32 MMDL_HitCheckMove(
  const MMDL *mmdl, const VecFx32 *vec, s16 x, s16 y, s16 z, u16 dir )
{
  u32 ret;
  VecFx32 pos;
  
  ret = MMDL_MOVEHITBIT_NON;
  
  pos.x = GRID_SIZE_FX32( x ) + GRID_HALF_FX32;
  pos.y = vec->y; //GRID_SIZE_FX32( y );
  pos.z = GRID_SIZE_FX32( z ) + GRID_HALF_FX32;
    
  if( MMDL_HitCheckMoveLimit(mmdl,x,z) == TRUE ){
    ret |= MMDL_MOVEHITBIT_LIM;
  }
  
  {
    u32 attr;
    fx32 height;
    
    if( MMdl_HitCheckMoveAttr(mmdl,pos) == TRUE ){
      ret |= MMDL_MOVEHITBIT_ATTR;
    }

    if( MMDL_GetMapPosHeight(mmdl,&pos,&height) == TRUE ){
      fx32 diff = vec->y - height;
      if( diff < 0 ){ diff = -diff; }
      if( diff >= HEIGHT_DIFF_COLLISION ){
        ret |= MMDL_MOVEHITBIT_HEIGHT;
      }
    }else{
      ret |= MMDL_MOVEHITBIT_HEIGHT;
    }
  }
  
  if( MMDL_HitCheckMoveFellow(mmdl,x,y,z) == TRUE ){
    ret |= MMDL_MOVEHITBIT_OBJ;
  }
  
  {
    const FLDMAPPER *pG3DMapper;
    
    pG3DMapper = MMDLSYS_GetG3DMapper( MMDL_GetMMdlSys(mmdl) );
    
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
 * @param  mmdl  MMDL * 
 * @param  x    �ړ���X���W  �O���b�h
 * @param  y    �ړ���Y���W
 * @param  z    �ړ���Z���W  �O���b�h
 * @param  dir    �ړ����� DIR_UP��
 * @retval  u32    �q�b�g�r�b�g�BMMDL_MOVEHITBIT_LIM��
 */
//--------------------------------------------------------------
u32 MMDL_HitCheckMoveCurrent(
  const MMDL * mmdl, s16 x, s16 y, s16 z, u16 dir )
{
  VecFx32 vec;
  MMDL_GetVectorPos( mmdl, &vec );
  return( MMDL_HitCheckMove(mmdl,&vec,x,y,z,dir) );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f���ړ��`�F�b�N�@���݈ʒu+������
 * @param  mmdl  MMDL * 
 * @param  dir    �ړ���������BDIR_UP��
 * @retval  u32    �q�b�g�r�b�g�BMMDL_MOVEHITBIT_LIM��
 */
//--------------------------------------------------------------
u32 MMDL_HitCheckMoveDir( const MMDL * mmdl, u16 dir )
{
  s16 x,y,z;
  x = MMDL_GetGridPosX( mmdl ) + MMDL_TOOL_GetDirAddValueGridX( dir );
  y = MMDL_GetHeightGrid( mmdl );
  z = MMDL_GetGridPosZ( mmdl ) + MMDL_TOOL_GetDirAddValueGridZ( dir );
  return( MMDL_HitCheckMoveCurrent(mmdl,x,y,z,dir) );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���샂�f�����m�̏Փ˃`�F�b�N
 * @param  mmdl  MMDL * 
 * @param  x    �ړ���X���W  �O���b�h
 * @param  y    �ړ���X���W  �O���b�h
 * @param  z    �ړ���X���W  �O���b�h
 * @retval  BOOL  TRUE=�Փ˃A��
 */
//--------------------------------------------------------------
BOOL MMDL_HitCheckMoveFellow( const MMDL * mmdl, s16 x, s16 y, s16 z )
{
  u32 no=0;
  int hx,hz;
  MMDL *cmmdl;
  const MMDLSYS *sys;
  
  sys = MMDL_GetMMdlSys( mmdl );
  
  while( MMDLSYS_SearchUseMMdl(sys,&cmmdl,&no) == TRUE ){
    if( cmmdl != mmdl ){
      if( MMDL_CheckStatusBit(
        cmmdl,MMDL_STABIT_FELLOW_HIT_NON) == 0 ){
        hx = MMDL_GetGridPosX( cmmdl );
        hz = MMDL_GetGridPosZ( cmmdl );
        {
          BOOL debug = FALSE;
          if( MMDL_GetOBJID(mmdl) == MMDL_ID_PLAYER ){
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
 * @param  mmdl  MMDL * 
 * @param  x    �ړ���X���W  �O���b�h
 * @param  y    �ړ���Y���W  �O���b�h
 * @param  z    �ړ���Z���W  �O���b�h
 * @retval  BOOL  TRUE=�����z��
 */
//--------------------------------------------------------------
BOOL MMDL_HitCheckMoveLimit( const MMDL * mmdl, s16 x, s16 z )
{
  s16 init,limit,min,max;
  
  init = MMDL_GetInitGridPosX( mmdl );
  limit = MMDL_GetMoveLimitX( mmdl );
  
  if( limit != MOVE_LIMIT_NOT ){
    min = init - limit;
    max = init + limit;
  
    if( min > x || max < x ){
      return( TRUE );
    }
  }
  
  init = MMDL_GetInitGridPosZ( mmdl );
  limit = MMDL_GetMoveLimitZ( mmdl );
  
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
 * @param  mmdl  MMDL * 
 * @param  pos    �ړ���X���W,���݈ʒu��Y,�ړ���Z���W
 * @param  dir    �ړ����� DIR_UP��
 * @retval  int    TRUE=�ړ��s�A�g���r���[�g
 */
//--------------------------------------------------------------
static BOOL MMdl_HitCheckMoveAttr(
  const MMDL * mmdl, const VecFx32 pos )
{
  if( MMDL_CheckMoveBitAttrGetOFF(mmdl) == FALSE ){
    MAPATTR attr;
    
    if( MMDL_GetMapPosAttr(mmdl,&pos,&attr) == TRUE ){
      return MAPATTR_GetHitchFlag(attr);
    }
  }
  
  return( TRUE ); //�ړ��s�A�g���r���[�g
}

//--------------------------------------------------------------
///  ���݈ʒu�A�g���r���[�g���画�肷��ړ�����A�g���r���[�g�`�F�b�N�֐�
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
///  �����ʒu�A�g���r���[�g���画�肷��ړ�����A�g���r���[�g�`�F�b�N�֐�
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
 * @param  mmdl  MMDL *
 * @param  attr  �A�g���r���[�g
 * @retval  BOOL  TRUE=���A�g���r���[�g
 */
//--------------------------------------------------------------
static BOOL MMdl_CheckMapAttrKind_Water( MMDL * mmdl, u32 attr )
{
  #ifndef MMDL_PL_NULL
  if( MAPATTR_IsBridgeWater(attr) ){
    if( MMDL_CheckStatusBitBridge(mmdl) == FALSE ){
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
 * @param  mmdl  MMDL *
 * @param  attr  �A�g���r���[�g
 * @retval  int    TRUE=���A�g���r���[�g
 */
//--------------------------------------------------------------
static BOOL MMdl_CheckMapAttrKind_Sand( MMDL * mmdl, u32 attr )
{
  #ifndef MMDL_PL_NULL
  if( MAPATTR_IsBridgeSand(attr) ){
    if( MMDL_CheckStatusBitBridge(mmdl) == FALSE ){
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
 * @param  mmdl  MMDL *
 * @param  attr  �A�g���r���[�g
 * @retval  int    TRUE=���A�g���r���[�g
 */
//--------------------------------------------------------------
static BOOL MMdl_CheckMapAttrKind_Snow( MMDL * mmdl, u32 attr )
{
  #ifndef MMDL_PL_NULL
  if( MAPATTR_IsBridgeSnowShallow(attr) ){
    if( MMDL_CheckStatusBitBridge(mmdl) == FALSE ){
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
 * @param  mmdl  MMDL *
 * @param  attr  �A�g���r���[�g
 * @retval  int    TRUE=���A�g���r���[�g
 */
//--------------------------------------------------------------
static BOOL MMdl_CheckMapAttrKind_MostShallowSnow(
    MMDL * mmdl, u32 attr )
{
  #ifndef MMDL_PL_NULL
  if( MAPATTR_IsBridgeSnowShallow(attr) ){
    if( MMDL_CheckStatusBitBridge(mmdl) == FALSE ){
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
 * @param  mmdl  MMDL *
 * @param  attr  �A�g���r���[�g
 * @retval  int    TRUE=���A�g���r���[�g
 */
//--------------------------------------------------------------
static BOOL MMdl_CheckMapAttrKind_Bridge( MMDL * mmdl, u32 attr )
{
  if( MMDL_CheckStatusBitBridge(mmdl) == TRUE ){
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
 * @param  mmdl  MMDL *
 * @param  attr  �A�g���r���[�g
 * @retval  int    TRUE=���A�g���r���[�g
 */
//--------------------------------------------------------------
static BOOL MMdl_CheckMapAttrKind_BridgeV( MMDL * mmdl, u32 attr )
{
  if( MMDL_CheckStatusBitBridge(mmdl) == TRUE ){
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
 * @param  mmdl  MMDL *
 * @param  attr  �A�g���r���[�g
 * @retval  int    TRUE=���A�g���r���[�g
 */
//--------------------------------------------------------------
static BOOL MMdl_CheckMapAttrKind_BridgeH( MMDL * mmdl, u32 attr )
{
  if( MMDL_CheckStatusBitBridge(mmdl) == TRUE ){
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
 * @param  mmdl  MMDL
 * @param  pos  �擾������W
 * @param  pGridInfo ���i�[��
 * @retval  BOOL FALSE=�}�b�v��񂪖���
 */
//--------------------------------------------------------------
static BOOL MMdl_GetMapGridInfo(
  const MMDL *mmdl, const VecFx32 *pos, FLDMAPPER_GRIDINFO *pGridInfo )
{
  const FLDMAPPER *pG3DMapper =
    MMDLSYS_GetG3DMapper( MMDL_GetMMdlSys(mmdl) );
  
  if( FLDMAPPER_GetGridInfo(pG3DMapper,pos,pGridInfo) == TRUE ){
    return( TRUE );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �}�b�v�O���b�h���擾
 * @param  mmdl  MMDL
 * @param  pos       �擾������W(x,z�Ɏ擾�������ʒu���W�Ay�ɂ͌��݂�height���i�[����Ă��邱��)
 * @param  pGridData  �O���b�h�A�g���r���[�g�f�[�^�i�[��
 * @retval  0���� ��񂪎擾�ł��Ȃ�
 *
 *  @li GridInfo�Ɏ擾���ꂽ�S�K�w���T�[�`���A���݂�Y�ɍł��߂�height,attr��Ԃ��܂�
 *  @li GridInfo�̔z��0�̕����v���C�I���e�B�������ł�
 */
//--------------------------------------------------------------
static int MMdl_GetMapPosGridData(
  const MMDL *mmdl, const VecFx32 *pos, FLDMAPPER_GRIDINFODATA* pGridData)
{
  const FLDMAPPER *pG3DMapper =
    MMDLSYS_GetG3DMapper( MMDL_GetMMdlSys(mmdl) );
  
  return FLDMAPPER_GetGridData(pG3DMapper,pos,pGridData);
}

//--------------------------------------------------------------
/**
 * �}�b�v�A�g���r���[�g�擾
 * @param  mmdl  MMDL
 * @param  pos  �擾������W(x,z�Ɏ擾�������ʒu���W�Ay�ɂ͌��݂�height���i�[����Ă��邱��)
 * @param  attr  �A�g���r���[�g�i�[��
 * @retval  BOOL  FALSE=�A�g���r���[�g�����݂��Ȃ��B
 */
//--------------------------------------------------------------
BOOL MMDL_GetMapPosAttr(
  const MMDL *mmdl, const VecFx32 *pos, u32 *attr )
{
  FLDMAPPER_GRIDINFODATA gridData;
  *attr = 0;
  
  if( MMdl_GetMapPosGridData(mmdl,pos,&gridData) == TRUE ){
    *attr = gridData.attr;
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �}�b�v�����擾
 * @param  mmdl  MMDL
 * @param  pos  �擾������W
 * @param  height  �����i�[��
 * @retval  BOOL  FALSE=���������݂��Ȃ��B
 */
//--------------------------------------------------------------
BOOL MMDL_GetMapPosHeight(
  const MMDL *mmdl, const VecFx32 *pos, fx32 *height )
{
#if 1
  FLDMAPPER_GRIDINFODATA gridData;
  
  *height = 0;
  if( MMdl_GetMapPosGridData(mmdl,pos,&gridData) == TRUE ){
    *height = gridData.height;
    return( TRUE );
  }
#else
  FLDMAPPER_GRIDINFO GridInfo;
  *height = 0;

  if( MMdl_GetMapGridInfo(mmdl,pos,&GridInfo) == TRUE ){
    if( GridInfo.count ){
      int    i = 0;
      fx32  h_tmp,diff1,diff2;
      
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
//  �O���b�h���W�ړ��n
//======================================================================
//--------------------------------------------------------------
/**
 * �����Ō��ݍ��W���X�V�B�����͍X�V����Ȃ��B
 * @param  mmdl    MMDL * 
 * @param  dir      �ړ�����
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_UpdateGridPosDir( MMDL * mmdl, u16 dir )
{
  MMDL_SetOldGridPosX( mmdl, MMDL_GetGridPosX(mmdl) );
  MMDL_SetOldGridPosY( mmdl, MMDL_GetGridPosY(mmdl) );
  MMDL_SetOldGridPosZ( mmdl, MMDL_GetGridPosZ(mmdl) );
  
  MMDL_AddGridPosX( mmdl, MMDL_TOOL_GetDirAddValueGridX(dir) );
  MMDL_AddGridPosY( mmdl, 0 );
  MMDL_AddGridPosZ( mmdl, MMDL_TOOL_GetDirAddValueGridZ(dir) );
}

//--------------------------------------------------------------
/**
 * ���ݍ��W�ŉߋ����W���X�V
 * @param  mmdl    MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_UpdateGridPosCurrent( MMDL * mmdl )
{
  MMDL_SetOldGridPosX( mmdl, MMDL_GetGridPosX(mmdl) );
  MMDL_SetOldGridPosY( mmdl, MMDL_GetHeightGrid(mmdl) );
  MMDL_SetOldGridPosZ( mmdl, MMDL_GetGridPosZ(mmdl) );
}

//--------------------------------------------------------------
/**
 * ���݂̃O���b�h���W����w�������̃A�g���r���[�g���擾
 * @param  mmdl  MMDL *
 * @param  dir    DIR_UP��
 * @retval  u32    MAPATTR
 */
//--------------------------------------------------------------
MAPATTR MMDL_GetMapDirAttr( MMDL * mmdl, u16 dir )
{
  MAPATTR attr = MAPATTR_ERROR;
  VecFx32 pos;
  const FLDMAPPER *pG3DMapper;
  
  MMDL_GetVectorPos( mmdl, &pos );
  MMDL_TOOL_AddDirVector( dir, &pos, GRID_FX32 );
  MMDL_GetMapPosAttr( mmdl, &pos, &attr );
  
  return( attr );
}

//======================================================================
//  3D���W�n
//======================================================================
//--------------------------------------------------------------
/**
 * �w��l�����Z����
 * @param  mmdl    MMDL * 
 * @param  val      �ړ���
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_AddVectorPos( MMDL * mmdl, const VecFx32 *val )
{
  VecFx32 vec;
  MMDL_GetVectorPos( mmdl, &vec );
  vec.x += val->x;
  vec.y += val->y;
  vec.z += val->z;
  MMDL_SetVectorPos( mmdl, &vec );
}

//--------------------------------------------------------------
/**
 * �w��l���w������ɉ��Z����
 * @param  mmdl    MMDL * 
 * @param  dir      �ړ�4�����BDIR_UP��
 * @param  val      �ړ���(��
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_AddVectorPosDir( MMDL *mmdl, u16 dir, fx32 val )
{
  VecFx32 vec;
  MMDL_GetVectorPos( mmdl, &vec );
  switch( dir ){
  case DIR_UP:    vec.z -= val;  break;
  case DIR_DOWN:    vec.z += val;  break;
  case DIR_LEFT:    vec.x -= val;  break;
  case DIR_RIGHT:    vec.x += val;  break;
  }
  MMDL_SetVectorPos( mmdl, &vec );
}

//--------------------------------------------------------------
/**
 * ���ݍ��W���獂���擾�������W�ɔ��f
 * MMDL_STABIT_HEIGHT_GET_ERROR�̃Z�b�g�������čs��
 * @param  mmdl    MMDL * 
 * @retval  BOOL  TRUE=��������ꂽ�BFALSE=���Ȃ��B
 * MMDL_STABIT_HEIGHT_GET_ERROR�Ŏ擾�\
 */
//--------------------------------------------------------------
BOOL MMDL_UpdateCurrentHeight( MMDL * mmdl )
{
  VecFx32 vec_pos,vec_pos_h;
  
  MMDL_GetVectorPos( mmdl, &vec_pos );
  vec_pos_h = vec_pos;
  
  if( MMDL_CheckStatusBitHeightGetOFF(mmdl) == TRUE ){
    MMDL_OffStatusBit( mmdl, MMDL_STABIT_HEIGHT_GET_ERROR );
    return( FALSE );
  }
  
  {
    fx32 height;
    int ret = MMDL_GetMapPosHeight( mmdl, &vec_pos_h, &height );
    
    if( ret == TRUE ){
      vec_pos.y = height;
      MMDL_SetVectorPos( mmdl, &vec_pos );
      MMDL_OffStatusBit( mmdl, MMDL_STABIT_HEIGHT_GET_ERROR );
    }else{
      MMDL_OnStatusBit( mmdl, MMDL_STABIT_HEIGHT_GET_ERROR );
    }
    return( ret );
  }
}

//--------------------------------------------------------------
/**
 * ���ݍ��W�ŃA�g���r���[�g���f
 * MMDL_STABIT_ATTR_GET_ERROR�̃Z�b�g�������čs��
 * @retval  BOOL TRUE=�擾�����BFALSE=���s�B
 */
//--------------------------------------------------------------
BOOL MMDL_UpdateCurrentMapAttr( MMDL * mmdl )
{
  MAPATTR old_attr = MAPATTR_ERROR;
  MAPATTR now_attr = old_attr;
  
  if( MMDL_CheckMoveBitAttrGetOFF(mmdl) == FALSE ){
    VecFx32 pos;
    int gx = MMDL_GetOldGridPosX( mmdl );
    int gy = MMDL_GetOldGridPosY( mmdl );
    int gz = MMDL_GetOldGridPosZ( mmdl );
    
    pos.x = GRID_SIZE_FX32( gx );
    pos.y = GRID_SIZE_FX32( gy );
    pos.z = GRID_SIZE_FX32( gz );
    MMDL_GetMapPosAttr( mmdl, &pos, &old_attr );
    
    gx = MMDL_GetGridPosX( mmdl );
    gz = MMDL_GetGridPosZ( mmdl );
    MMDL_TOOL_GetCenterGridPos( gx, gz, &pos );
    pos.y = MMDL_GetVectorPosY( mmdl );
    MMDL_GetMapPosAttr( mmdl, &pos, &now_attr );
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
//  �`�揉�����A���A����ɌĂ΂�铮�쏉�����֐�
//======================================================================
//--------------------------------------------------------------
/**
 * �`�揉�����A���A����ɌĂ΂�铮�쏉�����֐��B
 * �B�ꖪ�n����͓���֐����ŃG�t�F�N�g�n�̃Z�b�g���s���Ă���B
 * �C�x���g���̃|�[�Y�����������ォ�甭�����Ă���ꍇ�A
 * ����֐����Ă΂�Ȃ��ׁA�`�揉�������ɂ��̊֐����ĂԎ��ő΍�B
 * @param  mmdl  MMDL *
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_CallMoveProcAfterDrawInit( MMDL * mmdl )
{
  u16 code = MMDL_GetMoveCode( mmdl );
  if( code == MV_HIDE_SNOW || code == MV_HIDE_SAND ||
    code == MV_HIDE_GRND || code == MV_HIDE_KUSA ){
    MMDL_CallMoveProc( mmdl );
  }
}

//======================================================================
//  ����c�[��
//======================================================================
//--------------------------------------------------------------
///  4�����ʍ��W�����@X
//--------------------------------------------------------------
static const int DATA_DirGridValueX[] =
{ 0, 0, -1, 1 };

//--------------------------------------------------------------
///  4�����ʍ��W�����@Y
//--------------------------------------------------------------
static const int DATA_DirGridValueY[] =
{ 0, 0, 0, 0 };

//--------------------------------------------------------------
///  4�����ʍ��W�����@Z
//--------------------------------------------------------------
static const int DATA_DirGridValueZ[] =
{ -1, 1, 0, 0 };

//--------------------------------------------------------------
/**
 * ��������X���������l���擾
 * @param  dir    �����BDIR_UP��
 * @retval  s16 �O���b�h�P�ʂł̑����l
 */
//--------------------------------------------------------------
s16 MMDL_TOOL_GetDirAddValueGridX( u16 dir )
{
  return( (s16)DATA_DirGridValueX[dir] );
}

//--------------------------------------------------------------
/**
 * ��������Z���������l���擾
 * @param  dir    �����BDIR_UP��
 * @retval  int    �O���b�h�P�ʂł̑����l
 */
//--------------------------------------------------------------
s16 MMDL_TOOL_GetDirAddValueGridZ( u16 dir )
{
  return( (s16)DATA_DirGridValueZ[dir] );
}

//--------------------------------------------------------------
/**
 * �w��l���w������ɉ��Z����
 * @param  dir      �ړ�4�����BDIR_UP��
 * @param  vec      ������Ώ�
 * @param  val      �ړ���(��
 * @retval  nothing
 */
//--------------------------------------------------------------
void MMDL_TOOL_AddDirVector( u16 dir, VecFx32 *vec, fx32 val )
{
  switch( dir ){
  case DIR_UP:    vec->z -= val;  break;
  case DIR_DOWN:    vec->z += val;  break;
  case DIR_LEFT:    vec->x -= val;  break;
  case DIR_RIGHT:    vec->x += val;  break;
  }
}

//--------------------------------------------------------------
/**
 * �w��l���w������ɉ��Z����@�O���b�h
 * @param  dir      �ړ�4�����BDIR_UP��
 * @param  gx      ������Ώہ@�O���b�hx
 * @param gz      ������Ώہ@�O���b�hz
 * @param  val      �ړ���(��
 * @retval  nothing
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
 * @param  gx  �O���b�hX���W  
 * @param  gz  �O���b�hZ���W
 * @param  vec  ���W�i�[��
 * @retval  nothing
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
 * @param  gx  �O���b�hX���W�i�[��
 * @param  gy  �O���b�hY���W�i�[��
 * @param  gz  �O���b�hZ���W�i�[��
 * @param  vec  �ϊ�������W
 * @retval  nothing
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
 * @param  gx  �O���b�hX���W
 * @param  gy  �O���b�hY���W
 * @param  gz  �O���b�hZ���W
 * @param  vec  ���W�ϊ���
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
//  �������]�e�[�u��
//--------------------------------------------------------------
static const u32 DATA_DirFlipTbl[] =
{ DIR_DOWN, DIR_UP, DIR_RIGHT, DIR_LEFT };

//--------------------------------------------------------------
/**
 * �^����ꂽ�����𔽓]����
 * @param  dir    DIR_UP��
 * @retval  int    dir�̔��]����
 */
//--------------------------------------------------------------
u16 MMDL_TOOL_FlipDir( u16 dir )
{
  return( DATA_DirFlipTbl[dir] );
}

//--------------------------------------------------------------
/**
 * �����ԕ������擾
 * @param  ax  �Ώ�A X���W
 * @param  az  �Ώ�A Z���W
 * @param  bx  �Ώ�B X���W
 * @param  bz  �Ώ�B Z���W
 * @retval  int  �Ώ�A����Ώ�B�ւ̕��� DIR_UP��
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
/// �J�������p�x�ϊ�
//--------------------------------------------------------------
static const u8 data_angle8_4[16] =
{
  0,
  1, 1, 1, 1, 1, 1,
  2, 2,
  3, 3, 3, 3, 3, 3,
  0,
};

static const u16 data_dir_angle[DIR_MAX4] =
{
  0x8000, 0, 0x4000, 0xC000,
};

static const u8 data_angleChange360_4[4][4] =
{
  {DIR_UP,DIR_DOWN,DIR_LEFT,DIR_RIGHT}, //0 0x0000
  {DIR_RIGHT,DIR_LEFT,DIR_UP,DIR_DOWN}, //1 0x4000
  {DIR_DOWN,DIR_UP,DIR_RIGHT,DIR_LEFT}, //2 0x8000
  {DIR_LEFT,DIR_RIGHT,DIR_DOWN,DIR_UP}, //3 0xc000
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
  angleYaw += data_dir_angle[dir];  //�@dir�̕������������ɂȂ�p�x���A���O����ύX
  angleYaw >>= 12; // angle/0x1000(16)
  angleYaw = data_angle8_4[angleYaw];
  angleYaw = data_angleChange360_4[angleYaw][DIR_DOWN]; // ��������ōl���Ă���̂ŁB
  GF_ASSERT( angleYaw < DIR_MAX4 );
  return( angleYaw );
}
