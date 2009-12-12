//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		weather_snd.h
 *	@brief  �V�C�T�E���h�@
 *	@author	tomoya takahashi
 *	@date		2009.12.12
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif


#include "sound/pm_sndsys.h"
#include "sound/wb_sound_data.sadl"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

// �JSE
#define WEATHER_SND_SE_RAIN   ( SEQ_SE_FLD_32 )

// ��JSE
#define WEATHER_SND_SE_HIGHRAIN   ( SEQ_SE_FLD_31 )

// ��SE
#define WEATHER_SND_SE_SPARK  ( SEQ_SE_FLD_33 )

// ����SE
#define WEATHER_SND_SE_STORM  ( SEQ_SE_FLD_84 )

// ����SE
#define WEATHER_SND_SE_SNOWSTORM  ( SEQ_SE_FLD_84 )

// �����SE
#define WEATHER_SND_SE_ARARE  ( SEQ_SE_FLD_85 )


#ifdef _cplusplus
}	// extern "C"{
#endif



