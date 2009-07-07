//=============================================================================================
/**
 * @file  btl_field.h
 * @brief �|�P����WB �o�g���V�X�e�� �V��Ȃǁu��v�ɑ΂�����ʂɊւ��鏈��
 * @author  taya
 *
 * @date  2009.03.04  �쐬
 */
//=============================================================================================
#ifndef __BTL_FIELD_H__
#define __BTL_FIELD_H__

#include "battle\battle.h"
#include "btl_util.h"


/**
 *  �o�g���Ǝ��̃t�B�[���h��ԁi�d����������j
 */
typedef enum {
  BTL_FLDEFF_NULL = 0,

  BTL_FLDEFF_WEATHER,     ///< �e��V��
  BTL_FLDEFF_TRICKROOM,   ///< �g���b�N���[��
  BTL_FLDEFF_JURYOKU,     ///< ���イ��傭
  BTL_FLDEFF_FUIN,        ///< �ӂ�����
  BTL_FLDEFF_MIZUASOBI,   ///< �݂�������
  BTL_FLDEFF_DOROASOBI,   ///< �ǂ날����

  BTL_FLDEFF_MAX,

}BtlFieldEffect;


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/

extern void BTL_FIELD_Init( BtlWeather weather );
extern BtlWeather BTL_FIELD_GetWeather( void );
extern void BTL_FIELD_SetWeather( BtlWeather weather, u16 turn );
extern void BTL_FIELD_ClearWeather( void );

extern BOOL BTL_FIELD_AddEffect( BtlFieldEffect effect, BPP_SICK_CONT cont );
extern void BTL_FIELD_RemoveEffect( BtlFieldEffect state );
extern void BTL_FIELD_RemoveDependPokeEffect( u8 pokeID );
extern BOOL BTL_FIELD_CheckEffect( BtlFieldEffect state );



//=============================================================================================
/**
 * �V��ɂ���ă_���[�W�������E�������郏�U�̑�������Ԃ�
 *
 * @param   waza    ���UID
 *
 * @retval  fx32    �������i�p�[�Z���e�[�W�j
 */
//=============================================================================================
extern fx32 BTL_FIELD_GetWeatherDmgRatio( WazaID waza );


extern BtlWeather BTL_FIELD_TurnCheckWeather( void );


typedef void (*pFieldTurnCheckCallback)( BtlFieldEffect, void* );
extern void BTL_FIELD_TurnCheck( pFieldTurnCheckCallback callbackFunc, void* callbackArg);


#endif
