//=============================================================================================
/**
 * @file  btl_server_flow.h
 * @brief �|�P����WB �o�g���V�X�e�� �T�[�o�R�}���h��������
 * @author  taya
 *
 * @date  2009.03.06  �쐬
 */
//=============================================================================================
#ifndef __BTL_SERVER_FLOW_H__
#define __BTL_SERVER_FLOW_H__

#include "waza_tool/wazadata.h"

#include "btl_main.h"
#include "btl_adapter.h"
#include "btl_server.h"
#include "btl_server_cmd.h"
#include "btl_string.h"
#include "btl_sideeff.h"
#include "btl_poseff.h"
#include "btl_field.h"
#include "btl_wazarec.h"
#include "btl_deadrec.h"
#include "btl_action.h"
#include "btl_calc.h"

#include "app/b_bag.h"

/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
typedef enum {
  SVFLOW_RESULT_DEFAULT = 0,  ///< ���̂܂ܑ��s
  SVFLOW_RESULT_POKE_CHANGE,  ///< �^�[���r���̃|�P��������ւ����N�G�X�g
  SVFLOW_RESULT_POKE_COVER,   ///< �󂫈ʒu�Ƀ|�P�����𓊓����郊�N�G�X�g
  SVFLOW_RESULT_LEVELUP,      ///< ���x���A�b�v����
  SVFLOW_RESULT_BTL_SHOWDOWN, ///< �o�g���I���i�����j
  SVFLOW_RESULT_BTL_QUIT,     ///< �o�g���I���i�����j
  SVFLOW_RESULT_POKE_GET,     ///< �o�g���I���i�|�P�����ߊl�j
}SvflowResult;


/*--------------------------------------------------------------------------*/
/* Structures                                                               */
/*--------------------------------------------------------------------------*/

/**
 *  �T�[�o�{�́A�T�[�o�t���[�o������Q�Ƃ���N���C�A���g�p�����[�^
 */

struct _SVCL_WORK {

  BTL_ADAPTER*    adapter;
  BTL_PARTY*      party;
  u8        memberCount;
  u8        numCoverPos;
  u8        isLocalClient;
  u8        myID;
};

/**
 *  �T�[�o�t���[���n���h������Q�Ƃ��郏�U�p�����[�^
 */

typedef struct {

  WazaID          wazaID;
  PokeTypePair    userType;
  PokeType        wazaType;
  WazaDamageType  damageType;
  WazaTarget      targetType;

}SVFL_WAZAPARAM;

/*--------------------------------------------------------------------------*/
/* Typedefs                                                                 */
/*--------------------------------------------------------------------------*/
#include "btl_server_flow_def.h"


// server -> server_flow
extern BTL_SVFLOW_WORK* BTL_SVFLOW_InitSystem(
  BTL_SERVER* server, BTL_MAIN_MODULE* mainModule, BTL_POKE_CONTAINER* pokeCon,
  BTL_SERVER_CMD_QUE* que, BtlBagMode bagMode, HEAPID heapID );

extern void BTL_SVFLOW_ResetSystem( BTL_SVFLOW_WORK* wk );
extern void BTL_SVFLOW_QuitSystem( BTL_SVFLOW_WORK* wk );

extern SvflowResult BTL_SVFLOW_StartBtlIn( BTL_SVFLOW_WORK* wk );

extern void BTL_SVFLOW_StartTurn_Boot( BTL_SVFLOW_WORK* wk );
extern SvflowResult BTL_SVFLOW_StartTurn( BTL_SVFLOW_WORK* wk, const BTL_SVCL_ACTION* clientAction );

extern void BTL_SVFLOW_ContinueAfterPokeChange_Boot( BTL_SVFLOW_WORK* wk );
extern SvflowResult BTL_SVFLOW_ContinueAfterPokeChange( BTL_SVFLOW_WORK* wk, const BTL_SVCL_ACTION* clientAction );

extern void BTL_SVFLOW_StartAfterPokeIn_Boot( BTL_SVFLOW_WORK* wk );
extern SvflowResult BTL_SVFLOW_StartAfterPokeIn( BTL_SVFLOW_WORK* wk, const BTL_SVCL_ACTION* clientAction );



extern BOOL BTL_SVFLOW_MakeShooterChargeCommand( BTL_SVFLOW_WORK* wk );
extern void BTL_SVFLOW_MakeRotationCommand( BTL_SVFLOW_WORK* wk, u8 clientID, BtlRotateDir dir );
extern BOOL BTL_SVFLOW_MakePlayerEscapeCommand( BTL_SVFLOW_WORK* wk );

//------------------------------------------------
// �T�[�o�{�̂Ƃ̘A���֐�
//------------------------------------------------
// server_flow -> server
extern SVCL_WORK* BTL_SERVER_GetClientWork( BTL_SERVER* server, u8 clientID );
extern SVCL_WORK* BTL_SERVER_GetClientWorkIfEnable( BTL_SERVER* server, u8 clientID );
extern BOOL BTL_SERVER_IsClientEnable( const BTL_SERVER* server, u8 clientID );
extern u8 BTL_SVCL_GetNumActPoke( SVCL_WORK* clwk );
extern const BTL_ACTION_PARAM* BTL_SVCL_GetPokeAction( SVCL_WORK* clwk, u8 posIdx );
extern BTL_ACTION_PARAM BTL_SVCL_ACTION_Get( const BTL_SVCL_ACTION* clientAction, u8 clientID, u8 posIdx );


extern void BTL_SVF_SickDamageRecall( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, WazaSick sickID, u32 damage );
extern const BTL_ESCAPEINFO* BTL_SVFLOW_GetEscapeInfoPointer( const BTL_SVFLOW_WORK* wk );
extern BtlPokePos BTL_SVFLOW_GetCapturedPokePos( const BTL_SVFLOW_WORK* wk );


extern u16 BTL_SVF_GetTypeAffCnt_PutVoid( const BTL_SVFLOW_WORK* wk );
extern u16 BTL_SVF_GetTypeAffCnt_PutAdvantage( const BTL_SVFLOW_WORK* wk );
extern u16 BTL_SVF_GetTypeAffCnt_PutDisadvantage( const BTL_SVFLOW_WORK* wk );
extern u16 BTL_SVF_GetTypeAffCnt_RecvVoid( const BTL_SVFLOW_WORK* wk );
extern u16 BTL_SVF_GetTypeAffCnt_RecvAdvantage( const BTL_SVFLOW_WORK* wk );
extern u16 BTL_SVF_GetTypeAffCnt_RecvDisadvantage( const BTL_SVFLOW_WORK* wk );


//------------------------------------------------
// �C�x���g�n���h���Ƃ̘A���֐�
//------------------------------------------------
extern BtlRule BTL_SVFTOOL_GetRule( BTL_SVFLOW_WORK* wk );
extern BtlCompetitor BTL_SVFTOOL_GetCompetitor( BTL_SVFLOW_WORK* wk );
extern BtlPokePos BTL_SVFTOOL_GetExistFrontPokePos( BTL_SVFLOW_WORK* server, u8 pokeID );
extern u8 BTL_SVFTOOL_GetExistPokeID( BTL_SVFLOW_WORK* wk, BtlPokePos pos );
extern BOOL BTL_SVFTOOL_CheckExistTokuseiPokemon( BTL_SVFLOW_WORK* wk, PokeTokusei tokusei );
extern const BTL_POKEPARAM* BTL_SVFTOOL_GetPokeParam( BTL_SVFLOW_WORK* wk, u8 pokeID );
extern u8 BTL_SVFTOOL_GetAllOpponentFrontPokeID( BTL_SVFLOW_WORK* wk, u8 basePokeID, u8* dst );
extern u16 BTL_SVFTOOL_GetTurnCount( BTL_SVFLOW_WORK* wk );
extern u8 BTL_SVFTOOL_GetPokeChangeCount( BTL_SVFLOW_WORK* wk, u8 clientID );
extern u8 BTL_SVFTOOL_GetClientCoverPosCount( BTL_SVFLOW_WORK* wk, u8 pokeID );
extern u8 BTL_SVFTOOL_GetMyBenchIndex( BTL_SVFLOW_WORK* wk, u8 pokeID );
extern BOOL BTL_SVFTOOL_IsExistBenchPoke( BTL_SVFLOW_WORK* wk, u8 pokeID );
extern const BTL_WAZAREC* BTL_SVFTOOL_GetWazaRecord( BTL_SVFLOW_WORK* wk );
extern u16 BTL_SVFTOOL_ReserveQuePos( BTL_SVFLOW_WORK* wk, ServerCmd cmd );
extern const BTL_DEADREC* BTL_SVFTOOL_GetDeadRecord( BTL_SVFLOW_WORK* wk );
extern BtlTypeAff BTL_SVFTOOL_SimulationAffinity( BTL_SVFLOW_WORK* wk, u8 atkPokeID, u8 defPokeID, WazaID waza );
extern u32 BTL_SVFTOOL_SimulationDamage( BTL_SVFLOW_WORK* flowWk, u8 atkPokeID, u8 defPokeID, WazaID waza, BOOL fAffinity, BOOL fEnableRand );
extern BtlBgAttr BTL_SVFTOOL_GetLandForm( BTL_SVFLOW_WORK* wk );
extern u32 BTL_SVFTOOL_GetFrontPosNum( BTL_SVFLOW_WORK* wk );
extern const BTL_PARTY* BTL_SVFTOOL_GetPartyData( BTL_SVFLOW_WORK* wk, u8 pokeID );
extern BtlPokePos BTL_SVFTOOL_PokeIDtoPokePos( BTL_SVFLOW_WORK* wk, u8 pokeID );
extern BtlPokePos BTL_SVFTOOL_ReqWazaTargetAuto( BTL_SVFLOW_WORK* wk, u8 pokeID, WazaID waza );
extern u8 BTL_SVFTOOL_PokePosToPokeID( BTL_SVFLOW_WORK* wk, u8 pokePos );
extern BOOL BTL_SVFTOOL_GetThisTurnAction( BTL_SVFLOW_WORK* wk, u8 pokeID, BTL_ACTION_PARAM* dst );
extern BOOL BTL_SVFTOOL_GetMyActionOrder( BTL_SVFLOW_WORK* wk, u8 pokeID, u8* myOrder, u8* totalAction );
extern void* BTL_SVFTOOL_GetTmpWork( BTL_SVFLOW_WORK* wk, u32 size );
extern WazaID BTL_SVFTOOL_GetPrevExeWaza( BTL_SVFLOW_WORK* wk );
extern BOOL BTL_SVFTOOL_IsMultiMode( BTL_SVFLOW_WORK* wk );
extern BOOL BTL_SVFTOOL_CheckSinkaMae( BTL_SVFLOW_WORK* wk, u8 pokeID );
extern u16 BTL_SVFTOOL_CalcAgility( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, BOOL fTrickRoomEnable );
extern u16 BTL_SVFTOOL_CalcAgilityRank( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, BOOL fTrickRoomEnable );
extern BOOL BTL_SVFTOOL_IsFlyingPoke( BTL_SVFLOW_WORK* wk, u8 pokeID );
extern BtlWeather BTL_SVFTOOL_GetWeather( BTL_SVFLOW_WORK* wk );
extern BOOL BTL_SVFTOOL_IsExistSideEffect( BTL_SVFLOW_WORK* wk, BtlSide side, BtlSideEffect sideEffect );
extern BOOL BTL_SVFTOOL_GetDebugFlag( BTL_SVFLOW_WORK* wk, BtlDebugFlag flag );
extern u32 BTL_SVFTOOL_GetSideEffectCount( BTL_SVFLOW_WORK* wk, BtlPokePos pos, BtlSideEffect sideEffect );
extern BOOL BTL_SVFTOOL_IsExistPosEffect( BTL_SVFLOW_WORK* wk, BtlPokePos pos, BtlPosEffect effect );
extern BOOL BTL_SVFTOOL_IsMemberOutIntr( BTL_SVFLOW_WORK* wk );
extern void BTL_SVFRET_SetWazaEffectIndex( BTL_SVFLOW_WORK* wk, u8 effIndex );
extern BOOL BTL_SVFRET_AddBonusMoney( BTL_SVFLOW_WORK* wk, u32 volume, u8 pokeID );


//----------------------------



/**
 * ����D�揇�R�[�h�i�f��������ʁA���U�D�揇��艺�ʁF0�`7�Ɏ��܂�悤�ɁI�j
 */
typedef enum {

  BTL_SPPRI_LOW     = 0,
  BTL_SPPRI_DEFAULT = 1,
  BTL_SPPRI_HIGH    = 2,

}BtlSpecialPri;


/**
 *  ��Ԉُ�w��̊g���R�[�h
 */
typedef enum {

  // �S�Ă� WazaSick �R�[�h���ΏۂƂȂ�܂�

  WAZASICK_EX_POKEFULL = WAZASICK_MAX,  ///< �S�|�P�����n�ُ킪�Ώ�
  WAZASICK_EX_POKEFULL_PLUS,            ///< �S�|�P�����n�ُ�{��������Ώ�

}BtlWazaSickEx;

/**
 *  ��Ԉُ퉻���s�R�[�h
 */
typedef enum {

  BTL_ADDSICK_FAIL_NULL = 0,
  BTL_ADDSICK_FAIL_ALREADY,     ///< ���łɓ�����ԂɂȂ��Ă���
  BTL_ADDSICK_FAIL_NO_EFFECT,   ///< ���ʂ̖�������
  BTL_ADDSICK_FAIL_OTHER,       ///< ���̑�


}BtlAddSickFailCode;


/**
 *  �n���h�������i���ʂ��\�o������́j
 */
typedef enum {

  BTL_HANDEX_USE_ITEM,      ///< �A�C�e���g�p

  /*--- �ȉ��A�A�C�e���g�p�ȊO�̌��ʂɎg�� ---*/
  BTL_HANDEX_TOKWIN_IN,           ///< �Ƃ������E�B���h�E�C��
  BTL_HANDEX_TOKWIN_OUT,          ///< �Ƃ������E�B���h�E�A�E�g
  BTL_HANDEX_ITEM_EFFECT,         ///< �A�C�e�������G�t�F�N�g
  BTL_HANDEX_MESSAGE,             ///< ���b�Z�[�W�\��
  BTL_HANDEX_RECOVER_HP,          ///< HP��
  BTL_HANDEX_DRAIN,               ///< HP�񕜁i�h���C���n�j
  BTL_HANDEX_DAMAGE,              ///< �_���[�W��^����
  BTL_HANDEX_SHIFT_HP,            ///< HP��ύX�i�_���[�W�E�񕜂Ƃ��Ĉ���Ȃ��j
  BTL_HANDEX_RECOVER_PP,          ///< PP��
  BTL_HANDEX_DECREMENT_PP,        ///< PP����
  BTL_HANDEX_CURE_SICK,           ///< ��Ԉُ����
  BTL_HANDEX_ADD_SICK,            ///< ��Ԉُ�ɂ���
  BTL_HANDEX_RANK_EFFECT,         ///< �����N��������
  BTL_HANDEX_SET_RANK,            ///< �����N���w��n�ɋ�����������
  BTL_HANDEX_RESET_RANK,          ///< �����N���ʂ�S�ăt���b�g��
  BTL_HANDEX_SET_STATUS,          ///< �\�͒l�i�U���A�h�䓙�j��������������
  BTL_HANDEX_RECOVER_RANK,        ///< �}�C�i�X�����N���ʂ݂̂��t���b�g��
  BTL_HANDEX_KILL,                ///< �m���ɂ���
  BTL_HANDEX_CHANGE_TYPE,         ///< �|�P�����̃^�C�v��ς���
  BTL_HANDEX_SET_TURNFLAG,        ///< �^�[���t���O�Z�b�g
  BTL_HANDEX_RESET_TURNFLAG,      ///< �^�[���t���O�������Z�b�g
  BTL_HANDEX_SET_CONTFLAG,        ///< �p���t���O�Z�b�g
  BTL_HANDEX_RESET_CONTFLAG,      ///< �p���t���O���Z�b�g
  BTL_HANDEX_SIDEEFF_ADD,         ///< �T�C�h�G�t�F�N�g�ǉ�
  BTL_HANDEX_SIDEEFF_REMOVE,      ///< �T�C�h�G�t�F�N�g�폜
  BTL_HANDEX_ADD_FLDEFF,          ///< �t�B�[���h�G�t�F�N�g�ǉ�
  BTL_HANDEX_REMOVE_FLDEFF,       ///< �t�B�[���h�G�t�F�N�g�ǉ�
  BTL_HANDEX_CHANGE_WEATHER,      ///< �V��ω�
  BTL_HANDEX_POSEFF_ADD,          ///< �ʒu�G�t�F�N�g�ǉ�
  BTL_HANDEX_CHANGE_TOKUSEI,      ///< �Ƃ�������������
  BTL_HANDEX_SET_ITEM,            ///< �A�C�e����������
  BTL_HANDEX_EQUIP_ITEM,          ///< �A�C�e���������ʔ����`�F�b�N
  BTL_HANDEX_ITEM_SP,             ///< �A�C�e�����ʔ���
  BTL_HANDEX_CONSUME_ITEM,        ///< �����ŃA�C�e��������
  BTL_HANDEX_SWAP_ITEM,           ///< �A�C�e������ւ�
  BTL_HANDEX_CLEAR_CONSUMED_ITEM, ///< �A�C�e��������̃N���A
  BTL_HANDEX_UPDATE_WAZA,         ///< ���U��������
  BTL_HANDEX_COUNTER,             ///< �|�P�����J�E���^�l��������
  BTL_HANDEX_DELAY_WAZADMG,       ///< ���ԍ����U�_���[�W
  BTL_HANDEX_QUIT_BATTLE,         ///< �o�g�����E
  BTL_HANDEX_CHANGE_MEMBER,       ///< �����o�[����
  BTL_HANDEX_BATONTOUCH,          ///< �o�g���^�b�`�i�����N���ʓ��̈����p���j
  BTL_HANDEX_ADD_SHRINK,          ///< �Ђ�܂���
  BTL_HANDEX_RELIVE,              ///< �����Ԃ点��
  BTL_HANDEX_SET_WEIGHT,          ///< �̏d��ݒ�
  BTL_HANDEX_PUSHOUT,             ///< �ꂩ�琁����΂�
  BTL_HANDEX_INTR_POKE,           ///< �w��|�P�����̊��荞�ݍs���𔭐�������
  BTL_HANDEX_INTR_WAZA,           ///< �w�胏�U���g�����Ƃ��Ă���|�P�����̊��荞�ݍs���𔭐�������
  BTL_HANDEX_SEND_LAST,           ///< �w��|�P�����̍s������ԍŌ�ɉ�
  BTL_HANDEX_SWAP_POKE,           ///< ��ɏo�Ă���|�P�������m�����ւ���
  BTL_HANDEX_HENSIN,              ///< �ւ񂵂񂷂�
  BTL_HANDEX_FAKE_BREAK,          ///< �C�����[�W��������
  BTL_HANDEX_JURYOKU_CHECK,       ///< ���イ��傭�������̃`�F�b�N����
  BTL_HANDEX_TAMEHIDE_CANCEL,     ///< ����̏�����ԁi������ƂԂȂǁj���L�����Z��
  BTL_HANDEX_EFFECT_BY_POS,       ///< �ʒu�w�肵�ăG�t�F�N�g����
  BTL_HANDEX_CHANGE_FORM,         ///< �t�H�����i���o�[�`�F���W
  BTL_HANDEX_SET_EFFECT_IDX,      ///< ���U�G�t�F�N�g�C���f�b�N�X�ύX
  BTL_HANDEX_WAZAEFFECT_ENABLE,   ///< �������U�G�t�F�N�g�L��

  BTL_HANDEX_MAX,

}BtlEventHandlerExhibition;

/**
 *  �n���h���������[�N���L�w�b�_
 */
typedef struct {
  u8    equip;             ///< �n���h�������iBtlEventHandlerEquip�j
  u8    userPokeID;        ///< �n���h����̂̃|�P����ID
  u8    size;              ///< �\���̃T�C�Y
  u8    tokwin_flag  : 1;  ///< ��̃|�P�����̂Ƃ��������ʂł��邱�Ƃ�\������
  u8    failSkipFlag : 1;  ///< ���O�̖��߂����s������X�L�b�v�����
  u8    padding : 6;       ///< �p�f�B���O
}BTL_HANDEX_PARAM_HEADER;

/**
 *  �n���h���p���b�Z�[�W�o�̓p�����[�^
 */
typedef struct {

  u8    type;    ///< ������^�C�v�i BtlStrType  (btl_string.h) )
  u8    argCnt;  ///< �����̐�
  u16   ID;      ///< ������ID
  int   args[ BTL_STR_ARG_MAX ];  ///< ����

}BTL_HANDEX_STR_PARAMS;

static inline void HANDEX_STR_Clear( BTL_HANDEX_STR_PARAMS* param )
{
  GFL_STD_MemClear( param, sizeof(*param) );
  param->type = BTL_STRTYPE_NULL;
}

static inline BOOL HANDEX_STR_IsEnable( const BTL_HANDEX_STR_PARAMS* param )
{
  return param->type != BTL_STRTYPE_NULL;
}

static inline void HANDEX_STR_Setup( BTL_HANDEX_STR_PARAMS* param, BtlStrType type, u16 strID )
{
  param->type = type;
  param->ID = strID;
  param->argCnt = 0;
}
static inline void HANDEX_STR_AddArg( BTL_HANDEX_STR_PARAMS* param, int arg )
{
  if( param->argCnt < BTL_STR_ARG_MAX )
  {
    param->args[ param->argCnt++ ] = arg;
  }
}


typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
}BTL_HANDEX_PARAM_USE_ITEM;


/**
 * ���b�Z�[�W�\��
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
 BTL_HANDEX_STR_PARAMS  str;
}BTL_HANDEX_PARAM_MESSAGE;

/**
 * HP�񕜏���
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
 u16  recoverHP;                     ///< ��HP��
 u8   pokeID;                        ///< �Ώۃ|�P����ID
 BTL_HANDEX_STR_PARAMS     exStr;    ///< ���������b�Z�[�W
}BTL_HANDEX_PARAM_RECOVER_HP;

/**
 * HP�z��
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
 u16  recoverHP;                     ///< ��HP��
 u8   recoverPokeID;                 ///< �񕜑Ώۃ|�P����ID
 u8   damagedPokeID;                 ///< �_���[�W���󂯂����̃|�P����ID
 BTL_HANDEX_STR_PARAMS     exStr;    ///< ���������b�Z�[�W
}BTL_HANDEX_PARAM_DRAIN;

/**
 * �_���[�W��^����
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
 u8   pokeID;                        ///< �Ώۃ|�P����ID
 u8   fHitHidePoke;                  ///< ������Ƃԏ�ԂȂǉ�ʂ�������Ă���|�P�ɂ��^����
 u16  damage;                        ///< �_���[�W��
 BTL_HANDEX_STR_PARAMS     exStr;    ///< ���������b�Z�[�W
}BTL_HANDEX_PARAM_DAMAGE;

/**
 * HP�𑝌��i�_���[�W�E�񕜂Ƃ͌��Ȃ���Ȃ��j
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
  u8     poke_cnt;
  u8     fEffectDisable;              ///< �Q�[�W�G�t�F�N�g������
  u8     pokeID[ BTL_POS_MAX ];
  int    volume[ BTL_POS_MAX ];
}BTL_HANDEX_PARAM_SHIFT_HP;

/**
 * PP����
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
 u8   volume;                        ///< PP��
 u8   pokeID;                        ///< �Ώۃ|�P����ID
 u8   wazaIdx;                       ///< �Ώۃ��U�C���f�b�N�X
 BTL_HANDEX_STR_PARAMS     exStr;    ///< ���������b�Z�[�W
}BTL_HANDEX_PARAM_PP;

/**
 * �Ώۂ̏�Ԉُ����
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
 BtlWazaSickEx   sickCode;           ///< �Ή������Ԉُ�R�[�h�i�g���j
 u8   pokeID[ BTL_POS_MAX ];         ///< �Ώۃ|�P����ID
 u8   poke_cnt;                      ///< �Ώۃ|�P������
 u8   fStdMsgDisable;                ///< �������A�W�����b�Z�[�W���o�͂��Ȃ�
 BTL_HANDEX_STR_PARAMS     exStr;    ///< ���������ꃁ�b�Z�[�W
}BTL_HANDEX_PARAM_CURE_SICK;

/**
 * �Ώۂ���Ԉُ�ɂ���
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
 WazaSick       sickID;              ///< ��Ԉُ�ID
 BPP_SICK_CONT  sickCont;            ///< ��Ԉُ�p���p�����[�^
 u8   fAlmost;                       ///< �قڊm��t���O�i����v���Ŏ��s�����ꍇ�Ɍ������b�Z�[�W��\������j
 u8   fStdMsgDisable;                ///< �W�����b�Z�[�W�I�t
 u8   pokeID;                        ///< �Ώۃ|�P����ID
 BTL_HANDEX_STR_PARAMS  exStr;       ///< ���ꃁ�b�Z�[�W�\���i�W�����b�Z�[�W�̓I�t�j
}BTL_HANDEX_PARAM_ADD_SICK;

/**
 * �����N���ʁi�㏸�A���~�j
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
 WazaRankEffect rankType;            ///< �����N�������
 s8   rankVolume;                    ///< �����N�����l(=0���Ƌ������Z�b�g�j
 u8   fStdMsgDisable;                ///< �W�����b�Z�[�W��\�����Ȃ��t���O
 u8   fAlmost;                       ///< �قڊm��t���O�i����v���Ŏ��s�����ꍇ�Ɍ������b�Z�[�W��\������j
 u8   poke_cnt;                      ///< �Ώۃ|�P������
 u8   pokeID[ BTL_POS_MAX ];         ///< �Ώۃ|�P����ID
 BTL_HANDEX_STR_PARAMS  exStr;       ///< ���ꃁ�b�Z�[�W�ڍ�
}BTL_HANDEX_PARAM_RANK_EFFECT;

/**
 * �����N���ʂ����l�ɃZ�b�g
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
 u8 pokeID;                          ///< �Ώۃ|�P����ID
 s8 attack;
 s8 defence;
 s8 sp_attack;
 s8 sp_defence;
 s8 agility;
 s8 hit_ratio;
 s8 avoid_ratio;
}BTL_HANDEX_PARAM_SET_RANK;

/**
 * �Ώۂ̉������Ă��郉���N���ʂ��t���b�g�ɖ߂�
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
 u8 pokeID;                          ///< �Ώۃ|�P����ID
}BTL_HANDEX_PARAM_RECOVER_RANK;

/**
 * �Ώۂ̑S�Ẵ����N���ʂ��t���b�g�ɖ߂�
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
 u8   poke_cnt;                      ///< �Ώۃ|�P������
 u8   pokeID[ BTL_POS_MAX ];         ///< �Ώۃ|�P����ID
}BTL_HANDEX_PARAM_RESET_RANK;

/**
 * �Ώۂ̔\�͒l�𒼐ڏ�������
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
 u16  attack;
 u16  defence;
 u16  sp_attack;
 u16  sp_defence;
 u16  agility;
 u8  pokeID;        ///< �Ώۃ|�P����ID
 u8  fAttackEnable    : 1;
 u8  fDefenceEnable   : 1;
 u8  fSpAttackEnable  : 1;
 u8  fSpDefenceEnable : 1;
 u8  fAgilityEnable   : 1;
 u8  dmy : 3;
 BTL_HANDEX_STR_PARAMS     exStr;    ///< ���������b�Z�[�W
}BTL_HANDEX_PARAM_SET_STATUS;

/**
 * �Ώۂ��Ђ񎀂ɂ���
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
 u8   pokeID;
 BTL_HANDEX_STR_PARAMS      exStr;
}BTL_HANDEX_PARAM_KILL;

/**
 * �^�C�v�ύX����
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  PokeTypePair    next_type;
  u8              pokeID;
  u8              fStdMsgDisable;
}BTL_HANDEX_PARAM_CHANGE_TYPE;

/**
 * �^�[���t���O�Z�b�g����
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  BppTurnFlag     flag;
  u8              pokeID;
}BTL_HANDEX_PARAM_TURNFLAG;

/**
 * �i���t���O�Z�b�g����
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  BppContFlag     flag;
  u8              pokeID;
}BTL_HANDEX_PARAM_SET_CONTFLAG;

typedef BTL_HANDEX_PARAM_SET_CONTFLAG  BTL_HANDEX_PARAM_RESET_CONTFLAG;

/**
 * �T�C�h�G�t�F�N�g�ǉ�����
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  BtlSideEffect   effect;
  BPP_SICK_CONT   cont;
  BtlSide         side;
  BTL_HANDEX_STR_PARAMS    exStr;
}BTL_HANDEX_PARAM_SIDEEFF_ADD;

/**
 * �T�C�h�G�t�F�N�g��������
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8              flags[ BTL_SIDEEFF_BITFLG_BYTES ];
  u8              side;
  u8              fExMsg;
  u16             exStrID;
}BTL_HANDEX_PARAM_SIDEEFF_REMOVE;

/**
 * �t�B�[���h�G�t�F�N�g�ǉ�����
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  BtlFieldEffect           effect;
  BPP_SICK_CONT            cont;
  u8                       sub_param;

  BTL_HANDEX_STR_PARAMS    exStr;

}BTL_HANDEX_PARAM_ADD_FLDEFF;

/**
 * �t�B�[���h�G�t�F�N�g��������
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  BtlFieldEffect           effect;
}BTL_HANDEX_PARAM_REMOVE_FLDEFF;

/**
 * �V��ω�����
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  BtlWeather               weather;
  u8                       turn;
  BTL_HANDEX_STR_PARAMS    exStr;

}BTL_HANDEX_PARAM_CHANGE_WEATHER;

/**
 * �ʒu�G�t�F�N�g�ǉ�
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  BtlPosEffect             effect;
  BtlPokePos               pos;
  int                      param[ BTL_POSEFF_PARAM_MAX ];
  u8                       param_cnt;
}BTL_HANDEX_PARAM_POSEFF_ADD;

/**
 *  �Ƃ�����������������
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u16                      tokuseiID;  ///< ����������̂Ƃ������iPOKETOKUSEI_NULL�Ȃ�Ƃ������������j
  u8                       pokeID;     ///< �Ώۃ|�P����ID
  u8                       fSameTokEffective; /// < ���Ȃ��u�Ƃ������v�����Ώۂɂ�����
  BTL_HANDEX_STR_PARAMS    exStr;      ///< ���������b�Z�[�W
}BTL_HANDEX_PARAM_CHANGE_TOKUSEI;

/**
 *  �A�C�e��������������
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u16             itemID;       ///< ����������̃A�C�e���iITEM_DUMMY_DATA �Ȃ�A�C�e���������j
  u8              pokeID;       ///< �Ώۃ|�P����ID
  BTL_HANDEX_STR_PARAMS  exStr; ///< ���������b�Z�[�W
}BTL_HANDEX_PARAM_SET_ITEM;

/**
 *  �A�C�e������ւ��i�w�b�_�̎g�p�|�P�����Ɓj
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8              pokeID;            ///< �Ώۃ|�P����ID
  BTL_HANDEX_STR_PARAMS   exStr;     ///< ���������b�Z�[�W
}BTL_HANDEX_PARAM_SWAP_ITEM;

/**
 *  �A�C�e�����ʂ̔����`�F�b�N
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8              pokeID;            ///< �Ώۃ|�P����ID
}BTL_HANDEX_PARAM_EQUIP_ITEM;

/**
 *  �A�C�e�����ʂ̓���g�p�i���΂ށA�Ȃ�����ȂǑΉ��j
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8              pokeID;            ///< �Ώۃ|�P����ID
  u16             itemID;
}BTL_HANDEX_PARAM_ITEM_SP;

/**
 *  �A�C�e�������
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  BTL_HANDEX_STR_PARAMS    exStr;   ///< ���������b�Z�[�W
}BTL_HANDEX_PARAM_CONSUME_ITEM;

/**
 *  �A�C�e��������̃N���A
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8              pokeID;       ///< �Ώۃ|�P����ID
}BTL_HANDEX_PARAM_CLEAR_CONSUMED_ITEM;


/**
 * ���U������������
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8              pokeID;            ///< �Ώۃ|�P����ID
  u8              wazaIdx;
  u8              ppMax;             ///< PPMax�l:0�Ȃ�f�t�H���g
  u8              fPermanent;        ///< �o�g����܂ŏ����������c��t���O
  WazaID          waza;
}BTL_HANDEX_PARAM_UPDATE_WAZA;

/**
 * �J�E���^������������
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8              pokeID;            ///< �Ώۃ|�P����ID
  u8              counterID;         ///< �J�E���^ID�iBppCounter)
  u8              value;             ///< �J�E���^�ɐݒ肷��l
}BTL_HANDEX_PARAM_COUNTER;

/**
 * ���ԍ����U�_���[�W�����p
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8                       attackerPokeID;
  u8                       targetPokeID;
  WazaID                   wazaID;
}BTL_HANDEX_PARAM_DELAY_WAZADMG;

/**
 * �����o�[����ւ�����
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  BTL_HANDEX_STR_PARAMS    exStr;   ///< ���������b�Z�[�W
  u8                       pokeID;
  u8                       fIntrDisable;  ///< ���荞�݁i���������Ȃǁj�֎~
}BTL_HANDEX_PARAM_CHANGE_MEMBER;

/**
 * �o�g���^�b�`����
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8  userPokeID;     ///< ��Ԃ������p������|�PID
  u8  targetPokeID;   ///< ��Ԃ������p���|�PID
}BTL_HANDEX_PARAM_BATONTOUCH;

/**
 * �Ђ�܂��鏈��
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8  pokeID;     ///< �Ώۃ|�PID
  u8  per;        ///< �m��
}BTL_HANDEX_PARAM_ADD_SHRINK;

/**
 * �����Ԃ点�鏈��
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8   pokeID;        ///< �Ώۃ|�PID
  u16  recoverHP;     ///< ��HP��
  BTL_HANDEX_STR_PARAMS  exStr; ///< ���������b�Z�[�W
}BTL_HANDEX_PARAM_RELIVE;

/**
 * �̏d��ݒ�
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8   pokeID;        ///< �Ώۃ|�PID
  u16  weight;        ///< �̏d
  BTL_HANDEX_STR_PARAMS  exStr; ///< ���������b�Z�[�W
}BTL_HANDEX_PARAM_SET_WEIGHT;

/**
 * �|�P�������ꂩ�琁����΂�
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8   pokeID;                  ///< �Ώۃ|�PID
  u8   fForceChange;            ///< ��������ւ����[�h�t���O
  BTL_HANDEX_STR_PARAMS  exStr; ///< ���������b�Z�[�W
}BTL_HANDEX_PARAM_PUSHOUT;

/**
 * �w��|�P�������荞�ݍs��
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8   pokeID;                  ///< �Ώۃ|�PID
  BTL_HANDEX_STR_PARAMS  exStr; ///< ���������b�Z�[�W
}BTL_HANDEX_PARAM_INTR_POKE;

/**
 * �w�胏�U�g�p�|�P���荞�ݍs��
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  WazaID   waza;                ///< �Ώۃ��UID
}BTL_HANDEX_PARAM_INTR_WAZA;

/**
 * �w��|�P�����̍s�������Ō�ɉ�
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8   pokeID;                       ///< �Ώۃ|�PID
  BTL_HANDEX_STR_PARAMS    exStr;    ///< ���������b�Z�[�W
}BTL_HANDEX_PARAM_SEND_LAST;

/**
 * ��ɏo�Ă���|�P�������m�����ւ���i�����̂݁j
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8   pokeID1;                      ///< �Ώۃ|�PID 1
  u8   pokeID2;                      ///< �Ώۃ|�PID 2
  BTL_HANDEX_STR_PARAMS    exStr;    ///< ���������b�Z�[�W
}BTL_HANDEX_PARAM_SWAP_POKE;

/**
 * �ւ񂵂�i�w�b�_�w��|�P�����j
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8   pokeID;                       ///< �Ώۃ|�PID
  BTL_HANDEX_STR_PARAMS    exStr;    ///< ���������b�Z�[�W
}BTL_HANDEX_PARAM_HENSIN;

/**
 * �C�����[�W��������
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8   pokeID;                       ///< �Ώۃ|�PID
  BTL_HANDEX_STR_PARAMS    exStr;    ///< ���������b�Z�[�W
}BTL_HANDEX_PARAM_FAKE_BREAK;

/**
 * ���߃��U�ɂ�������ԁi������Ƃԓ��j���L�����Z������
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8            pokeID;               ///< �Ώۃ|�PID
  BppContFlag   flag;                 ///< �L�����Z�������ԃt���O
  BTL_HANDEX_STR_PARAMS    exStr;    ///< ���������b�Z�[�W

}BTL_HANDEX_PARAM_TAMEHIDE_CANCEL;

/**
 * �ʒu�w��G�t�F�N�g
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u16  effectNo;                      ///< �G�t�F�N�g�i���o�[
  u8   pos_from;                      ///< �G�t�F�N�g�����ʒu
  u8   pos_to;                        ///< �G�t�F�N�g�I�_�ʒu�i�s�v�Ȃ�BTL_POS_NULL�j
  u16  reservedQuePos;                ///< Que�\��̈�
  u8   fQueReserve;                   ///< Que�\��t���O
}BTL_HANDEX_PARAM_EFFECT_BY_POS;

/**
 * �t�H�����i���o�[�`�F���W
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8   pokeID;                   ///< �|�P����ID
  u8   formNo;                   ///< �t�H�����i���o�[
  BTL_HANDEX_STR_PARAMS    exStr;    ///< ���������b�Z�[�W
}BTL_HANDEX_PARAM_CHANGE_FORM;

/**
 * ���U�G�t�F�N�gIndex�Z�b�g
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8     effIndex;
}BTL_HANDEX_PARAM_SET_EFFECT_IDX;

/**
 *  ���U�G�t�F�N�g�������I�ɗL���ɂ���
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
}BTL_HANDEX_PARAM_WAZAEFFECT_ENABLE;



extern void* BTL_SVF_HANDEX_Push( BTL_SVFLOW_WORK* wk, BtlEventHandlerExhibition eq_type, u8 userPokeID );
extern u8 BTL_SVFTOOL_ExpandPokeID( BTL_SVFLOW_WORK* wk, BtlExPos exPos, u8* dst_pokeID );




#endif
