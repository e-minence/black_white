//======================================================================
/**
 * @file  map_attr.c
 * @brief	�}�b�v�A�g���r���[�g
 * @author kagaya
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "field_g3d_mapper.h"
#include "map_attr.h"

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
 * @retval MAPATTR MAPATTR_ERROR=�擾�G���[
 */
//--------------------------------------------------------------
MAPATTR MAPATTR_GetAttribute( FLDMAPPER *mapper, const VecFx32 *pos )
{
  MAPATTR attr = MAPATTR_ERROR;
  FLDMAPPER_GRIDINFO gridInfo;
  
  if( FLDMAPPER_GetGridInfo(mapper,pos,&gridInfo) == TRUE ){
    attr = gridInfo.gridData[0].attr;  
  }
  
  return( attr );
}

//--------------------------------------------------------------
/**
 * MAPATTR����MAPATTR_VAL���擾
 * @param attr MAPATTR
 * @retval MAPATTR_VAL
 */
//--------------------------------------------------------------
MAPATTR_VAL MAPATTR_GetAttrValue( const MAPATTR attr )
{
  MAPATTR_VAL val = attr & 0xffff;
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

//======================================================================
//  �A�g���r���[�g�o�����[�@����
//======================================================================
//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@������
 * @param val MAPATTR_VAL
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_CheckAttrValueLongGrass( const MAPATTR_VAL val )
{
  if( val == 0x06 || val == 0x07 ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@��W�����v�A�g���r���[�g
 * @param val MAPATTR_VAL
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_CheckAttrValueJumpUp( const MAPATTR_VAL val )
{
  if( val == 0x74 ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@���W�����v�A�g���r���[�g
 * @param val MAPATTR_VAL
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_CheckAttrValueJumpDown( const MAPATTR_VAL val )
{
  if( val == 0x75 ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@���W�����v�A�g���r���[�g
 * @param val MAPATTR_VAL
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_CheckAttrValueJumpLeft( const MAPATTR_VAL val )
{
  if( val == 0x73 ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@�E�W�����v�A�g���r���[�g
 * @param val MAPATTR_VAL
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_CheckAttrValueJumpRight( const MAPATTR_VAL val )
{
  if( val == 0x72 ){
    return( TRUE );
  }
  return( FALSE );
}
