//=============================================================================================
/**
 * @file  btl_pokeparam.c
 * @brief ポケモンWB バトルシステム  参加ポケモン戦闘用データ
 * @author  taya
 *
 * @date  2008.10.08  作成
 */
//=============================================================================================
#include <gflib.h>
#include "poke_tool/poke_tool.h"
#include "waza_tool/wazadata.h"
#include "item/itemsym.h"

#include "btl_common.h"
#include "btl_calc.h"
#include "btl_pokeparam.h"


/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
  RANK_STATUS_MIN = 0,
  RANK_STATUS_MAX = 12,
  RANK_STATUS_DEF = 6,

  RANK_CRITICAL_MIN = 0,
  RANK_CRITICAL_MAX = 4,
  RANK_CRITICAL_DEF = 0,

  TURNFLG_BUF_SIZE = (BPP_TURNFLG_MAX/8)+(BPP_TURNFLG_MAX%8!=0),
  ACTFLG_BUF_SIZE  = (BPP_ACTFLG_MAX/8)+(BPP_ACTFLG_MAX%8!=0),
  CONTFLG_BUF_SIZE = (BPP_CONTFLG_MAX/8)+(BPP_CONTFLG_MAX%8!=0),

  TURNCOUNT_NULL = BTL_TURNCOUNT_MAX+1,

  WAZADMG_REC_TURN_MAX = BPP_WAZADMG_REC_TURN_MAX,  ///< ワザダメージレコード：何ターン分の記録を取るか？
  WAZADMG_REC_MAX = BPP_WAZADMG_REC_MAX,            ///< ワザダメージレコード：１ターンにつき、何件分まで記録するか？
};


//--------------------------------------------------------------
/**
 *  基本パラメータ  --バトル中、不変な要素--
 */
//--------------------------------------------------------------
typedef struct {
  const POKEMON_PARAM*  ppSrc;
  const POKEMON_PARAM*  hensinSrc;
  u16   hp;
  u16   item;
  u8    myID;
}BPP_CORE_PARAM;

//--------------------------------------------------------------
/**
 *  基本パラメータ  --バトル中、不変な要素--
 */
//--------------------------------------------------------------
typedef struct {

  u16 monsno;       ///< ポケモンナンバー
  u16 hpMax;        ///< 最大HP

  u8  attack;       ///< こうげき
  u8  defence;      ///< ぼうぎょ
  u8  sp_attack;    ///< とくこう
  u8  sp_defence;   ///< とくぼう
  u8  agility;      ///< すばやさ

  u8  level;        ///< レベル
  u8  type1;        ///< タイプ１
  u8  type2;        ///< タイプ２
  u8  sex;          ///< 性別

}BPP_BASE_PARAM;

//--------------------------------------------------------------
/**
 *  ランク効果
 */
//--------------------------------------------------------------
typedef struct {

  s8  attack;     ///< こうげき
  s8  defence;    ///< ぼうぎょ
  s8  sp_attack;  ///< とくこう
  s8  sp_defence; ///< とくぼう
  s8  agility;    ///< すばやさ
  s8  hit;        ///< 命中率
  s8  avoid;      ///< 回避率
  s8  critical;   ///< クリティカル率

}BPP_VARIABLE_PARAM;

//--------------------------------------------------------------
/**
 *  実際に計算に使われるパラメータ値（初期値×ランク効果）
 */
//--------------------------------------------------------------
typedef struct {

  u16 attack;       ///< こうげき
  u16 defence;      ///< ぼうぎょ
  u16 sp_attack;    ///< とくこう
  u16 sp_defence;   ///< とくぼう
  u16 agility;      ///< すばやさ
  u16 dmy;

}BPP_REAL_PARAM;

//--------------------------------------------------------------
/**
 *  ワザ
 */
//--------------------------------------------------------------
typedef struct {
  u16  number;            ///< ワザナンバー
  u16  recoverNumber;     ///< バトル中にワザが書き換わった時に、巻き戻し用に前のワザを保存
  u8   pp;                ///< PP値
  u8   ppMax;             ///< PP最大値
  u8   usedFlag;          ///< 使用したフラグ
}BPP_WAZA;


struct _BTL_POKEPARAM {

  BPP_CORE_PARAM      coreParam;

  BPP_BASE_PARAM      baseParam;
  BPP_VARIABLE_PARAM  varyParam;
  BPP_REAL_PARAM      realParam;
  BPP_WAZA            waza[ PTL_WAZA_MAX ];

  PokeTypePair  type;
  u16  tokusei;
  u16  actionAgility;

  u16 turnCount;        ///< 継続して戦闘に出ているカウンタ
  u16 appearedTurn;     ///< 戦闘に出たターンを記録

  u16 prevWazaID;             ///< 直前に使ったワザナンバー
  u16 sameWazaCounter;        ///< 同じワザを何連続で出しているかカウンタ
  BtlPokePos  prevTargetPos;  ///< 直前に狙った相手

  BPP_SICK_CONT   sickCont[ WAZASICK_MAX ];
  u8  wazaSickCounter[ WAZASICK_MAX ];

  u8  turnFlag[ TURNFLG_BUF_SIZE ];
  u8  actFlag [ ACTFLG_BUF_SIZE ];
  u8  contFlag[ CONTFLG_BUF_SIZE ];
  u8  counter[ BPP_COUNTER_MAX ];

  BPP_WAZADMG_REC  wazaDamageRec[ WAZADMG_REC_TURN_MAX ][ WAZADMG_REC_MAX ];
  u8               dmgrecCount[ WAZADMG_REC_TURN_MAX ];
  u8               dmgrecTurnPtr;
  u8               dmgrecPtr;

  u8  wazaCnt;
  u8  formNo;

//  u32 dmy;
};


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void setupBySrcData( BTL_POKEPARAM* bpp, const POKEMON_PARAM* srcPP );
static void clearWazaSickWork( BTL_POKEPARAM* pp, BOOL fPokeSickInclude );
static void clearUsedWazaFlag( BTL_POKEPARAM* bpp );
static void Effrank_Init( BPP_VARIABLE_PARAM* rank );
static void Effrank_Reset( BPP_VARIABLE_PARAM* rank );
static void Effrank_Recover( BPP_VARIABLE_PARAM* rank );
static const s8* getRankVaryStatusConst( const BTL_POKEPARAM* pp, BppValueID type, s8* min, s8* max );
static s8* getRankVaryStatus( BTL_POKEPARAM* pp, BppValueID type, s8* min, s8* max );
static void dmgrecClearWork( BTL_POKEPARAM* bpp );
static void dmgrecFwdTurn( BTL_POKEPARAM* bpp );
static void update_RealParam( BTL_POKEPARAM* pp );
static inline void flgbuf_clear( u8* buf, u32 size );
static inline void flgbuf_set( u8* buf, u32 flagID );
static inline void flgbuf_reset( u8* buf, u32 flagID );
static inline BOOL flgbuf_get( const u8* buf, u32 flagID );



//=============================================================================================
/**
 * バトル用ポケモンパラメータ生成
 *
 * @param   pp      ゲーム内汎用ポケモンパラメータ
 * @param   pokeID    バトル参加ポケID（バトルに参加しているポケモンに割り振る一意の数値）
 * @param   heapID
 *
 * @retval  BTL_POKEPARAM*
 */
//=============================================================================================
BTL_POKEPARAM*  BTL_POKEPARAM_Create( const POKEMON_PARAM* pp, u8 pokeID, HEAPID heapID )
{
  BTL_POKEPARAM* bpp = GFL_HEAP_AllocClearMemory( heapID, sizeof(BTL_POKEPARAM) );

  bpp->coreParam.myID = pokeID;
  bpp->coreParam.ppSrc = pp;
  bpp->coreParam.hp = PP_Get( pp, ID_PARA_hp, 0 );
  bpp->coreParam.item = PP_Get( pp, ID_PARA_item, NULL );
  bpp->coreParam.hensinSrc = NULL;

  setupBySrcData( bpp, pp );

  // ランク効果初期化
  Effrank_Init( &bpp->varyParam );

  // 状態異常ワーク初期化
  clearWazaSickWork( bpp, TRUE );

  // この時点でポケモン用状態異常になっていれば引き継ぎ
  {
    PokeSick sick = PP_GetSick( pp );
    if( sick == POKESICK_NEMURI )
    {
      u32 turns = BTL_CALC_RandRange( BTL_NEMURI_TURN_MIN, BTL_NEMURI_TURN_MAX );
      bpp->sickCont[WAZASICK_NEMURI] = BPP_SICKCONT_MakeTurn( turns );
      bpp->wazaSickCounter[WAZASICK_NEMURI] = 0;
    }
  }

  // 各種ワーク領域初期化
  bpp->appearedTurn = TURNCOUNT_NULL;
  bpp->turnCount = 0;
  bpp->prevWazaID = WAZANO_NULL;
  bpp->sameWazaCounter = 0;
  bpp->actionAgility = 0;

  flgbuf_clear( bpp->turnFlag, sizeof(bpp->turnFlag) );
  flgbuf_clear( bpp->contFlag, sizeof(bpp->contFlag) );

  dmgrecClearWork( bpp );

  return bpp;
}
//----------------------------------------------------------------------------------
/**
 * 元になるポケモンパラメータ構造体から必要部分を構築
 *
 * @param   bpp
 * @param   srcPP
 */
//----------------------------------------------------------------------------------
static void setupBySrcData( BTL_POKEPARAM* bpp, const POKEMON_PARAM* srcPP )
{
  u16 monsno = PP_Get( srcPP, ID_PARA_monsno, 0 );
  int i;

  // 基本パラメタ初期化
  bpp->baseParam.monsno = monsno;
  bpp->baseParam.level = PP_Get( srcPP, ID_PARA_level, 0 );
  bpp->baseParam.hpMax = PP_Get( srcPP, ID_PARA_hpmax, 0 );
  bpp->baseParam.attack = PP_Get( srcPP, ID_PARA_pow, 0 );
  bpp->baseParam.defence = PP_Get( srcPP, ID_PARA_def, 0 );
  bpp->baseParam.sp_attack = PP_Get( srcPP, ID_PARA_spepow, 0 );
  bpp->baseParam.sp_defence = PP_Get( srcPP, ID_PARA_spedef, 0 );
  bpp->baseParam.agility = PP_Get( srcPP, ID_PARA_agi, 0 );
  bpp->baseParam.type1 = PP_Get( srcPP, ID_PARA_type1, 0 );
  bpp->baseParam.type2 = PP_Get( srcPP, ID_PARA_type2, 0 );
  bpp->baseParam.sex = PP_GetSex( srcPP );

  // 実パラメータ初期化
  bpp->realParam.attack = bpp->baseParam.attack;
  bpp->realParam.defence = bpp->baseParam.defence;
  bpp->realParam.sp_attack = bpp->baseParam.sp_attack;
  bpp->realParam.sp_defence = bpp->baseParam.sp_defence;
  bpp->realParam.agility = bpp->baseParam.agility;

  // 所有ワザデータ初期化
  bpp->wazaCnt = 0;
  for(i=0; i<PTL_WAZA_MAX; i++)
  {
    bpp->waza[i].number = PP_Get( srcPP, ID_PARA_waza1+i, 0 );
    if( bpp->waza[i].number != WAZANO_NULL )
    {
      bpp->waza[i].pp = PP_Get( srcPP, ID_PARA_pp1+i, 0 );
      bpp->waza[i].ppMax = PP_Get( srcPP, ID_PARA_pp_max1+i, 0 );
      bpp->wazaCnt++;
    }
    else
    {
      bpp->waza[i].pp = 0;
      bpp->waza[i].ppMax = 0;
    }
    bpp->waza[i].usedFlag = FALSE;
    bpp->waza[i].recoverNumber = bpp->waza[i].number;
  }

  bpp->type = PokeTypePair_Make( bpp->baseParam.type1, bpp->baseParam.type2 );
  bpp->tokusei = PP_Get( srcPP, ID_PARA_speabino, 0 );

  bpp->formNo = PP_Get( srcPP, ID_PARA_form_no, 0 );

}
//----------------------------------------------------------------------------------
/**
 * ワザ使用フラグのクリア
 *
 * @param   bpp
 */
//----------------------------------------------------------------------------------
static void clearUsedWazaFlag( BTL_POKEPARAM* bpp )
{
  u32 i;
  for(i=0; i<NELEMS(bpp->waza); ++i){
    bpp->waza[i].usedFlag = FALSE;
  }
}
//----------------------------------------------------------------------------------
/**
 * 各種カウンタ値をクリア
 *
 * @param   bpp
 */
//----------------------------------------------------------------------------------
static void clearCounter( BTL_POKEPARAM* bpp )
{
  u32 i;
  for(i=0; i<NELEMS(bpp->counter); ++i){
    bpp->counter[i] = 0;
  }
}

//=============================================================================================
/**
 * バトル用ポケモンパラメータ削除
 *
 * @param   bpp
 *
 */
//=============================================================================================
void BTL_POKEPARAM_Delete( BTL_POKEPARAM* bpp )
{
  GFL_HEAP_FreeMemory( bpp );
}

//=============================================================================================
/**
 * データコピー
 *
 * @param   dst
 * @param   src
 *
 */
//=============================================================================================
void BTL_POKEPARAM_Copy( BTL_POKEPARAM* dst, const BTL_POKEPARAM* src )
{
  *dst = *src;
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

static void Effrank_Init( BPP_VARIABLE_PARAM* rank )
{
  Effrank_Reset( rank );
  rank->critical = RANK_CRITICAL_DEF;
}
static void Effrank_Reset( BPP_VARIABLE_PARAM* rank )
{
  rank->attack = RANK_STATUS_DEF;
  rank->defence = RANK_STATUS_DEF;
  rank->sp_attack = RANK_STATUS_DEF;
  rank->sp_defence = RANK_STATUS_DEF;
  rank->agility = RANK_STATUS_DEF;
  rank->hit = RANK_STATUS_DEF;
  rank->avoid = RANK_STATUS_DEF;
}
static void Effrank_Recover( BPP_VARIABLE_PARAM* rank )
{
  if( rank->attack < RANK_STATUS_DEF ){ rank->attack = RANK_STATUS_DEF; };
  if( rank->defence < RANK_STATUS_DEF ){  rank->defence = RANK_STATUS_DEF; }
  if( rank->sp_attack < RANK_STATUS_DEF ){ rank->sp_attack = RANK_STATUS_DEF; }
  if( rank->sp_defence < RANK_STATUS_DEF ){  rank->sp_defence = RANK_STATUS_DEF; }
  if( rank->agility < RANK_STATUS_DEF ){ rank->agility = RANK_STATUS_DEF; }
  if( rank->hit < RANK_STATUS_DEF ){ rank->hit = RANK_STATUS_DEF; }
  if( rank->avoid < RANK_STATUS_DEF ) { rank->avoid = RANK_STATUS_DEF; }
}



//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

u8 BPP_GetID( const BTL_POKEPARAM* pp )
{
  return pp->coreParam.myID;
}

u16 BPP_GetMonsNo( const BTL_POKEPARAM* pp )
{
  return pp->baseParam.monsno;
}

u8 BPP_WAZA_GetCount( const BTL_POKEPARAM* pp )
{
  return pp->wazaCnt;
}

u8 BPP_WAZA_GetUsedCount( const BTL_POKEPARAM* pp )
{
  u8 cnt, i;
  for(i=0, cnt=0; i<pp->wazaCnt; ++i)
  {
    if( pp->waza[i].usedFlag ){
      ++cnt;
    }
  }
  return cnt;
}

WazaID BPP_WAZA_GetID( const BTL_POKEPARAM* pp, u8 idx )
{
  GF_ASSERT(idx < pp->wazaCnt);
  return pp->waza[idx].number;
}

WazaID BPP_WAZA_GetParticular( const BTL_POKEPARAM* pp, u8 idx, u8* PP, u8* PPMax )
{
  GF_ASSERT(idx < pp->wazaCnt);
  *PP = pp->waza[idx].pp;
  *PPMax = pp->waza[idx].ppMax;

  return pp->waza[idx].number;
}


PokeTypePair BPP_GetPokeType( const BTL_POKEPARAM* pp )
{
  return pp->type;
}

BOOL BPP_IsMatchType( const BTL_POKEPARAM* pp, PokeType type )
{
  PokeTypePair pair = PokeTypePair_Make( pp->baseParam.type1, pp->baseParam.type2 );
  return PokeTypePair_IsMatch( pair, type );
}

const POKEMON_PARAM* BPP_GetSrcData( const BTL_POKEPARAM* bpp )
{
  return bpp->coreParam.ppSrc;
}


//=============================================================================================
/**
 * 各種パラメータ取得
 *
 * @param   pp
 * @param   vid
 *
 * @retval  int
 */
//=============================================================================================
int BPP_GetValue( const BTL_POKEPARAM* pp, BppValueID vid )
{
  switch( vid ){
  case BPP_ATTACK:    return pp->realParam.attack;
  case BPP_DEFENCE:   return pp->realParam.defence;
  case BPP_SP_ATTACK:   return pp->realParam.sp_attack;
  case BPP_SP_DEFENCE:  return pp->realParam.sp_defence;
  case BPP_AGILITY:   return pp->realParam.agility;

  case BPP_ATTACK_RANK:     return pp->varyParam.attack;
  case BPP_DEFENCE_RANK:    return pp->varyParam.defence;
  case BPP_SP_ATTACK_RANK:  return pp->varyParam.sp_attack;
  case BPP_SP_DEFENCE_RANK: return pp->varyParam.sp_defence;
  case BPP_AGILITY_RANK:    return pp->varyParam.agility;

  case BPP_HIT_RATIO:     return pp->varyParam.hit;
  case BPP_AVOID_RATIO:   return pp->varyParam.avoid;
  case BPP_CRITICAL_RATIO:  return pp->varyParam.critical;

  case BPP_LEVEL:     return pp->baseParam.level;
  case BPP_HP:        return pp->coreParam.hp;
  case BPP_MAX_HP:    return pp->baseParam.hpMax;
  case BPP_SEX:       return pp->baseParam.sex;

  case BPP_TOKUSEI:   return pp->tokusei;
  case BPP_FORM:      return pp->formNo;

  case BPP_HEAVY:     return 50;  // @@@ 今はてきとう

  default:
    GF_ASSERT(0);
    return 0;
  };
}
//=============================================================================================
/**
 * ランク補正フラットな状態のパラメータ取得
 *
 * @param   pp
 * @param   vid
 *
 * @retval  int
 */
//=============================================================================================
int BPP_GetValue_Base( const BTL_POKEPARAM* pp, BppValueID vid )
{
  switch( vid ){
  case BPP_ATTACK:    return pp->baseParam.attack;
  case BPP_DEFENCE:   return pp->baseParam.defence;
  case BPP_SP_ATTACK:   return pp->baseParam.sp_attack;
  case BPP_SP_DEFENCE:  return pp->baseParam.sp_defence;
  case BPP_AGILITY:   return pp->baseParam.agility;

  case BPP_HIT_RATIO:     return RANK_STATUS_DEF;
  case BPP_AVOID_RATIO:   return RANK_STATUS_DEF;
  case BPP_CRITICAL_RATIO:  return RANK_CRITICAL_DEF;

  default:
    return BPP_GetValue( pp, vid );
  };
}
//=============================================================================================
/**
 * クリティカルヒット時のパラメータ取得（攻撃側に不利なランク補正をフラットにする）
 *
 * @param   pp
 * @param   vid
 *
 * @retval  int
 */
//=============================================================================================
int BPP_GetValue_Critical( const BTL_POKEPARAM* pp, BppValueID vid )
{
  switch( vid ){
  case BPP_ATTACK:    return (pp->varyParam.attack < 0)? pp->baseParam.attack : pp->realParam.attack;
  case BPP_SP_ATTACK: return (pp->varyParam.sp_attack < 0)? pp->baseParam.sp_attack : pp->realParam.sp_attack;
  case BPP_DEFENCE:   return (pp->varyParam.defence > 0)? pp->baseParam.defence : pp->realParam.defence;
  case BPP_SP_DEFENCE:return (pp->varyParam.sp_defence > 0)? pp->baseParam.sp_defence : pp->realParam.sp_defence;

  default:
    return BPP_GetValue( pp, vid );
  }
}
//=============================================================================================
/**
 * 所有アイテムIDを返す
 *
 * @param   pp
 *
 * @retval  u32
 */
//=============================================================================================
u32 BPP_GetItem( const BTL_POKEPARAM* pp )
{
  return pp->coreParam.item;
}
void BPP_SetItem( BTL_POKEPARAM* pp, u16 itemID )
{
  pp->coreParam.item = itemID;
}

//=============================================================================================
/**
 * HP満タンかチェック
 *
 * @param   pp
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BPP_IsHPFull( const BTL_POKEPARAM* pp )
{
  return BPP_GetValue(pp, BPP_HP) == BPP_GetValue(pp, BPP_MAX_HP);
}
//=============================================================================================
/**
 * ひんし状態かチェック
 *
 * @param   pp
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BPP_IsDead( const BTL_POKEPARAM* pp )
{
  return BPP_GetValue( pp, BPP_HP ) == 0;
}
//=============================================================================================
/**
 * ワザPP値を取得
 *
 * @param   pp
 * @param   wazaIdx
 *
 */
//=============================================================================================
u16 BPP_WAZA_GetPP( const BTL_POKEPARAM* pp, u8 wazaIdx )
{
  if( wazaIdx < pp->wazaCnt ){
    return  pp->waza[wazaIdx].pp;
  }else{
    GF_ASSERT_MSG(0,"wazaIdx:%d, wazaCnt:%d", wazaIdx, pp->wazaCnt);
    return 0;
  }
}
//=============================================================================================
/**
 * ワザPP値が満タンかどうかチェック
 *
 * @param   pp
 * @param   wazaIdx
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BPP_IsPPFull( const BTL_POKEPARAM* pp, u8 wazaIdx )
{
  GF_ASSERT(wazaIdx < pp->wazaCnt);
  return  pp->waza[wazaIdx].pp == pp->waza[wazaIdx].ppMax;
}
//=============================================================================================
/**
 * 継続して場に出ているターン数を返す
 *
 * @param   pp
 *
 * @retval  u16
 */
//=============================================================================================
u16 BPP_GetTurnCount( const BTL_POKEPARAM* pp )
{
  return pp->turnCount;
}
//=============================================================================================
/**
 * 場に出た時のターン数を返す
 *
 * @param   pp
 *
 * @retval  u16
 */
//=============================================================================================
u16 BPP_GetAppearTurn( const BTL_POKEPARAM* pp )
{
  return pp->appearedTurn;
}
//=============================================================================================
/**
 * ターンフラグ値取得
 *
 * @param   pp
 * @param   flagID
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BPP_TURNFLAG_Get( const BTL_POKEPARAM* pp, BppTurnFlag flagID )
{
  return flgbuf_get( pp->turnFlag, flagID );
}
//=============================================================================================
/**
 * アクション毎クリアフラグの取得
 *
 * @param   pp
 * @param   flagID
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BPP_GetActFlag( const BTL_POKEPARAM* pp, BppActFlag flagID )
{
  return flgbuf_get( pp->actFlag, flagID );
}
//=============================================================================================
/**
 * 永続フラグ値取得
 *
 * @param   pp
 * @param   flagID
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BPP_GetContFlag( const BTL_POKEPARAM* pp, BppContFlag flagID )
{
  return flgbuf_get( pp->contFlag, flagID );
}
//=============================================================================================
/**
 * HP残量のめやす（普通・半減・ピンチとか）を返す
 *
 * @param   pp
 *
 * @retval  BppHpBorder
 */
//=============================================================================================
BppHpBorder BPP_CheckHPBorder( const BTL_POKEPARAM* pp, u32 hp )
{
  if( hp <= (pp->baseParam.hpMax / 8) )
  {
    return BPP_HPBORDER_RED;
  }
  if( hp <= (pp->baseParam.hpMax / 3) )
  {
    return BPP_HPBORDER_YELLOW;
  }
  return BPP_HPBORDER_GREEN;
}
//=============================================================================================
/**
 * HP残量のめやす（普通・半減・ピンチとか）を返す
 *
 * @param   pp
 *
 * @retval  BppHpBorder
 */
//=============================================================================================
BppHpBorder BPP_GetHPBorder( const BTL_POKEPARAM* pp )
{
  return BPP_CheckHPBorder( pp, pp->coreParam.hp );
}

//=============================================================================================
/**
 * 残りHP/MaxHP 比率（パーセンテージ）取得
 *
 * @param   pp
 *
 * @retval  fx32
 */
//=============================================================================================
fx32 BPP_GetHPRatio( const BTL_POKEPARAM* pp )
{
  double r = (double)(pp->coreParam.hp * 100) / (double)(pp->baseParam.hpMax);
  return FX32_CONST( r );
}
//=============================================================================================
/**
 * 指定ワザのindexを返す（自分が持っていないワザならPTL_WAZA_MAXを返す)
  *
 * @param   pp
 * @param   waza
 *
 * @retval  u8
 */
//=============================================================================================
u8 BPP_WAZA_SearchIdx( const BTL_POKEPARAM* pp, WazaID waza )
{
  u32 i;
  for(i=0; i<PTL_WAZA_MAX; ++i){
    if( pp->waza[i].number == waza ){
      return i;
    }
  }
  return PTL_WAZA_MAX;
}

//-----------------------------
static const s8* getRankVaryStatusConst( const BTL_POKEPARAM* pp, BppValueID type, s8* min, s8* max )
{
  const s8* ptr;

  *min = RANK_STATUS_MIN;
  *max = RANK_STATUS_MAX;

  switch( type ) {
  case BPP_ATTACK:      ptr = &pp->varyParam.attack; break;
  case BPP_DEFENCE:     ptr = &pp->varyParam.defence; break;
  case BPP_SP_ATTACK:   ptr = &pp->varyParam.sp_attack; break;
  case BPP_SP_DEFENCE:  ptr = &pp->varyParam.sp_defence; break;
  case BPP_AGILITY:     ptr = &pp->varyParam.agility; break;
  case BPP_HIT_RATIO:   ptr = &pp->varyParam.hit; break;
  case BPP_AVOID_RATIO: ptr = &pp->varyParam.avoid; break;
  case BPP_CRITICAL_RATIO:
    ptr = &pp->varyParam.critical;
    *min = RANK_CRITICAL_MIN;
    *max = RANK_CRITICAL_MAX;
    break;
  default:
    GF_ASSERT(0);
    return NULL;
  }
  return ptr;
}
static s8* getRankVaryStatus( BTL_POKEPARAM* pp, BppValueID type, s8* min, s8* max )
{
  return (s8*) getRankVaryStatusConst( pp, type, min, max );
}

//=============================================================================================
/**
 * ランク増減効果が有効か？
 *
 * @param   pp
 * @param   rankType
 * @param   volume
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BPP_IsRankEffectValid( const BTL_POKEPARAM* pp, BppValueID rankType, int volume )
{
  const s8* ptr;
  s8  min, max;

  ptr = getRankVaryStatusConst( pp, rankType, &min, &max );
  if( volume > 0 ){
    return ((*ptr) < max);
  }else{
    return ((*ptr) > min);
  }
}
//=============================================================================================
/**
 * ランク増加効果があと何段階効くか判定
 *
 * @param   pp
 * @param   rankType
 *
 * @retval  int   段階数
 */
//=============================================================================================
int BPP_RankEffectUpLimit( const BTL_POKEPARAM* pp, BppValueID rankType )
{
  const s8* ptr;
  s8  min, max;

  ptr = getRankVaryStatusConst( pp, rankType, &min, &max );
  return max - (*ptr);
}
//=============================================================================================
/**
 * ランク減少効果があと何段階効くか判定
 *
 * @param   pp
 * @param   rankType
 *
 * @retval  int   段階数（マイナス）
 */
//=============================================================================================
int BPP_RankEffectDownLimit( const BTL_POKEPARAM* pp, BppValueID rankType )
{
  const s8* ptr;
  s8  min, max;

  ptr = getRankVaryStatusConst( pp, rankType, &min, &max );
  return (*ptr) - min;
}

//=============================================================================================
/**
 * ランクアップ効果
 *
 * @param   pp
 * @param   rankType
 * @param   volume
 *
 * @retval  u8 result   実際に上がった段階数
 */
//=============================================================================================
u8 BPP_RankUp( BTL_POKEPARAM* pp, BppValueID rankType, u8 volume )
{
  s8 *ptr;
  s8 max = RANK_STATUS_MAX;

  switch( rankType ){
  case BPP_ATTACK:      ptr = &pp->varyParam.attack; break;
  case BPP_DEFENCE:     ptr = &pp->varyParam.defence; break;
  case BPP_SP_ATTACK:   ptr = &pp->varyParam.sp_attack; break;
  case BPP_SP_DEFENCE:  ptr = &pp->varyParam.sp_defence; break;
  case BPP_AGILITY:     ptr = &pp->varyParam.agility; break;
  case BPP_HIT_RATIO:   ptr = &pp->varyParam.hit; break;
  case BPP_AVOID_RATIO: ptr = &pp->varyParam.avoid; break;
  case BPP_CRITICAL_RATIO:
    ptr = &pp->varyParam.critical;
    max = RANK_CRITICAL_MAX;
    break;
  default:
    GF_ASSERT(0);
    return FALSE;
  }

  if( *ptr < max )
  {
    if( (*ptr + volume) > max ){
      volume = max - (*ptr);
    }
    *ptr += volume;
    update_RealParam( pp );
    return volume;
  }

  return 0;
}
//=============================================================================================
/**
 * ランクダウン効果
 *
 * @param   pp
 * @param   rankType
 * @param   volume
 *
 * @retval  u8    実際に下がった段階数
 */
//=============================================================================================
u8 BPP_RankDown( BTL_POKEPARAM* pp, BppValueID rankType, u8 volume )
{
  s8 *ptr;
  s8 min = RANK_STATUS_MIN;

  switch( rankType ){
  case BPP_ATTACK:    ptr = &pp->varyParam.attack; break;
  case BPP_DEFENCE:   ptr = &pp->varyParam.defence; break;
  case BPP_SP_ATTACK:   ptr = &pp->varyParam.sp_attack; break;
  case BPP_SP_DEFENCE:  ptr = &pp->varyParam.sp_defence; break;
  case BPP_AGILITY:   ptr = &pp->varyParam.agility; break;
  case BPP_HIT_RATIO:   ptr = &pp->varyParam.hit; break;
  case BPP_AVOID_RATIO: ptr = &pp->varyParam.avoid; break;
  case BPP_CRITICAL_RATIO:
    ptr = &pp->varyParam.critical;
    min = RANK_CRITICAL_MIN;
    break;
  default:
    GF_ASSERT(0);
    return FALSE;
  }

  if( *ptr > min )
  {
    if( (*ptr - volume) < min ){
      volume = (*ptr) - min;
    }
    *ptr -= volume;
    update_RealParam( pp );
    return volume;
  }
  return 0;
}
//=============================================================================================
/**
 * ランク値強制セット
 *
 * @param   pp
 * @param   rankType
 * @param   volume
 *
 */
//=============================================================================================
void BPP_RankSet( BTL_POKEPARAM* pp, BppValueID rankType, u8 value )
{
  s8 *ptr;
  u8 min = RANK_STATUS_MIN;
  u8 max = RANK_STATUS_MAX;

  switch( rankType ){
  case BPP_ATTACK:        ptr = &pp->varyParam.attack; break;
  case BPP_DEFENCE:       ptr = &pp->varyParam.defence; break;
  case BPP_SP_ATTACK:     ptr = &pp->varyParam.sp_attack; break;
  case BPP_SP_DEFENCE:    ptr = &pp->varyParam.sp_defence; break;
  case BPP_AGILITY:       ptr = &pp->varyParam.agility; break;
  case BPP_HIT_RATIO:     ptr = &pp->varyParam.hit; break;
  case BPP_AVOID_RATIO:   ptr = &pp->varyParam.avoid; break;
  case BPP_CRITICAL_RATIO:
    ptr = &pp->varyParam.critical;
    min = RANK_CRITICAL_MIN;
    max = RANK_CRITICAL_MAX;
    break;
  default:
    GF_ASSERT(0);
    return;
  }

  if( (min <= value) && (value <=max) ){
    *ptr = value;
  }else{
    GF_ASSERT_MSG(0, "value=%d", value);
  }
}

//=============================================================================================
/**
 * 下がっているランク効果をフラットに戻す
 *
 * @param   pp
 */
//=============================================================================================
void BPP_RankRecover( BTL_POKEPARAM* pp )
{
  Effrank_Recover( &pp->varyParam );
  update_RealParam( pp );
}
//=============================================================================================
/**
 * 全てのランク効果をフラットに戻す
 *
 * @param   pp
 */
//=============================================================================================
void BPP_RankReset( BTL_POKEPARAM* pp )
{
  Effrank_Reset( &pp->varyParam );
  update_RealParam( pp );
}

//=============================================================================================
/**
 * HP値を減少
 *
 * @param   pp
 * @param   value
 *
 */
//=============================================================================================
void BPP_HpMinus( BTL_POKEPARAM* pp, u16 value )
{
  if( pp->coreParam.hp > value )
  {
    pp->coreParam.hp -= value;
  }
  else
  {
    pp->coreParam.hp = 0;
  }
}
//=============================================================================================
/**
 * HP値を増加
 *
 * @param   pp
 * @param   value
 *
 */
//=============================================================================================
void BPP_HpPlus( BTL_POKEPARAM* pp, u16 value )
{
  pp->coreParam.hp += value;
  if( pp->coreParam.hp > pp->baseParam.hpMax )
  {
    pp->coreParam.hp = pp->baseParam.hpMax;
  }
}
//=============================================================================================
/**
 * HP値をゼロにする
 *
 * @param   pp
 *
 */
//=============================================================================================
void BPP_HpZero( BTL_POKEPARAM* pp )
{
  pp->coreParam.hp = 0;
}
//=============================================================================================
/**
 * ワザPP値を減少
 *
 * @param   pp
 * @param   wazaIdx
 * @param   value
 *
 */
//=============================================================================================
void BPP_PPMinus( BTL_POKEPARAM* pp, u8 wazaIdx, u8 value )
{
  GF_ASSERT(wazaIdx < pp->wazaCnt);

  if( pp->waza[wazaIdx].pp >= value )
  {
    pp->waza[wazaIdx].pp -= value;
  }
  else
  {
    pp->waza[wazaIdx].pp = 0;
  }
}
//=============================================================================================
/**
 * ワザPP値を増加
 *
 * @param   pp
 * @param   wazaIdx
 * @param   value
 *
 */
//=============================================================================================
void BPP_PPPlus( BTL_POKEPARAM* pp, u8 wazaIdx, u8 value )
{
  GF_ASSERT(wazaIdx < pp->wazaCnt);

  pp->waza[wazaIdx].pp += value;
  if( pp->waza[wazaIdx].pp > pp->waza[wazaIdx].ppMax )
  {
    pp->waza[wazaIdx].pp = pp->waza[wazaIdx].ppMax;
  }
}
//=============================================================================================
/**
 * 使用したワザフラグを立てる
 *
 * @param   pp
 * @param   wazaIdx
 */
//=============================================================================================
void BPP_WAZA_SetUsedFlag( BTL_POKEPARAM* pp, u8 wazaIdx )
{
  pp->waza[ wazaIdx ].usedFlag = TRUE;
}
//=============================================================================================
/**
 * ワザナンバー上書き
 *
 * @param   pp
 * @param   wazaIdx       何番目のワザ[0-3]？
 * @param   waza          上書き後ワザナンバー
 * @param   ppMax         PP最大値の上限（0ならデフォルト値）
 * @param   fPermenent    永続フラグ（TRUEならバトル後まで引き継ぐ／FALSEなら瀕死・入れかえで元に戻る）
 */
//=============================================================================================
void BPP_WAZA_UpdateID( BTL_POKEPARAM* pp, u8 wazaIdx, WazaID waza, u8 ppMax, BOOL fPermenent )
{
  BPP_WAZA* pWaza = &pp->waza[ wazaIdx ];

  if( !fPermenent ){
    pWaza->recoverNumber = pWaza->number;
  }else{
    pWaza->recoverNumber = waza;
  }
  pWaza->number = waza;
  pWaza->usedFlag = FALSE;
  pWaza->ppMax = WAZADATA_GetMaxPP( waza, 0 );
  if( (ppMax != 0)
  &&  (pWaza->ppMax < ppMax)
  ){
    pWaza->ppMax = ppMax;
  }
  pWaza->pp = pWaza->ppMax;
  BTL_Printf("ワザ上書き: ppMax指定=%d, 実地:%d\n", ppMax, pWaza->ppMax);
}
//=============================================================================================
/**
 * １ターン有効フラグのセット
 *
 * @param   pp
 * @param   flagID
 *
 */
//=============================================================================================
void BPP_TURNFLAG_Set( BTL_POKEPARAM* pp, BppTurnFlag flagID )
{
  flgbuf_set( pp->turnFlag, flagID );
}
//=============================================================================================
/**
 * アクションごとフラグのセット
 *
 * @param   pp
 * @param   flagID
 */
//=============================================================================================
void BPP_ACTFLAG_Set( BTL_POKEPARAM* pp, BppActFlag flagID )
{
  flgbuf_set( pp->actFlag, flagID );
}
//=============================================================================================
/**
 * 永続フラグのセット
 *
 * @param   pp
 * @param   flagID
 *
 */
//=============================================================================================
void BPP_CONTFLAG_Set( BTL_POKEPARAM* pp, BppContFlag flagID )
{
  flgbuf_set( pp->contFlag, flagID );
}
//=============================================================================================
/**
 * 永続フラグのリセット
 *
 * @param   pp
 * @param   flagID
 *
 */
//=============================================================================================
void BPP_CONTFLAG_Clear( BTL_POKEPARAM* pp, BppContFlag flagID )
{
  flgbuf_reset( pp->contFlag, flagID );
}

//==================================================================================================
// 状態異常関連
//==================================================================================================


//=============================================================================================
/**
 * 状態異常を設定
 *
 * @param   pp
 * @param   sick
 * @param   contParam   継続パラメータ
 *
 */
//=============================================================================================
void BPP_SetWazaSick( BTL_POKEPARAM* bpp, WazaSick sick, BPP_SICK_CONT contParam )
{
  if( sick < POKESICK_MAX )
  {
    PokeSick pokeSick = BPP_GetPokeSick( bpp );
    GF_ASSERT(pokeSick == POKESICK_NULL);
  }

  bpp->sickCont[ sick ] = contParam;
  bpp->wazaSickCounter[sick] = 0;
}


//=============================================================================================
/**
 * 全状態異常のターンチェック処理
 *
 * @param   bpp
 * @param   callbackFunc    かかっている状態異常ごとに呼び出されるコールバック関数
 * @param   callbackArg     コールバックに引き渡す任意引数
 */
//=============================================================================================
void BPP_WazaSick_TurnCheck( BTL_POKEPARAM* bpp, BtlSickTurnCheckFunc callbackFunc, void* callbackWork )
{
  WazaSick  sick;

  for(sick=0; sick<NELEMS(bpp->sickCont); ++sick)
  {
    if( bpp->sickCont[sick].type != WAZASICK_CONT_NONE )
    {
      u32 turnMax = BPP_SICCONT_GetTurnMax( bpp->sickCont[sick] );
      BOOL fCure = FALSE;

      // 継続ターン経過チェック
      if( turnMax )
      {
        u8 n = 1;
        if( (sick == WAZASICK_NEMURI) && (bpp->tokusei == POKETOKUSEI_HAYAOKI) ){
          n = 2;    // とくせい「はやおき」は眠りカウンタ２倍速
        }
        BTL_Printf("ポケ[%d - %p], 状態異常[%d] 最大ターン=%d, counter=%d ->",
          bpp->coreParam.myID, bpp, sick, turnMax, bpp->wazaSickCounter[sick] );
        bpp->wazaSickCounter[sick] += n;
        OS_TPrintf(" %d (adrs=%p)\n", bpp->wazaSickCounter[sick], &(bpp->wazaSickCounter[sick]));

        if( bpp->wazaSickCounter[sick] >= turnMax )
        {
          bpp->wazaSickCounter[sick] = 0;
          // 眠り自体のオフは行動チェック時に行う
          if( sick != WAZASICK_NEMURI ){
            bpp->sickCont[sick].type = WAZASICK_CONT_NONE;
            fCure = TRUE;
          }
        }
      }
      // 永続型で最大ターン数が指定されているものはカウンタをインクリメント
      // （現状、この機構を利用しているのは「もうどく」のみ）
      else if( bpp->sickCont[sick].type == WAZASICK_CONT_PERMANENT )
      {
        if( (bpp->sickCont[sick].permanent.count_max != 0 )
        &&  (bpp->wazaSickCounter[sick] < bpp->sickCont[sick].permanent.count_max)
        ){
          bpp->wazaSickCounter[sick]++;
        }
      }
      if( callbackFunc != NULL ){
        callbackFunc( bpp, sick, fCure, callbackWork );
      }
    }
  }
}
//=============================================================================================
/**
 * ポケモン系状態異常を回復させる
 *
 * @param   pp
 *
 */
//=============================================================================================
void BPP_CurePokeSick( BTL_POKEPARAM* pp )
{
  u32 i;
  for(i=POKESICK_ORIGIN; i<POKESICK_MAX; ++i)
  {
    pp->sickCont[ i ] = BPP_SICKCONT_MakeNull();
  }
  pp->sickCont[WAZASICK_AKUMU] = BPP_SICKCONT_MakeNull();  // 眠りが治れば“あくむ”も治る
}
//=============================================================================================
/**
 * ワザ系状態異常を回復させる
 *
 * @param   pp
 * @param   sick
 *
 */
//=============================================================================================
void BPP_CureWazaSick( BTL_POKEPARAM* pp, WazaSick sick )
{
  if( sick < POKESICK_MAX )
  {
    BPP_CurePokeSick( pp );
  }
  else
  {
    pp->sickCont[ sick ] = BPP_SICKCONT_MakeNull();
  }
}
//=============================================================================================
/**
 * 特定ポケモンに依存している状態異常を回復させる
 *
 * @param   pp
 * @param   depend_pokeID
 */
//=============================================================================================
void BPP_CureWazaSickDependPoke( BTL_POKEPARAM* pp, u8 depend_pokeID )
{
  u32 i;
  u8 fCure;
  for(i=0; i<WAZASICK_MAX; ++i)
  {
    switch( pp->sickCont[i].type ){
    case WAZASICK_CONT_POKE:
      fCure = ( pp->sickCont[i].poke.ID == depend_pokeID );
      break;
    case WAZASICK_CONT_POKETURN:
      fCure = ( pp->sickCont[i].poketurn.pokeID == depend_pokeID );
      break;
    default:
      fCure = FALSE;
    }
    if( fCure ){
      pp->sickCont[i] = BPP_SICKCONT_MakeNull();
      if( i == WAZASICK_NEMURI ){
        pp->sickCont[WAZASICK_AKUMU] = BPP_SICKCONT_MakeNull();
      }
    }
  }
}
//=============================================================================================
/**
 * 「ねむり」ターン進行　※ポケモン状態異常チェックは、「たたかう」を選んだ場合にのみ行う
 *
 * @param   pp
 *
 * @retval  BOOL    目が覚めた場合はTrue
 */
//=============================================================================================
BOOL BPP_Nemuri_CheckWake( BTL_POKEPARAM* pp )
{
  if( BPP_CheckSick(pp, WAZASICK_NEMURI) )
  {
    u8 turnMax = BPP_SICCONT_GetTurnMax( pp->sickCont[POKESICK_NEMURI] );
    if( (turnMax != 0 ) && (pp->wazaSickCounter[POKESICK_NEMURI] >= turnMax) )
    {
      pp->sickCont[ POKESICK_NEMURI ] = BPP_SICKCONT_MakeNull();
      pp->sickCont[ WAZASICK_AKUMU ] = BPP_SICKCONT_MakeNull();
     return TRUE;
    }
  }else{
    GF_ASSERT(0);
  }
  return FALSE;
}
//=============================================================================================
/**
 * ポケモン状態異常にかかっているかチェック
 *
 * @param   pp
 *
 * @retval  PokeSick    かかっている状態異常の識別子（かかっていない場合 POKESICK_NULL）
 */
//=============================================================================================
PokeSick BPP_GetPokeSick( const BTL_POKEPARAM* pp )
{
  u32 i;
  for(i=POKESICK_ORIGIN; i<POKESICK_MAX; ++i)
  {
    if( pp->sickCont[i].type != WAZASICK_CONT_NONE ){
      return i;
    }
  }
  return POKESICK_NULL;
}
//=============================================================================================
/**
 * 特定の状態異常にかかっているかチェック
 *
 * @param   pp
 * @param   sickType
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BPP_CheckSick( const BTL_POKEPARAM* pp, WazaSick sickType )
{
  GF_ASSERT(sickType < NELEMS(pp->sickCont));

  return pp->sickCont[ sickType ].type != WAZASICK_CONT_NONE;
}
//=============================================================================================
/**
 * 特定状態異常に設定されたパラメータ値（8bit）を取得
 *
 * @param   pp
 * @param   sick
 *
 * @retval  u8
 */
//=============================================================================================
u8 BPP_GetSickParam( const BTL_POKEPARAM* pp, WazaSick sick )
{
  GF_ASSERT(sick < NELEMS(pp->sickCont));

  switch(  pp->sickCont[sick].type ){
  case WAZASICK_CONT_TURN:
    return pp->sickCont[sick].turn.param;

  case WAZASICK_CONT_POKE:
    return pp->sickCont[sick].poke.ID;

  case WAZASICK_CONT_POKETURN:
    return pp->sickCont[sick].poketurn.pokeID;

  }

  GF_ASSERT(0); // パラメ無いのに呼び出された
  return 0;
}
//=============================================================================================
/**
 * 特定状態異常の継続パラメータ取得
 *
 * @param   bpp
 * @param   sick
 *
 * @retval  BPP_SICK_CONT
 */
//=============================================================================================
BPP_SICK_CONT BPP_GetSickCont( const BTL_POKEPARAM* bpp, WazaSick sick )
{
  GF_ASSERT(sick < NELEMS(bpp->sickCont));
  return bpp->sickCont[ sick ];
}
//=============================================================================================
/**
 * 特定状態異常の継続ターンカウンタを取得
 *
 * @param   bpp
 * @param   sick
 *
 * @retval  u8
 */
//=============================================================================================
u8 BPP_GetSickTurnCount( const BTL_POKEPARAM* bpp, WazaSick sick )
{
  GF_ASSERT(sick < NELEMS(bpp->sickCont));
  return bpp->wazaSickCounter[ sick ];
}
//=============================================================================================
/**
 * 状態異常のターンチェックで減るHPの量を計算
 *
 * @param   pp
 *
 * @retval  int
 */
//=============================================================================================
int BPP_CalcSickDamage( const BTL_POKEPARAM* pp, WazaSick sick )
{
  if( BPP_CheckSick(pp, sick) )
  {
    switch( sick ){
    case WAZASICK_DOKU:
      // カウンタが0なら通常の「どく」
      if( pp->wazaSickCounter[sick] == 0 ){
        return BTL_CALC_QuotMaxHP( pp, 8 );
      }
      // カウンタが1〜なら「どくどく」
      else{
        return (pp->baseParam.hpMax / 16) * pp->wazaSickCounter[sick];
      }
      break;

    case WAZASICK_YAKEDO:
      return BTL_CALC_QuotMaxHP( pp, 8 );

    case WAZASICK_AKUMU:
      if( BPP_CheckSick(pp, WAZASICK_NEMURI) ){
        return BTL_CALC_QuotMaxHP( pp, 4 );
      }
      break;

    case WAZASICK_NOROI:
      return BTL_CALC_QuotMaxHP( pp, 4 );

    default:
      return 0;
    }
  }
  return 0;
}
//----------------------------------------------------------------------------------
/**
 * ワザ系状態異常のカウンタクリア
 *
 * @param   pp
 */
//----------------------------------------------------------------------------------
static void clearWazaSickWork( BTL_POKEPARAM* bpp, BOOL fPokeSickInclude )
{
  u32 i, start;

  start = (fPokeSickInclude)? 0 : POKESICK_MAX;

  for(i=start; i<NELEMS(bpp->sickCont); ++i){
    bpp->sickCont[i].raw = 0;
    bpp->sickCont[i].type = WAZASICK_CONT_NONE;
  }
  GFL_STD_MemClear( bpp->wazaSickCounter, sizeof(bpp->wazaSickCounter) );
  BTL_Printf("ポケ[%d]の状態異常カウンタがクリアされた\n", bpp->coreParam.myID);
}





//=============================================================================================
/**
 * 場に入場した時のターンナンバーをセット
 *
 * @param   pp
 * @param   turn
 *
 */
//=============================================================================================
void BPP_SetAppearTurn( BTL_POKEPARAM* pp, u16 turn )
{
  GF_ASSERT(turn < BTL_TURNCOUNT_MAX);
  pp->appearedTurn = turn;
  pp->turnCount = 0;
  dmgrecClearWork( pp );
}
//=============================================================================================
/**
 * ターン終了処理
 *
 * @param   pp
 *
 */
//=============================================================================================
void BPP_TurnCheck( BTL_POKEPARAM* pp )
{
  flgbuf_clear( pp->turnFlag, sizeof(pp->turnFlag) );

  if( pp->turnCount < BTL_TURNCOUNT_MAX )
  {
    pp->turnCount++;
  }

  dmgrecFwdTurn( pp );
}
//=============================================================================================
/**
 * ターンフラグの個別途中クリア
 *
 * @param   pp
 * @param   flagID
 *
 */
//=============================================================================================
void BPP_TURNFLAG_ForceOff( BTL_POKEPARAM* bpp, BppTurnFlag flagID )
{
  flgbuf_reset( bpp->turnFlag, flagID );
}
//=============================================================================================
/**
 * アクションごとフラグのクリア
 *
 * @param   bpp
 */
//=============================================================================================
void BPP_ACTFLAG_Clear( BTL_POKEPARAM* bpp )
{
  flgbuf_clear( bpp->actFlag, sizeof(bpp->actFlag) );
}
//=============================================================================================
/**
 * 死亡による各種状態クリア
 *
 * @param   pp
 */
//=============================================================================================
void BPP_DeadClear( BTL_POKEPARAM* bpp )
{
  flgbuf_clear( bpp->actFlag, sizeof(bpp->actFlag) );
  flgbuf_clear( bpp->turnFlag, sizeof(bpp->turnFlag) );
  flgbuf_clear( bpp->contFlag, sizeof(bpp->contFlag) );

  clearWazaSickWork( bpp, TRUE );
  clearUsedWazaFlag( bpp );
  clearCounter( bpp );
}
//=============================================================================================
/**
 * 退場による各種状態クリア
 *
 * @param   bpp
 */
//=============================================================================================
void BPP_OutClear( BTL_POKEPARAM* bpp )
{
  flgbuf_clear( bpp->actFlag, sizeof(bpp->actFlag) );
  flgbuf_clear( bpp->turnFlag, sizeof(bpp->turnFlag) );
  flgbuf_clear( bpp->contFlag, sizeof(bpp->contFlag) );

  clearWazaSickWork( bpp, FALSE );
  clearUsedWazaFlag( bpp );
  clearCounter( bpp );
}
//=============================================================================================
/**
 * タイプかきかえ
 *
 * @param   pp
 * @param   type
 */
//=============================================================================================
void BPP_ChangePokeType( BTL_POKEPARAM* pp, PokeTypePair type )
{
  pp->type = type;
}
//=============================================================================================
/**
 * とくせい書き換え
 *
 * @param   pp
 * @param   tok
 */
//=============================================================================================
void BPP_ChangeTokusei( BTL_POKEPARAM* pp, PokeTokusei tok )
{
  pp->tokusei = tok;
}
//=============================================================================================
/**
 * フォルム変更
 *
 * @param   pp
 * @param   formNo
 */
//=============================================================================================
void BPP_ChangeForm( BTL_POKEPARAM* pp, u8 formNo )
{
  pp->formNo = formNo;
}
//=============================================================================================
/**
 * 所有アイテム削除
 *
 * @param   pp
 */
//=============================================================================================
void BPP_RemoveItem( BTL_POKEPARAM* pp )
{
  pp->coreParam.item = ITEM_DUMMY_DATA;
}


//=============================================================================================
/**
 * 直前に使ったワザナンバーを記録
 *
 * @param   pp
 * @param   waza
 */
//=============================================================================================
void BPP_UpdatePrevWazaID( BTL_POKEPARAM* pp, WazaID waza, BtlPokePos targetPos )
{
  // @@@ まもる・みきりの関係
  WazaID prev = pp->prevWazaID;
  if( prev != waza ){
    pp->prevWazaID = waza;
    pp->sameWazaCounter = 0;
  }else
  {
    pp->sameWazaCounter++;
  }
  pp->prevTargetPos = targetPos;
}
//=============================================================================================
/**
 * 直前に使ったワザナンバーをクリア
 *
 * @param   pp
 */
//=============================================================================================
void BPP_ResetWazaContConter( BTL_POKEPARAM* bpp )
{
  bpp->sameWazaCounter = 0;
}
//=============================================================================================
/**
 * 直前に使ったワザナンバーを返す（場に出てから使ったワザが無い場合は WAZANO_NULL ）
 *
 * @param   pp
 *
 * @retval  WazaID
 */
//=============================================================================================
WazaID BPP_GetPrevWazaID( const BTL_POKEPARAM* pp )
{
  return pp->prevWazaID;
}
//=============================================================================================
/**
 * 直前に狙った相手の位置を返す
 *
 * @param   pp
 *
 * @retval  BtlPokePos
 */
//=============================================================================================
BtlPokePos BPP_GetPrevTargetPos( const BTL_POKEPARAM* pp )
{
  return pp->prevTargetPos;
}
//=============================================================================================
/**
 * 同じワザを連続何回出しているか返す（連続していない場合は0、連続中は1オリジンの値が返る）
 *
 * @param   pp
 *
 * @retval  u32
 */
//=============================================================================================
u32 BPP_GetWazaContCounter( const BTL_POKEPARAM* pp )
{
  return pp->sameWazaCounter;
}


//----------------------------------------------------------------------------------
/**
 * ワザダメージレコード：ワーク初期化
 *
 * @param   bpp
 */
//----------------------------------------------------------------------------------
static void dmgrecClearWork( BTL_POKEPARAM* bpp )
{
  GFL_STD_MemClear( bpp->wazaDamageRec, sizeof(bpp->wazaDamageRec) );
  GFL_STD_MemClear( bpp->dmgrecCount, sizeof(bpp->dmgrecCount) );
  bpp->dmgrecTurnPtr = 0;
  bpp->dmgrecPtr = 0;
}
//----------------------------------------------------------------------------------
/**
 * ワザダメージレコード：ターンチェック
 *
 * @param   pp
 */
//----------------------------------------------------------------------------------
static void dmgrecFwdTurn( BTL_POKEPARAM* bpp )
{
  if( ++(bpp->dmgrecTurnPtr) >= WAZADMG_REC_TURN_MAX ){
    bpp->dmgrecTurnPtr = 0;
  }
  bpp->dmgrecCount[ bpp->dmgrecTurnPtr ] = 0;
}
//=============================================================================================
/**
 * ワザダメージレコード：１件記録
 *
 * @param   pp
 * @param   rec
 */
//=============================================================================================
void BPP_WAZADMGREC_Add( BTL_POKEPARAM* bpp, const BPP_WAZADMG_REC* rec )
{
  u8 turn, idx;

  turn = bpp->dmgrecTurnPtr;
  idx = bpp->dmgrecCount[ turn ];

  // 最大記録数を超えていたら古いものを破棄する
  if( idx == WAZADMG_REC_MAX )
  {
    u32 i;
    for(i=0; i<(WAZADMG_REC_MAX-1); ++i){
      bpp->wazaDamageRec[ turn ][ i ] = bpp->wazaDamageRec[ turn ][ i+1 ];
    }
    --idx;
  }

  bpp->wazaDamageRec[ turn ][ idx ] = *rec;

  if( bpp->dmgrecCount[ turn ] < WAZADMG_REC_MAX ){
    bpp->dmgrecCount[ turn ]++;
  }
}
//=============================================================================================
/**
 * ワザダメージレコード：指定ターンの記録件数を取得
 *
 * @param   pp
 * @param   turn_ridx   遡るターン数（0なら当該ターンの記録）
 */
//=============================================================================================
u8 BPP_WAZADMGREC_GetCount( const BTL_POKEPARAM* bpp, u8 turn_ridx )
{
  if( turn_ridx < WAZADMG_REC_TURN_MAX )
  {
    int turn = bpp->dmgrecTurnPtr - turn_ridx;
    if( turn < 0 ){
      turn += WAZADMG_REC_TURN_MAX;
    }
    return bpp->dmgrecCount[ turn ];
  }
  else
  {
    GF_ASSERT( 0 );
    return 0;
  }
}
//=============================================================================================
/**
 * ワザダメージレコード：レコードを取得
 *
 * @param   bpp
 * @param   turn_ridx   遡るターン数（0なら当該ターンの記録）
 * @param   rec_ridx    何件目のレコードか？（0=最新）
 * @param   dst         [out] 取得先ワークアドレス
 *
 * @retval  BOOL        レコードを取得できたらTRUE／それ以上の記録が無い場合などFALSE
 */
//=============================================================================================
BOOL BPP_WAZADMGREC_Get( const BTL_POKEPARAM* bpp, u8 turn_ridx, u8 rec_ridx, BPP_WAZADMG_REC* dst )
{
  if( turn_ridx < WAZADMG_REC_TURN_MAX )
  {
    int turn = bpp->dmgrecTurnPtr - turn_ridx;
    if( turn < 0 ){
      turn += WAZADMG_REC_TURN_MAX;
    }
    if( rec_ridx < bpp->dmgrecCount[ turn ] )
    {
      u8 idx = bpp->dmgrecCount[ turn ] - rec_ridx - 1;
      *dst = bpp->wazaDamageRec[ turn ][ idx ];
      return TRUE;
    }
  }
  return FALSE;
}
//=============================================================================================
/**
 * 行動順ソートの時に使用された「すばやさ」を記録しておく
 *（とくせい、ランク効果、アイテム装備効果などが反映されたもの）
 *
 * @param   bpp
 * @param   actionAgility
 */
//=============================================================================================
void BPP_SetActionAgility( BTL_POKEPARAM* bpp, u16 actionAgility )
{
  bpp->actionAgility = actionAgility;
}
//=============================================================================================
/**
 * 行動順ソートの時に使用された「すばやさ」を取得
 *（とくせい、ランク効果、アイテム装備効果などが反映されたもの）
 *
 * @param   bpp
 *
 * @retval  u16
 */
//=============================================================================================
u16 BPP_GetActionAgility( const BTL_POKEPARAM* bpp )
{
  return bpp->actionAgility;
}

//=============================================================================================
/**
 * カウンタ値をセット
 *
 * @param   bpp
 * @param   cnt     カウンタ用途
 * @param   value   カウンタ値
 */
//=============================================================================================
void BPP_COUNTER_Set( BTL_POKEPARAM* bpp, BppCounter cnt, u8 value )
{
  GF_ASSERT(cnt < BPP_COUNTER_MAX);
  bpp->counter[ cnt ] = value;
}
//=============================================================================================
/**
 * カウンタ値を取得
 *
 * @param   bpp
 * @param   cnt   カウンタ用途
 *
 * @retval  u8
 */
//=============================================================================================
u8 BPP_COUNTER_Get( const BTL_POKEPARAM* bpp, BppCounter cnt )
{
  GF_ASSERT(cnt < BPP_COUNTER_MAX);
  return bpp->counter[ cnt ];
}


//--------------------------------------------------------------------------
/**
 * ランク効果とデフォルト値をかけあわせて実際値を算出しておく
 *
 * @param   pp
 *
 */
//--------------------------------------------------------------------------
static void update_RealParam( BTL_POKEPARAM* pp )
{
  pp->realParam.attack     = BTL_CALC_StatusRank( pp->baseParam.attack, pp->varyParam.attack );
  pp->realParam.defence    = BTL_CALC_StatusRank( pp->baseParam.defence, pp->varyParam.defence );
  pp->realParam.sp_attack  = BTL_CALC_StatusRank( pp->baseParam.sp_attack, pp->varyParam.sp_attack );
  pp->realParam.sp_defence = BTL_CALC_StatusRank( pp->baseParam.sp_defence, pp->varyParam.sp_defence );
  pp->realParam.agility    = BTL_CALC_StatusRank( pp->baseParam.agility, pp->varyParam.agility );
}
//---------------------------------------------------------------------------------------------
// bitフラグバッファ処理
//---------------------------------------------------------------------------------------------
static inline void flgbuf_clear( u8* buf, u32 size )
{
  GFL_STD_MemClear( buf, size );
}
static inline void flgbuf_set( u8* buf, u32 flagID )
{
  {
    u8 byte = flagID / 8;
    u8 bit = (1 << (flagID%8));
    buf[ byte ] |= bit;
  }
}
static inline void flgbuf_reset( u8* buf, u32 flagID )
{
  {
    u8 byte = flagID / 8;
    u8 bit = (1 << (flagID%8));
    buf[ byte ] &= (~bit);
  }
}
static inline BOOL flgbuf_get( const u8* buf, u32 flagID )
{
//  GF_ASSERT(flagID<BPP_TURNFLG_MAX);
  {
    u8 byte = flagID / 8;
    u8 bit = (1 << (flagID%8));
    return (buf[ byte ] & bit) != 0;
  }
}

//---------------------------------------------------------------------------------------------
// 変身用ワーク処理
//---------------------------------------------------------------------------------------------

BOOL BPP_HENSIN_Set( BTL_POKEPARAM* bpp, const BTL_POKEPARAM* target )
{
  if( (bpp->coreParam.hensinSrc == NULL)
  &&  (target->coreParam.hensinSrc == NULL)
  ){
    static BPP_CORE_PARAM  core = {0};
    int i;

    core = bpp->coreParam;

    *bpp = *target;
    bpp->coreParam = core;

    // ワザのMAXPPは最大5に揃える
    for(i=0; i<PTL_WAZA_MAX; ++i)
    {
      if( bpp->waza[i].number != WAZANO_NULL )
      {
        if( bpp->waza[i].ppMax > 5 ){
          bpp->waza[i].ppMax = 5;
        }
        bpp->waza[i].pp = bpp->waza[i].ppMax;
        bpp->waza[i].usedFlag = FALSE;
      }
    }

    clearWazaSickWork( bpp, TRUE );
    flgbuf_clear( bpp->turnFlag, sizeof(bpp->turnFlag) );
    flgbuf_clear( bpp->contFlag, sizeof(bpp->contFlag) );

    bpp->appearedTurn = TURNCOUNT_NULL;
    bpp->turnCount = 0;
    bpp->prevWazaID = WAZANO_NULL;
    bpp->sameWazaCounter = 0;
    bpp->actionAgility = 0;

    bpp->coreParam.hensinSrc = target->coreParam.ppSrc;

    return TRUE;
  }

  return FALSE;
}


