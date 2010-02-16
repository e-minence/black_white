//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		weather_mist.h
 *	@brief  �V�C�F��
 *	@author	tomoya takahashi
 *	@date		2010.02.11
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#include "weather_task.h"

FS_EXTERN_OVERLAY(field_weather_mist);

//-----------------------------------------------------------------------------
/**
 *		��
*/
//-----------------------------------------------------------------------------
extern WEATHER_TASK_DATA c_WEATHER_TASK_DATA_MIST;
extern WEATHER_TASK_DATA c_WEATHER_TASK_DATA_PALACE_WHITE_MIST;
extern WEATHER_TASK_DATA c_WEATHER_TASK_DATA_PALACE_BLACK_MIST;


#ifdef _cplusplus
}	// extern "C"{
#endif



