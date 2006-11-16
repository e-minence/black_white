//=============================================================================================
/**
 * @file	display.c                                                  
 * @brief	表示システム設定
 */
//=============================================================================================
#include "gflib.h"

#define	__DISPLAY_H_GLOBAL
#include "display.h"

//=============================================================================================
//	プロトタイプ宣言
//=============================================================================================
//=============================================================================================
//	ローカル変数宣言
//=============================================================================================
static	int				MainVisiblePlane;
static	int				SubVisiblePlane;

//=============================================================================================
//	設定関数
//=============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * ＶＲＡＭバンク全体設定
 *
 * @param	data	設定データ
 */
//--------------------------------------------------------------------------------------------
void	GF_DISP_SetBank( const GF_BGL_DISPVRAM* dispvram )
{
	// VRAM設定リセット
	GX_ResetBankForBG();			// メイン2DエンジンのBG
	GX_ResetBankForBGExtPltt();		// メイン2DエンジンのBG拡張パレット
	GX_ResetBankForSubBG();			// サブ2DエンジンのBG
	GX_ResetBankForSubBGExtPltt();	// サブ2DエンジンのBG拡張パレット
	GX_ResetBankForOBJ();			// メイン2DエンジンのOBJ
	GX_ResetBankForOBJExtPltt();	// メイン2DエンジンのOBJ拡張パレット
	GX_ResetBankForSubOBJ();		// サブ2DエンジンのOBJ
	GX_ResetBankForSubOBJExtPltt();	// サブ2DエンジンのOBJ拡張パレット
	GX_ResetBankForTex();			// テクスチャイメージ
	GX_ResetBankForTexPltt();		// テクスチャパレット

	GX_SetBankForBG( dispvram->main_bg );
	GX_SetBankForBGExtPltt( dispvram->main_bg_expltt );

	GX_SetBankForSubBG( dispvram->sub_bg );
	GX_SetBankForSubBGExtPltt( dispvram->sub_bg_expltt );

	GX_SetBankForOBJ( dispvram->main_obj );
	GX_SetBankForOBJExtPltt( dispvram->main_obj_expltt );

	GX_SetBankForSubOBJ( dispvram->sub_obj );
	GX_SetBankForSubOBJExtPltt( dispvram->sub_obj_expltt );

	GX_SetBankForTex( dispvram->teximage );
	GX_SetBankForTexPltt( dispvram->texpltt );
}

//--------------------------------------------------------------------------------------------
/**
 * メイン画面の各面の表示コントロール初期化
 *
 * @param	
 */
//--------------------------------------------------------------------------------------------
void GF_DISP_GX_VisibleControlInit( void )
{
	MainVisiblePlane = 0;
	//GX_SetVisiblePlane( MainVisiblePlane );
}

//--------------------------------------------------------------------------------------------
/**
 * メイン画面の各面の表示コントロール
 *
 * @param	gxplanemask		設定面
 * @param	flg				コントロールフラグ
 *
 * @li	flg = VISIBLE_ON	: 表示
 * @li	flg = VISIBLE_OFF	: 非表示
 */
//--------------------------------------------------------------------------------------------
void GF_DISP_GX_VisibleControl( u8 gxplanemask, u8 flg )
{
	if( flg == VISIBLE_ON ){
		if( MainVisiblePlane & gxplanemask ){ return; }
	}else{
		if( !(MainVisiblePlane & gxplanemask) ){ return; }
	}
	MainVisiblePlane ^= gxplanemask;
	GX_SetVisiblePlane( MainVisiblePlane );
}

//--------------------------------------------------------------------------------------------
/**
 * メイン画面の各面の表示コントロール（復帰とか）
 *
 * @param	prm		表示パラメータ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void GF_DISP_GX_VisibleControlDirect( int prm )
{
	MainVisiblePlane = prm;
	GX_SetVisiblePlane( MainVisiblePlane );
}

//--------------------------------------------------------------------------------------------
/**
 * サブ画面の各面の表示コントロール初期化
 *
 * @param	
 */
//--------------------------------------------------------------------------------------------
void GF_DISP_GXS_VisibleControlInit( void )
{
	SubVisiblePlane = 0;
	//GXS_SetVisiblePlane( MainVisiblePlane );
}

//--------------------------------------------------------------------------------------------
/**
 * サブ画面の各面の表示コントロール
 *
 * @param	gxplanemask		設定面
 * @param	flg				コントロールフラグ
 *
 * @li	flg = VISIBLE_ON	: 表示
 * @li	flg = VISIBLE_OFF	: 非表示
 */
//--------------------------------------------------------------------------------------------
void GF_DISP_GXS_VisibleControl( u8 gxplanemask, u8 flg )
{
	if( flg == VISIBLE_ON ){
		if( SubVisiblePlane & gxplanemask ){ return; }
	}else{
		if( !(SubVisiblePlane & gxplanemask) ){ return; }
	}
	SubVisiblePlane ^= gxplanemask;
	GXS_SetVisiblePlane( SubVisiblePlane );
}

//--------------------------------------------------------------------------------------------
/**
 * サブ画面の各面の表示コントロール（復帰とか）
 *
 * @param	prm		表示パラメータ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void GF_DISP_GXS_VisibleControlDirect( int prm )
{
	SubVisiblePlane = prm;
	GXS_SetVisiblePlane( SubVisiblePlane );
}

//--------------------------------------------------------------------------------------------
/**
 * 表示有効設定
 *
 * @param	none
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void	GF_DISP_DispOn( void )
{
	GX_DispOn();
	GXS_DispOn();
}

//--------------------------------------------------------------------------------------------
/**
 * 表示無効設定
 *
 * @param	none
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void	GF_DISP_DispOff( void )
{
	GX_DispOff();
	GXS_DispOff();
}


//--------------------------------------------------------------------------------------------
/**
 * 画面の表示サイド設定
 *
 * @param	value	上下どちらをメイン画面にするか（DISP_3D_TO_MAIN:上画面がメイン　DISP_3D_TO_SUB:下画面がメイン）
 */
//--------------------------------------------------------------------------------------------
void GF_DISP_DispSelect( int value )
{
	GX_SetDispSelect(value);
}


//--------------------------------------------------------------------------------------------
/**
 * メイン画面の表示状況取得
 *
 * @param	none
 * @return	none
 */
//--------------------------------------------------------------------------------------------
int GF_DISP_MainVisibleGet(void)
{
	return MainVisiblePlane;
}

//--------------------------------------------------------------------------------------------
/**
 * サブ画面の表示状況取得
 *
 * @param	none
 * @return	none
 */
//--------------------------------------------------------------------------------------------
int GF_DISP_SubVisibleGet(void)
{
	return SubVisiblePlane;
}
