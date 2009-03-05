//=============================================================================================
/**
 * @file	btl_field.c
 * @brief	�|�P����WB �o�g���V�X�e��	�V��Ȃǁu��v�ɑ΂�����ʂɊւ��鏈��
 * @author	taya
 *
 * @date	2009.03.04	�쐬
 */
//=============================================================================================
#include <gflib.h>

#include "btl_common.h"
#include "btl_calc.h"

#include "btl_field.h"

//--------------------------------------------------------------
/**
 *	�萔
 */
//--------------------------------------------------------------
enum {
	TURN_PERMANENT = -1,
	TURN_MAX = 16,
};
//--------------------------------------------------------------
/**
 *	�O���[�o�����[�N
 */
//--------------------------------------------------------------
static struct {

	BtlWeather		weather;
	s16						weatherTurn;

}Work;



void BTL_FIELD_Init( BtlWeather weather )
{
	Work.weather = weather;
	Work.weatherTurn = TURN_PERMANENT;
}

BtlWeather BTL_FIELD_GetWeather( void )
{
	return Work.weather;
}

void BTL_FIELD_SetWeather( BtlWeather weather, u16 turn )
{
	Work.weather = weather;
	Work.weatherTurn = turn;
}

void BTL_FIELD_ClearWeather( void )
{
	Work.weather = BTL_WEATHER_NONE;
	Work.weatherTurn = 0;
}

fx32 BTL_FIELD_GetWeatherDmgRatio( WazaID waza )
{
	switch( Work.weather ){
	case BTL_WEATHER_SHINE:
		{
			PokeType type = WAZADATA_GetType( waza );
			if( type == POKETYPE_FIRE ){
				return BTL_CALC_DMG_WEATHER_RATIO_ADVANTAGE;
			}
			if( type == POKETYPE_WATER ){
				return BTL_CALC_DMG_WEATHER_RATIO_DISADVANTAGE;
			}
		}
		break;
	case BTL_WEATHER_RAIN:
		{
			PokeType type = WAZADATA_GetType( waza );
			if( type == POKETYPE_FIRE ){
				return BTL_CALC_DMG_WEATHER_RATIO_DISADVANTAGE;
			}
			if( type == POKETYPE_WATER ){
				return BTL_CALC_DMG_WEATHER_RATIO_ADVANTAGE;
			}
		}
		break;
	default:
		break;
	}
	return BTL_CALC_DMG_WEATHER_RATIO_NONE;
}

//=============================================================================================
/**
 * �^�[���`�F�b�N
 *
 * @retval  BtlWeather		�^�[���`�F�b�N�ɂ��I������V��
 */
//=============================================================================================
BtlWeather BTL_FIELD_TurnCheckWeather( void )
{
	if( Work.weather != BTL_WEATHER_NONE )
	{
		if( Work.weatherTurn != TURN_PERMANENT )
		{
			Work.weatherTurn--;
			if( Work.weatherTurn == 0 )
			{
				BtlWeather endWeather = Work.weather;
				Work.weather = BTL_WEATHER_NONE;
				return endWeather;
			}
		}
	}
	return BTL_WEATHER_NONE;
}



