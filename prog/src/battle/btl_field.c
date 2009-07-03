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

  BtlWeather    weather;
  u16           weatherTurn;

  BTL_EVENT_FACTOR*  factor[ BTL_FLDEFF_MAX ];
  BPP_SICK_CONT      cont[ BTL_FLDEFF_MAX ];
  u8                 turnCount[ BTL_FLDEFF_MAX ];

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
 * @param   waza    ���UID
 *
 * @retval  fx32    �������i�p�[�Z���e�[�W�j
 */
//=============================================================================================
fx32 BTL_FIELD_GetWeatherDmgRatio( WazaID waza )
{
  switch( Work.weather ){
  case BTL_WEATHER_SHINE:
    {
      PokeType type = WAZADATA_GetType( waza );
      if( type == POKETYPE_HONOO ){
        return BTL_CALC_DMG_WEATHER_RATIO_ADVANTAGE;
      }
      if( type == POKETYPE_MIZU ){
        return BTL_CALC_DMG_WEATHER_RATIO_DISADVANTAGE;
      }
    }
    break;
  case BTL_WEATHER_RAIN:
    {
      PokeType type = WAZADATA_GetType( waza );
      if( type == POKETYPE_HONOO ){
        return BTL_CALC_DMG_WEATHER_RATIO_DISADVANTAGE;
      }
      if( type == POKETYPE_MIZU ){
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
    Work.factor[ effect ] = BTL_HANDLER_FLD_Add( effect, 0 );
    if( Work.factor[ effect ] ){
      Work.cont[ effect ] = cont;
      Work.turnCount[ effect ] = 0;
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
void BTL_FIELD_RemoveEffect( BtlFieldEffect effect )
{

}
//=============================================================================================
/**
 * �^�[���`�F�b�N
 */
//=============================================================================================
void BTL_FIELD_TurnCheck( pFieldTurnCheckCallback callbackFunc, void* callbackArg )
{
  u32 i;
  for(i=0; i<BTL_FLDEFF_MAX; ++i)
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
 * ����|�P�����ˑ��̃G�t�F�N�g������
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
      if( BPP_SICCONT_GetPokeID(Work.cont[i]) == pokeID )
      {
        BTL_HANDLER_FLD_Remove( Work.factor[i] );
        clearFactorWork( i );
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

