//======================================================================
/**
 * @file  field_encount.c
 * @brief	�t�B�[���h�@�G���J�E���g����
 * @author kagaya
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "fieldmap.h"
#include "map_attr.h"

#include "field_encount.h"

//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
/// FIELD_ENCOUNT
//--------------------------------------------------------------
struct _TAG_FIELD_ENCOUNT
{
  FIELDMAP_WORK *fwork;
};

//======================================================================
//  proto
//======================================================================

//======================================================================
//  �t�B�[���h�G���J�E���g
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h�G���J�E���g�@���[�N�쐬
 * @param fwork FIELDMAP_WORK
 * @retval FIELD_ENCOUNT*
 */
//--------------------------------------------------------------
FIELD_ENCOUNT * FIELD_ENCOUNT_Create( FIELDMAP_WORK *fwork )
{
  HEAPID heapID;
  FIELD_ENCOUNT *enc;
  
  heapID = FIELDMAP_GetHeapID( fwork );
  enc = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_ENCOUNT) );
  
  enc->fwork = fwork;
  return( enc );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�G���J�E���g�@�폜
 * @param enc FIELD_ENCOUNT*
 * @retval nothing
 */
//--------------------------------------------------------------
void FIELD_ENCOUNT_Delete( FIELD_ENCOUNT *enc )
{
  GFL_HEAP_FreeMemory( enc );
}

//======================================================================
//  �t�B�[���h�G���J�E���g�@�`�F�b�N
//======================================================================
//--------------------------------------------------------------
/**
 * �G���J�E���g�`�F�b�N
 * @param enc FIELD_ENCOUNT
 * @retval BOOL TRUE=�G���J�E���g����
 */
//--------------------------------------------------------------
BOOL FIELD_ENCOUNT_CheckEncount( FIELD_ENCOUNT *enc )
{
  int gx,gz;
  VecFx32 pos;
  MAPATTR attr;
  MAPATTR_VAL attr_flag;
  FIELD_PLAYER *fplayer = FIELDMAP_GetFieldPlayer( enc->fwork );
  FLDMAPPER *mapper = FIELDMAP_GetFieldG3Dmapper( enc->fwork );
  
  FIELD_PLAYER_GetPos( fplayer, &pos );
  attr = MAPATTR_GetMapAttribute( mapper, &pos );
  
  if( attr == MAPATTR_ERROR ){
    return( FALSE );
  }
  
  attr_flag = MAPATTR_GetMapAttrFlag( attr );
  
  if( (attr_flag & MAPATTR_FLAGBIT_ENCOUNT) == 0 ){
    return( FALSE );
  }
  
  { //�莝���|�P�����`�F�b�N
  }
  
  { //�X�v���[�`�F�b�N
  }
  
  { //�G���J�E���g�m���擾
  }
  
  { //�p�����^�ɂ��m���ϓ�
  }
  
  { //�G���J�E���g�`�F�b�N
  }
  
  return( FALSE );
}

