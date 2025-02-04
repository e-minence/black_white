//==============================================================================
/**
 * @file    intrude_save_field.c
 * @brief   侵入のセーブデータ関連でフィールドでしかアクセスしないもの
 * @author  matsuda
 * @date    2010.02.01(月)
 *
 * FIELDMAPオーバーレイに配置
 */
//==============================================================================
#include <gflib.h>
#include "savedata/save_control.h"
#include "savedata/save_tbl.h"
#include "savedata/intrude_save.h"
#include "savedata/intrude_save_field.h"
#include "savedata/intrude_save_local.h"
#include "field/intrude_secret_item.h"


///ISC_SAVE_CheckItemでヒットしなかった場合の戻り値
#define ISC_SAVE_SEARCH_NONE    (0xff)


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
int ISC_SAVE_CheckItem(INTRUDE_SAVE_WORK *intsave, u16 zone_id, s16 grid_x, s16 grid_y, s16 grid_z)
{
  int i;
  const INTRUDE_SECRET_ITEM_SAVE *isis = intsave->secret_item;
  const INTRUDE_SECRET_ITEM_POSDATA *posdata;
  
  for(i = 0; i < INTRUDE_SECRET_ITEM_SAVE_MAX; i++){
    if(isis->item == 0){
      return ISC_SAVE_SEARCH_NONE;
    }
    posdata = &IntrudeSecretItemPosDataTbl[isis->tbl_no];
    if(posdata->zone_id == zone_id && posdata->grid_x == grid_x 
        && posdata->grid_z == grid_z
        && posdata->grid_y - 1 <= grid_y && grid_y <= posdata->grid_y + 1 ) {
      return i;
    }
    isis++;
  }
  return ISC_SAVE_SEARCH_NONE;
}

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
void ISC_SAVE_GetItem(INTRUDE_SAVE_WORK *intsave, int work_no, INTRUDE_SECRET_ITEM_SAVE *dest)
{
  int i;
  
  GF_ASSERT(work_no != ISC_SAVE_SEARCH_NONE);
  
  *dest = intsave->secret_item[work_no];
  GF_ASSERT(dest->item != 0);
  
  //前詰め
  for(i = work_no + 1; i < INTRUDE_SECRET_ITEM_SAVE_MAX; i++){
    intsave->secret_item[i - 1] = intsave->secret_item[i];
  }
  intsave->secret_item[INTRUDE_SECRET_ITEM_SAVE_MAX - 1].item = 0;
}

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
const INTRUDE_SECRET_ITEM_POSDATA * ISC_SAVE_GetItemPosData(INTRUDE_SAVE_WORK *intsave, int *work_no)
{
  const INTRUDE_SECRET_ITEM_POSDATA *posdata;
  
  for( ; *work_no < INTRUDE_SECRET_ITEM_SAVE_MAX; (*work_no)++){
    if(intsave->secret_item[*work_no].item != 0){
      posdata = &IntrudeSecretItemPosDataTbl[intsave->secret_item[*work_no].tbl_no];
      (*work_no)++; //次の検索は1つ先から進むようにインクリメント
      return posdata;
    }
  }
  return NULL;
}

