//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		weather_rain.h
 *	@brief		�V�C�J
 *	@author		tomoya takahashi
 *	@data		2009.03.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WEATHER_RAIN_H__
#define __WEATHER_RAIN_H__

#ifdef _cplusplus
extern "C"{
#endif

#include "weather_task.h"

FS_EXTERN_OVERLAY(field_weather_rain);

//-----------------------------------------------------------------------------
/**
 *		���ʂ̉J
*/
//-----------------------------------------------------------------------------
extern WEATHER_TASK_DATA c_WEATHER_TASK_DATA_RAIN;

#ifdef _cplusplus
}	// extern "C"{
#endif

#endif		// __WEATHER_RAIN_H__


