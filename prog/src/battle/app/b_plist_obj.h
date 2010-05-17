//============================================================================================
/**
 * @file	b_plist_obj.h
 * @brief	戦闘用ポケモンリスト画面OBJ処理
 * @author	Hiroyuki Nakamura
 * @date	05.02.07
 */
//============================================================================================
#pragma	once


//============================================================================================
//	プロトタイプ宣言
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		戦闘用ポケリストOBJ初期化
 *
 * @param		wk    ワーク
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void BattlePokeList_ObjInit( BPLIST_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		戦闘用ポケリストOBJ削除
 *
 * @param		wk    ワーク
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void BattlePokeList_ObjFree( BPLIST_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		ページごとにOBJをセット
 *
 * @param		wk    ワーク
 * @param		page  ページ
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void BattlePokeList_PageObjSet( BPLIST_WORK * wk, u32 page );

//--------------------------------------------------------------------------------------------
/**
 * @brief		技アイコンセット：戦闘 or コンテスト
 *
 * @param wk    ワーク
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void BattlePokelist_WazaTypeSet( BPLIST_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモンアイコンアニメ
 *
 * @param		wk    ワーク
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void BPL_PokeIconAnime( BPLIST_WORK * wk );


//============================================================================================
//  カーソル
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		瀕死入れ替え時のＯＢＪ動作
 *
 * @param		wk    ワーク
 * @param		num		ポケモン位置
 * @param		mv		移動値
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void BPLISTOBJ_MoveDeadChg( BPLIST_WORK * wk, u8 num, s8 mv );

//--------------------------------------------------------------------------------------------
/**
 * @brief		瀕死入れ替え時のＯＢＪ入れ替え
 *
 * @param		wk    ワーク
 * @param		num1	ポケモン位置１
 * @param		num2	ポケモン位置２
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void BPLISTOBJ_ChgDeadSel( BPLIST_WORK * wk, u8 num1, u8 num2 );
