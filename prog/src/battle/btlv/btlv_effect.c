
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
#include "sound/pm_voice.h"

#include "btlv_effect.h"
#include "btlv_effect_def.h"

#include "field/field_light_status.h"

#include "tr_tool/trtype_def.h"
#include "tr_tool/trtype_sex.h"

#include "arc_def.h"
#include "battle/batt_bg_tbl.h"
#include "batt_bg_tbl.naix"

//============================================================================================
/**
 *  定数宣言
 */
//============================================================================================

#define BTLV_EFFECT_TCB_MAX ( 32 )    //使用するTCBのMAX

#define BTLV_EFFECT_BLINK_TIME    ( 3 )
#define BTLV_EFFECT_BLINK_WAIT    ( 3 )
#define BTLV_EFFECT_BLINK_PHYSIC  ( 0x0842 )
#define BTLV_EFFECT_BLINK_SPECIAL ( 0x6318 )

#define BTLV_EFFECT_TRAINER_INDEX_NONE  ( 0xffffffff )

#define TRTYPE_NONE ( 0xffff )

//============================================================================================
/**
 *  構造体宣言
 */
//============================================================================================

struct _BTLV_EFFECT_SETUP_PARAM
{
  BtlRule             rule;
  BTL_FIELD_SITUATION bfs;
  u16                 tr_type[ 4 ];
  BOOL                multi;
};

struct _BTLV_EFFECT_WORK
{
  //本来はスクリプトエンジンを載せて、動作させるが、暫定でTCBを利用する
  //最終的にはエフェクトで使用するTCBをBTLV_MCSS、BTLV_CAMERA、BTLV_EFFECTでシェアする形にする
  GFL_TCBSYS*             tcb_sys;
  void*                   tcb_work;
  VMHANDLE*               vm_core;
  PALETTE_FADE_PTR        pfd;
  BTLV_MCSS_WORK*         bmw;
  BTLV_STAGE_WORK*        bsw;
  BTLV_FIELD_WORK*        bfw;
  BTLV_CAMERA_WORK*       bcw;
  BTLV_CLACT_WORK*        bclw;
  BTLV_GAUGE_WORK*        bgw;
  BTLV_BALL_GAUGE_WORK*   bbgw[ BTLV_BALL_GAUGE_TYPE_MAX ];
  BTLV_TIMER_WORK*        btw;
  BTLV_BG_WORK*           bbw;
  GFL_TCB*                v_tcb;
  BTLV_EFFECT_SETUP_PARAM besp;
  int                     execute_flag;
  int                     tcb_execute_flag;
  HEAPID                  heapID;

  int                     trainer_index[ BTLV_MCSS_POS_MAX ];
};

typedef struct
{
  BtlvMcssPos target;
  int         seq_no;
  int         work;
  int         wait;
  int         color;
}BTLV_EFFECT_DAMAGE_TCB;

typedef struct
{
  BtlvMcssPos   vpos;
  int           seq_no;
  MCSS_ADD_WORK maw;
}BTLV_EFFECT_HENGE_TCB;

typedef struct
{
  int           seq_no;
  BtlRotateDir  dir;
  int           side;
}TCB_ROTATION;

static  BTLV_EFFECT_WORK  *bew = NULL;

//============================================================================================
/**
 *  プロトタイプ宣言
 */
//============================================================================================

static  void  BTLV_EFFECT_VBlank( GFL_TCB *tcb, void *work );
static  void  BTLV_EFFECT_TCB_Damage( GFL_TCB *tcb, void *work );
static  void  BTLV_EFFECT_TCB_Henge( GFL_TCB *tcb, void *work );
static  void  TCB_BTLV_EFFECT_Rotation( GFL_TCB *tcb, void *work );

#ifdef PM_DEBUG
void  BTLV_EFFECT_SetPokemonDebug( const MCSS_ADD_DEBUG_WORK *madw, int position );
#endif

//============================================================================================
/**
 * @brief システム初期化用のセットアップパラメータ生成
 *
 * @param[in] heapID      ヒープID
 */
//============================================================================================
BTLV_EFFECT_SETUP_PARAM*  BTLV_EFFECT_MakeSetUpParam( BtlRule rule, const BTL_FIELD_SITUATION* bfs, BOOL multi, u16* tr_type, HEAPID heapID )
{
  BTLV_EFFECT_SETUP_PARAM* besp = GFL_HEAP_AllocMemory( heapID, sizeof( BTLV_EFFECT_SETUP_PARAM ) );
  int i;

  besp->rule  = rule;
  besp->bfs   = *bfs;
  besp->multi = multi;

  for( i = 0 ; i < 4 ; i++ )
  {
    besp->tr_type[ i ] = tr_type[ i ];
  }

  return besp;
}

//============================================================================================
/**
 * @brief システム初期化用のセットアップパラメータ生成（バトル用）
 *
 * @param[in] mainModule  戦闘メインモジュール
 * @param[in] heapID      ヒープID
 */
//============================================================================================
BTLV_EFFECT_SETUP_PARAM*  BTLV_EFFECT_MakeSetUpParamBtl( const BTL_MAIN_MODULE* mainModule, HEAPID heapID )
{
  int i;
  u16 tr_type[ 4 ];

  if( BTL_MAIN_IsMultiMode( mainModule ) == TRUE )
  {
    tr_type[ 0 ] = BTL_MAIN_GetClientTrainerType( mainModule, BTL_MAIN_GetPlayerClientID( mainModule ) );
    tr_type[ 1 ] = BTL_MAIN_GetClientTrainerType( mainModule, BTL_MAIN_GetEnemyClientID( mainModule, 0 ) );
    tr_type[ 2 ] = BTL_MAIN_GetClientTrainerType( mainModule, BTL_MAIN_GetFriendCleintID( mainModule ) );
    tr_type[ 3 ] = BTL_MAIN_GetClientTrainerType( mainModule, BTL_MAIN_GetEnemyClientID( mainModule, 1 ) );
  }
  else
  {
    tr_type[ 0 ] = BTL_MAIN_GetClientTrainerType( mainModule, BTL_MAIN_GetPlayerClientID( mainModule ) );
    tr_type[ 1 ] = BTL_MAIN_GetClientTrainerType( mainModule, BTL_MAIN_GetEnemyClientID( mainModule, 0 ) );
    tr_type[ 2 ] = 0;
    tr_type[ 3 ] = 0;
  }


  return BTLV_EFFECT_MakeSetUpParam( BTL_MAIN_GetRule( mainModule ), BTL_MAIN_GetFieldSituation( mainModule ),
                                     BTL_MAIN_IsMultiMode( mainModule ), tr_type, heapID );
}

//============================================================================================
/**
 * @brief システム初期化
 *
 * @param[in] besp        セットアップパラメータ
 * @param[in] fontHandle  フォントハンドル
 * @param[in] heapID      ヒープID
 */
//============================================================================================
void  BTLV_EFFECT_Init( BTLV_EFFECT_SETUP_PARAM* besp, GFL_FONT* fontHandle, HEAPID heapID )
{
  GF_ASSERT( bew == NULL );
  bew = GFL_HEAP_AllocClearMemory( heapID, sizeof( BTLV_EFFECT_WORK ) );

  bew->besp = *besp;

  bew->heapID = heapID;

  bew->tcb_work = GFL_HEAP_AllocClearMemory( heapID, GFL_TCB_CalcSystemWorkSize( BTLV_EFFECT_TCB_MAX ) );
  bew->tcb_sys = GFL_TCB_Init( BTLV_EFFECT_TCB_MAX, bew->tcb_work );

  bew->vm_core = BTLV_EFFVM_Init( bew->tcb_sys, heapID );

  //パレットフェード初期化
  bew->pfd = PaletteFadeInit( heapID );
  PaletteTrans_AutoSet( bew->pfd, TRUE );
  PaletteFadeWorkAllocSet( bew->pfd, FADE_MAIN_BG, 0x200, heapID );
  PaletteFadeWorkAllocSet( bew->pfd, FADE_SUB_BG, 0x1e0, heapID );
  PaletteFadeWorkAllocSet( bew->pfd, FADE_MAIN_OBJ, 0x200, heapID );
  PaletteFadeWorkAllocSet( bew->pfd, FADE_SUB_OBJ, 0x1e0, heapID );

  bew->bmw  = BTLV_MCSS_Init( besp->rule, bew->tcb_sys, heapID );

  {
    BATT_BG_TBL_ZONE_SPEC_TABLE*  bbtzst = GFL_ARC_LoadDataAlloc( ARCID_BATT_BG_TBL,
                                                                  NARC_batt_bg_tbl_zone_spec_table_bin,
                                                                  bew->heapID );
    u8  season = 0;

    if( bbtzst[ besp->bfs.bgType ].season )
    {
      season = besp->bfs.season;
    }
    bew->bsw  = BTLV_STAGE_Init( besp->rule, bbtzst[ besp->bfs.bgType ].stage_file[ besp->bfs.bgAttr ], season, heapID );
    bew->bfw  = BTLV_FIELD_Init( besp->rule, bbtzst[ besp->bfs.bgType ].bg_file[ besp->bfs.bgAttr ], season, heapID );

    //ライト設定
    if( bbtzst[ besp->bfs.bgType ].time_zone )
    {
      GFL_G3D_LIGHT light;
      FIELD_LIGHT_STATUS  fls;

      FIELD_LIGHT_STATUS_Get( besp->bfs.zoneID, besp->bfs.hour, besp->bfs.minute, besp->bfs.weather, besp->bfs.season, &fls, bew->heapID );

      light.color = fls.light;
      light.vec.x = 0;
      light.vec.y = -FX32_ONE;
      light.vec.z = 0;
      GFL_G3D_SetSystemLight( 0, &light );
    }
    GFL_HEAP_FreeMemory( bbtzst );
  }

  bew->bcw  = BTLV_CAMERA_Init( bew->tcb_sys, heapID );
  bew->bclw = BTLV_CLACT_Init( bew->tcb_sys, heapID );
  bew->bgw  = BTLV_GAUGE_Init( fontHandle, heapID );
  bew->btw  = BTLV_TIMER_Init( heapID );
  bew->bbw  = BTLV_BG_Init( bew->tcb_sys, heapID );


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
  bew->v_tcb = GFUser_VIntr_CreateTCB( BTLV_EFFECT_VBlank, NULL, 1 );

  //3体同時で鳴き声を鳴かせられるようにバッファを確保
  PMVOICE_PlayerHeapReserve( 2, bew->heapID );
}

//============================================================================================
/**
 *  @brief  システム終了
 */
//============================================================================================
void  BTLV_EFFECT_Exit( void )
{
  GF_ASSERT( bew != NULL );

  PMVOICE_PlayerHeapRelease();

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
  BTLV_GAUGE_Exit( bew->bgw );
  BTLV_TIMER_Exit( bew->btw );
  BTLV_BG_Exit( bew->bbw );
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
 *  @brief  システムメイン
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
  BTLV_GAUGE_Main( bew->bgw );

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
 * @brief  エフェクト起動
 *
 * @param[in] eff_no  起動するエフェクトナンバー
 */
//============================================================================================
void  BTLV_EFFECT_Add( int eff_no )
{
  BTLV_EFFVM_Start( bew->vm_core, BTLV_MCSS_POS_ERROR, BTLV_MCSS_POS_ERROR, eff_no, NULL );
  bew->execute_flag = TRUE;
}
//=============================================================================================
/**
 * @brief エフェクト起動（発動位置の指定が必要なもの ... 能力アップ・ダウン，入場・退場等）
 *
 * @param   pos       発動位置
 * @param   eff_no    エフェクトナンバー
 *
 */
//=============================================================================================
void BTLV_EFFECT_AddByPos( BtlvMcssPos pos, int eff_no )
{
  BTLV_EFFVM_Start( bew->vm_core, pos, BTLV_MCSS_POS_ERROR, eff_no, NULL );
  bew->execute_flag = TRUE;
}
//=============================================================================================
/**
 * @brief エフェクト起動（発動位置の指定と方向が必要なもの ... ターンチェック時のやどりぎのタネ等）
 *
 * @param   from      発動位置
 * @param   to        方向
 * @param   eff_no    エフェクトナンバー
 *
 */
//=============================================================================================
void BTLV_EFFECT_AddByDir( BtlvMcssPos from, BtlvMcssPos to, int eff_no )
{
  BTLV_EFFVM_Start( bew->vm_core, from, to, eff_no, NULL );
  bew->execute_flag = TRUE;
}
//=============================================================================================
/**
 * @brief エフェクト起動（ワザエフェクト専用）
 *
 * @param   param   [in] エフェクトパラメータ
 */
//=============================================================================================
void BTLV_EFFECT_AddWazaEffect( const BTLV_WAZAEFFECT_PARAM* param )
{
  BTLV_EFFVM_PARAM  effvm_param;

  effvm_param.waza_range = WAZADATA_GetParam( param->waza, WAZAPARAM_TARGET );
  effvm_param.turn_count = param->turn_count;
  effvm_param.continue_count = param->continue_count;

  BTLV_EFFVM_Start( bew->vm_core, param->from, param->to, param->waza, &effvm_param );
  bew->execute_flag = TRUE;
}

//=============================================================================================
/**
 * @brief エフェクト強制停止
 */
//=============================================================================================
void BTLV_EFFECT_Stop( void )
{
  BTLV_EFFVM_Stop( bew->vm_core );
  bew->execute_flag = FALSE;
}

//=============================================================================================
/**
 * @brief エフェクト再開
 */
//=============================================================================================
void BTLV_EFFECT_Restart( void )
{
  BTLV_EFFVM_Restart( bew->vm_core );
  bew->execute_flag = TRUE;
}

//=============================================================================================
/**
 * @brief ダメージエフェクト起動
 *
 * @param   target    発動位置
 * @param   wazaID    ワザナンバー
 */
//=============================================================================================
void BTLV_EFFECT_Damage( BtlvMcssPos target, WazaID waza )
{
  BTLV_EFFECT_DAMAGE_TCB *bedt = GFL_HEAP_AllocMemory( bew->heapID, sizeof(BTLV_EFFECT_DAMAGE_TCB) );

  bedt->seq_no = 0;
  bedt->target = target;
  bedt->work = BTLV_EFFECT_BLINK_TIME;
  bedt->wait = 0;

  bew->tcb_execute_flag = 1;

  if( WAZADATA_GetDamageType( waza ) == WAZADATA_DMG_PHYSIC )
  {
    //物理ダメージ
    bedt->color = BTLV_EFFECT_BLINK_PHYSIC;
  }
  else
  {
    //特殊ダメージ
    bedt->color = BTLV_EFFECT_BLINK_SPECIAL;
  }

  GFL_TCB_AddTask( bew->tcb_sys, BTLV_EFFECT_TCB_Damage, bedt, 0 );
}
//=============================================================================================
/**
 * @brief 瀕死エフェクト起動
 *
 * @param   target    発動位置
 */
//=============================================================================================
void BTLV_EFFECT_Hinshi( BtlvMcssPos target )
{
  BTLV_EFFECT_AddByPos( target, BTLEFF_HINSHI );
}

//=============================================================================================
/**
 * @brief 捕獲時ボール投げエフェクト起動
 *
 * @param   vpos        対象ポケモンの描画位置
 * @param   itemNum     投げたボールのアイテムナンバー
 * @param   yure_cnt    ボール揺れ回数（0〜3）
 * @param   f_success   捕獲成功フラグ
 * @param   f_critical  クリティカルフラグ
 */
//=============================================================================================
void BTLV_EFFECT_BallThrow( int vpos, u16 item_no, u8 yure_cnt, BOOL f_success, BOOL f_critical )
{
  BTLV_EFFVM_PARAM  effvm_param;

  effvm_param.yure_cnt      = yure_cnt;
  effvm_param.get_success   = f_success;
  effvm_param.get_critical  = f_critical;
  effvm_param.item_no       = item_no;

  BTLV_EFFVM_Start( bew->vm_core, BTLV_MCSS_POS_AA, vpos, BTLEFF_BALL_THROW, &effvm_param );
}

//=============================================================================================
/**
 * @brief 捕獲時ボール投げエフェクト起動（トレーナー戦用）
 *
 * @param   vpos        対象ポケモンの描画位置
 * @param   itemNum     投げたボールのアイテムナンバー
 */
//=============================================================================================
void BTLV_EFFECT_BallThrowTrainer( int vpos, u16 item_no )
{
  BTLV_EFFVM_PARAM  effvm_param;

  effvm_param.item_no = item_no;

  BTLV_EFFVM_Start( bew->vm_core, BTLV_MCSS_POS_AA, vpos, BTLEFF_BALL_THROW_TRAINER, &effvm_param );
}

//=============================================================================================
/**
 * @brief 変化エフェクト起動
 *
 * @param[in] pp    対象ポケモンのPOKEMON_PARAM
 * @param[in] vpos  対象ポケモンの描画位置
 */
//=============================================================================================
void BTLV_EFFECT_Henge( const POKEMON_PARAM* pp, BtlvMcssPos vpos )
{
  BTLV_EFFECT_HENGE_TCB *beht = GFL_HEAP_AllocMemory( bew->heapID, sizeof(BTLV_EFFECT_HENGE_TCB) );

  beht->seq_no = 0;
  beht->vpos = vpos;

  BTLV_MCSS_MakeMAW( pp, &beht->maw, vpos );

  bew->tcb_execute_flag = 1;

  GFL_TCB_AddTask( bew->tcb_sys, BTLV_EFFECT_TCB_Henge, beht, 0 );
}

//============================================================================================
/**
 * @brief  エフェクト起動中かチェック
 *
 * @retval FALSE:起動していない　TRUE:起動中
 */
//============================================================================================
BOOL  BTLV_EFFECT_CheckExecute( void )
{
  return ( ( bew->execute_flag | bew->tcb_execute_flag ) != 0 );
}

//============================================================================================
/**
 * @brief  指定された立ち位置にポケモンをセット
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
 * @brief  指定された立ち位置のポケモンを削除
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
 * @brief  指定された立ち位置にMCSSが存在するかチェック
 *
 * @param[in] position  チェックする立ち位置
 *
 * @retval  TRUE:存在する　FALSE:存在しない
 */
//============================================================================================
BOOL  BTLV_EFFECT_CheckExist( int position )
{
  if( position < BTLV_MCSS_POS_MAX )
  {
    return BTLV_MCSS_CheckExist( bew->bmw, position );
  }
  else
  {
    return( bew->trainer_index[ position - BTLV_MCSS_POS_MAX ] != BTLV_EFFECT_TRAINER_INDEX_NONE );
  }
}

//============================================================================================
/**
 * @brief  指定された位置にトレーナーをセット
 *
 * @param[in] trtype    セットするトレーナータイプ
 * @param[in] position  セットするトレーナーの立ち位置
 * @param[in] pos_x     セットするトレーナーのX座標（０で立ち位置のX座標を代入）
 * @param[in] pos_y     セットするトレーナーのY座標（０で立ち位置のY座標を代入）
 */
//============================================================================================
void  BTLV_EFFECT_SetTrainer( int trtype, int position, int pos_x, int pos_y, int pos_z )
{
  GF_ASSERT_MSG( ( position - BTLV_MCSS_POS_MAX ) >= 0, "position:%d\n", position );
  GF_ASSERT( ( position - BTLV_MCSS_POS_MAX ) < BTLV_MCSS_POS_MAX );
  GF_ASSERT( bew->trainer_index[ position - BTLV_MCSS_POS_MAX ] == BTLV_EFFECT_TRAINER_INDEX_NONE );

#if 0
  if( pos_x == 0 )
  {
  }
  if( pos_y == 0 )
  {
  }
#endif
  if( position & 1 )
  {
    BTLV_MCSS_AddTrainer( bew->bmw, trtype, position );
    if( ( pos_x != 0 ) || ( pos_y != 0 ) || ( pos_z != 0 ) )
    {
      BTLV_MCSS_SetPosition( bew->bmw, position, pos_x, pos_y, pos_z );
    }
    //BTLV_MCSS_SetShadowVanishFlag( bew->bmw, position, 1 );
    bew->trainer_index[ position - BTLV_MCSS_POS_MAX ] = position;
  }
  else
  {
    switch( trtype ){
    case TRTYPE_HERO:
    case TRTYPE_HEROINE:
      break;
    case TRTYPE_DOCTOR:
      trtype = 2;
      break;
    case TRTYPE_RIVAL:
      trtype = 3;
      break;
    default:
      if( TrTypeSexTable[ trtype ] == PTL_SEX_MALE )
      {
        trtype = TRTYPE_HERO;
      }
      else
      {
        trtype = TRTYPE_HEROINE;
      }
      break;
    }
    pos_x = pos_x >> FX32_SHIFT;
    pos_y = pos_y >> FX32_SHIFT;
    bew->trainer_index[ position - BTLV_MCSS_POS_MAX ] = BTLV_CLACT_Add( bew->bclw, ARCID_TRBGRA, trtype * 4, pos_x, pos_y );
  }
}

//============================================================================================
/**
 * @brief  指定された立ち位置のトレーナーを削除
 *
 * @param[in] position  削除するトレーナーの立ち位置
 */
//============================================================================================
void  BTLV_EFFECT_DelTrainer( int position )
{
  GF_ASSERT_MSG( ( position - BTLV_MCSS_POS_MAX ) >= 0, "position:%d\n", position );
  GF_ASSERT( position - BTLV_MCSS_POS_MAX < BTLV_MCSS_POS_MAX );
  GF_ASSERT_MSG( bew->trainer_index[ position - BTLV_MCSS_POS_MAX ] != BTLV_EFFECT_TRAINER_INDEX_NONE, "pos=%d", position );

  if( position & 1 )
  {
    BTLV_MCSS_Del( bew->bmw, position );
    bew->trainer_index[ position - BTLV_MCSS_POS_MAX ] = BTLV_EFFECT_TRAINER_INDEX_NONE;
  }
  else
  {
    BTLV_CLACT_Delete( bew->bclw, bew->trainer_index[ position - BTLV_MCSS_POS_MAX ] );
    bew->trainer_index[ position - BTLV_MCSS_POS_MAX ] = BTLV_EFFECT_TRAINER_INDEX_NONE;
  }
}

//============================================================================================
/**
 * @brief  指定された位置にゲージをセット
 *
 * @param[in] pp        ゲージ表示するポケモンのPOEKMON_PARAM構造体のポインタ
 * @param[in] position  セットするゲージ位置
 */
//============================================================================================
void  BTLV_EFFECT_SetGauge( const BTL_MAIN_MODULE* wk, const BTL_POKEPARAM* bpp, int position )
{
  if( bew->besp.rule == BTL_RULE_TRIPLE )
  {
    BTLV_GAUGE_Add( bew->bgw, wk, bpp, BTLV_GAUGE_TYPE_3vs3, position );
  }
  else
  {
    BTLV_GAUGE_Add( bew->bgw, wk, bpp, BTLV_GAUGE_TYPE_1vs1, position );
  }
}

//============================================================================================
/**
 * @brief  指定された位置にゲージをセット（POKEMON_PARAMバージョン）
 *
 * @param[in] pp        ゲージ表示するポケモンのPOEKMON_PARAM構造体のポインタ
 * @param[in] position  セットするゲージ位置
 */
//============================================================================================
void  BTLV_EFFECT_SetGaugePP( const ZUKAN_SAVEDATA* zs, const POKEMON_PARAM* pp, int position )
{
  if( bew->besp.rule == BTL_RULE_TRIPLE )
  {
    BTLV_GAUGE_AddPP( bew->bgw, zs, pp, BTLV_GAUGE_TYPE_3vs3, position );
  }
  else
  {
    BTLV_GAUGE_AddPP( bew->bgw, zs, pp, BTLV_GAUGE_TYPE_1vs1, position );
  }
}

//============================================================================================
/**
 * @brief  指定された位置のゲージを削除
 *
 * @param[in] position  削除するゲージ位置
 */
//============================================================================================
void  BTLV_EFFECT_DelGauge( int position )
{
  BTLV_GAUGE_Del( bew->bgw, position );
}

//============================================================================================
/**
 * @brief  指定された位置のHPゲージ計算
 *
 * @param[in] position  計算するゲージ位置
 * @param[in] value     計算量
 */
//============================================================================================
void  BTLV_EFFECT_CalcGaugeHP( int position, int value )
{
  BTLV_GAUGE_CalcHP( bew->bgw, position, value );
}

//============================================================================================
/**
 * @brief  指定された位置のHPゲージ計算（ゲージの増減エフェクトなしですぐ反映）
 *
 * @param[in] position  計算するゲージ位置
 * @param[in] value     計算量
 */
//============================================================================================
void  BTLV_EFFECT_CalcGaugeHPAtOnce( int position, int value )
{
  BTLV_GAUGE_CalcHPAtOnce( bew->bgw, position, value );
}

//============================================================================================
/**
 * @brief  指定された位置のEXPゲージ計算
 *
 * @param[in] position  計算するゲージ位置
 * @param[in] value     計算量
 */
//============================================================================================
void  BTLV_EFFECT_CalcGaugeEXP( int position, int value )
{
  BTLV_GAUGE_CalcEXP( bew->bgw, position, value );
}

//=============================================================================================
/**
 * @brief  指定された位置のEXPゲージレベルアップ
 *
 * @param   position    計算するゲージ位置
 * @param   bpp         レベルアップ後のパラメータ
 */
//=============================================================================================
void BTLV_EFFECT_CalcGaugeEXPLevelUp( int position, const BTL_POKEPARAM* bpp )
{
  BTLV_GAUGE_CalcEXPLevelUp( bew->bgw, bpp, position );
}

//============================================================================================
/**
 * @brief  ゲージが計算中かチェック
 */
//============================================================================================
BOOL  BTLV_EFFECT_CheckExecuteGauge( void )
{
  return BTLV_GAUGE_CheckExecute( bew->bgw );
}

//============================================================================================
/**
 * @brief  ゲージ表示/非表示
 */
//============================================================================================
void  BTLV_EFFECT_SetGaugeDrawEnable( BOOL on_off )
{
  BTLV_GAUGE_SetDrawEnable( bew->bgw, on_off );
}

//============================================================================================
/**
 * @brief  ゲージにステータスアイコンを表示
 *
 * @param[in] sick  セットする状態異常
 * @param[in] pos   セットするポケモンの立ち位置
 */
//============================================================================================
void  BTLV_EFFECT_SetGaugeStatus( PokeSick sick,  BtlvMcssPos pos )
{
  BTLV_GAUGE_SetStatus( bew->bgw, sick, pos );
}

//============================================================================================
/**
 * @brief  指定された位置にボールゲージをセット
 *
 * @param[in] bbgp  ボールゲージ用初期化パラメータ
 */
//============================================================================================
void  BTLV_EFFECT_SetBallGauge( const BTLV_BALL_GAUGE_PARAM* bbgp )
{
  bew->bbgw[ bbgp->type ] = BTLV_BALL_GAUGE_Create( bbgp, bew->heapID );
}

//============================================================================================
/**
 * @brief  指定された位置のボールゲージを削除
 *
 * @param[in] type  削除するゲージタイプ
 */
//============================================================================================
void  BTLV_EFFECT_DelBallGauge( BTLV_BALL_GAUGE_TYPE type )
{
  BTLV_BALL_GAUGE_Delete( bew->bbgw[ type ] );
}

//============================================================================================
/**
 * @brief  ゲージが計算中かチェック
 */
//============================================================================================
BOOL  BTLV_EFFECT_CheckExecuteBallGauge( BTLV_BALL_GAUGE_TYPE type )
{
  return BTLV_BALL_GAUGE_CheckExecute( bew->bbgw[ type ] );
}

//============================================================================================
/**
 * @brief  指定された3Dモデルに対するパレットフェードをセット
 *
 * @param[in] model       対象とする3Dモデル
 * @param[in] start_evy   セットするパラメータ（フェードさせる色に対する開始割合16段階）
 * @param[in] end_evy     セットするパラメータ（フェードさせる色に対する終了割合16段階）
 * @param[in] wait        セットするパラメータ（ウェイト）
 * @param[in] rgb         セットするパラメータ（フェードさせる色）
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
  if( ( model == BTLEFF_PAL_FADE_EFFECT ) ||
      ( model == BTLEFF_PAL_FADE_ALL ) )
  {
    PaletteFadeReq( bew->pfd, PF_BIT_MAIN_BG, BTLEFF_PAL_FADE_EFFECT_BIT, wait,
                    start_evy, end_evy, rgb, bew->tcb_sys );
  }
}

//============================================================================================
/**
 * @brief  指定された3Dモデルに対するパレットフェードの実行チェック
 *
 * @param[in] model       対象とする3Dモデル
 *
 * @retval  TRUE:実行中　FALSE:終了
 */
//============================================================================================
BOOL  BTLV_EFFECT_CheckExecutePaletteFade( int model )
{
  BOOL  ret_stage   = FALSE;
  BOOL  ret_field   = FALSE;
  BOOL  ret_effect  = FALSE;

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
  if( ( model == BTLEFF_PAL_FADE_EFFECT ) ||
      ( model == BTLEFF_PAL_FADE_ALL ) )
  {
    ret_effect = ( PaletteFadeCheck( bew->pfd ) != 0 );
  }

  return ( ( ret_stage == TRUE ) || ( ret_field == TRUE ) || ( ret_effect == TRUE ) );
}

//============================================================================================
/**
 * @brief  指定された3Dモデルに対するバニッシュフラグセット
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
    GFL_BG_SetVisible( GFL_BG_FRAME3_M, flag ^ 1 );
    break;
  }
}

//============================================================================================
/**
 * @brief  ローテーションアニメセット
 *
 * @param[in] dir   ローテーションの向き
 * @param[in] side  自分側か相手側か
 */
//============================================================================================
void  BTLV_EFFECT_SetRotateEffect( BtlRotateDir dir, int side )
{
  TCB_ROTATION *tr = GFL_HEAP_AllocMemory( bew->heapID, sizeof( TCB_ROTATION ) );
  GF_ASSERT( dir != BTL_ROTATEDIR_NONE )
  if( dir == BTL_ROTATEDIR_STAY )
  {
    return;
  }
  bew->tcb_execute_flag = 1;
  tr->seq_no = 0;
  tr->dir = ( dir == BTL_ROTATEDIR_L ) ? 0 : 1;
  tr->side = side;
  GFL_TCB_AddTask( bew->tcb_sys, TCB_BTLV_EFFECT_Rotation, tr, 0 );
}

//============================================================================================
/**
 * @brief  指定された立ち位置のトレーナーインデックスを取得
 *
 * @param[in] position  取得するトレーナーの立ち位置
 */
//============================================================================================
int BTLV_EFFECT_GetTrainerIndex( int position )
{
  GF_ASSERT_MSG( ( position - BTLV_MCSS_POS_MAX ) >= 0, "position:%d\n", position );
  GF_ASSERT( ( position - BTLV_MCSS_POS_MAX ) < BTLV_MCSS_POS_MAX );
  GF_ASSERT( bew->trainer_index[ position - BTLV_MCSS_POS_MAX ] != BTLV_EFFECT_TRAINER_INDEX_NONE );

  return bew->trainer_index[ position - BTLV_MCSS_POS_MAX ];
}

//============================================================================================
/**
 * @brief  時間制限タイマークリエイト
 *
 * @param[in] game_time     ゲーム時間（秒）
 * @param[in] command_time  コマンド時間（秒）
 */
//============================================================================================
void  BTLV_EFFECT_CreateTimer( int game_time, int command_time )
{
  BTLV_TIMER_Create( bew->btw, game_time, command_time );
}

//============================================================================================
/**
 *  @brief  タイマー描画許可
 *
 *  @param[in]  type    タイマータイプ
 *  @param[in]  enable  TRUE:描画　FALSE:非描画
 *  @param[in]  init    TRUE:カウンタを初期化して描画　FALSE:初期化せず描画（enableがTRUEのときにしか意味がありません）
 *
 */
//============================================================================================
void  BTLV_EFFECT_DrawEnableTimer( BTLV_TIMER_TYPE type, BOOL enable, BOOL init )
{
  BTLV_TIMER_SetDrawEnable( bew->btw, type, enable, init );
}

//============================================================================================
/**
 *  @brief  タイマーゼロチェック
 *
 *  @param[in]  type    タイマータイプ
 *
 */
//============================================================================================
BOOL  BTLV_EFFECT_IsZero( BTLV_TIMER_TYPE type )
{
  return BTLV_TIMER_IsZero( bew->btw, type );
}

//============================================================================================
/**
 *  @brief  指定された立ち位置のMCSSのバニッシュフラグを取得
 *
 *  @param[in]  position  取得したい立ち位置
 */
//============================================================================================
BTLV_MCSS_VANISH_FLAG  BTLV_EFFECT_GetMcssVanishFlag( BtlvMcssPos position )
{
  return  BTLV_MCSS_GetVanishFlag( bew->bmw, position );
}

//============================================================================================
/**
 *  @brief  指定された立ち位置のMCSSにバニッシュフラグをセット
 *
 *  @param[in]  position  セットしたい立ち位置
 *  @param[in]  flag      セットするフラグ
 */
//============================================================================================
void    BTLV_EFFECT_SetMcssVanishFlag( BtlvMcssPos position, BTLV_MCSS_VANISH_FLAG flag )
{
  BTLV_MCSS_SetVanishFlag( bew->bmw, position, flag );
}

//============================================================================================
/**
 * @brief  エフェクトで使用されているカメラ管理構造体のポインタを取得
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
 * @brief  エフェクトで使用されているMCSS管理構造体のポインタを取得
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
 * @brief  エフェクトで使用されているVMHANDLE管理構造体のポインタを取得
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
 * @brief  エフェクトで使用されているGFL_TCBSYS管理構造体のポインタを取得
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
 * @brief  エフェクトで使用されているPALETTE_FADE_PTR管理構造体のポインタを取得
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
 * @brief  エフェクトで使用されているCLWK管理構造体のポインタを取得
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
 * @brief  エフェクトで使用されているBTLV_BG_WORK管理構造体のポインタを取得
 *
 * @retval bbw BTLV_BG_WORK管理構造体
 */
//============================================================================================
BTLV_BG_WORK *BTLV_EFFECT_GetBGWork( void )
{
  return bew->bbw;
}

//============================================================================================
/**
 * @brief  エフェクトで使用されている時間制限管理構造体のポインタを取得
 *
 * @retval bbw BTLV_TIMER_WORK管理構造体
 */
//============================================================================================
BTLV_TIMER_WORK*  BTLV_EFFECT_GetTimerWork( void )
{
  return bew->btw;
}

//============================================================================================
/**
 * @brief  バトルルールを取得
 *
 * @retval BtlRule
 */
//============================================================================================
BtlRule BTLV_EFFECT_GetBtlRule( void )
{
  return bew->besp.rule;
}

//============================================================================================
/**
 * @brief  マルチかどうかを取得
 *
 * @retval TRUE:マルチ  FALSE:マルチ以外
 */
//============================================================================================
BOOL BTLV_EFFECT_GetMulti( void )
{
  return bew->besp.multi;
}

//============================================================================================
/**
 * @brief  立ち位置を指定してTRTYPEを取得
 *
 * @param[in] pos 立ち位置
 *
 * @retval TRTYPE_
 */
//============================================================================================
int BTLV_EFFECT_GetTrType( int pos )
{
  GF_ASSERT( pos < 4 );
  return bew->besp.tr_type[ pos ];
}

//============================================================================================
/**
 * @brief  ゲージワークを取得
 *
 * @retval bgw
 */
//============================================================================================
BTLV_GAUGE_WORK* BTLV_EFFECT_GetGaugeWork( void )
{
  return bew->bgw;
}

//============================================================================================
/**
 *  @brief  VBlank関数
 */
//============================================================================================
static  void  BTLV_EFFECT_VBlank( GFL_TCB *tcb, void *work )
{
  GFL_CLACT_SYS_VBlankFunc();
  PaletteFadeTrans( bew->pfd );
}

//============================================================================================
/**
 *  @brief  ダメージエフェクトシーケンス（多重起動があるのでTCBで作成）
 */
//============================================================================================
static  void  BTLV_EFFECT_TCB_Damage( GFL_TCB *tcb, void *work )
{
  BTLV_EFFECT_DAMAGE_TCB *bedt = (BTLV_EFFECT_DAMAGE_TCB*)work;

  if( bedt->wait ){
    bedt->wait--;
    return;
  }
  switch( bedt->seq_no ){
  case 0:
    BTLV_MCSS_SetPaletteFade( bew->bmw, bedt->target, 16, 16, 0, bedt->color );
    bedt->wait = BTLV_EFFECT_BLINK_WAIT;
    bedt->seq_no = 1;
    break;
  case 1:
    BTLV_MCSS_SetVanishFlag( bew->bmw, bedt->target, BTLV_MCSS_VANISH_ON );
    bedt->wait = BTLV_EFFECT_BLINK_WAIT;
    bedt->seq_no = 2;
    break;
  case 2:
    bedt->seq_no = 0;
    BTLV_MCSS_SetVanishFlag( bew->bmw, bedt->target, BTLV_MCSS_VANISH_OFF );
    BTLV_MCSS_SetPaletteFade( bew->bmw, bedt->target, 0, 0, 0, 0x7fff );
    bedt->wait = BTLV_EFFECT_BLINK_WAIT;
    if( --bedt->work == 0 ){
      bew->tcb_execute_flag = 0;
      GFL_HEAP_FreeMemory( bedt );
      GFL_TCB_DeleteTask( tcb );
    }
    break;
  }
}

//============================================================================================
/**
 *  @brief  ダメージエフェクトシーケンス（多重起動があるのでTCBで作成）
 */
//============================================================================================
static  void  BTLV_EFFECT_TCB_Henge( GFL_TCB *tcb, void *work )
{
  BTLV_EFFECT_HENGE_TCB *beht = (BTLV_EFFECT_HENGE_TCB*)work;

  switch( beht->seq_no ){
  case 0:
    BTLV_MCSS_MoveAlpha( bew->bmw, beht->vpos, EFFTOOL_CALCTYPE_DIRECT, 16, 0, 0, 0 );
    BTLV_MCSS_MoveMosaic( bew->bmw, beht->vpos, EFFTOOL_CALCTYPE_INTERPOLATION, 8, 8, 1, 0 );
    beht->seq_no++;
    break;
  case 1:
    if( !BTLV_MCSS_CheckTCBExecute( bew->bmw, beht->vpos ) )
    {
      BTLV_MCSS_OverwriteMAW( bew->bmw, beht->vpos, &beht->maw );
      beht->seq_no++;
    }
    break;
  case 2:
    BTLV_MCSS_MoveMosaic( bew->bmw, beht->vpos, EFFTOOL_CALCTYPE_INTERPOLATION, 0, 8, 1, 0 );
    beht->seq_no++;
    break;
  case 3:
    if( !BTLV_MCSS_CheckTCBExecute( bew->bmw, beht->vpos ) )
    {
      BTLV_MCSS_MoveAlpha( bew->bmw, beht->vpos, EFFTOOL_CALCTYPE_DIRECT, 31, 0, 0, 0 );
      bew->tcb_execute_flag = 0;
      GFL_HEAP_FreeMemory( beht );
      GFL_TCB_DeleteTask( tcb );
    }
    break;
  }
}

//============================================================================================
/**
 *  @brief  ローテーションアニメ
 */
//============================================================================================
static  void  TCB_BTLV_EFFECT_Rotation( GFL_TCB *tcb, void *work )
{
  TCB_ROTATION *tr = ( TCB_ROTATION* )work;

  switch( tr->seq_no ){
  case 0:
    BTLV_STAGE_SetAnmReq( bew->bsw, tr->side, 0, ( ( tr->dir == 0 ) ? -FX32_ONE : FX32_ONE ), 100 );
    BTLV_MCSS_SetRotation( bew->bmw, tr->side, tr->dir );
    tr->seq_no++;
    break;
  case 1:
    if( ( BTLV_STAGE_CheckExecuteAnmReq( bew->bsw ) == FALSE ) &&
        ( BTLV_MCSS_CheckTCBExecuteAllPos( bew->bmw ) == FALSE ) )
    {
      bew->tcb_execute_flag = 0;
      GFL_HEAP_FreeMemory( tr );
      GFL_TCB_DeleteTask( tcb );
    }
    break;
  }
}
#ifdef PM_DEBUG
//============================================================================================
/**
 * @brief  指定された立ち位置にポケモンをセット
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
