//=============================================================================================
/**
 * @file  hand_waza.c
 * @brief �|�P����WB �o�g���V�X�e�� �C�x���g�t�@�N�^�[ [���U] ����
 * @author  taya
 *
 * @date  2009.05.15  �쐬
 */
//=============================================================================================

#include "poke_tool\poketype.h"
#include "poke_tool\monsno_def.h"
#include "waza_tool\wazano_def.h"

#include "..\btl_common.h"
#include "..\btl_calc.h"
#include "..\btl_field.h"
#include "..\btl_client.h"
#include "..\btl_event_factor.h"

#include "hand_waza.h"

/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
  WORKIDX_STICK = EVENT_HANDLER_WORK_ELEMS - 1,
};

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static BOOL is_registerd( u8 pokeID, WazaID waza );
static BTL_EVENT_FACTOR*  ADD_Texture( u16 pri, WazaID waza, u8 pokeID );
static void handler_Texture( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Refresh( u16 pri, WazaID waza, u8 pokeID );
static void handler_Refresh( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_KumoNoSu( u16 pri, WazaID waza, u8 pokeID );
static void handler_KumoNoSu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_KuroiKiri( u16 pri, WazaID waza, u8 pokeID );
static void handler_KuroiKiri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Haneru( u16 pri, WazaID waza, u8 pokeID );
static void handler_Haneru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Yumekui( u16 pri, WazaID waza, u8 pokeID );
static void handler_Yumekui( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_TriAttack( u16 pri, WazaID waza, u8 pokeID );
static void handler_TriAttack( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_IkariNoMaeba( u16 pri, WazaID waza, u8 pokeID );
static void handler_IkariNoMaeba( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_RyuuNoIkari( u16 pri, WazaID waza, u8 pokeID );
static void handler_RyuuNoIkari( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_SonicBoom( u16 pri, WazaID waza, u8 pokeID );
static void handler_SonicBoom( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Gamusyara( u16 pri, WazaID waza, u8 pokeID );
static void handler_Gamusyara( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_TikyuuNage( u16 pri, WazaID waza, u8 pokeID );
static void handler_TikyuuNage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_PsycoWave( u16 pri, WazaID waza, u8 pokeID );
static void handler_PsycoWave( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Ibiki( u16 pri, WazaID waza, u8 pokeID );
static void handler_Ibiki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Ikari( u16 pri, WazaID waza, u8 pokeID );
static void handler_Ikari_Exe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Ikari_React( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Abareru( u16 pri, WazaID waza, u8 pokeID );
static void handler_Abareru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Abareru_turnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Jitabata( u16 pri, WazaID waza, u8 pokeID );
static void handler_Jitabata( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Funka( u16 pri, WazaID waza, u8 pokeID );
static void handler_Funka( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Siboritoru( u16 pri, WazaID waza, u8 pokeID );
static void handler_Siboritoru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Siomizu( u16 pri, WazaID waza, u8 pokeID );
static void handler_Siomizu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_MezamasiBinta( u16 pri, WazaID waza, u8 pokeID );
static void handler_MezamasiBinta( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MezamasiBinta_AfterDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_WeatherBall( u16 pri, WazaID waza, u8 pokeID );
static void handler_WeatherBall_Type( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_WeatherBall_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Mineuti( u16 pri, WazaID waza, u8 pokeID );
static void handler_Mineuti( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Koraeru( u16 pri, WazaID waza, u8 pokeID );
static void handler_Koraeru_ExeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Koraeru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Mamoru( u16 pri, WazaID waza, u8 pokeID );
static void handler_Mamoru_ExeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Mamoru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_Nekodamasi( u16 pri, WazaID waza, u8 pokeID );
static void handler_Nekodamasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR*  ADD_AsaNoHizasi( u16 pri, WazaID waza, u8 pokeID );
static void handler_AsaNoHizasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );




//=============================================================================================
/**
 * �|�P�����́u���U�v�n���h�����V�X�e���ɒǉ�
 *
 * @param   pp
 *
 * @retval  BTL_EVENT_FACTOR*
 */
//=============================================================================================
BTL_EVENT_FACTOR*  BTL_HANDLER_Waza_Add( const BTL_POKEPARAM* pp, WazaID waza )
{
  typedef BTL_EVENT_FACTOR* (*pEventAddFunc)( u16 pri, WazaID wazaID, u8 pokeID );

  static const struct {
    WazaID         waza;
    pEventAddFunc  func;
  }funcTbl[] = {
    { WAZANO_TEKUSUTYAA,      ADD_Texture       },
    { WAZANO_KUROIKIRI,       ADD_KuroiKiri     },
    { WAZANO_YUMEKUI,         ADD_Yumekui       },
    { WAZANO_TORAIATAKKU,     ADD_TriAttack     },
    { WAZANO_IKARINOMAEBA,    ADD_IkariNoMaeba  },
    { WAZANO_RYUUNOIKARI,     ADD_RyuuNoIkari   },
    { WAZANO_TIKYUUNAGE,      ADD_TikyuuNage    },
    { WAZANO_NAITOHEDDO,      ADD_TikyuuNage    },  // �i�C�g�w�b�h=�����イ�Ȃ��Ɠ���
    { WAZANO_SAIKOWHEEBU,     ADD_PsycoWave     },
    { WAZANO_IBIKI,           ADD_Ibiki         },
    { WAZANO_ZITABATA,        ADD_Jitabata      },
    { WAZANO_KISIKAISEI,      ADD_Jitabata      },  // ������������=�����΂� �Ɠ���
    { WAZANO_MINEUTI,         ADD_Mineuti       },
    { WAZANO_KUMONOSU,        ADD_KumoNoSu      },
    { WAZANO_KUROIMANAZASI,   ADD_KumoNoSu      },  // ���낢�܂Ȃ��� = �����̂� �Ɠ���
    { WAZANO_TOOSENBOU,       ADD_KumoNoSu      },  // �Ƃ�����ڂ�  = �����̂� �Ɠ���
    { WAZANO_KORAERU,         ADD_Koraeru       },
    { WAZANO_SONIKKUBUUMU,    ADD_SonicBoom     },
    { WAZANO_NEKODAMASI,      ADD_Nekodamasi    },
    { WAZANO_GAMUSYARA,       ADD_Gamusyara     },
    { WAZANO_HUNKA,           ADD_Funka         },
    { WAZANO_SIOHUKI,         ADD_Funka         },  // �����ӂ�=�ӂ� �Ɠ���
    { WAZANO_RIHURESSYU,      ADD_Refresh       },
    { WAZANO_ASANOHIZASI,     ADD_AsaNoHizasi   },
    { WAZANO_TUKINOHIKARI,    ADD_AsaNoHizasi   },  // ���̂Ђ��� = �����̂Ђ��� �Ɠ���
    { WAZANO_KOUGOUSEI,       ADD_AsaNoHizasi   },  // ������������ = �����̂Ђ��� �Ɠ���
    { WAZANO_SIBORITORU,      ADD_Siboritoru    },
    { WAZANO_NIGIRITUBUSU,    ADD_Siboritoru    },  // �ɂ���Ԃ� = ���ڂ�Ƃ� �Ɠ���
    { WAZANO_WHEZAABOORU,     ADD_WeatherBall   },
    { WAZANO_MAMORU,          ADD_Mamoru        },
    { WAZANO_MIKIRI,          ADD_Mamoru        },  // �݂��� = �܂��� �Ɠ���
    { WAZANO_HANERU,          ADD_Haneru        },
    { WAZANO_ABARERU,         ADD_Abareru       },
    { WAZANO_HANABIRANOMAI,   ADD_Abareru       },  // �͂Ȃт�̂܂� = ���΂�� �Ɠ���
    { WAZANO_GEKIRIN,         ADD_Abareru       },  // ������� = ���΂�� �Ɠ���
    { WAZANO_SIOMIZU,         ADD_Siomizu       },
  };

  int i;

  for(i=0; i<NELEMS(funcTbl); i++)
  {
    if( funcTbl[i].waza == waza )
    {
      u8 pokeID = BTL_POKEPARAM_GetID( pp );

      if( !is_registerd(pokeID, waza) ){
        u16 agi = BTL_POKEPARAM_GetValue( pp, BPP_AGILITY );
        return funcTbl[i].func( agi, waza, pokeID );
      }
    }
  }

  return NULL;
}
//----------------------------------------------------------------------------------
/**
 * ���ɓ����|�P���������U�̃n���h�����o�^����Ă��邩�H
 *
 * @param   pokeID
 * @param   waza
 *
 * @retval  BOOL    �o�^����Ă�����TRUE
 */
//----------------------------------------------------------------------------------
static BOOL is_registerd( u8 pokeID, WazaID waza )
{
  BTL_EVENT_FACTOR* factor;

  factor = BTL_EVENT_SeekFactor( BTL_EVENT_FACTOR_WAZA, pokeID );
  while( factor )
  {
    if( BTL_EVENT_FACTOR_GetSubID(factor) == waza ){
      return TRUE;
    }
    factor = BTL_EVENT_GetNextFactor( factor );
  }
  return FALSE;
}
//=============================================================================================
/**
 * �|�P�����́u���U�v�n���h�����V�X�e������폜�i���U�̋����ɂ��\��t���Ă�����͍̂폜���Ȃ��j
 *
 * @param   pp
 * @param   waza
 */
//=============================================================================================
void BTL_HANDLER_Waza_Remove( const BTL_POKEPARAM* pp, WazaID waza )
{
  BTL_EVENT_FACTOR *factor, *next;
  u8 pokeID = BTL_POKEPARAM_GetID( pp );

  factor = BTL_EVENT_SeekFactor( BTL_EVENT_FACTOR_WAZA, pokeID );
  while( factor )
  {
    next = BTL_EVENT_GetNextFactor( factor );
    if( (BTL_EVENT_FACTOR_GetSubID(factor) == waza)
    &&  (BTL_EVENT_FACTOR_GetWorkValue(factor, WORKIDX_STICK) == 0)
    ){
      BTL_Printf("pokeID[%d], waza[%d]�n���h���폜\n", pokeID, waza);
      BTL_EVENT_FACTOR_Remove( factor );
    }
    factor = next;
  }
}
//=============================================================================================
/**
 * �|�P�����́u���U�v�n���h�����V�X�e�����狭���폜�i�\��t���Ă�����̂������I�ɍ폜�j
 *
 * @param   pp
 * @param   waza
 */
//=============================================================================================
void BTL_HANDLER_Waza_RemoveForce( const BTL_POKEPARAM* pp, WazaID waza )
{
  BTL_EVENT_FACTOR* factor;
  u8 pokeID = BTL_POKEPARAM_GetID( pp );

  while( (factor = BTL_EVENT_SeekFactor(BTL_EVENT_FACTOR_WAZA, pokeID)) != NULL )
  {
    if( BTL_EVENT_FACTOR_GetSubID(factor) == waza){
      BTL_EVENT_FACTOR_Remove( factor );
    }
  }
}




//----------------------------------------------------------------------------------
/**
 * �e�N�X�`���[
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Texture( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORY_WAZA, handler_Texture },    // �����ރ��U�n���h��
//    { BTL_EVENT_USE_ITEM,      handler_KuraboNomi_Exe },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Texture( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    u16 waza_cnt = BTL_POKEPARAM_GetWazaCount( bpp );
    PokeType next_type;
    {
      PokeType type[ PTL_WAZA_MAX ];
      u16 i;
      for(i=0; i<waza_cnt; ++i){
        type[i] = WAZADATA_GetType( BTL_POKEPARAM_GetWazaNumber(bpp, i) );
      }
      i = GFL_STD_MtRand( waza_cnt );
      next_type = PokeTypePair_MakePure( type[i] );
    }

    {
      BTL_HANDEX_PARAM_CHANGE_TYPE* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_CHANGE_TYPE, pokeID );
      param->next_type = next_type;
      param->pokeID = pokeID;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ���t���b�V��
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Refresh( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORY_WAZA, handler_Refresh },    // �����ރ��U�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Refresh( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    PokeSick sick = BTL_POKEPARAM_GetPokeSick( bpp );

    if( (sick == POKESICK_DOKU)
    ||  (sick == POKESICK_MAHI)
    ||  (sick == POKESICK_YAKEDO)
    ){
      BTL_HANDEX_PARAM_CURE_SICK* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
      param->sickID = WAZASICK_NULL;
      param->pokeID = pokeID;
      // @@@ ���b�Z�[�W���邩�H
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * �N���̂��A���낢�܂Ȃ����A�Ƃ�����ڂ�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_KumoNoSu( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORY_WAZA, handler_KumoNoSu },    // �����ރ��U�n���h��
//    { BTL_EVENT_USE_ITEM,      handler_KuraboNomi_Exe },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_KumoNoSu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    u16 waza_cnt = BTL_POKEPARAM_GetWazaCount( bpp );
    PokeType next_type;
    {
      PokeType type[ PTL_WAZA_MAX ];
      u16 i;
      for(i=0; i<waza_cnt; ++i){
        type[i] = WAZADATA_GetType( BTL_POKEPARAM_GetWazaNumber(bpp, i) );
      }
      i = GFL_STD_MtRand( waza_cnt );
      next_type = PokeTypePair_MakePure( type[i] );
    }

    {
      BTL_HANDEX_PARAM_CHANGE_TYPE* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_CHANGE_TYPE, pokeID );
      param->next_type = next_type;
      param->pokeID = pokeID;
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * ���낢����
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_KuroiKiri( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORY_WAZA, handler_KuroiKiri },    // �����ރ��U�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_KuroiKiri( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_RESET_RANK* reset_param;
    BTL_HANDEX_PARAM_MESSAGE* msg_param;

    reset_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RESET_RANK, pokeID );
    msg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE_STD, pokeID );

    {
      BtlPokePos myPos = BTL_SVFLOW_CheckExistFrontPokeID( flowWk, pokeID );
      BtlExPos   expos = EXPOS_MAKE( BTL_EXPOS_ALL, myPos );

      reset_param->poke_cnt = BTL_SERVERFLOW_RECEPT_GetTargetPokeID( flowWk, expos, reset_param->pokeID );
    }

    msg_param->strID = BTL_STRID_STD_RankReset;
    msg_param->arg_cnt = 0;
  }
}

//----------------------------------------------------------------------------------
/**
 * �͂˂�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Haneru( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_UNCATEGORY_WAZA, handler_Haneru },    // �����ރ��U�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Haneru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_MESSAGE* param;

    param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE_STD, pokeID );
    param->strID = BTL_STRID_STD_NotHappen;
    param->arg_cnt = 0;
  }
}

//----------------------------------------------------------------------------------
/**
 * ��߂���
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Yumekui( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2, handler_Yumekui },    // �������`�F�b�N���x���Q�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Yumekui( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* target = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, targetPokeID );

    if( !BTL_POKEPARAM_CheckSick(target, POKESICK_NEMURI) )
    {
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, TRUE );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �g���C�A�^�b�N
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_TriAttack( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2, handler_TriAttack },    // �������`�F�b�N���x���Q�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_TriAttack( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    static const PokeSick sick_tbl[] = {
      POKESICK_MAHI, POKESICK_YAKEDO, POKESICK_KOORI
    };
    PokeSick sick;
    BPP_SICK_CONT cont;
    u8 rand = GFL_STD_MtRand( NELEMS(sick_tbl) );

    sick = sick_tbl[ rand ];
    BTL_CALC_MakeDefaultPokeSickCont( sick, &cont );

    BTL_EVENTVAR_RewriteValue( BTL_EVAR_SICKID, sick );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_SICK_CONT, cont.raw );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_ADD_PER, 20 );
  }
}
//----------------------------------------------------------------------------------
/**
 * ������̂܂���
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_IkariNoMaeba( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2, handler_IkariNoMaeba },    // �_���[�W�v�Z�ŏI�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_IkariNoMaeba( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    const BTL_POKEPARAM* target = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, targetPokeID );

    u16 damage = BTL_POKEPARAM_GetValue(target, BPP_HP) / 2;

    BTL_EVENTVAR_RewriteValue( BTL_EVAR_DAMAGE, damage );
  }
}
//----------------------------------------------------------------------------------
/**
 * ��イ�̂�����
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_RyuuNoIkari( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2, handler_RyuuNoIkari },    // �_���[�W�v�Z�ŏI�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_RyuuNoIkari( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_DAMAGE, 40 );
  }
}
//----------------------------------------------------------------------------------
/**
 * �\�j�b�N�u�[��
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_SonicBoom( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2, handler_SonicBoom },    // �_���[�W�v�Z�ŏI�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_SonicBoom( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_DAMAGE, 20 );
  }
}
//----------------------------------------------------------------------------------
/**
 * ���ނ����
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Gamusyara( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2, handler_Gamusyara },    // �_���[�W�v�Z�ŏI�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Gamusyara( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM *attacker, *defender;
    int diff;

    attacker = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    defender = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    diff = BTL_POKEPARAM_GetValue(defender, BPP_HP) - BTL_POKEPARAM_GetValue(attacker, BPP_HP);
    if( diff < 0 ){
      diff = 0;
    }
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_DAMAGE, diff );
  }
}
//----------------------------------------------------------------------------------
/**
 * �����イ�Ȃ��A�i�C�g�w�b�h
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_TikyuuNage( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2, handler_TikyuuNage },    // �_���[�W�v�Z�ŏI�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_TikyuuNage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    u8 level = BTL_POKEPARAM_GetValue( bpp, BPP_LEVEL );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_DAMAGE, level );
  }
}
//----------------------------------------------------------------------------------
/**
 * �T�C�R�E�F�[�u
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_PsycoWave( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2, handler_PsycoWave },    // �_���[�W�v�Z�ŏI�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_PsycoWave( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    u8 level = BTL_POKEPARAM_GetValue( bpp, BPP_LEVEL );
    u16 rand = 50 + GFL_STD_MtRand(101);
    u16 damage = level * rand / 100;
    if( damage == 0 ){
      damage = 1;
    }
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_DAMAGE, damage );
  }
}
//----------------------------------------------------------------------------------
/**
 * ���т�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Ibiki( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK, handler_Ibiki },    // ���U�o�����ۃ`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Ibiki( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    if( !BTL_POKEPARAM_CheckSick(bpp, POKESICK_NEMURI) ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_OTHER );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ������
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Ikari( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_FIX, handler_Ikari_Exe },    // ���U�o���m��n���h��
    { BTL_EVENT_WAZA_DMG_AFTER, handler_Ikari_React },    // �_���[�W�����n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Ikari_Exe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_WAZAID) == BTL_EVENT_FACTOR_GetSubID(myHandle) ){
      work[ WORKIDX_STICK ] = 1;
    }else{
      const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
      BTL_HANDLER_Waza_RemoveForce( bpp, BTL_EVENT_FACTOR_GetSubID(myHandle) );
    }
  }
}
static void handler_Ikari_React( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    if( !BTL_POKEPARAM_IsDead(bpp) )
    {
      BTL_HANDEX_PARAM_RANK_EFFECT* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RANK_EFFECT, pokeID );

      param->rankType = WAZA_RANKEFF_ATTACK;
      param->rankVolume = 1;
      param->fAlmost = FALSE;
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
    }
    BTL_HANDLER_Waza_RemoveForce( bpp, BTL_EVENT_FACTOR_GetSubID(myHandle) );
  }
}


//----------------------------------------------------------------------------------
/**
 * ���΂��E�͂Ȃт�̂܂��E�������
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Abareru( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_FIX, handler_Abareru },    // ���U�o���m��n���h��
    { BTL_EVENT_TURNCHECK_END,  handler_Abareru_turnCheck },    // �^�[���`�F�b�N�I���n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}

static void handler_Abareru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    // ���������U���b�N��ԂɂȂ��Ă��Ȃ��Ȃ�A���U���b�N��Ԃɂ���
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    if( (!BTL_POKEPARAM_CheckSick(bpp, WAZASICK_WAZALOCK_HARD))
    &&  ( work[ WORKIDX_STICK ] == 0 )
    ){
      BTL_HANDEX_PARAM_ADD_SICK* param;
      u8 turns;

      turns = 3 + GFL_STD_MtRand(2);

      param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
      param->sickID = WAZASICK_WAZALOCK_HARD;
      BPP_SICKCONT_Set_Turn( &param->sickCont, turns );
      param->fAlmost = FALSE;
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
      param->fExMsg = FALSE;

      work[ WORKIDX_STICK ] = 1;
    }
  }
}
static void handler_Abareru_turnCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( work[WORKIDX_STICK] != 0)
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    BTL_Printf("���΂�Ă��\n");
    if( !BTL_POKEPARAM_CheckSick(bpp, WAZASICK_WAZALOCK_HARD)
    &&  !BTL_POKEPARAM_CheckSick(bpp, WAZASICK_KONRAN)
    ){
      BTL_HANDEX_PARAM_ADD_SICK* param;

      BTL_Printf("���������\n");

      param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
      param->sickID = WAZASICK_KONRAN;
      BTL_CALC_MakeDefaultWazaSickCont( param->sickID, bpp, &param->sickCont );
      param->fAlmost = FALSE;
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
      param->fExMsg = FALSE;

      BTL_HANDLER_Waza_RemoveForce( bpp, BTL_EVENT_FACTOR_GetSubID(myHandle) );
    }
  }
}


//----------------------------------------------------------------------------------
/**
 * �����΂��A������������
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Jitabata( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVAR_WAZA_POWER, handler_Jitabata },    // ���U�З̓`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Jitabata( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  static const struct {
    fx32  ratio;
    u32   pow;
  }tbl[]={
    { FX32_CONST(2.0),  200 },
    { FX32_CONST(7.8),  150 },
    { FX32_CONST(18.7), 100 },
    { FX32_CONST(32.8),  80 },
    { FX32_CONST(65.6),  40 },
    { FX32_CONST(100),   20 },
  };

  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    fx32 r = BTL_POKEPARAM_GetHPRatio( bpp );
    u32 i, max;

    max = NELEMS(tbl) - 1;
    for(i=0; i<max; ++i)
    {
      if( tbl[i].ratio < r ){ break; }
    }
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, tbl[i].pow );
  }
}
//----------------------------------------------------------------------------------
/**
 * �ӂ񂩁A�����ӂ�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Funka( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVAR_WAZA_POWER, handler_Funka },    // ���U�З̓`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Funka( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    fx32 r = BTL_POKEPARAM_GetHPRatio( bpp );
    u32 pow = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER );

    pow = BTL_CALC_MulRatio_OverZero( pow, r );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
//----------------------------------------------------------------------------------
/**
 * ���ڂ�Ƃ�E�ɂ���Ԃ�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Siboritoru( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_Siboritoru },    // ���U�З̓`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Siboritoru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    fx32 r = BTL_POKEPARAM_GetHPRatio( bpp );
    u32 pow = BTL_CALC_MulRatio_OverZero( 120, r );
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}
//----------------------------------------------------------------------------------
/**
 * �����݂�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Siomizu( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER, handler_Siomizu },    // ���U�З̓`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Siomizu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    fx32 r = BTL_POKEPARAM_GetHPRatio( bpp );
    if( r <= FX32_CONST(50) )
    {
      u32 pow = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER );
      pow *= 2;
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �߂��܂��r���^
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_MezamasiBinta( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_POWER,   handler_MezamasiBinta },    // ���U�З̓`�F�b�N�n���h��
    { BTL_EVENT_AFTER_DAMAGE, handler_MezamasiBinta_AfterDamage },
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_MezamasiBinta( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );
    if( BTL_POKEPARAM_CheckSick(bpp, WAZASICK_NEMURI) ){
      u32 pow = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER );
      pow *= 2;
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
    }
  }
}
static void handler_MezamasiBinta_AfterDamage( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    u8 target_pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, target_pokeID );
    if( BTL_POKEPARAM_CheckSick(bpp, WAZASICK_NEMURI) ){
      BTL_HANDEX_PARAM_CURE_SICK* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
      param->pokeID = target_pokeID;
      param->sickID = WAZASICK_NEMURI;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �E�F�U�[�{�[��
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_WeatherBall( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_PARAM, handler_WeatherBall_Type },  // ���U�p�����[�^�`�F�b�N�n���h��
    { BTL_EVENT_WAZA_POWER, handler_WeatherBall_Pow },   // ���U�З̓`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_WeatherBall_Type( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  {
    BtlWeather weather = BTL_FIELD_GetWeather();
    PokeType type = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_TYPE );
    switch( weather ){
    case BTL_WEATHER_SHINE:  type = POKETYPE_HONOO; break;
    case BTL_WEATHER_RAIN:   type = POKETYPE_MIZU; break;
    case BTL_WEATHER_SAND:   type = POKETYPE_IWA; break;
    case BTL_WEATHER_SNOW:   type = POKETYPE_KOORI; break;
    }
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_TYPE, type );
  }
}
static void handler_WeatherBall_Pow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) != POKETYPE_NORMAL)
  ){
    u32 pow = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER );
    pow *= 2;
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_WAZA_POWER, pow );
  }
}



//----------------------------------------------------------------------------------
/**
 * �݂˂���
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Mineuti( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_KORAERU_CHECK, handler_Mineuti },    // ���炦��`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Mineuti( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_KORAERU_CAUSE, BPP_KORAE_WAZA_ATTACKER );
  }
}
//----------------------------------------------------------------------------------
/**
 * ���炦��
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Koraeru( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK,  handler_Koraeru_ExeCheck }, // ���U�o�����ۃ`�F�b�N�n���h��
    { BTL_EVENT_KORAERU_CHECK,       handler_Koraeru },          // ���炦��`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Koraeru_ExeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �����������񂾂񉺂���e�[�u�� 1/2, 1/4, 1/8, ---
  static const u8 randRange[] = {
    2, 4, 8,
  };

  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    u8 counter = BTL_POKEPARAM_GetSameWazaUsedCounter( bpp );

    if( counter )
    {
      if( counter >= NELEMS(randRange) ){
        counter = NELEMS(randRange) - 1;
      }
      if( GFL_STD_MtRand( randRange[counter] ) != 0 )
      {
        // �A�����p�ɂ�鎸�s�B���s������n���h�����Ə��ł��邱�ƂŁA���炦��`�F�b�N�ɔ������Ȃ��B
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_OTHER );
        BTL_EVENT_FACTOR_Remove( myHandle );
      }
    }
  }
}
static void handler_Koraeru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_KORAERU_CAUSE, BPP_KORAE_WAZA_DEFENDER );
  }
}
//----------------------------------------------------------------------------------
/**
 * �܂���E�݂���
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Mamoru( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK,  handler_Mamoru_ExeCheck }, // ���U�o�����ۃ`�F�b�N�n���h��
    { BTL_EVENT_UNCATEGORY_WAZA,     handler_Mamoru },          // �����ރ��U�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Mamoru_ExeCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  // �����������񂾂񉺂���e�[�u�� 1/2, 1/4, 1/8, ---
  static const u8 randRange[] = {
    2, 4, 8,
  };

  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    u8 counter = BTL_POKEPARAM_GetSameWazaUsedCounter( bpp );

    if( counter )
    {
      if( counter >= NELEMS(randRange) ){
        counter = NELEMS(randRange) - 1;
      }
      if( GFL_STD_MtRand( randRange[counter] ) != 0 )
      {
        // �A�����p�ɂ�鎸�s�B���s������n���h�����Ə��ł��邱�ƂŁA���̌�̃C�x���g�ɔ������Ȃ��B
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_OTHER );
        BTL_EVENT_FACTOR_Remove( myHandle );
      }
    }
  }
}
static void handler_Mamoru( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
  {
    BTL_HANDEX_PARAM_SET_TURNFLAG* flagParam;

    flagParam = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_SET_TURNFLAG, pokeID );
    flagParam->pokeID = pokeID;
    flagParam->flag = BPP_TURNFLG_MAMORU;
  }
}


//----------------------------------------------------------------------------------
/**
 * �˂����܂�
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_Nekodamasi( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_EXECUTE_CHECK,       handler_Nekodamasi },     // ���U�o�����ۃ`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_Nekodamasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    if( BTL_POKEPARAM_GetTurnCount(bpp) )
    {
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_OTHER );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �����̂Ђ����E�������������E���̂Ђ���
 */
//----------------------------------------------------------------------------------
static BTL_EVENT_FACTOR*  ADD_AsaNoHizasi( u16 pri, WazaID waza, u8 pokeID )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_RECOVER_HP_RATIO,       handler_AsaNoHizasi },     // HP�񕜗��v�Z�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, waza, pri, pokeID, HandlerTable );
}
static void handler_AsaNoHizasi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
  if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
  {
    BtlWeather w = BTL_FIELD_GetWeather();
    u8 ratio = 50;
    switch( w ){
    case BTL_WEATHER_SHINE:  ratio = 66; break;
    case BTL_WEATHER_RAIN:
    case BTL_WEATHER_SAND:
    case BTL_WEATHER_SNOW:
      ratio = 25;
      break;
    }
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_RATIO, ratio );
  }
}
