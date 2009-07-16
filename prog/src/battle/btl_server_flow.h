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
#include "btl_field.h"

/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
typedef enum {
  SVFLOW_RESULT_DEFAULT = 0,  ///< ���̂܂ܑ��s
  SVFLOW_RESULT_POKE_CHANGE,  ///< �|�P��������ւ��t�F�[�Y->�o�g���p��
  SVFLOW_RESULT_BTL_QUIT,     ///< �o�g���I��
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
  BTL_POKEPARAM*  member[ TEMOTI_POKEMAX ];
  BTL_POKEPARAM*  frontMember[ BTL_POSIDX_MAX ];
  u8        memberCount;
  u8        numCoverPos;
  u8        isLocalClient;
  u8        myID;
};

/**
 *  �T�[�o�t���[���n���h������Q�Ƃ��郏�U�p�����[�^
 */

typedef struct {

  WazaID        wazaID;
  PokeTypePair  userType;
  PokeType      wazaType;

}SVFL_WAZAPARAM;

/*--------------------------------------------------------------------------*/
/* Typedefs                                                                 */
/*--------------------------------------------------------------------------*/
typedef struct _BTL_SVFLOW_WORK   BTL_SVFLOW_WORK;
typedef struct _SVCL_WORK         SVCL_WORK;


// server -> server_flow
extern BTL_SVFLOW_WORK* BTL_SVFLOW_InitSystem(
  BTL_SERVER* server, BTL_MAIN_MODULE* mainModule, BTL_POKE_CONTAINER* pokeCon,
  BTL_SERVER_CMD_QUE* que, u32 numClient, HEAPID heapID );

extern void BTL_SVFLOW_QuitSystem( BTL_SVFLOW_WORK* wk );

extern SvflowResult BTL_SVFLOW_Start( BTL_SVFLOW_WORK* wk );
extern SvflowResult BTL_SVFLOW_Start_AfterPokemonIn( BTL_SVFLOW_WORK* wk );
extern SvflowResult BTL_SVFLOW_StartAfterPokeSelect( BTL_SVFLOW_WORK* wk );



// server_flow -> server
extern SVCL_WORK* BTL_SERVER_GetClientWork( BTL_SERVER* server, u8 clientID );
extern SVCL_WORK* BTL_SERVER_GetClientWorkIfEnable( BTL_SERVER* server, u8 clientID );

extern u8 BTL_SVCL_GetNumActPoke( SVCL_WORK* clwk );
extern u8 BTL_SVCL_GetNumCoverPos( SVCL_WORK* clwk );
extern const BTL_ACTION_PARAM* BTL_SVCL_GetPokeAction( SVCL_WORK* clwk, u8 posIdx );


//------------------------------------------------
// �C�x���g�n���h���Ƃ̘A���֐�
//------------------------------------------------
extern BtlRule BTL_SVFLOW_GetRule( BTL_SVFLOW_WORK* wk );
extern BtlPokePos BTL_SVFLOW_CheckExistFrontPokeID( BTL_SVFLOW_WORK* server, u8 pokeID );
extern BOOL BTL_SVFLOW_RECEPT_CheckExistTokuseiPokemon( BTL_SVFLOW_WORK* wk, PokeTokusei tokusei );
extern const BTL_POKEPARAM* BTL_SVFLOW_RECEPT_GetPokeParam( BTL_SVFLOW_WORK* wk, u8 pokeID );
extern u8 BTL_SVFLOW_RECEPT_GetAllFrontPokeID( BTL_SVFLOW_WORK* wk, u8* dst );
extern u8 BTL_SVFLOW_RECEPT_GetAllOpponentFrontPokeID( BTL_SVFLOW_WORK* wk, u8 basePokeID, u8* dst );
extern u16 BTL_SVFLOW_GetTurnCount( BTL_SVFLOW_WORK* wk );

extern void BTL_SERVER_RECEPT_TokuseiWinIn( BTL_SVFLOW_WORK* wk, u8 pokeID );
extern void BTL_SERVER_RECEPT_TokuseiWinOut( BTL_SVFLOW_WORK* wk, u8 pokeID );
extern void BTL_SERVER_RECTPT_StdMessage( BTL_SVFLOW_WORK* wk, u16 msgID );
extern void BTL_SERVER_RECTPT_SetMessage( BTL_SVFLOW_WORK* wk, u16 msgID, u8 pokeID );
extern void BTL_SERVER_RECTPT_SetMessageEx( BTL_SVFLOW_WORK* wk, u16 msgID, u8 pokeID, int arg );
extern void BTL_SERVER_RECEPT_RankDownEffect( BTL_SVFLOW_WORK* wk, BtlExPos exPos, BppValueID statusType, u8 volume, BOOL fAlmost );
extern void BTL_SERVER_RECEPT_RankUpEffect( BTL_SVFLOW_WORK* wk, BtlExPos exPos, BppValueID statusType, u8 volume, BOOL fAlmost );
extern void BTL_SERVER_RECEPT_HP_Add( BTL_SVFLOW_WORK* wk, u8 PokeID, int value );
extern void BTL_SVFLOW_RECEPT_ChangeWeather( BTL_SVFLOW_WORK* wk, BtlWeather weather );
extern void BTL_SVFLOW_RECEPT_CurePokeSick( BTL_SVFLOW_WORK* wk, u8 pokeID );
extern void BTL_SVFLOW_RECEPT_CureWazaSick( BTL_SVFLOW_WORK* wk, u8 pokeID, WazaSick sick );
extern void BTL_SVFLOW_RECEPT_AddSick( BTL_SVFLOW_WORK* wk, u8 targetPokeID, u8 attackPokeID,
  WazaSick sick, BPP_SICK_CONT contParam, BOOL fAlmost );
extern void BTL_SVFLOW_RECEPT_CantEscapeAdd( BTL_SVFLOW_WORK* wk, u8 pokeID, BtlCantEscapeCode code );
extern void BTL_SVFLOW_RECEPT_CantEscapeSub( BTL_SVFLOW_WORK* wk, u8 pokeID, BtlCantEscapeCode code );
extern void BTL_SVFLOW_RECEPT_TraceTokusei( BTL_SVFLOW_WORK* wk, u8 pokeID, u8 targetPokeID );
extern HEAPID BTL_SVFLOW_RECEPT_GetHeapID( BTL_SVFLOW_WORK* wk );
extern u32 BTL_SVFLOW_SimulationDamage( BTL_SVFLOW_WORK* flowWk, u8 atkPokeID, u8 defPokeID, WazaID waza, BOOL fAffinity, BOOL fCritical );
extern BtlLandForm BTL_SVFLOW_GetLandForm( BTL_SVFLOW_WORK* wk );
extern const BTL_PARTY* BTL_SVFLOW_GetPartyData( BTL_SVFLOW_WORK* wk, u8 pokeID );
extern BtlPokePos BTL_SVFLOW_PokeIDtoPokePos( BTL_SVFLOW_WORK* wk, u8 pokeID );
extern BtlPokePos BTL_SVFLOW_ReqWazaTargetAuto( BTL_SVFLOW_WORK* wk, u8 pokeID, WazaID waza );
extern u8 BTL_SVFLOW_PokePosToPokeID( BTL_SVFLOW_WORK* wk, u8 pokePos );
extern BOOL BTL_SVFLOW_GetThisTurnAction( BTL_SVFLOW_WORK* wk, u8 pokeID, BTL_ACTION_PARAM* dst );
extern void* BTL_SVFLOW_GetHandlerTmpWork( BTL_SVFLOW_WORK* wk );




/**
 * ����D�揇�R�[�hA�i���U�D�揇����ʁF0�`7�Ɏ��܂�悤�ɁI�j
 */
typedef enum {

  BTL_SPPRI_A_LOW     = 0,
  BTL_SPPRI_A_DEFAULT = 1,
  BTL_SPPRI_A_HIGH    = 2,

}BtlSpecialPriA;

/**
 * ����D�揇�R�[�hB�i���U�D�揇��艺�ʁF�F0�`7�Ɏ��܂�悤�ɁI�j
 */
typedef enum {

  BTL_SPPRI_B_LOW     = 0,
  BTL_SPPRI_B_DEFAULT = 1,
  BTL_SPPRI_B_HIGH    = 2,

}BtlSpPriA;

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
  BTL_ADDSICK_FAIL_ALREADY,
  BTL_ADDSICK_FAIL_OTHER,

}BtlAddSickFailCode;


/**
 *  �n���h�������i���ʂ��\�o������́j
 */
typedef enum {

  BTL_HANDEX_USE_ITEM,      ///< �A�C�e���g�p

  /*--- �ȉ��A�A�C�e���g�p�ȊO�̌��ʂɎg�� ---*/
  BTL_HANDEX_TOKWIN_IN,     ///< �Ƃ������E�B���h�E�C��
  BTL_HANDEX_TOKWIN_OUT,    ///< �Ƃ������E�B���h�E�A�E�g
  BTL_HANDEX_MESSAGE,       ///< ���b�Z�[�W�\��
  BTL_HANDEX_RECOVER_HP,    ///< HP��
  BTL_HANDEX_DRAIN,         ///< HP�񕜁i�h���C���n�j
  BTL_HANDEX_DAMAGE,        ///< �_���[�W��^����
  BTL_HANDEX_SHIFT_HP,      ///< HP��ύX�i�_���[�W�E�񕜂Ƃ��Ĉ���Ȃ��j
  BTL_HANDEX_RECOVER_PP,    ///< PP��
  BTL_HANDEX_DECREMENT_PP,  ///< PP����
  BTL_HANDEX_CURE_SICK,     ///< ��Ԉُ����
  BTL_HANDEX_ADD_SICK,      ///< ��Ԉُ�ɂ���
  BTL_HANDEX_RANK_EFFECT,   ///< �����N��������
  BTL_HANDEX_SET_RANK,      ///< �����N���w��n�ɋ�����������
  BTL_HANDEX_RESET_RANK,    ///< �����N���ʂ�S�ăt���b�g��
  BTL_HANDEX_RECOVER_RANK,  ///< �}�C�i�X�����N���ʂ݂̂��t���b�g��
  BTL_HANDEX_KILL,          ///< �m���ɂ���
  BTL_HANDEX_CHANGE_TYPE,   ///< �|�P�����̃^�C�v��ς���
  BTL_HANDEX_SET_TURNFLAG,  ///< �^�[���t���O�Z�b�g
  BTL_HANDEX_RESET_TURNFLAG,///< �^�[���t���O�������Z�b�g
  BTL_HANDEX_SET_CONTFLAG,  ///< �p���t���O�Z�b�g
  BTL_HANDEX_RESET_CONTFLAG,///< �p���t���O���Z�b�g
  BTL_HANDEX_SIDEEFF_REMOVE, ///< �T�C�h�G�t�F�N�g�폜
  BTL_HANDEX_ADD_FLDEFF,    ///< �t�B�[���h�G�t�F�N�g�ǉ�
  BTL_HANDEX_REMOVE_FLDEFF, ///< �t�B�[���h�G�t�F�N�g�ǉ�
  BTL_HANDEX_POSEFF_ADD,    ///< �ʒu�G�t�F�N�g�ǉ�
  BTL_HANDEX_CHANGE_TOKUSEI,///< �Ƃ�������������
  BTL_HANDEX_SET_ITEM,      ///< �A�C�e����������
  BTL_HANDEX_SWAP_ITEM,     ///< �A�C�e������ւ�
  BTL_HANDEX_UPDATE_WAZA,   ///< ���U��������

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

typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
}BTL_HANDEX_PARAM_TOKWIN_INOUT;

typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
 BTL_HANDEX_STR_PARAMS  str;
}BTL_HANDEX_PARAM_MESSAGE;


typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
 u16  recoverHP;                     ///< ��HP��
 u8   pokeID;                        ///< �Ώۃ|�P����ID
 BTL_HANDEX_STR_PARAMS     exStr;    ///< ���������b�Z�[�W
}BTL_HANDEX_PARAM_RECOVER_HP;

typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
 u16  recoverHP;                     ///< ��HP��
 u8   recoverPokeID;                 ///< �񕜑Ώۃ|�P����ID
 u8   damagedPokeID;                 ///< �_���[�W���󂯂����̃|�P����ID
 BTL_HANDEX_STR_PARAMS     exStr;    ///< ���������b�Z�[�W
}BTL_HANDEX_PARAM_DRAIN;

typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
 u8   poke_cnt;                      ///< �Ώۃ|�P������
 u8   fSucceedStrEx;                 ///< ���������b�Z�[�W�\��
 u16  succeedStrID;                  ///< ���������b�Z�[�WID
 u8   pokeID[ BTL_POS_MAX ];         ///< �Ώۃ|�P����ID
 u16  damage[ BTL_POS_MAX ];         ///< �_���[�W��
}BTL_HANDEX_PARAM_DAMAGE;

typedef struct {
  BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
  u8     poke_cnt;
  u8     pokeID[ BTL_POS_MAX ];
  int    volume[ BTL_POS_MAX ];
}BTL_HANDEX_PARAM_SHIFT_HP;

typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
 u8   volume;                        ///< PP��
 u8   pokeID;                        ///< �Ώۃ|�P����ID
 u8   wazaIdx;                       ///< �Ώۃ��U�C���f�b�N�X
}BTL_HANDEX_PARAM_PP;

typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
 BtlWazaSickEx   sickCode;           ///< �Ή������Ԉُ�R�[�h�i�g���j
 u8   pokeID[ BTL_POS_MAX ];         ///< �Ώۃ|�P����ID
 u8   poke_cnt;                      ///< �Ώۃ|�P������
 u8   fStdMsgDisable;                ///< �������A�W�����b�Z�[�W���o�͂��Ȃ�
 u8   fExMsg;                        ///< �������A�w�胁�b�Z�[�W���o�͂���
 u16  exStrID;
}BTL_HANDEX_PARAM_CURE_SICK;

typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
 WazaSick       sickID;              ///< ��Ԉُ�ID
 BPP_SICK_CONT  sickCont;            ///< ��Ԉُ�p���p�����[�^
 u8   fAlmost;                       ///< �قڊm��t���O�i����v���Ŏ��s�����ꍇ�Ɍ������b�Z�[�W��\������j
 u8   poke_cnt;                      ///< �Ώۃ|�P������
 u8   pokeID[ BTL_POS_MAX ];         ///< �Ώۃ|�P����ID
 u8   fExMsg;                        ///< �������A���ꃁ�b�Z�[�W�\��
 BTL_HANDEX_STR_PARAMS  exStr;       ///< ���ꃁ�b�Z�[�W�ڍ�
}BTL_HANDEX_PARAM_ADD_SICK;

typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
 WazaRankEffect rankType;            ///< �����N�������
 s8   rankVolume;                    ///< �����N�����l(=0���Ƌ������Z�b�g�j
 u8   fStdMsgDisable;                ///< �W�����b�Z�[�W��\�����Ȃ��t���O
 u8   fAlmost;                       ///< �قڊm��t���O�i����v���Ŏ��s�����ꍇ�Ɍ������b�Z�[�W��\������j
 u8   poke_cnt;                      ///< �Ώۃ|�P������
 u8   pokeID[ BTL_POS_MAX ];         ///< �Ώۃ|�P����ID
}BTL_HANDEX_PARAM_RANK_EFFECT;

typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
 u8 pokeID;                          ///< �Ώۃ|�P����ID
 s8 attack;
 s8 defence;
 s8 sp_attack;
 s8 sp_defence;
 s8 agility;
}BTL_HANDEX_PARAM_SET_RANK;


typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
 u8 pokeID;                          ///< �Ώۃ|�P����ID
}BTL_HANDEX_PARAM_RECOVER_RANK;

typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
 u8   poke_cnt;                      ///< �Ώۃ|�P������
 u8   pokeID[ BTL_POS_MAX ];         ///< �Ώۃ|�P����ID
}BTL_HANDEX_PARAM_RESET_RANK;

typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
 u32   pokeID;
}BTL_HANDEX_PARAM_KILL;

typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  PokeTypePair    next_type;
  u8              pokeID;
}BTL_HANDEX_PARAM_CHANGE_TYPE;

typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  BppTurnFlag     flag;
  u8              pokeID;
}BTL_HANDEX_PARAM_TURNFLAG;

typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  BppContFlag     flag;
  u8              pokeID;
}BTL_HANDEX_PARAM_SET_CONTFLAG;

typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  BtlSideEffect   effect;
  BPP_SICK_CONT   cont;
  u8              pokeID;
}BTL_HANDEX_PARAM_SIDE_EFFECT;

typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8              flags[ BTL_SIDEEFF_BITFLG_BYTES ];
  u8              side;
  u8              fExMsg;
  u16             exStrID;
}BTL_HANDEX_PARAM_SIDEEFF_REMOVE;

typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  BtlFieldEffect           effect;
  BPP_SICK_CONT            cont;
  u8                       sub_param;

  BTL_HANDEX_STR_PARAMS    exStr;

}BTL_HANDEX_PARAM_ADD_FLDEFF;

typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  BtlFieldEffect           effect;
}BTL_HANDEX_PARAM_REMOVE_FLDEFF;

/**
 * �ʒu�G�t�F�N�g�ǉ�
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  BtlPosEffect             effect;
  BtlPokePos               pos;
  int                      param;
}BTL_HANDEX_PARAM_POSEFF_ADD;

typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u16             tokuseiID;    ///< ����������̂Ƃ������iPOKETOKUSEI_NULL�Ȃ�Ƃ������������j
  u8              pokeID;       ///< �Ώۃ|�P����ID
}BTL_HANDEX_PARAM_CHANGE_TOKUSEI;

typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u16             itemID;       ///< ����������̃A�C�e���iITEM_DUMMY_DATA �Ȃ�A�C�e���������j
  u8              pokeID;       ///< �Ώۃ|�P����ID
  u8              fSucceedMsg;
  u8              succeedStrArgCnt;
  u16             succeedStrID;
  int             succeedStrArgs[ BTL_STR_ARG_MAX ];

}BTL_HANDEX_PARAM_SET_ITEM;

/**
 *  �A�C�e������ւ��i�w�b�_�̎g�p�|�P�����Ɓj
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8              pokeID;            ///< �Ώۃ|�P����ID
  u8              fSucceedMsg;
  u8              succeedStrArgCnt;
  u16             succeedStrID;
  int             succeedStrArgs[ BTL_STR_ARG_MAX ];
}BTL_HANDEX_PARAM_SWAP_ITEM;

typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8              pokeID;            ///< �Ώۃ|�P����ID
  u8              wazaIdx;
  u8              ppMax;             ///< PPMax�l:0�Ȃ�f�t�H���g
  u8              fPermanent;        ///< �o�g����܂ŏ����������c��t���O
  WazaID          waza;
}BTL_HANDEX_PARAM_UPDATE_WAZA;



extern void* BTL_SVFLOW_HANDLERWORK_Push( BTL_SVFLOW_WORK* wk, BtlEventHandlerExhibition eq_type, u8 userPokeID );
extern u8 BTL_SERVERFLOW_RECEPT_GetTargetPokeID( BTL_SVFLOW_WORK* wk, BtlExPos exPos, u8* dst_pokeID );




#endif
