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
MAPATTR MAPATTR_GetMapAttribute( FLDMAPPER *mapper, const VecFx32 *pos )
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
MAPATTR_VAL MAPATTR_GetMapAttrValue( const MAPATTR attr )
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
MAPATTR_FLAG MAPATTR_GetMapAttrFlag( const MAPATTR attr )
{
  MAPATTR_FLAG flag = (attr >> 16) & 0xffff;
  return( flag );
}
