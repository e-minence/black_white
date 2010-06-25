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
  DEPEND_POKE_NUM_MAX = BTL_POS_MAX,
};

//--------------------------------------------------------------
/**
 *  ���[�N�\����
 */
//--------------------------------------------------------------
struct _BTL_FIELD_WORK {

  /*
   * VRAM_H �ɒu���̂�u32�^�ɑ�����K�v����
   */
  u32       weather;
  u32       weatherTurn;

  BTL_EVENT_FACTOR*  factor[ BTL_FLDEFF_MAX ];
  BPP_SICK_CONT      cont[ BTL_FLDEFF_MAX ];
  u32                turnCount[ BTL_FLDEFF_MAX ];
  u32                dependPokeID[ BTL_FLDEFF_MAX ][ DEPEND_POKE_NUM_MAX ];
  u32                dependPokeCount[ BTL_FLDEFF_MAX ];
  u32                enableFlag[ BTL_FLDEFF_MAX ];
};

//--------------------------------------------------------------
/**
 *  �O���[�o�����[�N
 */
//--------------------------------------------------------------
static BTL_FIELD_WORK  Work =  {0};





static void clearFactorWork( BTL_FIELD_WORK* wk, BtlFieldEffect effect )
{
  u32 i;

  wk->factor[ effect ] = NULL;
  wk->cont[ effect ] = BPP_SICKCONT_MakeNull();
  wk->turnCount[ effect ] = 0;
  wk->dependPokeCount[ effect ] = 0;
  wk->enableFlag[ effect ] = FALSE;

  for(i=0; i<DEPEND_POKE_NUM_MAX; ++i)
  {
    wk->dependPokeID[effect][i] = BTL_POKEID_NULL;
  }
}


//=============================================================================================
/**
 * ���[�N������
 *
 * @param   weather
 */
//=============================================================================================
void BTL_FIELD_Init( BtlWeather weather )
{
  BTL_FIELDSIM_Init( &Work, weather );
}

//=============================================================================================
/**
 * �V��擾
 *
 * @param   none
 *
 * @retval  BtlWeather
 */
//=============================================================================================
BtlWeather BTL_FIELD_GetWeather( void )
{
  return BTL_FIELDSIM_GetWeather( &Work );
}
//=============================================================================================
/**
 * �V��c��^�[���擾
 *
 * @param   none
 *
 * @retval  u32
 */
//=============================================================================================
u32 BTL_FIELD_GetWeatherTurn( void )
{
  return BTL_FIELDSIM_GetWeatherTurn( &Work );
}
//=============================================================================================
/**
 *  �V��Z�b�g
 *
 * @param   weather
 * @param   turn
 *
 */
//=============================================================================================
void BTL_FIELD_SetWeather( BtlWeather weather, u16 turn )
{
  BTL_FIELDSIM_SetWeather( &Work, weather, turn );
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
  return BTL_FIELDSIM_TurnCheckWeather( &Work );
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
  return BTL_FIELDSIM_AddEffect( &Work, effect, cont, TRUE );
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
  return BTL_FIELDSIM_RemoveEffect( &Work, effect );
}
//=============================================================================================
/**
 * �ˑ��Ώۃ|�P������ǉ�
 *
 * @param   BtlFieldEffect
 * @param   pokeID
 *
 * @retval  BOOL  �V�K�ǉ��ł�����TRUE / ���ɓo�^����Ă�ꍇ���o�^���s������FALSE
 */
//=============================================================================================
BOOL BTL_FIELD_AddDependPoke( BtlFieldEffect effect, u8 pokeID )
{
  return BTL_FIELDSIM_AddDependPoke( &Work, effect, pokeID );
}
//=============================================================================================
/**
 * �w��G�t�F�N�g�̈ˑ��Ώۃ|�P�������ǂ�������
 *
 * @param   effect
 * @param   pokeID
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTL_FIELD_IsDependPoke( BtlFieldEffect effect, u8 pokeID )
{
  return BTL_FIELDSIM_IsDependPoke( &Work, effect, pokeID );
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
  BTL_FIELDSIM_RemoveDependPokeEffect( &Work, pokeID );
}
//=============================================================================================
/**
 * �ӂ�����֎~�Ώۂ̃��U���ǂ�������
 *
 * @param   pokeCon
 * @param   attacker
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTL_FIELD_CheckFuin( const BTL_POKE_CONTAINER* pokeCon, const BTL_POKEPARAM* attacker, WazaID waza )
{
  return BTL_FIELDSIM_CheckFuin( &Work, pokeCon, attacker, waza );
}

//=============================================================================================
/**
 * �^�[���`�F�b�N
 */
//=============================================================================================
void BTL_FIELD_TurnCheck( pFieldTurnCheckCallback callbackFunc, void* callbackArg )
{
  BTL_FIELDSIM_TurnCheck( &Work, callbackFunc, callbackArg );
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
  return BTL_FIELDSIM_CheckEffect( &Work, effect );
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
  return BTL_FIELDSIM_GetDependPokeID( &Work, effect );
}


/*=============================================================================================*/
/*                                                                                             */
/* BTL_FIELD �Ɠ����̋@�\���N���C�A���g���Ɏ�������ׂ̎d�g��                                  */
/*                                                                                             */
/*=============================================================================================*/

BTL_FIELD_WORK*  BTL_FIELDSIM_CreateWork( HEAPID heapID )
{
  BTL_FIELD_WORK* wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(BTL_FIELD_WORK) );

  BTL_FIELDSIM_Init( wk, BTL_WEATHER_NONE );

  return wk;
}
void BTL_FIELDSIM_DeleteWork( BTL_FIELD_WORK* wk )
{
  GFL_HEAP_FreeMemory( wk );
}

//---------------------------------------------------------------------
/**
 * ���[�N������
 *
 * @param   wk
 * @param   weather
 */
//---------------------------------------------------------------------
void BTL_FIELDSIM_Init( BTL_FIELD_WORK* wk, BtlWeather weather )
{
  u32 i;

  for(i=0; i<BTL_FLDEFF_MAX; ++i){
    clearFactorWork( wk, i );
  }

  wk->weather = weather;
  wk->weatherTurn = BTL_WEATHER_TURN_PERMANENT;
}
//---------------------------------------------------------------------
/**
 * �V��擾
 *
 * @retval  BtlWeather
 */
//---------------------------------------------------------------------
BtlWeather BTL_FIELDSIM_GetWeather( const BTL_FIELD_WORK* wk )
{
  return wk->weather;
}
//---------------------------------------------------------------------
/**
 * �V��c��^�[���擾
 *
 * @retval  u32
 */
//---------------------------------------------------------------------
u32 BTL_FIELDSIM_GetWeatherTurn( const BTL_FIELD_WORK* wk )
{
  if( wk->weather != BTL_WEATHER_NONE ){
    return wk->weatherTurn;
  }
  return 0;
}
//---------------------------------------------------------------------
/**
 *  �V��Z�b�g
 *
 * @param   weather
 * @param   turn
 *
 */
//---------------------------------------------------------------------
void BTL_FIELDSIM_SetWeather( BTL_FIELD_WORK* wk, BtlWeather weather, u16 turn )
{
  wk->weather = weather;
  wk->weatherTurn = turn;
}
//---------------------------------------------------------------------
/**
 *  �V��I��
 *
 * @param   weather
 * @param   turn
 *
 */
//---------------------------------------------------------------------
void BTL_FIELDSIM_EndWeather( BTL_FIELD_WORK* wk )
{
  wk->weather = BTL_WEATHER_NONE;
  wk->weatherTurn = 0;
}

//---------------------------------------------------------------------
/**
 * �^�[���`�F�b�N
 *
 * @retval  BtlWeather    �^�[���`�F�b�N�ɂ��I������V��
 */
//---------------------------------------------------------------------
BtlWeather BTL_FIELDSIM_TurnCheckWeather( BTL_FIELD_WORK* wk )
{
  if( wk->weather != BTL_WEATHER_NONE )
  {
    if( wk->weatherTurn != BTL_WEATHER_TURN_PERMANENT )
    {
      wk->weatherTurn--;
      if( wk->weatherTurn == 0 )
      {
        BtlWeather endWeather = wk->weather;
        wk->weather = BTL_WEATHER_NONE;
        return endWeather;
      }
    }
  }
  return BTL_WEATHER_NONE;
}
//---------------------------------------------------------------------
/**
 * �t�B�[���h�G�t�F�N�g�ǉ�
 *
 * @param   state
 *
 * @retval  BOOL
 */
//---------------------------------------------------------------------
BOOL BTL_FIELDSIM_AddEffect( BTL_FIELD_WORK* wk, BtlFieldEffect effect, BPP_SICK_CONT cont, BOOL fForServer )
{
  GF_ASSERT(effect < BTL_FLDEFF_MAX);

  if( wk->enableFlag[effect] == FALSE )
  {
    if( fForServer )
    {
      wk->factor[ effect ] = BTL_HANDLER_FLD_Add( effect, 0 );
      if( wk->factor[ effect ] == NULL )
      {
        return FALSE;
      }
    }

    wk->enableFlag[ effect ] = TRUE;

    {
      u32 i;

      wk->cont[ effect ] = cont;
      wk->turnCount[ effect ] = 0;
      wk->dependPokeCount[ effect ] = 0;
      for(i=0; i<DEPEND_POKE_NUM_MAX; ++i){
        wk->dependPokeID[ effect ][i] = BTL_POKEID_NULL;
      }

      {
        u8 dependPokeID = BPP_SICKCONT_GetPokeID( cont );

        if( dependPokeID != BTL_POKEID_NULL )
        {
          BTL_FIELDSIM_AddDependPoke( wk, effect, dependPokeID );
        }
      }
    }
    return TRUE;
  }

  return FALSE;
}
//---------------------------------------------------------------------
/**
 * �t�B�[���h�G�t�F�N�g����
 *
 * @param   effect
 */
//---------------------------------------------------------------------
BOOL BTL_FIELDSIM_RemoveEffect( BTL_FIELD_WORK* wk, BtlFieldEffect effect )
{
  GF_ASSERT(effect < BTL_FLDEFF_MAX);

  if( BTL_FIELDSIM_CheckEffect(wk, effect) )
  {
    if( wk->factor[effect] != NULL ){
      BTL_HANDLER_FLD_Remove( wk->factor[effect] );
      wk->factor[effect] = NULL;
    }
    clearFactorWork( wk, effect );
    return TRUE;
  }
  return FALSE;
}
//---------------------------------------------------------------------
/**
 * �ˑ��Ώۃ|�P������ǉ�
 *
 * @param   BtlFieldEffect
 * @param   pokeID
 *
 * @retval  BOOL  �V�K�ǉ��ł�����TRUE / ���ɓo�^����Ă�ꍇ���o�^���s������FALSE
 */
//---------------------------------------------------------------------
BOOL BTL_FIELDSIM_AddDependPoke( BTL_FIELD_WORK* wk, BtlFieldEffect effect, u8 pokeID )
{
  GF_ASSERT(effect < BTL_FLDEFF_MAX);

  if( BTL_FIELDSIM_CheckEffect(wk, effect) )
  {
    u32 count = wk->dependPokeCount[ effect ];

    if( count < DEPEND_POKE_NUM_MAX )
    {
      u32 i;
      for(i=0; i<count; ++i)
      {
        if( wk->dependPokeID[ effect ][i] == pokeID ){
          return FALSE;
        }
      }

      wk->dependPokeID[ effect ][ count++ ] = pokeID;
      wk->dependPokeCount[ effect ] = count;
      return TRUE;
    }
  }
  return FALSE;
}
//---------------------------------------------------------------------
/**
 * �w��G�t�F�N�g�̈ˑ��Ώۃ|�P�������ǂ�������
 *
 * @param   effect
 * @param   pokeID
 *
 * @retval  BOOL
 */
//---------------------------------------------------------------------
BOOL BTL_FIELDSIM_IsDependPoke( BTL_FIELD_WORK* wk, BtlFieldEffect effect, u8 pokeID )
{
  GF_ASSERT(effect < BTL_FLDEFF_MAX);

  if( BTL_FIELDSIM_CheckEffect(wk, effect) )
  {
    u32 count = wk->dependPokeCount[ effect ];
    u32 i;
    for(i=0; i<count; ++i)
    {
      if( wk->dependPokeID[ effect ][i] == pokeID ){
        return TRUE;
      }
    }
  }
  return FALSE;
}
//---------------------------------------------------------------------
/**
 * ����|�P�����ˑ��̃G�t�F�N�g��S�ď���
 *
 * @param   pokeID
 */
//---------------------------------------------------------------------
void BTL_FIELDSIM_RemoveDependPokeEffect( BTL_FIELD_WORK* wk, u8 pokeID )
{
  u32 i;
  for(i=0; i<BTL_FLDEFF_MAX; ++i)
  {
    if( BTL_FIELDSIM_CheckEffect(wk, i) )
    {
      if( wk->dependPokeCount[i] )
      {
        u32 p, fMatch = FALSE;
        for(p=0; p<wk->dependPokeCount[i]; ++p)
        {
          if( wk->dependPokeID[i][p] == pokeID ){
            fMatch = TRUE;
            break;
          }
        }
        if( fMatch )
        {
          for( ; p<(DEPEND_POKE_NUM_MAX-1); ++p){
            wk->dependPokeID[i][p] = wk->dependPokeID[i][p+1];
          }
          wk->dependPokeID[i][p] = BTL_POKEID_NULL;
          wk->dependPokeCount[i]--;

          if( wk->dependPokeCount[i] == 0 )
          {
            if( wk->factor[i] != NULL ){
              BTL_HANDLER_FLD_Remove( wk->factor[i] );
              wk->factor[i] = NULL;
            }
            clearFactorWork( wk, i );
          }
          else if( BPP_SICKCONT_GetPokeID(wk->cont[i]) == pokeID )
          {
            u8 nextPokeID = wk->dependPokeID[i][0];
            BPP_SICKCONT_SetPokeID( &(wk->cont[i]), nextPokeID );
          }
        }
      }
    }
  }
}
//---------------------------------------------------------------------
/**
 * �ӂ�����֎~�Ώۂ̃��U���ǂ�������
 *
 * @param   pokeCon
 * @param   attacker
 *
 * @retval  BOOL
 */
//---------------------------------------------------------------------
BOOL BTL_FIELDSIM_CheckFuin( BTL_FIELD_WORK* wk, const BTL_POKE_CONTAINER* pokeCon, const BTL_POKEPARAM* attacker, WazaID waza )
{
  u8  atkPokeID = BPP_GetID( attacker );
  u8  fuinPokeID;
  u32 i, fuinPokeCnt = wk->dependPokeCount[ BTL_FLDEFF_FUIN ];

  for(i=0; i<fuinPokeCnt; ++i)
  {
    fuinPokeID = wk->dependPokeID[ BTL_FLDEFF_FUIN ][ i ];

    BTL_N_Printf( DBGSTR_FIDLD_FuinCheck, i, fuinPokeID, waza );
    if( !BTL_MAINUTIL_IsFriendPokeID(atkPokeID, fuinPokeID) )
    {
      const BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParamConst( pokeCon, fuinPokeID );
      if( BPP_WAZA_IsUsable(bpp, waza) )
      {
        BTL_N_Printf( DBGSTR_FIELD_FuinHit, fuinPokeID, waza);
        return TRUE;
      }
    }
  }
  return FALSE;
}
//---------------------------------------------------------------------
/**
 * �^�[���`�F�b�N
 */
//---------------------------------------------------------------------
void BTL_FIELDSIM_TurnCheck( BTL_FIELD_WORK* wk, pFieldTurnCheckCallback callbackFunc, void* callbackArg )
{
  u32 i;
  for(i=1; i<BTL_FLDEFF_MAX; ++i)
  {
    if( BTL_FIELDSIM_CheckEffect(wk, i) )
    {
      u8 turnMax = BPP_SICCONT_GetTurnMax( wk->cont[i] );
      if( turnMax )
      {
        if( ++(wk->turnCount[i]) >= turnMax )
        {
          if( wk->factor[i] ){
            BTL_HANDLER_FLD_Remove( wk->factor[i] );
            wk->factor[i] = NULL;
          }
          clearFactorWork( wk, i );

          if( callbackFunc ){
            callbackFunc( i, callbackArg );
          }
        }
      }
    }
  }
}
//---------------------------------------------------------------------
/**
 * ����̃t�B�[���h�G�t�F�N�g�������Ă��邩�`�F�b�N
 *
 * @param   effect
 *
 * @retval  BOOL
 */
//---------------------------------------------------------------------
BOOL BTL_FIELDSIM_CheckEffect( BTL_FIELD_WORK* wk, BtlFieldEffect effect )
{
  GF_ASSERT(effect < BTL_FLDEFF_MAX);
  return wk->enableFlag[ effect ];
}
//---------------------------------------------------------------------
/**
 * �p���ˑ��|�P����ID��Ԃ�
 *
 * @param   effect
 *
 * @retval  u8
 */
//---------------------------------------------------------------------
u8 BTL_FIELDSIM_GetDependPokeID( BTL_FIELD_WORK* wk, BtlFieldEffect effect )
{
  GF_ASSERT(effect < BTL_FLDEFF_MAX);

  if( BTL_FIELDSIM_CheckEffect(wk, effect) )
  {
    return BPP_SICKCONT_GetPokeID( wk->cont[effect] );
  }

  return BTL_POKEID_NULL;
}





