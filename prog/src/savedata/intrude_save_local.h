//==============================================================================
/**
 * @file    intrude_save_local.h
 * @brief   侵入セーブデータ：ローカルヘッダ
 * @author  matsuda
 * @date    2010.02.01(月)
 */
//==============================================================================
#pragma once

//==============================================================================
//  定数定義
//==============================================================================
///侵入隠しアイテム保持数
#define INTRUDE_SECRET_ITEM_SAVE_MAX    (20)


//==============================================================================
//  構造体定義
//==============================================================================
///侵入セーブワーク
struct _INTRUDE_SAVE_WORK{
  OCCUPY_INFO occupy;
  INTRUDE_SECRET_ITEM_SAVE secret_item[INTRUDE_SECRET_ITEM_SAVE_MAX]; ///<侵入隠しアイテム
  u8 gpower_id;           ///<自身が装備しているGパワーID(セットしていない場合はGPOWER_ID_NULL)
  u8 gpower_distribution_bit[INTRUDE_SAVE_DISTRIBUTION_BIT_WORK_MAX]; ///<Gパワー配布受信bit
  u8 padding;
  u32 clear_mission_count;       ///<ミッションクリア数
  s64 palace_sojourn_time;       ///<パレス滞在時間
};
