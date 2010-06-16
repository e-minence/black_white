//==============================================================================
/**
 * @file    symbol_save_local.h
 * @brief   �V���{���|�P�����Z�[�u�p���[�J���w�b�_
 * @author  matsuda
 * @date    2010.03.03(��)
 */
//==============================================================================
#pragma once



//==============================================================================
//  �\���̒�`
//==============================================================================
typedef struct{
  u16 crc16ccitt_hash;  ///<CRC
  u16 coded_number;   ///<�Í����L�[
}RECORD_CRC;

///�V���{���G���J�E���g�Z�[�u�f�[�^
struct _SYMBOL_SAVE_WORK{
  struct _SYMBOL_POKEMON symbol_poke[SYMBOL_POKE_MAX]; ///<�V���{���|�P����
  u8 map_level_large;   ///<�V���{���}�b�v(64x64)�̑傫��(SYMBOL_MAP_LEVEL_LARGE)
  u8 map_level_small;   ///<�V���{���}�b�v(32x32)�̑傫��(SYMBOL_MAP_LEVEL_SMALL)
  u8 padding[2];
  RECORD_CRC crc;  ///< �Í����R�[�h
};

//==============================================================================
//  �萔��`
//==============================================================================
//--------------------------------------------------------------
///�z�uNo
//--------------------------------------------------------------
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

//==============================================================================
//  �O���Q�Ɛ錾
//==============================================================================
//--------------------------------------------------------------
//  �ȉ��̊֐���SymbolSave�������炵���Ăяo�����Ȃ�
//--------------------------------------------------------------
extern void SymbolSave_Local_Encode(SYMBOL_SAVE_WORK *pSymbol);
extern void SymbolSave_Local_Decode(SYMBOL_SAVE_WORK *pSymbol);
extern void SymbolSave_Local_CalcSmallLevel( SYMBOL_SAVE_WORK * symbol_save, u16 new_no );

