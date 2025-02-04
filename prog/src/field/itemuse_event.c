//============================================================================================
/**
 * @file    itemuse_proc.c
 * @brief   フィールドに関連したアイテムの使用処理
 * @author  k.ohno
 * @date    09.08.04
 */
//============================================================================================
#include "gflib.h"
#include "itemuse_event.h"
#include "field/fieldmap.h"
#include "event_mapchange.h"
#include "arc/fieldmap/zone_id.h"
#include "event_fishing.h"
#include "fskill_amaikaori.h"
#include "field/zonedata.h"
#include "app/itemuse.h"

#include "field_comm/intrude_work.h"  //Intrude_CheckNetIsExit

#define ITEMCHECK_NONE  (0xFF)

typedef struct _FLD_ITEM_FUNCTION{
  ItemUseEventFunc useFunc;
  u8  checkID;  //ITEMCHECK_ENABLE型、必要ない時はITEMCHECK_NONE
}FLD_ITEM_FUNCTION;

//////////////////////////////////////////////////
///プロトタイプ
//////////////////////////////////////////////////
static BOOL itemcheck_Cycle( GAMEDATA* gdata, FIELDMAP_WORK* field_wk, PLAYER_WORK* playerWork );
static BOOL itemcheck_Ananukenohimo( GAMEDATA* gdata, FIELDMAP_WORK* field_wk, PLAYER_WORK* playerWork );
static BOOL itemcheck_Amaimitu( GAMEDATA* gdata, FIELDMAP_WORK* field_wk, PLAYER_WORK* playerWork );
static BOOL itemcheck_Turizao( GAMEDATA* gdata, FIELDMAP_WORK* field_wk, PLAYER_WORK* playerWork );
static BOOL itemcheck_DowsingMachine( GAMEDATA* gdata, FIELDMAP_WORK* field_wk, PLAYER_WORK* playerWork );

//=============================================================================
/**
 *  共通
 */
//=============================================================================
//-------------------------------------
/// フィールドアイテム使用共通呼び出し用テーブル
//=====================================
static FLD_ITEM_FUNCTION const DATA_FldItemFunction[ EVENT_ITEMUSE_CALL_MAX ] =
{ 
  { //自転車
    &EVENT_CycleUse,
    ITEMCHECK_CYCLE,
  },
  { //パレスへGo
    &EVENT_PalaceJumpUse,
    ITEMCHECK_NONE,
  },
  { //穴抜けの紐
    &EVENT_ChangeMapByAnanukenohimo,
    ITEMCHECK_ANANUKENOHIMO,
  },
  { //甘い蜜
    &EVENT_FieldAmaimitu,
    ITEMCHECK_NONE,
  },
  { //釣竿
    &EVENT_FieldFishing,
    ITEMCHECK_TURIZAO,
  },
  { //ダウジングマシン
    &EVENT_DowsingMachineUse,
    ITEMCHECK_DOWSINGMACHINE,
  },
};

//-------------------------------------
/*
 * フィールドアイテム使用チェック関数テーブル
 *
 * このテーブル登録とは別に、パレスでは「大切なもの」は使えないチェックが
 * バッグ画面で行われている
 */
//-------------------------------------
static ItemUseCheckFunc const DATA_ItemUseCheckFunc[ITEMCHECK_MAX] = {
  itemcheck_Cycle, // 自転車(乗っていない時はTRUEで乗れる、乗っている時はTRUEで降りれる)
  NULL, // タウンマップ
  NULL, // ともだち手帳
  itemcheck_Ananukenohimo, // あなぬけのヒモ
  itemcheck_Amaimitu, // あまいミツ
  itemcheck_Turizao, // つりざお
  NULL, // バトルレコーダー
  NULL, // メール
  NULL, // スプレー
  itemcheck_DowsingMachine, // ダウジングマシン
};


//----------------------------------------------------------------------------
/**
 *  @brief  バッグ画面に引き渡すアイテム使用チェック
 *
 *  @param  GMEVENT *event  イベント
 *  @param  *seq            シーケンス
 *  @param  *wk_adrs        ワーク
 *
 *  @return 終了コード
 */
//-----------------------------------------------------------------------------
void ITEMUSE_InitCheckWork( ITEMCHECK_WORK* icwk, GAMESYS_WORK* gsys, FIELDMAP_WORK* field_wk )
{
  int i;
  GAMEDATA* gdata = GAMESYSTEM_GetGameData( gsys );
  PLAYER_WORK *playerWork = GAMEDATA_GetMyPlayerWork( gdata );

  MI_CpuClear8( icwk, sizeof(ITEMCHECK_WORK) );

  for( i = 0;i < ITEMCHECK_MAX; i++){
    ITEMUSE_SetItemUseCheck( icwk, i, ITEMUSE_UseCheckCall( i, gsys, field_wk ));
  }
  //自機フォルムチェック
  icwk->PlayerForm = PLAYERWORK_GetMoveForm( playerWork ); 
  // ( PlayerForm == PLAYER_MOVE_FORM_CYCLE );
}

//----------------------------------------------------------------------------
/**
 *  @brief  共通アイテム使用チェック関数呼び出し
 *
 *  @param  EVENT_ITEMUSE_CALL_TYPE type  呼び出すチェックタイプ
 *  @param  *gsys       ゲームシステム
 *  @param  *field_wk   フィールド
 *
 *  @return イベント
 */
//-----------------------------------------------------------------------------
BOOL ITEMUSE_UseCheckCall( ITEMCHECK_ENABLE type, GAMESYS_WORK *gsys, FIELDMAP_WORK *field_wk )
{ 
  GAMEDATA* gdata = GAMESYSTEM_GetGameData( gsys );
  PLAYER_WORK *playerWork = GAMEDATA_GetMyPlayerWork( gdata );

  GF_ASSERT( type < ITEMCHECK_MAX );
  
  if( DATA_ItemUseCheckFunc[type] == NULL ){
    return TRUE;  //無条件で使用していい
  }
  return (DATA_ItemUseCheckFunc[ type ])( gdata, field_wk, playerWork );
}

//----------------------------------------------------------------------------
/**
 *  @brief  共通アイテム使用イベント呼び出し
 *
 *  @param  EVENT_ITEMUSE_CALL_TYPE type  呼び出すアイテム
 *  @param  *gsys       ゲームシステム
 *  @param  *field_wk   フィールド
 *
 *  @return イベント
 */
//-----------------------------------------------------------------------------
GMEVENT * EVENT_FieldItemUse( EVENT_ITEMUSE_CALL_TYPE type, GAMESYS_WORK *gsys, FIELDMAP_WORK *field_wk )
{ 
  GF_ASSERT( type < EVENT_ITEMUSE_CALL_MAX );
  OS_Printf("event type = %d\n", type);
  return (DATA_FldItemFunction[ type ].useFunc)( field_wk, gsys );
}

//----------------------------------------------------------------------------
/**
 *  @brief  共通アイテム使用チェック呼び出し
 *
 *  @param  EVENT_ITEMUSE_CALL_TYPE type  呼び出すアイテム
 *  @param  *gsys       ゲームシステム
 *  @param  *field_wk   フィールド
 *
 *  @retval TRUE：使用可
 *  @retval FALSE：使用不可
 */
//-----------------------------------------------------------------------------
BOOL FieldItemUseCheck( EVENT_ITEMUSE_CALL_TYPE type, GAMESYS_WORK *gsys, FIELDMAP_WORK *field_wk )
{ 
  GF_ASSERT( type < EVENT_ITEMUSE_CALL_MAX );

  if( DATA_FldItemFunction[ type ].checkID == ITEMCHECK_NONE ){
    return TRUE;  //使用制限無し
  }
  return ITEMUSE_UseCheckCall( DATA_FldItemFunction[type].checkID, gsys, field_wk );
}

//-----------------------------------------------------------------------------
//=============================================================================
///個別使用チェック関数
//=============================================================================
//-----------------------------------------------------------------------------

/*
 *  @brief  自転車使用チェック
 */
static BOOL itemcheck_Cycle( GAMEDATA* gdata, FIELDMAP_WORK* field_wk, PLAYER_WORK* playerWork )
{
//  PLAYER_WORK *playerWork = GAMEDATA_GetMyPlayerWork( gdata );
  u16 zone_id = PLAYERWORK_getZoneID( playerWork );
  u8 form = PLAYERWORK_GetMoveForm( playerWork ); 
  MAPATTR attr = FIELD_PLAYER_GetMapAttr( FIELDMAP_GetFieldPlayer( field_wk ) );
  BOOL not_use_f = MAPATTR_VALUE_CheckCycleNotUse( MAPATTR_GetAttrValue(attr) );

  if( form == PLAYER_MOVE_FORM_CYCLE ){
    if( not_use_f ){
      return FALSE;
    }
    return TRUE;  //降りれる
  }
  //ゾーンチェック
  if( ZONEDATA_BicycleEnable( zone_id ) == FALSE ){
    return FALSE;
  }

  //フォルムチェック
  if( form != PLAYER_MOVE_FORM_NORMAL ){
    return FALSE;
  }
  //アトリビュートによるチェック
  if( not_use_f ){
    return FALSE;
  }
  return TRUE;  //乗れる
}

/*
 *  @brief  穴抜けの紐チェック
 */
static BOOL itemcheck_Ananukenohimo( GAMEDATA* gdata, FIELDMAP_WORK* field_wk, PLAYER_WORK* playerWork )
{
//  PLAYER_WORK *playerWork = GAMEDATA_GetMyPlayerWork( gdata );
  u16 zone_id = PLAYERWORK_getZoneID( playerWork );
  
  //フィールド技が使えない場所では使えない(ユニオンルーム、通信対戦部屋、パレス)
  if( ZONEDATA_CheckFieldSkillUse(zone_id) == FALSE){
    return FALSE;
  }

  if(!ZONEDATA_EscapeEnable( zone_id )){
    return FALSE;
  }
  return TRUE;
}

/*
 *  @brief  あまいみつチェック
 */
static BOOL itemcheck_Amaimitu( GAMEDATA* gdata, FIELDMAP_WORK* field_wk, PLAYER_WORK* playerWork )
{
  u16 zone_id = PLAYERWORK_getZoneID( playerWork );

  //フィールド技が使えない場所では使えない(ユニオンルーム、通信対戦部屋、パレス)
  return ( ZONEDATA_CheckFieldSkillUse(zone_id));
}

/*
 *  @brief  釣竿使用チェック
 */
static BOOL itemcheck_Turizao( GAMEDATA* gdata, FIELDMAP_WORK* field_wk, PLAYER_WORK* playerWork )
{
  u16 zone_id = PLAYERWORK_getZoneID( playerWork );

  //フィールド技が使えない場所では使えない(ユニオンルーム、通信対戦部屋、パレス)
  if( ZONEDATA_CheckFieldSkillUse(zone_id) == FALSE){
    return FALSE;
  }
  //釣竿チェック
  return FieldFishingCheckPos( gdata, field_wk, NULL );
}

/*
 *  @brief  ダウジングマシン使用チェック
 */
static BOOL itemcheck_DowsingMachine( GAMEDATA* gdata, FIELDMAP_WORK* field_wk, PLAYER_WORK* playerWork )
{
  u16 zone_id = PLAYERWORK_getZoneID( playerWork );

  //遊覧船の中ではつかえない
  if ( ZONEDATA_IsPlBoat(zone_id) ) {
    return FALSE;
  }
  //フィールド技が使えない場所では使えない(ユニオンルーム、通信対戦部屋、パレス)
  return ( ZONEDATA_CheckFieldSkillUse(zone_id));
}

//=============================================================================
/**
 *  自転車呼び出し
 */
//=============================================================================
typedef struct{
  GAMESYS_WORK *gameSys;
  u32 watchDogTimer;      ///<監視用タイマー
} CYCLEUSE_STRUCT;

//------------------------------------------------------------------------------
/**
 * @brief   自転車を使う
 * @retval  none
 */
//------------------------------------------------------------------------------
static GMEVENT_RESULT CycleEvent(GMEVENT * event, int * seq, void *work)
{
  CYCLEUSE_STRUCT* pCy = work;
  FIELDMAP_WORK *fieldmap = GAMESYSTEM_GetFieldMapWork( pCy->gameSys );
  MMDLSYS *mmdlsys = FIELDMAP_GetMMdlSys( fieldmap );
  
  switch( (*seq) ){
  case 0:
    { //通信切断中の場合、終了するか指定フレーム経過しないと先にすすめない
      GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr( pCy->gameSys );
      pCy->watchDogTimer ++;
      if ( Intrude_CheckNetIsExit( game_comm ) == TRUE && pCy->watchDogTimer < 30 )
      {
        OS_Printf("Waiting Intrude Exit...\n");
        break;
      }
    }
    FIELDMAP_SetPlayerItemCycle( fieldmap );
    (*seq)++;
  case 1:
    {
      FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldmap );
      
      if( FIELD_PLAYER_UpdateRequest(fld_player) == TRUE ){
        (*seq)++;
      }
    }
    break;
  case 2:
    {
      FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldmap );
      
      if( FIELD_PLAYER_CheckDrawFormWait(fld_player) == TRUE ){
        return( GMEVENT_RES_FINISH );
      }
    }
  }
  
  return GMEVENT_RES_CONTINUE;
}

#if 0
static GMEVENT_RESULT CycleEvent(GMEVENT * event, int * seq, void *work)
{
  CYCLEUSE_STRUCT* pCy = work;
  FIELDMAP_WORK *fieldmap = GAMESYSTEM_GetFieldMapWork( pCy->gameSys );
  
  switch( (*seq) ){
  case 0:
    FIELDMAP_SetPlayerItemCycle( fieldmap );
    (*seq)++;
    break;
  case 1:
    {
      FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldmap );
      
      if( FIELD_PLAYER_CheckDrawFormWait(fld_player) == TRUE ){
        return( GMEVENT_RES_FINISH );
      }
    }
  }
  
  return GMEVENT_RES_CONTINUE;
}
#endif

//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT * EVENT_CycleUse(FIELDMAP_WORK *fieldWork,GAMESYS_WORK *gsys)
{
  GMEVENT * event = GMEVENT_Create(gsys, NULL, CycleEvent, sizeof(CYCLEUSE_STRUCT));
  CYCLEUSE_STRUCT * pCy = GMEVENT_GetEventWork(event);
  pCy->gameSys = gsys;
  pCy->watchDogTimer = 0;
  return event;
}


//------------------------------------------------------------------------------
/**
 * @brief   パレスにジャンプする
 * @retval  none
 */
//------------------------------------------------------------------------------


GMEVENT * EVENT_PalaceJumpUse(FIELDMAP_WORK *fieldWork,GAMESYS_WORK *gsys)
{
  //2010.06.28(月) 未使用ソース。ここに来る事はないがテーブルを削るのは時期的に危険なため、
  //                             通常のパレスジャンプをさせています。
  VecFx32 pos;
  pos.x = 512 << FX32_SHIFT;
  pos.y = 32;
  pos.z = 488 << FX32_SHIFT;
  return EVENT_ChangeMapFldToPalace( gsys, ZONE_ID_PALACE01, &pos, FALSE );
}

//=============================================================================
/**
 *  ダウジングマシン呼び出し
 */
//=============================================================================
typedef struct{
  GAMESYS_WORK*      gameSys;
  FIELDMAP_WORK*     fieldWork;
} DOWSINGMACHINEUSE_STRUCT;
//------------------------------------------------------------------------------
/**
 * @brief   ダウジングマシンを使う
 * @retval  
 */
//------------------------------------------------------------------------------
static GMEVENT_RESULT DowsingMachineEvent(GMEVENT* event, int* seq, void* work)
{
  DOWSINGMACHINEUSE_STRUCT*   pDMU        = work;
  FIELD_SUBSCREEN_WORK*       subscreen   = FIELDMAP_GetFieldSubscreenWork(pDMU->fieldWork);

  if( FIELD_SUBSCREEN_GetMode( subscreen ) == FIELD_SUBSCREEN_DOWSING )  // ダウジングマシンを使っているときは、ダウジングマシンを終了させる
  {
    FIELD_SUBSCREEN_Change( subscreen, FIELD_SUBSCREEN_NORMAL );
  }
  else
  {
    FIELD_SUBSCREEN_Change( subscreen, FIELD_SUBSCREEN_DOWSING );
  }

  return GMEVENT_RES_FINISH;
}
GMEVENT* EVENT_DowsingMachineUse(FIELDMAP_WORK* fieldWork, GAMESYS_WORK* gsys)
{
  GMEVENT*                    event  = GMEVENT_Create(gsys, NULL, DowsingMachineEvent, sizeof(DOWSINGMACHINEUSE_STRUCT));
  DOWSINGMACHINEUSE_STRUCT*   pDMU   = GMEVENT_GetEventWork(event);
  pDMU->gameSys     = gsys;
  pDMU->fieldWork   = fieldWork;
  return event;
}

