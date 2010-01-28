//=============================================================================================
/**
 * @file  btl_server_flow.c
 * @brief �|�P����WB �o�g���V�X�e�� �T�[�o�R�}���h��������
 * @author  taya
 *
 * @date  2009.03.06  �쐬
 */
//=============================================================================================
#include <gflib.h>

#include "sound\pm_sndsys.h"
#include "waza_tool\wazadata.h"
#include "waza_tool\wazano_def.h"
#include "item\item.h"
#include "item\itemtype_def.h"
#include "poke_tool\shinka_check.h"

#include "btl_event.h"
#include "btl_server_cmd.h"
#include "btl_field.h"
#include "btl_sick.h"
#include "btl_pospoke_state.h"
#include "btl_pokeset.h"
#include "btl_shooter.h"
#include "btlv\btlv_effect.h"

#include "handler\hand_tokusei.h"
#include "handler\hand_item.h"
#include "handler\hand_waza.h"
#include "handler\hand_side.h"
#include "handler\hand_pos.h"

#include "btl_server_local.h"
#include "btl_server.h"
#include "btl_server_flow.h"

#define SOGA_DEBUG  //�b���soga���f�o�b�O�����ӏ�



//--------------------------------------------------------
/**
 *    �T�[�o�t���[�����t���O
 */
//--------------------------------------------------------
enum {
  HANDLER_EXHIBISION_WORK_TOTALSIZE = 512,    ///< �n���h�����������i�[���郏�[�N�̃g�[�^���T�C�Y
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
 *  �A�N�V�����D�揇�L�^�\����
 */
typedef struct {

  BTL_POKEPARAM      *bpp;
  BTL_ACTION_PARAM   action;
  REQWAZA_WORK       reqWaza;
  u32                priority;

  u8  clientID;   ///< �N���C�A���gID
  u8  pokeIdx;    ///< ���̃N���C�A���g�́A���̖ځH 0�`

  u8                 fDone;   ///< �����I���t���O
  u8                 fIntr;   ///< ���荞�݋��t���O

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

  u8   workBuffer[ HANDLER_EXHIBISION_WORK_TOTALSIZE ];

}HANDLER_EXHIBISION_MANAGER;


/**
 *  ���b�Z�[�W�\�������R�[���o�b�N�֐��^
 */
typedef void (*pMsgCallback)( BTL_SVFLOW_WORK*, const BTL_POKEPARAM*, void* );


/**
 *  �o���l�v�Z�p���[�N
 */
typedef struct {
  u8   fBonus;    ///< ���߂ɂ��炦���t���O
  u32  exp;       ///< ���炦��o���l
}CALC_EXP_WORK;


/**
 *  ���U�G�t�F�N�g�����Ǘ�
 */
typedef struct {
  u8  fEnable;      ///< �����m��t���O
  u8  attackerPos;  ///< �����J�n�ʒu
  u8  targetPos;    ///< �Ώۈʒu�i�ʒu�����m�łȂ����U�� BTL_POS_NULL �j
}WAZAEFF_CTRL;

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
  WAZAEFF_CTRL            wazaEffCtrl;

  HEAPID  heapID;

  u8      numClient;
  u8      numActOrder;
  u8      numEndActOrder;
  u8      escapeClientID;
  u8      getPokePos;
  u8      numRelivePoke;
  u8      relivedPokeID[ BTL_POKEID_MAX ];
  u8      pokeDeadFlag[ BTL_POKEID_MAX ];
  u8      pokeInFlag [ BTL_POKEID_MAX ];

  ACTION_ORDER_WORK   actOrder[ BTL_POS_MAX ];
  ACTION_ORDER_WORK   actOrderTmp;

  BTL_POKESET         pokesetTarget;
  BTL_POKESET         pokesetSubTarget;
  BTL_POKESET         pokesetDamaged;
  BTL_POKESET         pokesetRobTarget;

  SVFL_WAZAPARAM      wazaParam;
  BTL_POSPOKE_WORK    pospokeWork;
  BTL_HANDEX_STR_PARAMS strParam;
  CALC_EXP_WORK       calcExpWork[ BTL_PARTY_MEMBER_MAX ];

  BTL_EVENT_WORK_STACK        eventWork;
  HANDLER_EXHIBISION_MANAGER  HEManager;
  BPP_WAZADMG_REC             wazaDamageRec;
  WazaID                      prevExeWaza;

  u8          flowFlags[ FLOWFLG_BYTE_MAX ];
  u8          handlerTmpWork[ EVENT_HANDLER_TMP_WORK_SIZE ];
};

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void clearPokeCantActionFlag( BTL_SVFLOW_WORK* wk );
static u32 ActOrderProc_Main( BTL_SVFLOW_WORK* wk, u32 startOrderIdx );
static BOOL ActOrderProc_PokeIn( BTL_SVFLOW_WORK* wk );
static BOOL scproc_CheckShowdown( BTL_SVFLOW_WORK* wk );
static void scproc_countup_shooter_energy( BTL_SVFLOW_WORK* wk );
static BOOL reqChangePokeForServer( BTL_SVFLOW_WORK* wk );
static void scproc_BeforeFirstFight( BTL_SVFLOW_WORK* wk );
static void scproc_CheckFlyingAllPoke( BTL_SVFLOW_WORK* wk );
static BOOL scproc_CheckFlying( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static BOOL scEvent_CheckFlying( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static u8 sortClientAction( BTL_SVFLOW_WORK* wk, ACTION_ORDER_WORK* order, u32 orderMax );
static void reqWazaUseForCalcActOrder( BTL_SVFLOW_WORK* wk, ACTION_ORDER_WORK* order );
static u8 scEvent_GetWazaPriority( BTL_SVFLOW_WORK* wk, WazaID waza, const BTL_POKEPARAM* bpp );
static u16 scEvent_CalcAgility( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker );
static inline WazaID ActOrder_GetWazaID( const ACTION_ORDER_WORK* wk );
static ACTION_ORDER_WORK* SearchActOrderByPokeID( BTL_SVFLOW_WORK* wk, u8 pokeID );
static ACTION_ORDER_WORK* SearchActOrderByWaza( BTL_SVFLOW_WORK* wk, WazaID waza, u8 startIndex );
static int IntrActOrder( BTL_SVFLOW_WORK* wk, const ACTION_ORDER_WORK* actOrder, u32 intrIndex );
static void SendLastActOrder( BTL_SVFLOW_WORK* wk, const ACTION_ORDER_WORK* actOrder );
static void ActOrder_Proc( BTL_SVFLOW_WORK* wk, ACTION_ORDER_WORK* actOrder );
static BOOL ActOrder_IntrProc( BTL_SVFLOW_WORK* wk, u8 intrPokeID );
static BOOL ActOrder_IntrReserve( BTL_SVFLOW_WORK* wk, u8 intrPokeID );
static BOOL ActOrder_IntrReserveByWaza( BTL_SVFLOW_WORK* wk, WazaID waza );
static BOOL ActOrder_SendLast( BTL_SVFLOW_WORK* wk, u8 pokeID );
static inline void FlowFlg_Set( BTL_SVFLOW_WORK* wk, FlowFlag flg );
static inline void FlowFlg_Clear( BTL_SVFLOW_WORK* wk, FlowFlag flg );
static inline BOOL FlowFlg_Get( BTL_SVFLOW_WORK* wk, FlowFlag flg );
static inline void FlowFlg_ClearAll( BTL_SVFLOW_WORK* wk );
static void wazaEffCtrl_Init( WAZAEFF_CTRL* ctrl, BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKESET* rec );
static inline void wazaEffCtrl_SetEnable( WAZAEFF_CTRL* ctrl );
static inline BOOL wazaEffCtrl_IsEnable( const WAZAEFF_CTRL* ctrl );
static inline BOOL IsBppExist( const BTL_POKEPARAM* bpp );
static void FRONT_POKE_SEEK_InitWork( FRONT_POKE_SEEK_WORK* fpsw, BTL_SVFLOW_WORK* wk );
static BOOL FRONT_POKE_SEEK_GetNext( FRONT_POKE_SEEK_WORK* fpsw, BTL_SVFLOW_WORK* wk, BTL_POKEPARAM** bpp );
static void scproc_Move( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static void scproc_MoveCore( BTL_SVFLOW_WORK* wk, u8 clientID, u8 posIdx1, u8 posIdx2 );
static BOOL scproc_NigeruCmd( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static BOOL scEvent_SkipNigeruCalc( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static BOOL scproc_NigeruCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static BOOL scEvent_SkipNigeruForbid( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static BOOL scEvent_CheckNigeruForbid( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static BOOL scEvent_NigeruExMessage( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static void scproc_MemberInCore( BTL_SVFLOW_WORK* wk, u8 clientID, u8 posIdx, u8 nextPokeIdx );
static void scproc_MemberInForChange( BTL_SVFLOW_WORK* wk, u8 clientID, u8 posIdx, u8 next_poke_idx, BOOL fPutMsg );
static void scproc_AfterMemberIn( BTL_SVFLOW_WORK* wk );
static void scPut_MemberOutMessage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static void scPut_MemberOut( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static void scproc_TrainerItem_Root( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, u8 actParam, u8 targetIdx );
static void scproc_TrainerItem_BallRoot( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID );
static u8 ItemEff_SleepRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_PoisonRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_YakedoRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_KooriRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_MahiRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_KonranRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_MeromeroRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_EffectGuard( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_Relive( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_AttackRank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_DefenceRank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_SPAttackRank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_SPDefenceRank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_AgilityRank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_HitRank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_CriticalUp( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_PP_Rcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_AllPP_Rcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_HP_Rcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_Common_Cure( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, WazaSick sickID );
static u8 ItemEff_Common_Rank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, BppValueID rankType );
static u8 ShooterEff_ItemCall( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ShooterEff_SkillCall( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ShooterEff_ItemDrop( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ShooterEff_FlatCall( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static void scproc_MemberChange( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* outPoke, u8 nextPokeIdx );
static BOOL scproc_MemberOutForChange( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* outPoke );
static BOOL scproc_MemberOutCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* outPoke );
static void scEvent_MemberOutFixed( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* outPoke );
static void scproc_Fight( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, REQWAZA_WORK* reqWaza, BTL_ACTION_PARAM* action );
static void scproc_WazaExe_Decide( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza );
static void scEvent_WazaExeDecide( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza );
static BOOL scproc_Fight_CheckDelayWazaSet( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza, BtlPokePos targetPos );
static BOOL scEvent_CheckDelayWazaSet( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, BtlPokePos targetPos );
static void scEvent_StartWazaSeq( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza );
static void scEvent_EndWazaSeq( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza );
static void scEvent_WazaRob( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* robPoke, const BTL_POKEPARAM* orgAtkPoke, WazaID waza );
static void scEvent_WazaReflect( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* robPoke, const BTL_POKEPARAM* orgAtkPoke, WazaID waza );
static u8 scproc_Check_WazaRob( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza,
  BTL_POKESET* rec, BtlPokePos* targetPos, u8 *fReflect );
static BOOL scEvent_CheckWazaRob( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza,
  BTL_POKESET* targetRec, u8* robberPokeID, u8* robTargetPokeID, u8* fReflect );
static BOOL scproc_Fight_CheckReqWazaFail( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID orgWazaID );
static void scPut_ReqWazaEffect( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, WazaID waza, BtlPokePos targetPos );
static u8 registerWazaTargets( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BtlPokePos targetPos,
  const SVFL_WAZAPARAM* wazaParam, BTL_POKESET* rec );
static u8 registerTarget_single( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BtlPokePos targetPos,
  const SVFL_WAZAPARAM* wazaParam, u8 intrPokeID, BTL_POKESET* rec );
static u8 registerTarget_double( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BtlPokePos targetPos,
  const SVFL_WAZAPARAM* wazaParam, u8 intrPokeID, BTL_POKESET* rec );
static u8 registerTarget_triple( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BtlPokePos targetPos,
  const SVFL_WAZAPARAM* wazaParam, u8 intrPokeID, BTL_POKESET* rec );
static BTL_POKEPARAM* get_opponent_pokeparam( BTL_SVFLOW_WORK* wk, BtlPokePos pos, u8 pokeSideIdx );
static BTL_POKEPARAM* get_next_pokeparam( BTL_SVFLOW_WORK* wk, BtlPokePos pos );
static BOOL correctTargetDead( BTL_SVFLOW_WORK* wk, BtlRule rule, const BTL_POKEPARAM* attacker,
  const SVFL_WAZAPARAM* wazaParam, BtlPokePos targetPos, BTL_POKESET* rec );
static void scproc_Fight_WazaExe( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza, BTL_POKESET* targetRec );
static void scproc_WazaExe_Effective( BTL_SVFLOW_WORK* wk, u8 pokeID, WazaID waza );
static void scproc_WazaExe_NotEffective( BTL_SVFLOW_WORK* wk, u8 pokeID, WazaID waza );
static void scproc_WazaExe_Done( BTL_SVFLOW_WORK* wk, u8 pokeID, WazaID waza );
static void scEvent_WazaExeEnd_Common( BTL_SVFLOW_WORK* wk, u8 pokeID, WazaID waza, BtlEventType eventID );
static BOOL IsMustHit( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target );
static void flowsub_checkNotEffect( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, const BTL_POKEPARAM* attacker, BTL_POKESET* targets );
static void flowsub_checkWazaAvoid( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, const BTL_POKEPARAM* attacker, BTL_POKESET* targets );
static BOOL IsTripleFarPos( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, WazaID waza );
static BOOL scEvent_CheckHit( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender,
  const SVFL_WAZAPARAM* wazaParam );
static void scPut_WazaEffect( BTL_SVFLOW_WORK* wk, WazaID waza, const WAZAEFF_CTRL* effCtrl, u32 que_reserve_pos );
static BOOL scproc_Fight_TameWazaExe( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, const BTL_POKESET* targetRec, WazaID waza );
static BOOL scproc_Fight_CheckWazaExecuteFail_1st( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza );
static BOOL scproc_Fight_CheckWazaExecuteFail_2nd( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza );
static BOOL scproc_Fight_CheckConf( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker );
static BOOL scproc_Fight_CheckMeroMero( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker );
static void scproc_PokeSickCure_WazaCheck( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza );
static void scproc_WazaExecuteFailed( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza, SV_WazaFailCause fail_cause );
static void scPut_WazaExecuteFailMsg( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, WazaID waza, SV_WazaFailCause cause );
static void scproc_decrementPPUsedWaza( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, u8 wazaIdx, BTL_POKESET* rec );
static BOOL scproc_decrementPP( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u8 wazaIdx, u8 volume );
static void scproc_Fight_Damage_Root( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
   BTL_POKEPARAM* attacker, BTL_POKESET* targets );
static void scproc_Fight_Damage_ToRecover( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker,
  const SVFL_WAZAPARAM* wazaParam, BTL_POKESET* targets );
static void scproc_Fight_Damage_side( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker,
  BTL_POKESET* targets, fx32 dmg_ratio );
static void scproc_Fight_Damage_Determine( BTL_SVFLOW_WORK* wk,
  BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam );
static void scEvent_WazaDamageDetermine( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam );
static void scproc_Fight_Damage_side_single( BTL_SVFLOW_WORK* wk,
      BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam,
      fx32 targetDmgRatio, BtlTypeAff affinity );
static void svflowsub_damage_act_singular(  BTL_SVFLOW_WORK* wk,
    BTL_POKEPARAM* attacker,  BTL_POKEPARAM* defender,
    const SVFL_WAZAPARAM* wazaParam, BtlTypeAff affinity, fx32 targetDmgRatio );
static void scproc_Fight_damage_side_plural( BTL_SVFLOW_WORK* wk,
    BTL_POKEPARAM* attacker, BTL_POKESET* targets, BtlTypeAff* affAry, const SVFL_WAZAPARAM* wazaParam, fx32 dmg_ratio );
static u32 MarumeDamage( const BTL_POKEPARAM* bpp, u32 damage );
static BppKoraeruCause scEvent_CheckKoraeru( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, u16* damage );
static void scproc_Koraeru( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppKoraeruCause cause );
static void scEvent_KoraeruExe( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppKoraeruCause cause );
static void wazaDmgRec_Add( const BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam, u16 damage );
static void scproc_WazaAdditionalEffect( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
  BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, u32 damage );
static void scproc_WazaDamageReaction( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender,
  const SVFL_WAZAPARAM* wazaParam, BtlTypeAff affinity, u32 damage, BOOL critical_flag );
static void scproc_WazaDamageAfter( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker,
  const SVFL_WAZAPARAM* wazaParam, u32 damage, BOOL critical_flag );
static void scproc_CheckItemReaction( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static void scEvent_CheckItemReaction( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static void scproc_Fight_Damage_After( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKESET* targets );
static void scproc_Fight_Damage_Shrink( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, BTL_POKESET* targets );
static void scproc_Fight_Damage_KoriCure( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
  BTL_POKEPARAM* attacker, BTL_POKESET* targets );
static BOOL scproc_AddShrinkCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, WazaID waza, u32 per );
static void scproc_Fight_Damage_Drain( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, BTL_POKESET* targets );
static BOOL scproc_DrainCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, u16 drainHP );
static void scEvent_DamageProcEnd( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, BTL_POKESET* targets, WazaID waza );
static BOOL scEvent_CalcDamage( BTL_SVFLOW_WORK* wk,
    const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam,
    BtlTypeAff typeAff, fx32 targetDmgRatio, BOOL criticalFlag, u16* dstDamage );
static void scproc_Fight_Damage_Kickback( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza, u32 wazaDamage );
static BOOL scproc_SimpleDamage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u32 damage, BTL_HANDEX_STR_PARAMS* str );
static BOOL scproc_UseItemEquip( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static BOOL scEvent_CheckItemEquipFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u16 itemID );
static void scproc_KillPokemon( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static void scproc_Fight_Damage_AddSick( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target );
static WazaSick scEvent_CheckWazaAddSick( BTL_SVFLOW_WORK* wk, WazaID waza,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, BPP_SICK_CONT* pSickCont );
static void scproc_Fight_SimpleSick( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, BTL_POKESET* targetRec );
static BOOL scproc_Fight_WazaSickCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target,
  WazaID waza, WazaSick sick, BPP_SICK_CONT sickCont, BOOL fAlmost );
static WazaSick scEvent_DecideSpecialWazaSick( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, BTL_HANDEX_STR_PARAMS* str );
static void scEvent_WazaSickCont( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target,
  WazaSick sick, BPP_SICK_CONT* sickCont );
static BOOL scproc_AddSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, BTL_POKEPARAM* attacker,
  WazaSick sick, BPP_SICK_CONT sickCont, BOOL fAlmost, BOOL fDefaultMsgEnable );
static BOOL addsick_core( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, BTL_POKEPARAM* attacker,
  WazaSick sick, BPP_SICK_CONT sickCont, BOOL fAlmost  );
static BtlAddSickFailCode addsick_check_fail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target,
  WazaSick sick, BPP_SICK_CONT sickCont );
static BtlWeather scEvent_GetWeather( BTL_SVFLOW_WORK* wk );
static BOOL scEvent_StdSick_CheckFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, WazaSick sick  );
static void scEvent_AddSick_Failed( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, WazaSick sick );
static void scEvent_PokeSickFixed( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, const BTL_POKEPARAM* attacker, PokeSick sick );
static void scEvent_WazaSickFixed( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, const BTL_POKEPARAM* attacker, WazaSick sick );
static void scEvent_IekiFixed( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target );
static void scproc_Fight_Damage_AddEffect( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target );
static BOOL scEvent_CheckAddRankEffectOccur( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target );
static void scproc_Fight_SimpleEffect( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKESET* targetRec );
static BOOL scproc_WazaRankEffect_Common( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, BOOL fAlmost );
static void scEvent_GetWazaRankEffectValue( BTL_SVFLOW_WORK* wk, WazaID waza, u32 idx,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, WazaRankEffect* effect, int* volume );
static BOOL scproc_RankEffectCore( BTL_SVFLOW_WORK* wk, u8 atkPokeID, BTL_POKEPARAM* target,
  WazaRankEffect effect, int volume, u8 wazaUsePokeID, u16 itemID, BOOL fAlmost, BOOL fStdMsg );
static int scEvent_RankValueChange( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, WazaRankEffect rankType,
  u8 wazaUsePokeID, u16 itemID, int volume );
static void scproc_Fight_EffectSick( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKESET* targetRec );
static void scproc_Fight_SimpleRecover( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, BTL_POKESET* targetRec );
static BOOL scproc_RecoverHP( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 recoverHP );
static void scproc_Fight_Ichigeki( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKESET* targets );
static void scproc_Fight_PushOut( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, BTL_POKESET* targetRec );
static BOOL scproc_PushOutCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target,
  BOOL fForceChange, BTL_HANDEX_STR_PARAMS* succeedMsg );
static PushOutEffect check_pushout_effect( BTL_SVFLOW_WORK* wk );
static u8 get_pushout_nextpoke_idx( BTL_SVFLOW_WORK* wk, const SVCL_WORK* clwk );
static BOOL scEvent_CheckPushOutFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target );
static void scput_Fight_FieldEffect( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker );
static BOOL scproc_ChangeWeatherCore( BTL_SVFLOW_WORK* wk, BtlWeather weather, u8 turn );
static u8 scEvent_WazaWeatherTurnUp( BTL_SVFLOW_WORK* wk, BtlWeather weather, const BTL_POKEPARAM* attacker );
static BOOL scEvent_CheckChangeWeather( BTL_SVFLOW_WORK* wk, BtlWeather weather, u8* turn );
static BOOL scproc_FieldEffectCore( BTL_SVFLOW_WORK* wk, BtlFieldEffect effect, BPP_SICK_CONT contParam );
static void scEvent_FieldEffectCall( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza );
static void scput_Fight_Uncategory( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKESET* targets );
static void scput_Fight_Uncategory_SkillSwap( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BTL_POKESET* targetRec );
static void scproc_Migawari_Create( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static BOOL scproc_Migawari_Damage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 damage );
static void scproc_Migawari_CheckNoEffect( BTL_SVFLOW_WORK* wk, SVFL_WAZAPARAM* wazaParam,
  BTL_POKEPARAM* attacker, BTL_POKESET* rec );
static BOOL scEvent_UnCategoryWaza( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
  const BTL_POKEPARAM* attacker, BTL_POKESET* targets );
static void scproc_TurnCheck( BTL_SVFLOW_WORK* wk );
static void   scproc_turncheck_CommSupport( BTL_SVFLOW_WORK* wk );
static void scproc_turncheck_sub( BTL_SVFLOW_WORK* wk, BTL_POKESET* pokeSet, BtlEventType event_type );
static void scEvent_TurnCheck( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, BtlEventType event_type );
static void scproc_turncheck_sick( BTL_SVFLOW_WORK* wk, BTL_POKESET* pokeSet );
static u32 scEvent_SickDamage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, WazaSick sickID, u32 damage );
static void scproc_turncheck_side( BTL_SVFLOW_WORK* wk );
static void scproc_turncheck_side_callback( BtlSide side, BtlSideEffect sideEffect, void* arg );
static void scproc_turncheck_field( BTL_SVFLOW_WORK* wk );
static void turncheck_field_callback( BtlFieldEffect effect, void* arg );
static void scproc_FieldEff_End( BTL_SVFLOW_WORK* wk, BtlFieldEffect effect );
static void scproc_turncheck_weather( BTL_SVFLOW_WORK* wk, BTL_POKESET* pokeSet );
static int scEvent_CheckWeatherReaction( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, BtlWeather weather, u32 damage );
static void scPut_WeatherDamage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BtlWeather weather, int damage );
static BOOL scproc_CheckDeadCmd( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* poke );
static void scproc_ClearPokeDependEffect( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* poke );
static void CurePokeDependSick_CallBack( void* wk_ptr, BTL_POKEPARAM* bpp, WazaSick sickID, u8 dependPokeID );
static void scproc_CheckExpGet( BTL_SVFLOW_WORK* wk );
static void scproc_GetExp( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* deadPoke );
static void getexp_calc( BTL_SVFLOW_WORK* wk, const BTL_PARTY* party, const BTL_POKEPARAM* deadPoke, CALC_EXP_WORK* result );
static u32 getexp_calc_adjust_level( const BTL_POKEPARAM* bpp, u32 base_exp, u16 getpoke_lv, u16 deadpoke_lv );
static void getexp_make_cmd( BTL_SVFLOW_WORK* wk, BTL_PARTY* party, const CALC_EXP_WORK* calcExp );
static inline u32 _calc_adjust_level_sub( u32 val );
static inline int roundValue( int val, int min, int max );
static inline int roundMin( int val, int min );
static inline int roundMax( int val, int max );
static inline BOOL perOccur( BTL_SVFLOW_WORK* wk, u8 per );
static void eventWork_Init( BTL_EVENT_WORK_STACK* stack );
static void* eventWork_Push( BTL_EVENT_WORK_STACK* stack, u32 size );
static void eventWork_Pop( BTL_EVENT_WORK_STACK* stack, void* adrs );
static void scPut_EffectByPokePos( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u16 effectNo );
static void scPut_CantAction( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static void scPut_ConfCheck( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static void scPut_MeromeroAct( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static void scPut_ConfDamage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u32 damage );
static void scPut_CurePokeSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, WazaSick sick );
static void scPut_WazaMsg( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza );
static WazaSick scPut_CureSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BtlWazaSickEx exSickCode, BPP_SICK_CONT* oldCont );
static WazaSick trans_sick_code( const BTL_POKEPARAM* bpp, BtlWazaSickEx exCode );
static void scPut_ResetSameWazaCounter( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static void scPut_WazaFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza );
static void scPut_WazaAvoid( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* defender, WazaID waza );
static void scput_WazaNoEffect( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* defender );
static void scPut_WazaDamageSingle( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
  BTL_POKEPARAM* defender, BtlTypeAff aff, u32 damage, BOOL critical_flag, BOOL plural_hit_flag );
static void scPut_WazaDamagePlural( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
  u32 poke_cnt, BtlTypeAff aff, BTL_POKEPARAM** bpp, const u16* damage, const u8* critical_flag );
static void scPut_Koraeru( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, BppKoraeruCause cause );
static void scPut_TokWin_In( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static void scPut_TokWin_Out( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static void scPut_RankEffectLimit( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, WazaRankEffect effect, int volume );
static void scPut_RankEffect( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, WazaRankEffect effect, int volume, u16 itemID, BOOL fStdMsg );
static void scPut_Ichigeki( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target );
static void scPut_SimpleHp( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, int value, BOOL fEffectEnable );
static void scPut_KillPokemon( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u8 effType );
static void scPut_AddSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, WazaSick sick, BPP_SICK_CONT sickCont );
static void scPut_AddSickFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, BtlAddSickFailCode failCode, WazaSick sick );
static void scPut_Message_Set( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u16 strID );
static void scPut_Message_StdEx( BTL_SVFLOW_WORK* wk, u16 strID, u32 argCnt, const int* args );
static void scPut_Message_SetEx( BTL_SVFLOW_WORK* wk, u16 strID, u32 argCnt, const int* args );
static void scPut_DecreaseHP( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u32 value );
static void scPut_DecrementPP( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, u8 wazaIdx, u8 vol );
static void scPut_RecoverPP( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u8 wazaIdx, u8 volume, u16 itemID );
static void scPut_UseItemAct( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static void scPut_ConsumeItem( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static void scPut_SetContFlag( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppContFlag flag );
static void scPut_ResetContFlag( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppContFlag flag );
static void scPut_SetTurnFlag( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppTurnFlag flag );
static void scPut_ResetTurnFlag( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppTurnFlag flag );
static BOOL scEvent_MemberChangeIntr( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* outPoke );
static BOOL scEvent_GetReqWazaForCalcActOrder( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, WazaID orgWazaID, REQWAZA_WORK* reqWaza );
static BOOL scEvent_GetReqWazaParam( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker,
  WazaID orgWazaid, BtlPokePos orgTargetPos, REQWAZA_WORK* reqWaza );
static void scEvent_CheckSpecialActPriority( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, u8* spePriA, u8* spePriB );
static void scEvent_BeforeFight( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, WazaID waza );
static u16 scEvent_CalcConfDamage( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker );
static BOOL scEvent_CheckWazaMsgCustom( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker,
  WazaID orgWazaID, WazaID actWazaID, BTL_HANDEX_STR_PARAMS* str );
static SV_WazaFailCause scEvent_CheckWazaExecute1ST( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza, SV_WazaFailCause cause );
static SV_WazaFailCause scEvent_CheckWazaExecute2ND( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza );
static void scEvent_GetWazaParam( BTL_SVFLOW_WORK* wk, WazaID waza, const BTL_POKEPARAM* attacker, SVFL_WAZAPARAM* param );
static void scEvent_CheckWazaExeFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp,
  WazaID waza, SV_WazaFailCause cause );
static BOOL scEvent_WazaExecuteStart( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza, BTL_POKESET* rec );
static u8 scEvent_GetWazaTargetIntr( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const SVFL_WAZAPARAM* wazaParam );
static BOOL scEvent_CheckMamoruBreak( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
static void scEvent_WazaAvoid( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza, u8 targetCount, const u8* targetPokeID );
static BOOL scEvent_CheckTameTurnSkip( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza );
static void scEvent_TameStart( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKESET* targetRec, WazaID waza );
static void scEvent_TameSkip( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza );
static void scEvent_TameRelease( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKESET* rec, WazaID waza );
static BOOL scEvent_CheckPokeHideAvoid( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
static BOOL scEvent_IsExcuseCalcHit( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
static u8 scEvent_getHitPer( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam );
static BOOL scEvent_IchigekiCheck( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
static BOOL scEvent_CheckNotEffect( BTL_SVFLOW_WORK* wk, WazaID waza, u8 lv,
    const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, BTL_HANDEX_STR_PARAMS* customMsg );
static BOOL scEvent_CheckDmgToRecover( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender,
  const SVFL_WAZAPARAM* wazaParam );
static void scEvent_DmgToRecover( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender );
static BOOL scEvent_CheckCritical( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
static u32 scEvent_CalcKickBack( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza, u32 damage );
static void scEvent_WazaDamageReaction( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam, BtlTypeAff aff,
  u32 damage, BOOL criticalFlag );
static void scEvent_WazaDamageAfter( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const SVFL_WAZAPARAM* wazaParam,
  u32 damage, BOOL criticalFlag );
static void scEvent_ItemEquip( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static void scEvent_ItemEquipTmp( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static BtlTypeAff scProc_checkWazaDamageAffinity( BTL_SVFLOW_WORK* wk,
  BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam );
static BtlTypeAff scEvent_CheckDamageAffinity( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, PokeType waza_type, PokeType poke_type );
static BtlTypeAff CalcTypeAffForDamage( PokeType wazaType, PokeTypePair pokeTypePair );
static u32 scEvent_DecrementPPVolume( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, BTL_POKESET* rec );
static BOOL scEvent_DecrementPP_Reaction( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, u8 wazaIdx );
static BOOL scEvent_CheckPluralHit( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza, u8* hitCount );
static u16 scEvent_getWazaPower( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam );
static BOOL scEvent_CheckEnableSimpleDamage( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u32 damage );
static u16 scEvent_getAttackPower( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam, BOOL criticalFlag );
static u16 scEvent_getDefenderGuard( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender,
  const SVFL_WAZAPARAM* wazaParam, BOOL criticalFlag );
static fx32 scEvent_CalcTypeMatchRatio( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, PokeType waza_type );
static void scEvent_AfterMemberIn( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static u32 scEvent_GetWazaShrinkPer( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker );
static BOOL scEvent_CheckShrink( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* target, WazaID waza, u32 per );
static void scEvent_FailShrink( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target );
static BOOL scEvent_CheckRankEffectSuccess( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target,
  WazaRankEffect effect, u8 wazaUsePokeID, int volume );
static void scEvent_RankEffect_Failed( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static void scEvent_RankEffect_Fix( BTL_SVFLOW_WORK* wk, u8 atkPokeID,
  const BTL_POKEPARAM* bpp, WazaRankEffect rankType, int volume );
static void scEvent_WazaRankEffectFixed( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target,
  WazaID wazaID, WazaRankEffect effectID, int volume );
static int scEvent_RecalcDrainVolume( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, u32 volume );
static void scEvent_CheckSpecialDrain( BTL_SVFLOW_WORK* wk, WazaID waza,
  const BTL_POKEPARAM* attacker, u32 total_damage );
static void scEvent_AfterChangeWeather( BTL_SVFLOW_WORK* wk, BtlWeather weather );
static u32 scEvent_CalcRecoverHP( BTL_SVFLOW_WORK* wk, WazaID waza, const BTL_POKEPARAM* bpp );
static BOOL scEvent_CheckItemSet( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u16 itemID );
static void scEvent_ItemSetDecide( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u16 nextItemID );
static void scEvent_ItemSetFixed( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static void scEvent_ChangeTokuseiBefore( BTL_SVFLOW_WORK* wk, u8 pokeID, u16 tokuseiID );
static void scEvent_ChangeTokuseiAfter( BTL_SVFLOW_WORK* wk, u8 pokeID );
static void scEvent_CheckSideEffectParam( BTL_SVFLOW_WORK* wk, BtlSideEffect effect, BtlSide side, BPP_SICK_CONT* cont );
static void Hem_Init( HANDLER_EXHIBISION_MANAGER* wk );
static u32 Hem_PushState( HANDLER_EXHIBISION_MANAGER* wk );
static void Hem_PopState( HANDLER_EXHIBISION_MANAGER* wk, u32 state );
static u16 Hem_GetStackPtr( const HANDLER_EXHIBISION_MANAGER* wk );
static BTL_HANDEX_PARAM_HEADER* Hem_ReadWork( HANDLER_EXHIBISION_MANAGER* wk );
static BTL_HANDEX_PARAM_HEADER* Hem_PushWork( HANDLER_EXHIBISION_MANAGER* wk, BtlEventHandlerExhibition eq_type, u8 userPokeID );
static void relivePokeRec_Init( BTL_SVFLOW_WORK* wk );
static void relivePokeRec_Add( BTL_SVFLOW_WORK* wk, u8 pokeID );
static BOOL relivePokeRec_CheckNecessaryPokeIn( BTL_SVFLOW_WORK* wk );
static BOOL scproc_HandEx_Root( BTL_SVFLOW_WORK* wk, u16 useItemID );
static u8 scproc_HandEx_TokWinIn( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_TokWinOut( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_ItemEffect( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_recoverHP( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID );
static u8 scproc_HandEx_drain( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID );
static u8 scproc_HandEx_damage( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_shiftHP( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_recoverPP( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID );
static u8 scproc_HandEx_decrementPP( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID );
static u8 scproc_HandEx_cureSick( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID );
static u8 scproc_HandEx_addSick( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_rankEffect( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID );
static u8 scproc_HandEx_setRank( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_recoverRank( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_resetRank( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_setStatus( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_kill( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_changeType( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_message( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_setTurnFlag( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_resetTurnFlag( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_setContFlag( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_resetContFlag( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_sideEffectAdd( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_sideEffectRemove( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_addFieldEffect( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_removeFieldEffect( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_changeWeather( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static BOOL handexSub_putString( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_STR_PARAMS* strParam );
static u8 scproc_HandEx_PosEffAdd( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_tokuseiChange( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_setItem( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_swapItem( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_EquipItem( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_useItem( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_ItemSP( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_consumeItem( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_clearConsumedItem( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static void handexSub_itemSet( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID );
static u8 scproc_HandEx_updateWaza( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_counter( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_delayWazaDamage( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_quitBattle( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_changeMember( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_batonTouch( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_addShrink( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_relive( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_setWeight( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_pushOut( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_intrPoke( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_intrWaza( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_sendLast( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_swapPoke( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_hensin( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_fakeBreak( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_juryokuCheck( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_effectByPos( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );

BTL_SVFLOW_WORK* BTL_SVFLOW_InitSystem(
  BTL_SERVER* server, BTL_MAIN_MODULE* mainModule, BTL_POKE_CONTAINER* pokeCon,
  BTL_SERVER_CMD_QUE* que, BtlBagMode bagMode, HEAPID heapID )
{
  BTL_SVFLOW_WORK* wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(BTL_SVFLOW_WORK) );

  wk->server = server;
  wk->pokeCon = pokeCon;
  wk->mainModule = mainModule;
  wk->numActOrder = 0;
  wk->turnCount = 0;
  wk->flowResult = SVFLOW_RESULT_DEFAULT;
  wk->que = que;
  wk->heapID = heapID;
  wk->prevExeWaza = WAZANO_NULL;
  wk->bagMode = bagMode;
  wk->escapeClientID = BTL_CLIENTID_NULL;
  wk->getPokePos = BTL_POS_NULL;
  wk->sinkaArcHandle = SHINKA_GetArcHandle( heapID );
  BTL_WAZAREC_Init( &wk->wazaRec );
  BTL_DEADREC_Init( &wk->deadRec );
  {
    BtlRule rule = BTL_MAIN_GetRule( mainModule );
    BTL_POSPOKE_InitWork( &wk->pospokeWork, wk->mainModule, wk->pokeCon, rule );
  }

  GFL_STD_MemClear( wk->pokeDeadFlag, sizeof(wk->pokeDeadFlag) );
  GFL_STD_MemClear( wk->pokeInFlag, sizeof(wk->pokeInFlag) );

  Hem_Init( &wk->HEManager );

  eventWork_Init( &wk->eventWork );

  BTL_HANDLER_SIDE_InitSystem();

  return wk;
}


void BTL_SVFLOW_QuitSystem( BTL_SVFLOW_WORK* wk )
{
  GFL_ARC_CloseDataHandle( wk->sinkaArcHandle );
  GFL_HEAP_FreeMemory( wk );
}




//--------------------------------------------------------------------------
/**
 * �T�[�o�R�}���h�����i�o�g���J�n����j
 *
 * @param   wk
 *
 * @retval  SvflowResult
 */
//--------------------------------------------------------------------------
SvflowResult BTL_SVFLOW_Start_AfterPokemonIn( BTL_SVFLOW_WORK* wk )
{
  SVCL_WORK* cw;
  u32 i, posIdx;

  SCQUE_Init( wk->que );

  for(i=0; i<BTL_CLIENT_MAX; ++i)
  {
    cw = BTL_SERVER_GetClientWorkIfEnable( wk->server, i );
    if( cw == NULL ){ continue; }

    for(posIdx=0; posIdx<cw->numCoverPos; ++posIdx)
    {
      BTL_POKEPARAM* bpp = BTL_PARTY_GetMemberData(cw->party, posIdx);
      if( (bpp!=NULL) && !BPP_IsDead(bpp) )
      {
        scproc_MemberInCore( wk, i, posIdx, posIdx );
      }
    }
  }

  scproc_AfterMemberIn( wk );

  // @@@ ���ʂ��Ƃ����邩?  ��Ԉُ�E�V��Ȃǂ̌��ʂ̓^�[���G���h���Ȃ̂ŁA�Ȃ�����
  return SVFLOW_RESULT_DEFAULT;
}
//--------------------------------------------------------------------------
/**
 * �T�[�o�R�}���h�����i�ʏ�̂P�^�[�����R�}���h�����j
 *
 * @param   wk
 *
 * @retval  SvflowResult
 */
//--------------------------------------------------------------------------
SvflowResult BTL_SVFLOW_StartTurn( BTL_SVFLOW_WORK* wk )
{
  relivePokeRec_Init( wk );
  BTL_DEADREC_StartTurn( &wk->deadRec );
  FlowFlg_ClearAll( wk );
  BTL_EVENT_StartTurn();

  BTL_SERVER_InitChangePokemonReq( wk->server );
  BTL_EVENTVAR_CheckStackCleared();
  SCQUE_Init( wk->que );

  clearPokeCantActionFlag( wk );

  wk->flowResult =  SVFLOW_RESULT_DEFAULT;
  wk->numActOrder = sortClientAction( wk, wk->actOrder, NELEMS(wk->actOrder) );
  wk->numEndActOrder = ActOrderProc_Main( wk, 0 );
  BTL_N_Printf( DBGSTR_SVFL_TurnStart_Result, wk->numEndActOrder, wk->numActOrder );

  return wk->flowResult;
}
/**
 *  �S�|�P�����̔����œ����Ȃ���ԃt���O���N���A
 */
static void clearPokeCantActionFlag( BTL_SVFLOW_WORK* wk )
{
  FRONT_POKE_SEEK_WORK  fps;
  BTL_POKEPARAM* bpp;

  FRONT_POKE_SEEK_InitWork( &fps, wk );
  while( FRONT_POKE_SEEK_GetNext(&fps, wk, &bpp) )
  {
    if( BPP_CONTFLAG_Get(bpp, BPP_CONTFLG_CANT_ACTION) ){
      scPut_ResetContFlag( wk, bpp, BPP_CONTFLG_CANT_ACTION );
    }
  }
}
//=============================================================================================
/**
 * �T�[�o�R�}���h�����i�^�[���r���̃|�P��������ւ��I����A�p���j
 *
 * @param   wk
 *
 * @retval  SvflowResult
 */
//=============================================================================================
SvflowResult BTL_SVFLOW_ContinueAfterPokeChange( BTL_SVFLOW_WORK* wk )
{
  BTL_N_Printf( DBGSTR_SVFL_StartAfterPokeChange );

  BTL_SERVER_InitChangePokemonReq( wk->server );
  BTL_EVENTVAR_CheckStackCleared();
  SCQUE_Init( wk->que );

  wk->flowResult = SVFLOW_RESULT_DEFAULT;

  if( ActOrderProc_PokeIn(wk) ){
    wk->flowResult = SVFLOW_RESULT_BTL_SHOWDOWN;
    return wk->flowResult;
  }

  wk->numEndActOrder = ActOrderProc_Main( wk, wk->numEndActOrder );
  return wk->flowResult;
}
//=============================================================================================
/**
 * �T�[�o�R�}���h�����i�|�P�������S�E�����Ԃ�ɂ��^�[���ŏ��̓��ꏈ���j
 *
 * @param   wk
 *
 * @retval  SvflowResult
 */
//=============================================================================================
SvflowResult BTL_SVFLOW_StartAfterPokeIn( BTL_SVFLOW_WORK* wk )
{
  const BTL_ACTION_PARAM* action;
  u32 i;
  u8 numDeadPoke;

  BTL_SERVER_InitChangePokemonReq( wk->server );
  BTL_EVENTVAR_CheckStackCleared();
  SCQUE_Init( wk->que );

  wk->flowResult =  SVFLOW_RESULT_DEFAULT;

  BTL_Printf("�󂫈ʒu�Ƀ|�P����������̃T�[�o�[�R�}���h����\n");

  numDeadPoke = BTL_DEADREC_GetCount( &wk->deadRec, 0 );
  wk->flowResult =  SVFLOW_RESULT_DEFAULT;

  wk->numActOrder = sortClientAction( wk, wk->actOrder, NELEMS(wk->actOrder) );
  wk->numEndActOrder = 0;
  for(i=0; i<wk->numActOrder; i++)
  {
    action = &wk->actOrder[i].action;
    if( action->gen.cmd != BTL_ACTION_CHANGE ){ continue; }
    if( action->change.depleteFlag ){ continue; }

    BTL_Printf("�N���C�A���g_%d �̈ʒu_%d �ցA%d�Ԗڂ̃|�P���o��\n",
          wk->actOrder[i].clientID, action->change.posIdx, action->change.memberIdx );
    scproc_MemberInForChange( wk, wk->actOrder[i].clientID, action->change.posIdx, action->change.memberIdx, TRUE );
  }
  scproc_AfterMemberIn( wk );
  reqChangePokeForServer( wk );

  {
    u8 numDeadPokeAfter = BTL_DEADREC_GetCount( &wk->deadRec, 0 );
    if( numDeadPoke == numDeadPokeAfter ){
      return SVFLOW_RESULT_DEFAULT;
    }else{
      if( scproc_CheckShowdown(wk) == FALSE){
        return SVFLOW_RESULT_POKE_COVER;
      }else{
        return SVFLOW_RESULT_BTL_SHOWDOWN;
      }
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * �R�}���h�����R�A�i�I���A�N�V�����z�񂩂�j
 *
 * @param   wk
 * @param   startOrderIdx   �J�n�A�N�V����Index
 *
 * @retval  u32   �������I�����A�N�V������
 */
//----------------------------------------------------------------------------------
static u32 ActOrderProc_Main( BTL_SVFLOW_WORK* wk, u32 startOrderIdx )
{
  u32 i;

  BTL_N_Printf( DBGSTR_SVFL_ActOrderMainStart, startOrderIdx );

  for(i=startOrderIdx; i<wk->numActOrder; i++)
  {
    ActOrder_Proc( wk, &wk->actOrder[i] );
    scproc_CheckExpGet( wk );

    // �唚���ȂǓ����S�ł̃P�[�X�́A���S���R�[�h������Ή�������񂶂��Ǝv���Ă�B
    if( scproc_CheckShowdown(wk) ){
      wk->flowResult = SVFLOW_RESULT_BTL_SHOWDOWN;
      return i+1;
    }

    if( wk->flowResult !=  SVFLOW_RESULT_DEFAULT ){
      BTL_N_Printf( DBGSTR_SVFL_ActOrderMainDropOut, wk->flowResult );
      break;
    }

    BTL_N_Printf( DBGSTR_SVFL_QueWritePtr, wk->que->writePtr );
  }

  // �S�A�N�V�����������I����
  if( i == wk->numActOrder )
  {
    u8 numDeadPoke;

    // �^�[���`�F�b�N����
    scproc_TurnCheck( wk );
    if( scproc_CheckShowdown(wk) ){
      wk->flowResult = SVFLOW_RESULT_BTL_SHOWDOWN;
      return i;
    }

    numDeadPoke = BTL_DEADREC_GetCount( &wk->deadRec, 0 );
    // ���S�E�����Ԃ�ȂǂŃ|�P�����̕K�v�����邩�`�F�b�N
    if( relivePokeRec_CheckNecessaryPokeIn(wk)
    ||  (numDeadPoke != 0)
    ){
      reqChangePokeForServer( wk );
      wk->flowResult = SVFLOW_RESULT_POKE_COVER;
      return i;
    }

    wk->flowResult = SVFLOW_RESULT_DEFAULT;
    return i;
  }
  // �܂��A�N�V�������c���Ă��邪����ւ��E�����Ȃǔ����ŃT�[�o�[�֕Ԃ�
  else
  {
    // @@@ �g���{������Łc����ꂽ��������ŏI���̏ꍇ�͈������ޏ����v��Ȃ��˂��B
    reqChangePokeForServer( wk );
    return i;
  }
}
//----------------------------------------------------------------------------------
/**
 * �R�}���h�����i�|�P��������A�N�V�����̂ݏ����j
 *
 * @param   wk
 *
 * @retval  BOOL  �����̏����Ń|�P���������S�A�������t�����ꍇ��TRUE
 */
//----------------------------------------------------------------------------------
static BOOL ActOrderProc_PokeIn( BTL_SVFLOW_WORK* wk )
{
  SVCL_WORK* clwk;
  const BTL_ACTION_PARAM* action;
  u32 i, j, actionCnt;

  for(i=0; i<BTL_CLIENT_MAX; ++i)
  {
    clwk = BTL_SERVER_GetClientWork( wk->server, i );
    if( clwk == NULL ){
      continue;
    }
    actionCnt = BTL_SVCL_GetNumActPoke( clwk );

    for(j=0; j<actionCnt; ++j)
    {
      action = BTL_SVCL_GetPokeAction( clwk, j );
      if( action->gen.cmd != BTL_ACTION_CHANGE ){ continue; }
      if( action->change.depleteFlag ){ continue; }

      BTL_N_Printf( DBGSTR_SVFL_PokeChangeRootInfo,
            i, action->change.posIdx, action->change.memberIdx );

      scproc_MemberInForChange( wk, i, action->change.posIdx, action->change.memberIdx, TRUE );
    }
  }
  scproc_AfterMemberIn( wk );
  return scproc_CheckShowdown( wk );
}
//----------------------------------------------------------------------------------
/**
 * ���݂̃p�[�e�B�f�[�^��r���A����������������
 *
 * @param   wk
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL scproc_CheckShowdown( BTL_SVFLOW_WORK* wk )
{
  u8  pokeExist[2];
  u32 i;

  pokeExist[0] = pokeExist[1] = FALSE;

  for(i=0; i<BTL_CLIENT_MAX; ++i)
  {
    if( BTL_MAIN_IsExistClient(wk->mainModule, i) )
    {
      BTL_PARTY* party = BTL_POKECON_GetPartyData( wk->pokeCon, i );
      u8 side = BTL_MAIN_GetClientSide( wk->mainModule, i );
      if( BTL_PARTY_GetAliveMemberCount(party) )
      {
        BTL_N_Printf( DBGSTR_SVFL_ClientPokeStillAlive, i, side);
        pokeExist[ side ] = TRUE;
      }
      else{
        BTL_N_Printf( DBGSTR_SVFL_ClientPokeDeadAll, i, side);
      }
    }
  }

  if( (pokeExist[0] == FALSE) || (pokeExist[1] == FALSE) ){
    return TRUE;
  }
  return FALSE;
}

//=============================================================================================
/**
 * �T�[�o�R�}���h�����i�����鏈���j
 *
 * @param   wk
 *
 * @retval  BOOL    �����邱�Ƃ��o������TRUE
 */
//=============================================================================================
BOOL BTL_SVFLOW_CreatePlayerEscapeCommand( BTL_SVFLOW_WORK* wk )
{
  u8 clientID = BTL_MAIN_GetPlayerClientID( wk->mainModule );
  BTL_POKEPARAM* bpp = BTL_POKECON_GetClientPokeData( wk->pokeCon, clientID, 0 );

  SCQUE_Init( wk->que );

  if( scproc_NigeruCmd( wk, bpp ) )
  {
    wk->escapeClientID = clientID;
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

//=============================================================================================
/**
 * �V���[�^�[�`���[�W�R�}���h����
 *
 * @param   wk
 *
 * @retval  BOOL    �R�}���h����������TRUE
 */
//=============================================================================================
BOOL BTL_SVFLOW_MakeShooterChargeCommand( BTL_SVFLOW_WORK* wk )
{
  SCQUE_Init( wk->que );

  // �V���[�^�[�̃G�l���M�[�`���[�W
  scproc_countup_shooter_energy( wk );

  return wk->que->writePtr != 0;
}
//----------------------------------------------------------------------------------
/**
 * �V���[�^�[�̃G�l���M�[�`���[�W�v�Z���e�N���C�A���g�ɒʒm
 *
 * @param   wk
 */
//----------------------------------------------------------------------------------
static void scproc_countup_shooter_energy( BTL_SVFLOW_WORK* wk )
{
  if( wk->bagMode == BBAG_MODE_SHOOTER )
  {
    u32 i;
    for(i=0; i<BTL_CLIENT_MAX; ++i)
    {
      if( !BTL_SERVER_IsClientEnable(wk->server, i) ){
        continue;
      }
      if( BTL_MAIN_GetRule(wk->mainModule) != BTL_RULE_TRIPLE ){
        SCQUE_PUT_OP_ShooterCharge( wk->que, i, 1 );
      }else{
        u8 myClientID, opponentClientID, emptyCnt, fEnable, p;
        u8 emptyPos[ BTL_POSIDX_MAX ];

        myClientID = BTL_MAIN_GetPlayerClientID( wk->mainModule );
        opponentClientID = BTL_MAIN_GetOpponentClientID( wk->mainModule, myClientID, 0 );

        fEnable = TRUE;
        emptyCnt = BTL_POSPOKE_GetClientEmptyPos( &wk->pospokeWork, myClientID, emptyPos );
        for(p=0; p<emptyCnt; ++p)
        {
          if( BTL_MAINUTIL_IsTripleCenterPos(emptyPos[p]) ){
            fEnable = FALSE;  // �����̒������󂢂Ă�����`���[�W����Ȃ�
            break;
          }
        }
        if( fEnable )
        {
          u8 value = 1 + emptyCnt;  // ��{�͎����̋󂫈ʒu + 1
          emptyCnt = BTL_POSPOKE_GetClientEmptyPos( &wk->pospokeWork, opponentClientID, emptyPos );
          for(p=0; p<emptyCnt; ++p){
            if( BTL_MAINUTIL_IsTripleCenterPos(emptyPos[p]) ){
              value *= 2;   // ����̒������󂢂Ă�����Q�{
              break;
            }
          }
          SCQUE_PUT_OP_ShooterCharge( wk->que, i, value );
        }
      }
    }
  }
}

//=============================================================================================
/**
 * ���[�e�[�V�����R�}���h�����i���[�e�[�V�����o�g���̂݁j
 *
 * @param   wk
 * @param   clientID
 * @param   dir
 */
//=============================================================================================
void BTL_SVFLOW_CreateRotationCommand( BTL_SVFLOW_WORK* wk, u8 clientID, BtlRotateDir dir )
{
  BTL_Printf("�N���C�A���g[%d]�͉�]���� %d �փ��[�e\n", clientID, dir);
  SCQUE_PUT_ACT_Rotation( wk->que, clientID, dir );

  if( dir != BTL_ROTATEDIR_STAY )
  {
    BTL_PARTY* party = BTL_POKECON_GetPartyData( wk->pokeCon, clientID );
    BTL_POKEPARAM *outPoke, *inPoke;
    BTL_PARTY_RotateMembers( party, dir, &outPoke, &inPoke );
    if( !BPP_IsDead(outPoke) ){
      BTL_HANDLER_TOKUSEI_Remove( outPoke );
    }
    if( !BPP_IsDead(inPoke) ){
      BTL_HANDLER_TOKUSEI_Add( inPoke );
    }
    BTL_POSPOKE_Rotate( &wk->pospokeWork, dir, clientID, inPoke, wk->pokeCon );
  }
}
//----------------------------------------------------------------------------------
/**
 * �T�[�o�ɑ΂��A�e�N���C�A���g�̋󂫈ʒu��ʒm
 *
 * @param   wk
 *
 * @retval  BOOL  �N��������Ō�������K�v����ꍇ��TRUE
 */
//----------------------------------------------------------------------------------
static BOOL reqChangePokeForServer( BTL_SVFLOW_WORK* wk )
{
  u8 posAry[ BTL_POSIDX_MAX ];
  u8 empty_pos_cnt, clientID, i;
  u8 result = FALSE;

  SVCL_WORK* clwk;
  for(clientID=0; clientID<BTL_CLIENT_MAX; ++clientID)
  {
    empty_pos_cnt = BTL_POSPOKE_GetClientEmptyPos( &wk->pospokeWork, clientID, posAry );
    if( empty_pos_cnt )
    {
      // �󂢂Ă���ʒu��S�ăT�[�o�֒ʒm
      BTL_N_Printf( DBGSTR_SVFL_ChangePokeReqInfo, clientID, empty_pos_cnt );
      for(i=0; i<empty_pos_cnt; ++i)
      {
        BTL_SERVER_RequestChangePokemon( wk->server, posAry[i] );
        BTL_N_PrintfSimple( DBGSTR_val_comma, posAry[i] );
      }
      BTL_N_PrintfSimple( DBGSTR_LF );
      result = TRUE;
    }
  }
  return result;
}
//=============================================================================================
/**
 * �������N���C�A���gID���擾
 *
 * @param   wk
 *
 * @retval  u8
 */
//=============================================================================================
u8 BTL_SVFLOW_GetEscapeClientID( const BTL_SVFLOW_WORK* wk )
{
  return wk->escapeClientID;
}
//=============================================================================================
/**
 * �ߊl�����|�P�����̈ʒu���擾
 *
 * @param   wk
 *
 * @retval  BtlPokePos
 */
//=============================================================================================
BtlPokePos BTL_SVFLOW_GetCapturedPokePos( const BTL_SVFLOW_WORK* wk )
{
  return wk->getPokePos;
}
//----------------------------------------------------------------------------------
/**
 * [Proc] �^�[���ŏ��́u���������v�R�}���h�����O�Ɏ��s���鏈��
 *
 * @param   wk
 */
//----------------------------------------------------------------------------------
static void scproc_BeforeFirstFight( BTL_SVFLOW_WORK* wk )
{
  u32 hem_state, i;
  WazaID waza;

  hem_state = Hem_PushState( &wk->HEManager );
  for(i=0; i<wk->numActOrder; ++i)
  {
    waza = ActOrder_GetWazaID( &wk->actOrder[i] );
    if( waza != WAZANO_NULL )
    {
      if( BTL_HANDLER_Waza_Add(wk->actOrder[i].bpp, waza) )
      {
        scEvent_BeforeFight(wk, wk->actOrder[i].bpp, waza);
        BTL_HANDLER_Waza_Remove( wk->actOrder[i].bpp, waza );
      }
    }
  }
  scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
  Hem_PopState( &wk->HEManager, hem_state );
}
//----------------------------------------------------------------------------------
/**
 * ��ɂ���S�Ẵ|�P������s�t���O�̃`�F�b�N
 *
 * @param   wk
 */
//----------------------------------------------------------------------------------
static void scproc_CheckFlyingAllPoke( BTL_SVFLOW_WORK* wk )
{
  FRONT_POKE_SEEK_WORK  fps;
  BTL_POKEPARAM* bpp;

  FRONT_POKE_SEEK_InitWork( &fps, wk );
  while( FRONT_POKE_SEEK_GetNext(&fps, wk, &bpp) )
  {
    scproc_CheckFlying( wk, bpp );
  }
}
//----------------------------------------------------------------------------------
/**
 * �|�P�����P�̂ɔ�s�t���O�̃`�F�b�N
 *
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
static BOOL scproc_CheckFlying( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  BPP_TURNFLAG_ForceOff( bpp, BPP_TURNFLG_FLYING );

  // ���イ��傭�������Ă�����N�������Ȃ��̂Ń`�F�b�N���Ȃ�
  if( !BTL_FIELD_CheckEffect(BTL_FLDEFF_JURYOKU) )
  {
    if( scEvent_CheckFlying(wk, bpp) ){
      scPut_SetTurnFlag( wk, bpp, BPP_TURNFLG_FLYING );
      return TRUE;
    }
  }

  return FALSE;
}

//----------------------------------------------------------------------------------
/**
 * [Event] ��s�t���O�L���`�F�b�N
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  BOOL    �L���ł����TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_CheckFlying( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  // ���イ��傭�������Ă�����N�������Ȃ�
  if( BTL_FIELD_CheckEffect(BTL_FLDEFF_JURYOKU) ){
    return FALSE;
  }
  else
  {
    u8 flyFlag = BPP_IsMatchType( bpp, POKETYPE_HIKOU );
    u8 failFlag = FALSE;

    BTL_EVENTVAR_Push();
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
      BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, flyFlag );
      BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, failFlag );
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHECK_FLYING );
      BTL_SICKEVENT_CheckFlying( wk, bpp );
      flyFlag = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
      failFlag = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
    BTL_EVENTVAR_Pop();

    if( failFlag ){
      return FALSE;
    }
    return flyFlag;
  }
}

//----------------------------------------------------------------------------------
/**
 *
 * �|�P�������Ƃ̃A�N�V�������`�F�b�N���A�����������m��
 *
 * @param   server
 * @param   order       �������鏇�ԂɃN���C�A���gID����ג����Ċi�[���邽�߂̔z��
 * @param   orderMax    �z�� order �̗v�f��
 *
 * @retval  u8    ��������|�P������
 */
//----------------------------------------------------------------------------------
static u8 sortClientAction( BTL_SVFLOW_WORK* wk, ACTION_ORDER_WORK* order, u32 orderMax )
{
  /*
    �s���D�揇  ...  3 BIT
    ����D�揇A ...  3 BIT
    ���U�D�揇  ...  6 BIT
    ����D�揇B ...  3 BIT
    �f����      ... 16 BIT
  */
  #define MakePriValue( actPri, spePriA, wazaPri, spePriB, agility )  \
            ( ((actPri&0x07)<<28) | ((spePriA&0x07)<<25) | ((wazaPri&0x3f)<<19) | ((spePriB&0x07)<<16) | (agility&0xffff) )

  SVCL_WORK* clwk;
  const BTL_ACTION_PARAM* actParam;
  const BTL_POKEPARAM* bpp;
  u32   hem_state;
  u16 agility;
  u8  action, actionPri, wazaPri, spPri_A, spPri_B;
  u8  i, j, p, numPoke;

// �S�|�P�����̍s�����e�����[�N�Ɋi�[
  for(i=0, p=0; i<BTL_CLIENT_MAX; ++i)
  {
    clwk = BTL_SERVER_GetClientWork( wk->server, i );
    if( clwk == NULL ){ continue; }

    numPoke = BTL_SVCL_GetNumActPoke( clwk );
    for(j=0; j<numPoke; j++)
    {
      order[p].bpp = BTL_PARTY_GetMemberData( clwk->party, j );
      order[p].action = *BTL_SVCL_GetPokeAction( clwk, j );
      order[p].clientID = i;
      order[p].pokeIdx = j;
      order[p].fDone = FALSE;
      order[p].fIntr = FALSE;
      reqWazaUseForCalcActOrder( wk, &order[p] );
      ++p;
    }
  }
// �e�|�P�����̍s���v���C�I���e�B�l�𐶐�
  numPoke = p;
  for(i=0; i<numPoke; ++i)
  {
    hem_state = Hem_PushState( &wk->HEManager );
    bpp = order[i].bpp;
    actParam = &(order[i].action);

    // �s���ɂ��D�揇�i�D��x�����قǐ��l��j
    switch( actParam->gen.cmd ){
    case BTL_ACTION_ESCAPE: actionPri = 4; break;
    case BTL_ACTION_ITEM:   actionPri = 3; break;
    case BTL_ACTION_CHANGE: actionPri = 2; break;
    case BTL_ACTION_SKIP:   actionPri = 1; break;
    case BTL_ACTION_MOVE:   actionPri = 0; break;
    case BTL_ACTION_FIGHT:  actionPri = 0; break;
    case BTL_ACTION_NULL: continue;
    default:
      GF_ASSERT(0);
      actionPri = 0;
      break;
    }
    // �u���������v�ꍇ�̓��U�ɂ��D�揇�A�A�C�e�������ɂ��D��t���O��S�Č���
    if( actParam->gen.cmd == BTL_ACTION_FIGHT ){
      WazaID  waza;
      waza = ActOrder_GetWazaID( &order[i] );
      BTL_Printf("�|�P[%d]�̃��U�D��`�F�b�N .. waza=%d\n", BPP_GetID(bpp), waza);
      wazaPri = scEvent_GetWazaPriority( wk, waza, bpp );
      // �A�C�e�������ȂǁA����ȗD��t���O
      scEvent_CheckSpecialActPriority( wk, bpp, &spPri_A, &spPri_B );

    // �u���[�u�v�̏ꍇ�A�A�C�e���������ɂ��D��t���O������
    }else if( actParam->gen.cmd == BTL_ACTION_MOVE ){
      wazaPri = 0 - WAZAPRI_MIN;
      scEvent_CheckSpecialActPriority( wk, bpp, &spPri_A, &spPri_B );

    // ����ȊO�͍s���D�揇�݂̂Ŕ���
    }else{
      wazaPri = 0;
      spPri_A = BTL_SPPRI_A_DEFAULT;
      spPri_B = BTL_SPPRI_B_DEFAULT;
    }
    // ���΂₳
    agility = scEvent_CalcAgility( wk, bpp );

    BTL_Printf("�s���v���C�I���e�B����I Client{%d-%d}'s actionPri=%d, wazaPri=%d, agility=%d\n",
        i, j, actionPri, wazaPri, agility );

    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    Hem_PopState( &wk->HEManager, hem_state );

    // �v���C�I���e�B�l�ƃN���C�A���gID��΂ɂ��Ĕz��ɕۑ�
    order[i].priority = MakePriValue( actionPri, spPri_A, wazaPri, spPri_B, agility );
  }
// �v���C�I���e�B�l�ɂ��\�[�g
  for(i=0; i<numPoke; i++)
  {
    for(j=i+1; j<numPoke; j++)
    {
      if( order[i].priority > order[j].priority ){
        continue;
      }
      if( order[i].priority == order[j].priority ){ // �S�������v���C�I���e�B�l���������烉���_��
        if( BTL_CALC_GetRand(2) == 0 ){
          continue;
        }
      }

      {
        ACTION_ORDER_WORK tmp = order[i];
        order[i] = order[j];
        order[j] = tmp;
      }
    }
  }

  return p;
}
//----------------------------------------------------------------------------------
/**
 * �����U�Ăяo�����s�����U�ŁA�A�N�V�������������v�Z�ɂ��Ăяo����̃��U��K�p����ꍇ�A
 * �Ăяo�����UID�������[�N�Ɋi�[����
 *
 * @param   flowWk
 * @param   order
 */
//----------------------------------------------------------------------------------
static void reqWazaUseForCalcActOrder( BTL_SVFLOW_WORK* wk, ACTION_ORDER_WORK* order )
{
  order->reqWaza.wazaID = WAZANO_NULL;
  order->reqWaza.targetPos = BTL_POS_NULL;

  if( order->action.gen.cmd == BTL_ACTION_FIGHT )
  {
    WazaID orgWazaID = order->action.fight.waza;
    if( BTL_HANDLER_Waza_Add(order->bpp, orgWazaID) )
    {
      scEvent_GetReqWazaForCalcActOrder( wk, order->bpp, orgWazaID, &order->reqWaza );
      BTL_HANDLER_Waza_Remove( order->bpp, orgWazaID );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���̃��U���Ăяo�����U���A�A�N�V�����������v�Z���Ăяo����̃��U�ōs���ꍇ�A���̃p�����[�^�擾
 *
 * @param   wk
 * @param   bpp
 * @param   orgWazaID   �Ăяo�����̃��U�i��т��ӂ铙�j
 * @param   reqWaza     [out] �Ăяo�����U�̃p�����[�^���i�[����
 *
 * @retval  BOOL    �Ăяo�����U�̃p�����[�^���ݒ肳��Ă����TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_GetReqWazaForCalcActOrder( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, WazaID orgWazaID, REQWAZA_WORK* reqWaza )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID,  BPP_GetID(bpp) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID,  WAZANO_NULL );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEPOS, BTL_POS_NULL );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_REQWAZA_FOR_ACT_ORDER );
    reqWaza->wazaID = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    reqWaza->targetPos = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEPOS );
    if( reqWaza->wazaID != WAZANO_NULL )
    {
      BTL_N_Printf( DBGSTR_SVFL_ReqWazaCallActOrder, orgWazaID, reqWaza->wazaID, reqWaza->targetPos);
    }
  BTL_EVENTVAR_Pop();
  return (reqWaza->wazaID != WAZANO_NULL);
}

//----------------------------------------------------------------------------------
/**
 * [Event] ���U�v���C�I���e�B�擾
 * �����U�f�[�^�ɐݒ肳��Ă��� min �` max �̒l�ł͂Ȃ��A0�I���W���̒l�ɒ��������́B
 *
 * @param   wk
 * @param   waza
 * @param   bpp
 *
 * @retval  u8
 */
//----------------------------------------------------------------------------------
static u8 scEvent_GetWazaPriority( BTL_SVFLOW_WORK* wk, WazaID waza, const BTL_POKEPARAM* bpp )
{
  u8 pri = WAZADATA_GetParam( waza, WAZAPARAM_PRIORITY ) - WAZAPRI_MIN;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZA_PRI, pri );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_GET_WAZA_PRI );
    pri = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_PRI );
  BTL_EVENTVAR_Pop();

  return pri;
}
//--------------------------------------------------------------------------
/**
 * [Event] ���΂₳�擾
 *
 * @param   wk
 * @param   attacker
 *
 * @retval  u16
 */
//--------------------------------------------------------------------------
static u16 scEvent_CalcAgility( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker )
{
  u32 agi = BPP_GetValue( attacker, BPP_AGILITY );
  BTL_Printf("�f�̑f���� poke=%d, agi=%d\n", BPP_GetID(attacker), agi);
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_AGILITY, agi );
    BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, TRUE ); // �܂Ѓ`�F�b�N�t���O�Ƃ��Ďg���Ă���
    BTL_EVENTVAR_SetValue( BTL_EVAR_TRICK_FLAG, FALSE );
    BTL_EVENTVAR_SetMulValue( BTL_EVAR_RATIO, FX32_CONST(1), FX32_CONST(0.1f), FX32_CONST(32) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CALC_AGILITY );
    agi = BTL_EVENTVAR_GetValue( BTL_EVAR_AGILITY );
    {
      fx32 ratio = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
      agi = BTL_CALC_MulRatio( agi, ratio );
      BTL_Printf("  �n���h��������̑f����=%d, �{��=%08x, ����=%d\n",
          BTL_EVENTVAR_GetValue(BTL_EVAR_AGILITY), ratio, agi);
    }
    if( BPP_GetPokeSick(attacker) == POKESICK_MAHI )
    {
      if( BTL_EVENTVAR_GetValue(BTL_EVAR_GEN_FLAG) ){
        agi = (agi * BTL_MAHI_AGILITY_RATIO) / 100;
        BTL_Printf("    �}�q��%d\n", agi);
      }
    }
    if( agi > BTL_CALC_AGILITY_MAX ){
      agi = BTL_CALC_AGILITY_MAX;
    }
    // �g���b�N���[���łЂ����肩����
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_TRICK_FLAG) )
    {
      agi = BTL_CALC_AGILITY_MAX - agi;
    }
  BTL_EVENTVAR_Pop();

  return agi;
}

// �A�N�V�������e����A�D�揇�ʌv�Z�Ɏg�p���郏�UID���擾�i���U�g��Ȃ��ꍇ��WAZANO_NULL�j
static inline WazaID ActOrder_GetWazaID( const ACTION_ORDER_WORK* wk )
{
  if( wk->action.gen.cmd == BTL_ACTION_FIGHT )
  {
    return (wk->reqWaza.wazaID == WAZANO_NULL)? wk->action.fight.waza : wk->reqWaza.wazaID;
  }
  return WAZANO_NULL;
}
// �w��|�P�����̃A�N�V�������e�f�[�^�|�C���^���擾
static ACTION_ORDER_WORK* SearchActOrderByPokeID( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  u32 i;
  for(i=0; i<wk->numActOrder; ++i){
    if( BPP_GetID(wk->actOrder[i].bpp) == pokeID ){
      return &(wk->actOrder[i]);
    }
  }
  return NULL;
}
// �����s�̎w�胏�U�g�p�A�N�V�������e�f�[�^�|�C���^���擾
static ACTION_ORDER_WORK* SearchActOrderByWaza( BTL_SVFLOW_WORK* wk, WazaID waza, u8 startIndex )
{
  u32 i;
  for(i=startIndex; i<wk->numActOrder; ++i)
  {
    if( wk->actOrder[i].fDone == FALSE )
    {
      if( (BTL_ACTION_GetAction(&wk->actOrder[i].action) == BTL_ACTION_FIGHT)
      &&  (BTL_ACTION_GetWazaID(&wk->actOrder[i].action) == waza)
      ){
        return &wk->actOrder[i];
      }
    }
  }
  return NULL;
}


//----------------------------------------------------------------------------------
/**
 * �w��A�N�V�������A�w��index�ȍ~�̖����s��̐擪�Ɋ��荞�܂���
 *
 * @param   wk
 * @param   actOrder
 * @param   intrIndex   ���荞�݋�����ŏ���index�i����ȍ~�Ɋ��荞�݋��j
 *
 * @retval  int   ���荞��Index �i���荞�߂Ȃ����-1)
 */
//----------------------------------------------------------------------------------
static int IntrActOrder( BTL_SVFLOW_WORK* wk, const ACTION_ORDER_WORK* actOrder, u32 intrIndex )
{
  int prevOrder, topOrder, i;

  // �������g�̌��X��Index�ƁA�����s��̐擪Index���T�[�`
  prevOrder = topOrder = -1;
  for(i=intrIndex; i<wk->numActOrder; ++i)
  {
    if( &(wk->actOrder[i]) == actOrder ){
      prevOrder = i;
      break;
    }
  }
  for(i=intrIndex; i<wk->numActOrder; ++i)
  {
    if( wk->actOrder[i].fDone == FALSE){
      topOrder = i;
      break;
    }
  }

  // �ǂ�����L���l���������g�������s�̐擪����̏��ԂȂ���בւ�
  if( (prevOrder>=0) && (topOrder>=0)
  &&  (prevOrder > topOrder)
  ){
    wk->actOrderTmp = *actOrder;
    while( prevOrder > topOrder ){
      wk->actOrder[prevOrder] = wk->actOrder[prevOrder-1];
      --prevOrder;
    }
    wk->actOrder[topOrder] = wk->actOrderTmp;
    return topOrder;
  }

  return -1;
}
//----------------------------------------------------------------------------------
/**
 * �w��A�N�V�������A�Ō�ɉ�
 *
 * @param   wk
 * @param   actOrder
 */
//----------------------------------------------------------------------------------
static void SendLastActOrder( BTL_SVFLOW_WORK* wk, const ACTION_ORDER_WORK* actOrder )
{
  // �������g�̌��X��Index
  int prevOrder = -1;
  int i;

  for(i=0; i<wk->numActOrder; ++i)
  {
    if( &(wk->actOrder[i]) == actOrder ){
      prevOrder = i;
      break;
    }
  }

  if( prevOrder >= 0 )
  {
    wk->actOrderTmp = *actOrder;
    for(i=prevOrder; i<(wk->numActOrder-1); ++i)
    {
      wk->actOrder[i] = wk->actOrder[i+1];
    }
    wk->actOrder[i] = wk->actOrderTmp;
  }
}

//----------------------------------------------------------------------------------
/**
 *  �A�N�V�������e�̎��s
 *
 * @param   wk
 * @param   actOrder
 */
//----------------------------------------------------------------------------------
static void ActOrder_Proc( BTL_SVFLOW_WORK* wk, ACTION_ORDER_WORK* actOrder )
{
  if( actOrder->fDone == FALSE )
  {
    BTL_POKEPARAM* bpp = actOrder->bpp;
    actOrder->fDone = TRUE;

    if( !BPP_IsDead(bpp)
    &&  (BTL_POSPOKE_GetPokeExistPos(&wk->pospokeWork, BPP_GetID(bpp)) != BTL_POS_NULL)
    &&  (!BPP_CheckSick(bpp, WAZASICK_FREEFALL))
    ){
      BTL_ACTION_PARAM action = actOrder->action;

      BTL_N_Printf( DBGSTR_SVFL_ActOrderStart, BPP_GetID(bpp), BPP_GetMonsNo(bpp));

      switch( action.gen.cmd ){
      case BTL_ACTION_FIGHT:
        if( !FlowFlg_Get(wk, FLOWFLG_FIRST_FIGHT) ){
          scproc_BeforeFirstFight( wk );
          FlowFlg_Set( wk, FLOWFLG_FIRST_FIGHT );
        }
        BTL_N_Printf( DBGSTR_SVFL_ActOrder_Fight, action.fight.waza, action.fight.targetPos);
        scproc_Fight( wk, bpp, &actOrder->reqWaza, &action );
        break;
      case BTL_ACTION_ITEM:
        BTL_N_Printf( DBGSTR_SVFL_ActOrder_Item, action.item.number, action.item.targetIdx);
        scproc_TrainerItem_Root( wk, bpp, action.item.number, action.item.param, action.item.targetIdx );
        break;
      case BTL_ACTION_CHANGE:
        BTL_Printf( DBGSTR_SVFL_ActOrder_Change, action.change.posIdx, action.change.memberIdx);
        scproc_MemberChange( wk, bpp, action.change.memberIdx );
        break;
      case BTL_ACTION_ESCAPE:
        BTL_N_Printf( DBGSTR_SVFL_ActOrder_Escape );
        if( scproc_NigeruCmd( wk, bpp ) )
        {
          wk->flowResult = SVFLOW_RESULT_BTL_QUIT;
          wk->escapeClientID = actOrder->clientID;
        } else {
          //SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_EscapeFail );
        }
        break;
      case BTL_ACTION_MOVE:
        scproc_Move( wk, bpp );
        break;
      case BTL_ACTION_SKIP:
        scPut_CantAction( wk, bpp );
        break;
      case BTL_ACTION_NULL:
        BTL_N_Printf( DBGSTR_SVFL_ActOrder_Dead );
//        scPut_CantAction( wk, bpp );
        break;
      }

      BPP_TURNFLAG_Set( bpp, BPP_TURNFLG_ACTION_DONE );
      scPut_SetContFlag( wk, bpp, BPP_CONTFLG_ACTION_DONE );
    }

  }
}
//--------------------------------------------------------------
/**
 *  �A�N�V�����̊��荞�ݎ��s�i�|�P����ID�w��j
 *  �����荞��ő����Ɏ��s�i����́g���������h�ɂ̂ݎg�p�j
 */
//--------------------------------------------------------------
static BOOL ActOrder_IntrProc( BTL_SVFLOW_WORK* wk, u8 intrPokeID )
{
  ACTION_ORDER_WORK* actOrder = SearchActOrderByPokeID( wk, intrPokeID );
  if( actOrder && (actOrder->fDone == FALSE) ){
    ActOrder_Proc( wk, actOrder );
    return TRUE;
  }
  return FALSE;
}
//--------------------------------------------------------------
/**
 *  �A�N�V�����̊��荞�ݗ\��i�|�P����ID�w��j
 *  �����荞��ŁA���ݏ������Ă���A�N�V�����̒���Ɏ��s�i����́g�������ɂǂ����h�ɂ̂ݎg�p�j
 */
//--------------------------------------------------------------
static BOOL ActOrder_IntrReserve( BTL_SVFLOW_WORK* wk, u8 intrPokeID )
{
  ACTION_ORDER_WORK* actOrder = SearchActOrderByPokeID( wk, intrPokeID );
  if( actOrder && (actOrder->fDone == FALSE) ){
    if( IntrActOrder(wk, actOrder, 0) >= 0 ){
      return TRUE;
    }
  }
  return FALSE;
}
//--------------------------------------------------------------
/**
 *  �A�N�V�����̊��荞�ݎ��s�i�g�p���U�w��j
 *  �����荞��ŁA���ݏ������Ă���A�N�V�����̒���Ɏ��s�i����́g��񂵂傤�h�ɂ̂ݎg�p�j
 */
//--------------------------------------------------------------
static BOOL ActOrder_IntrReserveByWaza( BTL_SVFLOW_WORK* wk, WazaID waza )
{
  ACTION_ORDER_WORK* actOrder = SearchActOrderByWaza( wk, waza, 0 );
  int intrIdx = 0;
  BOOL result = FALSE;
  while( actOrder )
  {
    intrIdx = IntrActOrder( wk, actOrder, intrIdx );
    if( intrIdx >= 0 ){
      actOrder = SearchActOrderByWaza( wk, waza, ++intrIdx );
      result = TRUE;
    }else{
      break;
    }
  }
  return result;
}
//--------------------------------------------------------------
/**
 *  �A�N�V�������Ō�ɉ񂷁i�|�P����ID�w��j
 */
//--------------------------------------------------------------
static BOOL ActOrder_SendLast( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  ACTION_ORDER_WORK* actOrder = SearchActOrderByPokeID( wk, pokeID );
  if( actOrder && (actOrder->fDone == FALSE) ){
    SendLastActOrder( wk, actOrder );
    return TRUE;
  }
  return FALSE;
}



//----------------------------------------------------------------------------------------------
// �T�[�o�[�t���[�t���O����
//----------------------------------------------------------------------------------------------
static inline void FlowFlg_Set( BTL_SVFLOW_WORK* wk, FlowFlag flg )
{
  wk->flowFlags[ flg ] = 1;
}
static inline void FlowFlg_Clear( BTL_SVFLOW_WORK* wk, FlowFlag flg )
{
  wk->flowFlags[ flg ] = 0;
}
static inline BOOL FlowFlg_Get( BTL_SVFLOW_WORK* wk, FlowFlag flg )
{
  return wk->flowFlags[ flg ];
}
static inline void FlowFlg_ClearAll( BTL_SVFLOW_WORK* wk )
{
  int i;
  for(i=0; i<NELEMS(wk->flowFlags); ++i)
  {
    wk->flowFlags[ i ] = 0;
  }
}

/**
 *  ���U�G�t�F�N�g�����Ǘ��F������
 */
static void wazaEffCtrl_Init( WAZAEFF_CTRL* ctrl, BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKESET* rec )
{
  u32 target_cnt = BTL_POKESET_GetCountMax( rec );

  ctrl->attackerPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, BPP_GetID(attacker) );

  // �^�[�Q�b�g���P�̂Ȃ疾�m�ȃ^�[�Q�b�g�ʒu�����L�^
  if( target_cnt == 1 ){
    const BTL_POKEPARAM* bpp = BTL_POKESET_Get( rec, 0 );
    ctrl->targetPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, BPP_GetID(bpp) );
  }
  // �^�[�Q�b�g�������̂܂��͏�Ɍ������U�Ȃǂ̏ꍇ�̓^�[�Q�b�g�ʒu���Ȃ�
  else{
    ctrl->targetPos = BTL_POS_NULL;
  }

  ctrl->fEnable = FALSE;
}


/**
 *  ���U�G�t�F�N�g�����Ǘ��F�����m��
 */
static inline void wazaEffCtrl_SetEnable( WAZAEFF_CTRL* ctrl )
{
  if( ctrl->fEnable == FALSE )
  {
    ctrl->fEnable = TRUE;
  }
}
static inline BOOL wazaEffCtrl_IsEnable( const WAZAEFF_CTRL* ctrl )
{
  return ctrl->fEnable;
}

/**
 *  �|�P�������݁������`�F�b�N�ꎞ�֐�
 */
static inline BOOL IsBppExist( const BTL_POKEPARAM* bpp )
{
  if( bpp != NULL ){
    return !BPP_IsDead(bpp);
  }
  return FALSE;
}

//----------------------------------------------------------------------------------------------
// �퓬�ɏo�Ă���|�P�����f�[�^���ԃA�N�Z�X����
//----------------------------------------------------------------------------------------------

/**
 *  ���ԃA�N�Z�X�F���[�N������
 */
static void FRONT_POKE_SEEK_InitWork( FRONT_POKE_SEEK_WORK* fpsw, BTL_SVFLOW_WORK* wk )
{
  fpsw->clientIdx = 0;
  fpsw->pokeIdx = 0;
  fpsw->endFlag = TRUE;

  {
    SVCL_WORK* cw;
    u8 i, j;

    for(i=0; i<BTL_CLIENT_MAX; ++i)
    {
      cw = BTL_SERVER_GetClientWorkIfEnable( wk->server, i );
      if( cw == NULL ){ continue; }

      for(j=0; j<cw->numCoverPos; ++j)
      {
        if( IsBppExist( BTL_PARTY_GetMemberDataConst(cw->party, j)) )
        {
          fpsw->clientIdx = i;
          fpsw->pokeIdx = j;
          fpsw->endFlag = FALSE;
          return;
        }
      }
    }
  }
}
/**
 *  ���ԃA�N�Z�X�F���̃|�P�����f�[�^���擾�iFALSE���Ԃ�����I���j
 */
static BOOL FRONT_POKE_SEEK_GetNext( FRONT_POKE_SEEK_WORK* fpsw, BTL_SVFLOW_WORK* wk, BTL_POKEPARAM** bpp )
{
  if( fpsw->endFlag )
  {
    return FALSE;
  }
  else
  {
    BTL_POKEPARAM* nextPoke = NULL;
    SVCL_WORK* cw = BTL_SERVER_GetClientWork( wk->server, fpsw->clientIdx );

    *bpp = BTL_PARTY_GetMemberData( cw->party, fpsw->pokeIdx );
    fpsw->pokeIdx++;

    while( fpsw->clientIdx < BTL_CLIENT_MAX )
    {
      cw = BTL_SERVER_GetClientWorkIfEnable( wk->server, fpsw->clientIdx );
      if( cw )
      {
        while( fpsw->pokeIdx < cw->numCoverPos )
        {
          nextPoke = BTL_PARTY_GetMemberData( cw->party, fpsw->pokeIdx );
          if( IsBppExist(nextPoke) )
          {
            return TRUE;
          }
          fpsw->pokeIdx++;
        }
      }
      fpsw->clientIdx++;
      fpsw->pokeIdx = 0;
    }
    fpsw->endFlag = TRUE;
    return TRUE;
  }
}
//======================================================================================================
// �T�[�o�[�t���[����
//======================================================================================================

//-----------------------------------------------------------------------------------
// �T�[�o�[�t���[�F�u���[�u�v
//-----------------------------------------------------------------------------------
static void scproc_Move( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  BTL_PARTY* party;
  u8 clientID;
  u8 pokeID;
  int posIdx;

  pokeID = BPP_GetID( bpp );
  clientID = BTL_MAINUTIL_PokeIDtoClientID( pokeID );
  party = BTL_POKECON_GetPartyData( wk->pokeCon, clientID );
  posIdx = BTL_PARTY_FindMember( party, bpp );
  if( (posIdx == 0) || (posIdx == 2) )
  {
    scproc_MoveCore( wk, clientID, posIdx, 1 );
  }
  else
  {
    GF_ASSERT_MSG(0, "clientID=%d, pokeID=%d, posIdx=%d\n", clientID, pokeID, posIdx);
  }
}
static void scproc_MoveCore( BTL_SVFLOW_WORK* wk, u8 clientID, u8 posIdx1, u8 posIdx2 )
{
  BtlPokePos pos1, pos2;

  pos1 = BTL_MAIN_GetClientPokePos( wk->mainModule, clientID, posIdx1 );
  pos2 = BTL_MAIN_GetClientPokePos( wk->mainModule, clientID, posIdx2 );

  BTL_POSPOKE_Swap( &wk->pospokeWork, pos1, pos2 );
  {
    BTL_PARTY* party = BTL_POKECON_GetPartyData( wk->pokeCon, clientID );
    BTL_PARTY_SwapMembers( party, posIdx1, posIdx2 );
  }
  SCQUE_PUT_ACT_MemberMove( wk->que, clientID, pos1, pos2 );
}

//-----------------------------------------------------------------------------------
// �T�[�o�[�t���[�F�u�ɂ���v
//-----------------------------------------------------------------------------------
static BOOL scproc_NigeruCmd( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  BOOL fSkipNigeruCalc = scEvent_SkipNigeruCalc( wk, bpp );

  #ifdef PM_DEBUG
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L ){
    return TRUE;
  }
  #endif

  if( !fSkipNigeruCalc )
  {
    // @@@ �{���̓R�R�œ�����v�Z
    // @@@ ���͂��΂₳�v�Z�������N�ł���������悤�ɂ��Ă���
    // SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_EscapeFail );
    // return FALSE;
  }

  return scproc_NigeruCore( wk, bpp );
}
//----------------------------------------------------------------------------------
/**
 * [Event] �����鎞�̊m���v�Z�������X�L�b�v���邩����
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  BOOL    �X�L�b�v����Ȃ�TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_SkipNigeruCalc( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  if( !BPP_IsDead(bpp) )
  {
    BOOL result = FALSE;
    BTL_EVENTVAR_Push();
      BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
      BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, result );
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_SKIP_NIGERU_CALC );
      result = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
    BTL_EVENTVAR_Pop();
    return result;
  }
  return FALSE;
}
static BOOL scproc_NigeruCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  if( !BPP_IsDead(bpp) )
  {
    // �����֎~�`�F�b�N
    if( !scEvent_SkipNigeruForbid(wk, bpp) )
    {
      u32 hem_state = Hem_PushState( &wk->HEManager );
      BOOL fForbid = scEvent_CheckNigeruForbid( wk, bpp );
      scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
      Hem_PopState( &wk->HEManager, hem_state );
      if( fForbid ){
        return FALSE;
      }
    }
  }

  {
    u32 hem_state = Hem_PushState( &wk->HEManager );
    if( scEvent_NigeruExMessage(wk, bpp) ){
      scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    }else{
//      SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_EscapeSuccess );
      SCQUE_PUT_MSG_STD_SE( wk->que, BTL_STRID_STD_EscapeSuccess, SEQ_SE_NIGERU );
    }
    Hem_PopState( &wk->HEManager, hem_state );
  }

  return TRUE;
}
//----------------------------------------------------------------------------------
/**
 * [Event] �����镕���̃`�F�b�N�������X�L�b�v���邩����
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  BOOL    �X�L�b�v����Ȃ�TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_SkipNigeruForbid( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  BOOL result = FALSE;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, result );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_SKIP_NIGERU_FORBID );
    result = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  BTL_EVENTVAR_Pop();
  return result;
}
//----------------------------------------------------------------------------------
/**
 * [Event] �����镕���`�F�b�N����
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  BOOL  �����镕�����������Ă�����TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_CheckNigeruForbid( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  BOOL result = FALSE;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, result );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_NIGERU_FORBID );
    result = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  BTL_EVENTVAR_Pop();
  return result;
}
//----------------------------------------------------------------------------------
/**
 * [Event] �����邱�Ƃ����������Ƃ��ɓ��ꃁ�b�Z�[�W��\�����邩����
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  BOOL    ���ꃁ�b�Z�[�W��\������Ȃ�TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_NigeruExMessage( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  BOOL result = FALSE;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, result );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_NIGERU_EXMSG );
    result = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  BTL_EVENTVAR_Pop();
  return result;
}


//----------------------------------------------------------------------------------
/**
 * �����o�[����R�A
 *
 * @param   wk
 * @param   clientID      �N���C�A���gID
 * @param   posIdx        ����ʒu index
 * @param   nextPokeIdx   ���ꂳ����|�P�����̌��p�[�e�B�� index
 */
//----------------------------------------------------------------------------------
static void scproc_MemberInCore( BTL_SVFLOW_WORK* wk, u8 clientID, u8 posIdx, u8 nextPokeIdx )
{
  SVCL_WORK* clwk;
  BTL_POKEPARAM* bpp;
  u8 pokeID;


  clwk = BTL_SERVER_GetClientWork( wk->server, clientID );
  GF_ASSERT(posIdx < clwk->numCoverPos);

  {
    BTL_POKEPARAM* tmpBpp = BTL_PARTY_GetMemberData( clwk->party, nextPokeIdx );
    if( BPP_GetValue(tmpBpp, BPP_TOKUSEI_EFFECTIVE) == POKETOKUSEI_IRYUUJON ){
      SCQUE_PUT_OP_SetFakeSrcMember( wk->que, clientID, nextPokeIdx );
      BTL_PARTY_SetFakeSrcMember( clwk->party, nextPokeIdx );
    }
  }

  if( posIdx != nextPokeIdx ){
    BTL_PARTY_SwapMembers( clwk->party, posIdx, nextPokeIdx );
  }
  bpp = BTL_PARTY_GetMemberData( clwk->party, posIdx );
  pokeID = BPP_GetID( bpp );


  BTL_HANDLER_TOKUSEI_Add( bpp );
  BTL_HANDLER_ITEM_Add( bpp );
  BPP_SetAppearTurn( bpp, wk->turnCount );
  BPP_Clear_ForIn( bpp );
  wk->pokeInFlag[ pokeID ] = TRUE;

  SCQUE_PUT_OP_MemberIn( wk->que, clientID, posIdx, nextPokeIdx, wk->turnCount );
  {
    BtlPokePos  pos = BTL_MAIN_GetClientPokePos( wk->mainModule, clientID, posIdx );
    BTL_POSPOKE_PokeIn( &wk->pospokeWork, pos, BPP_GetID(bpp), wk->pokeCon );
  }
}
//----------------------------------------------------------------------------------
/**
 * �����o�[����ւ�
 *
 * @param   wk
 * @param   clientID
 * @param   posIdx
 * @param   next_poke_idx
 * @param   fPutMsg
 */
//----------------------------------------------------------------------------------
static void scproc_MemberInForChange( BTL_SVFLOW_WORK* wk, u8 clientID, u8 posIdx, u8 next_poke_idx, BOOL fPutMsg )
{
  scproc_MemberInCore( wk, clientID, posIdx, next_poke_idx );
  SCQUE_PUT_ACT_MemberIn( wk->que, clientID, posIdx, next_poke_idx, fPutMsg );
}


//----------------------------------------------------------------------------------
/**
 * �T�[�o�[�t���[�F�V������ɓ��ꂵ���|�P��������
 *
 * @param   wk
 *
 */
//----------------------------------------------------------------------------------
static void scproc_AfterMemberIn( BTL_SVFLOW_WORK* wk )
{
  FRONT_POKE_SEEK_WORK fps;
  BTL_POKEPARAM* bpp;
  u8 pokeID;

  u32 hem_state = Hem_PushState( &wk->HEManager );

  FRONT_POKE_SEEK_InitWork( &fps, wk );
  while( FRONT_POKE_SEEK_GetNext(&fps, wk, &bpp) )
  {
    pokeID = BPP_GetID( bpp );
    // @todo ����z���g�̓��N�G�X�g���������ɂ��Ȃ��ƃ_����
    if( wk->pokeInFlag[ pokeID ] )
    {
      scEvent_AfterMemberIn( wk, bpp );
      scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
      wk->pokeInFlag[ pokeID ] = FALSE;
    }
  }
  Hem_PopState( &wk->HEManager, hem_state );

  scproc_CheckExpGet( wk );
}
//----------------------------------------------------------------------------------
/**
 * [Put] �����o�[��֎��́u���� ���ǂ�I�v�Ȃǂ̃��b�Z�[�W�\���R�}���h
 *
 * @param   wk
 * @param   bpp   �������߂�|�P�����p�����[�^
 */
//----------------------------------------------------------------------------------
static void scPut_MemberOutMessage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  u8 pokeID = BPP_GetID( bpp );
  // @@@ �z���g�͏󋵂ɉ����ă��b�Z�[�W�ʂ���
  SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_MemberOut1, pokeID );
}
//----------------------------------------------------------------------------------
/**
 * [Put] �����o�[��֎��Ƀ|�P�������������߂鏈���R�}���h
 *
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
static void scPut_MemberOut( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  u8 pokeID = BPP_GetID( bpp );
  BtlPokePos pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, pokeID );

  SCQUE_PUT_ACT_MemberOut( wk->que, pos );
}
//-----------------------------------------------------------------------------------
// �T�[�o�[�t���[�F�A�C�e���g�p
//-----------------------------------------------------------------------------------
static void scproc_TrainerItem_Root( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, u8 actParam, u8 targetIdx )
{
  typedef u8 (*pItemEffFunc)( BTL_SVFLOW_WORK*, BTL_POKEPARAM*, u16, int, u8 );

  static const struct {
    u16            effect;
    pItemEffFunc   func;
  }ItemParallelEffectTbl[] = {
    { ITEM_PRM_SLEEP_RCV,     ItemEff_SleepRcv      },   // �����
    { ITEM_PRM_POISON_RCV,    ItemEff_PoisonRcv     },   // �ŉ�
    { ITEM_PRM_BURN_RCV,      ItemEff_YakedoRcv     },   // �Ώ���
    { ITEM_PRM_ICE_RCV,       ItemEff_KooriRcv      },   // �X��
    { ITEM_PRM_PARALYZE_RCV,  ItemEff_MahiRcv       },   // ��჉�
    { ITEM_PRM_PANIC_RCV,     ItemEff_KonranRcv     },   // ������
    { ITEM_PRM_MEROMERO_RCV,  ItemEff_MeromeroRcv   },   // ����������
    { ITEM_PRM_ABILITY_GUARD, ItemEff_EffectGuard   },   // �\�̓K�[�h
    { ITEM_PRM_DEATH_RCV,     ItemEff_Relive        },   // �m����
    { ITEM_PRM_ATTACK_UP,     ItemEff_AttackRank    },   // �U���̓A�b�v
    { ITEM_PRM_DEFENCE_UP,    ItemEff_DefenceRank   },   // �h��̓A�b�v
    { ITEM_PRM_SP_ATTACK_UP,  ItemEff_SPAttackRank  },   // ���U�A�b�v
    { ITEM_PRM_SP_DEFENCE_UP, ItemEff_SPDefenceRank },   // ���h�A�b�v
    { ITEM_PRM_AGILITY_UP,    ItemEff_AgilityRank   },   // �f�����A�b�v
    { ITEM_PRM_HIT_UP,        ItemEff_HitRank       },   // �������A�b�v
    { ITEM_PRM_CRITICAL_UP,   ItemEff_CriticalUp    },   // �N���e�B�J�����A�b�v
    { ITEM_PRM_PP_RCV,        ItemEff_PP_Rcv        },   // PP��
    { ITEM_PRM_ALL_PP_RCV,    ItemEff_AllPP_Rcv     },   // PP�񕜁i�S�Ă̋Z�j
    { ITEM_PRM_HP_RCV,        ItemEff_HP_Rcv        },   // HP��
  };

  static const struct {
    u16           itemID;
    pItemEffFunc  func;
  }ShooterItemParam[] = {
    { ITEM_AITEMUKOORU,     ShooterEff_ItemCall  },
    { ITEM_SUKIRUKOORU,     ShooterEff_SkillCall },
    { ITEM_AITEMUDOROPPU,   ShooterEff_ItemDrop  },
    { ITEM_HURATTOKOORU,    ShooterEff_FlatCall  },
  };

  u8 clientID = BTL_MAINUTIL_PokeIDtoClientID( BPP_GetID(bpp) );
  u8 targetPokeID;
  BTL_POKEPARAM* target = NULL;
  int i, itemParam;
  u32 hem_state;

  // @@@ targetIdx = ���p�[�e�B�̉��Ԗڂ̃|�P�����Ɏg�����H�Ƃ����Ӗ������A�}���`�ł��L�����Ƙb������Ă��܂�
  if( targetIdx != BTL_PARTY_MEMBER_MAX ){
    BTL_PARTY* party = BTL_POKECON_GetPartyData( wk->pokeCon, clientID );
    target = BTL_PARTY_GetMemberData( party, targetIdx );
    targetPokeID = BPP_GetID( target );
  }

  // �����́~�~���g�����I
  {
    u8 pokeID = BPP_GetID( bpp );
    u8 clientID = BTL_MAINUTIL_PokeIDtoClientID( pokeID );
    if( wk->bagMode != BBAG_MODE_SHOOTER ){
      int args[2];
      args[0] = clientID;
      args[1] = itemID;
      scPut_Message_StdEx( wk, BTL_STRID_STD_UseItem_Self, 2, args );
    }else{
      SCQUE_PUT_MSG_STD_SE( wk->que, BTL_STRID_STD_UseItem_Shooter, SEQ_SE_SHOOTER, clientID, targetPokeID, itemID );
    }
  }

  // �{�[�������Ȃ�{�[�������V�[�P���X��
  if( BTL_CALC_ITEM_GetParam(itemID, ITEM_PRM_ITEM_TYPE) == ITEMTYPE_BALL )
  {
    BTL_MAIN_DecrementPlayerItem( wk->mainModule, clientID, itemID );
    scproc_TrainerItem_BallRoot( wk, bpp, itemID );
    return;
  }


  {
    BtlPokePos targetPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, targetPokeID );
    if( (targetPos != BTL_POS_NULL) && !BPP_IsDead(target) )
    {
      SCQUE_PUT_ACT_EffectByPos( wk->que, targetPos, BTLEFF_USE_ITEM );
    }
  }

  // �V���[�^�[�Ǝ��̃A�C�e������
  for(i=0; i<NELEMS(ShooterItemParam); ++i)
  {
    if( itemID == ShooterItemParam[i].itemID )
    {
      if( BTL_POSPOKE_GetPokeExistPos(&wk->pospokeWork, targetPokeID) != BTL_POS_NULL )
      {
        ShooterItemParam[i].func( wk, target, itemID, 0, actParam );
      }
      else
      {
        SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_PokeNotExist, targetPokeID );
      }
      return;
    }
  }

  // �ʏ�̃A�C�e������
  {
    BOOL fEffective = FALSE;
    hem_state = Hem_PushState( &wk->HEManager );
    for(i=0; i<NELEMS(ItemParallelEffectTbl); ++i)
    {
      itemParam = BTL_CALC_ITEM_GetParam( itemID, ItemParallelEffectTbl[i].effect );
      if( itemParam ){
        if( ItemParallelEffectTbl[i].func(wk, target, itemID, itemParam, actParam) ){
          fEffective = TRUE;
        }
      }
    }
    // �g�p���ʂ���
    if( fEffective )
    {
      scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
      if( wk->bagMode != BBAG_MODE_SHOOTER ){
        BTL_MAIN_DecrementPlayerItem( wk->mainModule, clientID, itemID );
      }
    }
    else{
      scPut_Message_StdEx( wk, BTL_STRID_STD_UseItem_NoEffect, 0, NULL );
    }
    Hem_PopState( &wk->HEManager, hem_state );
  }
}

//----------------------------------------------------------------------------------
/**
 * �{�[���g�p���[�g
 *
 * @param   wk
 * @param   bpp
 * @param   itemID
 */
//----------------------------------------------------------------------------------
static void scproc_TrainerItem_BallRoot( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID )
{
  if( (BTL_MAIN_GetCompetitor(wk->mainModule) == BTL_COMPETITOR_WILD)
  &&  (BTL_MAIN_GetRule(wk->mainModule) == BTL_RULE_SINGLE)
  ){
    BTL_POKEPARAM* targetPoke;
    BtlPokePos  targetPos = BTL_POS_NULL;

    {
      BtlExPos exPos;
      u8 posAry[ BTL_POSIDX_MAX ];
      u8 basePos;
      u8 posCnt, i;

      basePos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, BPP_GetID(bpp) );

      exPos = EXPOS_MAKE( BTL_EXPOS_AREA_ENEMY, basePos );
      posCnt = BTL_MAIN_ExpandBtlPos( wk->mainModule, exPos, posAry );

      for(i=0; i<posCnt; ++i)
      {
        targetPoke = BTL_POKECON_GetFrontPokeData( wk->pokeCon, posAry[i] );
        if( !BPP_IsDead(targetPoke) )
        {
          targetPos = posAry[i];
          break;
        }
      }
    }

    if( targetPos != BTL_POS_NULL )
    {
      u8 yure_cnt, fSuccess, fZukanRegister;

      // @todo �����ŕߊl���ۃ`�F�b�N�����s�Ȃ�h�ꐔ���v�Z����B���͓K���B
      if( itemID == ITEM_MASUTAABOORU ){
        yure_cnt = 3;
        fSuccess = TRUE;
      }else{
        fSuccess = BTL_CALC_GetRand(100) < 75;
        if( fSuccess ){
          yure_cnt = 3;
        }else{
          yure_cnt = BTL_CALC_GetRand( 4 );
        }
      }

      if( fSuccess ){
        wk->flowResult = SVFLOW_RESULT_POKE_GET;
        wk->getPokePos = targetPos;
        fZukanRegister = !BTL_MAIN_IsZukanRegistered( wk->mainModule, targetPoke );
        BTL_SERVER_NotifyPokemonCapture( wk->server, targetPoke );
      }else{
        fZukanRegister = FALSE;
      }

      SCQUE_PUT_ACT_BallThrow( wk->que, targetPos, yure_cnt, fSuccess, fZukanRegister, itemID );
    }
  }
}

// �A�C�e�����ʁF�˂ނ��
static u8 ItemEff_SleepRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Common_Cure( wk, bpp, itemID, itemParam, WAZASICK_NEMURI );
}
static u8 ItemEff_PoisonRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Common_Cure( wk, bpp, itemID, itemParam, WAZASICK_DOKU );
}
static u8 ItemEff_YakedoRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Common_Cure( wk, bpp, itemID, itemParam, WAZASICK_YAKEDO );
}
static u8 ItemEff_KooriRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Common_Cure( wk, bpp, itemID, itemParam, WAZASICK_KOORI );
}
static u8 ItemEff_MahiRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Common_Cure( wk, bpp, itemID, itemParam, WAZASICK_MAHI );
}
static u8 ItemEff_KonranRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Common_Cure( wk, bpp, itemID, itemParam, WAZASICK_KONRAN );
}
static u8 ItemEff_MeromeroRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Common_Cure( wk, bpp, itemID, itemParam, WAZASICK_MEROMERO );
}
static u8 ItemEff_EffectGuard( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  BtlSide side = BTL_MAINUTIL_PokeIDtoSide( BPP_GetID(bpp) );
  BPP_SICK_CONT cont = BPP_SICKCONT_MakeTurn( 5 );
  if( BTL_HANDLER_SIDE_Add(side, BTL_SIDEEFF_SIROIKIRI, cont) )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( wk, BTL_HANDEX_MESSAGE, BTL_POKEID_NULL );
    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_STD, BTL_STRID_STD_SiroiKiri );
    HANDEX_STR_AddArg( &param->str, side );
    return TRUE;
  }
  return FALSE;
}
static u8 ItemEff_Relive( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  if( BPP_IsDead(bpp))
  {
    u8 pokeID = BPP_GetID( bpp );
    BTL_HANDEX_PARAM_RELIVE* param = BTL_SVF_HANDEX_Push( wk, BTL_HANDEX_RELIVE, pokeID );
    param->pokeID = pokeID;

    itemParam = BTL_CALC_ITEM_GetParam( itemID, ITEM_PRM_HP_RCV_POINT );
    switch( itemParam ){
    case ITEM_RECOVER_HP_FULL:
      param->recoverHP = BPP_GetValue( bpp, BPP_MAX_HP ); break;
    case ITEM_RECOVER_HP_HALF:
      param->recoverHP = BTL_CALC_QuotMaxHP( bpp, 2 ); break;
    default:
      param->recoverHP = BTL_CALC_ITEM_GetParam( itemID, ITEM_PRM_HP_RCV_POINT ); break;
    }

    HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Relive );
    HANDEX_STR_AddArg( &param->exStr, pokeID );

    // ���^�[���ɐ����Ԃ����|�P�������L�^
    relivePokeRec_Add( wk, pokeID );
    return TRUE;
  }
  return FALSE;
}
static u8 ItemEff_AttackRank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Common_Rank( wk, bpp, itemID, itemParam, BPP_ATTACK );
}
static u8 ItemEff_DefenceRank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Common_Rank( wk, bpp, itemID, itemParam, BPP_DEFENCE );
}
static u8 ItemEff_SPAttackRank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Common_Rank( wk, bpp, itemID, itemParam, BPP_SP_ATTACK );
}
static u8 ItemEff_SPDefenceRank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Common_Rank( wk, bpp, itemID, itemParam, BPP_SP_DEFENCE );
}
static u8 ItemEff_AgilityRank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Common_Rank( wk, bpp, itemID, itemParam, BPP_AGILITY );
}
static u8 ItemEff_HitRank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Common_Rank( wk, bpp, itemID, itemParam, BPP_HIT_RATIO );
}
static u8 ItemEff_CriticalUp( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  u8 result = FALSE;
  if( !BPP_CONTFLAG_Get(bpp, BPP_CONTFLG_KIAIDAME) )
  {
    scPut_SetContFlag( wk, bpp, BPP_CONTFLG_KIAIDAME );
    result = TRUE;
  }
  if( itemParam > 1 ){
    --itemParam;
    if( BPP_AddCriticalRank(bpp, itemParam) ){
      SCQUE_PUT_OP_AddCritical( wk->que, BPP_GetID(bpp), itemParam );
      result = TRUE;
    }
  }
  if( result ){
    scPut_Message_Set( wk, bpp, BTL_STRID_SET_KiaiDame );
    return TRUE;
  }
  return FALSE;
}
static u8 ItemEff_PP_Rcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  if( BPP_WAZA_GetCount(bpp) > actParam )
  {
    u8 pokeID = BPP_GetID( bpp );
    u8 volume = BPP_WAZA_GetPPShort( bpp, actParam );
    u8 ppValue = BTL_CALC_ITEM_GetParam( itemID, ITEM_PRM_PP_RCV_POINT );
    if( ppValue != ITEM_RECOVER_PP_FULL ){
      if( volume > ppValue ){
        volume = ppValue;
      }
    }
    if( volume )
    {
      BTL_HANDEX_PARAM_PP* param = BTL_SVF_HANDEX_Push( wk, BTL_HANDEX_RECOVER_PP, pokeID );
      param->volume = volume;
      param->pokeID = pokeID;
      param->wazaIdx = actParam;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_PP_Recover );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
      HANDEX_STR_AddArg( &param->exStr, BPP_WAZA_GetID(bpp, actParam) );
      return 1;
    }
  }
  return 0;
}
static u8 ItemEff_AllPP_Rcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  BTL_HANDEX_PARAM_MESSAGE* msg_param;
  u32 cnt, volume, i;
  u8 pokeID = BPP_GetID( bpp );

  cnt = BPP_WAZA_GetCount( bpp );
  for(i=0; i<cnt; ++i)
  {
    volume = BPP_WAZA_GetPPShort( bpp, i );
    if( volume )
    {
      BTL_HANDEX_PARAM_PP* param = BTL_SVF_HANDEX_Push( wk, BTL_HANDEX_RECOVER_PP, pokeID );
      param->pokeID = pokeID;
      param->volume = volume;
    }
  }

  msg_param = BTL_SVF_HANDEX_Push( wk, BTL_HANDEX_MESSAGE, pokeID );
  HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_PP_AllRecover );
  HANDEX_STR_AddArg( &msg_param->str, pokeID );
  return 1;
}
static u8 ItemEff_HP_Rcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  /*
  if( BTL_CALC_ITEM_GetParam(itemID, ITEM_PRM_DEATH_RCV) ){
    return ItemEff_Relive( wk, bpp, itemID, itemParam, actParam );
  }
  */

  if( !BPP_IsHPFull(bpp)
  &&  !BPP_IsDead(bpp)
  ){
    u8 pokeID = BPP_GetID( bpp );
    BTL_HANDEX_PARAM_RECOVER_HP* param = BTL_SVF_HANDEX_Push( wk, BTL_HANDEX_RECOVER_HP, pokeID );
    param->pokeID = pokeID;
    itemParam = BTL_CALC_ITEM_GetParam( itemID, ITEM_PRM_HP_RCV_POINT );
    switch( itemParam ){
    case ITEM_RECOVER_HP_FULL:
      param->recoverHP = BPP_GetValue( bpp, BPP_MAX_HP ); break;
    case ITEM_RECOVER_HP_HALF:
      param->recoverHP = BTL_CALC_QuotMaxHP(bpp, 2 ); break;
    default:
      param->recoverHP = itemParam; break;
    }

    HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_HP_Recover );
    HANDEX_STR_AddArg( &param->exStr, pokeID );

    return 1;
  }
  return 0;
}

/**
 *  ��Ԉُ펡���n����
 */
static u8 ItemEff_Common_Cure( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, WazaSick sickID )
{
  if( !BPP_IsDead(bpp) )
  {
    if( BPP_CheckSick(bpp, sickID) )
    {
      BTL_HANDEX_PARAM_CURE_SICK* param;
      u8 pokeID = BPP_GetID( bpp );
      param = BTL_SVF_HANDEX_Push( wk, BTL_HANDEX_CURE_SICK, pokeID );
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
      param->sickCode = sickID;
      return TRUE;
    }
  }
  return FALSE;
}
/**
 *  �����N�A�b�v�n����
 */
static u8 ItemEff_Common_Rank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, BppValueID rankType )
{
  u8 pokeID = BPP_GetID( bpp );

  if( (BTL_POSPOKE_GetPokeExistPos(&wk->pospokeWork, pokeID) != BTL_POS_NULL )
  &&  !BPP_IsDead(bpp)
  ){
    if( BPP_IsRankEffectValid(bpp, rankType, itemParam) )
    {
      BTL_HANDEX_PARAM_RANK_EFFECT* param = BTL_SVF_HANDEX_Push( wk, BTL_HANDEX_RANK_EFFECT, pokeID );
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
      param->rankType = rankType;
      param->rankVolume = itemParam;
      param->fAlmost = TRUE;
      return 1;
    }
  }
  else
  {
    BTL_Printf("��ɏo�Ă��Ȃ��̂Ń����N���ʂȂ�\n");
  }

  return 0;
}
/**
 *  �V���[�^�[��p�F�A�C�e���R�[��
 */
static u8 ShooterEff_ItemCall( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  u8 pokeID = BPP_GetID( bpp );

  BTL_N_Printf( DBGSTR_SVFL_UseItemCall, BPP_GetID(bpp) );
  scproc_CheckItemReaction( wk, bpp );
  // @todo �g����A�C�e����������΁u���������ʂ����������v�Ƃ��K�v�H
  return TRUE;
}
/**
 *  �V���[�^�[��p�F�X�L���R�[��
 */
static u8 ShooterEff_SkillCall( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  u32 hem_state = Hem_PushState( &wk->HEManager );
  scEvent_AfterMemberIn( wk, bpp );
  scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
  Hem_PopState( &wk->HEManager, hem_state );
  return TRUE;
}
/**
 *  �V���[�^�[��p�F�A�C�e���h���b�v
 */
static u8 ShooterEff_ItemDrop( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  u16 equipItemID = BPP_GetItem( bpp );
  if( equipItemID != ITEM_DUMMY_DATA )
  {
    u32 hem_state = Hem_PushState( &wk->HEManager );
    {
      BTL_HANDEX_PARAM_SET_ITEM* param;
      u8 pokeID = BPP_GetID( bpp );

      param = BTL_SVF_HANDEX_Push( wk, BTL_HANDEX_SET_ITEM, pokeID );
      param->pokeID = pokeID;
      param->itemID = ITEM_DUMMY_DATA;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Shooter_ItemDrop );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
      HANDEX_STR_AddArg( &param->exStr, equipItemID );

      scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    }
    Hem_PopState( &wk->HEManager, hem_state );
    return TRUE;
  }
  return FALSE;
}
/**
 *  �V���[�^�[��p�F�t���b�g�R�[��
 */
static u8 ShooterEff_FlatCall( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  u32 hem_state = Hem_PushState( &wk->HEManager );
  {
    BTL_HANDEX_PARAM_RESET_RANK* reset_param;
    BTL_HANDEX_PARAM_MESSAGE* msg_param;
    u8 pokeID = BPP_GetID( bpp );

    reset_param = BTL_SVF_HANDEX_Push( wk, BTL_HANDEX_RESET_RANK, pokeID );
    msg_param = BTL_SVF_HANDEX_Push( wk, BTL_HANDEX_MESSAGE, pokeID );

    reset_param->poke_cnt = 1;
    reset_param->pokeID[0] = pokeID;

    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Shooter_FlatCall );
  }
  scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
  Hem_PopState( &wk->HEManager, hem_state );
  return TRUE;
}





//-----------------------------------------------------------------------------------
// �T�[�o�[�t���[�F�����o�[���
//-----------------------------------------------------------------------------------
static void scproc_MemberChange( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* outPoke, u8 nextPokeIdx )
{
  // @@@ ���ۃ`�F�b�N ... �R�}���h�I�����Ƀn�W���̂ŕK�v�Ȃ����H

  scPut_MemberOutMessage( wk, outPoke );

  if( scproc_MemberOutForChange(wk, outPoke) )
  {
    BtlPokePos pos;
    u8 clientID, posIdx;
    u8 outPokeID = BPP_GetID( outPoke );

    pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, outPokeID );
    BTL_MAIN_BtlPosToClientID_and_PosIdx( wk->mainModule, pos, &clientID, &posIdx );

    scproc_MemberInForChange( wk, clientID, posIdx, nextPokeIdx, TRUE );
    scproc_AfterMemberIn( wk );
  }
}
//----------------------------------------------------------------------------------
/**
 * �����o�[���ꂩ��ޏꂳ����i����ւ������O�j
 *
 * @param   wk
 * @param   bpp     �ޏꂳ����|�P�����p�����[�^
 *
 * @retval  BOOL    �ޏ�ł�����TRUE�^���荞�܂�Ď��񂾂肵����FALSE
 */
//----------------------------------------------------------------------------------
static BOOL scproc_MemberOutForChange( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* outPoke )
{
  // ���荞�݃`�F�b�N
  {
    u8 intrPokeID = scEvent_MemberChangeIntr( wk, outPoke );
    if( intrPokeID != BTL_POKEID_NULL )
    {
      ActOrder_IntrProc( wk, intrPokeID );
    }
  }

  if( !BPP_IsDead(outPoke) )
  {
    if( scproc_MemberOutCore(wk, outPoke) ){
      return TRUE;
    }
  }

  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * �����o�[�ޏ�i����ւ��E������΂��E���S�j���ʏ���
 *
 * @param   wk
 * @param   outPoke
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL scproc_MemberOutCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* outPoke )
{
  scPut_MemberOut( wk, outPoke );
  BPP_Clear_ForOut( outPoke );
  SCQUE_PUT_OP_OutClear( wk->que, BPP_GetID(outPoke) );

  {
    u32 hem_state = Hem_PushState( &wk->HEManager );
    scEvent_MemberOutFixed( wk, outPoke );
    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    Hem_PopState( &wk->HEManager, hem_state );
  }

  BTL_POSPOKE_PokeOut( &wk->pospokeWork, BPP_GetID(outPoke) );
  scproc_ClearPokeDependEffect( wk, outPoke );

  return TRUE;
}
//----------------------------------------------------------------------------------
/**
 * [Event] �����o�[�ޏ�̊m��
 *
 * @param   wk
 * @param   outPoke
 */
//----------------------------------------------------------------------------------
static void scEvent_MemberOutFixed( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* outPoke )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(outPoke) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_MEMBER_OUT_FIXED );
  BTL_EVENTVAR_Pop();
}

//-----------------------------------------------------------------------------------
// �T�[�o�[�t���[�F�u���������v���[�g
//-----------------------------------------------------------------------------------
static void scproc_Fight( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, REQWAZA_WORK* reqWaza, BTL_ACTION_PARAM* action )
{
  WazaID  orgWaza, actWaza;
  BtlPokePos  orgTargetPos, actTargetPos;
  u8 waza_exe_flag, waza_lock_flag;

  orgWaza = action->fight.waza;
  orgTargetPos = action->fight.targetPos;
  waza_exe_flag = FALSE;
  waza_lock_flag = BPP_CheckSick( attacker, WAZASICK_WAZALOCK );

  BTL_HANDLER_Waza_Add( attacker, orgWaza );
  scEvent_StartWazaSeq( wk, attacker, orgWaza );

  do {
    // ���U�o�����s����P�i�|�P�����n��Ԉُ큕������A���������A�Ђ�݁j
    if( scproc_Fight_CheckWazaExecuteFail_1st(wk, attacker, orgWaza) ){ break; }

    // �h�����U�Ăяo���p�����[�^�`�F�b�N�����s����
    if( reqWaza->wazaID == WAZANO_NULL )
    {
      if( !scEvent_GetReqWazaParam(wk, attacker, orgWaza, orgTargetPos, reqWaza) ){
        scPut_WazaMsg( wk, attacker, orgWaza );
        scproc_WazaExecuteFailed( wk, attacker, orgWaza, SV_WAZAFAIL_OTHER );
        break;
      }
    }
    // �h�����U�Ăяo������ꍇ�A���b�Z�[�W�o�́����U�o���m��C�x���g
    if( reqWaza->wazaID != WAZANO_NULL )
    {
      scPut_WazaMsg( wk, attacker, orgWaza );
      scPut_ReqWazaEffect( wk, attacker, orgWaza, orgTargetPos );
      BTL_HANDLER_Waza_Add( attacker, reqWaza->wazaID );
      actWaza = reqWaza->wazaID;
      actTargetPos = reqWaza->targetPos;
      BTL_Printf("�����U���s�� [%d] -> [%d] ���o�� ... targetPos=%d\n", orgWaza, actWaza, actTargetPos);
      scproc_WazaExe_Decide( wk, attacker, orgWaza );
    }else{
      actWaza = orgWaza;
      actTargetPos = orgTargetPos;
    }
    // ���U�p�����[�^�m��
    scEvent_GetWazaParam( wk, actWaza, attacker, &wk->wazaParam );

    // ���U���b�Z�[�W�o��
    if( scEvent_CheckWazaMsgCustom(wk, attacker, orgWaza, actWaza, &wk->strParam) ){
      // �����U�Ăяo�����ȂǁA���U�����b�Z�[�W���J�X�^�}�C�Y�����ꍇ
      handexSub_putString( wk, &wk->strParam );
      HANDEX_STR_Clear( &wk->strParam );
    }else{
      scPut_WazaMsg( wk, attacker, actWaza );
    }

    // ���U�o�����s����Q
    if( scproc_Fight_CheckWazaExecuteFail_2nd(wk, attacker, actWaza) ){ break; }

    // �x���������U�̏�������
    if( scproc_Fight_CheckDelayWazaSet(wk, attacker, actWaza, actTargetPos) ){ break; }

    // ���U�Ώۂ����[�N�Ɏ擾
    BTL_POKESET_Clear( &wk->pokesetTarget );
    registerWazaTargets( wk, attacker, actTargetPos, &wk->wazaParam, &wk->pokesetTarget );

    // �����܂ŗ����烏�U���o�邱�Ƃ͊m��
    BTL_WAZAREC_Add( &wk->wazaRec, actWaza, wk->turnCount, BPP_GetID(attacker) );
    BTL_Printf( "�|�P[%d]�����U_%d���o��\n", BPP_GetID(attacker), actWaza );
    scproc_WazaExe_Decide( wk, attacker, actWaza );

    {
      // ���U�����蔻��
      u8 robPokeID, fReflect;
      BtlPokePos robTargetPos;
      robPokeID = scproc_Check_WazaRob(wk, attacker, actWaza, &wk->pokesetTarget, &robTargetPos, &fReflect );

      // �N������������
      if( robPokeID != BTL_POKEID_NULL )
      {
        BTL_POKEPARAM* robPoke = BTL_POKECON_GetPokeParam( wk->pokeCon, robPokeID );

        // ������i���������|�P����̂ɂȂ�A�w�肳�ꂽ�^�[�Q�b�g�Ƀ��U���o���j
        if( !fReflect )
        {
          u32 hem_state = Hem_PushState( &wk->HEManager );
          scEvent_WazaRob( wk, robPoke, attacker, actWaza );
          scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
          Hem_PopState( &wk->HEManager, hem_state );

          // ���U�p�����[�^�����ւ�
          scEvent_GetWazaParam( wk, actWaza, robPoke, &wk->wazaParam );
          registerWazaTargets( wk, robPoke, actTargetPos, &wk->wazaParam, &wk->pokesetRobTarget );

          BTL_HANDLER_Waza_Add( robPoke, actWaza );
          scproc_Fight_WazaExe( wk, robPoke, actWaza, &wk->pokesetRobTarget );
          BTL_HANDLER_Waza_Remove( robPoke, actWaza );
        }
        // ���˕Ԃ��i��������O�̃��U���o��������ŁA���˕Ԃ��j
        else
        {
          // ���˕Ԃ��|�P���^�[�Q�b�g���珜�O
          BTL_POKESET_Remove( &wk->pokesetTarget, robPoke );

          // �܂��^�[�Q�b�g���c���Ă���܂��͕��ʂɏo��
          if( BTL_POKESET_GetCount(&wk->pokesetTarget) ){
            scproc_Fight_WazaExe( wk, attacker, actWaza, &wk->pokesetTarget );
          // �c���ĂȂ�������A���˕Ԃ��|�P���^�[�Q�b�g�ɖ߂��ăG�t�F�N�g��������������
          }else{
            u16 que_pos = SCQUE_RESERVE_Pos( wk->que, SC_ACT_WAZA_EFFECT );
            BTL_POKESET_Add( &wk->pokesetTarget, robPoke );
            wazaEffCtrl_Init( &wk->wazaEffCtrl, wk, attacker, &wk->pokesetTarget );
            wazaEffCtrl_SetEnable( &wk->wazaEffCtrl );
            scPut_WazaEffect( wk, actWaza, &wk->wazaEffCtrl, que_pos );
          }

          // ���˕Ԃ��m��C�x���g
          {
            u32 hem_state = Hem_PushState( &wk->HEManager );
            scEvent_WazaReflect( wk, robPoke, attacker, actWaza );
            scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
            Hem_PopState( &wk->HEManager, hem_state );
          }

          // ���̌�A���˕Ԃ�����
          // ���U�p�����[�^�����ւ�
          scEvent_GetWazaParam( wk, actWaza, robPoke, &wk->wazaParam );
          registerWazaTargets( wk, robPoke, actTargetPos, &wk->wazaParam, &wk->pokesetRobTarget );

          BTL_HANDLER_Waza_Add( robPoke, actWaza );
          scproc_Fight_WazaExe( wk, robPoke, actWaza, &wk->pokesetRobTarget );
          BTL_HANDLER_Waza_Remove( robPoke, actWaza );
        }
      }
      // ������Ȃ��Œʏ폈��
      else
      {
        scproc_Fight_WazaExe( wk, attacker, actWaza, &wk->pokesetTarget );
      }

      BPP_UpdatePrevWazaID( attacker, actWaza, actTargetPos );
      SCQUE_PUT_OP_UpdateUseWaza( wk->que, BPP_GetID(attacker), actTargetPos, actWaza );
      wk->prevExeWaza = actWaza;
      waza_exe_flag = TRUE;
    }

  }while(0);

  // ���U�v���Z�X�C������
  BPP_TURNFLAG_Set( attacker, BPP_TURNFLG_WAZAPROC_DONE );

  // �g�������U��PP���炷�i�O�^�[�����烍�b�N����Ă���ꍇ�͌��炳�Ȃ��j
  if( !waza_lock_flag && waza_exe_flag ){
    u8 wazaIdx = BPP_WAZA_SearchIdx( attacker, orgWaza );
    if( wazaIdx != PTL_WAZA_MAX ){
      scproc_decrementPPUsedWaza( wk, attacker, wazaIdx, &wk->pokesetDamaged );
    }
  }

  if( reqWaza->wazaID != WAZANO_NULL ){
    BTL_HANDLER_Waza_Remove( attacker, reqWaza->wazaID );
  }
  BTL_HANDLER_Waza_Remove( attacker, orgWaza );
  scEvent_EndWazaSeq( wk, attacker, orgWaza );

  // @@@ ���U�n���h���o�^�Ɏ��s�������̏����͂ǁ[������
  if( waza_exe_flag == FALSE ){
    scPut_ResetSameWazaCounter( wk, attacker );
  }
}
//----------------------------------------------------------------------------------
/**
 * ���U�����m��C�x���g�Ăяo������
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scproc_WazaExe_Decide( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza )
{
  u32 hem_state = Hem_PushState( &wk->HEManager );

  scEvent_WazaExeDecide( wk, attacker, waza );
  scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
  Hem_PopState( &wk->HEManager, hem_state );
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U�����m��
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scEvent_WazaExeDecide( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_EXE_DECIDE );
  BTL_EVENTVAR_Pop();
}

//----------------------------------------------------------------------------------
/**
 * �K�v�Ȃ�A�x���������U�̏����������s���B
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 * @param   targetPos
 *
 * @retval  BOOL      �x���������U�����������s�����ꍇTRUE
 */
//----------------------------------------------------------------------------------
static BOOL scproc_Fight_CheckDelayWazaSet( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza, BtlPokePos targetPos )
{
  u32 hem_state = Hem_PushState( &wk->HEManager );
  BOOL result = scEvent_CheckDelayWazaSet( wk, attacker, targetPos );

  if( result ){
    BtlPokePos  atPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, BPP_GetID(attacker) );
    SCQUE_PUT_ACT_WazaEffect( wk->que, atPos, targetPos, waza );
  }

  scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
  Hem_PopState( &wk->HEManager, hem_state );

  return result;
}
//----------------------------------------------------------------------------------
/**
 * [Event] �x�����U�����`�F�b�N
 *
 * @param   wk
 * @param   attacker
 * @param   targetPos
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_CheckDelayWazaSet( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, BtlPokePos targetPos )
{
  BOOL result;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEPOS, targetPos );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, FALSE );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHECK_DELAY_WAZA );
    result = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  BTL_EVENTVAR_Pop();

  return result;
}

//----------------------------------------------------------------------------------
/**
 * [Event] ���U�V�[�P���X�J�n
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scEvent_StartWazaSeq( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZASEQ_START );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U�V�[�P���X�I��
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scEvent_EndWazaSeq( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZASEQ_END );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U������m��
 *
 * @param   wk
 * @param   robPoke       ����������
 * @param   orgAtkPoke    ���X�A���U���o������
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scEvent_WazaRob( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* robPoke, const BTL_POKEPARAM* orgAtkPoke, WazaID waza )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(robPoke) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(orgAtkPoke) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZASEQ_ROB );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U���˕Ԃ��m��
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scEvent_WazaReflect( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* robPoke, const BTL_POKEPARAM* orgAtkPoke, WazaID waza )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(robPoke) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(orgAtkPoke) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZASEQ_REFRECT );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * �킴������`�F�b�N
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 * @param   rec       [io] �{���̃^�[�Q�b�g�Q�B���������|�P�͂������珜�O�����B
 * @param   targetPos [out] ���������ꍇ�A���̑Ώۈʒu��ێ�
 * @param   fReflect  [out] ���肪���U���o���Ă���A����𒵂˕Ԃ��P�[�X��TRUE������
 *
 * @retval  u8    ���������|�P������ID�^���������|�P�����Ȃ��ꍇ��BTL_POKEID_NULL
 */
//----------------------------------------------------------------------------------
static u8 scproc_Check_WazaRob( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza,
  BTL_POKESET* rec, BtlPokePos* targetPos, u8 *fReflect )
{
  u8 robberPokeID = BTL_POKEID_NULL;
  u8 robTargetPokeID = BTL_POKEID_NULL;

  scEvent_CheckWazaRob( wk, attacker, waza, rec, &robberPokeID, &robTargetPokeID, fReflect );
  if( robberPokeID != BTL_POKEID_NULL )
  {
    if( robTargetPokeID != BTL_POKEID_NULL ){
      *targetPos = BTL_POSPOKE_GetPokeExistPos( &wk->pospokeWork, robTargetPokeID );
    }else{
      *targetPos = BTL_POS_NULL;
    }

    // ���˕Ԃ��t���O @todo ���˕Ԃ����Ώۃ|�PID�͎w�肵�Ȃ��P�[�X������
    *fReflect = (robberPokeID != robTargetPokeID);
    return robberPokeID;
  }

  return BTL_POKEID_NULL;
}
//----------------------------------------------------------------------------------
/**
 * [Event] �����ȊO�̃|�P���o�������U�������锻��
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 * @param   targetPos
 * @param   robberPokeID    [out] ���������|�P����ID
 * @param   robTargetPokeID [out] ���������ꍇ�̑Ώۃ|�P����ID
 * @param   strParam        [out] ���������ꍇ�̃��b�Z�[�W�p�����[�^
 *
 * @retval  BOOL    ����������TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_CheckWazaRob( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza,
  BTL_POKESET* targetRec, u8* robberPokeID, u8* robTargetPokeID, u8* fReflect )
{
  u32 targetCnt = BTL_POKESET_GetCount( targetRec );
  u32 i;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_TARGET_POKECNT, targetCnt );
    {
      BTL_POKEPARAM* bpp;
      for(i=0; i<targetCnt; ++i)
      {
        bpp = BTL_POKESET_Get( targetRec, i );
        BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_TARGET1+i, BPP_GetID(bpp) );
      }
    }
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_POKEID, BTL_POKEID_NULL );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, FALSE );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEID_NULL );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHECK_WAZA_ROB );

    *robberPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID );
    *robTargetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
    *fReflect = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );

  BTL_EVENTVAR_Pop();

  return (*robberPokeID) != BTL_POKEID_NULL;
}

// �����U�Ăяo�����郏�U�ŁA�����s����ꍇ�̂�TRUE
static BOOL scproc_Fight_CheckReqWazaFail( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID orgWazaID )
{
  BOOL failFlag = FALSE;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, orgWazaID );
    BTL_EVENTVAR_SetValue( BTL_EVAR_FAIL_FLAG, failFlag );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_REQWAZA_CHECKFAIL );
    failFlag = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  BTL_EVENTVAR_Pop();

  return failFlag;
}
// �����U�Ăяo�����郏�U�̃G�t�F�N�g�𔭓�
static void scPut_ReqWazaEffect( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, WazaID waza, BtlPokePos targetPos )
{
  BtlPokePos  atkPos;
  u8 pokeID = BPP_GetID( bpp );

  atkPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, pokeID );

  SCQUE_PUT_ACT_WazaEffect( wk->que, atkPos, targetPos, waza );
  BPP_UpdatePrevWazaID( bpp, waza, targetPos );
  SCQUE_PUT_OP_UpdateUseWaza( wk->que, pokeID, targetPos, waza );
}
//--------------------------------------------------------------------------
/**
 * �Ώۃ|�P�����̃f�[�^�|�C���^���ABTL_POKESET�\���̂Ɋi�[
 *
 * @param   wk
 * @param   atPos
 * @param   waza
 * @param   action
 * @param   rec       [out]
 *
 * @retval  u8        �Ώۃ|�P������
 */
//--------------------------------------------------------------------------
static u8 registerWazaTargets( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BtlPokePos targetPos,
  const SVFL_WAZAPARAM* wazaParam, BTL_POKESET* rec )
{
  BtlRule rule = BTL_MAIN_GetRule( wk->mainModule );

  u8 intrPokeID = scEvent_GetWazaTargetIntr( wk, attacker, wazaParam );
  u8 numTarget;

  BTL_Printf("���荞�݃^�[�Q�b�g�|�P=%d\n", intrPokeID);

  BTL_POKESET_Clear( rec );

  switch( rule ){
  case BTL_RULE_SINGLE:
  default:
    numTarget = registerTarget_single( wk, attacker, targetPos, wazaParam, intrPokeID, rec );
    break;

  case BTL_RULE_DOUBLE:
  case BTL_RULE_ROTATION:
    numTarget = registerTarget_double( wk, attacker, targetPos, wazaParam, intrPokeID, rec );
    break;

  case BTL_RULE_TRIPLE:
    numTarget = registerTarget_triple( wk, attacker, targetPos, wazaParam, intrPokeID, rec );
    break;
  }

  if( numTarget == 1 ){
    if( correctTargetDead(wk, rule, attacker, wazaParam, targetPos, rec) == FALSE ){
      numTarget = 0;
    }
  }

  return numTarget;
}
/**
 *  ���U�Ώۊi�[�F�V���O���p
 */
static u8 registerTarget_single( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BtlPokePos targetPos,
  const SVFL_WAZAPARAM* wazaParam, u8 intrPokeID, BTL_POKESET* rec )
{
  WazaTarget  targetType = WAZADATA_GetParam( wazaParam->wazaID, WAZAPARAM_TARGET );
  BtlPokePos  atPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, BPP_GetID(attacker) );

  switch( targetType ){
  case WAZA_TARGET_OTHER_SELECT:  ///< �����ȊO�̂P�́i�I���j
  case WAZA_TARGET_ENEMY_SELECT:  ///< �G�P�́i�I���j
  case WAZA_TARGET_ENEMY_RANDOM:  ///< �G�����_��
  case WAZA_TARGET_ENEMY_ALL:     ///< �G���Q��
  case WAZA_TARGET_OTHER_ALL:     ///< �����ȊO�S��
    if( intrPokeID == BTL_POKEID_NULL ){
      BTL_POKESET_Add( rec, get_opponent_pokeparam(wk, atPos, 0) );
    }else{
      BTL_POKESET_Add( rec, BTL_POKECON_GetPokeParam(wk->pokeCon, intrPokeID) );
    }
    return 1;

  case WAZA_TARGET_USER:                ///< �����P�̂̂�
  case WAZA_TARGET_FRIEND_USER_SELECT:  ///< �������܂ޖ����P��
    if( intrPokeID == BTL_POKEID_NULL ){
      BTL_POKESET_Add( rec, attacker );
    }else{
      BTL_POKESET_Add( rec, BTL_POKECON_GetPokeParam(wk->pokeCon, intrPokeID) );
    }
    return 1;

  case WAZA_TARGET_ALL:
    BTL_POKESET_Add( rec, attacker );
    BTL_POKESET_Add( rec, get_opponent_pokeparam(wk, atPos, 0) );
    return 2;

  case WAZA_TARGET_UNKNOWN:
    if( intrPokeID != BTL_POKEID_NULL ){
      BTL_POKESET_Add( rec, BTL_POKECON_GetPokeParam(wk->pokeCon, intrPokeID) );
      return 1;
    }else{
      return 0;
    }
    break;

  default:
    return 0;
  }
}
/**
 *  ���U�Ώۊi�[�F�_�u���p
 */
static u8 registerTarget_double( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BtlPokePos targetPos,
  const SVFL_WAZAPARAM* wazaParam, u8 intrPokeID, BTL_POKESET* rec )
{
  WazaTarget  targetType = WAZADATA_GetParam( wazaParam->wazaID, WAZAPARAM_TARGET );
  BtlPokePos  atPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, BPP_GetID(attacker) );

  BTL_POKEPARAM* bpp = NULL;

  switch( targetType ){
  case WAZA_TARGET_OTHER_SELECT:        ///< �����ȊO�̂P�́i�I���j
    bpp = BTL_POKECON_GetFrontPokeData( wk->pokeCon, targetPos );
    break;
  case WAZA_TARGET_ENEMY_SELECT:        ///< �G�P�́i�I���j
    bpp = BTL_POKECON_GetFrontPokeData( wk->pokeCon, targetPos );
    break;
  case WAZA_TARGET_ENEMY_RANDOM:        ///< �G�����_���ɂP��
    bpp = get_opponent_pokeparam( wk, atPos, BTL_CALC_GetRand(2) );
    break;

  case WAZA_TARGET_ENEMY_ALL:           ///< �G���S��
    BTL_POKESET_Add( rec, get_opponent_pokeparam(wk, atPos, 0) );
    BTL_POKESET_Add( rec, get_opponent_pokeparam(wk, atPos, 1) );
    return 2;
  case WAZA_TARGET_OTHER_ALL:           ///< �����ȊO�S��
    BTL_POKESET_Add( rec, get_next_pokeparam( wk, atPos ) );
    BTL_POKESET_Add( rec, get_opponent_pokeparam( wk, atPos, 0 ) );
    BTL_POKESET_Add( rec, get_opponent_pokeparam( wk, atPos, 1 ) );
    return 3;
  case WAZA_TARGET_ALL:                ///< �S��
    BTL_POKESET_Add( rec, attacker );
    BTL_POKESET_Add( rec, get_next_pokeparam( wk, atPos ) );
    BTL_POKESET_Add( rec, get_opponent_pokeparam( wk, atPos, 0 ) );
    BTL_POKESET_Add( rec, get_opponent_pokeparam( wk, atPos, 1 ) );
    return 3;

  case WAZA_TARGET_USER:      ///< �����P�̂̂�
    BTL_POKESET_Add( rec, BTL_POKECON_GetFrontPokeData(wk->pokeCon, atPos) );
    return 1;
  case WAZA_TARGET_FRIEND_USER_SELECT:  ///< �������܂ޖ����P�́i�I���j
    BTL_POKESET_Add( rec, BTL_POKECON_GetFrontPokeData(wk->pokeCon, targetPos) );
    return 1;
  case WAZA_TARGET_FRIEND_SELECT:       ///< �����ȊO�̖����P��
    BTL_POKESET_Add( rec, get_next_pokeparam(wk, atPos) );
    return 1;

  case WAZA_TARGET_UNKNOWN:
    {
      // @@@ �R�R�͊��荞�݂̃^�[�Q�b�g�C�x���g�Ƃ͏�����ʂ���K�v������Ǝv���B��������B
      u8 pokeID = scEvent_GetWazaTargetIntr( wk, attacker, wazaParam );
      if( pokeID != BTL_POKEID_NULL ){
        bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, pokeID );
      }
    }
    break;

  default:
    return 0;
  }

  // �Ώۂ��G�܂ނP�̂̃��U
  if( bpp )
  {
    u8 atkPokeID, targetPokeID;
    if( intrPokeID != BTL_POKEID_NULL ){
      bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, intrPokeID );
    }
    BTL_POKESET_Add( rec, bpp );
    return 1;
  }
  else
  {
    return 0;
  }
}
/**
 *  ���U�Ώۊi�[�F�g���v���p
 */
static u8 registerTarget_triple( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BtlPokePos targetPos,
  const SVFL_WAZAPARAM* wazaParam, u8 intrPokeID, BTL_POKESET* rec )
{
  WazaTarget  targetType = WAZADATA_GetParam( wazaParam->wazaID, WAZAPARAM_TARGET );
  BtlPokePos  atPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, BPP_GetID(attacker) );
  const BTL_TRIPLE_ATTACK_AREA* area = BTL_MAINUTIL_GetTripleAttackArea( atPos );
  u32 i, cnt;

  BTL_POKEPARAM* bpp = NULL;

  switch( targetType ){
  case WAZA_TARGET_OTHER_SELECT:        ///< �����ȊO�̂P�́i�I���j
    bpp = BTL_POKECON_GetFrontPokeData( wk->pokeCon, targetPos );
    break;
  case WAZA_TARGET_ENEMY_SELECT:        ///< �G�P�́i�I���j
    bpp = BTL_POKECON_GetFrontPokeData( wk->pokeCon, targetPos );
    break;
  case WAZA_TARGET_ENEMY_RANDOM:        ///< �G�����_���ɂP��
    {
      u8 r = BTL_CALC_GetRand( area->numEnemys );
      bpp = BTL_POKECON_GetFrontPokeData( wk->pokeCon, area->enemyPos[r] );
    }
    break;

  case WAZA_TARGET_ENEMY_ALL:           ///< �G���S��
    for(i=0; i<area->numEnemys; ++i){
      BTL_POKESET_Add( rec, BTL_POKECON_GetFrontPokeData( wk->pokeCon, area->enemyPos[i]) );
    }
    return area->numEnemys;

  case WAZA_TARGET_OTHER_ALL:           ///< �����ȊO�S��
    cnt = 0;
    for(i=0; i<area->numEnemys; ++i){
      BTL_POKESET_Add( rec, BTL_POKECON_GetFrontPokeData(wk->pokeCon, area->enemyPos[i]) );
      ++cnt;
    }
    for(i=0; i<area->numFriends; ++i){
      if( area->friendPos[i] == atPos ){ continue; }
      BTL_POKESET_Add( rec, BTL_POKECON_GetFrontPokeData(wk->pokeCon, area->friendPos[i]) );
      ++cnt;
    }
    return cnt;

  case WAZA_TARGET_ALL:                ///< �S��
    cnt = 0;
    for(i=0; i<area->numEnemys; ++i){
      BTL_POKESET_Add( rec, BTL_POKECON_GetFrontPokeData(wk->pokeCon, area->enemyPos[i]) );
      ++cnt;
    }
    for(i=0; i<area->numFriends; ++i){
      BTL_POKESET_Add( rec, BTL_POKECON_GetFrontPokeData(wk->pokeCon, area->friendPos[i]) );
      ++cnt;
    }
    return cnt;

  case WAZA_TARGET_USER:      ///< �����P�̂̂�
    BTL_POKESET_Add( rec, BTL_POKECON_GetFrontPokeData(wk->pokeCon, atPos) );
    return 1;
  case WAZA_TARGET_FRIEND_USER_SELECT:  ///< �������܂ޖ����P�́i�I���j
    BTL_POKESET_Add( rec, BTL_POKECON_GetFrontPokeData(wk->pokeCon, targetPos) );
    return 1;
  case WAZA_TARGET_FRIEND_SELECT:       ///< �����ȊO�̖����P��
    BTL_POKESET_Add( rec, BTL_POKECON_GetFrontPokeData(wk->pokeCon, targetPos) );
    return 1;

  case WAZA_TARGET_UNKNOWN:
    {
      // @@@ �R�R�͊��荞�݂̃^�[�Q�b�g�C�x���g�Ƃ͏�����ʂ���K�v������Ǝv���B��������B
      u8 pokeID = scEvent_GetWazaTargetIntr( wk, attacker, wazaParam );
      if( pokeID != BTL_POKEID_NULL ){
        bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, pokeID );
      }
    }
    break;

  default:
    return 0;
  }

  if( bpp )
  {
    if( intrPokeID != BTL_POKEID_NULL ){
      bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, intrPokeID );
    }
    BTL_POKESET_Add( rec, bpp );
    return 1;
  }
  else
  {
    return 0;
  }
}
// �w��ʒu���猩�đΐ푊��̃|�P�����f�[�^��Ԃ�
static BTL_POKEPARAM* get_opponent_pokeparam( BTL_SVFLOW_WORK* wk, BtlPokePos pos, u8 pokeSideIdx )
{
  SVCL_WORK* clwk;
  BtlPokePos targetPos;
  u8 clientID, posIdx;

  targetPos = BTL_MAIN_GetOpponentPokePos( wk->mainModule, pos, pokeSideIdx );
  BTL_MAIN_BtlPosToClientID_and_PosIdx( wk->mainModule, targetPos, &clientID, &posIdx );

  clwk = BTL_SERVER_GetClientWork( wk->server, clientID );

  return BTL_PARTY_GetMemberData( clwk->party, posIdx );
}
// �w��ʒu���猩�ėׂ̃|�P�����f�[�^��Ԃ�
static BTL_POKEPARAM* get_next_pokeparam( BTL_SVFLOW_WORK* wk, BtlPokePos pos )
{
  SVCL_WORK* clwk;
  BtlPokePos nextPos;
  u8 clientID, posIdx;

  nextPos = BTL_MAIN_GetNextPokePos( wk->mainModule, pos );
  BTL_MAIN_BtlPosToClientID_and_PosIdx( wk->mainModule, nextPos, &clientID, &posIdx );
  clwk = BTL_SERVER_GetClientWork( wk->server, clientID );

  return BTL_PARTY_GetMemberData( clwk->party, posIdx );
}
/**
 *  �Ώۃ|�P����������ł������̑ΏۏC��
 *
 *  @retval   BOOL �␳�ł�����TRUE�^�␳�Ώی��ɂ��|�P���������Ȃ��ꍇFALSE
 */
static BOOL correctTargetDead( BTL_SVFLOW_WORK* wk, BtlRule rule, const BTL_POKEPARAM* attacker,
  const SVFL_WAZAPARAM* wazaParam, BtlPokePos targetPos, BTL_POKESET* rec )
{
  // �퓬�ʒu���w�c������Q�ȏ�i�_�u���A�g���v���A���[�e�j
  u8 numFrontPos = BTL_RULE_GetNumFrontPos( rule );
  if( numFrontPos > 1 )
  {
    // ���X�̑Ώۂ��P�̂ŁA
    if( BTL_POKESET_GetCount(rec) == 1 )
    {
      // �U���|�P�̓G�ŁA������ł���
      BTL_POKEPARAM* defaulTarget = BTL_POKESET_Get( rec, 0 );
      if( BPP_IsDead(defaulTarget)
      &&  !BTL_MAINUTIL_IsFriendPokeID( BPP_GetID(attacker), BPP_GetID(defaulTarget) )
      ){
        u8  nextTargetPos[ BTL_POSIDX_MAX ];
        BTL_POKEPARAM* nextTarget[ BTL_POSIDX_MAX ];
        BtlPokePos  atPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, BPP_GetID(attacker) );
        BtlExPos    exPos;
        u8 nextTargetCnt, aliveCnt, i;

        BTL_N_Printf( DBGSTR_SVFL_CorrectTarget_Info,
          BPP_GetID(attacker), atPos, BPP_GetID(defaulTarget), targetPos);

      // ���X�̑Ώۂ͎���ł�̂ŏ��O���Ă���
        BTL_POKESET_Remove( rec, defaulTarget );

      // ���X�̑Ώ۔͈͂��
        if( WAZADATA_GetFlag(wazaParam->wazaID, WAZAFLAG_TripleFar) ){
          exPos = EXPOS_MAKE( BTL_EXPOS_FULL_ENEMY, atPos );
          BTL_N_Printf( DBGSTR_SVFL_CorrectHitFarOn, wazaParam->wazaID);
        }else{
          exPos = EXPOS_MAKE( BTL_EXPOS_AREA_ENEMY, atPos );
          BTL_N_Printf( DBGSTR_SVFL_CorrectHitFarOff, wazaParam->wazaID);
        }
        nextTargetCnt = BTL_MAIN_ExpandBtlPos( wk->mainModule, exPos, nextTargetPos );

      // ���X�̑Ώ۔͈͂��琶���Ă�|�P���
        for(i=0; i<NELEMS(nextTarget); ++i){
          nextTarget[i] = NULL;
        }
        aliveCnt = 0;
        for(i=0; i<nextTargetCnt; ++i)
        {
          nextTarget[aliveCnt] = BTL_POKECON_GetFrontPokeData( wk->pokeCon, nextTargetPos[i] );
          if( !BPP_IsDead(nextTarget[aliveCnt]) ){
            nextTargetPos[ aliveCnt ] = nextTargetPos[i]; // �ʒu����Index�𑵂��Ă���
            ++aliveCnt;
          }
        }

        BTL_N_Printf( DBGSTR_SVFL_CorrectTargetNum, aliveCnt, nextTargetCnt );
        if( aliveCnt == 0 ){
          return FALSE;
        }

        // �Ώۂ��Q�̂���Ȃ���΂P�̂̃n�Y�Ȃ̂ŁA��⃊�X�g�̐擪��_��
        if( aliveCnt != 2 ){
          i = 0;
        }
        // ��₪�Q�̂̏ꍇ�i�g���v���̏ꍇ�̂݋N���肤��j
        else
        {
          // ��{�́A���X�̑Ώۂɋ߂�����_��
          int diff_0 = BTL_CALC_ABS( (int)(nextTargetPos[0]) - (int)targetPos );
          int diff_1 = BTL_CALC_ABS( (int)(nextTargetPos[1]) - (int)targetPos );

          BTL_N_Printf( DBGSTR_SVFL_CorrectTargetDiff,
            BPP_GetID(nextTarget[0]), diff_0, BPP_GetID(nextTarget[1]), diff_1);

          // ���X�̑ΏۂƂ̋����������ꍇ�i�^�񒆂�_�����ꍇ�̂݋N���肤��j�AHP�̏��Ȃ�����_��
          if( diff_0 == diff_1 )
          {
            diff_0 = BPP_GetValue( nextTarget[0], BPP_HP );
            diff_1 = BPP_GetValue( nextTarget[1], BPP_HP );
            BTL_N_Printf( DBGSTR_SVFL_CorrectTargetHP,
              BPP_GetID(nextTarget[0]), diff_0, BPP_GetID(nextTarget[1]), diff_1);

          }

          if( diff_0 < diff_1 ){
            i = 0;
          }else if( diff_1 < diff_0 ){
            i = 1;
          }else{
            // HP�������Ȃ烉���_��
            i = BTL_CALC_GetRand( 2 );
          }
        }

        if( nextTarget[i] != NULL )
        {
          BTL_N_Printf( DBGSTR_SVFL_CorrectResult,
            nextTargetPos[i], BPP_GetID(nextTarget[i]) );
          BTL_POKESET_Add( rec, nextTarget[ i ] );
          return TRUE;
        }
      }
    }
  }
  return FALSE;
}


//----------------------------------------------------------------------------------
/**
 * �u���������v-> ���U�o�������ȍ~�̏���
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scproc_Fight_WazaExe( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza, BTL_POKESET* targetRec )
{
  WazaCategory  category = WAZADATA_GetCategory( waza );
  u8 pokeID = BPP_GetID( attacker );
  u16  que_reserve_pos;

  BTL_Printf("�|�P[%d], waza=%d�̃��U�o�������J�n\n", BPP_GetID(attacker), waza );

  // �P�^�[�����߃��U�̔����`�F�b�N
  if( scproc_Fight_TameWazaExe(wk, attacker, targetRec, waza) ){
    return;
  }

  // �_���[�W�󂯃|�P�������[�N���N���A���Ă���
  BTL_POKESET_Clear( &wk->pokesetDamaged );

  // ���U�G�t�F�N�g�R�}���h�����p�Ƀo�b�t�@�̈��\�񂵂Ă���
  que_reserve_pos = SCQUE_RESERVE_Pos( wk->que, SC_ACT_WAZA_EFFECT );

  // ���U�o�������J�n�C�x���g
  {
    u32 hem_state = Hem_PushState( &wk->HEManager );
    BOOL fQuit = scEvent_WazaExecuteStart( wk, attacker, waza, targetRec );
    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    Hem_PopState( &wk->HEManager, hem_state );
    if( fQuit ){
      wazaEffCtrl_SetEnable( &wk->wazaEffCtrl );
      return;
    }
  }

  // ����ł�|�P�����͏��O
  BTL_POKESET_RemoveDeadPoke( targetRec );
  // �ŏ��͋����^�[�Q�b�g���c���Ă��Ȃ�->���܂����܂�Ȃ������A�I��
  if( BTL_POKESET_IsRemovedAll(targetRec) ){
    scPut_WazaFail( wk, attacker, waza );
    return;
  }

  // �Ώۂ��Ƃ̖����`�F�b�N������`�F�b�N->�����\���͂��̐�ɔC����
  flowsub_checkNotEffect( wk, &wk->wazaParam, attacker, targetRec );
  if( category != WAZADATA_CATEGORY_ICHIGEKI ){
    flowsub_checkWazaAvoid( wk, &wk->wazaParam, attacker, targetRec );
  }
  // �ŏ��͋����^�[�Q�b�g���c���Ă��Ȃ� -> �����C�x���g�Ăяo����I��
  if( BTL_POKESET_IsRemovedAll(targetRec) ){
    scproc_WazaExe_NotEffective( wk, pokeID, waza );
    return;
  }

  wazaEffCtrl_Init( &wk->wazaEffCtrl, wk, attacker, targetRec );
  BTL_Printf("���U=%d, �J�e�S��=%d\n", wk->wazaParam.wazaID, category );
  switch( category ){
  case WAZADATA_CATEGORY_SIMPLE_DAMAGE:
  case WAZADATA_CATEGORY_DAMAGE_EFFECT_USER:
  case WAZADATA_CATEGORY_DAMAGE_EFFECT:
  case WAZADATA_CATEGORY_DAMAGE_SICK:
  case WAZADATA_CATEGORY_DRAIN:
    scproc_Fight_Damage_Root( wk, &wk->wazaParam, attacker, targetRec );
    break;
  case WAZADATA_CATEGORY_SIMPLE_EFFECT:
    scproc_Migawari_CheckNoEffect( wk, &wk->wazaParam, attacker, targetRec );
    scproc_Fight_SimpleEffect( wk, &wk->wazaParam, attacker, targetRec );
    break;
  case WAZADATA_CATEGORY_SIMPLE_SICK:
    scproc_Migawari_CheckNoEffect( wk, &wk->wazaParam, attacker, targetRec );
    scproc_Fight_SimpleSick( wk, waza, attacker, targetRec );
    break;
  case WAZADATA_CATEGORY_EFFECT_SICK:
    scproc_Migawari_CheckNoEffect( wk, &wk->wazaParam, attacker, targetRec );
    scproc_Fight_EffectSick( wk, &wk->wazaParam, attacker, targetRec );
    break;
  case WAZADATA_CATEGORY_ICHIGEKI:
    scproc_Fight_Ichigeki( wk, &wk->wazaParam, attacker, targetRec );
    break;
  case WAZADATA_CATEGORY_PUSHOUT:
    scproc_Fight_PushOut( wk, waza, attacker, targetRec );
    break;
  case WAZADATA_CATEGORY_SIMPLE_RECOVER:
    scproc_Fight_SimpleRecover( wk, waza, attacker, targetRec );
    break;
  case WAZADATA_CATEGORY_FIELD_EFFECT:
    scput_Fight_FieldEffect( wk, &wk->wazaParam, attacker );
    break;
  case WAZADATA_CATEGORY_SIDE_EFFECT:
  case WAZADATA_CATEGORY_OTHERS:
    scput_Fight_Uncategory( wk, &wk->wazaParam, attacker, targetRec );
    break;
  default:
    SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_WazaFail );
    break;
  }

  // ���U���ʂ��聨�G�t�F�N�g�R�}���h�����Ȃǂ�
  if( wazaEffCtrl_IsEnable(&wk->wazaEffCtrl) ){
    scPut_WazaEffect( wk, waza, &wk->wazaEffCtrl, que_reserve_pos );
    BTL_WAZAREC_SetEffectiveLast( &wk->wazaRec );

    // �����œ����Ȃ��Ȃ鏈��
    if( !BPP_IsDead(attacker) ){
      if( WAZADATA_GetFlag(waza, WAZAFLAG_Tire) ){
        scPut_SetContFlag( wk, attacker, BPP_CONTFLG_CANT_ACTION );
      }
    }

    scproc_WazaExe_Effective( wk, pokeID, waza );

  }else{
    scproc_WazaExe_NotEffective( wk, pokeID, waza );
  }

  scproc_WazaExe_Done( wk, pokeID, waza );
  scproc_CheckDeadCmd( wk, attacker );
}

//----------------------------------------------------------------------------------
/**
 * ���U�o����->�L�����������̃C�x���g�Ăяo��
 *
 * @param   wk
 * @param   pokeID
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scproc_WazaExe_Effective( BTL_SVFLOW_WORK* wk, u8 pokeID, WazaID waza )
{
  u32 hem_state = Hem_PushState( &wk->HEManager );
  scEvent_WazaExeEnd_Common( wk, pokeID, waza, BTL_EVENT_WAZA_EXECUTE_EFFECTIVE );
  scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
  Hem_PopState( &wk->HEManager, hem_state );
}
//----------------------------------------------------------------------------------
/**
 * ���U�o����->�������������̃C�x���g�Ăяo��
 *
 * @param   wk
 * @param   pokeID
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scproc_WazaExe_NotEffective( BTL_SVFLOW_WORK* wk, u8 pokeID, WazaID waza )
{
  u32 hem_state = Hem_PushState( &wk->HEManager );
  scEvent_WazaExeEnd_Common( wk, pokeID, waza, BTL_EVENT_WAZA_EXECUTE_NO_EFFECT );
  scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
  Hem_PopState( &wk->HEManager, hem_state );
}
//----------------------------------------------------------------------------------
/**
 * ���U�o���ŏI�C�x���g�Ăяo��
 *
 * @param   wk
 * @param   pokeID
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scproc_WazaExe_Done( BTL_SVFLOW_WORK* wk, u8 pokeID, WazaID waza )
{
  u32 hem_state = Hem_PushState( &wk->HEManager );
  scEvent_WazaExeEnd_Common( wk, pokeID, waza, BTL_EVENT_WAZA_EXECUTE_DONE );
  scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
  Hem_PopState( &wk->HEManager, hem_state );
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U�o���I������
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scEvent_WazaExeEnd_Common( BTL_SVFLOW_WORK* wk, u8 pokeID, WazaID waza, BtlEventType eventID )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, pokeID );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENT_CallHandlers( wk, eventID );
  BTL_EVENTVAR_Pop();
}


//----------------------------------------------------------------------------------
/**
 * �K����Ԃ��ǂ����`�F�b�N
 *
 * @param   attacker
 * @param   target
 *
 * @retval  BOOL    �K����ԂȂ�TRUE
 */
//----------------------------------------------------------------------------------
static BOOL IsMustHit( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target )
{
  if( BPP_CheckSick(attacker, WAZASICK_MUSTHIT) ){
    return TRUE;
  }
  if( BPP_CheckSick(attacker, WAZASICK_MUSTHIT_TARGET) )
  {
    u8 targetPokeID = BPP_GetSickParam( attacker, WAZASICK_MUSTHIT_TARGET );
    if( targetPokeID == BPP_GetID(target) ){
      return TRUE;
    }
  }

  return FALSE;
}

//--------------------------------------------------------------------------
/**
 * ���U�����O�́A�Ώەʖ������`�F�b�N
 *
 * @param   wk
 * @param   attacker  [in] ���U���o���|�P����
 * @param   targets   [io] ���U�Ώۃ|�P�����R���e�i�^�����ɂȂ����|�P�����̓R���e�i����폜�����
 *
 */
//--------------------------------------------------------------------------
static void flowsub_checkNotEffect( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, const BTL_POKEPARAM* attacker, BTL_POKESET* targets )
{
  BTL_POKEPARAM* bpp;
  u32 hem_state;

  // Lv1 �������`�F�b�N�i�U���|�P���K����ԂȂ疳�����ł��Ȃ��j
  BTL_POKESET_SeekStart( targets );
  while( (bpp = BTL_POKESET_SeekNext(targets)) != NULL )
  {
    if( !IsMustHit(attacker, bpp) )
    {
      hem_state = Hem_PushState( &wk->HEManager );
      if( scEvent_CheckNotEffect(wk, wazaParam->wazaID, 0, attacker, bpp, &wk->strParam) )
      {
        BTL_POKESET_Remove( targets, bpp );

        // �Ƃ肠�����������o�������̃��A�N�V����
        if( HANDEX_STR_IsEnable(&wk->strParam) ){
          handexSub_putString( wk, &wk->strParam );
          HANDEX_STR_Clear( &wk->strParam );
        }else{
          // HandEx�𗘗p�������A�N�V����
          if( !scproc_HandEx_Root( wk, ITEM_DUMMY_DATA ) ){
            // �������A�N�V�����Ȃ���΁u���ʂ��Ȃ��悤���c�v
            SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_NoEffect, BPP_GetID(bpp) );
          }
        }
      }
      Hem_PopState( &wk->HEManager, hem_state );
    }
  }

  // Lv2 �������`�F�b�N�i�U���|�P���K����Ԃł��������j
  BTL_POKESET_SeekStart( targets );
  while( (bpp = BTL_POKESET_SeekNext(targets)) != NULL )
  {
    hem_state = Hem_PushState( &wk->HEManager );
    if( scEvent_CheckNotEffect(wk, wazaParam->wazaID, 1, attacker, bpp, &wk->strParam) )
    {
      BTL_POKESET_Remove( targets, bpp );

      // �Ƃ肠�����������o�������̃��A�N�V����
      if( HANDEX_STR_IsEnable(&wk->strParam) ){
        handexSub_putString( wk, &wk->strParam );
        HANDEX_STR_Clear( &wk->strParam );
      }else{
        // HandEx�𗘗p�������A�N�V����
        if( !scproc_HandEx_Root( wk, ITEM_DUMMY_DATA ) ){
          // �������A�N�V�����Ȃ���΁u���ʂ��Ȃ��悤���c�v
          SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_NoEffect, BPP_GetID(bpp) );
        }
      }
    }
    Hem_PopState( &wk->HEManager, hem_state );
  }

  // �܂���`�F�b�N
  if( WAZADATA_GetFlag(wazaParam->wazaID, WAZAFLAG_Mamoru) )
  {
    BTL_POKESET_SeekStart( targets );
    while( (bpp = BTL_POKESET_SeekNext(targets)) != NULL )
    {
      if( BPP_TURNFLAG_Get(bpp, BPP_TURNFLG_MAMORU) )
      {
        // �܂��閳��������Ȃ���΃^�[�Q�b�g���珜�O
        if( !scEvent_CheckMamoruBreak(wk, attacker, bpp, wazaParam->wazaID) ){
          BTL_POKESET_Remove( targets, bpp );
          SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_Mamoru, BPP_GetID(bpp) );
        }
      }
    }
  }
}
//--------------------------------------------------------------------------
/**
 * ���U�������� -> �������Ȃ������ꍇ�� targets ���珜�O
 *
 * @param   wk
 * @param   attacker  [in] ���U���o���|�P����
 * @param   targets   [io] ���U�Ώۃ|�P�����R���e�i�^�����ɂȂ����|�P�����̓R���e�i����폜�����
 *
 */
//--------------------------------------------------------------------------
static void flowsub_checkWazaAvoid( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, const BTL_POKEPARAM* attacker, BTL_POKESET* targets )
{
  BTL_POKEPARAM* bpp;
  u8 pokeID[ BTL_POS_MAX ];
  u8 count = 0;

  // �Ώۂ��ŏ����烏�U��ł����|�P�����P�̂̃��U�͖������Ȃǂɂ�锻����s��Ȃ�
  if( (BTL_POKESET_GetCountMax(targets) == 1)
  &&  (BTL_POKESET_Get(targets,0) == attacker)
  ){
    return;
  }

  BTL_POKESET_SeekStart( targets );
  while( (bpp = BTL_POKESET_SeekNext(targets)) != NULL )
  {
    if( IsTripleFarPos(wk, attacker, bpp, wazaParam->wazaID)
    ||  !scEvent_CheckHit(wk, attacker, bpp, wazaParam)
    ){
      pokeID[count++] = BPP_GetID( bpp );
      BTL_POKESET_Remove( targets, bpp );
      scPut_WazaAvoid( wk, bpp, wazaParam->wazaID );
    }
  }
  if( count )
  {
    u32 hem_state = Hem_PushState( &wk->HEManager );
    scEvent_WazaAvoid( wk, attacker, wazaParam->wazaID, count, pokeID );
    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    Hem_PopState( &wk->HEManager, hem_state );
  }
}
//----------------------------------------------------------------------------------
/**
 * �g���v�����A������Ȃ��ʒu�̃|�P�������ǂ����𔻒�
 *
 * @param   wk
 * @param   attacker    �U������|�P
 * @param   target      �U�������|�P
 * @param   waza        ���UID
 *
 * @retval  BOOL        �g���v���œ�����Ȃ��ʒu�Ȃ�TRUE�^����ȊOFALSE
 */
//----------------------------------------------------------------------------------
static BOOL IsTripleFarPos( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, WazaID waza )
{
  if( (BTL_MAIN_GetRule(wk->mainModule) == BTL_RULE_TRIPLE)
  &&  (WAZADATA_GetFlag(waza, WAZAFLAG_TripleFar) == FALSE)
  ){
    BtlPokePos  atkPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, BPP_GetID(attacker) );
    BtlPokePos  tgtPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, BPP_GetID(target) );

    if( !BTL_MAINUTIL_CheckTripleHitArea(atkPos, tgtPos) ){
      return TRUE;
    }
  }
  return FALSE;
}


//--------------------------------------------------------------------------
/**
 * [Event] �o�������U���Ώۂɓ����邩����i�ꌂ�K�E�ȊO�̃|�P�����Ώۃ��U�j
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   waza
 *
 * @retval  BOOL
 */
//--------------------------------------------------------------------------
static BOOL scEvent_CheckHit( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender,
  const SVFL_WAZAPARAM* wazaParam )
{
  if( IsMustHit(attacker, defender) ){
    return TRUE;
  }

  if( scEvent_CheckPokeHideAvoid(wk, attacker, defender, wazaParam->wazaID) ){
    return FALSE;
  }

  if( scEvent_IsExcuseCalcHit(wk, attacker, defender, wazaParam->wazaID) ){
    return TRUE;
  }

  if( BPP_CheckSick(defender, WAZASICK_TELEKINESIS) ){
    return TRUE;
  }

  {
    u8 wazaHitPer;
    s8 hitRank, avoidRank, totalRank;
    u32 totalPer;
    fx32 ratio;

    wazaHitPer = scEvent_getHitPer(wk, attacker, defender, wazaParam);

    BTL_EVENTVAR_Push();
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
      BTL_EVENTVAR_SetValue( BTL_EVAR_HIT_RANK, BPP_GetValue(attacker, BPP_HIT_RATIO) );
      BTL_EVENTVAR_SetValue( BTL_EVAR_AVOID_RANK, BPP_GetValue(defender, BPP_AVOID_RATIO) );
      BTL_EVENTVAR_SetMulValue( BTL_EVAR_RATIO, FX32_CONST(1), FX32_CONST(0.01f), FX32_CONST(32) );
      BTL_SICKEVENT_CheckHitRatio( wk, attacker, defender );
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_HIT_RANK );

      hitRank = BTL_EVENTVAR_GetValue( BTL_EVAR_HIT_RANK );
      avoidRank = BTL_EVENTVAR_GetValue( BTL_EVAR_AVOID_RANK );
      if( BPP_CheckSick(defender, WAZASICK_MIYABURU) )
      {
        if( avoidRank > BTL_CALC_HITRATIO_MID ){
          avoidRank = BTL_CALC_HITRATIO_MID;
        }
      }
      ratio = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );

    BTL_EVENTVAR_Pop();

    totalRank = roundValue( (int)(BTL_CALC_HITRATIO_MID + hitRank - avoidRank), BTL_CALC_HITRATIO_MIN, BTL_CALC_HITRATIO_MAX );
    totalPer  = BTL_CALC_HitPer( wazaHitPer, totalRank );

    BTL_N_Printf( DBGSTR_SVFL_HitCheckInfo1, BPP_GetID(attacker), hitRank, wazaHitPer );
    BTL_N_Printf( DBGSTR_SVFL_HitCheckInfo2, BPP_GetID(defender), avoidRank );
    BTL_N_Printf( DBGSTR_SVFL_HitCheckInfo3, totalRank, totalPer, ratio );

    totalPer = BTL_CALC_MulRatio( totalPer, ratio );
    if( totalPer > 100 ){
      totalPer = 100;
    }
    if( BTL_MAIN_GetDebugFlag(wk->mainModule, BTL_DEBUGFLAG_HIT100PER) ){
      totalPer = 100;
    }
    BTL_N_Printf( DBGSTR_SVFL_HitCheckInfo4, totalPer);

    return perOccur( wk, totalPer );
  }
}
//----------------------------------------------------------------------------------
/**
 * ���U���ʂ��邱�Ƃ��m�聨�K�v�ȉ��o�R�}���h�̐���
 *
 * @param   wk
 * @param   waza
 * @param   defPokeID
 * @param   que_reserve_pos
 */
//----------------------------------------------------------------------------------
static void scPut_WazaEffect( BTL_SVFLOW_WORK* wk, WazaID waza, const WAZAEFF_CTRL* effCtrl, u32 que_reserve_pos )
{
  SCQUE_PUT_ReservedPos( wk->que, que_reserve_pos, SC_ACT_WAZA_EFFECT,
          effCtrl->attackerPos, effCtrl->targetPos, waza );
}
//----------------------------------------------------------------------------------
/**
 * �K�v�Ȃ�A�P�^�[�����߃��U�̗��߃^�[���������s���B
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 *
 * @retval  BOOL      ���߃^�[���������s�����ꍇTRUE
 */
//----------------------------------------------------------------------------------
static BOOL scproc_Fight_TameWazaExe( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, const BTL_POKESET* targetRec, WazaID waza )
{
  if( WAZADATA_GetFlag(waza, WAZAFLAG_Tame) )
  {
    if( !BPP_CheckSick(attacker, WAZASICK_TAMELOCK) )
    {
      BtlPokePos  atPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, BPP_GetID(attacker) );

      // ���߃^�[���X�L�b�v����
      if( !scEvent_CheckTameTurnSkip(wk, attacker, waza) )
      {
        BPP_SICK_CONT  sickCont = BPP_SICKCONT_MakeTurnParam( 2, waza );
        scPut_AddSick( wk, attacker, WAZASICK_TAMELOCK, sickCont );
        SCQUE_PUT_ACT_WazaEffectEx( wk->que, atPos, BTL_POS_NULL, waza, BTLV_WAZAEFF_TURN_TAME );
        {
          u32 hem_state = Hem_PushState( &wk->HEManager );
          scEvent_TameStart( wk, attacker, targetRec, waza );
          scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
          Hem_PopState( &wk->HEManager, hem_state );
        }
        // �X�L�b�v�ł��Ȃ����͂�����return
        return TRUE;
      }

      SCQUE_PUT_ACT_WazaEffectEx( wk->que, atPos, BTL_POS_NULL, waza, BTLV_WAZAEFF_TURN_TAME );
      {
        u32 hem_state = Hem_PushState( &wk->HEManager );
        scEvent_TameSkip( wk, attacker, waza );
        scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
        Hem_PopState( &wk->HEManager, hem_state );
      }
    }
    // @TODO ���߃����[�X�͂����ƑO�̃^�C�~���O����Ȃ��ƃ}�Y�������H
    // @TODO �t���[�t�H�[���̉�����s���Ȃ��ƁB

    scPut_CureSick( wk, attacker, WAZASICK_TAMELOCK, NULL );
    {
      u32 hem_state = Hem_PushState( &wk->HEManager );
      scEvent_TameRelease( wk, attacker, targetRec, waza );
      scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
      Hem_PopState( &wk->HEManager, hem_state );
    }
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * ���U�o�����s�`�F�b�N
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 *
 * @retval  SV_WazaFailCause    ���s�����ꍇ���s�R�[�h�^�����̏ꍇ SV_WAZAFAIL_NULL
 */
//----------------------------------------------------------------------------------
static BOOL scproc_Fight_CheckWazaExecuteFail_1st( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza )
{
  SV_WazaFailCause  cause = SV_WAZAFAIL_NULL;
  PokeSick sick;

  do {

    // �����񎩔�����
    if( scproc_Fight_CheckConf(wk, attacker) ){
      cause = SV_WAZAFAIL_KONRAN;
      break;
    }
    // ������������
    if( scproc_Fight_CheckMeroMero(wk, attacker) ){
      cause = SV_WAZAFAIL_MEROMERO;
      break;
    }
    // ���Ȃ��΂蔻��
    if( BPP_CheckSick(attacker, WAZASICK_KANASIBARI) ){
      BPP_SICK_CONT  sickCont = BPP_GetSickCont( attacker, WAZASICK_KANASIBARI );
      if( BPP_SICKCONT_GetParam(sickCont) == waza ){
        cause = SV_WAZAFAIL_KANASIBARI;
        break;
      }
    }

    // �˂ނ�E�����蓙�̉����`�F�b�N
    scproc_PokeSickCure_WazaCheck( wk, attacker, waza );

    // �|�P�����n ��Ԉُ�ɂ�鎸�s�`�F�b�N
    sick = BPP_GetPokeSick( attacker );
    switch( sick ){
    case POKESICK_NEMURI:
      cause = SV_WAZAFAIL_NEMURI;
      break;
    case POKESICK_MAHI:
      if( BTL_CALC_IsOccurPer(BTL_MAHI_EXE_PER) ){
       cause = SV_WAZAFAIL_MAHI;
      }
      break;
    case POKESICK_KOORI:
      cause = SV_WAZAFAIL_KOORI;
      break;
    }

    // ���̑��̎��s�`�F�b�N
    cause = scEvent_CheckWazaExecute1ST( wk, attacker, waza, cause );
    if( cause != SV_WAZAFAIL_NULL ){  break; }

    // �Ђ�݂ɂ�鎸�s�`�F�b�N
    if( BPP_TURNFLAG_Get(attacker, BPP_TURNFLG_SHRINK) ){
      cause = SV_WAZAFAIL_SHRINK;
      break;
    }


  }while( 0 );

  if( cause != SV_WAZAFAIL_NULL ){
    BTL_N_Printf( DBGSTR_SVFL_WazaExeFail_1, BPP_GetID(attacker), cause);
    scproc_WazaExecuteFailed( wk, attacker, waza, cause );
    return TRUE;
  }
  return FALSE;
}

static BOOL scproc_Fight_CheckWazaExecuteFail_2nd( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza )
{
  SV_WazaFailCause  cause = SV_WAZAFAIL_NULL;

  do {
    // PP���[��
    {
      u8 wazaIdx = BPP_WAZA_SearchIdx( attacker, waza );
      if( (wazaIdx != PTL_WAZA_MAX)
      &&  (BPP_WAZA_GetPP(attacker, wazaIdx) == 0)
      ){
        cause = SV_WAZAFAIL_PP_ZERO;
        break;
      }
    }

    // �����ӂ��ӂ����`�F�b�N
    if( BPP_CheckSick(attacker, WAZASICK_KAIHUKUHUUJI)
    &&  (WAZADATA_GetCategory(waza) == WAZADATA_CATEGORY_SIMPLE_RECOVER)
    ){
      cause = SV_WAZAFAIL_KAIHUKUHUUJI;
      break;
    }

    // ���イ��傭�`�F�b�N
    if( BTL_FIELD_CheckEffect(BTL_FLDEFF_JURYOKU)
    &&  (WAZADATA_GetFlag(waza, WAZAFLAG_Flying))
    ){
      cause = SV_WAZAFAIL_JURYOKU;
      break;
    }

    // ���̑��̎��s�`�F�b�N
    cause = scEvent_CheckWazaExecute2ND( wk, attacker, waza );

  }while( 0 );

  if( cause != SV_WAZAFAIL_NULL ){
    BTL_N_Printf( DBGSTR_SVFL_WazaExeFail_2, BPP_GetID(attacker), cause);
    scproc_WazaExecuteFailed( wk, attacker, waza, cause );
    return TRUE;
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * �����񎩔��`�F�b�N������
 *
 * @param   wk
 * @param   attacker
 *
 * @retval  BOOL    ���������ꍇTRUE
 */
//----------------------------------------------------------------------------------
static BOOL scproc_Fight_CheckConf( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker )
{
  if( BPP_CheckSick(attacker, WAZASICK_KONRAN) )
  {
    if( BPP_CheckKonranWakeUp(attacker) ){
      scPut_CureSick( wk, attacker, WAZASICK_KONRAN, NULL );
      SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_KonranCure, BPP_GetID(attacker) );
      return FALSE;
    }
    scPut_ConfCheck( wk, attacker );
    return BTL_CALC_IsOccurPer( BTL_CONF_EXE_RATIO );
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * ���������ɂ�郏�U�������s����
 *
 * @param   wk
 * @param   attacker
 *
 * @retval  BOOL    ���s�����ꍇTRUE
 */
//----------------------------------------------------------------------------------
static BOOL scproc_Fight_CheckMeroMero( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker )
{
  if( BPP_CheckSick(attacker, WAZASICK_MEROMERO) )
  {
    scPut_MeromeroAct( wk, attacker );
    if( BTL_CALC_IsOccurPer(BTL_MEROMERO_EXE_PER) )
    {
      return TRUE;
    }
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * �˂ނ�E�����蓙�A�A�N�V�����I����ɉ����`�F�b�N���s������
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scproc_PokeSickCure_WazaCheck( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza )
{
  PokeSick sick = BPP_GetPokeSick( attacker );
  BOOL f_cured = FALSE;

  switch( sick ){
  case POKESICK_NEMURI:
    if( BPP_CheckNemuriWakeUp(attacker) ){
      f_cured = TRUE;
    }
    break;
  case POKESICK_KOORI:
    if( WAZADATA_GetFlag(waza, WAZAFLAG_KooriMelt)
    ||  BTL_CALC_IsOccurPer( BTL_KORI_MELT_PER )
    ){
      f_cured = TRUE;
    }
    break;
  }

  if( f_cured ){
    scPut_CurePokeSick( wk, attacker, sick );
  }
}
//----------------------------------------------------------------------------------
/**
 * [Proc] ���U�o�����s������
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 * @param   fail_cause
 */
//----------------------------------------------------------------------------------
static void scproc_WazaExecuteFailed( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza, SV_WazaFailCause fail_cause )
{
  // ������Ȃ玩���_���[�W�`�F�b�N
  if( fail_cause == SV_WAZAFAIL_KONRAN )
  {
    u16 conf_dmg = scEvent_CalcConfDamage( wk, attacker );
    scPut_ConfDamage( wk, attacker, conf_dmg );
    if( scproc_CheckDeadCmd(wk, attacker) ){
      if( scproc_CheckShowdown(wk) ){
        return;
      }
    }
  }else{
    switch( fail_cause ){
    case SV_WAZAFAIL_NEMURI:     scPut_EffectByPokePos( wk, attacker, BTLEFF_NEMURI ); break;
    case SV_WAZAFAIL_MAHI:       scPut_EffectByPokePos( wk, attacker, BTLEFF_MAHI ); break;
    case SV_WAZAFAIL_KOORI:      scPut_EffectByPokePos( wk, attacker, BTLEFF_KOORI ); break;
    case SV_WAZAFAIL_MEROMERO:   scPut_EffectByPokePos( wk, attacker, BTLEFF_MEROMERO ); break;
    }
    scPut_WazaExecuteFailMsg( wk, attacker, waza, fail_cause );
  }

  {
    u32 hem_state = Hem_PushState( &wk->HEManager );
    scEvent_CheckWazaExeFail( wk, attacker, waza, fail_cause );
    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    Hem_PopState( &wk->HEManager, hem_state );
  }
}
//--------------------------------------------------------------------------
/**
 * ���U�o�����s���b�Z�[�W�\��
 */
//--------------------------------------------------------------------------
static void scPut_WazaExecuteFailMsg( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, WazaID waza, SV_WazaFailCause cause )
{
  u8 pokeID = BPP_GetID( bpp );

  switch( cause ){
  case SV_WAZAFAIL_NEMURI:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_NemuriAct, pokeID );
    break;
  case SV_WAZAFAIL_MAHI:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_MahiAct, pokeID );
    break;
  case SV_WAZAFAIL_KOORI:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_KoriAct, pokeID );
    break;
  case SV_WAZAFAIL_SHRINK:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_ShrinkExe, pokeID );
    break;
  case SV_WAZAFAIL_MEROMERO:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_MeromeroExe, pokeID );
    break;
  case SV_WAZAFAIL_KANASIBARI:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_KanasibariWarn, pokeID, waza );
    break;
  case SV_WAZAFAIL_NAMAKE:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_Namake, pokeID );
    break;
  case SV_WAZAFAIL_TYOUHATSU:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_ChouhatuWarn, pokeID, waza );
    break;
  case SV_WAZAFAIL_ICHAMON:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_IchamonWarn, pokeID, waza );
    break;
  case SV_WAZAFAIL_KAIHUKUHUUJI:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_KaifukuFuji, pokeID, waza );
    break;
  case SV_WAZAFAIL_HPFULL:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_HPFull, pokeID );
    break;
  case SV_WAZAFAIL_FUMIN:
    SCQUE_PUT_TOKWIN_IN( wk->que, pokeID );
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_Fumin, pokeID );
    SCQUE_PUT_TOKWIN_OUT( wk->que, pokeID );
    break;
  case SV_WAZAFAIL_JURYOKU:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_JyuryokuWazaFail, pokeID, waza );
    break;
  default:
    SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_WazaFail, pokeID );
    break;
  }
}
//--------------------------------------------------------------------------
/**
 * [Proc] �g�������U��PP����
 *
 * @param   wk
 * @param   attacker  �U���|�P�����p�����[�^
 * @param   wazaIdx   �g�������U�C���f�b�N�X
 * @param   rec       �_���[�W���󂯂��|�P�����p�����[�^�Q
 *
 */
//--------------------------------------------------------------------------
static void scproc_decrementPPUsedWaza( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, u8 wazaIdx, BTL_POKESET* rec )
{
  u8 vol = scEvent_DecrementPPVolume( wk, attacker, rec );
  scproc_decrementPP( wk, attacker, wazaIdx, vol );
}
//----------------------------------------------------------------------------------
/**
 * [Proc] ���U��PP����
 *
 * @param   wk
 * @param   bpp       �Ώۃ|�P�����p�����[�^
 * @param   wazaIdx   ���������郏�U�C���f�b�N�X
 * @param   volume    ������
 *
 * @retval  BOOL      ������������TRUE�^���S���Ȃǌ����������Ȃ�������FALSE
 */
//----------------------------------------------------------------------------------
static BOOL scproc_decrementPP( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u8 wazaIdx, u8 volume )
{
  if( !BPP_IsDead(bpp) )
  {
    u8 restPP = BPP_WAZA_GetPP( bpp, wazaIdx );
    if( volume > restPP ){
      volume = restPP;
    }

    if( volume )
    {
      scPut_DecrementPP( wk, bpp, wazaIdx, volume );
      if( scEvent_DecrementPP_Reaction(wk, bpp, wazaIdx) ){
        scproc_UseItemEquip( wk, bpp );
      }
      return TRUE;
    }
  }
  return FALSE;
}
//----------------------------------------------------------------------
// �T�[�o�[�t���[�F�u���������v> �_���[�W���U�n
//----------------------------------------------------------------------
static void scproc_Fight_Damage_Root( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
   BTL_POKEPARAM* attacker, BTL_POKESET* targets )
{
  fx32  dmg_ratio;
  u8    target_cnt;

  FlowFlg_Clear( wk, FLOWFLG_SET_WAZAEFFECT );

  target_cnt = BTL_POKESET_GetCount( targets );
  dmg_ratio = (target_cnt == 1)? BTL_CALC_DMG_TARGET_RATIO_NONE : BTL_CALC_DMG_TARGET_RATIO_PLURAL;

// �����ɑ΂���_���[�W
  if( BTL_POKESET_CopyFriends(targets, attacker, &wk->pokesetSubTarget) )
  {
    scproc_Fight_Damage_side( wk, wazaParam, attacker, &wk->pokesetSubTarget, dmg_ratio );
  }
// �G�ɑ΂���_���[�W
  if( BTL_POKESET_CopyEnemys(targets, attacker, &wk->pokesetSubTarget) )
  {
    scproc_Fight_Damage_side( wk, wazaParam, attacker, &wk->pokesetSubTarget, dmg_ratio );
  }

  scproc_Fight_Damage_After( wk, wazaParam, attacker, &wk->pokesetDamaged );
}
static void scproc_Fight_Damage_ToRecover( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker,
  const SVFL_WAZAPARAM* wazaParam, BTL_POKESET* targets )
{
  BTL_POKEPARAM* bpp;

  // �񕜃`�F�b�N -> ����^�C�v�̃_���[�W�𖳌����A�񕜂��Ă��܂��|�P�� targets ���珜�O
  BTL_POKESET_SeekStart( targets );
  while( (bpp = BTL_POKESET_SeekNext(targets)) != NULL )
  {
    if( scEvent_CheckDmgToRecover(wk, attacker, bpp, wazaParam) )
    {
      u32 hem_state;

      wazaEffCtrl_SetEnable( &wk->wazaEffCtrl );

      hem_state = Hem_PushState( &wk->HEManager );
      scEvent_DmgToRecover( wk, attacker, bpp );
      scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );

      Hem_PopState( &wk->HEManager, hem_state );
      BTL_POKESET_Remove( targets, bpp );
    }
  }
}

static void scproc_Fight_Damage_side( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker,
  BTL_POKESET* targets, fx32 dmg_ratio )
{
  BtlTypeAff affAry[ BTL_POSIDX_MAX ];
  BTL_POKEPARAM* bpp;
  u8 poke_cnt;

//  scproc_Fight_Damage_CalcAff( wk, attacker, wazaParam, targets, affAry );
  scproc_Fight_Damage_ToRecover( wk, attacker, wazaParam, targets );

  poke_cnt = 0;
  BTL_POKESET_SeekStart( targets );
  while( (bpp = BTL_POKESET_SeekNext(targets) ) != NULL )
  {
    affAry[poke_cnt] = scProc_checkWazaDamageAffinity( wk, attacker, bpp, wazaParam );
    if( affAry[ poke_cnt ] == BTL_TYPEAFF_0 ){
      scput_WazaNoEffect( wk, bpp );
      BTL_POKESET_Remove( targets, bpp );
    }
    else{
      ++poke_cnt;
    }
  }

  BTL_Printf("�c��|�P��=%d\n", poke_cnt);

  if( poke_cnt == 1 )
  {
    bpp = BTL_POKESET_Get( targets, 0 );
    scproc_Fight_Damage_side_single( wk, attacker, bpp, wazaParam, dmg_ratio, affAry[0] );
  }
  else if( poke_cnt > 1 )
  {
    u8 f_same_aff;
    int i, j;

    f_same_aff = TRUE;

    // �����S��v�`�F�b�N
    if( f_same_aff )
    {
      for(i=0; i<poke_cnt; ++i)
      {
        bpp = BTL_POKESET_Get( targets, i );
        if( BPP_MIGAWARI_IsExist(bpp) ){    // �P�̂ł��݂���肪����ꍇ�͈�v�Ƃ݂Ȃ��Ȃ�
          f_same_aff = FALSE;
          break;
        }

        for(j=0; j<i; ++j)
        {
          if( BTL_CALC_TypeAffAbout(affAry[i]) != BTL_CALC_TypeAffAbout(affAry[i]) ){
            f_same_aff = FALSE;
            i = poke_cnt; // for loopOut
            break;
          }
        }
      }
    }
    // �������P�̂ł��ʂ̃��c������Ȃ�ʂ̏���
    if( !f_same_aff ){
      for(i=0; i<poke_cnt; ++i){
        bpp = BTL_POKESET_Get( targets, i );
        scproc_Fight_Damage_side_single( wk, attacker, bpp, wazaParam, dmg_ratio, affAry[i] );
      }
    }
    // �������S�Ẵ^�[�Q�b�g�ň�v�Ȃ�A�̗̓o�[�𓯎��Ɍ��炷�悤�ɃR�}���h��������
    else{
      scproc_Fight_damage_side_plural( wk, attacker, targets, affAry, wazaParam, dmg_ratio );
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * ���U�ɂ��_���[�W��^���邱�Ƃ��m�肵��
 *
 * @param   wk
 * @param   attacker
 * @param   wazaParam
 * @param   targets
 */
//----------------------------------------------------------------------------------
static void scproc_Fight_Damage_Determine( BTL_SVFLOW_WORK* wk,
  BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam )
{
  u32 hem_state = Hem_PushState( &wk->HEManager );

  scEvent_WazaDamageDetermine( wk, attacker, defender, wazaParam );
  scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );

  Hem_PopState( &wk->HEManager, hem_state );
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U�ɂ��_���[�W��^���邱�Ƃ��m�肵��
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   wazaParam
 */
//----------------------------------------------------------------------------------
static void scEvent_WazaDamageDetermine( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_DMG_DETERMINE );
  BTL_EVENTVAR_Pop();
}
//------------------------------------------------------------------
// �T�[�o�[�t���[�������F�P�̃_���[�W�����i��ʕ���j
//------------------------------------------------------------------
static void scproc_Fight_Damage_side_single( BTL_SVFLOW_WORK* wk,
      BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam,
      fx32 targetDmgRatio, BtlTypeAff affinity )
{
  svflowsub_damage_act_singular( wk, attacker, defender, wazaParam, affinity, targetDmgRatio );

  scproc_CheckDeadCmd( wk, attacker );
  scproc_CheckDeadCmd( wk, defender );
}
//------------------------------------------------------------------
// �T�[�o�[�t���[�������F�P�̃_���[�W�����i���ʁj
//------------------------------------------------------------------
static void svflowsub_damage_act_singular(  BTL_SVFLOW_WORK* wk,
    BTL_POKEPARAM* attacker,  BTL_POKEPARAM* defender,
    const SVFL_WAZAPARAM* wazaParam, BtlTypeAff affinity, fx32 targetDmgRatio )
{
  u8 fPluralHit = FALSE; // ������q�b�g�t���O
  u8 fDead = FALSE;

  u8 hit_count, fCritical, fFixDamage;
  u16 damage;
  u32 damage_sum;
  int i;

  fPluralHit = scEvent_CheckPluralHit( wk, attacker, wazaParam->wazaID, &hit_count );
  if( fPluralHit ){
    BTL_Printf("������q�b�g���U ... ��=%d\n", hit_count);
  }

  wazaEffCtrl_SetEnable( &wk->wazaEffCtrl );
  damage_sum = 0;

  for(i=0; i<hit_count; ++i)
  {
    fCritical = scEvent_CheckCritical( wk, attacker, defender, wazaParam->wazaID );
    fFixDamage = scEvent_CalcDamage( wk, attacker, defender, wazaParam,
                  affinity, targetDmgRatio, fCritical, &damage );

    if( fFixDamage ){
      affinity = BTL_TYPEAFF_100;
      fCritical = FALSE;
    }

    // �ŏ��̂P�񂾂��_���[�W���O�Ƀ_���[�W�m��C�x���g�R�[��
    if( i == 0 ){
      scproc_Fight_Damage_Determine( wk, attacker, defender, wazaParam );
    }
    // �Q��ڈȍ~�̓��U�G�t�F�N�g��ǉ��Ŕ���������
    else{
      BtlPokePos atkPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, BPP_GetID(attacker) );
      BtlPokePos defPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, BPP_GetID(defender) );

      SCQUE_PUT_ACT_WazaEffect( wk->que, atkPos, defPos, wazaParam->wazaID );
    }


    // �f�o�b�O���ȒP�ɂ��邽�ߕK����_���[�W�ɂ���[�u
    #if 0
    if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L )
    {
      u8 pokeID = BPP_GetID(defender);
      if( pokeID >= BTL_PARTY_MEMBER_MAX )
      {
        damage = 999;
      }
    }
    if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R )
    {
      u8 pokeID = BPP_GetID(defender);
      if( pokeID < BTL_PARTY_MEMBER_MAX )
      {
        damage = 999;
      }
    }
    #endif

    if( BPP_MIGAWARI_IsExist(defender) ){
      scproc_Migawari_Damage( wk, defender, damage );
    }
    else{
      u8 koraeru_cause;

      damage = MarumeDamage( defender, damage );

      koraeru_cause = scEvent_CheckKoraeru( wk, attacker, defender, &damage );
      scPut_WazaDamageSingle( wk, wazaParam, defender, affinity, damage, fCritical, fPluralHit );

      // ���炦
      if( koraeru_cause != BPP_KORAE_NONE ){
        scproc_Koraeru( wk, defender, koraeru_cause );
      }
      damage_sum += damage;

      // �ǉ����ʁA���A�N�V��������
      scproc_WazaAdditionalEffect( wk, wazaParam, attacker, defender, damage );
      scproc_WazaDamageReaction( wk, attacker, defender, wazaParam, affinity, damage, fCritical );
      scproc_Fight_Damage_Kickback( wk, attacker, wazaParam->wazaID, damage );
    }

    if( BPP_IsDead(defender) ){
      fDead = TRUE;
    }else{
      scproc_CheckItemReaction( wk, defender );
    }

    if( BPP_IsDead(attacker) ){
      fDead = TRUE;
    }

    if( fDead ){ break; }
  }

  BTL_POKESET_AddWithDamage( &wk->pokesetDamaged, defender, damage_sum );
  if( damage_sum ){
    scproc_WazaDamageAfter( wk, attacker, wazaParam, damage_sum, fCritical );
    wazaDmgRec_Add( attacker, defender, wazaParam, damage_sum );
    BPP_TURNFLAG_Set( defender, BPP_TURNFLG_DAMAGED );
  }

  // �����񂠂������I���b�Z�[�W
  if( fPluralHit && (hit_count > 1)){
    SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_Hit_PluralTimes, hit_count );
  }
}
//
static void scproc_Fight_damage_side_plural( BTL_SVFLOW_WORK* wk,
    BTL_POKEPARAM* attacker, BTL_POKESET* targets, BtlTypeAff* affAry, const SVFL_WAZAPARAM* wazaParam, fx32 dmg_ratio )
{
  BTL_POKEPARAM *bpp[ BTL_POSIDX_MAX ];
  u16 dmg[ BTL_POSIDX_MAX ];
  u32 dmg_sum;
  u8  critical_flg[ BTL_POSIDX_MAX ];
  u8  koraeru_cause[ BTL_POSIDX_MAX ];
  u8 i, poke_cnt, fFixDamage;

  poke_cnt = BTL_POKESET_GetCount( targets );
  GF_ASSERT(poke_cnt <= BTL_POSIDX_MAX);

  wazaEffCtrl_SetEnable( &wk->wazaEffCtrl );

  dmg_sum = 0;
  for(i=0; i<poke_cnt; ++i)
  {
    bpp[i] = BTL_POKESET_Get( targets, i );
    critical_flg[i] = scEvent_CheckCritical( wk, attacker, bpp[i], wazaParam->wazaID );
    fFixDamage = scEvent_CalcDamage( wk, attacker, bpp[i], wazaParam, affAry[i], dmg_ratio, critical_flg[i], &dmg[i] );
    if( fFixDamage ){
      affAry[i] = BTL_TYPEAFF_100;
      critical_flg[i] = FALSE;
    }
    dmg[i] = MarumeDamage( bpp[i], dmg[i] );
    koraeru_cause[i] = scEvent_CheckKoraeru( wk, attacker, bpp[i], &dmg[i] );
    dmg_sum += dmg[i];
  }

  // �_���[�W�m��C�x���g�R�[��
  for(i=0; i<poke_cnt; ++i){
    if( dmg[i] ){
      scproc_Fight_Damage_Determine( wk, attacker, bpp[i], wazaParam );
    }
  }

  // �_���[�W�R�}���h�o��
  scPut_WazaDamagePlural( wk, wazaParam, poke_cnt, affAry[0], bpp, dmg, critical_flg );

  for(i=0; i<poke_cnt; ++i)
  {
    if( dmg[i] ){
      BTL_POKESET_AddWithDamage( &wk->pokesetDamaged, bpp[i], dmg[i] );
      wazaDmgRec_Add( attacker, bpp[i], wazaParam, dmg[i] );
      BPP_TURNFLAG_Set( bpp[i], BPP_TURNFLG_DAMAGED );
    }
  }


  // ���炦
  for(i=0; i<poke_cnt; ++i)
  {
    if( koraeru_cause[i] != BPP_KORAE_NONE ){
      scproc_Koraeru( wk, bpp[i], koraeru_cause[i] );
    }
  }

  // �ǉ����ʁA���A�N�V��������
  for(i=0; i<poke_cnt; ++i){
    scproc_WazaAdditionalEffect( wk, wazaParam, attacker, bpp[i], dmg[i] );
    scproc_WazaDamageReaction( wk, attacker, bpp[i], wazaParam, affAry[i], dmg[i], critical_flg[i] );
    scproc_CheckItemReaction( wk, bpp[i] );
  }
  if( dmg_sum ){
    scproc_WazaDamageAfter( wk, attacker, wazaParam, dmg[i], critical_flg[i] );
  }

  scproc_Fight_Damage_Kickback( wk, attacker, wazaParam->wazaID, dmg_sum );

  for(i=0; i<poke_cnt; ++i){
    scproc_CheckDeadCmd( wk, bpp[i] );
  }
  scproc_CheckDeadCmd( wk, attacker );
}
/**
 * �ō��Ŏc��g�o�͈̔͂Ɏ��܂�悤�Ƀ_���[�W�ŏI�␳
 */
static u32 MarumeDamage( const BTL_POKEPARAM* bpp, u32 damage )
{
  u32 hp = BPP_GetValue( bpp, BPP_HP );
  if( damage > hp ){
    damage = hp;
  }
  return damage;
}

//----------------------------------------------------------------------------------
/**
 * [Event] ���U�ɂ��_���[�W���u���炦��v���ǂ����`�F�b�N
 * ���u���炦��v= HP��1�c��
 *
 * @param   wk
 * @param   defender    ���U�_���[�W�����炤�|�P
 * @param   damage      [io] �_���[�W�ʁi���炦���ꍇ�AHP���P�c��悤�ɕ␳�����j
 *
 * @retval  BppKoraeruCause   �u���炦��v���R�^���炦�Ȃ��ꍇ�� BPP_KORAE_NONE
 */
//----------------------------------------------------------------------------------
static BppKoraeruCause scEvent_CheckKoraeru( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, u16* damage )
{
  if( BPP_GetValue(defender, BPP_HP) > (*damage) )
  {
    return BPP_KORAE_NONE;
  }
  else
  {
    BppKoraeruCause  cause = BPP_KORAE_NONE;

    BTL_EVENTVAR_Push();
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
      BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_KORAERU_CAUSE, cause );
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_KORAERU_CHECK );
      cause = BTL_EVENTVAR_GetValue( BTL_EVAR_KORAERU_CAUSE );
    BTL_EVENTVAR_Pop();

    if( cause != BPP_KORAE_NONE ){
      *damage = BPP_GetValue(defender, BPP_HP) - 1;
    }

    return cause;
  }
}
//----------------------------------------------------------------------------------
/**
 * �u���炦��v�����\�����A�C�e������̏���
 *
 * @param   wk
 * @param   bpp
 * @param   cause
 */
//----------------------------------------------------------------------------------
static void scproc_Koraeru( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppKoraeruCause cause )
{
  u8 pokeID = BPP_GetID( bpp );

  switch( cause ){
  case BPP_KORAE_WAZA_DEFENDER:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_Koraeru_Exe, pokeID );
    break;

  case BPP_KORAE_ITEM:
  default:
    {
      u32 hem_state = Hem_PushState( &wk->HEManager );
      scEvent_KoraeruExe( wk, bpp, cause );
      scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
      Hem_PopState( &wk->HEManager, hem_state );
    }
    break;
  }
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���炦�铮�씭��
 *
 * @param   wk
 * @param   bpp
 * @param   cause
 */
//----------------------------------------------------------------------------------
static void scEvent_KoraeruExe( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppKoraeruCause cause )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_KORAERU_EXE );
  BTL_EVENTVAR_Pop();
}

//----------------------------------------------------------------------------------
/**
 * ���U�_���[�W���R�[�h��ǉ�
 *
 * @param   attacker
 * @param   defender
 * @param   wazaParam
 * @param   damage
 */
//----------------------------------------------------------------------------------
static void wazaDmgRec_Add( const BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam, u16 damage )
{
  BPP_WAZADMG_REC rec;
  BPP_WAZADMG_REC_Setup( &rec, BPP_GetID(attacker), wazaParam->wazaID, wazaParam->wazaType, damage );
  BPP_WAZADMGREC_Add( defender, &rec );
}

//--------------------------------------------------------------------------
/**
 * ���U�_���[�W��̒ǉ����ʏ���
 *
 * @param   wk
 * @param   waza        �g�p���U
 * @param   attacker    �U���|�P�����p�����[�^
 * @param   defender    �h��|�P�����p�����[�^
 * @param   damage      �^�����_���[�W
 *
 */
//--------------------------------------------------------------------------
static void scproc_WazaAdditionalEffect( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
  BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, u32 damage )
{
  WazaCategory category = WAZADATA_GetCategory( wazaParam->wazaID );

  if( !BPP_IsDead(defender) )
  {
    switch( category ){
    case WAZADATA_CATEGORY_DAMAGE_EFFECT_USER:
      scproc_Fight_Damage_AddEffect( wk, wazaParam, attacker, attacker );
      break;
    case WAZADATA_CATEGORY_DAMAGE_EFFECT:
      scproc_Fight_Damage_AddEffect( wk, wazaParam, attacker, defender );
      break;
    case WAZADATA_CATEGORY_DAMAGE_SICK:
      scproc_Fight_Damage_AddSick( wk, wazaParam, attacker, defender );
      break;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ���U�_���[�W�ɑ΂��锽������
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   wazaParam
 * @param   affinity
 * @param   damage
 * @param   critical_flag
 */
//----------------------------------------------------------------------------------
static void scproc_WazaDamageReaction( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender,
  const SVFL_WAZAPARAM* wazaParam, BtlTypeAff affinity, u32 damage, BOOL critical_flag )
{
  u32 hem_state = Hem_PushState( &wk->HEManager );

  scEvent_WazaDamageReaction( wk, attacker, defender, wazaParam, affinity, damage, critical_flag );
  scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
  Hem_PopState( &wk->HEManager, hem_state );
}
//----------------------------------------------------------------------------------
/**
 * ���U�_���[�W��^���I�������̒ǉ�����
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   wazaParam
 * @param   affinity
 * @param   damage
 * @param   critical_flag
 */
//----------------------------------------------------------------------------------
static void scproc_WazaDamageAfter( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker,
  const SVFL_WAZAPARAM* wazaParam, u32 damage, BOOL critical_flag )
{
  u32 hem_state = Hem_PushState( &wk->HEManager );

  scEvent_WazaDamageAfter( wk, attacker, wazaParam, damage, critical_flag );
  scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
  Hem_PopState( &wk->HEManager, hem_state );
}
//----------------------------------------------------------------------------------
/**
 * �A�C�e�������`�F�b�N
 *
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
static void scproc_CheckItemReaction( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  u32 hem_state = Hem_PushState( &wk->HEManager );

  BTL_Printf("�|�P[%d]�̑����A�C�e�������`�F�b�N���܂�\n", BPP_GetID(bpp));

  scEvent_CheckItemReaction( wk, bpp );
  scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
  Hem_PopState( &wk->HEManager, hem_state );
}
//----------------------------------------------------------------------------------
/**
 * [Event]�A�C�e�������`�F�b�N
 *
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
static void scEvent_CheckItemReaction( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHECK_ITEM_REACTION );
  BTL_EVENTVAR_Pop();
}

//------------------------------------------------------------------
// �T�[�o�[�t���[�F�_���[�W�󂯌�̏���
//------------------------------------------------------------------
static void scproc_Fight_Damage_After( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKESET* targets )
{
  scproc_Fight_Damage_Drain( wk, wazaParam->wazaID, attacker, targets );
  scproc_Fight_Damage_Shrink( wk, wazaParam->wazaID, attacker, targets );
  scproc_Fight_Damage_KoriCure( wk, wazaParam, attacker, targets );

  {
    u32 hem_state = Hem_PushState( &wk->HEManager );

    scEvent_DamageProcEnd( wk, attacker, targets, wazaParam->wazaID );
    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );

    Hem_PopState( &wk->HEManager, hem_state );
  }
}
//------------------------------------------------------------------
// �T�[�o�[�t���[�F�_���[�W�󂯌�̏��� > �Ђ�݃`�F�b�N
//------------------------------------------------------------------
static void scproc_Fight_Damage_Shrink( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, BTL_POKESET* targets )
{
  u32 waza_per = scEvent_GetWazaShrinkPer( wk, waza, attacker );
  BTL_POKEPARAM* bpp;
  u32 i=0;

  while( (bpp = BTL_POKESET_Get(targets, i++)) != NULL )
  {
    if( !BPP_TURNFLAG_Get(bpp, BPP_TURNFLG_ACTION_DONE) )
    {
      scproc_AddShrinkCore( wk, bpp, waza, waza_per );
    }
  }
}
//------------------------------------------------------------------
// �T�[�o�[�t���[�F�_���[�W�󂯌�̏��� > �ق̂����U�Łu������v�����鏈��
// �ǉ����ʏ����̌�ɒu�����ƁB�łȂ��Ɓu�����肪�Ƃ����v���u�₯�ǂɂȂ����v���N���Ă��܂��B
//------------------------------------------------------------------
static void scproc_Fight_Damage_KoriCure( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
  BTL_POKEPARAM* attacker, BTL_POKESET* targets )
{
  if( wazaParam->wazaType == POKETYPE_HONOO )
  {
    BTL_POKEPARAM* bpp;

    BTL_POKESET_SeekStart( targets );
    while( (bpp = BTL_POKESET_SeekNext(targets)) != NULL )
    {
      if( BPP_GetPokeSick(bpp) == POKESICK_KOORI )
      {
        scPut_CurePokeSick( wk, bpp, POKESICK_KOORI );
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �Ώۃ|�P�����ɂЂ�݌��ʂ�^����
 *
 * @param   wk
 * @param   target
 * @param   waza    ���U�ǉ����ʂɂ��Ђ�݂̏ꍇ�A���U�i���o�[�i����ȊO WAZANO_NULL�j
 * @param   per     �m���i�p�[�Z���e�[�W�j
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL scproc_AddShrinkCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, WazaID waza, u32 per )
{
  BOOL fShrink;

  BTL_Printf("�Ђ�݌��ʂ�^�����\n");

  if( BPP_TURNFLAG_Get(target, BPP_TURNFLG_MUST_SHRINK) ){
    fShrink = TRUE;
  }else{
    fShrink = scEvent_CheckShrink(wk, target, waza, per);
  }

  if( fShrink )
  {
    BPP_TURNFLAG_Set( target, BPP_TURNFLG_SHRINK );
    return TRUE;
  }
  else if( per >= 100 )
  {
    // �m��100���Ȃ̂Ɏ��s�����猴���\����
    u32 hem_state = Hem_PushState( &wk->HEManager );
    scEvent_FailShrink( wk, target );
    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    Hem_PopState( &wk->HEManager, hem_state );
  }
  return FALSE;
}

//---------------------------------------------------------------------------------------------
// �T�[�o�[�t���[�F���U�_���[�W�ɉ�����HP�z��������
//---------------------------------------------------------------------------------------------
static void scproc_Fight_Damage_Drain( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, BTL_POKESET* targets )
{
  u32 total_damage = 0;
  u32 hem_state;

  if( WAZADATA_GetCategory(waza) == WAZADATA_CATEGORY_DRAIN )
  {
    BTL_POKEPARAM* bpp;
    u32 damage, recoverHP;
    u32 i;

    hem_state = Hem_PushState( &wk->HEManager );
    BTL_POKESET_SeekStart( targets );
    while( (bpp = BTL_POKESET_SeekNext(targets)) != NULL )
    {
      damage = BTL_POKESET_GetDamage( targets, bpp );
      total_damage += damage;
      recoverHP = (WAZADATA_GetParam(waza, WAZAPARAM_DAMAGE_RECOVER_RATIO) * damage) / 100;
      //�Œ�ł�1�͉񕜂���悤�ɂ���
      if( recoverHP == 0 ) recoverHP = 1;

      if( scproc_DrainCore(wk, attacker, bpp, recoverHP) )
      {
        scPut_Message_Set( wk, bpp, BTL_STRID_SET_Drain );
      }
    }
  }

  {
    hem_state = Hem_PushState( &wk->HEManager );
    scEvent_CheckSpecialDrain( wk, waza, attacker, total_damage );
    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    Hem_PopState( &wk->HEManager, hem_state );
  }
}
//
static BOOL scproc_DrainCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, u16 drainHP )
{
  u32 hem_state = Hem_PushState( &wk->HEManager );
  BOOL result = FALSE;

  drainHP = scEvent_RecalcDrainVolume( wk, attacker, target, drainHP );
  if( drainHP && !BPP_IsDead(attacker) )
  {
    if( scproc_RecoverHP(wk, attacker, drainHP) ){
      result = TRUE;
    }
  }

  scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
  Hem_PopState( &wk->HEManager, hem_state );

  return result;
}
//----------------------------------------------------------------------------------
/**
 * [Event] �_���[�W���U�����I��
 *
 * @param   wk
 * @param   attacker
 * @param   targets
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scEvent_DamageProcEnd( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, BTL_POKESET* targets, WazaID waza )
{
  u32 cnt;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    {
      u32 i, damage_sum;
      const BTL_POKEPARAM* bpp;

      cnt = BTL_POKESET_GetCount( targets );
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_TARGET_POKECNT, cnt );
      if( cnt )
      {
        for(i=0, damage_sum=0; i<cnt; ++i){
          bpp = BTL_POKESET_Get( targets, i );
          BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_TARGET1+i, BPP_GetID(bpp) );
          damage_sum += BTL_POKESET_GetDamage( targets, bpp );
        }
      }
      else
      {
        BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_TARGET1, BTL_POKEID_NULL );
      }
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_DAMAGE, damage_sum );
    }
    if( cnt ){
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_DAMAGEPROC_END_INFO );
    }
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_DAMAGEPROC_END );

  BTL_EVENTVAR_Pop();
}


//------------------------------------------------------------------
// �T�[�o�[�t���[�������F �������� > �_���[�W���U�n > �_���[�W�l�v�Z
//------------------------------------------------------------------
//----------------------------------------------------------------------------------
/**
 * [Event] �_���[�W�v�Z
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   wazaParam
 * @param   typeAff
 * @param   targetDmgRatio
 * @param   criticalFlag
 * @param   dstDamage       [out] �v�Z����
 *
 * @retval  BOOL     �Œ�_���[�W�l�i�h��E�U�����̒l�Ɗ֌W�Ȃ��^����_���[�W�j�̏ꍇ�ATRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_CalcDamage( BTL_SVFLOW_WORK* wk,
    const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam,
    BtlTypeAff typeAff, fx32 targetDmgRatio, BOOL criticalFlag, u16* dstDamage )
{
  enum {
    PRINT_FLG = TRUE,
  };
  WazaDamageType dmgType = WAZADATA_GetDamageType( wazaParam->wazaID );
  u32  rawDamage = 0;
  BOOL fFixDamage = FALSE;

  BTL_EVENTVAR_Push();

  BTL_EVENTVAR_SetConstValue( BTL_EVAR_TYPEAFF, typeAff );
  BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
  BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
  BTL_EVENTVAR_SetConstValue( BTL_EVAR_CRITICAL_FLAG, criticalFlag );
  BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
  BTL_EVENTVAR_SetConstValue( BTL_EVAR_DAMAGE_TYPE, dmgType );
  BTL_EVENTVAR_SetValue( BTL_EVAR_FIX_DAMAGE, 0 );

  BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_DMG_PROC1 );
  rawDamage = BTL_EVENTVAR_GetValue( BTL_EVAR_FIX_DAMAGE );

  if( rawDamage )
  {
    fFixDamage = TRUE;
  }
  else
  {
    u32 fxDamage;
    u16 atkPower, defGuard, wazaPower;
    u8 atkLevel;
    // �e��p�����[�^����f�̃_���[�W�l�v�Z
    wazaPower = scEvent_getWazaPower( wk, attacker, defender, wazaParam );
    atkPower  = scEvent_getAttackPower( wk, attacker, defender, wazaParam, criticalFlag );
    defGuard  = scEvent_getDefenderGuard( wk, attacker, defender, wazaParam, criticalFlag );
    {
      atkLevel = BPP_GetValue( attacker, BPP_LEVEL );

      fxDamage = (wazaPower * atkPower * (atkLevel*2/5+2));
      fxDamage = fxDamage / defGuard / 50;
      fxDamage += 2;
      BTL_N_PrintfEx(PRINT_FLG, DBGSTR_CALCDMG_BaseDamage, fxDamage);
    }
    fxDamage  = BTL_CALC_MulRatio( fxDamage, targetDmgRatio );
    BTL_N_PrintfEx( PRINT_FLG, DBGSTR_CALCDMG_RangeHosei, fxDamage);
    // �V��␳
    {
      fx32 weatherDmgRatio = BTL_FIELD_GetWeatherDmgRatio( wazaParam->wazaID );
      if( weatherDmgRatio != BTL_CALC_DMG_WEATHER_RATIO_NONE )
      {
        u32 prevDmg = fxDamage;
        fxDamage = BTL_CALC_MulRatio( fxDamage, weatherDmgRatio );
        BTL_N_PrintfEx( PRINT_FLG, DBGSTR_CALCDMG_WeatherHosei, weatherDmgRatio, prevDmg, fxDamage);
      }
    }
    // �f�_���[�W�l���m��
    BTL_N_PrintfEx( PRINT_FLG, DBGSTR_CALCDMG_RAW_DAMAGE,
        wazaPower, atkLevel, atkPower, defGuard, fxDamage );

    // �N���e�B�J���łQ�{
    if( criticalFlag ){
      fxDamage *= 2;
      BTL_N_PrintfEx( PRINT_FLG, DBGSTR_CALCDMG_Critical, fxDamage);
    }
    //�����_���␳(100�`85��)
    if( !BTL_MAIN_GetDebugFlag(wk->mainModule, BTL_DEBUGFLAG_DMG_RAND_OFF) )
    {
      u16 ratio = 100 - BTL_CALC_GetRand( 16 );
//      rawDamage = (rawDamage * ratio) / 100;
      fxDamage = (fxDamage * ratio) / 100;
      BTL_N_PrintfEx( PRINT_FLG, DBGSTR_CALCDMG_RandomHosei, ratio, fxDamage);
    }
    // �^�C�v��v�␳
    if( !(wazaParam->fTypeFlat) )
    {
      fx32 ratio = scEvent_CalcTypeMatchRatio( wk, attacker, wazaParam->wazaType );
      fxDamage = BTL_CALC_MulRatio( fxDamage, ratio );
      if( ratio != FX32_ONE ){
        BTL_N_PrintfEx( PRINT_FLG, DBGSTR_CALCDMG_TypeMatchHosei, (ratio*100>>FX32_SHIFT), fxDamage);
      }
    }
    // �^�C�v�����v�Z
    fxDamage = BTL_CALC_AffDamage( fxDamage, typeAff );
    BTL_N_PrintfEx( PRINT_FLG, DBGSTR_CALCDMG_TypeAffInfo, typeAff, fxDamage);
    // �₯�Ǖ␳
    if( (dmgType == WAZADATA_DMG_PHYSIC)
    &&  (BPP_GetPokeSick(attacker) == POKESICK_YAKEDO)
    ){
      fxDamage = (fxDamage * BTL_YAKEDO_DAMAGE_RATIO) / 100;
    }
    rawDamage = fxDamage;
    if( rawDamage == 0 ){ rawDamage = 1; }

    // �e��n���h���ɂ��{���v�Z
    BTL_EVENTVAR_SetMulValue( BTL_EVAR_RATIO, FX32_CONST(1), FX32_CONST(0.01f), FX32_CONST(32) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_DAMAGE, rawDamage );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_DMG_PROC2 );
    {
      fx32 ratio = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
      rawDamage = BTL_EVENTVAR_GetValue( BTL_EVAR_DAMAGE );
      rawDamage = BTL_CALC_MulRatio( rawDamage, ratio );
      BTL_N_PrintfEx( PRINT_FLG, DBGSTR_CALCDMG_DamageResult,
            BPP_GetID(defender), ratio, BTL_EVENTVAR_GetValue( BTL_EVAR_DAMAGE ), rawDamage);
    }
  } /* if( rawDamage == 0 ) */


  // �ō��Ŏc��g�o�͈̔͂Ɏ��܂�悤�ɍŏI�␳
  #if 0
  if( rawDamage > BPP_GetValue(defender, BPP_HP) ){
    rawDamage = BPP_GetValue( defender, BPP_HP );
    BTL_N_PrintfEx( PRINT_FLG, DBGSTR_CALCDMG_DamageMarume, rawDamage);
  }
  #endif

  BTL_EVENTVAR_Pop();

  *dstDamage = rawDamage;
  return fFixDamage;
}
//---------------------------------------------------------------------------------------------
// ���U�_���[�W������̔��������i�_���[�W�~�������j
//---------------------------------------------------------------------------------------------
static void scproc_Fight_Damage_Kickback( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza, u32 wazaDamage )
{
  u32 damage = scEvent_CalcKickBack( wk, attacker, waza, wazaDamage );
  if( damage )
  {
    HANDEX_STR_Setup( &wk->strParam, BTL_STRTYPE_SET, BTL_STRID_SET_ReactionDmg );
    HANDEX_STR_AddArg( &wk->strParam, BPP_GetID(attacker) );
    scproc_SimpleDamage( wk, attacker, damage, &wk->strParam );
  }
}
//---------------------------------------------------------------------------------------------
// ���U�ȊO�̃_���[�W���ʏ���
//---------------------------------------------------------------------------------------------
static BOOL scproc_SimpleDamage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u32 damage, BTL_HANDEX_STR_PARAMS* str )
{
  if( scEvent_CheckEnableSimpleDamage(wk, bpp, damage) )
  {
    int value = -damage;

    if( value )
    {
      if( str != NULL ){
        handexSub_putString( wk, str );
        HANDEX_STR_Clear( str );
      }

      scPut_SimpleHp( wk, bpp, value, TRUE );
      scproc_CheckItemReaction( wk, bpp );
      BPP_TURNFLAG_Set( bpp, BPP_TURNFLG_DAMAGED );

      if( scproc_CheckDeadCmd(wk, bpp) ){
        if( scproc_CheckShowdown(wk) ){
          return TRUE;
        }
      }

      scproc_CheckItemReaction( wk, bpp );
      return TRUE;
    }
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * [Proc] �����A�C�e���g�p
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  BOOL  ����v���ɂ�葕���A�C�e���̎g�p���o���Ȃ����FALSE�^����ȊOTRUE
 */
//----------------------------------------------------------------------------------
static BOOL scproc_UseItemEquip( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  u32 itemID = BPP_GetItem( bpp );
  u32 hem_state_1st;
  BOOL result;

  hem_state_1st = Hem_PushState( &wk->HEManager );
  result = !scEvent_CheckItemEquipFail( wk, bpp, itemID );
  if( result )
  {
    u32 hem_state_2nd = Hem_PushState( &wk->HEManager );
    scEvent_ItemEquip( wk, bpp );

    scPut_UseItemAct( wk, bpp );
    scproc_HandEx_Root( wk, itemID );

    Hem_PopState( &wk->HEManager, hem_state_2nd );
    if( BTL_CALC_ITEM_GetParam(itemID, ITEM_PRM_ITEM_SPEND) ){
      scPut_ConsumeItem( wk, bpp );
      scPut_SetTurnFlag( wk, bpp, BPP_TURNFLG_ITEM_CONSUMED );
    }
  }
  else
  {
    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
  }

  Hem_PopState( &wk->HEManager, hem_state_1st );
  return result;
}
//----------------------------------------------------------------------------------
/**
 * [Event] �����A�C�e���g�p�ۃ`�F�b�N
 *
 * @param   wk
 * @param   bpp
 * @param   itemID
 *
 * @retval  BOOL    ����Ȍ����Ŏg�p�ł��Ȃ�������TRUE�^����ȊOFALSE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_CheckItemEquipFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u16 itemID )
{
  BOOL failFlag = FALSE;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_ITEM, itemID );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, FALSE );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHECK_ITEMEQUIP_FAIL );
    failFlag = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  BTL_EVENTVAR_Pop();
  return failFlag;
}

//---------------------------------------------------------------------------------------------
// �|�P�����������I�ɕm���ɂ���
//---------------------------------------------------------------------------------------------
static void scproc_KillPokemon( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  scPut_KillPokemon( wk, bpp, 0 );
  scproc_CheckDeadCmd( wk, bpp );
}
//---------------------------------------------------------------------------------------------
// �T�[�o�[�t���[�F�_���[�W���U�̒ǉ����ʂɂ���Ԉُ�
//---------------------------------------------------------------------------------------------
static void scproc_Fight_Damage_AddSick( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target )
{
  WazaSick sick;
  BPP_SICK_CONT sick_cont;
  u32 i=0;

  sick = scEvent_CheckWazaAddSick( wk, wazaParam->wazaID, attacker, target, &sick_cont );
  if( sick != WAZASICK_NULL )
  {
    if( !BPP_IsDead(target) ){
      scproc_Fight_WazaSickCore( wk,  attacker, target, wazaParam->wazaID, sick, sick_cont, FALSE );
    }
  }
}
//--------------------------------------------------------------------------
/**
 * �yEvent�z���U�ǉ����ʂɂ���Ԉُ�̔����`�F�b�N
 *
 * @param   wk
 * @param   waza
 * @param   attacker
 * @param   defender
 * @param   [out] pSickCont   �p������
 *
 * @retval  WazaSick          ��Ԉُ�ID
 */
//--------------------------------------------------------------------------
static WazaSick scEvent_CheckWazaAddSick( BTL_SVFLOW_WORK* wk, WazaID waza,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, BPP_SICK_CONT* pSickCont )
{
  BPP_SICK_CONT sickCont;
  WazaSick sick = WAZADATA_GetParam( waza, WAZAPARAM_SICK );
  WAZA_SICKCONT_PARAM  waza_contParam = WAZADATA_GetSickCont( waza );
  u8 per = WAZADATA_GetParam( waza, WAZAPARAM_SICK_PER );
  u8 fFail = FALSE;

  BTL_CALC_WazaSickContToBppSickCont( waza_contParam, attacker, &sickCont );

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENTVAR_SetValue( BTL_EVAR_SICKID, sick );
//    BTL_EVENT_CallHandlers( wk, BTL_EVENT_ADD_SICK_TYPE );

    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, fFail );
    BTL_EVENTVAR_SetValue( BTL_EVAR_SICK_CONT, sickCont.raw );
    BTL_EVENTVAR_SetValue( BTL_EVAR_ADD_PER, per );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_ADD_SICK );
    sick = BTL_EVENTVAR_GetValue( BTL_EVAR_SICKID );
    per = BTL_EVENTVAR_GetValue( BTL_EVAR_ADD_PER );
    fFail = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
    if( sick == WAZASICK_SPECIAL_CODE ){
      sick = WAZASICK_NULL;
    }
    sickCont.raw = BTL_EVENTVAR_GetValue( BTL_EVAR_SICK_CONT );
  BTL_EVENTVAR_Pop();

  if( !fFail )
  {
    if( sick != WAZASICK_NULL )
    {
      if( perOccur(wk, per) ){
        *pSickCont = sickCont;
        return sick;
      }
      // �f�o�b�O�@�\�ɂ��K������
      if( BTL_MAIN_GetDebugFlag(wk->mainModule, BTL_DEBUGFLAG_MUST_TUIKA) ){
        *pSickCont = sickCont;
        return sick;
      }
    }
  }
  return  WAZASICK_NULL;
}

//---------------------------------------------------------------------------------------------
// �T�[�o�[�t���[�F���U�ɂ�钼�ڂ̏�Ԉُ�
//---------------------------------------------------------------------------------------------
static void scproc_Fight_SimpleSick( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, BTL_POKESET* targetRec )
{
  BTL_POKEPARAM* target;
  WazaSick sick;
  WAZA_SICKCONT_PARAM contParam;
  BOOL fSucceed;
  u32  targetCnt;

  sick = WAZADATA_GetParam( waza, WAZAPARAM_SICK );
  contParam = WAZADATA_GetSickCont( waza );
  fSucceed = FALSE;

  targetCnt = BTL_POKESET_GetCount( targetRec );

  if( targetCnt )
  {
    BTL_POKESET_SeekStart( targetRec );
    while( (target = BTL_POKESET_SeekNext(targetRec)) != NULL )
    {
      BPP_SICK_CONT sickCont;
      BTL_CALC_WazaSickContToBppSickCont( contParam, attacker, &sickCont );
      if( scproc_Fight_WazaSickCore( wk, attacker, target, waza, sick, sickCont, TRUE) ){
        wazaEffCtrl_SetEnable( &wk->wazaEffCtrl );
        fSucceed = TRUE;  // �^�[�Q�b�g�����āA�P�̂ł���Ԉُ�ɂ�����ΐ���
      }
    }
  }
  else{
    // ���Ƀ^�[�Q�b�g�������Ȃ��Ă���->���������܂����܂�Ȃ�����
    scPut_WazaFail( wk, attacker, waza );
  }
}
//---------------------------------------------------------------------------------------------
// �T�[�o�[�t���[�F���U�ɂ���Ԉُ틤��
//---------------------------------------------------------------------------------------------
static BOOL scproc_Fight_WazaSickCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target,
  WazaID waza, WazaSick sick, BPP_SICK_CONT sickCont, BOOL fAlmost )
{
  HANDEX_STR_Clear( &wk->strParam );

  if( sick == WAZASICK_SPECIAL_CODE )
  {
    sick = scEvent_DecideSpecialWazaSick( wk, attacker, target, &wk->strParam );
    if( (sick == WAZASICK_NULL) || (sick == WAZASICK_SPECIAL_CODE) ){
      return FALSE;
    }
  }
  scEvent_WazaSickCont( wk, attacker, target, sick, &sickCont );

  {
    // ���ꃁ�b�Z�[�W�������p�ӂ���Ă��Ȃ���΁Ascproc_AddSick ���ŁA
    // �ł��邾���W�����b�Z�[�W���o���悤�ɂ��Ă���B
    BOOL fDefaultMsg = (HANDEX_STR_IsEnable(&wk->strParam) == FALSE );

    if( scproc_AddSick(wk, target, attacker, sick, sickCont, fAlmost, fDefaultMsg) )
    {
      if( !fDefaultMsg ){
        handexSub_putString( wk, &wk->strParam );
        HANDEX_STR_Clear( &wk->strParam );
      }
      return TRUE;
    }
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U�ɂ����ꏈ���̏�Ԉُ�ID�����������b�Z�[�W�����肷��
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   str
 *
 * @retval  WazaSick
 */
//----------------------------------------------------------------------------------
static WazaSick scEvent_DecideSpecialWazaSick( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, BTL_HANDEX_STR_PARAMS* str )
{
  WazaSick sickID;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WORK_ADRS, (int)(str) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_SICKID, WAZASICK_NULL );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZASICK_SPECIAL );
    sickID = BTL_EVENTVAR_GetValue( BTL_EVAR_SICKID );
  BTL_EVENTVAR_Pop();
  return sickID;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U�ɂ���Ԉُ�̌p���p�����[�^����
 *
 * @param   wk
 * @param   attacker
 * @param   target
 * @param   sick
 * @param   sickCont    [io]�����O�p�����[�^�^������̌��ʂ��i�[
 */
//----------------------------------------------------------------------------------
static void scEvent_WazaSickCont( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target,
  WazaSick sick, BPP_SICK_CONT* sickCont )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(target) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_SICKID, sick );
    BTL_EVENTVAR_SetValue( BTL_EVAR_SICK_CONT, sickCont->raw );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZASICK_PARAM );
    sickCont->raw = BTL_EVENTVAR_GetValue( BTL_EVAR_SICK_CONT );
  BTL_EVENTVAR_Pop();
}
//--------------------------------------------------------------------------
/**
 * [proc] ��Ԉُ틤�ʏ���
 *
 * @param   wk
 * @param   target
 * @param   attacker
 * @param   sick
 * @param   sickCont
 * @param   fAlmost            ���s�������Ɍ������b�Z�[�W��\������
 *
 * @retval  BOOL       ���������ꍇTRUE
 */
//--------------------------------------------------------------------------
static BOOL scproc_AddSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, BTL_POKEPARAM* attacker,
  WazaSick sick, BPP_SICK_CONT sickCont, BOOL fAlmost, BOOL fDefaultMsgEnable )
{
  BOOL fSucceed;

  fSucceed = addsick_core( wk, target, attacker, sick, sickCont, fAlmost );
  if( fSucceed )
  {
    // �G�t�F�N�g�̗p�ӂ���Ă���ُ�͂����\��
    switch( sick ){
    case WAZASICK_DOKU:
    case WAZASICK_DOKUDOKU:
      scPut_EffectByPokePos( wk, target, BTLEFF_DOKU ); break;
    case WAZASICK_YAKEDO:   scPut_EffectByPokePos( wk, target, BTLEFF_YAKEDO ); break;
    case WAZASICK_MAHI:     scPut_EffectByPokePos( wk, target, BTLEFF_MAHI ); break;
    case WAZASICK_KOORI:    scPut_EffectByPokePos( wk, target, BTLEFF_KOORI ); break;
    case WAZASICK_NEMURI:   scPut_EffectByPokePos( wk, target, BTLEFF_NEMURI ); break;
    case WAZASICK_KONRAN:   scPut_EffectByPokePos( wk, target, BTLEFF_KONRAN ); break;
    case WAZASICK_MEROMERO: scPut_EffectByPokePos( wk, target, BTLEFF_MEROMERO ); break;
    }

    if( fDefaultMsgEnable ){
      BTL_SICK_MakeDefaultMsg( sick, sickCont, target, &wk->strParam );
      handexSub_putString( wk, &wk->strParam );
      HANDEX_STR_Clear( &wk->strParam );
    }

    {
      u32 hem_state = Hem_PushState( &wk->HEManager );

      if( BTL_CALC_IsBasicSickID(sick) ){
        scEvent_PokeSickFixed( wk, target, attacker, sick );
      }else if( sick == WAZASICK_IEKI ){
        scEvent_IekiFixed( wk, target );
      }else {
        scEvent_WazaSickFixed( wk, target, attacker, sick );
      }
      scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
      Hem_PopState( &wk->HEManager, hem_state );
    }


    scproc_CheckItemReaction( wk, target );
  }

  return fSucceed;
}
//--------------------------------------------------------------------------
/**
 * ��Ԉُ�R�A����
 *
 * @param   wk
 * @param   target            ��Ԉُ�����炤�|�P
 * @param   sick              ��Ԉُ�ID
 * @param   sickCont          ��Ԉُ�p���p�����[�^
 * @param   fAlmost           �قڊm��t���O
 *
 * @retval  BOOL    ��Ԉُ�ɂ����ꍇ�ATRUE
 */
//--------------------------------------------------------------------------
static BOOL addsick_core( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, BTL_POKEPARAM* attacker,
  WazaSick sick, BPP_SICK_CONT sickCont, BOOL fAlmost  )
{
  BtlAddSickFailCode  fail_code;

  fail_code = addsick_check_fail( wk, target, sick, sickCont );

  if( fail_code != BTL_ADDSICK_FAIL_NULL )
  {
    BTL_Printf("��Ԉُ편�s�R�[�h=%d\n", fail_code);
    if( fAlmost ){
      scPut_AddSickFail( wk, target, fail_code, sick );
    }
    return FALSE;
  }
  else
  {
    BOOL fFail = FALSE;
    u32 hem_state = Hem_PushState( &wk->HEManager );

    if( sick < WAZASICK_STD_MAX ){
      fFail = scEvent_StdSick_CheckFail( wk, target, sick );
    }

    if( !fFail ){
      BTL_Printf("��Ԉُ팈��F�R�[�h=%d\n", sick);
      scPut_AddSick( wk, target, sick, sickCont );
    }
    else if( fAlmost )
    {
      scEvent_AddSick_Failed( wk, target, sick );
      scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    }

    Hem_PopState( &wk->HEManager, hem_state );
    return !fFail;
  }
}
static BtlAddSickFailCode addsick_check_fail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target,
  WazaSick sick, BPP_SICK_CONT sickCont )
{
  // �Ă񂱂��u�͂�v�̎��Ɂu������v�ɂ͂Ȃ�Ȃ�
  if( (scEvent_GetWeather(wk) == BTL_WEATHER_SHINE)
  &&  (sick == POKESICK_KOORI)
  ){
    return BTL_ADDSICK_FAIL_OTHER;
  }
  // ���łɍ����Ȃ獬���ɂȂ�Ȃ��Ƃ��c
  if( BPP_CheckSick(target, sick) ){
    return BTL_ADDSICK_FAIL_ALREADY;
  }

  // ���łɃ|�P�����n��Ԉُ�ɂȂ��Ă���Ȃ�A�V���Ƀ|�P�����n��Ԉُ�ɂ͂Ȃ�Ȃ�
  if( (sick < POKESICK_MAX) || (sick == WAZASICK_DOKUDOKU) )
  {
    if( BPP_GetPokeSick(target) != POKESICK_NULL ){
      return BTL_ADDSICK_FAIL_OTHER;
    }
  }

  // �͂��� or �ǂ��^�C�v�́A�u�ǂ��v�ɂȂ�Ȃ�
  if( (sick==WAZASICK_DOKU) || (sick==WAZASICK_DOKUDOKU) )
  {
    PokeTypePair type = BPP_GetPokeType( target );
    if( PokeTypePair_IsMatch(type, POKETYPE_HAGANE)
    ||  PokeTypePair_IsMatch(type, POKETYPE_DOKU)
    ){
      return BTL_ADDSICK_FAIL_OTHER;
    }
  }

  // ���߂�^�C�v�́A�u�܂Ёv�ɂȂ�Ȃ�
  if( sick==WAZASICK_MAHI )
  {
    PokeTypePair type = BPP_GetPokeType( target );
    if( PokeTypePair_IsMatch(type, POKETYPE_JIMEN) ){
      return BTL_ADDSICK_FAIL_OTHER;
    }
  }

  // �ق̂��^�C�v�́A�u�₯�ǁv�ɂȂ�Ȃ�
  if( sick==WAZASICK_YAKEDO )
  {
    PokeTypePair type = BPP_GetPokeType( target );
    if( PokeTypePair_IsMatch(type, POKETYPE_HONOO) ){
      return BTL_ADDSICK_FAIL_OTHER;
    }
  }

  // �����^�C�v�́A�u��ǂ肬�̃^�l�v�ɂȂ�Ȃ�
  if( sick==WAZASICK_YADORIGI )
  {
    PokeTypePair type = BPP_GetPokeType( target );
    if( PokeTypePair_IsMatch(type, POKETYPE_KUSA) ){
      return BTL_ADDSICK_FAIL_OTHER;
    }
  }

  return BTL_ADDSICK_FAIL_NULL;
}
//----------------------------------------------------------------------------------
/**
 * [Event] �V��`�F�b�N
 *
 * @param   wk
 *
 * @retval  BtlWeather
 */
//----------------------------------------------------------------------------------
static BtlWeather scEvent_GetWeather( BTL_SVFLOW_WORK* wk )
{
  BOOL fDisable = FALSE;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, FALSE );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WEATHER_CHECK );
    fDisable = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  BTL_EVENTVAR_Pop();

  if( fDisable ){
    return BTL_WEATHER_NONE;
  }

  return BTL_FIELD_GetWeather();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ��Ԉُ�����s����P�[�X�̃`�F�b�N
 *
 * @param   wk
 * @param   target
 * @param   sick
 *
 * @retval  BOOL    ���s����ꍇ��TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_StdSick_CheckFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, WazaSick sick  )
{
  BOOL fFail = FALSE;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(target) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_SICKID, sick );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, fFail );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_ADDSICK_CHECKFAIL );
    fFail = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  BTL_EVENTVAR_Pop();
  return fFail;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ��Ԉُ편�s���m�肵��
 *
 * @param   wk
 * @param   target
 * @param   sick
 */
//----------------------------------------------------------------------------------
static void scEvent_AddSick_Failed( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, WazaSick sick )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BPP_GetID(target) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_SICKID, sick );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_ADDSICK_FAILED );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] �|�P�����n��Ԉُ�m��
 *
 * @param   wk
 * @param   target
 * @param   attacker
 * @param   sick
 */
//----------------------------------------------------------------------------------
static void scEvent_PokeSickFixed( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, const BTL_POKEPARAM* attacker, PokeSick sick )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(target) );
    {
      u8 atkPokeID = (attacker!=NULL)? BPP_GetID(attacker) : BTL_POKEID_NULL;
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, atkPokeID );
    }
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_SICKID, sick );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_POKESICK_FIXED );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U�n��Ԉُ�m��
 *
 * @param   wk
 * @param   target
 * @param   attacker
 * @param   sick
 */
//----------------------------------------------------------------------------------
static void scEvent_WazaSickFixed( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, const BTL_POKEPARAM* attacker, WazaSick sick )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(target) );
    {
      u8 atkPokeID = (attacker!=NULL)? BPP_GetID(attacker) : BTL_POKEID_NULL;
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, atkPokeID );
    }
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_SICKID, sick );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZASICK_FIXED );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] �������ɂ������������̊m��
 *
 * @param   wk
 * @param   target
 */
//----------------------------------------------------------------------------------
static void scEvent_IekiFixed( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(target) );
    BTL_EVENT_ForceCallHandlers( wk, BTL_EVENT_IEKI_FIXED );
  BTL_EVENTVAR_Pop();
}


//---------------------------------------------------------------------------------------------
// �T�[�o�[�t���[�F�_���[�W���U�̒ǉ����ʂɂ�郉���N����
//---------------------------------------------------------------------------------------------
static void scproc_Fight_Damage_AddEffect( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target )
{
  if( !BPP_IsDead(target) )
  {
    if( scEvent_CheckAddRankEffectOccur(wk, wazaParam, attacker, target) )
    {
      scproc_WazaRankEffect_Common( wk, wazaParam, attacker, target, FALSE );
    }
  }
}
//--------------------------------------------------------------------------
/**
 * �yEvent�z���U�ɂ��ǉ������N���ʂ̔����`�F�b�N
 *
 * @param   wk
 * @param   attacker
 * @param   target
 * @param   waza
 *
 * @retval  BOOL    ����������TRUE
 */
//--------------------------------------------------------------------------
static BOOL scEvent_CheckAddRankEffectOccur( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target )
{
  u8 per = WAZADATA_GetParam( wazaParam->wazaID, WAZAPARAM_RANKPER_1 );
  u8 failFlag;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(target) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, FALSE );
    BTL_EVENTVAR_SetValue( BTL_EVAR_ADD_PER, per );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_ADD_RANK_TARGET );
    failFlag = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
    per = BTL_EVENTVAR_GetValue( BTL_EVAR_ADD_PER );
  BTL_EVENTVAR_Pop();

  if( !failFlag )
  {
    if( perOccur(wk, per) ){
      return TRUE;
    }
    // �f�o�b�O�@�\�ɂ��K������
    if( BTL_MAIN_GetDebugFlag(wk->mainModule, BTL_DEBUGFLAG_MUST_TUIKA) ){
      return TRUE;
    }
  }

  return FALSE;
}
//---------------------------------------------------------------------------------------------
// �T�[�o�[�t���[�F���U�ɂ�钼�ڂ̃����N����
//---------------------------------------------------------------------------------------------
static void scproc_Fight_SimpleEffect( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKESET* targetRec )
{
  BTL_POKEPARAM* target;
  u32 i = 0;

  BTL_POKESET_SeekStart( targetRec );
  while( (target = BTL_POKESET_SeekNext(targetRec)) != NULL )
  {
    if( scproc_WazaRankEffect_Common(wk, wazaParam, attacker, target, TRUE) )
    {
      wazaEffCtrl_SetEnable( &wk->wazaEffCtrl );
    }
  }
}
//--------------------------------------------------------------------------
/**
 * ���U�ɂ��i���ځE�ǉ����ʁj�����N�������ʂ̏���
 *
 * @param   wk
 * @param   waza
 * @param   attacker
 * @param   target
 *
 * @retval  BOOL    �����N�������ʂ�����������TRUE
 */
//--------------------------------------------------------------------------
static BOOL scproc_WazaRankEffect_Common( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, BOOL fAlmost )
{
  BOOL ret = FALSE;
  u32 eff_cnt, i;
  u8 atkPokeID = BPP_GetID( attacker );

  eff_cnt = WAZADATA_GetRankEffectCount( wazaParam->wazaID );
  BTL_Printf("���U:%d�̃G�t�F�N�g��=%d\n", wazaParam->wazaID, eff_cnt);
  for(i=0; i<eff_cnt; ++i)
  {
    WazaRankEffect  effect;
    int  volume;
    BOOL fEffective = FALSE;

    scEvent_GetWazaRankEffectValue( wk, wazaParam->wazaID, i, attacker, target, &effect, &volume );
    if( effect != WAZA_RANKEFF_NULL )
    {
      if( effect != WAZA_RANKEFF_MULTI5 )
      {
        fEffective = scproc_RankEffectCore( wk, atkPokeID, target, effect, volume, atkPokeID, ITEM_DUMMY_DATA, fAlmost, TRUE );
      }
      else
      {
        u8 e;
        for( e=WAZA_RANKEFF_ORIGIN; e<WAZA_RANKEFF_BASE_MAX; ++e )
        {
          if( scproc_RankEffectCore(wk, atkPokeID, target, e, volume, atkPokeID,
              ITEM_DUMMY_DATA, fAlmost, TRUE)
          ){
            fEffective = TRUE;
          }
        }
      }
      if( fEffective )
      {
        u32 hem_state = Hem_PushState( &wk->HEManager );
        scEvent_WazaRankEffectFixed( wk, target, wazaParam->wazaID, effect, volume );
        scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
        Hem_PopState( &wk->HEManager, hem_state );
        ret = TRUE;
      }
    }
  }
  return ret;
}
//--------------------------------------------------------------------------
/**
 * [Event] ���U�ɂ��i���ځE�ǉ��j�����N�������ʂ��擾
 *
 * @param   wk
 * @param   waza
 * @param   idx
 * @param   attacker
 * @param   target
 * @param   effect    [out] ���ʎ��
 * @param   volume    [out] ���ʒl
 *
 */
//--------------------------------------------------------------------------
static void scEvent_GetWazaRankEffectValue( BTL_SVFLOW_WORK* wk, WazaID waza, u32 idx,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, WazaRankEffect* effect, int* volume )
{
  *effect = WAZADATA_GetRankEffect( waza, idx, volume );
  BTL_Printf("���U[%d]�̃����N����[%d] type=%d, volume=%d\n", waza, idx, *effect, *volume);

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(target) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_STATUS_TYPE, *effect );
    BTL_EVENTVAR_SetValue( BTL_EVAR_VOLUME, *volume );
    BTL_EVENTVAR_SetValue( BTL_EVAR_RATIO, 1 );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_GET_RANKEFF_VALUE );
    *effect = BTL_EVENTVAR_GetValue( BTL_EVAR_STATUS_TYPE );
    *volume = BTL_EVENTVAR_GetValue( BTL_EVAR_VOLUME );
    {
      u8 ratio = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
      if( ratio > 1 ){
        (*volume) *= ratio;
      }
    }
  BTL_EVENTVAR_Pop();

  if( *effect == WAZA_RANKEFF_SP ){
    *effect = WAZA_RANKEFF_NULL;
  };
}
//--------------------------------------------------------------------------
/**
 * �����N�������ʂ̃R�A�i���U�ȊO��������Ăяo���j
 *
 * @param   wk
 * @param   target
 * @param   effect
 * @param   volume
 * @param   wazaUsePokeID    ���U�ɂ�郉���N�����Ȃ烏�U�g�p�҃|�PID�i����ȊO�� BTL_POKEID_NULL�j
 * @param   itemID    �A�C�e���g�p�ɂ�郉���N�����Ȃ�A�C�e��ID�w��i����ȊO�� ITEM_DUMMY_DATA ���w��j
 * @param   fAlmost   ����v���Ō��ʎ��s�������A���̃��b�Z�[�W��\������
 * @param   fStdMsg   �W�����b�Z�[�W���o�͂���i�����́~�~�����������I�Ȃǁj
 *
 * @retval  BOOL    �����N�������ʂ�����������TRUE
 */
//--------------------------------------------------------------------------
static BOOL scproc_RankEffectCore( BTL_SVFLOW_WORK* wk, u8 atkPokeID, BTL_POKEPARAM* target,
  WazaRankEffect effect, int volume, u8 wazaUsePokeID, u16 itemID, BOOL fAlmost, BOOL fStdMsg )
{
  volume = scEvent_RankValueChange( wk, target, effect, wazaUsePokeID, itemID, volume );


  BTL_Printf("�|�P[%d]�̔\�̓����N(%d)��%d�i�K�A�㏸������\n", BPP_GetID(target), effect, volume );
  if( !BPP_IsRankEffectValid(target, effect, volume) ){
    if( fAlmost ){
      scPut_RankEffectLimit( wk, target, effect, volume );
    }
    return FALSE;
  }

  {
    BOOL ret = TRUE;

    if( scEvent_CheckRankEffectSuccess(wk, target, effect, wazaUsePokeID, volume) )
    {
      // �����܂ŗ����烉���N���ʔ���
      BTL_Printf("�����N���ʔ����Ftype=%d, volume=%d, itemID=%d\n", effect, volume, itemID );
      scPut_RankEffect( wk, target, effect, volume, itemID, fStdMsg );
      {
        u32 hem_state = Hem_PushState( &wk->HEManager );
        scEvent_RankEffect_Fix( wk, atkPokeID, target, effect, volume );
        scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
        Hem_PopState( &wk->HEManager, hem_state );
      }
    }
    else
    {
      BTL_Printf("�����N���ʎ��s\n");
      if( fAlmost )
      {
        u32 hem_state = Hem_PushState( &wk->HEManager );
        scEvent_RankEffect_Failed( wk, target );
        scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
        Hem_PopState( &wk->HEManager, hem_state );
      }
      ret = FALSE;
    }

    return ret;
  }
}
//----------------------------------------------------------------------------------
/**
 * [Event] �����N���ʂ̑����l��ω�������
 *
 * @param   wk
 * @param   target
 * @param   rankType
 * @param   wazaUsePokeID
 * @param   itemID
 * @param   volume
 *
 * @retval  int   �ω���̑����l
 */
//----------------------------------------------------------------------------------
static int scEvent_RankValueChange( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, WazaRankEffect rankType,
  u8 wazaUsePokeID, u16 itemID, int volume )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(target) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, wazaUsePokeID );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_STATUS_TYPE, rankType );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_VOLUME, volume );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_RANKVALUE_CHANGE );
    volume  = BTL_EVENTVAR_GetValue( BTL_EVAR_VOLUME );
  BTL_EVENTVAR_Pop();
  return volume;
}

//---------------------------------------------------------------------------------------------
// �T�[�o�[�t���[�F�����N���ʁ���Ԉُ�𓯎��ɗ^���郏�U
//---------------------------------------------------------------------------------------------
static void scproc_Fight_EffectSick( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKESET* targetRec )
{
  WazaSick sick;
  WAZA_SICKCONT_PARAM contParam;
  BPP_SICK_CONT sickCont;
  BTL_POKEPARAM* target;

  sick = WAZADATA_GetParam( wazaParam->wazaID, WAZAPARAM_SICK );
  contParam = WAZADATA_GetSickCont( wazaParam->wazaID );

  BTL_CALC_WazaSickContToBppSickCont( contParam, attacker, &sickCont );

  BTL_POKESET_SeekStart( targetRec );
  while( (target = BTL_POKESET_SeekNext(targetRec)) != NULL )
  {
    if( scproc_WazaRankEffect_Common(wk, wazaParam, attacker, target, TRUE) ){
      wazaEffCtrl_SetEnable( &wk->wazaEffCtrl );
    }
    if( scproc_Fight_WazaSickCore(wk, attacker, target, wazaParam->wazaID, sick, sickCont, FALSE) ){
      wazaEffCtrl_SetEnable( &wk->wazaEffCtrl );
    }
  }
}
//---------------------------------------------------------------------------------------------
// �T�[�o�[�t���[�FHP��
//---------------------------------------------------------------------------------------------
static void scproc_Fight_SimpleRecover( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, BTL_POKESET* targetRec )
{
  BTL_POKEPARAM* target;
  u32 recoverHP;
  u8  pokeID;

  BTL_POKESET_SeekStart( targetRec );
  while( (target = BTL_POKESET_SeekNext(targetRec)) != NULL )
  {
    recoverHP = scEvent_CalcRecoverHP( wk, waza, target );
    pokeID = BPP_GetID( target );
    if( scproc_RecoverHP(wk, target, recoverHP) )
    {
      wazaEffCtrl_SetEnable( &wk->wazaEffCtrl );
      SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_HP_Recover, pokeID );
    }
    else
    {
      // @todo HPREC_MSG
      SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_NoEffect, pokeID );
    }
  }
}
static BOOL scproc_RecoverHP( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 recoverHP )
{
  if( !BPP_CheckSick(bpp, WAZASICK_KAIHUKUHUUJI)
  &&  !BPP_IsDead(bpp)
  &&  !BPP_IsHPFull(bpp)
  ){
    u8 pokeID = BPP_GetID( bpp );
    BtlPokePos pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, pokeID );
    if( pos != BTL_POS_NULL ){
      SCQUE_PUT_ACT_EffectByPos( wk->que, pos, BTLEFF_HP_RECOVER );
    }

    scPut_SimpleHp( wk, bpp, recoverHP, TRUE );
    return TRUE;
  }
  return FALSE;
}
//---------------------------------------------------------------------------------------------
// �T�[�o�[�t���[�F�ꌂ���U����
//---------------------------------------------------------------------------------------------
static void scproc_Fight_Ichigeki( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKESET* targets )
{
  BTL_POKEPARAM* target;
  u32 i = 0;

  while( 1 )
  {
    target = BTL_POKESET_Get( targets, i++ );
    if( target == NULL ){ break; }
    if( !BPP_IsDead(target) )
    {
      u8 atkLevel = BPP_GetValue( attacker, BPP_LEVEL );
      u8 defLevel = BPP_GetValue( target, BPP_LEVEL );
      u8 targetPokeID = BPP_GetID( target );

      if( IsTripleFarPos(wk, attacker, target, wazaParam->wazaID) ){
        scput_WazaNoEffect( wk, target );
      }
      else if( atkLevel < defLevel ){
        scput_WazaNoEffect( wk, target );
      }
      else if( scProc_checkWazaDamageAffinity(wk, attacker, target, wazaParam) == BTL_TYPEAFF_0 ){
        scput_WazaNoEffect( wk, target );
      }
      else
      {
        u32 hem_state = Hem_PushState( &wk->HEManager );

        if( scEvent_IchigekiCheck(wk, attacker, target, wazaParam->wazaID) )
        {
          wazaEffCtrl_SetEnable( &wk->wazaEffCtrl );
          scPut_Ichigeki( wk, target );
          scproc_CheckDeadCmd( wk, target );
        }
        else
        {
          if( !scproc_HandEx_Root(wk, ITEM_DUMMY_DATA) ){
            scPut_WazaAvoid( wk, target, wazaParam->wazaID );
          }
        }

        Hem_PopState( &wk->HEManager, hem_state );
      }
    }
  }
}
//---------------------------------------------------------------------------------------------
// �T�[�o�[�t���[�F�ӂ��Ƃ΂����U����
//---------------------------------------------------------------------------------------------
static void scproc_Fight_PushOut( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, BTL_POKESET* targetRec )
{
  BTL_POKEPARAM *target;

  BTL_POKESET_SeekStart( targetRec );
  while( (target = BTL_POKESET_SeekNext(targetRec)) != NULL )
  {
    if( scproc_PushOutCore(wk, attacker, target, FALSE, NULL) ){
      wazaEffCtrl_SetEnable( &wk->wazaEffCtrl );
    }else{
      scPut_WazaFail( wk, attacker, waza );
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * �ӂ��Ƃ΂������R�A
 *
 * @param   wk
 * @param   attacker
 * @param   target
 * @param   fForceChange  �����I�ɓ���ւ����[�h�Ŏ��s�iFALSE�Ȃ烋�[�����ɉ����Ď������ʁj
 * @param   succeedMsg    �������o�̓��b�Z�[�W�p�����[�^
 *
 * @retval  BOOL    ������TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scproc_PushOutCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target,
  BOOL fForceChange, BTL_HANDEX_STR_PARAMS* succeedMsg )
{
  PushOutEffect   eff;

  if( fForceChange ){
    eff = PUSHOUT_EFFECT_CHANGE;
  }else{
    eff = check_pushout_effect( wk );
  }

  if( eff == PUSHOUT_EFFECT_MUSTFAIL ){
    return FALSE;
  }else{

    BtlPokePos targetPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, BPP_GetID(target) );

    if( BPP_IsDead(target) ){
      return FALSE;
    }

    if( scEvent_CheckPushOutFail(wk, attacker, target) ){
      return FALSE;
    }

    {
      u8 clientID, posIdx;
      BTL_MAIN_BtlPosToClientID_and_PosIdx( wk->mainModule, targetPos, &clientID, &posIdx );

      if( eff == PUSHOUT_EFFECT_CHANGE )
      {
        SVCL_WORK* clwk;
        clwk = BTL_SERVER_GetClientWork( wk->server, clientID );
        if( BTL_PARTY_GetAliveMemberCount(clwk->party) <= clwk->numCoverPos )
        {
          return FALSE;
        }
        else
        {
          u8 nextPokeIdx = get_pushout_nextpoke_idx( wk, clwk );
          BTL_POKEPARAM* nextPoke = BTL_POKECON_GetClientPokeData( wk->pokeCon, clientID, nextPokeIdx );
          u8 nextPokeID;

          nextPokeID = BPP_GetID( nextPoke );

          scproc_MemberOutCore( wk, target );
          if( succeedMsg != NULL ){
            handexSub_putString( wk, succeedMsg );
          }
          scproc_MemberInForChange( wk, clientID, posIdx, nextPokeIdx, FALSE );
          SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_PushOutChange, nextPokeID );
          scproc_AfterMemberIn( wk );
        }
      }
      else
      {
        scproc_MemberOutCore( wk, target );
        wk->flowResult = SVFLOW_RESULT_BTL_QUIT;
      }
    }
    return TRUE;
  }
}
//--------------------------------------------------------------------------
/**
 * �u�ӂ��Ƃ΂��v�n�̃��U���ʂ��o�g���̃��[�������画�肷��
 */
//--------------------------------------------------------------------------
static PushOutEffect check_pushout_effect( BTL_SVFLOW_WORK* wk )
{
  BtlRule rule = BTL_MAIN_GetRule( wk->mainModule );
  BtlCompetitor competitor = BTL_MAIN_GetCompetitor( wk->mainModule );

  switch( rule ){
  case BTL_RULE_SINGLE:
    if( competitor == BTL_COMPETITOR_WILD ){
      return PUSHOUT_EFFECT_ESCAPE;
    }else{
      return PUSHOUT_EFFECT_CHANGE;
    }

  default:
    return PUSHOUT_EFFECT_CHANGE;
  }
}
//--------------------------------------------------------------------------
/**
 * �u�ӂ��Ƃ΂��v�n�̃��U�ŋ������ꂩ���������������A
 * ���ɏo���|�P�����̃p�[�e�B���C���f�b�N�X�������_���Ō��߂�
 *
 * @param   wk
 * @param   clwk
 *
 * @retval  u8
 */
//--------------------------------------------------------------------------
static u8 get_pushout_nextpoke_idx( BTL_SVFLOW_WORK* wk, const SVCL_WORK* clwk )
{
  const BTL_POKEPARAM* bpp;
  u32 i, count, members;
  u8 list[ BTL_PARTY_MEMBER_MAX ];

  members = BTL_PARTY_GetMemberCount( clwk->party );
  for(i=clwk->numCoverPos, count=0; i<members; ++i)
  {
    bpp = BTL_PARTY_GetMemberDataConst( clwk->party, i );
    if( IsBppExist(bpp) ){
      list[count++] = i;
    }
  }
  GF_ASSERT( count );

  {
    i = BTL_CALC_GetRand( count );
    return list[ i ];
  }
}

//--------------------------------------------------------------------------
/**
 * [Event] �ӂ��Ƃ΂����U���s�`�F�b�N
 *
 * @param   wk
 * @param   attacker
 * @param   target
 *
 * @retval  BOOL      ���s�Ȃ�TRUE
 */
//--------------------------------------------------------------------------
static BOOL scEvent_CheckPushOutFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target )
{
  BOOL failFlag = FALSE;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, failFlag );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(target) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHECK_PUSHOUT );
    BTL_SICKEVENT_CheckPushOutFail( wk, target );
    failFlag = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  BTL_EVENTVAR_Pop();
  return failFlag;
}
//---------------------------------------------------------------------------------------------
// �T�[�o�[�t���[�F�t�B�[���h�G�t�F�N�g����
//---------------------------------------------------------------------------------------------
static void scput_Fight_FieldEffect( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker )
{
  BtlWeather  weather = WAZADATA_GetWeather( wazaParam->wazaID );
  if( weather != BTL_WEATHER_NONE )
  {
    u8 turn_upcnt = scEvent_WazaWeatherTurnUp( wk, weather, attacker );

    if( scproc_ChangeWeatherCore( wk, weather, BTL_WEATHER_TURN_DEFAULT+turn_upcnt ) ){
      wazaEffCtrl_SetEnable( &wk->wazaEffCtrl );
    }else{
      scPut_WazaFail( wk, attacker, wazaParam->wazaID );
    }
  }
  else
  {
    u32 hem_state = Hem_PushState( &wk->HEManager );
    BOOL fSucceed;
    scEvent_FieldEffectCall( wk, attacker, wazaParam->wazaID );
    fSucceed = scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    Hem_PopState( &wk->HEManager, hem_state );

    if( fSucceed ){
      wazaEffCtrl_SetEnable( &wk->wazaEffCtrl );
    }else{
      scPut_WazaFail( wk, attacker, wazaParam->wazaID );
    }
  }
}
/**
 *  �V��ω��R�A
 */
static BOOL scproc_ChangeWeatherCore( BTL_SVFLOW_WORK* wk, BtlWeather weather, u8 turn )
{
  if( BTL_FIELD_GetWeather() == weather ){
    return FALSE;
  }
  else
  {
//    BOOL result = scEvent_CheckChangeWeather( wk, weather, &turn );
//BTL_EVENT_WAZA_WEATHER_TURNCNT
    BTL_FIELD_SetWeather( weather, turn );
    SCQUE_PUT_ACT_WeatherStart( wk->que, weather );

    {
      u32 hem_state = Hem_PushState( &wk->HEManager );
      scEvent_AfterChangeWeather( wk, weather );
      scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
      Hem_PopState( &wk->HEManager, hem_state );
    }
    return TRUE;
  }
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U�ɂ��V��ω��̃^�[���������`�F�b�N
 *
 * @param   wk
 * @param   weather
 * @param   wk
 * @param   weather
 * @param   turn
 *
 * @retval  u8
 */
//----------------------------------------------------------------------------------
static u8 scEvent_WazaWeatherTurnUp( BTL_SVFLOW_WORK* wk, BtlWeather weather, const BTL_POKEPARAM* attacker )
{
  u8 cnt;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WEATHER, weather );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_TURN_COUNT, 0 );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_WEATHER_TURNCNT );
    cnt = BTL_EVENTVAR_GetValue( BTL_EVAR_TURN_COUNT );
  BTL_EVENTVAR_Pop();
  return cnt;
}


//--------------------------------------------------------------------------
/**
 * �yEvent�z�V��̕ω��`�F�b�N
 *
 * @param   wk
 * @param   weather �V��
 * @param   turn    [io]�p���^�[����
 *
 * @retval  BOOL    �ω�����ꍇ��TRUE
 */
//--------------------------------------------------------------------------
static BOOL scEvent_CheckChangeWeather( BTL_SVFLOW_WORK* wk, BtlWeather weather, u8* turn )
{
  GF_ASSERT(weather != BTL_WEATHER_NONE);

  {
    BOOL fFail;

    BTL_EVENTVAR_Push();
      BTL_EVENTVAR_SetValue( BTL_EVAR_WEATHER, weather );
      BTL_EVENTVAR_SetValue( BTL_EVAR_TURN_COUNT, *turn );
      BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, FALSE );
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_WEATHER_CHANGE );
      weather = BTL_EVENTVAR_GetValue( BTL_EVAR_WEATHER );
      *turn = BTL_EVENTVAR_GetValue( BTL_EVAR_TURN_COUNT );
      fFail = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
    BTL_EVENTVAR_Pop();
    return !fFail;
  }
}
static BOOL scproc_FieldEffectCore( BTL_SVFLOW_WORK* wk, BtlFieldEffect effect, BPP_SICK_CONT contParam )
{
  if( BTL_FIELD_AddEffect(effect, contParam) ){
    SCQUE_PUT_OP_AddFieldEffect( wk->que, effect, contParam.raw );
    return TRUE;
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * [Event] �t�B�[���h�G�t�F�N�g�ǉ������Ăяo��
 *
 * @param   wk
 * @param   attacker
 * @param   target
 *
 */
//----------------------------------------------------------------------------------
static void scEvent_FieldEffectCall( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza )
{
  BOOL failFlag = FALSE;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_FIELD_EFFECT_CALL );
  BTL_EVENTVAR_Pop();
}

//---------------------------------------------------------------------------------------------
// �T�[�o�[�t���[�F���ނ�����Ȃ����U
//---------------------------------------------------------------------------------------------
static void scput_Fight_Uncategory( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKESET* targets )
{
  BTL_POKEPARAM* bpp;

  // �^�[�Q�b�g�͋����͂��������������̗��R�Ŏc���Ă��Ȃ��ꍇ�͏I��
  if( BTL_POKESET_IsRemovedAll(targets) ){
    return;
  }

  switch( wazaParam->wazaID ){
  case WAZANO_SUKIRUSUWAPPU:
    scput_Fight_Uncategory_SkillSwap( wk, attacker, targets );
    break;
  case WAZANO_MIGAWARI:
    scproc_Migawari_Create( wk, attacker );
    break;
  default:
    {
      u32 hem_state = Hem_PushState( &wk->HEManager );
      BOOL fEffective = FALSE;
      if( scEvent_UnCategoryWaza(wk, wazaParam, attacker, targets) ){
        if( scproc_HandEx_Root( wk, ITEM_DUMMY_DATA ) ){
          wazaEffCtrl_SetEnable( &wk->wazaEffCtrl );
          fEffective = TRUE;
        }
      }

      if( !fEffective ){
        SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_WazaFail );
      }

      Hem_PopState( &wk->HEManager, hem_state );
    }
    break;
  }
}
// �X�L���X���b�v
static void scput_Fight_Uncategory_SkillSwap( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BTL_POKESET* targetRec )
{
  BTL_POKEPARAM* target = BTL_POKESET_Get( targetRec, 0 );
  PokeTokusei atk_tok, tgt_tok;

  atk_tok = BPP_GetValue( attacker, BPP_TOKUSEI_EFFECTIVE );
  tgt_tok = BPP_GetValue( target, BPP_TOKUSEI_EFFECTIVE );

  if( (!BTL_CALC_TOK_CheckCantChange(atk_tok)) && (!BTL_CALC_TOK_CheckCantChange(tgt_tok)) )
  {
    u8 atkPokeID = BPP_GetID( attacker );
    u8 tgtPokeID = BPP_GetID( target );

    wazaEffCtrl_SetEnable( &wk->wazaEffCtrl );
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_SkillSwap, atkPokeID );

    {
      u32 hem_state = Hem_PushState( &wk->HEManager );
      scEvent_ChangeTokuseiBefore( wk, atkPokeID, tgt_tok );
      scEvent_ChangeTokuseiBefore( wk, tgtPokeID, atk_tok );
      scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
      Hem_PopState( &wk->HEManager, hem_state );
    }

    BPP_ChangeTokusei( attacker, tgt_tok );
    BPP_ChangeTokusei( target, atk_tok );
    SCQUE_PUT_OP_ChangeTokusei( wk->que, atkPokeID, tgt_tok );
    SCQUE_PUT_OP_ChangeTokusei( wk->que, tgtPokeID, atk_tok );
    BTL_HANDLER_TOKUSEI_Swap( attacker, target );

    {
      u32 hem_state = Hem_PushState( &wk->HEManager );
      scEvent_ChangeTokuseiAfter( wk, atkPokeID );
      scEvent_ChangeTokuseiAfter( wk, tgtPokeID );
      scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
      Hem_PopState( &wk->HEManager, hem_state );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �݂���� - �쐬
 *
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
static void scproc_Migawari_Create( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  if( !BPP_MIGAWARI_IsExist(bpp) ){
    int migawariHP = BTL_CALC_QuotMaxHP( bpp, 4 );
    if( BPP_GetValue(bpp, BPP_HP) > migawariHP )
    {
      scPut_SimpleHp( wk, bpp, -migawariHP, TRUE );
      scproc_CheckItemReaction( wk, bpp );

      BPP_MIGAWARI_Create( bpp, migawariHP );
      SCQUE_PUT_OP_MigawariCreate( wk->que, BPP_GetID(bpp), migawariHP );
      // @todo �����ł��Ԃ�g����쐬�G�t�F�N�g�R�}���h
    }
    else{
      scPut_Message_StdEx( wk, BTL_STRID_STD_MigawariFail, 0, NULL );
    }
  }
  else{
    // ���łɏo�Ă������b�Z�[�W
    scPut_Message_Set( wk, bpp, BTL_STRID_SET_MigawariExist );
  }
}
//----------------------------------------------------------------------------------
/**
 * �݂���� - �_���[�W����
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  BOOL  �݂���肪��������TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scproc_Migawari_Damage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 damage )
{
  scPut_Message_Set( wk, bpp, BTL_STRID_SET_MigawariDamage );
  if( BPP_MIGAWARI_AddDamage(bpp, damage) )
  {
     SCQUE_PUT_OP_MigawariDelete( wk->que, BPP_GetID(bpp) );
     // @@@ �����ł��Ԃ�g����폜�G�t�F�N�g�R�}���h
     scPut_Message_Set( wk, bpp, BTL_STRID_SET_MigawariDestract );
     return TRUE;
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * �݂���� - �_���[�W���U�ȊO�̖���������
 *
 * @param   wk
 * @param   wazaParam
 * @param   attacker    �U���|�P�����f�[�^
 * @param   rec         [io] ���U�Ώۃ|�P�f�[�^�Q�B���������ꂽ��Y���|�P�����f�[�^�͏��������B
 */
//----------------------------------------------------------------------------------
static void scproc_Migawari_CheckNoEffect( BTL_SVFLOW_WORK* wk, SVFL_WAZAPARAM* wazaParam,
  BTL_POKEPARAM* attacker, BTL_POKESET* rec )
{
  BTL_POKEPARAM* bpp;

  BTL_POKESET_SeekStart( rec );
  while( (bpp = BTL_POKESET_SeekNext(rec)) != NULL )
  {
    if( (bpp != attacker)
    &&  (BPP_MIGAWARI_IsExist(bpp))
    ){
      BTL_POKESET_Remove( rec, bpp );
      SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_NoEffect, BPP_GetID(bpp) );
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * [Event] �����ރ��U�����R�[��
 *
 * @param   wk
 * @param   waza
 * @param   attacker
 * @param   targets
 *
 * @retval  BOOL      �����������U�n���h���������TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_UnCategoryWaza( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
  const BTL_POKEPARAM* attacker, BTL_POKESET* targets )
{
  // @TODO �{���̓X�^�b�N�|�C���^�̒l�ł͂Ȃ��A���m�Ƀn���h�������琬�ۃt���O���󂯎��ׂ�
  u16 sp = Hem_GetStackPtr( &wk->HEManager );

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    {
      BTL_POKEPARAM* bpp;
      u8 targetMax, targetCnt, i;

      targetMax = BTL_POKESET_GetCountMax( targets );
      targetCnt = BTL_POKESET_GetCount( targets );
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_TARGET_POKECNT, targetCnt );

      BTL_N_Printf( DBGSTR_SVFL_UncategoryWazaInfo, BPP_GetID(attacker), targetCnt, targetMax);

      for(i=0; i<targetCnt; ++i){
        bpp = BTL_POKESET_Get( targets, i );
        BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_TARGET1+i, BPP_GetID(bpp) );
      }
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
      if( targetMax )
      {
        if( targetCnt ){
          BTL_EVENT_CallHandlers( wk, BTL_EVENT_UNCATEGORIZE_WAZA );
        }
      }
      else{
        BTL_EVENT_CallHandlers( wk, BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET );
      }
    }
  BTL_EVENTVAR_Pop();

  return (sp != Hem_GetStackPtr(&wk->HEManager));
}

//==============================================================================
// �T�[�o�[�t���[�F�^�[���`�F�b�N���[�g
//==============================================================================
static void scproc_TurnCheck( BTL_SVFLOW_WORK* wk )
{
  // ����POKESET�ɑΏۃ|�P�������i�[����
  BTL_POKESET* pokeSet = &wk->pokesetTarget;
  BTL_POKESET_Clear( pokeSet );

  // POKESET�i�[->�f�������\�[�g
  {
    FRONT_POKE_SEEK_WORK  fps;
    BTL_POKEPARAM* bpp;

    FRONT_POKE_SEEK_InitWork( &fps, wk );
    while( FRONT_POKE_SEEK_GetNext(&fps, wk, &bpp) ){
      BTL_POKESET_Add( pokeSet, bpp );
    }
    BTL_POKESET_SortByAgility( pokeSet, wk );
  }

  scproc_turncheck_sub( wk, pokeSet, BTL_EVENT_TURNCHECK_BEGIN );

  scproc_turncheck_CommSupport( wk );

  scproc_turncheck_sick( wk, pokeSet );
  scproc_turncheck_side( wk );
  scproc_turncheck_field( wk );
  scproc_turncheck_weather( wk, pokeSet );

  scproc_turncheck_sub( wk, pokeSet, BTL_EVENT_TURNCHECK_END );

  // �S�|�P�����̃^�[���t���O���N���A
  {
    BTL_POKEPARAM* bpp;
    BTL_POKESET_SeekStart( pokeSet );
    while( (bpp = BTL_POKESET_SeekNext(pokeSet)) != NULL ){
      BPP_TurnCheck( bpp );
    }
  }

  if( wk->turnCount < BTL_TURNCOUNT_MAX ){
    wk->turnCount++;
  }
}
/**
 *  �^�[���`�F�b�N�F�ʐM�N���҂ɂ��T�|�[�g
 */
static void   scproc_turncheck_CommSupport( BTL_SVFLOW_WORK* wk )
{
  COMM_PLAYER_SUPPORT* supportHandle = BTL_MAIN_GetCommSupportHandle( wk->mainModule );
  if( supportHandle )
  {
    SUPPORT_TYPE support_type;

// �e�X�g�p�ɋ����n�m���������Ɏg����
//    COMM_PLAYER_SUPPORT_SetParam( supportHandle, SUPPORT_TYPE_RECOVER_HALF, BTL_MAIN_GetPlayerStatus(wk->mainModule) );

    support_type = COMM_PLAYER_SUPPORT_GetSupportType( supportHandle );
    if( support_type != SUPPORT_TYPE_NULL )
    {
      u8 clientID = BTL_MAIN_GetPlayerClientID( wk->mainModule );
      SVCL_WORK* clwk = BTL_SERVER_GetClientWork( wk->server, clientID );
      u8 pokeIDAry[ BTL_POSIDX_MAX ];
      u8 cnt, i;
      BTL_POKEPARAM* bpp;

      // �����Ă�HP�����^���ł͂Ȃ��|�P�̂ݗ�
      for(i=0, cnt=0; i<clwk->numCoverPos; ++i)
      {
        bpp = BTL_POKECON_GetClientPokeData( wk->pokeCon, clientID, i );
        if( IsBppExist(bpp) && !BPP_IsHPFull(bpp) )
        {
          pokeIDAry[ cnt++ ] = BPP_GetID( bpp );
        }
      }
      if( cnt )
      {
        u32 recoverHP;
        u8 pokeID = pokeIDAry[ GFL_STD_MtRand(cnt) ];
        bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, pokeID );
        recoverHP = BPP_GetValue( bpp, BPP_MAX_HP );
        if( support_type == SUPPORT_TYPE_RECOVER_HALF ){
          recoverHP /= 2;
        }
        if( recoverHP ){
          scproc_RecoverHP( wk, bpp, recoverHP );
          SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_CommSupport, BTL_CLIENTID_COMM_SUPPORT, pokeID );
          COMM_PLAYER_SUPPORT_Init( supportHandle );
        }
      }
    }
  }

}
/**
 *  �^�[���`�F�b�N�F�J�n�E�I��
 */
static void scproc_turncheck_sub( BTL_SVFLOW_WORK* wk, BTL_POKESET* pokeSet, BtlEventType event_type )
{
  BTL_POKEPARAM* bpp;
  BTL_POKESET_SeekStart( pokeSet );

  while( (bpp = BTL_POKESET_SeekNext(pokeSet)) != NULL )
  {
    {
      u32 hem_state = Hem_PushState( &wk->HEManager );
      scEvent_TurnCheck( wk, bpp, event_type );
      scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
      Hem_PopState( &wk->HEManager, hem_state );

      scproc_CheckExpGet( wk );
    }
  }
}
static void scEvent_TurnCheck( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, BtlEventType event_type )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENT_CallHandlers( wk, event_type );
  BTL_EVENTVAR_Pop();
}

//------------------------------------------------------------------
// �T�[�o�[�t���[�������F �^�[���`�F�b�N > ��Ԉُ�
//------------------------------------------------------------------
static void scproc_turncheck_sick( BTL_SVFLOW_WORK* wk, BTL_POKESET* pokeSet )
{
  BTL_POKEPARAM* bpp;
  u32 hem_state;

  BTL_POKESET_SeekStart( pokeSet );
  while( (bpp = BTL_POKESET_SeekNext(pokeSet)) != NULL )
  {
    hem_state = Hem_PushState( &wk->HEManager );

    BPP_WazaSick_TurnCheck( bpp, BTL_SICK_TurnCheckCallback, wk );
    SCQUE_PUT_OP_WazaSickTurnCheck( wk->que, BPP_GetID(bpp) );

    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    Hem_PopState( &wk->HEManager, hem_state );

    scproc_CheckExpGet( wk );
  }
}
//--------------------------------------------------------------------------------
/**
 * �^�[���`�F�b�N�ŏ�Ԉُ�ɂ��_���[�W���������邽�тɃR�[���o�b�N�����
 *
 * @param   wk
 * @param   bpp
 * @param   sickID
 * @param   damage
 */
//--------------------------------------------------------------------------------
void BTL_SVF_SickDamageRecall( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, WazaSick sickID, u32 damage )
{
  u32 hem_state = Hem_PushState( &wk->HEManager );
  damage = scEvent_SickDamage( wk, bpp, sickID, damage );
  if( damage )
  {
    HANDEX_STR_Clear( &wk->strParam );
    switch( sickID ){
    case WAZASICK_DOKU:
    case WAZASICK_DOKUDOKU:
      scPut_EffectByPokePos( wk, bpp, BTLEFF_DOKU );
      break;
    case WAZASICK_YAKEDO:
      scPut_EffectByPokePos( wk, bpp, BTLEFF_YAKEDO );
      break;
    }
    BTL_SICK_MakeSickDamageMsg( &wk->strParam, bpp, sickID );
    scproc_SimpleDamage( wk, bpp, damage, &wk->strParam );
  }
  else{
    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
  }
  Hem_PopState( &wk->HEManager, hem_state );
}
//----------------------------------------------------------------------------------
/**
 * [Event] ��Ԉُ�ɂ��^�[���`�F�b�N�_���[�W
 *
 * @param   wk
 * @param   bpp
 * @param   sickID
 * @param   damage
 */
//----------------------------------------------------------------------------------
static u32 scEvent_SickDamage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, WazaSick sickID, u32 damage )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_SICKID, sickID );
    BTL_EVENTVAR_SetValue( BTL_EVAR_DAMAGE, damage );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_SICK_DAMAGE );
    damage = BTL_EVENTVAR_GetValue( BTL_EVAR_DAMAGE );
  BTL_EVENTVAR_Pop();
  return damage;
}

//------------------------------------------------------------------
// �T�[�o�[�t���[�������F �^�[���`�F�b�N > �T�C�h�G�t�F�N�g
//------------------------------------------------------------------
static void scproc_turncheck_side( BTL_SVFLOW_WORK* wk )
{
  BTL_HANDLER_SIDE_TurnCheck( scproc_turncheck_side_callback, wk );
}
static void scproc_turncheck_side_callback( BtlSide side, BtlSideEffect sideEffect, void* arg )
{
  BTL_SVFLOW_WORK* wk = arg;
  int strID = -1;
  switch( sideEffect ){
  case BTL_SIDEEFF_REFRECTOR:     strID = BTL_STRID_STD_ReflectorOff; break;
  case BTL_SIDEEFF_HIKARINOKABE:  strID = BTL_STRID_STD_HikariNoKabeOff; break;
  case BTL_SIDEEFF_SINPINOMAMORI: strID = BTL_STRID_STD_SinpiNoMamoriOff; break;
  case BTL_SIDEEFF_SIROIKIRI:     strID = BTL_STRID_STD_SiroiKiriOff; break;
  case BTL_SIDEEFF_OIKAZE:        strID = BTL_STRID_STD_OikazeOff; break;
  case BTL_SIDEEFF_OMAJINAI:      strID = BTL_STRID_STD_OmajinaiOff; break;
  }
  if( strID >= 0 )
  {
    SCQUE_PUT_MSG_STD( wk->que, strID, side );
  }
}
//------------------------------------------------------------------
// �T�[�o�[�t���[�������F �^�[���`�F�b�N > �t�B�[���h�G�t�F�N�g
//------------------------------------------------------------------
static void scproc_turncheck_field( BTL_SVFLOW_WORK* wk )
{
  BTL_FIELD_TurnCheck( turncheck_field_callback, wk );
}
static void turncheck_field_callback( BtlFieldEffect effect, void* arg )
{
  BTL_SVFLOW_WORK* wk = arg;

  scproc_FieldEff_End( wk, effect );
}

static void scproc_FieldEff_End( BTL_SVFLOW_WORK* wk, BtlFieldEffect effect )
{
  int strID = -1;
  switch( effect ){
  case BTL_FLDEFF_TRICKROOM:   strID = BTL_STRID_STD_TrickRoomOff; break; ///< �g���b�N���[��
  case BTL_FLDEFF_JURYOKU:     strID = BTL_STRID_STD_JyuryokuOff; break;  ///< ���イ��傭
  case BTL_FLDEFF_WONDERROOM:  strID = BTL_STRID_STD_WonderRoom_End; break;
  case BTL_FLDEFF_MAGICROOM:   strID = BTL_STRID_STD_MagicRoom_End; break;
  }

  if( strID >= 0 )
  {
    SCQUE_PUT_MSG_STD( wk->que, strID );
  }
  SCQUE_PUT_OP_RemoveFieldEffect( wk->que, effect );
}

//------------------------------------------------------------------
// �T�[�o�[�t���[�������F �^�[���`�F�b�N > �V��
//------------------------------------------------------------------
static void scproc_turncheck_weather( BTL_SVFLOW_WORK* wk, BTL_POKESET* pokeSet )
{
  BtlWeather weather = BTL_FIELD_TurnCheckWeather();
  // �^�[���`�F�b�N�ɂ��V�󂪏I�����
  if( weather != BTL_WEATHER_NONE )
  {
    SCQUE_PUT_ACT_WeatherEnd( wk->que, weather );
    return;
  }
  // �V�󂪌p��
  else
  {
    BTL_POKEPARAM* bpp;

    weather = scEvent_GetWeather( wk );
    BTL_POKESET_SeekStart( pokeSet );
    while( (bpp = BTL_POKESET_SeekNext(pokeSet)) != NULL )
    {
      if( !BPP_IsDead(bpp) )
      {
        u32 hem_state = Hem_PushState( &wk->HEManager );
        int  damage = BTL_CALC_RecvWeatherDamage( bpp, weather );
        scEvent_CheckWeatherReaction( wk, bpp, weather, damage );
        if( !scproc_HandEx_Root(wk, ITEM_DUMMY_DATA) )
        {
          if( damage ){
            scPut_WeatherDamage( wk, bpp, weather, damage );
          }
        }
        Hem_PopState( &wk->HEManager, hem_state );
        scproc_CheckDeadCmd( wk, bpp );
        scproc_CheckExpGet( wk );
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * [Event] �V�󃊃A�N�V�����`�F�b�N
 *
 * @param   wk
 * @param   weather
 */
//----------------------------------------------------------------------------------
static int scEvent_CheckWeatherReaction( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, BtlWeather weather, u32 damage )
{
  u8  failFlag;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WEATHER, weather );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, FALSE );
    BTL_EVENTVAR_SetValue( BTL_EVAR_DAMAGE,  damage );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WEATHER_REACTION );
    damage = BTL_EVENTVAR_GetValue( BTL_EVAR_DAMAGE );
    failFlag = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  BTL_EVENTVAR_Pop();
  if( failFlag ){
    return 0;
  }
  return damage;
}
//----------------------------------------------------------------------------------
/**
 * [Put]�V��ɂ��_���[�W����
 *
 * @param   wk
 * @param   bpp
 * @param   weather
 * @param   damage
 */
//----------------------------------------------------------------------------------
static void scPut_WeatherDamage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BtlWeather weather, int damage )
{
  u8 pokeID = BPP_GetID( bpp );

  switch( weather ){
  case BTL_WEATHER_SAND:
    HANDEX_STR_Setup( &wk->strParam, BTL_STRTYPE_SET, BTL_STRID_SET_WeatherDmg_Sand );
    HANDEX_STR_AddArg( &wk->strParam, pokeID );
    break;
  case BTL_WEATHER_SNOW:
    HANDEX_STR_Setup( &wk->strParam, BTL_STRTYPE_SET, BTL_STRID_SET_WeatherDmg_Snow );
    HANDEX_STR_AddArg( &wk->strParam, pokeID );
    break;
  default:
    HANDEX_STR_Clear( &wk->strParam );
    break;
  }

  BTL_Printf("�_���[�W�l=%d\n", damage);
  if( damage > 0 ){
    scproc_SimpleDamage( wk, bpp, damage, &wk->strParam );
  }
}

//--------------------------------------------------------------------------
/**
 * �Ώۃ|�P����������ł����玀�S�������K�v�R�}���h����
 */
//--------------------------------------------------------------------------
static BOOL scproc_CheckDeadCmd( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* poke )
{
  u8 pokeID = BPP_GetID( poke );

  if( wk->pokeDeadFlag[pokeID] == 0 )
  {
    if( BPP_IsDead(poke) )
    {
      BTL_Printf("�|�P[ID=%d]���ɂ܂�\n", pokeID);
      wk->pokeDeadFlag[pokeID] = 1;
      BTL_DEADREC_Add( &wk->deadRec, pokeID );

      // @@@ �݂����o�Ă����ʂ�������R�}���h�����H

      SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_Dead, pokeID );
      SCQUE_PUT_ACT_Dead( wk->que, pokeID );

      BPP_Clear_ForDead( poke );
      scproc_ClearPokeDependEffect( wk, poke );


      // �o���l�擾 -> �ޏ�̏��ɂ��Ȃ��ƌo���l�v�Z�ł��������Ȃ�܂�
//      scproc_GetExp( wk, poke );
      BTL_POSPOKE_PokeOut( &wk->pospokeWork, pokeID );
      return TRUE;
    }
  }
  return FALSE;
}
//--------------------------------------------------------------------------
/**
 * ����|�P�����ˑ��̏�Ԉُ�E�T�C�h�G�t�F�N�g���A�e��n���h�����N���A����
 */
//--------------------------------------------------------------------------
static void scproc_ClearPokeDependEffect( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* poke )
{
  FRONT_POKE_SEEK_WORK fps;
  BTL_POKEPARAM* bpp;
  u8 dead_pokeID = BPP_GetID( poke );

  BTL_HANDLER_TOKUSEI_Remove( poke );
  BTL_HANDLER_ITEM_Remove( poke );
  BTL_HANDLER_Waza_RemoveForceAll( poke );

  FRONT_POKE_SEEK_InitWork( &fps, wk );
  while( FRONT_POKE_SEEK_GetNext( &fps, wk, &bpp ) )
  {
    BPP_CureWazaSickDependPoke( bpp, dead_pokeID, CurePokeDependSick_CallBack, wk );
  }
  BTL_FIELD_RemoveDependPokeEffect( dead_pokeID );
}
/**
 * ����|�P�����ˑ��̏�Ԉُ킪���邲�ƂɌĂяo�����R�[���o�b�N
 */
static void CurePokeDependSick_CallBack( void* wk_ptr, BTL_POKEPARAM* bpp, WazaSick sickID, u8 dependPokeID )
{
  BTL_SVFLOW_WORK* wk = wk_ptr;

  if( sickID == WAZASICK_FREEFALL )
  {
    u8 pokeID = BPP_GetID( bpp );
    scPut_Message_Set( wk, bpp, BTL_STRID_SET_FreeFall_End );
    scPut_ResetContFlag( wk, bpp, BPP_CONTFLG_SORAWOTOBU );
  }
}


//----------------------------------------------------------------------------------
/**
 * ���S�|�P���R�[�h�����ĕK�v�Ȃ�o���l�擾�̏������s��
 *
 * @param   wk
 */
//----------------------------------------------------------------------------------
static void scproc_CheckExpGet( BTL_SVFLOW_WORK* wk )
{
  if( BTL_MAIN_IsExpSeqEnable(wk->mainModule) )
  {
    u32 i, deadPokeCnt = BTL_DEADREC_GetCount( &wk->deadRec, 0 );

    for(i=0; i<deadPokeCnt; ++i)
    {
      if( !BTL_DEADREC_GetExpCheckedFlag(&wk->deadRec, 0, i) )
      {
        u8 pokeID = BTL_DEADREC_GetPokeID( &wk->deadRec, 0, i );
        const BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, pokeID );
        scproc_GetExp( wk, bpp );

        BTL_DEADREC_SetExpCheckedFlag( &wk->deadRec, 0, i );
      }
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * �o���l�擾�����R�}���h����
 *
 * @param   wk
 * @param   deadPoke
 *
 * @retval  BOOL    �o���l�擾�������I��������i�擾�����𖞂����Ă�����jTRUE
 */
//----------------------------------------------------------------------------------
static void scproc_GetExp( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* deadPoke )
{
  u8 deadPokeID = BPP_GetID( deadPoke );
  if( BTL_MAINUTIL_PokeIDtoSide(deadPokeID) == BTL_SIDE_2ND )
  {
    BTL_PARTY* party = BTL_POKECON_GetPartyData( wk->pokeCon, BTL_MAIN_GetPlayerClientID(wk->mainModule) );

    getexp_calc( wk, party, deadPoke, wk->calcExpWork );
    getexp_make_cmd( wk, party, wk->calcExpWork );
  }
}
//----------------------------------------------------------------------------------
/**
 * �o���l�v�Z���ʂ��p���[�N�ɏo��
 *
 * @param   wk
 * @param   party     �v���C���[���p�[�e�B
 * @param   deadPoke  ���񂾃|�P����
 * @param   result    [out] �v�Z���ʊi�[��
 */
//----------------------------------------------------------------------------------
static void getexp_calc( BTL_SVFLOW_WORK* wk, const BTL_PARTY* party, const BTL_POKEPARAM* deadPoke, CALC_EXP_WORK* result )
{
  u32 baseExp = BTL_CALC_CalcBaseExp( deadPoke );
  u16 memberCount  = BTL_PARTY_GetMemberCount( party );
  u16 gakusyuCount = 0;

  const BTL_POKEPARAM* bpp;
  u16 i;

  // ���[�N�N���A
  for(i=0; i<BTL_PARTY_MEMBER_MAX; ++i){
    result[i].exp = 0;
  }

  // �������イ���������̌o���l�v�Z
  for(i=0; i<memberCount; ++i)
  {
    // �����Ă��Ă������イ�������𑕔����Ă���|�P�������Ώ�
    bpp = BTL_PARTY_GetMemberDataConst( party, i );
    if( !BPP_IsDead(bpp)
    &&  (BPP_GetValue(bpp, BPP_LEVEL) < PTL_LEVEL_MAX)
    &&  (BPP_GetItem(bpp) == ITEM_GAKUSYUUSOUTI)
    ){
      ++gakusyuCount;
    }
  }
  if( gakusyuCount )
  {
    u32 gakusyuExp = baseExp / 2;
    baseExp -= gakusyuExp;

    for(i=0; i<memberCount; ++i)
    {
      bpp = BTL_PARTY_GetMemberDataConst( party, i );
      if( !BPP_IsDead(bpp)
      &&  (BPP_GetValue(bpp, BPP_LEVEL) < PTL_LEVEL_MAX)
      &&  (BPP_GetItem(bpp) == ITEM_GAKUSYUUSOUTI)
      ){
        result[i].exp = gakusyuExp / gakusyuCount;
      }
    }
  }

  BTL_N_Printf( DBGSTR_SVFL_ExpCalc_Base, baseExp );


  // �ΐ�o���l�擾
  {
    u8 confrontCnt = BPP_CONFRONT_REC_GetCount( deadPoke );
    u8 aliveCnt = 0;
    u8 pokeID;

    for(i=0; i<confrontCnt; ++i)
    {
      pokeID = BPP_CONFRONT_REC_GetPokeID( deadPoke, i );
      bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, pokeID );
      if( !BPP_IsDead(bpp) ){
        ++aliveCnt;
      }
    }
    BTL_N_Printf( DBGSTR_SVFL_ExpCalc_MetInfo, BPP_GetID(deadPoke), confrontCnt, aliveCnt);

    for(i=0; i<memberCount; ++i)
    {
      bpp = BTL_PARTY_GetMemberDataConst( party, i );
      if( !BPP_IsDead(bpp) && (BPP_GetValue(bpp, BPP_LEVEL) < PTL_LEVEL_MAX) )
      {
        u8 j;
        pokeID = BPP_GetID( bpp );
        for(j=0; j<confrontCnt; ++j)
        {
          if( BPP_CONFRONT_REC_GetPokeID(deadPoke, j) == pokeID ){
            u32 addExp = (baseExp / aliveCnt );
            BTL_N_Printf( DBGSTR_SVFL_ExpCalc_DivideInfo, i, (baseExp/aliveCnt));
            result[i].exp += (baseExp / aliveCnt);
          }
        }
      }
    }
  }

  // �{�[�i�X�v�Z
  for(i=0; i<BTL_PARTY_MEMBER_MAX; ++i)
  {
    if( result[i].exp )
    {
      const MYSTATUS* status = BTL_MAIN_GetPlayerStatus( wk->mainModule );
      const POKEMON_PARAM* pp;
      u16 myLv, enemyLv;
      bpp = BTL_PARTY_GetMemberDataConst( party, i );
      pp = BPP_GetSrcData( bpp );

      // �|�����|�P�Ƃ̃��x�����ɂ���Čo���l����������iWB����ǉ����ꂽ�d�l�j
      myLv = BPP_GetValue( bpp, BPP_LEVEL );
      enemyLv = BPP_GetValue( deadPoke, BPP_LEVEL );
      result[i].exp = getexp_calc_adjust_level( bpp, result[i].exp, myLv, enemyLv );

#ifndef SOGA_DEBUG
      //����|�P�����ɐ�����ID�Ɛe�̖��O���͂����Ă��Ȃ����߁A��Ɂu�����߂Ɂv�ɂȂ��Ă��܂����߁A
      //�s�{�ӂȂ���10��Ver�ł͂��̏������O���Ă���
      // ���l���e�Ȃ�{�[�i�X
      if( !PP_IsMatchOya(pp, status) )
      {
        // �O���̐e�Ȃ�x1.7�C��������Ȃ����x1.5
        fx32 ratio;
        ratio = ( PP_Get(pp, ID_PARA_country_code, NULL) != MyStatus_GetRegionCode(status) )?
            FX32_CONST(1.7f) : FX32_CONST(1.5f);

        result[i].exp = BTL_CALC_MulRatio( result[i].exp, ratio );
        result[i].fBonus = TRUE;
      }
#endif
      // �����킹�^�}�S�����Ă���{�[�i�X x1.5
      if( BPP_GetItem(bpp) == ITEM_SIAWASETAMAGO ){
        result[i].exp = BTL_CALC_MulRatio( result[i].exp, FX32_CONST(1.5f) );
        result[i].fBonus = TRUE;
      }

      BTL_N_Printf( DBGSTR_SVFL_ExpCalc_Result, i, result[i].exp);
    }
  }
}


//----------------------------------------------------------------------------------
/**
 * �|���ꂽ�|�P�����ƌo���l���擾����|�P�����̃��x�����ɉ����Čo���l��␳����i�v�a���j
 *
 * @param   base_exp      ��{�o���l�iGS�܂ł̎d�l�ɂ��v�Z���ʁj
 * @param   getpoke_lv    �o���l���擾����|�P�����̃��x��
 * @param   deadpoke_lv   �|���ꂽ�|�P�����̃��x��
 *
 * @retval  u32   �␳��o���l
 */
//----------------------------------------------------------------------------------
static u32 getexp_calc_adjust_level( const BTL_POKEPARAM* bpp, u32 base_exp, u16 getpoke_lv, u16 deadpoke_lv )
{
  u32 denom, numer;
  fx32 ratio;
  u32  denom_int, numer_int, result, expMargin;

  numer_int = deadpoke_lv * 2 + 10;
  denom_int = deadpoke_lv + getpoke_lv + 10;

  numer = _calc_adjust_level_sub( numer_int );
  denom = _calc_adjust_level_sub( denom_int );
  ratio = (numer / (denom >> FX32_SHIFT));

  result = BTL_CALC_MulRatio( base_exp, ratio ) + 1;
  expMargin = BPP_GetExpMargin( bpp );
  if( result > expMargin ){
    result = expMargin;
  }

  BTL_N_Printf( DBGSTR_SVFL_ExpAdjustCalc,
      getpoke_lv, deadpoke_lv, ratio, base_exp, result );

  return result;
}

//----------------------------------------------------------------------------------
/**
 * �o���l�v�Z���ʂ����ɃR�}���h����
 *
 * @param   wk
 * @param   party     �v���C���[���p�[�e�B
 * @param   calcExp   �o���l�v�Z���ʃ��[�N
 */
//----------------------------------------------------------------------------------
static void getexp_make_cmd( BTL_SVFLOW_WORK* wk, BTL_PARTY* party, const CALC_EXP_WORK* calcExp )
{
  BTL_POKEPARAM* bpp;
  u32 i;

  for(i=0; i<BTL_PARTY_MEMBER_MAX; ++i)
  {
    if( calcExp[i].exp )
    {
      BTL_POKEPARAM* bpp = BTL_PARTY_GetMemberData( party, i );
      u32 exp   = calcExp[i].exp;
      u16 strID = (calcExp[i].fBonus)? BTL_STRID_STD_GetExp_Bonus : BTL_STRID_STD_GetExp;
      u8  pokeID = BPP_GetID( bpp );

      BTL_Printf("�o���l�͂��������b�Z�[�W :strID=%d, pokeID=%d, exp=%d\n", strID, pokeID, exp);
      SCQUE_PUT_MSG_STD( wk->que, strID, pokeID, exp );

      SCQUE_PUT_ACT_AddExp( wk->que, pokeID, exp );
    }
  }
}
/**
 *  �o���l�␳�v�Z�T�u
 */
static inline u32 _calc_adjust_level_sub( u32 val )
{
  fx32  fx_val, fx_sqrt;
  u64 result;

  fx_val = FX32_CONST( val );
  fx_sqrt = FX_Sqrt( fx_val );
  val *= val;
  result = (val * fx_sqrt) >> FX32_SHIFT;

  return result;
}


//---------------------------------------------------------------------------------------------
//  ���l�֘A�֐��Q
//---------------------------------------------------------------------------------------------
static inline int roundValue( int val, int min, int max )
{
  if( val < min ){ val = min; }
  if( val > max ){ val = max; }
  return val;
}
static inline int roundMin( int val, int min )
{
  if( val < min ){ val = min; }
  return val;
}
static inline int roundMax( int val, int max )
{
  if( val > max ){ val = max; }
  return val;
}
static inline BOOL perOccur( BTL_SVFLOW_WORK* wk, u8 per )
{
  return BTL_CALC_GetRand( 100 ) < per;
}
//---------------------------------------------------------------------------------------------
//  �C�x���g���[�N�m��
//---------------------------------------------------------------------------------------------
static void eventWork_Init( BTL_EVENT_WORK_STACK* stack )
{
  stack->sp = 0;
}
static void* eventWork_Push( BTL_EVENT_WORK_STACK* stack, u32 size )
{
  GF_ASSERT( stack->sp < EVENT_WORK_DEPTH );

  while( size % 4 ){ ++size; }

  {
    u32 usedSize;
    u32 i;

    for(i=0, usedSize=0; i<stack->sp; ++i)
    {
      usedSize += stack->size[ i ];
    }

    if( (usedSize + size) < sizeof(stack->work) )
    {
      void* adrs;
      adrs = &stack->work[ usedSize ];
      stack->size[ stack->sp ] = size;
      stack->sp++;
      return adrs;
    }
    else
    {
      GF_ASSERT(0);
      return NULL;
    }
  }
}

static void eventWork_Pop( BTL_EVENT_WORK_STACK* stack, void* adrs )
{
  GF_ASSERT( stack->sp );

  stack->sp--;

  {
    u32 used_size, i;

    for(i=0, used_size=0; i<stack->sp; ++i)
    {
      used_size += stack->size[ i ];
    }

    if( adrs != &(stack->work[used_size]) )
    {
      GF_ASSERT(0); // �Ō�Ɋm�ۂ��ꂽ���[�N�ȊO�̗̈悪��ɉ������悤�Ƃ��Ă���
    }
  }
}
//---------------------------------------------------------------------------------------------
//
// �R�}���h�o�͑w
// �T�[�o�[�R�}���h���o�b�t�@�ɐ��� �� �K�v�Ȃ炻��ɔ����|�P�����p�����[�^������s��
// �C�x���g�w�̊֐��Ăяo���͍s���Ȃ�
//
//---------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 * [Put] �w��|�P�����ʒu�ɃG�t�F�N�g����
 */
//--------------------------------------------------------------------------
static void scPut_EffectByPokePos( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u16 effectNo )
{
  u8 pokeID = BPP_GetID( bpp );
  BtlPokePos pos = BTL_POSPOKE_GetPokeExistPos( &wk->pospokeWork, pokeID );
  if( pos != BTL_POS_NULL )
  {
    SCQUE_PUT_ACT_EffectByPos( wk->que, pos, effectNo );
  }
}

//--------------------------------------------------------------------------
/**
 * [Put] �u�����́@�͂�ǂ��ł������Ȃ��v�\��
 */
//--------------------------------------------------------------------------
static void scPut_CantAction( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  scPut_Message_Set( wk, bpp, BTL_STRID_SET_CantAction );
}
//--------------------------------------------------------------------------
/**
 * �u�����͂����񂵂Ă���v�\��
 */
//--------------------------------------------------------------------------
static void scPut_ConfCheck( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  scPut_EffectByPokePos( wk, bpp, BTLEFF_KONRAN );
  SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_KonranAct, BPP_GetID(bpp) );
}
//--------------------------------------------------------------------------
/**
 * �u�����́����Ƀ����������v�\��
 */
//--------------------------------------------------------------------------
static void scPut_MeromeroAct( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  u8 target_pokeID = BPP_GetSickParam( bpp, WAZASICK_MEROMERO );
  SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_MeromeroAct, BPP_GetID(bpp), target_pokeID );
}
//--------------------------------------------------------------------------
/**
 * ������ɂ�鎩���_���[�W����
 */
//--------------------------------------------------------------------------
static void scPut_ConfDamage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u32 damage )
{
  u8 pokeID = BPP_GetID( bpp );

  // ������̓}�W�b�N�K�[�h�̉e�����󂯂Ȃ��̂�SimpleHP�𒼐ڌĂ�ł���  taya
  SCQUE_PUT_ACT_ConfDamage( wk->que, pokeID );
  scPut_SimpleHp( wk, bpp, (int)(-damage), TRUE );
  SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_KonranExe, pokeID );

}
//--------------------------------------------------------------------------
/**
 * ���U�o�����莞�̃|�P�����n��Ԉُ�񕜏���
 */
//--------------------------------------------------------------------------
static void scPut_CurePokeSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, WazaSick sick )
{
  u8 pokeID = BPP_GetID( bpp );

  BPP_CurePokeSick( bpp );
  SCQUE_PUT_OP_CurePokeSick( wk->que, pokeID );
  SCQUE_PUT_ACT_SickIcon( wk->que, pokeID, POKESICK_NULL );

  switch( sick ){
  case POKESICK_NEMURI:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_NemuriWake, pokeID );
    break;
  case POKESICK_KOORI:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_KoriMelt, pokeID );
    break;
  default:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_PokeSick_Cure, pokeID );
    break;
  }
}
//--------------------------------------------------------------------------
/**
 * ���U���b�Z�[�W�\���i�����́~�~���������I�Ȃǁj
 */
//--------------------------------------------------------------------------
static void scPut_WazaMsg( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza )
{
  SCQUE_PUT_MSG_WAZA( wk->que, BPP_GetID(attacker), waza );
}
//----------------------------------------------------------------------------------
/**
 * [Put]��Ԉُ�񕜏����̃R�}���h����
 *
 * @param   wk
 * @param   bpp       �Ώۃ|�P����
 * @param   sick_id   ��Ԉُ�R�[�h
 * @param   oldCont   [out] �񕜑O�̌p���p�����[�^�i�s�v�Ȃ�NULL)
 */
//----------------------------------------------------------------------------------
static WazaSick scPut_CureSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BtlWazaSickEx exSickCode, BPP_SICK_CONT* oldCont )
{
  WazaSick sick = trans_sick_code( bpp, exSickCode );

  if( sick != WAZASICK_NULL )
  {
    u8 pokeID = BPP_GetID( bpp );

    if( oldCont ){
      *oldCont = BPP_GetSickCont( bpp, sick );
    }

    if( sick < POKESICK_MAX )
    {
      BPP_CurePokeSick( bpp );
      SCQUE_PUT_OP_CurePokeSick( wk->que, pokeID );
      if ( BTL_POSPOKE_GetPokeExistPos(&wk->pospokeWork, pokeID) != BTL_POS_NULL ){
        SCQUE_PUT_ACT_SickIcon( wk->que, pokeID, POKESICK_NULL );
      }
    }
    else
    {
      BPP_CureWazaSick( bpp, sick );
      SCQUE_PUT_OP_CureWazaSick( wk->que, pokeID, sick );
    }
  }
  return sick;
}
//----------------------------------------------------------------------------------
/**
 * �g����Ԉُ�R�[�h���A�Y������R�[�h�ɕϊ�����
 *
 * @param   bpp       �Ώۃ|�P����
 * @param   exCode
 *
 * @retval  WazaSick  �Y���R�[�h�^�Y���R�[�h�Ȃ��̏ꍇ�AWAZASICK_NULL��Ԃ�
 */
//----------------------------------------------------------------------------------
static WazaSick trans_sick_code( const BTL_POKEPARAM* bpp, BtlWazaSickEx exCode )
{
  WazaSick result = WAZASICK_NULL;
  PokeSick poke_sick = BPP_GetPokeSick( bpp );

  // �g���R�[�h�ϊ�
  if( (exCode == WAZASICK_EX_POKEFULL) || (exCode == WAZASICK_EX_POKEFULL_PLUS) )
  {
    // �|�P�����n��Ԉُ�ɂȂ��Ă���Ȃ炻�������
    if( poke_sick != WAZASICK_NULL ){
      result = poke_sick;
    }
    // ����ȊO�͍����̂ݎ���
    else{
      if( (exCode == WAZASICK_EX_POKEFULL_PLUS)
      &&  (BPP_CheckSick(bpp, WAZASICK_KONRAN))
      ){
        result = WAZASICK_KONRAN;
      }
    }
  }
  // �ʏ�R�[�h���͜늳���Ă��Ȃ���Ԉُ���w�肳�ꂽ��NULL��Ԃ�
  else{
    if( BPP_CheckSick(bpp, exCode) ){
      result = exCode;
    }else{
      result = WAZASICK_NULL;
    }
  }

  return result;
}
//--------------------------------------------------------------------------
/**
 * �A�����U�����J�E���^���N���A
 */
//--------------------------------------------------------------------------
static void scPut_ResetSameWazaCounter( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  BPP_ResetWazaContConter( bpp );
  SCQUE_PUT_OP_ResetUsedWazaCounter( wk->que, BPP_GetID(bpp) );
}
//--------------------------------------------------------------------------
/**
 * �u���������܂����܂�Ȃ������v�\��
 */
//--------------------------------------------------------------------------
static void scPut_WazaFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza )
{
  SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_WazaFail );
}
//--------------------------------------------------------------------------
/**
 * �u�����ɂ͓�����Ȃ������v�\��
 */
//--------------------------------------------------------------------------
static void scPut_WazaAvoid( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* defender, WazaID waza )
{
  SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_WazaAvoid, BPP_GetID(defender) );
}
//--------------------------------------------------------------------------
/**
 * �u�����ɂ͂��������Ȃ��悤���v�\��
 */
//--------------------------------------------------------------------------
static void scput_WazaNoEffect( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* defender )
{
  SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_NoEffect, BPP_GetID(defender) );
}
//--------------------------------------------------------------------------
/**
 * [Put] ���U�ɂ��_���[�W�i�P�́j
 */
//--------------------------------------------------------------------------
static void scPut_WazaDamageSingle( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
  BTL_POKEPARAM* defender, BtlTypeAff aff, u32 damage, BOOL critical_flag, BOOL plural_hit_flag )
{
  u8 pokeID = BPP_GetID( defender );

  // ������q�b�g�͍Ō�ɂP�񂾂��u���ʂ́������I�v���o�����߁A�����ł͖�����
  if( plural_hit_flag ){
    aff = BTL_TYPEAFF_100;
  }


  scPut_DecreaseHP( wk, defender, damage );

  BTL_Printf("�|�P[%d]�ɑ΂��郏�U�_���[�W�l:%d -> �c��HP %d\n", BPP_GetID(defender), damage,
    BPP_GetValue(defender, BPP_HP));

  SCQUE_PUT_ACT_WazaDamage( wk->que, pokeID, aff, damage, wazaParam->wazaID );

  if( critical_flag )
  {
    SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_CriticalHit );
  }
}
//--------------------------------------------------------------------------
/**
 * [Put] ���U�ɂ��_���[�W�i�����́j
 */
//--------------------------------------------------------------------------
static void scPut_WazaDamagePlural( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
  u32 poke_cnt, BtlTypeAff aff, BTL_POKEPARAM** bpp, const u16* damage, const u8* critical_flag )
{
  u32 i;

  for(i=0; i<poke_cnt; ++i)
  {
    scPut_DecreaseHP( wk, bpp[i], damage[i] );
  }

  SCQUE_PUT_ACT_WazaDamagePlural( wk->que, poke_cnt, aff, wazaParam->wazaID );
  for(i=0; i<poke_cnt; ++i)
  {
    SCQUE_PUT_ArgOnly( wk->que, BPP_GetID(bpp[i]) );
  }
  for(i=0; i<poke_cnt; ++i)
  {
    if( critical_flag[i] ){
      SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_CriticalHit, BPP_GetID(bpp[i]) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * [Put] ���U�_���[�W���u���炦���v���̃��b�Z�[�W�\��
 *
 * @param   wk
 * @param   bpp
 * @param   cause
 */
//----------------------------------------------------------------------------------
static void scPut_Koraeru( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, BppKoraeruCause cause )
{
  u8 pokeID = BPP_GetID( bpp );

  switch( cause ){
  case BPP_KORAE_WAZA_DEFENDER:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_Koraeru_Exe, pokeID );
    break;
  case BPP_KORAE_ITEM:
    {
      u16 itemID = BPP_GetItem( bpp );
      if( itemID ){
        SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_KoraeItem, pokeID, itemID );
      }
    }
  }
}
//--------------------------------------------------------------------------
/**
 * [Put] �Ƃ������E�B���h�E�\���J�n
 */
//--------------------------------------------------------------------------
static void scPut_TokWin_In( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  SCQUE_PUT_TOKWIN_IN( wk->que, BPP_GetID(bpp) );
}
//--------------------------------------------------------------------------
/**
 * [Put] �Ƃ������E�B���h�E�����J�n
 */
//--------------------------------------------------------------------------
static void scPut_TokWin_Out( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  SCQUE_PUT_TOKWIN_OUT( wk->que, BPP_GetID(bpp) );
}
//--------------------------------------------------------------------------
/**
 * [Put] ����ȏ�A�\�̓����N��������Ȃ��i������Ȃ��j���b�Z�[�W
 */
//--------------------------------------------------------------------------
static void scPut_RankEffectLimit( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, WazaRankEffect effect, int volume )
{
  u8 pokeID = BPP_GetID( target );

  if( volume > 0 ){
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_RankupMax_ATK, pokeID, effect );
  }else{
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_RankdownMin_ATK, pokeID, effect );
  }
}
//--------------------------------------------------------------------------
/**
 * [Put] �����N�������ʂ�����
 */
//--------------------------------------------------------------------------
static void scPut_RankEffect( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, WazaRankEffect effect, int volume, u16 itemID, BOOL fStdMsg )
{
  u8 pokeID = BPP_GetID( target );

  if( volume > 0 )
  {
    volume = BPP_RankUp( target, effect, volume );
    SCQUE_PUT_OP_RankUp( wk->que, pokeID, effect, volume );
    SCQUE_PUT_ACT_RankUp( wk->que, pokeID, effect, volume );
    if( fStdMsg )
    {
      if( itemID == ITEM_DUMMY_DATA ){
        SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_Rankup_ATK, pokeID, effect, volume );
      }else{
        SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_UseItem_Rankup_ATK, pokeID, itemID, effect, volume );
      }
    }
  }
  else
  {
    volume *= -1;
    volume = BPP_RankDown( target, effect, volume );
    SCQUE_PUT_OP_RankDown( wk->que, pokeID, effect, volume );
    SCQUE_PUT_ACT_RankDown( wk->que, pokeID, effect, volume );
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_Rankdown_ATK, pokeID, effect, volume );
  }
}
//--------------------------------------------------------------------------
/**
 * [Put] �ꌂ�K�E���U����
 */
//--------------------------------------------------------------------------
static void scPut_Ichigeki( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target )
{
  u8 pokeID = BPP_GetID( target );
  BPP_HpZero( target );
  SCQUE_PUT_OP_HpZero( wk->que, pokeID );
  SCQUE_PUT_ACT_WazaIchigeki( wk->que, pokeID );
}
//--------------------------------------------------------------------------
/**
 * [Put] �V���v��HP�����G�t�F�N�g
 */
//--------------------------------------------------------------------------
static void scPut_SimpleHp( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, int value, BOOL fEffectEnable )
{
  u8 pokeID = BPP_GetID( bpp );

  if( value > 0 )
  {
    BPP_HpPlus( bpp, value );
    SCQUE_PUT_OP_HpPlus( wk->que, pokeID, value );
  }
  else if( value < 0 )
  {
    value *= -1;
    scPut_DecreaseHP( wk, bpp, value );
  }

  // ��ɂ��Ȃ��|�P���񕜂����邱�Ƃ����邽��
  if( fEffectEnable
  &&  (BTL_POSPOKE_GetPokeExistPos(&wk->pospokeWork, pokeID) != BTL_POS_NULL)
  ){
    SCQUE_PUT_ACT_SimpleHP( wk->que, pokeID );
  }
  else{
    BTL_Printf("�G�t�F�N�g�L���t���O=%d, �G�t�F�N�g�𐶐����܂���\n", fEffectEnable);
  }
}
//--------------------------------------------------------------------------
/**
 * [Put] �|�P���������Ђ�
 */
//--------------------------------------------------------------------------
static void scPut_KillPokemon( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u8 effType )
{
  u16 minusHP = BPP_GetValue( bpp, BPP_HP );
  u8 pokeID = BPP_GetID( bpp );

  BPP_HpZero( bpp );
  SCQUE_PUT_OP_HpZero( wk->que, pokeID );
  SCQUE_PUT_ACT_Kill( wk->que, pokeID, effType );
}
//--------------------------------------------------------------------------
/**
 * [Put] ��Ԉُ퉻
 */
//--------------------------------------------------------------------------
static void scPut_AddSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, WazaSick sick, BPP_SICK_CONT sickCont )
{
  u8 pokeID = BPP_GetID( target );

  BPP_SetWazaSick( target, sick, sickCont );
  SCQUE_PUT_OP_SetSick( wk->que, pokeID, sick, sickCont.raw );

  // ��Ԉُ�A�C�R���t��
  {
    if( sick == WAZASICK_DOKUDOKU ){
      sick = WAZASICK_DOKU;
    }
    if( sick < POKESICK_MAX ){
      SCQUE_PUT_ACT_SickIcon( wk->que, pokeID, sick );
    }
  }
}
static void scPut_AddSickFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, BtlAddSickFailCode failCode, WazaSick sick )
{
  u8 pokeID = BPP_GetID( target );
  SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_NoEffect, pokeID );
}
//--------------------------------------------------------------------------
/**
 * [Put] �Z�b�g���b�Z�[�W�\��
 */
//--------------------------------------------------------------------------
static void scPut_Message_Set( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u16 strID )
{
  u8 pokeID = BPP_GetID( bpp );
  SCQUE_PUT_MSG_SET( wk->que, strID, pokeID );
}
//--------------------------------------------------------------------------
/**
 * [Put] �W�����b�Z�[�W�\���i�����g���j
 */
//--------------------------------------------------------------------------
static void scPut_Message_StdEx( BTL_SVFLOW_WORK* wk, u16 strID, u32 argCnt, const int* args )
{
  switch( argCnt ){
  case 0:
  default:
    SCQUE_PUT_MSG_STD( wk->que, strID );
    break;
  case 1:
    SCQUE_PUT_MSG_STD( wk->que, strID, args[0] );
    break;
  case 2:
    SCQUE_PUT_MSG_STD( wk->que, strID, args[0], args[1] );
    break;
  case 3:
    SCQUE_PUT_MSG_STD( wk->que, strID, args[0], args[1], args[2] );
    break;
  case 4:
    SCQUE_PUT_MSG_STD( wk->que, strID, args[0], args[1], args[2], args[3] );
    break;
  }
}
//--------------------------------------------------------------------------
/**
 * [Put] �Z�b�g���b�Z�[�W�\���i�����g���j
 */
//--------------------------------------------------------------------------
static void scPut_Message_SetEx( BTL_SVFLOW_WORK* wk, u16 strID, u32 argCnt, const int* args )
{
  switch( argCnt ){
  case 0:
  default:
    SCQUE_PUT_MSG_SET( wk->que, strID );
    break;
  case 1:
    SCQUE_PUT_MSG_SET( wk->que, strID, args[0] );
    break;
  case 2:
    SCQUE_PUT_MSG_SET( wk->que, strID, args[0], args[1] );
    break;
  case 3:
    SCQUE_PUT_MSG_SET( wk->que, strID, args[0], args[1], args[2] );
    break;
  case 4:
    SCQUE_PUT_MSG_SET( wk->que, strID, args[0], args[1], args[2], args[3] );
    break;
  }
}
//----------------------------------------------------------------------------------
/**
 * [Put] �|�P����HP�����炷
 *
 * @param   wk
 * @param   bpp
 * @param   value
 */
//----------------------------------------------------------------------------------
static void scPut_DecreaseHP( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u32 value )
{
  if( !BTL_MAIN_GetDebugFlag(wk->mainModule, BTL_DEBUGFLAG_HP_CONST) )
  {
    BPP_HpMinus( bpp, value );
    SCQUE_PUT_OP_HpMinus( wk->que, BPP_GetID(bpp), value );
  }
}
//----------------------------------------------------------------------------------
/**
 * [Put] �g�������U��PP�f�N�������g
 *
 * @param   wk
 * @param   attacker
 * @param   wazaIdx
 * @param   vol
 */
//----------------------------------------------------------------------------------
static void scPut_DecrementPP( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, u8 wazaIdx, u8 vol )
{
  if( !BTL_MAIN_GetDebugFlag(wk->mainModule, BTL_DEBUGFLAG_PP_CONST) )
  {
    u8 pokeID = BPP_GetID( attacker );
    BPP_PPMinus( attacker, wazaIdx, vol );
    BPP_WAZA_SetUsedFlag( attacker, wazaIdx );
    SCQUE_PUT_OP_PPMinus( wk->que, pokeID, wazaIdx, vol );
  }
}
//----------------------------------------------------------------------------------
/**
 * [Put] PP��
 *
 * @param   wk
 * @param   bpp
 * @param   wazaIdx
 * @param   volume
 * @param   itemID    �A�C�e�����g�������ʂ̏ꍇ�̓A�C�e��ID
 */
//----------------------------------------------------------------------------------
static void scPut_RecoverPP( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u8 wazaIdx, u8 volume, u16 itemID )
{
  u8 pokeID = BPP_GetID( bpp );

  BPP_PPPlus( bpp, wazaIdx, volume );
  SCQUE_PUT_OP_PPPlus( wk->que, pokeID, wazaIdx, volume );
  if( itemID != ITEM_DUMMY_DATA )
  {
    WazaID waza = BPP_WAZA_GetID( bpp, wazaIdx );
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_UseItem_RecoverPP, pokeID, itemID, waza );
  }
}
//----------------------------------------------------------------------------------
/**
 * [Put] ���݂̂�H�ׂ�A�N�V����
 */
//----------------------------------------------------------------------------------
static void scPut_UseItemAct( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  SCQUE_PUT_ACT_KINOMI( wk->que, BPP_GetID(bpp) );
}
//----------------------------------------------------------------------------------
/**
 * [Put] �����A�C�e�����폜
 */
//----------------------------------------------------------------------------------
static void scPut_ConsumeItem( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  BTL_HANDLER_ITEM_Remove( bpp );
  BPP_ConsumeItem( bpp );
  SCQUE_PUT_OP_ConsumeItem( wk->que, BPP_GetID(bpp) );
}
//----------------------------------------------------------------------------------
/**
 * [Put] �p���t���O�Z�b�g
 *
 * @param   wk
 * @param   bpp
 * @param   flag
 */
//----------------------------------------------------------------------------------
static void scPut_SetContFlag( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppContFlag flag )
{
  BPP_CONTFLAG_Set( bpp, flag );
  SCQUE_PUT_OP_SetContFlag( wk->que, BPP_GetID(bpp), flag );
}
//----------------------------------------------------------------------------------
/**
 * [Put] �p���t���O�N���A
 *
 * @param   wk
 * @param   bpp
 * @param   flag
 */
//----------------------------------------------------------------------------------
static void scPut_ResetContFlag( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppContFlag flag )
{
  BPP_CONTFLAG_Clear( bpp, flag );
  SCQUE_PUT_OP_ResetContFlag( wk->que, BPP_GetID(bpp), flag );
}
//----------------------------------------------------------------------------------
/**
 * [Put] �^�[���t���O�Z�b�g
 *
 * @param   wk
 * @param   bpp
 * @param   flag
 */
//----------------------------------------------------------------------------------
static void scPut_SetTurnFlag( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppTurnFlag flag )
{
  BPP_TURNFLAG_Set( bpp, flag );
  SCQUE_PUT_OP_SetTurnFlag( wk->que, BPP_GetID(bpp), flag );
}
//----------------------------------------------------------------------------------
/**
 * [Put] �^�[���t���O�Z�b�g
 *
 * @param   wk
 * @param   bpp
 * @param   flag
 */
//----------------------------------------------------------------------------------
static void scPut_ResetTurnFlag( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppTurnFlag flag )
{
  BPP_TURNFLAG_ForceOff( bpp, flag );
  SCQUE_PUT_OP_ResetTurnFlag( wk->que, BPP_GetID(bpp), flag );
}

//---------------------------------------------------------------------------------------------
//
// �y�C�x���g�w�z
//
// ���C�������w�ł̔���ɕK�v�Ȋe������̌�����s���B
// �n���h���ނ̌Ăяo���̓C�x���g�w����̂ݍs����B
// �|�P�����p�����[�^�̑���͍s��Ȃ��B
// �R�}���h�o�͍͂s��Ȃ��B
//
//---------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/**
 * [Event] �����o�[����ւ��O�ɓ���|�P�����̃A�N�V���������荞�܂��邩�`�F�b�N
 *
 * @param   wk
 * @param   outPoke
 *
 * @retval  u8        ���荞�ޏꍇ�A�Y���|�P����ID�^����ȊO BTL_POKEID_NULL
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_MemberChangeIntr( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* outPoke )
{
  u8 intrPokeID = BTL_POKEID_NULL;
  u32 i;

  // �P�x�A�������|�P�����̑S���U�n���h����o�^���Ă���
  for(i=0; i<wk->numActOrder; ++i)
  {
    if( (wk->actOrder[i].bpp != outPoke) && (wk->actOrder[i].fDone == FALSE) )
    {
      WazaID waza = ActOrder_GetWazaID( &wk->actOrder[i] );
      if( waza != WAZANO_NULL ){
        BTL_HANDLER_Waza_Add( wk->actOrder[i].bpp, waza );
        wk->actOrder[i].fIntr = TRUE;
      }
    }
  }

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_TARGET1, BPP_GetID(outPoke) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, intrPokeID );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_MENBERCHANGE_INTR );
    intrPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
  BTL_EVENTVAR_Pop();

  // �������|�P�����̑S���U�n���h�����폜
  for(i=0; i<wk->numActOrder; ++i)
  {
    if( wk->actOrder[i].fIntr)
    {
      WazaID waza = ActOrder_GetWazaID( &wk->actOrder[i] );
      if( waza != WAZANO_NULL ){
        BTL_HANDLER_Waza_RemoveForce( wk->actOrder[i].bpp, waza );
      }
    }
  }

  return intrPokeID;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���̃��U���Ăяo�����U�Ȃ�A�Ăяo�����U�̃p�����[�^���Z�b�g����
 *
 * @param   wk
 * @param   attacker
 * @param   orgWazaid
 * @param   orgTargetPos
 * @param   reqWaza         [io] ���ɃA�N�V���������������ݒ肳��Ă���
 *
 * @retval  BOOL    ���̃��U���Ăяo�����U���̂��o���̂Ɏ��s����ꍇFALSE�^����ȊOTRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_GetReqWazaParam( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker,
  WazaID orgWazaid, BtlPokePos orgTargetPos, REQWAZA_WORK* reqWaza )
{
  BOOL failFlag = FALSE;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEPOS_ORG, orgTargetPos );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, reqWaza->wazaID );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEPOS, BTL_POS_NULL );
    BTL_EVENTVAR_SetValue( BTL_EVAR_FAIL_FLAG, failFlag );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_REQWAZA_PARAM );
    reqWaza->wazaID = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    reqWaza->targetPos = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEPOS );
    failFlag = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  BTL_EVENTVAR_Pop();
  return !failFlag;
}
//----------------------------------------------------------------------------------
/**
 * [Event] �g���������h��I�񂾂Ƃ��̓���D��x�𔻒�
 *
 * @param   wk
 * @param   attacker
 * @param   spePriA     [out] ����D��xA���󂯎��i���U��荂�����背�x���j
 * @param   spePriB     [out] ����D��xB���󂯎��i���U���Ⴂ���背�x���j
 *
 */
//----------------------------------------------------------------------------------
static void scEvent_CheckSpecialActPriority( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, u8* spePriA, u8* spePriB )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_SP_PRIORITY_A, BTL_SPPRI_A_DEFAULT );
    BTL_EVENTVAR_SetValue( BTL_EVAR_SP_PRIORITY_B, BTL_SPPRI_B_DEFAULT );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHECK_SP_PRIORITY );
    *spePriA = BTL_EVENTVAR_GetValue( BTL_EVAR_SP_PRIORITY_A );
    *spePriB = BTL_EVENTVAR_GetValue( BTL_EVAR_SP_PRIORITY_B );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] �^�[���ŏ��̃��U�����V�[�P���X���O
 *
 * @param   wk
 * @param   bpp
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scEvent_BeforeFight( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, WazaID waza )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_BEFORE_FIGHT );
  BTL_EVENTVAR_Pop();
}
//--------------------------------------------------------------------------
/**
 * [Event] ������_���[�W�v�Z
 *
 * @param   wk
 * @param   attacker
 *
 * @retval  u16   �_���[�W�l
 */
//--------------------------------------------------------------------------
static u16 scEvent_CalcConfDamage( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker )
{
  u16 dmg = roundMin( BPP_GetValue(attacker, BPP_MAX_HP)/8, 1 );

  BTL_EVENTVAR_Push();

  BTL_EVENTVAR_SetValue( BTL_EVAR_CONF_DMG, dmg );
  BTL_EVENT_CallHandlers( wk, BTL_EVENT_CALC_CONF_DAMAGE );
  dmg = BTL_EVENTVAR_GetValue( BTL_EVAR_CONF_DMG );

  BTL_EVENTVAR_Pop();
  return dmg;
}
//----------------------------------------------------------------------------------
/**
 * [Event] �����U�Ăяo�����ȂǁA�Ǝ��̃��U���b�Z�[�W���o�͂��邩����
 *
 * @param   wk
 * @param   attacker    ���U���o���|�P����
 * @param   orgWazaID   �Ăяo�����̃��UID�iex. ��т��ӂ�j
 * @param   actWazaID   �Ăяo����̃��UID
 * @param   str         [out] �Ǝ����U���b�Z�[�W�p�����[�^���i�[
 *
 * @retval  BOOL    �Ǝ����U���b�Z�[�W���o�͂���ꍇ��TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_CheckWazaMsgCustom( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker,
  WazaID orgWazaID, WazaID actWazaID, BTL_HANDEX_STR_PARAMS* str )
{
  BOOL result;

  HANDEX_STR_Clear( str );

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, actWazaID );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WORK_ADRS, (int)str );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, FALSE );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_REQWAZA_MSG );
    result = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  BTL_EVENTVAR_Pop();

  return result;
}
//--------------------------------------------------------------------------
/**
 * �yEvent�z���U�o�����s����P��ځi���U���b�Z�[�W�O�j
 *
 * @param   wk
 * @param   attacker    �U���|�P�����p�����[�^
 * @param   waza        �o�����Ƃ��Ă��郏�U
 *
 * @retval  SV_WazaFailCause
 */
//--------------------------------------------------------------------------
static SV_WazaFailCause scEvent_CheckWazaExecute1ST( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza, SV_WazaFailCause cause )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_CAUSE, cause );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, FALSE );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_EXECUTE_CHECK_1ST );
    cause = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_CAUSE );
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_FAIL_FLAG) ){
      cause = SV_WAZAFAIL_NULL;
    }
  BTL_EVENTVAR_Pop();
  return cause;
}
//--------------------------------------------------------------------------
/**
 * �yEvent�z���U�o�����s����Q��ځi���U���b�Z�[�W��j
 *
 * @param   wk
 * @param   attacker    �U���|�P�����p�����[�^
 * @param   waza        �o�����Ƃ��Ă��郏�U
 *
 * @retval  SV_WazaFailCause
 */
//--------------------------------------------------------------------------
static SV_WazaFailCause scEvent_CheckWazaExecute2ND( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza )
{
  WazaSick sick = BPP_GetPokeSick( attacker );
  SV_WazaFailCause  cause = SV_WAZAFAIL_NULL;
  u8 pokeID = BPP_GetID( attacker );

  do {

    // ���U���b�N�ɂ�鎸�s�`�F�b�N
    if( (BPP_CheckSick(attacker, WAZASICK_ENCORE)
       ||  BPP_CheckSick(attacker, WAZASICK_WAZALOCK) )
    &&  (BPP_GetPrevWazaID(attacker) != waza)
    ){
      cause = SV_WAZAFAIL_WAZALOCK;
      break;
    }
    // ���傤�͂ɂ�鎸�s�`�F�b�N
    if( BPP_CheckSick(attacker, WAZASICK_TYOUHATSU)
    &&  !WAZADATA_IsDamage(waza)
    ){
      cause = SV_WAZAFAIL_TYOUHATSU;
      break;
    }
    // ���������ɂ�鎸�s�`�F�b�N
    if( BPP_CheckSick(attacker, WAZASICK_ICHAMON)
    &&  (BPP_GetPrevWazaID(attacker) == waza)
    ){
      cause = SV_WAZAFAIL_ICHAMON;
      break;
    }
    // �ӂ�����ɂ�鎸�s�`�F�b�N
    if( BTL_FIELD_CheckEffect(BTL_FLDEFF_FUIN) )
    {
      u8 dependPokeID = BTL_FIELD_GetDependPokeID( BTL_FLDEFF_FUIN );
      if( BPP_GetID(attacker) != dependPokeID )
      {
        const BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, dependPokeID );
        if( BPP_WAZA_SearchIdx(bpp, waza) != PTL_WAZA_MAX ){
          cause = SV_WAZAFAIL_FUUIN;
          break;
        }
      }
    }

  }while(0);

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_CAUSE, cause );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, pokeID );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_EXECUTE_CHECK_2ND );
    cause = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_CAUSE );
  BTL_EVENTVAR_Pop();
  return cause;
}
//--------------------------------------------------------------------------
/**
 * [Event] �������郏�U�̃p�����[�^���\���̂ɃZ�b�g
 *
 * @param   wk
 * @param   waza
 * @param   attacker
 * @param   [out] param
 *
 */
//--------------------------------------------------------------------------
static void scEvent_GetWazaParam( BTL_SVFLOW_WORK* wk, WazaID waza, const BTL_POKEPARAM* attacker, SVFL_WAZAPARAM* param )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZA_TYPE, WAZADATA_GetType(waza) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_USER_TYPE, BPP_GetPokeType(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_DAMAGE_TYPE, WAZADATA_GetDamageType(waza) );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FLAT_FLAG, FALSE );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_PARAM );
    param->wazaID = waza;
    param->wazaType    = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_TYPE );
    param->userType    = BTL_EVENTVAR_GetValue( BTL_EVAR_USER_TYPE );
    param->damageType  = BTL_EVENTVAR_GetValue( BTL_EVAR_DAMAGE_TYPE );
    param->fTypeFlat   = BTL_EVENTVAR_GetValue( BTL_EVAR_FLAT_FLAG );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U�������s�m��
 *
 * @param   wk
 * @param   bpp
 * @param   waza
 * @param   cause
 */
//----------------------------------------------------------------------------------
static void scEvent_CheckWazaExeFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp,
  WazaID waza, SV_WazaFailCause cause )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENTVAR_SetValue( BTL_EVAR_FAIL_CAUSE, cause );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_EXECUTE_FAIL );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U�o�������m��
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_WazaExecuteStart( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza, BTL_POKESET* rec )
{
  BOOL fQuit = FALSE;
  u32 targetCnt = BTL_POKESET_GetCount( rec );

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_TARGET_POKECNT, targetCnt );
    {
      u32 i;
      const BTL_POKEPARAM* bpp;
      for(i=0; i<targetCnt; ++i){
        bpp = BTL_POKESET_Get( rec, i );
        BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_TARGET1+i, BPP_GetID(bpp) );
      }
    }
    BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, fQuit );

    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_EXE_START );
    fQuit = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  BTL_EVENTVAR_Pop();

  return fQuit;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U�^�[�Q�b�g���荞��
 *
 * @param   wk
 * @param   attacker
 * @param   wazaParam
 *
 * @retval  u8
 */
//----------------------------------------------------------------------------------
static u8 scEvent_GetWazaTargetIntr( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const SVFL_WAZAPARAM* wazaParam )
{
  u8 pokeID = BTL_POKEID_NULL;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_POKEID_DEF, pokeID );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_DECIDE_TARGET );
    pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  BTL_EVENTVAR_Pop();
  return pokeID;
}
//----------------------------------------------------------------------------------
/**
 * [Event] �܂���E�݂���̖������`�F�b�N
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   waza
 *
 * @retval  BOOL    ����������ꍇTRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_CheckMamoruBreak( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza )
{
  BOOL result = FALSE;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, result );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHECK_MAMORU_BREAK );
    result = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  BTL_EVENTVAR_Pop();
  return result;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U���O�ꂽ
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 * @param   targetCount
 * @param   targetPokeID
 */
//----------------------------------------------------------------------------------
static void scEvent_WazaAvoid( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza, u8 targetCount, const u8* targetPokeID )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENTVAR_SetValue( BTL_EVAR_TARGET_POKECNT, targetCount );
    {
      u32 i;
      for(i=0; i<targetCount; ++i){
        BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_TARGET1+i, targetPokeID[i] );
      }
    }
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_AVOID );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���߃^�[���X�L�b�v����
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 *
 * @retval  BOOL    ���߃^�[���X�L�b�v����ꍇTRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_CheckTameTurnSkip( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza )
{
  BOOL result = FALSE;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, result );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHECK_TAMETURN_SKIP );
    result = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  BTL_EVENTVAR_Pop();
  return result;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���߃^�[�������m��
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scEvent_TameStart( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKESET* targetRec, WazaID waza )
{
  const BTL_POKEPARAM* target;
  u32 targetCnt, i;

  targetCnt = BTL_POKESET_GetCount( targetRec );

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_TARGET_POKECNT, targetCnt );
    for(i=0; i<targetCnt; ++i)
    {
      target = BTL_POKESET_Get( targetRec, i );
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_TARGET1+i, BPP_GetID(target) );
    }
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_TAME_START );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���߃^�[���X�L�b�v�m��
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scEvent_TameSkip( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_TAME_SKIP );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���߃^�[�������m��
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scEvent_TameRelease( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKESET* rec, WazaID waza )
{
  const BTL_POKEPARAM* target;
  u32 targetCnt, i;

  targetCnt = BTL_POKESET_GetCount( rec );

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_TARGET_POKECNT, targetCnt );
    for(i=0; i<targetCnt; ++i)
    {
      target = BTL_POKESET_Get( rec, i );
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_TARGET1+i, BPP_GetID(target) );
    }
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_TAME_RELEASE );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ������ƂԁA�_�C�r���O���A�ꂩ��B��Ă���|�P�����ւ̃q�b�g�`�F�b�N
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   waza
 *
 * @retval  BOOL    �B��Ă��邽�߃q�b�g���Ȃ��ꍇ��TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_CheckPokeHideAvoid( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza )
{
  static const BppContFlag  hideState[] = {
    BPP_CONTFLG_SORAWOTOBU, BPP_CONTFLG_ANAWOHORU, BPP_CONTFLG_DIVING,
  };

  BOOL avoidFlag = FALSE;
  u32 i;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
    for(i=0; i<NELEMS(hideState); ++i)
    {
      if( BPP_CONTFLAG_Get(defender, hideState[i]) )
      {
        BTL_EVENTVAR_SetValue( BTL_EVAR_AVOID_FLAG, TRUE );
        BTL_EVENTVAR_SetValue( BTL_EVAR_POKE_HIDE, hideState[i] );
        BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHECK_POKE_HIDE );
        if( BTL_EVENTVAR_GetValue(BTL_EVAR_AVOID_FLAG) ){
          avoidFlag = TRUE;
          break;
        }
      }
    }
  BTL_EVENTVAR_Pop();

  return avoidFlag;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U�̖����`�F�b�N���A�m���v�Z���X�L�b�v���Ė��������邩�`�F�b�N
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   waza
 *
 * @retval  BOOL        �m���v�Z���X�L�b�v����ꍇ��TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_IsExcuseCalcHit( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza )
{
  BOOL result = WAZADATA_IsAlwaysHit( waza );
  if( !result )
  {
    BTL_EVENTVAR_Push();
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
      BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, FALSE );
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_EXCUSE_CALC_HIT );
      result = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
    BTL_EVENTVAR_Pop();
  }
  return result;
}
//--------------------------------------------------------------------------
/**
 * [Event] ���U�I����(0-100)�擾
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   waza
 *
 * @retval  u8    �I���� (0-100)
 */
//--------------------------------------------------------------------------
static u8 scEvent_getHitPer( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam )
{
  u32 per = WAZADATA_GetParam( wazaParam->wazaID, WAZAPARAM_HITPER );
  fx32 ratio = FX32_CONST(1);

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_DAMAGE_TYPE, wazaParam->damageType );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_HIT_PER, per );
    BTL_EVENTVAR_SetMulValue( BTL_EVAR_RATIO, FX32_CONST(1), FX32_CONST(0.1), FX32_CONST(32) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_HIT_RATIO );
    per = BTL_EVENTVAR_GetValue( BTL_EVAR_HIT_PER );
    ratio = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
  BTL_EVENTVAR_Pop();

  per = BTL_CALC_MulRatio( per, ratio );
  if( per > 100 ){
    per = 100;
  }

  return per;
}
//--------------------------------------------------------------------------
/**
 * [Event] �o�������U���Ώۂɓ����邩����i�ꌂ�K�E���U�j
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   waza
 *
 * @retval  BOOL
 */
//--------------------------------------------------------------------------
static BOOL scEvent_IchigekiCheck( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza )
{
  u8 atkLevel = BPP_GetValue( attacker, BPP_LEVEL );
  u8 defLevel = BPP_GetValue( defender, BPP_LEVEL );

  // �U�������x�����h�䑤���x�������Ȃ��Ɏ��s
  if( atkLevel < defLevel )
  {
    return FALSE;
  }

  {
    BOOL ret = FALSE;
    BOOL fFail = FALSE;

    BTL_EVENTVAR_Push();
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
      BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, FALSE );
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_ICHIGEKI_CHECK );
      fFail = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
    BTL_EVENTVAR_Pop();

    // �Ƃ��������ɂ�鋭��������������Ύ��s
    if( fFail ){
      return FALSE;
    }

    if( IsMustHit(attacker, defender) ){
      ret = TRUE;
    }else{
      u8 per = WAZADATA_GetParam( waza, WAZAPARAM_HITPER );
      BTL_Printf("�f�t�H������=%d\n", per);
      per += (atkLevel - defLevel);
      ret = perOccur( wk, per );
      BTL_Printf("���x���␳������=%d, hit=%d\n", per, ret);
    }
    return ret;
  }
}
//--------------------------------------------------------------------------
/**
 * [Event] ���U�����`�F�b�N�i�Ƃ��������A�ʃP�[�X�ɂ�閳�����j
 *
 * @param   wk
 * @param   waza
 * @param   attacker
 * @param   defender
 * @param   customMsg   �����������ꍇ�ɓ���̃��b�Z�[�W���o�͂���ꍇ�̃p�����[�^
 *
 * @retval  BOOL    �����ȏꍇTRUE
 */
//--------------------------------------------------------------------------
static BOOL scEvent_CheckNotEffect( BTL_SVFLOW_WORK* wk, WazaID waza, u8 lv,
    const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, BTL_HANDEX_STR_PARAMS* customMsg )
{
  BOOL fNoEffect = FALSE;
  HANDEX_STR_Clear( customMsg );

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WORK_ADRS, (int)customMsg );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_NOEFFECT_FLAG, fNoEffect );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_NOEFFECT_CHECK_L1+lv );
    fNoEffect = BTL_EVENTVAR_GetValue( BTL_EVAR_NOEFFECT_FLAG );
  BTL_EVENTVAR_Pop();

  return fNoEffect;
}
//--------------------------------------------------------------------------
/**
 * [Event] ����^�C�v�̃_���[�W���U�ŉ񕜂��Ă��܂��|�P�̔���
 *
 * @param   wk
 * @param   defender
 * @param   waza
 *
 * @retval  BOOL
 */
//--------------------------------------------------------------------------
static BOOL scEvent_CheckDmgToRecover( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender,
  const SVFL_WAZAPARAM* wazaParam )
{
  BOOL ret = FALSE;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, ret );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_DMG_TO_RECOVER_CHECK );
    ret = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  BTL_EVENTVAR_Pop();
  return ret;
}
//----------------------------------------------------------------------------------
/**
 * [Event] �_���[�W���U�񕜉�����
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 */
//----------------------------------------------------------------------------------
static void scEvent_DmgToRecover( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_DMG_TO_RECOVER_FIX );
  BTL_EVENTVAR_Pop();
}
//--------------------------------------------------------------------------
/**
 * [Event] �N���e�B�J���q�b�g�����`�F�b�N
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   waza
 *
 * @retval  BOOL
 */
//--------------------------------------------------------------------------
static BOOL scEvent_CheckCritical( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza )
{
  BOOL flag = FALSE;
  u16  rank = WAZADATA_GetParam( waza, WAZAPARAM_CRITICAL_RANK );
  rank += BPP_GetCriticalRank( attacker );

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_CRITICAL_RANK, rank );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, FALSE );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CRITICAL_CHECK );
    if( !BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG ) )
    {
      if( BTL_MAIN_GetDebugFlag(wk->mainModule, BTL_DEBUGFLAG_MUST_CRITICAL) ){
        return TRUE;
      }
      if( WAZADATA_IsMustCritical(waza) ){
        flag = TRUE;
      }else{
        rank = roundMax( BTL_EVENTVAR_GetValue(BTL_EVAR_CRITICAL_RANK), BTL_CALC_CRITICAL_MAX );
        flag = BTL_CALC_CheckCritical( rank );
      }
    }
    else{
      flag = FALSE;
    }
  BTL_EVENTVAR_Pop();
  return flag;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U�ɂ��_���[�W��^������̔����v�Z�i�_���[�W�~�������j
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 * @param   damage
 *
 * @retval  u32
 */
//----------------------------------------------------------------------------------
static u32 scEvent_CalcKickBack( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza, u32 damage )
{
  u8 pokeID = BPP_GetID( attacker );
  u8 ratio = WAZADATA_GetParam( waza, WAZAPARAM_DAMAGE_REACTION_RATIO );
  u8 ratio_ex = 0;
  u8 fail_flag = FALSE;

#ifdef SOGA_DEBUG
  //�h���C���n�͔����ɂȂ�Ȃ��悤�ɂ���
  if( WAZADATA_GetCategory(waza) == WAZADATA_CATEGORY_DRAIN )
  {
    return 0;
  }
#endif

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_RATIO, ratio );
    BTL_EVENTVAR_SetValue( BTL_EVAR_RATIO_EX, ratio_ex );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, pokeID );
    BTL_EVENTVAR_SetValue( BTL_EVAR_FAIL_FLAG, FALSE );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CALC_KICKBACK );
    ratio = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
    fail_flag = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
    ratio_ex = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO_EX );
  BTL_EVENTVAR_Pop();

  if( fail_flag )
  {
    ratio = 0;
  }
  ratio += ratio_ex;

  if( ratio )
  {
    damage = roundMin( (damage*ratio)/100, 1 );
    return damage;
  }
  return 0;
}
//--------------------------------------------------------------------------
/**
 * [Event] ���U�_���[�W�󂯂���̔�������
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   waza
 * @param   aff
 * @param   damage
 * @param   criticalFlag
 *
 */
//--------------------------------------------------------------------------
static void scEvent_WazaDamageReaction( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam, BtlTypeAff aff,
  u32 damage, BOOL criticalFlag )
{
  u8 defPokeID = BPP_GetID( defender );

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, defPokeID );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_TYPEAFF, aff );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_DAMAGE_TYPE, wazaParam->damageType );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_CRITICAL_FLAG, criticalFlag );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_DMG_REACTION );
  BTL_EVENTVAR_Pop();
}
//--------------------------------------------------------------------------
/**
 * [Event] ���U�_���[�W��^���I�������̒ǉ�����
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   waza
 * @param   aff
 * @param   damage
 * @param   criticalFlag
 *
 */
//--------------------------------------------------------------------------
static void scEvent_WazaDamageAfter( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const SVFL_WAZAPARAM* wazaParam,
  u32 damage, BOOL criticalFlag )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
    BTL_EVENTVAR_SetValue( BTL_EVAR_CRITICAL_FLAG, criticalFlag );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_DMG_AFTER );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] �����A�C�e���g�p
 *
 * @param   wk
 * @param   bpp
 * @param   evwk
 */
//----------------------------------------------------------------------------------
static void scEvent_ItemEquip( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_USE_ITEM );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] �ꎞ�I�ɓo�^�����A�C�e���̎g�p�i���΂ނȂǁj
 *
 * @param   wk
 * @param   bpp
 * @param   evwk
 */
//----------------------------------------------------------------------------------
static void scEvent_ItemEquipTmp( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENT_ForceCallHandlers( wk, BTL_EVENT_USE_ITEM_TMP );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * ���U�̑����v�Z�i�_���[�W�E�ꌂ���U�̂݁j
 *
 * @param   wk
 * @param   defender
 * @param   waza_type
 *
 * @retval  BtlTypeAff
 */
//----------------------------------------------------------------------------------
static BtlTypeAff scProc_checkWazaDamageAffinity( BTL_SVFLOW_WORK* wk,
  BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam )
{
  if( wazaParam->fTypeFlat ){
    return BTL_TYPEAFF_100;
  }

  // ��s��Ԃ̃|�P�����ɒn�ʃ��U�͌��ʂ��Ȃ�
  if( wazaParam->wazaType == POKETYPE_JIMEN )
  {
    scproc_CheckFlying( wk, defender );
    if( BPP_TURNFLAG_Get(defender, BPP_TURNFLG_FLYING) ){
      return BTL_TYPEAFF_0;
    }
  }

  {
    BtlTypeAff affinity;
    PokeTypePair defPokeTypePair = BPP_GetPokeType( defender );
    PokeType  defPokeType = PokeTypePair_GetType1( defPokeTypePair );

    affinity = scEvent_CheckDamageAffinity( wk, attacker, defender, wazaParam->wazaType, defPokeType );
    if( !PokeTypePair_IsPure(defPokeTypePair) ){
      PokeTypePair  type2 = PokeTypePair_GetType2( defPokeTypePair );
      BtlTypeAff    aff2 = scEvent_CheckDamageAffinity( wk, attacker, defender, wazaParam->wazaType, type2 );
      affinity = BTL_CALC_TypeAffMul( affinity, aff2 );
    }

    return affinity;
  }
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U�����v�Z
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   waza_type
 * @param   poke_type
 *
 * @retval  BtlTypeAff
 */
//----------------------------------------------------------------------------------
static BtlTypeAff scEvent_CheckDamageAffinity( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, PokeType waza_type, PokeType poke_type )
{
  BOOL  flatFlag, flatMasterFlag;
  BtlTypeAff affinity;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKE_TYPE, poke_type );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, waza_type );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FLAT_FLAG, FALSE );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FLATMASTER_FLAG, FALSE );

    BTL_SICKEVENT_CheckNotEffectByType( wk, defender );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHECK_AFFINITY );

    flatFlag = BTL_EVENTVAR_GetValue( BTL_EVAR_FLAT_FLAG );
    flatMasterFlag = BTL_EVENTVAR_GetValue( BTL_EVAR_FLATMASTER_FLAG );
  BTL_EVENTVAR_Pop();

  if( flatMasterFlag ){
    return BTL_TYPEAFF_100;
  }

  affinity = BTL_CALC_TypeAff( waza_type, poke_type );
  if( (affinity == BTL_TYPEAFF_0) && (flatFlag) ){
    return BTL_TYPEAFF_100;
  }

  return affinity;
}
//----------------------------------------------------------------------------------
/**
 * �_���[�W�v�Z�p�̑����擾
 *
 * @param   wazaType
 * @param   pokeType
 *
 * @retval  BtlTypeAff
 */
//----------------------------------------------------------------------------------
static BtlTypeAff CalcTypeAffForDamage( PokeType wazaType, PokeTypePair pokeTypePair )
{
  // �_���[�W�v�Z�܂ŗ��Ă���Ȃ�0�Ƃ������Ƃ͖����A�P�{�œ������Ԃ̂͂�
  PokeType  type1 = PokeTypePair_GetType1( pokeTypePair );
  BtlTypeAff aff = BTL_CALC_TypeAff( wazaType, type1 );
  if( !PokeTypePair_IsPure(pokeTypePair) )
  {
    PokeType  type2 = PokeTypePair_GetType2( pokeTypePair );
    BtlTypeAff aff2 = BTL_CALC_TypeAff( wazaType, type2 );
    aff = BTL_CALC_TypeAffMul( aff, aff2 );
  }
  return aff;
}


//----------------------------------------------------------------------------------
/**
 * [Event] ���U�g�p��̃f�N�������g����PP�l���擾
 *
 * @param   wk
 * @param   attacker  ���U�g�p�|�P����
 * @param   rec       ���U�ɂ���ă_���[�W���󂯂��|�P�����p�����[�^�Q
 *
 * @retval  u32       �f�N�������gPP�l
 */
//----------------------------------------------------------------------------------
static u32 scEvent_DecrementPPVolume( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, BTL_POKESET* rec )
{
  u32 volume = 1;

  BTL_EVENTVAR_Push();
    {
      u8 i = 0;
      BTL_POKEPARAM* bpp;
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_TARGET_POKECNT, BTL_POKESET_GetCount(rec) );
      while( (bpp=BTL_POKESET_Get(rec, i)) != NULL )
      {
        BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_TARGET1+i, BPP_GetID(bpp) );
        ++i;
      }
    }
    BTL_EVENTVAR_SetValue( BTL_EVAR_VOLUME, volume );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_DECREMENT_PP_VOLUME );
    volume = BTL_EVENTVAR_GetValue( BTL_EVAR_VOLUME );
  BTL_EVENTVAR_Pop();

  return volume;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U�g�p��APP���f�N�������g������̔���
 *
 * @param   wk
 * @param   attacker
 *
 * @retval  BOOL    �A�C�e���g�p�����������TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_DecrementPP_Reaction( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, u8 wazaIdx )
{
  BOOL flag = FALSE;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID,   BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZA_IDX, wazaIdx );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, flag );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_DECREMENT_PP_DONE );
    flag = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  BTL_EVENTVAR_Pop();

  return flag;
}
//--------------------------------------------------------------------------
/**
 * [Event] ������q�b�g���U�̃`�F�b�N����
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 * @param   hitCount    [out] ���s����q�b�g�񐔁i�����_���j
 *
 * @retval  BOOL    ������q�b�g���郏�U�Ȃ�TRUE
 */
//--------------------------------------------------------------------------
static BOOL scEvent_CheckPluralHit( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza, u8* hitCount )
{
  u8 max = WAZADATA_GetParam( waza, WAZAPARAM_HITCOUNT_MAX );

  BTL_Printf("���U[%d]�̍ő�q�b�g��=%d\n", waza, max);

  if( max > 1 ){
    *hitCount = BTL_CALC_HitCountMax( max );
  }else{
    *hitCount = 1;
  }

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_HITCOUNT_MAX, max );
    BTL_EVENTVAR_SetValue( BTL_EVAR_HITCOUNT, *hitCount );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_HIT_COUNT );
    *hitCount = BTL_EVENTVAR_GetValue( BTL_EVAR_HITCOUNT );
  BTL_EVENTVAR_Pop();

  if( (max>1) || (*hitCount) >1 ){
    return TRUE;
  }

  return FALSE;
}

//----------------------------------------------------------------------------------
/**
 * [Event] ���U�З͎擾
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   waza
 * @param   wazaParam
 *
 * @retval  u16   ���U�З͒l
 */
//----------------------------------------------------------------------------------
static u16 scEvent_getWazaPower( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam )
{
  u16 power = WAZADATA_GetPower( wazaParam->wazaID );
  fx32 ratio;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_DAMAGE_TYPE, wazaParam->damageType );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZA_POWER, power );
    BTL_EVENTVAR_SetMulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(1.0), FX32_CONST(0.1f), FX32_CONST(512) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_POWER );
    power = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER );
    ratio = (fx32)BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER_RATIO );
  BTL_EVENTVAR_Pop();

  power = BTL_CALC_MulRatio( power, ratio );
  BTL_N_Printf( DBGSTR_SVFL_WazaPower, wazaParam->wazaID, power, ratio);
  return power;
}
//--------------------------------------------------------------------------
/**
 * [Event] ���U�ȊO�̃_���[�W�L���^�����`�F�b�N
 *
 * @param   wk
 * @param   bpp
 * @param   damage  �_���[�W��
 *
 * @retval  BOOL    �L���Ȃ�TRUE
 */
//--------------------------------------------------------------------------
static BOOL scEvent_CheckEnableSimpleDamage( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u32 damage )
{
  BOOL flag;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, TRUE );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_SIMPLE_DAMAGE_ENABLE );
    flag = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  BTL_EVENTVAR_Pop();

  return flag;
}

// �U�����\�͒l�擾
static u16 scEvent_getAttackPower( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam, BOOL criticalFlag )
{
  BppValueID vid = (WAZADATA_GetDamageType(wazaParam->wazaID) == WAZADATA_DMG_SPECIAL)? BPP_SP_ATTACK : BPP_ATTACK;

  {
    const BTL_POKEPARAM* calc_attacker = attacker;
    fx32 ratio;
    u16 power;
    BTL_EVENTVAR_Push();
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
      BTL_EVENTVAR_SetValue( BTL_EVAR_SWAP_POKEID, BTL_POKEID_NULL );
      BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, FALSE );    //
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_ATTACKER_POWER_PREV );

      {
        u8 swapPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_SWAP_POKEID );
        if( swapPokeID != BTL_POKEID_NULL ){
          calc_attacker = BTL_POKECON_GetPokeParam( wk->pokeCon, swapPokeID );
        }
      }

      // �N���e�B�J���`�F�b�N
      if( BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG ) ){
        power = BPP_GetValue_Base( calc_attacker, vid );
      }
      else
      {
        if( criticalFlag ){
          power = BPP_GetValue_Critical( calc_attacker, vid );
        }else{
          power = BPP_GetValue( calc_attacker, vid );
        }
      }

      BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
      BTL_EVENTVAR_SetValue( BTL_EVAR_POWER, power );
      BTL_EVENTVAR_SetMulValue( BTL_EVAR_RATIO, FX32_ONE, FX32_CONST(0.1), FX32_CONST(32) );
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_ATTACKER_POWER );
      power = BTL_EVENTVAR_GetValue( BTL_EVAR_POWER );
      ratio = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
      power = BTL_CALC_MulRatio( power, ratio );
      BTL_N_Printf( DBGSTR_SVFL_AtkPower, power, criticalFlag, ratio);
    BTL_EVENTVAR_Pop();
    return power;
  }
}

// �h�䑤�\�͒l�擾
static u16 scEvent_getDefenderGuard( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender,
  const SVFL_WAZAPARAM* wazaParam, BOOL criticalFlag )
{
  BppValueID vid = (WAZADATA_GetDamageType(wazaParam->wazaID) == WAZADATA_DMG_SPECIAL)? BPP_SP_DEFENCE : BPP_DEFENCE;
  fx32 ratio = FX32_CONST(1);
  u16 guard;
  u8 forceFlatFlag;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_VID, vid );
    BTL_EVENTVAR_SetValue( BTL_EVAR_VID_SWAP_CNT, 0 );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, FALSE );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_DEFENDER_GUARD_PREV );
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_VID_SWAP_CNT) & 1 ){
      vid = (vid == BPP_DEFENCE)? BPP_SP_DEFENCE : BPP_DEFENCE;
    }
    forceFlatFlag = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  BTL_EVENTVAR_Pop();


  if( forceFlatFlag ){
    guard = BPP_GetValue_Base( defender, vid );
  }else{
    if( criticalFlag ){
      guard = BPP_GetValue_Critical( defender, vid );
    }else{
      guard = BPP_GetValue( defender, vid );
    }
  }

  // �Ă񂱂��u���Ȃ��炵�v�̎��A����^�C�v�̂Ƃ��ڂ�1.5�{
  if( (scEvent_GetWeather(wk) == BTL_WEATHER_SAND)
  &&  (BPP_IsMatchType(defender, POKETYPE_IWA))
  &&  (vid == BPP_SP_DEFENCE)
  ){
    guard = BTL_CALC_MulRatio( guard, FX32_CONST(1.5) );
  }

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
    BTL_EVENTVAR_SetValue( BTL_EVAR_GUARD, guard );
    BTL_EVENTVAR_SetMulValue( BTL_EVAR_RATIO, ratio, FX32_CONST(0.1), FX32_CONST(32) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_DEFENDER_GUARD );
    guard = BTL_EVENTVAR_GetValue( BTL_EVAR_GUARD );
    ratio = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
  BTL_EVENTVAR_Pop();

  guard = BTL_CALC_MulRatio( guard, ratio );
  BTL_N_Printf( DBGSTR_SVFL_DefGuard, guard, criticalFlag, ratio);
  return guard;
}
//--------------------------------------------------------------------------
/**
 * [Event] �U���|�P�ƃ��U�̃^�C�v��v�␳�����v�Z
 *
 * @param   wk
 * @param   attacker
 * @param   waza_type
 *
 * @retval  fx32    �␳��
 */
//--------------------------------------------------------------------------
static fx32 scEvent_CalcTypeMatchRatio( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, PokeType waza_type )
{
  BOOL f_match = BPP_IsMatchType( attacker, waza_type );
  fx32 ratio = (f_match)? FX32_CONST(1.5f) : FX32_CONST(1);

  BTL_EVENTVAR_Push();
  {
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_RATIO, ratio );
    BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, f_match );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_TYPEMATCH_RATIO );
    ratio = (fx32)BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
  }
  BTL_EVENTVAR_Pop();
  return ratio;
}
//--------------------------------------------------------------------------
/**
 * [Event] �����o�[����ɓo�ꂳ��������
 *
 * @param   wk
 * @param   bpp
 *
 */
//--------------------------------------------------------------------------
static void scEvent_AfterMemberIn( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_MEMBER_IN );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U�ǉ����ʂɂ��Ђ�݊m���擾
 *
 * @param   wk
 * @param   waza
 * @param   attacker
 *
 * @retval  u32
 */
//----------------------------------------------------------------------------------
static u32 scEvent_GetWazaShrinkPer( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker )
{
  u32 per = WAZADATA_GetParam( waza, WAZAPARAM_SHRINK_PER );
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_ADD_PER, per );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_SHRINK_PER );
    per = BTL_EVENTVAR_GetValue( BTL_EVAR_ADD_PER );
  BTL_EVENTVAR_Pop();
  return per;
}
//----------------------------------------------------------------------------------
/**
 * [Event] �Ђ�ݔ�������
 *
 * @param   wk
 * @param   target
 * @param   waza    ���U�ǉ����ʂɂ��ꍇ���UID�i����ȊOWAZANO_NULL�j
 * @param   per     �Ђ�݊m��
 *
 * @retval  BOOL    �Ђ�ނȂ�TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_CheckShrink( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* target, WazaID waza, u32 per )
{
  BOOL fail_flag = FALSE;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENTVAR_SetValue( BTL_EVAR_ADD_PER, per );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, fail_flag );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BPP_GetID(target) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_SHRINK_CHECK );
    per = BTL_EVENTVAR_GetValue( BTL_EVAR_ADD_PER );
    fail_flag = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  BTL_EVENTVAR_Pop();

  if( fail_flag ){
    return FALSE;
  }else{
    return perOccur( wk, per );
  }
}
//----------------------------------------------------------------------------------
/**
 * [Event] �Ђ�ݎ��s�����\��
 *
 * @param   wk
 * @param   target
 */
//----------------------------------------------------------------------------------
static void scEvent_FailShrink( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(target) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_SHRINK_FAIL );
  BTL_EVENTVAR_Pop();
}
//--------------------------------------------------------------------------
/**
 * [Event] �����N�������ʂ̍ŏI���ۃ`�F�b�N
 *
 * @param   wk
 * @param   target
 * @param   effect
 * @param   volume
 *
 * @retval  BOOL      ��������ꍇTRUE
 */
//--------------------------------------------------------------------------
static BOOL scEvent_CheckRankEffectSuccess( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target,
  WazaRankEffect effect, u8 wazaUsePokeID, int volume )
{
  BOOL failFlag = FALSE;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(target) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, wazaUsePokeID );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_STATUS_TYPE, effect );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_VOLUME, volume );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, failFlag );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_RANKEFF_LAST_CHECK );
    failFlag = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  BTL_EVENTVAR_Pop();

  return !failFlag;
//  return evwk->failFlag == FALSE;
}
//----------------------------------------------------------------------------------
/**
 * [Event] �����N�������ʂ̎��s�m��
 *
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
static void scEvent_RankEffect_Failed( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_RANKEFF_FAILED );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] �����N�������ʂ̐����m��
 *
 * @param   wk
 * @param   atkPokeID   ���ʂ𔭐����������U�E�Ƃ������̎�̃|�PID�i���Ȃ��ꍇBTL_POKEID_NULL�j
 * @param   bpp
 * @param   rankType
 * @param   volume
 */
//----------------------------------------------------------------------------------
static void scEvent_RankEffect_Fix( BTL_SVFLOW_WORK* wk, u8 atkPokeID,
  const BTL_POKEPARAM* bpp, WazaRankEffect rankType, int volume )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, atkPokeID );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_STATUS_TYPE, rankType );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_VOLUME, volume );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_RANKEFF_FIXED );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U�ɂ�郉���N�������ʂ̐����m��
 *
 * @param   wk
 * @param   target
 * @param   wazaID
 * @param   effectID
 * @param   volume
 */
//----------------------------------------------------------------------------------
static void scEvent_WazaRankEffectFixed( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target,
  WazaID wazaID, WazaRankEffect effectID, int volume )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(target) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaID );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_STATUS_TYPE, effectID );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_VOLUME, volume );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_RANKEFF_FIXED );
  BTL_EVENTVAR_Pop();
}

//--------------------------------------------------------------------------
/**
 * [Event] �h���C���U���̃h���C��HP�ʂ��Čv�Z
 *
 * @param   wk
 * @param   attacker
 * @param   target
 * @param   volume     �f�t�H���g�񕜗�
 *
 * @retval  int     �񕜗�
 */
//--------------------------------------------------------------------------
static int scEvent_RecalcDrainVolume( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, u32 volume )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(target) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_VOLUME, volume );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CALC_DRAIN );
    volume = BTL_EVENTVAR_GetValue( BTL_EVAR_VOLUME );
  BTL_EVENTVAR_Pop();
  return volume;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ����h���C��
 *
 * @param   wk
 * @param   waza
 * @param   attacker
 * @param   total_damage
 */
//----------------------------------------------------------------------------------
static void scEvent_CheckSpecialDrain( BTL_SVFLOW_WORK* wk, WazaID waza,
  const BTL_POKEPARAM* attacker, u32 total_damage )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_VOLUME, total_damage );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CALC_SPECIAL_DRAIN );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * �yEvent�z�V��̕ω���C�x���g
 *
 * @param   wk
 * @param   weather
 */
//----------------------------------------------------------------------------------
static void scEvent_AfterChangeWeather( BTL_SVFLOW_WORK* wk, BtlWeather weather )
{
  BOOL fDisable = FALSE;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WEATHER, weather );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, FALSE );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WEATHER_CHANGE );
    fDisable = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
    if( !fDisable ){
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_WEATHER_CHANGE_AFTER );
    }
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U�ɂ��HP�񕜗ʌv�Z
 *
 * @param   wk
 * @param   waza
 * @param   bpp
 *
 * @retval  u32
 */
//----------------------------------------------------------------------------------
static u32 scEvent_CalcRecoverHP( BTL_SVFLOW_WORK* wk, WazaID waza, const BTL_POKEPARAM* bpp )
{
  u32 ratio = WAZADATA_GetParam( waza, WAZAPARAM_HP_RECOVER_RATIO );

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_RATIO, ratio );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_RECOVER_HP_RATIO );
    ratio = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
  BTL_EVENTVAR_Pop();

  {
    u32 volume = (BPP_GetValue(bpp, BPP_MAX_HP) * ratio) / 100;
    return volume;
  }
}
//----------------------------------------------------------------------------------
/**
 * [Event] �A�C�e�����Z�b�g�i���邢�͏����j�����`�F�b�N
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  BOOL    ���s����ꍇTRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_CheckItemSet( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u16 itemID )
{
  BOOL failed = FALSE;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_ITEM, itemID );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, failed );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_ITEMSET_CHECK );
    failed = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  BTL_EVENTVAR_Pop();

  return failed;
}
//----------------------------------------------------------------------------------
/**
 * [Event] �A�C�e��������������
 *
 * @param   wk
 * @param   bpp
 * @param   nextItemID   ����������̃A�C�e���i���o�[
 */
//----------------------------------------------------------------------------------
static void scEvent_ItemSetDecide( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u16 nextItemID )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_ITEM, nextItemID );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_ITEMSET_DECIDE );
  BTL_EVENTVAR_Pop();
}

//----------------------------------------------------------------------------------
/**
 * [Event] �A�C�e��������������
 *
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
static void scEvent_ItemSetFixed( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_ITEMSET_FIXED );
  BTL_EVENTVAR_Pop();
}


//----------------------------------------------------------------------------------
/**
 * [Event] �Ƃ������ύX���m��i�ύX�̒��O�j
 *
 * @param   wk
 * @param   pokeID
 * @param   tokuseiID
 */
//----------------------------------------------------------------------------------
static void scEvent_ChangeTokuseiBefore( BTL_SVFLOW_WORK* wk, u8 pokeID, u16 tokuseiID )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, pokeID );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_TOKUSEI, tokuseiID );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHANGE_TOKUSEI_BEFORE );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] �Ƃ��������ύX���ꂽ
 *
 * @param   wk
 * @param   pokeID
 */
//----------------------------------------------------------------------------------
static void scEvent_ChangeTokuseiAfter( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, pokeID );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHANGE_TOKUSEI_AFTER );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] �T�C�h�G�t�F�N�g�p���p�����[�^����
 *
 * @param   wk
 * @param   param->effect
 * @param   param->side
 * @param   &param->cont
 */
//----------------------------------------------------------------------------------
static void scEvent_CheckSideEffectParam( BTL_SVFLOW_WORK* wk, BtlSideEffect effect, BtlSide side, BPP_SICK_CONT* cont )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_SIDE, side );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_SIDE_EFFECT, effect );
    BTL_EVENTVAR_SetValue( BTL_EVAR_SICK_CONT, cont->raw );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHECK_SIDEEFF_PARAM );
    cont->raw = BTL_EVENTVAR_GetValue( BTL_EVAR_SICK_CONT );
  BTL_EVENTVAR_Pop();
}


//----------------------------------------------------------------------------------------------------------
// �ȉ��A�n���h������̉�����M�֐��ƃ��[�e�B���e�B�Q
//----------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] ��������
 *
 * @param   wk
 * @param   range   �����͈̔�  ��0�`(range-1) ���Ԃ�^0���w�肷���32bit�̍ő�l�܂ŕԂ�
 *
 * @retval  u32     ����
 */
//--------------------------------------------------------------------------------------
u32 BTL_SVFTOOL_GetRand( BTL_SVFLOW_WORK* wk, u32 range )
{
  return BTL_CALC_GetRand( range );
}

//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��]�w��ID�̃|�P�����p�����[�^��Ԃ�
 *
 * @param   wk
 * @param   pokeID
 *
 * @retval  const BTL_POKEPARAM*
 */
//--------------------------------------------------------------------------------------
const BTL_POKEPARAM* BTL_SVFTOOL_GetPokeParam( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  return BTL_POKECON_GetPokeParam( wk->pokeCon, pokeID );
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �w��|�PID�����|�P�������퓬�ɏo�Ă��邩�`�F�b�N���A�o�Ă����炻�̐퓬�ʒu��Ԃ�
 *
 * @param   server
 * @param   pokeID
 *
 * @retval  BtlPokePos    �o�Ă���ꍇ�͐퓬�ʒuID�^�o�Ă��Ȃ��ꍇ��BTL_POS_MAX
 */
//--------------------------------------------------------------------------------------
BtlPokePos BTL_SVFTOOL_GetExistFrontPokeID( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  return BTL_POSPOKE_GetPokeExistPos( &wk->pospokeWork, pokeID );
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] ��ɏo�Ă���S�Ă̑��葤�|�P����ID��z��Ɋi�[����
 *
 * @param   wk
 * @param   dst
 *
 * @retval  u8    ��ɏo�Ă���|�P������
 */
//--------------------------------------------------------------------------------------
u8 BTL_SVFTOOL_GetAllOpponentFrontPokeID( BTL_SVFLOW_WORK* wk, u8 basePokeID, u8* dst )
{
  FRONT_POKE_SEEK_WORK fps;
  BTL_POKEPARAM* bpp;
  u8 cnt = 0;

  FRONT_POKE_SEEK_InitWork( &fps, wk );
  while( FRONT_POKE_SEEK_GetNext( &fps, wk, &bpp ) )
  {
    if( !BTL_MAINUTIL_IsFriendPokeID( basePokeID, BPP_GetID(bpp) ) )
    {
      dst[ cnt++ ] = BPP_GetID( bpp );
    }
  }
  return cnt;
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �w��̂Ƃ����������|�P�������퓬�ɏo�Ă��邩�`�F�b�N
 *
 * @param   wk
 * @param   tokusei
 *
 * @retval  BOOL    �o�Ă�����TRUE
 */
//--------------------------------------------------------------------------------------
BOOL BTL_SVFTOOL_CheckExistTokuseiPokemon( BTL_SVFLOW_WORK* wk, PokeTokusei tokusei )
{
  FRONT_POKE_SEEK_WORK fps;
  BTL_POKEPARAM* bpp;
  FRONT_POKE_SEEK_InitWork( &fps, wk );
  while( FRONT_POKE_SEEK_GetNext( &fps, wk, &bpp ) )
  {
    if( BPP_GetValue(bpp, BPP_TOKUSEI_EFFECTIVE) == tokusei )
    {
      return TRUE;
    }
  }
  return FALSE;
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �_���[�W�v�Z�V�~�����[�g���ʂ�Ԃ�
 *
 * @param   flowWk
 * @param   atkPokeID       �U���|�PID
 * @param   defPokeID       �h��|�PID
 * @param   waza            ���U
 * @param   fAffinity       �����v�Z���邩�iFALSE�Ȃ瓙�{�Ōv�Z�j
 * @param   fCriticalCheck  �N���e�B�J�����肷�邩
 *
 * @retval  u32
 */
//--------------------------------------------------------------------------------------
u32 BTL_SVFTOOL_SimulationDamage( BTL_SVFLOW_WORK* wk, u8 atkPokeID, u8 defPokeID, WazaID waza, BOOL fAffinity, BOOL fCriticalCheck )
{
  const BTL_POKEPARAM* attacker = BTL_POKECON_GetPokeParam( wk->pokeCon, atkPokeID );
  const BTL_POKEPARAM* defender = BTL_POKECON_GetPokeParam( wk->pokeCon, defPokeID );

  BtlTypeAff  aff;
  BOOL  critical_flag;
  SVFL_WAZAPARAM  wazaParam;
  u16  damage;

  //
  if( fAffinity ){
    aff = CalcTypeAffForDamage( WAZADATA_GetType(waza), BPP_GetPokeType(defender) );
  }else{
    aff = BTL_TYPEAFF_100;
  }

  if( fCriticalCheck ){
    critical_flag = scEvent_CheckCritical( wk, attacker, defender, waza );
  }else{
    critical_flag = FALSE;
  }

  scEvent_GetWazaParam( wk, waza, attacker, &wazaParam );

  scEvent_CalcDamage( wk, attacker, defender, &wazaParam, aff,
    BTL_CALC_DMG_TARGET_RATIO_NONE, critical_flag, &damage );

  return damage;
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] ���N���C�A���g���S�����Ă���퓬�ʒu�̐���Ԃ�
 *
 * @param   wk
 *
 * @retval  u8
 */
//--------------------------------------------------------------------------------------
u8 BTL_SVFTOOL_GetClientCoverPosCount( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  SVCL_WORK* clwk;
  u8 clientID = BTL_MAINUTIL_PokeIDtoClientID( pokeID );

  clwk = BTL_SERVER_GetClientWork( wk->server, clientID );
  return clwk->numCoverPos;
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] ���N���C�A���g�p�[�e�B���A�T���̊J�nIndex���擾
 *
 * @param   wk
 * @param   pokeID
 *
 * @retval  u8
 */
//--------------------------------------------------------------------------------------
u8 BTL_SVFTOOL_GetMyBenchIndex( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  u8 pos = BTL_SVFTOOL_GetClientCoverPosCount( wk, pokeID );
  if( BTL_MAIN_GetRule(wk->mainModule) == BTL_RULE_ROTATION ){
    ++pos;
  }
  return pos;
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] ���N���C�A���g�p�[�e�B�ɁA�T���̃|�P�����i�퓬�\�ȁj�����邩
 *
 * @param   wk
 * @param   pokeID
 *
 * @retval  BOOL
 */
//--------------------------------------------------------------------------------------
BOOL BTL_SVFTOOL_IsExistBenchPoke( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  const BTL_PARTY* party = BTL_SVFTOOL_GetPartyData( wk, pokeID );
  u8 startIdx = BTL_SVFTOOL_GetMyBenchIndex( wk, pokeID );
  if( BTL_PARTY_GetAliveMemberCountRear(party, startIdx) ){
    return TRUE;
  }
  return FALSE;
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �Y���ʒu�ɂ��鐶�����Ă���|�P����ID��z��Ɋi�[������Ԃ�
 *
 * @param   wk
 * @param   exPos
 * @param   dst_pokeID    [out]�i�[��z��
 *
 * @retval  u8    �i�[������
 */
//--------------------------------------------------------------------------------------
u8 BTL_SVFTOOL_ExpandPokeID( BTL_SVFLOW_WORK* wk, BtlExPos exPos, u8* dst_pokeID )
{
  u8 pos[ BTL_POSIDX_MAX ];
  BTL_POKEPARAM* bpp;
  u8 max, num, i;

  max = BTL_MAIN_ExpandBtlPos( wk->mainModule, exPos, pos );
  for(i=0, num=0; i<max; ++i)
  {
    bpp = BTL_POKECON_GetFrontPokeData( wk->pokeCon, pos[i] );
    if( IsBppExist(bpp) ){
      dst_pokeID[num++] = BPP_GetID( bpp );
    }
  }
  return num;
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �w��|�P�������܂ރp�[�e�B�f�[�^���擾
 *
 * @param   wk
 * @param   pokeID
 *
 * @retval  const BTL_PARTY*
 */
//--------------------------------------------------------------------------------------
const BTL_PARTY* BTL_SVFTOOL_GetPartyData( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  u8 clientID = BTL_MAINUTIL_PokeIDtoClientID( pokeID );
  return BTL_POKECON_GetPartyDataConst( wk->pokeCon, clientID );
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �w��|�P�����̐퓬�ʒu��Ԃ�
 *
 * @param   wk
 * @param   pokeID
 *
 * @retval  BtlPokePos
 */
//--------------------------------------------------------------------------------------
BtlPokePos BTL_SVFTOOL_PokeIDtoPokePos( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  return BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, pokeID );
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �w��ʒu�̃|�P����ID��Ԃ�
 *
 * @param   wk
 * @param   pokePos
 *
 * @retval  u8
 */
//--------------------------------------------------------------------------------------
u8 BTL_SVFTOOL_PokePosToPokeID( BTL_SVFLOW_WORK* wk, u8 pokePos )
{
  const BTL_POKEPARAM* bpp = BTL_POKECON_GetFrontPokeDataConst( wk->pokeCon, pokePos );
  return BPP_GetID( bpp );
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �w��|�P�������u��т��ӂ�v�ȂǑ��̃��U���Ăяo�����ہA�^�[�Q�b�g�������Ō��߂�
 * ���^�[�Q�b�g�I���̕K�v���������U�̏ꍇ�ABTL_POS_NULL ���Ԃ�
 *
 * @param   wk
 * @param   pokeID
 * @param   waza
 *
 * @retval  BtlPokePos
 */
//--------------------------------------------------------------------------------------
BtlExPos BTL_SVFTOOL_ReqWazaTargetAuto( BTL_SVFLOW_WORK* wk, u8 pokeID, WazaID waza )
{
  BtlRule rule = BTL_MAIN_GetRule( wk->mainModule );
  BtlPokePos targetPos = BTL_POS_NULL;
  BtlPokePos myPos = BTL_SVFTOOL_PokeIDtoPokePos( wk, pokeID );
  WazaTarget  targetType = WAZADATA_GetParam( waza, WAZAPARAM_TARGET );

  // �V���O��
  if( rule == BTL_RULE_SINGLE )
  {
    switch( targetType ){
    case WAZA_TARGET_OTHER_SELECT:  ///< �����ȊO�̂P�́i�I���j
    case WAZA_TARGET_ENEMY_SELECT:  ///< �G�P�́i�I���j
    case WAZA_TARGET_ENEMY_RANDOM:  ///< �G�����_��
    case WAZA_TARGET_ENEMY_ALL:     ///< �G���Q��
    case WAZA_TARGET_OTHER_ALL:     ///< �����ȊO�S��
      targetPos = BTL_MAIN_GetOpponentPokePos( wk->mainModule, myPos, 0 );
      return targetPos;

    case WAZA_TARGET_USER:                ///< �����P�̂̂�
    case WAZA_TARGET_FRIEND_USER_SELECT:  ///< �������܂ޖ����P��
      targetPos = myPos;
      return targetPos;

    case WAZA_TARGET_ALL:
    case WAZA_TARGET_UNKNOWN:
    default:
      break;
    }
    return BTL_POS_NULL;
  }
  // �_�u���A�g���v���A���[�e����
  else
  {
    BtlExPos exPos;
    u8 pokeID[ BTL_POSIDX_MAX ];
    u8 pokeCnt = 0;

    switch( targetType ){
    case WAZA_TARGET_OTHER_SELECT:       ///< �ʏ�|�P�i�P�̑I���j
    case WAZA_TARGET_ENEMY_SELECT:       ///< ���葤�|�P�i�P�̑I���j
    case WAZA_TARGET_ENEMY_RANDOM:       ///< ����|�P�P�̃����_��
      exPos = EXPOS_MAKE( BTL_EXPOS_AREA_ENEMY, myPos );
      break;

    case WAZA_TARGET_FRIEND_USER_SELECT: ///< �������܂ޖ����|�P�i�P�̑I���j
      exPos = EXPOS_MAKE( BTL_EXPOS_AREA_MYTEAM, myPos );
      break;

    case WAZA_TARGET_FRIEND_SELECT:      ///< �����ȊO�̖����|�P�i�P�̑I���j
      exPos = EXPOS_MAKE( BTL_EXPOS_AREA_FRIENDS, myPos );
      break;

    case WAZA_TARGET_USER:               ///< �����̂�
      return myPos;

    default:
      return BTL_POS_NULL;
    }

    pokeCnt = BTL_SVFTOOL_ExpandPokeID( wk, exPos, pokeID );
    if( pokeCnt )
    {
      u8 rnd = BTL_CALC_GetRand( pokeCnt );
      return BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, pokeID[rnd] );
    }

    return BTL_POS_NULL;
  }
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] ���^�[���ɑI�����ꂽ�A�N�V�������e���擾
 *
 * @param   wk
 * @param   pokeID
 * @param   dst       [out]
 *
 * @retval  BOOL    �������擾�ł�����TRUE�i���^�[���A�Q�����Ă��Ȃ��|�P�Ȃǂ��w�肳�ꂽ��FALSE�j
 */
//--------------------------------------------------------------------------------------
BOOL BTL_SVFTOOL_GetThisTurnAction( BTL_SVFLOW_WORK* wk, u8 pokeID, BTL_ACTION_PARAM* dst )
{
  u32 i;
  for(i=0; i<wk->numActOrder; ++i)
  {
    if( BPP_GetID(wk->actOrder[i].bpp) == pokeID ){
      *dst = wk->actOrder[i].action;
      return TRUE;
    }
  }
  return FALSE;
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] ���̃^�[���̍s���v���C�I���e�B����Ɏg��ꂽ�f�����̒l���擾
 * �����̃^�[���ɍs�����Ă��Ȃ��|�P�������w�肳�ꂽ�ꍇ�A�f�̂��΂₳�l��Ԃ�
 *
 * @param   wokk
 * @param   pokeID
 *
 * @retval  u16
 */
//--------------------------------------------------------------------------------------
u16 BTL_SVFTOOL_GetThisTurnAgility( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  u32 i;
  for(i=0; i<wk->numActOrder; ++i)
  {
    if( BPP_GetID(wk->actOrder[i].bpp) == pokeID ){
      return (wk->actOrder[i].priority & 0xffff);
    }
  }

  {
    BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, pokeID );
    return BPP_GetValue( bpp, BPP_AGILITY );
  }
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �����̃A�N�V�������S�̂̉��Ԗڂ����擾
 *
 * @param   wk
 * @param   pokeID
 * @param   myOrder       [out] �����̏���
 * @param   totalAction   [out] �S�A�N�V������
 *
 * @retval  BOOL    �������擾�ł�����TRUE�i���^�[���A�Q�����Ă��Ȃ��|�P�Ȃǂ��w�肳�ꂽ��FALSE�j
 */
//--------------------------------------------------------------------------------------
BOOL BTL_SVFTOOL_GetMyActionOrder( BTL_SVFLOW_WORK* wk, u8 pokeID, u8* myOrder, u8* totalAction )
{
  u32 i;
  for(i=0; i<wk->numActOrder; ++i)
  {
    if( BPP_GetID(wk->actOrder[i].bpp) == pokeID ){
      *myOrder = i;
      *totalAction = wk->numActOrder;
      return TRUE;
    }
  }
  return FALSE;
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] ���O�ɏo���ꂽ���U���擾
 *
 * @param   wk
 *
 * @retval  WazaID
 */
//--------------------------------------------------------------------------------------
WazaID BTL_SVFTOOL_GetPrevExeWaza( BTL_SVFLOW_WORK* wk )
{
  return wk->prevExeWaza;
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �S�n���h�����L�̃e���|�������[�N�擪�A�h���X��Ԃ�
 *
 * @param   wk
 *
 * @retval  void*
 */
//--------------------------------------------------------------------------------------
void* BTL_SVFTOOL_GetTmpWork( BTL_SVFLOW_WORK* wk, u32 size )
{
  GF_ASSERT(size < sizeof(wk->handlerTmpWork) );
  return wk->handlerTmpWork;
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �o�����U���R�[�h�̃|�C���^���擾
 *
 * @param   wk
 *
 * @retval  const BTL_WAZAREC*
 */
//--------------------------------------------------------------------------------------
const BTL_WAZAREC* BTL_SVFTOOL_GetWazaRecord( BTL_SVFLOW_WORK* wk )
{
  return &wk->wazaRec;
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] Que�̈�\��
 *
 * @param   wk
 *
 * @retval  const BTL_WAZAREC*
 */
//--------------------------------------------------------------------------------------
u16 BTL_SVFTOOL_ReserveQuePos( BTL_SVFLOW_WORK* wk, ServerCmd cmd )
{
  return SCQUE_RESERVE_Pos( wk->que, cmd );
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] ���S�|�P���R�[�h�̃|�C���^���擾
 *
 * @param   wk
 *
 * @retval  const BTL_DEADREC*
 */
//--------------------------------------------------------------------------------------
const BTL_DEADREC* BTL_SVFTOOL_GetDeadRecord( BTL_SVFLOW_WORK* wk )
{
  return &wk->deadRec;
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] ���݂̌o�߃^�[�������擾
 *
 * @param   wk
 *
 * @retval  u16
 */
//--------------------------------------------------------------------------------------
u16 BTL_SVFTOOL_GetTurnCount( BTL_SVFLOW_WORK* wk )
{
  return wk->turnCount;
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �o�g�����[���擾
 *
 * @param   wk
 *
 * @retval  BtlRule
 */
//--------------------------------------------------------------------------------------
BtlRule BTL_SVFTOOL_GetRule( BTL_SVFLOW_WORK* wk )
{
  return BTL_MAIN_GetRule( wk->mainModule );
}
//--------------------------------------------------------------------------------------
/**
 *  [�n���h���p�c�[��] �ΐ�҃^�C�v���擾
 *
 * @param   wk
 *
 * @retval  BtlCompetitor
 */
//--------------------------------------------------------------------------------------
BtlCompetitor BTL_SVFTOOL_GetCompetitor( BTL_SVFLOW_WORK* wk )
{
  return BTL_MAIN_GetCompetitor( wk->mainModule );
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �퓬�n�`�^�C�v���擾
 *
 * @param   wk
 *
 * @retval  BtlBgType
 */
//--------------------------------------------------------------------------------------
BtlBgType BTL_SVFTOOL_GetLandForm( BTL_SVFLOW_WORK* wk )
{
  const BTL_FIELD_SITUATION* sit = BTL_MAIN_GetFieldSituation( wk->mainModule );
  return sit->bgType;
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �}���`���[�h���ǂ����`�F�b�N
 *
 * @param   wk
 *
 * @retval  BOOL
 */
//--------------------------------------------------------------------------------------
BOOL BTL_SVFTOOL_IsMultiMode( BTL_SVFLOW_WORK* wk )
{
  return BTL_MAIN_IsMultiMode( wk->mainModule );
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �i���O�|�P�������ǂ����`�F�b�N
 *
 * @param   wk
 * @param   pokeID
 *
 * @retval  BOOL
 */
//--------------------------------------------------------------------------------------
BOOL BTL_SVFTOOL_CheckSinkaMae( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  const BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, pokeID );
  u16 monsno = BPP_GetMonsNo( bpp );
  u16 i;
  for(i=SHINKA_COND_START; i<SHINKA_COND_MAX; ++i){
    if( SHINKA_GetParamByHandle(wk->sinkaArcHandle, monsno, 0, i) ){
      return TRUE;
    }
  }
  return FALSE;
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �f�����v�Z
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  u16
 */
//--------------------------------------------------------------------------------------
u16 BTL_SVFTOOL_CalcAgility( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  return scEvent_CalcAgility( wk, bpp );
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �V��擾
 *
 * @param   wk
 *
 * @retval  BtlWeather
 */
//--------------------------------------------------------------------------------------
BtlWeather BTL_SVFTOOL_GetWeather( BTL_SVFLOW_WORK* wk )
{
  return scEvent_GetWeather( wk );
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �f�o�b�O�t���O�擾
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  u16
 */
//--------------------------------------------------------------------------------------
BOOL BTL_SVFTOOL_GetDebugFlag( BTL_SVFLOW_WORK* wk, BtlDebugFlag flag )
{
  return BTL_MAIN_GetDebugFlag( wk->mainModule, flag );
}


//=============================================================================================
/**
 * [�n���h������̑���Ăяo��] ���������֎~�R�[�h�̒ǉ���S�N���C�A���g�ɒʒm
 *
 * @param   wk
 * @param   clientID      �֎~�R�[�h�𔭍s�����|�P����ID
 * @param   code          �֎~�R�[�h
 *
 */
//=============================================================================================
void BTL_SVFRET_CantEscapeAdd( BTL_SVFLOW_WORK* wk, u8 pokeID, BtlCantEscapeCode code )
{
  SCQUE_PUT_OP_CantEscape_Add( wk->que, pokeID, code );
}
//=============================================================================================
/**
 * [�n���h������̑���Ăяo��]
 *  �X�^���h�A���[���o�g���ŁA�v���C���[�|�P�����̎g�p�����n���h������Ăяo���ꂽ�ꍇ�̂�
 *  �����Â�������悹����i�l�R�ɂ��΂��p�j
 *
 * @param   wk
 * @param   volume    ��悹�z
 * @param   pokeID    ���U�g�p�|�P����ID
 *
 * @retval  BOOL      �����𖞂������i��悹�����j�ꍇ��TRUE���Ԃ�
 */
//=============================================================================================
BOOL BTL_SVFRET_AddBonusMoney( BTL_SVFLOW_WORK* wk, u32 volume, u8 pokeID )
{
  if( (BTL_MAIN_GetCommMode(wk->mainModule) == BTL_COMM_NONE)
  &&  (BTL_MAINUTIL_PokeIDtoClientID(pokeID) == BTL_STANDALONE_PLAYER_CLIENT_ID)
  ){
    BTL_SERVER_AddBonusMoney( wk->server, volume );
    return TRUE;
  }
  return FALSE;
}


//--------------------------------------------------------------------------------------------------------
// HandEx
//--------------------------------------------------------------------------------------------------------

static void Hem_Init( HANDLER_EXHIBISION_MANAGER* wk )
{
  wk->stack_ptr = 0;
  wk->read_ptr = 0;
}

static u32 Hem_PushState( HANDLER_EXHIBISION_MANAGER* wk )
{
  u32 state = (wk->stack_ptr<<16) | wk->read_ptr;
//  BTL_Printf(" *HEM-Push* sp=%d, rp=%d, next_rp=%d\n", wk->stack_ptr, wk->read_ptr, wk->stack_ptr);
  wk->read_ptr = wk->stack_ptr;
  return state;
}

static void Hem_PopState( HANDLER_EXHIBISION_MANAGER* wk, u32 state )
{
  wk->stack_ptr = (state >> 16) & 0xffff;
  wk->read_ptr  = state & 0xffff;
//  BTL_Printf(" *HEM-Pop* sp=%d, rp=%d\n", wk->stack_ptr, wk->read_ptr);
}

static u16 Hem_GetStackPtr( const HANDLER_EXHIBISION_MANAGER* wk )
{
  return wk->stack_ptr;
}

static BTL_HANDEX_PARAM_HEADER* Hem_ReadWork( HANDLER_EXHIBISION_MANAGER* wk )
{
  if( wk->read_ptr < wk->stack_ptr )
  {
    BTL_HANDEX_PARAM_HEADER* header = (BTL_HANDEX_PARAM_HEADER*)(&wk->workBuffer[wk->read_ptr]);
    wk->read_ptr += header->size;
//    BTL_Printf(" *HEM-Read sp=%d, rp=%d\n", wk->stack_ptr, wk->read_ptr);

    GF_ASSERT( header->equip < BTL_HANDEX_MAX );
    GF_ASSERT( wk->read_ptr <= wk->stack_ptr );

    return header;
  }
  else
  {
    return NULL;
  }
}

static BTL_HANDEX_PARAM_HEADER* Hem_PushWork( HANDLER_EXHIBISION_MANAGER* wk, BtlEventHandlerExhibition eq_type, u8 userPokeID )
{
  static const struct {
    u8 eq_type;
    u8 size;
  }work_size_table[] = {
    { BTL_HANDEX_USE_ITEM,             sizeof(BTL_HANDEX_PARAM_USE_ITEM)            },
    { BTL_HANDEX_RECOVER_HP,           sizeof(BTL_HANDEX_PARAM_RECOVER_HP)          },
    { BTL_HANDEX_DRAIN,                sizeof(BTL_HANDEX_PARAM_DRAIN)               },
    { BTL_HANDEX_SHIFT_HP,             sizeof(BTL_HANDEX_PARAM_SHIFT_HP)            },
    { BTL_HANDEX_RECOVER_PP,           sizeof(BTL_HANDEX_PARAM_PP)                  },
    { BTL_HANDEX_DECREMENT_PP,         sizeof(BTL_HANDEX_PARAM_PP)                  },
    { BTL_HANDEX_CURE_SICK,            sizeof(BTL_HANDEX_PARAM_CURE_SICK)           },
    { BTL_HANDEX_ADD_SICK,             sizeof(BTL_HANDEX_PARAM_ADD_SICK)            },
    { BTL_HANDEX_RANK_EFFECT,          sizeof(BTL_HANDEX_PARAM_RANK_EFFECT)         },
    { BTL_HANDEX_SET_RANK,             sizeof(BTL_HANDEX_PARAM_SET_RANK)            },
    { BTL_HANDEX_RECOVER_RANK,         sizeof(BTL_HANDEX_PARAM_RECOVER_RANK)        },
    { BTL_HANDEX_RESET_RANK,           sizeof(BTL_HANDEX_PARAM_RESET_RANK)          },
    { BTL_HANDEX_SET_STATUS,           sizeof(BTL_HANDEX_PARAM_SET_STATUS)          },
    { BTL_HANDEX_DAMAGE,               sizeof(BTL_HANDEX_PARAM_DAMAGE)              },
    { BTL_HANDEX_KILL,                 sizeof(BTL_HANDEX_PARAM_KILL)                },
    { BTL_HANDEX_CHANGE_TYPE,          sizeof(BTL_HANDEX_PARAM_CHANGE_TYPE)         },
    { BTL_HANDEX_MESSAGE,              sizeof(BTL_HANDEX_PARAM_MESSAGE)             },
    { BTL_HANDEX_TOKWIN_IN,            sizeof(BTL_HANDEX_PARAM_HEADER)              },
    { BTL_HANDEX_TOKWIN_OUT,           sizeof(BTL_HANDEX_PARAM_HEADER)              },
    { BTL_HANDEX_ITEM_EFFECT,          sizeof(BTL_HANDEX_PARAM_HEADER)              },
    { BTL_HANDEX_SET_TURNFLAG,         sizeof(BTL_HANDEX_PARAM_TURNFLAG)            },
    { BTL_HANDEX_RESET_TURNFLAG,       sizeof(BTL_HANDEX_PARAM_TURNFLAG)            },
    { BTL_HANDEX_SET_CONTFLAG,         sizeof(BTL_HANDEX_PARAM_SET_CONTFLAG)        },
    { BTL_HANDEX_RESET_CONTFLAG,       sizeof(BTL_HANDEX_PARAM_SET_CONTFLAG)        },
    { BTL_HANDEX_SIDEEFF_ADD,          sizeof(BTL_HANDEX_PARAM_SIDEEFF_ADD)         },
    { BTL_HANDEX_SIDEEFF_REMOVE,       sizeof(BTL_HANDEX_PARAM_SIDEEFF_REMOVE)      },
    { BTL_HANDEX_ADD_FLDEFF,           sizeof(BTL_HANDEX_PARAM_ADD_FLDEFF)          },
    { BTL_HANDEX_CHANGE_WEATHER,       sizeof(BTL_HANDEX_PARAM_CHANGE_WEATHER)      },
    { BTL_HANDEX_REMOVE_FLDEFF,        sizeof(BTL_HANDEX_PARAM_REMOVE_FLDEFF)       },
    { BTL_HANDEX_POSEFF_ADD,           sizeof(BTL_HANDEX_PARAM_POSEFF_ADD)          },
    { BTL_HANDEX_CHANGE_TOKUSEI,       sizeof(BTL_HANDEX_PARAM_CHANGE_TOKUSEI)      },
    { BTL_HANDEX_SET_ITEM,             sizeof(BTL_HANDEX_PARAM_SET_ITEM)            },
    { BTL_HANDEX_EQUIP_ITEM,           sizeof(BTL_HANDEX_PARAM_EQUIP_ITEM)          },
    { BTL_HANDEX_ITEM_SP,              sizeof(BTL_HANDEX_PARAM_ITEM_SP)             },
    { BTL_HANDEX_CONSUME_ITEM,         sizeof(BTL_HANDEX_PARAM_CONSUME_ITEM)        },
    { BTL_HANDEX_CLEAR_CONSUMED_ITEM,  sizeof(BTL_HANDEX_PARAM_CLEAR_CONSUMED_ITEM) },
    { BTL_HANDEX_SWAP_ITEM,            sizeof(BTL_HANDEX_PARAM_SWAP_ITEM)           },
    { BTL_HANDEX_UPDATE_WAZA,          sizeof(BTL_HANDEX_PARAM_UPDATE_WAZA)         },
    { BTL_HANDEX_COUNTER,              sizeof(BTL_HANDEX_PARAM_COUNTER)             },
    { BTL_HANDEX_DELAY_WAZADMG,        sizeof(BTL_HANDEX_PARAM_DELAY_WAZADMG)       },
    { BTL_HANDEX_QUIT_BATTLE,          sizeof(BTL_HANDEX_PARAM_HEADER)              },
    { BTL_HANDEX_CHANGE_MEMBER,        sizeof(BTL_HANDEX_PARAM_CHANGE_MEMBER)       },
    { BTL_HANDEX_BATONTOUCH,           sizeof(BTL_HANDEX_PARAM_BATONTOUCH)          },
    { BTL_HANDEX_ADD_SHRINK,           sizeof(BTL_HANDEX_PARAM_ADD_SHRINK)          },
    { BTL_HANDEX_RELIVE,               sizeof(BTL_HANDEX_PARAM_RELIVE)              },
    { BTL_HANDEX_SET_WEIGHT,           sizeof(BTL_HANDEX_PARAM_SET_WEIGHT)          },
    { BTL_HANDEX_PUSHOUT,              sizeof(BTL_HANDEX_PARAM_PUSHOUT)             },
    { BTL_HANDEX_INTR_POKE,            sizeof(BTL_HANDEX_PARAM_INTR_POKE)           },
    { BTL_HANDEX_INTR_WAZA,            sizeof(BTL_HANDEX_PARAM_INTR_WAZA)           },
    { BTL_HANDEX_SEND_LAST,            sizeof(BTL_HANDEX_PARAM_SEND_LAST)           },
    { BTL_HANDEX_SWAP_POKE,            sizeof(BTL_HANDEX_PARAM_SWAP_POKE)           },
    { BTL_HANDEX_HENSIN,               sizeof(BTL_HANDEX_PARAM_HENSIN)              },
    { BTL_HANDEX_FAKE_BREAK,           sizeof(BTL_HANDEX_PARAM_FAKE_BREAK)          },
    { BTL_HANDEX_JURYOKU_CHECK,        sizeof(BTL_HANDEX_PARAM_HEADER)              },
    { BTL_HANDEX_EFFECT_BY_POS,        sizeof(BTL_HANDEX_PARAM_EFFECT_BY_POS)       },
  };
  u32 size, i;

  for(i=0, size=0; i<NELEMS(work_size_table); ++i)
  {
    if( work_size_table[i].eq_type == eq_type ){
      size = work_size_table[i].size;
      break;
    }
  }

  if( size )
  {
    while( size % 4 ){ ++size; }
    if( (wk->stack_ptr + size) <= sizeof(wk->workBuffer) )
    {
      BTL_HANDEX_PARAM_HEADER* header = (BTL_HANDEX_PARAM_HEADER*) &(wk->workBuffer[wk->stack_ptr]);

      for(i=0; i<size; ++i){
        wk->workBuffer[ wk->stack_ptr + i ] = 0;
      }
      header->equip = eq_type;
      header->size = size;
      header->userPokeID = userPokeID;
      header->tokwin_flag = 0;
      wk->stack_ptr += size;
      BTL_Printf("Get Hem Work: ADRS=0x%p, type=%d, size=%d, pokeID=%d\n", header, header->equip, size, userPokeID);
      return header;
    }
    else
    {
      GF_ASSERT_MSG(0, "stack over flow ... eq_type=%d, pokeID=%d", eq_type, userPokeID);
    }
  }
  else
  {
    GF_ASSERT_MSG(0, "illegal eq_type = %d", eq_type);
  }
  return NULL;
}


//=============================================================================================
/**
 * HandEx ���[�N�P���擾
 *
 * @param   wk
 * @param   eq_type
 * @param   userPokeID
 *
 * @retval  void*
 */
//=============================================================================================
void* BTL_SVF_HANDEX_Push( BTL_SVFLOW_WORK* wk, BtlEventHandlerExhibition eq_type, u8 userPokeID )
{
  return Hem_PushWork( &wk->HEManager, eq_type, userPokeID );
}

//---------------------------------------------------------------------------------------------
// ���^�[�������Ԃ�|�P���R�[�h
//---------------------------------------------------------------------------------------------

// ������
static void relivePokeRec_Init( BTL_SVFLOW_WORK* wk )
{
  wk->numRelivePoke = 0;
}

// �����Ԃ�L�^�ǉ�
static void relivePokeRec_Add( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  u32 i;

  for(i=0; i<wk->numRelivePoke; ++i)
  {
    if( wk->relivedPokeID[i] == pokeID ){
      return;
    }
  }

  if( i < NELEMS(wk->relivedPokeID) ){
    wk->relivedPokeID[i] = pokeID;
    wk->numRelivePoke++;
  }
}

// �^�[���I�����A�|�P��������̕K�v�����邩����
static BOOL relivePokeRec_CheckNecessaryPokeIn( BTL_SVFLOW_WORK* wk )
{
  u32 i;
  u8 pos[ BTL_POSIDX_MAX ];
  u8 clientID;

  for(i=0; i<wk->numRelivePoke; ++i)
  {
    clientID = BTL_MAINUTIL_PokeIDtoClientID( wk->relivedPokeID[i] );

    // �P���ł��󂫈ʒu������Γ��ꂳ����K�v�A��
    if( BTL_POSPOKE_GetClientEmptyPos(&wk->pospokeWork, clientID, pos) ){
      return TRUE;
    }
  }
  return FALSE;
}


//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/**
 * �n���h�������X�^�b�N�������[�g
 *
 * @param   wk
 * @param   useItemID   �A�C�e���g�p�ɂ��Ăяo���̏ꍇ�A���̃A�C�e��ID�^����ȊO�AITEM_DUMMY_DATA
 */
//----------------------------------------------------------------------------------
static BOOL scproc_HandEx_Root( BTL_SVFLOW_WORK* wk, u16 useItemID )
{
  BTL_HANDEX_PARAM_HEADER* handEx_header;
  u8 fSucceed, fPrevSucceed;

  fSucceed = FALSE;
  fPrevSucceed = TRUE;

  while( (handEx_header = Hem_ReadWork(&wk->HEManager)) != NULL )
  {
    BTL_Printf("ProcHandEX : ADRS=0x%p, type=%d\n", handEx_header, handEx_header->equip);
    if( handEx_header->failSkipFlag && (fPrevSucceed == FALSE) ){
      continue;
    }
    switch( handEx_header->equip ){
    case BTL_HANDEX_TOKWIN_IN:          fPrevSucceed = scproc_HandEx_TokWinIn( wk, handEx_header ); break;
    case BTL_HANDEX_TOKWIN_OUT:         fPrevSucceed = scproc_HandEx_TokWinOut( wk, handEx_header ); break;
    case BTL_HANDEX_ITEM_EFFECT:        fPrevSucceed = scproc_HandEx_ItemEffect( wk, handEx_header ); break;
    case BTL_HANDEX_USE_ITEM:           fPrevSucceed = scproc_HandEx_useItem( wk, handEx_header ); break;
    case BTL_HANDEX_RECOVER_HP:         fPrevSucceed = scproc_HandEx_recoverHP( wk, handEx_header, useItemID ); break;
    case BTL_HANDEX_DRAIN:              fPrevSucceed = scproc_HandEx_drain( wk, handEx_header, useItemID ); break;
    case BTL_HANDEX_DAMAGE:             fPrevSucceed = scproc_HandEx_damage( wk, handEx_header ); break;
    case BTL_HANDEX_SHIFT_HP:           fPrevSucceed = scproc_HandEx_shiftHP( wk, handEx_header ); break;
    case BTL_HANDEX_RECOVER_PP:         fPrevSucceed = scproc_HandEx_recoverPP( wk, handEx_header, useItemID ); break;
    case BTL_HANDEX_DECREMENT_PP:       fPrevSucceed = scproc_HandEx_decrementPP( wk, handEx_header, useItemID ); break;
    case BTL_HANDEX_CURE_SICK:          fPrevSucceed = scproc_HandEx_cureSick( wk, handEx_header, useItemID ); break;
    case BTL_HANDEX_ADD_SICK:           fPrevSucceed = scproc_HandEx_addSick( wk, handEx_header ); break;
    case BTL_HANDEX_RANK_EFFECT:        fPrevSucceed = scproc_HandEx_rankEffect( wk, handEx_header, useItemID ); break;
    case BTL_HANDEX_SET_RANK:           fPrevSucceed = scproc_HandEx_setRank( wk, handEx_header ); break;
    case BTL_HANDEX_RECOVER_RANK:       fPrevSucceed = scproc_HandEx_recoverRank( wk, handEx_header ); break;
    case BTL_HANDEX_RESET_RANK:         fPrevSucceed = scproc_HandEx_resetRank( wk, handEx_header ); break;
    case BTL_HANDEX_SET_STATUS:         fPrevSucceed = scproc_HandEx_setStatus( wk, handEx_header ); break;
    case BTL_HANDEX_KILL:               fPrevSucceed = scproc_HandEx_kill( wk, handEx_header ); break;
    case BTL_HANDEX_CHANGE_TYPE:        fPrevSucceed = scproc_HandEx_changeType( wk, handEx_header ); break;
    case BTL_HANDEX_MESSAGE:            fPrevSucceed = scproc_HandEx_message( wk, handEx_header ); break;
    case BTL_HANDEX_SET_TURNFLAG:       fPrevSucceed = scproc_HandEx_setTurnFlag( wk, handEx_header ); break;
    case BTL_HANDEX_RESET_TURNFLAG:     fPrevSucceed = scproc_HandEx_resetTurnFlag( wk, handEx_header ); break;
    case BTL_HANDEX_SET_CONTFLAG:       fPrevSucceed = scproc_HandEx_setContFlag( wk, handEx_header ); break;
    case BTL_HANDEX_RESET_CONTFLAG:     fPrevSucceed = scproc_HandEx_resetContFlag( wk, handEx_header ); break;
    case BTL_HANDEX_SIDEEFF_ADD:        fPrevSucceed = scproc_HandEx_sideEffectAdd( wk, handEx_header ); break;
    case BTL_HANDEX_SIDEEFF_REMOVE:     fPrevSucceed = scproc_HandEx_sideEffectRemove( wk, handEx_header ); break;
    case BTL_HANDEX_ADD_FLDEFF:         fPrevSucceed = scproc_HandEx_addFieldEffect( wk, handEx_header ); break;
    case BTL_HANDEX_CHANGE_WEATHER:     fPrevSucceed = scproc_HandEx_changeWeather( wk, handEx_header ); break;
    case BTL_HANDEX_REMOVE_FLDEFF:      fPrevSucceed = scproc_HandEx_removeFieldEffect( wk, handEx_header ); break;
    case BTL_HANDEX_POSEFF_ADD:         fPrevSucceed = scproc_HandEx_PosEffAdd( wk, handEx_header ); break;
    case BTL_HANDEX_CHANGE_TOKUSEI:     fPrevSucceed = scproc_HandEx_tokuseiChange( wk, handEx_header ); break;
    case BTL_HANDEX_SET_ITEM:           fPrevSucceed = scproc_HandEx_setItem( wk, handEx_header ); break;
    case BTL_HANDEX_EQUIP_ITEM:         fPrevSucceed = scproc_HandEx_EquipItem( wk, handEx_header ); break;
    case BTL_HANDEX_ITEM_SP:            fPrevSucceed = scproc_HandEx_ItemSP( wk, handEx_header ); break;
    case BTL_HANDEX_CONSUME_ITEM:       fPrevSucceed = scproc_HandEx_consumeItem( wk, handEx_header ); break;
    case BTL_HANDEX_CLEAR_CONSUMED_ITEM:fPrevSucceed = scproc_HandEx_clearConsumedItem( wk, handEx_header ); break;
    case BTL_HANDEX_SWAP_ITEM:          fPrevSucceed = scproc_HandEx_swapItem( wk, handEx_header ); break;
    case BTL_HANDEX_UPDATE_WAZA:        fPrevSucceed = scproc_HandEx_updateWaza( wk, handEx_header ); break;
    case BTL_HANDEX_COUNTER:            fPrevSucceed = scproc_HandEx_counter( wk, handEx_header ); break;
    case BTL_HANDEX_DELAY_WAZADMG:      fPrevSucceed = scproc_HandEx_delayWazaDamage( wk, handEx_header ); break;
    case BTL_HANDEX_QUIT_BATTLE:        fPrevSucceed = scproc_HandEx_quitBattle( wk, handEx_header ); break;
    case BTL_HANDEX_CHANGE_MEMBER:      fPrevSucceed = scproc_HandEx_changeMember( wk, handEx_header ); break;
    case BTL_HANDEX_BATONTOUCH:         fPrevSucceed = scproc_HandEx_batonTouch( wk, handEx_header ); break;
    case BTL_HANDEX_ADD_SHRINK:         fPrevSucceed = scproc_HandEx_addShrink( wk, handEx_header ); break;
    case BTL_HANDEX_RELIVE:             fPrevSucceed = scproc_HandEx_relive( wk, handEx_header ); break;
    case BTL_HANDEX_SET_WEIGHT:         fPrevSucceed = scproc_HandEx_setWeight( wk, handEx_header ); break;
    case BTL_HANDEX_PUSHOUT:            fPrevSucceed = scproc_HandEx_pushOut( wk, handEx_header ); break;
    case BTL_HANDEX_INTR_POKE:          fPrevSucceed = scproc_HandEx_intrPoke( wk, handEx_header ); break;
    case BTL_HANDEX_INTR_WAZA:          fPrevSucceed = scproc_HandEx_intrWaza( wk, handEx_header ); break;
    case BTL_HANDEX_SEND_LAST:          fPrevSucceed = scproc_HandEx_sendLast( wk, handEx_header ); break;
    case BTL_HANDEX_SWAP_POKE:          fPrevSucceed = scproc_HandEx_swapPoke( wk, handEx_header ); break;
    case BTL_HANDEX_HENSIN:             fPrevSucceed = scproc_HandEx_hensin( wk, handEx_header ); break;
    case BTL_HANDEX_FAKE_BREAK:         fPrevSucceed = scproc_HandEx_fakeBreak( wk, handEx_header ); break;
    case BTL_HANDEX_JURYOKU_CHECK:      fPrevSucceed = scproc_HandEx_juryokuCheck( wk, handEx_header ); break;
    case BTL_HANDEX_EFFECT_BY_POS:      fPrevSucceed = scproc_HandEx_effectByPos( wk, handEx_header ); break;
    default:
      GF_ASSERT_MSG(0, "illegal handEx type = %d, userPokeID=%d", handEx_header->equip, handEx_header->userPokeID);
    }
    if( fPrevSucceed ){
      fSucceed = TRUE;
    }
  }
  return fSucceed;
}
/**
 * �Ƃ������E�B���h�E�\��
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_TokWinIn( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_POKEPARAM* pp_user = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );
  scPut_TokWin_In( wk, pp_user );
  return 1;
}
/**
 * �Ƃ������E�B���h�E����
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_TokWinOut( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_POKEPARAM* pp_user = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );
  scPut_TokWin_Out( wk, pp_user );
  return 1;
}
/**
 * �Ƃ������E�B���h�E����
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_ItemEffect( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BtlPokePos  pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, param_header->userPokeID );
  SCQUE_PUT_ACT_EffectByPos( wk->que, pos, BTLEFF_USE_ITEM );
  return 1;
}
/**
 * �|�P����HP��
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_recoverHP( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID )
{
  const BTL_HANDEX_PARAM_RECOVER_HP* param = (const BTL_HANDEX_PARAM_RECOVER_HP*)param_header;
  BTL_POKEPARAM* pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  BTL_POKEPARAM* pp_user = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );

  u8 result = 0;

  if( !BPP_IsDead(pp_target)
  &&  !BPP_IsHPFull(pp_target)
  ){
    if( param_header->tokwin_flag ){
      scPut_TokWin_In( wk, pp_user );
    }

    if( scproc_RecoverHP(wk, pp_target, param->recoverHP) ){
      if( param->exStr.type != BTL_STRTYPE_NULL ){
        handexSub_putString( wk, &(param->exStr) );
      }else if( itemID != ITEM_DUMMY_DATA ){
        SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_UseItem_RecoverHP, param->pokeID, itemID );
      }
      result = 1;
    }

    if( param_header->tokwin_flag ){
      scPut_TokWin_Out( wk, pp_user );
    }
  }
  return result;
}
/**
 * �|�P����HP�񕜁i�h���C���n�j
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_drain( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID )
{
  const BTL_HANDEX_PARAM_DRAIN* param = (BTL_HANDEX_PARAM_DRAIN*)param_header;
  BTL_POKEPARAM* pp_recover = BTL_POKECON_GetPokeParam( wk->pokeCon, param->recoverPokeID );
  BTL_POKEPARAM* pp_damaged = BTL_POKECON_GetPokeParam( wk->pokeCon, param->damagedPokeID );

  if( !BPP_IsDead(pp_recover)
  &&  !BPP_IsHPFull(pp_recover)
  ){
    if( scproc_DrainCore(wk, pp_recover, pp_damaged, param->recoverHP) ){
      if( param->exStr.type != BTL_STRTYPE_NULL ){
        handexSub_putString( wk, &(param->exStr) );
      }
      return 1;
    }
  }
  return 0;
}
/**
 * �|�P�����Ƀ_���[�W
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_damage( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_DAMAGE* param = (BTL_HANDEX_PARAM_DAMAGE*)param_header;
  BTL_POKEPARAM* pp_target;

  pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  if( !BPP_IsDead(pp_target) )
  {
    if( scproc_SimpleDamage(wk, pp_target, param->damage, &param->exStr) ){
      return 1;
    }
  }

  return 0;
}
/**
 * �|�P����HP�����i�񕜁^�_���[�W�ƌ��Ȃ��Ȃ��j
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_shiftHP( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_SHIFT_HP* param = (BTL_HANDEX_PARAM_SHIFT_HP*)param_header;
  BTL_POKEPARAM* pp_target;
  u32 i;
  u8 result = 0;

  for(i=0; i<param->poke_cnt; ++i)
  {
    pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID[i] );
    if( !BPP_IsDead(pp_target) ){
      scPut_SimpleHp( wk, pp_target, param->volume[i], !param->fEffectDisable );
      scproc_CheckItemReaction( wk, pp_target );
      result = 1;
    }
  }
  return result;
}
/**
 * �|�P����PP��
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_recoverPP( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID )
{
  const BTL_HANDEX_PARAM_PP* param = (BTL_HANDEX_PARAM_PP*)param_header;
  BTL_POKEPARAM* pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  BTL_POKEPARAM* pp_user = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );

  if( !BPP_IsDead(pp_target) )
  {
    if( !BPP_IsPPFull(pp_target, param->wazaIdx) ){
      scPut_RecoverPP( wk, pp_target, param->wazaIdx, param->volume, itemID );
      handexSub_putString( wk, &param->exStr );
      return 1;
    }
  }
  return 0;
}
/**
 * �|�P����PP����
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_decrementPP( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID )
{
  const BTL_HANDEX_PARAM_PP* param = (BTL_HANDEX_PARAM_PP*)param_header;

  BTL_POKEPARAM* pokesetTarget = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  if( scproc_decrementPP(wk, pokesetTarget, param->wazaIdx, param->volume) ){
    return 1;
  }
  return 0;
}
/**
 * �|�P������Ԉُ��
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_cureSick( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID )
{
  const BTL_HANDEX_PARAM_CURE_SICK* param = (BTL_HANDEX_PARAM_CURE_SICK*)param_header;
  BTL_POKEPARAM* pp_user = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );
  u8 result = 0;

  if( param_header->tokwin_flag ){
    scPut_TokWin_In( wk, pp_user );
  }

  {
    BTL_POKEPARAM* pp_target;
    WazaSick cured_sick;
    u32 i;
    for(i=0; i<param->poke_cnt; ++i)
    {
      pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID[i] );
      if( !BPP_IsDead(pp_target) )
      {
        BPP_SICK_CONT oldCont;
        cured_sick = scPut_CureSick( wk, pp_target, param->sickCode, &oldCont );
        if( cured_sick != WAZASICK_NULL )
        {
          if( !param->fStdMsgDisable )
          {
            if( BTL_SICK_MakeDefaultCureMsg(cured_sick, oldCont, pp_target, itemID, &wk->strParam) ){
              handexSub_putString( wk, &wk->strParam );
            }
          }
          else
          {
            handexSub_putString( wk, &param->exStr );
          }
          result = 1;
        }
      }
    }
  }

  if( param_header->tokwin_flag ){
    scPut_TokWin_Out( wk, pp_user );
  }

  return result;
}
/**
 * �|�P������Ԉُ퉻
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_addSick( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_ADD_SICK* param = (BTL_HANDEX_PARAM_ADD_SICK*)param_header;
  BTL_POKEPARAM* pp_user = (param_header->userPokeID != BTL_POKEID_NULL)?
                      BTL_POKECON_GetPokeParam(wk->pokeCon, param_header->userPokeID) : NULL;

  BTL_POKEPARAM* target;
  u32 i;

  u8 result = 0;
  u8 fDefaultMsg = ( (HANDEX_STR_IsEnable(&param->exStr) == FALSE) && (param->fStdMsgDisable == FALSE) );

  for(i=0; i<param->poke_cnt; ++i)
  {
    target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID[i] );
    if( !BPP_IsDead(target) )
    {
      if( scproc_AddSick(wk, target, pp_user, param->sickID, param->sickCont, param->fAlmost, fDefaultMsg) ){
        result = 1;
      }
    }
  }

  if( result )
  {
    if( !fDefaultMsg ){
      if( param->header.tokwin_flag ){
        scPut_TokWin_In( wk, pp_user );
      }
      handexSub_putString( wk, &param->exStr );
      if( param->header.tokwin_flag ){
        scPut_TokWin_Out( wk, pp_user );
      }
    }
  }

  return result;
}
/**
 * �|�P���������N���ʁi�i�K�����w�肵�A�㏸�܂��͉��~������B����̂Ƃ��������ɂ�莸�s���邱�Ƃ����蓾��j
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_rankEffect( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID )
{
  const BTL_HANDEX_PARAM_RANK_EFFECT* param = (BTL_HANDEX_PARAM_RANK_EFFECT*)param_header;
  BTL_POKEPARAM* pp_user = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );
  BTL_POKEPARAM* pp_target;
  u8 fEffective = FALSE;
  u8 result = 0;
  u32 i;


  for(i=0; i<param->poke_cnt; ++i)
  {
    pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID[i] );
    if( !BPP_IsDead( pp_target )
    &&  BPP_IsRankEffectValid( pp_target, param->rankType, param->rankVolume )
    ){
      fEffective = TRUE;
      break;
    }
  }
  if( fEffective && param_header->tokwin_flag ){
    scPut_TokWin_In( wk, pp_user );
  }

  for(i=0; i<param->poke_cnt; ++i)
  {
    pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID[i] );
    if( !BPP_IsDead(pp_target) )
    {
      if( scproc_RankEffectCore(wk, param_header->userPokeID, pp_target, param->rankType, param->rankVolume,
        BTL_POKEID_NULL, itemID, param->fAlmost, !(param->fStdMsgDisable))
      ){
        handexSub_putString( wk, &param->exStr );
        result = 1;
      }
    }
  }

  if( fEffective && param_header->tokwin_flag ){
    scPut_TokWin_Out( wk, pp_user );
  }

  return result;
}
/**
 * �|�P���������N�����Z�b�g�i�w��l�ɋ����I�ɏ���������B���s���Ȃ��j
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_setRank( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_SET_RANK* param = (BTL_HANDEX_PARAM_SET_RANK*)param_header;
  BTL_POKEPARAM* pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  if( !BPP_IsDead(pp_target) )
  {
    BPP_RankSet( pp_target, BPP_ATTACK,     param->attack );
    BPP_RankSet( pp_target, BPP_DEFENCE,    param->defence );
    BPP_RankSet( pp_target, BPP_SP_ATTACK,  param->sp_attack );
    BPP_RankSet( pp_target, BPP_SP_DEFENCE, param->sp_defence );
    BPP_RankSet( pp_target, BPP_AGILITY,    param->agility );
    SCQUE_PUT_OP_RankSet5( wk->que,
      param->pokeID, param->attack, param->defence, param->sp_attack, param->sp_defence, param->agility );
    return 1;
  }
  return 0;
}
/**
 * �|�P���������N�̓��A�}�C�i�X�ɂȂ��Ă�����̂̂݃t���b�g�ɖ߂�
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_recoverRank( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_RECOVER_RANK* param = (BTL_HANDEX_PARAM_RECOVER_RANK*)param_header;
  BTL_POKEPARAM* pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  if( !BPP_IsDead(pp_target) )
  {
    BPP_RankRecover( pp_target );
    return 1;
  }
  return 0;
}
/**
 * �|�P���������N��S�ăt���b�g�ɖ߂�
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_resetRank( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_RESET_RANK* param = (BTL_HANDEX_PARAM_RESET_RANK*)param_header;
  BTL_POKEPARAM* pp_target;
  u32 i;
  u8 result = 0;

  for(i=0; i<param->poke_cnt; ++i){
    pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID[i] );
    if( !BPP_IsDead(pp_target) ){
      BPP_RankReset( pp_target );
      result = 1;
    }
  }
  return result;
}
/**
 * �|�P�����\�͒l�i�U���A�h�䓙�j���w��l�ɏ�������
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_setStatus( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_SET_STATUS* param = (BTL_HANDEX_PARAM_SET_STATUS*)param_header;
  BTL_POKEPARAM* bpp;

  bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  if( param->fAttackEnable ){
    BPP_SetBaseStatus( bpp, BPP_ATTACK, param->attack );
    SCQUE_PUT_OP_SetStatus( wk->que, param->pokeID, BPP_ATTACK, param->attack );
  }
  if( param->fDefenceEnable ){
    BPP_SetBaseStatus( bpp, BPP_DEFENCE, param->defence );
    SCQUE_PUT_OP_SetStatus( wk->que, param->pokeID, BPP_DEFENCE, param->defence );
  }
  if( param->fSpAttackEnable ){
    BPP_SetBaseStatus( bpp, BPP_SP_ATTACK, param->sp_attack );
    SCQUE_PUT_OP_SetStatus( wk->que, param->pokeID, BPP_SP_ATTACK, param->sp_attack );
  }
  if( param->fSpDefenceEnable ){
    BPP_SetBaseStatus( bpp, BPP_SP_DEFENCE, param->sp_defence );
    SCQUE_PUT_OP_SetStatus( wk->que, param->pokeID, BPP_SP_DEFENCE, param->sp_defence );
  }
  if( param->fAgilityEnable ){
    BPP_SetBaseStatus( bpp, BPP_AGILITY, param->agility );
    SCQUE_PUT_OP_SetStatus( wk->que, param->pokeID, BPP_AGILITY, param->agility );
  }

  handexSub_putString( wk, &param->exStr );

  return 1;
}
/**
 * �|�P�����������I�ɂЂ񎀂ɂ�����
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_kill( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_KILL* param = (BTL_HANDEX_PARAM_KILL*)param_header;
  BTL_POKEPARAM* pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  if( !BPP_IsDead(pp_target) ){
    scproc_KillPokemon( wk, pp_target );
    handexSub_putString( wk, &param->exStr );
    return 1;
  }
  return 0;
}
/**
 * �|�P�����^�C�v�ύX
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_changeType( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_CHANGE_TYPE* param= (const BTL_HANDEX_PARAM_CHANGE_TYPE*)param_header;

  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  if( !BPP_IsDead(bpp) )
  {
    PokeType type_pure = PokeTypePair_GetType1( param->next_type );

    SCQUE_PUT_OP_ChangePokeType( wk->que, param->pokeID, param->next_type );
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_ChangePokeType, param->pokeID, type_pure );
    BPP_ChangePokeType( bpp, param->next_type );

    return 1;
  }
  return 0;
}
/**
 * SET���b�Z�[�W�\��
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_message( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_MESSAGE* param = (const BTL_HANDEX_PARAM_MESSAGE*)(param_header);
  handexSub_putString( wk, &param->str );
  return 1;
}
/**
 * �|�P�����^�[���t���O�Z�b�g
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_setTurnFlag( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_TURNFLAG* param = (const BTL_HANDEX_PARAM_TURNFLAG*)(param_header);
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );

  if( !BPP_IsDead(bpp) ){
    BPP_TURNFLAG_Set( bpp, param->flag );
    return 1;
  }
  return 0;
}
/**
 * �|�P�����^�[���t���O���Z�b�g
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_resetTurnFlag( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_TURNFLAG* param = (const BTL_HANDEX_PARAM_TURNFLAG*)(param_header);
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );

  if( !BPP_IsDead(bpp) ){
    BPP_TURNFLAG_ForceOff( bpp, param->flag );
    return 1;
  }
  return 0;
}
/**
 * �|�P�����p���t���O�Z�b�g
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_setContFlag( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_SET_CONTFLAG* param = (const BTL_HANDEX_PARAM_SET_CONTFLAG*)(param_header);
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );

  if( !BPP_IsDead(bpp) ){
    scPut_SetContFlag( wk, bpp, param->flag );
    return 1;
  }
  return 0;
}
/**
 * �|�P�����p���t���O���Z�b�g
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_resetContFlag( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_SET_CONTFLAG* param = (const BTL_HANDEX_PARAM_SET_CONTFLAG*)(param_header);
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );

  if( !BPP_IsDead(bpp) ){
    scPut_ResetContFlag( wk, bpp, param->flag );
    return 1;
  }
  return 0;
}
/**
 * �T�C�h�G�t�F�N�g�ǉ�
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_sideEffectAdd( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_SIDEEFF_ADD* param = (const BTL_HANDEX_PARAM_SIDEEFF_ADD*)(param_header);
  BPP_SICK_CONT cont = param->cont;

  scEvent_CheckSideEffectParam( wk, param->effect, param->side, &cont );

  if( BTL_HANDLER_SIDE_Add(param->side, param->effect, cont) )
  {
    handexSub_putString( wk, &param->exStr );
    return 1;
  }
  return 0;
}
/**
 * �T�C�h�G�t�F�N�g�n���h������
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_sideEffectRemove( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_SIDEEFF_REMOVE* param = (const BTL_HANDEX_PARAM_SIDEEFF_REMOVE*)(param_header);
  u32 i;
  u8 result = 0;
  for(i=BTL_SIDEEFF_START; i<BTL_SIDEEFF_MAX; ++i)
  {
    if( BTL_CALC_BITFLG_Check(param->flags, i) ){
      if( BTL_HANDLER_SIDE_Remove(param->side, i) ){
        if( param->fExMsg )
        {
          WazaID waza = BTL_CALC_SideEffectIDtoWazaID( i );
          SCQUE_PUT_MSG_SET( wk->que, param->exStrID, param_header->userPokeID, waza );
        }
        result = 1;
      }
    }
  }
  return result;
}
/**
 * �t�B�[���h�G�t�F�N�g�n���h���ǉ�
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_addFieldEffect( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_ADD_FLDEFF* param = (const BTL_HANDEX_PARAM_ADD_FLDEFF*)(param_header);

  if( scproc_FieldEffectCore(wk, param->effect, param->cont) ){
    handexSub_putString( wk, &param->exStr );
    return 1;
  }
  return 0;
}
/**
 * �t�B�[���h�G�t�F�N�g�n���h���폜
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_removeFieldEffect( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_REMOVE_FLDEFF* param = (const BTL_HANDEX_PARAM_REMOVE_FLDEFF*)(param_header);

  if( BTL_FIELD_RemoveEffect(param->effect) ){
    scproc_FieldEff_End( wk, param->effect );
    return 1;
  }
  return 0;
}

/**
 * �V��ω�
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_changeWeather( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_CHANGE_WEATHER* param = (const BTL_HANDEX_PARAM_CHANGE_WEATHER*)(param_header);
  const BTL_POKEPARAM* pp_user = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );
  BOOL result;

  if( param_header->tokwin_flag ){
    scPut_TokWin_In( wk, pp_user );
  }

  if( param->weather != BTL_WEATHER_NONE )
  {
    result = scproc_ChangeWeatherCore( wk, param->weather, param->turn );
  }
  else
  {
    BtlWeather prevWeather = BTL_FIELD_GetWeather();
    if( prevWeather != BTL_WEATHER_NONE )
    {
      BTL_FIELD_ClearWeather();
      SCQUE_PUT_ACT_WeatherEnd( wk->que, prevWeather );
      result = TRUE;
    }
    else{
      result = FALSE;
    }
  }
  if( result ){
    handexSub_putString( wk, &param->exStr );
  }

  if( param_header->tokwin_flag ){
    scPut_TokWin_Out( wk, pp_user );
  }

  return result;
}


//----------------------------------------------------------------------------------
// �����o�͋��ʏ���
//----------------------------------------------------------------------------------
static BOOL handexSub_putString( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_STR_PARAMS* strParam )
{
  switch( strParam->type ){
  case BTL_STRTYPE_STD:
    scPut_Message_StdEx( wk, strParam->ID, strParam->argCnt, strParam->args );
    return TRUE;
  case BTL_STRTYPE_SET:
    scPut_Message_SetEx( wk, strParam->ID, strParam->argCnt, strParam->args );
    return TRUE;
  }
  return FALSE;
}

/**
 * �ʒu�G�t�F�N�g�n���h���ǉ�
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_PosEffAdd( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_POSEFF_ADD* param = (const BTL_HANDEX_PARAM_POSEFF_ADD*)(param_header);

  if( BTL_HANDLER_POS_Add(param->effect, param->pos, param_header->userPokeID, param->param, param->param_cnt) != NULL ){
    BTL_N_Printf( DBGSTR_SVFL_PosEffAdd, param->pos, param->effect, param_header->userPokeID);
    return 1;
  }
  BTL_N_Printf( DBGSTR_SVFL_PosEffDupFail, param->pos, param->effect, param_header->userPokeID );
  return 0;
}
/**
 * �|�P�����Ƃ�������������
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_tokuseiChange( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_CHANGE_TOKUSEI* param = (const BTL_HANDEX_PARAM_CHANGE_TOKUSEI*)(param_header);

  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  u16 prevTokusei = BPP_GetValue( bpp, BPP_TOKUSEI );

  if( ( param->tokuseiID != prevTokusei )
  &&  !BTL_CALC_TOK_CheckCantChange(param->tokuseiID)
  ){
    SCQUE_PUT_ACT_ChangeTokusei( wk->que, param->pokeID, param->tokuseiID );

    // �Ƃ����������������O�C�x���g
    {
      u32 hem_state = Hem_PushState( &wk->HEManager );
      scEvent_ChangeTokuseiBefore( wk, param->pokeID, param->tokuseiID );
      scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
      Hem_PopState( &wk->HEManager, hem_state );
    }

    BTL_HANDLER_TOKUSEI_Remove( bpp );
    BPP_ChangeTokusei( bpp, param->tokuseiID );
    SCQUE_PUT_OP_ChangeTokusei( wk->que, param->pokeID, param->tokuseiID );
    BTL_HANDLER_TOKUSEI_Add( bpp );

    handexSub_putString( wk, &param->exStr );

    SCQUE_PUT_TOKWIN_OUT( wk->que, param->pokeID );

    // �Ƃ������������������C�x���g
    {
      u32 hem_state = Hem_PushState( &wk->HEManager );
      scEvent_ChangeTokuseiAfter( wk, param->pokeID );
      scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
      Hem_PopState( &wk->HEManager, hem_state );
    }

    return 1;
  }
  return 0;
}
/**
 * �|�P���������A�C�e����������
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_setItem( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_SET_ITEM* param = (const BTL_HANDEX_PARAM_SET_ITEM*)(param_header);

  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );

  // �������g�ւ̃��N�G�X�g�łȂ���΃A�C�e���Z�b�g�̎��s�`�F�b�N
  if( param_header->userPokeID != param->pokeID )
  {
    u32 hem_state = Hem_PushState( &wk->HEManager );
    u8  failed = scEvent_CheckItemSet( wk, bpp, param->itemID );
    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    Hem_PopState( &wk->HEManager, hem_state );

    if( failed ){
      return 0;
    }
  }

  // �����܂ŗ����琬��
  handexSub_itemSet( wk, bpp, param->itemID );

  if( param_header->tokwin_flag ){
    SCQUE_PUT_TOKWIN_IN( wk->que, param_header->userPokeID );
  }

  handexSub_putString( wk, &param->exStr );

  if( param_header->tokwin_flag ){
    SCQUE_PUT_TOKWIN_OUT( wk->que, param_header->userPokeID );
  }


  scproc_CheckItemReaction( wk, bpp );
  return 1;
}
/**
 * �|�P���������A�C�e������
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_swapItem( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_SWAP_ITEM* param = (const BTL_HANDEX_PARAM_SWAP_ITEM*)(param_header);

  BTL_POKEPARAM* target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  BTL_POKEPARAM* self = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );
  u16 selfItem = BPP_GetItem( self );
  u16 targetItem = BPP_GetItem( target );

  // �Ώۂ̔\�͂Ŏ��s����P�[�X���`�F�b�N
  {
    u32 hem_state = Hem_PushState( &wk->HEManager );
    u8  failed = scEvent_CheckItemSet( wk, target, selfItem );
    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    Hem_PopState( &wk->HEManager, hem_state );
    if( failed ){
      return 0;
    }
  }

  // �����܂ŗ����琬��
  if( param_header->tokwin_flag ){
    scPut_TokWin_In( wk, self );
  }
  handexSub_putString( wk, &param->exStr );
  if( param_header->tokwin_flag ){
    scPut_TokWin_Out( wk, self );
  }

  {
    handexSub_itemSet( wk, self, targetItem );
    handexSub_itemSet( wk, target, selfItem );
  }

  scproc_CheckItemReaction( wk, self );
  scproc_CheckItemReaction( wk, target );

  return 1;
}
/**
 * �|�P���������A�C�e�������`�F�b�N
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_EquipItem( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_EQUIP_ITEM* param = (const BTL_HANDEX_PARAM_EQUIP_ITEM*)(param_header);
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );

  scproc_CheckItemReaction( wk, bpp );
  return 1;
}
/**
 * �����A�C�e���g�p
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_useItem( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_POKEPARAM* pp_user = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );
  if( !BPP_IsDead(pp_user) )
  {
    if( scproc_UseItemEquip(wk, pp_user) ){
      return 1;
    }
  }
  return 0;
}
/**
 * �|�P�����ɃA�C�e�����ʂ���������
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_ItemSP( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_ITEM_SP* param = (BTL_HANDEX_PARAM_ITEM_SP*)param_header;
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  if( !BPP_IsDead(bpp) )
  {
    BTL_EVENT_FACTOR* factor = BTL_HANDLER_ITEM_TMP_Add( bpp, param->itemID );
    if( factor )
    {
      u32 hem_state = Hem_PushState( &wk->HEManager );
      scEvent_ItemEquipTmp( wk, bpp );
      scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
      Hem_PopState( &wk->HEManager, hem_state );
      BTL_HANDLER_ITEM_TMP_Remove( factor );
      return 1;
    }
  }
  return 0;
}
/**
 * �|�P���������̃A�C�e��������b�Z�[�W�\��
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_consumeItem( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_CONSUME_ITEM* param = (const BTL_HANDEX_PARAM_CONSUME_ITEM*)param_header;
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );

  scPut_UseItemAct( wk, bpp );
  scPut_ConsumeItem( wk, bpp );
  scPut_SetTurnFlag( wk, bpp, BPP_TURNFLG_ITEM_CONSUMED );
  handexSub_putString( wk, &param->exStr );

  return 1;
}
/**
 * �A�C�e��������̃N���A
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_clearConsumedItem( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_CLEAR_CONSUMED_ITEM* param = (const BTL_HANDEX_PARAM_CLEAR_CONSUMED_ITEM*)param_header;
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );

  BPP_ClearConsumedItem( bpp );
  SCQUE_PUT_OP_ClearConsumedItem( wk->que, param->pokeID );

  return 1;
}

//----------------------------------------------------------------------------------
// �A�C�e�������������ʏ���
//----------------------------------------------------------------------------------
static void handexSub_itemSet( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID )
{
  u8 pokeID = BPP_GetID( bpp );
  u16 prevItemID = BPP_GetItem( bpp );
  u32 hem_state;

  // �A�C�e�����������m��n���h���Ăяo��
  hem_state = Hem_PushState( &wk->HEManager );
  scEvent_ItemSetDecide( wk, bpp, itemID );
  scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
  Hem_PopState( &wk->HEManager, hem_state );

  BTL_HANDLER_ITEM_Remove( bpp );
  SCQUE_PUT_OP_SetItem( wk->que, pokeID, itemID );
  BPP_SetItem( bpp, itemID );

  // �A�C�e���������������n���h���Ăяo��
  if( itemID != ITEM_DUMMY_DATA )
  {
    BTL_HANDLER_ITEM_Add( bpp );
    hem_state = Hem_PushState( &wk->HEManager );
    scEvent_ItemSetFixed( wk, bpp );
    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    Hem_PopState( &wk->HEManager, hem_state );

  }
}

/**
 * �|�P�����킴��������
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_updateWaza( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_UPDATE_WAZA* param = (const BTL_HANDEX_PARAM_UPDATE_WAZA*)(param_header);

  BTL_POKEPARAM* target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  SCQUE_PUT_OP_UpdateWaza( wk->que, param->pokeID, param->wazaIdx, param->waza, param->ppMax, param->fPermanent );
  BPP_WAZA_UpdateID( target, param->wazaIdx, param->waza, param->ppMax, param->fPermanent );
  return 1;
}
/**
 * �|�P�����J�E���^�l��������
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_counter( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_COUNTER* param = (const BTL_HANDEX_PARAM_COUNTER*)(param_header);

  BTL_POKEPARAM* target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  SCQUE_PUT_OP_SetPokeCounter( wk->que, param->pokeID, param->counterID, param->value );
  BPP_COUNTER_Set( target, param->counterID, param->value );
  return 1;
}
/**
 * ���ԍ����U�_���[�W
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_delayWazaDamage( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_DELAY_WAZADMG* param = (const BTL_HANDEX_PARAM_DELAY_WAZADMG*)param_header;

  BTL_POKEPARAM* attacker = BTL_POKECON_GetPokeParam( wk->pokeCon, param->attackerPokeID );
  BTL_POKEPARAM* target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->targetPokeID );

  SVFL_WAZAPARAM   wazaParam;
  WAZAEFF_CTRL  ctrlBackup;
  u16  que_reserve_pos;
  u8 result;

  scEvent_GetWazaParam( wk, param->wazaID, attacker, &wazaParam );

  // ���U���b�Z�[�W�C�G�t�F�N�g�C���U�o���m��
  que_reserve_pos = SCQUE_RESERVE_Pos( wk->que, SC_ACT_WAZA_EFFECT );

  // ���U�Ώۂ����[�N�Ɏ擾
  BTL_POKESET_Clear( &wk->pokesetDamaged );
  BTL_POKESET_Clear( &wk->pokesetTarget );
  BTL_POKESET_Add( &wk->pokesetTarget, target );

  // ��ɂ��Ȃ��|�P�����ɂ͓�����Ȃ��i�␳�Ȃ��j
  BTL_POKESET_RemoveDeadPoke( &wk->pokesetTarget );
  if( BTL_POKESET_IsRemovedAll(&wk->pokesetTarget) ){
    scPut_WazaFail( wk, attacker, wazaParam.wazaID );
    return 0;
  }

  // �Ώۂ��Ƃ̖����`�F�b�N������`�F�b�N
  flowsub_checkNotEffect( wk, &wazaParam, attacker, &wk->pokesetTarget );
  flowsub_checkWazaAvoid( wk, &wazaParam, attacker, &wk->pokesetTarget );
  // �ŏ��͋����^�[�Q�b�g���c���Ă��Ȃ� -> ���������I��
  if( BTL_POKESET_IsRemovedAll(&wk->pokesetTarget) ){
    return 0;
  }

  // ���U�G�t�F�N�g�Ǘ��̃o�b�N�A�b�v�����A�V�X�e��������
  ctrlBackup = wk->wazaEffCtrl;
  wazaEffCtrl_Init( &wk->wazaEffCtrl, wk, attacker, &wk->pokesetTarget );

  scproc_Fight_Damage_Root( wk, &wazaParam, attacker, &wk->pokesetTarget );

  // ���U���ʂ���m�聨���o�\���R�}���h�����Ȃǂ�
  result = wazaEffCtrl_IsEnable( &wk->wazaEffCtrl );
  if( result ){
    scPut_WazaEffect( wk, wazaParam.wazaID, &wk->wazaEffCtrl, que_reserve_pos );
  }

  wk->wazaEffCtrl = ctrlBackup;
  return result;
}
/**
 * �o�g�����E
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_quitBattle( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );
  if( scproc_NigeruCore(wk, bpp ) ){
    wk->flowResult = SVFLOW_RESULT_BTL_QUIT;
    return 1;
  }
  return 0;
}
/**
 * �����o�[����ւ�
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_changeMember( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_CHANGE_MEMBER* param = (BTL_HANDEX_PARAM_CHANGE_MEMBER*)param_header;
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  if( scproc_MemberOutForChange(wk, bpp) )
  {
    BtlPokePos pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, param->pokeID );
    BTL_SERVER_RequestChangePokemon( wk->server, pos );
    handexSub_putString( wk, &param->exStr );
    wk->flowResult = SVFLOW_RESULT_POKE_CHANGE;
    return 1;
  }
  return 0;
}
/**
 * �o�g���^�b�`
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_batonTouch( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_BATONTOUCH* param = (BTL_HANDEX_PARAM_BATONTOUCH*)param_header;

  BTL_POKEPARAM* user = BTL_POKECON_GetPokeParam( wk->pokeCon, param->userPokeID );
  BTL_POKEPARAM* target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->targetPokeID );

  BPP_BatonTouchParam( target, user );
  SCQUE_PUT_OP_BatonTouch( wk->que, param->userPokeID, param->targetPokeID );

  return 1;
}
/**
 * �Ђ�܂���
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_addShrink( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_ADD_SHRINK* param = (BTL_HANDEX_PARAM_ADD_SHRINK*)param_header;

  BTL_POKEPARAM* target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  if( scproc_AddShrinkCore(wk, target, WAZANO_NULL, param->per) ){
    return 1;
  }
  return 0;
}
/**
 * �m�������
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_relive( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_RELIVE* param = (BTL_HANDEX_PARAM_RELIVE*)param_header;
  BTL_POKEPARAM* target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  BPP_HpPlus( target, param->recoverHP );
  SCQUE_PUT_OP_HpPlus( wk->que, param->pokeID, param->recoverHP );
  wk->pokeDeadFlag[param->pokeID] = FALSE;

  {
    BtlPokePos targetPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, param->pokeID );
    if( targetPos != BTL_POS_NULL )
    {
      BTL_POSPOKE_PokeIn( &wk->pospokeWork, targetPos, param->pokeID, wk->pokeCon );
      SCQUE_PUT_ACT_RelivePoke( wk->que, param->pokeID );
    }
  }

  handexSub_putString( wk, &param->exStr );
  return 1;
}
/**
 * �̏d���Z�b�g
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_setWeight( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_SET_WEIGHT* param = (BTL_HANDEX_PARAM_SET_WEIGHT*)param_header;
  BTL_POKEPARAM* target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );

  BPP_SetWeight( target, param->weight );
  SCQUE_PUT_OP_SetWeight( wk->que, param->pokeID, param->weight );
  handexSub_putString( wk, &param->exStr );
  return 1;
}
/**
 * �ꂩ�琁����΂�
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_pushOut( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_PUSHOUT* param = (BTL_HANDEX_PARAM_PUSHOUT*)param_header;
  BTL_POKEPARAM* target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  BTL_POKEPARAM* attacker = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );

  if( scproc_PushOutCore(wk, attacker, target, param->fForceChange, &param->exStr) ){
    return 1;
  }
  return 0;
}
/**
 * �w��|�P�����̍s�������荞�݂�����
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_intrPoke( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_INTR_POKE* param = (BTL_HANDEX_PARAM_INTR_POKE*)param_header;

  if( ActOrder_IntrReserve(wk, param->pokeID) ){
    handexSub_putString( wk, &param->exStr );
    return 1;
  }
  return 0;
}
/**
 * �w�胏�U�g�p�|�P�����̍s�������荞�݂�����
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_intrWaza( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_INTR_WAZA* param = (BTL_HANDEX_PARAM_INTR_WAZA*)param_header;

  if( ActOrder_IntrReserveByWaza(wk, param->waza) ){
    return 1;
  }
  return 0;
}
/**
 * �w��|�P�����̍s�������Ō�ɉ�
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_sendLast( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_SEND_LAST* param = (BTL_HANDEX_PARAM_SEND_LAST*)param_header;

  if( ActOrder_SendLast(wk, param->pokeID) ){
    handexSub_putString( wk, &param->exStr );
    return 1;
  }
  return 0;
}
/**
 * �w��|�P�������m�̏ꏊ����ւ�
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_swapPoke( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_SWAP_POKE* param = (BTL_HANDEX_PARAM_SWAP_POKE*)param_header;

  if( param->pokeID1 != param->pokeID2 )
  {
    u8 clientID = BTL_MAINUTIL_PokeIDtoClientID( param->pokeID1 );
    if( clientID == BTL_MAINUTIL_PokeIDtoClientID(param->pokeID2) )
    {
      const BTL_POKEPARAM* bpp1 = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID1 );
      const BTL_POKEPARAM* bpp2 = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID2 );
      if( !BPP_IsDead(bpp1) && !BPP_IsDead(bpp2) )
      {
        const BTL_PARTY* party = BTL_POKECON_GetPartyData( wk->pokeCon, clientID );
        s16 posIdx1 = BTL_PARTY_FindMember( party, bpp1 );
        s16 posIdx2 = BTL_PARTY_FindMember( party, bpp2 );
        if( (posIdx1 >= 0) && (posIdx2 >= 0) )
        {
          scproc_MoveCore( wk, clientID, posIdx1, posIdx2 );
          handexSub_putString( wk, &param->exStr );
          return 1;
        }
      }
    }
  }
  return 0;
}
/**
 * �w�b�_�|�P�������w��|�P�����ɕϐg
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_hensin( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_HENSIN* param = (BTL_HANDEX_PARAM_HENSIN*)param_header;

  BTL_POKEPARAM* user = BTL_POKECON_GetPokeParam( wk->pokeCon,  param_header->userPokeID );
  BTL_POKEPARAM* target = BTL_POKECON_GetPokeParam( wk->pokeCon,  param->pokeID );

  if( BPP_HENSIN_Set(user, target) )
  {
    u8 usrPokeID = BPP_GetID( user );
    u8 tgtPokeID = BPP_GetID( target );

    BTL_HANDLER_Waza_RemoveForceAll( user );
    BTL_HANDLER_TOKUSEI_Remove( user );
    BTL_HANDLER_TOKUSEI_Add( user );

    SCQUE_PUT_OP_Hensin( wk->que, usrPokeID, tgtPokeID );

    if( param_header->tokwin_flag ){
      scPut_TokWin_In( wk, user );
    }
    handexSub_putString( wk, &param->exStr );
    if( param_header->tokwin_flag ){
      scPut_TokWin_Out( wk, user );
    }
    return 1;
  }
  return 0;
}
/**
 * �C�����[�W��������
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_fakeBreak( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_HENSIN* param = (BTL_HANDEX_PARAM_HENSIN*)param_header;

  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon,  param->pokeID );

  if( BPP_IsFakeEnable(bpp) )
  {
    BPP_FakeDisable( bpp );
    SCQUE_PUT_ACT_FakeDisable( wk->que, param->pokeID );
    handexSub_putString( wk, &param->exStr );
    return 1;
  }
  return 0;
}
/**
 * ���イ��傭�������`�F�b�N�i����ƂԁE�_�C�r���O���ӂ䂤�t���O�𗎂Ƃ��j
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_juryokuCheck( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  u8 pokeID[ BTL_POS_MAX ];
  u8 cnt, pos, fFall, i;
  BtlExPos exPos;
  BTL_POKEPARAM* bpp;

  pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, param_header->userPokeID );
  exPos = EXPOS_MAKE( BTL_EXPOS_FULL_ALL, pos );
  cnt = BTL_SVFTOOL_ExpandPokeID( wk, exPos, pokeID );
  for(i=0; i<cnt; ++i)
  {
    bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, pokeID[i] );
    fFall = FALSE;
    if( BPP_CONTFLAG_Get(bpp, BPP_CONTFLG_SORAWOTOBU) ){
      scPut_ResetContFlag( wk, bpp, BPP_CONTFLG_SORAWOTOBU );
      scPut_CureSick( wk, bpp, WAZASICK_TAMELOCK, NULL );
      fFall = TRUE;
    }
    if( BPP_CheckSick(bpp, WAZASICK_FLYING) ){
      BPP_CureWazaSick( bpp, WAZASICK_FLYING );
      fFall = TRUE;
    }
    if( BPP_IsMatchType(bpp, POKETYPE_HIKOU) ){
      fFall = TRUE;
    }
    if( BPP_GetValue(bpp, BPP_TOKUSEI_EFFECTIVE) == POKETOKUSEI_FUYUU ){
      fFall = TRUE;
    }

    if( fFall ){
      SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_JyuryokuFall, pokeID[i] );
    }
  }

  return 1;
}
/**
 * �ʒu�w��G�t�F�N�g����
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_effectByPos( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_EFFECT_BY_POS* param = (const BTL_HANDEX_PARAM_EFFECT_BY_POS*)(param_header);

  if( param->fQueReserve )
  {
    if( param->pos_to != BTL_POS_NULL )
    {
      SCQUE_PUT_ReservedPos( wk->que, param->reservedQuePos, SC_ACT_EFFECT_BYVECTOR,
          param->pos_from, param->pos_to, param->effectNo );
    }
    else
    {
      SCQUE_PUT_ReservedPos( wk->que, param->reservedQuePos, SC_ACT_EFFECT_BYPOS,
          param->pos_from, param->effectNo );
    }
  }
  else
  {
    if( param->pos_to != BTL_POS_NULL )
    {
      SCQUE_PUT_ACT_EffectByVector( wk->que, param->pos_from, param->pos_to, param->effectNo );
    }
    else
    {
      SCQUE_PUT_ACT_EffectByPos( wk->que, param->pos_from, param->effectNo );
    }
  }
  return 1;
}


/*

typedef struct {

  u8    type;       ///< �ق̂��A�݂��A�����Ȃǂ̃^�C�v
  u8    category;   ///< ���U�J�e�S���i���P�j
  s8    priority;   ///< �D��x
  u8    hitPer;     ///< ������
  u8    damageType; ///< �_���[�W�^�C�v�i"�Ȃ�"=0, "����"=1, "����"=2�j
  u8    power;      ///< �З͒l
  u8    critical;   ///< �N���e�B�J�������N
  u8    hitMax;     ///< �ő�q�b�g��
  u8    hitMin;     ///< �ŏ��q�b�g��
  u8    shrinkPer;  ///< �Ђ�݊m��

  u8    sickID;     ///< ��Ԉُ����
  u8    sickCont;   ///< ��Ԉُ�̌p���p�^�[��
  u8    sickTurnMin;///< ��Ԉُ�̍ŏ��^�[����
  u8    sickTurnMax;///< ��Ԉُ�̍ő�^�[����
  u8    sickPer;    ///< �ǉ����ʂɂ���Ԉُ�̔�����

  struct {
    u8 statusID;
    s8 value;
    u8 per;
  }rankEffect[ 3 ];

  s8    recoverRatio_damage;
  s8    recoverRatio_HP;

  u8    coverArea;

  u16   touchFlag : 1;      ///< �ڐG���U
  u16   tameFlag  : 1;      ///< �P�^�[������
  u16   tireFlag  : 1;      ///< �P�^�[�������œ����Ȃ�
  u16   guardFlag : 1;      ///< �u�܂���v�u�݂���v�̑Ώ�
  u16   magicCoatFlag : 1;  ///< �u�}�W�b�N�R�[�g�v�̑Ώ�
  u16   yokodoriFlag : 1;   ///< �u�悱�ǂ�v�̑Ώ�
  u16   oumuFlag  : 1;      ///< �u�I�E���������v�̑Ώ�
  u16   hitFarFlag : 1;     ///< �g���v���ŗ��ꂽ�|�P�����ɂ�������



}WAZADATA;

/*
���P
"�V���v���_���[�W"=0
"�V���v���V�b�N"
"

*/