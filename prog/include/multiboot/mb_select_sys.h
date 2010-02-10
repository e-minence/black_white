//======================================================================
/**
 * @file	mb_select_sys.h
 * @brief	�}���`�u�[�g�E�|�P�����I��
 * @author	ariizumi
 * @data	09/11/19
 *
 * ���W���[�����FMB_SELECT
 */
//======================================================================
#pragma once

#include "multiboot/mb_local_def.h"
#include "multiboot/mb_data_def.h"
#include "poke_tool/poke_tool.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
typedef struct
{
  int msgSpeed;
  HEAPID parentHeap;
  DLPLAY_CARD_TYPE cardType;
  
  //�{�b�N�X�f�[�^
  STRCODE *boxName[MB_POKE_BOX_TRAY];
  POKEMON_PASO_PARAM *boxData[MB_POKE_BOX_TRAY][MB_POKE_BOX_POKE];
  
  //�߂�l�p
  //(0:�{�b�N�X�ԍ� 1:�C���f�b�N�X(0xFF�s���l
  u8   selectPoke[MB_CAP_POKE_NUM][2];
  BOOL isCancel;
}MB_SELECT_INIT_WORK;


//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
extern GFL_PROC_DATA MultiBootSelect_ProcData;


