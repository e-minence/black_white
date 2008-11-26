//==============================================================================
/**
 * @file	pokeicon.c
 * @brief	ポケモンアイコン
 * @author	matsuda
 * @date	2008.11.25(火)
 */
//==============================================================================
#include <gflib.h>
#include "arc_def.h"
#include "poke_tool/poke_personal.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/monsno_def.h"

#include "pokeicon.h"
#include "poke_icon.naix"


//============================================================================================
//	定数定義
//============================================================================================
#define	POKEICON_GetPAL_TAMAGO			( 1 )	// タマゴのパレット番号
#define	POKEICON_GetPAL_TAMAGO_MNF		( 2 )	// マナフィのタマゴのパレット番号

//フォルムによってパレットが変わるポケモンのIconPalAtrテーブル参照番号
#define	POKEICON_GetTAMAGO		( 494 )		// タマゴ
#define	POKEICON_GetTAMAGO_MNF	( 495 )		// マナフィのタマゴ
#define	POKEICON_GetDEOKISISU	( 496 )		// デオキシス
#define	POKEICON_GetANNOON		( 499 )		// アンノーン
#define	POKEICON_GetMINOMUTTI	( 527 )		// ミノムッチ
#define	POKEICON_GetMINOMESU	( 529 )		// ミノメス
#define	POKEICON_GetSIIUSI		( 531 )		// シーウシ
#define	POKEICON_GetSIIDORUGO	( 532 )		// シードルゴ
#define	POKEICON_GetGIRATHINA	( 533 )		// ギラティナ
#define	POKEICON_GetSHEIMI		( 534 )		// シェイミ
#define	POKEICON_GetROTOMU		( 535 )		// ロトム


//==============================================================================
//	データ
//==============================================================================
#include "pokeicon.dat"


//--------------------------------------------------------------------------------------------
/**
 * キャラのアーカイブインデックス取得 ( POKEMON_PASO_PARAM 版 )
 *
 * @param	ppp			POKEMON_PASO_PARAM
 *
 * @return	アーカイブインデックス
 *
 */
//--------------------------------------------------------------------------------------------
u32 POKEICON_GetCgxArcIndex( const POKEMON_PASO_PARAM* ppp )
{
	u32  monsno;
	u32  arcIndex;
	BOOL fastMode;
	u32  form_no;
	u32  egg;

	fastMode = PPP_FastModeOn((POKEMON_PASO_PARAM*)ppp);
	monsno = PPP_Get(ppp, ID_PARA_monsno, NULL );
	egg = PPP_Get(ppp, ID_PARA_tamago_flag, NULL );
	form_no = POKEICON_GetCgxForm(ppp);

	arcIndex = POKEICON_GetCgxArcIndexByMonsNumber( monsno, form_no, egg );
	PPP_FastModeOff((POKEMON_PASO_PARAM*)ppp, fastMode);
	return arcIndex;
}

//--------------------------------------------------------------------------------------------
/**
 * キャラのアーカイブインデックス取得
 *
 * @param	mons		ポケモン番号
 * @param	form_no		フォルム番号
 * @param	egg			タマゴフラグ(TRUE=タマゴ)
 *
 * @return	アーカイブインデックス
 *
 *	form_noはデオキシスやアンノーンに使用
 */
//--------------------------------------------------------------------------------------------
u32 POKEICON_GetCgxArcIndexByMonsNumber( u32 mons, u32 form_no, BOOL egg )
{
	if( egg == TRUE ){
		if( mons == MONSNO_MANAFI ){
			return NARC_poke_icon_poke_icon_mnf_NCGR;
		}else{
			return NARC_poke_icon_poke_icon_tam_NCGR;
		}
	}

//	form_no = PokeFuseiFormNoCheck(mons, form_no);

	if( form_no != 0 ){
		if( mons == MONSNO_DEOKISISU ){
			return ( NARC_poke_icon_poke_icon_d01_NCGR + form_no - 1 );
		}
		if( mons == MONSNO_ANNOON ){
			return ( NARC_poke_icon_poke_icon_u02_NCGR + form_no - 1 );
		}
		if( mons == MONSNO_MINOMUTTI ){
			return ( NARC_poke_icon_poke_icon_455_01_NCGR + form_no - 1 );
		}
		if( mons == MONSNO_MINOMESU ){
			return ( NARC_poke_icon_poke_icon_457_01_NCGR + form_no - 1 );
		}
		if( mons == MONSNO_SIIUSI ){
			return ( NARC_poke_icon_poke_icon_458_01_NCGR + form_no - 1 );
		}
		if( mons == MONSNO_SIIDORUGO ){
			return ( NARC_poke_icon_poke_icon_459_01_NCGR + form_no - 1 );
		}
		if( mons == MONSNO_KIMAIRAN ){
			return ( NARC_poke_icon_poke_icon_509_01_NCGR + form_no - 1 );
		}
		if( mons == MONSNO_EURISU ){
			return ( NARC_poke_icon_poke_icon_516_01_NCGR + form_no - 1 );
		}
		if( mons == MONSNO_PURAZUMA ){
			return ( NARC_poke_icon_poke_icon_519_01_NCGR + form_no - 1 );
		}
	}

	if( mons > MONSNO_END ){ mons = 0; }

	return ( NARC_poke_icon_poke_icon_000_NCGR + mons );
}

//------------------------------------------------------------------
/**
 * キャラの拡張パターンナンバーを取得
 *
 * @param   ppp		
 *
 * @retval  u16		格調パターンナンバー
 */
//------------------------------------------------------------------
u16 POKEICON_GetCgxForm( const POKEMON_PASO_PARAM* ppp )
{
	u32 monsno;

	monsno = PPP_Get( ppp, ID_PARA_monsno_egg, NULL );

	switch( monsno ){
	case MONSNO_ANNOON:
	case MONSNO_DEOKISISU:
	case MONSNO_MINOMUTTI:
	case MONSNO_MINOMESU:
	case MONSNO_SIIUSI:
	case MONSNO_SIIDORUGO:
	case MONSNO_KIMAIRAN:
	case MONSNO_EURISU:
	case MONSNO_PURAZUMA:
		return PPP_Get( (POKEMON_PASO_PARAM*)ppp, ID_PARA_form_no, NULL );

	default:
		return 0;
	}
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * パレット番号取得
 *
 * @param	mons		ポケモン番号
 * @param	form		フォルム番号
 * @param	egg			タマゴフラグ(TRUE:タマゴ)
 *
 * @return	パレット番号
 */
//--------------------------------------------------------------------------------------------
const u8 POKEICON_GetPalNum( u32 mons, u32 form, BOOL egg )
{
	if( egg == 1 ){
		if( mons == MONSNO_MANAFI ){
			mons = POKEICON_GetTAMAGO_MNF;
		}else{
			mons = POKEICON_GetTAMAGO;
		}
	}else if( mons > MONSNO_END ){
		mons = 0;
	}else if( form != 0 ){
		if( mons == MONSNO_DEOKISISU ){
			mons = POKEICON_GetDEOKISISU + form - 1;
		}else if( mons == MONSNO_ANNOON ){
			mons = POKEICON_GetANNOON + form - 1;
		}else if( mons == MONSNO_MINOMUTTI ){
			mons = POKEICON_GetMINOMUTTI + form - 1;
		}else if( mons == MONSNO_MINOMESU ){
			mons = POKEICON_GetMINOMESU + form - 1;
		}else if( mons == MONSNO_SIIUSI ){
			mons = POKEICON_GetSIIUSI + form - 1;
		}else if( mons == MONSNO_SIIDORUGO ){
			mons = POKEICON_GetSIIDORUGO + form - 1;
		}else if( mons == MONSNO_KIMAIRAN ){
			mons = POKEICON_GetGIRATHINA + form - 1;
		}else if( mons == MONSNO_EURISU ){
			mons = POKEICON_GetSHEIMI + form - 1;
		}else if( mons == MONSNO_PURAZUMA ){
			mons = POKEICON_GetROTOMU + form - 1;
		}
	}
	return IconPalAtr[mons];
}

//--------------------------------------------------------------------------------------------
/**
 * パレット番号取得（POKEMON_PASO_PARAM版）
 *
 * @param	ppp		POKEMON_PASO_PARAM
 *
 * @return	パレット番号
 */
//--------------------------------------------------------------------------------------------
u8 POKEICON_GetPalNumGetByPPP( const POKEMON_PASO_PARAM * ppp )
{
	BOOL fast;
	u32  mons;
	u32  form;
	u32  egg;

	fast = PPP_FastModeOn( (POKEMON_PASO_PARAM *)ppp );

	form = POKEICON_GetCgxForm( ppp );
	mons = PPP_Get( (POKEMON_PASO_PARAM*)ppp, ID_PARA_monsno, NULL );
	egg  = PPP_Get( (POKEMON_PASO_PARAM*)ppp, ID_PARA_tamago_flag, NULL );

	PPP_FastModeOff( (POKEMON_PASO_PARAM *)ppp, fast );

	return POKEICON_GetPalNum( mons, form, egg );
}

//--------------------------------------------------------------------------------------------
/**
 * パレットのアーカイブインデックス取得
 *
 * @param	none
 *
 * @return	アーカイブインデックス
 */
//--------------------------------------------------------------------------------------------
u32 POKEICON_GetPalArcIndex(void)
{
	return NARC_poke_icon_poke_icon_NCLR;
}

//--------------------------------------------------------------------------------------------
/**
 * セルのアーカイブインデックス取得
 *
 * @param	none
 *
 * @return	アーカイブインデックス
 */
//--------------------------------------------------------------------------------------------
u32 POKEICON_GetCellArcIndex(void)
{
	return NARC_poke_icon_poke_icon01_NCER;
}

//--------------------------------------------------------------------------------------------
/**
 * セルのアーカイブインデックス取得（アニメ入り）
 *
 * @param	none
 *
 * @return	アーカイブインデックス
 */
//--------------------------------------------------------------------------------------------
u32 POKEICON_GetAnmCellArcIndex(void)
{
	return NARC_poke_icon_poke_icon_anm_NCER;
}

//--------------------------------------------------------------------------------------------
/**
 * セルのアーカイブインデックス取得（64k,アニメ入り）
 *
 * @param	none
 *
 * @return	アーカイブインデックス
 */
//--------------------------------------------------------------------------------------------
u32 POKEICON_Get64kCellArcIndex(void)
{
	return NARC_poke_icon_poke_icon_64k_NCER;
}

//--------------------------------------------------------------------------------------------
/**
 * セルアニメのアーカイブインデックス取得
 *
 * @param	none
 *
 * @return	アーカイブインデックス
 */
//--------------------------------------------------------------------------------------------
u32 POKEICON_GetCellAnmArcIndex(void)
{
	return NARC_poke_icon_poke_icon01_NANR;
}

//--------------------------------------------------------------------------------------------
/**
 * セルアニメのアーカイブインデックス取得（アニメ入り）
 *
 * @param	none
 *
 * @return	アーカイブインデックス
 */
//--------------------------------------------------------------------------------------------
u32 POKEICON_GetAnmCellAnmArcIndex(void)
{
	return NARC_poke_icon_poke_icon_anm_NANR;
}

//--------------------------------------------------------------------------------------------
/**
 * セルアニメのアーカイブインデックス取得（64k,アニメ入り）
 *
 * @param	none
 *
 * @return	アーカイブインデックス
 */
//--------------------------------------------------------------------------------------------
u32 POKEICON_Get64kCellAnmArcIndex(void)
{
	return NARC_poke_icon_poke_icon_64k_NANR;
}
