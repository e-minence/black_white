//======================================================================
/**
 * @file	palace_warl_check.h
 * @brief�@�p���X���[�v�\�`�F�b�N
 * @author	Saito
 */
//======================================================================

#pragma once

#include <gflib.h>
#include "gamesystem/gamesystem.h"

extern BOOL PLC_WP_CHK_Check(GAMESYS_WORK * gsys);

#ifdef PM_DEBUG
extern void PLC_WP_CHK_CheckDebugPrint(GAMESYS_WORK * gsys);
#endif

