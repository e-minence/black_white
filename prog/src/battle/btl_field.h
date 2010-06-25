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

#include "btl_main_def.h"
#include "btl_util.h"


/**
 *  �o�g���Ǝ��̃t�B�[���h��ԁi�d����������j
 */
typedef enum {

  BTL_FLDEFF_WEATHER,     ///< �e��V��
  BTL_FLDEFF_TRICKROOM,   ///< �g���b�N���[��
  BTL_FLDEFF_JURYOKU,     ///< ���イ��傭
  BTL_FLDEFF_FUIN,        ///< �ӂ�����
  BTL_FLDEFF_MIZUASOBI,   ///< �݂�������
  BTL_FLDEFF_DOROASOBI,   ///< �ǂ날����
  BTL_FLDEFF_WONDERROOM,  ///< �����_�[���[��
  BTL_FLDEFF_MAGICROOM,   ///< �}�W�b�N���[��

  BTL_FLDEFF_MAX,
  BTL_FLDEFF_NULL = BTL_FLDEFF_MAX,

}BtlFieldEffect;


/*--------------------------------------------------------------------------*/
/*                                                                          */
/*--------------------------------------------------------------------------*/
typedef struct _BTL_FIELD_WORK    BTL_FIELD_WORK;


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/

extern void BTL_FIELD_Init( BtlWeather weather );
extern BtlWeather BTL_FIELD_GetWeather( void );
extern u32 BTL_FIELD_GetWeatherTurn( void );
extern void BTL_FIELD_SetWeather( BtlWeather weather, u16 turn );

extern BOOL BTL_FIELD_AddEffect( BtlFieldEffect effect, BPP_SICK_CONT cont );
extern BOOL BTL_FIELD_AddDependPoke( BtlFieldEffect effect, u8 pokeID );
extern BOOL BTL_FIELD_IsDependPoke( BtlFieldEffect effect, u8 pokeID );
extern BOOL BTL_FIELD_RemoveEffect( BtlFieldEffect state );
extern void BTL_FIELD_RemoveDependPokeEffect( u8 pokeID );
extern BOOL BTL_FIELD_CheckEffect( BtlFieldEffect state );
extern u8 BTL_FIELD_GetDependPokeID( BtlFieldEffect effect );


extern BOOL BTL_FIELD_CheckFuin( const BTL_POKE_CONTAINER* pokeCon, const BTL_POKEPARAM* attacker, WazaID waza );

extern BtlWeather BTL_FIELD_TurnCheckWeather( void );


typedef void (*pFieldTurnCheckCallback)( BtlFieldEffect, void* );
extern void BTL_FIELD_TurnCheck( pFieldTurnCheckCallback callbackFunc, void* callbackArg);


/*=============================================================================================*/
/*                                                                                             */
/* BTL_FIELD �Ɠ����̋@�\���N���C�A���g���Ɏ�������ׂ̎d�g��                                  */
/*                                                                                             */
/*=============================================================================================*/
extern BTL_FIELD_WORK*  BTL_FIELDSIM_CreateWork( HEAPID heapID );
extern void BTL_FIELDSIM_DeleteWork( BTL_FIELD_WORK* wk );

extern void BTL_FIELDSIM_Init( BTL_FIELD_WORK* wk, BtlWeather weather );
extern void BTL_FIELDSIM_SetWeather( BTL_FIELD_WORK* wk, BtlWeather weather, u16 turn );
extern void BTL_FIELDSIM_EndWeather( BTL_FIELD_WORK* wk );
extern BtlWeather BTL_FIELDSIM_GetWeather( const BTL_FIELD_WORK* wk );
extern u32 BTL_FIELDSIM_GetWeatherTurn( const BTL_FIELD_WORK* wk );
extern void BTL_FIELDSET_SetWeather( BTL_FIELD_WORK* wk, BtlWeather weather, u16 turn );
extern BtlWeather BTL_FIELDSIM_TurnCheckWeather( BTL_FIELD_WORK* wk );
extern BOOL BTL_FIELDSIM_AddEffect( BTL_FIELD_WORK* wk, BtlFieldEffect effect, BPP_SICK_CONT cont, BOOL fForServer );
extern BOOL BTL_FIELDSIM_RemoveEffect( BTL_FIELD_WORK* wk, BtlFieldEffect effect );
extern BOOL BTL_FIELDSIM_AddDependPoke( BTL_FIELD_WORK* wk, BtlFieldEffect effect, u8 pokeID );
extern BOOL BTL_FIELDSIM_IsDependPoke( BTL_FIELD_WORK* wk, BtlFieldEffect effect, u8 pokeID );
extern void BTL_FIELDSIM_RemoveDependPokeEffect( BTL_FIELD_WORK* wk, u8 pokeID );
extern BOOL BTL_FIELDSIM_CheckFuin( BTL_FIELD_WORK* wk, const BTL_POKE_CONTAINER* pokeCon, const BTL_POKEPARAM* attacker, WazaID waza );
extern void BTL_FIELDSIM_TurnCheck( BTL_FIELD_WORK* wk, pFieldTurnCheckCallback callbackFunc, void* callbackArg );
extern BOOL BTL_FIELDSIM_CheckEffect( BTL_FIELD_WORK* wk, BtlFieldEffect effect );
extern u8 BTL_FIELDSIM_GetDependPokeID( BTL_FIELD_WORK* wk, BtlFieldEffect effect );


#endif
