/*
 *  @file   btl_setup.c
 *  @brief  バトルパラメータセットアップ
 *  @author Taya & Iwasawa
 *  @date   09.10.07
 */

#include <gflib.h>
#include "system/main.h"
#include "gamesystem/pm_season.h"
#include "sound/wb_sound_data.sadl" //サウンドラベルファイル
#include "tr_tool/tr_tool.h"
#include "tr_tool/trno_def.h"
#include "tr_tool/trtype_def.h"
#include "buflen.h"

#include "gamesystem/btl_setup.h"

//------------------------------------------------------------
//------------------------------------------------------------
enum {
  BTL_RECORD_BUFFER_SIZE = 4096,
};


///プロトタイプ
void BATTLE_PARAM_Init( BATTLE_SETUP_PARAM* bp );
void BATTLE_PARAM_Release( BATTLE_SETUP_PARAM* bp );

static BSP_TRAINER_DATA* BSP_TRAINER_DATA_Create( HEAPID heapID );
static void BSP_TRAINER_DATA_Delete( BSP_TRAINER_DATA* tr_data );
static void setup_common_situation( BTL_FIELD_SITUATION* sit );

/*
 *  @brief  戦闘パラメータワーク生成
 *  @param  heapID  ワークメモリを確保するheapID
 *
 *  ＊BATTLE_SETUP_PARAM構造体領域をアロケートし、初期化します
 *  ＊デフォルトのパラメータで構造体を初期化します。バトルタイプに応じて、必要な初期化を追加で行ってください
 *  ＊必ず BATTLE_PARAM_Delete()で解放してください
 */
BATTLE_SETUP_PARAM* BATTLE_PARAM_Create( HEAPID heapID )
{
  BATTLE_SETUP_PARAM* bp;

  bp = GFL_HEAP_AllocMemory( heapID, sizeof(BATTLE_SETUP_PARAM) );
  BATTLE_PARAM_Init( bp );
  return bp;
}

/*
 *  @brief  戦闘パラメータワーク解放
 *  @param  bp  BATTLE_PARAM_Create()で生成されたBATTLE_SETUP_PARAM構造体型ワークへのポインタ
 */
void BATTLE_PARAM_Delete( BATTLE_SETUP_PARAM* bp )
{
  BATTLE_PARAM_Release( bp );
  GFL_HEAP_FreeMemory( bp );
  bp = NULL;
}

/**
 *  @brief  戦闘パラメータワークの内部初期化処理
 *  @param  bp  確保済みのBATTLE_SETUP_PARAM構造体型ワークへのポインタ
 *
 *  ＊デフォルトのパラメータで構造体を初期化します。バトルタイプに応じて、必要な初期化を追加で行ってください
 *  ＊使い終わったら必ずBATTLE_PARAM_Release()関数で解放処理をしてください
 */
void BATTLE_PARAM_Init( BATTLE_SETUP_PARAM* bp )
{
  MI_CpuClear8( bp, sizeof(BATTLE_SETUP_PARAM) );
  setup_common_situation( &bp->fieldSituation );
}

/*
 *  @brief  戦闘パラメータワークの内部アロケーションメモリ解放とクリア
 *  @param  bp  確保済みのBATTLE_SETUP_PARAM構造体型ワークへのポインタ
 */
void BATTLE_PARAM_Release( BATTLE_SETUP_PARAM* bp )
{
  int i;
  if(bp->partyPlayer){
    GFL_HEAP_FreeMemory(bp->partyPlayer);
  }
  if(bp->partyPartner){
    GFL_HEAP_FreeMemory(bp->partyPartner);
  }
  if(bp->partyEnemy1){
    GFL_HEAP_FreeMemory(bp->partyEnemy1);
  }
  if(bp->partyEnemy2){
    GFL_HEAP_FreeMemory(bp->partyEnemy2);
  }
  for( i = 0;i < BTL_CLIENT_NUM;i++){
    if(bp->tr_data[i] != NULL){
      BSP_TRAINER_DATA_Delete( bp->tr_data[i]);
    }
  }
  if( bp->recBuffer ){
    GFL_HEAP_FreeMemory( bp->recBuffer );
  }

  MI_CpuClear8(bp,sizeof(BATTLE_SETUP_PARAM));
}

/*
 *  @brief  バトルパラム　PokePartyデータセット
 *
 *  渡されたポインタの内容をコピーします
 */
void BATTLE_PARAM_SetPokeParty( BATTLE_SETUP_PARAM* bp, const POKEPARTY* party, BTL_CLIENT_ID client )
{
  GF_ASSERT( party );

  switch( client ){
  case BTL_CLIENT_PLAYER:
    PokeParty_Copy( party, bp->partyPlayer );
    break;
  case BTL_CLIENT_PARTNER:
    PokeParty_Copy( party, bp->partyPartner );
    break;
  case BTL_CLIENT_ENEMY1:
    PokeParty_Copy( party, bp->partyEnemy1 );
    break;
  case BTL_CLIENT_ENEMY2:
    PokeParty_Copy( party, bp->partyEnemy2 );
    break;
  }
}

/*
 *  @brief  バトルパラム　PokePartyポインタ取得
 */
POKEPARTY* BATTLE_PARAM_GetPokePartyPointer( BATTLE_SETUP_PARAM* bp, BTL_CLIENT_ID client )
{
  switch( client ){
  case BTL_CLIENT_PLAYER:
    return bp->partyPlayer;
  case BTL_CLIENT_PARTNER:
    return bp->partyPartner;
  case BTL_CLIENT_ENEMY1:
    return bp->partyEnemy1;
  case BTL_CLIENT_ENEMY2:
    return bp->partyEnemy2;
  }
  GF_ASSERT(0);
  return NULL;
}

/*
 *  @brief  戦闘フィールドシチュエーションデータデフォルト初期化
 */
void BTL_FIELD_SITUATION_Init( BTL_FIELD_SITUATION* sit )
{
  setup_common_situation( sit );
}

static void setup_common_situation( BTL_FIELD_SITUATION* sit )
{
  // @todo 現在は仮作成
  sit->bgType = BATTLE_BG_TYPE_GRASS;
  sit->bgAttr = BATTLE_BG_ATTR_LAWN;
  sit->timeZone = TIMEZONE_MORNING;
  sit->season = PMSEASON_SPRING;
  sit->weather = BTL_WEATHER_NONE;
}

/*
 *  @brief  戦闘トレーナーデータワーク初期化
 *
 *  内部でメモリをアロケートするため、必ずBSP_TRAINER_DATA_Createで解放してください
 */
static BSP_TRAINER_DATA* BSP_TRAINER_DATA_Create( HEAPID heapID )
{
  BSP_TRAINER_DATA* tr_data = GFL_HEAP_AllocClearMemory( heapID, sizeof( BSP_TRAINER_DATA ) );
  tr_data->name =   GFL_STR_CreateBuffer( BUFLEN_PERSON_NAME, heapID );

  return tr_data;
}

/*
 *  @brief  戦闘トレーナーデータワーク解放
 *
 *  内部でメモリをアロケートするため、必ず解放してください
 */
static void BSP_TRAINER_DATA_Delete( BSP_TRAINER_DATA* tr_data )
{
  GFL_STR_DeleteBuffer( tr_data->name );

  GFL_HEAP_FreeMemory( tr_data );
}

/*
 *  @brief  トレーナーパラメータセット
 */
static void setup_trainer_param( BATTLE_SETUP_PARAM* dst, BTL_CLIENT_ID client, POKEPARTY** party, TrainerID tr_id, HEAPID heapID )
{
  *party = PokeParty_AllocPartyWork( heapID );
  dst->tr_data[client] = BSP_TRAINER_DATA_Create( heapID );

  if(tr_id != TRID_NULL){
    TT_EncountTrainerPersonalDataMake( tr_id, dst->tr_data[client], heapID );
    TT_EncountTrainerPokeDataMake( tr_id, *party, heapID );
  }
}

/*
 *  @brief  プレイヤーパラメータセット
 */
static void setup_player_param( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData, HEAPID heapID )
{
  BSP_TRAINER_DATA* tr_data;
  PLAYER_WORK * player = GAMEDATA_GetMyPlayerWork( gameData );

  dst->partyPlayer = PokeParty_AllocPartyWork( heapID );
  PokeParty_Copy( GAMEDATA_GetMyPokemon( gameData ), dst->partyPlayer );

  dst->tr_data[BTL_CLIENT_PLAYER] = BSP_TRAINER_DATA_Create( heapID );
  tr_data = dst->tr_data[BTL_CLIENT_PLAYER];

  MyStatus_CopyNameString( (const MYSTATUS*)&player->mystatus , tr_data->name );
  tr_data->tr_type = TRTYPE_HERO + MyStatus_GetMySex( (const MYSTATUS*)&player->mystatus );
}

static void setup_common( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData, BTL_FIELD_SITUATION* sit )
{
  dst->netHandle = NULL;
  dst->commMode = BTL_COMM_NONE;
  dst->commPos = 0;
  dst->netID = 0;
  dst->multiMode = 0;
  dst->recBuffer = NULL;
  dst->fRecordPlay = FALSE;

  dst->partyPlayer = NULL;
  dst->partyEnemy1 = NULL;
  dst->partyPartner = NULL;
  dst->partyEnemy2 = NULL;

  dst->statusPlayer = GAMEDATA_GetMyStatus( gameData );
  dst->itemData     = GAMEDATA_GetMyItem( gameData );
  dst->bagCursor    = GAMEDATA_GetBagCursor( gameData );
  dst->zukanData    = GAMEDATA_GetZukanSave( gameData );
  {
    // @todo いずれSAVE_CONTROL_WORKに直アクセスしないように書き換える？
    SAVE_CONTROL_WORK* saveCtrl = GAMEDATA_GetSaveControlWork( gameData );
    dst->configData = SaveData_GetConfig( saveCtrl );
  }
  MI_CpuCopy8( sit, &dst->fieldSituation, sizeof( BTL_FIELD_SITUATION ) );

  dst->musicDefault = SEQ_BGM_VS_NORAPOKE;
  dst->musicPinch = SEQ_BGM_BATTLEPINCH;

  dst->result = BTL_RESULT_WIN;
}

//=============================================================================================
/**
 * @brief 野生戦コモンパラメータセット
 *
 * @param   dst
 * @param   gameData
 * @param   heapID
 * @param   rule
 * @param   partyEnemy
 * @param   sit
 *
 * @li  内部でメモリの確保を行います。
 *      バトル終了後、BATTLE_PARAM_Release()もしくはBATTLE_PARAM_Delete()で必ず解放処理を行ってください
 */
//=============================================================================================
void BTL_SETUP_Wild( BATTLE_SETUP_PARAM* bp, GAMEDATA* gameData,
  const POKEPARTY* partyEnemy, const BTL_FIELD_SITUATION* sit, const BtlRule rule, HEAPID heapID )
{
  BATTLE_PARAM_Init( bp );
  setup_common( bp, gameData, (BTL_FIELD_SITUATION*)sit );
  setup_player_param( bp, gameData, heapID );

  bp->partyEnemy1 = PokeParty_AllocPartyWork( heapID );
  if( partyEnemy != NULL ){
    PokeParty_Copy( partyEnemy, bp->partyEnemy1 );
  }

  bp->competitor = BTL_COMPETITOR_WILD;
  bp->rule = rule;
}

//=============================================================================================
/**
 * ゲーム内トレーナー対戦コモン
 *
 * @param   dst
 * @param   gameData
 * @param   partyEnemy
 * @param   landForm
 * @param   weather
 * @param   trID
 *
 * @li  内部でメモリの確保を行います。
 *      バトル終了後、BATTLE_PARAM_Release()もしくはBATTLE_PARAM_Delete()で必ず解放処理を行ってください
 */
//=============================================================================================
static void setup_common_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  BtlRule rule, BTL_FIELD_SITUATION* sit, HEAPID heapID )
{
  BATTLE_PARAM_Init( dst );
  setup_common( dst, gameData, sit );
  setup_player_param( dst, gameData, heapID );

  dst->tr_data[BTL_CLIENT_PLAYER] = BSP_TRAINER_DATA_Create( heapID );

  dst->competitor = BTL_COMPETITOR_TRAINER;
  dst->rule = rule;
}

//=============================================================================================
/**
 * 通信対戦コモン
 *
 * @param   dst
 * @param   gameData
 * @param   netHandle
 * @param   commMode
 *
 * @li  内部でメモリの確保を行います。
 *      バトル終了後、BATTLE_PARAM_Release()もしくはBATTLE_PARAM_Delete()で必ず解放処理を行ってください
 */
//=============================================================================================
static void setup_common_CommTrainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  BtlRule rule, BOOL multi, GFL_NETHANDLE* netHandle, BtlCommMode commMode, HEAPID heapID )
{
  BTL_FIELD_SITUATION sit;

  BATTLE_PARAM_Init( dst );

  BTL_FIELD_SITUATION_Init(&sit);
  sit.bgType = BATTLE_BG_TYPE_ROOM;
  sit.bgAttr = BATTLE_BG_ATTR_NORMAL_GROUND;

  setup_common( dst, gameData, &sit );
  setup_player_param( dst, gameData, heapID );

  dst->competitor = BTL_COMPETITOR_COMM;
  dst->rule = rule;

  dst->netHandle = netHandle;
  dst->commMode = commMode;
  dst->netID = GFL_NET_GetNetID( netHandle );
  dst->multiMode = multi;
}

///////////////////////////////////////////////////////////////////////////////
//  ローカルトレーナー対戦関連
///////////////////////////////////////////////////////////////////////////////

//=============================================================================================
/**
 * シングル ゲーム内トレーナー対戦
 *
 * @param   dst
 * @param   gameData
 * @param   partyEnemy
 * @param   sit
 * @param   trID
 *
 * @li  内部でメモリの確保を行います。
 *      バトル終了後、BATTLE_PARAM_Release()もしくはBATTLE_PARAM_Delete()で必ず解放処理を行ってください
 */
//=============================================================================================
void BTL_SETUP_Single_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  BTL_FIELD_SITUATION* sit, TrainerID trID, HEAPID heapID )
{
  setup_common_Trainer( dst, gameData, BTL_RULE_SINGLE, sit, heapID );

  setup_trainer_param( dst, BTL_CLIENT_ENEMY1, &dst->partyEnemy1, trID, heapID );
}

//=============================================================================================
/**
 * ダブル ゲーム内トレーナー対戦
 *
 * @param   dst
 * @param   gameData
 * @param   landForm
 * @param   weather
 * @param   trID
 *
 * @li  内部でメモリの確保を行います。
 *      バトル終了後、BATTLE_PARAM_Release()もしくはBATTLE_PARAM_Delete()で必ず解放処理を行ってください
 */
//=============================================================================================
void BTL_SETUP_Double_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  BTL_FIELD_SITUATION* sit, TrainerID trID, HEAPID heapID )
{
  setup_common_Trainer( dst, gameData, BTL_RULE_DOUBLE, sit, heapID );

  setup_trainer_param( dst, BTL_CLIENT_ENEMY1, &dst->partyEnemy1, trID, heapID );
}
//=============================================================================================
/**
 * トリプル ゲーム内トレーナー対戦
 *
 * @param   dst
 * @param   gameData
 * @param   landForm
 * @param   weather
 * @param   trID
 *
 * @li  内部でメモリの確保を行います。
 *      バトル終了後、BATTLE_PARAM_Release()もしくはBATTLE_PARAM_Delete()で必ず解放処理を行ってください
 */
//=============================================================================================
void BTL_SETUP_Triple_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  BTL_FIELD_SITUATION* sit, TrainerID trID, HEAPID heapID )
{
  setup_common_Trainer( dst, gameData, BTL_RULE_TRIPLE, sit, heapID );
  setup_trainer_param( dst, BTL_CLIENT_ENEMY1, &dst->partyEnemy1, trID, heapID );
}

//=============================================================================================
/**
 * ローテーション ゲーム内トレーナー対戦
 *
 * @param   dst
 * @param   gameData
 * @param   landForm
 * @param   weather
 * @param   trID
 *
 * @li  内部でメモリの確保を行います。
 *      バトル終了後、BATTLE_PARAM_Release()もしくはBATTLE_PARAM_Delete()で必ず解放処理を行ってください
 */
//=============================================================================================
void BTL_SETUP_Rotation_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  BTL_FIELD_SITUATION* sit, TrainerID trID, HEAPID heapID )
{
  setup_common_Trainer( dst, gameData, BTL_RULE_ROTATION, sit, heapID );
  setup_trainer_param( dst, BTL_CLIENT_ENEMY1, &dst->partyEnemy1, trID, heapID );
}

//=============================================================================================
/**
 * タッグ ゲーム内トレーナー対戦
 *
 * @param   dst
 * @param   gameData
 * @param   partyEnemy
 * @param   landForm
 * @param   weather
 * @param   trID
 *
 * @li  内部でメモリの確保を行います。
 *      バトル終了後、BATTLE_PARAM_Release()もしくはBATTLE_PARAM_Delete()で必ず解放処理を行ってください
 */
//=============================================================================================
void BTL_SETUP_Tag_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  BTL_FIELD_SITUATION* sit, TrainerID tr_id0, TrainerID tr_id1, HEAPID heapID )
{
  setup_common_Trainer( dst, gameData, BTL_RULE_DOUBLE, sit, heapID );

  setup_trainer_param( dst, BTL_CLIENT_ENEMY1, &dst->partyEnemy1, tr_id0, heapID );
  setup_trainer_param( dst, BTL_CLIENT_ENEMY2, &dst->partyEnemy2, tr_id1, heapID );
}

//=============================================================================================
/**
 * AIマルチ ゲーム内トレーナー対戦
 *
 * @param   dst
 * @param   gameData
 * @param   partyEnemy
 * @param   landForm
 * @param   weather
 * @param   trID
 *
 * @li  内部でメモリの確保を行います。
 *      バトル終了後、BATTLE_PARAM_Release()もしくはBATTLE_PARAM_Delete()で必ず解放処理を行ってください
 */
//=============================================================================================
void BTL_SETUP_AIMulti_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  BTL_FIELD_SITUATION* sit, TrainerID partner, TrainerID tr_id0, TrainerID tr_id1, HEAPID heapID )
{
  setup_common_Trainer( dst, gameData, BTL_RULE_DOUBLE, sit, heapID );

  setup_trainer_param( dst, BTL_CLIENT_PARTNER, &dst->partyPartner, partner, heapID );
  setup_trainer_param( dst, BTL_CLIENT_ENEMY1, &dst->partyEnemy1, tr_id0, heapID );
  setup_trainer_param( dst, BTL_CLIENT_ENEMY2, &dst->partyEnemy2, tr_id1, heapID );
}

///////////////////////////////////////////////////////////////////////////////
//  通信対戦関連
///////////////////////////////////////////////////////////////////////////////
//=============================================================================================
/**
 * シングル 通信対戦
 *
 * @param   dst
 * @param   gameData
 * @param   netHandle
 * @param   commMode
 *
 * @li  内部でメモリの確保を行います。
 *      バトル終了後、BATTLE_PARAM_Release()もしくはBATTLE_PARAM_Delete()で必ず解放処理を行ってください
 */
//=============================================================================================
void BTL_SETUP_Single_Comm( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  GFL_NETHANDLE* netHandle, BtlCommMode commMode, HEAPID heapID )
{
  setup_common_CommTrainer( dst, gameData, BTL_RULE_SINGLE, FALSE, netHandle, commMode, heapID );
}

//=============================================================================================
/**
 * ダブル 通信対戦
 *
 * @param   dst
 * @param   gameData
 * @param   netHandle
 * @param   commMode
 *
 * @li  内部でメモリの確保を行います。
 *      バトル終了後、BATTLE_PARAM_Release()もしくはBATTLE_PARAM_Delete()で必ず解放処理を行ってください
 */
//=============================================================================================
void BTL_SETUP_Double_Comm( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  GFL_NETHANDLE* netHandle, BtlCommMode commMode, HEAPID heapID )
{
  setup_common_CommTrainer( dst, gameData, BTL_RULE_DOUBLE, FALSE, netHandle, commMode, heapID );
}


//=============================================================================================
/**
 * ダブル（マルチ）通信対戦
 *
 * @param   dst
 * @param   gameData
 * @param   netHandle
 * @param   commMode
 * @param   commPos
 *
 * @li  内部でメモリの確保を行います。
 *      バトル終了後、BATTLE_PARAM_Release()もしくはBATTLE_PARAM_Delete()で必ず解放処理を行ってください
 */
//=============================================================================================
void BTL_SETUP_Multi_Comm( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  GFL_NETHANDLE* netHandle, BtlCommMode commMode, u8 commPos, HEAPID heapID )
{
  setup_common_CommTrainer( dst, gameData, BTL_RULE_DOUBLE, TRUE, netHandle, commMode, heapID );
}

//=============================================================================================
/**
 * トリプル 通信対戦
 *
 * @param   dst
 * @param   gameData
 * @param   netHandle
 * @param   commMode
 *
 * @li  内部でメモリの確保を行います。
 *      バトル終了後、BATTLE_PARAM_Release()もしくはBATTLE_PARAM_Delete()で必ず解放処理を行ってください
 */
//=============================================================================================
void BTL_SETUP_Triple_Comm( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  GFL_NETHANDLE* netHandle, BtlCommMode commMode, HEAPID heapID )
{
  setup_common_CommTrainer( dst, gameData, BTL_RULE_TRIPLE, FALSE, netHandle, commMode, heapID );
}


//=============================================================================================
/**
 * ローテーション 通信対戦
 *
 * @param   dst
 * @param   gameData
 * @param   netHandle
 * @param   commMode
 *
 * @li  内部でメモリの確保を行います。
 *      バトル終了後、BATTLE_PARAM_Release()もしくはBATTLE_PARAM_Delete()で必ず解放処理を行ってください
 */
//=============================================================================================
void BTL_SETUP_Rotation_Comm( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  GFL_NETHANDLE* netHandle, BtlCommMode commMode, HEAPID heapID )
{
  setup_common_CommTrainer( dst, gameData, BTL_RULE_ROTATION, FALSE, netHandle, commMode, heapID );
}


/*
 *  @brief  セットアップ済みパラメータをバトルサブウェイモード用に切り替え
 */
void BTL_SETUP_SetSubwayMode( BATTLE_SETUP_PARAM* dst )
{
  if( dst->competitor == BTL_COMPETITOR_TRAINER ){
    dst->competitor = BTL_COMPETITOR_SUBWAY;
  }
}
/*
 *  @brief  セットアップ済みパラメータに録画用バッファを生成
 */
void BTL_SETUP_AllocRecBuffer( BATTLE_SETUP_PARAM* dst, HEAPID heapID )
{
  if( dst->recBuffer == NULL ){
    dst->recBuffer = GFL_HEAP_AllocMemory( heapID, BTL_RECORD_BUFFER_SIZE );
  }
}
/*
 *  @brief セットアップパラメータを録画再生モードに切り替え
 */
void BTL_SETUP_SetRecordPlayMode( BATTLE_SETUP_PARAM* dst )
{
  dst->fRecordPlay = TRUE;
}

/*
 *  @brief デバッグ用フラグセット
 */
void BTL_SETUP_SetDebugFlag( BATTLE_SETUP_PARAM* dst, BtlDebugFlag flag )
{
  GF_ASSERT(flag < BTL_DEBUGFLAG_MAX);

  dst->DebugFlag |= (1 << flag);
}


