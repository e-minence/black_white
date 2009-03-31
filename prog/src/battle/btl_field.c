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
	TURN_MAX = 16,
};
//--------------------------------------------------------------
/**
 *	�O���[�o�����[�N
 */
//--------------------------------------------------------------
static struct {

	BtlWeather		weather;
	u16						weatherTurn;

}Work;






void BTL_FIELD_Init( BtlWeather weather )
{
	Work.weather = weather;
	Work.weatherTurn = BTL_WEATHER_TURN_PERMANENT;
}

BtlWeather BTL_FIELD_GetWeather( void )
{
	return Work.weather;
}

//=============================================================================================
/**
 * 
 *
 * @param   weather		
 * @param   turn		
 *
 */
//=============================================================================================
void BTL_FIELD_SetWeather( BtlWeather weather, u16 turn )
{
	Work.weather = weather;
	Work.weatherTurn = turn;
	BTL_Printf("�V��ω�=%d\n", weather);
}

void BTL_FIELD_ClearWeather( void )
{
	Work.weather = BTL_WEATHER_NONE;
	Work.weatherTurn = 0;
	BTL_Printf("�V��t���b�g��\n");
}

//=============================================================================================
/**
 * �V��ɂ���ă_���[�W�������E�������郏�U�̑�������Ԃ�
 *
 * @param   waza		���UID
 *
 * @retval  fx32		�������i�p�[�Z���e�[�W�j
 */
//=============================================================================================
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
		if( Work.weatherTurn != BTL_WEATHER_TURN_PERMANENT )
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


//=============================================================================================
/**
 * �ꂪ�Ƃ��Ă��̏�Ԃɂ��邩�ǂ�������
 *
 * @param   state		
 *
 * @retval  BOOL		
 */
//=============================================================================================
BOOL BTL_FIELD_CheckState( BtlFieldState state )
{
	// @@@ �Ƃ肠����
	return FALSE;
}

