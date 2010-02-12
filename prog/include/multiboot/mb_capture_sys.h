//======================================================================
/**
 * @file	mb_capture_sys.h
 * @brief	�}���`�u�[�g�E�ߊl�Q�[��
 * @author	ariizumi
 * @data	09/11/24
 *
 * ���W���[�����FMB_CAPTURE
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
  HEAPID parentHeap;
  DLPLAY_CARD_TYPE cardType;
  ARCHANDLE *arcHandle;
  void *ppp[MB_CAP_POKE_NUM];
  u16  highScore;
  
  //���œ��鐔�l
  u16  score;
  BOOL isCapture[MB_CAP_POKE_NUM];
}MB_CAPTURE_INIT_WORK;


//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
extern GFL_PROC_DATA MultiBootCapture_ProcData;


