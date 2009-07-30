
#include "btl_common.h"
#include "btl_pokeparam.h"
#include "btl_server_flow.h"
#include "btl_event.h"
#include "handler\hand_item.h"

#include "btl_sick.h"

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void contProc( BTL_SVFLOW_WORK* flowWk, BTL_POKEPARAM* bpp, u8 pokeID, WazaSick sick );
static void contDamageCommon( BTL_SVFLOW_WORK* flowWk, const BTL_POKEPARAM* bpp, u8 pokeID, WazaSick sick, u16 damage );
static int getWazaSickDamageStrID( WazaSick sick );
static void cont_HorobiNoUta( BTL_SVFLOW_WORK* flowWk, BTL_POKEPARAM* bpp, u8 pokeID );
static void cont_Yadorigi( BTL_SVFLOW_WORK* flowWk, BTL_POKEPARAM* bpp, u8 pokeID );
static void cont_NeWoHaru( BTL_SVFLOW_WORK* flowWk, BTL_POKEPARAM* bpp, u8 pokeID );
static void cont_Bind( BTL_SVFLOW_WORK* flowWk, BTL_POKEPARAM* bpp, u8 pokeID );
static void cureProc( BTL_SVFLOW_WORK* flowWk, BTL_POKEPARAM* bpp, u8 pokeID, WazaSick sick, BPP_SICK_CONT oldCont );
static void cure_Akubi( BTL_SVFLOW_WORK* flowWk, BTL_POKEPARAM* bpp );
static void cure_HorobiNoUta( BTL_SVFLOW_WORK* flowWk, BTL_POKEPARAM* bpp );
static void putHorobiCounter( BTL_SVFLOW_WORK* flowWk, const BTL_POKEPARAM* bpp, u8 count );
static void cure_Sasiosae( BTL_SVFLOW_WORK* flowWk, const BTL_POKEPARAM* bpp );
static void cure_Bind( BTL_SVFLOW_WORK* flowWk, const BTL_POKEPARAM* bpp, BPP_SICK_CONT oldCont );
static int getDefaultSickStrID( WazaSick sickID, BPP_SICK_CONT cont );
static int getCureStrID( WazaSick sick, BOOL fUseItem );



//=============================================================================================
/**
 * ��Ԉُ퉻������ꂽ���̏���
 *
 * @param   bpp
 * @param   sick
 */
//=============================================================================================
void BTL_SICK_AddProc( BTL_SVFLOW_WORK* flowWk, BTL_POKEPARAM* bpp, WazaSick sick )
{
  switch( sick ){
  case WAZASICK_SASIOSAE:
    BTL_HANDLER_ITEM_Remove( bpp );
    break;
  }
}



void BTL_SICK_TurnCheckCallback( BTL_POKEPARAM* bpp, WazaSick sick, BPP_SICK_CONT oldCont, BOOL fCure, void* work )
{
  u8 pokeID = BPP_GetID( bpp );

  if( fCure ){
    cureProc( work, bpp, pokeID, sick, oldCont );
  }else{
    contProc( work, bpp, pokeID, sick );
  }
}

//----------------------------------------------------------------------------------------------
// ��Ԉُ�p��
//----------------------------------------------------------------------------------------------
static void contProc( BTL_SVFLOW_WORK* flowWk, BTL_POKEPARAM* bpp, u8 pokeID, WazaSick sick )
{
  {
    u32 damage =  BPP_CalcSickDamage( bpp, sick );
    if( damage ){
      contDamageCommon( flowWk, bpp, pokeID, sick, damage );
    }
  }
  BTL_Printf("�|�P[%d], ��Ԉُ�[%d] ���p����...\n", pokeID, sick);
  switch( sick ){
  case WAZASICK_HOROBINOUTA:    cont_HorobiNoUta( flowWk, bpp, pokeID ); break;
  case WAZASICK_YADORIGI:       cont_Yadorigi( flowWk, bpp, pokeID ); break;
  case WAZASICK_NEWOHARU:       cont_NeWoHaru( flowWk, bpp, pokeID ); break;
  case WAZASICK_BIND:           cont_Bind( flowWk, bpp, pokeID ); break;
  }
}

/**
 *  �_���[�W���󂯂��Ԉُ�̋��ʏ���
 */
static void contDamageCommon( BTL_SVFLOW_WORK* flowWk, const BTL_POKEPARAM* bpp, u8 pokeID, WazaSick sick, u16 damage )
{
  BTL_HANDEX_PARAM_DAMAGE* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_DAMAGE, pokeID );
  int strID;

  param->poke_cnt = 1;
  param->pokeID[0] = pokeID;
  param->damage[0] = damage;

  strID = getWazaSickDamageStrID( sick );
  if( strID >= 0 ){
    HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, strID );
    HANDEX_STR_AddArg( &param->exStr, pokeID );
  }
}
/**
 *  ��Ԉُ�ɂ��_���[�W���̕W�����b�Z�[�WID�擾
 */
static int getWazaSickDamageStrID( WazaSick sick )
{
  switch( sick ){
  case WAZASICK_DOKU:   return BTL_STRID_SET_DokuDamage;
  case WAZASICK_YAKEDO: return BTL_STRID_SET_YakedoDamage;
  case WAZASICK_AKUMU:  return BTL_STRID_SET_AkumuDamage;
  case WAZASICK_NOROI:  return BTL_STRID_SET_NoroiDamage;
  }
  return -1;
}

/**
 *  �ق�т̂����F�p��
 */
static void cont_HorobiNoUta( BTL_SVFLOW_WORK* flowWk, BTL_POKEPARAM* bpp, u8 pokeID )
{
  BPP_SICK_CONT cont = BPP_GetSickCont( bpp, WAZASICK_HOROBINOUTA );
  u8 turnMax = BPP_SICCONT_GetTurnMax( cont );
  u8 turnNow = BPP_GetSickTurnCount( bpp, WAZASICK_HOROBINOUTA );
  int turnDiff = turnMax - turnNow;
  if( turnDiff > 0 ){
    putHorobiCounter( flowWk, bpp, turnDiff );
  }
}
/**
 *  ��ǂ肬�̂��ˁF�p��
 */
static void cont_Yadorigi( BTL_SVFLOW_WORK* flowWk, BTL_POKEPARAM* bpp, u8 pokeID )
{
  BTL_HANDEX_PARAM_DAMAGE* dmg_param;
  u16 damage = BTL_CALC_QuotMaxHP( bpp, 16 );

  dmg_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_DAMAGE, pokeID );
  dmg_param->poke_cnt = 1;
  dmg_param->pokeID[0] = pokeID;
  dmg_param->damage[0] = damage;
  HANDEX_STR_Setup( &dmg_param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_YadorigiTurn );
  HANDEX_STR_AddArg( &dmg_param->exStr, pokeID );

  {
    BPP_SICK_CONT  cont = BPP_GetSickCont( bpp, WAZASICK_YADORIGI );
    BtlPokePos  pos = BPP_SICKCONT_GetParam( cont );
    u8 userPokeID[BTL_POS_MAX];
    if( BTL_SERVERFLOW_RECEPT_GetTargetPokeID(flowWk, pos, userPokeID) )
    {
      BTL_HANDEX_PARAM_DRAIN* drain_param;
      drain_param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_DRAIN, pokeID );
      drain_param->header.failSkipFlag = TRUE;
      drain_param->recoverPokeID = userPokeID[0];
      drain_param->damagedPokeID = pokeID;
      drain_param->recoverHP = damage;
    }
  }

}
/**
 *  �˂��͂�F�p��
 */
static void cont_NeWoHaru( BTL_SVFLOW_WORK* flowWk, BTL_POKEPARAM* bpp, u8 pokeID )
{
  if( !BPP_IsHPFull(bpp) )
  {
    BTL_HANDEX_PARAM_RECOVER_HP* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_RECOVER_HP, pokeID );
    param->recoverHP = BTL_CALC_QuotMaxHP( bpp, 16 );
    param->pokeID = pokeID;
    HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_NeWoHaruRecover );
    HANDEX_STR_AddArg( &param->exStr, pokeID );
  }
}
/**
 *  �܂������F�p��
 */
static void cont_Bind( BTL_SVFLOW_WORK* flowWk, BTL_POKEPARAM* bpp, u8 pokeID )
{
  if( !BPP_IsDead(bpp) )
  {
    BTL_HANDEX_PARAM_DAMAGE* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_DAMAGE, pokeID );
    BPP_SICK_CONT cont = BPP_GetSickCont( bpp, WAZASICK_BIND );
    WazaID waza = BPP_SICKCONT_GetParam( cont );

    param->poke_cnt = 1;
    param->pokeID[0] = pokeID;
    param->damage[0] = BTL_CALC_QuotMaxHP( bpp, 16 );

    HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Bind );
    HANDEX_STR_AddArg( &param->exStr, pokeID );
    HANDEX_STR_AddArg( &param->exStr, waza );
  }
}



//----------------------------------------------------------------------------------------------
// ��Ԉُ��
//----------------------------------------------------------------------------------------------
static void cureProc( BTL_SVFLOW_WORK* flowWk, BTL_POKEPARAM* bpp, u8 pokeID, WazaSick sick, BPP_SICK_CONT oldCont )
{
  int strID = getCureStrID( sick, FALSE );
  if( strID >= 0 )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, strID );
    HANDEX_STR_AddArg( &param->str, pokeID );
    return;
  }

  switch( sick ){
  case WAZASICK_AKUBI:       cure_Akubi( flowWk, bpp ); break;
  case WAZASICK_HOROBINOUTA: cure_HorobiNoUta( flowWk, bpp ); break;
  case WAZASICK_SASIOSAE:    cure_Sasiosae( flowWk, bpp ); break;
  case WAZASICK_BIND:        cure_Bind( flowWk, bpp, oldCont ); break;
  }
}

static void cure_Akubi( BTL_SVFLOW_WORK* flowWk, BTL_POKEPARAM* bpp )
{
  int turns = BTL_CALC_RandRange( BTL_NEMURI_TURN_MIN, BTL_NEMURI_TURN_MAX );
  turns--;  // �^�[���`�F�b�N�������̂��߁A�P�^�[�������炩���ߒZ�����Ă���
  if( turns > 0 )
  {
    u8 pokeID = BPP_GetID( bpp );
    BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
    param->fAlmost = TRUE;
    param->sickID = WAZASICK_NEMURI;
    param->sickCont = BPP_SICKCONT_MakeTurn( turns );
    param->pokeID[0] = pokeID;
    param->poke_cnt = 1;
  }
}

static void cure_HorobiNoUta( BTL_SVFLOW_WORK* flowWk, BTL_POKEPARAM* bpp )
{
  putHorobiCounter( flowWk, bpp, 0 );
  {
    u8 pokeID = BPP_GetID( bpp );
    BTL_HANDEX_PARAM_KILL* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_KILL, pokeID );
    param->pokeID = pokeID;
  }
}

// �ق�т̂����J�E���g�\�����ʏ���
static void putHorobiCounter( BTL_SVFLOW_WORK* flowWk, const BTL_POKEPARAM* bpp, u8 count )
{
  u8 pokeID = BPP_GetID( bpp );
  BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
  HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_HorobiCountDown );
  HANDEX_STR_AddArg( &param->str, pokeID );
  HANDEX_STR_AddArg( &param->str, count );

}

static void cure_Sasiosae( BTL_SVFLOW_WORK* flowWk, const BTL_POKEPARAM* bpp )
{
  BTL_HANDLER_ITEM_Add( bpp );
}
static void cure_Bind( BTL_SVFLOW_WORK* flowWk, const BTL_POKEPARAM* bpp, BPP_SICK_CONT oldCont )
{
  u8 pokeID = BPP_GetID( bpp );
  WazaID waza = BPP_SICKCONT_GetParam( oldCont );
  BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVFLOW_HANDLERWORK_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
  HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_BindCure );
  HANDEX_STR_AddArg( &param->str, pokeID );
  HANDEX_STR_AddArg( &param->str, waza );
}



//=============================================================================================
/**
 * ��Ԉُ�ɂ����������̃f�t�H���g���b�Z�[�W�h�c��Ԃ�
 *
 * @param   sickID
 * @param   cont
 *
 * @retval  int   ���b�Z�[�WID�i���݂��Ȃ��ꍇ-1�j
 */
//=============================================================================================
static int getDefaultSickStrID( WazaSick sickID, BPP_SICK_CONT cont )
{
  int strID = -1;


  switch( sickID ){
  case WAZASICK_YAKEDO:     strID = BTL_STRID_SET_YakedoGet; break;
  case WAZASICK_MAHI:       strID = BTL_STRID_SET_MahiGet; break;
  case WAZASICK_KOORI:      strID = BTL_STRID_SET_KoriGet; break;
  case WAZASICK_NEMURI:     strID = BTL_STRID_SET_NemuriGet; break;
  case WAZASICK_KONRAN:     strID = BTL_STRID_SET_KonranGet; break;
  case WAZASICK_MEROMERO:   strID = BTL_STRID_SET_MeromeroGet; break;
  case WAZASICK_AKUMU:      strID = BTL_STRID_SET_AkumuGet;  break;
  case WAZASICK_NOROI:      strID = BTL_STRID_SET_Noroi; break;
  case WAZASICK_TYOUHATSU:  strID = BTL_STRID_SET_Chouhatu; break;
  case WAZASICK_ICHAMON:    strID = BTL_STRID_SET_Ichamon; break;
  case WAZASICK_AKUBI:      strID = BTL_STRID_SET_Akubi; break;
  case WAZASICK_YADORIGI:   strID = BTL_STRID_SET_Yadorigi; break;
  case WAZASICK_MIYABURU:   strID = BTL_STRID_SET_Miyaburu; break;
  case WAZASICK_NEWOHARU:   strID = BTL_STRID_SET_NeWoHaru; break;
  case WAZASICK_SASIOSAE:   strID = BTL_STRID_SET_Sasiosae; break;
  case WAZASICK_POWERTRICK: strID = BTL_STRID_SET_PowerTrick; break;
  case WAZASICK_BIND:       strID = BTL_STRID_SET_Bind; break;
  case WAZASICK_DOKU:
    strID = BPP_SICKCONT_IsMoudokuCont(cont)? BTL_STRID_SET_MoudokuGet : BTL_STRID_SET_DokuGet;
    break;

  default:
    break;
  }

  return strID;
}

//=============================================================================================
/**
 * ��Ԉُ�ɂ����������̃f�t�H���g������𐶐�
 *
 * @param   sickID
 * @param   cont
 * @param   bpp
 * @param   str
 */
//=============================================================================================
void BTL_SICK_MakeDefaultMsg( WazaSick sickID, BPP_SICK_CONT cont, const BTL_POKEPARAM* bpp, BTL_HANDEX_STR_PARAMS* str )
{
  int strID = getDefaultSickStrID( sickID, cont );
  if( strID >= 0 )
  {
    HANDEX_STR_Setup( str, BTL_STRTYPE_SET, strID );
    HANDEX_STR_AddArg( str, BPP_GetID(bpp) );

    switch( sickID ){
    case WAZASICK_BIND:
      {
        WazaID  waza = BPP_SICKCONT_GetParam( cont );
        HANDEX_STR_AddArg( str, waza );
      }
      break;
    }
  }
}


int BTL_SICK_GetDefaultSickCureStrID( WazaSick sickID, BOOL fUseItem )
{
  return getCureStrID( sickID, fUseItem );
}

static int getCureStrID( WazaSick sick, BOOL fUseItem )
{
  static const struct {
    WazaSick  sick;
    s16       strID_notItem;
    s16       strID_useItem;
  }dispatchTbl[] = {
    { WAZASICK_DOKU,      BTL_STRID_SET_DokuCure,         BTL_STRID_SET_UseItem_CureDoku    },
    { POKESICK_YAKEDO,    BTL_STRID_SET_YakedoCure   ,    BTL_STRID_SET_UseItem_CureYakedo  },
    { WAZASICK_NEMURI,    BTL_STRID_SET_NemuriWake,       BTL_STRID_SET_UseItem_CureNemuri  },
    { WAZASICK_KOORI,     BTL_STRID_SET_KoriMelt,         BTL_STRID_SET_UseItem_CureKoori   },
    { WAZASICK_MAHI,      BTL_STRID_SET_MahiCure,         BTL_STRID_SET_UseItem_CureMahi    },
    { WAZASICK_ENCORE,    BTL_STRID_SET_EncoreCure,       -1                                },
    { WAZASICK_KANASIBARI,BTL_STRID_SET_KanasibariCure,   -1                                },
    { WAZASICK_SASIOSAE,  BTL_STRID_SET_SasiosaeCure,     -1                                },
    { WAZASICK_BIND,      BTL_STRID_SET_BindCure,         -1                                },
    { WAZASICK_YADORIGI,  BTL_STRID_SET_BindCure,         -1                                },

    { WAZASICK_KONRAN,    -1,                             BTL_STRID_SET_UseItem_CureKonran  },
    { WAZASICK_MEROMERO,  -1,                             BTL_STRID_SET_UseItem_CureMero    },
  };

  u32 i;

  for(i=0; i<NELEMS(dispatchTbl); ++i){
    if( dispatchTbl[i].sick == sick ){
      return (fUseItem)? dispatchTbl[i].strID_useItem : dispatchTbl[i].strID_notItem;
    }
  }

  return -1;

}

//=============================================================================================
/**
 * �񕜎��f�t�H���g������𐶐�
 *
 * @param   sickID
 * @param   cont
 * @param   bpp
 * @param   fUseItem
 * @param   str
 */
//=============================================================================================
BOOL BTL_SICK_MakeDefaultCureMsg( WazaSick sickID, BPP_SICK_CONT oldCont, const BTL_POKEPARAM* bpp, u16 itemID, BTL_HANDEX_STR_PARAMS* str )
{
  BOOL fUseItem = (itemID != ITEM_DUMMY_DATA );
  int strID = getCureStrID( sickID, fUseItem );
  if( strID ){
    BTL_HANDEX_STR_PARAMS  str;

    HANDEX_STR_Setup( &str, BTL_STRTYPE_SET, strID );
    HANDEX_STR_AddArg( &str, BPP_GetID( bpp ) );
    if( fUseItem ){
      HANDEX_STR_AddArg( &str, itemID );
    }

    switch( sickID ){
    case WAZASICK_BIND:
      {
        WazaID  waza = BPP_SICKCONT_GetParam( oldCont );
        HANDEX_STR_AddArg( &str, waza );
      }
      break;
    }
    return TRUE;
  }
  return FALSE;
}



//=============================================================================================
/**
 *
 *
 * @param   flowWk
 * @param   defender
 */
//=============================================================================================
void BTL_SICK_CheckNotEffectByType( BTL_SVFLOW_WORK* flowWk, const BTL_POKEPARAM* defender )
{
  if( BPP_CheckSick(defender, WAZASICK_MIYABURU) )
  {
    BPP_SICK_CONT cont = BPP_GetSickCont( defender, WAZASICK_MIYABURU );
    PokeType type = BPP_SICKCONT_GetParam( cont );
    if( BPP_IsMatchType(defender, type) ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_NOEFFECT_FLAG, FALSE );
    }
  }
}

void BTL_SICK_CheckDamageAffinity(  BTL_SVFLOW_WORK* flowWk, const BTL_POKEPARAM* defender )
{
  if( BPP_CheckSick(defender, WAZASICK_MIYABURU) )
  {
    BPP_SICK_CONT cont = BPP_GetSickCont( defender, WAZASICK_MIYABURU );
    PokeType type = BPP_SICKCONT_GetParam( cont );
    if( BPP_IsMatchType(defender, type) ){
      BtlTypeAff aff = BTL_EVENTVAR_GetValue( BTL_EVAR_TYPEAFF );
      if( aff == BTL_TYPEAFF_0 ){
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_TYPEAFF, BTL_TYPEAFF_100 );
      }
    }
  }
}







//=============================================================================================
/**
 * �o�g���^�b�`�Ŏ󂯌p����Ԉُ�R�[�h���ǂ�������
 *
 * @param   sick
 *
 * @retval  BOOL    �󂯌p���Ȃ�TRUE
 */
//=============================================================================================
BOOL BTL_SICK_CheckBatonTouch( WazaSick sick )
{
  static const u16 sickTbl[] = {
    WAZASICK_KONRAN,
    WAZASICK_KAIHUKUHUUJI,
    WAZASICK_IEKI,
    WAZASICK_YADORIGI,
    WAZASICK_SASIOSAE,
    WAZASICK_HOROBINOUTA,
    WAZASICK_NEWOHARU,
    WAZASICK_TOOSENBOU,
    WAZASICK_MUSTHIT_TARGET,
    WAZASICK_FLYING,
    WAZASICK_POWERTRICK,
    WAZASICK_TOOSENBOU,
    WAZASICK_NOROI,
  };
  u32 i;
  for(i=0; i<NELEMS(sickTbl); ++i){
    if( sickTbl[i] == sick ){
      return TRUE;
    }
  }
  return FALSE;
}
