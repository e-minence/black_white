//======================================================================
/**
 * @file  gym_init.h
 * @brief  �W��������
 * @author  Saito
 */
//======================================================================
#pragma once

#include <gflib.h>
#include "gamesystem/gamesystem.h"

extern void GYM_INIT_Elec(GAMESYS_WORK *gsys, const u16 inEvtFlgBtl1, const u16 inEvtFlgBtl2);
extern void GYM_INIT_Normal(GAMESYS_WORK *gsys, const BOOL inOpened);
extern void GYM_INIT_Anti(GAMESYS_WORK *gsys, const int inCurtain);
extern void GYM_INIT_Fly(GAMESYS_WORK *gsys);
extern void GYM_INIT_Insect(GAMESYS_WORK *gsys, u16 **sw_work);
extern void GYM_INIT_Dragon(GAMESYS_WORK *gsys);

