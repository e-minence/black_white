//=============================================================================================
/**
 * @file	btl_field.h
 * @brief	�|�P����WB �o�g���V�X�e��	�V��Ȃǁu��v�ɑ΂�����ʂɊւ��鏈��
 * @author	taya
 *
 * @date	2009.03.04	�쐬
 */
//=============================================================================================
#ifndef __BTL_FIELD_H__
#define __BTL_FIELD_H__

#include "battle\battle.h"

/**
 *	�o�g���Ǝ��̃t�B�[���h��ԁi�d����������j
 */
typedef enum {
	BTL_FLDSTATE_NONE = 0,

	BTL_FLDSTATE_TRICKROOM,		///< �g���b�N���[��
	BTL_FLDSTATE_GRAVITY,			///< ���イ��傭

	BTL_FLDSTATE_MAX,

}BtlFieldState;


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/

extern void BTL_FIELD_Init( BtlWeather weather );
extern BtlWeather BTL_FIELD_GetWeather( void );
extern void BTL_FIELD_SetWeather( BtlWeather weather, u16 turn );
extern void BTL_FIELD_ClearWeather( void );

extern BOOL BTL_FIELD_CheckState( BtlFieldState state );

//=============================================================================================
/**
 * �V��ɂ���ă_���[�W�������E�������郏�U�̑�������Ԃ�
 *
 * @param   waza		���UID
 *
 * @retval  fx32		�������i�p�[�Z���e�[�W�j
 */
//=============================================================================================
extern fx32 BTL_FIELD_GetWeatherDmgRatio( WazaID waza );


extern BtlWeather BTL_FIELD_TurnCheckWeather( void );

#endif
