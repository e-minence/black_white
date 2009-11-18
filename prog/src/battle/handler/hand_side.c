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
  u16            turn_counter;    ///< �^�[�����J�E���^
  u8             add_counter;     ///< �d�˂����J�E���^
  u8             enableFlag;      ///< �ғ����t���O

}EXIST_EFFECT;

static EXIST_EFFECT ExistEffect[ BTL_SIDE_MAX ][ BTL_SIDEEFF_MAX ];


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static u8 getMyAddCounter( BTL_EVENT_FACTOR* myHandle, BtlSide side );
static BTL_EVENT_FACTOR* ADD_SIDE_Refrector( u16 pri, BtlSide side, BtlSideEffect eff );
static void handler_side_Refrector( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 side, int* work );
static BTL_EVENT_FACTOR* ADD_SIDE_Hikarinokabe( u16 pri, BtlSide side, BtlSideEffect eff );
static void handler_side_HikariNoKabe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work );
static BTL_EVENT_FACTOR* ADD_SIDE_Sinpinomamori( u16 pri, BtlSide side, BtlSideEffect eff );
static void handler_side_SinpiNoMamori_CheckFail( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work );
static void handler_side_SinpiNoMamori_FixFail( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work );
static BTL_EVENT_FACTOR* ADD_SIDE_SiroiKiri( u16 pri, BtlSide side, BtlSideEffect eff );
static void handler_side_SiroiKiri_CheckFail( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work );
static void handler_side_SiroiKiri_FixFail( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work );
static BTL_EVENT_FACTOR* ADD_SIDE_Oikaze( u16 pri, BtlSide side, BtlSideEffect eff );
static void handler_side_Oikaze( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work );
static BTL_EVENT_FACTOR* ADD_SIDE_Omajinai( u16 pri, BtlSide side, BtlSideEffect eff );
static void handler_side_Omajinai( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work );
static BTL_EVENT_FACTOR* ADD_SIDE_StealthRock( u16 pri, BtlSide side, BtlSideEffect eff );
static void handler_side_StealthRock( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work );
static BTL_EVENT_FACTOR* ADD_SIDE_WideGuard( u16 pri, BtlSide side, BtlSideEffect eff );
static void handler_side_WideGuard( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work );
static BTL_EVENT_FACTOR* ADD_SIDE_Makibisi( u16 pri, BtlSide side, BtlSideEffect eff );
static void handler_side_Makibisi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work );
static BTL_EVENT_FACTOR* ADD_SIDE_Dokubisi( u16 pri, BtlSide side, BtlSideEffect eff );
static void handler_side_Dokubisi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work );




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
  typedef BTL_EVENT_FACTOR* (*pEventAddFunc)( u16 pri, BtlSide side, BtlSideEffect eff );

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
  };

  GF_ASSERT(side < BTL_SIDE_MAX);
  GF_ASSERT(sideEffect < BTL_SIDEEFF_MAX);

  {
    u32 i;
    for(i=0; i<NELEMS(funcTbl); ++i)
    {
      if( funcTbl[i].eff == sideEffect )
      {
        if( ExistEffect[ side ][ sideEffect ].add_counter == 0 )
        {
          BTL_EVENT_FACTOR* factor = funcTbl[i].func( 0, side, sideEffect );
          BTL_EVENT_FACTOR_SetWorkValue( factor, WORKIDX_CONT, contParam.raw );

          ExistEffect[ side ][ sideEffect ].add_counter = 1;
          ExistEffect[ side ][ sideEffect ].turn_counter = 0;
          ExistEffect[ side ][ sideEffect ].contParam = contParam;
          ExistEffect[ side ][ sideEffect ].factor = factor;
          return factor;
        }
        else if( ExistEffect[ side ][ sideEffect ].add_counter < funcTbl[i].add_max )
        {
          ExistEffect[ side ][ sideEffect ].add_counter++;
          return ExistEffect[ side ][ sideEffect ].factor;
        }
      }
    }
  }
  return NULL;
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
static BTL_EVENT_FACTOR* ADD_SIDE_Refrector( u16 pri, BtlSide side, BtlSideEffect eff )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,  handler_side_Refrector   },  // �_���[�W�␳
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, eff, pri, side, HandlerTable );
}
static void handler_side_Refrector( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 side, int* work )
{
  u8 defPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  if( BTL_MAINUTIL_PokeIDtoSide(defPokeID) == side )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_DAMAGE_TYPE) == WAZADATA_DMG_PHYSIC )
    {
      BTL_Printf("���t���N�^�[�ł͂�Ԃ��\n");
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(0.5f) );
    }
  }
}
//--------------------------------------------------------------------------------------
/**
 *  �Ђ���̂���
 */
//--------------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* ADD_SIDE_Hikarinokabe( u16 pri, BtlSide side, BtlSideEffect eff )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_WAZA_DMG_PROC2,  handler_side_HikariNoKabe   },  // �_���[�W�␳
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, eff, pri, side, HandlerTable );
}
static void handler_side_HikariNoKabe( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work )
{
  u8 defPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  if( BTL_MAINUTIL_PokeIDtoSide(defPokeID) == mySide )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_DAMAGE_TYPE) == WAZADATA_DMG_SPECIAL )
    {
      BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(0.5f) );
    }
  }
}
//--------------------------------------------------------------------------------------
/**
 *  ����҂̂܂���
 */
//--------------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* ADD_SIDE_Sinpinomamori( u16 pri, BtlSide side, BtlSideEffect eff )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_ADDSICK_CHECKFAIL,  handler_side_SinpiNoMamori_CheckFail   },  // ��Ԉُ편�s�`�F�b�N
    { BTL_EVENT_ADDSICK_FAILED,     handler_side_SinpiNoMamori_FixFail     },  // ���s�m��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, eff, pri, side, HandlerTable );
}
static void handler_side_SinpiNoMamori_CheckFail( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work )
{
  u8 pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  if( (BTL_MAINUTIL_PokeIDtoSide(pokeID) == mySide)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_SICKID) < POKESICK_MAX)
  ){
    work[0] = BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, TRUE );
  }
}
static void handler_side_SinpiNoMamori_FixFail( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work )
{
  if( work[0] )
  {
    u8 pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_SinpiNoMamori_Exe );
    HANDEX_STR_AddArg( &param->str, pokeID );
    work[0] = 0;
  }
}
//--------------------------------------------------------------------------------------
/**
 *  ���낢����
 */
//--------------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* ADD_SIDE_SiroiKiri( u16 pri, BtlSide side, BtlSideEffect eff )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_RANKEFF_LAST_CHECK,  handler_side_SiroiKiri_CheckFail   },  // �����N�������s�`�F�b�N
    { BTL_EVENT_RANKEFF_FAILED,      handler_side_SiroiKiri_FixFail     },   // ���s�m��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, eff, pri, side, HandlerTable );
}
static void handler_side_SiroiKiri_CheckFail( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work )
{
  u8 pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID );
  if( (BTL_MAINUTIL_PokeIDtoSide(pokeID) == mySide)
  &&  (BTL_EVENTVAR_GetValue(BTL_EVAR_VOLUME) < 0)
  ){
    work[0] = BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, TRUE );
    if( work[0] ){
      BTL_Printf("���낢����Ŗh����\n");
    }
  }
}
static void handler_side_SiroiKiri_FixFail( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work )
{
  if( work[0] )
  {
    u8 pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID );
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_SiroiKiri_Exe );
    HANDEX_STR_AddArg( &param->str, pokeID );
  }
}
//--------------------------------------------------------------------------------------
/**
 *  ��������
 */
//--------------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* ADD_SIDE_Oikaze( u16 pri, BtlSide side, BtlSideEffect eff )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_RANKEFF_LAST_CHECK,  handler_side_Oikaze  },  // �����N�������s�`�F�b�N
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, eff, pri, side, HandlerTable );
}
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
static BTL_EVENT_FACTOR* ADD_SIDE_Omajinai( u16 pri, BtlSide side, BtlSideEffect eff )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_CRITICAL_CHECK,  handler_side_Omajinai  },  // �����N�������s�`�F�b�N
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, eff, pri, side, HandlerTable );
}
static void handler_side_Omajinai( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work )
{
  u8 pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID );
  if( (BTL_MAINUTIL_PokeIDtoSide(pokeID) == mySide)
  ){
    BTL_EVENTVAR_MulValue( BTL_EVAR_FAIL_FLAG, TRUE );
  }
}
//--------------------------------------------------------------------------------------
/**
 *  �X�e���X���b�N
 */
//--------------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* ADD_SIDE_StealthRock( u16 pri, BtlSide side, BtlSideEffect eff )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,  handler_side_StealthRock  },  // �����o�[����n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, eff, pri, side, HandlerTable );
}
static void handler_side_StealthRock( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work )
{
  u8 pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID );
  if( (BTL_MAINUTIL_PokeIDtoSide(pokeID) == mySide)
  ){
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
    BtlTypeAff  affinity = BTL_CALC_TypeAff( POKETYPE_IWA, BPP_GetPokeType(bpp) );
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
      BTL_HANDEX_PARAM_DAMAGE* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_DAMAGE, BTL_POKEID_NULL );

      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
      param->damage[0] = BTL_CALC_QuotMaxHP( bpp, denom );
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_StealthRockDamage );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
    }
  }
}
//--------------------------------------------------------------------------------------
/**
 *  ���C�h�K�[�h
 */
//--------------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* ADD_SIDE_WideGuard( u16 pri, BtlSide side, BtlSideEffect eff )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_NOEFFECT_CHECK_L2,  handler_side_WideGuard  },  // ���U���������x���Q
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, eff, pri, side, HandlerTable );
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
 *  �܂��т�
 */
//--------------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* ADD_SIDE_Makibisi( u16 pri, BtlSide side, BtlSideEffect eff )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,  handler_side_Makibisi  },  // ����`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, eff, pri, side, HandlerTable );
}
static void handler_side_Makibisi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work )
{
  u8 pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID );
  if( (BTL_MAINUTIL_PokeIDtoSide(pokeID) == mySide)
  ){
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );

    if( !BPP_TURNFLAG_Get(bpp, BPP_TURNFLG_FLYING) )
    {
      u8 add_counter = getMyAddCounter( myHandle, mySide );
      u8 denom;
      switch( add_counter) {
      case 1:
      default:
        denom = 8; break;
      case 2:
        denom = 6; break;
      case 3:
        denom = 4; break;
      }
      {
        BTL_HANDEX_PARAM_DAMAGE* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_DAMAGE, BTL_POKEID_NULL );

        param->poke_cnt = 1;
        param->pokeID[0] = pokeID;
        param->damage[0] = BTL_CALC_QuotMaxHP( bpp, denom );
        HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_MakibisiDamage );
        HANDEX_STR_AddArg( &param->exStr, pokeID );
      }
    }
  }
}
//--------------------------------------------------------------------------------------
/**
 *  �ǂ��т�
 */
//--------------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* ADD_SIDE_Dokubisi( u16 pri, BtlSide side, BtlSideEffect eff )
{
  static const BtlEventHandlerTable HandlerTable[] = {
    { BTL_EVENT_MEMBER_IN,  handler_side_Dokubisi  },  // ����`�F�b�N�n���h��
    { BTL_EVENT_NULL, NULL },
  };
  return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_WAZA, eff, pri, side, HandlerTable );
}
static void handler_side_Dokubisi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 mySide, int* work )
{
  u8 pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID );
  if( (BTL_MAINUTIL_PokeIDtoSide(pokeID) == mySide)
  ){
    const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );

    if( !BPP_TURNFLAG_Get(bpp, BPP_TURNFLG_FLYING) )
    {
      BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_ADD_SICK, BTL_POKEID_NULL );

      param->sickID = WAZASICK_DOKU;
      if( getMyAddCounter(myHandle, mySide) > 1 ){
        param->sickCont = BPP_SICKCONT_MakePermanentInc( BTL_MOUDOKU_COUNT_MAX );
      }else{
        param->sickCont = BPP_SICKCONT_MakePermanent();
      }

      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
    }
  }
}

