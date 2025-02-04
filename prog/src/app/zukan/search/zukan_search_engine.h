//============================================================================
/**
 *  @file   zukan_search_engine.h
 *  @brief  図鑑検索エンジン
 *  @author Koji Kawada
 *  @data   2010.02.05
 *  @note   
 *
 *  モジュール名：ZUKAN_SEARCH_ENGINE
 */
//============================================================================
#pragma once


// インクルード
#include <gflib.h>


// オーバーレイ
//FS_EXTERN_OVERLAY(zukan_search);


//=============================================================================
/**
*  定数定義
*/
//=============================================================================
#define	ZKNCOMM_LIST_SORT_NONE	( 0xff )

enum {
	// 図鑑モード
	ZKNCOMM_LIST_SORT_MODE_ZENKOKU = 0,
	ZKNCOMM_LIST_SORT_MODE_LOCAL,
	ZKNCOMM_LIST_SORT_MODE_MAX,

	// 並び
	ZKNCOMM_LIST_SORT_ROW_NUMBER = 0,
	ZKNCOMM_LIST_SORT_ROW_NAME,
	ZKNCOMM_LIST_SORT_ROW_WEIGHT_HI,
	ZKNCOMM_LIST_SORT_ROW_WEIGHT_LOW,
	ZKNCOMM_LIST_SORT_ROW_HEIGHT_HI,
	ZKNCOMM_LIST_SORT_ROW_HEIGHT_LOW,
	ZKNCOMM_LIST_SORT_ROW_MAX,

	// 名前
	// タイプ
	// 形
	/* 直値で。 */
};

// 分割検索エンジンの状態
typedef enum
{
  ZKN_SCH_EGN_DIV_STATE_CONTINUE,  // 検索継続中
  ZKN_SCH_EGN_DIV_STATE_FINISH,    // 検索完了
}
ZKN_SCH_EGN_DIV_STATE;


//=============================================================================
/**
*  構造体宣言
*/
//=============================================================================
typedef struct {
	u8	mode;				// 図鑑モード
	u8	row;				// 並び（６種）
	u8	name;				// 名前
	u8	type1;			// タイプ１（１７種）  // prog/include/poke_tool/poketype_def.h
	u8	type2;			// タイプ２（１７種）
  u8  color;      // 色（１０種）  // prog/include/poke_tool/poke_personal.h
	u8	form;				// 形（１４種）
}ZKNCOMM_LIST_SORT;

// 分割検索エンジンワーク
typedef struct  _ZKN_SCH_EGN_DIV_WORK  ZKN_SCH_EGN_DIV_WORK;


//=============================================================================
/**
*  関数のプロトタイプ宣言
*/
//=============================================================================
//-------------------------------------
/// 検索する
//=====================================
extern u16 ZUKAN_SEARCH_ENGINE_Search(
               const ZUKAN_SAVEDATA*      zkn_sv,
               const ZKNCOMM_LIST_SORT*   term,
               HEAPID                     heap_id,
               u16**                      list );

//-------------------------------------
/// 分割検索する(1フレームに少しずつ検索する)
//=====================================
extern  ZKN_SCH_EGN_DIV_WORK*  ZUKAN_SEARCH_ENGINE_DivInit(
               const ZUKAN_SAVEDATA*      zkn_sv,
               const ZKNCOMM_LIST_SORT*   term,      // 呼び出し元はこれを削除しないこと
               HEAPID                     heap_id
);
extern  void                   ZUKAN_SEARCH_ENGINE_DivExit(
               ZKN_SCH_EGN_DIV_WORK*      work
);
extern  ZKN_SCH_EGN_DIV_STATE  ZUKAN_SEARCH_ENGINE_DivSearch(  // 1フレームに1回呼び出す
               ZKN_SCH_EGN_DIV_WORK*      work,
               HEAPID                     heap_id,
               u16*                       num,      // 戻り値がZKN_SCH_EGN_DIV_STATE_FINISHになったら有効
               u16**                      list      // 戻り値がZKN_SCH_EGN_DIV_STATE_FINISHになったら有効
);

//-------------------------------------
/// 全国/地方図鑑の番号順リストを得る
//=====================================
extern u16 ZUKAN_GetNumberRow(
               u8       mode,  // ZKNCOMM_LIST_SORT_MODE_ZENKOKU / ZKNCOMM_LIST_SORT_MODE_LOCAL
               HEAPID   heap_id,
               u16**    list );

