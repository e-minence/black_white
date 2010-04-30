//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		pm_weather.h
 *	@brief  �V�C�Ǘ��@�ړ��|�P�����@�]�[���@�J�����_�[
 *	@author	tomoya takahashi
 *	@date		2010.01.29
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#include "gamesystem.h"


//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
// ����WeatherNo���擾
extern u32 GAMEDATA_GetWeatherNo( const GAMEDATA* cp_data );
extern void GAMEDATA_SetWeatherNo( GAMEDATA* p_data, u8 weather_no );


// ���̎w��ZONE�̓V�C���擾����
extern u8 PM_WEATHER_GetZoneWeatherNo( GAMESYS_WORK* p_gamesystem, int zone_id );

// ZoneChange�ł̓V�C�X�V����
extern void PM_WEATHER_UpdateZoneChangeWeatherNo( GAMESYS_WORK* p_gamesystem, int zone_id );

// �Z�[�u���A�ł̓V�C�X�V����
extern void PM_WEATHER_UpdateSaveLoadWeatherNo( GAMEDATA* p_data, int zone_id );



#ifdef _cplusplus
}	// extern "C"{
#endif



