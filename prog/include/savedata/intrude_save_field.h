//==============================================================================
/**
 * @file    intrude_save_field.h
 * @brief   侵入のセーブデータ関連でフィールドでしかアクセスしないもの
 * @author  matsuda
 * @date    2010.02.01(月)
 */
//==============================================================================
#pragma once

#include "savedata/intrude_save.h"
#include "field/intrude_secret_item.h"


//==============================================================================
//  定数定義
//==============================================================================
///ISC_SAVE_CheckItemでヒットしなかった場合の戻り値
#define ISC_SAVE_SEARCH_NONE    (0xff)



//==============================================================================
//  外部関数宣言
//==============================================================================
//==================================================================
/**
 * セーブワークに指定座標の位置に隠しアイテムがあるか調べる
 *
 * @param   intsave		
 * @param   zone_id		ゾーンID
 * @param   grid_x		グリッド座標X
 * @param   grid_y		グリッド座標Y
 * @param   grid_z		グリッド座標Z
 *
 * @retval  int		    ヒットした場合：ワーク位置
 * @retval  int       ヒットしなかった場合：ISC_SAVE_SEARCH_NONE
 */
//==================================================================
extern int ISC_SAVE_CheckItem(INTRUDE_SAVE_WORK *intsave, u16 zone_id, s16 grid_x, s16 grid_y, s16 grid_z);

//==================================================================
/**
 * 指定ワーク位置の隠しアイテムデータを取得します
 *
 * @param   intsave		
 * @param   work_no		ワーク位置(ISC_SAVE_CheckItemの戻り値)
 * @param   dest      アイテムデータ代入先へのポインタ
 *
 * この関数を使用すると、取得したデータを削除し、データの前詰め処理も実行されます
 */
//==================================================================
extern void ISC_SAVE_GetItem(INTRUDE_SAVE_WORK *intsave, int work_no, INTRUDE_SECRET_ITEM_SAVE *dest);

//==================================================================
/**
 * セーブデータにあるアイテムの配置座標データへのポインタを取得する
 *
 * @param   intsave		
 * @param   work_no		セーブデータの検索開始するワーク位置
 *
 * @retval  const INTRUDE_SECRET_ITEM_POSDATA *		配置座標へのポインタ(ヒットしなかった場合はNULL)
 *
 * ダウジングマシン等、あらかじめ隠しアイテムが配置されている座標一覧を
 * 取得しておきたい場合に使用する
 * 
 * 使用例)
 *    work_no = 0;
 *    while(1){
 *      posdata = ISC_SAVE_GetItemPosData(intsave, &work_no);
 *      if(posdata == NULL){
 *        break;  //セーブの終端まで探しきった
 *      }
 *      my_buffer[count++] = *posdata;  //座標データを自分のバッファにコピー
 *    }
 */
//==================================================================
extern const INTRUDE_SECRET_ITEM_POSDATA * ISC_SAVE_GetItemPosData(INTRUDE_SAVE_WORK *intsave, int *work_no);


