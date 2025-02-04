////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   iss_dungeon_sys.h
 * @brief  ダンジョンISSユニット
 * @author obata_toshihiro
 * @date   2009.07.16
 */
////////////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "iss_dungeon_sys.h"
#include "gamesystem/playerwork.h"
#include "gamesystem/pm_season.h"
#include "gamesystem/game_data.h"
#include "sound/pm_sndsys.h"
#include "arc/arc_def.h"
#include "../../../resource/fldmapdata/zonetable/zone_id.h"
#include "../field/field_sound.h"


//=========================================================================================
// ■定数・マクロ
//=========================================================================================
//#define DEBUG_PRINT_ON // デバッグ出力スイッチ
#define PRINT_TARGET (1)

// 無効なゾーンID
#define INVALID_ZONE_ID (0xffff)

// ピッチ(キー)設定対象トラック
#define PITCH_TRACK_MASK (~((1<<(9-1)) | (1<<(10-1))))  // 9, 10トラック

// リバーブ設定
#define REVERB_SAMPLE_RATE (16000)	// サンプリングレート
#define REVERB_VOLUME      (63)		  // ボリューム
#define REVERB_STOP_FRAME  (0)		  // 停止までのフレーム数


//=========================================================================================
// ■BGMパラメータ
//=========================================================================================
typedef struct {

	u16 zoneID;	                // ゾーンID
  u16 padding;
	s16 pitch[PMSEASON_TOTAL];	// ピッチ(キー)
	u16 tempo[PMSEASON_TOTAL];	// テンポ
	u16 reverb[PMSEASON_TOTAL];	// リバーブ

} BGM_PARAM; 

//------------------------------------------------------------------------------------------
/**
 * @brief 指定パラメータを反映させる
 *
 * @param param  設定するパラメータ
 * @param season 季節を指定
 */
//------------------------------------------------------------------------------------------
static void SetBGMParam( const BGM_PARAM* param, u8 season )
{
  GF_ASSERT( param );
  GF_ASSERT( season < PMSEASON_TOTAL );

  // テンポ
	PMSND_SetStatusBGM( param->tempo[season], PMSND_NOEFFECT, 0 ); 

  // ピッチ
	NNS_SndPlayerSetTrackPitch( 
      PMSND_GetBGMhandlePointer(), PITCH_TRACK_MASK, param->pitch[season] ); 

  // リバーブ
#if 0 // 2010.04.20 リバーブは使用しない!!
  if( param->reverb[season] == 0 )
  {
    PMSND_DisableCaptureReverb();
  }
  else
  {
    PMSND_EnableCaptureReverb( 
        param->reverb[season], REVERB_SAMPLE_RATE, REVERB_VOLUME, REVERB_STOP_FRAME );
  }
#endif

#ifdef DEBUG_PRINT_ON
	OS_TFPrintf( PRINT_TARGET, "ISS-D: set BGM param\n" );
	OS_TFPrintf( PRINT_TARGET, "- pitch  = %d\n", param->pitch[season] );
	OS_TFPrintf( PRINT_TARGET, "- tempo  = %d\n", param->tempo[season] );
	OS_TFPrintf( PRINT_TARGET, "- reverb = %d\n", param->reverb[season] );
#endif 
}


//=========================================================================================
// ■ダンジョンISSデータセット
//=========================================================================================
typedef struct {

	u8         dataNum;	 // データ数
	BGM_PARAM*   param;	 // パラメータ配列

} BGM_PARAMSET;


//-----------------------------------------------------------------------------------------
/**
 * @brief データを読み込む
 *
 * @param heap_id 使用するヒープID
 *
 * @return 読み込んだデータ
 */
//-----------------------------------------------------------------------------------------
static BGM_PARAMSET* LoadParamset( HEAPID heap_id )
{
	int dat_id;
	BGM_PARAMSET* paramset;

	// 本体を作成
	paramset          = (BGM_PARAMSET*)GFL_HEAP_AllocMemory( heap_id, sizeof(BGM_PARAMSET) ); 
	paramset->dataNum = GFL_ARC_GetDataFileCnt( ARCID_ISS_DUNGEON ); 
	paramset->param   = GFL_HEAP_AllocMemory( heap_id, sizeof(BGM_PARAM) * paramset->dataNum );

	// 全データを取得
	for( dat_id=0; dat_id<paramset->dataNum; dat_id++ )
	{
    GFL_ARC_LoadDataOfs( &paramset->param[dat_id], 
                         ARCID_ISS_DUNGEON, dat_id, 0, sizeof(BGM_PARAM) );
	} 

#ifdef DEBUG_PRINT_ON
	OS_TFPrintf( PRINT_TARGET, "ISS-D: load BGM parameters\n" );
	OS_TFPrintf( PRINT_TARGET, "- dataNum = %d\n", paramset->dataNum );
	for( dat_id=0; dat_id<paramset->dataNum; dat_id++ )
	{ 
		OS_TFPrintf( PRINT_TARGET, "- data[%d].zoneID = %d\n", dat_id, paramset->param[dat_id].zoneID ); 
		OS_TFPrintf( PRINT_TARGET, "- data[%d].pitch  = %d, %d, %d, %d\n", dat_id, 
        paramset->param[dat_id].pitch[PMSEASON_SPRING], 
        paramset->param[dat_id].pitch[PMSEASON_SUMMER], 
        paramset->param[dat_id].pitch[PMSEASON_AUTUMN], 
        paramset->param[dat_id].pitch[PMSEASON_WINTER] );
		OS_TFPrintf( PRINT_TARGET, "- data[%d].tempo  = %d, %d, %d, %d\n", dat_id,
        paramset->param[dat_id].tempo[PMSEASON_SPRING], 
        paramset->param[dat_id].tempo[PMSEASON_SUMMER], 
        paramset->param[dat_id].tempo[PMSEASON_AUTUMN], 
        paramset->param[dat_id].tempo[PMSEASON_WINTER] );
		OS_TFPrintf( PRINT_TARGET, "- data[%d].reverb = %d, %d, %d, %d\n", dat_id,
        paramset->param[dat_id].reverb[PMSEASON_SPRING], 
        paramset->param[dat_id].reverb[PMSEASON_SUMMER], 
        paramset->param[dat_id].reverb[PMSEASON_AUTUMN], 
        paramset->param[dat_id].reverb[PMSEASON_WINTER] );
	}
#endif

	// 読み込んだデータを返す
	return paramset;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief データを破棄する
 *
 * @param paramset 破棄するデータ
 */
//-----------------------------------------------------------------------------------------
static void UnloadIssData( BGM_PARAMSET* paramset )
{ 
	// パラメータ配列を破棄
	GFL_HEAP_FreeMemory( paramset->param );

	// 本体を破棄
	GFL_HEAP_FreeMemory( paramset );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief 指定ゾーンIDのBGMパラメータを取得する
 *
 * @param paramset 検索元データ
 * @param zone_id  検索対象ゾーンID
 *
 * @return 指定ゾーンIDのパラメータ(存在しない場合, NULL)
 */
//-----------------------------------------------------------------------------------------
static const BGM_PARAM* GetBGMParam( const BGM_PARAMSET* paramset, u16 zone_id )
{
	int i;

	// 指定ゾーンIDを検索
	for( i=0; i<paramset->dataNum; i++ )
	{
		// 発見
		if( paramset->param[i].zoneID == zone_id ) {
			return &paramset->param[i];
		}
	}

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_TARGET, "ISS-D: BGM param is not found\n" );
#endif

	return NULL;
}


//=========================================================================================
// ■ダンジョンISSシステム管理ワーク
//=========================================================================================
struct _ISS_DUNGEON_SYS
{
	// 使用するヒープID
	HEAPID heapID;

	// 監視対象
	PLAYER_WORK* player;  // プレイヤー
  GAMEDATA*     gdata;  // ゲームデータ

	// 起動状態
	BOOL boot_flag; 

	// ゾーンID
	u16 currentZoneID;
	u16 nextZoneID;

	// データ
	BGM_PARAMSET* paramset;

	// 現在の設定データ
	const BGM_PARAM* pActiveParam;

  // デフォルト・パラメータ
  BGM_PARAM defaultParam;
};


//=========================================================================================
// ■非公開関数のプロトタイプ宣言
//=========================================================================================
static void SetupDefaultParam( ISS_DUNGEON_SYS* sys );
static void BootSystem( ISS_DUNGEON_SYS* sys );
static void StopSystem( ISS_DUNGEON_SYS* sys );
static void UpdateBGMParam( ISS_DUNGEON_SYS* sys, u16 zone_id );


//=========================================================================================
// ■公開関数
//========================================================================================= 

//-----------------------------------------------------------------------------------------
/**
 * @brief  ダンジョンISSシステムを作成する
 *
 * @param  gdata   ゲームデータ
 * @param  player  監視対象のプレイヤー
 * @param  heap_id 使用するヒープID
 * 
 * @return ダンジョンISSシステム
 */
//-----------------------------------------------------------------------------------------
ISS_DUNGEON_SYS* ISS_DUNGEON_SYS_Create( GAMEDATA* gdata, 
                                         PLAYER_WORK* player, HEAPID heap_id )
{
	ISS_DUNGEON_SYS* sys;

	// メモリ確保
	sys = (ISS_DUNGEON_SYS*)GFL_HEAP_AllocMemory( heap_id, sizeof(ISS_DUNGEON_SYS) );

	// 初期化
	sys->heapID        = heap_id;
  sys->gdata         = gdata;
	sys->player        = player;
	sys->boot_flag     = FALSE;
	sys->currentZoneID = INVALID_ZONE_ID;
	sys->nextZoneID    = INVALID_ZONE_ID;
	sys->paramset      = LoadParamset( heap_id );
	sys->pActiveParam  = NULL;

  // デフォルトパラメータ設定
  SetupDefaultParam( sys );

#ifdef DEBUG_PRINT_ON
	OS_TFPrintf( PRINT_TARGET, "ISS-D: create\n" );
#endif

	// 作成したダンジョンISSシステムを返す
	return sys;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief  ダンジョンISSシステムを破棄する
 *
 * @param sys 破棄するダンジョンISSシステム
 */
//-----------------------------------------------------------------------------------------
void ISS_DUNGEON_SYS_Delete( ISS_DUNGEON_SYS* sys )
{
	// システム停止
	ISS_DUNGEON_SYS_Off( sys );
	
	// データを破棄
	UnloadIssData( sys->paramset );

	// 本体を破棄
	GFL_HEAP_FreeMemory( sys );

#ifdef DEBUG_PRINT_ON
	OS_TFPrintf( PRINT_TARGET, "ISS-D: delete\n" );
#endif
}

//----------------------------------------------------------------------------
/**
 * @brief プレイヤーを監視し, 音量を調整する
 *
 * @param sys 動作対象のダンジョンISSシステム
 */
//----------------------------------------------------------------------------
void ISS_DUNGEON_SYS_Update( ISS_DUNGEON_SYS* sys )
{
  // 起動してない
  if( !sys->boot_flag ){ return; }

  // ゾーンの切り替わりを検出
	if( sys->currentZoneID != sys->nextZoneID ) {
    sys->currentZoneID = sys->nextZoneID;
    UpdateBGMParam( sys, sys->currentZoneID );
  }
} 

//-----------------------------------------------------------------------------------------
/**
 * @brief ゾーン切り替えを通知する
 *
 * @param sys          通知対象のダンジョンISSシステム
 * @param next_zone_id 新しいゾーンID
 */
//-----------------------------------------------------------------------------------------
void ISS_DUNGEON_SYS_ZoneChange( ISS_DUNGEON_SYS* sys, u16 next_zone_id )
{ 
  // ゾーン更新
	sys->nextZoneID = next_zone_id; 

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_TARGET, "ISS-D: zone change\n" );
  OS_TFPrintf( PRINT_TARGET, "- next zone id = %d\n", next_zone_id );
#endif
}

//-----------------------------------------------------------------------------------------
/**
 * @brief システムを起動する
 *
 * @param sys 起動するシステム
 */
//-----------------------------------------------------------------------------------------
void ISS_DUNGEON_SYS_On( ISS_DUNGEON_SYS* sys )
{
  BootSystem( sys );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief システムを停止させる
 *
 * @param sys 停止させるシステム
 */
//-----------------------------------------------------------------------------------------
void ISS_DUNGEON_SYS_Off( ISS_DUNGEON_SYS* sys )
{
  StopSystem( sys );
}

//-----------------------------------------------------------------------------------------
/**
 * @breif 動作状態を取得する
 *
 * @param sys 状態を調べるISSシステム
 * 
 * @return 動作中かどうか
 */
//-----------------------------------------------------------------------------------------
BOOL ISS_DUNGEON_SYS_IsOn( const ISS_DUNGEON_SYS* sys )
{
	return sys->boot_flag; 
}

//-----------------------------------------------------------------------------------------
/**
 * @breif 設定データの有無を調べる
 *
 * @param system
 * @param zone_id
 * 
 * @return 指定したゾーンの設定データを持っている場合 TRUE
 *         そうでなければ FALSE
 */
//-----------------------------------------------------------------------------------------
BOOL ISS_DUNGEON_SYS_IsActiveAt( const ISS_DUNGEON_SYS* system, u16 zone_id )
{
  // データを読み込んでいない
  if( system->paramset == NULL ) {
    GF_ASSERT(0);
    return FALSE;
  }

  // 指定されたゾーンのデータは持っていない
  if( GetBGMParam( system->paramset, zone_id ) == NULL ) {
    return FALSE;
  }

  return TRUE;
}


//=========================================================================================
// ■非公開関数
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief デフォルトパラメータを設定する
 *
 * @param sys ダンジョンISSシステム
 */
//-----------------------------------------------------------------------------------------
static void SetupDefaultParam( ISS_DUNGEON_SYS* sys )
{
  int i;
  sys->defaultParam.zoneID = INVALID_ZONE_ID;
  for( i=0; i<PMSEASON_TOTAL; i++ )
  {
    sys->defaultParam.pitch[i]  = 0;
    sys->defaultParam.tempo[i]  = 256;
    sys->defaultParam.reverb[i] = 0;
  }
} 

//-----------------------------------------------------------------------------------------
/**
 * @brief システムを起動する
 *
 * @param sys ダンジョンISSシステム
 */
//-----------------------------------------------------------------------------------------
static void BootSystem( ISS_DUNGEON_SYS* sys )
{
  // 起動済み
  if( sys->boot_flag ){ return; }

  // 起動
	sys->boot_flag = TRUE;
  UpdateBGMParam( sys, sys->currentZoneID );

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_TARGET, "ISS-D: boot\n" );
#endif
}

//-----------------------------------------------------------------------------------------
/**
 * @brief システムを停止する
 *
 * @param sys ダンジョンISSシステム
 */
//-----------------------------------------------------------------------------------------
static void StopSystem( ISS_DUNGEON_SYS* sys )
{
  // 停止済み
  if( !sys->boot_flag ){ return; }

	// 停止
	sys->boot_flag = FALSE;

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_TARGET, "ISS-D: stop\n" );
#endif
}

//-----------------------------------------------------------------------------------------
/**
 * @brief BGMパラメータを更新する
 *
 * @param sys      ダンジョンISSシステム
 * @param zone_id  パラメータを適用するゾーンID
 */
//-----------------------------------------------------------------------------------------
static void UpdateBGMParam( ISS_DUNGEON_SYS* sys, u16 zone_id )
{
	// 起動していない
  GF_ASSERT( sys->boot_flag );

  // 新ゾーンIDのBGMパラメータを検索
  sys->pActiveParam = GetBGMParam( sys->paramset, zone_id );

  // BGMパラメータが設定されていない
  if( sys->pActiveParam == NULL ){ return; }

  // BGMの設定を反映させる
  {
    u8 season = GAMEDATA_GetSeasonID( sys->gdata );
    SetBGMParam( sys->pActiveParam, season ); 
  }
} 
