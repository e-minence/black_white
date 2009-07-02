
//============================================================================================
/**
 * @file  btlv_effect.c
 * @brief 戦闘エフェクト制御
 * @author  soga
 * @date  2008.11.21
 */
//============================================================================================

#include <gflib.h>

#include "system/gfl_use.h"
#include "system/vm_cmd.h"

#include "btlv_effect.h"
#include "btlv_effect_def.h"

//暫定で戻し
#include "arc_def.h"
#include "particle/wazaeffect/spa.naix"

//============================================================================================
/**
 *  定数宣言
 */
//============================================================================================

#define BTLV_EFFECT_TCB_MAX ( 10 )    //使用するTCBのMAX

#define BTLV_EFFECT_BLINK_TIME  ( 3 )
#define BTLV_EFFECT_BLINK_WAIT  ( 4 )

#define BTLV_EFFECT_TRAINER_INDEX_NONE  ( 0xffffffff )

//============================================================================================
/**
 *  構造体宣言
 */
//============================================================================================

struct _BTLV_EFFECT_WORK
{
  //本来はスクリプトエンジンを載せて、動作させるが、暫定でTCBを利用する
  //最終的にはエフェクトで使用するTCBをBTLV_MCSS、BTLV_CAMERA、BTLV_EFFECTでシェアする形にする
  GFL_TCBSYS        *tcb_sys;
  void              *tcb_work;
  VMHANDLE          *vm_core;
  PALETTE_FADE_PTR  pfd;
  BTLV_MCSS_WORK    *bmw;
  BTLV_STAGE_WORK   *bsw;
  BTLV_FIELD_WORK   *bfw;
  BTLV_CAMERA_WORK  *bcw;
  BTLV_CLACT_WORK   *bclw;
  GFL_TCB           *v_tcb;
  int               execute_flag;
  HEAPID            heapID;

  int               trainer_index[ BTLV_MCSS_POS_MAX ];

  //暫定で戻し
  GFL_PTC_PTR       ptc;
  u8                spa_work[ PARTICLE_LIB_HEAP_SIZE ];
};

typedef struct
{
  void        *resource;
  BtlvMcssPos target;
  int         seq_no;
  int         work;
  int         wait;
}BTLV_EFFECT_TCB;

static  BTLV_EFFECT_WORK  *bew = NULL;

//============================================================================================
/**
 *  プロトタイプ宣言
 */
//============================================================================================

static  void  BTLV_EFFECT_VBlank( GFL_TCB *tcb, void *work );

//暫定で作ったエフェクトシーケンス
static  void  BTLV_EFFECT_TCB_Damage( GFL_TCB *tcb, void *work );

#ifdef PM_DEBUG
void  BTLV_EFFECT_SetPokemonDebug( const MCSS_ADD_DEBUG_WORK *madw, int position );
#endif

//============================================================================================
/**
 *  システム初期化
 *
 * @param[in] index     背景を決定するインデックスナンバー
 * @param[in] heapID    ヒープID
 */
//============================================================================================
void  BTLV_EFFECT_Init( int index, HEAPID heapID )
{
  GF_ASSERT( bew == NULL );
  bew = GFL_HEAP_AllocClearMemory( heapID, sizeof( BTLV_EFFECT_WORK ) );

  bew->heapID = heapID;

  //本来はスクリプトエンジンを載せて、動作させるが、暫定でTCBを利用する
  //最終的にはエフェクトで使用するTCBをBTLV_MCSS、BTLV_CAMERA、BTLV_EFFECTでシェアする形にする
  bew->tcb_work = GFL_HEAP_AllocClearMemory( heapID, GFL_TCB_CalcSystemWorkSize( BTLV_EFFECT_TCB_MAX ) );
  bew->tcb_sys = GFL_TCB_Init( BTLV_EFFECT_TCB_MAX, bew->tcb_work );

  bew->vm_core = BTLV_EFFVM_Init( bew->tcb_sys, heapID );

  //パレットフェード初期化
  bew->pfd = PaletteFadeInit( heapID );
  PaletteTrans_AutoSet( bew->pfd, TRUE );
  PaletteFadeWorkAllocSet( bew->pfd, FADE_MAIN_BG, 0x200, heapID );
  PaletteFadeWorkAllocSet( bew->pfd, FADE_SUB_BG, 0x200, heapID );
  PaletteFadeWorkAllocSet( bew->pfd, FADE_MAIN_OBJ, 0x200, heapID );
  PaletteFadeWorkAllocSet( bew->pfd, FADE_SUB_OBJ, 0x1e0, heapID );

  bew->bmw  = BTLV_MCSS_Init( bew->tcb_sys, heapID );
  bew->bsw  = BTLV_STAGE_Init( index, heapID );
  bew->bfw  = BTLV_FIELD_Init( index, heapID );
  bew->bcw  = BTLV_CAMERA_Init( bew->tcb_sys, heapID );
  bew->bclw = BTLV_CLACT_Init( bew->tcb_sys, heapID );

  BTLV_MCSS_SetOrthoMode( bew->bmw );

  //パーティクル初期化
  GFL_PTC_Init( heapID );

  //トレーナーインデックス管理配列初期化
  {
    int index;

    for( index = 0 ; index < BTLV_MCSS_POS_MAX ; index++ )
    {
      bew->trainer_index[ index ] = BTLV_EFFECT_TRAINER_INDEX_NONE;
    }
  }

  //VBlank関数
  bew->v_tcb = GFUser_VIntr_CreateTCB( BTLV_EFFECT_VBlank, NULL, 0 );
}

//============================================================================================
/**
 *  システム終了
 */
//============================================================================================
void  BTLV_EFFECT_Exit( void )
{
  GF_ASSERT( bew != NULL );

  PaletteFadeWorkAllocFree( bew->pfd, FADE_MAIN_BG );
  PaletteFadeWorkAllocFree( bew->pfd, FADE_SUB_BG );
  PaletteFadeWorkAllocFree( bew->pfd, FADE_MAIN_OBJ );
  PaletteFadeWorkAllocFree( bew->pfd, FADE_SUB_OBJ );
  PaletteFadeFree( bew->pfd );
  BTLV_MCSS_Exit( bew->bmw );
  BTLV_STAGE_Exit( bew->bsw );
  BTLV_FIELD_Exit( bew->bfw );
  BTLV_CAMERA_Exit( bew->bcw );
  BTLV_CLACT_Exit( bew->bclw );
  GFL_PTC_Exit();
  BTLV_EFFVM_Exit( bew->vm_core );
  GFL_TCB_DeleteTask( bew->v_tcb );
  GFL_TCB_Exit( bew->tcb_sys );
  GFL_HEAP_FreeMemory( bew->tcb_work );
  GFL_HEAP_FreeMemory( bew );

  bew = NULL;

}

//============================================================================================
/**
 *  システムメイン
 */
//============================================================================================
void  BTLV_EFFECT_Main( void )
{
  if( bew == NULL ) return;

  bew->execute_flag = BTLV_EFFVM_Main( bew->vm_core );

  GFL_TCB_Main( bew->tcb_sys );

  BTLV_MCSS_Main( bew->bmw );
  BTLV_STAGE_Main( bew->bsw );
  BTLV_FIELD_Main( bew->bfw );
  BTLV_CAMERA_Main( bew->bcw );
  BTLV_CLACT_Main( bew->bclw );

  //3D描画
  {
    GFL_G3D_DRAW_Start();
    GFL_G3D_DRAW_SetLookAt();
    {
      //ここの描画順番は、NitroSDKで描画されているもののあとにNitroSystemで描画されているものを置く
      //順番を混在させると正しく表示されず最悪フリーズする
      BTLV_MCSS_Draw( bew->bmw );
      {
        s32 particle_count;

        particle_count = G3X_GetPolygonListRamCount();

        GFL_PTC_Main();

        particle_count = G3X_GetPolygonListRamCount() - particle_count;
      }
      BTLV_STAGE_Draw( bew->bsw );
      BTLV_FIELD_Draw( bew->bfw );
    }

    GFL_G3D_DRAW_End();
  }
}

//============================================================================================
/**
 *  エフェクト起動
 *
 * @param[in] eff_no  起動するエフェクトナンバー
 */
//============================================================================================
void  BTLV_EFFECT_Add( int eff_no )
{
  BTLV_EFFVM_Start( bew->vm_core, BTLV_MCSS_POS_ERROR, BTLV_MCSS_POS_ERROR, eff_no );
}
//=============================================================================================
/**
 * エフェクト起動（発動位置の指定が必要なもの ... 能力アップ・ダウン，入場・退場等）
 *
 * @param   pos       発動位置
 * @param   eff_no    エフェクトナンバー
 *
 */
//=============================================================================================
void BTLV_EFFECT_AddByPos( BtlvMcssPos pos, int eff_no )
{
  BTLV_EFFVM_Start( bew->vm_core, pos, BTLV_MCSS_POS_ERROR, eff_no );
}
//=============================================================================================
/**
 * エフェクト起動（ワザエフェクト専用）
 *
 * @param   param   [in] エフェクトパラメータ
 */
//=============================================================================================
void BTLV_EFFECT_AddWazaEffect( const BTLV_WAZAEFFECT_PARAM* param )
{
  // @@@ 今は基本情報しか引き渡していない
  BTLV_EFFVM_Start( bew->vm_core, param->from, param->to, param->waza );
}

//=============================================================================================
/**
 * エフェクト強制停止
 */
//=============================================================================================
void BTLV_EFFECT_Stop( void )
{
  BTLV_EFFVM_Stop( bew->vm_core );
}

//=============================================================================================
/**
 * ダメージエフェクト起動
 *
 * @param   target    発動位置
 *
 */
//=============================================================================================
void BTLV_EFFECT_Damage( BtlvMcssPos target )
{
  BTLV_EFFECT_TCB *bet = GFL_HEAP_AllocMemory( bew->heapID, sizeof( BTLV_EFFECT_TCB ) );

  bet->seq_no = 0;
  bet->target = target;
  bet->work = BTLV_EFFECT_BLINK_TIME;
  bet->wait = 0;

  bew->execute_flag = 1;

  GFL_TCB_AddTask( bew->tcb_sys, BTLV_EFFECT_TCB_Damage, bet, 0 );
}

//============================================================================================
/**
 *  エフェクト起動中かチェック
 *
 * @retval FALSE:起動していない　TRUE:起動中
 */
//============================================================================================
BOOL  BTLV_EFFECT_CheckExecute( void )
{
  return ( bew->execute_flag != 0 );
}

//============================================================================================
/**
 *  指定された立ち位置にポケモンをセット
 *
 * @param[in] pp      セットするポケモンのPOKEMON_PARAM構造体へのポインタ
 * @param[in] position  セットするポケモンの立ち位置
 */
//============================================================================================
void  BTLV_EFFECT_SetPokemon( const POKEMON_PARAM *pp, int position )
{
  BTLV_MCSS_Add( bew->bmw, pp, position );
}

//============================================================================================
/**
 *  指定された立ち位置のポケモンを削除
 *
 * @param[in] position  削除するポケモンの立ち位置
 */
//============================================================================================
void  BTLV_EFFECT_DelPokemon( int position )
{
  BTLV_MCSS_Del( bew->bmw, position );
}

//============================================================================================
/**
 *  指定された立ち位置のポケモンが存在するかチェック
 *
 * @param[in] position  チェックするポケモンの立ち位置
 *
 * @retval  TRUE:存在する　FALSE:存在しない
 */
//============================================================================================
BOOL  BTLV_EFFECT_CheckExistPokemon( int position )
{
  return BTLV_MCSS_CheckExistPokemon( bew->bmw, position );
}

//============================================================================================
/**
 *  指定された位置にトレーナーをセット
 *
 * @param[in] trtype    セットするトレーナータイプ
 * @param[in] position  セットするトレーナーの立ち位置
 * @param[in] pos_x     セットするトレーナーのX座標（０で立ち位置のX座標を代入）
 * @param[in] pos_y     セットするトレーナーのY座標（０で立ち位置のY座標を代入）
 */
//============================================================================================
void  BTLV_EFFECT_SetTrainer( int trtype, int position, int pos_x, int pos_y )
{
  GF_ASSERT( position < BTLV_MCSS_POS_MAX );
  GF_ASSERT( bew->trainer_index[ position ] == BTLV_EFFECT_TRAINER_INDEX_NONE );

#if 0
  if( pos_x == 0 )
  {
  }
  if( pos_y == 0 )
  {
  }
#endif

  bew->trainer_index[ position ] = BTLV_CLACT_Add( bew->bclw, ARCID_TRGRA, trtype * 4, pos_x, pos_y );
}

//============================================================================================
/**
 *  指定された立ち位置のトレーナーを削除
 *
 * @param[in] position  削除するトレーナーの立ち位置
 */
//============================================================================================
void  BTLV_EFFECT_DelTrainer( int position )
{
  GF_ASSERT( position < BTLV_MCSS_POS_MAX );
  GF_ASSERT_MSG( bew->trainer_index[ position ] != BTLV_EFFECT_TRAINER_INDEX_NONE, "pos=%d", position );

  BTLV_CLACT_Delete( bew->bclw, bew->trainer_index[ position ] );
  bew->trainer_index[ position ] = BTLV_EFFECT_TRAINER_INDEX_NONE;
}

//============================================================================================
/**
 *  指定された3Dモデルに対するパレットフェードをセット
 *
 * @param[in] model       対象とする3Dモデル
 * @param[in]	start_evy   セットするパラメータ（フェードさせる色に対する開始割合16段階）
 * @param[in]	end_evy   	セットするパラメータ（フェードさせる色に対する終了割合16段階）
 * @param[in]	wait	    	セットするパラメータ（ウェイト）
 * @param[in]	rgb		    	セットするパラメータ（フェードさせる色）
 */
//============================================================================================
void  BTLV_EFFECT_SetPaletteFade( int model, u8 start_evy, u8 end_evy, u8 wait, u16 rgb )
{ 
  if( ( model == BTLEFF_PAL_FADE_STAGE ) || 
      ( model == BTLEFF_PAL_FADE_3D ) || 
      ( model == BTLEFF_PAL_FADE_ALL ) )
  { 
    BTLV_STAGE_SetPaletteFade( bew->bsw, start_evy, end_evy, wait, rgb );
  }
  if( ( model == BTLEFF_PAL_FADE_FIELD ) || 
      ( model == BTLEFF_PAL_FADE_3D ) || 
      ( model == BTLEFF_PAL_FADE_ALL ) )
  { 
    BTLV_FIELD_SetPaletteFade( bew->bfw, start_evy, end_evy, wait, rgb );
  }
}

//============================================================================================
/**
 *  指定された3Dモデルに対するパレットフェードの実行チェック
 *
 * @param[in] model       対象とする3Dモデル
 *
 * @retval  TRUE:実行中　FALSE:終了
 */
//============================================================================================
BOOL  BTLV_EFFECT_CheckExecutePaletteFade( int model )
{ 
  BOOL  ret_stage = FALSE;
  BOOL  ret_field = FALSE;

  if( ( model == BTLEFF_PAL_FADE_STAGE ) || 
      ( model == BTLEFF_PAL_FADE_3D ) || 
      ( model == BTLEFF_PAL_FADE_ALL ) )
  { 
    ret_stage = BTLV_STAGE_CheckExecutePaletteFade( bew->bsw );
  }
  if( ( model == BTLEFF_PAL_FADE_FIELD ) || 
      ( model == BTLEFF_PAL_FADE_3D ) || 
      ( model == BTLEFF_PAL_FADE_ALL ) )
  { 
    ret_field = BTLV_FIELD_CheckExecutePaletteFade( bew->bfw );
  }

  return ( ( ret_stage == TRUE ) || ( ret_field == TRUE ) );
}

//============================================================================================
/**
 *  指定された3Dモデルに対するバニッシュフラグセット
 *
 * @param[in] model  対象とする3Dモデル
 * @param[in] flag   セットするフラグ
 */
//============================================================================================
void  BTLV_EFFECT_SetVanishFlag( int model, int flag )
{ 
  switch( model )
  { 
  case BTLEFF_STAGE:
    BTLV_STAGE_SetVanishFlag( bew->bsw, flag );
    break;
  case BTLEFF_FIELD:
    BTLV_FIELD_SetVanishFlag( bew->bfw, flag );
    break;
  case BTLEFF_EFFECT:
    break;
  }
}

//============================================================================================
/**
 *  指定された立ち位置のトレーナーインデックスを取得
 *
 * @param[in] position  取得するトレーナーの立ち位置
 */
//============================================================================================
int BTLV_EFFECT_GetTrainerIndex( int position )
{
  GF_ASSERT( position < BTLV_MCSS_POS_MAX );

  return bew->trainer_index[ position ];
}

//============================================================================================
/**
 *  エフェクトで使用されているカメラ管理構造体のポインタを取得
 *
 * @retval bcw カメラ管理構造体
 */
//============================================================================================
BTLV_CAMERA_WORK  *BTLV_EFFECT_GetCameraWork( void )
{
  return bew->bcw;
}

//============================================================================================
/**
 *  エフェクトで使用されているMCSS管理構造体のポインタを取得
 *
 * @retval bmw MCSS管理構造体
 */
//============================================================================================
BTLV_MCSS_WORK  *BTLV_EFFECT_GetMcssWork( void )
{
  return bew->bmw;
}

//============================================================================================
/**
 *  エフェクトで使用されているVMHANDLE管理構造体のポインタを取得
 *
 * @retval vm_core VMHANDLE管理構造体
 */
//============================================================================================
VMHANDLE  *BTLV_EFFECT_GetVMHandle( void )
{
  return bew->vm_core;
}

//============================================================================================
/**
 *  エフェクトで使用されているGFL_TCBSYS管理構造体のポインタを取得
 *
 * @retval vm_core VMHANDLE管理構造体
 */
//============================================================================================
GFL_TCBSYS  *BTLV_EFFECT_GetTCBSYS( void )
{
  return bew->tcb_sys;
}

//============================================================================================
/**
 *  エフェクトで使用されているPALETTE_FADE_PTR管理構造体のポインタを取得
 *
 * @retval vm_core VMHANDLE管理構造体
 */
//============================================================================================
PALETTE_FADE_PTR  BTLV_EFFECT_GetPfd( void )
{
  return bew->pfd;
}

//============================================================================================
/**
 *  エフェクトで使用されているCLWK管理構造体のポインタを取得
 *
 * @retval bclw CLWK管理構造体
 */
//============================================================================================
BTLV_CLACT_WORK *BTLV_EFFECT_GetCLWK( void )
{
  return bew->bclw;
}

//============================================================================================
/**
 *  VBlank関数
 */
//============================================================================================
static  void  BTLV_EFFECT_VBlank( GFL_TCB *tcb, void *work )
{
  GFL_CLACT_SYS_VBlankFunc();
  PaletteFadeTrans( bew->pfd );
}

//============================================================================================
/**
 *  ダメージエフェクトシーケンス（仮でTCBで作成）
 */
//============================================================================================
static  void  BTLV_EFFECT_TCB_Damage( GFL_TCB *tcb, void *work )
{
  BTLV_EFFECT_TCB *bet = (BTLV_EFFECT_TCB*)work;

  if( bet->wait ){
    bet->wait--;
    return;
  }
  switch( bet->seq_no ){
  case 0:
    bet->seq_no ^= 1;
    BTLV_MCSS_SetVanishFlag( bew->bmw, bet->target, BTLV_MCSS_VANISH_ON );
    bet->wait = BTLV_EFFECT_BLINK_WAIT;
    break;
  case 1:
    bet->seq_no ^= 1;
    BTLV_MCSS_SetVanishFlag( bew->bmw, bet->target, BTLV_MCSS_VANISH_OFF );
    bet->wait = BTLV_EFFECT_BLINK_WAIT;
    if( --bet->work == 0 ){
      bew->execute_flag = 0;
      GFL_HEAP_FreeMemory( bet );
      GFL_TCB_DeleteTask( tcb );
    }
    break;
  }
}

#ifdef PM_DEBUG
//============================================================================================
/**
 *  指定された立ち位置にポケモンをセット
 *
 * @param[in] pp      セットするポケモンのPOKEMON_PARAM構造体へのポインタ
 * @param[in] position  セットするポケモンの立ち位置
 */
//============================================================================================
void  BTLV_EFFECT_SetPokemonDebug( const MCSS_ADD_DEBUG_WORK *madw, int position )
{
  BTLV_MCSS_AddDebug( bew->bmw, madw, position );
}
#endif
