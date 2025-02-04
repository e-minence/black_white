//==============================================================================
/**
 * @file    intrude_save.h
 * @brief   侵入セーブデータ
 * @author  matsuda
 * @date    2009.10.18(日)
 */
//==============================================================================
#pragma once

#include "gamesystem/gamedata_def.h"  //GAMEDATA
#include "field/intrude_common.h"
#include "buflen.h"
#include "field/gpower_id.h"
#include "savedata/save_control.h"
#include "savedata/playtime.h"


//==============================================================================
//  定数定義
//==============================================================================
enum {
  OCCUPY_ID_MAX = FIELD_COMM_MEMBER_MAX + 1,

  OCCUPY_ID_MINE = OCCUPY_ID_MAX - 1,   ///<自分自身の占拠情報ID
};

///占拠レベルの最大値
#define OCCUPY_LEVEL_MAX    (999)

///ミッションクリア状況
enum{
  MISSION_CLEAR_NONE,       ///<クリアしていない
  MISSION_CLEAR_WHITE,      ///<白でクリア達成
  MISSION_CLEAR_BLACK,      ///<黒でクリア達成
};

///Gパワー配布受信bitのワーク数
#define INTRUDE_SAVE_DISTRIBUTION_BIT_WORK_MAX  (2)

///パレス滞在時間アクセスモード
typedef enum{
  SOJOURN_TIME_PUSH,
  SOJOURN_TIME_CALC_SET,
  SOJOURN_TIME_CALC_GET,
}SOJOURN_TIME_MODE;

///ミッションクリアした回数最大値
#define INTRUDE_MISSION_CLEAR_COUNT_MAX   (99999)


//==============================================================================
//  構造体定義
//==============================================================================
///侵入セーブ構造体の不完全型
typedef struct _INTRUDE_SAVE_WORK INTRUDE_SAVE_WORK;

//--------------------------------------------------------------
//  占拠
//--------------------------------------------------------------
///ミッションリスト数
#define MISSION_LIST_MAX   (6)  //MISSION_TYPE_MAXと同じ値である必要があります

///ミッションリストステータス
typedef struct{
  u8 mission_no[MISSION_LIST_MAX];      ///<ミッション番号
  u8 mission_clear[MISSION_LIST_MAX];   ///<ミッションクリア状況(MISSION_CLEAR_xxx)
}MISSION_LIST_STATUS;

///占拠情報
typedef struct{
  MISSION_LIST_STATUS mlst;              ///<ミッションリストステータス
  u16 white_level;                       ///<白レベル
  u16 black_level;                       ///<黒レベル
}OCCUPY_INFO;

//--------------------------------------------------------------
//  隠しアイテム
//--------------------------------------------------------------
///侵入隠しアイテムセーブワーク
typedef struct{
  STRCODE name[PERSON_NAME_SIZE + EOM_SIZE];  ///<隠した人の名前
  u16 item;                                   ///<アイテム番号
  u8 tbl_no;                                  ///<隠されている場所を示すテーブル番号
  u8 padding;
}INTRUDE_SECRET_ITEM_SAVE;


//==============================================================================
//  
//==============================================================================
extern INTRUDE_SAVE_WORK * SaveData_GetIntrude( SAVE_CONTROL_WORK * p_sv);


//============================================================================================
//============================================================================================
//--------------------------------------------------------------
/**
 * @brief   OCCUPY_INFOへのポインタ取得(プレイヤーID指定)
 * @param   gamedata		GAMEDATAへのポインタ
 * @param   player_id   プレイヤーID
 * @retval  OCCUPY_INFOへのポインタ
 * @note
 * 実態はsrc/gamesystem/game_data.cにあるが、情報隠蔽を
 * 考慮してアクセス関数群を扱うヘッダに配置している
 */
//--------------------------------------------------------------
extern OCCUPY_INFO * GAMEDATA_GetOccupyInfo(GAMEDATA * gamedata, u32 player_id);
//--------------------------------------------------------------
/**
 * @brief   OCCUPY_INFOへのポインタ取得
 * @param   gamedata		GAMEDATAへのポインタ
 * @retval  OCCUPY_INFOへのポインタ
 * @note
 * 実態はsrc/gamesystem/game_data.cにあるが、情報隠蔽を
 * 考慮してBGM_INFO_SYSを扱うヘッダに配置している
 */
//--------------------------------------------------------------
extern OCCUPY_INFO * GAMEDATA_GetMyOccupyInfo(GAMEDATA * gamedata);

//==============================================================================
//  外部関数宣言
//==============================================================================
extern u32 IntrudeSave_GetWorkSize( void );
extern void IntrudeSave_WorkInit(void *work);

//--------------------------------------------------------------
//  占拠情報
//--------------------------------------------------------------
extern void OccupyInfo_WorkInit(OCCUPY_INFO *occupy);
extern u16 OccupyInfo_GetWhiteLevel(const OCCUPY_INFO *occupy);
extern u16 OccupyInfo_GetBlackLevel(const OCCUPY_INFO *occupy);
extern void SaveData_OccupyInfoUpdate(SAVE_CONTROL_WORK * sv, const OCCUPY_INFO *occupy);
extern void SaveData_OccupyInfoLoad(SAVE_CONTROL_WORK * sv, OCCUPY_INFO *dest_occupy);
extern void OccupyInfo_LevelUpWhite(OCCUPY_INFO *occupy, int add_level);
extern void OccupyInfo_LevelUpBlack(OCCUPY_INFO *occupy, int add_level);

//--------------------------------------------------------------
//  隠しアイテム
//--------------------------------------------------------------
extern void ISC_SAVE_SetItem(INTRUDE_SAVE_WORK *intsave, const INTRUDE_SECRET_ITEM_SAVE *src);

//--------------------------------------------------------------
//  Gパワー
//--------------------------------------------------------------
extern void ISC_SAVE_SetGPowerID(INTRUDE_SAVE_WORK *intsave, GPOWER_ID gpower_id);
extern GPOWER_ID ISC_SAVE_GetGPowerID(INTRUDE_SAVE_WORK *intsave);
extern void ISC_SAVE_SetDistributionGPower(INTRUDE_SAVE_WORK *intsave, GPOWER_ID gpower_id);
extern BOOL ISC_SAVE_GetDistributionGPower(INTRUDE_SAVE_WORK *intsave, GPOWER_ID gpower_id);
extern void ISC_SAVE_GetDistributionGPower_Array(INTRUDE_SAVE_WORK *intsave, u8 *dest_array, int array_num);

//--------------------------------------------------------------
//  パレス
//--------------------------------------------------------------
extern u32 ISC_SAVE_GetMissionClearCount(INTRUDE_SAVE_WORK *intsave);
extern u32 ISC_SAVE_IncMissionClearCount(INTRUDE_SAVE_WORK *intsave);
extern s64 ISC_SAVE_GetPalaceSojournTime(INTRUDE_SAVE_WORK *intsave);
extern void ISC_SAVE_SetPalaceSojournTime(INTRUDE_SAVE_WORK *intsave, s64 second);
extern s64 ISC_SAVE_PalaceSojournParam(INTRUDE_SAVE_WORK *intsave, PLAYTIME *playtime, SOJOURN_TIME_MODE mode);
