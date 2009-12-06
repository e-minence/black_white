//======================================================================
/**
 * @file	mb_select_sys.h
 * @brief	マルチブート・ポケモン選択
 * @author	ariizumi
 * @data	09/11/19
 *
 * モジュール名：MB_SELECT
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
  
  //ボックスデータ
  STRCODE *boxName[MB_POKE_BOX_TRAY];
  void *boxData[MB_POKE_BOX_TRAY][MB_POKE_BOX_POKE];
  
  //戻り値用
  //(0:ボックス番号 1:インデックス(0xFF不正値
  u8   selectPoke[MB_CAP_POKE_NUM][2];
  //pppサイズ分のワークを確保しておいてください。
  void *ppp[MB_CAP_POKE_NUM];
}MB_SELECT_INIT_WORK;


//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
extern GFL_PROC_DATA MultiBootSelect_ProcData;


