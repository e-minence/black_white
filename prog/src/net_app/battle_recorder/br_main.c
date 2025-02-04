//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *  @file   battle_recorder.c
 *  @brief  バトルレコーダーメイン
 *  @author Toru=Nagihashi
 *  @data   2009.11.09
 *
 *  このプロセスは、各アプリケーションプロセスを繋ぐ役割と
 *  アプリケーション間の情報のやりとりをするために存在する。
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//ライブラリ
#include <gflib.h>

//システム
#include "system/gfl_use.h"
#include "system/main.h"  //HEAPID
#include "src\field\field_sound.h"

//プロセス
#include "gamesystem/btl_setup.h"
#include "battle/battle.h"
#include "net_app/wifi_login.h"
#include "net_app/wifi_logout.h"

//自分のモジュール
#include "br_core.h"
#include "br_data.h"
#include "br_snd.h"

//外部公開
#include "net_app/battle_recorder.h"

//-------------------------------------
/// コアプロセス
//=====================================
FS_EXTERN_OVERLAY( battle_recorder_core );

//=============================================================================
/**
 *          定数宣言
*/
//=============================================================================
//-------------------------------------
/// デバッグ
//=====================================
#ifdef PM_DEBUG
static int s_debug_flag = 0;
#endif //PM_DEBUG

//=============================================================================
/**
 *          構造体宣言
*/
//=============================================================================
//-------------------------------------
/// サブプロック移動
//=====================================
typedef void *(*SUBPROC_ALLOC_FUNCTION)( HEAPID heapID, void *p_wk_adrs, u32 pre_procID );
typedef void (*SUBPROC_FREE_FUNCTION)( void *p_param, void *p_wk_adrs );
typedef struct
{
  FSOverlayID             ov_id;
  const GFL_PROC_DATA     *cp_procdata;
  SUBPROC_ALLOC_FUNCTION  alloc_func;
  SUBPROC_FREE_FUNCTION   free_func;
} SUBPROC_DATA;
typedef struct {
  GFL_PROCSYS     *p_procsys;
  u32             seq;
  void            *p_proc_param;
  SUBPROC_DATA    *p_data;

  HEAPID          heapID;
  void            *p_wk_adrs;
  const SUBPROC_DATA      *cp_procdata_tbl;

  u32             pre_procID;
  u32             next_procID;
  u32             now_procID;
} SUBPROC_WORK;

//-------------------------------------
/// メインワーク
//=====================================
typedef struct
{
  //サブプロセスシステム
  SUBPROC_WORK          subproc;

  //常駐におくデータ  （coreは戦闘にいくと解放されるので残しておきたいものはここに）
  BR_DATA               data;

  //引数
  BATTLERECORDER_PARAM  *p_param;
} BR_SYS_WORK;

//=============================================================================
/**
 *          プロトタイプ宣言
*/
//=============================================================================
//-------------------------------------
/// BRメインプロセス
//=====================================
static GFL_PROC_RESULT BR_SYS_PROC_Init
  ( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT BR_SYS_PROC_Exit
  ( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT BR_SYS_PROC_Main
  ( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );

//-------------------------------------
/// サブプロセス
//=====================================
static void SUBPROC_Init( SUBPROC_WORK *p_wk, const SUBPROC_DATA *cp_procdata_tbl, void *p_wk_adrs, HEAPID heapID );
static BOOL SUBPROC_Main( SUBPROC_WORK *p_wk );
static void SUBPROC_Exit( SUBPROC_WORK *p_wk );
static void SUBPROC_CallProc( SUBPROC_WORK *p_wk, u32 procID );

//-------------------------------------
/// サブプロセス用引数の解放、破棄関数
//=====================================
//バトルレコーダーコア
static void *BR_CORE_AllocParam( HEAPID heapID, void *p_wk_adrs, u32 pre_procID );
static void BR_CORE_FreeParam( void *p_param_adrs, void *p_wk_adrs );
//戦闘
static void *BR_BATTLE_AllocParam( HEAPID heapID, void *p_wk_adrs, u32 pre_procID );
static void BR_BATTLE_FreeParam( void *p_param_adrs, void *p_wk_adrs );

//-------------------------------------
///	そのた
//=====================================
static void Br_Music_Modify( u16 *p_default_bgm, u16 *p_win_bgm );

//=============================================================================
/**
 *          データ
*/
//=============================================================================
//-------------------------------------
/// サブプロセス移動データ
//=====================================
typedef enum
{
  SUBPROCID_CORE,
  SUBPROCID_BTLREC,

  SUBPROCID_MAX
} SUBPROC_ID;
static const SUBPROC_DATA sc_subproc_data[SUBPROCID_MAX]  =
{
  //SUBPROCID_CORE
  {
    FS_OVERLAY_ID(battle_recorder_core),
    &BR_CORE_ProcData,
    BR_CORE_AllocParam,
    BR_CORE_FreeParam,
  },
  //SUBPROCID_BTLREC
  {
    FS_OVERLAY_ID(battle),
    &BtlProcData,
    BR_BATTLE_AllocParam,
    BR_BATTLE_FreeParam,
  },
};

//=============================================================================
/**
 *          外部参照
*/
//=============================================================================
//-------------------------------------
/// バトルレコーダープロセス
//=====================================
const GFL_PROC_DATA BattleRecorder_ProcData =
{
  BR_SYS_PROC_Init,
  BR_SYS_PROC_Main,
  BR_SYS_PROC_Exit,
};

//=============================================================================
/**
 *          BRメインプロセス
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  バトルレコーダーPROC読み替え用PROC  初期化
 *
 *  @param  GFL_PROC *p_proc  プロセス
 *  @param  *p_seq            シーケンス
 *  @param  *p_param          親ワーク
 *  @param  *p_work           ワーク
 *
 *  @return 終了コード
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_SYS_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{
  BR_SYS_WORK *p_wk;

  //ヒープ作成
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_BATTLE_RECORDER_SYS, 0x12000 );

  //プロセスワーク作成
  p_wk  = GFL_PROC_AllocWork( p_proc, sizeof(BR_SYS_WORK), HEAPID_BATTLE_RECORDER_SYS );
  GFL_STD_MemClear( p_wk, sizeof(BR_SYS_WORK) );
  p_wk->p_param   = p_param_adrs;

  //録画データ開始
  BattleRec_Init( HEAPID_BATTLE_RECORDER_SYS );

  //モジュール作成
  SUBPROC_Init( &p_wk->subproc, sc_subproc_data, p_wk, HEAPID_BATTLE_RECORDER_SYS );
  SUBPROC_CallProc( &p_wk->subproc, SUBPROCID_CORE );

#ifdef PM_DEBUG
  if( p_wk->p_param->p_gamedata== NULL )
  {
    p_wk->p_param->p_gamedata = GAMEDATA_Create( HEAPID_BATTLE_RECORDER_SYS );
    s_debug_flag  = TRUE;
  }
#endif

  return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *  @brief  バトルレコーダーPROC読み替え用PROC  破棄
 *
 *  @param  GFL_PROC *p_proc  プロセス
 *  @param  *p_seq            シーケンス
 *  @param  *p_param          親ワーク
 *  @param  *p_work           ワーク
 *
 *  @return 終了コード
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_SYS_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{
  BR_SYS_WORK *p_wk = p_wk_adrs;

#ifdef PM_DEBUG
  if( s_debug_flag )
  {
    GAMEDATA_Delete( p_wk->p_param->p_gamedata );
    s_debug_flag  = FALSE;
  }
#endif

  //常駐データでアロックしていたものを破棄
  if( p_wk->data.rec_saveinfo.is_check )
  { 
    int i;
    for( i = 0; i < BR_SAVEDATA_NUM; i++ )
    { 
      if( p_wk->data.rec_saveinfo.p_name[i] )
      { 
        GFL_STR_DeleteBuffer( p_wk->data.rec_saveinfo.p_name[i] );
      }
    }
  }

  //モジュール破棄
  SUBPROC_Exit( &p_wk->subproc );

  //録画データ終了
  BattleRec_Exit();

  //プロセスワーク破棄
  GFL_PROC_FreeWork( p_proc );

  //ヒープ破棄
  GFL_HEAP_DeleteHeap( HEAPID_BATTLE_RECORDER_SYS );

  return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *  @brief  バトルレコーダーPROC読み替え用PROC  メイン処理
 *
 *  @param  GFL_PROC *p_proc  プロセス
 *  @param  *p_seq            シーケンス
 *  @param  *p_param          親ワーク
 *  @param  *p_work           ワーク
 *
 *  @return 終了コード
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_SYS_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{
  enum
  {
    BR_SYS_SEQ_INIT,
    BR_SYS_SEQ_MAIN,
    BR_SYS_SEQ_EXIT,
  };

  BR_SYS_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  {
  case BR_SYS_SEQ_INIT:
    *p_seq  = BR_SYS_SEQ_MAIN;
    break;

  case BR_SYS_SEQ_MAIN:
    {
      if( SUBPROC_Main( &p_wk->subproc ) )
      {
        *p_seq  = BR_SYS_SEQ_EXIT;
      }
    }
    break;

  case BR_SYS_SEQ_EXIT:
    return GFL_PROC_RES_FINISH;
  }

  return GFL_PROC_RES_CONTINUE;
}
//=============================================================================
/**
 *      SUBPROCシステム
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  SUBPROCシステム 初期化
 *
 *  @param  SUBPROC_WORK *p_wk  ワーク
 *  @param  cp_procdata_tbl     プロセス接続テーブル
 *  @param  void *p_wk_adrs     アロック関数と解放関数に渡すワーク
 *  @param  heapID              システム構築用ヒープID
 *
 */
//-----------------------------------------------------------------------------
static void SUBPROC_Init( SUBPROC_WORK *p_wk, const SUBPROC_DATA *cp_procdata_tbl, void *p_wk_adrs, HEAPID heapID )
{
  GFL_STD_MemClear( p_wk, sizeof(SUBPROC_WORK) );
  p_wk->p_procsys       = GFL_PROC_LOCAL_boot( heapID );
  p_wk->p_wk_adrs       = p_wk_adrs;
  p_wk->cp_procdata_tbl = cp_procdata_tbl;
  p_wk->heapID          = heapID;
}

//----------------------------------------------------------------------------
/**
 *  @brief  SUBPROCシステム メイン処理
 *
 *  @param  SUBPROC_WORK *p_wk  ワーク
 *
 *  @retval TRUEならば終了  FALSEならばPROCが存在する
 */
//-----------------------------------------------------------------------------
static BOOL SUBPROC_Main( SUBPROC_WORK *p_wk )
{
  enum
  {
    SEQ_INIT,
    SEQ_ALLOC_PARAM,
    SEQ_MAIN,
    SEQ_FREE_PARAM,
    SEQ_END,
  };

  switch( p_wk->seq )
  {
  case SEQ_INIT:
    p_wk->pre_procID  = p_wk->now_procID;
    p_wk->now_procID  = p_wk->next_procID;
    p_wk->seq = SEQ_ALLOC_PARAM;
    break;

  case SEQ_ALLOC_PARAM:
    //プロセス引数作成関数があれば呼び出し
    if( p_wk->cp_procdata_tbl[ p_wk->now_procID ].alloc_func )
    {
      p_wk->p_proc_param  = p_wk->cp_procdata_tbl[ p_wk->now_procID ].alloc_func(
          p_wk->heapID, p_wk->p_wk_adrs, p_wk->pre_procID );
    }
    else
    {
      p_wk->p_proc_param  = NULL;
    }

    //プロック呼び出し
    GFL_PROC_LOCAL_CallProc( p_wk->p_procsys, p_wk->cp_procdata_tbl[  p_wk->now_procID ].ov_id,
          p_wk->cp_procdata_tbl[  p_wk->now_procID ].cp_procdata, p_wk->p_proc_param );

    p_wk->seq = SEQ_MAIN;
    break;

  case SEQ_MAIN:
    {
      GFL_PROC_MAIN_STATUS result;
      result  = GFL_PROC_LOCAL_Main( p_wk->p_procsys );
      if( GFL_PROC_MAIN_NULL == result )
      {
        p_wk->seq = SEQ_FREE_PARAM;
      }
    }
    break;

  case SEQ_FREE_PARAM:
    //プロセス引数破棄関数呼び出し
    if( p_wk->cp_procdata_tbl[  p_wk->now_procID ].free_func )
    {
      p_wk->cp_procdata_tbl[  p_wk->now_procID ].free_func( p_wk->p_proc_param, p_wk->p_wk_adrs );
      p_wk->p_proc_param  = NULL;
    }

    //もし次のプロセスがあれば呼び出し
    //なければ終了
    if( p_wk->now_procID  != p_wk->next_procID )
    {
      p_wk->seq = SEQ_INIT;
    }
    else
    {
      p_wk->seq = SEQ_END;
    }
    break;

  case SEQ_END:
    return TRUE;
  }

  return FALSE;
}
//----------------------------------------------------------------------------
/**
 *  @brief  SUBPROCシステム 破棄
 *
 *  @param  SUBPROC_WORK *p_wk  ワーク
 *
 */
//-----------------------------------------------------------------------------
static void SUBPROC_Exit( SUBPROC_WORK *p_wk )
{
  GF_ASSERT_HEAVY( p_wk->p_proc_param == NULL );

  GFL_PROC_LOCAL_Exit( p_wk->p_procsys );
  GFL_STD_MemClear( p_wk, sizeof(SUBPROC_WORK) );
}

//----------------------------------------------------------------------------
/**
 *  @brief  SUBPROCシステム プロセスリクエスト
 *
 *  @param  SUBPROC_WORK *p_wk  ワーク
 *  @param  proc_id             呼ぶプロセスID
 *
 */
//-----------------------------------------------------------------------------
static void SUBPROC_CallProc( SUBPROC_WORK *p_wk, u32 procID )
{
  p_wk->next_procID = procID;
}

//=============================================================================
/**
 *    サブプロセス用作成破棄
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  バトルレコーダーコアの引数  作成
 *
 *  @param  HEAPID heapID     ヒープID
 *  @param  *p_wk_adrs        ワーク
 *  @param  pre_procID
 *
 *  @return 引数
 */
//-----------------------------------------------------------------------------
static void *BR_CORE_AllocParam( HEAPID heapID, void *p_wk_adrs, u32 pre_procID )
{
  BR_CORE_PARAM *p_param;

  BR_SYS_WORK   *p_wk = p_wk_adrs;

  p_param = GFL_HEAP_AllocMemory( heapID, sizeof(BR_CORE_PARAM) );
  GFL_STD_MemClear( p_param, sizeof(BR_CORE_PARAM) );
  p_param->p_param  = p_wk->p_param;
  p_param->p_data   = &p_wk->data;

  if( pre_procID == SUBPROCID_BTLREC )
  {
    p_param->mode = BR_CORE_MODE_RETURN;
  }
  else
  {
    p_param->mode = BR_CORE_MODE_INIT;
  }

  return p_param;;
}
//----------------------------------------------------------------------------
/**
 *  @brief  バトルレコーダーコアの引数  破棄
 *
 *  @param  void *p_param_adrs        引数
 *  @param  *p_wk_adrs                ワーク
 */
//-----------------------------------------------------------------------------
static void BR_CORE_FreeParam( void *p_param_adrs, void *p_wk_adrs )
{
  BR_CORE_PARAM *p_param  = p_param_adrs;
  BR_SYS_WORK   *p_wk     = p_wk_adrs;

  switch( p_param->ret )
  {
  case BR_CORE_RETURN_BTLREC:
    SUBPROC_CallProc( &p_wk->subproc, SUBPROCID_BTLREC );
    break;

  case BR_CORE_RETURN_FINISH:
    break;
  }

  GFL_HEAP_FreeMemory( p_param );
}
//----------------------------------------------------------------------------
/**
 *  @brief  バトルの引数  破棄
 *
 *  @param  void *p_param_adrs        引数
 *  @param  *p_wk_adrs                ワーク
 */
//-----------------------------------------------------------------------------
static void *BR_BATTLE_AllocParam( HEAPID heapID, void *p_wk_adrs, u32 pre_procID )
{
  BR_SYS_WORK   *p_wk     = p_wk_adrs;
  BATTLE_SETUP_PARAM  *p_param;
  GAMEDATA            *p_gamedata;

  p_param = GFL_HEAP_AllocMemory( heapID, sizeof(BATTLE_SETUP_PARAM) );
  GFL_STD_MemClear( p_param, sizeof(BATTLE_SETUP_PARAM) );
  p_gamedata  = p_wk->p_param->p_gamedata;

  BTL_SETUP_InitForRecordPlay( p_param, p_gamedata, heapID );

  BattleRec_LoadToolModule();
  BattleRec_RestoreSetupParam( p_param, heapID );
  BattleRec_UnloadToolModule();

  PMSND_PauseBGM(TRUE);
  PMSND_PushBGM();

  //アッパーバージョンで増える可能性があるので、
  //今回録画される曲にすべて置換する
  Br_Music_Modify( &p_param->musicDefault, &p_param->musicWin );

  PMSND_PlayBGM( p_param->musicDefault );

  return p_param;
}
//----------------------------------------------------------------------------
/**
 *  @brief  バトルの引数  破棄
 *
 *  @param  void *p_param_adrs        引数
 *  @param  *p_wk_adrs                ワーク
 */
//-----------------------------------------------------------------------------
static void BR_BATTLE_FreeParam( void *p_param_adrs, void *p_wk_adrs )
{
  BR_SYS_WORK         *p_wk     = p_wk_adrs;
  BATTLE_SETUP_PARAM  *p_param  = p_param_adrs;

  { 
    FIELD_SOUND *p_fld_snd  = GAMEDATA_GetFieldSound( p_wk->p_param->p_gamedata );

    PMSND_PopBGM();
    PMSND_PauseBGM(FALSE);
    PMSND_FadeInBGM( PMSND_FADE_SHORT );

    if( p_wk->p_param->mode == BR_MODE_BROWSE )
    { 
      FSND_HoldBGMVolume_inApp( p_fld_snd );
    }
  }


  p_wk->data.is_recplay_finish = p_param->recPlayCompleteFlag;
  NAGI_Printf( "バトルビデオ全部みたか？ %d\n", p_param->recPlayCompleteFlag );

#ifdef PM_DEBUG
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L )
  { 
    p_wk->data.is_recplay_finish  = TRUE;
  }
#endif

  GFL_HEAP_FreeMemory( p_param->playerStatus[ BTL_CLIENT_PLAYER ] );  //プレイヤーのMySatusは開放されないので
  BTL_SETUP_QuitForRecordPlay( p_param );
  GFL_HEAP_FreeMemory( p_param );

  SUBPROC_CallProc( &p_wk->subproc, SUBPROCID_CORE );
}

//----------------------------------------------------------------------------
/**
 *	@brief  BGMを置き換え
 *
 *	@param	u16 *p_default_bgm  通常曲  [in/out]
 *	@param	*p_win_bgm          勝利曲  [in/out]
 */
//-----------------------------------------------------------------------------
static void Br_Music_Modify( u16 *p_default_bgm, u16 *p_win_bgm )
{
  const int pre_default_bgm = *p_default_bgm;
  const int pre_win_bgm     = *p_win_bgm;

  //アッパーバージョンで増える可能性があるので、
  //今回録画される曲にすべて置換する

  //戦闘曲
  switch( pre_default_bgm )
  { 
  //置換しない曲
  case SEQ_BGM_VS_SUBWAY_TRAINER:
  case SEQ_BGM_VS_CHAMP:
  case SEQ_BGM_VS_TRAINER_WIFI:
  case SEQ_BGM_VS_WCS:
    /* そのままなのでなにもしない */
    break;
  //置換するもの
  case SEQ_BGM_VS_TRAINER_M:
  case SEQ_BGM_VS_TRAINER_S:
  default:
    *p_default_bgm = SEQ_BGM_VS_TRAINER_WIFI;
    break;
  }

  //勝利曲
  switch( pre_win_bgm )
  { 
  //置換しない曲
  case SEQ_BGM_WIN2:
  case SEQ_BGM_WIN5:
    /* そのままなのでなにもしない */
    break;
  //置換するもの
  default:
    *p_win_bgm = SEQ_BGM_WIN2;
    break;
  }
}
