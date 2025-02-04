///////////////////////////////////////////////////////////////////////////////////
/**
 * @brief フィールドのサウンド操作
 * @file  field_sound_system.c
 * @author obata
 * @date   2009.12.25
 */
///////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "gamesystem/gamedata_def.h"
#include "gamesystem/game_data.h"
#include "field_sound_proc.h"
#include "field_sound_system.h"
#include "sound/pm_sndsys.h"
#include "player_volume_fader.h"
#include "ringtone_sys.h" 
#include "field_envse_data.h"

#include "test/performance.h"
#include "net/net_whpipe.h"


//================================================================================= 
// ■デバッグ
//================================================================================= 
//#define DEBUG_PRINT_ON         // デバッグ出力スイッチ
#define PRINT_NO            (1)  // printf出力先コンソール番号

//#define DEBUG_DIV_LOAD_COUNT_ON // 分割読み込み回数カウントスイッチ
#ifdef DEBUG_DIV_LOAD_COUNT_ON // 分割読み込み回数カウンタ
static int DivLoadCount = 0;
#endif


//================================================================================= 
// ■定数
//================================================================================= 
#define BGM_NONE            (0)  // BGM番号の無効値
#define REQUEST_QUEUE_SIZE (10)  // バッファできるリクエストの数

// 内部状態
typedef enum {
  FSND_STATE_STOP,              // 停止中
  FSND_STATE_PLAY,              // 再生中
  FSND_STATE_WAIT,              // 待機中
  FSND_STATE_FADE_OUT,          // BGMフェードアウト中
  FSND_STATE_FADE_IN,           // BGMフェードイン中
  FSND_STATE_PUSH,              // BGM退避中(フェードアウト完了待ち)
  FSND_STATE_POP_out,           // BGM復帰中(フェードアウト完了待ち)
  FSND_STATE_POP_in,            // BGM復帰中(pop実行待ち)
  FSND_STATE_CHANGE_out,        // BGM変更中(フェードアウト完了待ち)
  FSND_STATE_CHANGE_load,       // BGM変更中(分割ロード完了待ち)
  FSND_STATE_CHANGE_in,         // BGM変更中(フェードイン完了待ち)
  FSND_STATE_CHANGE_PUSH_out,   // BGM変更→退避中(フェードアウト完了待ち)
  FSND_STATE_CHANGE_PUSH_load,  // BGM変更→退避中(分割ロード完了待ち)
  FSND_STATE_STAND_BY_out,      // BGM再生準備中(フェードアウト完了待ち)
  FSND_STATE_STAND_BY_load,     // BGM再生準備中(分割ロード完了待ち)
  FSND_STATE_NUM
} FSND_STATE;

//---------------------------------------------------------------------------------
// 各内部状態に対する, リクエストの可不可テーブル
//
// RequestAcceptTable[ システム状態 ][ リクエスト ] が TRUE の時,
// システムは そのリクエストを処理することが出来る.
//
static const u8 RequestAcceptTable[ FSND_STATE_NUM ][ FSND_BGM_REQUEST_NUM ] = 
{
                        /* NONE    F_IN    F_OUT   PUSH    POP     CHANGE  S_BY    F_PLAY */
  /* STOP             */{  FALSE,  FALSE,  FALSE,  FALSE,   TRUE,   TRUE,   TRUE,   TRUE,  },
  /* PLAY             */{  FALSE,   TRUE,   TRUE,   TRUE,   TRUE,   TRUE,   TRUE,   TRUE,  },
  /* WAIT             */{  FALSE,   TRUE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,   TRUE,  },
  /* FADE_OUT         */{  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  },
  /* FALDE_IN         */{  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  },
  /* PUSH             */{  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  },
  /* POP_out          */{  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  },
  /* POP_in           */{  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  },
  /* CHANGE_out       */{  FALSE,  FALSE,  FALSE,   TRUE,  FALSE,   TRUE,  FALSE,  FALSE,  },
  /* CHANGE_load      */{  FALSE,  FALSE,  FALSE,   TRUE,  FALSE,   TRUE,  FALSE,  FALSE,  },
  /* CHANGE_in        */{  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,   TRUE,  FALSE,  FALSE,  },
  /* CHANGE_PUSH_out  */{  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  },
  /* CHANGE_PUSH_load */{  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  },
  /* STAND_BY_out     */{  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  },
  /* STAND_BY_load    */{  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  },
};

//---------------------------------------------------------------------------------
//  新規リクエストによるデッドロック テーブル
//
//  DeadRockTable[ 最終リクエスト ][ 新規リクエスト ] が FALSE の時, 
//  新規リクエストを受理することが出来ずに, デッドロック状態となる.
//
static const u8 DeadRockTable[ FSND_BGM_REQUEST_NUM ][ FSND_BGM_REQUEST_NUM ] = 
{
              /* NONE    F_IN    F_OUT   PUSH    POP     CHANGE  S_BY    F_PLAY */
  /* NONE   */{  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  }, 
  /* F_IN   */{  FALSE,   TRUE,   TRUE,   TRUE,   TRUE,   TRUE,   TRUE,   TRUE,  },
  /* F_OUT  */{  FALSE,   TRUE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,   TRUE,  },
  /* PUSH   */{  FALSE,  FALSE,  FALSE,  FALSE,   TRUE,   TRUE,   TRUE,   TRUE,  },
  /* POP    */{  FALSE,   TRUE,   TRUE,   TRUE,   TRUE,   TRUE,   TRUE,   TRUE,  },
  /* CHANGE */{  FALSE,   TRUE,   TRUE,   TRUE,   TRUE,   TRUE,   TRUE,   TRUE,  },
  /* S_BY   */{  FALSE,   TRUE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,   TRUE,  },
  /* F_PLAY */{  FALSE,   TRUE,   TRUE,   TRUE,   TRUE,   TRUE,   TRUE,   TRUE,  },
};


//=================================================================================
// ■リクエストの詳細データ
//=================================================================================
typedef struct {

  FSND_BGM_REQUEST request;  // リクエストの種類
  u32             soundIdx;  // BGM No.
  u16         fadeOutFrame;  // 再生中のBGMのフェードアウト フレーム数
  u16          fadeInFrame;  // 再生するBGMのフェードイン フレーム数

} FSND_REQUEST_DATA;


//=================================================================================
// ■環境音SE管理
//=================================================================================
#define FSND_ENVSE_PLAYER_MAX (2)
#define FSND_ENVSE_NONE (0xffff)  // SEなし用定数
#define FSND_ENVSE_VOL_NONE (0xffff)  // SEボリュームなし用定数
typedef struct {
  u16 envse_tbl[ FSND_ENVSE_PLAYER_MAX ];
  u16 envse_vol_tbl[ FSND_ENVSE_PLAYER_MAX ];
  u32  pause; //FSND_ENVSE_PAUSE_TYPE
} FSND_ENVSE_DATA;


//================================================================================= 
// ■フィールドサウンド管理ワーク
//================================================================================= 
struct _FIELD_SOUND {

  GAMEDATA* gameData;

  // 内部状態
  FSND_STATE state;  // 現在の状態

  // 内部情報
  u32  currentBGM;    // 現在再生中のBGM
  u32  requestBGM;    // リクエストされたBGM
  u32  loadBGM;       // 分割ロード中のBGM
  u32  loadSeq;       // 分割ロードのシーケンス
  u16  fadeInFrame;   // フェードイン フレーム数 
  u16  fadeOutFrame;  // フェードアウト フレーム数
  FSND_BGM_REQUEST request;  // 処理中のリクエスト

  // BGM 退避
  FSND_PUSHCOUNT pushCount;                    // 退避数
  u32            pushBGM[FSND_PUSHCOUNT_MAX];  // 退避中のBGN No.

  // リクエストキュー
  FSND_REQUEST_DATA requestData[ REQUEST_QUEUE_SIZE ];  // リクエスト配列
  u8                requestHeadPos;  // キューの先頭位置( 先頭リクエストの位置 )
  u8                requestTailPos;  // キューの末尾位置( 新規リクエストの追加位置 )

  // プレイヤーボリューム管理
  PLAYER_VOLUME_FADER* playerVolumeFader;
  RINGTONE_SYS*        ringToneSys;

  // 環境SE管理
  FSND_ENVSE_DATA envse;

};


//================================================================================= 
// ■非公開関数
//================================================================================= 
// システムの初期化
static void InitFieldSoundSystem( FIELD_SOUND* fieldSound, GAMEDATA* gameData );
static void ResetFieldSoundSystem( FIELD_SOUND* fieldSound, GAMEDATA* gameData );

// リクエストキューの操作
static void RequestQueueCheck( FIELD_SOUND* fieldSound );
static const FSND_REQUEST_DATA* GetHeadRequest( const FIELD_SOUND* fieldSound );
static const FSND_REQUEST_DATA* GetNextRequest( const FIELD_SOUND* fieldSound );
static const FSND_REQUEST_DATA* GetTailRequest( const FIELD_SOUND* fieldSound );
static BOOL QueueHaveRequest( const FIELD_SOUND* fieldSound );
static u8 GetRequestCountInQueue( const FIELD_SOUND* fieldSound, FSND_BGM_REQUEST request );
static void RegisterNewRequest( FIELD_SOUND* fieldSound, const FSND_REQUEST_DATA* requestData );
static void RemoveHeadRequest( FIELD_SOUND* fieldSound );
// キューにリクエストを登録できるかどうかの判定
static BOOL CanRegisterRequest( const FIELD_SOUND* fieldSound, FSND_BGM_REQUEST request );

// リクエストの発行
static void ThrowRequest( FIELD_SOUND* fieldSound, const FSND_REQUEST_DATA* requestData );
static void ThrowRequest_FADE_IN( FIELD_SOUND* fieldSound, u16 fadeInFrame );
static void ThrowRequest_FADE_OUT( FIELD_SOUND* fieldSound, u16 fadeOutFrame );
static void ThrowRequest_PUSH( FIELD_SOUND* fieldSound, u16 fadeOutFrame );
static void ThrowRequest_POP( FIELD_SOUND* fieldSound, u16 fadeOutFrame, u16 fadeInFrame );
static void ThrowRequest_CHANGE( FIELD_SOUND* fieldSound, u32 soundIdx, u16 fadeOutFrame, u16 fadeInFrame );
static void ThrowRequest_STAND_BY( FIELD_SOUND* fieldSound, u32 soundIdx, u16 fadeOutFrame );
static void ThrowRequest_FORCE_PLAY( FIELD_SOUND* fieldSound, u16 soundIdx );
// システムがリクエストを受理できるかどうかの判定
static BOOL CanAcceptRequest( const FIELD_SOUND* fieldSound, FSND_BGM_REQUEST request );
// リクエストに対する動作
static void RequestCheck_STOP            ( FIELD_SOUND* fieldSound );
static void RequestCheck_PLAY            ( FIELD_SOUND* fieldSound );
static void RequestCheck_WAIT            ( FIELD_SOUND* fieldSound );
static void RequestCheck_FADE_OUT        ( FIELD_SOUND* fieldSound );
static void RequestCheck_FADE_IN         ( FIELD_SOUND* fieldSound );
static void RequestCheck_PUSH            ( FIELD_SOUND* fieldSound );
static void RequestCheck_POP_out         ( FIELD_SOUND* fieldSound );
static void RequestCheck_POP_in          ( FIELD_SOUND* fieldSound );
static void RequestCheck_CHANGE_out      ( FIELD_SOUND* fieldSound );
static void RequestCheck_CHANGE_load     ( FIELD_SOUND* fieldSound );
static void RequestCheck_CHANGE_in       ( FIELD_SOUND* fieldSound );
static void RequestCheck_CHANGE_PUSH_out ( FIELD_SOUND* fieldSound );
static void RequestCheck_CHANGE_PUSH_load( FIELD_SOUND* fieldSound );
static void RequestCheck_STAND_BY_out    ( FIELD_SOUND* fieldSound );
static void RequestCheck_STAND_BY_load   ( FIELD_SOUND* fieldSound );
// 内部動作
static void Main_STOP            ( FIELD_SOUND* fieldSound );
static void Main_PLAY            ( FIELD_SOUND* fieldSound );
static void Main_WAIT            ( FIELD_SOUND* fieldSound );
static void Main_FADE_OUT        ( FIELD_SOUND* fieldSound );
static void Main_FADE_IN         ( FIELD_SOUND* fieldSound );
static void Main_PUSH            ( FIELD_SOUND* fieldSound );
static void Main_POP_out         ( FIELD_SOUND* fieldSound );
static void Main_POP_in          ( FIELD_SOUND* fieldSound );
static void Main_CHANGE_out      ( FIELD_SOUND* fieldSound );
static void Main_CHANGE_load     ( FIELD_SOUND* fieldSound );
static void Main_CHANGE_in       ( FIELD_SOUND* fieldSound );
static void Main_CHANGE_PUSH_out ( FIELD_SOUND* fieldSound );
static void Main_CHANGE_PUSH_load( FIELD_SOUND* fieldSound );
static void Main_STAND_BY_out    ( FIELD_SOUND* fieldSound );
static void Main_STAND_BY_load   ( FIELD_SOUND* fieldSound );
// 内部状態の変更
static void ChangeState( FIELD_SOUND* fieldSound, FSND_STATE nextState );
// リクエスト受理状態の更新
static void ResetRequest( FIELD_SOUND* fieldSound );
static void CancelRequest( FIELD_SOUND* fieldSound );
static void FinishRequest( FIELD_SOUND* fieldSound );

// BGMの操作
static void FadeInBGM ( FIELD_SOUND* fieldSound );
static void FadeOutBGM( FIELD_SOUND* fieldSound );
static void PushBGM( FIELD_SOUND* fieldSound );
static void PopBGM ( FIELD_SOUND* fieldSound );
static BOOL DivLoadBGM_start( FIELD_SOUND* fieldSound );
static BOOL DivLoadBGM_load( FIELD_SOUND* fieldSound );
static void ForcePlayBGM( FIELD_SOUND* fieldSound );

// ビーコンスキャンの一時停止
static void PauseBeaconScan( void ); // 一時停止する
static void StartBeaconScan( void ); // 一時停止を解除する

// セーブ中かどうか
static BOOL CheckSaveNow( const FIELD_SOUND* fieldSound );

// デバッグ
#ifdef DEBUG_PRINT_ON
static void DebugPrint_RequestQueue( const FIELD_SOUND* fieldSound );
static void DebugPrint_pushedBGM( const FIELD_SOUND* fieldSound );
static void DebugPrint_AllInfo( const FIELD_SOUND* fieldSound );
#endif

//常駐SE判定のためのコールバック関数
static BOOL checkEnableSE( u32 sndIndex );
//static BOOL checkStaticEntrySE( u32 sndIndex );


//================================================================================= 
// ■動作関数テーブル
//================================================================================= 
// リクエストに対する反応
static void (*RequestCheckFunc[ FSND_STATE_NUM ])( FIELD_SOUND* fsnd ) = 
{
  RequestCheck_STOP,            // FSND_STATE_STOP
  RequestCheck_PLAY,            // FSND_STATE_PLAY
  RequestCheck_WAIT,            // FSND_STATE_WAIT
  RequestCheck_FADE_OUT,        // FSND_STATE_FADE_OUT
  RequestCheck_FADE_IN,         // FSND_STATE_FADE_IN
  RequestCheck_PUSH,            // FSND_STATE_PUSH
  RequestCheck_POP_out,         // FSND_STATE_POP_out
  RequestCheck_POP_in,          // FSND_STATE_POP_in
  RequestCheck_CHANGE_out,      // FSND_STATE_CHANGE_out
  RequestCheck_CHANGE_load,     // FSND_STATE_CHANGE_load
  RequestCheck_CHANGE_in,       // FSND_STATE_CHANGE_in
  RequestCheck_CHANGE_PUSH_out, // FSND_STATE_CHANGE_PUSH_out
  RequestCheck_CHANGE_PUSH_load,// FSND_STATE_CHANGE_PUSH_load
  RequestCheck_STAND_BY_out,    // FSND_STATE_STAND_BY_out
  RequestCheck_STAND_BY_load,   // FSND_STATE_STAND_BY_load
};

// システム処理
static void (*MainFunc[ FSND_STATE_NUM ])( FIELD_SOUND* fsnd ) = 
{
  Main_STOP,             // FSND_STATE_STOP
  Main_PLAY,             // FSND_STATE_PLAY
  Main_WAIT,             // FSND_STATE_WAIT
  Main_FADE_OUT,         // FSND_STATE_FADE_OUT
  Main_FADE_IN,          // FSND_STATE_FADE_IN
  Main_PUSH,             // FSND_STATE_PUSH
  Main_POP_out,          // FSND_STATE_POP_out
  Main_POP_in,           // FSND_STATE_POP_in
  Main_CHANGE_out,       // FSND_STATE_CHANGE_out
  Main_CHANGE_load,      // FSND_STATE_CHANGE_load
  Main_CHANGE_in,        // FSND_STATE_CHANGE_in
  Main_CHANGE_PUSH_out,  // FSND_STATE_CHANGE_PUSH_out
  Main_CHANGE_PUSH_load, // FSND_STATE_CHANGE_PUSH_load
  Main_STAND_BY_out,     // FSND_STATE_STAND_BY_out
  Main_STAND_BY_load,    // FSND_STATE_STAND_BY_load
};


//================================================================================= 
// ■システム作成/破棄
//================================================================================= 

//---------------------------------------------------------------------------------
/**
 * @brief フィールドサウンド作成
 *
 * @param gameData
 * @param heapID ヒープID
 *
 * @return 作成したフィールドサウンド
 */
//---------------------------------------------------------------------------------
FIELD_SOUND* FIELD_SOUND_Create( GAMEDATA* gameData, HEAPID heapID )
{
  FIELD_SOUND* fieldSound;

  // 作成
  fieldSound = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_SOUND) );

  // 初期化
  InitFieldSoundSystem( fieldSound, gameData );
  fieldSound->playerVolumeFader = PLAYER_VOLUME_FADER_Create( heapID, PLAYER_BGM );
  fieldSound->ringToneSys = RINGTONE_SYS_Create( heapID, fieldSound->playerVolumeFader );

  //常駐判定用コールバックを登録
  PMSND_SetPlayableCallBack( checkEnableSE );

  return fieldSound;
}

//---------------------------------------------------------------------------------
/**
 * @brief フィールドサウンド破棄
 *
 * @param fieldSound フィールドサウンド
 */
//---------------------------------------------------------------------------------
void FIELD_SOUND_Delete( FIELD_SOUND* fieldSound )
{
  PMSND_SetPlayableCallBack( NULL );
  RINGTONE_SYS_Delete( fieldSound->ringToneSys );
  PLAYER_VOLUME_FADER_Delete( fieldSound->playerVolumeFader );
  GFL_HEAP_FreeMemory( fieldSound );
}


//=================================================================================
// ■取得
//=================================================================================

//---------------------------------------------------------------------------------
/**
 * @brief BGMプッシュ回数を取得
 *
 * @param fieldSound
 *
 * @return 積まれているBGMの数
 */
//---------------------------------------------------------------------------------
FSND_PUSHCOUNT FIELD_SOUND_GetBGMPushCount( const FIELD_SOUND* fieldSound )
{
  return fieldSound->pushCount;
}

//---------------------------------------------------------------------------------
/**
 * @brief 現在登録されているリクエストが完了した時点での, BGM退避数を取得する
 *
 * @param fieldSound
 *
 * @return 全てのリクエストを完了した時点での, 積まれているBGMの数
 */
//---------------------------------------------------------------------------------
FSND_PUSHCOUNT FIELD_SOUND_GetBGMPushCount_atAllRequestFinished( const FIELD_SOUND* fieldSound )
{
  FSND_PUSHCOUNT pushCount;

  // 現在積まれているBGMの数を取得
  pushCount = fieldSound->pushCount;

  // キューに登録されている PUSH, POP リクエストの分を考慮
  pushCount += GetRequestCountInQueue( fieldSound, FSND_BGM_REQUEST_PUSH );
  pushCount -= GetRequestCountInQueue( fieldSound, FSND_BGM_REQUEST_POP );

  // PUSH リクエスト処理中
  if( fieldSound->request == FSND_BGM_REQUEST_PUSH ) {
    // PUSH 処理が未実行
    if( (fieldSound->state == FSND_STATE_PUSH) ||
        (fieldSound->state == FSND_STATE_CHANGE_PUSH_out) ||
        (fieldSound->state == FSND_STATE_CHANGE_PUSH_load) ) {
      pushCount++;
    }
  }

  // POP リクエスト処理中
  if( fieldSound->request == FSND_BGM_REQUEST_POP ) {
    // POP処理が未実行
    if( fieldSound->state == FSND_STATE_POP_out ) {
      pushCount--;
    }
  }

  GF_ASSERT( FSND_PUSHCOUNT_NONE <= pushCount );
  GF_ASSERT( pushCount <= FSND_PUSHCOUNT_MAX );
  return pushCount;
}

//---------------------------------------------------------------------------------
/**
 * @brief 現在登録されているリクエストが完了した時点での, 再生中のBGMを取得する
 *
 * @param fieldSound
 *
 * @return 全てのリクエストを完了した時点で再生中となるBGM
 */
//---------------------------------------------------------------------------------
u32 FIELD_SOUND_GetPlayingBGM_atAllRequestFinished( const FIELD_SOUND* fieldSound )
{
  u32 lastBGM = fieldSound->currentBGM;

  // 処理中のリクエストを考慮
  if( fieldSound->requestBGM != BGM_NONE ) {
    lastBGM = fieldSound->requestBGM; // 再生リクエスト処理中
  }
  if( fieldSound->loadBGM != BGM_NONE ) {
    lastBGM = fieldSound->loadBGM; // BGM ロード中
  }

  // キューにあるリクエストを考慮
  {
    int i;
    int pos = fieldSound->requestHeadPos;
    FSND_PUSHCOUNT pushCount = fieldSound->pushCount;
    u32 pushBGM[ FSND_PUSHCOUNT_MAX ];

    // BGM 退避状態をコピー
    pushCount = fieldSound->pushCount;
    for( i=0; i<pushCount; i++ ) {
      pushBGM[i] = fieldSound->pushBGM[i];
    }

    // 全リクエストの動作をシミュレート
    while( pos != fieldSound->requestTailPos )
    {
      const FSND_REQUEST_DATA* reqData = &( fieldSound->requestData[ pos ] );

      if( reqData->request == FSND_BGM_REQUEST_PUSH ) {
        pushBGM[ pushCount++ ] = lastBGM;
      }
      else if( reqData->request == FSND_BGM_REQUEST_POP ) {
        lastBGM = pushBGM[ --pushCount ];
      }
      else if( (reqData->request == FSND_BGM_REQUEST_CHANGE) ||
               (reqData->request == FSND_BGM_REQUEST_STAND_BY) ||
               (reqData->request == FSND_BGM_REQUEST_FORCE_PLAY) ) {
        lastBGM = reqData->soundIdx;
      }

      pos = ( pos + 1 ) % REQUEST_QUEUE_SIZE;
    }
  }

  return lastBGM;
}

//---------------------------------------------------------------------------------
/**
 * @brief BGMフェード検出
 *
 * @param fieldSound
 * 
 * @return フェード中なら TRUE
 *         そうでなければ FALSE
 */
//---------------------------------------------------------------------------------
BOOL FIELD_SOUND_IsBGMFade( const FIELD_SOUND* fieldSound )
{
  if( (fieldSound->state == FSND_STATE_FADE_IN) ||
      (fieldSound->state == FSND_STATE_FADE_OUT) ||
      (fieldSound->state == FSND_STATE_POP_out) ||
      (fieldSound->state == FSND_STATE_POP_in) ||
      (fieldSound->state == FSND_STATE_PUSH) ||
      (fieldSound->state == FSND_STATE_CHANGE_out) ||
      (fieldSound->state == FSND_STATE_CHANGE_in) ||
      (fieldSound->state == FSND_STATE_CHANGE_PUSH_out) ||
      (fieldSound->state == FSND_STATE_STAND_BY_out) )
  {
    return TRUE;
  } 
  return FALSE;
}

//---------------------------------------------------------------------------------
/**
 * @brief リクエスト保持判定
 *
 * @param fieldSound
 *
 * @return 未処理のリクエストを持っている場合 TRUE
 *         そうでなければ（全てのリクエストに対する処理が完了している場合）FALSE
 */
//---------------------------------------------------------------------------------
BOOL FIELD_SOUND_HaveRequest( const FIELD_SOUND* fieldSound )
{
  // キューにリクエストがない and リクエスト処理中でない
  if( (QueueHaveRequest(fieldSound) == FALSE) &&
      (fieldSound->request == FSND_BGM_REQUEST_NONE) )
  {
    return FALSE;
  }
  return TRUE;
}

//---------------------------------------------------------------------------------
/**
 * @brief BGMの復帰が全て終わったかどうかをチェックする
 *
 * @param fieldSound
 *
 * @return すべてのBGMの復帰リクエストが完了している場合 TRUE
 *         そうでなければ FALSE
 */
//---------------------------------------------------------------------------------
BOOL FIELD_SOUND_CheckBGMPopFinished( const FIELD_SOUND* fieldSound )
{
  // POPリクエスト処理中
  if( fieldSound->request == FSND_BGM_REQUEST_POP ) {
    // まだPOPしていない
    if( fieldSound->state == FSND_STATE_POP_out ) {
      return FALSE;
    }
  }

  // キューにPOPリクエストが登録されている
  if( GetRequestCountInQueue( fieldSound, FSND_BGM_REQUEST_POP ) != 0 ) {
    return FALSE;
  }

  return TRUE;
}


//=================================================================================
// ■リクエスト
//=================================================================================

//---------------------------------------------------------------------------------
/**
 * @brief リクエストをリクエストキューに登録する
 *
 * @param fieldSound
 * @param request      登録するリクエスト
 * @param soundIdx     再生するBGM
 * @param fadeOutFrame 再生中のBGMのフェードアウト フレーム数
 * @param fadeInFrame  変更後のBGMのフェードイン フレーム数
 */
//---------------------------------------------------------------------------------
void FIELD_SOUND_RegisterRequest( FIELD_SOUND* fieldSound, 
                                  FSND_BGM_REQUEST request,
                                  u32 soundIdx, 
                                  u16 fadeOutFrame, 
                                  u16 fadeInFrame )
{ 
  FSND_REQUEST_DATA requestData;

  // リクエストデータ作成
  requestData.request = request;
  requestData.soundIdx = soundIdx;
  requestData.fadeOutFrame = fadeOutFrame;
  requestData.fadeInFrame = fadeInFrame;

  // リクエストキューに登録
  RegisterNewRequest( fieldSound, &requestData ); 
}


//=================================================================================
// ■システム動作
//=================================================================================

//---------------------------------------------------------------------------------
/**
 * @brief BGMの状態を管理し, BGMの変更を行う
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
void FIELD_SOUND_Main( FIELD_SOUND* fieldSound )
{
  // リクエストキューからリクエストを取り出す
  RequestQueueCheck( fieldSound );

  // リクエストに対する反応
  RequestCheckFunc[ fieldSound->state ]( fieldSound );

  // 内部動作
  MainFunc[ fieldSound->state ]( fieldSound );

  // TVトランシーバー着信音処理
  RINGTONE_SYS_Main( fieldSound->ringToneSys );

  // プレイヤーボリューム フェード処理
  PLAYER_VOLUME_FADER_Main( fieldSound->playerVolumeFader );
} 

//---------------------------------------------------------------------------------
/**
 * @brief システムをリセットする
 *
 * @param fieldSound
 * @param gameData
 */
//---------------------------------------------------------------------------------
void FIELD_SOUND_Reset( FIELD_SOUND* fieldSound, GAMEDATA* gameData )
{
  ResetFieldSoundSystem( fieldSound, gameData );
}


//=================================================================================
// ■プレイヤーボリューム制御
//=================================================================================

//---------------------------------------------------------------------------------
/**
 * @brief プレイヤーボリュームを変更する
 *
 * @param fieldSound
 * @param volume     設定するボリューム[0, 127]
 * @param fadeFrame  フェード フレーム数
 */
//---------------------------------------------------------------------------------
void FIELD_SOUND_ChangePlayerVolume( FIELD_SOUND* fieldSound, u8 volume, u8 fadeFrame )
{
  PLAYER_VOLUME_FADER_SetVolume( fieldSound->playerVolumeFader, volume, fadeFrame );
}


//=================================================================================
// ■TVトランシーバー着信音制御
//=================================================================================
//---------------------------------------------------------------------------------
/**
 * @brief TVトランシーバー着信音を鳴らす
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
void FIELD_SOUND_PlayTVTRingTone( FIELD_SOUND* fieldSound )
{
  RINGTONE_SYS_Play( fieldSound->ringToneSys );
}

//---------------------------------------------------------------------------------
/**
 * @brief TVトランシーバー着信音を停止する
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
void FIELD_SOUND_StopTVTRingTone( FIELD_SOUND* fieldSound )
{
  RINGTONE_SYS_Stop( fieldSound->ringToneSys );
}


//=================================================================================
// ■環境音
//=================================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  環境音SEの再生
 *
 *	@param	fieldSound    フィールドサウンド
 *	@param	soundIdx      サウンドID
 */
//-----------------------------------------------------------------------------
void FIELD_SOUND_PlayEnvSE( FIELD_SOUND* fieldSound, u32 soundIdx )
{
  // 環境音SEかチェック
  if( FLD_ENVSE_DATA_IsEnvSE( soundIdx ) == FALSE ){
    GF_ASSERT( 0 );
    //環境音SEじゃないので、鳴らす
    PMSND_PlaySE( soundIdx );
  }else{

    // ループサウンドなら保存
    if( FLD_ENVSE_DATA_IsLoopSE( soundIdx ) ){
      SEPLAYER_ID player_ID = PMSND_GetSE_DefaultPlayerID( soundIdx );
      GF_ASSERT( (player_ID == SEPLAYER_SE1) || (player_ID == SEPLAYER_SE2) );
      fieldSound->envse.envse_tbl[ player_ID - SEPLAYER_SE1 ] = soundIdx;
      fieldSound->envse.envse_vol_tbl[ player_ID - SEPLAYER_SE1 ] = FSND_ENVSE_VOL_NONE;
    }

    // Pauseではないときにだけ鳴らす
    if( fieldSound->envse.pause == FSND_ENVSE_PAUSE_NONE ){
      PMSND_PlaySE( soundIdx );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  環境音SEの再生  ボリューム指定
 *
 *	@param	fieldSound    フィールドサウンド
 *	@param	soundIdx      サウンドID
 *	@param	vol           ボリューム
 */
//-----------------------------------------------------------------------------
void FIELD_SOUND_PlayEnvSEVol( FIELD_SOUND* fieldSound, u32 soundIdx, u32 vol )
{
  // 環境音SEかチェック
  if( FLD_ENVSE_DATA_IsEnvSE( soundIdx ) == FALSE ){
    GF_ASSERT( 0 );
    //環境音SEじゃないので、鳴らす
    PMSND_PlaySE( soundIdx );
  }else{

    // ループサウンドなら保存
    if( FLD_ENVSE_DATA_IsLoopSE( soundIdx ) ){
      SEPLAYER_ID player_ID = PMSND_GetSE_DefaultPlayerID( soundIdx );
      GF_ASSERT( (player_ID == SEPLAYER_SE1) || (player_ID == SEPLAYER_SE2) );
      fieldSound->envse.envse_tbl[ player_ID - SEPLAYER_SE1 ] = soundIdx;
      fieldSound->envse.envse_vol_tbl[ player_ID - SEPLAYER_SE1 ] = vol;
    }

    // Pauseではないときにだけ鳴らす
    if( fieldSound->envse.pause == FSND_ENVSE_PAUSE_NONE ){
      PMSND_PlaySEVolume( soundIdx, vol );
    }
  }
}

// 環境音のボリューム操作
//----------------------------------------------------------------------------
/**
 *	@brief  環境音ボリューム操作
 *
 *	@param	fieldSound  フィールドサウンド
 *	@param	soundIdx    SEインデックス
 *	@param	vol         ボリューム
 */
//-----------------------------------------------------------------------------
void FIELD_SOUND_SetEnvSEVol( FIELD_SOUND* fieldSound, u32 soundIdx, u32 vol )
{
  int i;
  // 環境音SEかチェック
  if( FLD_ENVSE_DATA_IsEnvSE( soundIdx ) == FALSE ){
    GF_ASSERT( 0 );
    return ;
  }
  
  // ボリューム変更
  for( i=0; i<FSND_ENVSE_PLAYER_MAX; i++ ){
    if( fieldSound->envse.envse_tbl[ i ] == soundIdx ){
      // ボリューム変更
      PMSND_PlayerSetVolume( i+SEPLAYER_SE1, vol );
      fieldSound->envse.envse_vol_tbl[ i ] = vol;
      break;
    }
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  環境音を停止する
 *
 *	@param	fieldSound  フィールドサウンド
 *	@param	soundIdx    サウンド
 */   
//-----------------------------------------------------------------------------
void FIELD_SOUND_StopEnvSE( FIELD_SOUND* fieldSound, u32 soundIdx )
{
  SEPLAYER_ID player_ID = PMSND_GetSE_DefaultPlayerID( soundIdx );
  GF_ASSERT( (player_ID == SEPLAYER_SE1) || (player_ID == SEPLAYER_SE2) );

  // 環境音SEかチェック
  if( FLD_ENVSE_DATA_IsEnvSE( soundIdx ) == FALSE ){
    GF_ASSERT( 0 );
    //環境音SEじゃないけれど停止する
    PMSND_StopSE_byPlayerID( player_ID );
  }else{

    // ループサウンドテーブルから破棄
    if( FLD_ENVSE_DATA_IsLoopSE( soundIdx ) ){

      // 一致したら破棄
      if( fieldSound->envse.envse_tbl[ player_ID - SEPLAYER_SE1 ] == soundIdx ){
        fieldSound->envse.envse_tbl[ player_ID - SEPLAYER_SE1 ] = FSND_ENVSE_NONE;
        fieldSound->envse.envse_vol_tbl[ player_ID - SEPLAYER_SE1 ] = FSND_ENVSE_VOL_NONE;
      }
    }

    //環境音SE停止
    PMSND_StopSE_byPlayerID( player_ID );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  環境音SEを停止する
 *
 *	@param	fieldSound  フィールドサウンド
 */
//-----------------------------------------------------------------------------
void FIELD_SOUND_PauseEnvSE( FIELD_SOUND* fieldSound, FSND_ENVSE_PAUSE_TYPE type )
{
  int i;

  if(fieldSound->envse.pause == FSND_ENVSE_PAUSE_NONE)
  {
    // 停止
    for( i=0; i<FSND_ENVSE_PLAYER_MAX; i++ ){
      if( fieldSound->envse.envse_tbl[ i ] != FSND_ENVSE_NONE ){
        // SE停止
        PMSND_StopSE_byPlayerID( i+SEPLAYER_SE1 );
      }
    }
  }
  
  fieldSound->envse.pause |= type;
  

  TOMOYA_Printf( "EnvSe Stop msk0x%x\n", type );
}

//----------------------------------------------------------------------------
/**
 *	@brief  環境音SEの再始動
 *
 *	@param	fieldSound  フィールドサウンド
 */
//-----------------------------------------------------------------------------
void FIELD_SOUND_RePlayEnvSE( FIELD_SOUND* fieldSound, FSND_ENVSE_PAUSE_TYPE type )
{
  int i;
  
  if( fieldSound->envse.pause & type ){

    // typeでのpause状態をCLEAR
    fieldSound->envse.pause &= ~type;

    if( fieldSound->envse.pause == FSND_ENVSE_PAUSE_NONE ){
      for( i=0; i<FSND_ENVSE_PLAYER_MAX; i++ ){
        if( fieldSound->envse.envse_tbl[ i ] != FSND_ENVSE_NONE ){
          if( fieldSound->envse.envse_vol_tbl[ i ] == FSND_ENVSE_VOL_NONE ){
            PMSND_PlaySE( fieldSound->envse.envse_tbl[ i ] );
          }else{
            PMSND_PlaySEVolume( fieldSound->envse.envse_tbl[ i ], 
                fieldSound->envse.envse_vol_tbl[ i ] );
          }
        }
      }
    }

    TOMOYA_Printf( "EnvSe Replay type=0x%x msk=0x%x\n", type, fieldSound->envse.pause );
  }
}

//================================================================================= 
//  常駐SEの判定
//================================================================================= 
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
static const u16 StaticEntryList[] = {
	SEQ_SE_MESSAGE,
	SEQ_SE_SELECT1 ,
	SEQ_SE_SELECT2,
	SEQ_SE_SELECT4,
	SEQ_SE_DECIDE1 ,
	SEQ_SE_DECIDE2,
	SEQ_SE_DECIDE3,
	SEQ_SE_CANCEL1,
	SEQ_SE_CANCEL2,
	SEQ_SE_CANCEL3,
	SEQ_SE_OPEN1,
	SEQ_SE_CLOSE1,
	SEQ_SE_BEEP,
	SEQ_SE_DANSA ,
	SEQ_SE_WALL_HIT,
	SEQ_SE_BICYCLE ,
	SEQ_SE_FLD_07,
	SEQ_SE_FLD_08,
	SEQ_SE_FLD_09,
	SEQ_SE_FLD_10,
	SEQ_SE_FLD_11,
	SEQ_SE_FLD_12,
	SEQ_SE_FLD_13,
	SEQ_SE_FLD_14,
	SEQ_SE_FLD_31,
	SEQ_SE_FLD_32,
	SEQ_SE_FLD_49,
	SEQ_SE_FLD_84,
	SEQ_SE_FLD_85,
	SEQ_SE_FLD_91,
	SEQ_SE_FLD_100,
	SEQ_SE_FLD_120,
	SEQ_SE_FLD_122,
	SEQ_SE_SYS_35,
  SEQ_SE_MSCL_07,
	SEQ_SE_SYS_11,
	SEQ_SE_SYS_79,
	SEQ_SE_SYS_100,
};

//---------------------------------------------------------------------------------
//  常駐登録かどうかリストを検索する
//---------------------------------------------------------------------------------
static BOOL checkStaticEntrySE( u32 sndIndex )
{
  int i;
  for ( i = 0; i < NELEMS(StaticEntryList); i ++ )
  {
    if ( StaticEntryList[i] == sndIndex )
    {
      return TRUE;
    }
  }
  return FALSE;
}

//---------------------------------------------------------------------------------
//  SndHeap内を検索する
//---------------------------------------------------------------------------------
static BOOL checkSeqInSndArc( u32 sndIndex )
{
  int i;
  const NNSSndArcSeqInfo*			seqInfo;
  const NNSSndArcBankInfo*		bnkInfo;

  seqInfo = NNS_SndArcGetSeqInfo(sndIndex);
  if(NNS_SndArcGetFileAddress(seqInfo->fileId) == NULL) return FALSE;

  bnkInfo = NNS_SndArcGetBankInfo(seqInfo->param.bankNo);
  if(NNS_SndArcGetFileAddress(bnkInfo->fileId) == NULL) return FALSE;

  for(i=0; i<4; i++){
    if(bnkInfo->waveArcNo[i] != NNS_SND_ARC_INVALID_WAVEARC_NO){
      const NNSSndArcWaveArcInfo* wavInfo = NNS_SndArcGetWaveArcInfo(bnkInfo->waveArcNo[i]);
      if(NNS_SndArcGetFileAddress(wavInfo->fileId) == NULL) return FALSE;
    }
  }
  return TRUE;
}
//---------------------------------------------------------------------------------
// 非同期読み込み中に鳴らしても大丈夫なSEかどうかをチェックする
//---------------------------------------------------------------------------------
static BOOL checkEnableSE( u32 sndIndex )
{
  if ( checkStaticEntrySE( sndIndex ) == FALSE ) return FALSE;
  if ( checkSeqInSndArc( sndIndex) == FALSE )
  {
    OS_TPrintf("!!!!!!常駐SE（%4d）なのにロードされていない!!!!!!\n", sndIndex );
    return FALSE;
  }
  return TRUE;
}


//================================================================================= 
// ■非公開関数
//================================================================================= 

//---------------------------------------------------------------------------------
/**
 * @brief システムの初期化
 *
 * @param fieldSound
 * @param gameData
 */
//---------------------------------------------------------------------------------
static void InitFieldSoundSystem( FIELD_SOUND* fieldSound, GAMEDATA* gameData )
{
  int i;

  fieldSound->gameData     = gameData;
  fieldSound->state        = FSND_STATE_STOP;
  fieldSound->request      = FSND_BGM_REQUEST_NONE;
  fieldSound->currentBGM   = BGM_NONE;
  fieldSound->requestBGM   = BGM_NONE;
  fieldSound->loadBGM      = BGM_NONE;
  fieldSound->loadSeq      = 0;
  fieldSound->fadeInFrame  = 0;
  fieldSound->fadeOutFrame = 0;
  fieldSound->pushCount    = FSND_PUSHCOUNT_NONE;

  for( i=0; i<FSND_PUSHCOUNT_MAX; i++ ) {
    fieldSound->pushBGM[i] = BGM_NONE; 
  }

  for( i=0; i<REQUEST_QUEUE_SIZE; i++ ) {
    fieldSound->requestData[i].request = FSND_BGM_REQUEST_NONE;
  } 
  fieldSound->requestHeadPos = 0;
  fieldSound->requestTailPos = 0;

  // 環境音SE管理ワークの初期化
  for( i=0; i<FSND_ENVSE_PLAYER_MAX; i++ ) {
    fieldSound->envse.envse_tbl[i] = FSND_ENVSE_NONE;
  }
  fieldSound->envse.pause = FSND_ENVSE_PAUSE_NONE;
}

//---------------------------------------------------------------------------------
/**
 * @brief システムのリセット
 *
 * @param fieldSound
 * @param gameData
 */
//---------------------------------------------------------------------------------
static void ResetFieldSoundSystem( FIELD_SOUND* fieldSound, GAMEDATA* gameData )
{
  while( fieldSound->pushCount != FSND_PUSHCOUNT_NONE )
  {
    PopBGM( fieldSound );
  }
  PMSND_StopBGM();
  InitFieldSoundSystem( fieldSound, gameData );
}

//---------------------------------------------------------------------------------
/**
 * @brief リクエストキューの先頭リクエストを取り出す
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void RequestQueueCheck( FIELD_SOUND* fieldSound )
{
  const FSND_REQUEST_DATA* headRequestData;

  // 先頭リクエスト取得
  headRequestData = GetHeadRequest( fieldSound );

  // 先頭リクエスト発行
  if( CanAcceptRequest( fieldSound, headRequestData->request ) )
  {
    ThrowRequest( fieldSound, headRequestData );
    RemoveHeadRequest( fieldSound );
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief 先頭リクエストを取得する
 *
 * @param fieldSound
 *
 * @return 先頭リクエスト
 */
//---------------------------------------------------------------------------------
const FSND_REQUEST_DATA* GetHeadRequest( const FIELD_SOUND* fieldSound )
{
  int pos;

  pos = fieldSound->requestHeadPos;

  return &fieldSound->requestData[ pos ];
}

//---------------------------------------------------------------------------------
/**
 * @brief 次のリクエストを取得する
 *
 * @param fieldSound
 *
 * @return 次のリクエスト
 */
//---------------------------------------------------------------------------------
const FSND_REQUEST_DATA* GetNextRequest( const FIELD_SOUND* fieldSound )
{
  int pos;

  pos = (fieldSound->requestHeadPos + 1) % REQUEST_QUEUE_SIZE;

  return &fieldSound->requestData[ pos ];
}

//---------------------------------------------------------------------------------
/**
 * @brief 末尾リクエストを取得する
 *
 * @param fieldSound
 *
 * @return 末尾リクエスト
 */
//---------------------------------------------------------------------------------
const FSND_REQUEST_DATA* GetTailRequest( const FIELD_SOUND* fieldSound )
{
  int pos;

  if( QueueHaveRequest( fieldSound ) == FALSE )
  { // リクエストがない
    pos = fieldSound->requestHeadPos;
  }
  else
  { // リクエストがある
    pos = ( fieldSound->requestTailPos - 1 + REQUEST_QUEUE_SIZE ) % REQUEST_QUEUE_SIZE;
  }

  return &fieldSound->requestData[ pos ];
}

//---------------------------------------------------------------------------------
/**
 * @brief キューにリクエストがあるかどうかを判定する
 *
 * @param fieldSound
 * 
 * @return キューにリクエストがある場合 TRUE
 *         キューにリクエストがない場合 FALSE
 */
//---------------------------------------------------------------------------------
static BOOL QueueHaveRequest( const FIELD_SOUND* fieldSound )
{
  const FSND_REQUEST_DATA* headRequest;

  headRequest = GetHeadRequest( fieldSound );

  return ( headRequest->request != FSND_BGM_REQUEST_NONE );
}

//---------------------------------------------------------------------------------
/**
 * @brief キューに登録された, リクエストの数を取得する
 *
 * @param fieldSound
 * @param request    リクエスト
 * 
 * @return 指定したリクエストがキューに登録されている数
 */
//---------------------------------------------------------------------------------
static u8 GetRequestCountInQueue( const FIELD_SOUND* fieldSound, FSND_BGM_REQUEST request )
{
  int pos;
  int num;

  pos = fieldSound->requestHeadPos;
  num = 0;
  while( pos != fieldSound->requestTailPos )
  {
    // 指定されたリクエストを発見
    if( fieldSound->requestData[ pos ].request == request ) { num++; }

    // 次のリクエストへ
    pos = (pos + 1) % REQUEST_QUEUE_SIZE;
  }

  return num;
}

//---------------------------------------------------------------------------------
/**
 * @brief 新規リクエストをリクエストキューに登録する
 *
 * @param fieldSound
 * @param requestData 追加するリクエストデータ
 */
//---------------------------------------------------------------------------------
void RegisterNewRequest( FIELD_SOUND* fieldSound, const FSND_REQUEST_DATA* requestData )
{
  int addPos;

  addPos = fieldSound->requestTailPos;

  // デッドロック
  if( CanRegisterRequest(fieldSound, requestData->request) == FALSE ) {
#ifdef DEBUG_PRINT_ON
    OS_TFPrintf( PRINT_NO, 
                 "FIELD-SOUND-QUEUE: Can't register request(%d)\n", requestData->request );
    DebugPrint_AllInfo( fieldSound );
#endif
    GF_ASSERT(0);
    return;
  }

  // リクエストキュー オーバーフロー
  if( fieldSound->requestData[ addPos ].request != FSND_BGM_REQUEST_NONE ) {
#ifdef DEBUG_PRINT_ON
    OS_Printf( "FIELD-SOUND: Request queue overflow\n" );
#endif
    GF_ASSERT(0);
    return;
  }

  // 登録
  fieldSound->requestData[ addPos ] = *requestData;
  fieldSound->requestTailPos = (addPos + 1) % REQUEST_QUEUE_SIZE;

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND-QUEUE: Regist new request\n" );
  DebugPrint_RequestQueue( fieldSound );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief リクエストキューにリクエストを登録できるかどうかを判定する
 *
 * @param fieldSound
 * @param request    判定するリクエスト
 *
 * @param 登録できる場合 TRUE
 *        そうでなければ　FALSE
 */
//---------------------------------------------------------------------------------
static BOOL CanRegisterRequest( const FIELD_SOUND* fieldSound, FSND_BGM_REQUEST request )
{
  const FSND_REQUEST_DATA* tailRequest;

  // 最終リクエストを取得
  tailRequest = GetTailRequest( fieldSound );

  // キューにリクエストが存在する
  if( tailRequest->request != FSND_BGM_REQUEST_NONE ) {
    return DeadRockTable[ tailRequest->request ][ request ];
  }

  // 最後のリクエストを処理中
  if( fieldSound->request != FSND_BGM_REQUEST_NONE ) {
    return DeadRockTable[ fieldSound->request ][ request ];
  }

  // リクエストが存在しなければ, 状態に依存
  return CanAcceptRequest( fieldSound, request );
}

//---------------------------------------------------------------------------------
/**
 * @brief 先頭リクエストを取り除く
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
void RemoveHeadRequest( FIELD_SOUND* fieldSound )
{
  int delPos;

  delPos = fieldSound->requestHeadPos;

  // リクエストを持っていない
  if( QueueHaveRequest( fieldSound ) == FALSE ) {
#ifdef DEBUG_PRINT_ON
    OS_Printf( "FIELD-SOUND: Can't remove head request\n" );
#endif
    GF_ASSERT(0);
    return;
  }

  // 削除
  fieldSound->requestData[ delPos ].request = FSND_BGM_REQUEST_NONE;
  fieldSound->requestHeadPos = (delPos + 1) % REQUEST_QUEUE_SIZE;

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND-QUEUE: Remove head request\n" );
  DebugPrint_RequestQueue( fieldSound );
#endif
} 

//---------------------------------------------------------------------------------
/**
 * @brief リクエストの発行
 *
 * @param fieldSound
 * @param requestData 発行するリクエスト データ
 */
//---------------------------------------------------------------------------------
static void ThrowRequest( FIELD_SOUND* fieldSound, const FSND_REQUEST_DATA* requestData )
{
  switch( requestData->request )
  {
  case FSND_BGM_REQUEST_NONE:  
    break;
  case FSND_BGM_REQUEST_FADE_IN:  
    ThrowRequest_FADE_IN( fieldSound, requestData->fadeInFrame );
    break;
  case FSND_BGM_REQUEST_FADE_OUT:
    ThrowRequest_FADE_OUT( fieldSound, requestData->fadeOutFrame );
    break;
  case FSND_BGM_REQUEST_PUSH:
    ThrowRequest_PUSH( fieldSound, requestData->fadeOutFrame );
    break;
  case FSND_BGM_REQUEST_POP:
    ThrowRequest_POP( fieldSound, requestData->fadeOutFrame, requestData->fadeInFrame );
    break;
  case FSND_BGM_REQUEST_CHANGE:
    ThrowRequest_CHANGE( fieldSound, requestData->soundIdx, 
                         requestData->fadeOutFrame, requestData->fadeInFrame );
    break;
  case FSND_BGM_REQUEST_STAND_BY:
    ThrowRequest_STAND_BY( fieldSound, requestData->soundIdx, requestData->fadeOutFrame );
    break;
  case FSND_BGM_REQUEST_FORCE_PLAY:
    ThrowRequest_FORCE_PLAY( fieldSound, requestData->soundIdx );
    break;
  default:
    GF_ASSERT(0);
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief リクエストの発行 ( FSND_BGM_REQUEST_FADE_IN )
 *
 * @param fieldSound
 * @param fadeInFrame フェードイン フレーム数
 */
//---------------------------------------------------------------------------------
static void ThrowRequest_FADE_IN( FIELD_SOUND* fieldSound, u16 fadeInFrame )
{
  // リクエストを受理できない
  if( CanAcceptRequest(fieldSound, FSND_BGM_REQUEST_FADE_IN) == FALSE )
  {
#ifdef DEBUG_PRINT_ON
    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: Can't Accept BGM fade in request\n" );
#endif
    GF_ASSERT(0);
    return;
  }

  // リクエストを受理
  fieldSound->request = FSND_BGM_REQUEST_FADE_IN;
  fieldSound->fadeInFrame = fadeInFrame;

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: accept request(FADE_IN)\n" );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief リクエストの発行 ( FSND_BGM_REQUEST_FADE_OUT )
 *
 * @param fieldSound
 * @param fadeOutFrame フェードアウト フレーム数
 */
//---------------------------------------------------------------------------------
static void ThrowRequest_FADE_OUT( FIELD_SOUND* fieldSound, u16 fadeOutFrame )
{
  // リクエストを受理できない
  if( CanAcceptRequest(fieldSound, FSND_BGM_REQUEST_FADE_OUT) == FALSE )
  {
#ifdef DEBUG_PRINT_ON
    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: Can't Accept BGM fade out request\n" );
#endif
    GF_ASSERT(0);
    return;
  }

  // リクエストを受理
  fieldSound->request = FSND_BGM_REQUEST_FADE_OUT;
  fieldSound->fadeOutFrame = fadeOutFrame;

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: accept request(FADE_OUT)\n" );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief リクエストの発行 ( FSND_BGM_REQUEST_PUSH )
 *
 * @param fieldSound
 * @param fadeOutFrame フェードアウト フレーム数
 */
//---------------------------------------------------------------------------------
static void ThrowRequest_PUSH( FIELD_SOUND* fieldSound, u16 fadeOutFrame )
{
  // リクエストを受理できない
  if( CanAcceptRequest(fieldSound, FSND_BGM_REQUEST_PUSH) == FALSE )
  {
#ifdef DEBUG_PRINT_ON
    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: Can't Accept BGM push request\n" );
#endif
    GF_ASSERT(0);
    return;
  }

  // リクエストを受理
  fieldSound->request = FSND_BGM_REQUEST_PUSH;
  fieldSound->fadeOutFrame = fadeOutFrame;

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: accept request(PUSH)\n" );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief リクエストの発行 ( FSND_BGM_REQUEST_POP )
 *
 * @param fieldSound
 * @param fadeOutFrame 再生中のBGMのフェードアウト フレーム数
 * @param fadeInFrame  復帰したBGMのフェードイン フレーム数
 */
//---------------------------------------------------------------------------------
static void ThrowRequest_POP( FIELD_SOUND* fieldSound,
                              u16 fadeOutFrame, u16 fadeInFrame )
{
  // リクエストを受理できない
  if( CanAcceptRequest(fieldSound, FSND_BGM_REQUEST_POP) == FALSE )
  {
#ifdef DEBUG_PRINT_ON
    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: Can't Accept BGM pop request\n" );
#endif
    GF_ASSERT(0);
    return;
  }

  // リクエストを受理
  fieldSound->request = FSND_BGM_REQUEST_POP;
  fieldSound->fadeInFrame = fadeInFrame;
  fieldSound->fadeOutFrame = fadeOutFrame;

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: accept request(POP)\n" );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief リクエストの発行 ( FSND_BGM_REQUEST_CHANGE )
 *
 * @param fieldSound
 * @param soundIdx     再生するBGM
 * @param fadeOutFrame 再生中のBGMのフェードアウト フレーム数
 * @param fadeInFrame  再生するBGMのフェードイン フレーム数
 */
//---------------------------------------------------------------------------------
static void ThrowRequest_CHANGE( FIELD_SOUND* fieldSound, u32 soundIdx, 
                                 u16 fadeOutFrame, u16 fadeInFrame )
{
  // リクエストを受理できない
  if( CanAcceptRequest(fieldSound, FSND_BGM_REQUEST_CHANGE) == FALSE )
  {
#ifdef DEBUG_PRINT_ON
    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: Can't Accept BGM change request\n" );
#endif
    GF_ASSERT(0);
    return;
  } 

  // リクエストを受理
  fieldSound->request = FSND_BGM_REQUEST_CHANGE;
  fieldSound->requestBGM = soundIdx;
  fieldSound->fadeInFrame = fadeInFrame;
  fieldSound->fadeOutFrame = fadeOutFrame;

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: accept request(CHANGE)\n" );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief リクエストの発行 ( FSND_BGM_REQUEST_STAND_BY )
 *
 * @param fieldSound
 * @param soundIdx     再生の準備をするするBGM
 * @param fadeOutFrame 再生中のBGMのフェードアウト フレーム数
 */
//---------------------------------------------------------------------------------
static void ThrowRequest_STAND_BY( FIELD_SOUND* fieldSound, 
                                   u32 soundIdx, u16 fadeOutFrame )
{
  // リクエストを受理できない
  if( CanAcceptRequest(fieldSound, FSND_BGM_REQUEST_STAND_BY) == FALSE )
  {
#ifdef DEBUG_PRINT_ON
    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: Can't Accept BGM stand-by request\n" );
#endif
    GF_ASSERT(0);
    return;
  }

  // リクエストを受理
  fieldSound->request = FSND_BGM_REQUEST_STAND_BY;
  fieldSound->requestBGM = soundIdx;
  fieldSound->fadeOutFrame = fadeOutFrame;
  
#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: accept request(STAND_BY)\n" );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief リクエストの発行 ( FSND_BGM_REQUEST_FORCE_PLAY )
 *
 * @param fieldSound
 * @param soundIdx   再生するBGM
 */
//---------------------------------------------------------------------------------
static void ThrowRequest_FORCE_PLAY( FIELD_SOUND* fieldSound, u16 soundIdx )
{
  // リクエストを受理できない
  if( CanAcceptRequest(fieldSound, FSND_BGM_REQUEST_FORCE_PLAY) == FALSE )
  {
#ifdef DEBUG_PRINT_ON
    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: Can't Accept BGM force play request\n" );
#endif
    GF_ASSERT(0);
    return;
  }

  // リクエストを受理
  fieldSound->request = FSND_BGM_REQUEST_FORCE_PLAY;
  fieldSound->requestBGM = soundIdx;

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: accept request(FORCE_PLAY)\n" );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief リクエストを受理できるかどうかを判定する
 *
 * @param fieldSound
 * @param request    チェックするリクエスト
 *
 * @return 指定したリクエストを受理できる状態なら TRUE
 *         そうでなければ                         FALSE
 */
//---------------------------------------------------------------------------------
static BOOL CanAcceptRequest( const FIELD_SOUND* fieldSound, FSND_BGM_REQUEST request )
{
  return RequestAcceptTable[ fieldSound->state ][ request ];
}

//---------------------------------------------------------------------------------
/**
 * @brief 内部状態 STOP 時の, 各種リクエストに対する動作
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void RequestCheck_STOP( FIELD_SOUND* fieldSound )
{
  switch( fieldSound->request )
  {
  case FSND_BGM_REQUEST_NONE:  
    break;
  case FSND_BGM_REQUEST_POP:
    PopBGM( fieldSound );
    FadeInBGM( fieldSound );
    ChangeState( fieldSound, FSND_STATE_POP_in );
    break;
  case FSND_BGM_REQUEST_CHANGE:  
    ChangeState( fieldSound, FSND_STATE_CHANGE_out );
    break;
  case FSND_BGM_REQUEST_STAND_BY:  
    ChangeState( fieldSound, FSND_STATE_STAND_BY_out );
    break;
  case FSND_BGM_REQUEST_FORCE_PLAY:
    ForcePlayBGM( fieldSound );
    ChangeState( fieldSound, FSND_STATE_PLAY );
    FinishRequest( fieldSound );
    break;
  default:  
    GF_ASSERT(0);
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief 内部状態 PLAY 時の, 各種リクエストに対する動作
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void RequestCheck_PLAY( FIELD_SOUND* fieldSound )
{
  switch( fieldSound->request )
  {
  case FSND_BGM_REQUEST_NONE: 
    break;
  case FSND_BGM_REQUEST_FADE_IN:
    // 再生中のリクエストは無視する
    CancelRequest( fieldSound );
    break;
  case FSND_BGM_REQUEST_FADE_OUT: 
    FadeOutBGM( fieldSound );
    ChangeState( fieldSound, FSND_STATE_FADE_OUT );
    break;
  case FSND_BGM_REQUEST_PUSH: 
    FadeOutBGM( fieldSound );
    ChangeState( fieldSound, FSND_STATE_PUSH );
    break;
  case FSND_BGM_REQUEST_POP: 
    FadeOutBGM( fieldSound );
    ChangeState( fieldSound, FSND_STATE_POP_out );
    break;
  case FSND_BGM_REQUEST_CHANGE: 
    // 再生中の曲のリクエストは無視する
    if( fieldSound->requestBGM == fieldSound->currentBGM ) {
      CancelRequest( fieldSound );
    }
    else {
      FadeOutBGM( fieldSound );
      ChangeState( fieldSound, FSND_STATE_CHANGE_out );
    }
    break;
  case FSND_BGM_REQUEST_STAND_BY: 
    // 再生中の曲のリクエストは無視する
    if( fieldSound->requestBGM == fieldSound->currentBGM ) {
      CancelRequest( fieldSound );
    }
    else {
      FadeOutBGM( fieldSound );
      ChangeState( fieldSound, FSND_STATE_STAND_BY_out );
    }
    break;
  case FSND_BGM_REQUEST_FORCE_PLAY:
    // 再生中の曲のリクエストは無視する
    if( fieldSound->requestBGM == fieldSound->currentBGM ) {
      CancelRequest( fieldSound );
    }
    else {
      ForcePlayBGM( fieldSound );
      ChangeState( fieldSound, FSND_STATE_PLAY );
      FinishRequest( fieldSound );
    }
    break;
  default:
    GF_ASSERT(0);
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief 内部状態 WAIT 時の, 各種リクエストに対する動作
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void RequestCheck_WAIT( FIELD_SOUND* fieldSound )
{
  switch( fieldSound->request )
  {
  case FSND_BGM_REQUEST_NONE: 
    break;
  case FSND_BGM_REQUEST_FADE_IN: 
    FadeInBGM( fieldSound );
    ChangeState( fieldSound, FSND_STATE_FADE_IN );
    break;
  case FSND_BGM_REQUEST_FORCE_PLAY: 
    ForcePlayBGM( fieldSound );
    ChangeState( fieldSound, FSND_STATE_PLAY );
    FinishRequest( fieldSound );
    break;
  default:
    GF_ASSERT(0);
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief 内部状態 FADE_OUT 時の, 各種リクエストに対する動作
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void RequestCheck_FADE_OUT( FIELD_SOUND* fieldSound )
{
  switch( fieldSound->request )
  {
  case FSND_BGM_REQUEST_NONE: 
    break;
  case FSND_BGM_REQUEST_FADE_OUT: 
    break;
  default:
    GF_ASSERT(0);
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief 内部状態 FADE_IN 時の, 各種リクエストに対する動作
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void RequestCheck_FADE_IN( FIELD_SOUND* fieldSound )
{
  switch( fieldSound->request )
  {
  case FSND_BGM_REQUEST_NONE: 
    break;
  case FSND_BGM_REQUEST_FADE_IN: 
    break;
  default:
    GF_ASSERT(0);
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief 内部状態 PUSH 時の, 各種リクエストに対する動作
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void RequestCheck_PUSH( FIELD_SOUND* fieldSound )
{
  switch( fieldSound->request )
  {
  case FSND_BGM_REQUEST_NONE: 
    break;
  case FSND_BGM_REQUEST_PUSH:
    break;
  default:
    GF_ASSERT(0);
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief 内部状態 POP_out 時の, 各種リクエストに対する動作
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void RequestCheck_POP_out( FIELD_SOUND* fieldSound )
{
  switch( fieldSound->request )
  {
  case FSND_BGM_REQUEST_NONE: 
    break;
  case FSND_BGM_REQUEST_POP:
    break;
  default:
    GF_ASSERT(0);
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief 内部状態 POP_in 時の, 各種リクエストに対する動作
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void RequestCheck_POP_in( FIELD_SOUND* fieldSound )
{
  switch( fieldSound->request )
  {
  case FSND_BGM_REQUEST_NONE: 
    break;
  case FSND_BGM_REQUEST_POP:
    break;
  default:
    GF_ASSERT(0);
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief 内部状態 CHANGE_out 時の, 各種リクエストに対する動作
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void RequestCheck_CHANGE_out( FIELD_SOUND* fieldSound )
{
  switch( fieldSound->request )
  {
  case FSND_BGM_REQUEST_NONE: 
    break;
  case FSND_BGM_REQUEST_PUSH: 
    ChangeState( fieldSound, FSND_STATE_CHANGE_PUSH_out);
    break;
  case FSND_BGM_REQUEST_CHANGE:
    break;
  default:
    GF_ASSERT(0);
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief 内部状態 CHANGE_load 時の, 各種リクエストに対する動作
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void RequestCheck_CHANGE_load( FIELD_SOUND* fieldSound )
{
  switch( fieldSound->request )
  {
  case FSND_BGM_REQUEST_NONE: 
    break;
  case FSND_BGM_REQUEST_PUSH: 
    ChangeState( fieldSound, FSND_STATE_CHANGE_PUSH_load );
    break;
  case FSND_BGM_REQUEST_CHANGE:
    break;
  default:
    GF_ASSERT(0);
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief 内部状態 CHANGE_in 時の, 各種リクエストに対する動作
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void RequestCheck_CHANGE_in( FIELD_SOUND* fieldSound )
{
  switch( fieldSound->request )
  {
  case FSND_BGM_REQUEST_NONE: 
    break;
  case FSND_BGM_REQUEST_CHANGE:
    break;
  default:
    GF_ASSERT(0);
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief 内部状態 CHANGE_PUSH_out 時の, 各種リクエストに対する動作
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void RequestCheck_CHANGE_PUSH_out ( FIELD_SOUND* fieldSound )
{
  switch( fieldSound->request )
  {
  case FSND_BGM_REQUEST_NONE: 
    break;
  case FSND_BGM_REQUEST_PUSH:
    break;
  default:
    GF_ASSERT(0);
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief 内部状態 CHANGE_PUSH_load 時の, 各種リクエストに対する動作
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void RequestCheck_CHANGE_PUSH_load( FIELD_SOUND* fieldSound )
{
  switch( fieldSound->request )
  {
  case FSND_BGM_REQUEST_NONE: 
    break;
  case FSND_BGM_REQUEST_PUSH:
    break;
  default:
    GF_ASSERT(0);
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief 内部状態 STAND_BY_out 時の, 各種リクエストに対する動作
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void RequestCheck_STAND_BY_out( FIELD_SOUND* fieldSound )
{
  switch( fieldSound->request )
  {
  case FSND_BGM_REQUEST_NONE: 
    break;
  case FSND_BGM_REQUEST_STAND_BY:
    break;
  default:
    GF_ASSERT(0);
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief 内部状態 STAND_BY_load 時の, 各種リクエストに対する動作
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void RequestCheck_STAND_BY_load( FIELD_SOUND* fieldSound )
{
  switch( fieldSound->request )
  {
  case FSND_BGM_REQUEST_NONE: 
    break;
  case FSND_BGM_REQUEST_STAND_BY:
    break;
  default:
    GF_ASSERT(0);
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief 内部状態 STOP 時の動作
 */
//---------------------------------------------------------------------------------
static void Main_STOP( FIELD_SOUND* fieldSound )
{ 
}

//---------------------------------------------------------------------------------
/**
 * @brief 内部状態 PLAY 時の動作
 */
//---------------------------------------------------------------------------------
static void Main_PLAY( FIELD_SOUND* fieldSound )
{
}

//---------------------------------------------------------------------------------
/**
 * @brief 内部状態 WAIT 時の動作
 */
//---------------------------------------------------------------------------------
static void Main_WAIT( FIELD_SOUND* fieldSound )
{
}

//---------------------------------------------------------------------------------
/**
 * @brief 内部状態 FADE_OUT 時の動作
 */
//---------------------------------------------------------------------------------
static void Main_FADE_OUT( FIELD_SOUND* fieldSound )
{
  if( PMSND_CheckFadeOnBGM() == FALSE )
  {
    ChangeState( fieldSound, FSND_STATE_WAIT );
    FinishRequest( fieldSound );
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief 内部状態 FADE_IN 時の動作
 */
//---------------------------------------------------------------------------------
static void Main_FADE_IN( FIELD_SOUND* fieldSound )
{
  if( PMSND_CheckFadeOnBGM() == FALSE )
  {
    ChangeState( fieldSound, FSND_STATE_PLAY );
    FinishRequest( fieldSound );
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief 内部状態 PUSH 時の動作
 */
//---------------------------------------------------------------------------------
static void Main_PUSH( FIELD_SOUND* fieldSound )
{
  if( PMSND_CheckFadeOnBGM() == FALSE )
  {
    PushBGM( fieldSound );
    ChangeState( fieldSound, FSND_STATE_STOP );
    FinishRequest( fieldSound );
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief 内部状態 POP_out 時の動作
 */
//---------------------------------------------------------------------------------
static void Main_POP_out( FIELD_SOUND* fieldSound )
{
  if( PMSND_CheckFadeOnBGM() == FALSE )
  {
    PopBGM( fieldSound );
    FadeInBGM( fieldSound );
    ChangeState( fieldSound, FSND_STATE_POP_in );
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief 内部状態 POP_in 時の動作
 */
//---------------------------------------------------------------------------------
static void Main_POP_in( FIELD_SOUND* fieldSound )
{
  if( PMSND_CheckFadeOnBGM() == FALSE )
  {
    ChangeState( fieldSound, FSND_STATE_PLAY );
    FinishRequest( fieldSound );
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief 内部状態 CHANGE_out 時の動作
 */
//---------------------------------------------------------------------------------
static void Main_CHANGE_out( FIELD_SOUND* fieldSound )
{
  if( PMSND_CheckFadeOnBGM() == FALSE )
  {
    if( CheckSaveNow( fieldSound ) == FALSE ) {
      DivLoadBGM_start( fieldSound );
      ChangeState( fieldSound, FSND_STATE_CHANGE_load );
    }
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief 内部状態 CHANGE_load 時の動作
 */
//---------------------------------------------------------------------------------
static void Main_CHANGE_load( FIELD_SOUND* fieldSound )
{
  BOOL loadFinished;

  // 分割ロード実行
  loadFinished = DivLoadBGM_load( fieldSound );

  // 分割ロード完了(再生開始)
  if( loadFinished ) { 
    if( (fieldSound->requestBGM != BGM_NONE) &&
        (fieldSound->requestBGM != fieldSound->currentBGM) ) { 
      // 再ロード開始
      PMSND_StopBGM();
      ChangeState( fieldSound, FSND_STATE_CHANGE_out );
    }
    else { 
      // フェードイン開始
      FadeInBGM( fieldSound );
      ChangeState( fieldSound, FSND_STATE_CHANGE_in );
    }
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief 内部状態 CHANGE_in 時の動作
 */
//---------------------------------------------------------------------------------
static void Main_CHANGE_in( FIELD_SOUND* fieldSound )
{
  // フェードイン完了
  if( PMSND_CheckFadeOnBGM() == FALSE )
  {
    // 再生開始以降に BGM変更リクエストを受けた場合
    if( (fieldSound->requestBGM != BGM_NONE) &&
        (fieldSound->requestBGM != fieldSound->currentBGM) )
    {
      // フェードアウト開始
      FadeOutBGM( fieldSound );
      ChangeState( fieldSound, FSND_STATE_CHANGE_out );
    }
    else
    {
      ChangeState( fieldSound, FSND_STATE_PLAY );
      FinishRequest( fieldSound );
    }
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief 内部状態 CHANGE_PUSH_out 時の動作
 */
//---------------------------------------------------------------------------------
static void Main_CHANGE_PUSH_out( FIELD_SOUND* fieldSound )
{
  if( PMSND_CheckFadeOnBGM() == FALSE )
  {
    if( CheckSaveNow( fieldSound ) == FALSE ) {
      DivLoadBGM_start( fieldSound );
      ChangeState( fieldSound, FSND_STATE_CHANGE_PUSH_load );
    }
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief 内部状態 CHANGE_PUSH_load 時の動作
 */
//---------------------------------------------------------------------------------
static void Main_CHANGE_PUSH_load( FIELD_SOUND* fieldSound )
{
  BOOL loadFinished;

  // 分割ロード実行
  loadFinished = DivLoadBGM_load( fieldSound );

  // 分割ロード完了(再生開始)
  if( loadFinished ) { 
    // BGM 退避
    PushBGM( fieldSound );
    ChangeState( fieldSound, FSND_STATE_STOP );
    FinishRequest( fieldSound ); 
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief 内部状態 STAND_BY_out 時の動作
 */
//---------------------------------------------------------------------------------
static void Main_STAND_BY_out( FIELD_SOUND* fieldSound )
{
  if( PMSND_CheckFadeOnBGM() == FALSE ) { 
    ChangeState( fieldSound, FSND_STATE_STAND_BY_load );
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief 内部状態 STAND_BY_load 時の動作
 */
//---------------------------------------------------------------------------------
static void Main_STAND_BY_load( FIELD_SOUND* fieldSound )
{
  ForcePlayBGM( fieldSound );
  PMSND_PauseBGM( TRUE ); // BGM 一時停止
  ChangeState( fieldSound, FSND_STATE_WAIT );
  FinishRequest( fieldSound );
} 

//---------------------------------------------------------------------------------
/**
 * @brief 内部状態を変更する
 *
 * @param fieldSound 
 * @param nextState  遷移先の状態
 */
//---------------------------------------------------------------------------------
static void ChangeState( FIELD_SOUND* fieldSound, FSND_STATE nextState )
{
#ifdef DEBUG_PRINT_ON
  switch( fieldSound->state ) {
  case FSND_STATE_STOP:              OS_TFPrintf( PRINT_NO, "FIELD-SOUND: change state (STOP ==> " );  break;
  case FSND_STATE_PLAY:              OS_TFPrintf( PRINT_NO, "FIELD-SOUND: change state (PLAY ==> " );  break;
  case FSND_STATE_WAIT:              OS_TFPrintf( PRINT_NO, "FIELD-SOUND: change state (WAIT ==> " );  break;
  case FSND_STATE_FADE_OUT:          OS_TFPrintf( PRINT_NO, "FIELD-SOUND: change state (FADE OUT ==> " );  break;
  case FSND_STATE_FADE_IN:           OS_TFPrintf( PRINT_NO, "FIELD-SOUND: change state (FADE IN ==> " );  break;
  case FSND_STATE_PUSH:              OS_TFPrintf( PRINT_NO, "FIELD-SOUND: change state (PUSH ==> " );  break;
  case FSND_STATE_POP_out:           OS_TFPrintf( PRINT_NO, "FIELD-SOUND: change state (POP out ==> " );  break;
  case FSND_STATE_POP_in:            OS_TFPrintf( PRINT_NO, "FIELD-SOUND: change state (POP in ==> " );  break;
  case FSND_STATE_CHANGE_out:        OS_TFPrintf( PRINT_NO, "FIELD-SOUND: change state (CHANGE out ==> " );  break;
  case FSND_STATE_CHANGE_load:       OS_TFPrintf( PRINT_NO, "FIELD-SOUND: change state (CHANGE load ==> " );  break;
  case FSND_STATE_CHANGE_in:         OS_TFPrintf( PRINT_NO, "FIELD-SOUND: change state (CHANGE in ==> " );  break;
  case FSND_STATE_CHANGE_PUSH_out:   OS_TFPrintf( PRINT_NO, "FIELD-SOUND: change state (CHANGE PUSH out ==> " );  break;
  case FSND_STATE_CHANGE_PUSH_load:  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: change state (CHANGE PUSH load ==> " );  break;
  case FSND_STATE_STAND_BY_out:      OS_TFPrintf( PRINT_NO, "FIELD-SOUND: change state (STAND BY out ==> " );  break;
  case FSND_STATE_STAND_BY_load:     OS_TFPrintf( PRINT_NO, "FIELD-SOUND: change state (STAND BY load ==> " );  break;
  default: GF_ASSERT(0);
  }
  switch( nextState ) {
  case FSND_STATE_STOP:              OS_TFPrintf( PRINT_NO, "STOP)\n" );  break;
  case FSND_STATE_PLAY:              OS_TFPrintf( PRINT_NO, "PLAY)\n" );  break;
  case FSND_STATE_WAIT:              OS_TFPrintf( PRINT_NO, "WAIT)\n" );  break;
  case FSND_STATE_FADE_OUT:          OS_TFPrintf( PRINT_NO, "FADE OUT)\n" );  break;
  case FSND_STATE_FADE_IN:           OS_TFPrintf( PRINT_NO, "FADE IN)\n" );  break;
  case FSND_STATE_PUSH:              OS_TFPrintf( PRINT_NO, "PUSH)\n" );  break;
  case FSND_STATE_POP_out:           OS_TFPrintf( PRINT_NO, "POP out)\n" );  break;
  case FSND_STATE_POP_in:            OS_TFPrintf( PRINT_NO, "POP in)\n" );  break;
  case FSND_STATE_CHANGE_out:        OS_TFPrintf( PRINT_NO, "CHANGE out)\n" );  break;
  case FSND_STATE_CHANGE_load:       OS_TFPrintf( PRINT_NO, "CHANGE load)\n" );  break;
  case FSND_STATE_CHANGE_in:         OS_TFPrintf( PRINT_NO, "CHANGE in)\n" );  break;
  case FSND_STATE_CHANGE_PUSH_out:   OS_TFPrintf( PRINT_NO, "CHANGE PUSH out)\n" );  break;
  case FSND_STATE_CHANGE_PUSH_load:  OS_TFPrintf( PRINT_NO, "CHANGE PUSH load)\n" );  break;
  case FSND_STATE_STAND_BY_out:      OS_TFPrintf( PRINT_NO, "STAND BY out)\n" );  break;
  case FSND_STATE_STAND_BY_load:     OS_TFPrintf( PRINT_NO, "STAND BY load)\n" );  break;
  default: GF_ASSERT(0);
  }
#endif

  // 更新
  fieldSound->state = nextState; 
}

//---------------------------------------------------------------------------------
/**
 * @brief リクエスト受理状態をリセットする
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void ResetRequest( FIELD_SOUND* fieldSound )
{
  fieldSound->request = FSND_BGM_REQUEST_NONE;
  fieldSound->requestBGM = BGM_NONE;
  fieldSound->fadeInFrame = 0;
  fieldSound->fadeOutFrame = 0;
}

//---------------------------------------------------------------------------------
/**
 * @brief 受け取ったリクエストをキャンセルする
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void CancelRequest( FIELD_SOUND* fieldSound )
{
  // リクエストを持っていない
  if( fieldSound->request == FSND_BGM_REQUEST_NONE )
  {
#ifdef DEBUG_PRINT_ON
    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: Don't have BGM request\n" );
#endif
    GF_ASSERT(0);
    return;
  }

  // リクエスト受理状態をリセット
  ResetRequest( fieldSound );

  // DEBUG:
#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: Cancel request\n" );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief リクエストに対する動作の完了処理
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void FinishRequest( FIELD_SOUND* fieldSound )
{
  // リクエストを持っていない
  if( fieldSound->request == FSND_BGM_REQUEST_NONE )
  {
#ifdef DEBUG_PRINT_ON
    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: Don't have BGM request\n" );
#endif
    GF_ASSERT(0);
    return;
  }

#ifdef DEBUG_PRINT_ON
  switch( fieldSound->request ) {
  case FSND_BGM_REQUEST_FADE_IN:     OS_TFPrintf( PRINT_NO, "FIELD-SOUND: request(FADE IN) finished!!\n" );  break;
  case FSND_BGM_REQUEST_FADE_OUT:    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: request(FADE OUT) finished!!\n" );  break;
  case FSND_BGM_REQUEST_PUSH:        OS_TFPrintf( PRINT_NO, "FIELD-SOUND: request(PUSH) finished!!\n" );  break;
  case FSND_BGM_REQUEST_POP:         OS_TFPrintf( PRINT_NO, "FIELD-SOUND: request(POP) finished!!\n" );  break;
  case FSND_BGM_REQUEST_CHANGE:      OS_TFPrintf( PRINT_NO, "FIELD-SOUND: request(CHANGE) finished!!\n" );  break;
  case FSND_BGM_REQUEST_STAND_BY:    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: request(STAND BY) finished!!\n" );  break;
  case FSND_BGM_REQUEST_FORCE_PLAY:  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: request(FORCE_PLAY) finished!!\n" );  break;
  }
#endif

  // リクエスト受理状態をリセット
  ResetRequest( fieldSound );
}

//---------------------------------------------------------------------------------
/**
 * @brief BGMのフェードインを開始する
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void FadeInBGM( FIELD_SOUND* fieldSound )
{
  // BGMが鳴っていない
  if( fieldSound->currentBGM == BGM_NONE )
  {
#ifdef DEBUG_PRINT_ON
    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: Can't fade in BGM (Don't playing BGM)\n" );
#endif
    GF_ASSERT(0);
    return;
  } 

  PMSND_FadeInBGM( fieldSound->fadeInFrame );
  PMSND_PauseBGM( FALSE );

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: BGM fade in start\n" );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief BGMのフェードアウトを開始する
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void FadeOutBGM( FIELD_SOUND* fieldSound )
{
  // BGMが鳴っていない
  if( fieldSound->currentBGM == BGM_NONE )
  {
#ifdef DEBUG_PRINT_ON
    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: Can't fade out BGM (Don't playing BGM)\n" );
#endif
    GF_ASSERT(0);
    return;
  } 

  PMSND_FadeOutBGM( fieldSound->fadeOutFrame );

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: BGM fade out start\n" );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief BGMを退避する
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void PushBGM( FIELD_SOUND* fieldSound )
{
  // BGMが鳴っていない
  if( fieldSound->currentBGM == BGM_NONE )
  {
#ifdef DEBUG_PRINT_ON
    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: Can't push BGM (Don't playing BGM)\n" );
#endif
    GF_ASSERT(0);
    return;
  } 
  // 退避数オーバー
  if( FSND_PUSHCOUNT_MAX <= fieldSound->pushCount )
  {
#ifdef DEBUG_PRINT_ON
    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: Can't push BGM (Push count over)\n" );
#endif
    GF_ASSERT(0);
    return;
  }

  // 0->1ならば、環境音を停止
  if( fieldSound->pushCount == FSND_PUSHCOUNT_NONE ){
    FIELD_SOUND_PauseEnvSE( fieldSound, FSND_ENVSE_PAUSE_BGM_PUSH );
  }

  // 退避
  PMSND_PauseBGM( TRUE );
  PMSND_PushBGM();

  // 内部情報を更新
  fieldSound->pushBGM[ fieldSound->pushCount ] = fieldSound->currentBGM;
  fieldSound->pushCount++;
  fieldSound->currentBGM = BGM_NONE; 

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_NO, 
      "FIELD-SOUND: push BGM(%d). pushCount==>%d\n", 
      fieldSound->pushBGM[ fieldSound->pushCount-1 ], fieldSound->pushCount );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief BGMを復帰する
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void PopBGM( FIELD_SOUND* fieldSound )
{
  // 退避されていない
  if( fieldSound->pushCount <= FSND_PUSHCOUNT_NONE )
  {
#ifdef DEBUG_PRINT_ON
    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: Can't pop BGM (There is no pushed BGM)\n" );
#endif
    GF_ASSERT(0);
    return;
  }

  // 復帰
  PMSND_PopBGM();
  PMSND_PauseBGM( FALSE );

  // 内部情報を更新
  fieldSound->pushCount--;
  fieldSound->currentBGM = fieldSound->pushBGM[ fieldSound->pushCount ];

  // 1->0ならば、環境音を停止
  if( fieldSound->pushCount == FSND_PUSHCOUNT_NONE ){
    FIELD_SOUND_RePlayEnvSE( fieldSound, FSND_ENVSE_PAUSE_BGM_PUSH );
  }

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_NO, 
      "FIELD-SOUND: pop BGM(%d). pushCount==>%d\n", 
      fieldSound->currentBGM, fieldSound->pushCount );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief BGMの分割ロードを開始する
 *
 * @param fieldSound
 *
 * @return 分割ロードを開始した場合 TRUE
 *         そうでなければ FALSE
 */
//---------------------------------------------------------------------------------
static BOOL DivLoadBGM_start( FIELD_SOUND* fieldSound )
{
  // 読み込むBGMが予約されていない
  if( fieldSound->requestBGM == BGM_NONE )
  {
#ifdef DEBUG_PRINT_ON
    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: Can't start load BGM (Don't know target BGM)\n" );
#endif
    GF_ASSERT(0);
    return FALSE;
  }

  // 分割ロード開始
  PMSND_PlayBGMdiv( fieldSound->requestBGM, &(fieldSound->loadSeq), TRUE );

  // 内部情報を更新
  fieldSound->loadBGM = fieldSound->requestBGM;
  fieldSound->requestBGM = BGM_NONE;
  fieldSound->currentBGM = BGM_NONE;

  // ビーコンサーチを一定時間停止
  GFL_NET_ChangeoverChangeSpeed( GFL_NET_CROSS_SPEED_PAUSE );

#ifdef DEBUG_DIV_LOAD_COUNT_ON
  DivLoadCount = 0;
#endif

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: div load BGM(%d) start\n", fieldSound->loadBGM );
#endif

  return TRUE;
}

//---------------------------------------------------------------------------------
/**
 * @brief BGMの分割ロードを進める
 *
 * @param fieldSound
 *
 * @return 分割ロードが終了し, 再生が開始された場合 TRUE
 *         分割ロード未完了なら　FALSE
 */
//---------------------------------------------------------------------------------
static BOOL DivLoadBGM_load( FIELD_SOUND* fieldSound )
{
  BOOL loadFinished;

  // ロードが開始されていない
  if( fieldSound->loadBGM == BGM_NONE )
  {
#ifdef DEBUG_PRINT_ON
    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: Can't load BGM (Don't know target BGM)\n" );
#endif
    GF_ASSERT(0); 
  }

  // 分割ロード実行
  loadFinished = PMSND_PlayBGMdiv( fieldSound->loadBGM, &(fieldSound->loadSeq), FALSE );

  // ビーコンサーチを一定時間停止
  GFL_NET_ChangeoverChangeSpeed( GFL_NET_CROSS_SPEED_PAUSE );

#ifdef DEBUG_DIV_LOAD_COUNT_ON
  DivLoadCount++;
#endif

  // 分割ロード完了(再生開始)
  if( loadFinished ) {
    // 内部情報を更新
    fieldSound->currentBGM = fieldSound->loadBGM;
    fieldSound->loadBGM = BGM_NONE;

#ifdef DEBUG_PRINT_ON
    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: div load BGM(%d) finish\n", fieldSound->currentBGM );
#endif

#ifdef DEBUG_DIV_LOAD_COUNT_ON
    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: div load count = %d\n", DivLoadCount );
#endif
  } 
  return loadFinished;
}

//---------------------------------------------------------------------------------
/**
 * @brief BGMの即時再生を行う
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void ForcePlayBGM( FIELD_SOUND* fieldSound )
{
  // 再生するBGMが指定されていない
  if( fieldSound->requestBGM == BGM_NONE )
  {
#ifdef DEBUG_PRINT_ON
    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: Can't force play BGM (Don't know target BGM)\n" );
#endif
    GF_ASSERT(0);
    return;
  }

  // 即時再生
  PMSND_PlayBGM( fieldSound->requestBGM );

  // 内部情報を更新
  fieldSound->currentBGM = fieldSound->requestBGM;
  fieldSound->requestBGM = BGM_NONE;
  fieldSound->loadBGM = BGM_NONE;

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: force play BGM(%d) finish\n", fieldSound->currentBGM );
#endif
} 

//---------------------------------------------------------------------------------
/**
 * @brief ビーコンスキャンを一時停止する
 */
//---------------------------------------------------------------------------------
static void PauseBeaconScan( void )
{
#ifdef PM_DEBUG
  DEBUG_PerformanceStressON( FALSE, STRESS_ID_SND );
#endif

	if( GFL_NET_IsInit() ) {
		GFL_NET_WL_PauseScan( TRUE );
	}
}

//---------------------------------------------------------------------------------
/**
 * @brief ビーコンスキャンの一時停止を解除する
 */
//---------------------------------------------------------------------------------
static void StartBeaconScan( void ) 
{
#ifdef PM_DEBUG
  DEBUG_PerformanceStressON( TRUE, STRESS_ID_SND );
#endif

	if( GFL_NET_IsInit() ) {
		GFL_NET_WL_PauseScan( FALSE );
	}
}

//---------------------------------------------------------------------------------
/**
 * @brief セーブ中かどうかを判定する
 *
 * @param fieldSound
 *
 * @return セーブ中なら TRUE
 *         そうでなければ FALSE
 */
//---------------------------------------------------------------------------------
static BOOL CheckSaveNow( const FIELD_SOUND* fieldSound )
{
  return GAMEDATA_GetIsSave( fieldSound->gameData );
}

#ifdef DEBUG_PRINT_ON
//---------------------------------------------------------------------------------
/**
 * @brief リクエストキューを表示する
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void DebugPrint_RequestQueue( const FIELD_SOUND* fieldSound )
{
  int pos;
  const FSND_REQUEST_DATA* requestData;

  OS_TFPrintf( PRINT_NO, "FIELD-SOUND-QUEUE: " );

  // 先頭リクエスト取得
  pos = fieldSound->requestHeadPos;
  requestData = &( fieldSound->requestData[ pos ] );

  while( requestData->request != FSND_BGM_REQUEST_NONE )
  {
    switch( requestData->request ) {
    case FSND_BGM_REQUEST_FADE_IN:     OS_TFPrintf( PRINT_NO, "FADE IN" );  break;
    case FSND_BGM_REQUEST_FADE_OUT:    OS_TFPrintf( PRINT_NO, "FADE OUT" );  break;
    case FSND_BGM_REQUEST_PUSH:        OS_TFPrintf( PRINT_NO, "PUSH" );  break;
    case FSND_BGM_REQUEST_POP:         OS_TFPrintf( PRINT_NO, "POP" );  break;
    case FSND_BGM_REQUEST_CHANGE:      OS_TFPrintf( PRINT_NO, "CHANGE" );  break;
    case FSND_BGM_REQUEST_STAND_BY:    OS_TFPrintf( PRINT_NO, "STAND BY" );  break;
    case FSND_BGM_REQUEST_FORCE_PLAY:  OS_TFPrintf( PRINT_NO, "FORCE_PLAY" );  break;
    }
    OS_TFPrintf( PRINT_NO, " | " );

    // 次のリクエストへ
    pos = (pos + 1) % REQUEST_QUEUE_SIZE;
    requestData = &( fieldSound->requestData[ pos ] );

    // 一周した
    if( pos == fieldSound->requestHeadPos ){ break; }
  }
  OS_TFPrintf( PRINT_NO, "\n" );
}

//---------------------------------------------------------------------------------
/**
 * @brief BGM 退避状況を出力する
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void DebugPrint_pushedBGM( const FIELD_SOUND* fieldSound )
{
  int i;
  int pushCount;

  pushCount = fieldSound->pushCount;

  // 退避数
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: pushCount = %d\n", pushCount );

  // 退避中のBGM No.
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: pushBGM = " );
  for( i=0; i < pushCount; i++ )
  {
    OS_TFPrintf( PRINT_NO, "%d, ", fieldSound->pushBGM[i] );
  }
  OS_TFPrintf( PRINT_NO, "\n" );
}

//---------------------------------------------------------------------------------
/**
 * @brief 全内部情報を出力する
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void DebugPrint_AllInfo( const FIELD_SOUND* fieldSound )
{ 
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: state = %d\n", fieldSound->state );
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: currentBGM = %d\n", fieldSound->currentBGM );
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: requestBGM = %d\n", fieldSound->requestBGM );
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: loadBGM = %d\n", fieldSound->loadBGM );
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: loadSeq = %d\n", fieldSound->loadSeq );
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: fadeInFrame = %d\n", fieldSound->fadeInFrame );
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: fadeOutFrame = %d\n", fieldSound->fadeOutFrame );
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: request = %d\n", fieldSound->request );
  DebugPrint_pushedBGM( fieldSound );
  DebugPrint_RequestQueue( fieldSound );
}
#endif
