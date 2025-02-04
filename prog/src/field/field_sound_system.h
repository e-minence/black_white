///////////////////////////////////////////////////////////////////////////////////
/**
 * @brief フィールドのサウンド制御
 * @file  field_sound_system.h
 * @author obata
 * @date   2009.12.25
 */
///////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <gflib.h>
#include "field_sound_proc.h"


//=================================================================================
// ■定数
//=================================================================================
// BGM 退避回数
typedef enum {
  FSND_PUSHCOUNT_NONE,   // 退避なし
  FSND_PUSHCOUNT_BASE,   // フィールド or イベント BGM 退避中
  FSND_PUSHCOUNT_OVER,   // 退避数オーバー
  FSND_PUSHCOUNT_MAX = FSND_PUSHCOUNT_OVER-1  // 最大退避数
} FSND_PUSHCOUNT;

// BGM リクエスト
typedef enum {
  FSND_BGM_REQUEST_NONE,        // リクエストなし
  FSND_BGM_REQUEST_FADE_IN,     // BGM フェードイン
  FSND_BGM_REQUEST_FADE_OUT,    // BGM フェードアウト
  FSND_BGM_REQUEST_PUSH,        // BGM 退避
  FSND_BGM_REQUEST_POP,         // BGM 復帰
  FSND_BGM_REQUEST_CHANGE,      // BGM 変更
  FSND_BGM_REQUEST_STAND_BY,    // BGM 再生準備
  FSND_BGM_REQUEST_FORCE_PLAY,  // 即時再生
  FSND_BGM_REQUEST_NUM          // 総数
} FSND_BGM_REQUEST;

// 環境音SE　停止タイプ
typedef enum {
  FSND_ENVSE_PAUSE_NONE,        // 停止なし
  FSND_ENVSE_PAUSE_BGM_PUSH = 1<<0,    // BGMプッシュによる停止
  FSND_ENVSE_PAUSE_BGM_CHANGE = 1<<1,  // BGM変更による停止
} FSND_ENVSE_PAUSE_TYPE;



//=================================================================================
// ■取得
//================================================================================= 
// BGM退避数の取得
extern FSND_PUSHCOUNT FIELD_SOUND_GetBGMPushCount( const FIELD_SOUND* fieldSound );

// 現在登録されているリクエストが完了した時点での, BGM退避数を取得する
extern FSND_PUSHCOUNT 
FIELD_SOUND_GetBGMPushCount_atAllRequestFinished( const FIELD_SOUND* fieldSound );

// 現在登録されているリクエストが完了した時点での, 再生中のBGMを取得する
extern u32 FIELD_SOUND_GetPlayingBGM_atAllRequestFinished( const FIELD_SOUND* fieldSound );

// フェード中かどうか (TRUE: フェード中)
extern BOOL FIELD_SOUND_IsBGMFade( const FIELD_SOUND* fieldSound );

// 未処理( or 処理中 )のリクエストを持っているかどうか (TRUE: 持っている)
extern BOOL FIELD_SOUND_HaveRequest( const FIELD_SOUND* fieldSound );

// BGMの復帰が全て終わったかどうかをチェックする
extern BOOL FIELD_SOUND_CheckBGMPopFinished( const FIELD_SOUND* fieldSound );



//=================================================================================
// ■リクエスト
//
// ※基本的に使用しないでください。
// 　field_sound.h に宣言されている関数を使ってBGMの操作を行ってください。
//=================================================================================
// 登録
extern void FIELD_SOUND_RegisterRequest( FIELD_SOUND* fieldSound, 
                                         FSND_BGM_REQUEST request,
                                         u32 soundIdx, 
                                         u16 fadeOutFrame, 
                                         u16 fadeInFrame ); 

#define FIELD_SOUND_RegisterRequest_FADE_IN( fieldSound, fadeInFrame ) \
        FIELD_SOUND_RegisterRequest( fieldSound, FSND_BGM_REQUEST_FADE_IN, 0, 0, fadeInFrame )

#define FIELD_SOUND_RegisterRequest_FADE_OUT( fieldSound, fadeOutFrame ) \
        FIELD_SOUND_RegisterRequest( fieldSound, FSND_BGM_REQUEST_FADE_OUT, 0, fadeOutFrame, 0 )

#define FIELD_SOUND_RegisterRequest_PUSH( fieldSound, fadeOutFrame ) \
        FIELD_SOUND_RegisterRequest( fieldSound, FSND_BGM_REQUEST_PUSH, 0, fadeOutFrame, 0 )

#define FIELD_SOUND_RegisterRequest_POP( fieldSound, fadeOutFrame, fadeInFrame ) \
        FIELD_SOUND_RegisterRequest( fieldSound, FSND_BGM_REQUEST_POP, 0, fadeOutFrame, fadeInFrame )

#define FIELD_SOUND_RegisterRequest_CHANGE( fieldSound, soundIdx, fadeOutFrame, fadeInFrame ) \
        FIELD_SOUND_RegisterRequest( fieldSound, FSND_BGM_REQUEST_CHANGE, soundIdx, fadeOutFrame, fadeInFrame ) 

#define FIELD_SOUND_RegisterRequest_STAND_BY( fieldSound, soundIdx, fadeOutFrame ) \
        FIELD_SOUND_RegisterRequest( fieldSound, FSND_BGM_REQUEST_STAND_BY, soundIdx, fadeOutFrame, 0 )

#define FIELD_SOUND_RegisterRequest_FORCE_PLAY( fieldSound, soundIdx ) \
        FIELD_SOUND_RegisterRequest( fieldSound, FSND_BGM_REQUEST_FORCE_PLAY, soundIdx, 0, 0 )


//=================================================================================
// ■システム動作
//=================================================================================
extern void FIELD_SOUND_Main ( FIELD_SOUND* fieldSound );
extern void FIELD_SOUND_Reset( FIELD_SOUND* fieldSound, GAMEDATA* gameData );


//=================================================================================
// ■プレイヤーボリューム制御
//
// ※基本的に使用しないでください。
// 　field_sound.h に宣言されている関数を使ってください。
//=================================================================================
extern void FIELD_SOUND_ChangePlayerVolume( FIELD_SOUND* fieldSound, u8 volume, u8 fadeFrame );

//=================================================================================
// ■TVトランシーバー着信音制御
//=================================================================================
extern void FIELD_SOUND_PlayTVTRingTone( FIELD_SOUND* fieldSound );
extern void FIELD_SOUND_StopTVTRingTone( FIELD_SOUND* fieldSound );


//=================================================================================
// ■環境音
//=================================================================================
// 環境音の再生
extern void FIELD_SOUND_PlayEnvSE( FIELD_SOUND* fieldSound, u32 soundIdx );
extern void FIELD_SOUND_PlayEnvSEVol( FIELD_SOUND* fieldSound, u32 soundIdx, u32 vol );
// 環境音のボリューム操作
extern void FIELD_SOUND_SetEnvSEVol( FIELD_SOUND* fieldSound, u32 soundIdx, u32 vol );
// 環境音の停止
extern void FIELD_SOUND_StopEnvSE( FIELD_SOUND* fieldSound, u32 soundIdx );
// 環境音の一時停止
extern void FIELD_SOUND_PauseEnvSE( FIELD_SOUND* fieldSound, FSND_ENVSE_PAUSE_TYPE type );
// 環境音の再始動
extern void FIELD_SOUND_RePlayEnvSE( FIELD_SOUND* fieldSound, FSND_ENVSE_PAUSE_TYPE type );
