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
#include "btlv_scu_def.h"

#include "btlv_efftool.h"
#include "btlv_effvm.h"

#include "battle/battle.h"
#include "battle/app/b_bag.h"

#include "waza_tool/wazadata.h"
#include "system/palanm.h"

#include  "waza_tool/wazano_def.h"

//�퓬�G�t�F�N�g��`
enum{
  BTLEFF_SINGLE_ENCOUNT_1 = WAZANO_MAX + 1,   //561
  BTLEFF_SINGLE_ENCOUNT_2_SOLO,
  BTLEFF_SINGLE_ENCOUNT_2_TAG,
  BTLEFF_SINGLE_ENCOUNT_3,
  BTLEFF_CAMERA_WORK,
  BTLEFF_CAMERA_INIT,
  BTLEFF_SINGLE_TRAINER_ENCOUNT_1,
  BTLEFF_SINGLE_TRAINER_ENCOUNT_2,
  BTLEFF_SINGLE_TRAINER_ENCOUNT_3,
  BTLEFF_BALL_THROW,                          //570
  BTLEFF_HINSHI,
  BTLEFF_SINGLE_ENCOUNT_2_ARARAGI,
  BTLEFF_BALL_THROW_TRAINER,
  BTLEFF_PDC_ENCOUNT,
  BTLEFF_1vs1_POS_AA_FOCUS,
  BTLEFF_2vs2_POS_A_FOCUS,
  BTLEFF_2vs2_POS_C_FOCUS,
  BTLEFF_3vs3_POS_A_FOCUS,
  BTLEFF_3vs3_POS_E_FOCUS,
  BTLEFF_3vs3_CAMERA_ZOOMOUT,                 //580
  BTLEFF_ROTATION_POS_A_FOCUS,
  BTLEFF_ROTATION_POS_A_TO_C,
  BTLEFF_ROTATION_POS_A_TO_E,
  BTLEFF_ROTATION_POS_C_TO_A,
  BTLEFF_ROTATION_POS_E_TO_A,
  BTLEFF_WCS_CAMERA_WORK_E_M,
  BTLEFF_WCS_CAMERA_WORK_M_E,
  BTLEFF_CAMERA_WORK_ROTATE_R_L,
  BTLEFF_CAMERA_WORK_ROTATE_L_R,
  BTLEFF_CAMERA_WORK_UPDOWN_E_M,              //590
  BTLEFF_CAMERA_WORK_UPDOWN_M_E,
  BTLEFF_CAMERA_WORK_TRIANGLE,
  BTLEFF_CAMERA_WORK_WCS_INIT,
  BTLEFF_CAMERA_WORK_INIT,
  BTLEFF_ZOOM_IN_ORTHO, 
  BTLEFF_CAMERA_INIT_ORTHO, 

  //�X�e�[�^�X�G�t�F�N�g
  BTLEFF_STATUS_EFFECT_START,
  BTLEFF_NEMURI = BTLEFF_STATUS_EFFECT_START, //�˂ނ� 
  BTLEFF_DOKU,                                //�ǂ�
  BTLEFF_YAKEDO,                              //�₯��
  BTLEFF_KOORI,                               //������  600
  BTLEFF_MAHI,                                //�܂�
  BTLEFF_KONRAN,                              //������
  BTLEFF_MEROMERO,                            //��������
  BTLEFF_LVUP,                                //���x���A�b�v
  BTLEFF_USE_ITEM,                            //�|�P�����ɃA�C�e�����g�p
  BTLEFF_SOUBI_ITEM,                          //��������� 10
  BTLEFF_RARE,                                //���A�G�t�F�N�g
  BTLEFF_STATUS_UP,                           //�X�e�[�^�X�㏸
  BTLEFF_STATUS_DOWN,                         //�X�e�[�^�X���~
  BTLEFF_HP_RECOVER,                          //HP��  610
  BTLEFF_MIGAWARI_WAZA_BEFORE,                //�݂���莞�ɋZ���J��o���O�G�t�F�N�g
  BTLEFF_MIGAWARI_WAZA_AFTER,                 //�݂���莞�ɋZ���J��o������G�t�F�N�g
  BTLEFF_ITEM_ESCAPE,                         //�G�l�R�̂�����or�s�b�s�l�`
  BTLEFF_WEATHER_KIRI,                        //����i�V���`�ł͂Ȃ������H�j
  BTLEFF_WEATHER_AME,                         //����
  BTLEFF_WEATHER_ARARE,                       //�����  20
  BTLEFF_WEATHER_SUNAARASHI,                  //���Ȃ��炵
  BTLEFF_WEATHER_HARE,                        //�͂�
  BTLEFF_POKEMON_MODOSU,                      //�|�P�����������߂�G�t�F�N�g
  BTLEFF_POKEMON_KURIDASU,                    //�|�P�����J��o���G�t�F�N�g  620
  BTLEFF_MIGAWARI_FALL,                       //�݂���藎���Ă���
  BTLEFF_MIGAWARI_DEAD,                       //�݂�������
  BTLEFF_TRAINER_IN,                          //�g���[�i�[�t���[���C��
  BTLEFF_TRAINER_OUT,                         //�g���[�i�[�t���[���A�E�g
  BTLEFF_KIAIPUNCH_TAME,                      //�������p���`�̃^���G�t�F�N�g
  BTLEFF_NOROI,                               //�̂낢  30
  BTLEFF_AKUMU,                               //������
  BTLEFF_YADORIGI,                            //��ǂ肬
  BTLEFF_SIMETUKERU,                          //���߂���
  BTLEFF_MAKITUKU,                            //�܂���
  BTLEFF_HONOONOUZU,                          //�ق̂��̂���
  BTLEFF_MAGUMASUTOOMU,                       //�}�O�}�X�g�[��
  BTLEFF_KARADEHASAMU,                        //����ł͂���  630
  BTLEFF_UZUSIO,                              //��������
  BTLEFF_SUNAZIGOKU,                          //���Ȃ�����
  BTLEFF_NEWOHARU,                            //�˂��͂�  40
  BTLEFF_ZOOM_IN,                             //�J�����Y�[���C��  634
  BTLEFF_IYASINONEGAI_KAIHUKU,                //���₵�̂˂����񕜃G�t�F�N�g
  BTLEFF_MIKADUKINOMAI_KAIHUKU,               //�݂��Â��̂܂��񕜃G�t�F�N�g
  BTLEFF_RESET_MOVE,                          //���Z�b�g���[�u�G�t�F�N�g
  BTLEFF_SHOOTER_EFFECT,                      //�V���[�^�[�G�t�F�N�g
  BTLEFF_TONBOGAERI_RETURN,                   //�Ƃ�ڂ�����߂�G�t�F�N�g
  BTLEFF_VOLTCHANGE_RETURN,                   //�{���g�`�F���W�߂�G�t�F�N�g
  BTLEFF_POKEMON_VANISH_ON,                   //�|�P�����o�j�b�V��ON
  BTLEFF_POKEMON_VANISH_OFF,                  //�|�P�����o�j�b�V��OFF
  BTLEFF_RESHUFFLE_FOCUS_DEFAULT,             //����ւ��J�����t�H�[�J�X�f�t�H���g 50
  BTLEFF_RESHUFFLE_FOCUS_2vs2_POS_A,          //����ւ��J�����t�H�[�J�X2vs2POS_A
  BTLEFF_RESHUFFLE_FOCUS_3vs3_POS_A,          //����ւ��J�����t�H�[�J�X3vs3POS_A
  BTLEFF_RESHUFFLE_FOCUS_3vs3_POS_E,          //����ւ��J�����t�H�[�J�X3vs3POS_E
  BTLEFF_TOMOENAGE_RETURN,                    //�Ƃ����Ȃ��߂�G�t�F�N�g
  BTLEFF_DRAGONTAIL_RETURN,                   //�h���S���e�[���߂�G�t�F�N�g
  BTLEFF_RAINBOW,                             //���G�t�F�N�g
  BTLEFF_BURNING,                             //�΂̊C�G�t�F�N�g
  BTLEFF_MOOR,                                //���n�G�t�F�N�g
  BTLEFF_DAMAGE,                              //�_���[�W�G�t�F�N�g

  BTLEFF_STATUS_EFFECT_END,
};

typedef struct _BTLV_EFFECT_SETUP_PARAM BTLV_EFFECT_SETUP_PARAM;
typedef struct _BTLV_EFFECT_WORK        BTLV_EFFECT_WORK;

typedef void  (BTLV_EFFECT_TCB_CALLBACK_FUNC)( GFL_TCB* tcb );

#define BTLV_EFFECT_GAUGE_YURE_STOP ( BTLV_MCSS_POS_MAX )

//OBJ�p���b�g�g�p����
enum{
  BTLV_OBJ_PLTT_HP_GAUGE    = 0x20 * 0,   //0-5:HP�Q�[�W�i6�{���j
  BTLV_OBJ_PLTT_BALL_GAUGE  = 0x20 * 6,   //6:�{�[���Q�[�W
  BTLV_OBJ_PLTT_TIMER       = 0x20 * 7,   //7:�^�C�}�[
  BTLV_OBJ_PLTT_STATUS_ICON = 0x20 * 8,   //8:��Ԉُ�A�C�R��
  BTLV_OBJ_PLTT_CLACT       = 0x20 * 9,   //9:BTLV_CLACT�Ŏg�p����Z���A�N�^�[�i7���j
};

//BTLV_EFFECT_SetTCB�Ŏg�p����O���[�v�w��
typedef enum{ 
  GROUP_DEFAULT = 0,
  GROUP_EFFVM,
  GROUP_MCSS,
  GROUP_CLACT,
}BTLV_EFFECT_TCB_GROUP;

typedef enum{ 
  BTLV_EFFECT_CWE_NONE = 0,
  BTLV_EFFECT_CWE_NORMAL,         //�ʏ퓮��
  BTLV_EFFECT_CWE_NO_STOP,        //���͂������Ă�CAMERA_INIT���Ă΂Ȃ�
}BTLV_EFFECT_CWE;

typedef enum{ 
  BTLV_EFFECT_SE_MODE_PLAY = 0, //SE�Đ�����
  BTLV_EFFECT_SE_MODE_MUTE,     //SE�Đ����Ȃ�
}BTLV_EFFECT_SE_MODE;

typedef enum{ 
  BTLV_EFFECT_REVERSE_DRAW_OFF  = BTLV_MCSS_REVERSE_DRAW_OFF,
  BTLV_EFFECT_REVERSE_DRAW_ON   = BTLV_MCSS_REVERSE_DRAW_ON,
}BTLV_EFFECT_REVERSE_DRAW;

//--------------------------------------------
/**
 *  �^�[����ގw��
 */
//--------------------------------------------
typedef enum {
  BTLV_WAZAEFF_INDEX_DEFAULT = 0,

  // ���߃��U�S�ʁi������ƂԂȂǁj
  BTLV_WAZAEFF_TAME_START = 0,      ///< ���ߊJ�n
  BTLV_WAZAEFF_TAME_RELEASE,        ///< ���߉��

  // ���ԍ����U�S�ʁi�݂炢�悿�A�͂߂̂˂������j
  BTLV_WAZAEFF_DELAY_START = 0,     ///< ���U����
  BTLV_WAZAEFF_DELAY_ATTACK,        ///< �U�����s

  // �ǂ�ڂ�
  BTLV_WAZAEFF_DOROBOU_NORMAL = 0,  ///< �q�b�g�̂�
  BTLV_WAZAEFF_DOROBOU_STEAL,       ///< �q�b�g�����ݐ���

  // �̂낢
  BTLV_WAZAEFF_NOROI_NORMAL = 0,    ///< �S�[�X�g�ȊO
  BTLV_WAZAEFF_NOROI_GHOST,         ///< �S�[�X�g

  // �v���[���g
  BTLV_WAZAEFF_PRESENT_DAMAGE = 0,  ///< �_���[�W
  BTLV_WAZAEFF_PRESENT_RECOVER,     ///< HP��

  // �������
  BTLV_WAZAEFF_KAWARAWARI_DEFAULT = 0, ///< �ʏ�
  BTLV_WAZAEFF_KAWARAWARI_BREAK,       ///< ���t���N�^�[����j��

  // �E�F�U�[�{�[��
  BTLV_WAZAEFF_WEATHERBALL_NORMAL = 0,  ///< �m�[�}��
  BTLV_WAZAEFF_WEATHERBALL_SHINE,       ///< �ɂق�΂�
  BTLV_WAZAEFF_WEATHERBALL_SNOW,        ///< �����
  BTLV_WAZAEFF_WEATHERBALL_SAND,        ///< ���Ȃ��炵
  BTLV_WAZAEFF_WEATHERBALL_RAIN,        ///< ���܂���

  // �e�N�m�o�X�^�[
  BTLV_WAZAEFF_TECKNOBASTER_NORMAL = 0, ///< �m�[�}��
  BTLV_WAZAEFF_TECKNOBASTER_MIZU,       ///< ��
  BTLV_WAZAEFF_TECKNOBASTER_DENKI,      ///< �d�C
  BTLV_WAZAEFF_TECKNOBASTER_HONOO,      ///< ��
  BTLV_WAZAEFF_TECKNOBASTER_KOORI,      ///< ������

  // �t���C���\�E���E�T���_�[�\�E��
  BTLV_WAZAEFF_LINKWAZA_NORMAL = 0,     ///< �ʏ�
  BTLV_WAZAEFF_LINKWAZA_LINK,           ///< �����N�����i�����ďo�����j

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
  WazaTarget   waza_range;
}BTLV_WAZAEFFECT_PARAM;


typedef enum{
  BTLV_EFFECT_ROTATE_DIR_LEFT = 0,
  BTLV_EFFECT_ROTATE_DIR_RIGHT,
}BTLV_EFFECT_ROTATE_DIR;

extern  BTLV_EFFECT_SETUP_PARAM*  BTLV_EFFECT_MakeSetUpParam( BtlRule rule, const BTL_FIELD_SITUATION* bfs, BOOL multi,
                                                              u16* tr_type, const BTL_MAIN_MODULE* mainModule,
                                                              const BTLV_SCU* scu, HEAPID heapID );

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
extern  void              BTLV_EFFECT_HengeShortCut( const POKEMON_PARAM* pp, BtlvMcssPos vpos );
extern  void              BTLV_EFFECT_PokemonVanishOn( BtlvMcssPos vpos );
extern  void              BTLV_EFFECT_PokemonVanishOff( BtlvMcssPos vpos );
extern  void              BTLV_EFFECT_CreateMigawari( BtlvMcssPos vpos );
extern  void              BTLV_EFFECT_DeleteMigawari( BtlvMcssPos vpos );
extern  BOOL              BTLV_EFFECT_CheckExecuteGauge( void );
extern  void              BTLV_EFFECT_SetGaugeDrawEnable( BOOL on_off, int side );
extern  void              BTLV_EFFECT_SetGaugeStatus( PokeSick sick,  BtlvMcssPos pos );
extern  void              BTLV_EFFECT_SetGaugeYure( BtlvMcssPos pos );
extern  BOOL              BTLV_EFFECT_CheckExistGauge( BtlvMcssPos pos );
extern  BOOL              BTLV_EFFECT_GetGaugeStatus( BtlvMcssPos pos, int* color, int* sick_anm );
extern  void              BTLV_EFFECT_SetBallGauge( const BTLV_BALL_GAUGE_PARAM* bbgp );
extern  void              BTLV_EFFECT_DelBallGauge( BTLV_BALL_GAUGE_TYPE type );
extern  BOOL              BTLV_EFFECT_CheckExecuteBallGauge( BTLV_BALL_GAUGE_TYPE type );
extern  void              BTLV_EFFECT_SetPaletteFade( int model, u8 start_evy, u8 end_evy, u8 wait, u16 rgb );
extern  BOOL              BTLV_EFFECT_CheckExecutePaletteFade( int model );
extern  void              BTLV_EFFECT_SetVanishFlag( int model, int flag );
extern  void              BTLV_EFFECT_SetRotateEffect( BtlRotateDir dir, int side );
extern  void              BTLV_EFFECT_SetSideChange( BtlvMcssPos pos1, BtlvMcssPos pos2 );
extern  int               BTLV_EFFECT_GetTrainerIndex( int position );
extern  void              BTLV_EFFECT_CreateTimer( int game_time, int command_time );
extern  void              BTLV_EFFECT_DrawEnableTimer( BTLV_TIMER_TYPE type, BOOL enable, BOOL init );
extern  BOOL              BTLV_EFFECT_IsZero( BTLV_TIMER_TYPE type );
extern  BTLV_MCSS_VANISH_FLAG BTLV_EFFECT_GetMcssVanishFlag( BtlvMcssPos position );
extern  void              BTLV_EFFECT_SetMcssVanishFlag( BtlvMcssPos position, BTLV_MCSS_VANISH_FLAG flag );
extern  void              BTLV_EFFECT_SetCameraFocus( BtlvMcssPos position, int move_type, int frame, int wait, int brake );
extern  void              BTLV_EFFECT_GetCameraFocus( BtlvMcssPos position, VecFx32* pos, VecFx32* target );
extern  BTLV_EFFECT_WORK* BTLV_EFFECT_GetEffectWork( void );
extern  BTLV_CAMERA_WORK* BTLV_EFFECT_GetCameraWork( void );
extern  BTLV_MCSS_WORK*   BTLV_EFFECT_GetMcssWork( void );
extern  BTLV_STAGE_WORK*  BTLV_EFFECT_GetStageWork( void );
extern  BTLV_GAUGE_WORK*  BTLV_EFFECT_GetGaugeWork( void );
extern  VMHANDLE*         BTLV_EFFECT_GetVMHandle( void );
extern  GFL_TCBSYS*       BTLV_EFFECT_GetTCBSYS( void );
extern  PALETTE_FADE_PTR  BTLV_EFFECT_GetPfd( void );
extern  BTLV_CLACT_WORK*  BTLV_EFFECT_GetCLWK( void );
extern  BTLV_BG_WORK*     BTLV_EFFECT_GetBGWork( void );
extern  BTLV_TIMER_WORK*  BTLV_EFFECT_GetTimerWork( void );
extern  BtlRule           BTLV_EFFECT_GetBtlRule( void );
extern  BOOL              BTLV_EFFECT_GetMulti( void );
extern  int               BTLV_EFFECT_GetTrType( int pos );
extern  const BTL_MAIN_MODULE* BTLV_EFFECT_GetMainModule( void );
extern  const BTLV_SCU*   BTLV_EFFECT_GetScu( void );
extern  int               BTLV_EFFECT_GetPinchBGMFlag( void );
extern  void              BTLV_EFFECT_SetTrainerBGMChangeFlag( int bgm_no );
extern  BOOL              BTLV_EFFECT_GetTrainerBGMChangeFlag( void );
extern  void              BTLV_EFFECT_SetBGMNoCheckPush( int bgm_no );
extern  void              BTLV_EFFECT_SetBagMode( BtlBagMode bagMode );
extern  BtlBagMode        BTLV_EFFECT_GetBagMode( void );
extern  void              BTLV_EFFECT_SetSEMode( BTLV_EFFECT_SE_MODE mode );
extern  BTLV_EFFECT_SE_MODE  BTLV_EFFECT_GetSEMode( void );
extern  BOOL              BTLV_EFFECT_CheckShooterEnable( void );
extern  BOOL              BTLV_EFFECT_CheckItemEnable( void );
extern  void              BTLV_EFFECT_SetReverseDrawFlag( BTLV_EFFECT_REVERSE_DRAW draw_flag );

extern  void              BTLV_EFFECT_SetTCB( GFL_TCB* tcb, BTLV_EFFECT_TCB_CALLBACK_FUNC* callback_func,
                                              BTLV_EFFECT_TCB_GROUP group );
extern  int               BTLV_EFFECT_SearchTCBIndex( GFL_TCB* tcb );
extern  void              BTLV_EFFECT_FreeTCB( GFL_TCB* tcb );
extern  void              BTLV_EFFECT_FreeTCBGroup( BTLV_EFFECT_TCB_GROUP group );
extern  void              BTLV_EFFECT_SetCameraWorkExecute( BTLV_EFFECT_CWE cwe );
extern  void              BTLV_EFFECT_SetCameraWorkStop( void );
extern  void              BTLV_EFFECT_SetCameraWorkSwitch( BTLV_EFFECT_CWE type );

#ifdef PM_DEBUG
extern  void        BTLV_EFFECT_SetPokemonDebug( const MCSS_ADD_DEBUG_WORK *madw, int position );
extern  void        BTLV_EFFECT_SetBtlRule( BtlRule rule );
#endif

