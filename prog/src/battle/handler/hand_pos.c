//=============================================================================================
/**
 * @file  hand_pos.c
 * @brief �|�P����WB �o�g���V�X�e�� �C�x���g�t�@�N�^�[ [�ʒu�G�t�F�N�g]
 * @author  taya
 *
 * @date  2009.06.18  �쐬
 */
//=============================================================================================

#include "poke_tool\poketype.h"
#include "poke_tool\monsno_def.h"

#include "..\btl_common.h"
#include "..\btl_calc.h"
#include "..\btl_field.h"
#include "..\btl_client.h"
#include "..\btl_event_factor.h"

#include "hand_pos.h"

/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
  WORKIDX_USER_POKEID = EVENT_HANDLER_WORK_ELEMS-1,
};


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static BOOL is_registable( BtlPosEffect effect, BtlPokePos pokePos);
static BTL_EVENT_FACTOR* ADD_POS_Negaigoto( u16 pri, BtlPokePos pos, BtlPosEffect eff );
static void handler_pos_Negaigoto( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokePos, int* work );
static BTL_EVENT_FACTOR* ADD_POS_MikadukiNoMai( u16 pri, BtlPokePos pos, BtlPosEffect eff );
static void handler_pos_MikadukiNoMai( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokePos, int* work );
static BTL_EVENT_FACTOR* ADD_POS_IyasiNoNegai( u16 pri, BtlPokePos pos, BtlPosEffect eff );
static void handler_pos_IyasiNoNegai( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokePos, int* work );
static BTL_EVENT_FACTOR* ADD_POS_DelayAttack( u16 pri, BtlPokePos pos, BtlPosEffect eff );
static void handler_pos_DelayAttack( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokePos, int* work );
static BTL_EVENT_FACTOR* ADD_POS_BatonTouch( u16 pri, BtlPokePos pos, BtlPosEffect eff );
static void handler_pos_BatonTouch( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokePos, int* work );





//=============================================================================================
/**
 * �T�C�h�G�t�F�N�g�n���h�����V�X�e���ɒǉ�
 *
 * @param   effect
 * @param   pos
 * @param   pokeID
 * @param   param
 *
 * @retval  BTL_EVENT_FACTOR*   �ǉ����ꂽ�C�x���g�n���h���i�d�����Ēǉ��ł��Ȃ��ꍇNULL�j
 */
//=============================================================================================
BTL_EVENT_FACTOR*  BTL_HANDLER_POS_Add( BtlPosEffect effect, BtlPokePos pos, u8 pokeID, const int* param, u8 param_cnt )
{
  typedef BTL_EVENT_FACTOR* (*pEventAddFunc)( u16 pri, BtlPokePos pos, BtlPosEffect eff );

  static const struct {
    BtlPosEffect   eff;
    pEventAddFunc  func;
  }funcTbl[] = {
    { BTL_POSEFF_NEGAIGOTO,       ADD_POS_Negaigoto       },
    { BTL_POSEFF_MIKADUKINOMAI,   ADD_POS_MikadukiNoMai   },
    { BTL_POSEFF_IYASINONEGAI,    ADD_POS_IyasiNoNegai    },
    { BTL_POSEFF_DELAY_ATTACK,    ADD_POS_DelayAttack     },
    { BTL_POSEFF_BATONTOUCH,      ADD_POS_BatonTouch      },
  };

  GF_ASSERT(effect < BTL_POSEFF_MAX);

  {
    u32 i;
    for(i=0; i<NELEMS(funcTbl); ++i)
    {
      if( funcTbl[i].eff == effect )
      {
        if( is_registable(effect, pos) )
        {
          BTL_EVENT_FACTOR* factor = funcTbl[i].func( 0, pos, effect );
          if( factor )
          {
            u32 j;
            for(j=0; j<param_cnt; ++j){
              BTL_EVENT_FACTOR_SetWorkValue( factor, j, param[j] );
            }
            BTL_EVENT_FACTOR_SetWorkValue( factor, WORKIDX_USER_POKEID, pokeID );
          }
          return factor;
        }
        break;
      }
    }
  }
  return NULL;
}
//----------------------------------------------------------------------------------
/**
 * �o�^�ł�������𖞂����Ă��邩����  �������ʒu�ɓ����G�t�F�N�g�͂P�܂�
 *
 * @param   effect
 * @param   pokePos
 *
 * @retval  BOOL    �����𖞂����Ă�����TRUE
 */
//----------------------------------------------------------------------------------
static BOOL is_registable( BtlPosEffect effect, BtlPokePos pokePos)
{
  BTL_EVENT_FACTOR* factor;

  factor = BTL_EVENT_SeekFactor( BTL_EVENT_FACTOR_POS, pokePos );
  while( factor )
  {
    if( BTL_EVENT_FACTOR_GetSubID(factor) == effect ){
      return FALSE;
    }
    factor = BTL_EVENT_GetNextFactor( factor );
  }
  return TRUE;
}

//--------------------------------------------------------------------------------------
/**
 *  �˂�������
 */
//--------------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* ADD_POS_Negaigoto( u16 pri, BtlPokePos pos, BtlPosEffect eff )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TURNCHECK_BEGIN,  handler_pos_Negaigoto   },  // �_���[�W�␳
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_POS, eff, pri, pos, HandlerTable );
}
static void handler_pos_Negaigoto( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokePos, int* work )
{
  work[0]++;
  if( work[0] >= 2 )
  {
    u8 targetPokeID = BTL_SVFLOW_PokePosToPokeID( flowWk, pokePos );
    const BTL_POKEPARAM* target = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, targetPokeID );

    if( !BPP_IsHPFull(target) )
    {
      u8 userPokeID = work[ WORKIDX_USER_POKEID ];
      const BTL_POKEPARAM* user = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, userPokeID );

      BTL_HANDEX_PARAM_RECOVER_HP* hp_param;
      hp_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RECOVER_HP, userPokeID );
      hp_param->pokeID = targetPokeID;
      hp_param->recoverHP = BTL_CALC_QuotMaxHP( user, 2 );
      HANDEX_STR_Setup( &hp_param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Negaigoto );
      HANDEX_STR_AddArg( &hp_param->exStr, userPokeID );
    }
    BTL_EVENT_FACTOR_Remove( myHandle );
  }
}
//--------------------------------------------------------------------------------------
/**
 *  �݂��Â��̂܂�
 */
//--------------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* ADD_POS_MikadukiNoMai( u16 pri, BtlPokePos pos, BtlPosEffect eff )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,  handler_pos_MikadukiNoMai   },  // �|�P����n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_POS, eff, pri, pos, HandlerTable );
}
static void handler_pos_MikadukiNoMai( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokePos, int* work )
{
  u8 pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID );
  if( BTL_SVFLOW_PokeIDtoPokePos(flowWk, pokeID) == pokePos )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    BTL_HANDEX_PARAM_MESSAGE* msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, BTL_POKEID_NULL );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_MikadukiNoMai );
    HANDEX_STR_AddArg( &msg_param->str, pokeID );

    // �̗͑S��
    if( !BPP_IsHPFull(bpp) ){
      BTL_HANDEX_PARAM_RECOVER_HP* hp_param;
      hp_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RECOVER_HP, pokeID );
      hp_param->pokeID = pokeID;
      hp_param->recoverHP = BPP_GetValue(bpp, BPP_MAX_HP) - BPP_GetValue(bpp, BPP_HP);
    }

    // �|�P�n��Ԉُ��
    if( BPP_GetPokeSick(bpp) != POKESICK_NULL ){
      BTL_HANDEX_PARAM_CURE_SICK* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
      param->sickCode = WAZASICK_EX_POKEFULL;
    }

    // �S���UPP�S��
    {
      u32 i;
      u8 volume;
      for(i=0; i<PTL_WAZA_MAX; ++i){
        volume = BPP_WAZA_GetPPShort( bpp, i );
        if( volume )
        {
          BTL_HANDEX_PARAM_PP* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RECOVER_PP, pokeID );
          param->pokeID = pokeID;
          param->volume = volume;
          param->wazaIdx = i;
        }
      }
    }

    BTL_EVENT_FACTOR_Remove( myHandle );
  }
}
//--------------------------------------------------------------------------------------
/**
 *  ���₵�̂˂���
 */
//--------------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* ADD_POS_IyasiNoNegai( u16 pri, BtlPokePos pos, BtlPosEffect eff )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,  handler_pos_IyasiNoNegai   },  // �_���[�W�␳
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_POS, eff, pri, pos, HandlerTable );
}
static void handler_pos_IyasiNoNegai( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokePos, int* work )
{
  u8 pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID );
  if( BTL_SVFLOW_PokeIDtoPokePos(flowWk, pokeID) == pokePos )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    BTL_HANDEX_PARAM_MESSAGE* msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, BTL_POKEID_NULL );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_IyasiNoNegai );
    HANDEX_STR_AddArg( &msg_param->str, pokeID );

    // �̗͑S��
    if( !BPP_IsHPFull(bpp) ){
      BTL_HANDEX_PARAM_RECOVER_HP* hp_param;
      hp_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RECOVER_HP, pokeID );
      hp_param->pokeID = pokeID;
      hp_param->recoverHP = BPP_GetValue(bpp, BPP_MAX_HP) - BPP_GetValue(bpp, BPP_HP);
    }

    // �|�P�n��Ԉُ��
    if( BPP_GetPokeSick(bpp) != POKESICK_NULL ){
      BTL_HANDEX_PARAM_CURE_SICK* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
      param->sickCode = WAZASICK_EX_POKEFULL;
    }

    BTL_EVENT_FACTOR_Remove( myHandle );
  }
}
//--------------------------------------------------------------------------------------
/**
 *  ���ԍ����U�U���i�݂炢�悿�A�͂߂̂˂������j
 */
//--------------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* ADD_POS_DelayAttack( u16 pri, BtlPokePos pos, BtlPosEffect eff )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TURNCHECK_BEGIN,  handler_pos_DelayAttack   },  // �_���[�W�␳
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_POS, eff, pri, pos, HandlerTable );
}
static void handler_pos_DelayAttack( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokePos, int* work )
{
  enum {
    WORKIDX_TURN = 0,
    WORKIDX_WAZAID,
  };

  if( work[WORKIDX_TURN] == 0 )
  {
    BTL_HANDEX_PARAM_DELAY_WAZADMG* param;
    BTL_HANDEX_PARAM_MESSAGE* msg_param;
    u8 targetPokeID = BTL_SVFLOW_PokePosToPokeID( flowWk, pokePos );

    msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, work[WORKIDX_USER_POKEID] );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_DelayAttack );
    HANDEX_STR_AddArg( &msg_param->str, targetPokeID );
    HANDEX_STR_AddArg( &msg_param->str, work[ WORKIDX_WAZAID ] );
    BTL_Printf("���ԍ��U��:��U��%d\n", work[ WORKIDX_WAZAID ] );

    param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_DELAY_WAZADMG, work[WORKIDX_USER_POKEID] );
    param->attackerPokeID = work[ WORKIDX_USER_POKEID ];
    param->targetPokeID = targetPokeID;
    param->wazaID = work[ WORKIDX_WAZAID ];
    BTL_EVENT_FACTOR_Remove( myHandle );
  }
  else{
    work[WORKIDX_TURN]--;
  }
}
//--------------------------------------------------------------------------------------
/**
 *  �o�g���^�b�`
 */
//--------------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* ADD_POS_BatonTouch( u16 pri, BtlPokePos pos, BtlPosEffect eff )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,  handler_pos_BatonTouch   },  // �_���[�W�␳
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_POS, eff, pri, pos, HandlerTable );
}
static void handler_pos_BatonTouch( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokePos, int* work )
{
  enum {
    WORKIDX_USER_POKEID = 0,
  };

  u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID );
  BTL_Printf("[�o�g���^�b�`] targetPokeID=%d, NowPow=%d, NowPosPokeID=%d\n",
    targetPokeID, pokePos, BTL_SVFLOW_PokePosToPokeID(flowWk, pokePos));
  if( targetPokeID == BTL_SVFLOW_PokePosToPokeID(flowWk, pokePos) )
  {
    BTL_HANDEX_PARAM_BATONTOUCH* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_BATONTOUCH, BTL_POKEID_NULL );
    param->userPokeID = work[ WORKIDX_USER_POKEID ];
    param->targetPokeID = targetPokeID;
    BTL_EVENT_FACTOR_Remove( myHandle );
  }
}


