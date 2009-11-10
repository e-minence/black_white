///////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   iss_sys.h
 * @brief  ISSシステム
 * @author obata_toshihiro
 * @date   2009.07.16
 */
///////////////////////////////////////////////////////////////////////////////////////////
#include "iss_sys.h"
#include "iss_city_sys.h"
#include "iss_road_sys.h"
#include "iss_dungeon_sys.h"
#include "iss_zone_sys.h"
#include "../field/field_sound.h"
#include "sound/bgm_info.h"
#include "../../../resource/sound/bgm_info/iss_type.h"
#include "gamesystem/playerwork.h"
#include "gamesystem/game_data.h"
#include "sound/pm_sndsys.h" 


//=========================================================================================
/**
 * @brief 定数・マクロ
 */
//=========================================================================================

// 再生中BGM番号の無効値
#define INVALID_BGM_NO (0xffff)


//=========================================================================================
/**
 * @brief ISSシステムワーク
 */
//=========================================================================================
struct _ISS_SYS
{
	// 使用するヒープID
	HEAPID heapID;

	// 監視対象ゲームデータ
	GAMEDATA* pGameData;

	// 自転車状態かどうか
	BOOL cycle;

	// なみのり状態かどうか
	BOOL surfing;

	// 各ISSシステム
	ISS_CITY_SYS*    issC;	// 街
	ISS_ROAD_SYS*    issR;	// 道路
	ISS_DUNGEON_SYS* issD;	// ダンジョン 
  ISS_ZONE_SYS*    issZ;  // ゾーン

	// 再生中のBGM番号
	u16 bgmNo; 

	// フレームカウンタ
	u32 frame;
};


//=========================================================================================
/**
 * @brief 非公開関数のプロトタイプ宣言
 */
//=========================================================================================

// 主人公の自転車状態を監視する
void CycleCheck( ISS_SYS* p_sys );

// 主人公のなみのり状態を監視する
void SurfingCheck( ISS_SYS* p_sys );

// BGMの変化を監視する
void BGMChangeCheck( ISS_SYS* p_sys );


//=========================================================================================
/**
 * @brief 公開関数の実装
 */
//=========================================================================================

//----------------------------------------------------------------------------
/**
 * @brief  ISSシステムを作成する
 *
 * @param p_gdata	監視対象ゲームデータ
 * @param heap_id   使用するヒープID
 * 
 * @return ISSシステム
 */
//----------------------------------------------------------------------------
ISS_SYS* ISS_SYS_Create( GAMEDATA* p_gdata, HEAPID heap_id )
{
	ISS_SYS* p_sys;
	PLAYER_WORK* p_player;

	// 監視対象の主人公を取得
	p_player = GAMEDATA_GetMyPlayerWork( p_gdata );

	// メモリ確保
	p_sys = (ISS_SYS*)GFL_HEAP_AllocMemory( heap_id, sizeof( ISS_SYS ) );

	// 初期設定
	p_sys->heapID    = heap_id;
	p_sys->pGameData = p_gdata;
	p_sys->cycle     = FALSE;
	p_sys->surfing   = FALSE;
	p_sys->issC      = ISS_CITY_SYS_Create( p_player, heap_id );
	p_sys->issR      = ISS_ROAD_SYS_Create( p_player, heap_id );
	p_sys->issD      = ISS_DUNGEON_SYS_Create( p_player, heap_id );
  p_sys->issZ      = ISS_ZONE_SYS_Create( p_gdata, p_player, heap_id );
	p_sys->bgmNo     = INVALID_BGM_NO;
	p_sys->frame     = 0;


	// 作成したISSシステムを返す
	return p_sys;
}

//----------------------------------------------------------------------------
/**
 * @brief  ISSシステムを破棄する
 * * @param p_sys 破棄するISSシステム 
 */
//----------------------------------------------------------------------------
void ISS_SYS_Delete( ISS_SYS* p_sys )
{
	// 各ISSシステムを破棄
	ISS_CITY_SYS_Delete( p_sys->issC );
	ISS_ROAD_SYS_Delete( p_sys->issR );
	ISS_DUNGEON_SYS_Delete( p_sys->issD );
  ISS_ZONE_SYS_Delete( p_sys->issZ );

	// 本体を破棄
	GFL_HEAP_FreeMemory( p_sys );

	// 9, 10トラックの音量を元に戻す
  PMSND_ChangeBGMVolume( (1<<8)|(1<<9), 127 );
}

//----------------------------------------------------------------------------
/**
 * @brief プレイヤーを監視し, 音量を調整する
 *
 * @param p_sys 操作対象のシステム
 */
//----------------------------------------------------------------------------
void ISS_SYS_Update( ISS_SYS* p_sys )
{ 
	p_sys->frame++;

	// 自転車チェック
	CycleCheck( p_sys );

	// なみのりチェック
	SurfingCheck( p_sys );

	// BGM変更チェック
	BGMChangeCheck( p_sys );

	// 街ISS
	ISS_CITY_SYS_Update( p_sys->issC );

	// 道路ISS(主人公が30fpsで動作するため, 道路ISSもそれに合わせる)
	if( p_sys->frame % 2 == 0 ) ISS_ROAD_SYS_Update( p_sys->issR ); 
	
	// ダンジョンISS
	ISS_DUNGEON_SYS_Update( p_sys->issD );

  // ゾーンISS
  ISS_ZONE_SYS_Update( p_sys->issZ );
}
	

//---------------------------------------------------------------------------
/**
 * @brief ゾーン切り替えを通知する
 *
 * @param p_sys       通知対象のISSシステム
 * @param next_zone_id 新しいゾーンID
 */
//---------------------------------------------------------------------------
void ISS_SYS_ZoneChange( ISS_SYS* p_sys, u16 next_zone_id )
{
	PLAYER_MOVE_FORM form;
	BGM_INFO_SYS*    p_bgm_info_sys;

	// 自転車 or なみのり中なら, ISSに変更はない
	if( p_sys->cycle | p_sys->surfing ) return;

	// 街ISS
	ISS_CITY_SYS_ZoneChange( p_sys->issC, next_zone_id );

	// ダンジョンISS
	ISS_DUNGEON_SYS_ZoneChange( p_sys->issD, next_zone_id );

  // ゾーンISS
  ISS_ZONE_SYS_ZoneChange( p_sys->issZ, next_zone_id );

	// DEBUG:
	//OBATA_Printf( "ISS_SYS_ZoneChange()\n" );
}


//=========================================================================================
/**
 * @param 非公開関数の実装
 */
//=========================================================================================

//----------------------------------------------------------------------------
/**
 * @brief 主人公の自転車状態を監視する
 */
//----------------------------------------------------------------------------
void CycleCheck( ISS_SYS* p_sys )
{
	PLAYER_WORK*     p_player;
	PLAYER_MOVE_FORM form;

	// 自機の状態を取得
	p_player = GAMEDATA_GetMyPlayerWork( p_sys->pGameData );
	form     = PLAYERWORK_GetMoveForm( p_player );

	// 自転車に乗るのを検出したら, ISSを停止
	if( ( p_sys->cycle == FALSE ) && ( form == PLAYER_MOVE_FORM_CYCLE ) )
	{
		p_sys->cycle = TRUE;
		ISS_CITY_SYS_Off( p_sys->issC );
		ISS_ROAD_SYS_Off( p_sys->issR );
		ISS_DUNGEON_SYS_Off( p_sys->issD );
	}

	// 自転車から降りるのを検出したら, ゾーン切り替え時と同じ処理
	if( ( p_sys->cycle == TRUE ) && ( form != PLAYER_MOVE_FORM_CYCLE ) )
	{
		p_sys->cycle = FALSE;
		ISS_SYS_ZoneChange( p_sys, PLAYERWORK_getZoneID( p_player ) );
	}
}

//----------------------------------------------------------------------------
/**
 * @brief 主人公のなみのり状態を監視する
 */
//----------------------------------------------------------------------------
void SurfingCheck( ISS_SYS* p_sys )
{
	PLAYER_WORK*     p_player;
	PLAYER_MOVE_FORM form;

	// 自機の状態を取得
	p_player = GAMEDATA_GetMyPlayerWork( p_sys->pGameData );
	form     = PLAYERWORK_GetMoveForm( p_player );

	// なみのり開始を検出したら, ISSを停止
	if( ( p_sys->surfing == FALSE ) && ( form == PLAYER_MOVE_FORM_SWIM ) )
	{
		p_sys->surfing = TRUE;
		ISS_CITY_SYS_Off( p_sys->issC );
		ISS_ROAD_SYS_Off( p_sys->issR );
		ISS_DUNGEON_SYS_Off( p_sys->issD );
	}

	// 自転車から降りるのを検出したら, ゾーン切り替え時と同じ処理
	if( ( p_sys->surfing == TRUE ) && ( form != PLAYER_MOVE_FORM_SWIM ) )
	{
		p_sys->surfing = FALSE;
		ISS_SYS_ZoneChange( p_sys, PLAYERWORK_getZoneID( p_player ) );
	}
} 

//----------------------------------------------------------------------------
/**
 * @brief BGMの変化を監視する
 *
 * @param 動かすシステム
 */
//----------------------------------------------------------------------------
void BGMChangeCheck( ISS_SYS* p_sys )
{
	BGM_INFO_SYS* p_bgm_info_sys = GAMEDATA_GetBGMInfoSys( p_sys->pGameData );
	int bgm_no, iss_type;

	// 次に再生予定のBGMのISSタイプを取得
	// (次のBGMが指定されていない場合, 現在再生中のBGMのISSタイプを取得する)
	bgm_no   = PMSND_GetBGMsoundNo();
	iss_type = BGM_INFO_GetIssType( p_bgm_info_sys, bgm_no ); 

  // BGMに変化がなければ何もしない
  if( p_sys->bgmNo == bgm_no ) return;

  // 各ISSシステムの起動状態を変更
  switch( iss_type )
  {
  case ISS_TYPE_LOAD:  // 道路
      ISS_ROAD_SYS_On( p_sys->issR );
      ISS_CITY_SYS_Off( p_sys->issC );
      ISS_DUNGEON_SYS_Off( p_sys->issD );
    break;
  case ISS_TYPE_CITY:  // 街
      ISS_ROAD_SYS_Off( p_sys->issR );
      ISS_CITY_SYS_On( p_sys->issC );
      ISS_DUNGEON_SYS_Off( p_sys->issD );
    break;
  case ISS_TYPE_DUNGEON:  // ダンジョン
      ISS_ROAD_SYS_Off( p_sys->issR );
      ISS_CITY_SYS_Off( p_sys->issC );
      ISS_DUNGEON_SYS_On( p_sys->issD );
    break;
  default:  // その他
      ISS_ROAD_SYS_Off( p_sys->issR );
      ISS_CITY_SYS_Off( p_sys->issC );
      ISS_DUNGEON_SYS_Off( p_sys->issD );
    break;
  } 

  // BGM番号を記憶
  p_sys->bgmNo = bgm_no;
}
