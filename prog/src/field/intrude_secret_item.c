//==============================================================================
/**
 * @file    intrude_secret_item.c
 * @brief   侵入隠しアイテム
 * @author  matsuda
 * @date    2010.02.01(月)
 */
//==============================================================================
#include <gflib.h>
#include "field/intrude_secret_item.h"
#include "fieldmap/zone_id.h"
#include "gamesystem/game_comm.h"
#include "field\intrude_common.h"
#include "field_comm/intrude_work.h"
#include "field/intrude_secret_item_def.h"


//==============================================================================
//  外部データ
//==============================================================================
#include "intrude_secret_item.cdat"


//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * 隠しアイテム配置テーブルに載っている座標かチェック
 *
 * @param   zone_id		ゾーンID
 * @param   grid_x		グリッド座標X
 * @param   grid_y		グリッド座標Y
 * @param   grid_z		グリッド座標Z
 *
 * @retval  int		    ヒットした場合：テーブル位置
 * @retval  int		    ヒットしなかった場合：ISC_SEARCH_NONE
 */
//==================================================================
int ISC_TblSearch(u16 zone_id, u16 grid_x, u16 grid_y, u16 grid_z)
{
  int i;
  const INTRUDE_SECRET_ITEM_POSDATA *postbl = IntrudeSecretItemPosDataTbl;
  
  for(i = 0; i < SECRET_ITEM_DATA_TBL_MAX; i++){
    if(postbl->zone_id == zone_id && postbl->grid_x == grid_x && postbl->grid_y == grid_y
        && postbl->grid_z == grid_z){
      return i;
    }
    postbl++;
  }
  return ISC_SEARCH_NONE;
}

