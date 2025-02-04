//============================================================================================
/**
 * @file	eventdata_sxy.h
 * @brief	イベント起動チェック用関数
 * @date	2005.10.14
 *
 * src/fielddata/eventdata/のイベントデータコンバータからも参照されている。
 * そのためこの中に記述できる内容にはかなり制限があることを注意すること。
 *
 * 2008.11.20	DPプロジェクトからコピー。なので上記注釈は今のところ無効
 */
//============================================================================================
#pragma once

#include "field/eventdata_system.h"
#include "field/location.h"
#include "field/fldmmdl.h"

//============================================================================================
//============================================================================================
#include "field/eventdata_type.h"

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	出入口情報を探す
 * @param	evdata	イベントデータへのポインタ
 * @param	pos		探す位置
 * @return	int	出入口データのインデックスEXIT_ID_NONEのとき、出入口は存在しない
 */
//------------------------------------------------------------------
extern int EVENTDATA_SearchConnectIDByPos(const EVENTDATA_SYSTEM * evdata, const VecFx32 * pos);

//------------------------------------------------------------------
/**
 * @brief	出入口情報を探す
 * @param	evdata	イベントデータへのポインタ
 * @param	rail_location		探すレールロケーション
 * @return	int	出入口データのインデックスEXIT_ID_NONEのとき、出入口は存在しない
 */
//------------------------------------------------------------------
extern int EVENTDATA_SearchConnectIDByRailLocation(const EVENTDATA_SYSTEM * evdata, const RAIL_LOCATION* rail_location);

//------------------------------------------------------------------
/**
 * @brief	出入口情報をインデックス指定で取得する
 * @param	evdata			イベントデータへのポインタ
 * @param	exit_id			出入口情報のインデックス
 * @return	CONNECT_DATA	出入口情報へのポインタ
 */
//------------------------------------------------------------------
extern const CONNECT_DATA * EVENTDATA_GetConnectByID(const EVENTDATA_SYSTEM * evdata, u16 exit_id);

//------------------------------------------------------------------
/**
 * @brief	出入口情報指定からLOCATIONをセットする
 * @param	evdata		イベントデータへのポインタ
 * @param	loc			セットするLOCATIONへのポインタ
 * @param	exit_id		出入口情報のインデックス
 * @return	BOOL		FALSEのとき、出入口情報が存在しない
 */
//------------------------------------------------------------------
extern BOOL EVENTDATA_SetLocationByExitID(const EVENTDATA_SYSTEM * evdata, LOCATION * loc, u16 exit_id,
    u16 exit_ofs );


//------------------------------------------------------------------
/**
 * @brief	出入口情報を探す
 * @param	evdata	イベントデータへのポインタ
 * @param	pos		探す位置
 * @return	int	出入口データのインデックスEXIT_ID_NONEのとき、出入口は存在しない
 *
 * ※球判定
 */
//------------------------------------------------------------------
extern int EVENTDATA_SearchConnectIDBySphere(const EVENTDATA_SYSTEM * evdata, const VecFx32 * sphere);

//------------------------------------------------------------------
/**
 * @brief
 */
//------------------------------------------------------------------
extern void CONNECTDATA_SetNextLocation(const CONNECT_DATA * connect, LOCATION * loc, u16 exit_ofs);

//------------------------------------------------------------------
/**
 * @brief	出入口情報が特殊で入り口かどうかの判定
 * @param	connect		参照する出入口情報
 * @return	BOOL		TRUEのとき、特殊接続出入口
 */
//------------------------------------------------------------------
extern BOOL CONNECTDATA_IsSpecialExit(const CONNECT_DATA * connect);

//------------------------------------------------------------------
/**
 * @brief 無効出入口のチェック
 * @retval  TRUE  無効出入口
 * @retval  FALSE 有効な出入口
 */
//------------------------------------------------------------------
extern BOOL CONNECTDATA_IsClosedExit( const CONNECT_DATA * connect );

//------------------------------------------------------------------
//------------------------------------------------------------------
extern LOC_EXIT_OFS CONNECTDATA_GetExitOfs(const CONNECT_DATA * connect, const VecFx32 * pos);
extern LOC_EXIT_OFS CONNECTDATA_GetRailExitOfs(const CONNECT_DATA * connect, const RAIL_LOCATION* loc);

//------------------------------------------------------------------
//  出入口データの持つEXIT_DIRを返す
//------------------------------------------------------------------
extern EXIT_DIR CONNECTDATA_GetExitDir(const CONNECT_DATA * connect);

//------------------------------------------------------------------
//  出入口データの持つDIRを返す（EXIT_DIRを変換する）
//------------------------------------------------------------------
extern BOOL CONNECTDATA_IsEnableDir(const CONNECT_DATA * connect, u16 dir);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern EXIT_TYPE CONNECTDATA_GetExitType(const CONNECT_DATA * connect);


//------------------------------------------------------------------
/**
 * @brief	動作モデルヘッダーを取得
 * @param	evdata		イベントデータへのポインタ
 * @retval	MMDL_HEADER*
 */
//------------------------------------------------------------------
extern const MMDL_HEADER * EVENTDATA_GetNpcData( const EVENTDATA_SYSTEM *evdata );

//------------------------------------------------------------------
/**
 * @brief	動作モデルヘッダーを取得
 * @param	evdata		イベントデータへのポインタ
 * @retval	MMDL_HEADER*
 */
//------------------------------------------------------------------
extern const MMDL_HEADER * EVENTDATA_GetNpcData( const EVENTDATA_SYSTEM *evdata );

//------------------------------------------------------------------
/**
 * @brief	動作モデル総数を取得
 * @param	evdata		イベントデータへのポインタ
 * @retval	u16
 */
//------------------------------------------------------------------
extern u16 EVENTDATA_GetNpcCount( const EVENTDATA_SYSTEM *evdata );


//------------------------------------------------------------------
/**
 * @brief	座標イベントを取得
 * @param	evdata イベントデータへのポインタ
 * @param evwork イベントワークへのポインタ 
 * @param pos チェックする座標
 * @param dir チェックする方向（DIR_NOTのとき、向き指定なし）
 * @retval NULL = イベントなし
 */
//------------------------------------------------------------------
extern const POS_EVENT_DATA * EVENTDATA_GetPosEvent( 
    const EVENTDATA_SYSTEM *evdata, EVENTWORK *evwork, const VecFx32 *pos, u16 dir );

//------------------------------------------------------------------
/**
 * @brief	座標イベントを取得(高さを無視するver.)
 * @param	evdata イベントデータへのポインタ
 * @param evwork イベントワークへのポインタ 
 * @param pos チェックする座標
 * @param dir チェックする方向（DIR_NOTのとき、向き指定なし）
 * @retval NULL = イベントなし
 */
//------------------------------------------------------------------
extern const POS_EVENT_DATA * EVENTDATA_GetPosEvent_XZ( 
    const EVENTDATA_SYSTEM *evdata, EVENTWORK *evwork, const VecFx32 *pos, u16 dir ); 


//------------------------------------------------------------------
/**
 * @brief	ダミーイベントを取得  "使用注意"
 * @param	evdata イベントデータへのポインタ
 * @param evwork イベントワークへのポインタ 
 * @param pos チェックする座標
 * @retval NULL = イベントなし
 */
//------------------------------------------------------------------
extern const POS_EVENT_DATA * EVENTDATA_GetDummyPosEvent( 
    const EVENTDATA_SYSTEM *evdata, EVENTWORK *evwork, const VecFx32 *pos );

//------------------------------------------------------------------
/**
 * @brief	座標イベントを取得
 * @param	evdata イベントデータへのポインタ
 * @param evwork イベントワークへのポインタ 
 * @param location チェックするレールロケーション
 * @retval NULL = イベントなし
 */
//------------------------------------------------------------------
extern const POS_EVENT_DATA * EVENTDATA_GetPosEventRailLocation( 
    const EVENTDATA_SYSTEM *evdata, EVENTWORK *evwork, const RAIL_LOCATION* location );




//----------------------------------------------------------------------------
/**
 *	@brief  出入り口イベント　中心３D座標を取得
 *  
 *	@param	data    イベント
 *	@param	pos     座標格納先
 */
//-----------------------------------------------------------------------------
extern void EVENTDATA_GetConnectCenterPos( const CONNECT_DATA * data, VecFx32* pos );

//----------------------------------------------------------------------------
/**
 *	@brief  BG話しかけイベント　中心３D座標を取得
 *  
 *	@param	data    イベント
 *	@param	pos     座標格納先
 */
//-----------------------------------------------------------------------------
extern void EVENTDATA_GetBGTalkCenterPos( const BG_TALK_DATA * data, VecFx32* pos );
extern void EVENTDATA_GetBGTalkCenterRailLocation( const BG_TALK_DATA * data, RAIL_LOCATION* location );

//----------------------------------------------------------------------------
/**
 *	@brief  POS発動イベント　中心３D座標を取得
 *  
 *	@param	data    イベント
 *	@param	pos     座標格納先
 */
//-----------------------------------------------------------------------------
extern void EVENTDATA_GetPosEventCenterPos( const POS_EVENT_DATA * data, VecFx32* pos );
extern void EVENTDATA_GetPosEventCenterRailLocation( const POS_EVENT_DATA * data, RAIL_LOCATION* location );

