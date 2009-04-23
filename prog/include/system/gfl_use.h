//=============================================================================================
/**
 * @file	gfl_use.h
 * @author	GAME FREAK inc.
 * @date	2006.12.06
 * @brief	GFライブラリ呼び出し部分
 */
//=============================================================================================

#ifndef	__GFL_USE_H__
#define	__GFL_USE_H__

//gsまでのポケモンで使っていたgf_randはu16で返していたので、
//移植用に定義nagihashi
#define GFUSER_RAND_PAST_MAX (0xFFFF)

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//GFL関連初期化処理
extern void GFLUser_Init(void);

//GFL関連Main呼び出し
extern void GFLUser_Main(void);

//GFL関連描画処理呼び出し
extern void GFLUser_Display(void);

//GFL関連終了処理
extern void GFLUser_Exit(void);

//GFL関連HBlank割り込み処理
extern void GFLUser_HIntr(void);

//HBlank中TCBの登録処理を追加
extern GFL_TCB * GFUser_HIntr_CreateTCB(GFL_TCB_FUNC * func, void * work, u32 pri);

//GFL関連VBlank割り込み処理
extern void GFLUser_VIntr(void);

//VBlank中TCBの登録処理を追加
extern GFL_TCB * GFUser_VIntr_CreateTCB(GFL_TCB_FUNC * func, void * work, u32 pri);

//VBlank中TCBSYSの取得
extern	GFL_TCBSYS * GFUser_VIntr_GetTCBSYS( void );

//VBlankカウンター取得とリセット
extern void GFUser_VIntr_ResetVblankCounter( void );
extern int GFUser_VIntr_GetVblankCounter( void );

// 汎用乱数を得る
extern u32 GFUser_GetPublicRand(u32 max);

//	アーカイブIDでファイル名を取得
extern const char * GFUser_GetFileNameByArcID(ARCID arcID);

#endif	__GFL_USE_H__
