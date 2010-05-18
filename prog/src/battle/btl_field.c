//=============================================================================================
/**
 * @file  btl_field.c
 * @brief �|�P����WB �o�g���V�X�e�� �V��Ȃǁu��v�ɑ΂�����ʂɊւ��鏈��
 * @author  taya
 *
 * @date  2009.03.04  �쐬
 */
//=============================================================================================
#include <gflib.h>

#include "btl_common.h"
#include "btl_calc.h"
#include "btl_event_factor.h"

#include "handler\hand_field.h"
#include "btl_field.h"

//--------------------------------------------------------------
/**
 *  �萔
 */
//--------------------------------------------------------------
enum {
  TURN_MAX = 16,
};
//--------------------------------------------------------------
/**
 *  �O���[�o�����[�N
 */
//--------------------------------------------------------------
static struct {

  u32       weather;
  u32       weatherTurn;

  BTL_EVENT_FACTOR*  factor[ BTL_FLDEFF_MAX ];
  BPP_SICK_CONT      cont[ BTL_FLDEFF_MAX ];
  u32                turnCount[ BTL_FLDEFF_MAX ];

}Work;



static void clearFactorWork( BtlFieldEffect effect )
{
  Work.factor[ effect ] = NULL;
  Work.cont[ effect ] = BPP_SICKCONT_MakeNull();
  Work.turnCount[ effect ] = 0;
}


void BTL_FIELD_Init( BtlWeather weather )
{
  u32 i;

  for(i=0; i<BTL_FLDEFF_MAX; ++i){
    clearFactorWork( i );
  }

  Work.weather = weather;
  Work.weatherTurn = BTL_WEATHER_TURN_PERMANENT;
}

BtlWeather BTL_FIELD_GetWeather( void )
{
  return Work.weather;
}
u32 BTL_FIELD_GetWeatherTurn( void )
{
  if( Work.weather != BTL_WEATHER_NONE ){
    return Work.weatherTurn;
  }
  return 0;
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
}

//=============================================================================================
/**
 * �^�[���`�F�b�N
 *
 * @retval  BtlWeather    �^�[���`�F�b�N�ɂ��I������V��
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
 * �t�B�[���h�G�t�F�N�g�ǉ�
 *
 * @param   state
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTL_FIELD_AddEffect( BtlFieldEffect effect, BPP_SICK_CONT cont )
{
  GF_ASSERT(effect < BTL_FLDEFF_MAX);

  if( Work.factor[effect] == NULL )
  {
    BTL_Printf("FldEff %d �ǉ�\n");
    Work.factor[ effect ] = BTL_HANDLER_FLD_Add( effect, 0 );
    if( Work.factor[ effect ] ){
      Work.cont[ effect ] = cont;
      Work.turnCount[ effect ] = 0;
      return TRUE;
    }
  }
  return FALSE;
}
//=============================================================================================
/**
 * �t�B�[���h�G�t�F�N�g����
 *
 * @param   effect
 */
//=============================================================================================
BOOL BTL_FIELD_RemoveEffect( BtlFieldEffect effect )
{
  GF_ASSERT(effect < BTL_FLDEFF_MAX);

  if( Work.factor[effect] != NULL )
  {
    BTL_Printf("FldEff %d ����\n");

    BTL_HANDLER_FLD_Remove( Work.factor[effect] );
    clearFactorWork( effect );
    return TRUE;
  }
  return FALSE;
}
//=============================================================================================
/**
 * ����|�P�����ˑ��̃G�t�F�N�g��S�ď���
 *
 * @param   pokeID
 */
//=============================================================================================
void BTL_FIELD_RemoveDependPokeEffect( u8 pokeID )
{
  u32 i;
  for(i=0; i<BTL_FLDEFF_MAX; ++i)
  {
    if( Work.factor[i] )
    {
      if( BPP_SICKCONT_GetPokeID(Work.cont[i]) == pokeID )
      {
        BTL_HANDLER_FLD_Remove( Work.factor[i] );
        clearFactorWork( i );
      }
    }
  }
}
//=============================================================================================
/**
 * ����|�P�����ˑ��̃G�t�F�N�g��ʃ|�P�����Ɉ����p��
 *
 * @param   oldPokeID
 * @param   nextPokeID
 */
//=============================================================================================
void BTL_FIELD_BatonTouchPokeEffect( u8 oldPokeID, u8 nextPokeID )
{
  u32 i;
  for(i=0; i<BTL_FLDEFF_MAX; ++i)
  {
    if( Work.factor[i] )
    {
      if( BPP_SICKCONT_GetPokeID(Work.cont[i]) == oldPokeID )
      {
        BPP_SICKCONT_SetPokeID( &(Work.cont[i]), nextPokeID );
      }
    }
  }
}


//=============================================================================================
/**
 * �^�[���`�F�b�N
 */
//=============================================================================================
void BTL_FIELD_TurnCheck( pFieldTurnCheckCallback callbackFunc, void* callbackArg )
{
  u32 i;
  for(i=1; i<BTL_FLDEFF_MAX; ++i)
  {
    if( Work.factor[i] )
    {
      u8 turnMax = BPP_SICCONT_GetTurnMax( Work.cont[i] );
      if( turnMax )
      {
        if( ++(Work.turnCount[i]) >= turnMax )
        {
          BTL_HANDLER_FLD_Remove( Work.factor[i] );
          clearFactorWork( i );
          callbackFunc( i, callbackArg );
        }
      }
    }
  }
}
//=============================================================================================
/**
 * ����̃t�B�[���h�G�t�F�N�g�������Ă��邩�`�F�b�N
 *
 * @param   effect
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTL_FIELD_CheckEffect( BtlFieldEffect effect )
{
  GF_ASSERT(effect < BTL_FLDEFF_MAX);

  return Work.factor[ effect ] != NULL;
}

//=============================================================================================
/**
 * �p���ˑ��|�P����ID��Ԃ�
 *
 * @param   effect
 *
 * @retval  u8
 */
//=============================================================================================
u8 BTL_FIELD_GetDependPokeID( BtlFieldEffect effect )
{
  GF_ASSERT(effect < BTL_FLDEFF_MAX);

  if( Work.factor[ effect ] )
  {
    return BPP_SICKCONT_GetPokeID( Work.cont[effect] );
  }

  return BTL_POKEID_NULL;
}

