#pragma once
//#include <gflib/fade.h>
// フェードに指定する輝度
#define BRIGHTNESS_WHITE	(  16 )		// 真っ白
#define BRIGHTNESS_NORMAL	(   0 )		// 通常パレット反映状態
#define BRIGHTNESS_BLACK	( -16 )		// 真っ暗

#define	COMM_BRIGHTNESS_SYNC	( 6 )	// 基本の輝度変更Sync数


//アセンブラでincludeされている場合は、下の宣言を無視できるようにifndefで囲んである
//#ifndef	__ASM_NO_DEF_

// フェードにかかるLCDの設定
typedef enum{
	MASK_MAIN_DISPLAY	= 0x1,									// メイン
	MASK_SUB_DISPLAY	= 0x02,									// サブ
	MASK_DOUBLE_DISPLAY = MASK_MAIN_DISPLAY|MASK_SUB_DISPLAY,	// 両方
}BRIGHT_DISPMASK;

// フェードがかかるBG面の指定
typedef enum{
	PLANEMASK_NONE	= GX_BLEND_PLANEMASK_NONE,
	PLANEMASK_BG0	= GX_BLEND_PLANEMASK_BG0,
	PLANEMASK_BG1	= GX_BLEND_PLANEMASK_BG1,
	PLANEMASK_BG2	= GX_BLEND_PLANEMASK_BG2,
	PLANEMASK_BG3	= GX_BLEND_PLANEMASK_BG3,
	PLANEMASK_OBJ	= GX_BLEND_PLANEMASK_OBJ,
	PLANEMASK_BD	= GX_BLEND_PLANEMASK_BD,
	PLANEMASK_ALL	= GX_BLEND_PLANEMASK_BG0|GX_BLEND_PLANEMASK_BG1|GX_BLEND_PLANEMASK_BG2|GX_BLEND_PLANEMASK_BG3|GX_BLEND_PLANEMASK_OBJ|GX_BLEND_PLANEMASK_BD
}BRIGHT_PLANEMASK;

//輝度変更パラメータ
typedef struct BrightParam_tag{
	BRIGHT_PLANEMASK PlaneMask;
	BRIGHT_DISPMASK DisplayMask;
	u16 Sync;
	s16 Dst;
	s16 Src;
//	BOOL *IsFinished;
}BRIGHT_PARAM;
//==============================================================================
/**
 * 輝度変更フェードリクエスト
 *
 * @param   inSync			フェードするのにかかるSync数
 * @param   inDstBright		フェード終了時の輝度（上にdefineあり)	←  こっちが終了時
 * @param   inSrcBright		フェード開始時の輝度（上にdefineあり)
 * @param   inPlaneMask		フェードが反映されるBG面（上にenumあり)
 * @param   inDisplayMask	フェードが反映されるLCD（上にenumあり)
 * @param   outFnishFlg		ポインタを指定する事でフェード終了を検知できる
 *
 * @retval  なし
 */
//==============================================================================
extern  void ChangeBrightnessRequest(
				const u8 inSync, const s16 inDstBright, const s16 inSrcBright,
				const BRIGHT_PLANEMASK inPlaneMask, const BRIGHT_DISPMASK inDisplayMask );

//==============================================================================
/**
 * 輝度変更
 *
 * @param   inDstBright		輝度（上にdefineあり)
 * @param   inPlaneMask		フェードが反映されるBG面（上にenumあり)
 * @param   inDisplayMask	フェードが反映されるLCD（上にenumあり)
 *
 * @retval  なし
 */
//==============================================================================
extern  void SetBrightness(	const s16 inDstBright,
					const BRIGHT_PLANEMASK inPlaneMask,
					const BRIGHT_DISPMASK inDisplayMask);

//==============================================================================
/**
 * 輝度変更構造体初期化
 *
 * @param	なし
 *
 * @retval  なし
 */
//==============================================================================
static inline  void BrightnessChgInit(void) { /* dummy */ }

//==============================================================================
/**
 * 輝度変更構造体のリセット
 *
 * @param   inDisplayMask	フェードが反映されるLCD（上にenumあり)
 *
 * @retval  なし
 */
//==============================================================================
extern void BrightnessChgReset(const BRIGHT_DISPMASK inDisplayMask);

//==============================================================================
/**
 * 輝度変更メイン
 *
 * @param	なし
 *
 * @retval  なし
 */
//==============================================================================
static inline  void BrightnessChgMain(void) { /* dummy */ }

//==============================================================================
/**
 * 輝度変更終了終了チェック
 *
 * @param	flg 調べるＬＣＤＣ
 *
 * @retval  終了有無
 */
//==============================================================================

static inline  BOOL IsFinishedBrightnessChg(BRIGHT_DISPMASK flg)
{
	return GFL_FADE_CheckFade();
}

