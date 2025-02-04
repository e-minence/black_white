//======================================================================
/**
 * @file  bsubway_savedata.c
 * @brief  バトルサブウェイ　セーブデータ関連
 * @authaor  iwasawa
 * @date  2008.12.11
 * @note PLより移植 kagaya
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include <dpw_bt.h>

#include "savedata/bsubway_savedata.h"
#include "bsubway_savedata_local.h"
#include "../field/bsubway_scr_def.h"
#include "net/dwc_tool.h"

#if 0 //wb null
#ifdef _NITRO
// 構造体が想定のサイズとなっているかチェック
SDK_COMPILER_ASSERT(sizeof(BSUBWAY_LEADER_DATA) == 34);
SDK_COMPILER_ASSERT(sizeof(BSUBWAY_POKEMON) == 56);
SDK_COMPILER_ASSERT(sizeof(BSUBWAY_WIFI_PLAYER) == 228);
#endif
#endif

//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
/// バトルサブウェイプレイ進行データ
/// 挑戦開始時にクリアされる
/// size=20byte
//--------------------------------------------------------------
struct _BSUBWAY_PLAYDATA
{
  u16 itemfix_f  :1;  ///<固定アイテムかどうかのフラグ
  u16 saved_f    :1;  ///<セーブ済みかどうか
  u16 play_mode  :4;  ///<現在どこにチャレンジ中か?
  u16 partner    :3;  ///<現在誰と組んでいるか?
  u16 use_battle_box:1; ///<バトルボックスを使用するか
  u16 sel_wifi_dl_play:1; ///<wifiダウンロードプレイを行うか
  u16 padding_bit:5; ///<余り
  u8 round;  ///<バトルサブウェイ ラウンド数
  u8 wifi_rec_down;    ///<勝ち抜きまでに倒されたポケモン数
  u16 wifi_rec_turn;    ///<勝ち抜きにかかったターン数
  u16 wifi_rec_damage;  ///<勝ち抜きまでに受けたダメージ数
  
  u8 member_poke[BSUBWAY_STOCK_MEMBER_MAX];    ///<選択したポケモンの位置
  u16 trainer_no[BSUBWAY_STOCK_TRAINER_MAX];    ///<対戦トレーナーNo保存
  
  ///<AIマルチのペアのポケモン再生成に必要なパラメータ
  struct _BSUBWAY_PAREPOKE_PARAM  pare_poke;
};

//--------------------------------------------------------------
/// バトルサブウェイ　スコアデータ
/// size= 20+168+168=356byte
/// ゲームを通して保存&管理しておく、プレイヤー成績
//--------------------------------------------------------------
struct _BSUBWAY_SCOREDATA
{
  u16 btl_point;  ///<バトルポイント
  u8 wifi_lose;  ///<連続敗戦カウント
  u8 wifi_rank;  ///<WiFiランク
  
  u32  flags;
  
  //連勝記録 0 origin
  u16 renshou[BSWAY_PLAYMODE_MAX];
  u16 renshou_max[BSWAY_PLAYMODE_MAX];
  
  //ステージ数記録 1 origin 0 = error
  u16 stage[BSWAY_PLAYMODE_MAX];
  
  //WiFiチャレンジデータ
  u16  wifi_score;  ///<WiFi成績
  u8 debug_flag; ///<デバッグ用フラグ
  u8 padding;
  
  //WiFiポケモンデータストック
  struct _BSUBWAY_POKEMON  wifi_poke[BSUBWAY_STOCK_WIFI_MEMBER_MAX];
  //トレーナーロード用シングルデータストック
  struct _BSUBWAY_POKEMON  single_poke[BSUBWAY_STOCK_WIFI_MEMBER_MAX];
};

//======================================================================
//  proto
//======================================================================

//======================================================================
//  バトルサブウェイ　プレイデータ
//======================================================================
//--------------------------------------------------------------
/**
 * プレイデータサイズ
 * @param nothing
 * @retval int ワークサイズ
 */
//--------------------------------------------------------------
int BSUBWAY_PLAYDATA_GetWorkSize( void )
{
  return( sizeof(BSUBWAY_PLAYDATA) );
}

//--------------------------------------------------------------
/**
 * プレイデータ　初期化
 * @param bsw_play BSUBWAY_PLAYDATA
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_PLAYDATA_Init( BSUBWAY_PLAYDATA *bsw_play )
{
  MI_CpuClear8( bsw_play, sizeof(BSUBWAY_PLAYDATA) );
}

//--------------------------------------------------------------
/**
 * プレイデータ　セーブフラグセット
 * @param bsw_play BSUBWAY_PLAYDATA
 * @param flag TRUE=セーブあり FALSE=なし
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_PLAYDATA_SetSaveFlag( BSUBWAY_PLAYDATA *bsw_play, BOOL flag )
{
  bsw_play->saved_f = flag;
}

//--------------------------------------------------------------
/**
 *  プレイデータ　正しくセーブ済みかどうか？
 *  @retval  TRUE  正しくセーブされている
 *  @retval FALSE  セーブされていない
 */
//--------------------------------------------------------------
BOOL BSUBWAY_PLAYDATA_GetSaveFlag( BSUBWAY_PLAYDATA *bsw_play )
{
  return bsw_play->saved_f;
}

//--------------------------------------------------------------
/**
 * プレイデータ　取得
 * @param bsw_play BSUBWAY_PLAYDATA
 * @param id 取得するBSWAY_PLAYDATA_ID
 * @param buf データ取得ポインタ
 * @retval u32 IDから取得した結果
 */
//--------------------------------------------------------------
u32 BSUBWAY_PLAYDATA_GetData(
    const BSUBWAY_PLAYDATA *bsw_play, BSWAY_PLAYDATA_ID id, void *buf )
{
  switch( id )
  {
  case BSWAY_PLAYDATA_ID_playmode:
    return (u32)bsw_play->play_mode;
  case BSWAY_PLAYDATA_ID_round:
    return (u32)bsw_play->round;
  case BSWAY_PLAYDATA_ID_rec_down:
    return (u32)bsw_play->wifi_rec_down;
  case BSWAY_PLAYDATA_ID_rec_turn:
    return bsw_play->wifi_rec_turn;
  case BSWAY_PLAYDATA_ID_rec_damage:
    return bsw_play->wifi_rec_damage;
  case BSWAY_PLAYDATA_ID_pokeno:
    GF_ASSERT( buf != NULL );
    MI_CpuCopy8( bsw_play->member_poke, buf, BSUBWAY_STOCK_MEMBER_MAX );
    return 0;
  case BSWAY_PLAYDATA_ID_pare_poke:
    GF_ASSERT( buf != NULL );
    MI_CpuCopy8( &bsw_play->pare_poke, buf, sizeof(BSUBWAY_PAREPOKE_PARAM) );
    return 0;
  case BSWAY_PLAYDATA_ID_pare_itemfix:
    return bsw_play->itemfix_f;
  case BSWAY_PLAYDATA_ID_trainer:
    GF_ASSERT( buf != NULL );
    MI_CpuCopy8(bsw_play->trainer_no,buf, 2*BSUBWAY_STOCK_TRAINER_MAX );
    return 0;
  case BSWAY_PLAYDATA_ID_partner:
    return bsw_play->partner;
  case BSWAY_PLAYDATA_ID_use_battle_box:
    return bsw_play->use_battle_box;
  case BSWAY_PLAYDATA_ID_sel_wifi_dl_play:
    return bsw_play->sel_wifi_dl_play;
  default:
    GF_ASSERT( 0 );
  }
  
  return 0;
}

//--------------------------------------------------------------
/**
 * プレイデータ　設定
 * @param bsw_play BSUBWAY_PLAYDATA
 * @param id 取得するBSWAY_PLAYDATA_ID
 * @param buf 反映するデータポインタ
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_PLAYDATA_SetData(
    BSUBWAY_PLAYDATA *bsw_play, BSWAY_PLAYDATA_ID id, const void *buf )
{
  u32 *buf32 = (u32*)buf;
  u16 *buf16 = (u16*)buf;
  u8 *buf8 = (u8*)buf;
  
  GF_ASSERT( buf != NULL );
  
  switch(id){
  case BSWAY_PLAYDATA_ID_playmode:
    bsw_play->play_mode = buf8[0];
    break;
  case BSWAY_PLAYDATA_ID_round:
    bsw_play->round = buf8[0];
    break;
  case BSWAY_PLAYDATA_ID_rec_down:
    bsw_play->wifi_rec_down = buf8[0];
    break;
  case BSWAY_PLAYDATA_ID_rec_turn:
    bsw_play->wifi_rec_turn = buf16[0];
    break;
  case BSWAY_PLAYDATA_ID_rec_damage:
    bsw_play->wifi_rec_damage = buf16[0];
    break;
  case BSWAY_PLAYDATA_ID_pokeno:
    MI_CpuCopy8( buf8, bsw_play->member_poke, 4 );
    break;
  case BSWAY_PLAYDATA_ID_pare_poke:
    MI_CpuCopy8( buf16,
        &bsw_play->pare_poke, sizeof(BSUBWAY_PAREPOKE_PARAM) );
    break;
  case BSWAY_PLAYDATA_ID_pare_itemfix:
    bsw_play->itemfix_f = buf8[0];
    break;
  case BSWAY_PLAYDATA_ID_trainer:
    MI_CpuCopy8( buf16, bsw_play->trainer_no, 2*BSUBWAY_STOCK_TRAINER_MAX );
    break;
  case BSWAY_PLAYDATA_ID_partner:
    bsw_play->partner = buf8[0];
    break;
  case BSWAY_PLAYDATA_ID_use_battle_box:
    bsw_play->use_battle_box = buf8[0];
    break;
  case BSWAY_PLAYDATA_ID_sel_wifi_dl_play:
    bsw_play->sel_wifi_dl_play = buf8[0];
    break;
  default:
    GF_ASSERT( 0 );
  }
}

//--------------------------------------------------------------
/**
 *  @brief プレイデータ　WifiレコードデータAdd  
 *  @param  down  倒されたポケモン追加数
 *  @param  turn  かかったターン追加数
 *  @param  damage  受けたダメージ追加値
 */
//--------------------------------------------------------------
void BSUBWAY_PLAYDATA_AddWifiRecord(
    BSUBWAY_PLAYDATA *bsw_play, u8 down,u16 turn,u16 damage )
{
  if(bsw_play->wifi_rec_down + down < 255){
    bsw_play->wifi_rec_down += down;
  }
  if(bsw_play->wifi_rec_turn + turn < 65535){
    bsw_play->wifi_rec_turn += turn;
  }
  if(bsw_play->wifi_rec_damage + damage < 65535){
    bsw_play->wifi_rec_damage += damage;
  }
}

//--------------------------------------------------------------
/**
 * プレイデータ　ラウンド数リセット
 * @param bsw_play BSUBWAY_PLAYDATA
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_PLAYDATA_ResetRoundNo( BSUBWAY_PLAYDATA *bsw_play )
{
  u8 buf = 0;
  BSUBWAY_PLAYDATA_SetData( bsw_play, BSWAY_PLAYDATA_ID_round, &buf );
}

//--------------------------------------------------------------
/**
 * プレイデータ　ラウンド数増加
 * @param bsw_play BSUBWAY_PLAYDATA
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_PLAYDATA_IncRoundNo( BSUBWAY_PLAYDATA *bsw_play )
{
  u8 buf = BSUBWAY_PLAYDATA_GetData( 
      bsw_play, BSWAY_PLAYDATA_ID_round, NULL );
  if( buf < 0xff ){
    buf++;
  }
  KAGAYA_Printf( "BSW ラウンド数増加 %d\n", buf );
  BSUBWAY_PLAYDATA_SetData( bsw_play, BSWAY_PLAYDATA_ID_round, &buf );
}

//--------------------------------------------------------------
/**
 * プレイデータ　ラウンド数セット
 * @param bsw_play BSUBWAY_PLAYDATA
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_PLAYDATA_SetRoundNo( BSUBWAY_PLAYDATA *bsw_play, u8 round )
{
  BSUBWAY_PLAYDATA_SetData( bsw_play, BSWAY_PLAYDATA_ID_round, &round );
}

//--------------------------------------------------------------
/**
 * プレイデータ　ラウンド数取得
 * @param bsw_play BSUBWAY_PLAYDATA
 * @retval nothing
 */
//--------------------------------------------------------------
u16 BSUBWAY_PLAYDATA_GetRoundNo( const BSUBWAY_PLAYDATA *bsw_play )
{
  u16 buf = BSUBWAY_PLAYDATA_GetData( bsw_play, BSWAY_PLAYDATA_ID_round, NULL );
  return( buf );
}

//======================================================================
//  バトルサブウェイ　スコアデータ
//======================================================================
//--------------------------------------------------------------
/**
 * スコアデータサイズ
 * @param none
 * @retval int ワークサイズ
 */
//--------------------------------------------------------------
int BSUBWAY_SCOREDATA_GetWorkSize( void )
{
  return( sizeof(BSUBWAY_SCOREDATA) );
}

//--------------------------------------------------------------
/**
 * スコアデータ初期化
 * @param bsw_score BSUBWAY_SCOREDATA
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCOREDATA_Init( BSUBWAY_SCOREDATA *bsw_score )
{
  MI_CpuClear8( bsw_score, sizeof(BSUBWAY_SCOREDATA) );
  bsw_score->wifi_rank = 1;
}

//--------------------------------------------------------------
/**
 * スコアデータ　バトルポイント追加
 * @param bsw_score BSUBWAY_SCOREDATA
 * @param num セットする値
 * @retval u16 追加後のバトルポイント
 */
//--------------------------------------------------------------
void BSUBWAY_SCOREDATA_AddBattlePoint( BSUBWAY_SCOREDATA *bsw_score, u16 num )
{
  if( bsw_score->btl_point+num > 9999 ){
    bsw_score->btl_point = 9999;
  }else{
    bsw_score->btl_point += num;
  }
}

//--------------------------------------------------------------
/**
 * スコアデータ　バトルポイント減少
 * @param bsw_score BSUBWAY_SCOREDATA
 * @param num セットする値
 * @retval u16 追加後のバトルポイント
 */
//--------------------------------------------------------------
void BSUBWAY_SCOREDATA_SubBattlePoint( BSUBWAY_SCOREDATA *bsw_score, u16 num )
{
  if( (s16)((s16)bsw_score->btl_point - (s16)num) >= 0 ){
    bsw_score->btl_point -= num;
  }else{
    bsw_score->btl_point = 0;
  }
}

//--------------------------------------------------------------
/**
 * スコアデータ　バトルポイントセット
 * @param bsw_score BSUBWAY_SCOREDATA
 * @param num セットする値
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCOREDATA_SetBattlePoint( BSUBWAY_SCOREDATA *bsw_score, u16 num )
{
  if( num > 9999 ){
    bsw_score->btl_point = 9999;
  }else{
    bsw_score->btl_point = num;
  }
}

//--------------------------------------------------------------
/**
 * スコアデータ　バトルポイント取得
 * @param bsw_score BSUBWAY_SCOREDATA
 * @retval u16 バトルポイント
 */
//--------------------------------------------------------------
u16 BSUBWAY_SCOREDATA_GetBattlePoint( const BSUBWAY_SCOREDATA *bsw_score )
{
  return( bsw_score->btl_point );
}

//--------------------------------------------------------------
/**
 * 通信マルチ、スーパー通信マルチをマルチ、スーパーマルチに変換
 * @param
 * @retval
 */
//--------------------------------------------------------------
static BSWAY_PLAYMODE scoreData_ChgMultiMode( BSWAY_PLAYMODE mode )
{
#if 0 //元に戻りました。BTS4549 100609
  if( mode == BSWAY_PLAYMODE_COMM_MULTI ){
    mode = BSWAY_PLAYMODE_MULTI;
  }else if( mode == BSWAY_PLAYMODE_S_COMM_MULTI ){
    mode = BSWAY_PLAYMODE_S_MULTI;
  }
#endif
  return( mode );
}

//--------------------------------------------------------------
/**
 * スコアデータ　連勝数増加
 * @param bsw_score BSUBWAY_SCOREDATA
 * @param mode BSWAY_PLAYMODE
 * @retval nothing
 */
//--------------------------------------------------------------
#if 0
void BSUBWAY_SCOREDATA_IncRenshou(
    BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode )
{
  mode = scoreData_ChgMultiMode( mode );
  
  if( bsw_score->renshou[mode] < 65534 ){
    bsw_score->renshou[mode]++;
  }
}
#endif

//--------------------------------------------------------------
/**
 * スコアデータ　連勝数リセット
 * @param bsw_score BSUBWAY_SCOREDATA
 * @param mode BSWAY_PLAYMODE
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCOREDATA_ResetRenshou(
    BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode )
{
  mode = scoreData_ChgMultiMode( mode );
  bsw_score->renshou[mode] = 0;
}

//--------------------------------------------------------------
/**
 * スコアデータ　連勝数取得
 * @param bsw_score BSUBWAY_SCOREDATA
 * @param mode BSWAY_PLAYMODE
 * @retval u16 連勝数
 */
//--------------------------------------------------------------
u16 BSUBWAY_SCOREDATA_GetRenshou(
    const BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode )
{
  mode = scoreData_ChgMultiMode( mode );
  return( bsw_score->renshou[mode] );
}

//--------------------------------------------------------------
/**
 * スコアデータ、最大連勝記録を取得
 * @param bsw_score BSUBWAY_SCOREDATA
 * @retval u16
 */
//--------------------------------------------------------------
u16 BSUBWAY_SCOREDATA_GetRenshouMax(
    const BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode )
{
  mode = scoreData_ChgMultiMode( mode );
  return bsw_score->renshou_max[mode];
}

//--------------------------------------------------------------
/**
 * スコアデータ、最大連勝記録をセット
 * @param bsw_score BSUBWAY_SCOREDATA
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCOREDATA_SetRenshouMax(
    BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode, u16 count )
{
  mode = scoreData_ChgMultiMode( mode );
  bsw_score->renshou_max[mode] = count;
}

//--------------------------------------------------------------
/**
 * スコアデータ、最大連勝記録更新
 * @param bsw_score BSUBWAY_SCOREDATA
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCOREDATA_UpdateRenshouMax(
    BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode, u16 count )
{
  mode = scoreData_ChgMultiMode( mode );
  if( bsw_score->renshou_max[mode] < count ){
    bsw_score->renshou_max[mode] = count;
  }
}

//--------------------------------------------------------------
/**
 * スコアデータ、連勝数セット
 * @param bsw_score BSUBWAY_SCOREDATA
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCOREDATA_SetRenshou(
    BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode, u16 count )
{
  mode = scoreData_ChgMultiMode( mode );
  if( count > BSW_RENSHOU_DATA_MAX ){
    GF_ASSERT( 0 );
    count = BSW_RENSHOU_DATA_MAX;
  }
  bsw_score->renshou[mode] = count;
}

//--------------------------------------------------------------
/**
 * スコアデータ　ステージ数をエラーに
 * @param bsw_play BSUBWAY_SCOREDATA
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCOREDATA_ErrorStageNo(
    BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode )
{
  mode = scoreData_ChgMultiMode( mode );
  bsw_score->stage[mode] = 0;
}

//--------------------------------------------------------------
/**
 * スコアデータ　ステージ数初期化
 * @param bsw_play BSUBWAY_SCOREDATA
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCOREDATA_InitStageNo(
    BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode )
{
  mode = scoreData_ChgMultiMode( mode );
  bsw_score->stage[mode] = 1;
}

//--------------------------------------------------------------
/**
 * スコアデータ　ステージ数があるかチェック
 * @param bsw_play BSUBWAY_SCOREDATA
 * @retval BOOL
 */
//--------------------------------------------------------------
BOOL BSUBWAY_SCOREDATA_CheckExistStageNo(
    const BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode )
{
  mode = scoreData_ChgMultiMode( mode );
  
  if( bsw_score->stage[mode] == 0 ){
    return( FALSE );
  }
  return( TRUE );
}


//--------------------------------------------------------------
/**
 * スコアデータ　ステージ数増加
 * @param bsw_play BSUBWAY_SCOREDATA
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCOREDATA_IncStageNo(
    BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode )
{
  mode = scoreData_ChgMultiMode( mode );
  
  if( bsw_score->stage[mode] < BSW_STAGE_MAX ){ //-1=エラー判定用
    bsw_score->stage[mode]++;
  }
}

//--------------------------------------------------------------
/**
 * スコアデータ　ステージ数セット
 * @param bsw_play BSUBWAY_SCOREDATA
 * @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCOREDATA_SetStageNo_Org1(
    BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode, u16 stage )
{
  mode = scoreData_ChgMultiMode( mode );
  
  if( stage == 0 ){
    GF_ASSERT( 0 );
    stage = 1;
  }
  bsw_score->stage[mode] = stage;
}
//--------------------------------------------------------------
/**
 * スコアデータ　ステージ数取得 0origin
 * @param bsw_play BSUBWAY_SCOREDATA
 * @retval nothing
 */
//--------------------------------------------------------------
u16 BSUBWAY_SCOREDATA_GetStageNo_Org0(
    const BSUBWAY_SCOREDATA *bsw_score, BSWAY_PLAYMODE mode )
{
  mode = scoreData_ChgMultiMode( mode );
  
  {
    u16 stage = bsw_score->stage[mode];
    stage--;
  
    if( (s16)stage < 0 ){
      GF_ASSERT( 0 );
      stage = 0;
    }
  
    return( stage );
  }
}


//--------------------------------------------------------------
/**
 *  スコアデータ フラグエリアセット
 *  @param bsw_score BSUBWAY_SCOREDATA*
 *  @param id BSWAY_SCOREDATA_FLAG
 *  @param mode BSWAY_SETMODE
 *  @retval BOOL フラグの状態
 */
//--------------------------------------------------------------
BOOL BSUBWAY_SCOREDATA_SetFlag( BSUBWAY_SCOREDATA *bsw_score,
    BSWAY_SCOREDATA_FLAG id, BSWAY_SETMODE mode )
{
  u32 i;
  u32 flag = 1;
  
  //エラーチェック
  if( id >= BSWAY_SCOREDATA_FLAG_MAX ){
    GF_ASSERT( 0 );
    return 0;
  }
  
  //フラグID生成
  for( i = 0; i < id; i++ ){
    flag <<= 1;
  }
  
  switch( mode ){
  case BSWAY_SETMODE_reset:
    flag = (flag^0xFFFFFFFF);
    bsw_score->flags &= flag;
    break;
  case BSWAY_SETMODE_set:
    bsw_score->flags |= flag;
    break;
  case BSWAY_SETMODE_get:
    return (BOOL)((bsw_score->flags>>id)&0x0001);
  default:
    GF_ASSERT( 0 );
  }
  
  return 0;
}

//--------------------------------------------------------------
/**
 *  スコアデータ　Wifiランク増加
 *  @param bsw_score BSUBWAY_SCOREDATA*
 *  @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCOREDATA_IncWifiRank( BSUBWAY_SCOREDATA *bsw_score )
{
  if( bsw_score->wifi_rank < 10 ){
    bsw_score->wifi_rank++;
  }
}

//--------------------------------------------------------------
/**
 *  スコアデータ　Wifiランク減少
 *  @param bsw_score BSUBWAY_SCOREDATA*
 *  @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCOREDATA_DecWifiRank( BSUBWAY_SCOREDATA *bsw_score )
{
  if( bsw_score->wifi_rank > 1 ){
      bsw_score->wifi_rank--;
  }
}

//--------------------------------------------------------------
/**
 *  スコアデータ　Wifiランク取得
 *  @param bsw_score BSUBWAY_SCOREDATA*
 *  @retval u8 wifiランク
 */
//--------------------------------------------------------------
u8 BSUBWAY_SCOREDATA_GetWifiRank( const BSUBWAY_SCOREDATA *bsw_score )
{
  return( bsw_score->wifi_rank );
}

//--------------------------------------------------------------
/**
 *  スコアデータ　Wifiランクセット
 *  @param bsw_score BSUBWAY_SCOREDATA*
 *  @retval nothing
 */
//--------------------------------------------------------------
void BSUBWAY_SCOREDATA_SetWifiRank( BSUBWAY_SCOREDATA *bsw_score, u8 rank )
{
  if( (int)rank < 1 || (int)rank > 10 ){
    GF_ASSERT( 0 );
    rank = 1;
  }
  
  bsw_score->wifi_rank = rank;
}

//--------------------------------------------------------------
/**
 *  @brief  スコアデータ　Wifi連続敗戦カウント操作
 *  @return  操作後のカウント
 */
//--------------------------------------------------------------
u8 BSUBWAY_SCOREDATA_SetWifiLoseCount(
    BSUBWAY_SCOREDATA *bsw_score, BSWAY_SETMODE mode )
{
  BSWAY_SCOREDATA_FLAG flag = BSWAY_SCOREDATA_FLAG_WIFI_LOSE_F;

  switch(mode){
  case BSWAY_SETMODE_reset:
    BSUBWAY_SCOREDATA_SetFlag( bsw_score, flag, BSWAY_SETMODE_reset );
    bsw_score->wifi_lose = 0;
    break;
  case BSWAY_SETMODE_inc:
    if( BSUBWAY_SCOREDATA_SetFlag(bsw_score,flag,BSWAY_SETMODE_get) ){
      if( bsw_score->wifi_lose < 0xff ){
        bsw_score->wifi_lose++; //連続敗戦中
      }
    }else{
      BSUBWAY_SCOREDATA_SetFlag( bsw_score, flag, BSWAY_SETMODE_set );
      bsw_score->wifi_lose = 1;
    }
    break;
  }
  
  //get
  return bsw_score->wifi_lose;
}

//----------------------------------------------------------------------------
/**
 *  @brief  スコアデータ  使用ポケモンを設定
 *
 *  @param  bsw_score     スコアワーク
 *  @param  mode          設定するモード
 *  @param  poke          ポケモン情報(BSUBWAY_POKEMON * BSUBWAY_STOCK_WIFI_MEMBER_MAX　分のバッファが必要)
 */
//-----------------------------------------------------------------------------
void BSUBWAY_SCOREDATA_SetUsePokeData( BSUBWAY_SCOREDATA *bsw_score, BSWAY_SCORE_POKE_DATA mode, const BSUBWAY_POKEMON* poke_tbl )
{
  if( mode == BSWAY_SCORE_POKE_SINGLE ){
    GFL_STD_MemCopy( poke_tbl, bsw_score->single_poke, sizeof(BSUBWAY_POKEMON)*BSUBWAY_STOCK_WIFI_MEMBER_MAX );
  }else{
    GFL_STD_MemCopy( poke_tbl, bsw_score->wifi_poke, sizeof(BSUBWAY_POKEMON)*BSUBWAY_STOCK_WIFI_MEMBER_MAX );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  スコアデータ  使用ポケモンを取得
 *
 *  @param  bsw_score     スコアワーク
 *  @param  mode          設定するモード
 *  @param  poke          ポケモン情報(BSUBWAY_POKEMON * 3　分のバッファが必要)
 */
//-----------------------------------------------------------------------------
void BSUBWAY_SCOREDATA_GetUsePokeData( const BSUBWAY_SCOREDATA *bsw_score, BSWAY_SCORE_POKE_DATA mode, BSUBWAY_POKEMON* poke_tbl )
{
  if( mode == BSWAY_SCORE_POKE_SINGLE ){
    GFL_STD_MemCopy( bsw_score->single_poke, poke_tbl, sizeof(BSUBWAY_POKEMON)*BSUBWAY_STOCK_WIFI_MEMBER_MAX );
  }else{
    GFL_STD_MemCopy( bsw_score->wifi_poke, poke_tbl, sizeof(BSUBWAY_POKEMON)*BSUBWAY_STOCK_WIFI_MEMBER_MAX );
  }
}


//----------------------------------------------------------------------------
/**
 *  @brief  スコアデータ Wifi成績操作
 *
 *  @param  bsw_score   スコアワーク
 *  @param  bsw_play    プレイワーク
 */
//-----------------------------------------------------------------------------
void BSUBWAY_SCOREDATA_SetWifiScore( BSUBWAY_SCOREDATA *bsw_score, const BSUBWAY_PLAYDATA *bsw_play )
{
  u16  sa,sb,sc,sd,st;
  u16  score = 0;

  //ラウンド数は勝ち抜き数+1になっているのでマイナス１して計算する
  sa = (bsw_play->round-1)*1000;
  sb = bsw_play->wifi_rec_turn*10;
  sc = bsw_play->wifi_rec_down*20;
  
  if(sb+sc > 950){
    st = 0;
  }else{
    st = 950-(sb+sc);
  }
  
  if(bsw_play->wifi_rec_damage>(1000-30)){
    sd = 0;
  }else{
    sd = (1000-bsw_play->wifi_rec_damage)/30;
  }
  
  score = sa+st+sd;
  bsw_score->wifi_score = score;
}

//----------------------------------------------------------------------------
/**
 *  @brief  スコアデータ　Wifi成績0クリア
 *
 *  @param  bsw_score   ワーク
 */
//-----------------------------------------------------------------------------
void BSUBWAY_SCOREDATA_ClearWifiScore( BSUBWAY_SCOREDATA *bsw_score )
{
  bsw_score->wifi_score = 0;
}

//----------------------------------------------------------------------------
/**
 *  @brief  スコアデータ　Wifi成績取得
 *
 *  @param  bsw_score   ワーク
 *
 *  @return Wifi成績
 */
//-----------------------------------------------------------------------------
u16 BSUBWAY_SCOREDATA_GetWifiScore( const BSUBWAY_SCOREDATA *bsw_score )
{
  return bsw_score->wifi_score;
}

//----------------------------------------------------------------------------
/**
 *  @brief  スコアデータ  Wifi成績から　勝ち抜きすうを取得
 *
 *  @param  bsw_score   ワーク
 *
 *  @return 勝ち抜きすう
 */
//-----------------------------------------------------------------------------
u8 BSUBWAY_SCOREDATA_GetWifiNum( const BSUBWAY_SCOREDATA *bsw_score )
{
  u8 ret = 0;
  ret = (bsw_score->wifi_score)/1000;
  return ret;
}

//--------------------------------------------------------------
/**
 * スコアデータ　デバッグ用フラグ取得
 * @param
 * @retval
 */
//--------------------------------------------------------------
u8 BSUBWAY_SCOREDATA_DEBUG_GetFlag( const BSUBWAY_SCOREDATA *bsw_score )
{
#ifdef PM_DEBUG
  return( bsw_score->debug_flag );
#else
  return( 0 ); //ROM版の際は全てフラグオフ
#endif
}

//--------------------------------------------------------------
/**
 * スコアデータ　デバッグ用フラグセット
 * @param
 * @retval
 */
//--------------------------------------------------------------
void BSUBWAY_SCOREDATA_DEBUG_SetFlag( BSUBWAY_SCOREDATA *bsw_score, u8 flag )
{
#ifdef PM_DEBUG
  bsw_score->debug_flag = flag;
#else
  //ROM版の際は無効
#endif
}

//======================================================================
//  BSUBWAY_WIFIDATA
//======================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  BSUBWAY_WIFIDATAのサイズを取得
 *  @return サイズ
 */ 
//-----------------------------------------------------------------------------
int BSUBWAY_WIFIDATA_GetWorkSize( void )
{
  return sizeof(BSUBWAY_WIFI_DATA);
}

//----------------------------------------------------------------------------
/**
 *  @brief  WiFiデータ  初期化
 *
 *  @param  bsw_wifi  WiFiデータ
 */
//-----------------------------------------------------------------------------
void BSUBWAY_WIFIDATA_Init( BSUBWAY_WIFI_DATA *bsw_wifi )
{
  GFL_STD_MemClear( bsw_wifi, sizeof(BSUBWAY_WIFI_DATA) );
}

//----------------------------------------------------------------------------
/**
 *  @brief  WiFiデータ  指定ルームデータの取得フラグを設定する。
 *
 *  @param  bsw_wifi  WiFiデータ  
 *  @param  rank      ランク（1オリジン）
 *  @param  room      ルーム(1オリジン)
 *  @param  day       受信時間
 */
//-----------------------------------------------------------------------------
void BSUBWAY_WIFIDATA_SetRoomDataFlag( BSUBWAY_WIFI_DATA *bsw_wifi, u8 rank, u8 room, const RTCDate *day )
{
  u8  idx,ofs;
  u8  flag = 1;
  u16  roomid;
  
  if(room == 0 || room > 200){
    return;
  }
  if(rank == 0 || rank > 10){
    return;
  }
  //両方1オリジンなので-1して計算
  roomid = (rank-1)*200+(room-1);
  
  idx = roomid/8;
  ofs = roomid%8;
  flag <<= ofs;

  bsw_wifi->flags[idx] |= flag;

  bsw_wifi->day = GFDATE_RTCDate2GFDate(day);
}

//----------------------------------------------------------------------------
/**
 *  @brief  ルームデータ取得フラグをクリアする
 *
 *  @param  bsw_wifi  
 */
//-----------------------------------------------------------------------------
void BSUBWAY_WIFIDATA_ClearRoomDataFlag( BSUBWAY_WIFI_DATA *bsw_wifi )
{
  GFL_STD_MemClear(bsw_wifi->flags,BSUBWAY_ROOM_DATA_FLAGS_LEN);
  GFL_STD_MemClear(&bsw_wifi->day,sizeof(GFDATE));
}

//--------------------------------------------------------------
/**
 *  @brief  日付が変わっているかどうかチェック
 */
//--------------------------------------------------------------
static BOOL check_day(const RTCDate* new,const RTCDate* old)
{
  if(new->year > old->year){
    return TRUE;
  }
  if(new->month > old->month){
    return TRUE;
  }
  if(new->day > old->day){
    return TRUE;
  }
  return FALSE;
}
//----------------------------------------------------------------------------
/**
 *  @brief  指定ルームの情報がダウンロードされているかチェック
 *
 *  @param  bsw_wifi    ワーク
 *  @param  rank        ランク
 *  @param  room        ルームナンバー
 *  @param  day         日付
 *
 *  *日付が変わっているとフラグをオールクリアする。
 *
 *  @retval TRUE    受信されている
 *  @retval FALSE   受信されていない
 */
//-----------------------------------------------------------------------------
BOOL BSUBWAY_WIFIDATA_CheckRoomDataFlag( BSUBWAY_WIFI_DATA *bsw_wifi, u8 rank, u8 room, const RTCDate *day )
{
  u8  idx,ofs;
  u8  flag = 1;
  u16  roomid;
  RTCDate old_day;
  
  if(room > 200 || rank > 10){
    return FALSE;
  }

  //最後にDLした日付から、日が変わっているかどうかチェック
  GFDATE_GFDate2RTCDate(bsw_wifi->day,&old_day);
  if(check_day(day,&old_day)){
    //日が変わっているので、フラグ群をオールクリア
    BSUBWAY_WIFIDATA_ClearRoomDataFlag(bsw_wifi);
    return FALSE;
  }
  //両方1オリジンなので-1して計算
  roomid = (rank-1)*200+(room-1);
  
  idx = roomid/8;
  ofs = roomid%8;
  flag <<= ofs;

  if(bsw_wifi->flags[idx] & flag){
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  プレイヤーデータが存在しているかチェック
 *
 *  @param  bsw_wifi  ワーク
 *
 *  @retval TRUE    存在している
 *  @retval FALSE   存在していない
 */
//-----------------------------------------------------------------------------
BOOL BSUBWAY_WIFIDATA_CheckPlayerDataEnable( const BSUBWAY_WIFI_DATA *bsw_wifi )
{
  return bsw_wifi->player_data_f;
}

//----------------------------------------------------------------------------
/**
 *  @brief  リーダーデータが存在しているかチェック
 *
 *  @param  bsw_wifi  ワーク
 *
 *  @retval TRUE    存在している
 *  @retval FALSE   存在していない
 */
//-----------------------------------------------------------------------------
BOOL BSUBWAY_WIFIDATA_CheckLeaderDataEnable( const BSUBWAY_WIFI_DATA *bsw_wifi )
{
  return bsw_wifi->leader_data_f;
}

//----------------------------------------------------------------------------
/**
 *  @brief  プレイヤー情報を保存
 *
 *  @param  bsw_wifi    ワーク
 *  @param  dat         プレイヤー情報テーブル
 *  @param  rank        ランク
 *  @param  room        部屋No
 */
//-----------------------------------------------------------------------------
void BSUBWAY_WIFIDATA_SetPlayerData( BSUBWAY_WIFI_DATA *bsw_wifi, const BSUBWAY_WIFI_PLAYER* dat, u8 rank, u8 room )
{
  GFL_STD_MemCopy( dat, bsw_wifi->player,
    sizeof(BSUBWAY_WIFI_PLAYER)*BSUBWAY_STOCK_WIFI_PLAYER_MAX );

#ifdef DEBUG_ONLY_FOR_tomoya_takahashi
  {
    int i;

    for( i=0; i<BSUBWAY_STOCK_WIFI_PLAYER_MAX; i++ ){
      TOMOYA_Printf( "idx %d trtype %d\n", bsw_wifi->player[i].tr_type );
    }
  }
#endif
  
  //roomnoとrankを保存
  bsw_wifi->player_rank = rank;
  bsw_wifi->player_room = room;
  bsw_wifi->player_data_f = TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  プレイヤー情報をクリア
 *
 *  @param  bsw_wifi    ワーク
 */
//-----------------------------------------------------------------------------
void BSUBWAY_WIFIDATA_ClearPlayerData( BSUBWAY_WIFI_DATA *bsw_wifi )
{
  GFL_STD_MemClear( bsw_wifi->player,
    sizeof(BSUBWAY_WIFI_PLAYER)*BSUBWAY_STOCK_WIFI_PLAYER_MAX );
  bsw_wifi->player_data_f = FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  プレイヤー情報　ランク取得
 */
//-----------------------------------------------------------------------------
u8 BSUBWAY_WIFIDATA_GetPlayerRank( const BSUBWAY_WIFI_DATA *bsw_wifi )
{
  GF_ASSERT( bsw_wifi->player_data_f );
  return bsw_wifi->player_rank;
}

//----------------------------------------------------------------------------
/**
 *  @brief  プレイヤー情報　部屋No取得
 */
//-----------------------------------------------------------------------------
u8 BSUBWAY_WIFIDATA_GetPlayerRoomNo( const BSUBWAY_WIFI_DATA *bsw_wifi )
{
  GF_ASSERT( bsw_wifi->player_data_f );
  return bsw_wifi->player_room;
}

//----------------------------------------------------------------------------
/**
 *  @brief  プレイヤー情報　バトル用パラメータ取得
 */
//-----------------------------------------------------------------------------
void BSUBWAY_WIFIDATA_GetBtlPlayerData( const BSUBWAY_WIFI_DATA *bsw_wifi, BSUBWAY_PARTNER_DATA *player, u8 round, HEAPID heapID )
{
  BSUBWAY_TRAINER  *tr;      //トレーナーデータ
  BSUBWAY_POKEMON  *poke;    //持ちポケモンデータ
  const BSUBWAY_WIFI_PLAYER* src;
  GFL_MSGDATA* msgdata;

  tr = &(player->bt_trd);
  poke = player->btpwd;
  src = &(bsw_wifi->player[round]);

  //トレーナーパラメータ取得
  tr->player_id = BSP_TRAINERID_SUBWAY;//サブウェイ用IDは固定値
  tr->tr_type = src->tr_type;
  GFL_STD_MemCopy(src->name,tr->name,16);
  GFL_STD_MemCopy(src->appear_word,tr->appear_word,8);
  GFL_STD_MemCopy(src->win_word,tr->win_word,8);
  GFL_STD_MemCopy(src->lose_word,tr->lose_word,8);

  //ポケモンデータ取得
  MI_CpuCopy8(src->poke,poke,sizeof(BSUBWAY_POKEMON)*3);
}


//----------------------------------------------------------------------------
/**
 *  @brief  リーダー情報の設定
 *
 *  @param  bsw_wifi  ワーク
 *  @param  dat       リーダー情報テーブル
 *  @param  rank      ランク
 *  @param  room      部屋ナンバー
 */
//-----------------------------------------------------------------------------
void BSUBWAY_WIFIDATA_SetLeaderData( BSUBWAY_WIFI_DATA *bsw_wifi, const BSUBWAY_LEADER_DATA* dat, u8 rank, u8 room )
{
  GFL_STD_MemCopy( dat, &bsw_wifi->leader,
    sizeof(BSUBWAY_LEADER_DATA)*BSUBWAY_STOCK_WIFI_LEADER_MAX );
  
  //roomnoとrankを保存
  bsw_wifi->leader_rank = rank;
  bsw_wifi->leader_room = room;
  bsw_wifi->leader_data_f = TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  リーダー情報のクリア
 *
 *  @param  bsw_wifi  ワーク
 */
//-----------------------------------------------------------------------------
void BSUBWAY_WIFIDATA_ClearLeaderData( BSUBWAY_WIFI_DATA *bsw_wifi )
{
  MI_CpuClear8(&bsw_wifi->leader,
    sizeof(BSUBWAY_LEADER_DATA)*BSUBWAY_STOCK_WIFI_LEADER_MAX);
  bsw_wifi->leader_data_f = FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  リーダー情報のランク取得
 *
 *  @param  bsw_wifi  ワーク
 *
 *  @return ランク
 */
//-----------------------------------------------------------------------------
u8 BSUBWAY_WIFIDATA_GetLeaderRank( const BSUBWAY_WIFI_DATA *bsw_wifi )
{
  GF_ASSERT( bsw_wifi->leader_data_f );
  return bsw_wifi->leader_rank;
}

//----------------------------------------------------------------------------
/**
 *  @brief  リーダー情報の部屋ナンバー取得
 *
 *  @param  bsw_wifi  ワーク
 *  
 *  @return 部屋ナンバー
 */
//-----------------------------------------------------------------------------
u8 BSUBWAY_WIFIDATA_GetLeaderRoomNo( const BSUBWAY_WIFI_DATA *bsw_wifi )
{
  GF_ASSERT( bsw_wifi->leader_data_f );
  return bsw_wifi->leader_room;
}

//----------------------------------------------------------------------------
/**
 *  @brief  リーダー情報の取得
 *
 *  @param  bsw_wifi  ワーク
 *  @param  heapID    ヒープID
 *
 *  @return メモリ確保して情報をコピーしたワーク
 */
//-----------------------------------------------------------------------------
BSUBWAY_LEADER_DATA* BSUBWAY_WIFIDATA_GetLeaderDataAlloc( const BSUBWAY_WIFI_DATA *bsw_wifi, HEAPID heapID )
{
  BSUBWAY_LEADER_DATA* p_dat;

  p_dat = GFL_HEAP_AllocClearMemory( heapID, sizeof(BSUBWAY_LEADER_DATA)*BSUBWAY_STOCK_WIFI_LEADER_MAX );

  GFL_STD_MemCopy(bsw_wifi->leader, p_dat, sizeof(BSUBWAY_LEADER_DATA)*BSUBWAY_STOCK_WIFI_LEADER_MAX);
  return p_dat;
}

//----------------------------------------------------------------------------------
/**
 * @brief 歴代トレーナーデータの格納されている人数を返す
 *
 * @param   bsw_leader    
 *
 * @retval  int   トレーナーデータの入っている人数（最大３０人）
 */
//----------------------------------------------------------------------------------
int BSUBWAY_LEADERDATA_GetDataNum( const BSUBWAY_LEADER_DATA *bsw_leader )
{
  int i,result=0;
  for(i=0;i<BSUBWAY_STOCK_WIFI_LEADER_MAX;i++)
  {
    // 名前の１文字目が０じゃ無かったら存在している事にする
    if(bsw_leader[i].name[0]!=0)
    {
      result++;
    }
  }

  return result;
}

//----------------------------------------------------------------------------
/**
 *  @brief  リーダー情報からプレイヤーIDをu32で取得
 *  @param  pLeader   BSUBWAY_LEADER_DATA*
 *  @retval u32 プレイヤーID
 */
//-----------------------------------------------------------------------------
u32 BSUBWAY_LEADERDATA_GetPlayerID( const BSUBWAY_LEADER_DATA *pLeader )
{
  u32 id;
#if 0 //BTS6821
  //id_noを型キャストで４バイトデータとして読み取ると、
  //BSUBWAY_LEADER_DATAのサイズとid_noの位置から
  //アドレスによっては４バイトアライメントの境界により
  //上位２バイトのデータが失われる場合がある
  id = *(u32*)pLeader->id_no;
#else
  //型キャストせず１バイト単位で正しく生成
  id = (u32)((pLeader->id_no[0])|(pLeader->id_no[1]<<8)|
      (pLeader->id_no[2]<<16)|(pLeader->id_no[3]<<24));
#endif
  return( id );
}

//--------------------------------------------------------------
//  debug
//--------------------------------------------------------------
#ifdef PM_DEBUG
// ダミー情報を設定
void DEBUG_BSUBWAY_WIFIDATA_SetPlayerData( BSUBWAY_WIFI_DATA *bsw_wifi, u8 rank, u8 room )
{
  //roomnoとrankを保存
  bsw_wifi->player_rank = rank;
  bsw_wifi->player_room = room;
  bsw_wifi->player_data_f = TRUE;
}

void DEBUG_BSUBWAY_WIFIDATA_SetLeaderData( BSUBWAY_WIFI_DATA *bsw_wifi, u8 rank, u8 room )
{
  //roomnoとrankを保存
  bsw_wifi->leader_rank = rank;
  bsw_wifi->leader_room = room;
  bsw_wifi->leader_data_f = TRUE;
}
#endif

//======================================================================
//  wb null
//======================================================================
//----
#if 0 //wb null
//----
//--------------------------------------------------------------
/**
 *  @brief  サブウェイ　ポケモンデータサイズ
 */
//--------------------------------------------------------------
int BSUBWAY_WifiPoke_GetWorkSize(void)
{
  return sizeof(BSUBWAY_POKEMON);
}

//--------------------------------------------------------------
/**
 *  @brief  サブウェイ　Wifiデータサイズ
 */
//--------------------------------------------------------------
int  BSUBWAY_WifiData_GetWorkSize(void)
{
  return sizeof(BSUBWAY_WIFI_DATA);
}

//--------------------------------------------------------------
/**
 *  @brief　サブウェイ Wifi/トレーナーロードプレイヤーデータ構造体データサイズ
 */
//--------------------------------------------------------------
int BSUBWAY_DpwBtPlayer_GetWorkSize(void)
{
  return sizeof(Dpw_Bt_Player);
}

//--------------------------------------------------------------
/**
 *  @brief  サブウェイ　プレイデータクリア
 */
//--------------------------------------------------------------
void BSUBWAY_PlayData_Clear(BSUBWAY_PLAYWORK* dat)
{
  MI_CpuClear8(dat,sizeof(BSUBWAY_PLAYWORK));
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
}

//--------------------------------------------------------------
/**
 *  @brief  サブウェイ  スコアデータクリア
 */
//--------------------------------------------------------------
void BSUBWAY_ScoreData_Clear(BSUBWAY_SCOREWORK* dat)
{
  MI_CpuClear8(dat,sizeof(BSUBWAY_SCOREWORK));
  dat->wifi_rank = 1;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
}

//--------------------------------------------------------------
/**
 *  @brief  サブウェイ　プレイヤーメッセージデータクリア
 */
//--------------------------------------------------------------
void BSUBWAY_PlayerMsg_Clear(BSUBWAY_PLAYER_MSG* dat)
{
#if 0 //wb
  PMSDAT_SetupDefaultBattleTowerMessage(&dat->msg[0], BSWAY_MSG_PLAYER_READY);
  PMSDAT_SetupDefaultBattleTowerMessage(&dat->msg[1], BSWAY_MSG_PLAYER_WIN);
  PMSDAT_SetupDefaultBattleTowerMessage(&dat->msg[2], BSWAY_MSG_PLAYER_LOSE);
  PMSDAT_SetupDefaultBattleTowerMessage(&dat->msg[3], BSWAY_MSG_LEADER);
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
#endif
}

//--------------------------------------------------------------
/**
 *  @brief  サブウェイ　Wifiデータクリア
 */
//--------------------------------------------------------------
void BSUBWAY_WifiData_Clear(BSUBWAY_WIFI_DATA* dat)
{
  MI_CpuClear8(dat,sizeof(BSUBWAY_WIFI_DATA));
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
}

//======================================================================
//　サブウェイ　プレイデータアクセス系
//======================================================================

//======================================================================
//　サブウェイ　スコアデータアクセス系
//======================================================================
//--------------------------------------------------------------
/**
 *  @brief  スコアデータ　Wifi連続敗戦カウント操作
 *  有効コマンド get/reset/inc
 *  @return  操作後のカウント
 */
//--------------------------------------------------------------
u8 BSUBWAY_ScoreData_SetWifiLoseCount(BSUBWAY_SCOREWORK* dat,BSWAY_DATA_SETID mode)
{
  switch(mode){
  case BSWAY_DATA_reset:
    dat->wifi_lose = 0;
    dat->wifi_lose_f = 0;
    break;
  case BSWAY_DATA_inc:
    if(dat->wifi_lose_f){
      //連続敗戦中
      dat->wifi_lose += 1;
    }else{
      dat->wifi_lose = 1;
      dat->wifi_lose_f = 1;
    }
    break;
  }
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
  return dat->wifi_lose;
}

//--------------------------------------------------------------
/**
 *  @brief  使用ポケモンデータ保存
 */
//--------------------------------------------------------------
void BSUBWAY_ScoreData_SetUsePokeData(BSUBWAY_SCOREWORK* dat,
        BSWAY_SCORE_POKE_DATA mode,BSUBWAY_POKEMON* poke)
{
  if(mode == BSWAY_SCORE_POKE_SINGLE){
    MI_CpuCopy8(poke,dat->single_poke,sizeof(BSUBWAY_POKEMON)*3);
  }else{
    MI_CpuCopy8(poke,dat->wifi_poke,sizeof(BSUBWAY_POKEMON)*3);
  }
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
}

//--------------------------------------------------------------
/**
 *  @brief  使用ポケモンデータをバッファにコピーして取得
 */
//--------------------------------------------------------------
void BSUBWAY_ScoreData_GetUsePokeData(BSUBWAY_SCOREWORK* dat,
      BSWAY_SCORE_POKE_DATA mode,BSUBWAY_POKEMON* poke)
{
  if(mode == BSWAY_SCORE_POKE_SINGLE){
    MI_CpuCopy8(dat->single_poke,poke,sizeof(BSUBWAY_POKEMON)*3);
  }else{
    MI_CpuCopy8(dat->wifi_poke,poke,sizeof(BSUBWAY_POKEMON)*3);
  }
}

//--------------------------------------------------------------
/**
 *  @brief  使用ポケモンデータをバッファにコピーして取得(WiFi構造体データ型)
 */
//--------------------------------------------------------------
void BSUBWAY_ScoreData_GetUsePokeDataDpw(BSUBWAY_SCOREWORK* dat,
      BSWAY_SCORE_POKE_DATA mode,DPW_BT_POKEMON_DATA* poke)
{
  if(mode == BSWAY_SCORE_POKE_SINGLE){
    MI_CpuCopy8(dat->single_poke,poke,sizeof(BSUBWAY_POKEMON)*3);
  }else{
    MI_CpuCopy8(dat->wifi_poke,poke,sizeof(BSUBWAY_POKEMON)*3);
  }
}

//--------------------------------------------------------------
/**
 *  @brief  スコアデータ Wifi成績操作
 */
//--------------------------------------------------------------
u16  BSUBWAY_ScoreData_SetWifiScore(BSUBWAY_SCOREWORK* dat,BSUBWAY_PLAYWORK *playdata)
{
  u16  sa,sb,sc,sd,st;
  u16  score = 0;

  //ラウンド数は勝ち抜き数+1になっているのでマイナス１して計算する
  sa = (playdata->round-1)*1000;
  sb = playdata->wifi_rec_turn*10;
  sc = playdata->wifi_rec_down*20;
  if(sb+sc > 950){
    st = 0;
  }else{
    st = 950-(sb+sc);
  }
  if(playdata->wifi_rec_damage>(1000-30)){
    sd = 0;
  }else{
    sd = (1000-playdata->wifi_rec_damage)/30;
  }
  score = sa+st+sd;
  dat->wifi_score = score;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
  return score;
}

//--------------------------------------------------------------
/**
 *  @brief  スコアデータ　Wifi成績0クリア
 */
//--------------------------------------------------------------
void BSUBWAY_ScoreData_ClearWifiScore(BSUBWAY_SCOREWORK* dat)
{
  dat->wifi_score = 0;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
}

//--------------------------------------------------------------
/**
 *  @brief  スコアデータ　Wifi成績取得
 */
//--------------------------------------------------------------
u16  BSUBWAY_ScoreData_GetWifiScore(BSUBWAY_SCOREWORK* dat)
{
  return dat->wifi_score;
}

//--------------------------------------------------------------
/**
 *  @brief  スコアデータから勝ち抜いた数を取得
 */
//--------------------------------------------------------------
u8  BSUBWAY_ScoreData_GetWifiWinNum(BSUBWAY_SCOREWORK* dat)
{
  u8 ret = 0;
  ret = (dat->wifi_score)/1000;
  return ret;
}

//--------------------------------------------------------------
/**
 *  @brief  スコアデータ　周回数操作
 *  有効コマンド reset/inc/get
 */
//--------------------------------------------------------------

//--------------------------------------------------------------
/**
 *  @brief  現在のステージ値をセットする
 */
//--------------------------------------------------------------
u16 BSUBWAY_ScoreData_SetStageValue(BSUBWAY_SCOREWORK* dat,u16 id,u16 value)
{
  u16 id2;
  
  dat->tower_stage[id2] = value;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
  return dat->tower_stage[id2];
}

//--------------------------------------------------------------
/**
 *  @brief  デバッグ限定　現在のステージ値をセットする
 */
//--------------------------------------------------------------
#ifdef PM_DEBUG
u16 BSUBWAY_ScoreData_DebugSetStageValue(
    BSUBWAY_SCOREWORK* dat,u16 id,u16 value)
{
  return BSUBWAY_ScoreData_SetStageValue( dat, id, value );
}
#endif  //PM_DEBUG
  
//--------------------------------------------------------------
/**
 *  @brief  スコアデータ フラグエリアセット
 */
//--------------------------------------------------------------
BOOL  BSUBWAY_ScoreData_SetFlags(
    BSUBWAY_SCOREWORK* dat,u16 id,BSWAY_DATA_SETID mode)
{
  u16  i;
  u16  flag = 1;

  //エラーチェック
  if( id >= BSWAY_SFLAG_MAX ){
    GF_ASSERT( (0) && "TowerScoreData_SetFlags IDが不正です！" );
    return 0;
  }

  //フラグID生成
  for(i = 0;i < id;i++){
    flag <<= 1;
  }
  switch(mode){
  case BSWAY_DATA_reset:
    flag = (flag^0xFFFF);
    dat->flags &= flag;
    break;
  case BSWAY_DATA_set:
    dat->flags |= flag;
    break;
  case BSWAY_DATA_get:
    return (BOOL)((dat->flags>>id)&0x0001);
    
  }
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
  return 0;
}

//======================================================================
//　サブウェイ　プレイヤーメッセージデータアクセス系
//======================================================================
//--------------------------------------------------------------
/**
 *  @brief  簡易会話データをセット
 */
//--------------------------------------------------------------
#if 0 //wb
void BSUBWAY_PlayerMsg_Set(SAVEDATA* sv,BSWAY_PLAYER_MSG_ID id,PMS_DATA* src)
{
  FRONTIER_SAVEWORK* data = SaveData_Get(sv,GMDATA_ID_FRONTIER);
  
  PMSDAT_Copy(&(data->tower.player_msg.msg[id]),src);
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
}
#endif

//--------------------------------------------------------------
/**
 *  @brief  簡易会話データを取得
 */
//--------------------------------------------------------------
#if 0 //wb
PMS_DATA* BSUBWAY_PlayerMsg_Get(SAVEDATA* sv,BSWAY_PLAYER_MSG_ID id)
{
  FRONTIER_SAVEWORK* data = SaveData_Get(sv,GMDATA_ID_FRONTIER);
  
  return &(data->tower.player_msg.msg[id]);
}
#endif

//======================================================================
//　サブウェイ　Wifiデータアクセス系
//======================================================================
#if 0 //wb
//--------------------------------------------------------------
/**
 *  @brief  指定したルームデータの取得フラグを立てる
 *
 *  @param  rank  1オリジンなので注意
 *  @param  roomno  1オリジンなので注意
 */
//--------------------------------------------------------------
void BSUBWAY_WifiData_SetRoomDataFlag(BSUBWAY_WIFI_DATA* dat,
    u8 rank,u8 roomno,RTCDate *day)
{
  u8  idx,ofs;
  u8  flag = 1;
  u16  roomid;
  
  if(roomno == 0 || roomno > 200){
    return;
  }
  if(rank == 0 || rank > 10){
    return;
  }
  //両方1オリジンなので-1して計算
  roomid = (rank-1)*200+(roomno-1);
  
  idx = roomid/8;
  ofs = roomid%8;
  flag <<= ofs;

  dat->flags[idx] |= flag;

  dat->day = RTCDate2GFDate(day);
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
}

//--------------------------------------------------------------
/**
 *  @brief  ルームデータ取得フラグをクリアする
 */
//--------------------------------------------------------------
void BSUBWAY_WifiData_ClearRoomDataFlag(BSUBWAY_WIFI_DATA* dat)
{
  MI_CpuClear8(dat->flags,BSUBWAY_ROOM_DATA_FLAGS_LEN);
  MI_CpuClear8(&dat->day,sizeof(GFDATE));
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
}

//--------------------------------------------------------------
/**
 *  @brief  日付が変わっているかどうかチェック
 */
//--------------------------------------------------------------
static BOOL check_day(RTCDate* new,RTCDate* old)
{
  if(new->year > old->year){
    return TRUE;
  }
  if(new->month > old->month){
    return TRUE;
  }
  if(new->day > old->day){
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 *  @brief  指定したルームのDLフラグが立っているかチェック
 *
 *  @param  rank  1オリジンなので注意
 *  @param  roomno  1オリジンなので注意
 */
//--------------------------------------------------------------
BOOL BSUBWAY_WifiData_CheckRoomDataFlag(BSUBWAY_WIFI_DATA* dat,u8 rank,u8 roomno,RTCDate* day)
{
  u8  idx,ofs;
  u8  flag = 1;
  u16  roomid;
  RTCDate old_day;
  
  if(roomno > 200 || rank > 10){
    return FALSE;
  }

  //最後にDLした日付から、日が変わっているかどうかチェック
  GFDate2RTCDate(dat->day,&old_day);
  if(check_day(day,&old_day)){
    //日が変わっているので、フラグ群をオールクリア
    BSUBWAY_WifiData_ClearRoomDataFlag(dat);
    return FALSE;
  }
  //両方1オリジンなので-1して計算
  roomid = (rank-1)*200+(roomno-1);
  
  idx = roomid/8;
  ofs = roomid%8;
  flag <<= ofs;

  if(dat->flags[idx] & flag){
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 *  @brief  プレイヤーデータが存在しているかチェック
 */
//--------------------------------------------------------------
BOOL BSUBWAY_WifiData_IsPlayerDataEnable(BSUBWAY_WIFI_DATA* dat)
{
  return (BOOL)dat->player_data_f;
}

//--------------------------------------------------------------
/**
 *  @brief  リーダーデータがあるかどうかチェック
 */
//--------------------------------------------------------------
BOOL BSUBWAY_WifiData_IsLeaderDataEnable(BSUBWAY_WIFI_DATA* dat)
{
  return (BOOL)dat->leader_data_f;
}

//--------------------------------------------------------------
/**
 *  @brief  プレイヤーデータをセーブ
 *  
 *  @param  rank  1オリジンなので注意 
 *  @param  roomno  1オリジンなので注意
 */
//--------------------------------------------------------------
void BSUBWAY_WifiData_SetPlayerData(BSUBWAY_WIFI_DATA* dat,DPW_BT_PLAYER* src,u8 rank,u8 roomno)
{
  MI_CpuCopy8(src,dat->player,
    sizeof(BSUBWAY_WIFI_PLAYER)*BSUBWAY_STOCK_WIFI_PLAYER_MAX);
  
  //roomnoとrankを保存
  dat->player_rank = rank;
  dat->player_room = roomno;
  dat->player_data_f = 1;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
}

//--------------------------------------------------------------
/**
 *  @brief  プレイヤーデータをクリア
 */
//--------------------------------------------------------------
void BSUBWAY_WifiData_ClearPlayerData(BSUBWAY_WIFI_DATA* dat)
{
  MI_CpuClear8(dat->player,
    sizeof(BSUBWAY_WIFI_PLAYER)*BSUBWAY_STOCK_WIFI_PLAYER_MAX);
  dat->player_data_f = 0;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
}

//--------------------------------------------------------------
/**
 *  @brief  プレイヤーデータのRoomID(ランクとroomNo)を取得
 *
 *  @param  roomid  BSUBWAY_ROOMID型(b_tower.hで公開)
 */
//--------------------------------------------------------------
void BSUBWAY_WifiData_GetPlayerDataRoomID(BSUBWAY_WIFI_DATA* dat,BSUBWAY_ROOMID *roomid)
{
  roomid->rank = dat->player_rank;
  roomid->no = dat->player_room;
}

//--------------------------------------------------------------
/**
 *  @brief  プレイヤーデータをサブウェイ戦闘用に解凍
 */
//--------------------------------------------------------------
void BSUBWAY_WifiData_GetBtlPlayerData(BSUBWAY_WIFI_DATA* dat,
    BSUBWAY_PARTNER_DATA* player,const u8 round)
{
  BSUBWAY_TRAINER  *tr;      //トレーナーデータ
  BSUBWAY_POKEMON  *poke;    //持ちポケモンデータ
  BSUBWAY_WIFI_PLAYER* src;
  MSGDATA_MANAGER* pMan;

  tr = &(player->bt_trd);
  poke = player->btpwd;
  src = &(dat->player[round]);

  //トレーナーパラメータ取得
  tr->player_id = BSUBWAY_TRAINER_ID;//src->id_no;  //サブウェイ用IDは固定値
  tr->tr_type = src->tr_type;
  //NGネームフラグチェック
  if(src->ngname_f){
    pMan = MSGMAN_Create(MSGMAN_TYPE_NORMAL,ARC_MSG,
        NARC_msg_btower_app_dat,HEAPID_WORLD);

    MSGMAN_GetStr(pMan,msg_def_player_name01+src->gender,tr->name);
    MSGMAN_Delete(pMan);
  }else{
    MI_CpuCopy8(src->name,tr->name,16);
  }
  MI_CpuCopy8(src->appear_word,tr->appear_word,8);
  MI_CpuCopy8(src->win_word,tr->win_word,8);
  MI_CpuCopy8(src->lose_word,tr->lose_word,8);

  //ポケモンデータ取得
  MI_CpuCopy8(src->poke,poke,sizeof(BSUBWAY_POKEMON)*3);
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
}

/**
 *  @brief  リーダーデータをセーブ
 *  
 *  @param  rank  1オリジンなので注意
 *  @param  roomno  1オリジンなので注意
 */
void BSUBWAY_WifiData_SetLeaderData(BSUBWAY_WIFI_DATA* dat,DPW_BT_LEADER* src,u8 rank,u8 roomno)
{
  MI_CpuCopy8(src,&dat->leader,
    sizeof(BSUBWAY_LEADER_DATA)*BSUBWAY_STOCK_WIFI_LEADER_MAX);
  
  //roomnoとrankを保存
  dat->leader_rank = rank;
  dat->leader_room = roomno;
  dat->leader_data_f = 1;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
}

//--------------------------------------------------------------
/**
 *  @brief  リーダーデータをクリア
 */
//--------------------------------------------------------------
void BSUBWAY_WifiData_ClearLeaderData(BSUBWAY_WIFI_DATA* dat)
{
  MI_CpuClear8(&dat->leader,
    sizeof(BSUBWAY_LEADER_DATA)*BSUBWAY_STOCK_WIFI_LEADER_MAX);
  dat->leader_data_f = 0;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_SetCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
}

//--------------------------------------------------------------
/**
 *  @brief  リーダーデータのRoomID(ランクとroomNo)を取得
 *
 *  @param  roomid  BSUBWAY_ROOMID型(b_tower.hで公開)
 */
//--------------------------------------------------------------
void BSUBWAY_WifiData_GetLeaderDataRoomID(BSUBWAY_WIFI_DATA* dat,BSUBWAY_ROOMID *roomid)
{
  roomid->rank = dat->leader_rank;
  roomid->no = dat->leader_room;
}

//--------------------------------------------------------------
/**
 *  @brief  リーダーデータをAllocしたメモリにコピーして取得する
 *
 *  ＊内部でAllocしたメモリ領域を返すので、呼び出し側が明示的に解放すること！
 */
//--------------------------------------------------------------
BSUBWAY_LEADER_DATA* BSUBWAY_WifiData_GetLeaderDataAlloc(BSUBWAY_WIFI_DATA* dat,int heapID) 
{
  BSUBWAY_LEADER_DATA* bp;

  bp = sys_AllocMemory(heapID,sizeof(BSUBWAY_LEADER_DATA)*BSUBWAY_STOCK_WIFI_LEADER_MAX);
  MI_CpuCopy8(dat->leader,bp,sizeof(BSUBWAY_LEADER_DATA)*BSUBWAY_STOCK_WIFI_LEADER_MAX);

  return bp;
}
#endif

//======================================================================
//  サブウェイ　セーブデータブロック関連
//======================================================================
#if 0 //wb
//--------------------------------------------------------------
/**
 *  @brief  サブウェイ　プレイデータへのポインタを取得
 */
//--------------------------------------------------------------
BSUBWAY_PLAYWORK* SaveData_GetTowerPlayData(SAVEDATA* sv)
{
  FRONTIER_SAVEWORK* data = SaveData_Get(sv,GMDATA_ID_FRONTIER);
  
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_CheckCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
  return &data->play_tower;
}

//--------------------------------------------------------------
/**
 *  @brief  サブウェイ　スコアデータへのポインタを取得
 */
//--------------------------------------------------------------
BSUBWAY_SCOREWORK* SaveData_GetTowerScoreData(SAVEDATA* sv)
{
  FRONTIER_SAVEWORK* data = SaveData_Get(sv,GMDATA_ID_FRONTIER);
  
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_CheckCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
  return &data->tower.score;
}

//--------------------------------------------------------------
/**
 *  @brief  サブウェイ Wifiデータへのポインタを取得  
 */
//--------------------------------------------------------------
BSUBWAY_WIFI_DATA*  SaveData_GetTowerWifiData(SAVEDATA* sv)
{
  FRONTIER_SAVEWORK* data = SaveData_Get(sv,GMDATA_ID_FRONTIER);
  
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_FRONTIER)
  SVLD_CheckCrc(GMDATA_ID_FRONTIER);
#endif //CRC_LOADCHECK
  return &data->tower.wifi;
}
#endif

//----
#endif //wb null
//----
