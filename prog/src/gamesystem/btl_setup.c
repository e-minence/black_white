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

#include "gamesystem/btl_setup.h"

///プロトタイプ
void BATTLE_PARAM_Init( BATTLE_SETUP_PARAM* bp );
void BATTLE_PARAM_Release( BATTLE_SETUP_PARAM* bp );

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

static void setup_common( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData, BTL_FIELD_SITUATION* sit )

{
  dst->netHandle = NULL;
  dst->commMode = BTL_COMM_NONE;
  dst->commPos = 0;
  dst->netID = 0;
  dst->multiMode = 0;

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
void BTL_SETUP_Wild( BATTLE_SETUP_PARAM* bp, GAMEDATA* gdata,
  const POKEPARTY* partyEnemy, const BTL_FIELD_SITUATION* sit, const BtlRule rule, HEAPID heapID )
{
  BATTLE_PARAM_Init( bp );
  setup_common( bp, gdata, (BTL_FIELD_SITUATION*)sit );

  bp->partyPlayer = PokeParty_AllocPartyWork( heapID );
  PokeParty_Copy( GAMEDATA_GetMyPokemon(gdata), bp->partyPlayer );

  bp->partyEnemy1 = PokeParty_AllocPartyWork( heapID );
  if( partyEnemy != NULL ){
    PokeParty_Copy( partyEnemy, bp->partyEnemy1 );
  }

  bp->competitor = BTL_COMPETITOR_WILD;
  bp->rule = rule;

  bp->trID = TRID_NULL;
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
static void setup_common_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData, BtlRule rule,
  POKEPARTY* partyEnemy, BTL_FIELD_SITUATION* sit, TrainerID trID, HEAPID heapID )
{
  BATTLE_PARAM_Init( dst );
  setup_common( dst, gameData, sit );

  dst->partyPlayer = PokeParty_AllocPartyWork( heapID );
  PokeParty_Copy( GAMEDATA_GetMyPokemon(gameData), dst->partyPlayer );
  
  dst->partyEnemy1 = PokeParty_AllocPartyWork( heapID );
  if( partyEnemy != NULL ){
    PokeParty_Copy( partyEnemy, dst->partyEnemy1 );
  }

  dst->competitor = BTL_COMPETITOR_TRAINER;
  dst->rule = rule;

  dst->trID = trID;
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

  dst->partyPlayer = PokeParty_AllocPartyWork( heapID );
  PokeParty_Copy( GAMEDATA_GetMyPokemon(gameData), dst->partyPlayer );
  
  dst->competitor = BTL_COMPETITOR_COMM;
  dst->rule = rule;

  dst->netHandle = netHandle;
  dst->commMode = commMode;
  dst->netID = GFL_NET_GetNetID( netHandle );
  dst->multiMode = multi;

  dst->trID = TRID_NULL;
}

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
  POKEPARTY* partyEnemy, BTL_FIELD_SITUATION* sit, TrainerID trID, HEAPID heapID )
{
  setup_common_Trainer( dst, gameData, BTL_RULE_SINGLE, partyEnemy, sit, trID, heapID );
}

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
 * ダブル ゲーム内トレーナー対戦
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
void BTL_SETUP_Double_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  POKEPARTY* partyEnemy, BTL_FIELD_SITUATION* sit, TrainerID trID, HEAPID heapID )
{
  setup_common_Trainer( dst, gameData, BTL_RULE_DOUBLE, partyEnemy, sit, trID, heapID );
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
 * トリプル ゲーム内トレーナー対戦
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
void BTL_SETUP_Triple_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  POKEPARTY* partyEnemy, BTL_FIELD_SITUATION* sit, TrainerID trID, HEAPID heapID )
{
  setup_common_Trainer( dst, gameData, BTL_RULE_TRIPLE, partyEnemy, sit, trID, heapID );
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
 * ローテーション ゲーム内トレーナー対戦
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
void BTL_SETUP_Rotation_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  POKEPARTY* partyEnemy, BTL_FIELD_SITUATION* sit, TrainerID trID, HEAPID heapID )
{
  setup_common_Trainer( dst, gameData, BTL_RULE_ROTATION, partyEnemy, sit, trID, heapID );
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
