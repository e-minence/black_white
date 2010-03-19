//==============================================================================
/**
 * @file    intrude_symbol.h
 * @brief   �N���F�V���{���|�P����
 * @author  matsuda
 * @date    2010.03.04(��)
 */
//==============================================================================
#pragma once

#include "savedata/symbol_save.h"



//==============================================================================
//  �\���̒�`
//==============================================================================
///�ʐM�ł���肷�鑊��̃V���{���}�b�v�p�����[�^(1�}�b�v��)
typedef struct{
  SYMBOL_POKEMON spoke_array[SYMBOL_MAP_STOCK_MAX];   ///<1�}�b�v���̃V���{���|�P�����f�[�^�̔z��|�C���^
  u32 num:6;                       ///<spoke_array�̗L���ȃf�[�^�������Ă��鐔
  u32 map_level_small:4;           ///<SYMBOL_MAP_LEVEL_SMALL
  u32 map_level_large:4;           ///<SYMBOL_MAP_LEVEL_LARGE
  u32 net_id:4;                    ///<���̃V���{���f�[�^���������NetID
  u32 symbol_map_id:6;             ///<�V���{���}�b�vID
  u32 padding:8;
}INTRUDE_SYMBOL_WORK;

///�V���{���f�[�^���N�G�X�g�\����
typedef struct{
  u8 symbol_map_id;
  u8 occ;             ///<TRUE:���N�G�X�g�L��
  u8 padding[2];
}SYMBOL_DATA_REQ;

///�V���{���f�[�^�ύX�ʒm�\����
typedef struct{
  u8 symbol_map_id;
  u8 occ;
  u8 padding[2];
}SYMBOL_DATA_CHANGE;


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern NetID IntrudeSymbol_CheckIntrudeNetID(GAME_COMM_SYS_PTR game_comm, GAMEDATA *gamedata);
extern BOOL IntrudeSymbol_ReqSymbolData(INTRUDE_COMM_SYS_PTR intcomm, SYMBOL_MAP_ID symbol_map_id);
extern BOOL IntrudeSymbol_CheckRecvSymbolData(INTRUDE_COMM_SYS_PTR intcomm);
extern INTRUDE_SYMBOL_WORK * IntrudeSymbol_GetSymbolBuffer(INTRUDE_COMM_SYS_PTR intcomm);
extern void IntrudeSymbol_SendSymbolDataChange(INTRUDE_COMM_SYS_PTR intcomm, SYMBOL_MAP_ID symbol_map_id);
extern BOOL IntrudeSymbol_CheckSymbolDataChange(INTRUDE_COMM_SYS_PTR intcomm);
