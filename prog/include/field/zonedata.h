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
};

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
// 	ゾーン名データの取得
//------------------------------------------------------------------
extern void ZONEDATA_GetZoneName(HEAPID heapID, char * buffer, u16 zoneid);
//------------------------------------------------------------------
//  ゾーン名データを全部読み込み
//------------------------------------------------------------------
extern const char * ZONEDATA_GetAllZoneName(HEAPID heapID);

//------------------------------------------------------------------
//	ゾーンの最大数の取得
//------------------------------------------------------------------
extern u16 ZONEDATA_GetZoneIDMax(void);

//------------------------------------------------------------------
// マップアーカイブ指定の取得
//------------------------------------------------------------------
extern u8 ZONEDATA_GetMapArc(u16 zone_id);

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
//	マップ開始位置の取得（デバッグ用）
//------------------------------------------------------------------
extern void ZONEDATA_DEBUG_GetStartPos(u16 zone_id, VecFx32 * pos);

//------------------------------------------------------------------
//	カメラ指定IDの取得
//------------------------------------------------------------------
extern u8 ZONEDATA_GetCameraID(u16 zone_id);

//------------------------------------------------------------------
//	BGM指定IDの取得
//------------------------------------------------------------------
extern u16 ZONEDATA_GetBGMID(u16 zone_id, u8 season_id);

//------------------------------------------------------------------
//  天候指定IDの取得
//------------------------------------------------------------------
extern u16 ZONEDATA_GetWeatherID(u16 zone_id);

//------------------------------------------------------------------
//	特殊なサンプルOBJを使用するかどうかの設定取得
//------------------------------------------------------------------
extern BOOL ZONEDATA_DEBUG_IsSampleObjUse(u16 zone_id);

//------------------------------------------------------------------
//  レール移動を使用しているマップかどうかのチェック
//------------------------------------------------------------------
extern BOOL ZONEDATA_DEBUG_IsRailMap(u16 zone_id);

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
//  地名メッセージのIDを取得
//------------------------------------------------------------------
extern u16 ZONEDATA_GetPlaceNameID(u16 zone_id);

//------------------------------------------------------------------
//  地名メッセージ用のウィンドウIDを取得
//------------------------------------------------------------------
extern u16 ZONEDATA_GetPlaceNameWinID(u16 zone_id);
