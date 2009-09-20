//============================================================================================
/**
 * @file	warpdata.h
 * @brief	���[�v�p�f�[�^�֘A�w�b�_
 * @date	2005.12.19
 *
 * 2009.09.20 HGSS����ڐA�J�n
 */
//============================================================================================

#pragma once


#include "gamesystem/game_data.h"
#include "field/location.h"

//--------------------------------------------------------------------------------------------
//	���[�v�Ƃѐ�
//--------------------------------------------------------------------------------------------
extern void WARPDATA_GetWarpLocation(int warp_id, LOCATION * loc);
extern void WARPDATA_GetRevivalLocation(int warp_id, LOCATION * loc);
extern void WARPDATA_GetEscapeLocation(int warp_id, LOCATION * loc);
extern int WARPDATA_SearchByRoomID(int zone_id);
extern int WARPDATA_SearchByFieldID(int zone_id);
extern int WARPDATA_GetInitializeID(void);
extern int WARPDATA_SearchForFly(int zone_id);

//--------------------------------------------------------------------------------------------
//	�����t���O
//--------------------------------------------------------------------------------------------
extern void ARRIVEDATA_SetArriveFlag( GAMEDATA * gamedata, int zone_id );
extern BOOL ARRIVEDATA_GetArriveFlag( GAMEDATA * gamedata, int warp_id );


