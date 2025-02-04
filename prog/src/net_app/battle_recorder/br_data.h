//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_data.h
 *	@brief  常駐データ
 *	@author	Toru=Nagihashi
 *	@date		2010.03.25
 *
 *	@note   バトルレコーダー覚書
 *	        ・セキュアフラグについて
 *	          GDS上のバトルレコーダーは吹っ飛ぶものが入っている場合がある。
 *	          そのため、最後まで正常に見終わったらセキュアフラグを立てて保存する。
 *	          GDS上の場合必ずカウンターでセーブするので、ここで吹っ飛んでも復帰可能。
 *	          （３０件落とせるので、見たものにはフラグを立てておき、
 *	          　どれを保存しても良いようにする）
 *
 *	        ・最後まで見ないで保存した場合、
 *	          ブラウザモード上で見るとき、事前にセーブしてから、録画再生へ行く
 *	          無事全て録画再生し終わったら、セキュアフラグを立ててセーブする
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include "br_inner.h"
#include "br_proc_sys.h"
//=============================================================================
/**
 *					定数宣言
*/
//=============================================================================

//=============================================================================
/**
 *					構造体宣言
*/
//=============================================================================
//-------------------------------------
///	SYS領域におくデータ
//=====================================
typedef struct 
{
  BR_OUTLINE_DATA           outline;            //検索結果データ
  BR_RECORD_DATA            record;             //レコードで必要なデータ
  BR_PROC_SYS_RECOVERY_DATA proc_recovery;      //プロセスの繋がり復帰データ
  u32                       record_mode;        //レコード復帰時のモード
  BR_SAVE_INFO              rec_saveinfo;       //自分の録画保存状況
  BR_BTN_SYS_RECOVERY_DATA  btn_recovery;       //ボタンの繋がり復帰データ
  BOOL                      is_recplay_finish;  //最後まで録画を見終わった
  BR_LIST_POS               rank_pos;           //ランクのカーソル位置
  s32                       rank_mode;          //ランクのモード（バトルを介すので常駐）本来はBR_BVRANK_MODEの値
} BR_DATA;
