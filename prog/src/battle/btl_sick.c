
#include "btl_common.h"
#include "btl_pokeparam.h"
#include "btl_server_flow.h"
#include "btl_event.h"
#include "btlv\btlv_effect.h"
#include "handler\hand_item.h"

#include "btl_sick.h"


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void turncheck_contProc( BTL_SVFLOW_WORK* flowWk, BTL_POKEPARAM* bpp, u8 pokeID, WazaSick sick );
static int getWazaSickDamageStrID( WazaSick sick );
static void cont_HorobiNoUta( BTL_SVFLOW_WORK* flowWk, BTL_POKEPARAM* bpp, u8 pokeID );
static void cont_Yadorigi( BTL_SVFLOW_WORK* flowWk, BTL_POKEPARAM* bpp, u8 pokeID );
static void cont_NeWoHaru( BTL_SVFLOW_WORK* flowWk, BTL_POKEPARAM* bpp, u8 pokeID );
static void cont_Bind( BTL_SVFLOW_WORK* flowWk, BTL_POKEPARAM* bpp, u8 pokeID );
static void cont_AquaRing( BTL_SVFLOW_WORK* flowWk, BTL_POKEPARAM* bpp, u8 pokeID );
static void cont_Encore( BTL_SVFLOW_WORK* flowWk, BTL_POKEPARAM* bpp, u8 pokeID );
static void turncheck_cureProc( BTL_SVFLOW_WORK* flowWk, BTL_POKEPARAM* bpp, u8 pokeID, WazaSick sick, BPP_SICK_CONT oldCont );
static void cure_Akubi( BTL_SVFLOW_WORK* flowWk, BTL_POKEPARAM* bpp );
static void cure_HorobiNoUta( BTL_SVFLOW_WORK* flowWk, BTL_POKEPARAM* bpp );
static void putHorobiCounter( BTL_SVFLOW_WORK* flowWk, const BTL_POKEPARAM* bpp, u8 count );
static void cure_Sasiosae( BTL_SVFLOW_WORK* flowWk, const BTL_POKEPARAM* bpp );
static void cure_KaifukuFuji( BTL_SVFLOW_WORK* flowWk, const BTL_POKEPARAM* bpp );
static void cure_Bind( BTL_SVFLOW_WORK* flowWk, const BTL_POKEPARAM* bpp, BPP_SICK_CONT oldCont );
static int getDefaultSickStrID( WazaSick sickID, BPP_SICK_CONT cont );
static int getCureStrID( WazaSick sick, BOOL fUseItem );






void BTL_SICK_TurnCheckCallback( BTL_POKEPARAM* bpp, WazaSick sick, BPP_SICK_CONT oldCont, BOOL fCure, void* work )
{
  u8 pokeID = BPP_GetID( bpp );

  if( fCure ){
    turncheck_cureProc( work, bpp, pokeID, sick, oldCont );
  }else{
    turncheck_contProc( work, bpp, pokeID, sick );
  }
}

//----------------------------------------------------------------------------------------------
// 状態異常継続
//----------------------------------------------------------------------------------------------
static void turncheck_contProc( BTL_SVFLOW_WORK* flowWk, BTL_POKEPARAM* bpp, u8 pokeID, WazaSick sick )
{
  if( !BPP_IsDead(bpp) )
  {
    u32 damage =  BPP_CalcSickDamage( bpp, sick );
    if( damage ){
      BTL_SVF_SickDamageRecall( flowWk, bpp, sick, damage );
    }
    BTL_N_Printf( DBGSTR_SICK_ContProc, pokeID, sick);

    switch( sick ){
    case WAZASICK_HOROBINOUTA:    cont_HorobiNoUta( flowWk, bpp, pokeID ); break;
    case WAZASICK_YADORIGI:       cont_Yadorigi( flowWk, bpp, pokeID ); break;
    case WAZASICK_NEWOHARU:       cont_NeWoHaru( flowWk, bpp, pokeID ); break;
    case WAZASICK_BIND:           cont_Bind( flowWk, bpp, pokeID ); break;
    case WAZASICK_AQUARING:       cont_AquaRing( flowWk, bpp, pokeID ); break;
    case WAZASICK_ENCORE:         cont_Encore( flowWk, bpp, pokeID ); break;
    }
  }
}
//=============================================================================================
/**
 * 状態異常継続ダメージを受ける際のデフォルトメッセージパラメータを生成
 *
 * @param   strParam
 * @param   bpp
 * @param   sickID
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTL_SICK_MakeSickDamageMsg( BTL_HANDEX_STR_PARAMS* strParam, const BTL_POKEPARAM* bpp, WazaSick sickID )
{
  int strID = getWazaSickDamageStrID( sickID );
  if( strID > 0 )
  {
    HANDEX_STR_Setup( strParam, BTL_STRTYPE_SET, strID );
    HANDEX_STR_AddArg( strParam, BPP_GetID(bpp) );
  }
  return FALSE;
}
/**
 *  状態異常によるダメージ時の標準メッセージID取得
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
 *  ほろびのうた：継続
 */
static void cont_HorobiNoUta( BTL_SVFLOW_WORK* flowWk, BTL_POKEPARAM* bpp, u8 pokeID )
{
  if( !BPP_IsDead(bpp) )
  {
    BPP_SICK_CONT cont = BPP_GetSickCont( bpp, WAZASICK_HOROBINOUTA );
    u8 turnMax = BPP_SICCONT_GetTurnMax( cont );
    u8 turnNow = BPP_GetSickTurnCount( bpp, WAZASICK_HOROBINOUTA );
    int turnDiff = turnMax - turnNow;
    if( turnDiff > 0 ){
      putHorobiCounter( flowWk, bpp, turnDiff );
    }
  }
}
/**
 *  やどりぎのたね：継続
 */
static void cont_Yadorigi( BTL_SVFLOW_WORK* flowWk, BTL_POKEPARAM* bpp, u8 pokeID )
{
  BPP_SICK_CONT  cont = BPP_GetSickCont( bpp, WAZASICK_YADORIGI );
  BtlPokePos  pos_to = BPP_SICKCONT_GetParam( cont );
  u8 recoverPokeID = BTL_SVFTOOL_GetExistPokeID( flowWk, pos_to );

  if( recoverPokeID != BTL_POKEID_NULL )
  {
    BTL_HANDEX_PARAM_DAMAGE* dmg_param;
    u16 damage = BTL_CALC_QuotMaxHP( bpp, 8 );
    u16 hp = BPP_GetValue( bpp, BPP_HP );
//    u16 que_reserve_pos = BTL_SVFTOOL_ReserveQuePos( flowWk, SC_ACT_EFFECT_BYVECTOR );

    if( damage > hp ){
      damage = hp;
    }

    dmg_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_DAMAGE, pokeID );
      dmg_param->pokeID = pokeID;
      dmg_param->damage = damage;
      HANDEX_STR_Setup( &dmg_param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_YadorigiTurn );
      HANDEX_STR_AddArg( &dmg_param->exStr, pokeID );
      // ダメージが成功したらエフェクト
      {
        // エフェクト側の from, to の解釈が逆っぽいのでこうする。
        dmg_param->fExEffect = TRUE;
        dmg_param->effectNo = BTLEFF_YADORIGI;
        dmg_param->pos_from = pos_to;
        dmg_param->pos_to = BTL_SVFTOOL_PokeIDtoPokePos( flowWk, pokeID );
      }
    BTL_SVF_HANDEX_Pop( flowWk, dmg_param );

    // 続けて回復
    if( recoverPokeID != BTL_POKEID_NULL )
    {
      BTL_HANDEX_PARAM_DRAIN* drain_param;
      drain_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_DRAIN, pokeID );
        drain_param->header.failSkipFlag = TRUE;
        drain_param->recoverPokeID = recoverPokeID;
        drain_param->damagedPokeID = pokeID;
        drain_param->recoverHP = damage;
      BTL_SVF_HANDEX_Pop( flowWk, drain_param );
    }
  }
}
/**
 *  ねをはる：継続
 */
static void cont_NeWoHaru( BTL_SVFLOW_WORK* flowWk, BTL_POKEPARAM* bpp, u8 pokeID )
{
  if( !BPP_IsHPFull(bpp) )
  {
    // 「おおきなねっこ」対象にするため、ドレイン回復を使う
    BTL_HANDEX_PARAM_DRAIN* drain_param;
    drain_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_DRAIN, pokeID );
      drain_param->recoverPokeID = pokeID;
      drain_param->damagedPokeID = BTL_POKEID_NULL;
      drain_param->recoverHP = BTL_CALC_QuotMaxHP( bpp, 16 );
      HANDEX_STR_Setup( &drain_param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_NeWoHaruRecover );
      HANDEX_STR_AddArg( &drain_param->exStr, pokeID );
    BTL_SVF_HANDEX_Pop( flowWk, drain_param );
  }
}
/**
 *  まきつく他：継続
 */
static void cont_Bind( BTL_SVFLOW_WORK* flowWk, BTL_POKEPARAM* bpp, u8 pokeID )
{
  if( !BPP_IsDead(bpp) )
  {
    BTL_HANDEX_PARAM_DAMAGE* param ;
    BPP_SICK_CONT cont = BPP_GetSickCont( bpp, WAZASICK_BIND );
    WazaID waza = BPP_SICKCONT_GetParam( cont );
    int effNo = -1;

    switch( waza ){
    case WAZANO_MAKITUKU:      effNo = BTLEFF_MAKITUKU; break;
    case WAZANO_SIMETUKERU:    effNo = BTLEFF_SIMETUKERU; break;
    case WAZANO_HONOONOUZU:    effNo = BTLEFF_HONOONOUZU; break;
    case WAZANO_MAGUMASUTOOMU: effNo = BTLEFF_MAGUMASUTOOMU; break;
    case WAZANO_KARADEHASAMU:  effNo = BTLEFF_KARADEHASAMU; break;
    case WAZANO_UZUSIO:        effNo = BTLEFF_UZUSIO; break;
    case WAZANO_SUNAZIGOKU:    effNo = BTLEFF_SUNAZIGOKU; break;
    }

    param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_DAMAGE, pokeID );

      param->pokeID = pokeID;
      if( BPP_SICKCONT_GetFlag(cont) ){ // フラグONなら２倍（しめつけバンド対応）
        param->damage = BTL_CALC_QuotMaxHP( bpp, 8 );
      }else{
        param->damage = BTL_CALC_QuotMaxHP( bpp, 16 );
      }

      if( effNo != -1 )
      {
        param->fExEffect = TRUE;
        param->effectNo = effNo;
        param->pos_from = BTL_SVFTOOL_PokeIDtoPokePos( flowWk, pokeID );
        param->pos_to = BTL_POS_NULL;
      }

      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Bind );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
      HANDEX_STR_AddArg( &param->exStr, waza );

    BTL_SVF_HANDEX_Pop( flowWk, param );
  }
}
/**
 *  アクアリング：継続
 */
static void cont_AquaRing( BTL_SVFLOW_WORK* flowWk, BTL_POKEPARAM* bpp, u8 pokeID )
{
  if( !BPP_IsHPFull(bpp)
  &&  !BPP_IsDead(bpp)
  ){
    // 「おおきなねっこ」対象にするため、ドレイン回復を使う
    BTL_HANDEX_PARAM_DRAIN* drain_param;
    drain_param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_DRAIN, pokeID );
      drain_param->recoverPokeID = pokeID;
      drain_param->damagedPokeID = BTL_POKEID_NULL;
      drain_param->recoverHP = BTL_CALC_QuotMaxHP( bpp, 16 );
      HANDEX_STR_Setup( &drain_param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_AquaRingRecover );
      HANDEX_STR_AddArg( &drain_param->exStr, pokeID );
    BTL_SVF_HANDEX_Pop( flowWk, drain_param );
  }
}
/**
 *  アンコール：継続
 */
static void cont_Encore( BTL_SVFLOW_WORK* flowWk, BTL_POKEPARAM* bpp, u8 pokeID )
{
  WazaID  encoreWaza = BPP_SICKCONT_GetParam( BPP_GetSickCont(bpp, WAZASICK_ENCORE) );
  if( BPP_WAZA_GetPP_ByNumber(bpp, encoreWaza) == 0 )
  {
    BTL_HANDEX_PARAM_CURE_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CURE_SICK, pokeID );
      param->sickCode = WAZASICK_ENCORE;
      param->pokeID[0] = pokeID;
      param->poke_cnt = 1;
    BTL_SVF_HANDEX_Pop( flowWk, param );
  }
}


//----------------------------------------------------------------------------------------------
// 状態異常回復
//----------------------------------------------------------------------------------------------
static void turncheck_cureProc( BTL_SVFLOW_WORK* flowWk, BTL_POKEPARAM* bpp, u8 pokeID, WazaSick sick, BPP_SICK_CONT oldCont )
{
  BTL_HANDEX_STR_PARAMS  str;
  if( BTL_SICK_MakeDefaultCureMsg(sick, oldCont, bpp, ITEM_DUMMY_DATA, &str) )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
      param->str = str;
    BTL_SVF_HANDEX_Pop( flowWk, param );
  }

  switch( sick ){
  case WAZASICK_AKUBI:        cure_Akubi( flowWk, bpp ); break;
  case WAZASICK_HOROBINOUTA:  cure_HorobiNoUta( flowWk, bpp ); break;
  case WAZASICK_SASIOSAE:     cure_Sasiosae( flowWk, bpp ); break;
  case WAZASICK_KAIHUKUHUUJI: cure_KaifukuFuji( flowWk, bpp ); break;
  }
}

static void cure_Akubi( BTL_SVFLOW_WORK* flowWk, BTL_POKEPARAM* bpp )
{
  int turns = BTL_CALC_RandRange( BTL_NEMURI_TURN_MIN, BTL_NEMURI_TURN_MAX );
  if( turns > 0 )
  {
    if( BPP_GetPokeSick(bpp) == POKESICK_NULL )
    {
      u8 pokeID = BPP_GetID( bpp );
      BTL_HANDEX_PARAM_ADD_SICK* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_ADD_SICK, pokeID );
        param->fAlmost = TRUE;
        param->sickID = WAZASICK_NEMURI;
        param->sickCont = BPP_SICKCONT_MakeTurn( turns );
        param->pokeID = pokeID;
      BTL_SVF_HANDEX_Pop( flowWk, param );
    }
  }
}

static void cure_HorobiNoUta( BTL_SVFLOW_WORK* flowWk, BTL_POKEPARAM* bpp )
{
  putHorobiCounter( flowWk, bpp, 0 );
  {
    u8 pokeID = BPP_GetID( bpp );
    BTL_HANDEX_PARAM_KILL* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_KILL, pokeID );
      param->pokeID = pokeID;
    BTL_SVF_HANDEX_Pop( flowWk, param );
  }
}

// ほろびのうたカウント表示共通処理
static void putHorobiCounter( BTL_SVFLOW_WORK* flowWk, const BTL_POKEPARAM* bpp, u8 count )
{
  u8 pokeID = BPP_GetID( bpp );
  BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_HorobiCountDown );
    HANDEX_STR_AddArg( &param->str, pokeID );
    HANDEX_STR_AddArg( &param->str, count );
  BTL_SVF_HANDEX_Pop( flowWk, param );

}

// さしおさえ回復時
static void cure_Sasiosae( BTL_SVFLOW_WORK* flowWk, const BTL_POKEPARAM* bpp )
{
//  BTL_HANDLER_ITEM_Add( bpp );
  u8 pokeID = BPP_GetID( bpp );
  BTL_HANDEX_PARAM_CHECK_ITEM_EQUIP* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CHECK_ITEM_EQUIP, pokeID );
    param->pokeID = BPP_GetID( bpp );
    param->reactionType = BTL_ITEMREACTION_GEN;
  BTL_SVF_HANDEX_Pop( flowWk, param );
}
// かいふくふうじ回復時
static void cure_KaifukuFuji( BTL_SVFLOW_WORK* flowWk, const BTL_POKEPARAM* bpp )
{
//  BTL_HANDLER_ITEM_Add( bpp );
  u8 pokeID = BPP_GetID( bpp );
  BTL_HANDEX_PARAM_CHECK_ITEM_EQUIP* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_CHECK_ITEM_EQUIP, pokeID );
    param->pokeID = BPP_GetID( bpp );
    param->reactionType = BTL_ITEMREACTION_HP;
  BTL_SVF_HANDEX_Pop( flowWk, param );
}


static void cure_Bind( BTL_SVFLOW_WORK* flowWk, const BTL_POKEPARAM* bpp, BPP_SICK_CONT oldCont )
{
  u8 pokeID = BPP_GetID( bpp );
  WazaID waza = BPP_SICKCONT_GetParam( oldCont );
  BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_BindCure );
    HANDEX_STR_AddArg( &param->str, pokeID );
    HANDEX_STR_AddArg( &param->str, waza );
  BTL_SVF_HANDEX_Pop( flowWk, param );
}



//=============================================================================================
/**
 * 状態異常にかかった時のデフォルトメッセージＩＤを返す
 *
 * @param   sickID
 * @param   cont
 *
 * @retval  int   メッセージID（存在しない場合-1）
 */
//=============================================================================================
static int getDefaultSickStrID( WazaSick sickID, BPP_SICK_CONT cont )
{
  int strID = -1;

  switch( sickID ){
  case WAZASICK_YAKEDO:       strID = BTL_STRID_SET_YakedoGet; break;
  case WAZASICK_MAHI:         strID = BTL_STRID_SET_MahiGet; break;
  case WAZASICK_KOORI:        strID = BTL_STRID_SET_KoriGet; break;
  case WAZASICK_NEMURI:       strID = BTL_STRID_SET_NemuriGet; break;
  case WAZASICK_KONRAN:       strID = BTL_STRID_SET_KonranGet; break;
  case WAZASICK_MEROMERO:     strID = BTL_STRID_SET_MeromeroGet; break;
  case WAZASICK_AKUMU:        strID = BTL_STRID_SET_AkumuGet;  break;
  case WAZASICK_TYOUHATSU:    strID = BTL_STRID_SET_Chouhatu; break;
  case WAZASICK_ICHAMON:      strID = BTL_STRID_SET_Ichamon; break;
  case WAZASICK_AKUBI:        strID = BTL_STRID_SET_Akubi; break;
  case WAZASICK_YADORIGI:     strID = BTL_STRID_SET_Yadorigi; break;
  case WAZASICK_MIYABURU:     strID = BTL_STRID_SET_Miyaburu; break;
  case WAZASICK_NEWOHARU:     strID = BTL_STRID_SET_NeWoHaru; break;
  case WAZASICK_SASIOSAE:     strID = BTL_STRID_SET_Sasiosae; break;
  case WAZASICK_KANASIBARI:   strID = BTL_STRID_SET_Kanasibari; break;
  case WAZASICK_KAIHUKUHUUJI: strID = BTL_STRID_SET_KaifukuFuji; break;

  case WAZASICK_DOKU:
    strID = BPP_SICKCONT_IsMoudokuCont(cont)? BTL_STRID_SET_MoudokuGet : BTL_STRID_SET_DokuGet;
    break;

  default:
    break;
  }

  BTL_Printf("sickID=%d, defMsgID=%d\n", sickID, strID);

  return strID;
}

//=============================================================================================
/**
 * 状態異常にかかった時のデフォルト文字列を生成
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
    case WAZASICK_KANASIBARI:
      {
        WazaID  waza = BPP_SICKCONT_GetParam( cont );
        HANDEX_STR_AddArg( str, waza );
      }
      break;
    }
  }
  else
  {
    HANDEX_STR_Clear( str );
  }
}


static int getCureStrID( WazaSick sick, BOOL fUseItem )
{
  static const struct {
    WazaSick  sick;
    s16       strID_notItem;
    s16       strID_useItem;
  }dispatchTbl[] = {
    { WAZASICK_DOKU,          BTL_STRID_SET_DokuCure,         BTL_STRID_SET_UseItem_CureDoku    },
    { WAZASICK_YAKEDO,        BTL_STRID_SET_YakedoCure,       BTL_STRID_SET_UseItem_CureYakedo  },
    { WAZASICK_NEMURI,        BTL_STRID_SET_NemuriWake,       BTL_STRID_SET_UseItem_CureNemuri  },
    { WAZASICK_KOORI,         BTL_STRID_SET_KoriMelt,         BTL_STRID_SET_UseItem_CureKoori   },
    { WAZASICK_MAHI,          BTL_STRID_SET_MahiCure,         BTL_STRID_SET_UseItem_CureMahi    },
    { WAZASICK_ENCORE,        BTL_STRID_SET_EncoreCure,       -1                                },
    { WAZASICK_KANASIBARI,    BTL_STRID_SET_KanasibariCure,   -1                                },
    { WAZASICK_SASIOSAE,      BTL_STRID_SET_SasiosaeCure,     -1                                },
    { WAZASICK_BIND,          BTL_STRID_SET_BindCure,         BTL_STRID_SET_BindCure            },
    { WAZASICK_YADORIGI,      BTL_STRID_SET_BindCure,         BTL_STRID_SET_BindCure            },
    { WAZASICK_TELEKINESIS,   BTL_STRID_SET_Telekinesis_End,  -1                                },
    { WAZASICK_TYOUHATSU,     BTL_STRID_SET_ChouhatuCure,     -1                                },
    { WAZASICK_FLYING,        BTL_STRID_SET_DenjiFuyuCure,    -1                                },
    { WAZASICK_KAIHUKUHUUJI,  BTL_STRID_SET_KaifukuFujiCure,  -1                                },
    { WAZASICK_ICHAMON,       BTL_STRID_SET_IchamonCure,      -1                                },

    { WAZASICK_KONRAN,        BTL_STRID_SET_KonranCure,       BTL_STRID_SET_UseItem_CureKonran  },
    { WAZASICK_MEROMERO,      BTL_STRID_SET_MeromeroCure,     BTL_STRID_SET_UseItem_CureMero    },
  };

  u32 i;

  for(i=0; i<NELEMS(dispatchTbl); ++i){
    if( dispatchTbl[i].sick == sick )
    {
      int strID = (fUseItem)? dispatchTbl[i].strID_useItem : dispatchTbl[i].strID_notItem;
      if( strID < 0 ){
        strID = dispatchTbl[i].strID_notItem;
      }
      return strID;
    }
  }

  return -1;

}

//=============================================================================================
/**
 * 回復時デフォルト文字列を生成
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
  if( strID >= 0 )
  {
    HANDEX_STR_Setup( str, BTL_STRTYPE_SET, strID );
    HANDEX_STR_AddArg( str, BPP_GetID( bpp ) );
    if( fUseItem ){
      HANDEX_STR_AddArg( str, itemID );
    }

    switch( sickID ){
    case WAZASICK_BIND:
      {
        WazaID  waza = BPP_SICKCONT_GetParam( oldCont );
        HANDEX_STR_AddArg( str, waza );
      }
      break;
    case WAZASICK_YADORIGI:
      HANDEX_STR_AddArg( str, WAZANO_YADORIGINOTANE );
      break;

    }
    return TRUE;
  }
  return FALSE;
}

/*----------------------------------------------------------------------------------------------------*/
/* イベントハンドラ同様、特定イベントに反応する状態異常ハンドラ                                       */
/*----------------------------------------------------------------------------------------------------*/

/**-------------------------------------------------------------
 * タイプごとの相性無効ハンドラ
 *------------------------------------------------------------*/
void BTL_SICKEVENT_CheckNotEffectByType( BTL_SVFLOW_WORK* flowWk, const BTL_POKEPARAM* defender )
{
  // とくていタイプを「みやぶる」状態なら、相性無効をキャンセルする
  if( BPP_CheckSick(defender, WAZASICK_MIYABURU) )
  {
    BPP_SICK_CONT cont = BPP_GetSickCont( defender, WAZASICK_MIYABURU );
    PokeType flatType = BPP_SICKCONT_GetParam( cont );
    PokeType pokeType = BTL_EVENTVAR_GetValue( BTL_EVAR_POKE_TYPE );
    if( flatType == pokeType )
    {
      if( !BPP_SICKCONT_GetFlag(cont) ){
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_FLAT_FLAG, TRUE );
      }else{
        BTL_EVENTVAR_RewriteValue( BTL_EVAR_FLATMASTER_FLAG, TRUE );
      }
    }
  }

  // 「ねをはる」状態なら、地面ワザが等倍
  if( BPP_CheckSick(defender, WAZASICK_NEWOHARU) ){
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == POKETYPE_JIMEN ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_FLAT_FLAG, TRUE );
    }
  }

  // 「うちおとす」状態なら、地面ワザが等倍
  if( BPP_CheckSick(defender, WAZASICK_FLYING_CANCEL) )
  {
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_WAZA_TYPE) == POKETYPE_JIMEN ){
      BTL_EVENTVAR_RewriteValue( BTL_EVAR_FLAT_FLAG, TRUE );
    }
  }
}
/**-------------------------------------------------------------
 * 逃げ禁止チェックハンドラ
 *------------------------------------------------------------*/
void BTL_SICKEVENT_CheckEscapeForbit( BTL_SVFLOW_WORK* flowWk, const BTL_POKEPARAM* bpp )
{
  if( (BPP_CheckSick(bpp, WAZASICK_TOOSENBOU))
  ||  (BPP_CheckSick(bpp, WAZASICK_BIND))
  ){
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, TRUE );
  }
}
/**-------------------------------------------------------------
 * 飛行フラグチェックハンドラ
 *------------------------------------------------------------*/
void BTL_SICKEVENT_CheckFlying( BTL_SVFLOW_WORK* flowWk, const BTL_POKEPARAM* bpp )
{
  if( BPP_CheckSick(bpp, WAZASICK_NEWOHARU) ){
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, TRUE );
  }
  if( BPP_CheckSick(bpp, WAZASICK_FLYING_CANCEL) ){
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_FAIL_FLAG, TRUE );
  }

  if( BPP_CheckSick(bpp, WAZASICK_TELEKINESIS)
  ||  BPP_CheckSick(bpp, WAZASICK_FLYING)
  ){
    BTL_EVENTVAR_RewriteValue( BTL_EVAR_GEN_FLAG, TRUE );
  }
}
/**-------------------------------------------------------------
 * 押し出しチェックハンドラ
 *------------------------------------------------------------*/
void BTL_SICKEVENT_CheckPushOutFail( BTL_SVFLOW_WORK* flowWk, const BTL_POKEPARAM* bpp )
{
  if( BPP_CheckSick(bpp, WAZASICK_NEWOHARU) )
  {
    if( BTL_EVENTVAR_RewriteValue(BTL_EVAR_FAIL_FLAG, TRUE) )
    {
      u8 pokeID = BPP_GetID( bpp );
      BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( flowWk, BTL_HANDEX_MESSAGE, pokeID );
        HANDEX_STR_Setup( &param->str, BTL_STRTYPE_SET, BTL_STRID_SET_NeWoHaruStick );
        HANDEX_STR_AddArg( &param->str, pokeID );
      BTL_SVF_HANDEX_Pop( flowWk, param );
    }
  }
}
/**-------------------------------------------------------------
 * 命中率チェックハンドラ
 *------------------------------------------------------------*/
void BTL_SICKEVENT_CheckHitRatio( BTL_SVFLOW_WORK* flowWk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender )
{
  // ミクル効果はここで処理せずターンフラグ処理としてシステムに組み込む
  #if 0
  if( BPP_CheckSick(attacker, WAZASICK_HITRATIO_UP) )
  {
    fx32 ratio;
    BPP_SICK_CONT cont = BPP_GetSickCont( attacker, WAZASICK_HITRATIO_UP );
    u16 param = BPP_SICKCONT_GetParam( cont );
    ratio = FX32_CONST( param ) / 100;
    BTL_EVENTVAR_MulValue( BTL_EVAR_RATIO, ratio );
  }
  #endif
}





//=============================================================================================
/**
 * バトンタッチで受け継ぐ状態異常コードかどうか判定
 *
 * @param   sick
 *
 * @retval  BOOL    受け継ぐならTRUE
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
    WAZASICK_TOOSENBOU,
    WAZASICK_NOROI,
    WAZASICK_TELEKINESIS,
    WAZASICK_AQUARING,
  };
  u32 i;
  for(i=0; i<NELEMS(sickTbl); ++i){
    if( sickTbl[i] == sick ){
      return TRUE;
    }
  }
  return FALSE;
}
