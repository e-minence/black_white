//============================================================================================
/**
 * @file  game_data.h
 * @brief ゲーム進行データ管理
 * @date  2008.11.04
 *
 */
//============================================================================================
#pragma once

//============================================================================================
//============================================================================================
#include "gamesystem/gamedata_def.h"

#include "gamesystem/playerwork.h"
#include "field/eventdata_system.h"
#include "field/location.h"
#include "field/rail_location.h"       //RAIL_LOCATION
#include "savedata/myitem_savedata.h"  //BAG_CURSOR MYITEM
#include "poke_tool/pokeparty.h"

#include "field/eventwork.h"
#include "field/field_sound_proc.h"
#include "sound/bgm_info.h"

#include "field/game_beacon_search.h"
#include "savedata/mystatus.h"

//#include "field/field_encount.h"     //ENCOUNT_WORK
#include "savedata/box_savedata.h"     //BOX_MANAGER
//#include "savedata/intrude_save.h"   //OCCUPY_INFO
#include "system/shortcut_data.h"      //SHORTCUT_ID
//#include "savedata/gimmickwork.h"
#include "field/pleasure_boat_ptr.h"
#include "savedata/wifilist.h"
#include "savedata/misc.h"
#include "savedata/wifi_negotiation.h"
#include "savedata/record.h"
#include "savedata/musical_save.h"     //MUSICAL_SAVE
#include "musical/musical_define.h"    //MUSICAL_SCRIPT_WORK
#include "gamesystem/comm_player_support.h"
#include "savedata/dreamworld_data.h"  //DREAMWORLD_SAVEDATA
#include "savedata/dendou_save.h"      // DENDOU_RECORD
#include "savedata/perapvoice.h"       // PERAPVOICE
#include "savedata/playtime.h"         // PLAYTIME

#include "field/bsubway_scr_proc.h"

#include "field/calender.h"
#include "savedata/un_savedata.h"

#include "field/trial_house_ptr.h"
#include "savedata/trainercard_data.h"

//============================================================================================
//============================================================================================

//============================================================================================
//  ゲームデータ本体の生成・破棄処理
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief GAMEDATAの生成処理
 * @param heapID    使用するヒープ指定ID
 * @return  GAMEDATA  生成したGAMEDATAへのポインタ
 */
//------------------------------------------------------------------
extern GAMEDATA * GAMEDATA_Create(HEAPID heapID);
//------------------------------------------------------------------
/**
 * @brief GAMEDATAの破棄処理
 * @param gamedata  破棄するGAMEDATAへのポインタ
 */
//------------------------------------------------------------------
extern void GAMEDATA_Delete(GAMEDATA * gamedata);

//============================================================================================
//    セーブ処理
//============================================================================================
//--------------------------------------------------------------
/**
 * @brief   ゲームデータが持つ情報を元にセーブを実行
 *
 * @param   gamedata    ゲームデータへのポインタ
 *
 * @retval  セーブ結果
 */
//--------------------------------------------------------------
extern SAVE_RESULT GAMEDATA_Save(GAMEDATA *gamedata);

//--------------------------------------------------------------
/**
 * @brief   ゲームデータが持つ情報を元に分割セーブを開始
 *
 * @param   gamedata    ゲームデータへのポインタ
 *
 * @retval  none
 */
//--------------------------------------------------------------
extern void GAMEDATA_SaveAsyncStart(GAMEDATA *gamedata);

//--------------------------------------------------------------
/**
 * @brief   ゲームデータが持つ情報を元に分割セーブを実行
 *
 * @param   gamedata    ゲームデータへのポインタ
 *
 * @retval  セーブ結果
 */
//--------------------------------------------------------------
extern SAVE_RESULT GAMEDATA_SaveAsyncMain(GAMEDATA *gamedata);

//--------------------------------------------------------------
/**
 * @brief   分割セーブをキャンセルする
 *
 * @param   gamedata    ゲームデータへのポインタ
 *
 * @retval  none
 */
//--------------------------------------------------------------
extern void GAMEDATA_SaveAsyncCancel(GAMEDATA *gamedata);

//--------------------------------------------------------------
/**
 * @brief   外部セーブの分割セーブ開始
 *
 * @param   gamedata    ゲームデータへのポインタ
 * @param   extra_id    外部セーブデータ番号
 *
 * @retval  none
 */
//--------------------------------------------------------------
extern void GAMEDATA_ExtraSaveAsyncStart(GAMEDATA *gamedata, SAVE_EXTRA_ID extra_id);

//--------------------------------------------------------------
/**
 * @brief   外部セーブの分割セーブを実行
 *
 * @param   gamedata    ゲームデータへのポインタ
 * @param   extra_id    外部セーブデータ番号
 *
 * @retval  セーブ結果
 */
//--------------------------------------------------------------
extern SAVE_RESULT GAMEDATA_ExtraSaveAsyncMain(GAMEDATA *gamedata, SAVE_EXTRA_ID extra_id);

//==================================================================
/**
 * セーブ実行中か調べる
 *
 * @param   gamedata    ゲームデータへのポインタ
 *
 * @retval  BOOL        TRUE:セーブ実行中　　FALSE:セーブしていない
 */
//==================================================================
extern BOOL GAMEDATA_GetIsSave(const GAMEDATA *gamedata);

//============================================================================================
//  SAVE_CONTROL_WORKへのアクセス
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief   セーブコントロールワークを取得する
 * @param   gamedata      GAMEDATAへのポインタ
 * @return  SAVE_CONTROL_WORK のポインタ
 */
//------------------------------------------------------------------
extern SAVE_CONTROL_WORK * GAMEDATA_GetSaveControlWork(GAMEDATA * gamedata);

//------------------------------------------------------------------
/**
 * @brief   セーブコントロールワークを取得する  CONST版
 * @param   gamedata      GAMEDATAへのポインタ
 * @return  SAVE_CONTROL_WORK のポインタ
 */
//------------------------------------------------------------------
extern const SAVE_CONTROL_WORK* GAMEDATA_GetSaveControlWorkConst(const GAMEDATA * gamedata);





//============================================================================================
//  EVENTDATA_SYSTEMへのアクセス
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief イベント起動データシステムへのポインタを取得する
 * @param gamedata      GAMEDATAへのポインタ
 * @return  EVENTDATA_SYSTEM  イベント起動データシステムへのポインタ
 */
//------------------------------------------------------------------
extern EVENTDATA_SYSTEM * GAMEDATA_GetEventData(GAMEDATA * gamedata);

//------------------------------------------------------------------
/**
 * @brief 開始位置の取得
 * @param gamedata      GAMEDATAへのポインタ
 * @return  LOCATION    開始位置情報へのポインタ
 */
//------------------------------------------------------------------
extern const LOCATION * GAMEDATA_GetStartLocation(const GAMEDATA * gamedata);
//------------------------------------------------------------------
/**
 * @brief 開始位置のセット
 * @param gamedata
 * @param loc
 */
//------------------------------------------------------------------
extern void GAMEDATA_SetStartLocation(GAMEDATA * gamedata, const LOCATION * loc);

//------------------------------------------------------------------
/**
 * @brief 直前の出入口情報の取得
 * @param gamedata      GAMEDATAへのポインタ
 * @return  LOCATION    直前の出入口情報へのポインタ
 */
//------------------------------------------------------------------
extern const LOCATION * GAMEDATA_GetEntranceLocation(const GAMEDATA * gamedata);
//------------------------------------------------------------------
/**
 * @brief 直前の出入口情報のセット
 * @param gamedata
 * @param loc
 */
//------------------------------------------------------------------
extern void GAMEDATA_SetEntranceLocation(GAMEDATA * gamedata, const LOCATION * loc);

//------------------------------------------------------------------
/**
 * @brief 特殊出入口情報の取得
 * @param gamedata    GAMEDATAへのポインタ
 * @return  LOCATION    特殊出入口情報へのポインタ
 */
//------------------------------------------------------------------
extern const LOCATION * GAMEDATA_GetSpecialLocation(const GAMEDATA * gamedata);
//------------------------------------------------------------------
/**
 * @brief 特殊出入口情報のセット
 * @param gamedata  GAMEDATAへのポインタ
 * @param loc     特殊出入口情報へのポインタ
 */
//------------------------------------------------------------------
extern void GAMEDATA_SetSpecialLocation(GAMEDATA * gamedata, const LOCATION * loc);

//--------------------------------------------------------------
//--------------------------------------------------------------
extern const LOCATION * GAMEDATA_GetEscapeLocation(const GAMEDATA * gamedata);
//--------------------------------------------------------------
//--------------------------------------------------------------
extern void GAMEDATA_SetEscapeLocation(GAMEDATA * gamedata, const LOCATION * loc);

//--------------------------------------------------------------
//--------------------------------------------------------------
extern const LOCATION * GAMEDATA_GetPalaceReturnLocation(const GAMEDATA * gamedata);
//--------------------------------------------------------------
//--------------------------------------------------------------
extern void GAMEDATA_SetPalaceReturnLocation(GAMEDATA * gamedata, const LOCATION * loc);

//--------------------------------------------------------------
//--------------------------------------------------------------
extern void GAMEDATA_SetWarpID(GAMEDATA * gamedata, u16 warp_id);
//--------------------------------------------------------------
//--------------------------------------------------------------
extern u16 GAMEDATA_GetWarpID(GAMEDATA * gamedata);

//--------------------------------------------------------------
/**
 * @brief   バッグカーソルのポインタ取得
 * @param   gamedata    GAMEDATAへのポインタ
 * @retval  バッグカーソルポインタ
 */
//--------------------------------------------------------------
extern BAG_CURSOR* GAMEDATA_GetBagCursor(const GAMEDATA * gamedata);
//--------------------------------------------------------------
/**
 * @brief   手持ちアイテムへのポインタ取得
 * @param   gamedata    GAMEDATAへのポインタ
 * @retval  手持ちアイテムへのポインタ
 */
//--------------------------------------------------------------
extern MYITEM_PTR GAMEDATA_GetMyItem(const GAMEDATA * gamedata);

//--------------------------------------------------------------
/**
 * @brief   手持ちポケモンへのポインタ取得
 * @param   gamedata    GAMEDATAへのポインタ
 * @retval  手持ちアイテムへのポインタ
 */
//--------------------------------------------------------------
extern POKEPARTY * GAMEDATA_GetMyPokemon(const GAMEDATA * gamedata);

//--------------------------------------------------------------
/**
 * @brief   ボックス管理構造体のポインタ取得
 * @param   gamedata    GAMEDATAへのポインタ
 * @retval  ボックス管理構造体へのポインタ
 */
//--------------------------------------------------------------
extern BOX_MANAGER * GAMEDATA_GetBoxManager(const GAMEDATA * gamedata);

//--------------------------------------------------------------
/**
 * @brief   MYSTATUSへのポインタ取得
 * @param   gamedata    GAMEDATAへのポインタ
 * @retval  MYSTATUSへのポインタ
 */
//--------------------------------------------------------------
extern MYSTATUS * GAMEDATA_GetMyStatus(GAMEDATA * gamedata);

//--------------------------------------------------------------
/**
 * @brief   MYSTATUSへのポインタ取得(プレイヤーID指定)
 * @param   gamedata    GAMEDATAへのポインタ
 * @param   player_id   プレイヤーID
 * @retval  MYSTATUSへのポインタ
 */
//--------------------------------------------------------------
extern MYSTATUS * GAMEDATA_GetMyStatusPlayer(GAMEDATA * gamedata, u32 player_id);

//--------------------------------------------------------------
/**
 * @brief カレンダーの取得
 *
 * @param  gamedata GAMEDATAへのポインタ
 * @return CALENDER
 */
//--------------------------------------------------------------
extern CALENDER* GAMEDATA_GetCalender( GAMEDATA * gamedata );

//--------------------------------------------------------------
/**
 * @brief 季節の取得
 * @param   gamedata    GAMEDATAへのポインタ
 * @return  u8  季節ID（gamesystem/pm_season.h参照）
 */
//--------------------------------------------------------------
extern u8 GAMEDATA_GetSeasonID(const GAMEDATA *gamedata);



//--------------------------------------------------------------
/**
 * @brief   フィールド下画面の取得
 * @param   gamedata    GAMEDATAへのポインタ
 * @return  subscreen_mode
 */
//--------------------------------------------------------------
extern u8 GAMEDATA_GetSubScreenMode(const GAMEDATA *gamedata);

//--------------------------------------------------------------
/**
 * @brief   フィールド下画面の設定
 * @param   gamedata  GAMEDATAへのポインタ
 * @param   subscreen_mode
 */
//--------------------------------------------------------------
extern void GAMEDATA_SetSubScreenMode(GAMEDATA *gamedata, u8 subscreen_mode);

//--------------------------------------------------------------
/**
 * @brief   フィールド下画面タイプの取得
 * @param   gamedata    GAMEDATAへのポインタ
 * @return  subscreen_type
 */
//--------------------------------------------------------------
extern const u8 GAMEDATA_GetSubScreenType(const GAMEDATA *gamedata);

//--------------------------------------------------------------
/**
 * @brief   フィールド下画面タイプの設定
 * @param   gamedata  GAMEDATAへのポインタ
 * @param   subscreen_type
 */
//--------------------------------------------------------------
extern void GAMEDATA_SetSubScreenType(GAMEDATA *gamedata, const u8 subscreen_type);

//------------------------------------------------------------------
/**
 * @brief   フレーム分割動作で動作する場合の許可フラグ設定
 * @param   gamedata  GAMEDATAへのポインタ
 * @param   bEnable 分割する場合TRUE
 */
//------------------------------------------------------------------
extern void GAMEDATA_SetFrameSpritEnable(GAMEDATA *gamedata,BOOL bEnable);

//------------------------------------------------------------------
/**
 * @brief   フレーム分割動作で動作する場合のカウンタリセット
 * @param   gamedata  GAMEDATAへのポインタ
 */
//------------------------------------------------------------------
extern void GAMEDATA_ResetFrameSpritCount(GAMEDATA *gamedata);

//------------------------------------------------------------------
/**
 * @brief   フレーム分割動作で動作する場合のフレーム
 * @param   gamedata  GAMEDATAへのポインタ
 */
//------------------------------------------------------------------
extern u8 GAMEDATA_GetAndAddFrameSpritCount(GAMEDATA *gamedata);

//------------------------------------------------------------------
/**
 * @brief   フレーム分割動作で動作しているかどうか
 * @param   gamedata  GAMEDATAへのポインタ
 * @return  動作している場合TRUE
 */
//------------------------------------------------------------------
extern BOOL GAMEDATA_IsFrameSpritMode(GAMEDATA *gamedata);


//--------------------------------------------------------------
/**
 * @brief EVENTWORKへのポインタ取得
 * @param gamedata  GAMEDATAへのポインタ
 * @retval  EVENTWORKへのポインタ
 */
//--------------------------------------------------------------
extern EVENTWORK * GAMEDATA_GetEventWork(GAMEDATA *gamedata);

//------------------------------------------------------------------
/**
 * @brief   フィールドマップ歩数カウント取得
 * @param   gamedata  GAMEDATAへのポインタ
 * @return  int 歩数カウント
 */
//------------------------------------------------------------------
extern int GAMEDATA_GetFieldMapWalkCount(GAMEDATA *gamedata);

//------------------------------------------------------------------
/**
 * @brief   フィールドマップ歩数カウントセット
 * @param   gamedata  GAMEDATAへのポインタ
 * @param   count セットするカウント
 * @return  nothing
 */
//------------------------------------------------------------------
extern void GAMEDATA_SetFieldMapWalkCount(GAMEDATA *gamedata,int count);

//--------------------------------------------------------------
/**
 * @brief FIELD_SOUND取得
 * @param   gamedata  GAMEDATAへのポインタ
 * @return  FIELD_SOUND*
 */
//--------------------------------------------------------------
extern FIELD_SOUND * GAMEDATA_GetFieldSound( GAMEDATA *gamedata );

//--------------------------------------------------------------
/**
 * @brief 最新の戦闘結果をセット
 * @param   gamedata  GAMEDATAへのポインタ
 * @param btl_result  戦闘結果（include/battle/battle.hのBtlResult）
 */
//--------------------------------------------------------------
extern void GAMEDATA_SetLastBattleResult( GAMEDATA * gamedata, u32 btl_result );
//--------------------------------------------------------------
/**
 * @brief 最新の戦闘結果を取得
 * @param   gamedata  GAMEDATAへのポインタ
 * @return  u32 最新の戦闘結果（include/battle/battle.hのBtlResult）
 */
//--------------------------------------------------------------
extern u32 GAMEDATA_GetLastBattleResult( const GAMEDATA * gamedata );

//==================================================================
/**
 * 侵入接続人数取得
 *
 * @param   gamedata    GAMEDATAへのポインタ
 *
 * @retval  int   接続人数
 */
//==================================================================
extern int GAMEDATA_GetIntrudeNum(const GAMEDATA *gamedata);

//==================================================================
/**
 * 侵入接続人数取得
 *
 * @param   gamedata    GAMEDATAへのポインタ
 *
 * @retval  int   接続人数
 */
//==================================================================
extern void GAMEDATA_SetIntrudeNum(GAMEDATA *gamedata, int member_num);

//==================================================================
/**
 * 侵入時の自分のNetIDを取得
 *
 * @param   gamedata    GAMEDATAへのポインタ
 *
 * @retval  int   自分のNetID
 */
//==================================================================
extern int GAMEDATA_GetIntrudeMyID(const GAMEDATA *gamedata);

//==================================================================
/**
 * 侵入時の自分のNetIDをセット
 *
 * @param   gamedata    GAMEDATAへのポインタ
 *
 * @retval  int   自分のNetID
 */
//==================================================================
extern void GAMEDATA_SetIntrudeMyID(GAMEDATA *gamedata, int intrude_my_id);

//==================================================================
/**
 * 裏フィールド侵入フラグを取得
 *
 * @param   gamedata    GAMEDATAへのポインタ
 *
 * @retval  u8  TRUE:裏フィールド浸入中　FALSE:表フィールド
 */
//==================================================================
extern int GAMEDATA_GetIntrudeReverseArea(const GAMEDATA *gamedata);

//==================================================================
/**
 * 裏フィールド侵入フラグをセット
 *
 * @param   gamedata    GAMEDATAへのポインタ
 * @param   reverse_flag  TRUE:裏フィールド侵入中
 */
//==================================================================
extern void GAMEDATA_SetIntrudeReverseArea(GAMEDATA *gamedata, u8 reverse_flag);

//==================================================================
/**
 * 侵入中のパレスエリア番号を取得
 *
 * @param   gamedata		
 *
 * @retval  int		パレスエリア番号(player_id)
 */
//==================================================================
extern int GAMEDATA_GetIntrudePalaceArea(const GAMEDATA *gamedata);

//==================================================================
/**
 * 侵入中のパレスエリア番号をセット
 *
 * @param   gamedata		
 * @param   palace_area		パレスエリア番号(player_id)
 */
//==================================================================
extern void GAMEDATA_SetIntrudePalaceArea(GAMEDATA *gamedata, int palace_area);

//==================================================================
/**
 * 侵入相手の季節を取得
 *
 * @param   gamedata		
 * @param   player_id		プレイヤーID
 *
 * @retval  u8		季節
 */
//==================================================================
extern u8 GAMEDATA_GetIntrudeSeasonID(const GAMEDATA *gamedata, int player_id);

//==================================================================
/**
 * 侵入相手の季節を設定
 *
 * @param   gamedata		
 * @param   player_id		プレイヤーID
 * @param   season      季節
 */
//==================================================================
extern void GAMEDATA_SetIntrudeSeasonID(GAMEDATA *gamedata, int player_id, u8 season);

//----------------------------------------------------------------------------
/**
 *  @brief  常時通信モードフラグ取得
 *
 *  @param  gamedata    GAMEDATAへのポインタ
 *  
 *  @return TRUE常時通信モードON FALSE常時通信モードOFF
 */
//-----------------------------------------------------------------------------
extern BOOL GAMEDATA_GetAlwaysNetFlag( const GAMEDATA *gamedata );

//----------------------------------------------------------------------------
/**
 *  @brief  常時通信モードフラグ設定
 *
 *  @param  gamedata    GAMEDATAへのポインタ
 *  @param  is_on       TRUEならば常時通信モードON FALSEならば常時通信モードOFF
 */
//-----------------------------------------------------------------------------
extern void GAMEDATA_SetAlwaysNetFlag( GAMEDATA *gamedata, BOOL is_on );

//----------------------------------------------------------------------------
/**
 *  @brief  Yボタン登録設定
 *
 *  @param  gamedata    GAMEDATAへのポインタ
 *  @param  shortcutID  Yボタン登録するID
 *  @param  is_on       TRUEならばYボタン登録 FALSEならば解除
 */
//----------------------------------------------------------------------------
extern void GAMEDATA_SetShortCut( GAMEDATA *gamedata, SHORTCUT_ID shortcutID, BOOL is_on );

//----------------------------------------------------------------------------
/**
 *  @brief  Yボタン登録取得
 *
 *  @param  gamedata    GAMEDATAへのポインタ
 *  @param  shortcutID  Yボタン登録するID
 *  @retval TRUEならばYBTNに登録してある  FALSEならば登録していない
 */
//----------------------------------------------------------------------------
extern BOOL GAMEDATA_GetShortCut( const GAMEDATA *gamedata, SHORTCUT_ID shortcutID );

//----------------------------------------------------------------------------
/**
 *  @brief  Yボタン用メニューのカーソルセット
 *
 *  @param  gamedata  ゲームデータ
 *  @param  cursor    カーソル
 */
//-----------------------------------------------------------------------------
extern void GAMEDATA_SetShortCutCursor( GAMEDATA *gamedata, SHORTCUT_CURSOR *cursor );

//----------------------------------------------------------------------------
/**
 *  @brief  Yボタン用メニューのカーソル取得
 *
 *  @param  gamedata  ゲームデータ
 *  @retval cursor    カーソル
 */
//-----------------------------------------------------------------------------
extern SHORTCUT_CURSOR * GAMEDATA_GetShortCutCursor( GAMEDATA *gamedata );

//----------------------------------------------------------
/**
 * @brief   遊覧船へのポインタ取得
 * @param   gamedata      GAMEDATAへのポインタ
 * @return  遊覧船ワークへのポインタ
 */
//----------------------------------------------------------
extern PL_BOAT_WORK_PTR *GAMEDATA_GetPlBoatWorkPtr(GAMEDATA * gamedata);

//----------------------------------------------------------------------------
/**
 *  @brief  GAMEDATAからのWIFIListの取得
 *  @param  gamedata      ゲームデータ
 *  @return WIFIListのポインタ
 */
//-----------------------------------------------------------------------------
extern WIFI_LIST * GAMEDATA_GetWiFiList(GAMEDATA * gamedata);

//----------------------------------------------------------
/**
 * @brief   バトルサブウェイワーク取得
 * @param   gamedata      GAMEDATAへのポインタ
 * @return  BSUBWAY_SCRWORK*
 */
//----------------------------------------------------------
extern BSUBWAY_SCRWORK * GAMEDATA_GetBSubwayScrWork(GAMEDATA * gamedata);

//----------------------------------------------------------
/**
 * @brief   MISCワーク取得
 * @param   gamedata      GAMEDATAへのポインタ
 * @return  MISC*
 */
//----------------------------------------------------------
extern MISC * GAMEDATA_GetMiscWork(GAMEDATA * gamedata);

//----------------------------------------------------------
/**
 * @brief   WIFI_NEGOTIATION_SAVEDATAワーク取得
 * @param   gamedata      GAMEDATAへのポインタ
 * @return  WIFI_NEGOTIATION_SAVEDATA*
 */
//----------------------------------------------------------
extern WIFI_NEGOTIATION_SAVEDATA * GAMEDATA_GetWifiNegotiation(GAMEDATA * gamedata);

//----------------------------------------------------------
/**
 * @brief RECORDワークポインタ取得
 * @param   gamedata    GAMEDATAへのポインタ
 * @retval  RECORD *    
 */
//----------------------------------------------------------
extern RECORD *GAMEDATA_GetRecordPtr(GAMEDATA * gamedata);


//----------------------------------------------------------
/**
 * @brief   バトルサブウェイワークセット
 * @param   gamedata      GAMEDATAへのポインタ
 * @return  nothing
 */
//----------------------------------------------------------
extern void GAMEDATA_SetBSubwayScrWork(GAMEDATA * gamedata, BSUBWAY_SCRWORK *bsw_scr );


//----------------------------------------------------------
/**
 * @brief   トレーナーカードランク取得（ソースはtrcard_sys.c)make
 * @param   gamedata    GAMEDATAへのポインタ
 * @return  int         ランク値
 */
//----------------------------------------------------------
extern int TRAINERCARD_GetCardRank( GAMEDATA *gameData );

//----------------------------------------------------------
/**
 * @brief 国連ワークポインタ取得
 * @param   gamedata    GAMEDATAへのポインタ
 * @retval  UNSV_WORK *    
 */
//----------------------------------------------------------
extern UNSV_WORK *GAMEDATA_GetUnsvWorkPtr(GAMEDATA * gamedata);

//----------------------------------------------------------
/**
 * @brief ミュージカルセーブポインタ取得
 * @param   gamedata    GAMEDATAへのポインタ
 * @retval  UNSV_WORK *    
 */
//----------------------------------------------------------
extern MUSICAL_SAVE *GAMEDATA_GetMusicalSavePtr(GAMEDATA * gamedata);

//----------------------------------------------------------
/**
 * @brief   トライアルハウスワークへのポインタ取得
 * @param   gamedata      GAMEDATAへのポインタ
 * @return  トライアルワークへのポインタ
 */
//----------------------------------------------------------
extern TRIAL_HOUSE_WORK_PTR *GAMEDATA_GetTrialHouseWorkPtr(GAMEDATA * gamedata);

//----------------------------------------------------------
/**
 * @brief   トレーナーカードデータへのポインタ取得
 * @param   gamedata      GAMEDATAへのポインタ
 * @return  トレーナーカードデータへのポインタ
 */
//----------------------------------------------------------
extern TR_CARD_SV_PTR GAMEDATA_GetTrainerCardPtr(GAMEDATA * gamedata);

//----------------------------------------------------------
/**
 * @brief   初クリアポケモンデータへのポインタ取得
 * @param   gamedata      GAMEDATAへのポインタ
 * @return  殿堂入りデータへのポインタ
 */
//----------------------------------------------------------
extern DENDOU_RECORD *GAMEDATA_GetDendouRecord(GAMEDATA * gamedata);


//----------------------------------------------------------
/**
 * @brief   ミュージカル用スクリプトワークセット
 * @param   gamedata      GAMEDATAへのポインタ
 * @return  nothing
 */
//----------------------------------------------------------
extern void GAMEDATA_SetMusicalScrWork(GAMEDATA * gamedata, MUSICAL_SCRIPT_WORK* musScrWork );

//----------------------------------------------------------
/**
 * @brief   ミュージカル用スクリプトワーク取得
 * @param   gamedata      GAMEDATAへのポインタ
 * @return  MUSICAL_SCRIPT_WORK*
 */
//----------------------------------------------------------
extern MUSICAL_SCRIPT_WORK* GAMEDATA_GetMusicalScrWork(GAMEDATA * gamedata);

//----------------------------------------------------------
/**
 * @brief   PDWのセーブ取得
 * @param   gamedata      GAMEDATAへのポインタ
 * @return  DREAMWORLD_SAVEDATA*
 */
//----------------------------------------------------------
extern DREAMWORLD_SAVEDATA* GAMEDATA_GetDreamWorkdSaveWork(GAMEDATA * gamedata);


//=============================================================================================
/**
 * @brief ペラップ録音データのポインタを取得
 *
 * @param   gamedata    GAMEDATAへのポインタ
 *
 * @retval  extern PERAPVOICE *   ペラップ録音データへのポインタ
 */
//=============================================================================================
extern PERAPVOICE *GAMEDATA_GetPerapVoice( GAMEDATA *gamedata );

//==================================================================
/**
 * シンボルマップの現在位置をセットする
 *
 * @param   gamedata    
 * @param   symbol_map_id   シンボルマップID
 */
//==================================================================
extern void GAMEDATA_SetSymbolMapID(GAMEDATA *gamedata, u8 symbol_map_id);

//==================================================================
/**
 * シンボルマップの現在位置を取得する
 *
 * @param   gamedata    
 *
 * @retval  u8    シンボルマップID
 */
//==================================================================
extern u8 GAMEDATA_GetSymbolMapID(const GAMEDATA *gamedata);


//==================================================================
/**
 * PLAYTIME カウント処理
 *
 * @param   gamedata    
 * @param   value       カウント値
 */
//==================================================================
extern void GAMEDATA_PlayTimeCountUp(GAMEDATA *gamedata, u32 value);


//==================================================================
/**
 * PLAYTIME 取得 
 *
 * @param   gamedata    
 *
 * @return PLAYTIME (GAMEDATA生成時にallocしたワーク)
 */
//==================================================================
extern PLAYTIME* GAMEDATA_GetPlayTimeWork(GAMEDATA *gamedata);



