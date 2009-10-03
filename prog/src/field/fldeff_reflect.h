//======================================================================
/**
 * @file  fldeff_reflect.h
 * @brief  ���ՃG�t�F�N�g
 * @author  kagaya
 * @date  05.07.13
 */
//======================================================================
#pragma once
#include <gflib.h>
#include "system/gfl_use.h"

#include "fieldmap.h"
#include "field_effect.h"
#include "fldmmdl.h"

//======================================================================
//  define
//======================================================================
typedef enum
{
  REFLECT_TYPE_POOL = 0,  ///<������  �L�k�L��@�\���ʒu�Ⴂ
  REFLECT_TYPE_POND,    ///<�r�@�L�k�L��@�\���ʒu�[��
  REFLECT_TYPE_MIRROR,    ///<���@�L�k�����@�\���ʒuPOOL�Ɠ���
  REFLECT_TYPE_MAX,
}REFLECT_TYPE;

//======================================================================
//  struct
//======================================================================

//======================================================================
//  extern
//======================================================================
extern void * FLDEFF_REFLECT_Init( FLDEFF_CTRL *fectrl, HEAPID heapID );
extern void FLDEFF_REFLECT_Delete( FLDEFF_CTRL *fectrl, void *work );

extern void FLDEFF_REFLECT_SetMMdl( MMDLSYS *mmdlsys,
    MMDL *mmdl, FLDEFF_CTRL *fectrl, REFLECT_TYPE type );
