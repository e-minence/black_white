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
  u8 num:6;                       ///<spoke_array�̗L���ȃf�[�^�������Ă��鐔
  u8 zone_type:2;                 ///<SYMBOL_ZONE_TYPE
  u8 map_level_small:4;           ///<SYMBOL_MAP_LEVEL_SMALL
  u8 map_level_large:4;           ///<SYMBOL_MAP_LEVEL_LARGE
  u8 net_id:4;                    ///<���̃V���{���f�[�^���������NetID
  u8 map_no:4;                    ///<�}�b�v�ԍ�
  u8 padding;
}INTRUDE_SYMBOL_WORK;

///�V���{���f�[�^���N�G�X�g�\����
typedef struct{
  u8 zone_type;
  u8 map_no;
  u8 occ;             ///<TRUE:���N�G�X�g�L��
  u8 padding;
}SYMBOL_DATA_REQ;

///�V���{���f�[�^�ύX�ʒm�\����
typedef struct{
  u8 zone_type;
  u8 map_no;
  u8 occ;
  u8 padding;
}SYMBOL_DATA_CHANGE;


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern BOOL IntrudeSymbol_ReqSymbolData(INTRUDE_COMM_SYS_PTR intcomm, SYMBOL_ZONE_TYPE zone_type, u8 map_no);
extern BOOL IntrudeSymbol_CheckRecvSymbolData(INTRUDE_COMM_SYS_PTR intcomm);
extern INTRUDE_SYMBOL_WORK * IntrudeSymbol_GetSymbolBuffer(INTRUDE_COMM_SYS_PTR intcomm);
extern void IntrudeSymbol_SendSymbolDataChange(INTRUDE_COMM_SYS_PTR intcomm, SYMBOL_ZONE_TYPE zone_type, u8 map_no);
extern BOOL IntrudeSymbol_CheckSymbolDataChange(INTRUDE_COMM_SYS_PTR intcomm);
