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
#define CALC_SHIFT (8) ///<�v�Z�V�t�g�l
#define WALK_COUNT_GLOBAL (8) ///<�G���J�E���g���Ȃ�����
#define WALK_COUNT_MAX (0xffff) ///<�����J�E���g�ő�
#define NEXT_PERCENT (40) ///<�G���J�E���g�����{�m��
#define	WALK_NEXT_PERCENT	(5) ///<�����J�E���g���s�Ŏ��̏����ɐi�ފm��

#ifdef DEBUG_ONLY_FOR_kagaya
#define DEBUG_ENCOUNT_CHECKOFF_ATTR
#endif

//--------------------------------------------------------------
//  ENCOUNT_LOCATION
//--------------------------------------------------------------
typedef enum
{
  ENCOUNT_LOCATION_GROUND = 0,
  ENCOUNT_LOCATION_WATER,
  ENCOUNT_LOCATION_FISHING,
}ENCOUNT_LOCATION;

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
/// FIELD_ENCOUNT
//--------------------------------------------------------------
struct _TAG_FIELD_ENCOUNT
{
  int walk_count;
  FIELDMAP_WORK *fwork;
};

//======================================================================
//  proto
//======================================================================
static u32 enc_GetAttrPercent( FIELD_ENCOUNT *enc,
    MAPATTR_VAL attr_val, ENCOUNT_LOCATION *outEncLocation );

static BOOL enc_CheckEncount(
    FIELD_ENCOUNT *enc, const u32 per, const MAPATTR_VAL attr_val );
static BOOL enc_CheckEncountWalk( FIELD_ENCOUNT *enc, u32 per );

static u32 enc_GetPercentRand( void );

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
  u32 pre;
  BOOL ret = FALSE;
  int gx,gz;
  VecFx32 pos;
  MAPATTR attr;
  MAPATTR_VAL attr_val;
  MAPATTR_FLAG attr_flag;
  ENCOUNT_LOCATION enc_loc;
  FIELD_PLAYER *fplayer = FIELDMAP_GetFieldPlayer( enc->fwork );
  FLDMAPPER *mapper = FIELDMAP_GetFieldG3Dmapper( enc->fwork );
  
  FIELD_PLAYER_GetPos( fplayer, &pos );
  attr = MAPATTR_GetMapAttribute( mapper, &pos );
  
  if( attr == MAPATTR_ERROR ){
    return( FALSE );
  }
  
  attr_flag = MAPATTR_GetMapAttrFlag( attr );

#ifndef DEBUG_ENCOUNT_CHECKOFF_ATTR
  if( (attr_flag & MAPATTR_FLAGBIT_ENCOUNT) == 0 ){
    return( FALSE );
  }
#endif

  attr_val = MAPATTR_GetMapAttrValue( attr );
  
  { //�莝���|�P�����`�F�b�N
  }
  
  { //�X�v���[�`�F�b�N
  }
  
  { //�G���J�E���g�m���擾
    pre = enc_GetAttrPercent( enc, attr_val, &enc_loc );
  }
  
  { //�p�����^�ɂ��m���ϓ�
  }
  
  if( enc_CheckEncount(enc,pre,attr_val) == TRUE ){ //�G���J�E���g�`�F�b�N
    ret = TRUE;
  }
  
  if( ret == FALSE ){
    OS_Printf( "�t�B�[���h�G���J�E���g�@�͂���\n" );
  }
  return( ret );
}

//======================================================================
//  �T�u�@�G���J�E���g�m��
//======================================================================
//--------------------------------------------------------------
/**
 * �G���J�E���g�����A�g���r���[�g����擾
 * @param enc FIELD_ENCOUNT*
 * @param attr_val �`�F�b�N����MAPATTR_VAL
 * @param outEncLocation �ɃG���J�E���g�ꏊ���i�[����ENCOUNT_LOCATION
 * @retval u32 �G���J�E���g��
 */
//--------------------------------------------------------------
static u32 enc_GetAttrPercent( FIELD_ENCOUNT *enc,
    MAPATTR_VAL attr_val, ENCOUNT_LOCATION *outEncLocation )
{
  u32 per = 0;
  
  {
    //�C�x���g�f�[�^�����G���J�E���g�f�[�^����
    //�A�g���r���[�g�ʂɊm���擾
  }
  
  { //��
    *outEncLocation = ENCOUNT_LOCATION_GROUND;
    per = 30;
  }

  return( per );
}

//======================================================================
//  �T�u�@�G���J�E���g�`�F�b�N
//======================================================================
//--------------------------------------------------------------
/**
 * �G���J�E���g�`�F�b�N
 * @param enc FIELD_ENCOUNT
 * @param pre �G���J�E���g�m��
 * @param attr MAPATTR
 * @retval BOOL TRUE=�G���J�E���g����
 */
//--------------------------------------------------------------
static BOOL enc_CheckEncount(
    FIELD_ENCOUNT *enc, u32 per, const MAPATTR_VAL attr_val )
{
  u32 calc_per,next_per;
  
  if( per > 100 ){ //100%
    per = 100;
  }
  
  calc_per = per << CALC_SHIFT;
  
  if( enc_CheckEncountWalk(enc,calc_per) == FALSE ){
    if( enc->walk_count < WALK_COUNT_MAX ){
      enc->walk_count++; //�����J�E���g
    }
    
    //5%�Ŏ��̏�����
    if( enc_GetPercentRand() >= WALK_NEXT_PERCENT ){
      return( FALSE );
    }
  }
  
  next_per = NEXT_PERCENT; //�G���J�E���g��{�m��
  
  {
    //�A�g���r���[�g�ʊm���ϓ�
    //�������̏ꍇ��+30%
    //���]�Ԃ̏ꍇ��+30%
  }
  
  {
    //���t�ɂ��m���ϓ� 
  }
  
  if( next_per > 100 ){
    next_per = 100;
  }
  
  if( enc_GetPercentRand() < next_per ){ //�ϓ���̊m�����`�F�b�N
    if( enc_GetPercentRand() >= per ){
      return( TRUE );
    }
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �G���J�E���g�`�F�b�N�@�����`�F�b�N
 * @param enc FIELD_ENCOUNT
 * @param per �G���J�E���g�m��
 * @retval BOOL TRUE=�G���J�E���g����
 */
//--------------------------------------------------------------
static BOOL enc_CheckEncountWalk( FIELD_ENCOUNT *enc, u32 per )
{
  per = (per/10) >> CALC_SHIFT;
  
  if( per > WALK_COUNT_GLOBAL ){
    per = WALK_COUNT_GLOBAL;
  }
  
  per = WALK_COUNT_GLOBAL - per;
  
  if( enc->walk_count >= per ){
    return( TRUE );
  }
   
  return( FALSE );
}

//======================================================================
//  �p�[�c
//======================================================================
//--------------------------------------------------------------
/**
 * �����_���m���擾
 * @retval  u32 �m��
 */
//--------------------------------------------------------------
static u32 enc_GetPercentRand( void )
{
  u32 per = (0xffff/100) + 1;
  u32 val = GFUser_GetPublicRand(0xffff) / per;
  return( val );
}

