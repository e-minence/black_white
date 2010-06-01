//=============================================================================================
/**
 * @file  btl_server_flow_local.h
 * @brief �|�P����WB �o�g���V�X�e�� �T�[�o�R�}���h���������i���C���^�T�u���[�`���Q���狤�L����萔�E�\���́j
 * @author  taya
 *
 * @date  2010.05.28  �쐬
 */
//=============================================================================================
#pragma once

#include "btl_pokeset.h"
#include "btl_pospoke_state.h"
#include "btl_event_factor.h"

enum {
  HANDLER_EXHIBISION_WORK_TOTALSIZE = 800,    ///< �n���h�����������i�[���郏�[�N�̃g�[�^���T�C�Y
  NIGERU_COUNT_MAX = 30,                      ///< �u�ɂ���v�I���񐔃J�E���^�̍ő�l
  MEMBER_CHANGE_COUNT_MAX = 255,              ///< ����ւ��J�E���^�ő�l�i�o�g������p�j
  AFFCOUNTER_MAX = 9999,                      ///< �����J�E���^�ő�l�i�o�g������p�j

  EVENT_HANDLER_TMP_WORK_SIZE = 320,          ///< �n���h�����ꎞ�I�ɗ��p����o�b�t�@�̃T�C�Y
};



//--------------------------------------------------------
/**
 *    �T�[�o�t���[�����t���O
 */
//--------------------------------------------------------
typedef enum {

  FLOWFLG_FIRST_FIGHT,
  FLOWFLG_SET_WAZAEFFECT,

  FLOWFLG_MAX,
  FLOWFLG_BYTE_MAX = ((FLOWFLG_MAX/4)+((FLOWFLG_MAX&3)!=0)*4),

  EVENT_WORK_DEPTH = 16,
  EVENT_WORK_TOTAL_SIZE = 512,

  STRID_NULL = 0xffff,

}FlowFlag;

/**
 *  ���ߖ����p�^�[��
 */
typedef enum {

  SABOTAGE_NONE = 0,    ///< ���ߖ������Ȃ�
  SABOTAGE_OTHER_WAZA,  ///< ���̃��U������ɏo��
  SABOTAGE_GO_SLEEP,    ///< �����Ă��܂�
  SABOTAGE_CONFUSE,     ///< �������U��
  SABOTAGE_DONT_ANY,    ///< �������Ȃ��i���b�Z�[�W�����_���\���j
  SABOTAGE_SLEEPING,    ///< �������܂ܖ��ߖ����i���b�Z�[�W�\���j

}SabotageType;

/**
 *  ���߃��U���茋��
 */
typedef enum {

  TAMEWAZA_NONE = 0,    ///< ���߃��U�ł͂Ȃ�
  TAMEWAZA_START,       ///< ���߃��U�F�����^�[��
  TAMEWAZA_RELEASE,     ///< ���߃��U�F�����[�X�^�[��

}TameWazaSeq;

/**
 *  �ӂ��Ƃ΂��n�̃��U�����p�^�[��
 */
typedef enum {
  PUSHOUT_EFFECT_ESCAPE,    ///< �������̂Ɠ����i�퓬���I������j
  PUSHOUT_EFFECT_CHANGE,    ///< ��������ւ�������i�莝�������Ȃ���Ύ��s����j
  PUSHOUT_EFFECT_MUSTFAIL,  ///< �K�����s����
}PushOutEffect;

/**
 *  �����U�Ăяo���p�����[�^
 */
typedef struct {

  WazaID      wazaID;
  BtlPokePos  targetPos;

}REQWAZA_WORK;

/**
 * �q�b�g�`�F�b�N�p�����[�^
 */

typedef struct {

  u8    countMax;         ///< �ő�q�b�g��
  u8    count;            ///< ���ݏ������Ă����

  u8    fCheckEveryTime;  ///< ����A�q�b�g�`�F�b�N���s����
  u8    fPluralHitWaza;   ///< ������q�b�g���郏�U��
  u8    fPutEffectCmd;    ///< ���U�G�t�F�N�g�����ς�
  u8    pluralHitAffinity;


}HITCHECK_PARAM;

/**
 *  ���U�G�t�F�N�g�����Ǘ�
 */
typedef struct {
  u8  fEnable;      ///< �����m��t���O
  u8  attackerPos;  ///< �����J�n�ʒu
  u8  targetPos;    ///< �Ώۈʒu�i�ʒu�����m�łȂ����U�� BTL_POS_NULL �j
  u8  effectIndex;  ///< �G�t�F�N�gIndex
}WAZAEFF_CTRL;

/**
 *  ���U������p�����[�^
 */
typedef struct {
  u8  robberCount;                    ///< ������|�P������
  u8  robberPokeID[ BTL_POS_MAX ];    ///< �����葤�|�P����ID
  u8  targetPos[ BTL_POS_MAX ];
  u8  targetPokeID[ BTL_POS_MAX ];
}WAZA_ROB_PARAM;

/**
 * �P�A�N�V�������ƕK�v�ȃp�����[�^�X�^�b�N�\���P����
 */
typedef struct {

  BTL_POKESET     TargetOrg;
  BTL_POKESET     Target;
  BTL_POKESET     Friend;
  BTL_POKESET     Enemy;
  BTL_POKESET     Damaged;
  BTL_POKESET     RobTarget;
  BTL_POKESET     psetTmp;
  WAZAEFF_CTRL    effCtrl;
  SVFL_WAZAPARAM  wazaParam;
  HITCHECK_PARAM  hitCheck;
  WAZA_ROB_PARAM  wazaRobParam;
  WAZA_ROB_PARAM  magicCoatParam;
  BtlPokePos      defaultTargetPos;
  u8              fMemberChangeReserve;

}POKESET_STACK_UNIT;

/**
 *  �A�N�V�����D�揇�L�^�\����
 */
typedef struct {

  BTL_POKEPARAM      *bpp;
  BTL_ACTION_PARAM   action;
  u32                priority;

  u8                 clientID;    ///< �N���C�A���gID
  u8                 fDone;       ///< �����I���t���O
  u8                 fIntrCheck;  ///< ���荞�݃`�F�b�N���t���O
  u8                 defaultIdx;  ///< ����D�揈���O�̏������i0�`�j

}ACTION_ORDER_WORK;

/**
 *  �퓬�ɏo�Ă���|�P�����f�[�^�ɏ��ԂɃA�N�Z�X���鏈���̃��[�N
 */
typedef struct {
  u8 clientIdx;
  u8 pokeIdx;
  u8 endFlag;
}FRONT_POKE_SEEK_WORK;

/**
 *  �C�x���g - �n���h���� �A�����[�N�̈�
 */
typedef struct {

  u8    work[ EVENT_WORK_TOTAL_SIZE ];
  u16   size[ EVENT_WORK_DEPTH ];
  u32   sp;

}BTL_EVENT_WORK_STACK;


/**
 *  �n���h�������ۑ��p���[�N�}�l�[�W��
 */
typedef struct {

  u16  stack_ptr;
  u16  read_ptr;
//  u8   f_prev_result;
//  u8   f_total_result;

  union {
    u32 state;
    struct {
      u16  useItem;            ///< �A�C�e�����p�ɂ��Ăяo���Ȃ�A�C�e���i���o�[
      u16  fPushed      :  1;  ///< �P�x�ł����[�N�m�ۂ��ꂽ
      u16  fUsed        :  1;  ///< �P�x�ł��g�p���ꂽ
      u16  fPrevSucceed :  1;  ///< �O��̎g�p����������
      u16  fSucceed     :  1;  ///< �P�x�ł���������
      u16  _padd        : 12;  ///< �P�x�ł���������
    };
  };

  u8   workBuffer[ HANDLER_EXHIBISION_WORK_TOTALSIZE ];

}HANDLER_EXHIBISION_MANAGER;

/**
 * �_���[�W���U�����L�^
 */
typedef struct {

  BtlTypeAff   aff[ BTL_POKEID_MAX ];

} BTL_DMGAFF_REC;

/**
 * �����t���O�� u32 �ŊǗ�
 */
typedef union {
  u32 raw;

  struct {
    u32 hitPluralPoke : 1;   ///< �����|�P�����Ƀq�b�g����
    u32 delayAttack   : 1;   ///< �݂炢�悿���A�x���U��
    u32 _pad          : 29;
  };

  struct {
    u32 magicCoat    : 1;  ///< �}�W�b�N�R�[�g���˕Ԃ��U����
    u32 yokodori     : 1;  ///< �������s��
    u32 reqWaza      : 1;  ///< �h�����U�Ăяo����
  };

}FLAG_SET;

/**
 *  ���b�Z�[�W�\�������R�[���o�b�N�֐��^
 */
typedef void (*pMsgCallback)( BTL_SVFLOW_WORK*, const BTL_POKEPARAM*, void* );


/**
 *  �o���l�v�Z�p���[�N
 */
typedef struct {
  u32  exp;       ///< ���炦��o���l
  u8   fBonus;    ///< ���߂ɂ��炦���t���O

  u8   hp;        ///< HP�w�͒l
  u8   pow;       ///< ���������w�͒l
  u8   def;       ///< �ڂ�����w�͒l
  u8   agi;       ///< ���΂₳�w�͒l
  u8   sp_pow;    ///< �Ƃ������w�͒l
  u8   sp_def;    ///< �Ƃ��ڂ��w�͒l

}CALC_EXP_WORK;


/**
 *  ���U�����J�E���^�i�o�g������p�j
 */
typedef struct {
  u16 putVoid;          ///< �u���ʂȂ��v���o����
  u16 putAdvantage;     ///< �u���ʂ΂���v���o����
  u16 putDisadvantage;  ///< �u���ʂ��܂����v���o����
  u16 recvVoid;         ///< �u���ʖ����v���󂯂�
  u16 recvAdvantage;    ///< �u���ʂ΂���v���󂯂�
  u16 recvDisadvantage; ///< �u���ʂ��܂����v���󂯂�
}WAZA_AFF_COUNTER;


/**
 *  HandEx �߂�l
 */
typedef enum {
  HandExResult_NULL,    ///< ���������Ȃ�
  HandExResult_Fail,    ///< �������������A���ʂ͖�������
  HandExResult_Enable,  ///< ����������A���ʂ�������
}HandExResult;


//-----------------------------------------------------
/**
 *  ���C�����[�N
 */
//-----------------------------------------------------
struct _BTL_SVFLOW_WORK {

  BTL_SERVER*             server;
  const BTL_MAIN_MODULE*  mainModule;
  BTL_POKE_CONTAINER*     pokeCon;
  BTL_SERVER_CMD_QUE*     que;
  u32                     turnCount;
  SvflowResult            flowResult;
  BtlBagMode              bagMode;
  BTL_WAZAREC             wazaRec;
  BTL_DEADREC             deadRec;
  ARCHANDLE*              sinkaArcHandle;
  WAZAEFF_CTRL*           wazaEffCtrl;
  WAZA_ROB_PARAM*         wazaRobParam;
  WAZA_ROB_PARAM*         magicCoatParam;
  HITCHECK_PARAM*         hitCheckParam;
  BTL_ESCAPEINFO          escInfo;
  BTL_LEVELUP_INFO        lvupInfo;
  u32                     simulationCounter;
  u8                      cmdBuildStep;
  u8                      actOrderStep;
  u8                      turnCheckStep;
  u8                      defaultTargetPos;

  HEAPID  heapID;

  u8      numActOrder;
  u8      numEndActOrder;
  u8      getPokePos;
  u8      numRelivePoke;
  u8      nigeruCount;
  u8      wazaEffIdx;
  u8      MemberOutIntrPokeCount;
  u8      thruDeadMsgPokeID;
  u8      fMemberOutIntr     : 1;
  u8      fWinBGMPlayWild    : 1;
  u8      fEscMsgDisped      : 1;
  u8      fMemberChangeReserve : 1;

  u8      MemberOutIntrPokeID[ BTL_POS_MAX ];
  u8      relivedPokeID[ BTL_POKEID_MAX ];
  u8      pokeDeadFlag[ BTL_POKEID_MAX ];
  u8      pokeInFlag [ BTL_POKEID_MAX ];
  u8      memberChangeCount[ BTL_CLIENT_MAX ];

  ACTION_ORDER_WORK   actOrder[ BTL_POS_MAX ];
  ACTION_ORDER_WORK   actOrderTmp;

  // POKESET �� STACK�@�\�p�i���荞�݂���������̂� actOrder ����+1 ���K�v�j
  BTL_POKESET*   psetTargetOrg;
  BTL_POKESET*   psetTarget;
  BTL_POKESET*   psetFriend;
  BTL_POKESET*   psetEnemy;
  BTL_POKESET*   psetDamaged;
  BTL_POKESET*   psetRobTarget;
  BTL_POKESET*   psetTmp;
  POKESET_STACK_UNIT  pokesetUnit[ BTL_POS_MAX+1 ];
  u32            pokesetStackPtr;


  BTL_POKESET         pokesetMemberInProc;

  SVFL_WAZAPARAM         *wazaParam;
  BTL_POSPOKE_WORK       pospokeWork;
  BTL_HANDEX_STR_PARAMS  strParam;
  CALC_EXP_WORK          calcExpWork[ BTL_PARTY_MEMBER_MAX ];

  BTL_EVENT_WORK_STACK        eventWork;
  HANDLER_EXHIBISION_MANAGER  HEManager;
  BPP_WAZADMG_REC             wazaDamageRec;
  WazaID                      prevExeWaza;
  SabotageType                currentSabotageType;
  WAZA_AFF_COUNTER            affCounter;
  BTL_DMGAFF_REC              dmgAffRec;

  u8          flowFlags[ FLOWFLG_BYTE_MAX ];
  u8          handlerTmpWork[ EVENT_HANDLER_TMP_WORK_SIZE ];
};



extern HandExResult BTL_SVF_HandEx_Root( BTL_SVFLOW_WORK* wk, u16 useItemID );
