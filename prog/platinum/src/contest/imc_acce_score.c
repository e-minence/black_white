//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		imc_acce_score.c
 *	@brief		アクセサリ得点計算システム
 *	@author		tomoya takahshi
 *	@data		2006.04.06
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "system.h"
#include "assert.h"
#include <string.h>

#include "include/system/arc_util.h"
#include "src/contest/data/contest_data.naix"

#define	__IMC_ACCE_SCORE_H_GLOBAL
#include "include/contest/imc_acce_score.h"

//-----------------------------------------------------------------------------
/**
 *					コーディング規約
 *		●関数名
 *				１文字目は大文字それ以降は小文字にする
 *		●変数名
 *				・変数共通
 *						constには c_ を付ける
 *						staticには s_ を付ける
 *						ポインタには p_ を付ける
 *						全て合わさると csp_ となる
 *				・グローバル変数
 *						１文字目は大文字
 *				・関数内変数
 *						小文字と”＿”と数字を使用する 関数の引数もこれと同じ
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------



//----------------------------------------------------------------------------
/**
 *	@brief	アクセサリ得点計算
 *
 *	@param	cp_data			セーブデータ
 *	@param	cp_thema_score	テーマ得点データ
 *
 *	@return	得点
 */
//-----------------------------------------------------------------------------
u32	IMC_ACCE_ScoreGet( const IMC_CONTEST_SAVEDATA* cp_data, const u8* cp_theme_score )
{
	u8 acce_no;
	u32 score;
	int i;
	
	// アクセサリデータをチェックする
	score = 0;

	for( i=0; i<IMC_SAVEDATA_CONTEST_ACCE_NUM; i++ ){

		// 得点計算部分
		// そのインデックスの配列にアクセサリが登録済みかチェック
		if( ImcSaveData_CheckContestAcceDataSet( cp_data, i ) == TRUE ){
			acce_no = ImcSaveData_GetContestAcceNo( cp_data, i );	// アクセサリナンバー取得
			score += cp_theme_score[ acce_no ];
		}
	}

	return score;
}

//----------------------------------------------------------------------------
/**
 *	@brief	テーマの得点データを取得
 *
 *	@param	heap	ヒープ
 *	@param	theme	テーマ
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
u8* IMC_ACCE_ScoreDataGet( u32 heap, u32 theme )
{
	return ArcUtil_Load( ARC_CONTEST_DATA, 
			NARC_contest_data_imc_acce_score00_bin + theme, FALSE, heap, ALLOC_TOP );	
}
