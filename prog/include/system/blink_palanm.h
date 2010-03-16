//============================================================================================
/**
 * @file		blink_palanm.h
 * @brief		点滅パレットアニメ処理
 * @author	Hiroyuki Nakamura
 * @date		10.01.18
 *
 *	モジュール名：BLINKPALANM
 *
 *	指定カラーから指定カラーへのパレットアニメです。
 *	カーソルアニメで使用しています。
 */
//============================================================================================
#pragma	once


//============================================================================================
//	定数定義
//============================================================================================

typedef struct _BLINK_PALANM_WORK	BLINKPALANM_WORK;		// 点滅パレットアニメワーク

#define	BLINKPALANM_MODE_MAIN_OBJ		( 0xfffe )	// メインＯＢＪパレット定義
#define	BLINKPALANM_MODE_SUB_OBJ		( 0xffff )	// サブＯＢＪパレット定義


//============================================================================================
//	プロトタイプ宣言
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * 点滅パレットアニメワーク作成
 *
 * @param		offs		アニメをするパレット位置（カラー単位）
 * @param		size		アニメをするパレットのサイズ（カラー単位）
 * @param		bgfrm		ＢＧフレーム
 * @param		hapID		ヒープＩＤ
 *
 * @return	アニメワーク
 *
 * @li	bgfrm = BLINKPALANM_MODE_MAIN_OBJ でメインOBJのパレットに転送します
 * @li	bgfrm = BLINKPALANM_MODE_SUB_OBJ でサブOBJのパレットに転送します
 */
//--------------------------------------------------------------------------------------------
extern BLINKPALANM_WORK * BLINKPALANM_Create( u16 offs, u16 size, u16 bgfrm, HEAPID heapID );

//--------------------------------------------------------------------------------------------
/**
 * パレット設定
 *
 * @param		wk				パレットアニメワーク
 * @param		arcID			アーカイブＩＤ
 * @param		datID			データＩＤ
 * @param		startPos	開始基準パレット位置（カラー単位）
 * @param		endPos		終了基準パレット位置（カラー単位）
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BLINKPALANM_SetPalBufferArc( BLINKPALANM_WORK * wk, ARCID arcID, ARCDATID datID, u32 startPos, u32 endPos );

//--------------------------------------------------------------------------------------------
/**
 * パレット設定（ハンドル指定）
 *
 * @param		wk				パレットアニメワーク
 * @param		ah				アークハンドル
 * @param		datID			データＩＤ
 * @param		startPos	開始基準パレット位置（カラー単位）
 * @param		endPos		終了基準パレット位置（カラー単位）
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BLINKPALANM_SetPalBufferArcHDL( BLINKPALANM_WORK * wk, ARCHANDLE * ah, ARCDATID datID, u32 startPos, u32 endPos );

//--------------------------------------------------------------------------------------------
/**
 * 点滅パレットアニメワーク削除
 *
 * @param		wk		パレットアニメワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BLINKPALANM_Exit( BLINKPALANM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * 点滅パレットアニメ動作メイン
 *
 * @param		wk		パレットアニメワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BLINKPALANM_Main( BLINKPALANM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * アニメ動作カウンターを初期化
 *
 * @param		wk		パレットアニメワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BLINKPALANM_InitAnimeCount( BLINKPALANM_WORK * wk );
