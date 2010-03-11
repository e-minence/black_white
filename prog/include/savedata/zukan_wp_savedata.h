//============================================================================================
/**
 * @file		zukan_wp_savedata.h
 * @brief		図鑑外部セーブ処理
 * @author	Hiroyuki Nakamura
 * @date		10.02.08
 *
 *	モジュール名：ZUKAN_WP_SAVEDATA
 */
//============================================================================================
#pragma	once


//============================================================================================
//	定数定義
//============================================================================================

#define	ZUKANWP_SAVEDATA_CHAR_SIZE_X		( 32 )		// グラフィックのＸサイズ（キャラ単位）
#define	ZUKANWP_SAVEDATA_CHAR_SIZE_Y		( 24 )		// グラフィックのＹサイズ（キャラ単位）

// グラフィックのキャラデータサイズ
#define	ZUKANWP_SAVEDATA_CHAR_SIZE	( ZUKANWP_SAVEDATA_CHAR_SIZE_X*ZUKANWP_SAVEDATA_CHAR_SIZE_Y*0x20 )
// グラフィックのパレット数
#define	ZUKANWP_SAVEDATA_PAL_SIZE		( 256 )


typedef struct _ZUKANWP_SAVEDATA	ZUKANWP_SAVEDATA;


//============================================================================================
//	プロトタイプ宣言
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		セーブデータのサイズ取得
 *
 * @param		none
 *
 * @return	セーブデータのサイズ
 */
//--------------------------------------------------------------------------------------------
extern int ZUKANWP_SAVEDATA_GetWorkSize(void);

//--------------------------------------------------------------------------------------------
/**
 * @brief		セーブデータ初期化
 *
 * @param		sv		セーブデータ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZUKANWP_SAVEDATA_InitWork( ZUKANWP_SAVEDATA * sv );

//--------------------------------------------------------------------------------------------
/**
 * @brief		壁紙キャラ取得
 *
 * @param		sv		セーブデータ
 *
 * @return	キャラデータ
 */
//--------------------------------------------------------------------------------------------
extern u8 * ZUKANWP_SAVEDATA_GetCustomGraphicCharacter( ZUKANWP_SAVEDATA * sv );

//--------------------------------------------------------------------------------------------
/**
 * @brief		壁紙パレット取得
 *
 * @param		sv		セーブデータ
 *
 * @return	パレットデータ
 */
//--------------------------------------------------------------------------------------------
extern u16 * ZUKANWP_SAVEDATA_GetCustomGraphicPalette( ZUKANWP_SAVEDATA * sv );

//--------------------------------------------------------------------------------------------
/**
 * @brief		データ有無フラグ取得
 *
 * @param		sv		セーブデータ
 *
 * @retval	"TRUE = データあり"
 * @retval	"FALSE = データなし"
 */
//--------------------------------------------------------------------------------------------
extern BOOL ZUKANWP_SAVEDATA_GetDataCheckFlag( ZUKANWP_SAVEDATA * sv );

//--------------------------------------------------------------------------------------------
/**
 * @brief		データ有無フラグ設定
 *
 * @param		sv		セーブデータ
 *
 * @param	"TRUE = データあり" 	"FALSE = データなし"
 */
//--------------------------------------------------------------------------------------------
extern void ZUKANWP_SAVEDATA_SetDataCheckFlag( ZUKANWP_SAVEDATA * sv,BOOL flg );
