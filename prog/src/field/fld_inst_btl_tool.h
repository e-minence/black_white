//======================================================================
/**
 * @file  fld_inst_btl_tool.h
 * @brief  �{�݃o�g���Ăяo�����̋��ʕ���
 *
 * @note  �{�݁Finstitution
 * ���W���[�����F
 */
//======================================================================
#pragma once

#include "app/pokelist.h"
#include "gamesystem/gamesystem.h"

extern GMEVENT *FIB_TOOL_CreatePokeListEvt(
    GAMESYS_WORK *gsys,
    const int inType, const int inReg, POKEPARTY *pp,
    u8 *outSelNoAry, PL_SELECT_POS *outResult, PL_RETURN_TYPE *outRetMode );

