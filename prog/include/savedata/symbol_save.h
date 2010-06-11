//==============================================================================
/**
 * @file    symbol_save.h
 * @brief   �V���{���G���J�E���g�p�Z�[�u�f�[�^
 * @author  matsuda
 * @date    2010.01.06(��)
 */
//==============================================================================
#pragma once


//==============================================================================
//  �^��`
//==============================================================================
//�V���{���G���J�E���g�Z�[�u�f�[�^
typedef struct _SYMBOL_SAVE_WORK SYMBOL_SAVE_WORK;


//==============================================================================
//  �萔��`
//==============================================================================
enum {
  SYMBOL_POKE_MAX         = 530,         ///<�V���{���|�P�����ő吔

  SYMBOL_SPACE_NONE       = (0xffff),     ///<�Ώۃ]�[���ɋ󂫂��������������l
};

///1�}�b�v(32x32)�ł̃X�g�b�N��
#define SYMBOL_MAP_STOCK_SMALL    (20)
///1�}�b�v(64x64)�ł̃X�g�b�N��
#define SYMBOL_MAP_STOCK_LARGE    (10)
///�L�[�v�]�[���A�t���[�]�[���̒��ōł�1�}�b�v���A�����X�g�b�N�o���鐔
#define SYMBOL_MAP_STOCK_MAX      (SYMBOL_MAP_STOCK_SMALL)


//--------------------------------------------------------------
/**
 * @brief �V���{���|�P�����ێ��̈�w��ID�̒�`
 *
 * ��SymbolZoneTypeDataNo�ƕ��т𓯂��ɂ��Ă������ƁI
 */
//--------------------------------------------------------------
typedef enum{
  SYMBOL_ZONE_TYPE_KEEP_LARGE,      ///<�L�[�v�]�[��(64x64)��p�G���A
  SYMBOL_ZONE_TYPE_KEEP_SMALL,      ///<�L�[�v�]�[��(32x32)��p�G���A
  SYMBOL_ZONE_TYPE_FREE_LARGE,      ///<�t���[�]�[��(64x64)
  SYMBOL_ZONE_TYPE_FREE_SMALL,      ///<�t���[�]�[��(32x32)
  
  SYMBOL_ZONE_TYPE_KEEP_ALL,        ///<�L�[�v�]�[��LARGE��SMALL����
}SYMBOL_ZONE_TYPE;

//--------------------------------------------------------------
///�}�b�v�ԍ�
//--------------------------------------------------------------
typedef enum{
  SYMBOL_MAP_ID_KEEP,               ///<MAP ID �L�[�v�G���A(LARGE & SMALL)
  
  SYMBOL_MAP_ID_FREE_LARGE_START,   ///<MAP ID �t���[(64x64)�J�nNo
  SYMBOL_MAP_ID_FREE_LARGE_END = SYMBOL_MAP_ID_FREE_LARGE_START + 3,
  
  SYMBOL_MAP_ID_FREE_SMALL_START = SYMBOL_MAP_ID_FREE_LARGE_END,  ///<MAP ID �t���[(32x32)�J�nNo

  SYMBOL_MAP_ID_ENTRANCE  = 4,  ///< MAP ID �������ŏ��̂Ƃ���
}SYMBOL_MAP_ID;

//--------------------------------------------------------------
///�V���{���}�b�v(64x64)�̃}�b�v���x��(�������傫���قǍL��)
//--------------------------------------------------------------
typedef enum{
  SYMBOL_MAP_LEVEL_LARGE_NONE,      ///<�}�b�v�����݂��Ȃ�
  SYMBOL_MAP_LEVEL_LARGE_LEVEL_1,
  
  SYMBOL_MAP_LEVEL_LARGE_MAX = SYMBOL_MAP_LEVEL_LARGE_LEVEL_1,
}SYMBOL_MAP_LEVEL_LARGE;

//--------------------------------------------------------------
///�V���{���}�b�v(32x32)�̃}�b�v���x��(�������傫���قǍL��)
//--------------------------------------------------------------
typedef enum{
  SYMBOL_MAP_LEVEL_SMALL_1,
  SYMBOL_MAP_LEVEL_SMALL_2,
  SYMBOL_MAP_LEVEL_SMALL_3,
  SYMBOL_MAP_LEVEL_SMALL_4,
  SYMBOL_MAP_LEVEL_SMALL_5,
  SYMBOL_MAP_LEVEL_SMALL_6,
  SYMBOL_MAP_LEVEL_SMALL_7,
  
  SYMBOL_MAP_LEVEL_SMALL_MAX = SYMBOL_MAP_LEVEL_SMALL_7,
}SYMBOL_MAP_LEVEL_SMALL;


//==============================================================================
//  �\���̒�`
//==============================================================================
//--------------------------------------------------------------
///�V���{���G���J�E���g1�C���̃Z�[�u
//--------------------------------------------------------------
typedef struct _SYMBOL_POKEMON{
  u32 monsno:11;        ///<�|�P�����ԍ�
  u32 wazano:10;        ///<�Z�ԍ�
  u32 sex:2;            ///<����(PTL_SEX_MALE, PTL_SEX_FEMALE, PTL_SEX_UNKNOWN)
  u32 form_no:6;        ///<�t�H�����ԍ�
  u32 move_type:3;      ///<����^�C�v
}SYMBOL_POKEMON;


//--------------------------------------------------------------
///SYMBOL_ZONE_TYPE���̔z�uNo�̊J�n�ʒu�ƏI�[�ʒu���Ǘ�����\����
//--------------------------------------------------------------
typedef struct{
  u16 start;
  u16 end;
}SYMBOL_ZONE_TYPE_DATA_NO;


//==============================================================================
//  �O���֐��錾
//==============================================================================
//----------------------------------------------------------
//	�Z�[�u�f�[�^�擾�̂��߂̊֐�
//----------------------------------------------------------
extern SYMBOL_SAVE_WORK* SymbolSave_GetSymbolData(SAVE_CONTROL_WORK* pSave);
extern u32 SymbolSave_GetWorkSize( void );
extern void SymbolSave_WorkInit(void *work);


//--------------------------------------------------------------
//  �A�N�Z�X�֐�
//--------------------------------------------------------------
extern BOOL SymbolSave_GetSymbolPokemon(SYMBOL_SAVE_WORK *symbol_save, u32 no, SYMBOL_POKEMON* pSymbol);

extern u32 SymbolSave_CheckSpace(
  SYMBOL_SAVE_WORK *symbol_save, SYMBOL_ZONE_TYPE zone_type);
extern void SymbolSave_DataShift(SYMBOL_SAVE_WORK *symbol_save, u32 no);
extern void SymbolSave_SetFreeZone(SYMBOL_SAVE_WORK *symbol_save,
    u16 monsno, u16 wazano, u8 sex, u8 form_no, u8 move_type, SYMBOL_ZONE_TYPE zone_type);

static inline u32 SymbolSave_CheckFreeZoneSpace(
    SYMBOL_SAVE_WORK *symbol_save, SYMBOL_ZONE_TYPE zone_type)
{ return SymbolSave_CheckSpace( symbol_save, zone_type ); }

//--------------------------------------------------------------
//  �c�[���֐�
//--------------------------------------------------------------
extern SYMBOL_ZONE_TYPE SYMBOLZONE_GetZoneTypeFromNumber(u32 no);
extern int SymbolSave_CheckFlashLoad(SAVE_CONTROL_WORK *ctrl, const SYMBOL_POKEMON *spoke, HEAPID heap_id, BOOL pp_mode);

//==============================================================================
//  �O���f�[�^
//==============================================================================
extern const SYMBOL_ZONE_TYPE_DATA_NO SymbolZoneTypeDataNo[];



//==============================================================================
//  �O���f�[�^�A�N�Z�X�̂��߂̃C�����C���֐�
//==============================================================================
//--------------------------------------------------------------
/**
 * SYMBOL_ZONE_TYPE���Ƃ̕ێ��f�[�^�ő吔���擾
 * @param zone_type
 * @return  u16     �ێ��f�[�^�ő吔
 */
//--------------------------------------------------------------
static inline u16 SYMBOLZONE_GetStockMax( SYMBOL_ZONE_TYPE zone_type )
{
  return SymbolZoneTypeDataNo[zone_type].end - SymbolZoneTypeDataNo[zone_type].start;
}

//--------------------------------------------------------------
/// SYMBOL_ZONE_TYPE���Ƃ̕ێ��f�[�^�J�n�i���o�[���擾
//--------------------------------------------------------------
static inline u16 SYMBOLZONE_GetStartNo( SYMBOL_ZONE_TYPE zone_type )
{
  return SymbolZoneTypeDataNo[zone_type].start;
}

//--------------------------------------------------------------
/// SYMBOL_ZONE_TYPE���Ƃ̕ێ��f�[�^�I���i���o�[���擾
//--------------------------------------------------------------
static inline u16 SYMBOLZONE_GetEndNo( SYMBOL_ZONE_TYPE zone_type )
{
  return SymbolZoneTypeDataNo[zone_type].end;
}


