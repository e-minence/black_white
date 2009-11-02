/////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   iss_city_sys.c
 * @brief  街ISSシステム
 * @author obata_toshihiro
 * @date   2009.07.16
 */
/////////////////////////////////////////////////////////////////////////////////////////////
#include "iss_city_sys.h"
#include "../field/field_sound.h"
#include "gamesystem/playerwork.h"
#include "../../include/field/field_const.h"		// FIELD_CONST_GRID_FX32_SIZE
#include "arc/arc_def.h"
#include "arc/iss.naix"
#include "field/field_const.h"
#include "iss_city_unit.h"
#include "../../../resource/fldmapdata/zonetable/zone_id.h"
#include "../../../resource/iss/city/entry_table.cdat"


//===========================================================================================
// ■定数・マクロ
//===========================================================================================

// 無効ユニット番号(街ISSのBGMが鳴っているが, ISSユニットが配置されていない状況)
#define INVALID_UNIT_NO (0xff)

// 最大音量
#define MAX_VOLUME (127)
#define INVALID_VOLUME (0xffff)


//===========================================================================================
// ■街ISSシステム構造体
//===========================================================================================
struct _ISS_CITY_SYS
{
	// 使用するヒープID
	HEAPID heapID;

	// 監視対象プレイヤー
	PLAYER_WORK* pPlayer;

	// 起動状態
	BOOL isActive;		  // 動作中かどうか
	u8   activeUnitNo;	// 動作中のユニット番号
  int  volume;        // 音量

	// ISSユニット情報
	u8    unitNum;		// ユニット数
	ISS_C_UNIT** unit;		// ユニット配列
};


//===========================================================================================
// ■非公開関数のプロトタイプ宣言
//===========================================================================================
static void LoadUnitData( ISS_CITY_SYS* sys );
static void UpdateVolume( ISS_CITY_SYS* sys );


//===========================================================================================
// ■公開関数の実装
//===========================================================================================

//-------------------------------------------------------------------------------------------
/**
 * @brief  街ISSシステムを作成する
 *
 * @param  p_player 監視対象のプレイヤー
 * @param  heap_id  使用するヒープID
 * 
 * @return 街ISSシステム
 */
//-------------------------------------------------------------------------------------------
ISS_CITY_SYS* ISS_CITY_SYS_Create( PLAYER_WORK* p_player, HEAPID heap_id )
{
	ISS_CITY_SYS* sys;

	// メモリ確保
	sys = (ISS_CITY_SYS*)GFL_HEAP_AllocMemory( heap_id, sizeof( ISS_CITY_SYS ) );

	// 初期化
	sys->heapID       = heap_id;
	sys->pPlayer      = p_player;
	sys->isActive     = FALSE;
	sys->activeUnitNo = INVALID_UNIT_NO;
  sys->volume       = INVALID_VOLUME;
	sys->unitNum      = 0;
	sys->unit         = NULL;

	// ユニット情報の読み込み
	LoadUnitData( sys );

  // DEBUG:
  OBATA_Printf( "ISS-C: Create\n" );
	
	// 作成した街ISSシステムを返す
	return sys;
}

//------------------------------------------------------------------------------------------
/**
 * @brief  街ISSシステムを破棄する
 *
 * @param sys 破棄する街ISSシステム
 */
//------------------------------------------------------------------------------------------
void ISS_CITY_SYS_Delete( ISS_CITY_SYS* sys )
{
  int i;

	// 各ユニットを破棄
  for( i=0; i<sys->unitNum; i++ )
  {
    ISS_C_UNIT_Delete( sys->unit[i] );
  }
	GFL_HEAP_FreeMemory( sys->unit );

	// 本体の破棄
	GFL_HEAP_FreeMemory( sys );

  // DEBUG:
  OBATA_Printf( "ISS-C: Delete\n" );
}

//------------------------------------------------------------------------------------------
/**
 * @brief プレイヤーを監視し, 音量を調整する
 *
 * @param sys 動作対象の街ISSシステム
 */
//------------------------------------------------------------------------------------------
void ISS_CITY_SYS_Update( ISS_CITY_SYS* sys )
{
	const VecFx32* p_pos = NULL;

	// 起動していなければ, 何もしない
	if( sys->isActive != TRUE ) return;

  // 音量を調整する
  UpdateVolume( sys );
}
	
//---------------------------------------------------------------------------
/**
 * @brief ゾーン切り替えを通知する
 *
 * @param sys        通知対象の街ISSシステム
 * @param next_zone_id 新しいゾーンID
 */
//---------------------------------------------------------------------------
void ISS_CITY_SYS_ZoneChange( ISS_CITY_SYS* sys, u16 next_zone_id )
{
	int i;

	// 新しいゾーンIDを持つユニットを探す
	for( i=0; i<sys->unitNum; i++ )
	{
		// 発見 ==> ユニット番号を更新
		if( ISS_C_UNIT_GetZoneID( sys->unit[i] ) == next_zone_id )
		{ 
			sys->activeUnitNo = i;
      sys->isActive = TRUE;
      sys->volume = INVALID_VOLUME;
      // DEBUG:
      OBATA_Printf( "ISS-C: change unit index = %d\n", i );
			return;
		}
	}

	// 指定ゾーンIDにISSユニットが存在しない場合 ==> システム停止
  ISS_CITY_SYS_Off( sys );
}

//------------------------------------------------------------------------------------------
/**
 * @brief システムを起動する
 *
 * @param sys 起動するシステム
 */
//------------------------------------------------------------------------------------------
void ISS_CITY_SYS_On( ISS_CITY_SYS* sys )
{ 
  // 起動し, ISSユニットを更新する
  ISS_CITY_SYS_ZoneChange( sys, PLAYERWORK_getZoneID( sys->pPlayer ) );

  // DEBUG:
  OBATA_Printf( "ISS-C: On\n" );
}

//------------------------------------------------------------------------------------------
/**
 * @brief システムを停止させる
 *
 * @param sys 停止させるシステム
 */
//------------------------------------------------------------------------------------------
void ISS_CITY_SYS_Off( ISS_CITY_SYS* sys )
{
	sys->isActive     = FALSE;
  sys->activeUnitNo = INVALID_UNIT_NO;
  FIELD_SOUND_ChangeBGMActionVolume( MAX_VOLUME );

  // DEBUG:
  OBATA_Printf( "ISS-C: Off\n" );
}

//------------------------------------------------------------------------------------------
/**
 * @breif 動作状態を取得する
 *
 * @param sys 状態を調べるISSシステム
 * 
 * @return 動作中かどうか
 */
//------------------------------------------------------------------------------------------
BOOL ISS_CITY_SYS_IsOn( const ISS_CITY_SYS* sys )
{
	return sys->isActive;
}


//===========================================================================================
// ■非公開関数の実装
//===========================================================================================

//-------------------------------------------------------------------------------------------
/**
 * @brief ユニットデータを読み込む
 *
 * @param sys 読み込んだデータを保持するシステム
 */
//-------------------------------------------------------------------------------------------
static void LoadUnitData( ISS_CITY_SYS* sys )
{
  int i;

  // 登録されているユニットの数を取得
  sys->unitNum = NELEMS(entry_table);

  // 各ユニットを作成
  sys->unit = GFL_HEAP_AllocMemory( sys->heapID, sys->unitNum * sizeof(ISS_C_UNIT*) );
  for( i=0; i<sys->unitNum; i++ )
  {
    sys->unit[i] = ISS_C_UNIT_Create( sys->heapID, entry_table[i].zoneID );
  }

}

//-------------------------------------------------------------------------------------------
/**
 * @brief 音量を調整する
 *
 * @param sys 調整を行う
 */
//-------------------------------------------------------------------------------------------
static void UpdateVolume( ISS_CITY_SYS* sys )
{
  int new_volume = 0;
  const VecFx32* pos;

  // ユニットと自機の位置からボリュームを算出
	if( sys->activeUnitNo < sys->unitNum )
  { 
    pos = PLAYERWORK_getPosition( sys->pPlayer );
    new_volume = ISS_C_UNIT_GetVolume( sys->unit[sys->activeUnitNo], pos );
  }

  // 音量が変化した場合
  if( sys->volume != new_volume )
  {
    // 更新
    sys->volume = new_volume;
    FIELD_SOUND_ChangeBGMActionVolume( new_volume );
    // DEBUG:
    OBATA_Printf( "ISS-C: Update volume = %d\n", new_volume );
  }
}
