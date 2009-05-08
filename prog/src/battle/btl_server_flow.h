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

#include "btl_main.h"
#include "btl_adapter.h"
#include "btl_server.h"
#include "btl_server_cmd.h"
#include "btl_event.h"
#include "btl_string.h"

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
extern BTL_POKEPARAM* BTL_SVCL_GetFrontPokeData( SVCL_WORK* clwk, u8 posIdx );


//------------------------------------------------
// �C�x���g�n���h���Ƃ̘A���֐�
//------------------------------------------------
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
extern void BTL_SVFLOW_RECEPT_ChangePokeType( BTL_SVFLOW_WORK* wk, u8 pokeID, PokeType type );
extern void BTL_SVFLOW_RECEPT_ChangePokeForm( BTL_SVFLOW_WORK* wk, u8 pokeID, u8 formNo );
extern void BTL_SVFLOW_RECEPT_CantEscapeAdd( BTL_SVFLOW_WORK* wk, u8 pokeID, BtlCantEscapeCode code );
extern void BTL_SVFLOW_RECEPT_CantEscapeSub( BTL_SVFLOW_WORK* wk, u8 pokeID, BtlCantEscapeCode code );
extern void BTL_SVFLOW_RECEPT_TraceTokusei( BTL_SVFLOW_WORK* wk, u8 pokeID, u8 targetPokeID );
extern HEAPID BTL_SVFLOW_RECEPT_GetHeapID( BTL_SVFLOW_WORK* wk );


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
  BTL_HANDEX_MESSAGE_STD,   ///< STD���b�Z�[�W�\��
  BTL_HANDEX_MESSAGE_SET,   ///< SET���b�Z�[�W�\��
  BTL_HANDEX_RECOVER_HP,    ///< HP��
  BTL_HANDEX_RECOVER_PP,    ///< PP��
  BTL_HANDEX_CURE_SICK,     ///< ��Ԉُ����
  BTL_HANDEX_ADD_SICK,      ///< ��Ԉُ�ɂ���
  BTL_HANDEX_RANK_EFFECT,   ///< �����N��������
  BTL_HANDEX_RECOVER_RANK,  ///< �}�C�i�X�����N���t���b�g�ɖ߂�
  BTL_HANDEX_DAMAGE,        ///< �_���[�W��^����

  BTL_HANDEX_MAX,

}BtlEventHandlerExhibition;

/**
 *  �n���h���������[�N���L�w�b�_
 */
typedef struct {
  u8    equip;            ///< �n���h�������iBtlEventHandlerEquip�j
  u8    userPokeID;       ///< �n���h����̂̃|�P����ID
  u8    size;             ///< �\���̃T�C�Y
  u8    tokwin_flag  : 1; ///< ��̃|�P�����̂Ƃ��������ʂł��邱�Ƃ�\������
  u8    _padding     : 7; ///< �p�f�B���O
}BTL_HANDEX_PARAM_HEADER;

typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
}BTL_HANDEX_PARAM_USE_ITEM;

typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
}BTL_HANDEX_PARAM_TOKWIN_INOUT;

typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
 int  args[ BTL_STR_ARG_MAX ];
 u16  strID;
}BTL_HANDEX_PARAM_MESSAGE;


typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
 u16  recoverHP;                     ///< ��HP��
 u8   pokeID;                        ///< �Ώۃ|�P����ID
}BTL_HANDEX_PARAM_RECOVER_HP;

typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
 u8   recoverPP;                     ///< ��PP��
 u8   pokeID;                        ///< �Ώۃ|�P����ID
 u8   wazaIdx;                       ///< �Ώۃ��U�C���f�b�N�X
}BTL_HANDEX_PARAM_RECOVER_PP;

typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
 WazaSick   sickID;                  ///< �Ή������Ԉُ�ID�iWAZASICK_NULL�ŃI�[���}�C�e�B�j
 u8   pokeID;                        ///< �Ώۃ|�P����ID
}BTL_HANDEX_PARAM_CURE_SICK;

typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
 WazaSick       sickID;              ///< ��Ԉُ�ID
 BPP_SICK_CONT  sickCont;            ///< ��Ԉُ�p���p�����[�^
 u8   fAlmost;                       ///< �قڊm��t���O�i����v���Ŏ��s�����ꍇ�Ɍ������b�Z�[�W��\������j
 u8   poke_cnt;                      ///< �Ώۃ|�P������
 u8   pokeID[ BTL_POS_MAX ];         ///< �Ώۃ|�P����ID
 u8   fExMsg;                        ///< �������A���ꃁ�b�Z�[�W�\��
 u8   exStrArgCnt;                   ///< �������̓��ꃁ�b�Z�[�W�����̐�
 u16  exStrID;                       ///< �������̓��ꃁ�b�Z�[�WID
 int  exStrArgs[ BTL_STR_ARG_MAX ];  ///< �������̓��ꃁ�b�Z�[�W����
}BTL_HANDEX_PARAM_ADD_SICK;

typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
 WazaRankEffect rankType;            ///< �����N�������
 s8   rankVolume;                    ///< �����N�����l
 u8   fAlmost;                       ///< �قڊm��t���O�i����v���Ŏ��s�����ꍇ�Ɍ������b�Z�[�W��\������j
 u8   poke_cnt;                      ///< �Ώۃ|�P������
 u8   pokeID[ BTL_POS_MAX ];         ///< �Ώۃ|�P����ID
}BTL_HANDEX_PARAM_RANK_EFFECT;

typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
 u8 pokeID;                          ///< �Ώۃ|�P����ID
}BTL_HANDEX_PARAM_RECOVER_RANK;

typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< ���L�w�b�_
 u8   poke_cnt;                      ///< �Ώۃ|�P������
 u8   pokeID[ BTL_POS_MAX ];         ///< �Ώۃ|�P����ID
 u16  damage[ BTL_POS_MAX ];         ///< �_���[�W��
}BTL_HANDEX_PARAM_DAMAGE;

extern void* BTL_SVFLOW_HANDLERWORK_Push( BTL_SVFLOW_WORK* wk, BtlEventHandlerExhibition eq_type, u8 userPokeID );



#endif
