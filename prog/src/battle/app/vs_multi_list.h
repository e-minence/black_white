//============================================================================================
/**
 * @file		vs_multi_list.h
 * @brief		マルチバトル開始前ポケモンリスト
 * @author	Hiroyuki Nakamura
 * @date		10.01.15
 *
 *	モジュール名：VS_MULTI_LIST
 */
//============================================================================================
#pragma	once

#include "poke_tool/pokeparty.h"


//============================================================================================
//	定数定義
//============================================================================================

// 自分の立ち位置
enum {
	VS_MULTI_LIST_POS_LEFT = 0,		// 左
	VS_MULTI_LIST_POS_RIGHT,			// 右
};

// 外部設定データ
typedef struct {
	POKEPARTY * myPP;			// 自分のポケモンデータ
	POKEPARTY * ptPP;			// 相方のポケモンデータ

	u32	pos;							// 自分の立ち位置
}VS_MULTI_LIST_PARAM;


//============================================================================================
//	グローバル
//============================================================================================

// PROCデータ
extern const GFL_PROC_DATA VS_MULTI_LIST_ProcData;
FS_EXTERN_OVERLAY(vs_multi_list);
