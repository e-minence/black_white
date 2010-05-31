//============================================================================================
/**
 * @file		box2_bgwfrm.h
 * @brief		ボックス画面 BGウィンドウフレーム関連
 * @author	Hiroyuki Nakamura
 * @date		09.10.07
 *
 *	モジュール名：BOX2BGWFRM
 */
//============================================================================================
#pragma	once


//============================================================================================
//	定数定義
//============================================================================================

// 手持ちポケモンフレームデータ
#define	BOX2BGWFRM_PARTYPOKE_LX	( 2 )
#define	BOX2BGWFRM_PARTYPOKE_PY	( 6 )
#define	BOX2BGWFRM_PARTYPOKE_SX	( 11 )		// 手持ちポケモンフレームＸサイズ
#define	BOX2BGWFRM_PARTYPOKE_SY	( 15 )		// 手持ちポケモンフレームＹサイズ


//============================================================================================
//	プロトタイプ宣言
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		ＢＧフレームワーク作成（全体）
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BGWFRM_Init( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		ＢＧフレームワーク解放
 *
 * @param		appwk		ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BGWFRM_Exit( BOX2_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		手持ちポケモンフレーム表示
 *
 * @param		wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BGWFRM_PartyPokeFramePut( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		手持ちポケモンフレーム表示（右）
 *
 * @param		wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BGWFRM_PartyPokeFramePutRight( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		手持ちポケモンフレーム初期位置設定（左）
 *
 * @param		wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BGWFRM_PartyPokeFrameInitPutLeft( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		手持ちポケモンフレーム初期位置設定（右）
 *
 * @param		wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BGWFRM_PartyPokeFrameInitPutRight( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		手持ちポケモンフレームインセット
 *
 * @param		wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BGWFRM_PartyPokeFrameInSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		手持ちポケモンフレームアウトセット
 *
 * @param		wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BGWFRM_PartyPokeFrameOutSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		手持ちポケモンフレーム右移動セット
 *
 * @param		wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BGWFRM_PartyPokeFrameRightMoveSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		手持ちポケモンフレーム左移動セット
 *
 * @param		wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BGWFRM_PartyPokeFrameLeftMoveSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		手持ちポケモンフレーム移動
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @retval	"TRUE = 移動中"
 * @retval	"FALSE = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern BOOL BOX2BGWFRM_PartyPokeFrameMove( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		手持ちポケモンフレームが右にあるか
 *
 * @param		wk		ＢＧウィンドウフレームワーク
 *
 * @retval	"TRUE = あり"
 * @retval	"FALSE = なし"
 */
//--------------------------------------------------------------------------------------------
extern BOOL BOX2BGWFRM_CheckPartyPokeFrameRight( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		手持ちポケモンフレームが左にあるか
 *
 * @param		wk		ＢＧウィンドウフレームワーク
 *
 * @retval	"TRUE = あり"
 * @retval	"FALSE = なし"
 */
//--------------------------------------------------------------------------------------------
extern BOOL BOX2BGWFRM_CheckPartyPokeFrameLeft( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		手持ちポケモンフレームが表示中か
 *
 * @param		wk		ＢＧウィンドウフレームワーク
 *
 * @retval	"TRUE = あり"
 * @retval	"FALSE = なし"
 *
 * @li	Ｘ座標は見ていない
 */
//--------------------------------------------------------------------------------------------
extern BOOL BOX2BGWFRM_CheckPartyPokeFrame( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		手持ちポケモンフレームが左にあるかポケモンメニューボタンを画面内に配置
 *
 * @param		wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BGWFRM_PokeMenuOpenPosSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモンメニューボタン非表示
 *
 * @param		wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BGWFRM_PokeMenuOff( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモンメニューボタン画面内への移動セット
 *
 * @param		wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BGWFRM_PokeMenuInSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモンメニューボタン画面外への移動セット
 *
 * @param		wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BGWFRM_PokeMenuOutSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモンメニュー移動チェック
 *
 * @param		wk		ＢＧウィンドウフレームワーク
 *
 * @retval	"TRUE = 移動中"
 * @retval	"FLASE = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern BOOL BOX2BGWFRM_PokeMenuMoveMain( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモンメニュー配置チェック
 *
 * @param		wk		ＢＧウィンドウフレームワーク
 *
 * @retval	"TRUE = 配置済み"
 * @retval	"FLASE = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern BOOL BOX2BGWFRM_PokeMenuPutCheck( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモンメニュー配置チェック（完全に開いた状態か）
 *
 * @param		wk		ＢＧウィンドウフレームワーク
 *
 * @retval	"TRUE = 配置済み"
 * @retval	"FLASE = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern BOOL BOX2BGWFRM_PokeMenuOpenPutCheck( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		タッチバー配置
 *
 * @param		wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BGWFRM_PutTouchBar( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		「てもちポケモン」ボタン配置
 *
 * @param		appwk		ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BGWFRM_TemochiButtonOn( BOX2_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		「てもちポケモン」ボタン非表示
 *
 * @param		appwk		ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BGWFRM_TemochiButtonOff( BOX2_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		「ボックスリスト」ボタン配置
 *
 * @param		appwk		ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BGWFRM_BoxListButtonOn( BOX2_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		「ボックスリスト」ボタン非表示
 *
 * @param		appwk		ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BGWFRM_BoxListButtonOff( BOX2_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		マーキングフレームインセット
 *
 * @param		wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BGWFRM_MarkingFrameInSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		マーキングフレームアウトセット
 *
 * @param		wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BGWFRM_MarkingFrameOutSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		マーキングフレーム移動
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @retval	"TRUE = 移動中"
 * @retval	"FALSE = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2BGWFRM_MarkingFrameMove( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		トレイ選択フレーム画面内への移動セット
 *
 * @param		wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BGWFRM_BoxMoveFrmInSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		トレイ選択フレーム画面外への移動セット
 *
 * @param		wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BGWFRM_BoxMoveFrmOutSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		トレイ選択フレームが表示されているか
 *
 * @param		wk		ＢＧウィンドウフレームワーク
 *
 * @retval	"TRUE = 表示されている"
 * @retval	"FALSE = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern BOOL BOX2BGWFRM_CheckBoxMoveFrm( BGWINFRM_WORK * wk );
