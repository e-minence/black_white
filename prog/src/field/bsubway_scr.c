//======================================================================
/**
 * @file bsubway_scr.c
 * @brief バトルサブウェイ　スクリプト関連
 * @authaor kagaya
 * @date 2008.12.11
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "savedata/save_tbl.h"
#include "savedata/record.h"

#include "system/main.h"  //HEAPID_PROC

#include "poke_tool/poke_tool.h"
#include "app/pokelist.h"

#include "gamesystem\btl_setup.h"

#include "savedata/bsubway_savedata.h"
#include "savedata/bsubway_savedata_def.h"
#include "battle/bsubway_battle_data.h"
#include "bsubway_scr_def.h"
#include "bsubway_scr.h"
#include "bsubway_scrwork.h"
#include "bsubway_tr.h"

#include "savedata/battle_box_save.h"
#include "fld_btl_inst_tool.h"

//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================

//======================================================================
//  proto
//======================================================================
static void bsw_RecoverNowRenshou(
    BSUBWAY_SCRWORK *bsw_scr, u16 stage, u16 round );

static BOOL is_ConflictTrainer( u16* trainer,u16 id,u16 num );

static void bsw_SetCommonScore(
    BSUBWAY_SCRWORK *wk, SAVE_CONTROL_WORK *sv, u8 win_f, u16 now_renshou );
static void bsw_SaveMemberPokeData(
    BSUBWAY_SCRWORK *wk, SAVE_CONTROL_WORK *sv, BSWAY_SCORE_POKE_DATA mode );

//======================================================================
//  バトルサブウェイ　スクリプトワーク関連
//======================================================================
//--------------------------------------------------------------
/**
 * BSUBWAY_SCRWORK ワークポインタをクリア
 * @param bsw_scr BSUBWAY_SCRWORK*
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_ClearWork( GAMESYS_WORK *gsys )
{
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  GAMEDATA_SetBSubwayScrWork( gdata, NULL );
}

//--------------------------------------------------------------
/**
 * BSUBWAY_SCRWORK ワークエリアを取得して初期化する
 * @param gsys GAMESYS_WORK*
 * @param init 初期化モード BSWAY_PLAY_NEW:始め、BSWAY_PLAY_CONTINUE:続き
 * @param playmode プレイモード指定:BSWAY_MODE_〜*
 * @retval BSUBWAY_SCRWORK*
 */
//--------------------------------------------------------------
BSUBWAY_SCRWORK * BSUBWAY_SCRWORK_CreateWork(
    GAMESYS_WORK *gsys, u16 init, u16 playmode )
{
  int i;
  u8 buf8;
  BSUBWAY_SCRWORK *bsw_scr;
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  MYSTATUS *mystatus = GAMEDATA_GetMyStatus( gdata );
  SAVE_CONTROL_WORK *save = GAMEDATA_GetSaveControlWork( gdata );
  
  KAGAYA_Printf( "BSW CREATE HEAP : size %x(%d)\n",
      sizeof(BSUBWAY_SCRWORK), sizeof(BSUBWAY_SCRWORK) );

  bsw_scr = GFL_HEAP_AllocClearMemory(
      HEAPID_PROC, sizeof(BSUBWAY_SCRWORK) );
  
  bsw_scr->heapID = HEAPID_PROC;
  bsw_scr->magicNo = BSUBWAY_SCRWORK_MAGIC;
  
  bsw_scr->gdata = gdata;
  bsw_scr->my_sex = MyStatus_GetMySex( mystatus );
  
  bsw_scr->playData = SaveControl_DataPtrGet(
      save, GMDATA_ID_BSUBWAY_PLAYDATA );
  
  bsw_scr->scoreData = SaveControl_DataPtrGet(
      save, GMDATA_ID_BSUBWAY_SCOREDATA );

  bsw_scr->wifiData = SaveControl_DataPtrGet(
      save, GMDATA_ID_BSUBWAY_WIFIDATA );
  
  BSUBWAY_PLAYDATA_SetSaveFlag( bsw_scr->playData, FALSE ); //セーブなしに
  
  GF_ASSERT( GAMEDATA_GetBSubwayScrWork(gdata) == NULL );
  GF_ASSERT( bsw_scr->playData != NULL );
  GF_ASSERT( bsw_scr->scoreData != NULL );
  
  GAMEDATA_SetBSubwayScrWork( gdata, bsw_scr );
  
  if( init == BSWAY_PLAY_NEW ){ //新規
    bsw_scr->play_mode = playmode;
    bsw_scr->member_num =
      BSUBWAY_SCRWORK_GetPlayModeMemberNum( bsw_scr->play_mode );
    
    for(i = 0;i < BSUBWAY_STOCK_MEMBER_MAX;i++){
      bsw_scr->member[i] = BSWAY_NULL_POKE;
    }
    
    for(i = 0;i < BSUBWAY_STOCK_TRAINER_MAX;i++){
      bsw_scr->trainer[i] = BSWAY_NULL_TRAINER;
    }
    
    BSUBWAY_PLAYDATA_Init( bsw_scr->playData ); //プレイデータ初期化
    BSUBWAY_PLAYDATA_ResetRoundNo( bsw_scr->playData );
    
    if( BSUBWAY_SCOREDATA_CheckExistStageNo( //ステージ存在する
        bsw_scr->scoreData,bsw_scr->play_mode) == TRUE ){
      //新規プレイでステージが継続している場合は
      //セーブされている連勝数を反映
      u16 play_mode = bsw_scr->play_mode;
      u16 renshou = BSUBWAY_SCOREDATA_GetRenshou(
          bsw_scr->scoreData, play_mode );
      BSUBWAY_SCRWORK_SetNowRenshou( bsw_scr, renshou );
    }
    
    buf8 = bsw_scr->play_mode; //プレイモード
    
    BSUBWAY_PLAYDATA_SetData(
        bsw_scr->playData, BSWAY_PLAYDATA_ID_playmode, &buf8 );
  }else{ //BSWAY_PLAY_CONTINUE 続き
    bsw_scr->play_mode = (u8)BSUBWAY_PLAYDATA_GetData(
        bsw_scr->playData, BSWAY_PLAYDATA_ID_playmode, NULL );
    
    bsw_scr->member_num =
      BSUBWAY_SCRWORK_GetPlayModeMemberNum( bsw_scr->play_mode );
    
    //バトルボックス使用であれば準備
    if( (u32)BSUBWAY_PLAYDATA_GetData( bsw_scr->playData,
          BSWAY_PLAYDATA_ID_use_battle_box,NULL) != FALSE ){
      BSUBWAY_SCRWORK_PreparBattleBox( bsw_scr );
    }
    
    //選ばれているポケモンNo
    BSUBWAY_PLAYDATA_GetData( bsw_scr->playData,
        BSWAY_PLAYDATA_ID_pokeno, bsw_scr->member );
    
    //抽選済みのトレーナーNo
    BSUBWAY_PLAYDATA_GetData( bsw_scr->playData,
        BSWAY_PLAYDATA_ID_trainer, bsw_scr->trainer );
    
    //AIマルチならパートナー復帰
    if( bsw_scr->play_mode == BSWAY_MODE_MULTI ||
        bsw_scr->play_mode == BSWAY_MODE_S_MULTI ){
      bsw_scr->partner = (u8)BSUBWAY_PLAYDATA_GetData(
          bsw_scr->playData, BSWAY_PLAYDATA_ID_partner, NULL );
      
      //パートナートレーナーデータ再生成
      BSUBWAY_PLAYDATA_GetData( bsw_scr->playData,
          BSWAY_PLAYDATA_ID_pare_poke,
          &(bsw_scr->five_poke[bsw_scr->partner]) );
      
      {
        int i;
        int start_no = BSWAY_PARTNER_DATA_START_NO_WOMAN;
        PLAYER_WORK *player = GAMEDATA_GetMyPlayerWork( gdata );
        u32 sex = MyStatus_GetMySex( &player->mystatus );
        
        if( sex ){ //woman
          start_no = BSWAY_PARTNER_DATA_START_NO_MAN;
        }
        
        BSUBWAY_SCRWORK_MakePartnerRomData(
          bsw_scr, &bsw_scr->five_data[bsw_scr->partner],
          start_no + bsw_scr->partner,
          BSUBWAY_PLAYDATA_GetData(bsw_scr->playData,
            BSWAY_PLAYDATA_ID_pare_itemfix, NULL ),
          &(bsw_scr->five_poke[bsw_scr->partner]),
          bsw_scr->heapID );
      }
    }
    
    //続きからの場合は
    //セーブしたラウンド、ステージから連勝数復元
    {
      u16 play_mode = bsw_scr->play_mode;
      
      if( BSUBWAY_SCOREDATA_CheckExistStageNo(
            bsw_scr->scoreData,play_mode) == TRUE )
      {
        u16 round = BSUBWAY_PLAYDATA_GetRoundNo( bsw_scr->playData );
        u16 stage = BSUBWAY_SCOREDATA_GetStageNo_Org0(
            bsw_scr->scoreData, play_mode );
        bsw_RecoverNowRenshou( bsw_scr, stage, round );
      }
    }
  }
  
  return( bsw_scr );
}

//--------------------------------------------------------------
/**
 * BSUBWAY_SCRWORK ワークエリアを開放する
 * @param bsw_scr BSUBWAY_SCRWORK*
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_ReleaseWork(
    GAMESYS_WORK *gsys, BSUBWAY_SCRWORK *bsw_scr )
{
  if( bsw_scr != NULL ){
    GF_ASSERT( bsw_scr->magicNo == BSUBWAY_SCRWORK_MAGIC );
    
    if( bsw_scr->btl_box_party != NULL ){
      GFL_HEAP_FreeMemory( bsw_scr->btl_box_party );
      bsw_scr->btl_box_party = NULL;
    }
    
    if( bsw_scr->btl_setup_param != NULL ){
      BATTLE_PARAM_Delete( bsw_scr->btl_setup_param );
      bsw_scr->btl_setup_param = NULL;
    }
    
    MI_CpuClear8( bsw_scr, sizeof(BSUBWAY_SCRWORK) );
    GFL_HEAP_FreeMemory( bsw_scr );
  }
  
  {
    GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
    GAMEDATA_SetBSubwayScrWork( gdata, NULL );
  }
}

//--------------------------------------------------------------
/**
 * BSUBWAY_SCRWORK　通信マルチモードへ変更する
 * @param bsw_scr BSUBWAY_SCRWORK
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_ChangeCommMultiMode( BSUBWAY_SCRWORK *bsw_scr )
{
  u8 buf[4];
  
  if( bsw_scr->play_mode == BSWAY_MODE_MULTI ){
    buf[0] = BSWAY_MODE_COMM_MULTI;
  }else if( bsw_scr->play_mode == BSWAY_MODE_S_MULTI ){
    buf[0] = BSWAY_MODE_S_COMM_MULTI;
  }else{
    GF_ASSERT( 0 );
  }
  
  bsw_scr->play_mode = buf[0];
  
  BSUBWAY_PLAYDATA_SetData( bsw_scr->playData,
      BSWAY_PLAYDATA_ID_playmode, buf );
  
  //ステージが有る場合はセーブされている連勝数を反映
  //(MULTI->COMM_MULTIへの変換は新規プレイのみ)
  if( BSUBWAY_SCOREDATA_CheckExistStageNo(
        bsw_scr->scoreData,bsw_scr->play_mode) == TRUE )
  {
    u16 play_mode = bsw_scr->play_mode;
    u16 renshou = BSUBWAY_SCOREDATA_GetRenshou(
        bsw_scr->scoreData, play_mode );
    BSUBWAY_SCRWORK_SetNowRenshou( bsw_scr, renshou );
  }
  else  //存在しない場合は連勝数をリセット(連勝数送信、トレーナー抽選の為)
  {
    BSUBWAY_SCRWORK_ResetNowRenshou( bsw_scr );
  }
}

#if 0 //未使用となった。
//--------------------------------------------------------------
/**
 * BSUBWAY_SCRWORK　通信マルチモードからAIマルチモードへ変更。
 * @param bsw_scr BSUBWAY_SCRWORK
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_ChangeMultiMode( BSUBWAY_SCRWORK *bsw_scr )
{
  u8 buf[4];
  
  if( bsw_scr->play_mode == BSWAY_MODE_COMM_MULTI ){
    buf[0] = BSWAY_MODE_MULTI;
  }else if( bsw_scr->play_mode == BSWAY_MODE_S_COMM_MULTI ){
    buf[0] = BSWAY_MODE_S_MULTI;
  }
  
  bsw_scr->play_mode = buf[0];
  
  BSUBWAY_PLAYDATA_SetData( bsw_scr->playData,
      BSWAY_PLAYDATA_ID_playmode, buf );
  
  //ステージが無い場合はステージ数、連勝数初期化
  if( BSUBWAY_SCOREDATA_CheckExistStageNo(
        bsw_scr->scoreData,bsw_scr->play_mode) == FALSE ){
    BSUBWAY_SCOREDATA_InitStageNo( bsw_scr->scoreData, bsw_scr->play_mode );
    BSUBWAY_SCOREDATA_ResetRenshou( //連勝数リセット
          bsw_scr->scoreData, bsw_scr->play_mode );
  }
}
#endif

//--------------------------------------------------------------
/**
 * BSUBWAY_SCRWORK 休むときに現在のプレイ状況をセーブに書き出す
 * @param bsw_scr BSUBWAY_SCRWORK*
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_SaveRestPlayData( BSUBWAY_SCRWORK *bsw_scr )
{
  u16 i,renshou;
  u8 buf8[4];
  
  //プレイモード書き出し
  buf8[0] = bsw_scr->play_mode;
  BSUBWAY_PLAYDATA_SetData( bsw_scr->playData,
      BSWAY_PLAYDATA_ID_playmode, buf8 );
  
  //選んだポケモンNo
  BSUBWAY_PLAYDATA_SetData( bsw_scr->playData,
      BSWAY_PLAYDATA_ID_pokeno, bsw_scr->member );
  
#if 0  
  //バトル成績書き出し
  BSUBWAY_PLAYDATA_AddWifiRecord( bsw_scr->playData,
    bsw_scr->rec_down, bsw_scr->rec_turn, bsw_scr->rec_damage );
#endif
  
  //抽選されたトレーナーNo書き出し
  BSUBWAY_PLAYDATA_SetData( bsw_scr->playData,
      BSWAY_PLAYDATA_ID_trainer, bsw_scr->trainer );
  
  //現在の連勝数を反映 -> しません
  //連勝数の更新は
  //・負けた時
  //・７連勝毎のホーム
  //以外は行わない
#if 0
  renshou = BSUBWAY_SCRWORK_GetNowRenshou( bsw_scr );
  BSUBWAY_SCOREDATA_SetRenshou(
      bsw_scr->scoreData, bsw_scr->play_mode, renshou );
#endif
  
  //セーブフラグを有効状態にセット
  BSUBWAY_PLAYDATA_SetSaveFlag( bsw_scr->playData, TRUE );
  
  if( bsw_scr->play_mode == BSWAY_MODE_MULTI ||
      bsw_scr->play_mode == BSWAY_MODE_S_MULTI ){
    //AIマルチモードならパートナーを覚えておく
    buf8[0] = bsw_scr->partner;
    BSUBWAY_PLAYDATA_SetData(
        bsw_scr->playData, BSWAY_PLAYDATA_ID_partner, buf8 );
    
    //パートナーのポケモンパラメータを憶えておく
    BSUBWAY_PLAYDATA_SetData( bsw_scr->playData,
        BSWAY_PLAYDATA_ID_pare_poke,
        &(bsw_scr->five_poke[bsw_scr->partner]) );
    
    //アイテムが固定だったかどうか憶えておく
    BSUBWAY_PLAYDATA_SetData( bsw_scr->playData,
        BSWAY_PLAYDATA_ID_pare_itemfix,
        &(bsw_scr->five_item[bsw_scr->partner]) );
  }
}

//--------------------------------------------------------------
/**
 * BSUBWAY_SCRWORK ポケモンメンバーをロード
 * @param bsw_scr BSUBWAY_SCRWORK*
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_LoadPokemonMember(
    BSUBWAY_SCRWORK *bsw_scr, GAMESYS_WORK *gsys )
{
  int i;
  const POKEPARTY *party;
  POKEMON_PARAM *pp;
  
  bsw_scr->member_num =
      BSUBWAY_SCRWORK_GetPlayModeMemberNum( bsw_scr->play_mode );
  
  BSUBWAY_PLAYDATA_GetData( bsw_scr->playData, //選んだポケモンNo
      BSWAY_PLAYDATA_ID_pokeno, bsw_scr->member );
  
  party = BSUBWAY_SCRWORK_GetPokePartyUse( bsw_scr );
  
  for( i = 0; i < bsw_scr->member_num; i++ ){ //選択ポケモン手持ちNo
#ifdef DEBUG_BSW_PRINT
    KAGAYA_Printf( "BSW LOAD POKEMON MEMBER [%d] = %d\n",
        i, bsw_scr->member[i] );
#endif
    pp = PokeParty_GetMemberPointer(
        (POKEPARTY *)party, bsw_scr->member[i] );
    bsw_scr->mem_poke[i] = PP_Get( pp, ID_PARA_monsno, NULL );  
    bsw_scr->mem_item[i] = PP_Get( pp, ID_PARA_item, NULL );  
  }
}

//--------------------------------------------------------------
/**
 * BSUBWAY_SCRWORK ゲームクリア時に現在のプレイ状況をセーブに書き出す
 * @param bsw_scr BSUBWAY_SCRWORK*
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_SaveGameClearPlayData( BSUBWAY_SCRWORK *bsw_scr )
{
  u16  i;
  u8  buf8[4];
  
  //プレイモード書き出し
  buf8[0] = bsw_scr->play_mode;
  BSUBWAY_PLAYDATA_SetData( bsw_scr->playData,
      BSWAY_PLAYDATA_ID_playmode, buf8 );
  
  //ラウンド数リセット
  BSUBWAY_PLAYDATA_ResetRoundNo( bsw_scr->playData );
  
  //選んだポケモンNo
  BSUBWAY_PLAYDATA_SetData( bsw_scr->playData,
      BSWAY_PLAYDATA_ID_pokeno, bsw_scr->member );
  
#if 0  
  //バトル成績書き出し
  BSUBWAY_PLAYDATA_AddWifiRecord( bsw_scr->playData,
    bsw_scr->rec_down, bsw_scr->rec_turn, bsw_scr->rec_damage );
#endif
  
  //セーブフラグを有効状態にリセット
  BSUBWAY_PLAYDATA_SetSaveFlag( bsw_scr->playData, TRUE );
  
  if( bsw_scr->play_mode == BSWAY_MODE_MULTI ||
      bsw_scr->play_mode == BSWAY_MODE_S_MULTI ){
    //AIマルチモードならパートナーを覚えておく
    buf8[0] = bsw_scr->partner;
    BSUBWAY_PLAYDATA_SetData(
        bsw_scr->playData, BSWAY_PLAYDATA_ID_partner, buf8 );
  
    //パートナーのポケモンパラメータを憶えておく
    BSUBWAY_PLAYDATA_SetData( bsw_scr->playData,
        BSWAY_PLAYDATA_ID_pare_poke,
        &(bsw_scr->five_poke[bsw_scr->partner]) );
    
    //アイテムが固定だったかどうか憶えておく
    BSUBWAY_PLAYDATA_SetData( bsw_scr->playData,
        BSWAY_PLAYDATA_ID_pare_itemfix,
        &(bsw_scr->five_item[bsw_scr->partner]) );
  }
}

//--------------------------------------------------------------
/**
 * エラー時のワークセット
 * @param GAMESYS_WORK *gsys
 * @retval u16 プレイしていたモード
 */
//--------------------------------------------------------------
u16 BSUBWAY_SCRWORK_SetNGScore( GAMESYS_WORK *gsys )
{
  int id;
  u8  play_mode;
  BSUBWAY_PLAYDATA *playData;
  BSUBWAY_SCOREDATA *scoreData;
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  SAVE_CONTROL_WORK *save = GAMEDATA_GetSaveControlWork( gdata );
  
  //セーブデータ取得
  playData = SaveControl_DataPtrGet( save, GMDATA_ID_BSUBWAY_PLAYDATA );
  scoreData = SaveControl_DataPtrGet( save, GMDATA_ID_BSUBWAY_SCOREDATA );
  
  //どのモードをプレイしていたか？
  play_mode = BSUBWAY_PLAYDATA_GetData(
      playData, BSWAY_PLAYDATA_ID_playmode, NULL );

  //現在の周回数リセット
  BSUBWAY_PLAYDATA_ResetRoundNo( playData );
  
  //ステージ数をエラーに
  BSUBWAY_SCOREDATA_ErrorStageNo( scoreData, play_mode );
  
  //連勝記録クリア -> しません
#if 0  
  BSUBWAY_SCOREDATA_ResetRenshou( scoreData, play_mode );
#endif

  //レコード挑戦中フラグを落とす
  #if 0 //wb null
  BSUBWAY_SCOREDATA_SetFlag( scoreData,
    BSWAY_SCOREDATA_FLAG_SINGLE_RECORD + play_mode,
    BSWAY_SETMODE_reset );
  #endif
  
  return play_mode;  
}

//--------------------------------------------------------------
/**
 * 敗戦時のワークセット
 * @param GAMESYS_WORK *gsys
 * @param bsw_scr BSUBWAY_SCRWORK
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_SetLoseScore(
    GAMESYS_WORK *gsys, BSUBWAY_SCRWORK *bsw_scr )
{
  u16 play_mode = bsw_scr->play_mode;
  u16 renshou = bsw_scr->now_renshou;
  BSUBWAY_SCOREDATA *scoreData = bsw_scr->scoreData;
  
  //現在の連勝数をセーブ
  BSUBWAY_SCOREDATA_SetRenshou( scoreData, play_mode, renshou );
  
  //連勝記録更新
  BSUBWAY_SCOREDATA_UpdateRenshouMax( scoreData, play_mode, renshou );
  
  { //勝ち負け共通データ作成
    SAVE_CONTROL_WORK *save = GAMEDATA_GetSaveControlWork( bsw_scr->gdata );
    bsw_SetCommonScore( bsw_scr, save, TRUE, renshou );
  }
  
  //ステージ数をエラーに
  BSUBWAY_SCOREDATA_ErrorStageNo( bsw_scr->scoreData, play_mode );

  //周回数リセット
  BSUBWAY_PLAYDATA_ResetRoundNo( bsw_scr->playData );
}

#if 0 //wb null
void BSUBWAY_SCRWORK_SetLoseScore(
    GAMESYS_WORK *gsys, BSUBWAY_SCRWORK *bsw_scr )
{
  u32  ret = 0;
  int  id;
  u16  before,after,chg_flg;
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  SAVE_CONTROL_WORK *save = GAMEDATA_GetSaveControlWork( gdata );
  RECORD *record = SaveControl_DataPtrGet( save, GMDATA_ID_RECORD );
  
#ifdef DEBUG_BSW_PRINT
  KAGAYA_Printf( "TowerLoseScoreSet -> mode = %d\n", bsw_scr->play_mode );
#endif  
  id = bsw_scr->play_mode * 2 + RECID_RENSHOU_SINGLE;
  
  BSUBWAY_SCOREDATA_ResetRenshou( bsw_scr->scoreData, bsw_scr->play_mode );
  
  BSUBWAY_SCOREDATA_SetRenshou(
     bsw_scr->scoreData, BSWAY_SETMODE_reset );

  //現在の最大連勝数取得
  before = RECORD_Get( record, id );
  
  //最大連勝数更新
  after = RECORD_SetIfLarge(record,id,wk->renshou+wk->now_win);

  //更新している||(記録が7の倍数&&前後の値が同じ)なら番組作成
  if(after > 1){
    if(  (before < after) ||
      ((before == after) && (after%7==0)) ){
      towerscr_MakeTVRenshouMaxUpdate(wk,fsys,after);
    }
  }
  //成績モニタ用に現在の連勝数も書き出しておく
  chg_flg = TowerScoreData_SetFlags(wk->scoreSave,
      BSWAY_SFLAG_SINGLE_RECORD+wk->play_mode,BSWAY_DATA_get);
  if(chg_flg){  //連勝記録挑戦中なら加算
    ret = RECORD_Add(record,id+1,wk->now_win);
  }else{  //前回負けていればセット
    ret = RECORD_Set(record,id+1,wk->now_win);
  }

  //レコード挑戦中フラグを落とす
  TowerScoreData_SetFlags(
      wk->scoreSave,
      BSWAY_SFLAG_SINGLE_RECORD+wk->play_mode,BSWAY_DATA_reset);
  
  //延べ勝利数更新
  RECORD_Add(record,RECID_BTOWER_WIN,wk->now_win);

  //現在の周回数リセット
  BSUBWAY_SCOREDATA_ResetStageCount(
      bsw_scr->scoreData, bsw_scr->play_mode );

  #if 0  //wb null
  //バトルタワーへのチャレンジ数追加
  RECORD_Add(SaveData_GetRecord(savedata),RECID_BTOWER_CHALLENGE,1);
  
  //連勝リボンをもらえるかどうかのフラグをセット
  towerscr_IfRenshouPrizeGet(wk);
  #endif

  //勝ち負け共通データ処理
  ret+=1;
  
  if(ret > 9999){
    ret = 9999;  
  }

  towerscr_SetCommonScore(wk,savedata,FALSE,ret);
}
#endif

#if 0
static void towerscr_SetCommonScore(BTOWER_SCRWORK* wk,SAVEDATA* sv,u8 win_f,u16 now_renshou)
{
	u8	buf8;

	switch(wk->play_mode){
	case BSWAY_MODE_SINGLE:
		//ポケモンデータセット
		towerscr_SaveMemberPokeData(wk,sv,BTWR_SCORE_POKE_SINGLE);
	case BTWR_MODE_DOUBLE:
		if(now_renshou >= 7){
			//TVインタビューデータセット(シングルとダブルで実行)
			TVTOPIC_BTowerTemp_Set(SaveData_GetTvWork(sv),win_f,now_renshou);
		}
		break;
	case BTWR_MODE_WIFI:
		//ポケモンデータセット
		towerscr_SaveMemberPokeData(wk,sv,BTWR_SCORE_POKE_WIFI);
		//スコア押し出し
		TowerPlayData_WifiRecordAdd(wk->playSave,wk->rec_down,wk->rec_turn,wk->rec_damage);

		//プレイモード書き出し
		buf8 = wk->play_mode;
		TowerPlayData_Put(wk->playSave,BTWR_PSD_playmode,&buf8);
		//ラウンド数書き出し
		buf8 = wk->now_round;
		TowerPlayData_Put(wk->playSave,BTWR_PSD_round,&buf8);
		
		TowerScoreData_SetWifiScore(wk->scoreSave,wk->playSave);
		break;
	default:
		break;
	}
}
#endif


//--------------------------------------------------------------
//
//--------------------------------------------------------------
enum
{
  BTP_SINGLE,
  BTP_S_SINGLE,
  BTP_DOUBLE,
  BTP_S_DOUBLE,
  BTP_MULTI,
  BTP_S_MULTI,
  BTP_WIFI,
  BTP_TYPE_MAX,
};

static const u8 bpoint_tbl[BTP_TYPE_MAX][10] =
{
  //シングル
  {
    3, 3, 3, 0, 0,
    0, 0, 0, 0, 0
  },
  //スーパーシングル
  {
    5, 6, 7, 8, 9,
    10, 10, 10, 10, 10
  },
  //ダブル
  {
    3, 3, 3, 0, 0,
    0, 0, 0, 0, 0
  },
  //スーパーダブル
  {
    5, 6, 7, 8, 9,
    10, 10, 10, 10, 10
  },
  //マルチ
  {
    3, 3, 3, 0, 0,
    0, 0, 0, 0, 0
  },
  //スーパーマルチ
  {
    5, 6, 7, 8, 9,
    10, 10, 10, 10, 10
  },
  //WiFi ランク別
  {
    10, 10, 10, 10, 10,
    10, 10, 10, 10, 10
  },
};

//--------------------------------------------------------------
/**
 * バトルポイント追加
 * @param bsw_scr BSUBWAY_SCRWORK
 * @retval nothing
 */
//--------------------------------------------------------------
u16  BSUBWAY_SCRWORK_AddBattlePoint( BSUBWAY_SCRWORK *bsw_scr )
{
  u16 point = 0;
  
  if( bsw_scr->play_mode == BSWAY_MODE_WIFI ){ //ランクごと
    u16 dl_play = (u16)BSUBWAY_PLAYDATA_GetData(
        bsw_scr->playData, BSWAY_PLAYDATA_ID_sel_wifi_dl_play, NULL );
    
    if( dl_play == TRUE ){ //ダウンロードによる新規プレイである
      s8 rank = BSUBWAY_SCOREDATA_GetWifiRank( bsw_scr->scoreData );
      if( rank < 0 ){
        rank = 0;
      }else if( rank >= 10 ){
        rank = 9;
      }
      point = bpoint_tbl[BTP_WIFI][rank];
    }else{ //以前のデータ
      point = 5; //一律5
    }
  }else{ //周回数ごと
    u16 type;
    BOOL super = FALSE;
    u16 stage = BSUBWAY_SCOREDATA_GetStageNo_Org0(
        bsw_scr->scoreData, bsw_scr->play_mode );
    
    switch( bsw_scr->play_mode ){
    case BSWAY_MODE_SINGLE:
      type = BTP_SINGLE;
      break;
    case BSWAY_MODE_S_SINGLE:
      type = BTP_S_SINGLE;
      super = TRUE;
      break;
    case BSWAY_MODE_DOUBLE:
      type = BTP_DOUBLE;
      break;
    case BSWAY_MODE_S_DOUBLE:
      type = BTP_S_DOUBLE;
      super = TRUE;
      break;
    case BSWAY_MODE_MULTI:
      type = BTP_MULTI;
      break;
    case BSWAY_MODE_COMM_MULTI:
      type = BTP_MULTI;
      break;
    case BSWAY_MODE_S_MULTI:
      type = BTP_S_MULTI;
      super = TRUE;
      break;
    case BSWAY_MODE_S_COMM_MULTI:
      type = BTP_S_MULTI;
      super = TRUE;
      break;
    case BSWAY_MODE_WIFI:
      type = BTP_WIFI;
      break;
    default:
      GF_ASSERT( 0 );
      type = BTP_SINGLE;
    }

    stage--;
    
    if( (s16)stage < 0 ){
      stage = 0;
    }else if( stage >= 10 ){
      stage = 9;
    }
    
    if( bsw_scr->boss_f ){
      if( super == TRUE ){
        point = 30;
      }else{
        point = 10;
      }
    }else{
      point = bpoint_tbl[type][stage];
    }
  }
  
  if( point == 0 ){
    point = 1;
  }
  
  BSUBWAY_SCOREDATA_AddBattlePoint( bsw_scr->scoreData, point );
  return point;
}

//--------------------------------------------------------------
/**
 * トレーナーナンバーをセット
 * @param bsw_scr BSUBWAY_SCRWORK
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_SetBtlTrainerNo( BSUBWAY_SCRWORK *bsw_scr )
{
  int i;
  u8 play_mode;
  u16 no,stage,renshou;
  
  play_mode = bsw_scr->play_mode;
  
  //ステージ数がまだ存在していない場合も考慮し、連勝数からステージ数を出す
  renshou = BSUBWAY_SCRWORK_GetNowRenshou( bsw_scr );
  stage = BSUBWAY_SCRWORK_RenshouToStageNo( renshou );
  
#ifdef DEBUG_BSW_PRINT
  KAGAYA_Printf( "バトルサブウェイ　トレーナーNo抽選 STAGE=%d,連勝数%d\n",
    stage, renshou );
#endif
  
  if( play_mode == BSWAY_MODE_MULTI ||
      play_mode == BSWAY_MODE_COMM_MULTI ||
      play_mode == BSWAY_MODE_S_MULTI ||
      play_mode == BSWAY_MODE_S_COMM_MULTI )
  {
    if( stage < bsw_scr->pare_stage_no )
    {
      stage = bsw_scr->pare_stage_no;  //通信時には周回数の多いほうで抽選
    }
    
    for( i = 0; i < BSUBWAY_STOCK_TRAINER_MAX; i++ )
    {
      do
      {
        no = BSUBWAY_SCRWORK_GetTrainerNo(
            bsw_scr, stage, i/2, play_mode, (i)&0x01 );
      }while( is_ConflictTrainer(bsw_scr->trainer,no,i) );
      
      bsw_scr->trainer[i] = no;
    }
  }
  else
  {
    for( i = 0; i < (BSUBWAY_STOCK_TRAINER_MAX/2); i++ )
    {
      do
      {
        no = BSUBWAY_SCRWORK_GetTrainerNo(
            bsw_scr, stage, i, bsw_scr->play_mode, 0 );
      }while( is_ConflictTrainer(bsw_scr->trainer,no,i) );
      
      bsw_scr->trainer[i] = no;
    }
  }
}

//--------------------------------------------------------------
/**
 * @brief  選択したポケモンを取得
 * @param wk BSUBWAY_SCRWORK
 * @param gsys GAMESYS_WORK
 * @retval  BOOL TRUE  選択した FALSE  選択せずにやめた
 */
//--------------------------------------------------------------
BOOL BSUBWAY_SCRWORK_GetEntryPoke( BSUBWAY_SCRWORK *bsw_scr, GAMESYS_WORK *gsys )
{
  if( bsw_scr->pokelist_return_mode != PL_RET_NORMAL ||
      bsw_scr->pokelist_result_select == PL_SEL_POS_EXIT ||
      bsw_scr->pokelist_result_select == PL_SEL_POS_EXIT2 )
  {
    return FALSE;
  }
  else
  {
    u16  i = 0;
    const POKEPARTY *party;
    POKEMON_PARAM *pp;

    party = BSUBWAY_SCRWORK_GetPokePartyUse( bsw_scr );
    
    for( i = 0; i < bsw_scr->member_num; i++ ){ //ポケモン選択の手持ちNo
      if( (bsw_scr->pokelist_select_num[i]-1) >= 6 ){
        GF_ASSERT( 0 );
        bsw_scr->pokelist_select_num[i] = 1;
      }
      
      bsw_scr->member[i] = bsw_scr->pokelist_select_num[i] - 1;

      pp = PokeParty_GetMemberPointer(
          (POKEPARTY*)party, bsw_scr->member[i] );
      bsw_scr->mem_poke[i] = PP_Get( pp, ID_PARA_monsno, NULL );  
      bsw_scr->mem_item[i] = PP_Get( pp, ID_PARA_item, NULL );  

      KAGAYA_Printf( "BSW ENTRY POKE MENBER No.%d Pos %d\n",
          i, bsw_scr->member[i] );
    }
    
    return TRUE;
  }
}

//--------------------------------------------------------------
/**
 *  @brief  7連勝しているかどうかチェック
 *  @param bsw_scr BSUBWAY_SCRWORK
 *  @retval BOOL TRUE=７連勝している
 */
//--------------------------------------------------------------
BOOL BSUBWAY_SCRWORK_IsClear( BSUBWAY_SCRWORK *bsw_scr )
{
  if( bsw_scr->clear_f == FALSE ){
    u32 round = BSUBWAY_PLAYDATA_GetRoundNo( bsw_scr->playData );
    
    if( round < BSWAY_CLEAR_WINCNT ){
      return( FALSE );
    }
    
    bsw_scr->clear_f = 1; //クリアフラグon
  }
  
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * @brief  クリア処理
 * @param bsw_scr BSUBWAY_SCRWORK
 * @param gsys GAMESYS_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_SetClearScore(
    BSUBWAY_SCRWORK *bsw_scr, GAMESYS_WORK *gsys )
{
  u32 ret = 0;
  u16 before,after,chg_flg;
  u16 play_mode = bsw_scr->play_mode;
  u16 renshou = BSUBWAY_SCRWORK_GetNowRenshou( bsw_scr );
  
  //現在の連勝数をセーブ
  BSUBWAY_SCOREDATA_SetRenshou( bsw_scr->scoreData, play_mode, renshou );
  
  //最大連勝数更新
  BSUBWAY_SCOREDATA_UpdateRenshouMax(
      bsw_scr->scoreData, play_mode,
      BSUBWAY_SCOREDATA_GetRenshou(bsw_scr->scoreData,play_mode) );
  
  //周回数プラス
  BSUBWAY_SCOREDATA_IncStageNo( bsw_scr->scoreData, play_mode );
  
  { //勝ち負け共通データ作成
    SAVE_CONTROL_WORK *save = GAMEDATA_GetSaveControlWork( bsw_scr->gdata );
    bsw_SetCommonScore( bsw_scr, save, TRUE, renshou );
  }
  
  //ラウンド数リセット
  BSUBWAY_PLAYDATA_ResetRoundNo( bsw_scr->playData );
}

//--------------------------------------------------------------
/**
 * @brief  ホームに着いた際のワーク処理
 * @param bsw_scr BSUBWAY_SCRWORK
 * @param gsys GAMESYS_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_SetHomeWork(
    BSUBWAY_SCRWORK *bsw_scr, GAMESYS_WORK *gsys )
{
  bsw_scr->clear_f = 0;
  bsw_scr->boss_f = 0;
}

//--------------------------------------------------------------
/**
 * 対戦トレーナー抽選
 * @param bsw_scr BSUBWAY_SCRWORK
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_ChoiceBattlePartner( BSUBWAY_SCRWORK *bsw_scr )
{
  int i;
  u16 round;
  u16 monsno[2];
  u16 itemno[2];
   
  round = BSUBWAY_PLAYDATA_GetRoundNo( bsw_scr->playData );
  
  switch( bsw_scr->play_mode ){
  case BSWAY_MODE_WIFI:
    BSUBWAY_WIFIDATA_GetBtlPlayerData( bsw_scr->wifiData,
        &bsw_scr->tr_data[0], round, bsw_scr->heapID );
    break;
  case BSWAY_MODE_MULTI:
  case BSWAY_MODE_S_MULTI:
  case BSWAY_MODE_COMM_MULTI:
  case BSWAY_MODE_S_COMM_MULTI:
    BSUBWAY_SCRWORK_MakeRomTrainerData(
        bsw_scr, &(bsw_scr->tr_data[0]),
        bsw_scr->trainer[round*2+0],
        bsw_scr->member_num, NULL, NULL, NULL, bsw_scr->heapID );
    
    for( i = 0; i < bsw_scr->member_num; i++ ){ //モンスターNo重複チェック
      monsno[i] = bsw_scr->tr_data[0].btpwd[i].mons_no;
      itemno[i] = bsw_scr->tr_data[0].btpwd[i].item_no;
    }
    
    BSUBWAY_SCRWORK_MakeRomTrainerData(
        bsw_scr, &(bsw_scr->tr_data[1]),
        bsw_scr->trainer[round*2+1],
        bsw_scr->member_num, monsno, itemno, NULL, bsw_scr->heapID );
    break;
  case BSWAY_MODE_SINGLE:
  case BSWAY_MODE_DOUBLE:
  case BSWAY_MODE_S_SINGLE:
  case BSWAY_MODE_S_DOUBLE:
  default:
    BSUBWAY_SCRWORK_MakeRomTrainerData(
      bsw_scr, &(bsw_scr->tr_data[0]),
      bsw_scr->trainer[round],
      bsw_scr->member_num, NULL, NULL, NULL, bsw_scr->heapID );
    break;
  }
}

//--------------------------------------------------------------
/**
 * トレーナータイプからOBJコードを取得してくる
 * @param bsw_scr BSUBWAY_SCR
 * @param idx トレーナーデータインデックス
 * @retval u16 OBJコード
 */
//--------------------------------------------------------------
u16 BSUBWAY_SCRWORK_GetTrainerOBJCode( BSUBWAY_SCRWORK *bsw_scr, u16 idx )
{
//  return BSUBWAY_GetTrainerOBJCode( bsw_scr->tr_data[idx].bt_trd.tr_type );   //100424del saito
    return  FBI_TOOL_GetTrainerOBJCode( bsw_scr->tr_data[idx].bt_trd.tr_type );
}

//--------------------------------------------------------------
/**
 * @brief  現在のWIFIランクを操作して返す
 * @param bsw_scr BSUBWAY_SCRWORK*
 * @param gsys GAMESYS_WORK
 * @param mode BSWAY_SETMODE
 * @retval u16 操作後のランク
 */
//--------------------------------------------------------------
#if 0
u16 BSUBWAY_SCRWORK_SetWifiRank(
    BSUBWAY_SCRWORK *bsw_scr, GAMESYS_WORK *gsys, BSWAY_SETMODE mode )
{
  u8 ct,rank;
  BSUBWAY_SCOREDATA *score = bsw_scr->scoreData;
  static const u8 btower_wifi_rankdown[] = {
   0,5,4,4,3,3,2,2,1,1,
  };

  switch( mode ){
  case BSWAY_SETMODE_get:
    return (u16)BSUBWAY_SCOREDATA_GetWifiRank( score );
  case BSWAY_SETMODE_inc:  //Inc
    BSUBWAY_SCOREDATA_SetFlag( score,  //連続敗戦フラグを落とす
      BSWAY_SCOREDATA_FLAG_WIFI_LOSE_F, BSWAY_SETMODE_reset);
    rank = BSUBWAY_SCOREDATA_GetWifiRank( score );
    
    if( rank == 10 ){  
      bsw_scr->prize_f = 1; //ランク10ならリボンを貰える条件はクリア(08.06.01)
      return 0;  //もう上がらない
    }
    
    //ランクアップ処理
    BSUBWAY_SCOREDATA_IncWifiRank( score );
    
    //ランク5以上にアップしてたらリボンがもらえる
    if( rank+1 >= 5 ){
      bsw_scr->prize_f = 1;
    }
    return 1;
  case BSWAY_SETMODE_dec:  //dec
    //現在の連続敗戦数をカウント
    ct = BSUBWAY_SCOREDATA_SetWifiLoseCount( score, BSWAY_SETMODE_inc );
    rank = BSUBWAY_SCOREDATA_GetWifiRank( score );
    
    if( rank == 1 ){
      return 0;
    }
    
    //ランク別敗戦カウントチェック
    if( ct >= btower_wifi_rankdown[rank-1] ){
      BSUBWAY_SCOREDATA_DecWifiRank( score ); //ランクダウン
      //連続敗戦数と連続敗戦フラグをリセット
      BSUBWAY_SCOREDATA_SetWifiLoseCount( score, BSWAY_SETMODE_reset );
      BSUBWAY_SCOREDATA_SetFlag( score,
        BSWAY_SCOREDATA_FLAG_WIFI_LOSE_F, BSWAY_SETMODE_reset );
      return 1;
    }
    return 0;
  }
  return 0;
}
#else
u16 BSUBWAY_SCRWORK_SetWifiRank(
    BSUBWAY_SCOREDATA *score, GAMESYS_WORK *gsys, BSWAY_SETMODE mode )
{
  u8 ct,rank;
  static const u8 btower_wifi_rankdown[10] = {
   0,5,4,4,3,3,2,2,1,1,
  };
  
  rank = (u16)BSUBWAY_SCOREDATA_GetWifiRank( score );
  
  switch( mode ){
  case BSWAY_SETMODE_get:
    return rank;
  case BSWAY_SETMODE_inc:  //Inc
    BSUBWAY_SCOREDATA_SetFlag( score,  //連続敗戦フラグを落とす
      BSWAY_SCOREDATA_FLAG_WIFI_LOSE_F, BSWAY_SETMODE_reset);
    
    if( rank == 10 ){  
      return FALSE;  //もう上がらない
    }
    
    //ランクアップ処理
    BSUBWAY_SCOREDATA_IncWifiRank( score );
    return TRUE; //ランクアップした
  case BSWAY_SETMODE_dec:  //dec
    //現在の連続敗戦数をカウント
    ct = BSUBWAY_SCOREDATA_SetWifiLoseCount( score, BSWAY_SETMODE_inc );

    if( rank == 1 ){
      return FALSE;
    }
    
    //ランク別敗戦カウントチェック
    if( ct >= btower_wifi_rankdown[rank-1] ){
      BSUBWAY_SCOREDATA_DecWifiRank( score ); //ランクダウン
      
      //連続敗戦数と連続敗戦フラグをリセット
      BSUBWAY_SCOREDATA_SetWifiLoseCount( score, BSWAY_SETMODE_reset );
      BSUBWAY_SCOREDATA_SetFlag( score,
        BSWAY_SCOREDATA_FLAG_WIFI_LOSE_F, BSWAY_SETMODE_reset );
      return TRUE; //ランクダウンした
    }
    
    return FALSE;
  }
  return 0;
}
#endif

//--------------------------------------------------------------
/**
 * バトルボックス使用準備
 * @param bsw_scr BSUBWAY_SCRWORK
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_PreparBattleBox( BSUBWAY_SCRWORK *bsw_scr )
{
  GF_ASSERT( BSUBWAY_PLAYDATA_GetData(
        bsw_scr->playData,BSWAY_PLAYDATA_ID_use_battle_box, NULL ) );
  
  if( bsw_scr->btl_box_party != NULL ){ //開放が必要
    GFL_HEAP_FreeMemory( bsw_scr->btl_box_party );
  }
  
  {
    SAVE_CONTROL_WORK *save = GAMEDATA_GetSaveControlWork( bsw_scr->gdata );
    BATTLE_BOX_SAVE *bb_save = BATTLE_BOX_SAVE_GetBattleBoxSave( save );
    bsw_scr->btl_box_party =
      BATTLE_BOX_SAVE_MakePokeParty( bb_save, HEAPID_PROC );
  }
}

//--------------------------------------------------------------
/**
 * 使用するPOKEPARTY取得
 * @param bsw_scr BSUBWAY_SCRWORK
 * @retval POKEPARTY*
 */
//--------------------------------------------------------------
const POKEPARTY * BSUBWAY_SCRWORK_GetPokePartyUse(
    const BSUBWAY_SCRWORK *bsw_scr )
{
  const POKEPARTY *party;
  u32 use_bbox = (u32)BSUBWAY_PLAYDATA_GetData(
      bsw_scr->playData, BSWAY_PLAYDATA_ID_use_battle_box, NULL );
  
  if( use_bbox == FALSE ){
    party = GAMEDATA_GetMyPokemon( bsw_scr->gdata ); 
  }else{
    party = bsw_scr->btl_box_party; //バトルボックス用POKEPARTY
  }
  
  GF_ASSERT( party != NULL );
  return( party );
}

//======================================================================
//  連勝数
//======================================================================
//--------------------------------------------------------------
/**
 * 現在の連勝数取得
 * @param
 * @retval
 */
//--------------------------------------------------------------
u16 BSUBWAY_SCRWORK_GetNowRenshou( const BSUBWAY_SCRWORK *bsw_scr )
{
  return( bsw_scr->now_renshou );
}

//--------------------------------------------------------------
/**
 * 現在の連勝数セット
 * @param
 * @retval
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_SetNowRenshou( BSUBWAY_SCRWORK *bsw_scr, u16 renshou )
{
  bsw_scr->now_renshou = renshou;
}

//--------------------------------------------------------------
/**
 * 現在の連勝数を増加
 * @param
 * @retval
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_IncNowRenshou( BSUBWAY_SCRWORK *bsw_scr )
{
  if( bsw_scr->now_renshou < BSW_RENSHOU_DATA_MAX ){
    bsw_scr->now_renshou++;
  }
}

//--------------------------------------------------------------
/**
 * 現在の連勝数をリセット
 * @param
 * @retval
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_ResetNowRenshou( BSUBWAY_SCRWORK *bsw_scr )
{
  bsw_scr->now_renshou = 0;
}

//--------------------------------------------------------------
/**
 * 現在の連勝数を復元
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void bsw_RecoverNowRenshou(
    BSUBWAY_SCRWORK *bsw_scr, u16 stage, u16 round )
{
  u32 renshou;
  
  renshou = stage * 7; //1stage = 7round
  renshou += round;    //1-6round
  
  if( renshou > BSW_RENSHOU_DATA_MAX ){
    renshou = BSW_RENSHOU_DATA_MAX;
  }
  
  bsw_scr->now_renshou = renshou;
  
  #ifdef DEBUG_BSW_PRINT 
  KAGAYA_Printf( "バトルサブウェイ連勝数復元 %d\n", renshou );
  #endif
}

//======================================================================
//  parts
//======================================================================
//--------------------------------------------------------------
/**
 * プレイモード別メンバー数
 * @param play_mode 
 * @retval  
 */
//--------------------------------------------------------------
u32 BSUBWAY_SCRWORK_GetPlayModeMemberNum( u16 mode )
{
  switch(mode){
  case BSWAY_MODE_SINGLE:
  case BSWAY_MODE_S_SINGLE:
  case BSWAY_MODE_WIFI:
    return 3;
  case BSWAY_MODE_DOUBLE:
  case BSWAY_MODE_S_DOUBLE:
    return 4;
  case BSWAY_MODE_MULTI:
  case BSWAY_MODE_S_MULTI:
  case BSWAY_MODE_COMM_MULTI:
  case BSWAY_MODE_S_COMM_MULTI:
    return 2;
  }
  GF_ASSERT( 0 );
  return 0;
}

//--------------------------------------------------------------
/**
 * トレーナーID重複チェック
 * @param trainer トレーナーIDが格納されたテーブル
 * @param id チェックするID
 * @param num trainer要素数
 * @retval BOOL TRUE=重複している
 */
//--------------------------------------------------------------
static BOOL is_ConflictTrainer( u16 * trainer, u16 id, u16 num )
{
  u16 i;

  for( i = 0;i < num;i++){
    if( trainer[i] == id ){
#ifdef DEBUG_BSW_PRINT
      KAGAYA_Printf( "BSW is_ConflictTrainer() CONFLICT!! : " );
      KAGAYA_Printf( "TR NO = %d : DATA NO = %d : HIT NO = %d\n", id, num, i );
#endif
      return TRUE;
    }
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * 連勝数からステージ数を取得　ステージ数は0オリジン
 * @param
 * @retval
 */
//--------------------------------------------------------------
u16 BSUBWAY_SCRWORK_RenshouToStageNo( u16 renshou )
{
  u16 stage = renshou / 7;
  return( stage );
}

//======================================================================
//  wb null
//======================================================================
//--------------------------------------------------------------
/**
 * 特殊接続先に現在のロケーションをセット
 * @param
 * @retval
 */
//--------------------------------------------------------------
#if 0 //wb null
void BSUBWAY_SCRWORK_PushNowLocation( GAMEDATA *gdata )
{
  GAMEDATA_SetSpecialLocation( gdata, &loc );
}
#endif

//--------------------------------------------------------------
/**
 *  @brief  バトルタワー用ポケモン選択画面呼び出し呼出し
 */
//--------------------------------------------------------------
#if 0 //wb null
void BSUBWAY_SCRWORK_SelectPoke(
    BSUBWAY_SCRWORK *wk, GMEVENT *ev, void **proc_wk )
{
  EventCmd_BTowerPokeSelectCall( ev_work,proc_wk,
    PL_MODE_BATTLE_TOWER,PST_MODE_NORMAL,
    wk->member_num,wk->member_num,100,0);
}
#endif

//--------------------------------------------------------------
/**
 *  @brief  参加指定した手持ちポケモンの条件チェック
 *  @retval  0  参加OK
 *  @retval  1  同じポケモンがいる
 *  @retval 2  同じアイテムを持っている
 */
//--------------------------------------------------------------
#if 0 //wb null
int BSUBWAY_SCRWORK_CheckEntryPoke(
    BSUBWAY_SCRWORK *wk, GAMESYS_WORK *gsys ) //SAVEDATA *savedata )
{
#if 0 //wb
#if 1
  u16  i = 0,j = 0;
  u16  monsno[BSUBWAY_STOCK_MEMBER_MAX],itemno[BSUBWAY_STOCK_MEMBER_MAX];
  POKEPARTY *party;
  POKEMON_PARAM *poke;

  party = SaveData_GetTemotiPokemon( savedata );

  for( i = 0;i < wk->member_num;i++){
    poke = PokeParty_GetMemberPointer( party,wk->member[i]);
    monsno[i] = PokeParaGet( poke,ID_PARA_monsno,NULL );
    itemno[i] = PokeParaGet( poke,ID_PARA_item,NULL );

    if( i == 0){
      continue;
    }
    for( j = 0;j < i;j++){
      if( monsno[i] == monsno[j]){
        //同じポケモンがいる
        return 1;
      }
      if(( itemno[i] != 0) && ( itemno[i] == itemno[j])){
        //同じアイテムをもっている
        return 2;
      }
    }
  }
  return 0;
#else
  return BSUBWAY_SCRWORK_CheckEntryPokeSub( wk->member, wk->member_num, savedata );
#endif
#else
  return 0;
#endif
}
#endif

//--------------------------------------------------------------
/**
 *  @brief　対戦トレーナーNo抽選
 */
//--------------------------------------------------------------
#if 0 //wb null
void BSUBWAY_SCRWORK_BtlTrainerNoSet( BSUBWAY_SCRWORK *wk, GAMESYS_WORK *gsys )
{
#if 0 //wb
  int i;
  u16  no,stage;
  
  if(  wk->play_mode == BSWAY_MODE_MULTI ||
    wk->play_mode == BSWAY_MODE_WIFI_MULTI ||
    wk->play_mode == BSWAY_MODE_COMM_MULTI ){
    //if( wk->play_mode == BSWAY_MODE_COMM_MULTI && wk->pare_round > wk->stage ){
    if( ( wk->play_mode == BSWAY_MODE_COMM_MULTI && wk->pare_round > wk->stage ) ||
      ( wk->play_mode == BSWAY_MODE_WIFI_MULTI && wk->pare_round > wk->stage ) ){
      stage = wk->pare_round;  //通信時には周回数の多いほうで抽選
    }else{
      stage = wk->stage;
    }
    for( i = 0;i < BSUBWAY_STOCK_TRAINER_MAX;i++){
      do{
        no = BattleTowerTrainerNoGet( wk,stage,i/2,wk->play_mode );
      }while( is_trainer_conflict( wk->trainer,no,i ));
#ifdef BSUBWAY_AUTO_DEB
#if 0
      wk->trainer[i] = DebugBTowerAutoTrainer;
#ifdef DEBUG_BSW_PRINT
      KAGAYA_Printf(" #TowerStage = %d,%d\n",DebugBTowerAutoStage,DebugBTowerAutoTrainer );
#endif
      if( DebugBTowerAutoTrainer++ >= 299){
        DebugBTowerAutoTrainer = 0;
        DebugBTowerAutoStage++;
      }
#endif
      wk->trainer[i] = no;
      if( DebugBTowerAutoTrainer++ >= 299){
        DebugBTowerAutoStage++;
      }
#ifdef DEBUG_BSW_PRINT
      KAGAYA_Printf(" #TowerStage = %d,%d\n",DebugBTowerAutoStage,DebugBTowerAutoTrainer );
#endif
#else    
      wk->trainer[i] = no;
#endif
    }
  }else{
    for( i = 0;i < ( BSUBWAY_STOCK_TRAINER_MAX/2);i++){
      do{
        no = BattleTowerTrainerNoGet( wk,wk->stage,i,wk->play_mode );
      }while( is_trainer_conflict( wk->trainer,no,i ));
#ifdef BSUBWAY_AUTO_DEB
#if 0
      wk->trainer[i] = DebugBTowerAutoTrainer;
#ifdef DEBUG_BSW_PRINT
      KAGAYA_Printf(" #TowerStage = %d,%d\n",DebugBTowerAutoStage,DebugBTowerAutoTrainer );
#endif
      if( DebugBTowerAutoTrainer++ >= 299){
        DebugBTowerAutoTrainer = 0;
        DebugBTowerAutoStage++;
      }
#endif
      if( DebugBTowerAutoTrainer++ >= 299){
        DebugBTowerAutoStage++;
      }
#ifdef DEBUG_BSW_PRINT
      KAGAYA_Printf(" #TowerStage = %d,%d\n",DebugBTowerAutoStage,DebugBTowerAutoTrainer );
#endif
      wk->trainer[i] = 25;//no;
#else    
      wk->trainer[i] = no;
#endif
    }
  }
#endif
}
#endif

//--------------------------------------------------------------
/**
 *  @brief  最大連勝記録更新番組作成関数
 */
//--------------------------------------------------------------
#if 0 //wb null
static void bswayscr_MakeTVRenshouMaxUpdate(
    BSUBWAY_SCRWORK *wk,SAVEDATA *savedata,u16 renshou )
{
  POKEPARTY *party;

  if(  wk->play_mode != BSWAY_MODE_SINGLE &&
    wk->play_mode != BSWAY_MODE_DOUBLE ){
    return;
  }
  party = SaveData_GetTemotiPokemon( savedata );

  if( wk->play_mode == BSWAY_MODE_SINGLE ){
    //TVTOPIC_Entry_Record_BTower( fsys,
    TVTOPIC_Entry_Record_BTowerEx( savedata,
      renshou,PokeParty_GetMemberPointer( party,wk->member[0]), TRUE );
  }else{
    //TVTOPIC_Entry_Record_BTower( fsys,
    TVTOPIC_Entry_Record_BTowerEx( savedata,
      renshou,PokeParty_GetMemberPointer( party,wk->member[0]), FALSE );
  }
}
#endif

//--------------------------------------------------------------
/**
 *  @brief  7人抜き成功または負けた時の共通データセーブ処理
 *  @param  now_renshou  現在の連勝数
 */
//--------------------------------------------------------------
static void bsw_SetCommonScore(
    BSUBWAY_SCRWORK *wk, SAVE_CONTROL_WORK *sv, u8 win_f, u16 now_renshou )
{
  u8  buf8;
  
  switch( wk->play_mode ){
  case BSWAY_MODE_SINGLE:
    //ポケモンデータセット
    bsw_SaveMemberPokeData( wk,sv,BSWAY_SCORE_POKE_SINGLE );
  case BSWAY_MODE_DOUBLE:
    break;
  case BSWAY_MODE_WIFI:
    //ポケモンデータセット
    bsw_SaveMemberPokeData( wk, sv, BSWAY_SCORE_POKE_WIFI );
#if 0    
    //スコア押し出し
    BSUBWAY_PLAYDATA_AddWifiRecord(
        wk->playData, wk->rec_down, wk->rec_turn, wk->rec_damage );
#endif
    //プレイモード書き出し
    buf8 = wk->play_mode;
    BSUBWAY_PLAYDATA_SetData(
        wk->playData, BSWAY_PLAYDATA_ID_playmode, &buf8 );

    //ラウンド数書き出し
    buf8 = BSUBWAY_PLAYDATA_GetRoundNo( wk->playData ) + 1;
    BSUBWAY_PLAYDATA_SetData(
        wk->playData, BSWAY_PLAYDATA_ID_round, &buf8 );
    BSUBWAY_SCOREDATA_SetWifiScore( wk->scoreData, wk->playData );
    break;
  default:
    break;
  }
}

#if 0 //wb null
void BSUBWAY_SCRWORK_SetClearScore( BSUBWAY_SCRWORK *wk, GAMESYS_WORK *gsys )
{
/*
  u32  ret = 0;
  int  id;
//  void *note;
  u16  before,after,chg_flg;
//  RECORD * record;
//  FRONTIER_SAVEWORK *frontier;
    
  if( wk->play_mode == BSWAY_MODE_RETRY ){
    return;
  }
  
#if 0  
  record = SaveData_GetRecord( savedata );
  frontier = SaveData_GetFrontier( savedata );
#endif

#if 0
  if( wk->play_mode == BSWAY_MODE_WIFI_MULTI ){
    id = FRID_TOWER_MULTI_WIFI_RENSHOU;
  }else{
    id = wk->play_mode*2+FRID_TOWER_SINGLE_RENSHOU;
  }
#endif

  //現在の連勝数を書き出し
  if( wk->play_mode == BSWAY_MODE_WIFI_MULTI ){
    #if 0 //wb null
    chg_flg = FrontierRecord_Get( SaveData_GetFrontier( savedata ), 
          FRID_TOWER_MULTI_WIFI_CLEAR_BIT,
          Frontier_GetFriendIndex( FRID_TOWER_MULTI_WIFI_CLEAR_BIT ) );
    #else
    ch_flg = 0;
    #endif
  }else{
    chg_flg = BSUBWAY_SCOREDATA_SetFlag( wk->scoreData,
        BSWAY_SCOREDATA_FLAG_SINGLE_RECORD + wk->play_mode,
        BSWAY_DATA_get );
  }

  if( chg_flg ){  //現在も連勝記録挑戦中なら加算
    #if 0 //wb null
    ret = FrontierRecord_Get(  SaveData_GetFrontier( savedata ), 
                  ( id+1),
                  Frontier_GetFriendIndex( id+1) );

    FrontierRecord_Set(  SaveData_GetFrontier( savedata ), 
              ( id+1),
              Frontier_GetFriendIndex( id+1), ( ret+BSWAY_CLEAR_WINCNT ) );

    ret = FrontierRecord_Get(  SaveData_GetFrontier( savedata ), 
                  ( id+1),
                  Frontier_GetFriendIndex( id+1) );
    #endif
  }else{  //前回負けてた場合は一旦リセット
    #if 0 //wb null
    ret = FrontierRecord_Set(
        frontier,id+1,Frontier_GetFriendIndex( id+1),BSWAY_CLEAR_WINCNT );
    #endif
  }

#if 0
  //連勝してから、負けた時に、wk->renshouが必ず0になっていれば、
  //( renshou + now_win )をセットでいいはず(08.05.25)
  //7+7=14勝利(勝ち終わり)
  //7+3=10勝利(負け終わり)
  //0+3=3勝利(負け終わり)
#ifdef DEBUG_BSW_PRINT
  KAGAYA_Printf( "wk->renshou = %d\n", wk->renshou );
  KAGAYA_Printf( "wk->now_win = %d\n", wk->now_win );
#endif
  ret = FrontierRecord_Set( frontier,id+1,Frontier_GetFriendIndex( id+1),( wk->renshou+wk->now_win ));
#endif

  //レコード挑戦中フラグを立てる
#if 0
  if( wk->play_mode == BSWAY_MODE_WIFI_MULTI ){
#if 0
    TowerScoreData_SetFlags( wk->scoreSave,BSWAY_SFLAG_WIFI_MULTI_RECORD,BSWAY_DATA_set );
#else
    FrontierRecord_Set(  SaveData_GetFrontier( savedata ), 
              FRID_TOWER_MULTI_WIFI_CLEAR_BIT,
              Frontier_GetFriendIndex( FRID_TOWER_MULTI_WIFI_CLEAR_BIT ), 1 );
#endif
  }else{
    TowerScoreData_SetFlags( wk->scoreSave,BSWAY_SFLAG_SINGLE_RECORD+wk->play_mode,BSWAY_DATA_set );
  }
#endif

  //現在の最大連勝数取得
  before = FrontierRecord_Get( frontier,id,Frontier_GetFriendIndex( id ));
  //最大連勝数更新
  after = FrontierRecord_SetIfLarge( frontier,id,Frontier_GetFriendIndex( id ),ret );

  //更新してたら番組作成
#if 0
  if( before < after ){
    bswayscr_MakeTVRenshouMaxUpdate( wk,fsys->savedata,after );
  }
#endif

#if 0
  //延べ勝利数更新
  RECORD_Add( record,RECID_BSUBWAY_WIN,BSWAY_CLEAR_WINCNT );
  //周回数プラス
  TowerScoreData_SetStage( wk->scoreSave,wk->play_mode,BSWAY_DATA_inc );
#endif

#if 0
  //バトルタワーへのチャレンジ数追加
  if( wk->play_mode != BSWAY_MODE_WIFI_MULTI ){
    RECORD_Add( record,RECID_BSUBWAY_CHALLENGE,1);
  }

  //スコア加算
  RECORD_Score_Add( record,SCORE_ID_BSUBWAY_7WIN );
  
  //連勝リボンをもらえるかどうかのフラグをセット
  bswayscr_IfRenshouPrizeGet( wk );

  //勝ち負け共通データ処理
  bswayscr_SetCommonScore( wk,savedata,TRUE,ret );
#endif

#if 0
  //冒険ノート(通信マルチ&wifiのみ)
  if(  wk->play_mode == BSWAY_MODE_COMM_MULTI ||
    wk->play_mode == BSWAY_MODE_WIFI_MULTI ||
    wk->play_mode == BSWAY_MODE_WIFI ){
#else
  //冒険ノート( WIFI_DLのみ)
  if( wk->play_mode == BSWAY_MODE_WIFI ){
#endif
    note = FNOTE_SioBattleTowerDataMake( wk->heapID );
    FNOTE_DataSave( fnote, note, FNOTE_TYPE_SIO );
  }
*/
  u8 buf8;
  
  //プレイモード書き出し
  buf8 = wk->play_mode;
  BSUBWAY_PLAYDATA_SetData( wk->playData,
      BSWAY_PLAYDATA_ID_playmode, &buf8 );

  //ラウンド数書き出し
  buf8 = wk->now_round;
	BSUBWAY_PLAYDATA_SetData( wk->playData,
      BSWAY_PLAYDATA_ID_round, &buf8 );
  
  //連勝記録書き出し
  {
    u32 renshou = BSUBWAY_SCOREDATA_GetRenshou(
        wk->scoreData, wk->play_mode );
    
    if( renshou + wk->now_win > 0xffff ){
      renshou = 0xffff;
    }else{
      renshou += wk->now_win;
    }
#if 0   
    BSUBWAY_SCOREDATA_SetRenshouCount(
          wk->scoreData, wk->play_mode, renshou );
#endif
  }
}
#endif

void BSUBWAY_SCRWORK_ChoiceBtlSeven( BSUBWAY_SCRWORK *wk, u8 sex )
{
  int i;
  int start_no = BSWAY_PARTNER_DATA_START_NO_MAN;
  
  if( sex ){
    start_no = BSWAY_PARTNER_DATA_START_NO_WOMAN;
  }
  
  for(i = 0;i < BSWAY_PARTNER_NUM;i++){
    wk->five_item[i] = BSUBWAY_SCRWORK_MakeRomTrainerData(
        wk, &(wk->five_data[i]),
        start_no + i, wk->member_num,
        wk->mem_poke, wk->mem_item, &(wk->five_poke[i]), wk->heapID );
  }
}

// @brief  AIマルチペアポケモン抽選
#if 0 //wb
void BSUBWAY_SCRWORK_ChoiceBtlSeven( BSUBWAY_SCRWORK *wk )
{
  int i;
  
  for( i = 0;i < BSWAY_FIVE_NUM;i++){
    wk->five_item[i] = ( u8)RomBattleTowerTrainerDataMake( wk,&( wk->five_data[i]),
      TOWER_FIVE_FIRST+i,wk->member_num,wk->mem_poke,wk->mem_item,&( wk->five_poke[i]),wk->heapID );
  }
}
#endif

//--------------------------------------------------------------
/**
 *  @brief  戦闘呼び出し
 */
//--------------------------------------------------------------
#if 0 //wb
void BSUBWAY_SCRWORK_LocalBattleCall(
    GMEVENT_CONTROL *event,BSUBWAY_SCRWORK *wk,BOOL *win_flag )
{
  EventCmd_TowerLocalBattle( event,wk,win_flag );
}
#endif

//--------------------------------------------------------------
/**
 *  @brief  現在のプレイモードを返す
 */
//--------------------------------------------------------------
#if 0 // wb
u16 BSUBWAY_SCRWORK_GetPlayMode( BSUBWAY_SCRWORK *wk )
{
  return wk->play_mode;
}
#endif

//--------------------------------------------------------------
/**
 *  @brief  リーダークリアフラグを取得する
 */
//--------------------------------------------------------------
#if 0 // wb
u16 BSUBWAY_SCRWORK_GetLeaderClearFlag( BSUBWAY_SCRWORK *wk )
{
  return (u16)wk->leader_f;
}
#endif

//--------------------------------------------------------------
/**
 *  @brief  タワークリア時にバトルポイントを加算する
 *  @return  新たに取得したバトルポイント
 */
//--------------------------------------------------------------
#if 0 //wb
u16 BSUBWAY_SCRWORK_AddBtlPoint( BSUBWAY_SCRWORK *wk )
{
  u16  stage;
  u16  point = 0;
  static const u8  bpoint_wifi[] = {0,
    BTLPOINT_VAL_TOWER_WIFI1,BTLPOINT_VAL_TOWER_WIFI2,BTLPOINT_VAL_TOWER_WIFI3,
    BTLPOINT_VAL_TOWER_WIFI4,BTLPOINT_VAL_TOWER_WIFI5,BTLPOINT_VAL_TOWER_WIFI6,
    BTLPOINT_VAL_TOWER_WIFI7,BTLPOINT_VAL_TOWER_WIFI8,BTLPOINT_VAL_TOWER_WIFI9,
    BTLPOINT_VAL_TOWER_WIFI10,
  };
  static const u8 bpoint_normal[] = {0,
    BTLPOINT_VAL_TOWER_STAGE1,BTLPOINT_VAL_TOWER_STAGE2,BTLPOINT_VAL_TOWER_STAGE3,
    BTLPOINT_VAL_TOWER_STAGE4,BTLPOINT_VAL_TOWER_STAGE5,BTLPOINT_VAL_TOWER_STAGE6,
    BTLPOINT_VAL_TOWER_STAGE7,
  };
  static const u8 bpoint_comm[] = {0,
    BTLPOINT_VAL_TOWER_COMM1,BTLPOINT_VAL_TOWER_COMM2,BTLPOINT_VAL_TOWER_COMM3,
    BTLPOINT_VAL_TOWER_COMM4,BTLPOINT_VAL_TOWER_COMM5,BTLPOINT_VAL_TOWER_COMM6,
    BTLPOINT_VAL_TOWER_COMM7,
  };

  if( wk->play_mode == BSWAY_MODE_RETRY ){
    return 0;
  }
  if( wk->play_mode == BSWAY_MODE_WIFI ){
    //ランクごと
    point = bpoint_wifi[TowerScoreData_SetWifiRank( wk->scoreSave,BSWAY_DATA_get )];
  }else{

    //ワイアレスマルチ、WIFI
    //if( wk->play_mode == BSWAY_MODE_COMM_MULTI ){
    if( ( wk->play_mode == BSWAY_MODE_COMM_MULTI ) || ( wk->play_mode == BSWAY_MODE_WIFI_MULTI ) ){

      //周回数ごと
      stage = TowerScoreData_SetStage( wk->scoreSave,wk->play_mode,BSWAY_DATA_get );
      if( stage >= 7){                //この比較文はDPと合わせる
        point = BTLPOINT_VAL_TOWER_COMM8;
      }else{
        point = bpoint_comm[stage];
      }

#if 0
    //WIFI(32人データがあるので、周回数ワークが足りないため、連勝数から周回数を算出する
    }else if( wk->play_mode == BSWAY_MODE_WIFI_MULTI ){
#ifdef DEBUG_BSW_PRINT
      KAGAYA_Printf( "wk->renshou = %d\n", wk->renshou );
      KAGAYA_Printf( "wk->now_win = %d\n", wk->now_win );
#endif
      stage = ( wk->renshou / 7 );
      if( stage >= 7){                //この比較文はDPと合わせる
        point = BTLPOINT_VAL_TOWER_COMM8;
      }else{
        point = bpoint_comm[stage];
      }
#endif

    //シングル、ダブル、AIマルチ
    }else{

      //周回数ごと
      stage = TowerScoreData_SetStage( wk->scoreSave,wk->play_mode,BSWAY_DATA_get );
      if( wk->leader_f ){
        point = BTLPOINT_VAL_TOWER_LEADER;
      }else if( stage >= 7){
        point = BTLPOINT_VAL_TOWER_STAGE8;
      }else{
        point = bpoint_normal[stage];
      }
    }
  }
  //バトルポイントを加算する
  TowerScoreData_SetBattlePoint( wk->scoreSave,point,BSWAY_DATA_add );
  return point;
}

/**
 *  @brief  シングルで連勝した時のご褒美のトロフィーをあげるフラグを立てておく
 */
u16  BSUBWAY_SCRWORK_GoodsFlagSet( BSUBWAY_SCRWORK *wk,SAVEDATA *sv )
{
  u8  id,fid;
  u16  record,goods,ret;

  record = TowerScrTools_GetRenshouRecord( sv,wk->play_mode );

  if( record < BSWAY_50_RENSHOU_CNT ){
    return 0;
  }
  if( record >= BSWAY_100_RENSHOU_CNT ){
    if( TowerScoreData_SetFlags( wk->scoreSave,BSWAY_SFLAG_GOLD_GET,BSWAY_DATA_get )){
      return 0;
    }
  }else{
    if( TowerScoreData_SetFlags( wk->scoreSave,BSWAY_SFLAG_SILVER_GET,BSWAY_DATA_get )){
      return 0;
    }
  }
  return 1;
}
#endif

//--------------------------------------------------------------
/**
 *  @brief  現在のWIFIランクを操作して返す
 */
//--------------------------------------------------------------
#if 0 //wb
u16 BSUBWAY_SCRWORK_SetWifiRank(
    BSUBWAY_SCRWORK *wk, GAMESYS_WORK *gsys, u8 mode )
{
  u8  ct,rank;
  BTLTOWER_SCOREWORK *score = SaveData_GetTowerScoreData( sv );
  
  static const u8  btower_wifi_rankdown[] = {
   0,5,4,4,3,3,2,2,1,1,
//   0,2,1,1,1,1,1,1,1,1,
  };

  switch( mode ){
  //Get
  case 0:
    return ( u16)TowerScoreData_SetWifiRank( score,BSWAY_DATA_get );
  case 1:  //Inc
    //連続敗戦フラグを落とす
    TowerScoreData_SetFlags( score,BSWAY_SFLAG_WIFI_LOSE_F,BSWAY_DATA_reset );
    //現在のランクを取得
    rank = TowerScoreData_SetWifiRank( score,BSWAY_DATA_get );

    if( rank == 10){  
      wk->prize_f = 1;        //ランク10なら、リボンを貰える条件はクリア(08.06.01)
      return 0;  //もう上がらない
    }
#if 0
    //rank10の時も、リボンをもらえるフラグを立てる必要がある
    //ランクアップ処理は呼んではいけない
    //戻り値は、ランクアップしたか、ランクアップしていないかを返している
    //ランクアップした時しか、リボンを渡す流れに行っていないのは正しいのか？＞てつじさん
    //
    //msg_tower_56
    //「そして●さんは　ランク●に　なりました！」
    //
    //続いて、リボンを渡す流れへ
    //リボンをすでに持っているとか、全員持っているとかは、コメントを見る限りチェックされている
#endif

    //ランクアップ処理
    TowerScoreData_SetWifiRank( score,BSWAY_DATA_inc );

    //ランク5以上にアップしてたらリボンがもらえる
    if( rank+1 >= 5){
      wk->prize_f = 1;
    }
    return 1;
  case 2:  //dec
    //現在の連続敗戦数をカウント
    ct = TowerScoreData_SetWifiLoseCount( score,BSWAY_DATA_inc );
    rank = TowerScoreData_SetWifiRank( score,BSWAY_DATA_get );

    if( rank == 1){
      return 0;
    }
    //ランク別敗戦カウントチェック
    if( ct >= btower_wifi_rankdown[rank-1] ){
      //ランクダウン
      TowerScoreData_SetWifiRank( score,BSWAY_DATA_dec );
      //連続敗戦数と連続敗戦フラグをリセット
      TowerScoreData_SetWifiLoseCount( score,BSWAY_DATA_reset );
      TowerScoreData_SetFlags( score,BSWAY_SFLAG_WIFI_LOSE_F,BSWAY_DATA_reset );
      return 1;
    }
    return 0;
  }
  return 0;
}
#endif

//--------------------------------------------------------------
/**
 *  @brief  リーダーを倒したご褒美リボンをあげる
 */
//--------------------------------------------------------------
#if 0 //wb
u16 BSUBWAY_SCRWORK_LeaderRibbonSet( BSUBWAY_SCRWORK *wk, GAMESYS_WORK *gsys )
{
  if( wk->play_mode != BSWAY_MODE_SINGLE ){
    return 0;
  }
  
  //メンバーにリボンセット
  switch( wk->leader_f ){
  case 1:
    return bswayscr_PokeRibbonSet( sv,ID_PARA_sinou_battle_tower_ttwin_first,wk );
  case 2:
    return bswayscr_PokeRibbonSet( sv,ID_PARA_sinou_battle_tower_ttwin_second,wk );
  }
  return 0;
}
#endif

//--------------------------------------------------------------
/**
 *  @biref  プレイランダムシードを更新する
 *  d31r0201.ev
 *  case BSWAY_SUB_UPDATE_RANDOM で呼ばれる
 *  ここは、WIFIマルチは通過しないので、このままでよい
 */
//--------------------------------------------------------------
#if 0 //wb
u16 BSUBWAY_SCRWORK_PlayRandUpdate( BSUBWAY_SCRWORK *wk,SAVEDATA *sv )
{
  u8  chg_flg;

  //現在チャレンジ継続中かどうか？
  if( wk->play_mode == BSWAY_MODE_WIFI_MULTI ){
#if 0
    chg_flg = TowerScoreData_SetFlags( wk->scoreSave,
        BSWAY_SFLAG_WIFI_MULTI_RECORD,BSWAY_DATA_get );
#else
    chg_flg = FrontierRecord_Get( SaveData_GetFrontier( sv ), 
                  FRID_TOWER_MULTI_WIFI_CLEAR_BIT,
                  Frontier_GetFriendIndex( FRID_TOWER_MULTI_WIFI_CLEAR_BIT ) );
#endif
  }else{
    chg_flg = TowerScoreData_SetFlags( wk->scoreSave,
        BSWAY_SFLAG_SINGLE_RECORD+wk->play_mode,BSWAY_DATA_get );
  }
  
  if(!chg_flg ){
    //プレイ更新ランダムシード初期値取得&日付更新シード更新
    wk->play_rnd_seed = BtlTower_UpdatePlayRndSeed( sv );
  }else{
    //記録挑戦開始時のdayシードを基点に、現在のplayシードを取得する
    wk->play_rnd_seed = BtlTower_GetContinuePlayRndSeed( sv );
  }
  return ( wk->play_rnd_seed/65535);
}
#endif

//======================================================================
//  parts
//======================================================================
//--------------------------------------------------------------
/**
 *  @brief  ポケモンにリボンをつける　サブ
 */
//--------------------------------------------------------------
#if 0 //wb
static u16 bswayscr_PokeRibbonSet( SAVEDATA *sv,u8 ribbon,BSUBWAY_SCRWORK *wk )
{
  u8  flag = 1;
  u8 ct;
  int i;
  POKEPARTY  *party;
  POKEMON_PARAM *pp;
  
  party = SaveData_GetTemotiPokemon( sv );

  ct = 0;
  for( i = 0;i < wk->member_num;i++){
    pp = PokeParty_GetMemberPointer( party,wk->member[i]);
    if( PokeParaGet( pp,ribbon,NULL )){
      continue;  //既に持ってる
    }

    PokeParaPut( pp,ribbon,&flag );

    //TVトピック作成：リボンコレクター
    TVTOPIC_Entry_Record_Ribbon( sv, pp, ribbon );

    ++ct;
  }
  if( ct == 0){
    //みんなもう持ってた
    return FALSE;
  }
  return TRUE;
}
#endif

//--------------------------------------------------------------
/**
 *  @brief  連勝のご褒美リボンをもらえるかどうかのフラグを立てておく
 */
//--------------------------------------------------------------
#if 0
static u16 bswayscr_IfRenshouPrizeGet( BSUBWAY_SCRWORK *wk )
{
  u16 win;
  if(  wk->play_mode == BSWAY_MODE_RETRY ||
    wk->play_mode == BSWAY_MODE_SINGLE ||
    wk->play_mode == BSWAY_MODE_WIFI_MULTI ||
    wk->play_mode == BSWAY_MODE_WIFI ){
    return 0;
  }
  win = wk->renshou+wk->now_win;

  //50連勝以上でもらえる
  if( win < BSWAY_50_RENSHOU_CNT ){
    return 0;
  }
  //prize getフラグを立てておく
  wk->prize_f = 1;
  return 1;
}
#endif

//--------------------------------------------------------------
/**
 *  @brief  参加したポケモンのパラメータをB_TOWER_POKEMON型にパックする
 */
//--------------------------------------------------------------
static void bsw_PokeDataPack( BSUBWAY_POKEMON *dat, const POKEMON_PARAM *pp )
{
  int i;
  
  //mons_no/form_no/item_no
  dat->mons_no = PP_Get( pp,ID_PARA_monsno,NULL );
  dat->form_no = PP_Get( pp,ID_PARA_form_no,NULL );
  dat->item_no = PP_Get( pp,ID_PARA_item,NULL );
  
  //waza/pp_count
  for( i = 0;i < WAZA_TEMOTI_MAX;i++){
    dat->waza[i] = PP_Get( pp,ID_PARA_waza1+i,NULL );
    dat->pp_count |= ((PP_Get( pp, ID_PARA_pp_count1+i, NULL )) << (i*2));
  }
  //country,id,personal
  dat->country_code = PP_Get( pp,ID_PARA_country_code,NULL );
  dat->id_no = PP_Get( pp,ID_PARA_id_no,NULL );
  dat->personal_rnd = PP_Get( pp,ID_PARA_personal_rnd,NULL );

  //power_rnd
  dat->power_rnd = PP_Get( pp,ID_PARA_power_rnd,NULL );

  //exp
  for( i = 0;i < 6;i++){
    dat->exp[i] = PP_Get( pp,ID_PARA_hp_exp+i,NULL );
  }
  //tokusei,natukido
  dat->tokusei = PP_Get( pp,ID_PARA_speabino,NULL );
  dat->natuki = PP_Get( pp,ID_PARA_friend,NULL );
  
  //ニックネーム
  PP_Get( pp,ID_PARA_nickname_raw,dat->nickname );
}

//--------------------------------------------------------------
/**
 *  @brief  参加したポケモンのパラメータを保存する
 */
//--------------------------------------------------------------
static void bsw_SaveMemberPokeData(
    BSUBWAY_SCRWORK *wk, SAVE_CONTROL_WORK *sv, BSWAY_SCORE_POKE_DATA mode )
{
  int i = 0;
  BSUBWAY_POKEMON *dat;
  const POKEPARTY  *party;
  POKEMON_PARAM *pp;
  
  dat = GFL_HEAP_AllocMemoryLo( wk->heapID, sizeof(BSUBWAY_POKEMON)*3 );
  MI_CpuClear8( dat,sizeof(BSUBWAY_POKEMON)*3);

#if 0  
  party = SaveData_GetTemotiPokemon( sv );
#else
  party = BSUBWAY_SCRWORK_GetPokePartyUse( wk );
#endif

  for( i = 0;i < 3; i++ ){
    pp = PokeParty_GetMemberPointer( party, wk->member[i] );
    bsw_PokeDataPack( &(dat[i]), pp );  
  }
  
  BSUBWAY_SCOREDATA_SetUsePokeData( wk->scoreData,mode, dat );

  MI_CpuClear8( dat,sizeof( BSUBWAY_POKEMON )*3);
  GFL_HEAP_FreeMemory( dat );
}

//  フロンティアとフィールドで共通で使用するものを移動
//--------------------------------------------------------------
/**
 * バトルタワートレーナーの持ちポケモンのパワー乱数を決定する
 * @param  tr_no  トレーナーナンバー
 * @return  パワー乱数
 * b_tower_fld.c→b_tower_evに移動
 */
//--------------------------------------------------------------
#if 0
u8  BattleTowerPowRndGet( u16 tr_no )
{
  u8  pow_rnd;

  if( tr_no<100){
    pow_rnd=(0x1f/8)*1;
  }
  else if( tr_no<120){
    pow_rnd=(0x1f/8)*2;
  }
  else if( tr_no<140){
    pow_rnd=(0x1f/8)*3;
  }
  else if( tr_no<160){
    pow_rnd=(0x1f/8)*4;
  }
  else if( tr_no<180){
    pow_rnd=(0x1f/8)*5;
  }
  else if( tr_no<200){
    pow_rnd=(0x1f/8)*6;
  }
  else if( tr_no<220){
    pow_rnd=(0x1f/8)*7;
  }
  else{
    pow_rnd=0x1f;
  }
  return pow_rnd;
}
#endif

//--------------------------------------------------------------
/**
 * @brief  タワープレイ更新ランダムシード取得 ラウンド更新時に呼ぶ
 * b_tower_fld.c→b_tower_evに移動
 */
//--------------------------------------------------------------
#if 0
u16  btower_rand( BSUBWAY_SCRWORK *wk )
{
  //プラチナで追加されたWIFIマルチは通常のランダムを使用
  if( wk->play_mode == BSWAY_MODE_WIFI_MULTI ){
    return ( gf_rand() );
  }

  wk->play_rnd_seed = BtlTower_PlayFixRand( wk->play_rnd_seed );

#ifdef DEBUG_BSW_PRINT  
  KAGAYA_Printf("btower_rand = %d\n",wk->play_rnd_seed );
#endif
  return ( wk->play_rnd_seed/65535);
}
#endif
