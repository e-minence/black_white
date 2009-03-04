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

//=============================================================================================
/**
 * �^�[���`�F�b�N
 *
 * @retval  BOOL		�^�[���`�F�b�N�ɂ��I��������ʂ�����ꍇTRUE
 */
//=============================================================================================
BOOL BTL_FIELD_TurnCheck( void )
{
	if( Work.weather != BTL_WEATHER_NONE )
	{
		if( Work.weatherTurn != TURN_PERMANENT )
		{
			Work.weatherTurn--;
			if( Work.weatherTurn == 0 )
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

