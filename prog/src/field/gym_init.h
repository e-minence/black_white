//======================================================================
/**
 * @file  gym_init.h
 * @brief  ジム初期化
 * @author  Saito
 */
//======================================================================
#ifndef __GYM_INIT_H__

#define __GYM_INIT_H__

#include <gflib.h>
#include "gamesystem/gamesystem.h"

extern void GYM_INIT_Elec(GAMESYS_WORK *gsys);
extern void GYM_INIT_Normal(GAMESYS_WORK *gsys, const u8 inRoomNo);
extern void GYM_INIT_Anti(GAMESYS_WORK *gsys);
extern void GYM_INIT_Fly(GAMESYS_WORK *gsys);

#endif  //__GYM_INIT_H__
