//============================================================================================
/**
 * @file  btlv_effect.h
 * @brief �퓬�G�t�F�N�g����
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


//�퓬�G�t�F�N�g��`
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
 *  �^�[����ގw��
 */
//--------------------------------------------
typedef enum {
  BTLV_WAZAEFF_TURN_DEFAULT = 0,
  BTLV_WAZAEFF_TURN_TAME,
}BtlvWazaEffect_TurnType;

//--------------------------------------------
/**
 *  ���U�G�t�F�N�g�Ăяo���p�����[�^
 */
//--------------------------------------------
typedef struct {
  WazaID       waza;            ///< ���U�i���o�[
  BtlvMcssPos  from;            ///< ���U���o���|�P�ʒu
  BtlvMcssPos  to;              ///< ���U���󂯂�|�P�ʒu�i�s�v�̏ꍇ BTLV_MCSS_POS_ERROR ���w��j
  u8           turn_count;      ///< �^�[���ɂ���ĈقȂ�G�t�F�N�g���o���ꍇ�̃^�[���w��B�iex.������Ƃԁj
  u8           continue_count;  ///< �A�����ďo���ƃG�t�F�N�g���قȂ�ꍇ�̘A���J�E���^�iex. ���낪��j
}BTLV_WAZAEFFECT_PARAM;

extern  void              BTLV_EFFECT_Init( int index, HEAPID heapID );
extern  void              BTLV_EFFECT_Exit( void );
extern  void              BTLV_EFFECT_Main( void );
extern  void              BTLV_EFFECT_Add( int eff_no );
extern  void              BTLV_EFFECT_AddByPos( BtlvMcssPos pos, int eff_no );
extern  void              BTLV_EFFECT_AddWazaEffect( const BTLV_WAZAEFFECT_PARAM* param );
extern  void              BTLV_EFFECT_Stop( void );

//�b���TCB�ō쐬�����G�t�F�N�g
extern  void              BTLV_EFFECT_Damage( BtlvMcssPos target );

extern  BOOL              BTLV_EFFECT_CheckExecute( void );
extern  void              BTLV_EFFECT_SetPokemon( const POKEMON_PARAM *pp, int position );
extern  void              BTLV_EFFECT_DelPokemon( int position );
extern  BOOL              BTLV_EFFECT_CheckExistPokemon( int position );
extern  void              BTLV_EFFECT_SetTrainer( int trtype, int position, int pos_x, int pos_y, int pos_z );
extern  void              BTLV_EFFECT_DelTrainer( int position );
extern  void              BTLV_EFFECT_SetGauge( const POKEMON_PARAM* pp, int position );
extern  void              BTLV_EFFECT_DelGauge( int position );
extern  void              BTLV_EFFECT_CalcGauge( int position, int value );
extern  BOOL              BTLV_EFFECT_CheckExecuteGauge( void );
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

//�Ǘ��\���̂̃|�C���^��Ԃ��o�[�W����
#if 0
extern  BTLV_EFFECT_WORK  *BTLV_EFFECT_Init( int index, HEAPID heapID );
extern  void      BTLV_EFFECT_Exit( BTLV_EFFECT_WORK *bew );
extern  void      BTLV_EFFECT_Main( BTLV_EFFECT_WORK *bew );
extern  void      BTLV_EFFECT_Add( BTLV_EFFECT_WORK *bew, int eff_no );
extern  BOOL      BTLV_EFFECT_CheckExecute( BTLV_EFFECT_WORK *bew );
extern  void      BTLV_EFFECT_SetPokemon( BTLV_EFFECT_WORK *bew, POKEMON_PARAM *pp, int position );
extern  BTLV_CAMERA_WORK  *BTLV_EFFECT_GetCameraWork( BTLV_EFFECT_WORK *bew );
#endif
