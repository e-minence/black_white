/*
 *  @file   btl_setup.c
 *  @brief  バトルパラメータセットアップ
 *  @author Taya & Iwasawa
 *  @date   09.10.07
 */

#include "sound/wb_sound_data.sadl" //サウンドラベルファイル
#include "gamesystem/btl_setup.h"

///プロトタイプ
void BATTLE_PARAM_Init( BATTLE_SETUP_PARAM* bp );
void BATTLE_PARAM_Release( BATTLE_SETUP_PARAM* bp );

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

static void setup_common( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData, BtlLandForm landForm, BtlWeather weather )
{
  dst->netHandle = NULL;
  dst->commMode = BTL_COMM_NONE;
  dst->commPos = 0;
  dst->netID = 0;
  dst->multiMode = 0;
  dst->landForm = landForm;
  dst->weather  = weather;

  dst->partyPlayer = NULL;  //GAMEDATA_GetMyPokemon( gameData );
  dst->partyEnemy1 = NULL;
  dst->partyPartner = NULL;
  dst->partyEnemy2 = NULL;

  dst->statusPlayer = GAMEDATA_GetMyStatus( gameData );
  dst->itemData     = GAMEDATA_GetMyItem( gameData );
  dst->bagCursor    = GAMEDATA_GetBagCursor( gameData );

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
 * @param   partyEnemy
 * @param   landForm
 * @param   weather
 */
//=============================================================================================
void BP_SETUP_Wild( BATTLE_SETUP_PARAM* bp, GAMEDATA* gdata, HEAPID heapID, const BtlRule rule, 
  const POKEPARTY* partyEnemy, const BtlLandForm landForm, const BtlWeather weather )
{
  setup_common( bp, gdata, landForm, weather );
  
  bp->partyPlayer = PokeParty_AllocPartyWork( heapID );
  PokeParty_Copy( GAMEDATA_GetMyPokemon(gdata), bp->partyPlayer );

  bp->partyEnemy1 = (POKEPARTY*)partyEnemy;
  
  bp->engine = BTL_ENGINE_ALONE;
  bp->competitor = BTL_COMPETITOR_WILD;
  bp->rule = rule;

  bp->trID = TRID_NULL;
}


//=============================================================================================
/**
 * シングル ゲーム内トレーナー対戦
 *
 * @param   dst
 * @param   gameData
 * @param   partyEnemy
 * @param   landForm
 * @param   weather
 * @param   trID
 */
//=============================================================================================
void BTL_SETUP_Single_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  POKEPARTY* partyEnemy, BtlLandForm landForm, BtlWeather weather, TrainerID trID )
{
  setup_common( dst, gameData, landForm, weather );

  dst->engine = BTL_ENGINE_ALONE;
  dst->competitor = BTL_COMPETITOR_TRAINER;
  dst->rule = BTL_RULE_SINGLE;

  dst->partyEnemy1 = partyEnemy;
  dst->trID = trID;
}

//=============================================================================================
/**
 * シングル 通信対戦
 *
 * @param   dst
 * @param   gameData
 * @param   netHandle
 * @param   commMode
 */
//=============================================================================================
void BTL_SETUP_Single_Comm( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  GFL_NETHANDLE* netHandle, BtlCommMode commMode )
{
  setup_common( dst, gameData, BTL_LANDFORM_ROOM, BTL_WEATHER_NONE );
  
  dst->engine = BTL_ENGINE_ALONE;
  dst->competitor = BTL_COMPETITOR_COMM;
  dst->rule = BTL_RULE_SINGLE;

  dst->netHandle = netHandle;
  dst->commMode = commMode;
  dst->netID = GFL_NET_GetNetID( netHandle );
  dst->multiMode = 0;

  dst->trID = TRID_NULL;
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
 */
//=============================================================================================
void BTL_SETUP_Double_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  POKEPARTY* partyEnemy, BtlLandForm landForm, BtlWeather weather, TrainerID trID )
{
  setup_common( dst, gameData, landForm, weather );

  dst->engine = BTL_ENGINE_ALONE;
  dst->competitor = BTL_COMPETITOR_TRAINER;
  dst->rule = BTL_RULE_DOUBLE;

  dst->partyEnemy1 = partyEnemy;
  dst->trID = trID;
}

//=============================================================================================
/**
 * ダブル 通信対戦
 *
 * @param   dst
 * @param   gameData
 * @param   netHandle
 * @param   commMode
 */
//=============================================================================================
void BTL_SETUP_Double_Comm( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  GFL_NETHANDLE* netHandle, BtlCommMode commMode )
{
  setup_common( dst, gameData, BTL_LANDFORM_ROOM, BTL_WEATHER_NONE );
  
  dst->engine = BTL_ENGINE_ALONE;
  dst->competitor = BTL_COMPETITOR_COMM;
  dst->rule = BTL_RULE_DOUBLE;

  dst->netHandle = netHandle;
  dst->commMode = commMode;
  dst->netID = GFL_NET_GetNetID( netHandle );
  dst->multiMode = FALSE;

  dst->trID = TRID_NULL;
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
 */
//=============================================================================================
void BTL_SETUP_Multi_Comm( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  GFL_NETHANDLE* netHandle, BtlCommMode commMode, u8 commPos )
{
  setup_common( dst, gameData, BTL_LANDFORM_ROOM, BTL_WEATHER_NONE );
  
  dst->engine = BTL_ENGINE_ALONE;
  dst->competitor = BTL_COMPETITOR_COMM;
  dst->rule = BTL_RULE_DOUBLE;

  dst->netHandle = netHandle;
  dst->commMode = commMode;
  dst->netID = GFL_NET_GetNetID( netHandle );
  dst->multiMode = TRUE;

  dst->trID = TRID_NULL;
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
 */
//=============================================================================================
void BTL_SETUP_Triple_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  POKEPARTY* partyEnemy, BtlLandForm landForm, BtlWeather weather, TrainerID trID )
{
  setup_common( dst, gameData, landForm, weather );
  
  dst->engine = BTL_ENGINE_ALONE;
  dst->competitor = BTL_COMPETITOR_TRAINER;
  dst->rule = BTL_RULE_TRIPLE;

  dst->partyEnemy1 = partyEnemy;
  dst->trID = trID;
}

//=============================================================================================
/**
 * トリプル 通信対戦
 *
 * @param   dst
 * @param   gameData
 * @param   netHandle
 * @param   commMode
 */
//=============================================================================================
void BTL_SETUP_Triple_Comm( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  GFL_NETHANDLE* netHandle, BtlCommMode commMode )
{
  setup_common( dst, gameData, BTL_LANDFORM_ROOM, BTL_WEATHER_NONE );

  dst->engine = BTL_ENGINE_ALONE;
  dst->competitor = BTL_COMPETITOR_COMM;
  dst->rule = BTL_RULE_TRIPLE;

  dst->netHandle = netHandle;
  dst->commMode = commMode;
  dst->netID = GFL_NET_GetNetID( netHandle );
  dst->multiMode = 0;

  dst->trID = TRID_NULL;
}
