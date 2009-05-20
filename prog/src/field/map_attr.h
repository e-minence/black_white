//======================================================================
/**
 * @file  map_attr.h
 * @brief	�}�b�v�A�g���r���[�g
 * @author kagaya
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "field_g3d_mapper.h"

//======================================================================
//  define
//======================================================================
#define MAPATTR_ERROR (0xffffffff) ///<�A�g���r���[�g�G���[

//--------------------------------------------------------------
/// MAPATTR_FLAGBIT
//--------------------------------------------------------------
typedef enum
{
  MAPATTR_FLAGBIT_ENTER = (1<<0), ///<�i���\�t���O ON=�\
  MAPATTR_FLAGBIT_WATER = (1<<1), ///<���A�g���r���[�g ON=��
  MAPATTR_FLAGBIT_ENCOUNT = (1<<2), ///<�G�o���t���O ON=�o��
  MAPATTR_FLAGBIT_FOOTMARK = (1<<3), ///<���Օ\���t���O ON=���՗L��
  MAPATTR_FLAGBIT_SPLASH = (1<<4), ///<���򖗕\���t���O ON=���򖗗L��
  MAPATTR_FLAGBIT_REFLECT = (1<<5), ///<�f�荞�ݕ\���t���O ON=�f�荞�ݗL��
  MAPATTR_FLAGBIT_SHADOW = (1<<6), ///<�e�\���t���O ON=�e�\���L��
}MAPATTR_FLAGBIT;

//======================================================================
//  struct
//======================================================================
///MAPATTR
typedef u32 MAPATTR;
///MAPATTR_VAL
typedef u16 MAPATTR_VAL;
///MAPATTR_FLAG
typedef u16 MAPATTR_FLAG;

//======================================================================
//  proto
//======================================================================
extern MAPATTR MAPATTR_GetMapAttribute(FLDMAPPER *mapper, const VecFx32 *pos );
extern MAPATTR_VAL MAPATTR_GetMapAttrValue( const MAPATTR attr );
extern MAPATTR_FLAG MAPATTR_GetMapAttrFlag( const MAPATTR attr );
