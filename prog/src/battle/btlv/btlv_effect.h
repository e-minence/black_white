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

#include "btlv_efftool.h"
#include "btlv_effvm.h"

#include "waza_tool/wazadata.h"
#include "system/palanm.h"

#include  "waza_tool/wazano_def.h"


//戦闘エフェクト定義
enum{
  BTLEFF_SINGLE_ENCOUNT_1 = WAZANO_MAX + 1,
  BTLEFF_SINGLE_ENCOUNT_2_MALE,
  BTLEFF_SINGLE_ENCOUNT_2_FEMALE,
  BTLEFF_SINGLE_ENCOUNT_3,
  BTLEFF_CAMERA_WORK,
  BTLEFF_CAMERA_INIT,
  BTLEFF_SINGLE_TRAINER_ENCOUNT_1,
  BTLEFF_SINGLE_TRAINER_ENCOUNT_2,
  BTLEFF_SINGLE_TRAINER_ENCOUNT_3,
};

typedef struct _BTLV_EFFECT_WORK BTLV_EFFECT_WORK;


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

extern  void              BTLV_EFFECT_Init( int index, HEAPID heapID );
extern  void              BTLV_EFFECT_Exit( void );
extern  void              BTLV_EFFECT_Main( void );
extern  void              BTLV_EFFECT_Add( int eff_no );
extern  void              BTLV_EFFECT_AddByPos( BtlvMcssPos pos, int eff_no );
extern  void              BTLV_EFFECT_AddWazaEffect( const BTLV_WAZAEFFECT_PARAM* param );
extern  void              BTLV_EFFECT_Stop( void );

//暫定でTCBで作成したエフェクト
extern  void              BTLV_EFFECT_Damage( BtlvMcssPos target );

extern  BOOL              BTLV_EFFECT_CheckExecute( void );
extern  void              BTLV_EFFECT_SetPokemon( const POKEMON_PARAM *pp, int position );
extern  void              BTLV_EFFECT_DelPokemon( int position );
extern  BOOL              BTLV_EFFECT_CheckExistPokemon( int position );
extern  void              BTLV_EFFECT_SetTrainer( int trtype, int position, int pos_x, int pos_y, int pos_z );
extern  void              BTLV_EFFECT_DelTrainer( int position );
extern  void              BTLV_EFFECT_SetGauge( const BTL_POKEPARAM* bpp, int position );
extern  void              BTLV_EFFECT_DelGauge( int position );
extern  void              BTLV_EFFECT_CalcGauge( int position, int value );
extern  BOOL              BTLV_EFFECT_CheckExecuteGauge( void );
extern  void              BTLV_EFFECT_SetGaugeDrawEnable( BOOL on_off );
extern  void              BTLV_EFFECT_SetBallGauge( const BTLV_BALL_GAUGE_PARAM* bbgp );
extern  void              BTLV_EFFECT_DelBallGauge( BTLV_BALL_GAUGE_TYPE type );
extern  BOOL              BTLV_EFFECT_CheckExecuteBallGauge( BTLV_BALL_GAUGE_TYPE type );
extern  void              BTLV_EFFECT_SetPaletteFade( int model, u8 start_evy, u8 end_evy, u8 wait, u16 rgb );
extern  BOOL              BTLV_EFFECT_CheckExecutePaletteFade( int model );
extern  void              BTLV_EFFECT_SetVanishFlag( int model, int flag );
extern  int               BTLV_EFFECT_GetTrainerIndex( int position );
extern  BTLV_CAMERA_WORK* BTLV_EFFECT_GetCameraWork( void );
extern  BTLV_MCSS_WORK*   BTLV_EFFECT_GetMcssWork( void );
extern  VMHANDLE*         BTLV_EFFECT_GetVMHandle( void );
extern  GFL_TCBSYS*       BTLV_EFFECT_GetTCBSYS( void );
extern  PALETTE_FADE_PTR  BTLV_EFFECT_GetPfd( void );
extern  BTLV_CLACT_WORK*  BTLV_EFFECT_GetCLWK( void );

#ifdef PM_DEBUG
extern  void        BTLV_EFFECT_SetPokemonDebug( const MCSS_ADD_DEBUG_WORK *madw, int position );
#endif

//管理構造体のポインタを返すバージョン
#if 0
extern  BTLV_EFFECT_WORK  *BTLV_EFFECT_Init( int index, HEAPID heapID );
extern  void      BTLV_EFFECT_Exit( BTLV_EFFECT_WORK *bew );
extern  void      BTLV_EFFECT_Main( BTLV_EFFECT_WORK *bew );
extern  void      BTLV_EFFECT_Add( BTLV_EFFECT_WORK *bew, int eff_no );
extern  BOOL      BTLV_EFFECT_CheckExecute( BTLV_EFFECT_WORK *bew );
extern  void      BTLV_EFFECT_SetPokemon( BTLV_EFFECT_WORK *bew, POKEMON_PARAM *pp, int position );
extern  BTLV_CAMERA_WORK  *BTLV_EFFECT_GetCameraWork( BTLV_EFFECT_WORK *bew );
#endif
