//============================================================================================
/**
 * @file		box2_obj.h
 * @brief		ボックス画面 OBJ関連
 * @author	Hiroyuki Nakamura
 * @date		09.10.05
 */
//============================================================================================
#pragma	once


//============================================================================================
//	定数定義
//============================================================================================

// アニメID
enum {
	BOX2OBJ_ANM_TRAY_CURSOR = 0,	// トレイカーソル
	BOX2OBJ_ANM_L_ARROW_OFF,			// 左矢印（非選択）
	BOX2OBJ_ANM_L_ARROW_ON,				// 左矢印（選択）
	BOX2OBJ_ANM_R_ARROW_OFF,			// 右矢印（非選択）
	BOX2OBJ_ANM_R_ARROW_ON,				// 右矢印（選択）
	BOX2OBJ_ANM_TRAY_NAME,				// トレイ名
	BOX2OBJ_ANM_HAND_NORMAL,			// 手カーソル：通常
	BOX2OBJ_ANM_HAND_OPEN,				// 手カーソル：開く
	BOX2OBJ_ANM_HAND_CLOSE,				// 手カーソル：閉じる
	BOX2OBJ_ANM_HAND_SHADOW,			// 手カーソル：影
//	BOX2OBJ_ANM_TB_STATUS,				// タッチバーステータス
//	BOX2OBJ_ANM_TB_STATUS_ON,			// タッチバーステータス・ON
//	BOX2OBJ_ANM_TB_STATUS_OFF,		// タッチバーステータス・OFF
};

#define	BOX2OBJ_TRAYPOKE_PX		( 24 )		// トレイのポケモンアイコン表示開始Ｘ座標
#define	BOX2OBJ_TRAYPOKE_PY		( 48 )		// トレイのポケモンアイコン表示開始Ｙ座標
#define	BOX2OBJ_TRAYPOKE_SX		( 24 )		// トレイのポケモンアイコンＸサイズ
#define	BOX2OBJ_TRAYPOKE_SY		( 24 )		// トレイのポケモンアイコンＹサイズ

// ポケモンアイコンプライオィテイ切り替え定義
enum {
	BOX2OBJ_POKEICON_PRI_CHG_GET = 0,		// 取得
	BOX2OBJ_POKEICON_PRI_CHG_PUT,			// 配置
};

#define	BOX2OBJ_BLENDTYPE_TRAYPOKE	( 0x01 )	// トレイのポケモンアイコンに半透明設定
#define	BOX2OBJ_BLENDTYPE_PARTYPOKE	( 0x02 )	// 手持ちのポケモンアイコンに半透明設定
// アイテムを持っているポケモンに設定するフラグ
#define	BOX2OBJ_BLENDTYPE_ITEM		( 0x80 )	// トレイのポケモンアイコンに半透明設定
// トレイの持ち物を持っていないポケモンアイコンに半透明設定
#define	BOX2OBJ_BLENDTYPE_TRAYITEM	( BOX2OBJ_BLENDTYPE_ITEM | BOX2OBJ_BLENDTYPE_TRAYPOKE )
// 手持ちの持ち物を持っていないポケモンアイコンに半透明設定
#define	BOX2OBJ_BLENDTYPE_PARTYITEM	( BOX2OBJ_BLENDTYPE_ITEM | BOX2OBJ_BLENDTYPE_PARTYPOKE )

#define	BOX2OBJ_FNTOAM_BOXNAME_SX	( 12 )	// OAMフォント：ボックス名Ｘサイズ
#define	BOX2OBJ_FNTOAM_BOXNAME_SY	( 2 )		// OAMフォント：ボックス名Ｙサイズ
#define	BOX2OBJ_FNTOAM_BOXNUM_SX	( 2 )		// OAMフォント：格納数Ｘサイズ
#define	BOX2OBJ_FNTOAM_BOXNUM_SY	( 1 )		// OAMフォント：格納数Ｙサイズ

// タッチーバーに表示されているボタンの状態
enum {
	BOX2OBJ_TB_ICON_ON = 0,		// 表示
	BOX2OBJ_TB_ICON_OFF,			// 非表示
	BOX2OBJ_TB_ICON_PASSIVE,	// パッシブ
};


//============================================================================================
//	プロトタイプ宣言
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		セルアクター初期化
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_Init( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		セルアクター解放
 *
 * @param		appwk		ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_Exit( BOX2_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		セルアクターアニメメイン
 *
 * @param		appwk		ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_AnmMain( BOX2_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		セルアクターアニメ変更
 *
 * @param		appwk	ボックス画面アプリワーク
 * @param		id		OBJ ID
 * @param		anm		アニメ番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_AnmSet( BOX2_APP_WORK * appwk, u32 id, u32 anm );

extern void BOX2OBJ_AutoAnmSet( BOX2_APP_WORK * appwk, u32 id, u32 anm );

//--------------------------------------------------------------------------------------------
/**
 * @brief		セルアクターアニメ取得
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		id			OBJ ID
 *
 * @return	アニメ番号
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2OBJ_AnmGet( BOX2_APP_WORK * appwk, u32 id );

//--------------------------------------------------------------------------------------------
/**
 * @brief		セルアクターアニメ状態取得
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		id			OBJ ID
 *
 * @retval	"TRUE = アニメ中"
 * @retval	"FALSE = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern BOOL BOX2OBJ_AnmCheck( BOX2_APP_WORK * appwk, u32 id );

//--------------------------------------------------------------------------------------------
/**
 * @brief		セルアクター表示切り替え
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		id			OBJ ID
 * @param		flg			表示フラグ
 *
 * @return	none
 *
 * @li	flg = TRUE : 表示
 * @li	flg = FALSE : 非表示
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_Vanish( BOX2_APP_WORK * appwk, u32 id, BOOL flg );

//--------------------------------------------------------------------------------------------
/**
 * @brief		セルアクター表示チェック
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		id			OBJ ID
 *
 * @retval	"TRUE = 表示"
 * @retval	"FALSE = 非表示"
 */
//--------------------------------------------------------------------------------------------
extern BOOL BOX2OBJ_VanishCheck( BOX2_APP_WORK * appwk, u32 id );

//--------------------------------------------------------------------------------------------
/**
 * @brief		セルアクターBGプライオリティ変更
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		id			OBJ ID
 * @param		pri			プライオリティ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_BgPriChange( BOX2_APP_WORK * appwk, u32 id, int pri );

//--------------------------------------------------------------------------------------------
/**
 * @brief		セルアクターのOBJ同士のプライオリティ変更
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		id			OBJ ID
 * @param		pri			プライオリティ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_ObjPriChange( BOX2_APP_WORK * appwk, u32 id, int pri );

//--------------------------------------------------------------------------------------------
/**
 * @brief		セルアクター半透明設定
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		id			OBJ ID
 * @param		flg			ON/OFFフラグ
 *
 * @return	none
 *
 * @li	flg : TRUE = ON, FALSE = OFF
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_BlendModeSet( BOX2_APP_WORK * appwk, u32 id, BOOL flg );

//--------------------------------------------------------------------------------------------
/**
 * @brief		セルアクター座標設定
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		id			OBJ ID
 * @param		x				Ｘ座標
 * @param		y				Ｙ座標
 * @param		setsf		表示画面
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_SetPos( BOX2_APP_WORK * appwk, u32 id, s16 x, s16 y, u16 setsf );

//--------------------------------------------------------------------------------------------
/**
 * @brief		セルアクター座標取得
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		id			OBJ ID
 * @param		x				Ｘ座標
 * @param		y				Ｙ座標
 * @param		setsf		表示画面
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_GetPos( BOX2_APP_WORK * appwk, u32 id, s16 * x, s16 * y, u16 setsf );

//--------------------------------------------------------------------------------------------
/**
 * @brief		セルアクターパレット変更
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		id			OBJ ID
 * @param		pal			パレット番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_ChgPltt( BOX2_APP_WORK * appwk, u32 id, u32 pal );

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモンアイコン変更
 *
 * @param		syswk		ボックス画面システムワーク
 * @param		tray		トレイ番号
 * @param		pos			ポケモン位置
 * @param		id			OBJ ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_PokeIconChange( BOX2_SYS_WORK * syswk, u32 tray, u32 pos, u32 id );

//--------------------------------------------------------------------------------------------
/**
 * @brief		トレイのポケモンアイコンを変更
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_TrayPokeIconChange( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		手持ちのポケモンアイコンを変更
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_PartyPokeIconChange( BOX2_SYS_WORK * syswk );


//--------------------------------------------------------------------------------------------
/**
 * @brief		トレイアイコンのキャラデータ作成（個別）
 *
 * @param		syswk		ボックス画面システムワーク
 * @param		tray		トレイ番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_TrayIconCgxMake( BOX2_SYS_WORK * syswk, u32 tray );

//--------------------------------------------------------------------------------------------
/**
 * @brief		トレイアイコンのキャラデータ作成（全部）
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_TrayIconCgxMakeAll( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		トレイアイコンのキャラデータ切り替え
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_TrayIconChange( BOX2_SYS_WORK * syswk );


//--------------------------------------------------------------------------------------------
/**
 * 壁紙変更：壁紙ＯＢＪ切り替え
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2OBJ_WallPaperObjChange( BOX2_SYS_WORK * syswk );









//============================================================================================
//	ポケモンアイコン
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * ポケモンアイコンのキャラデータをワークに展開
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	tray	トレイ番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_PokeIconBufLoad( BOX2_SYS_WORK * syswk, u32 tray );

//--------------------------------------------------------------------------------------------
/**
 * ポケモンアイコンのデフォルト座標取得
 *
 * @param	pos		位置
 * @param	x		Ｘ座標格納場所
 * @param	y		Ｙ座標格納場所
 * @param	mode	ポケモンアイコン移動モード
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_PokeIconDefaultPosGet( u32 pos, s16 * x, s16 * y, u32 mode );

//--------------------------------------------------------------------------------------------
/**
 * ポケモンアイコンにデフォルト座標を設定
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	pos		位置
 * @param	id		OBJ ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_PokeIconDefaultPosSet( BOX2_SYS_WORK * syswk, u32 pos, u32 objID );

//--------------------------------------------------------------------------------------------
/**
 * 手持ちポケモンアイコンをフレームイン前の座標に移動（左）
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_PartyPokeIconFrmInSet( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * 手持ちポケモンアイコンをフレームイン前の座標に移動（右）
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_PartyPokeIconFrmInSetRight( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * 手持ちポケモンアイコンをフレームイン後の座標に移動
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_PartyPokeIconFrmSet( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * 手持ちポケモンアイコンをフレームイン後の座標に移動（右）
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_PartyPokeIconFrmSetRight( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * ポケモンアイコンのプライオリティを変更
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	pos		位置
 * @param	flg		取得か配置か
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_PokeIconPriChg( BOX2_APP_WORK * appwk, u32 pos, u32 flg );

extern void BOX2OBJ_PokeIconPriChg2( BOX2_APP_WORK * appwk, u32 icon_pos, u32 put_pos, u32 flg );

//--------------------------------------------------------------------------------------------
/**
 * ポケモンアイコンのプライオリティを変更（掴んだ手持ちをトレイに落とすとき）
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	dp		掴んだ位置
 * @param	mp		配置する位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_PokeIconPriChgDrop( BOX2_APP_WORK * appwk, u32 dp, u32 mp );

//--------------------------------------------------------------------------------------------
/**
 * トレイのポケモンアイコンスクロール処理
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	mv		移動量
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_TrayPokeIconScroll( BOX2_SYS_WORK * syswk, s8 mv );

//--------------------------------------------------------------------------------------------
/**
 * 手持ちのポケモンアイコンスクロール処理
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	mx		移動量Ｘ
 * @param	my		移動量Ｙ
 *
 * @return	none
 *
 *	※使用しているところ無し
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_PartyPokeIconScroll( BOX2_SYS_WORK * syswk, s8 mx, s8 my );

//--------------------------------------------------------------------------------------------
/**
 * 手持ちのポケモンアイコンスクロール処理
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_PartyPokeIconScroll2( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * ポケモンを逃がす：ポケモンアイコン動作初期化
 *
 * @param	wk		逃がすワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_PokeIconFreeStart( BOX2MAIN_POKEFREE_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * ポケモンを逃がす：ポケモンアイコン縮小処理
 *
 * @param	wk		逃がすワーク
 *
 * @retval	"TRUE = 処理中"
 * @retval	"FALSE = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern BOOL BOX2OBJ_PokeIconFreeMove( BOX2MAIN_POKEFREE_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * ポケモンを逃がす：ポケモンアイコン拡大処理
 *
 * @param	wk		逃がすワーク
 *
 * @retval	"TRUE = 処理中"
 * @retval	"FALSE = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern BOOL BOX2OBJ_PokeIconFreeErrorMove( BOX2MAIN_POKEFREE_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * ポケモンを逃がす：ポケモンアイコン終了設定
 *
 * @param	wk		逃がすワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_PokeIconFreeEnd( BOX2MAIN_POKEFREE_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * ポケモンを逃がす：ポケモンアイコンエラー終了設定
 *
 * @param	wk		逃がすワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_PokeIconFreeErrorEnd( BOX2MAIN_POKEFREE_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * ポケモンアイコンに半透明設定（全体）
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	type	アイコン位置
 * @param	flg		ON/OFFフラグ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_PokeIconBlendSetAll( BOX2_SYS_WORK * syswk, u32 type, BOOL flg );

//--------------------------------------------------------------------------------------------
/**
 * ポケモンアイコンに半透明設定
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	pos		アイコン位置
 * @param	flg		ON/OFFフラグ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_PokeIconBlendSet( BOX2_APP_WORK * appwk, u32 pos, BOOL flg );

//--------------------------------------------------------------------------------------------
/**
 * ポケモンアイコンに半透明設定（持ち物整理のトレイスクロール用）
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	pos		アイコン位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_PokeIconBlendSetItem( BOX2_SYS_WORK * syswk, u32 pos );

extern void BOX2OBJ_GetPokeIcon( BOX2_APP_WORK * appwk, u32 pos );
extern void BOX2OBJ_PutPokeIcon( BOX2_APP_WORK * appwk, u32 pos );

extern void BOX2OBJ_MovePokeIconHand( BOX2_SYS_WORK * syswk );


//============================================================================================
//	ポケモングラフィック
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * ポケモングラフィック切り替え
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	info	表示データ
 * @param	id		OBJ ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_PokeGraChange( BOX2_SYS_WORK * syswk, BOX2_POKEINFO_DATA * info, u32 id );


//============================================================================================
//	アイテムアイコン
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * アイテムグラフィック切り替え
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	item	アイテム番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_ItemIconChange( BOX2_APP_WORK * appwk, u16 item );

//--------------------------------------------------------------------------------------------
/**
 * アイテムグラフィック切り替え（サブ画面用）
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	item	アイテム番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_ItemIconChangeSub( BOX2_APP_WORK * appwk, u16 item );

//--------------------------------------------------------------------------------------------
/**
 * アイテムアイコン拡縮設定
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	flg		TRUE = 拡縮, FALSE = 通常
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_ItemIconAffineSet( BOX2_APP_WORK * appwk, BOOL flg );

//--------------------------------------------------------------------------------------------
/**
 * アイテムアイコン座標設定
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	x		Ｘ座標
 * @param	y		Ｙ座標
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_ItemIconPosSet( BOX2_APP_WORK * appwk, s16 x, s16 y );

//--------------------------------------------------------------------------------------------
/**
 * アイテムアイコン移動
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	mx		Ｘ座標移動量
 * @param	my		Ｙ座標移動量
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_ItemIconMove( BOX2_APP_WORK * appwk, s16 x, s16 y );

//--------------------------------------------------------------------------------------------
/**
 * 指定ポケモンアイコンの位置にアイテムアイコンをセット
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	poke	ポケモンアイコン位置
 * @param	mode	ポケモンアイコン表示モード
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_ItemIconPokePut( BOX2_APP_WORK * appwk, u32 poke, u32 mode );

//--------------------------------------------------------------------------------------------
/**
 * 指定ポケモンアイコンの位置にアイテムアイコンの掴んだ位置をセット
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	poke	ポケモンアイコン位置
 * @param	mode	ポケモンアイコン表示モード
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_ItemIconPokePutHand( BOX2_APP_WORK * appwk, u32 poke, u32 mode );

//--------------------------------------------------------------------------------------------
/**
 * アイテムアイコンのアウトラインカーソルを追加
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_ItemIconCursorAdd( BOX2_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * アイテムアイコンのアウトラインカーソルを表示
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_ItemIconCursorOn( BOX2_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * アイテムアイコンのアウトラインカーソル移動
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_ItemIconCursorMove( BOX2_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * アイテムアイコン配置（サブ画面用）
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_ItemIconPutSub( BOX2_APP_WORK * appwk );


//============================================================================================
//	タイプアイコン
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * タイプアイコン切り替え
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	info	表示データ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_TypeIconChange( BOX2_APP_WORK * appwk, BOX2_POKEINFO_DATA * info );


//============================================================================================
//	その他
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * 手カーソル表示切替
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	flg		ON = 表示、OFF = 非表示
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_HandCursorVanish( BOX2_APP_WORK * appwk, BOOL flg );

//--------------------------------------------------------------------------------------------
/**
 * 手カーソル移動
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	px		Ｘ座標
 * @param	py		Ｙ座標
 * @param	shadow	影表示フラグ ON = 表示、OFF = 非表示
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_HandCursorSet( BOX2_APP_WORK * appwk, s16 px, s16 py, BOOL shadow );

//--------------------------------------------------------------------------------------------
/**
 * アウトラインカーソル追加
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_PokeCursorAdd( BOX2_SYS_WORK * syswk );
extern void BOX2OBJ_PokeCursorAdd2( BOX2_SYS_WORK * syswk, u32 pos );

//--------------------------------------------------------------------------------------------
/**
 * アウトラインカーソル表示切り替え
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	flg		表示フラグ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_PokeCursorVanish( BOX2_SYS_WORK * syswk, BOOL flg );

//--------------------------------------------------------------------------------------------
/**
 * アウトラインカーソル移動
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2OBJ_PokeCursorMove( BOX2_SYS_WORK * syswk );
extern void BOX2OBJ_PokeCursorMove( BOX2_APP_WORK * appwk, u32 pos );

//--------------------------------------------------------------------------------------------
/**
 * トレイ切り替え矢印表示切替
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	flg		ON/OFFフラグ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_TrayMoveArrowVanish( BOX2_APP_WORK * appwk, BOOL flg );


//============================================================================================
//	壁紙変更関連
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * 壁紙変更：ＯＢＪ初期化
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2OBJ_WallPaperChgObjInit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * 壁紙変更：ＯＢＪスクロール
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	mv		移動量
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2OBJ_WallPaperChgFrmScroll( BOX2_APP_WORK * appwk, s16 mv );

//--------------------------------------------------------------------------------------------
/**
 * 壁紙変更：カーソルセット
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_WallPaperCursorSet( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * 壁紙変更：名前セット
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2OBJ_WallPaperNameSet( BOX2_SYS_WORK * syswk );


//============================================================================================
//	ボックス移動フレーム関連
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * ボックス移動：各ＯＢＪを初期化
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_BoxMoveObjInit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * ボックス移動：各ＯＢＪをスクロール
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	mv		移動量
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_BoxMoveFrmScroll( BOX2_SYS_WORK * syswk, s16 mv );

extern void BOX2OBJ_InitBoxMoveCursorPos( BOX2_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * ボックス移動：カーソルセット
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_BoxMoveCursorSet( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * ボックス移動：トレイ名関連ＯＢＪセット
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_BoxMoveNameSet( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * トレイアイコンの座標を取得
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	pos		トレイ表示位置
 * @param	x		Ｘ座標格納場所
 * @param	y		Ｙ座標格納場所
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_TrayIconPosGet( BOX2_APP_WORK * appwk, u32 pos, s16 * x, s16 * y );

extern void BOX2OBJ_InitTrayIconScroll( BOX2_SYS_WORK * syswk );
extern void BOX2OBJ_TrayIconCgxTransIdx( BOX2_SYS_WORK * syswk, u32 tray, u32 idx );
extern void BOX2OBJ_TrayIconCgxTransPos( BOX2_SYS_WORK * syswk, u32 tray, u32 pos );


//--------------------------------------------------------------------------------------------
/**
 * トレイアイコンのキャラデータ作成（個別）
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	tray	トレイ番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_TrayIconCgxMake( BOX2_SYS_WORK * syswk, u32 tray );

//--------------------------------------------------------------------------------------------
/**
 * トレイアイコンのキャラデータ作成（全部）
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_TrayIconCgxMakeAll( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * トレイアイコンのキャラデータ作成（全部）
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_TrayIconChange( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * トレイアイコンのキャラを転送
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	tray	トレイ番号
 *
 * @return	none
 *
 * @li	指定トレイとアイコンが一致しない場合は転送しない
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_TrayIconCgxTrans( BOX2_SYS_WORK * syswk, u32 tray );

extern void BOX2OBJ_TrayIconScroll( BOX2_SYS_WORK * syswk, s16 mv );

extern void BOX2OBJ_InitTrayCursorScroll( BOX2_SYS_WORK * syswk, s32 mv );
extern void BOX2OBJ_EndTrayCursorScroll( BOX2_SYS_WORK * syswk );
extern void BOX2OBJ_SetTrayNamePos( BOX2_SYS_WORK * syswk, u32 pos );
extern void BOX2OBJ_ChangeTrayName( BOX2_SYS_WORK * syswk, u32 pos, BOOL flg );


//============================================================================================
//	ポケモンを預けるボックス選択関連
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * 預ける：ＯＢＪ初期化（全体）
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_PokeOutBoxObjInit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * 預ける：関連ＯＢＪを非表示へ
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_PokeOutBoxObjVanish( BOX2_SYS_WORK * syswk );


//============================================================================================
//	ボックス名と数 ( OAM font )
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * ＯＡＭフォント初期化
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_FontOamInit( BOX2_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * ＯＡＭフォント削除
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_FontOamExit( BOX2_APP_WORK * appwk );

extern void BOX2OBJ_FontOamVanish( BOX2_APP_WORK * appwk, u32 idx, BOOL flg );
extern BOOL BOX2OBJ_CheckFontOamVanish( BOX2_APP_WORK * appwk, u32 idx );

extern void BOX2OBJ_SetBoxNamePos( BOX2_APP_WORK * appwk, u32 idx, u32 mv );
extern void BOX2OBJ_BoxNameScroll( BOX2_APP_WORK * appwk, s8 mv );

//--------------------------------------------------------------------------------------------
/**
 * ＯＡＭフォントにＢＭＰを再設定
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	win		ＢＭＰウィンドウ
 * @param	id		ＯＡＭフォントＩＤ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_FontOamResetBmp( BOX2_APP_WORK * appwk, GFL_BMPWIN * win, u32 id );

extern void BOX2OBJ_SetTouchBarButton( BOX2_SYS_WORK * syswk, u8 ret, u8 exit, u8 status );
extern void BOX2OBJ_VanishTouchBarButton( BOX2_SYS_WORK * syswk );
