//======================================================================
/**
 * @file	cg_help.h
 * @brief	C-Gearヘルプ画面
 * @author	ariizumi
 * @data	10/03/22
 *
 * モジュール名：CG_HELP_
 */
//======================================================================

#include "savedata/mystatus.h"
#include "gamesystem/gamesystem.h"

#pragma once

//======================================================================
//	define
//======================================================================
#pragma mark [> define
FS_EXTERN_OVERLAY(cg_help);


//======================================================================
//	enum
//======================================================================
#pragma mark [> enum
typedef struct
{
  MYSTATUS *myStatus;
  GAMESYS_WORK *gameSys;
}CG_HELP_INIT_WORK;

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
extern GFL_PROC_DATA CGearHelp_ProcData;

//--------------------------------------------------------------
//	
//--------------------------------------------------------------