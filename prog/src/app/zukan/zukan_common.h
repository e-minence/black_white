//============================================================================================
/**
 * @file		zukan_common.h
 * @brief		図鑑画面 共通処理
 * @author	Hiroyuki Nakamura
 * @date		09.12.15
 *
 *	モジュール名：ZKNCOMM
 */
//============================================================================================
#pragma	once

#include "savedata/zukan_savedata.h"
#include "search/zukan_search_engine.h"


//============================================================================================
//	定数定義
//============================================================================================

// 上下バーのスクロールスピード
#define ZKNCOMM_BAR_SPEED   (8)     // 毎フレームZKNCOMM_BAR_SPEEDだけ移動する
// フェードイン/アウトのスピード
#define ZKNCOMM_FADE_WAIT   (0)     // PaletteFadeReqWriteのwaitに渡せる値


// セルアクターデータ
typedef struct {
	GFL_CLWK_DATA	dat;

	u32	chrRes;
	u32	palRes;
	u32	celRes;

	u16	pal;
	u16	disp;

}ZKNCOMM_CLWK_DATA;

// リストデータ
enum {
	ZUKAN_LIST_MONS_NONE = 0,		// 未確認
	ZUKAN_LIST_MONS_SEE,				// 見た
	ZUKAN_LIST_MONS_GET,				// 捕獲した
};


extern u16 ZKNCOMM_MakeDefaultList( ZUKAN_SAVEDATA * sv, u16 ** list, HEAPID heapID );

extern BOOL ZKNCOMM_CheckLocalListNumberZero( ZUKAN_SAVEDATA * sv );

extern void ZKNCOMM_ResetSortData( const ZUKAN_SAVEDATA * sv, ZKNCOMM_LIST_SORT * sort );

extern void ZKNCOMM_SetFadeIn( HEAPID heapID );

extern void ZKNCOMM_SetFadeOut( HEAPID heapID );

extern void ZKNCOMM_ScrollBaseBG( u8 mainBG, u8 subBG, u32 * cnt );


extern GFL_CLWK * ZKNCOMM_CreateClact( GFL_CLUNIT * unit, const ZKNCOMM_CLWK_DATA * prm, HEAPID heapID );
