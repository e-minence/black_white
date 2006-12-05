//=============================================================================================
/**
 * @file	display.h                                                  
 * @brief	表示システム設定
 */
//=============================================================================================
#undef GLOBAL
#ifdef __DISPLAY_H_GLOBAL__
#define GLOBAL /*	*/
#else
#define GLOBAL extern
#endif

#include <nitro.h>
#include <nnsys.h>

//ＢＧ・ＯＢＪ表示用定義
#define	VISIBLE_OFF		(0)		// 非表示へ
#define	VISIBLE_ON		(1)		// 表示へ

//=============================================================================================
//	型宣言
//=============================================================================================
///ＶＲＡＭバンク設定構造体
typedef	struct	{
	GXVRamBG			main_bg;			//メイン2DエンジンのBGに割り当て 
	GXVRamBGExtPltt		main_bg_expltt;		//メイン2DエンジンのBG拡張パレットに割り当て
	GXVRamSubBG			sub_bg;				//サブ2DエンジンのBGに割り当て
	GXVRamSubBGExtPltt	sub_bg_expltt;		//サブ2DエンジンのBG拡張パレットに割り当て
	GXVRamOBJ			main_obj;			//メイン2DエンジンのOBJに割り当て
	GXVRamOBJExtPltt	main_obj_expltt;	//メイン2DエンジンのOBJ拡張パレットにに割り当て
	GXVRamSubOBJ		sub_obj;			//サブ2DエンジンのOBJに割り当て
	GXVRamSubOBJExtPltt	sub_obj_expltt;		//サブ2DエンジンのOBJ拡張パレットにに割り当て
	GXVRamTex			teximage;			//テクスチャイメージスロットに割り当て
	GXVRamTexPltt		texpltt;			//テクスチャパレットスロットに割り当て
}GFL_BG_DISPVRAM;

//--------------------------------------------------------------------------------------------
/**
 * ＶＲＡＭバンク全体設定
 *
 * @param	data	設定データ
 */
//--------------------------------------------------------------------------------------------
GLOBAL void	GFL_DISP_SetBank( const GFL_BG_DISPVRAM* dispvram );
//--------------------------------------------------------------------------------------------
/**
 * メイン画面の各面の表示コントロール初期化
 *
 * @param	
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_DISP_GX_VisibleControlInit( void );
GLOBAL void GFL_DISP_GXS_VisibleControlInit( void );
//--------------------------------------------------------------------------------------------
/**
 * メイン＆サブ画面の各面の表示コントロール
 *
 * @param	gxplanemask		設定面
 * @param	flg				コントロールフラグ
 *
 * @li	flg = VISIBLE_ON	: 表示
 * @li	flg = VISIBLE_OFF	: 非表示
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_DISP_GX_VisibleControl( u8 gxplanemask, u8 flg );
GLOBAL void GFL_DISP_GXS_VisibleControl( u8 gxplanemask, u8 flg );
//--------------------------------------------------------------------------------------------
/**
 * メイン＆サブ画面の各面の表示コントロール（復帰とか）
 *
 * @param	prm		表示パラメータ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_DISP_GX_VisibleControlDirect( int prm );
GLOBAL void GFL_DISP_GXS_VisibleControlDirect( int prm );

//--------------------------------------------------------------------------------------------
/**
 * 表示有効設定
 *
 * @param	none
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void	GFL_DISP_DispOn( void );
//--------------------------------------------------------------------------------------------
/**
 * 表示無効設定
 *
 * @param	none
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void	GFL_DISP_DispOff( void );
//--------------------------------------------------------------------------------------------
/**
 * 画面の表示サイド設定
 *
 * @param	value	上下どちらをメイン画面にするか（DISP_3D_TO_MAIN:上画面がメイン　DISP_3D_TO_SUB:下画面がメイン）
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_DISP_DispSelect( int value );

//--------------------------------------------------------------------------------------------
/**
 * メイン画面の表示状況取得
 *
 * @param	none
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL int GFL_DISP_MainVisibleGet(void);

//--------------------------------------------------------------------------------------------
/**
 * サブ画面の表示状況取得
 *
 * @param	none
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL int GFL_DISP_SubVisibleGet(void);


#undef GLOBAL
