//======================================================================
/**
 * @file	ctvt_beacon.c
 * @brief	通信トランシーバー：ビーコン
 * @author	ariizumi
 * @data	09/12/22
 *
 * モジュール名：CTVT_BCON
 */
//======================================================================

#pragma once
#include "savedata/mystatus_local.h"
#include "savedata/mystatus.h"

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
//ビーコンデータ
struct _CTVT_COMM_BEACON
{
  MYSTATUS myStatus;
  
  u8      connectNum;
  u8      pad;
  //呼び出しターゲット
  u8      callTarget[3][6];
};

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
