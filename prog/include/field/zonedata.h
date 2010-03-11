//============================================================================================
/**
 * @file	zonedata.h
 * @brief	ゾーン毎データへのアクセス
 * @author	tamada GAME FREAK inc.
 * @date	2008.11.12
 */
//============================================================================================
#pragma once


//------------------------------------------------------------------
/**
 * @brief	ゾーン名データの最大長（デバッグ用）
 */
//------------------------------------------------------------------
#define	ZONEDATA_NAME_LENGTH	(16)

enum {
  ZONEDATA_NO_RAILDATA_ID = -1,
  ZONEDATA_NO_CAMERA_AREA_ID = 0xffff,
  ZONEDATA_NO_SCENEAREA_ID = 0xffff,
};

//------------------------------------------------------------------
/**
 * @brief	ゾーンID変更テーブル
 *  定義を増やす手順
 *  １．下のZONEDATA_CHANGE_IDに定義を追加
 *  ２．src/field/zonedata.cのZONEDATA_CHANGE_ID_TBLに変更ZONEIDを指定
 */
//------------------------------------------------------------------
typedef enum
{
  ZONEDATA_CHANGE_BC_WF_ID, // BCをWFに変更する。
  ZONEDATA_CHANGE_PLC10_PLCW10_ID, // PalaceのBCをWFに変更する。

  ZONEDATA_CHANGE_MAX,
} ZONEDATA_CHANGE_ID;

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
// アーカイブハンドル・オープン
//------------------------------------------------------------------
extern void ZONEDATA_Open( HEAPID heap_id );

//------------------------------------------------------------------
// アーカイブハンドル・クローズ
//------------------------------------------------------------------
extern void ZONEDATA_Close( void );



//------------------------------------------------------------------
// ゾーンID変更設定
//------------------------------------------------------------------
extern void ZONEDATA_SetChangeZoneID( ZONEDATA_CHANGE_ID id, BOOL flag );
extern BOOL ZONEDATA_GetChangeZoneID( ZONEDATA_CHANGE_ID id );

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
// 	ゾーン名データの取得
//------------------------------------------------------------------
extern void ZONEDATA_DEBUG_GetZoneName(char * buffer, u16 zoneid);
//------------------------------------------------------------------
//  ゾーン名データを全部読み込み
//------------------------------------------------------------------
extern const char * ZONEDATA_GetAllZoneName(HEAPID heapID);

//------------------------------------------------------------------
//	ゾーンの最大数の取得
//------------------------------------------------------------------
extern u16 ZONEDATA_GetZoneIDMax(void);

//------------------------------------------------------------------
//	マトリックス指定の取得
//------------------------------------------------------------------
extern u16 ZONEDATA_GetMatrixID(u16 zone_id);

//------------------------------------------------------------------
//	エリア指定の取得
//------------------------------------------------------------------
extern u16 ZONEDATA_GetAreaID(u16 zone_id);

//------------------------------------------------------------------
//	マップ構造指定の取得
//------------------------------------------------------------------
extern u16 ZONEDATA_GetMapRscID(u16 zone_id);

//------------------------------------------------------------------
// スクリプト指定IDの取得
//------------------------------------------------------------------
extern u16 ZONEDATA_GetScriptArcID(u16 zone_id);

//------------------------------------------------------------------
// 特殊スクリプト指定IDの取得
//------------------------------------------------------------------
extern u16 ZONEDATA_GetSpScriptArcID(u16 zone_id);

//------------------------------------------------------------------
// フィールドメッセージ指定IDの取得
//------------------------------------------------------------------
extern u16 ZONEDATA_GetMessageArcID(u16 zone_id);

//------------------------------------------------------------------
//	マップ開始位置の取得
//------------------------------------------------------------------
extern void ZONEDATA_GetStartPos(u16 zone_id, VecFx32 * pos);


//------------------------------------------------------------------
//	マップ開始レール位置の取得
//------------------------------------------------------------------
extern void ZONEDATA_GetStartRailPos(u16 zone_id, VecFx32 * pos);

//------------------------------------------------------------------
//	カメラ指定IDの取得
//------------------------------------------------------------------
extern u8 ZONEDATA_GetCameraID(u16 zone_id);

//------------------------------------------------------------------
//	カメラエリア指定IDの取得
//------------------------------------------------------------------
extern u16 ZONEDATA_GetCameraAreaID(u16 zone_id);

//------------------------------------------------------------------
//	BGM指定IDの取得
//------------------------------------------------------------------
extern u16 ZONEDATA_GetBGMID(u16 zone_id, u8 season_id);

//------------------------------------------------------------------
//  天候指定IDの取得
//------------------------------------------------------------------
extern u16 ZONEDATA_GetWeatherID(u16 zone_id);

//------------------------------------------------------------------
/// 自転車に乗れるマップかどうか
//------------------------------------------------------------------
extern BOOL ZONEDATA_BicycleEnable( u16 zone_id );

//------------------------------------------------------------------
/// 自転車に乗った場合, BGMを変更するかどうか
//------------------------------------------------------------------
extern BOOL ZONEDATA_BicycleBGMEnable( u16 zone_id );

//------------------------------------------------------------------
/// ダッシュできるマップかどうか
//------------------------------------------------------------------
extern BOOL ZONEDATA_DashEnable( u16 zone_id );

//------------------------------------------------------------------
/// 「そらをとぶ」等が使えるマップかどうか
//------------------------------------------------------------------
extern BOOL ZONEDATA_FlyEnable( u16 zone_id );

//------------------------------------------------------------------
/// 「あなぬけ」による脱出を使えるマップかどうか
//------------------------------------------------------------------
extern BOOL ZONEDATA_EscapeEnable( u16 zone_id );

//------------------------------------------------------------------
/// パレスに遷移できるマップかどうか
//------------------------------------------------------------------
extern BOOL ZONEDATA_EnablePalaceUse( u16 zone_id );

//------------------------------------------------------------------
//	特殊なサンプルOBJを使用するかどうかの設定取得
//------------------------------------------------------------------
extern BOOL ZONEDATA_DEBUG_IsSampleObjUse(u16 zone_id);

//------------------------------------------------------------------
//  レール移動を使用しているマップかどうかのチェック
//------------------------------------------------------------------
extern BOOL ZONEDATA_IsRailMap(u16 zone_id);

//------------------------------------------------------------------
//  レールデータ指定IDを取得
//------------------------------------------------------------------
extern int ZONEDATA_GetRailDataID(u16 zone_id);

//------------------------------------------------------------------
//  ユニオンルームマップかどうかのチェック
//------------------------------------------------------------------
extern BOOL ZONEDATA_IsUnionRoom(u16 zone_id);

//------------------------------------------------------------------
//  ユニオンルームマップかどうかのチェック
//------------------------------------------------------------------
extern BOOL ZONEDATA_IsColosseum(u16 zone_id);

//------------------------------------------------------------------
//  パレスマップかどうかのチェック
//------------------------------------------------------------------
extern BOOL ZONEDATA_IsPalace(u16 zone_id);

//------------------------------------------------------------------
//  遊覧船内マップかどうかのチェック
//------------------------------------------------------------------
extern BOOL ZONEDATA_IsPlBoat(u16 zone_id);

//------------------------------------------------------------------
//  パレスフィールドかどうかのチェック
//------------------------------------------------------------------
extern BOOL ZONEDATA_IsPalaceField(u16 zone_id);

//------------------------------------------------------------------
//  ビンゴマップかどうかのチェック
//------------------------------------------------------------------
extern BOOL ZONEDATA_IsBingo(u16 zone_id);

//------------------------------------------------------------------
//  WFBCかどうかのチェック
//------------------------------------------------------------------
extern BOOL ZONEDATA_IsWfbc(u16 zone_id);

//------------------------------------------------------------------
//  ミュージカル待合室かどうかのチェック
//------------------------------------------------------------------
BOOL ZONEDATA_IsMusicalWaitingRoom(u16 zone_id);

//------------------------------------------------------------------
/// グループIDの取得
//------------------------------------------------------------------
extern u16 ZONEDATA_GetGroupID( u16 zone_id);

//------------------------------------------------------------------
//  地名メッセージのIDを取得
//------------------------------------------------------------------
extern u16 ZONEDATA_GetPlaceNameID(u16 zone_id);

//------------------------------------------------------------------
//  地名表示制御用フラグ取得
//------------------------------------------------------------------
extern BOOL ZONEDATA_GetPlaceNameFlag(u16 zone_id);

//------------------------------------------------------------------
//  指定ゾーンがフィールドマップのマトリックス指定かどうかを判定
//------------------------------------------------------------------
extern BOOL ZONEDATA_IsFieldMatrixID(u16 zone_id);

//------------------------------------------------------------------
//  指定ゾーンのエンカウントデータIDを取得
//------------------------------------------------------------------
extern u16 ZONEDATA_GetEncountDataID(u16 zone_id);


//------------------------------------------------------------------
// イベントデータ指定IDの取得
//------------------------------------------------------------------
extern u16 ZONEDATA_GetEventDataArcID(u16 zone_id);

//------------------------------------------------------------------
// 戦闘背景タイプIDの取得
//------------------------------------------------------------------
extern u8 ZONEDATA_GetBattleBGType(u16 zone_id);


//------------------------------------------------------------------
// ゾーンごとのFOG IDX
//------------------------------------------------------------------
extern u32 ZONEDATA_GetFog(u16 zone_id);

//------------------------------------------------------------------
// ゾーンごとのLIGHT IDX
//------------------------------------------------------------------
extern u32 ZONEDATA_GetLight(u16 zone_id);

//------------------------------------------------------------------
// フィールド技　マップエフェクト
// fieldskill_mapeff.h
//return FIELDSKILL_MAPEFF_MSK
//------------------------------------------------------------------
extern u32 ZONEDATA_GetFieldSkillMapEffMsk(u16 zone_id);

//------------------------------------------------------------------
//  シーンエリア（カメラ演出など）　の取得
//return 
//------------------------------------------------------------------
extern u32 ZONEDATA_GetSceneAreaID(u16 zone_id);

//------------------------------------------------------------------
// マップ切り替えタイプの取得
//------------------------------------------------------------------
extern u8 ZONEDATA_GetMapChangeType(u16 zone_id);


//------------------------------------------------------------------
// 海底神殿　ダンジョンマップか？
//------------------------------------------------------------------
extern BOOL ZONEDATA_IsSeaTempleDungeon( u16 zone_id );

