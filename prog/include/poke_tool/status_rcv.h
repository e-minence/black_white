//============================================================================================
/**
 * @file	status_rcv.h
 * @brief	アイテム使用時のポケモン回復処理
 * @author	Hiroyuki Nakamura
 * @date	05.12.02
 *        09.08.06 GSより移植
 */
//============================================================================================
#pragma once

//============================================================================================
//	定数定義
//============================================================================================


//============================================================================================
//	プロトタイプ宣言
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * 道具使用チェック処理 ( POKEMON_PARAM )
 *
 * @param	pp			回復するポケモンのデータ
 * @param	item		使用するアイテム
 * @param	pos			回復位置（技位置など）
 * @param	heap_id		ヒープID
 *
 * @retval	"TRUE = 使用可能"
 * @retval	"FALSE = 使用不可"
 */
//--------------------------------------------------------------------------------------------
extern u8 STATUS_RCV_RecoverCheck( POKEMON_PARAM * pp, u16 item, u16 pos, u32 heap_id );

//--------------------------------------------------------------------------------------------
/**
 * 道具使用チェック処理 ( POKEPARTY )
 *
 * @param	party		回復するポケモンのデータ
 * @param	item		使用するアイテム
 * @param	pos1		ポケモンデータの中の位置
 * @param	pos2		回復位置（技位置など）
 * @param	heap_id		ヒープID
 *
 * @retval	"TRUE = 使用可能"
 * @retval	"FALSE = 使用不可"
 */
//--------------------------------------------------------------------------------------------
extern u8 STATUS_RCV_PokeParty_RecoverCheck(
			POKEPARTY * party, u16 item, u8 pos1, u8 pos2, u32 heap_id );

//--------------------------------------------------------------------------------------------
/**
 * 回復処理 ( POKEMON_PARAM )
 *
 * @param	pp			回復するポケモンのデータ
 * @param	item		使用するアイテム
 * @param	pos			回復位置（技位置など）
 * @param	place		場所
 * @param	heap_id		ヒープID
 *
 * @retval	"TRUE = 使用可能"
 * @retval	"FALSE = 使用不可"
 */
//--------------------------------------------------------------------------------------------
extern u8 STATUS_RCV_Recover( POKEMON_PARAM * pp, u16 item, u16 pos, u16 place, u32 heap_id );

//--------------------------------------------------------------------------------------------
/**
 * 回復処理 ( POKEPARTY )
 *
 * @param	party		回復するポケモンのデータ
 * @param	item		使用するアイテム
 * @param	pos1		ポケモンデータの中の位置
 * @param	pos2		回復位置（技位置など）
 * @param	place		場所
 * @param	heap_id		ヒープID
 *
 * @retval	"TRUE = 使用可能"
 * @retval	"FALSE = 使用不可"
 */
//--------------------------------------------------------------------------------------------
extern u8 STATUS_RCV_PokeParty_Recover(
			POKEPARTY * party, u16 item, u8 pos1, u8 pos2, u16 place, u32 heap_id );


//--------------------------------------------------------------------------------------------
/**
 * 単体全回復
 */
//--------------------------------------------------------------------------------------------
extern void STATUS_RCV_PokeParam_RecoverAll(POKEMON_PARAM * pp);
//--------------------------------------------------------------------------------------------
/**
 * パーティー全回復
 */
//--------------------------------------------------------------------------------------------
extern void STATUS_RCV_PokeParty_RecoverAll(POKEPARTY * party);
