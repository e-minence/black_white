//======================================================================
/**
 * @file  scrcmd_bsybway.h
 * @brief  �o�g���T�u�E�F�C�@�X�N���v�g�R�}���h��p�\�[�X
 * @author	Miyuki Iwasawa
 * @date  2006.05.23
 *
 * 2007.05.24	Satoshi Nohara*
 *
 * @note pl����ڐA kagaya
 */
//======================================================================
#pragma once
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/vm_cmd.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "script.h"
#include "script_local.h"
#include "script_def.h"
#include "scrcmd.h"
#include "scrcmd_work.h"

#include "bsubway_scr.h"

//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================

//======================================================================
//  extern
//======================================================================
extern BOOL EvCmdBattleTowerWorkInit( VMHANDLE* core, void *wk );
extern BOOL EvCmdBattleTowerWorkClear(VMHANDLE* core, void *wk );
extern BOOL EvCmdBattleTowerWorkRelease(VMHANDLE* core, void *wk );
extern BOOL EvCmdBattleTowerTools( VMHANDLE *core, void *wk );
