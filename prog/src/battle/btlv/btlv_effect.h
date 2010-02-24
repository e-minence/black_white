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
#include "btlv_timer.h"
#include "btlv_bg.h"

#include "btlv_efftool.h"
#include "btlv_effvm.h"

#include "battle/battle.h"

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
  BTLEFF_BALL_THROW,
  BTLEFF_HINSHI,
  BTLEFF_SINGLE_ENCOUNT_2_ARARAGI,

  //�X�e�[�^�X�G�t�F�N�g
  BTLEFF_NEMURI,                //�˂ނ�
  BTLEFF_DOKU,                  //�ǂ�
  BTLEFF_YAKEDO,                //�₯��
  BTLEFF_KOORI,                 //������
  BTLEFF_MAHI,                  //�܂�
  BTLEFF_KONRAN,                //������
  BTLEFF_MEROMERO,              //��������
  BTLEFF_LVUP,                  //���x���A�b�v
  BTLEFF_USE_ITEM,              //�|�P�����ɃA�C�e�����g�p
  BTLEFF_SOUBI_ITEM,            //��������� 10
  BTLEFF_RARE,                  //���A�G�t�F�N�g
  BTLEFF_STATUS_UP,             //�X�e�[�^�X�㏸
  BTLEFF_STATUS_DOWN,           //�X�e�[�^�X���~
  BTLEFF_HP_RECOVER,            //HP��
  BTLEFF_MIGAWARI_WAZA_BEFORE,  //�݂���莞�ɋZ���J��o���O�G�t�F�N�g
  BTLEFF_MIGAWARI_WAZA_AFTER,   //�݂���莞�ɋZ���J��o������G�t�F�N�g
  BTLEFF_ITEM_ESCAPE,           //�G�l�R�̂�����or�s�b�s�l�`
  BTLEFF_WEATHER_KIRI,          //����i�V���`�ł͂Ȃ������H�j
  BTLEFF_WEATHER_AME,           //����
  BTLEFF_WEATHER_ARARE,         //�����  20
  BTLEFF_WEATHER_SUNAARASHI,    //���Ȃ��炵
  BTLEFF_WEATHER_HARE,          //�͂�
  BTLEFF_POKEMON_MODOSU,        //�|�P�����������߂�G�t�F�N�g
  BTLEFF_POKEMON_KURIDASU,      //�|�P�����J��o���G�t�F�N�g
  BTLEFF_MIGAWARI_FALL,         //�݂���藎���Ă���
  BTLEFF_MIGAWARI_DEAD,         //�݂�������
//  BTLEFF_FUN,                 //
//  BTLEFF_EAT,                 //
//  BTLEFF_ANGRY,               //
  BTLEFF_NOROI,                 //�̂낢  30
  BTLEFF_AKUMU,                 //������
  BTLEFF_YADORIGI,              //��ǂ肬 
  BTLEFF_SIMETUKERU,            //���߂���
  BTLEFF_MAKITUKU,              //�܂���
  BTLEFF_HONOONOUZU,            //�ق̂��̂���
  BTLEFF_MAGUMASUTOOMU,         //�}�O�}�X�g�[��
  BTLEFF_KARADEHASAMU,          //����ł͂���
  BTLEFF_UZUSIO,                //��������
  BTLEFF_SUNAZIGOKU,            //���Ȃ�����
  BTLEFF_NEWOHARU,              //�˂��͂�
};

typedef struct _BTLV_EFFECT_WORK BTLV_EFFECT_WORK;

//OBJ�p���b�g�g�p����
enum{
  BTLV_OBJ_PLTT_HP_GAUGE    = 0x20 * 0,   //0-5:HP�Q�[�W�i6�{���j
  BTLV_OBJ_PLTT_TIMER       = 0x20 * 6,   //6:�^�C�}�[
  BTLV_OBJ_PLTT_STATUS_ICON = 0x20 * 7,   //7:��Ԉُ�A�C�R��
  BTLV_OBJ_PLTT_CLACT       = 0x20 * 8,   //8:BTLV_CLACT�Ŏg�p����Z���A�N�^�[�i8���j
};

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


typedef enum{
  BTLV_EFFECT_ROTATE_DIR_LEFT = 0,
  BTLV_EFFECT_ROTATE_DIR_RIGHT,
}BTLV_EFFECT_ROTATE_DIR;

extern  void              BTLV_EFFECT_Init( BtlRule rule, const BTL_FIELD_SITUATION *bfs, GFL_FONT* fontHandle, HEAPID heapID );
extern  void              BTLV_EFFECT_Exit( void );
extern  void              BTLV_EFFECT_Main( void );
extern  void              BTLV_EFFECT_Add( int eff_no );
extern  void              BTLV_EFFECT_AddByPos( BtlvMcssPos pos, int eff_no );
extern  void              BTLV_EFFECT_AddByDir( BtlvMcssPos from, BtlvMcssPos to, int eff_no );
extern  void              BTLV_EFFECT_AddWazaEffect( const BTLV_WAZAEFFECT_PARAM* param );
extern  void              BTLV_EFFECT_Stop( void );

extern  void              BTLV_EFFECT_Damage( BtlvMcssPos target, WazaID waza );
extern  void              BTLV_EFFECT_Hinshi( BtlvMcssPos target );

extern  BOOL              BTLV_EFFECT_CheckExecute( void );
extern  void              BTLV_EFFECT_SetPokemon( const POKEMON_PARAM *pp, int position );
extern  void              BTLV_EFFECT_DelPokemon( int position );
extern  BOOL              BTLV_EFFECT_CheckExist( int position );
extern  void              BTLV_EFFECT_SetTrainer( int trtype, int position, int pos_x, int pos_y, int pos_z );
extern  void              BTLV_EFFECT_DelTrainer( int position );
extern  void              BTLV_EFFECT_SetGauge( const BTL_MAIN_MODULE* wk, const BTL_POKEPARAM* bpp, int position );
extern  void              BTLV_EFFECT_DelGauge( int position );
extern  void              BTLV_EFFECT_CalcGaugeHP( int position, int value );
extern  void              BTLV_EFFECT_CalcGaugeEXP( int position, int value );
extern  void              BTLV_EFFECT_CalcGaugeEXPLevelUp( int position, const BTL_POKEPARAM* bpp );
extern  void              BTLV_EFFECT_BallThrow( int position, u16 item_no, u8 yure_cnt, BOOL f_success );
extern  BOOL              BTLV_EFFECT_CheckExecuteGauge( void );
extern  void              BTLV_EFFECT_SetGaugeDrawEnable( BOOL on_off );
extern  void              BTLV_EFFECT_SetGaugeStatus( PokeSick sick,  BtlvMcssPos pos );
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

#ifdef PM_DEBUG
extern  void        BTLV_EFFECT_SetPokemonDebug( const MCSS_ADD_DEBUG_WORK *madw, int position );
#endif

