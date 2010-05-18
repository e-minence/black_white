//============================================================================================
/**
 * @file		b_bag_anm.h
 * @brief		戦闘用バッグ画面 ボタン制御
 * @author	Hiroyuki Nakamura
 * @date		09.08.26
 */
//============================================================================================
#pragma	once


//============================================================================================
//	定数定義
//============================================================================================

// ボタン
enum {
	BBAG_BGWF_POCKET01 = 0,
	BBAG_BGWF_POCKET02,
	BBAG_BGWF_POCKET03,
	BBAG_BGWF_POCKET04,

	BBAG_BGWF_ITEM01,
	BBAG_BGWF_ITEM02,
	BBAG_BGWF_ITEM03,
	BBAG_BGWF_ITEM04,
	BBAG_BGWF_ITEM05,
	BBAG_BGWF_ITEM06,

	BBAG_BGWF_RETURN,
	BBAG_BGWF_LEFT,
	BBAG_BGWF_RIGHT,

	BBAG_BGWF_USE,

	BBAG_BGWF_MAX,
};


//============================================================================================
//	プロトタイプ宣言
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		ボタン作成
 *
 * @param		wk		戦闘バッグのワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BBAGANM_ButtonInit( BBAG_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		ボタン削除
 *
 * @param		wk		戦闘バッグのワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BBAGANM_ButtonExit( BBAG_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		ボタンBG初期化
 *
 * @param		wk		戦闘バッグのワーク
 * @param		page	ページ番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BBAGANM_PageButtonPut( BBAG_WORK * wk, u8 page );

//--------------------------------------------------------------------------------------------
/**
 * @brief		ボタンアニメ初期化
 *
 * @param		wk		戦闘バッグのワーク
 * @param		id		ボタンID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BBAGANM_ButtonAnmInit( BBAG_WORK * wk, u8 id );

//--------------------------------------------------------------------------------------------
/**
 * @brief		ボタンアニメメイン
 *
 * @param		wk		戦闘バッグのワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BBAGANM_ButtonAnmMain( BBAG_WORK * wk );
