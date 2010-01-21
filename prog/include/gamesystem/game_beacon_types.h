//==============================================================================
/**
 * @file    game_beacon_types.h
 * @brief   すれ違いのビーコン情報
 * @author  matsuda
 * @date    2010.01.19(火)
 */
//==============================================================================
#pragma once

#include "gamesystem/game_beacon.h"


//==============================================================================
//  定数定義
//==============================================================================
///システムログ件数
#define GAMEBEACON_SYSTEM_LOG_MAX       (30)


//==============================================================================
//  構造体定義
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   詳細パラメータ
 *
 * フィールドビーコンに混ぜる為、外部公開しています。
 * 上記以外での直接参照は控えてください
 */
//--------------------------------------------------------------
typedef struct{
  u8 details_no;                 ///<詳細No(GAMEBEACON_DETAILS_NO_xxx)
  u8 padding;
  union{
    u16 battle_trainer;          ///<戦闘相手のトレーナーコード
    u16 battle_monsno;           ///<戦闘相手のポケモンNo
  };
}GAMEBEACON_INFO_DETAILS;

//--------------------------------------------------------------
/**
 * @brief   行動パラメータ
 *
 * フィールドビーコンに混ぜる為、外部公開しています。
 * 上記以外での直接参照は控えてください
 */
//--------------------------------------------------------------
typedef struct{
  u16 action_no;                            ///<行動No
  u16 padding;
  
  //action_noによってセットされるものが変わる
  union{
    u8 work[12];
    u16 itemno;                             ///<アイテム番号
    STRCODE nickname[BUFLEN_POKEMON_NAME];  ///<ポケモン名(ニックネーム)
  };
}GAMEBEACON_INFO_ACTION;

//==================================================================
/**
 * @brief   送受信されるビーコンパラメータ
 *
 * フィールドビーコンに混ぜる為、外部公開しています。
 * 上記以外での直接参照は控えてください
 */
//==================================================================
struct _GAMEBEACON_INFO{
  u32 version_bit;                          ///<受信可能なPM_VERSIONをbit指定
  u16 send_counter;                         ///<送信No(同じデータの無視判定に使用)
  u16 zone_id;                              ///<現在地
  u8 g_power_id;                            ///<発動しているGパワーID(GPOWER_ID_xxx)
  u32 trainer_id;                           ///<トレーナーID
  STRCODE name[PERSON_NAME_SIZE + EOM_SIZE];         ///<トレーナー名
  
  GXRgb favorite_color;                     ///<本体情報の色
  u8 trainer_view;                          ///<ユニオンルームでの見た目
  u8 sex;                                   ///<性別

  u16 thanks_recv_count;                    ///<お礼を受けた回数
  u16 suretigai_count;                      ///<すれ違い人数
  
  GAMEBEACON_INFO_DETAILS details;          ///<詳細情報
  GAMEBEACON_INFO_ACTION action;            ///<行動パラメータ
};

//==================================================================
/**
 * @brief   送受信されるビーコンパラメータのログテーブル
 *
 * フィールドビーコンに混ぜる為、外部公開しています。
 * 上記以外での直接参照は控えてください
 */
//==================================================================
struct _GAMEBEACON_INFO_TBL{
  struct _GAMEBEACON_INFO info[GAMEBEACON_INFO_TBL_MAX];
  u16 time[GAMEBEACON_INFO_TBL_MAX];    ///<受信日時(上位8bit：時(0〜23)、下位8bit：分(0〜59))
};
