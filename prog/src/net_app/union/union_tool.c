//==============================================================================
/**
 * @file    union_tool.c
 * @brief   ユニオンルームで使用するツール類：オーバーレイに配置
 * @author  matsuda
 * @date    2009.07.25(土)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "field\field_player.h"
#include "field\fieldmap.h"
#include "union_types.h"
#include "union_tool.h"

//==============================================================================
//  構造体定義
//==============================================================================
///s16のポイント型
typedef struct{
  s16 x;
  s16 z;
}POINT_S16;

//==============================================================================
//  データ
//==============================================================================
//--------------------------------------------------------------
//  出口座標
//--------------------------------------------------------------
///1vs1の時の立ち位置座標
static const POINT_S16 UnionWayOutPos[] = {
  {152, 248},
  {168, 248},
  {184, 248},
};


//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * ユニオンの出口座標にいるかチェック
 *
 * @param   fieldWork		
 *
 * @retval  BOOL		TRUE:出口座標にいる
 */
//==================================================================
BOOL UnionTool_CheckWayOut(FIELD_MAIN_WORK *fieldWork)
{
  VecFx32 pos;
  FIELD_PLAYER * player = FIELDMAP_GetFieldPlayer(fieldWork);
  int i;
  
  FIELD_PLAYER_GetPos(player, &pos);
  pos.x >>= FX32_SHIFT;
  pos.z >>= FX32_SHIFT;

  for(i = 0; i < NELEMS(UnionWayOutPos); i++){
    if(pos.x == UnionWayOutPos[i].x && pos.z == UnionWayOutPos[i].z){
      return TRUE;
    }
  }
  return FALSE;
}

//==================================================================
/**
 * play_categoryをアピール番号に変換
 *
 * @param   situ		
 *
 * @retval  UNION_APPEAL		
 */
//==================================================================
UNION_APPEAL UnionTool_PlayCategory_to_AppealNo(UNION_PLAY_CATEGORY play_category)
{
  //遊んでいるゲームがあるなら、そちらを優先してアピールさせる
  switch(play_category){
  case UNION_PLAY_CATEGORY_PICTURE:        //お絵かき
    return UNION_APPEAL_PICTURE;
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_50:         //コロシアム
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE:       //コロシアム
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_STANDARD:   //コロシアム
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_50:         //コロシアム
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE:       //コロシアム
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_STANDARD:   //コロシアム
  case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI:      //コロシアム
    return UNION_APPEAL_BATTLE;
  case UNION_PLAY_CATEGORY_TRADE:          //ポケモン交換
    return UNION_APPEAL_TRADE;
  case UNION_PLAY_CATEGORY_GURUGURU:       //ぐるぐる交換
    return UNION_APPEAL_GURUGURU;
  case UNION_PLAY_CATEGORY_RECORD:         //レコードコーナー
    return UNION_APPEAL_RECORD;
  }
  
  return UNION_APPEAL_NULL;
}
