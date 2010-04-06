//============================================================================================
/**
 * @file	event_gameclear.c
 * @brief	イベント：ゲームクリア処理
 * @date  2009.12.14
 * @author	tamada GAME FREAK inc.
 *
 * 2009.12.14 event_gameover.cからコピペで作成
 *
 * @todo
 * ゲームオーバーメッセージをPROCにしたのでメモリが足りないときには
 * 別オーバーレイ領域に引越しする
 */
//============================================================================================
#include <gflib.h>

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_data.h"
#include "gamesystem/game_event.h"

#include "event_gameclear.h"  //GAMECLEAR_MODE

#include "system/main.h"  //HEAPID_〜

#include "script.h"       //SCRIPT_CallScript
#include "event_gamestart.h"

#include "demo/demo3d.h"  //Demo3DProcData etc.
#include "app/local_tvt_sys.h"  //LocalTvt_ProcData etc.

#include "sound/pm_sndsys.h"  //PMSND_

#include "move_pokemon.h"

#include "proc_gameclear_save.h"

#include "savedata/save_tbl.h"
#include "savedata/record.h"
#include "savedata/gametime.h"
#include "savedata/playtime.h"
#include "ev_time.h"

//==============================================================================================
//==============================================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
typedef struct {
  GAMESYS_WORK * gsys;
  GAMECLEAR_MODE clear_mode;
  BOOL saveSuccessFlag;
  const MYSTATUS * mystatus;

  GAMECLEAR_MSG_PARAM para_child;
}GAMECLEAR_WORK;


//============================================================================================
//============================================================================================
// メインシーケンス
enum {
	GMCLEAR_SEQ_INIT = 0,			// 初期化
	GMCLEAR_SEQ_DENDOU_DEMO,		// 殿堂入りデモ
	GMCLEAR_SEQ_SAVE_START,			// セーブ開始
	GMCLEAR_SEQ_SAVE_MESSAGE,		// セーブ中メッセージ表示
	GMCLEAR_SEQ_SAVE_MAIN,			// セーブメイン
	GMCLEAR_SEQ_SAVE_END,			// セーブ終了
	GMCLEAR_SEQ_WAIT1,				// ウェイト１
	GMCLEAR_SEQ_FADE_OUT,			// フェードアウトセット
	GMCLEAR_SEQ_BGM_FADE_OUT,		// BGMフェードアウトセット
	GMCLEAR_SEQ_WAIT2,				// ウェイト２
	GMCLEAR_SEQ_ENDING_DEMO,		// エンディングデモ
	GMCLEAR_SEQ_END,				// 終了
};



//============================================================================================
//============================================================================================

//-----------------------------------------------------------------------------
/**
 * @brief	ゲームクリアイベント
 * @param	event		イベント制御ワークへのポインタ
 * @retval	TRUE		イベント終了
 * @retval	FALSE		イベント継続中
 *
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT GMEVENT_GameClear(GMEVENT * event, int * seq, void *work)
{
  GAMECLEAR_WORK * gcwk = work;
  GMEVENT * call_event;
  GAMEDATA * gamedata = GAMESYSTEM_GetGameData( gcwk->gsys );
  SAVE_CONTROL_WORK* save = GAMEDATA_GetSaveControlWork( gamedata );

	switch (*seq) {
	case GMCLEAR_SEQ_INIT:
    PMSND_FadeOutBGM( 30 );
    { //フィールドマップを終了させる
      FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( gcwk->gsys );
      GMEVENT * new_event = DEBUG_EVENT_GameEnd( gcwk->gsys, fieldmap );
      GMEVENT_CallEvent( event, new_event );
    }
		(*seq) ++;
		break;

	case GMCLEAR_SEQ_DENDOU_DEMO:
    if ( PMSND_CheckFadeOnBGM() == FALSE ) break;
    GFL_FADE_SetMasterBrightReq( 
        GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 
        0, 16, -16 );
    SCRIPT_CallGameClearScript( gcwk->gsys, HEAPID_PROC ); 
		(*seq) ++;
		break;

	case GMCLEAR_SEQ_SAVE_START:
    {
      RTCDate date;
      POKEPARTY* party;

      RTC_GetDate( &date );
      party = GAMEDATA_GetMyPokemon( gamedata ); 

      // 初回クリア
      if( gcwk->clear_mode == GAMECLEAR_MODE_FIRST ) {
        DENDOU_RECORD* record; 

        record = GAMEDATA_GetDendouRecord( gamedata ); 
        DendouRecord_Add( record, party, &date, HEAPID_PROC );
      }
      // 殿堂入り
      else {
        DENDOU_SAVEDATA* dendouData;
        LOAD_RESULT result; 

        result = SaveControl_Extra_Load( save, SAVE_EXTRA_ID_DENDOU, HEAPID_PROC );
        dendouData = SaveControl_Extra_DataPtrGet( save, SAVE_EXTRA_ID_DENDOU, EXGMDATA_ID_DENDOU );
        DendouData_AddRecord( dendouData, party, &date, HEAPID_PROC ); 
        SaveControl_Extra_SaveAsyncInit( save, SAVE_EXTRA_ID_DENDOU ); 
      }
    }
    // 電光掲示板にチャンピオンニュースを表示
    {
      MISC* misc;
      misc = GAMEDATA_GetMiscWork( gamedata );
      MISC_SetChampNewsMinutes( misc, 60*24 );
    }
    // レコード登録
    if( gcwk->clear_mode == GAMECLEAR_MODE_FIRST ) {
      RECORD* record = GAMEDATA_GetRecordPtr( gamedata );
      GMTIME* gmtime = SaveData_GetGameTime( save );
      EVTIME_SetGameClearDateTime( gamedata ); // 初回クリアの日時を記録
      {
        PLAYTIME* ptime = SaveData_GetPlayTime( save );
        u32 hour = PLAYTIME_GetHour( ptime );
        u32 minute = PLAYTIME_GetMinute( ptime );
        u32 second = PLAYTIME_GetSecond( ptime );
        u32 time = hour * 10000 + minute * 100 + second;
        GF_ASSERT( hour <= PTIME_HOUR_MAX );
        GF_ASSERT( minute <= PTIME_MINUTE_MAX );
        GF_ASSERT( second <= PTIME_SECOND_MAX );
        RECORD_Set( record, RECID_CLEAR_TIME, time );
      }
    }
    else if( gcwk->clear_mode == GAMECLEAR_MODE_DENDOU ) {
      RECORD* record = GAMEDATA_GetRecordPtr( gamedata );
      RECORD_Add( record, RECID_DENDOU_CNT, 1 );
    }
    (*seq) ++;
    break;

  case GMCLEAR_SEQ_SAVE_MESSAGE:
    GMEVENT_CallProc(
        event, FS_OVERLAY_ID(gameclear_demo), &GameClearMsgProcData, &gcwk->para_child );
    (*seq) ++;
		break;

	case GMCLEAR_SEQ_SAVE_MAIN:
    // 殿堂入りデータをセーブ
    if( gcwk->clear_mode == GAMECLEAR_MODE_DENDOU ) {
      SAVE_RESULT save_ret;

      // セーブ実行
      save_ret = SaveControl_Extra_SaveAsyncMain( save, SAVE_EXTRA_ID_DENDOU ); 
      // セーブ終了
      if( save_ret == SAVE_RESULT_OK || save_ret == SAVE_RESULT_NG ) { 
        SaveControl_Extra_Unload( save, SAVE_EXTRA_ID_DENDOU ); // 外部データを解放
        *seq = GMCLEAR_SEQ_END;
      }
    }
    else {
      *seq = GMCLEAR_SEQ_END;
    }
		break;

	case GMCLEAR_SEQ_END:
    return GMEVENT_RES_FINISH;

  case GMCLEAR_SEQ_END + 1:
    /* 無限ループ */
    return GMEVENT_RES_CONTINUE;

	}
	return GMEVENT_RES_CONTINUE;
}


//-----------------------------------------------------------------------------
/**
 * @brief	イベントコマンド：通常全滅処理
 * @param	event		イベント制御ワークへのポインタ
 */
//-----------------------------------------------------------------------------
GMEVENT * EVENT_GameClear( GAMESYS_WORK * gsys, GAMECLEAR_MODE mode )
{
  GAMEDATA * gamedata = GAMESYSTEM_GetGameData( gsys );
	GMEVENT * event;
  GAMECLEAR_WORK * gcwk;
  event = GMEVENT_Create( gsys, NULL, GMEVENT_GameClear, sizeof(GAMECLEAR_WORK) );
  gcwk = GMEVENT_GetEventWork( event );
  gcwk->gsys = gsys;
  gcwk->clear_mode = mode;
  gcwk->mystatus = GAMEDATA_GetMyStatus( gamedata );

  //子プロセスに渡すパラメータ
  gcwk->para_child.gsys = gsys;
  gcwk->para_child.clear_mode = mode;

  //移動ポケモン復活チェック
  MP_RecoverMovePoke( gamedata );

  return event;
}
