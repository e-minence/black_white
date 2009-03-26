//=============================================================================================
/**
 * @file	btl_pokeparam.c
 * @brief	ポケモンWB バトルシステム  参加ポケモン戦闘用データ
 * @author	taya
 *
 * @date	2008.10.08	作成
 */
//=============================================================================================
#include <gflib.h>
#include "poke_tool/poke_tool.h"
#include "waza_tool/wazadata.h"

#include "btl_common.h"
#include "btl_calc.h"
#include "btl_pokeparam.h"


/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
	RANK_STATUS_MIN = 0,
	RANK_STATUS_MAX = 13,
	RANK_STATUS_DEF = 6,

	RANK_CRITICAL_MIN = 0,
	RANK_CRITICAL_MAX = 4,
	RANK_CRITICAL_DEF = 0,

	TURNFLG_BUF_SIZE = (BPP_TURNFLG_MAX/8)+(BPP_TURNFLG_MAX%8!=0),
	CONTFLG_BUF_SIZE = (BPP_CONTFLG_MAX/8)+(BPP_CONTFLG_MAX%8!=0),
};



//--------------------------------------------------------------
/**
 *	基本パラメータ  --バトル中、不変な要素--
 */
//--------------------------------------------------------------
typedef struct {

	u16	monsno;				///< ポケモンナンバー

	u8	hpMax;				///< 最大HP
	u8	attack;				///< こうげき
	u8	defence;			///< ぼうぎょ
	u8	sp_attack;		///< とくこう
	u8	sp_defence;		///< とくぼう
	u8	agility;			///< すばやさ

	u8  level;				///< レベル
	u8  type1;				///< タイプ１
	u8  type2;				///< タイプ２
	u8	sex;					///< 性別

}BPP_BASE_PARAM;

//--------------------------------------------------------------
/**
 *	ランク効果
 */
//--------------------------------------------------------------
typedef struct {

	s8	attack;			///< こうげき
	s8	defence;		///< ぼうぎょ
	s8	sp_attack;	///< とくこう
	s8	sp_defence;	///< とくぼう
	s8	agility;		///< すばやさ
	s8	hit;				///< 命中率
	s8	avoid;			///< 回避率
	s8	critical;		///< クリティカル率

}BPP_VARIABLE_PARAM;

//--------------------------------------------------------------
/**
 *	実際に計算に使われるパラメータ値（初期値×ランク効果）
 */
//--------------------------------------------------------------
typedef struct {

	u16	attack;				///< こうげき
	u16	defence;			///< ぼうぎょ
	u16	sp_attack;		///< とくこう
	u16	sp_defence;		///< とくぼう
	u16	agility;			///< すばやさ
	u16	dmy;

}BPP_REAL_PARAM;

//--------------------------------------------------------------
/**
 *	ワザ
 */
//--------------------------------------------------------------
typedef struct {
	u16  number;
	u8   pp;
	u8   ppMax;
}BPP_WAZA;


struct _BTL_POKEPARAM {

	BPP_BASE_PARAM			baseParam;
	BPP_VARIABLE_PARAM	varyParam;
	BPP_REAL_PARAM			realParam;
	BPP_WAZA						waza[ PTL_WAZA_MAX ];
	const POKEMON_PARAM*	ppSrc;

	u16  item;
	u16  tokusei;
	u16  hp;

	u8	myID;
	u8	wazaCnt;
	u8	pokeSick;
	u8	pokeSickCounter;

	BPP_SICK_CONT		sickCont[ WAZASICK_MAX ];
	u8	wazaSickCounter[ WAZASICK_MAX ];
	u8	turnFlag[ TURNFLG_BUF_SIZE ];
	u8	contFlag[ CONTFLG_BUF_SIZE ];

	u32 dmy;
};


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void Effrank_Init( BPP_VARIABLE_PARAM* rank );
static void update_RealParam( BTL_POKEPARAM* pp, BppValueID rankType );
static inline void flgbuf_clear( u8* buf, u32 size );
static inline void flgbuf_set( u8* buf, u32 flagID );
static inline void flgbuf_reset( u8* buf, u32 flagID );
static inline BOOL flgbuf_get( const u8* buf, u32 flagID );



//=============================================================================================
/**
 * バトル用ポケモンパラメータ生成
 *
 * @param   pp			ゲーム内汎用ポケモンパラメータ
 * @param   pokeID		バトル参加ポケID（バトルに参加しているポケモンに割り振る一意の数値）
 * @param   heapID		
 *
 * @retval  BTL_POKEPARAM*		
 */
//=============================================================================================
BTL_POKEPARAM*  BTL_POKEPARAM_Create( const POKEMON_PARAM* pp, u8 pokeID, HEAPID heapID )
{
	BTL_POKEPARAM* bpp = GFL_HEAP_AllocClearMemory( heapID, sizeof(BTL_POKEPARAM) );
	int i;
	u16 monsno;

	// 基本パラメタ初期化

	monsno = PP_Get( pp, ID_PARA_monsno, 0 );

	bpp->baseParam.monsno = monsno;
	bpp->baseParam.level = PP_Get( pp, ID_PARA_level, 0 );
	bpp->baseParam.hpMax = PP_Get( pp, ID_PARA_hpmax, 0 );
	bpp->baseParam.attack = PP_Get( pp, ID_PARA_pow, 0 );
	bpp->baseParam.defence = PP_Get( pp, ID_PARA_def, 0 );
	bpp->baseParam.sp_attack = PP_Get( pp, ID_PARA_spepow, 0 );
	bpp->baseParam.sp_defence = PP_Get( pp, ID_PARA_spedef, 0 );
	bpp->baseParam.agility = PP_Get( pp, ID_PARA_agi, 0 );
	bpp->baseParam.type1 = PP_Get( pp, ID_PARA_type1, 0 );
	bpp->baseParam.type2 = PP_Get( pp, ID_PARA_type2, 0 );
	bpp->baseParam.sex = PP_GetSex( pp );

	// ランク効果初期化
	Effrank_Init( &bpp->varyParam );

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
		bpp->waza[i].number = PP_Get( pp, ID_PARA_waza1+i, 0 );
		if( bpp->waza[i].number )
		{
			bpp->waza[i].pp = PP_Get( pp, ID_PARA_pp1+i, 0 );
			bpp->waza[i].ppMax = PP_Get( pp, ID_PARA_pp_max1+i, 0 );
			bpp->wazaCnt++;
		}
		else
		{
			bpp->waza[i].pp = 0;
			bpp->waza[i].ppMax = 0;
		}
	}

	// ワザ系状態異常のカウンタゼロクリア
	for(i=0; i<NELEMS(bpp->sickCont); ++i){
		bpp->sickCont[i].raw = 0;
		bpp->sickCont[i].type = WAZASICK_CONT_NONE;
	}
	GFL_STD_MemClear( bpp->wazaSickCounter, sizeof(bpp->wazaSickCounter) );

	bpp->item = 0;
	bpp->tokusei = PP_Get( pp, ID_PARA_speabino, 0 );
	bpp->hp = PP_Get( pp, ID_PARA_hp, 0 );
	bpp->myID = pokeID;
	bpp->pokeSick = PP_GetSick( pp );
	if( bpp->pokeSick != POKESICK_NEMURI )
	{
		bpp->pokeSickCounter = 0;
	}
	else
	{
		bpp->pokeSickCounter = BTL_CALC_RandRange( BTL_NEMURI_TURN_MIN, BTL_NEMURI_TURN_MIN );
	}

	bpp->ppSrc = pp;

	flgbuf_clear( bpp->turnFlag, sizeof(bpp->turnFlag) );
	flgbuf_clear( bpp->contFlag, sizeof(bpp->contFlag) );

	return bpp;
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
	rank->attack = RANK_STATUS_DEF;
	rank->defence = RANK_STATUS_DEF;
	rank->sp_attack = RANK_STATUS_DEF;
	rank->sp_defence = RANK_STATUS_DEF;
	rank->agility = RANK_STATUS_DEF;
	rank->hit = RANK_STATUS_DEF;
	rank->avoid = RANK_STATUS_DEF;
	rank->critical = RANK_CRITICAL_DEF;
}




//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

u8 BTL_POKEPARAM_GetID( const BTL_POKEPARAM* pp )
{
	return pp->myID;
}

u16 BTL_POKEPARAM_GetMonsNo( const BTL_POKEPARAM* pp )
{
	return pp->baseParam.monsno;
}

u8 BTL_POKEPARAM_GetWazaCount( const BTL_POKEPARAM* pp )
{
	return pp->wazaCnt;
}

WazaID BTL_POKEPARAM_GetWazaNumber( const BTL_POKEPARAM* pp, u8 idx )
{
	GF_ASSERT(idx < pp->wazaCnt);
	return pp->waza[idx].number;
}

WazaID BTL_POKEPARAM_GetWazaParticular( const BTL_POKEPARAM* pp, u8 idx, u8* PP, u8* PPMax )
{
	GF_ASSERT(idx < pp->wazaCnt);
	*PP = pp->waza[idx].pp;
	*PPMax = pp->waza[idx].ppMax;

	return pp->waza[idx].number;
}


PokeTypePair BTL_POKEPARAM_GetPokeType( const BTL_POKEPARAM* pp )
{
	return PokeTypePair_Make( pp->baseParam.type1, pp->baseParam.type2 );
}

BOOL BTL_POKEPARAM_IsMatchType( const BTL_POKEPARAM* pp, PokeType type )
{
	PokeTypePair pair = PokeTypePair_Make( pp->baseParam.type1, pp->baseParam.type2 );
	return PokeTypePair_IsMatch( pair, type );
}

const POKEMON_PARAM* BTL_POKEPARAM_GetSrcData( const BTL_POKEPARAM* bpp )
{
	return bpp->ppSrc;
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
int BTL_POKEPARAM_GetValue( const BTL_POKEPARAM* pp, BppValueID vid )
{
	switch( vid ){
	case BPP_ATTACK:		return pp->realParam.attack;
	case BPP_DEFENCE:		return pp->realParam.defence;
	case BPP_SP_ATTACK:		return pp->realParam.sp_attack;
	case BPP_SP_DEFENCE:	return pp->realParam.sp_defence;
	case BPP_AGILITY:		return pp->realParam.agility;

	case BPP_HIT_RATIO:			return pp->varyParam.hit;
	case BPP_AVOID_RATIO:		return pp->varyParam.avoid;
	case BPP_CRITICAL_RATIO:	return pp->varyParam.critical;

	case BPP_LEVEL:			return pp->baseParam.level;
	case BPP_HP:				return pp->hp;
	case BPP_MAX_HP:		return pp->baseParam.hpMax;
	case BPP_SEX:				return pp->baseParam.sex;

	case BPP_TOKUSEI:		return pp->tokusei;

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
int BTL_POKEPARAM_GetValue_Base( const BTL_POKEPARAM* pp, BppValueID vid )
{
	switch( vid ){
	case BPP_ATTACK:		return pp->baseParam.attack;
	case BPP_DEFENCE:		return pp->baseParam.defence;
	case BPP_SP_ATTACK:		return pp->baseParam.sp_attack;
	case BPP_SP_DEFENCE:	return pp->baseParam.sp_defence;
	case BPP_AGILITY:		return pp->baseParam.agility;

	case BPP_HIT_RATIO:			return RANK_STATUS_DEF;
	case BPP_AVOID_RATIO:		return RANK_STATUS_DEF;
	case BPP_CRITICAL_RATIO:	return RANK_CRITICAL_DEF;

	default:
		return BTL_POKEPARAM_GetValue( pp, vid );
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
int BTL_POKEPARAM_GetValue_Critical( const BTL_POKEPARAM* pp, BppValueID vid )
{
	switch( vid ){
	case BPP_ATTACK:	return (pp->varyParam.attack < 0)? pp->baseParam.attack : pp->realParam.attack;
	case BPP_SP_ATTACK:	return (pp->varyParam.sp_attack < 0)? pp->baseParam.sp_attack : pp->realParam.sp_attack;
	case BPP_DEFENCE:	return (pp->varyParam.defence > 0)? pp->baseParam.defence : pp->realParam.defence;
	case BPP_SP_DEFENCE:return (pp->varyParam.sp_defence > 0)? pp->baseParam.sp_defence : pp->realParam.sp_defence;

	default:
		return BTL_POKEPARAM_GetValue( pp, vid );
	}
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
BOOL BTL_POKEPARAM_IsDead( const BTL_POKEPARAM* pp )
{
	return BTL_POKEPARAM_GetValue( pp, BPP_HP ) == 0;
}
//=============================================================================================
/**
 * 
 *
 * @param   pp		
 *
 * @retval  BOOL		
 */
//=============================================================================================
BOOL BTL_POKEPARAM_IsFullHP( const BTL_POKEPARAM* pp )
{
	return BTL_POKEPARAM_GetValue(pp, BPP_HP) == BTL_POKEPARAM_GetValue(pp, BPP_MAX_HP);
}
//=============================================================================================
/**
 * ポケモン状態異常にかかっているかチェック
 *
 * @param   pp		
 *
 * @retval  PokeSick		かかっている状態異常の識別子（かかっていない場合 POKESICK_NULL）
 */
//=============================================================================================
PokeSick BTL_POKEPARAM_GetPokeSick( const BTL_POKEPARAM* pp )
{
	return pp->pokeSick;
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
BOOL BTL_POKEPARAM_CheckSick( const BTL_POKEPARAM* pp, WazaSick sickType )
{
	GF_ASSERT(sickType < NELEMS(pp->sickCont));

	return pp->sickCont[ sickType ].type != WAZASICK_CONT_NONE;
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
int BTL_POKEPARAM_CalcSickDamage( const BTL_POKEPARAM* pp )
{
	switch( pp->pokeSick ){
	case POKESICK_DOKU:
		// カウンタが0なら通常の「どく」
		if( pp->pokeSickCounter == 0 )
		{
			return BTL_CALC_QuotMaxHP( pp, 8 );;
		}
		// カウンタが1〜なら「どくどく」
		else
		{
			return (pp->baseParam.hpMax / 16) * pp->pokeSickCounter;
		}
		break;

	case POKESICK_YAKEDO:
			return BTL_CALC_QuotMaxHP( pp, 8 );

	default:
		return 0;
	}
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
BOOL BTL_POKEPARAM_GetTurnFlag( const BTL_POKEPARAM* pp, BppTurnFlag flagID )
{
	return flgbuf_get( pp->turnFlag, flagID );
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
BOOL BTL_POKEPARAM_GetContFlag( const BTL_POKEPARAM* pp, BppContFlag flagID )
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
BppHpBorder BTL_POKEPARAM_CheckHPBorder( const BTL_POKEPARAM* pp, u32 hp )
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
BppHpBorder BTL_POKEPARAM_GetHPBorder( const BTL_POKEPARAM* pp )
{
	return BTL_POKEPARAM_CheckHPBorder( pp, pp->hp );
}

//-----------------------------

//=============================================================================================
/**
 * ランクアップ効果
 *
 * @param   pp			
 * @param   rankType	
 * @param   volume		
 *
 * @retval  BOOL		ランクが上がった場合TRUE／もう上がらない場合FALSE
 */
//=============================================================================================
BOOL BTL_POKEPARAM_RankUp( BTL_POKEPARAM* pp, BppValueID rankType, u8 volume )
{
	s8 *ptr;
	s8 max = RANK_STATUS_MAX;

	switch( rankType ){
	case BPP_ATTACK:		ptr = &pp->varyParam.attack; break;
	case BPP_DEFENCE:		ptr = &pp->varyParam.defence; break;
	case BPP_SP_ATTACK:		ptr = &pp->varyParam.sp_attack; break;
	case BPP_SP_DEFENCE:	ptr = &pp->varyParam.sp_defence; break;
	case BPP_AGILITY:		ptr = &pp->varyParam.agility; break;
	case BPP_HIT_RATIO:		ptr = &pp->varyParam.hit; break;
	case BPP_AVOID_RATIO:	ptr = &pp->varyParam.avoid; break;
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
		*ptr += volume;
		if( *ptr > max )
		{
			*ptr = max;
		}
		update_RealParam( pp, rankType );
		return TRUE;
	}

	return FALSE;
}

//=============================================================================================
/**
 * ランクダウン効果
 *
 * @param   pp			
 * @param   rankType	
 * @param   volume		
 *
 * @retval  BOOL		ランクが下がった場合TRUE／もう下がらない場合FALSE
 */
//=============================================================================================
BOOL BTL_POKEPARAM_RankDown( BTL_POKEPARAM* pp, BppValueID rankType, u8 volume )
{
	s8 *ptr;
	s8 min = RANK_STATUS_MIN;

	switch( rankType ){
	case BPP_ATTACK:		ptr = &pp->varyParam.attack; break;
	case BPP_DEFENCE:		ptr = &pp->varyParam.defence; break;
	case BPP_SP_ATTACK:		ptr = &pp->varyParam.sp_attack; break;
	case BPP_SP_DEFENCE:	ptr = &pp->varyParam.sp_defence; break;
	case BPP_AGILITY:		ptr = &pp->varyParam.agility; break;
	case BPP_HIT_RATIO:		ptr = &pp->varyParam.hit; break;
	case BPP_AVOID_RATIO:	ptr = &pp->varyParam.avoid; break;
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
		*ptr -= volume;
		if( *ptr < min )
		{
			*ptr = min;
		}
		update_RealParam( pp, rankType );
		return TRUE;
	}

	return FALSE;
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
void BTL_POKEPARAM_HpMinus( BTL_POKEPARAM* pp, u16 value )
{
	if( pp->hp > value )
	{
		pp->hp -= value;
	}
	else
	{
		pp->hp = 0;
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
void BTL_POKEPARAM_HpPlus( BTL_POKEPARAM* pp, u16 value )
{
	pp->hp += value;
	if( pp->hp > pp->baseParam.hpMax )
	{
		pp->hp = pp->baseParam.hpMax;
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
void BTL_POKEPARAM_HpZero( BTL_POKEPARAM* pp )
{
	pp->hp = 0;
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
void BTL_POKEPARAM_PPMinus( BTL_POKEPARAM* pp, u8 wazaIdx, u8 value )
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
void BTL_POKEPARAM_PPPlus( BTL_POKEPARAM* pp, u8 wazaIdx, u8 value )
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
 * 状態異常を設定
 *
 * @param   pp			
 * @param   sick		
 * @param   contParam		継続パラメータ
 *
 */
//=============================================================================================
void BTL_POKEPARAM_SetWazaSick( BTL_POKEPARAM* pp, WazaSick sick, BPP_SICK_CONT contParam )
{
	if( sick < POKESICK_MAX )
	{
		GF_ASSERT(pp->pokeSick == POKESICK_NULL);
		pp->pokeSick = sick;
		pp->pokeSickCounter = contParam.turn.count;
	}
	else
	{
		pp->wazaSickCounter[sick] = contParam.turn.count;
		pp->sickCont[ sick ] = contParam;
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
void BTL_POKEPARAM_CurePokeSick( BTL_POKEPARAM* pp )
{
	pp->pokeSick = POKESICK_NULL;
	pp->pokeSickCounter = 0;
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
void BTL_POKEPARAM_SetTurnFlag( BTL_POKEPARAM* pp, BppTurnFlag flagID )
{
	flgbuf_set( pp->turnFlag, flagID );
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
void BTL_POKEPARAM_SetContFlag( BTL_POKEPARAM* pp, BppContFlag flagID )
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
void BTL_POKEPARAM_ResetContFlag( BTL_POKEPARAM* pp, BppContFlag flagID )
{
	flgbuf_reset( pp->contFlag, flagID );
}

//=============================================================================================
/**
 * 「ねむり」ターン進行　※ポケモン状態異常チェックは、「たたかう」を選んだ場合にのみ行う
 *
 * @param   pp			
 *
 * @retval  BOOL		目が覚めた場合はTrue
 */
//=============================================================================================
BOOL BTL_POKEPARAM_Nemuri_CheckWake( BTL_POKEPARAM* pp )
{
	GF_ASSERT(pp->pokeSick == POKESICK_NEMURI);

	if( pp->sickCont[POKESICK_NEMURI].type == WAZASICK_CONT_TURN )
	{
		if( pp->sickCont[POKESICK_NEMURI].turn.count == 0 )
		{
			pp->sickCont[POKESICK_NEMURI].type = WAZASICK_CONT_NONE;
			return TRUE;
		}
	}
	return FALSE;
}
//=============================================================================================
/**
 * 全状態異常のターンチェック処理
 *
 * @param   pp		
 *
 */
//=============================================================================================
void BTL_POKEPARAM_WazaSick_TurnCheck( BTL_POKEPARAM* pp )
{
	u32 i;
	for(i=0; i<NELEMS(pp->sickCont); ++i)
	{
		if( pp->sickCont[i].type == WAZASICK_CONT_TURN )
		{
			u8 n = 1;
			if( (i == WAZASICK_NEMURI)
			&&	(pp->tokusei == POKETOKUSEI_HAYAOKI )
			){
				n = 2;
			}

			if( pp->sickCont[i].turn.count > n ){
				pp->sickCont[i].turn.count--;
			}else{
				pp->sickCont[i].turn.count = 0;
				// 眠りのみ、目覚めチェックでオフにする
				if( i != WAZASICK_NEMURI )
				{
					pp->sickCont[i].type = WAZASICK_CONT_NONE;
				}
			}
		}
	}

	if( pp->pokeSick == POKESICK_DOKU )
	{
		if( (pp->pokeSickCounter!=0) && (pp->pokeSickCounter < BTL_MOUDOKU_COUNT_MAX) )
		{
			pp->pokeSickCounter++;
		}
	}
}
//=============================================================================================
/**
 * １ターン有効フラグのクリア
 *
 * @param   pp		
 *
 */
//=============================================================================================
void BTL_POKEPARAM_ClearTurnFlag( BTL_POKEPARAM* pp )
{
	flgbuf_clear( pp->turnFlag, sizeof(pp->turnFlag) );
}
//=============================================================================================
/**
 * とくせい書き換え
 *
 * @param   pp		
 * @param   tok		
 *
 */
//=============================================================================================
void BTL_POKEPARAM_ChangeTokusei( BTL_POKEPARAM* pp, PokeTokusei tok )
{
	pp->tokusei = tok;
}


//--------------------------------------------------------------------------
/**
 * ランク効果とデフォルト値をかけあわせて実際値を算出しておく
 *
 * @param   pp		
 * @param   rankType		
 *
 */
//--------------------------------------------------------------------------
static void update_RealParam( BTL_POKEPARAM* pp, BppValueID rankType )
{
	switch( rankType ){
	case BPP_ATTACK:
		pp->realParam.attack = BTL_CALC_StatusRank(pp->baseParam.attack, pp->varyParam.attack);
		break;
	case BPP_DEFENCE:
		pp->realParam.defence = BTL_CALC_StatusRank(pp->baseParam.defence, pp->varyParam.defence);
		break;
	case BPP_SP_ATTACK:
		pp->realParam.defence = BTL_CALC_StatusRank(pp->baseParam.defence, pp->varyParam.defence);
		break;
	case BPP_SP_DEFENCE:
		pp->realParam.defence = BTL_CALC_StatusRank(pp->baseParam.defence, pp->varyParam.defence);
		break;
	case BPP_AGILITY:
		pp->realParam.defence = BTL_CALC_StatusRank(pp->baseParam.defence, pp->varyParam.defence);
		break;
	default:
		return;
	}

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
//	GF_ASSERT(flagID<BPP_TURNFLG_MAX);
	{
		u8 byte = flagID / 8;
		u8 bit = (1 << (flagID%8));
		return (buf[ byte ] & bit) != 0;
	}
}


	