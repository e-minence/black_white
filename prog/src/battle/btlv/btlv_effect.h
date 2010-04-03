//============================================================================================
/**
 * @file  btlv_effect.h
 * @brief 戦闘エフェクト制御
 * @author  soga
 * @date  2008.11.21
 */
//============================================================================================

#pragma once

#include "btlv_stage.h"
#include "btlv_field.h"
#include "btlv_camera.h"
#include "btlv_mcss.h"
#include "btlv_clact.h"
#include "btlv_gauge.h"
#include "btlv_b_gauge.h"
#include "btlv_timer.h"
#include "btlv_bg.h"
#include "btlv_scu_def.h"

#include "btlv_efftool.h"
#include "btlv_effvm.h"

#include "battle/battle.h"

#include "waza_tool/wazadata.h"
#include "system/palanm.h"

#include  "waza_tool/wazano_def.h"

//戦闘エフェクト定義
enum{
  BTLEFF_SINGLE_ENCOUNT_1 = WAZANO_MAX + 1,
  BTLEFF_SINGLE_ENCOUNT_2_SOLO,
  BTLEFF_SINGLE_ENCOUNT_2_TAG,
  BTLEFF_SINGLE_ENCOUNT_3,
  BTLEFF_CAMERA_WORK,
  BTLEFF_CAMERA_INIT,
  BTLEFF_SINGLE_TRAINER_ENCOUNT_1,
  BTLEFF_SINGLE_TRAINER_ENCOUNT_2,
  BTLEFF_SINGLE_TRAINER_ENCOUNT_3,
  BTLEFF_BALL_THROW,
  BTLEFF_HINSHI,
  BTLEFF_SINGLE_ENCOUNT_2_ARARAGI,
  BTLEFF_BALL_THROW_TRAINER,
  BTLEFF_PDC_ENCOUNT,

  //ステータスエフェクト
  BTLEFF_STATUS_EFFECT_START,
  BTLEFF_NEMURI = BTLEFF_STATUS_EFFECT_START, //ねむり
  BTLEFF_DOKU,                                //どく
  BTLEFF_YAKEDO,                              //やけど
  BTLEFF_KOORI,                               //こおり
  BTLEFF_MAHI,                                //まひ
  BTLEFF_KONRAN,                              //こんらん
  BTLEFF_MEROMERO,                            //メロメロ
  BTLEFF_LVUP,                                //レベルアップ
  BTLEFF_USE_ITEM,                            //ポケモンにアイテムを使用
  BTLEFF_SOUBI_ITEM,                          //装備道具発動 10
  BTLEFF_RARE,                                //レアエフェクト
  BTLEFF_STATUS_UP,                           //ステータス上昇
  BTLEFF_STATUS_DOWN,                         //ステータス下降
  BTLEFF_HP_RECOVER,                          //HP回復
  BTLEFF_MIGAWARI_WAZA_BEFORE,                //みがわり時に技を繰り出す前エフェクト
  BTLEFF_MIGAWARI_WAZA_AFTER,                 //みがわり時に技を繰り出した後エフェクト
  BTLEFF_ITEM_ESCAPE,                         //エネコのしっぽorピッピ人形
  BTLEFF_WEATHER_KIRI,                        //きり（シャチではないかも？）
  BTLEFF_WEATHER_AME,                         //あめ
  BTLEFF_WEATHER_ARARE,                       //あられ  20
  BTLEFF_WEATHER_SUNAARASHI,                  //すなあらし
  BTLEFF_WEATHER_HARE,                        //はれ
  BTLEFF_POKEMON_MODOSU,                      //ポケモン引っ込めるエフェクト
  BTLEFF_POKEMON_KURIDASU,                    //ポケモン繰り出しエフェクト
  BTLEFF_MIGAWARI_FALL,                       //みがわり落ちてくる
  BTLEFF_MIGAWARI_DEAD,                       //みがわり消滅
  BTLEFF_TRAINER_IN,                          //トレーナーフレームイン
  BTLEFF_TRAINER_OUT,                         //トレーナーフレームアウト
//  BTLEFF_ANGRY,                             //
  BTLEFF_NOROI,                               //のろい  30
  BTLEFF_AKUMU,                               //あくむ
  BTLEFF_YADORIGI,                            //やどりぎ
  BTLEFF_SIMETUKERU,                          //しめつける
  BTLEFF_MAKITUKU,                            //まきつく
  BTLEFF_HONOONOUZU,                          //ほのおのうず
  BTLEFF_MAGUMASUTOOMU,                       //マグマストーム
  BTLEFF_KARADEHASAMU,                        //からではさむ
  BTLEFF_UZUSIO,                              //うずしお
  BTLEFF_SUNAZIGOKU,                          //すなじごく
  BTLEFF_NEWOHARU,                            //ねをはる

  BTLEFF_STATUS_EFFECT_END,
};

typedef struct _BTLV_EFFECT_SETUP_PARAM BTLV_EFFECT_SETUP_PARAM;
typedef struct _BTLV_EFFECT_WORK        BTLV_EFFECT_WORK;

//OBJパレット使用内訳
enum{
  BTLV_OBJ_PLTT_HP_GAUGE    = 0x20 * 0,   //0-5:HPゲージ（6本分）
  BTLV_OBJ_PLTT_BALL_GAUGE  = 0x20 * 6,   //6:ボールゲージ
  BTLV_OBJ_PLTT_TIMER       = 0x20 * 7,   //7:タイマー
  BTLV_OBJ_PLTT_STATUS_ICON = 0x20 * 8,   //8:状態異常アイコン
  BTLV_OBJ_PLTT_CLACT       = 0x20 * 9,   //9:BTLV_CLACTで使用するセルアクター（7個分）
};

//--------------------------------------------
/**
 *  ターン種類指定
 */
//--------------------------------------------
typedef enum {
  BTLV_WAZAEFF_TURN_DEFAULT = 0,
  BTLV_WAZAEFF_TURN_TAME,
}BtlvWazaEffect_TurnType;

//--------------------------------------------
/**
 *  ワザエフェクト呼び出しパラメータ
 */
//--------------------------------------------
typedef struct {
  WazaID       waza;            ///< ワザナンバー
  BtlvMcssPos  from;            ///< ワザを出すポケ位置
  BtlvMcssPos  to;              ///< ワザを受けるポケ位置（不要の場合 BTLV_MCSS_POS_ERROR を指定）
  u8           turn_count;      ///< ターンによって異なるエフェクトを出す場合のターン指定。（ex.そらをとぶ）
  u8           continue_count;  ///< 連続して出すとエフェクトが異なる場合の連続カウンタ（ex. ころがる）
}BTLV_WAZAEFFECT_PARAM;


typedef enum{
  BTLV_EFFECT_ROTATE_DIR_LEFT = 0,
  BTLV_EFFECT_ROTATE_DIR_RIGHT,
}BTLV_EFFECT_ROTATE_DIR;

extern  BTLV_EFFECT_SETUP_PARAM*  BTLV_EFFECT_MakeSetUpParam( BtlRule rule, const BTL_FIELD_SITUATION* bfs,
                                                              BOOL multi, u16* tr_type, const BTLV_SCU* scu, HEAPID heapID );

extern  BTLV_EFFECT_SETUP_PARAM*  BTLV_EFFECT_MakeSetUpParamBtl( const BTL_MAIN_MODULE* mainModule, const BTLV_SCU* viewSCU, HEAPID heapID );

extern  void              BTLV_EFFECT_Init( BTLV_EFFECT_SETUP_PARAM* besp, GFL_FONT* fontHandle, HEAPID heapID );
extern  void              BTLV_EFFECT_Exit( void );
extern  void              BTLV_EFFECT_Main( void );
extern  void              BTLV_EFFECT_Add( int eff_no );
extern  void              BTLV_EFFECT_AddByPos( BtlvMcssPos pos, int eff_no );
extern  void              BTLV_EFFECT_AddByDir( BtlvMcssPos from, BtlvMcssPos to, int eff_no );
extern  void              BTLV_EFFECT_AddWazaEffect( const BTLV_WAZAEFFECT_PARAM* param );
extern  void              BTLV_EFFECT_Stop( void );
extern  void              BTLV_EFFECT_Restart( void );

extern  void              BTLV_EFFECT_Damage( BtlvMcssPos target, WazaID waza );
extern  void              BTLV_EFFECT_Hinshi( BtlvMcssPos target );

extern  BOOL              BTLV_EFFECT_CheckExecute( void );
extern  void              BTLV_EFFECT_SetPokemon( const POKEMON_PARAM *pp, int position );
extern  void              BTLV_EFFECT_DelPokemon( int position );
extern  BOOL              BTLV_EFFECT_CheckExist( int position );
extern  void              BTLV_EFFECT_SetTrainer( int trtype, int position, int pos_x, int pos_y, int pos_z );
extern  void              BTLV_EFFECT_DelTrainer( int position );
extern  void              BTLV_EFFECT_SetGauge( const BTL_MAIN_MODULE* wk, const BTL_POKEPARAM* bpp, int position );
extern  void              BTLV_EFFECT_SetGaugePP( const ZUKAN_SAVEDATA* zs, const POKEMON_PARAM* pp, int position );
extern  void              BTLV_EFFECT_DelGauge( int position );
extern  void              BTLV_EFFECT_CalcGaugeHP( int position, int value );
extern  void              BTLV_EFFECT_CalcGaugeHPAtOnce( int position, int value );
extern  void              BTLV_EFFECT_CalcGaugeEXP( int position, int value );
extern  void              BTLV_EFFECT_CalcGaugeEXPLevelUp( int position, const BTL_POKEPARAM* bpp );
extern  void              BTLV_EFFECT_BallThrow( int position, u16 item_no, u8 yure_cnt, BOOL f_success, BOOL f_critical );
extern  void              BTLV_EFFECT_BallThrowTrainer( int vpos, u16 item_no );
extern  void              BTLV_EFFECT_Henge( const POKEMON_PARAM* pp, BtlvMcssPos vpos );
extern  BOOL              BTLV_EFFECT_CheckExecuteGauge( void );
extern  void              BTLV_EFFECT_SetGaugeDrawEnable( BOOL on_off );
extern  void              BTLV_EFFECT_SetGaugeStatus( PokeSick sick,  BtlvMcssPos pos );
extern  void              BTLV_EFFECT_SetGaugeYure( BtlvMcssPos pos );
extern  void              BTLV_EFFECT_SetBallGauge( const BTLV_BALL_GAUGE_PARAM* bbgp );
extern  void              BTLV_EFFECT_DelBallGauge( BTLV_BALL_GAUGE_TYPE type );
extern  BOOL              BTLV_EFFECT_CheckExecuteBallGauge( BTLV_BALL_GAUGE_TYPE type );
extern  void              BTLV_EFFECT_SetPaletteFade( int model, u8 start_evy, u8 end_evy, u8 wait, u16 rgb );
extern  BOOL              BTLV_EFFECT_CheckExecutePaletteFade( int model );
extern  void              BTLV_EFFECT_SetVanishFlag( int model, int flag );
extern  void              BTLV_EFFECT_SetRotateEffect( BtlRotateDir dir, int side );
extern  int               BTLV_EFFECT_GetTrainerIndex( int position );
extern  void              BTLV_EFFECT_CreateTimer( int game_time, int command_time );
extern  void              BTLV_EFFECT_DrawEnableTimer( BTLV_TIMER_TYPE type, BOOL enable, BOOL init );
extern  BOOL              BTLV_EFFECT_IsZero( BTLV_TIMER_TYPE type );
extern  BTLV_MCSS_VANISH_FLAG BTLV_EFFECT_GetMcssVanishFlag( BtlvMcssPos position );
extern  void              BTLV_EFFECT_SetMcssVanishFlag( BtlvMcssPos position, BTLV_MCSS_VANISH_FLAG flag );
extern  BTLV_CAMERA_WORK* BTLV_EFFECT_GetCameraWork( void );
extern  BTLV_MCSS_WORK*   BTLV_EFFECT_GetMcssWork( void );
extern  VMHANDLE*         BTLV_EFFECT_GetVMHandle( void );
extern  GFL_TCBSYS*       BTLV_EFFECT_GetTCBSYS( void );
extern  PALETTE_FADE_PTR  BTLV_EFFECT_GetPfd( void );
extern  BTLV_CLACT_WORK*  BTLV_EFFECT_GetCLWK( void );
extern  BTLV_BG_WORK*     BTLV_EFFECT_GetBGWork( void );
extern  BTLV_TIMER_WORK*  BTLV_EFFECT_GetTimerWork( void );
extern  BtlRule           BTLV_EFFECT_GetBtlRule( void );
extern  BOOL              BTLV_EFFECT_GetMulti( void );
extern  int               BTLV_EFFECT_GetTrType( int pos );
extern  const BTLV_SCU*   BTLV_EFFECT_GetScu( void );
extern  BTLV_GAUGE_WORK*  BTLV_EFFECT_GetGaugeWork( void );

#ifdef PM_DEBUG
extern  void        BTLV_EFFECT_SetPokemonDebug( const MCSS_ADD_DEBUG_WORK *madw, int position );
#endif

