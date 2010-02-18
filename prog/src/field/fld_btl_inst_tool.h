//======================================================================
/**
 * @file  fld_btl_inst_tool.h
 * @brief  �{�݃o�g���Ăяo�����̋��ʕ���
 *
 * @note  �{�݁Finstitution
 * ���W���[�����F
 */
//======================================================================
#pragma once

#include "app/pokelist.h"
#include "gamesystem/gamesystem.h"

extern GMEVENT *FBI_TOOL_CreatePokeListEvt(
    GAMESYS_WORK *gsys,
    const PL_LIST_TYPE inType, const int inReg, POKEPARTY *pp,
    u8 *outSelNoAry, PL_SELECT_POS *outResult, PL_RETURN_TYPE *outRetMode );

