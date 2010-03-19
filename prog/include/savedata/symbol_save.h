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
///�V���{���|�P�����ő吔
#define SYMBOL_POKE_MAX           (530)

///�z�uNo
enum{
  SYMBOL_NO_START_KEEP_LARGE = 0,                             ///<�L�[�v�]�[��(64x64)�J�nNo
  SYMBOL_NO_END_KEEP_LARGE = SYMBOL_NO_START_KEEP_LARGE + 10, ///<�L�[�v�]�[��(64x64)�I��No
  SYMBOL_NO_START_KEEP_SMALL = SYMBOL_NO_END_KEEP_LARGE,      ///<�L�[�v�]�[��(32x32)�J�nNo
  SYMBOL_NO_END_KEEP_SMALL = SYMBOL_NO_START_KEEP_SMALL + 10, ///<�L�[�v�]�[��(32x32)�I��No
  
  SYMBOL_NO_START_FREE_LARGE = SYMBOL_NO_END_KEEP_SMALL,      ///<�t���[�]�[��(64x64)�J�nNo
  SYMBOL_NO_END_FREE_LARGE = SYMBOL_NO_START_FREE_LARGE + 30, ///<�t���[�]�[��(64x64)�I��No
  SYMBOL_NO_START_FREE_SMALL = SYMBOL_NO_END_FREE_LARGE,      ///<�t���[�]�[��(32x32)�J�nNo
  SYMBOL_NO_END_FREE_SMALL = SYMBOL_NO_START_FREE_SMALL + 480,///<�t���[�]�[��(32x32)�I��No
};

/// ��SymbolZoneTypeDataNo�ƕ��т𓯂��ɂ��Ă������ƁI
typedef enum{
  SYMBOL_ZONE_TYPE_KEEP_LARGE,      ///<�L�[�v�]�[��(64x64)��p�G���A
  SYMBOL_ZONE_TYPE_KEEP_SMALL,      ///<�L�[�v�]�[��(32x32)��p�G���A
  SYMBOL_ZONE_TYPE_FREE_LARGE,      ///<�t���[�]�[��(64x64)
  SYMBOL_ZONE_TYPE_FREE_SMALL,      ///<�t���[�]�[��(32x32)
  
  SYMBOL_ZONE_TYPE_KEEP_ALL,        ///<�L�[�v�]�[��LARGE��SMALL����
}SYMBOL_ZONE_TYPE;

///�Ώۃ]�[���ɋ󂫂��������������l
#define SYMBOL_SPACE_NONE        (0xffff)

///�V���{���}�b�v(64x64)�̃}�b�v���x��(�������傫���قǍL��)
typedef enum{
  SYMBOL_MAP_LEVEL_LARGE_NONE,      ///<�}�b�v�����݂��Ȃ�
  SYMBOL_MAP_LEVEL_LARGE_LEVEL_1,
  
  SYMBOL_MAP_LEVEL_LARGE_MAX = SYMBOL_MAP_LEVEL_LARGE_LEVEL_1,
}SYMBOL_MAP_LEVEL_LARGE;

///�V���{���}�b�v(32x32)�̃}�b�v���x��(�������傫���قǍL��)
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

///1�}�b�v(32x32)�ł̃X�g�b�N��
#define SYMBOL_MAP_STOCK_SMALL    (30)
///1�}�b�v(64x64)�ł̃X�g�b�N��
#define SYMBOL_MAP_STOCK_LARGE    (10)
///�L�[�v�]�[���A�t���[�]�[���̒��ōł�1�}�b�v���A�����X�g�b�N�o���鐔
#define SYMBOL_MAP_STOCK_MAX      (SYMBOL_MAP_STOCK_SMALL)


//==============================================================================
//  �\���̒�`
//==============================================================================
///�V���{���G���J�E���g1�C���̃Z�[�u
typedef struct _SYMBOL_POKEMON{
  u32 monsno:11;        ///<�|�P�����ԍ�
  u32 wazano:10;        ///<�Z�ԍ�
  u32 sex:2;            ///<����(PTL_SEX_MALE, PTL_SEX_FEMALE, PTL_SEX_UNKNOWN)
  u32 form_no:6;        ///<�t�H�����ԍ�
  u32        :3;        //�]��
}SYMBOL_POKEMON;


///SYMBOL_ZONE_TYPE���̔z�uNo�̊J�n�ʒu�ƏI�[�ʒu���Ǘ�����\����
typedef struct{
  u16 start;
  u16 end;
}SYMBOL_ZONE_TYPE_DATA_NO;


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern u32 SymbolSave_GetWorkSize( void );
extern void SymbolSave_WorkInit(void *work);

//----------------------------------------------------------
//	�Z�[�u�f�[�^�擾�̂��߂̊֐�
//----------------------------------------------------------
extern SYMBOL_SAVE_WORK* SymbolSave_GetSymbolData(SAVE_CONTROL_WORK* pSave);

//--------------------------------------------------------------
//  �A�N�Z�X�֐�
//--------------------------------------------------------------
extern u32 SymbolSave_CheckFreeZoneSpace(SYMBOL_SAVE_WORK *symbol_save, SYMBOL_ZONE_TYPE zone_type);
extern void SymbolSave_DataShift(SYMBOL_SAVE_WORK *symbol_save, u32 no);
extern void SymbolSave_SetFreeZone(SYMBOL_SAVE_WORK *symbol_save, u16 monsno, u16 wazano, u8 sex, u8 form_no, SYMBOL_ZONE_TYPE zone_type);

//==============================================================================
//  �O���f�[�^
//==============================================================================
extern const SYMBOL_ZONE_TYPE_DATA_NO SymbolZoneTypeDataNo[];
