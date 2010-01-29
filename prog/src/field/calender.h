////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �J�����_�[
 * @file   calender.h
 * @author obata
 * @date   2010.01.29
 */
////////////////////////////////////////////////////////////////////////////////
#pragma once 
#include <gflib.h>
#include "gamesystem/gamedata_def.h"
#include "field/weather_no.h"

typedef struct _CALENDER CALENDER;


// �쐬 / �j��
extern CALENDER* CALENDER_Create( GAMEDATA* gameData, HEAPID heapID );
extern void      CALENDER_Delete( CALENDER* calender );

// �V�C
extern WEATHER_NO CALENDER_GetWeather      ( const CALENDER* calender, u16 zoneID, u8 month, u8 day );
extern WEATHER_NO CALENDER_GetWeather_today( const CALENDER* calender, u16 zoneID );
