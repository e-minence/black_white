//=============================================================================================
/**
 * @file  hand_side.c
 * @brief �|�P����WB �o�g���V�X�e�� �C�x���g�t�@�N�^�[ [�T�C�h�G�t�F�N�g]
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
#include "..\btlv\btlv_effect.h"

#include "hand_side.h"

/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
  WORKIDX_CONT = EVENT_HANDLER_WORK_ELEMS - 1,
};


/**
 *  �d���Ǘ��p���[�N
 */
typedef struct {

  BTL_EVENT_FACTOR*  factor;      ///< �{�̃n���h��
  BPP_SICK_CONT  contParam;       ///< �p���p�����[�^
  u32            turn_counter;    ///< �^�[�����J�E���^
  u32            add_counter;     ///< �d�˂����J�E���^

}EXIST_EFFECT;

static EXIST_EFFECT ExistEffect[ BTL_SIDE_MAX ][ BTL_SIDEEFF_MAX ];


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static u8 getMyAddCounter( BTL_EVENT_FACTOR* myHandle, BtlSide side );
static const BtlEventHandlerTable* ADD_SIDE_Refrector( u32* numElems );
static void handler_side_Refrector( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work );
static const BtlEventHandlerTable* ADD_SIDE_Hikarinokabe( u32* numElems );
static void handler_side_HikariNoKabe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work );
static void common_wallEffect( BTL_SVFLOW_WORK* flowWk, u8 mySide, WazaDamageType dmgType );
static const BtlEventHandlerTable* ADD_SIDE_Sinpinomamori( u32* numElems );
static void handler_side_SinpiNoMamori_CheckFail( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work );
static void handler_side_SinpiNoMamori_FixFail( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work );
static const BtlEventHandlerTable* ADD_SIDE_SiroiKiri( u32* numElems );
static void handler_side_SiroiKiri_CheckFail( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work );
static void handler_side_SiroiKiri_FixFail( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work );
static const BtlEventHandlerTable* ADD_SIDE_Oikaze( u32* numElems );
static void handler_side_Oikaze( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work );
static const BtlEventHandlerTable* ADD_SIDE_Omajinai( u32* numElems );
static void handler_side_Omajinai( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work );
static const BtlEventHandlerTable* ADD_SIDE_StealthRock( u32* numElems );
static void handler_side_StealthRock( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work );
static const BtlEventHandlerTable* ADD_SIDE_WideGuard( u32* numElems );
static void handler_side_WideGuard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work );
static const BtlEventHandlerTable* ADD_SIDE_FastGuard( u32* numElems );
static void handler_side_FastGuard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work );
static const BtlEventHandlerTable* ADD_SIDE_Makibisi( u32* numElems );
static void handler_side_Makibisi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work );
static const BtlEventHandlerTable* ADD_SIDE_Dokubisi( u32* numElems );
static void handler_side_Dokubisi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work );
static const BtlEventHandlerTable* ADD_SIDE_Rainbow( u32* numElems );
static void handler_Rainbow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work );
static void handler_Rainbow_Shrink( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work );
static const BtlEventHandlerTable* ADD_SIDE_Burning( u32* numElems );
static void handler_side_Burning( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work );
static const BtlEventHandlerTable* ADD_SIDE_Moor( u32* numElems );
static void handler_side_Moor( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work );




//=============================================================================================
/**
 * �V�X�e��������
 *
 * @param   none
 */
//=============================================================================================
void BTL_HANDLER_SIDE_InitSystem( void )
{
  GFL_STD_MemClear( ExistEffect, sizeof(ExistEffect) );
}

//=============================================================================================
/**
 * �T�C�h�G�t�F�N�g�n���h�����V�X�e���ɒǉ�
 *
 * @param   side
 * @param   sideEffect
 * @param   contParam
 *
 * @retval  BTL_EVENT_FACTOR*   �ǉ����ꂽ�C�x���g�n���h���i�d�����Ēǉ��ł��Ȃ��ꍇNULL�j
 */
//=============================================================================================
BTL_EVENT_FACTOR*  BTL_HANDLER_SIDE_Add( BtlSide side, BtlSideEffect sideEffect, BPP_SICK_CONT contParam )
{
  typedef const BtlEventHandlerTable* (*pEventAddFunc)( u32* numHandlers );

  static const struct {
    BtlSideEffect  eff;
    pEventAddFunc  func;
    u32            add_max;
  }funcTbl[] = {
    { BTL_SIDEEFF_REFRECTOR,      ADD_SIDE_Refrector,      1   },
    { BTL_SIDEEFF_HIKARINOKABE,   ADD_SIDE_Hikarinokabe,   1   },
    { BTL_SIDEEFF_SINPINOMAMORI,  ADD_SIDE_Sinpinomamori,  1   },
    { BTL_SIDEEFF_SIROIKIRI,      ADD_SIDE_SiroiKiri,      1   },
    { BTL_SIDEEFF_OIKAZE,         ADD_SIDE_Oikaze,         1   },
    { BTL_SIDEEFF_OMAJINAI,       ADD_SIDE_Omajinai,       1   },
    { BTL_SIDEEFF_MAKIBISI,       ADD_SIDE_Makibisi,       3   },
    { BTL_SIDEEFF_DOKUBISI,       ADD_SIDE_Dokubisi,       2   },
    { BTL_SIDEEFF_STEALTHROCK,    ADD_SIDE_StealthRock,    1   },
    { BTL_SIDEEFF_WIDEGUARD,      ADD_SIDE_WideGuard,      1   },
    { BTL_SIDEEFF_FASTGUARD,      ADD_SIDE_FastGuard,      1   },
    { BTL_SIDEEFF_RAINBOW,        ADD_SIDE_Rainbow,        1   },
    { BTL_SIDEEFF_BURNING,        ADD_SIDE_Burning,        1   },
    { BTL_SIDEEFF_MOOR,           ADD_SIDE_Moor,           1   },

  };

  GF_ASSERT(side < BTL_SIDE_MAX);
  GF_ASSERT(sideEffect < BTL_SIDEEFF_MAX);

  {
    u32 i;
    for(i=0; i<NELEMS(funcTbl); ++i)
    {
      if( funcTbl[i].eff == sideEffect )
      {
        EXIST_EFFECT* effParam = &ExistEffect[ side ][ sideEffect ];

        if( effParam->add_counter == 0 )
        {
          const BtlEventHandlerTable* handlerTable;
          u32 numHandlers;
          BTL_EVENT_FACTOR* factor;
          handlerTable = funcTbl[i].func( &numHandlers );
          factor = BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_SIDE, sideEffect,
                BTL_EVPRI_SIDE_DEFAULT, 0, side, handlerTable, numHandlers );

          BTL_EVENT_FACTOR_SetWorkValue( factor, WORKIDX_CONT, contParam.raw );
          effParam->add_counter = 1;
          BTL_N_Printf( DBGSTR_SIDE_AddFirst, side, sideEffect, &(effParam->add_counter), effParam->add_counter );
          effParam->turn_counter = 0;
          effParam->contParam = contParam;
          effParam->factor = factor;

          return factor;
        }
        else if( effParam->add_counter < funcTbl[i].add_max )
        {
          effParam->add_counter++;
          return effParam->factor;
        }
        else{
          BTL_N_Printf( DBGSTR_SIDE_NoMoreAdd, side, sideEffect );
        }
      }
    }
  }
  return NULL;
}



//=============================================================================================
/**
 * �T�C�h�G�t�F�N�g�̏d�˂����J�E���^���擾
 *
 * @param   side
 * @param   effect
 *
 * @retval  u32
 */
//=============================================================================================
u32 BTL_HANDLER_SIDE_GetAddCount( BtlSide side, BtlSideEffect effect )
{
  GF_ASSERT(side < BTL_SIDE_MAX);
  GF_ASSERT(effect < BTL_SIDEEFF_MAX);

  return ExistEffect[ side ][ effect ].add_counter;
}


//=============================================================================================
/**
 * ����T�C�h�G�t�F�N�g�����݂��Ă��邩����
 *
 * @param   side
 * @param   effect
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTL_HANDER_SIDE_IsExist( BtlSide side, BtlSideEffect effect )
{
  EXIST_EFFECT* eff = &ExistEffect[ side ][ effect ];

  return  (eff->factor != NULL);
}
//=============================================================================================
/**
 * �n���h���폜
 *
 * @param   side
 * @param   sideEffect
 *
 * @retval  BOOL    �w�肳�ꂽ�T�C�h�G�t�F�N�g���L���������ꍇTRUE
 */
//=============================================================================================
BOOL BTL_HANDLER_SIDE_Remove( BtlSide side, BtlSideEffect sideEffect )
{
  EXIST_EFFECT* eff = &ExistEffect[ side ][ sideEffect ];

  if( eff->factor )
  {
    BTL_EVENT_FACTOR_Remove( eff->factor );
    eff->factor = NULL;
    eff->add_counter = 0;
    eff->contParam.type = WAZASICK_CONT_NONE;
    return TRUE;
  }
  return FALSE;
}

//=============================================================================================
/**
 *  �^�[���`�F�b�N����
 *
 * @param   callBack      �I��������ʂ�ʒm���邽�߂̃R�[���o�b�N�֐�
 * @param   callbackArg   �R�[���o�b�N�֐��Ɉ����n���ėp����
 */
//=============================================================================================
void BTL_HANDLER_SIDE_TurnCheck( pSideEffEndCallBack callBack, void* callbackArg )
{
  u32 side, i;
  for(side = 0; side < BTL_SIDE_MAX; ++side)
  {
    for(i=BTL_SIDEEFF_START; i<BTL_SIDEEFF_MAX; ++i)
    {
      if( ExistEffect[side][i].add_counter )
      {
        EXIST_EFFECT* eff = &ExistEffect[ side ][ i ];
        if( eff->contParam.type == WAZASICK_CONT_TURN )
        {
          if( ++(eff->turn_counter) >= eff->contParam.turn.count )
          {
            eff->add_counter = 0;
            eff->contParam.type = WAZASICK_CONT_NONE;
            BTL_EVENT_FACTOR_Remove( eff->factor );
            eff->factor = NULL;
            callBack( side, i, callbackArg );
          }
        }
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �����̏d�˂����J�E���^�l���擾
 *
 * @param   myHandle
 * @param   side
 *
 * @retval  u8
 */
//----------------------------------------------------------------------------------
static u8 getMyAddCounter( BTL_EVENT_FACTOR* myHandle, BtlSide side )
{
  BtlSideEffect  eff = BTL_EVENT_FACTOR_GetSubID( myHandle );
  if( (eff < BTL_SIDEEFF_MAX) && (side < BTL_SIDE_MAX) )
  {
    return ExistEffect[ side ][ eff ].add_counter;
  }
  GF_ASSERT_MSG(0, "effID=%d, side=%d\n", eff, side);
  return 0;
}


//--------------------------------------------------------------------------------------
/**
 *  ���t���N�^�[
 */
//--------------------------------------------------------------------------------------
static const BtlEventHandlerTable* ADD_SIDE_Refrector( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,  handler_side_Refrector   },  // �_���[�W�␳
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_side_Refrector( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work )
{
  common_wallEffect( flowWk, mySide, WAZADATA_DMG_PHYSIC );
}
//--------------------------------------------------------------------------------------
/**
 *  �Ђ���̂���
 */
//--------------------------------------------------------------------------------------
static const BtlEventHandlerTable* ADD_SIDE_Hikarinokabe( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,  handler_side_HikariNoKabe   },  // �_���[�W�␳
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_side_HikariNoKabe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work )
{
  common_wallEffect( flowWk, mySide, WAZADATA_DMG_SPECIAL );
}
/**
 *  ���t���N�^�[�E�Ђ���̂��׋���
 */
static void common_wallEffect( BTL_SVFLOW_WORK* flowWk, u8 mySide, WazaDamageType dmgType )
{
  u8 defPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  if( BTL_MAINUTIL_PokeIDtoSide(defPokeID) == mySide )
  {
    if( (BTL_EVENTVAR_GetValue(BTL_EVAR_DAMAGE_TYPE) == dmgType)
    &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_CRITICAL_FLAG) == FALSE)
    ){
      BtlRule rule = BTL_SVFTOOL_GetRule( flowWk );
      fx32 ratio = ((rule==BTL_RULE_SINGLE)||(rule==BTL_RULE_ROTATION))? FX32_CONST(0.5f) : FX32_CONST(0.66f);
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, ratio );
    }
  }
}


//--------------------------------------------------------------------------------------
/**
 *  ����҂̂܂���
 */
//--------------------------------------------------------------------------------------
static const BtlEventHandlerTable* ADD_SIDE_Sinpinomamori( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADDSICK_CHECKFAIL,  handler_side_SinpiNoMamori_CheckFail   },  // ��Ԉُ편�s�`�F�b�N
    { BTL_EVENT_ADDSICK_FAILED,     handler_side_SinpiNoMamori_FixFail     },  // ���s�m��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_side_SinpiNoMamori_CheckFail( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work )
{
  u8 pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );

  if( (BTL_MAINUTIL_PokeIDtoSide(pokeID) == mySide)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) != pokeID)  // �����Ŏ����ɑł��U�i�˂ނ铙�j�ɂ͌����Ȃ�
  ){
    WazaSick  sickID = BTL_EVENTVAR_GetValue( BTL_EVAR_SICKID );
    if( BTL_CALC_IsBasicSickID(sickID)
    ||  (sickID == WAZASICK_KONRAN)
    ||  (sickID == WAZASICK_AKUBI)
    ){
      work[0] = BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, TRUE );
    }
  }
}
static void handler_side_SinpiNoMamori_FixFail( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work )
{
  if( work[0] )
  {
    u8 pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_SinpiNoMamori_Exe );
      HANDEX_STR_AddArg( &param->str, pokeID );
    BTL_SVF_HANDEX_Pop( flowWk, param );
    work[0] = 0;
  }
}
//--------------------------------------------------------------------------------------
/**
 *  ���낢����
 */
//--------------------------------------------------------------------------------------
static const BtlEventHandlerTable* ADD_SIDE_SiroiKiri( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_RANKEFF_LAST_CHECK,  handler_side_SiroiKiri_CheckFail   },  // �����N�������s�`�F�b�N
    { BTL_EVENT_RANKEFF_FAILED,      handler_side_SiroiKiri_FixFail     },  // ���s�m��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_side_SiroiKiri_CheckFail( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work )
{
  u8 pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID );
  if( (BTL_MAINUTIL_PokeIDtoSide(pokeID) == mySide)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_VOLUME) < 0)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) != pokeID)
  ){
    work[0] = BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, TRUE );
  }
}
static void handler_side_SiroiKiri_FixFail( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work )
{
  if( work[0] )
  {
    u32 wazaSerial = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_SERIAL );
    if( (wazaSerial == 0)
    ||  (work[1] != wazaSerial)
    ){
      u8 pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID );
        BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
        HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_SiroiKiri_Exe );
        HANDEX_STR_AddArg( &param->str, pokeID );
      BTL_SVF_HANDEX_Pop( flowWk, param );

      work[1] = wazaSerial;
    }

    work[0] = 0;
  }
}
//--------------------------------------------------------------------------------------
/**
 *  ��������
 */
//--------------------------------------------------------------------------------------
static const BtlEventHandlerTable* ADD_SIDE_Oikaze( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY,  handler_side_Oikaze  },  // ���΂₳�v�Z�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// ���΂₳�v�Z�n���h��
static void handler_side_Oikaze( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work )
{
  u8 pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID );
  if( (BTL_MAINUTIL_PokeIDtoSide(pokeID) == mySide)
  ){
    BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(2) );
  }
}
//--------------------------------------------------------------------------------------
/**
 *  ���܂��Ȃ�
 */
//--------------------------------------------------------------------------------------
static const BtlEventHandlerTable* ADD_SIDE_Omajinai( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CRITICAL_CHECK,  handler_side_Omajinai  },  // �����N�������s�`�F�b�N
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_side_Omajinai( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work )
{
  u8 pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  if( (BTL_MAINUTIL_PokeIDtoSide(pokeID) == mySide)
  ){
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, TRUE );
  }
}
//--------------------------------------------------------------------------------------
/**
 *  �X�e���X���b�N
 */
//--------------------------------------------------------------------------------------
static const BtlEventHandlerTable* ADD_SIDE_StealthRock( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,  handler_side_StealthRock  },  // �����o�[����n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_side_StealthRock( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work )
{
  u8 pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID );
  if( (BTL_MAINUTIL_PokeIDtoSide(pokeID) == mySide)
  ){
    const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
    BtlTypeAff  affinity = BTL_CALC_TypeAffPair( POKETYPE_IWA, BPP_GetPokeType(bpp) );
    u8 denom = 8;
    switch( affinity ){
    case BTL_TYPEAFF_25:
    default:
      denom = 32;
      break;
    case BTL_TYPEAFF_50:    denom = 16; break;
    case BTL_TYPEAFF_100:   denom =  8; break;
    case BTL_TYPEAFF_200:   denom =  4; break;
    case BTL_TYPEAFF_400:   denom =  2; break;
    }

    {
      BTL_HANDEX_PARAM_DAMAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_DAMAGE, BTL_POKEID_NULL );

        param->pokeID = pokeID;
        param->damage = BTL_CALC_QuotMaxHP( bpp, denom );
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_StealthRockDamage );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
      BTL_SVF_HANDEX_Pop( flowWk, param );
    }
  }
}
//--------------------------------------------------------------------------------------
/**
 *  ���C�h�K�[�h
 */
//--------------------------------------------------------------------------------------
static const BtlEventHandlerTable* ADD_SIDE_WideGuard( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2,  handler_side_WideGuard  },  // ���U���������x���Q
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_side_WideGuard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work )
{
  // �h��|�P�������T�C�h��
  u8 pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  if( BTL_MAINUTIL_PokeIDtoSide(pokeID) == mySide)
  {
    // ���U���u�G�S�́v�u�����ȊO�S���v�̌��ʔ͈͂Ń_���[�W���U�Ȃ疳����
    WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    WazaTarget  targetType = WAZADATA_GetParam( waza, WAZAPARAM_TARGET );
    if( (WAZADATA_IsDamage(waza) )
    &&  ((targetType == WAZA_TARGET_ENEMY_ALL) || (targetType == WAZA_TARGET_OTHER_ALL))
    ){
      if( BTL_EVENTVAR_RewriteValue(BTL_EVAR_NOEFFECT_FLAG, TRUE) )
      {
        BTL_HANDEX_STR_PARAMS* strParam = (BTL_HANDEX_STR_PARAMS*)BTL_EVENTVAR_GetValue( BTL_EVAR_WORK_ADRS );
        HANDEX_STR_Setup( strParam, BTL_STRTYPE_SET, BTL_STRID_SET_WideGuard );
        HANDEX_STR_AddArg( strParam, pokeID );
      }
    }
  }
}
//--------------------------------------------------------------------------------------
/**
 *  �t�@�X�g�K�[�h
 */
//--------------------------------------------------------------------------------------
static const BtlEventHandlerTable* ADD_SIDE_FastGuard( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2,  handler_side_FastGuard  },  // ���U���������x���Q
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_side_FastGuard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work )
{
  // �h��|�P�������T�C�h��
  u8 pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  if( BTL_MAINUTIL_PokeIDtoSide(pokeID) == mySide)
  {
    // ���U�̎g�p�|�P���Ώۃ|�P�ł͂Ȃ��A���U�D��x���P�ȏ�̏ꍇ�A�K�[�h����
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) != pokeID )
    {
      WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
      if( WAZADATA_GetParam(waza, WAZAPARAM_PRIORITY) >= 1 )
      {
        if( BTL_EVENTVAR_RewriteValue(BTL_EVAR_NOEFFECT_FLAG, TRUE) )
        {
          BTL_HANDEX_STR_PARAMS* strParam = (BTL_HANDEX_STR_PARAMS*)BTL_EVENTVAR_GetValue( BTL_EVAR_WORK_ADRS );
          HANDEX_STR_Setup( strParam, BTL_STRTYPE_SET, BTL_STRID_SET_FastGuard );
          HANDEX_STR_AddArg( strParam, pokeID );
        }
      }
    }
  }
}
//--------------------------------------------------------------------------------------
/**
 *  �܂��т�
 */
//--------------------------------------------------------------------------------------
static const BtlEventHandlerTable* ADD_SIDE_Makibisi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,  handler_side_Makibisi  },  // ����`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_side_Makibisi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work )
{
  u8 pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID );
  if( BTL_MAINUTIL_PokeIDtoSide(pokeID) == mySide )
  {
    if( !BTL_SVFTOOL_IsFloatingPoke(flowWk, pokeID) )
    {
      u8 add_counter = getMyAddCounter( myHandle, mySide );
      u8 denom;
      switch( add_counter ) {
      case 1:
      default:
        denom = 8; break;
      case 2:
        denom = 6; break;
      case 3:
        denom = 4; break;
      }
      {
        BTL_HANDEX_PARAM_DAMAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_DAMAGE, BTL_POKEID_NULL );
          const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
          param->pokeID = pokeID;
          param->damage = BTL_CALC_QuotMaxHP( bpp, denom );
          HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_MakibisiDamage );
          HANDEX_STR_AddArg( &param->exStr, pokeID );
        BTL_SVF_HANDEX_Pop( flowWk, param );
      }
    }
  }
}
//--------------------------------------------------------------------------------------
/**
 *  �ǂ��т�
 */
//--------------------------------------------------------------------------------------
static const BtlEventHandlerTable* ADD_SIDE_Dokubisi( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,  handler_side_Dokubisi  },  // ����`�F�b�N�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_side_Dokubisi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work )
{
  u8 pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID );
  if( BTL_MAINUTIL_PokeIDtoSide(pokeID) == mySide )
  {
    if( !BTL_SVFTOOL_IsFloatingPoke( flowWk, pokeID ) )
    {
      const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );

      if( BPP_IsMatchType(bpp, POKETYPE_DOKU) )
      {
        BTL_HANDEX_PARAM_SIDEEFF_REMOVE* side_param;
        side_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_SIDEEFF_REMOVE, pokeID );
          side_param->side = mySide;
          BTL_CALC_BITFLG_Construction( side_param->flags, sizeof(side_param->flags) );
          BTL_CALC_BITFLG_Set( side_param->flags, BTL_SIDEEFF_DOKUBISI );
        BTL_SVF_HANDEX_Pop( flowWk, side_param );
      }
      else
      {
        BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, BTL_POKEID_NULL );

          param->sickID = WAZASICK_DOKU;
          if( getMyAddCounter(myHandle, mySide) > 1 ){
            param->sickCont = BPP_SICKCONT_MakeMoudokuCont();
          }else{
            param->sickCont = BPP_SICKCONT_MakePermanent();
          }

          param->pokeID = pokeID;
        BTL_SVF_HANDEX_Pop( flowWk, param );
      }
    }
  }
}

//--------------------------------------------------------------------------------------
/**
 *  ���̃��U���ʁF��
 */
//--------------------------------------------------------------------------------------
static const BtlEventHandlerTable* ADD_SIDE_Rainbow( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADD_SICK,         handler_Rainbow },  // �ǉ����ʁi��Ԉُ�j�`�F�b�N�n���h��
    { BTL_EVENT_ADD_RANK_TARGET,  handler_Rainbow },  // �ǉ����ʁi�����N���ʁj�`�F�b�N�n���h��
    { BTL_EVENT_WAZA_SHRINK_PER,  handler_Rainbow_Shrink },  // �Ђ�݃`�F�b�N�n���h��

  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
static void handler_Rainbow( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work )
{
  u8 atkPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );

  if( BTL_MAINUTIL_PokeIDtoSide(atkPokeID) == mySide )
  {
    u16 per = BTL_EVENTVAR_GetValue( BTL_EVAR_ADD_PER ) * 2;
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_ADD_PER, per );
  }
}
static void handler_Rainbow_Shrink( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work )
{
  u8 atkPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );

  if( BTL_MAINUTIL_PokeIDtoSide(atkPokeID) == mySide )
  {
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_CRITICAL_FLAG, TRUE );
  }
}


//--------------------------------------------------------------------------------------
/**
 *  ���̃��U���ʁF�΂̊C
 */
//--------------------------------------------------------------------------------------
static const BtlEventHandlerTable* ADD_SIDE_Burning( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_TURNCHECK_BEGIN,  handler_side_Burning  },  // �^�[���`�F�b�N�J�n�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// �^�[���`�F�b�N�J�n�n���h��
static void handler_side_Burning( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work )
{
  u8 pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID );

  if( (pokeID != BTL_POKEID_NULL)
  &&  (BTL_SVFTOOL_IsExistPokemon(flowWk, pokeID))
  &&  (!BTL_SVFTOOL_CheckShowDown(flowWk))
  ){
    if( BTL_MAINUTIL_PokeIDtoSide(pokeID) == mySide )
    {
      const BTL_POKEPARAM* bpp = BTL_SVFTOOL_GetPokeParam( flowWk, pokeID );
      if( !BPP_IsMatchType(bpp, POKETYPE_HONOO) )
      {
        BTL_HANDEX_PARAM_DAMAGE* param;

        // �G�t�F�N�g����������
        {
          BTL_HANDEX_PARAM_ADD_EFFECT*  viewEff_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_EFFECT, pokeID );
            viewEff_param->pos_from = BTL_SVFTOOL_PokeIDtoPokePos( flowWk, pokeID );
            viewEff_param->pos_to   = BTL_POS_NULL;
            viewEff_param->effectNo = BTLEFF_BURNING;
          BTL_SVF_HANDEX_Pop( flowWk, viewEff_param );
        }

        param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_DAMAGE, BTL_POKEID_NULL );
          param->pokeID = pokeID;
          param->damage = BTL_CALC_QuotMaxHP( bpp, 8 );
          HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_BurningDamage );
          HANDEX_STR_AddArg( &param->exStr, pokeID );
        BTL_SVF_HANDEX_Pop( flowWk, param );
      }
    }
  }
}

//--------------------------------------------------------------------------------------
/**
 *  ���̃��U���ʁF����
 */
//--------------------------------------------------------------------------------------
static const BtlEventHandlerTable* ADD_SIDE_Moor( u32* numElems )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CALC_AGILITY,  handler_side_Moor  },  // ���΂₳�v�Z�n���h��
  };
  *numElems = NELEMS( HandlerTable );
  return HandlerTable;
}
// ���΂₳�v�Z�n���h��
static void handler_side_Moor( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work )
{
  u8 pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID );
  if( (BTL_MAINUTIL_PokeIDtoSide(pokeID) == mySide)
  ){
    BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(0.25) );
  }
}


