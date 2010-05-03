//======================================================================
/**
 * @file  map_attr.c
 * @brief	�}�b�v�A�g���r���[�g
 * @author kagaya
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "gamesystem/pm_season.h"
#include "field_g3d_mapper.h"
#include "map_attr.h"

#include "map_attr_data.cdat"
//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================

//======================================================================
//  proto
//======================================================================

//======================================================================
//  �}�b�v�A�g���r���[�g
//======================================================================
//--------------------------------------------------------------
/**
 * �w��ʒu�̃A�g���r���[�g���擾
 * @param mapper FLDMAPPER
 * @param pos �A�g���r���[�g���擾������x,z���W,y�͌��݈ʒu(�������͊K�w����ɗp��������l)
 * @retval MAPATTR MAPATTR_ERROR=�擾�G���[
 */
//--------------------------------------------------------------
MAPATTR MAPATTR_GetAttribute( const FLDMAPPER *mapper, const VecFx32 *pos )
{
  MAPATTR attr = MAPATTR_ERROR;
#if 0 //�P�w
  FLDMAPPER_GRIDINFO gridInfo;
  
  if( FLDMAPPER_GetGridInfo(mapper,pos,&gridInfo) == TRUE ){
    attr = gridInfo.gridData[0].attr;  
  }
#else //���w�Ή�
  FLDMAPPER_GRIDINFODATA gridData;
  if( FLDMAPPER_GetGridData(mapper,pos,&gridData) == TRUE){
    return gridData.attr;
  }
#endif
  
  return( attr );
}

//--------------------------------------------------------------
/**
 * MAPATTR����MAPATTR_VALUE���擾
 * @param attr MAPATTR
 * @retval MAPATTR_VALUE
 */
//--------------------------------------------------------------
MAPATTR_VALUE MAPATTR_GetAttrValue( const MAPATTR attr )
{
  MAPATTR_VALUE val = attr & 0xffff;
  return( val );
}

//--------------------------------------------------------------
/**
 * MAPATTR����MAPATTR_FLAG���擾
 * @param attr MAPATTR
 * @retval MAPATTR_FLAG
 */
//--------------------------------------------------------------
MAPATTR_FLAG MAPATTR_GetAttrFlag( const MAPATTR attr )
{
  MAPATTR_FLAG flag = (attr >> 16) & 0xffff;
  return( flag );
}

//--------------------------------------------------------------
/**
 * @brief MAPATTR����A�A�g���r���[�g�o�����[�̗L��/�������擾
 *
 * @param attr MAPATTR
 * @retval TRUE   �L�� 
 * @retval FALSE  ����
 */
//--------------------------------------------------------------
BOOL MAPATTR_IsEnable( const MAPATTR attr )
{
  u16 val = MAPATTR_GetAttrValue( attr );

  //�A�g���r���[�g���̂��́A��������Value���������`�F�b�N
  if( (attr == MAPATTR_ERROR) ||
      (val == MAPATTR_VAL_ERR)){
    return FALSE;
  }
  return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief MAPATTR����i���s��Ԃ��擾
 *
 * @param attr MAPATTR
 * @retval TRUE �i���s��
 * @retval FALSE �i����
 *
 * @com �A�g���r���[�g�����l���i���s�ƕԂ����߁A������Hitch�t���O�݂̂�Ԃ��Ă���̂ł͂���܂���
 */
//--------------------------------------------------------------
BOOL MAPATTR_GetHitchFlag( const MAPATTR attr )
{
  u16 flag = MAPATTR_GetAttrFlag( attr );

  //�A�g���r���[�gValue���������A�i���s�t���O��On�Ȃ�ʂ�Ȃ�
  if( (MAPATTR_IsEnable( attr ) == FALSE) ||
      (flag & MAPATTR_FLAGBIT_HITCH)){
    return TRUE;
  }
  return FALSE;
}


//======================================================================
//  �A�g���r���[�g�t���O�@����
//======================================================================

//======================================================================
//  �A�g���r���[�g�o�����[�@����
//======================================================================
//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@������
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckLongGrass( const MAPATTR_VALUE val )
{
  if(val == MATTR_E_LGRASS_LOW || val == MATTR_E_LGRASS_HIGH ){
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@��W�����v�A�g���r���[�g
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckJumpUp( const MAPATTR_VALUE val )
{
  return (val == MATTR_JUMP_UP);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@���W�����v�A�g���r���[�g
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckJumpDown( const MAPATTR_VALUE val )
{
  return (val == MATTR_JUMP_DOWN);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@���W�����v�A�g���r���[�g
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckumpLeft( const MAPATTR_VALUE val )
{
  return (val == MATTR_JUMP_LEFT);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@�E�W�����v�A�g���r���[�g
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckJumpRight( const MAPATTR_VALUE val )
{
  return (val == MATTR_JUMP_RIGHT);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@����
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckSandStream( const MAPATTR_VALUE val )
{
  return (val == MATTR_DRIFT_SAND_01);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N �J�E���^�[
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckCounter( const MAPATTR_VALUE val )
{
  return (val == MATTR_COUNTER_01);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N �}�b�g
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckMat( const MAPATTR_VALUE val )
{
  return (val == MATTR_MAT_01);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N �p�\�R��
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckPC( const MAPATTR_VALUE val )
{
  return (val == MATTR_PC_01);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N ���E�n�}
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckMap( const MAPATTR_VALUE val )
{
  return (val == MATTR_WORLDMAP_01);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N �e���r
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckTV( const MAPATTR_VALUE val )
{
  return (val == MATTR_TV_01);
}

//--------------------------------------------------------------
/**
 * @brief �A�g���r���[�g�o�����[�@�����̔��@
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckVendorMachine( const MAPATTR_VALUE val )
{
  return ( val == MATTR_AUTO_SELL_01 );
}
//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N �{�I���̂P
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckBookShelf1( const MAPATTR_VALUE val )
{
  return (val == MATTR_BOOKSHELF_01);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N �{�I���̂Q
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckBookShelf2( const MAPATTR_VALUE val )
{
  return (val == MATTR_BOOKSHELF_02);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N �{�I���̂R
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckBookShelf3( const MAPATTR_VALUE val )
{
  return (val == MATTR_BOOKSHELF_03);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N �{�I���̂S
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckBookShelf4( const MAPATTR_VALUE val )
{
  return (val == MATTR_BOOKSHELF_04);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N ��
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckVase( const MAPATTR_VALUE val )
{
  return (val == MATTR_VASE_01);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N �S�~��
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckDustBox( const MAPATTR_VALUE val )
{
  return (val == MATTR_DUST_BOX);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N  ���i�I���̂P
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckShopShelf1( const MAPATTR_VALUE val )
{
  return (val == MATTR_SHOPSHELF_01);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N  ���i�I���̂Q
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckShopShelf2( const MAPATTR_VALUE val )
{
  return (val == MATTR_SHOPSHELF_02);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N  ���i�I���̂R
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckShopShelf3( const MAPATTR_VALUE val )
{
  return (val == MATTR_SHOPSHELF_03);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N ���ނ�@�Z�����ނ�@��
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckEncountShortGrassLow( const MAPATTR_VALUE val )
{
  if( val == MATTR_E_GRASS_LOW ||
      val == MATTR_E_GRASS_LOW_02 ||
      val == MATTR_SNOW_GRASS_LOW ){
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N ���ނ�@�Z�����ނ�@��
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckEncountShortGrassHigh( const MAPATTR_VALUE val )
{
  if( val == MATTR_E_GRASS_HIGH ||
      val == MATTR_E_GRASS_HIGH_02 ||
      val == MATTR_SNOW_GRASS_HIGH
  ){
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N ���ނ�@�������ނ�@��
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckEncountLongGrassLow( const MAPATTR_VALUE val )
{
  return ( val == MATTR_E_LGRASS_LOW );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N ���ނ�@�������ނ�@��
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckEncountLongGrassHigh( const MAPATTR_VALUE val )
{
  return ( val == MATTR_E_LGRASS_HIGH );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N ���ނ�G���J�E���g��@A�Q(A1,A2)
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckEncountGrassLow( const MAPATTR_VALUE val )
{
  if( MAPATTR_VALUE_CheckEncountShortGrassLow(val) == TRUE ||
      MAPATTR_VALUE_CheckEncountLongGrassLow(val) == TRUE ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N ���ނ�G���J�E���g���@B�Q(B1,B2)
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckEncountGrassHigh( const MAPATTR_VALUE val )
{
  if( MAPATTR_VALUE_CheckEncountShortGrassHigh(val) == TRUE ||
      MAPATTR_VALUE_CheckEncountLongGrassHigh(val) == TRUE ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N ���ނ�G���J�E���g
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckEncountGrass( const MAPATTR_VALUE val )
{
  if( MAPATTR_VALUE_CheckEncountGrassLow(val) == TRUE ||
      MAPATTR_VALUE_CheckEncountGrassHigh(val) == TRUE ){
    return( TRUE );
  }
  return( FALSE );
}


//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N ���͌�
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckKairikiAna( const MAPATTR_VALUE val )
{
  return( val == MATTR_KAIRIKI_ANA );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N ��
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckShore( const MAPATTR_VALUE val )
{
  if( val == MATTR_SHORE_01 || val == MATTR_SHORE_02 ){
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N �����܂�(�l�G����)
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckPool( const MAPATTR_VALUE val )
{
  return( val == MATTR_POOL_01 );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N ��
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckShoal( const MAPATTR_VALUE val )
{
  return( val == MATTR_SHOAL_01 );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@��
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckWaterFall( const MAPATTR_VALUE val )
{
  return( val == MATTR_WATERFALL_01 );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@��
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckSnow( const MAPATTR_VALUE val )
{
  return( val == MATTR_SNOW_01 );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@���]�Ԑi���s�Ȑ�
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckSnowNotCycle( const MAPATTR_VALUE val )
{
  return( val == MATTR_SNOW_02 );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@��n�A�g���r���[�g
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckSnowType( const MAPATTR_VALUE val )
{
  if( MAPATTR_VALUE_CheckSnow(val) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@�X
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckIce( const MAPATTR_VALUE val )
{
  if( val == MATTR_ICE_01 || val == MATTR_ICE_02 ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@�h��Ȃ��f�荞�ݕX
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckIce02( const MAPATTR_VALUE val )
{
  return( val == MATTR_ICE_02 );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@�󂢎���
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckMarsh( const MAPATTR_VALUE val )
{
  return( val == MATTR_MARSH_01 );
}
//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@�[���C
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckDeepSea( const MAPATTR_VALUE val )
{
  return ( val == MATTR_DEEP_SEA_01 );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@���u
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckSand( const MAPATTR_VALUE val )
{
  return( val == MATTR_SAND_01 );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@���u
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckDesert( const MAPATTR_VALUE val )
{
  if( val == MATTR_E_DESERT_01 ||
      val == MATTR_DESERT_01 ||
      val == MATTR_E_DESERT_02 ){
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@���n�A�g���r���[�g
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckSandType( const MAPATTR_VALUE val )
{
  if( MAPATTR_VALUE_CheckSand(val) || MAPATTR_VALUE_CheckDesert(val) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@���n�A�g���r���[�g
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckWaterType( const MAPATTR_VALUE val )
{
  if( (val == MATTR_WATER_01) || (val == MATTR_WATER_S01) ||
      (val == MATTR_SEA_01) || (val == MATTR_DEEP_MARSH_01) ||
      (val == MATTR_DEEP_SEA_01) ) {
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@�ʏ�n��
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckMonoGround( const MAPATTR_VALUE val )
{
  return ( val == MATTR_ZIMEN_01 );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@�G�ߕω��n�ʂP
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckSeasonGround1( const MAPATTR_VALUE val )
{
  return ( val == MATTR_ZIMEN_S01 );
}
//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@�G�ߕω��n�ʂQ
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckSeasonGround2( const MAPATTR_VALUE val )
{
  return ( val == MATTR_ZIMEN_S02 );
}
//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@�n�ʁi�G���J�E���g�j
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckEncountGround( const MAPATTR_VALUE val )
{
  return ( (val == MATTR_E_ZIMEN_01) || (val == MATTR_E_SLIP_01) );
}
//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@�����i�G���J�E���g�j
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckEncountIndoor( const MAPATTR_VALUE val )
{
  return ( val == MATTR_E_ZIMEN_02 );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@�Ő�
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckLawn( const MAPATTR_VALUE val )
{
  return ( val == MATTR_LAWN_01 );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@�r���r����
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckElecFloor( const MAPATTR_VALUE val )
{
  return ( val == MATTR_ELECTORIC_FLOOR );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N �d�C��
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckElecRock( const MAPATTR_VALUE val )
{
  return ( val == MATTR_ELECTORIC_ROCK );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@����
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckOze01( const MAPATTR_VALUE val )
{
  return ( val == MATTR_OZE_01 );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@�����K�i
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckOzeStairs( const MAPATTR_VALUE val )
{
  return ( val == MATTR_OZE_STAIRS );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@�[������
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckDesertDeep( const MAPATTR_VALUE val )
{
  return ( val == MATTR_E_DESERT_01 );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@����n�ё�
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckSnowGrassLow( const MAPATTR_VALUE val )
{
  return ( val == MATTR_SNOW_GRASS_LOW );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@����n�ё��@��
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckSnowGrassHigh( const MAPATTR_VALUE val )
{
  return ( val == MATTR_SNOW_GRASS_HIGH );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@����
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckMirrorFloor( const MAPATTR_VALUE val )
{
  return ( val == MATTR_MIRROR_01 );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@���@�㉺����(�_�b�V������)
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 *
 * �㉺������M�~�b�N�Ŏ������Ă��邽�߁A��R10R0901�ł̂ݗL���ł�
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckUpDownFloor( const MAPATTR_VALUE val )
{
  return ( val == MATTR_UPDOWN_FLOOR );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@�ǁA������ֈړ��s��
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckNotMoveUp( const MAPATTR_VALUE val )
{
  if( val == MATTR_NMOVE_UP || val == MATTR_NMOVE_LU ||
      val == MATTR_NMOVE_RU ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@�ǁA�������ֈړ��s��
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckNotMoveDown( const MAPATTR_VALUE val )
{
  if( val == MATTR_NMOVE_DOWN || val == MATTR_NMOVE_LD ||
      val == MATTR_NMOVE_RD ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@�ǁA�������ֈړ��s��
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckNotMoveLeft( const MAPATTR_VALUE val )
{
  if( val == MATTR_NMOVE_LEFT || val == MATTR_NMOVE_LU ||
      val == MATTR_NMOVE_LD ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@�ǁA�E�����ֈړ��s��
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckNotMoveRight( const MAPATTR_VALUE val )
{
  if( val == MATTR_NMOVE_RIGHT || val == MATTR_NMOVE_RU ||
      val == MATTR_NMOVE_RD ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * ���]�Ԃ�����Ȃ��A�g���r���[�g���`�F�b�N
 * @param val MAPATTR_VALUE
 * @retval BOOL TRUE=�i���s�A�g���r���[�g
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckCycleNotUse( MAPATTR_VALUE val )
{
  if( MAPATTR_VALUE_CheckLongGrass(val) ||
      MAPATTR_VALUE_CheckOze01(val) ||
      MAPATTR_VALUE_CheckOzeStairs(val) ||
      MAPATTR_VALUE_CheckSnowNotCycle(val) ){
    return( TRUE ); //hit
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@�l�G�ω��������ނ�@��
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckGrassAllYearLow( const MAPATTR_VALUE val )
{
  return ( val == MATTR_E_GRASS_LOW_02 );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@�l�G�ω��������ނ�@��
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckGrassAllYearHigh( const MAPATTR_VALUE val )
{
  return ( val == MATTR_E_GRASS_HIGH_02 );
}

/*
 *  @breif  �A�g���r���[�g�@�G�t�F�N�g�G���J�E���g�^�C�v�`�F�b�N
 */
EFFENC_TYPE_ID MAPATTR_GetEffectEncountType( MAPATTR attr )
{
  MAPATTR_FLAG flag = MAPATTR_GetAttrFlag( attr );
  MAPATTR_VALUE value = MAPATTR_GetAttrValue( attr );

  if( value == MATTR_BRIDGE_01 ){
    return EFFENC_TYPE_BRIDGE;
  }
  if( !(flag & MAPATTR_FLAGBIT_ENCOUNT)){
    return EFFENC_TYPE_MAX;
  }
  switch(value){
  case MATTR_E_GRASS_LOW:
    return EFFENC_TYPE_SGRASS_NORMAL;

  case MATTR_SNOW_GRASS_LOW:
    return EFFENC_TYPE_SGRASS_SNOW;
  
  case MATTR_E_GRASS_LOW_02:
    return EFFENC_TYPE_SGRASS_WARM;

  case MATTR_E_LGRASS_LOW:
    return EFFENC_TYPE_LGRASS;

  case MATTR_E_ZIMEN_01:
    return EFFENC_TYPE_CAVE;

  case MATTR_WATER_01:
    return EFFENC_TYPE_WATER;

  case MATTR_SEA_01:
  case MATTR_DEEP_SEA_01:
    return EFFENC_TYPE_SEA;
  }
  return EFFENC_TYPE_MAX;
}

//--------------------------------------------------------------
/**
 * @brief   BG�A�g���r���[�g����o�g��BG�A�g���r���[�g�̎w���Ԃ�
 * @param value   �}�b�v�A�g���r���[�g
 * @retval  BtlBgAttr
 */
//--------------------------------------------------------------
BtlBgAttr MAPATTR_GetBattleAttrID( MAPATTR_VALUE value )
{
  int i;

  for( i = 0; i < MATTR_TO_BTLBGATTR_TBL_SIZ;i++){
    if( DATA_MapAttr2BtlBgAttr[i][0] == value ){
      return DATA_MapAttr2BtlBgAttr[i][1];
    }
  }
  OS_Printf("Warning! GetBattleAttrID = 0x%02x\n", value ); //�ӂ[�͒�`�ς݂̂ł�������I
  return BATTLE_BG_ATTR_LAWN; //�W��
}

//--------------------------------------------------------------
/**
 * �g���\�A�g���r���[�g�`�F�b�N
 * @param   now_attr    ���ݒn�̃A�g���r���[�g
 * @param   front_attr  ���@�̈�O�̃A�g���r���[�g
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_CheckNaminoriUse( const MAPATTR now_attr, const MAPATTR front_attr )
{
  MAPATTR_FLAG  f_flag = MAPATTR_GetAttrFlag( front_attr );
  MAPATTR_VALUE f_val = MAPATTR_GetAttrValue( front_attr );

  if( MAPATTR_GetHitchFlag( now_attr ) ){
    return FALSE;
  }
  //�ڂ̑O���N���\�Ȑ��n�A�g���r���[�g��
  //�݃A�g���r���[�g�Ȃ�Δg����
  if( ((f_flag&MAPATTR_FLAGBIT_WATER) && (!MAPATTR_GetHitchFlag(front_attr))) ||
      MAPATTR_VALUE_CheckShore(f_val)
    ){
    return( TRUE );
  }
  return( FALSE );
}

