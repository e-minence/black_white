/*
 *  @file   beacon_status.c
 *  @brief  ゲーム中保持するビーコンログ管理
 *  @author Miyuki Iwasawa
 *  @date   10.01.21
 */
#pragma once

#include "gamesystem/game_beacon.h"

typedef struct _TAG_BEACON_STATUS BEACON_STATUS;

//--------------------------------------------------------------
/**
 * @brief ビーコンステータスワーク取得
 * @param   gamedata	GAMEDATAへのポインタ
 * @param   BEACON_STATUS*
 *
 * 実体はsrc/gamesystem/game_data.cにあるが、情報隠蔽を
 * 考慮してアクセス関数群を扱うヘッダに配置している
 */
//--------------------------------------------------------------
extern BEACON_STATUS* GAMEDATA_GetBeaconStatus( GAMEDATA * gamedata );

/*
 *  @brief  ビーコンステータスワーク生成
 */
extern BEACON_STATUS* BEACON_STATUS_Create( HEAPID heapID, HEAPID tmpHeapID );

/*
 *  @brief  ビーコンステータスワーク破棄
 */
extern void BEACON_STATUS_Delete( BEACON_STATUS* wk );

/*
 *  @brief  InfoTblを取得
 */
extern GAMEBEACON_INFO_TBL* BEACON_STATUS_GetInfoTbl( BEACON_STATUS* wk );

/*
 *  @brief  ビューリストのトップオフセットを取得
 */
extern u8  BEACON_STATUS_GetViewTopOffset( BEACON_STATUS* wk );

/*
 *  @brief  ビューリストのトップオフセットをセット
 */
extern void BEACON_STATUS_SetViewTopOffset( BEACON_STATUS* wk, u8 ofs );

/*
 *  @brief  フリーワードバッファのアドレスを取得
 */
extern STRBUF* BEACON_STATUS_GetFreeWordBuffer( BEACON_STATUS* wk );

/*
 *  @brief  フリーワードバッファの入力フラグアドレスを取得
 */
extern u16* BEACON_STATUS_GetFreeWordInputResultPointer( BEACON_STATUS* wk );


#ifdef PM_DEBUG

//------------------------------------------------------------
/*
 *  @brief  デバッグ用スタックチェックスルーフラグセット
 */
//------------------------------------------------------------
extern void DEBUG_BEACON_STATUS_SetStackCheckThrowFlag( BEACON_STATUS* wk, BOOL flag );

//------------------------------------------------------------
/*
 *  @brief  デバッグ用スタックチェックスルーフラグゲット
 */
//------------------------------------------------------------
extern BOOL DEBUG_BEACON_STATUS_GetStackCheckThrowFlag( BEACON_STATUS* wk );

#endif  //PM_DEBUG
