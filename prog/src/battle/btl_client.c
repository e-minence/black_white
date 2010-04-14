//=============================================================================================
/**
 * @file  btl_client.c
 * @brief �|�P����WB �o�g���V�X�e�� �N���C�A���g���W���[��
 * @author  taya
 *
 * @date  2008.09.06  �쐬
 */
//=============================================================================================
#include <gflib.h>

#include "sound\pm_sndsys.h"
#include "poke_tool\pokeparty.h"

#include "battle\battle.h"
#include "btl_common.h"
#include "btl_main.h"
#include "btl_adapter.h"
#include "btl_action.h"
#include "btl_field.h"
#include "btl_pokeselect.h"
#include "btl_rec.h"
#include "btl_server_cmd.h"
#include "btl_net.h"

#include "app\b_bag.h"
#include "app\b_plist.h"
#include "btlv\btlv_core.h"
#include "btlv\btlv_effect.h"
#include "btlv\btlv_timer.h"
#include "btlv\btlv_input.h"

#include "tr_ai/tr_ai.h"
#include "tr_tool/tr_tool.h"

#include "btl_client.h"


enum {
  PRINT_FLG = FALSE,
};

/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/

enum {
  CANTESC_COUNT_MAX = 8,    ///< �Ƃ肠�������Ȃ߃M���M���ȂƂ���ASSERT�����Ă݂�
  AI_ITEM_MAX = 4,          ///< AI�g���[�i�[���g�p����A�C�e����

  EMPTY_POS_NONE = -1,

  CLIENT_FLDEFF_BITFLAG_SIZE = BTL_CALC_BITFLAG_BUFSIZE( BTL_FLDEFF_MAX ) + 1,
};


/**
 *  AI�g���[�i�[�̃��b�Z�[�W���
 */
enum {
  AITRAINER_MSG_HP_HALF,        ///< HP�����ȉ�
  AITRAINER_MSG_FIRST_DAMAGE,   ///< �ŏ��̃_���[�W��
  AITRAINER_MSG_LAST,           ///< �Ō�̂P�̓o�꒼��
  AITRAINER_MSG_LAST_HP_HALF,   ///< �Ō�̂P�̂g�o�����ȉ�
  AITRAINER_MSG_MAX,
};



/*--------------------------------------------------------------------------*/
/* Typedefs                                                                 */
/*--------------------------------------------------------------------------*/
typedef BOOL (*ClientMainProc)( BTL_CLIENT* );
typedef BOOL (*ClientSubProc)( BTL_CLIENT*, int* );
typedef BOOL (*ServerCmdProc)( BTL_CLIENT*, int*, const int* );

/*--------------------------------------------------------------------------*/
/* Structures                                                               */
/*--------------------------------------------------------------------------*/


/**
 *  �^��f�[�^�R���g���[���R�[�h
 */
typedef enum {
  RECCTRL_NONE = 0,   ///< ���얳��
  RECCTRL_QUIT,       ///< �Đ��I��
  RECCTRL_CHAPTER,    ///< �`���v�^�[�ړ�
}RecCtrlCode;



/**
 *  �^��f�[�^�Đ��R���g���[���\����
 */
typedef struct {
  u8   seq;
  u8   ctrlCode;
  u8   fChapterSkip    : 1;
  u8   fFadeOutDone    : 1;
  u8   fTurnIncrement  : 1;
  u16  handlingTimer;
  u16  turnCount;
  u16  nextTurnCount;
  u16  maxTurnCount;
  u16  skipTurnCount;
}RECPLAYER_CONTROL;

//--------------------------------------------------------------
/**
 *  �N���C�A���g���W���[���\����`
 */
//--------------------------------------------------------------
struct _BTL_CLIENT {

  BTL_MAIN_MODULE*      mainModule;
  BTL_POKE_CONTAINER*   pokeCon;
  const BTL_POKEPARAM*  procPoke;
  BTL_ACTION_PARAM*     procAction;
  BTL_REC*              btlRec;
  BTL_RECREADER*        btlRecReader;
  RECPLAYER_CONTROL     recPlayer;
  ClientMainProc        mainProc;

  BTL_ADAPTER*    adapter;
  BTLV_CORE*      viewCore;
  BTLV_STRPARAM   strParam;
  BTLV_STRPARAM   strParamSub;
  BTL_SERVER*     cmdCheckServer;
  BTLV_ROTATION_WAZASEL_PARAM  rotWazaSelParam;
  BtlRotateDir    prevRotateDir;

  ClientSubProc  subProc;
  int            subSeq;

  ClientSubProc   selActProc;
  int             selActSeq;

  const void*    returnDataPtr;
  u32            returnDataSize;
  u32            dummyReturnData;
  u16            cmdLimitTime;
  u16            gameLimitTime;

  u16                   AIItem[ AI_ITEM_MAX ];
  VMHANDLE*             AIHandle;
  GFL_STD_RandContext   AIRandContext;

  s8             AIChangeIndex[ BTL_PARTY_MEMBER_MAX ];
  u8             AITrainerMsgCheckedFlag[ AITRAINER_MSG_MAX ];



  const BTL_PARTY*  myParty;
  u8                numCoverPos;    ///< �S������퓬�|�P������
  u8                procPokeIdx;    ///< �������|�P�����C���f�b�N�X
  s8                prevPokeIdx;    ///< �O��̏����|�P�����C���f�b�N�X
  u8                firstPokeIdx;   ///< �A�N�V�����w���ł���ŏ��̃|�P�����C���f�b�N�X
  u8                fStdMsgChanged; ///< �W�����b�Z�[�W���e�������������t���O
  BtlPokePos        basePos;        ///< �퓬�|�P�����̈ʒuID

  BTL_ACTION_PARAM     actionParam[ BTL_POSIDX_MAX ];
  u8                   shooterCost[ BTL_POSIDX_MAX ];
  BTL_SERVER_CMD_QUE*  cmdQue;
  int                  cmdArgs[ BTL_SERVERCMD_ARG_MAX ];
  ServerCmd            serverCmd;
  ServerCmdProc        scProc;
  int                  scSeq;

  BTL_POKESELECT_PARAM    pokeSelParam;
  BTL_POKESELECT_RESULT   pokeSelResult;


  HEAPID heapID;
  u8   myID;
  u8   myType;
  u8   myState;
  u8   commWaitInfoOn;
  u8   bagMode;
  u8   shooterEnergy;
  u8   escapeClientID;
  u8   change_escape_code;
  u8   fForceQuitSelAct;
  u8   cmdCheckTimingCode;
  u8   fAITrainerBGMChanged;
  u8   actionAddCount;
  u16  EnemyPokeHPBase;
  u16  AITrainerMsgID;

  u8          myChangePokeCnt;
  u8          myPuttablePokeCnt;
  BtlPokePos  myChangePokePos[ BTL_POSIDX_MAX ];

  u8          fieldEffectFlag[ CLIENT_FLDEFF_BITFLAG_SIZE ];

};


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void ChangeMainProc( BTL_CLIENT* wk, ClientMainProc proc );
static BOOL ClientMain_Normal( BTL_CLIENT* wk );
static BOOL ClientMain_ChapterSkip( BTL_CLIENT* wk );
static void setDummyReturnData( BTL_CLIENT* wk );
static ClientSubProc getSubProc( BTL_CLIENT* wk, BtlAdapterCmd cmd );
static BOOL SubProc_UI_Setup( BTL_CLIENT* wk, int* seq );
static BOOL SubProc_REC_Setup( BTL_CLIENT* wk, int* seq );
static void EnemyPokeHPBase_Update( BTL_CLIENT* wk );
static u32 EnemyPokeHPBase_CheckRatio( BTL_CLIENT* wk );
static const BTL_POKEPARAM* EnemyPokeHPBase_GetTargetPoke( BTL_CLIENT* wk );
static BOOL SubProc_UI_SelectRotation( BTL_CLIENT* wk, int* seq );
static BtlRotateDir decideNextDirRandom( BtlRotateDir prevDir );
static BOOL SubProc_REC_SelectRotation( BTL_CLIENT* wk, int* seq );
static BOOL SubProc_AI_SelectRotation( BTL_CLIENT* wk, int* seq );
static void CmdLimit_Start( BTL_CLIENT* wk );
static BOOL CmdLimit_CheckOver( BTL_CLIENT* wk );
static BOOL CheckSelactForceQuit( BTL_CLIENT* wk, ClientSubProc nextProc );
static void CmdLimit_End( BTL_CLIENT* wk );
static void ClientSubProc_Set( BTL_CLIENT* wk, ClientSubProc proc );
static BOOL ClientSubProc_Call( BTL_CLIENT* wk );
static BOOL SubProc_UI_SelectAction( BTL_CLIENT* wk, int* seq );
static BOOL SubProc_REC_SelectAction( BTL_CLIENT* wk, int* seq );
static BOOL selact_Start( BTL_CLIENT* wk, int* seq );
static void selact_startMsg( BTL_CLIENT* wk, const BTLV_STRPARAM* strParam );
static BOOL selact_ForceQuit( BTL_CLIENT* wk, int* seq );
static  BOOL  check_tr_message( BTL_CLIENT* wk, u16* msgID );
static BOOL selact_Root( BTL_CLIENT* wk, int* seq );
static BOOL selact_Fight( BTL_CLIENT* wk, int* seq );
static void setupRotationParams( BTL_CLIENT* wk, BTLV_ROTATION_WAZASEL_PARAM* param );
static BOOL selact_SelectChangePokemon( BTL_CLIENT* wk, int* seq );
static BOOL selact_Item( BTL_CLIENT* wk, int* seq );
static BOOL selact_Escape( BTL_CLIENT* wk, int* seq );
static BOOL selact_CheckFinish( BTL_CLIENT* wk, int* seq );
static BOOL selact_Finish( BTL_CLIENT* wk, int* seq );
static void shooterCost_Init( BTL_CLIENT* wk );
static void shooterCost_Save( BTL_CLIENT* wk, u8 procPokeIdx, u8 cost );
static u8 shooterCost_Get( BTL_CLIENT* wk, u8 procPokeIdx );
static u8 shooterCost_GetSum( BTL_CLIENT* wk );
static BOOL is_action_unselectable( BTL_CLIENT* wk, const BTL_POKEPARAM* bpp, BTL_ACTION_PARAM* action );
static BOOL is_waza_unselectable( BTL_CLIENT* wk, const BTL_POKEPARAM* bpp, BTL_ACTION_PARAM* action );
static void setWaruagakiAction( BTL_ACTION_PARAM* dst, BTL_CLIENT* wk, const BTL_POKEPARAM* bpp );
static BOOL is_unselectable_waza( BTL_CLIENT* wk, const BTL_POKEPARAM* bpp, WazaID waza, BTLV_STRPARAM* strParam );
static u8 StoreSelectableWazaFlag( BTL_CLIENT* wk, const BTL_POKEPARAM* bpp, u8* dst );
static BtlCantEscapeCode isForbidEscape( BTL_CLIENT* wk, const BTL_POKEPARAM* procPoke, u8* pokeID, u16* tokuseiID );
static BOOL checkForbitEscapeEffective_Kagefumi( BTL_CLIENT* wk, const BTL_POKEPARAM* procPoke );
static BOOL checkForbitEscapeEffective_Arijigoku( BTL_CLIENT* wk, const BTL_POKEPARAM* procPoke );
static BOOL checkForbitEscapeEffective_Jiryoku( BTL_CLIENT* wk, const BTL_POKEPARAM* procPoke );
static void ChangeAI_InitWork( BTL_CLIENT* wk );
static BOOL ChangeAI_CheckReserve( BTL_CLIENT* wk, u8 index );
static void ChangeAI_SetReserve( BTL_CLIENT* wk, u8 outPokeIdx, u8 inPokeIdx );
static BOOL ChangeAI_Root( BTL_CLIENT* wk, const BTL_POKEPARAM* procPoke, u8 procPokeIndex, u8* targetIndex );
static BTL_POKEPARAM* ChangeAI_DecideTarget( BTL_CLIENT* wk, BtlPokePos basePos );
static BOOL ChangeAI_CheckHorobi( BTL_CLIENT* wk, const BTL_POKEPARAM* procPoke );
static BOOL ChangeAI_CheckFusigiNaMamori( BTL_CLIENT* wk, const BTL_POKEPARAM* procPoke, const BTL_POKEPARAM* targetPoke );
static BOOL ChangeAI_CheckNoEffectWaza( BTL_CLIENT* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender );
static BOOL ChangeAI_CheckKodawari( BTL_CLIENT* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender );
static BOOL ChangeAI_CheckUkeTokusei( BTL_CLIENT* wk, const BTL_POKEPARAM* procPoke, const BTL_POKEPARAM* targetPoke, u8* changeIndex );
static BOOL AI_ChangeProcSub_SizenKaifuku( BTL_CLIENT* wk, const BTL_POKEPARAM* procPoke, u8* changeIndex );
static BOOL AI_ChangeProcSub_WazaAff( BTL_CLIENT* wk, const BTL_POKEPARAM* procPoke, const BTL_POKEPARAM* targetPoke, u8* changeIndex );
static BOOL AI_ChangeProcSub_CheckTokHikae( BTL_CLIENT* wk, u16 tokusei, u8* index );
static BOOL AI_ChangeProcSub_HikaeWazaAff( BTL_CLIENT* wk, const BTL_POKEPARAM* target, BtlTypeAff affMin );
static BOOL AI_ChangeProcSub_HikaePokeAff( BTL_CLIENT* wk, PokeType wazaType, BtlTypeAff affMax, u8* pokeIndex );
static BOOL AI_ChangeProcSub_CheckWazaAff( BTL_CLIENT* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, BtlTypeAff affMin );
static BOOL SubProc_AI_SelectAction( BTL_CLIENT* wk, int* seq );
static u8 calcPuttablePokemons( BTL_CLIENT* wk, u8* list );
static void sortPuttablePokemonList( BTL_CLIENT* wk, u8* list, u8 numPoke, const BTL_POKEPARAM* target );
static void setupPokeSelParam( BTL_CLIENT* wk, u8 mode, u8 numSelect, BTL_POKESELECT_PARAM* param, BTL_POKESELECT_RESULT* result );
static void storePokeSelResult( BTL_CLIENT* wk, const BTL_POKESELECT_RESULT* res );
static void storePokeSelResult_ForceQuit( BTL_CLIENT* wk );
static u8 storeMyChangePokePos( BTL_CLIENT* wk, BtlPokePos* myCoverPos );
static BOOL SubProc_UI_SelectChangeOrEscape( BTL_CLIENT* wk, int* seq );
static BOOL SubProc_UI_SelectPokemonForCover( BTL_CLIENT* wk, int* seq );
static BOOL SubProc_UI_SelectPokemonForChange( BTL_CLIENT* wk, int* seq );
static BOOL SubProc_AI_SelectPokemon( BTL_CLIENT* wk, int* seq );
static BOOL SubProc_REC_SelectPokemon( BTL_CLIENT* wk, int* seq );
static BOOL SelectPokemonUI_Core( BTL_CLIENT* wk, int* seq, u8 mode );
static u32 SetCoverRotateAction( BTL_CLIENT* wk, BTL_ACTION_PARAM* resultAction );
static BOOL SubProc_UI_ConfirmIrekae( BTL_CLIENT* wk, int* seq );
static BOOL SubProc_UI_RecordData( BTL_CLIENT* wk, int* seq );
static BOOL SubProc_UI_ExitCommTrainer( BTL_CLIENT* wk, int* seq );
static BtlResult checkResult( BTL_CLIENT* wk );
static BOOL SubProc_UI_ExitForNPC( BTL_CLIENT* wk, int* seq );
static BOOL SubProc_UI_LoseWild( BTL_CLIENT* wk, int* seq );
static BOOL SubProc_UI_NotifyTimeUp( BTL_CLIENT* wk, int* seq );
static BOOL SubProc_REC_ServerCmd( BTL_CLIENT* wk, int* seq );
static BOOL SubProc_UI_ServerCmd( BTL_CLIENT* wk, int* seq );
static BOOL scProc_ACT_MemberOutMsg( BTL_CLIENT* wk, int* seq, const int* args );
static u16 CheckMemberOutStrID( BTL_CLIENT* wk, u8 clientID, BOOL* fClientArg );
static BOOL scProc_ACT_MemberOut( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_MemberIn( BTL_CLIENT* wk, int* seq, const int* args );
static u16 CheckMemberPutStrID( BTL_CLIENT* wk );
static BOOL scProc_MSG_Std( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_MSG_StdSE( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_MSG_Set( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_MSG_Waza( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_WazaEffect( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_WazaEffectEx( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_TameWazaHide( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_WazaDmg( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_WazaDmg_Plural( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_WazaIchigeki( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_SickIcon( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_ConfDamage( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_Dead( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_Relive( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_RankDown( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_RankUp( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_WeatherDmg( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_WeatherStart( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_WeatherEnd( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_SimpleHP( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_Kinomi( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_Kill( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_Move( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_ResetMove( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_Exp( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL wazaOboeSeq( BTL_CLIENT* wk, int* seq, BTL_POKEPARAM* bpp );
static BOOL scProc_ACT_BallThrow( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_Rotation( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_ChangeTokusei( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_FakeDisable( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_EffectByPos( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_EffectByVector( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_ChangeForm( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_TOKWIN_In( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_TOKWIN_Out( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_HpMinus( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_HpPlus( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_PPMinus( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_HpZero( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_PPPlus( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_RankUp( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_RankDown( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_RankSet7( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_RankRecover( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_RankReset( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_AddCritical( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_SickSet( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_CurePokeSick( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_CureWazaSick( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_MemberIn( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_ChangePokeType( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_WSTurnCheck( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_ConsumeItem( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_UpdateUseWaza( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_SetContFlag( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_ResetContFlag( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_SetTurnFlag( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_ResetTurnFlag( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_ChangeTokusei( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_SetItem( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_UpdateWazaNumber( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_Hensin( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_OutClear( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_AddFldEff( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_RemoveFldEff( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_SetPokeCounter( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_BatonTouch( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_MigawariCreate( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_MigawariDelete( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_ShooterCharge( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_SetFakeSrc( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_ClearConsumedItem( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_AddWazaDamage( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_TurnCheck( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_SetStatus( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_SetWeight( BTL_CLIENT* wk, int* seq, const int* args );
static u8 countFrontPokeTokusei( BTL_CLIENT* wk, PokeTokusei tokusei );
static u8 countFrontPokeType( BTL_CLIENT* wk, PokeType type );
static void AIItem_Setup( BTL_CLIENT* wk );
static u16 AIItem_CheckUse( BTL_CLIENT* wk, const BTL_POKEPARAM* bpp, const BTL_PARTY* party );
static BOOL check_status_up_item( u16 itemID, const BTL_POKEPARAM* bpp );
static BOOL check_cure_sick_item( u16 itemID, const BTL_POKEPARAM* bpp );
static void RecPlayer_Init( RECPLAYER_CONTROL* ctrl );
static void RecPlayer_Setup( RECPLAYER_CONTROL* ctrl, u32 turnCnt );
static BOOL RecPlayer_CheckBlackOut( const RECPLAYER_CONTROL* ctrl );
static void RecPlayer_TurnIncReq( RECPLAYER_CONTROL* ctrl );
static RecCtrlCode RecPlayer_GetCtrlCode( const RECPLAYER_CONTROL* ctrl );
static void RecPlayer_ChapterSkipOn( RECPLAYER_CONTROL* ctrl, u32 nextTurnNum );
static void RecPlayer_ChapterSkipOff( RECPLAYER_CONTROL* ctrl );
static BOOL RecPlayer_CheckChapterSkipEnd( const RECPLAYER_CONTROL* ctrl );
static u32 RecPlayer_GetNextTurn( const RECPLAYER_CONTROL* ctrl );
static void RecPlayerCtrl_Main( BTL_CLIENT* wk, RECPLAYER_CONTROL* ctrl );



BTL_CLIENT* BTL_CLIENT_Create(
  BTL_MAIN_MODULE* mainModule, BTL_POKE_CONTAINER* pokecon, BtlCommMode commMode,
  GFL_NETHANDLE* netHandle, u16 clientID, u16 numCoverPos, BtlClientType clientType, BtlBagMode bagMode, BOOL fRecPlayMode,
  const GFL_STD_RandContext* randContext, HEAPID heapID )
{
  BTL_CLIENT* wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(BTL_CLIENT) );
  int i;
  BOOL fComm = ((commMode != BTL_COMM_NONE) && (clientType == BTL_CLIENT_TYPE_UI));

  wk->myID = clientID;
  wk->heapID = heapID;
  wk->myType = clientType;
  wk->adapter = BTL_ADAPTER_Create( netHandle, clientID, fComm, heapID );
  wk->myParty = BTL_POKECON_GetPartyDataConst( pokecon, clientID );
  wk->mainModule = mainModule;
  wk->pokeCon = pokecon;
  wk->numCoverPos = numCoverPos;
  wk->procPokeIdx = 0;
  wk->basePos = clientID;
  wk->prevRotateDir = BTL_ROTATEDIR_NONE;
  wk->viewCore = NULL;
  wk->EnemyPokeHPBase = 0;
  wk->cmdQue = GFL_HEAP_AllocClearMemory( heapID, sizeof(BTL_SERVER_CMD_QUE) );
  wk->mainProc = ClientMain_Normal;
  wk->myState = 0;
  wk->cmdCheckServer = NULL;
  wk->cmdCheckTimingCode = BTL_RECTIMING_None;

  wk->commWaitInfoOn = FALSE;
  wk->shooterEnergy = 0;
  wk->cmdLimitTime = 0;
  wk->gameLimitTime = 0;
  wk->fForceQuitSelAct = FALSE;
  wk->fAITrainerBGMChanged = FALSE;
//  wk->shooterEnergy = BTL_SHOOTER_ENERGY_MAX;

  wk->bagMode = bagMode;
  wk->escapeClientID = BTL_CLIENTID_NULL;

  RecPlayer_Init( &wk->recPlayer );

  BTL_CALC_BITFLG_Construction( wk->fieldEffectFlag, sizeof(wk->fieldEffectFlag) );

  AIItem_Setup( wk );
  wk->AIRandContext = *randContext;
  if( (wk->myType == BTL_CLIENT_TYPE_AI) && (!fRecPlayMode) )
  {
    u32 ai_bit = BTL_MAIN_GetClientAIBit( wk->mainModule, wk->myID );
    BTL_SVFLOW_WORK* svf_work = BTL_MAIN_GetSVFWorkForAI( wk->mainModule );
    TAYA_Printf( "trainer AI_bit = 0x%04x\n", ai_bit );
    wk->AIHandle = TR_AI_Init( wk->mainModule, svf_work, wk->pokeCon, ai_bit, wk->heapID );
  }
  else
  {
    wk->AIHandle = NULL;
  }

  for(i=0; i<NELEMS(wk->AITrainerMsgCheckedFlag); ++i){
    wk->AITrainerMsgCheckedFlag[ i ] = FALSE;
  }

  if( (wk->myType == BTL_CLIENT_TYPE_UI)
  &&  (BTL_MAIN_IsRecordEnable(wk->mainModule))
  ){
    wk->btlRec = BTL_REC_Create( heapID );
  }else{
    wk->btlRec = NULL;
  }

  return wk;
}

void BTL_CLIENT_Delete( BTL_CLIENT* wk )
{
  if( wk->btlRec ){
    BTL_REC_Delete( wk->btlRec );
  }
  if( wk->AIHandle ){
    TR_AI_Exit( wk->AIHandle );
  }
  GFL_HEAP_FreeMemory( wk->cmdQue );
  BTL_ADAPTER_Delete( wk->adapter );

  GFL_HEAP_FreeMemory( wk );
}

/**
 *  �^��L�^�f�[�^���擾
 */
const void* BTL_CLIENT_GetRecordData( BTL_CLIENT* wk, u32* size )
{
  if( wk->btlRec ){
    return BTL_REC_GetDataPtr( wk->btlRec, size );
  }
  return NULL;
}


void BTL_CLIENT_SetRecordPlayerMode( BTL_CLIENT* wk, BTL_RECREADER* recReader )
{
  u32 turnCnt;

  wk->myType = BTL_CLIENT_TYPE_RECPLAY;
  wk->btlRecReader = recReader;
  turnCnt = BTL_RECREADER_GetTurnCount( wk->btlRecReader );

  RecPlayer_Setup( &wk->recPlayer, turnCnt );
}



//=============================================================================================
/**
 * �`�揈�����W���[�����A�^�b�`�iUI�N���C�A���g�̂݁j
 *
 * @param   wk
 * @param   viewCore
 *
 */
//=============================================================================================
void BTL_CLIENT_AttachViewCore( BTL_CLIENT* wk, BTLV_CORE* viewCore )
{
  wk->viewCore = viewCore;
}

//=============================================================================================
/**
 * �R�}���h�������`�F�N�p�̃T�[�o���W���[�����A�^�b�`
 *
 * @param   wk
 * @param   server
 */
//=============================================================================================
void BTL_CLIENT_AttachCmeCheckServer( BTL_CLIENT* wk, BTL_SERVER* server )
{
  wk->cmdCheckServer = server;
}



//=============================================================================================
/**
 * ���������A�_�v�^��Ԃ�
 *
 * @param   wk
 *
 * @retval  BTL_ADAPTER*
 */
//=============================================================================================
BTL_ADAPTER* BTL_CLIENT_GetAdapter( BTL_CLIENT* wk )
{
  GF_ASSERT(wk);
  return wk->adapter;
}

//=============================================================================================
/**
 * �N���C�A���g���C�����[�v
 *
 * @param   wk
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTL_CLIENT_Main( BTL_CLIENT* wk )
{
  return wk->mainProc( wk );
}

//=============================================================================================
/**
 * �`���v�^�X�L�b�v
 *
 * @param   wk
 * @param   fChapterSkipMode
 */
//=============================================================================================
void BTL_CLIENT_SetChapterSkip( BTL_CLIENT* wk, u32 nextTurnNum )
{
  // @todo �����main�ɂ�������C����ׂ����H
  BTL_RECREADER_Reset( wk->btlRecReader );

  RecPlayer_ChapterSkipOn( &wk->recPlayer, nextTurnNum );
  ChangeMainProc( wk, ClientMain_ChapterSkip );
}

void BTL_CLIENT_StopChapterSkip( BTL_CLIENT* wk )
{
//  ChangeMainProc( wk, ClientMain_Normal );
  RecPlayer_ChapterSkipOff( &wk->recPlayer );
  ChangeMainProc( wk, ClientMain_Normal );
}

//=============================================================================================
/**
 * �N���C�A���g���^��Đ����[�h�œ����Ă��邩����
 *
 * @param   wk
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTL_CLIENT_IsRecPlayerMode( const BTL_CLIENT* wk )
{
  return wk->myType == BTL_CLIENT_TYPE_RECPLAY;
}
//=============================================================================================
/**
 *
 *
 * @param   wk
 *
 * @retval  u32
 */
//=============================================================================================
u32 BTL_CLIENT_GetRecPlayerMaxChapter( const BTL_CLIENT* wk )
{
  return wk->recPlayer.maxTurnCount;
}

//=============================================================================================
/**
 * �`���v�^�[�X�L�b�v���[�h�œ��삵�Ă��邩����
 *
 * @param   wk
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTL_CLIENT_IsChapterSkipMode( const BTL_CLIENT* wk )
{
  return (wk->mainProc == ClientMain_ChapterSkip);
}




/**
 *  ���C�����[�v�֐��������ւ���
 */
static void ChangeMainProc( BTL_CLIENT* wk, ClientMainProc proc )
{
  wk->mainProc = proc;
  wk->myState = 0;
}

/**
 *  ���C�����[�v�F�ʏ펞
 */
static BOOL ClientMain_Normal( BTL_CLIENT* wk )
{
  enum {
    SEQ_READ_ACMD = 0,
    SEQ_EXEC_CMD,
    SEQ_RETURN_TO_SV,
    SEQ_RETURN_TO_SV_QUIT,
    SEQ_RECPLAY_CTRL,
    SEQ_BGM_FADEOUT,
    SEQ_IDLE,
    SEQ_QUIT,
  };

  RecPlayerCtrl_Main( wk, &wk->recPlayer );

  switch( wk->myState ){
  case SEQ_READ_ACMD:
    {
      BtlAdapterCmd  cmd = BTL_ADAPTER_RecvCmd(wk->adapter);
      if( cmd == BTL_ACMD_QUIT_BTL )
      {
        const u32* p;
        BTL_ADAPTER_GetRecvData( wk->adapter, (const void**)&p );
        wk->escapeClientID = *p;
        BTL_N_Printf( DBGSTR_CLIENT_RecvedQuitCmd, wk->myID );
        setDummyReturnData( wk );
        wk->subSeq = 0;
        wk->myState = SEQ_RETURN_TO_SV_QUIT;
        break;
      }
      if( cmd != BTL_ACMD_NONE )
      {
        wk->subProc = getSubProc( wk, cmd );
        if( wk->subProc != NULL ){
          BTL_N_Printf( DBGSTR_CLIENT_StartCmd, wk->myID, cmd );
          wk->myState = SEQ_EXEC_CMD;
          wk->subSeq = 0;
        }
        else
        {
          setDummyReturnData( wk );
          wk->subSeq = 0;
          wk->myState = SEQ_RETURN_TO_SV;
        }
      }
    }
    break;

  case SEQ_EXEC_CMD:
    if( wk->subProc(wk, &wk->subSeq) )
    {
      if( RecPlayer_CheckBlackOut(&wk->recPlayer) )
      {
        wk->myState = SEQ_RECPLAY_CTRL;
      }
      else
      {
        BTL_N_Printf( DBGSTR_CLIENT_RETURN_CMD_START, wk->myID );
        wk->myState = SEQ_RETURN_TO_SV;
      }
    }
    break;

  case SEQ_RETURN_TO_SV:
    if( BTL_ADAPTER_ReturnCmd(wk->adapter, wk->returnDataPtr, wk->returnDataSize) ){
      wk->myState = SEQ_READ_ACMD;
      BTL_N_Printf( DBGSTR_CLIENT_RETURN_CMD_DONE, wk->myID, wk->returnDataSize );
    }
    break;

  case SEQ_RETURN_TO_SV_QUIT:
    if( BTL_ADAPTER_ReturnCmd(wk->adapter, wk->returnDataPtr, wk->returnDataSize) ){
      wk->myState = SEQ_QUIT;
      BTL_N_Printf( DBGSTR_CLIENT_ReplyToQuitCmd, wk->myID );
    }
    break;

  // �^��Đ��R���g���[���F�u���b�N�A�E�g��
  case SEQ_RECPLAY_CTRL:
    if( RecPlayer_GetCtrlCode(&wk->recPlayer) == RECCTRL_QUIT )
    {
      wk->myState = SEQ_QUIT;
    }
    else
    {
      if( wk->viewCore ){
        PMSND_FadeOutBGM( 30 );
      }
      wk->myState = SEQ_BGM_FADEOUT;
    }
    break;
  case SEQ_BGM_FADEOUT:
    if( !PMSND_CheckFadeOnBGM() )
    {
      // nextTurn �������ɂ��đS�N���C�A���g�� BTL_CLIENT_SetChapterSkip ���R�[���o�b�N�����
      u32 nextTurn = RecPlayer_GetNextTurn( &wk->recPlayer );

      PMSND_AllPlayerVolumeEnable( FALSE, PMSND_MASKPL_BGM );
      BTL_MAIN_ResetForRecPlay( wk->mainModule, nextTurn );
    }
    break;

  case SEQ_QUIT:
    return TRUE;
  }
  return FALSE;
}
/**
 *  ���C�����[�v�F�`���v�^�X�L�b�v��
 */
static BOOL ClientMain_ChapterSkip( BTL_CLIENT* wk )
{
  enum {
    SEQ_RECPLAY_READ_ACMD = 0,
    SEQ_RECPLAY_EXEC_CMD,
    SEQ_RECPLAY_RETURN_TO_SV,
    SEQ_RECPLAY_FADEIN,

    SEQ_RECPLAY_QUIT,
  };

  RecPlayerCtrl_Main( wk, &wk->recPlayer );

  switch( wk->myState ){

  case SEQ_RECPLAY_READ_ACMD:
    {
      BOOL fSkipEnd = RecPlayer_CheckChapterSkipEnd( &wk->recPlayer );
      if( !fSkipEnd )
      {
        BtlAdapterCmd  cmd = BTL_ADAPTER_RecvCmd( wk->adapter );
        if( cmd != BTL_ACMD_NONE )
        {
          TAYA_Printf("RecPlay acmd=%d\n", cmd);
          wk->subProc = getSubProc( wk, cmd );
          if( wk->subProc != NULL ){
            wk->myState = SEQ_RECPLAY_EXEC_CMD;
            wk->subSeq = 0;
          }
          else
          {
            setDummyReturnData( wk );
            wk->myState = SEQ_RECPLAY_RETURN_TO_SV;
            wk->subSeq = 0;
          }
        }
      }

      if( fSkipEnd )
      {
        BTL_N_Printf( DBGSTR_CLIENT_RecPlay_ChapterSkipped, wk->myID, wk->recPlayer.nextTurnCount);
        if( wk->viewCore ){
          PMSND_AllPlayerVolumeEnable( TRUE, PMSND_MASKPL_BGM );
          BTLV_RecPlayFadeIn_Start( wk->viewCore );
          PMSND_FadeInBGM( 30 );
        }
        wk->myState = SEQ_RECPLAY_FADEIN;
      }
    }
    break;

  case SEQ_RECPLAY_EXEC_CMD:
    if( wk->subProc(wk, &wk->subSeq) )
    {
      wk->myState = SEQ_RECPLAY_RETURN_TO_SV;
      wk->subSeq = 0;
    }
    break;

  case SEQ_RECPLAY_RETURN_TO_SV:
    if( BTL_ADAPTER_ReturnCmd(wk->adapter, wk->returnDataPtr, wk->returnDataSize) ){
      wk->myState = SEQ_RECPLAY_READ_ACMD;
      BTL_N_PrintfEx( PRINT_FLG, DBGSTR_CLIENT_RETURN_CMD_DONE, wk->myID );
    }
    break;

  case SEQ_RECPLAY_FADEIN:
    if( wk->viewCore != NULL )
    {
      if( BTLV_RecPlayFadeIn_Wait(wk->viewCore)
      &&  !PMSND_CheckFadeOnBGM()
      ){
        BTL_MAIN_NotifyChapterSkipEnd( wk->mainModule );
      }
    }
    break;

  case SEQ_RECPLAY_QUIT:
    return TRUE;

  }
  return FALSE;
}
/**
 *  �_�~�[�ԐM�f�[�^�i�T�[�o�ɕԐM����K�v�����邪���e�͖���Ȃ��P�[�X�j���쐬
 */
static void setDummyReturnData( BTL_CLIENT* wk )
{
  wk->dummyReturnData = 0;
  wk->returnDataPtr = &(wk->dummyReturnData);
  wk->returnDataSize = sizeof(wk->dummyReturnData);
}

static ClientSubProc getSubProc( BTL_CLIENT* wk, BtlAdapterCmd cmd )
{
  static const struct {
    BtlAdapterCmd   cmd;
    ClientSubProc   proc[ BTL_CLIENT_TYPE_MAX ];
  }procTbl[] = {

    { BTL_ACMD_WAIT_SETUP,
      { SubProc_UI_Setup,          NULL,                      SubProc_REC_Setup          } },

    { BTL_ACMD_SELECT_ROTATION,
      { SubProc_UI_SelectRotation, SubProc_AI_SelectRotation, SubProc_REC_SelectRotation } },

#if 1
    { BTL_ACMD_SELECT_ACTION,
     { SubProc_UI_SelectAction,    SubProc_AI_SelectAction,   SubProc_REC_SelectAction   } },
#else
// AI�Ƀe�X�g�쓮������
    { BTL_ACMD_SELECT_ACTION,
       { SubProc_AI_SelectAction,  SubProc_AI_SelectAction,   SubProc_REC_SelectAction   } },
#endif
    { BTL_ACMD_SELECT_CHANGE_OR_ESCAPE,
       { SubProc_UI_SelectChangeOrEscape, NULL,  NULL  }
    },

    { BTL_ACMD_SELECT_POKEMON_FOR_COVER,
       { SubProc_UI_SelectPokemonForCover, SubProc_AI_SelectPokemon,   SubProc_REC_SelectPokemon  }
    },

    { BTL_ACMD_SELECT_POKEMON_FOR_CHANGE,
       { SubProc_UI_SelectPokemonForChange, SubProc_AI_SelectPokemon,  SubProc_REC_SelectPokemon  } },

    { BTL_ACMD_CONFIRM_IREKAE,
       { SubProc_UI_ConfirmIrekae,      NULL,  SubProc_REC_SelectPokemon  } },

    { BTL_ACMD_SERVER_CMD,
       { SubProc_UI_ServerCmd,     NULL,                      SubProc_REC_ServerCmd      } },

    { BTL_ACMD_RECORD_DATA,
       { SubProc_UI_RecordData,    NULL,                      NULL                       } },

    { BTL_ACMD_EXIT_NPC,
      { SubProc_UI_ExitForNPC,   NULL,  NULL }, },

    { BTL_ACMD_EXIT_LOSE_WILD,
      { SubProc_UI_LoseWild,   NULL,  NULL }, },


    { BTL_ACMD_EXIT_COMM,
      { SubProc_UI_ExitCommTrainer,   NULL,  NULL }, },

    { BTL_ACMD_NOTIFY_TIMEUP,
      { SubProc_UI_NotifyTimeUp,   NULL,  NULL }, },
  };

  int i;

  for(i=0; i<NELEMS(procTbl); i++)
  {
    if( procTbl[i].cmd == cmd )
    {
      return procTbl[i].proc[ wk->myType ];
    }
  }

  GF_ASSERT(0);
  return NULL;
}



//=============================================================================================
/**
 * �������N���C�A���g��ID���擾�i�T�[�o�R�}���h����đ����Ă���j
 *
 * @param   wk
 *
 * @retval  u8
 */
//=============================================================================================
u8 BTL_CLIENT_GetEscapeClientID( const BTL_CLIENT* wk )
{
  return wk->escapeClientID;
}

//=============================================================================================
/**
 * �Q�[���������Ԃ̏I���`�F�b�N
 *
 * @param   wk
 *
 * @retval  BOOL    �Q�[���ɐ������Ԃ��ݒ肳��Ă���A�����ꂪ�I����Ă���ꍇ��TRUE
 */
//=============================================================================================
BOOL BTL_CLIENT_IsGameTimeOver( const BTL_CLIENT* wk )
{
  if( wk->gameLimitTime )
  {
    return BTLV_EFFECT_IsZero( BTLV_TIMER_TYPE_GAME_TIME );
  }
  return FALSE;
}

//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------



static BOOL SubProc_UI_Setup( BTL_CLIENT* wk, int* seq )
{
  switch( *seq ){
  case 0:
    if( BTL_MAIN_GetCompetitor(wk->mainModule) != BTL_COMPETITOR_DEMO_CAPTURE ){
      BTLV_StartCommand( wk->viewCore, BTLV_CMD_SETUP );
    }else{
      BTLV_StartCommand( wk->viewCore, BTLV_CMD_SETUP_DEMO );
    }
    (*seq)++;
    break;
  case 1:
    if( BTLV_WaitCommand(wk->viewCore) )
    {
      wk->cmdLimitTime  = BTL_MAIN_GetCommandLimitTime( wk->mainModule );
      wk->gameLimitTime = BTL_MAIN_GetGameLimitTime( wk->mainModule );
      if( wk->cmdLimitTime || wk->gameLimitTime )
      {
        BTLV_EFFECT_CreateTimer( wk->gameLimitTime, wk->cmdLimitTime );
        if( wk->gameLimitTime ){
          BTLV_EFFECT_DrawEnableTimer( BTLV_TIMER_TYPE_GAME_TIME, TRUE, TRUE );
        }
      }

      EnemyPokeHPBase_Update( wk );
      return TRUE;
    }
    break;
  }
  return FALSE;
}

static BOOL SubProc_REC_Setup( BTL_CLIENT* wk, int* seq )
{
  if( wk->viewCore )
  {
    return SubProc_UI_Setup( wk, seq );
  }
  return TRUE;
}
//------------------------------------------------------------------------------------------------------
// �G�擪�|�P�����̊HP�l���L�^�i�|�P�����������߃��b�Z�[�W�p�F��ʐM�A�V���O����̂݁j
//------------------------------------------------------------------------------------------------------
static void EnemyPokeHPBase_Update( BTL_CLIENT* wk )
{
  const BTL_POKEPARAM* bpp = EnemyPokeHPBase_GetTargetPoke( wk );
  if( bpp ){
    wk->EnemyPokeHPBase = BPP_GetValue( bpp, BPP_HP );
    BTL_N_Printf( DBGSTR_CLIENT_UpdateEnemyBaseHP, wk->EnemyPokeHPBase, BPP_GetID(bpp) );
  }
}
static u32 EnemyPokeHPBase_CheckRatio( BTL_CLIENT* wk )
{
  const BTL_POKEPARAM* bpp = EnemyPokeHPBase_GetTargetPoke( wk );
  if( bpp ){
    u32 hp = BPP_GetValue( bpp, BPP_HP );
    if( hp >= wk->EnemyPokeHPBase ){
      return 0;
    }
    return ((wk->EnemyPokeHPBase - hp) * 100) / hp;
  }
  return 0;
}
static const BTL_POKEPARAM* EnemyPokeHPBase_GetTargetPoke( BTL_CLIENT* wk )
{
  if( (BTL_MAIN_GetRule(wk->mainModule) == BTL_RULE_SINGLE)
  &&  (BTL_MAIN_GetCompetitor(wk->mainModule) != BTL_COMPETITOR_COMM)
  ){
    u8 clientID = BTL_MAIN_GetOpponentClientID( wk->mainModule, wk->myID, 0 );
    return BTL_POKECON_GetClientPokeDataConst( wk->pokeCon, clientID, 0 );
  }
  return NULL;
}



//------------------------------------------------------------------------------------------------------
// ���[�e�[�V�����I��
//------------------------------------------------------------------------------------------------------

static BOOL SubProc_UI_SelectRotation( BTL_CLIENT* wk, int* seq )
{
  switch( *seq ){
  case 0:
    {
      BTLV_UI_SelectRotation_Start( wk->viewCore, wk->prevRotateDir );
      CmdLimit_Start( wk );
      (*seq)++;
    }
    break;

  case 1:
    if( CmdLimit_CheckOver(wk) ){
      BTLV_UI_SelectRotation_ForceQuit( wk->viewCore );
      (*seq) = 2;
      break;
    }

    {
      BtlRotateDir  dir;
      if( BTLV_UI_SelectRotation_Wait(wk->viewCore, &dir) ){
        wk->prevRotateDir = dir;
        (*seq) = 3;
      }
    }
    break;

  // �����I�����̃����_�����菈��
  case 2:
    {
      wk->prevRotateDir = decideNextDirRandom( wk->prevRotateDir );
      (*seq)++;
    }
    break;

  case 3:
    CmdLimit_End( wk );
    (*seq)++;
    break;
  case 4:
    wk->returnDataPtr = &wk->prevRotateDir;
    wk->returnDataSize = sizeof(wk->prevRotateDir);
    return TRUE;
  }

  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * �������Ԃɂ�鋭���I�����A�����_���ŕ��������肷��
 *
 * @param   prevDir
 *
 * @retval  BtlRotateDir
 */
//----------------------------------------------------------------------------------
static BtlRotateDir decideNextDirRandom( BtlRotateDir prevDir )
{
  u32 rnd = GFL_STD_MtRand( 100 );

  if( prevDir == BTL_ROTATEDIR_NONE )
  {
    if( rnd < 30 ){
      prevDir = BTL_ROTATEDIR_L;
    }else if( rnd < 60 ){
      prevDir = BTL_ROTATEDIR_R;
    }else{
      prevDir = BTL_ROTATEDIR_STAY;
    }
  }
  else
  {
    switch( prevDir ){
    case BTL_ROTATEDIR_STAY:
    default:
      prevDir = (rnd < 50)? BTL_ROTATEDIR_R : BTL_ROTATEDIR_L;
      break;

    case BTL_ROTATEDIR_L:
      prevDir = (rnd < 40)? BTL_ROTATEDIR_R : BTL_ROTATEDIR_STAY;
      break;

    case BTL_ROTATEDIR_R:
      prevDir = (rnd < 40)? BTL_ROTATEDIR_L : BTL_ROTATEDIR_STAY;
      break;
    }
  }

  return prevDir;
}


static BOOL SubProc_REC_SelectRotation( BTL_CLIENT* wk, int* seq )
{
  // @todo ������
  return TRUE;
}

static BOOL SubProc_AI_SelectRotation( BTL_CLIENT* wk, int* seq )
{
  wk->prevRotateDir = decideNextDirRandom( wk->prevRotateDir );
  wk->returnDataPtr = &wk->prevRotateDir;
  wk->returnDataSize = sizeof(wk->prevRotateDir);

  return TRUE;
}

//----------------------------------------------------------------------------------
/**
 * �������� �\�����J�E���g�J�n
 *
 * @param   wk
 */
//----------------------------------------------------------------------------------
static void CmdLimit_Start( BTL_CLIENT* wk )
{
  if( wk->cmdLimitTime )
  {
    BTL_N_Printf( DBGSTR_CLIENT_CmdLimitTimeEnable, wk->cmdLimitTime );
    BTLV_EFFECT_DrawEnableTimer( BTLV_TIMER_TYPE_COMMAND_TIME, TRUE, TRUE );
    wk->fForceQuitSelAct = FALSE;
  }
}
//----------------------------------------------------------------------------------
/**
 * �������� �^�C���I�[�o�[�`�F�b�N
 *
 * @param   wk
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL CmdLimit_CheckOver( BTL_CLIENT* wk )
{
  if( wk->cmdLimitTime )
  {
    if( wk->fForceQuitSelAct == FALSE )
    {
      if( BTLV_EFFECT_IsZero( BTLV_TIMER_TYPE_COMMAND_TIME ) ){
        BTL_N_Printf( DBGSTR_CLIENT_CmdLimitTimeOver );
        wk->fForceQuitSelAct = TRUE;
      }
    }
    return wk->fForceQuitSelAct;
  }
  return FALSE;
}

/**
 *  �R�}���h���Ԑ����ɂ�鋭���I��������΁A���C���v���Z�X��؂�ւ���TRUE��Ԃ�
 */
static BOOL CheckSelactForceQuit( BTL_CLIENT* wk, ClientSubProc nextProc )
{
  if( CmdLimit_CheckOver(wk) ){
    ClientSubProc_Set( wk, nextProc );
    return TRUE;
  }
  return FALSE;
}
/**
 *  �R�}���h���Ԑ����\�����I��
 */
static void CmdLimit_End( BTL_CLIENT* wk )
{
  if( wk->cmdLimitTime )
  {
    BTLV_EFFECT_DrawEnableTimer( BTLV_TIMER_TYPE_COMMAND_TIME, FALSE, FALSE );
  }
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
/**
 *  �T�u�v���Z�X�ݒ�
 */
static void ClientSubProc_Set( BTL_CLIENT* wk, ClientSubProc proc )
{
  wk->selActProc = proc;
  wk->selActSeq = 0;
}
/**
 *  �T�u�v���Z�X�R�[��
 */
static BOOL ClientSubProc_Call( BTL_CLIENT* wk )
{
  return wk->selActProc( wk, &wk->selActSeq );
}



static BOOL SubProc_UI_SelectAction( BTL_CLIENT* wk, int* seq )
{
  switch( *seq ){
  case 0:
    ClientSubProc_Set( wk, selact_Start );
    CmdLimit_Start( wk );
    (*seq)++;
    break;

  case 1:
    CmdLimit_CheckOver( wk );
    if( ClientSubProc_Call(wk) ){
      return TRUE;
    }
    break;
  }

  return FALSE;
}

static BOOL SubProc_REC_SelectAction( BTL_CLIENT* wk, int* seq )
{
  u8 numAction, fChapter;

  const BTL_ACTION_PARAM* act = BTL_RECREADER_ReadAction( wk->btlRecReader, wk->myID, &numAction, &fChapter );
  if( fChapter ){
    RecPlayer_TurnIncReq( &wk->recPlayer );
  }

  wk->returnDataPtr = act;
  wk->returnDataSize = numAction * sizeof(BTL_ACTION_PARAM);

  BTL_N_Printf( DBGSTR_CLIENT_ReadRecAct, wk->myID, numAction);
  if( act->gen.cmd  == BTL_ACTION_FIGHT ){
    BTL_N_Printf( DBGSTR_CLIENT_ReadRecAct_Fight, act->fight.waza);
  }
  if( act->gen.cmd  == BTL_ACTION_CHANGE ){
    BTL_N_Printf( DBGSTR_CLIENT_ReadRecAct_Change, act->change.memberIdx);
  }
  if( act->gen.cmd  == BTL_ACTION_MOVE ){
    BTL_N_Printf( DBGSTR_CLIENT_ReadRecAct_Move );
  }

  return TRUE;
}



//----------------------------------------------------------------------
/**
 *  �A�N�V�����I�����[�N������
 */
//----------------------------------------------------------------------
static BOOL selact_Start( BTL_CLIENT* wk, int* seq )
{
  wk->procPokeIdx = 0;
  wk->prevPokeIdx = -1;
  wk->firstPokeIdx = 0;

  // �_�u���ȏ�̎��A�u���ɑI�΂�Ă���|�P�����v���L�^���邽�߂ɏ������������ōs��
  setupPokeSelParam( wk, BPL_MODE_NORMAL, wk->numCoverPos, &wk->pokeSelParam, &wk->pokeSelResult  );

  shooterCost_Init( wk );

  {
    const BTL_POKEPARAM* bpp;
    u32 i;

    for(i=0; i<NELEMS(wk->actionParam); ++i){
      BTL_ACTION_SetNULL( &wk->actionParam[i] );
    }

    for(i=0; i<wk->numCoverPos; ++i)
    {
      bpp = BTL_POKECON_GetClientPokeData( wk->pokeCon, wk->myID, i );
      if( !is_action_unselectable(wk, bpp,  NULL) ){
        wk->firstPokeIdx = i;
        break;
      }
    }
  }


  ClientSubProc_Set( wk, selact_Root );

  return FALSE;
}
/**
 *  �A�N�V�����I��r���̕W�����b�Z�[�W�E�B���h�E�����o��
 */
static void selact_startMsg( BTL_CLIENT* wk, const BTLV_STRPARAM* strParam )
{
  BTLV_StartMsg( wk->viewCore, strParam );
  wk->fStdMsgChanged = TRUE; // �u�����͂ǂ�����H�v���b�Z�[�W�������������t���OON
}
//----------------------------------------------------------------------
/**
 *  �A�N�V�����I���̋����I��
 */
//----------------------------------------------------------------------
static BOOL selact_ForceQuit( BTL_CLIENT* wk, int* seq )
{
  switch( *seq ){
  case 0:
    BTLV_ForceQuitInput_Notify( wk->viewCore );
    (*seq)++;
    break;
  case 1:
    if( BTLV_ForceQuitInput_Wait(wk->viewCore) )
    {
      while( wk->procPokeIdx < wk->numCoverPos )
      {
        wk->procPoke = BTL_POKECON_GetClientPokeData( wk->pokeCon, wk->myID, wk->procPokeIdx );
        wk->procAction = &wk->actionParam[ wk->procPokeIdx ];
        BTL_ACTION_SetNULL( wk->procAction );
        if( !is_action_unselectable(wk, wk->procPoke,  wk->procAction) )
        {
          u8 usableWazaFlag[ PTL_WAZA_MAX ];
          u8 wazaIdx = StoreSelectableWazaFlag( wk, wk->procPoke, usableWazaFlag );
          if( wazaIdx < PTL_WAZA_MAX )
          {
            WazaID waza = BPP_WAZA_GetID( wk->procPoke, wazaIdx );
            BtlPokePos  targetPos = BTL_CALC_DecideWazaTargetAuto( wk->mainModule, wk->pokeCon, wk->procPoke, waza );
            BTL_ACTION_SetFightParam( wk->procAction, waza, targetPos );
          }else{
            setWaruagakiAction( wk->procAction, wk, wk->procPoke );
          }
        }
        wk->procPokeIdx++;
      }
      (*seq)++;
    }
    break;
  case 2:
    wk->returnDataPtr = &(wk->actionParam[0]);
    wk->returnDataSize = sizeof(wk->actionParam[0]) * wk->numCoverPos;
    ClientSubProc_Set( wk, selact_Finish );
    break;
  }
  return FALSE;
}

//@todo �g���[�i�[���b�Z�[�W�\������
#include  "tr_tool/tr_tool.h"
#include  "tr_tool/trtype_def.h"
#include  "btlv/btlv_effect.h"
#include  "btlv/btlv_gauge.h"


static  BOOL  check_tr_message( BTL_CLIENT* wk, u16* msgID )
{
  if( BTL_MAIN_GetCompetitor(wk->mainModule) == BTL_COMPETITOR_TRAINER )
  {
    static const u16 TRMsgTbl[] = {
      TRMSG_FIGHT_POKE_HP_HALF,       ///< HP�����ȉ��i����j
      TRMSG_FIGHT_FIRST_DAMAGE,       ///< HP���^���łȂ��i����j
      TRMSG_FIGHT_POKE_LAST,          ///< �Ō�̂P�̓o��
      TRMSG_FIGHT_POKE_LAST_HP_HALF,  ///< �Ō�̂P�̂�HP�����ȉ�
    };

    u8 clientID = BTL_MAIN_GetEnemyClientID( wk->mainModule, 0 );
    u32 trainerID = BTL_MAIN_GetClientTrainerID( wk->mainModule, clientID );

    if( trainerID != TRID_NULL )
    {
      BTL_PARTY* party = BTL_POKECON_GetPartyData( wk->pokeCon, clientID );
      const BTL_POKEPARAM* bpp = BTL_POKECON_GetClientPokeData( wk->pokeCon, clientID, 0 );
      u32 i;
      int enableIndex;

      // ��ɏo�Ă���|�P�Ő����Ă��郄�c��T���Ă���
      if( BPP_IsDead(bpp) )
      {
        u32 numCoverPos = BTL_MAIN_GetClientCoverPosCount( wk->mainModule, clientID );
        for(i=1; i<numCoverPos; ++i){
          bpp = BTL_POKECON_GetClientPokeData( wk->pokeCon, clientID, i );
          if( !BPP_IsDead(bpp) ){
            break;
          }
        }
      }

      enableIndex = -1;
      for(i=0; i<NELEMS(TRMsgTbl); ++i)
      {
        if( wk->AITrainerMsgCheckedFlag[i] == FALSE )
        {
          if( TT_TrainerMessageCheck(trainerID, TRMsgTbl[i], wk->heapID ) )
          {
            switch( TRMsgTbl[i] ){
            case TRMSG_FIGHT_POKE_HP_HALF:
              if( BPP_GetValue(bpp,BPP_HP) <= BTL_CALC_QuotMaxHP(bpp,2) ){
                enableIndex = i;
                wk->AITrainerMsgCheckedFlag[ i ] = TRUE;
              }
              break;

            case TRMSG_FIGHT_FIRST_DAMAGE:       ///< HP���^���łȂ��i����j
              if( !BPP_IsHPFull(bpp) ){
                enableIndex = i;
                wk->AITrainerMsgCheckedFlag[ i ] = TRUE;
              }
              break;

            case TRMSG_FIGHT_POKE_LAST:          ///< �Ō�̂P�̓o��
              if( (BTL_PARTY_GetMemberCount(party) > 1)
              &&  (BTL_PARTY_GetAliveMemberCount(party) == 1)
              ){
                enableIndex = i;
                wk->AITrainerMsgCheckedFlag[ i ] = TRUE;
              }
              break;

            case TRMSG_FIGHT_POKE_LAST_HP_HALF:  ///< �Ō�̂P�̂�HP�����ȉ�
              if( (BTL_PARTY_GetMemberCount(party) > 1)
              &&  (BTL_PARTY_GetAliveMemberCount(party) == 1)
              &&  (BPP_GetValue(bpp,BPP_HP) <= BTL_CALC_QuotMaxHP(bpp,2) )
              ){
                enableIndex = i;
                wk->AITrainerMsgCheckedFlag[ i ] = TRUE;
              }
            }
          }
          else
          {
            wk->AITrainerMsgCheckedFlag[i] = TRUE;
          }
        }
      } /* for( i<NELEMS(TRMsgTbl) ) */

      if( enableIndex >= 0 )
      {
        *msgID = TRMsgTbl[ enableIndex ];
        return TRUE;
      }

    } /* if( trainerID != TRID_NULL ) */
  }

  return FALSE;
}

//----------------------------------------------------------------------
/**
 *  �A�N�V�����I�����[�g
 */
//----------------------------------------------------------------------
static BOOL selact_Root( BTL_CLIENT* wk, int* seq )
{
  switch( *seq ){
  case 0:
    wk->procPoke = BTL_POKECON_GetClientPokeData( wk->pokeCon, wk->myID, wk->procPokeIdx );
    wk->procAction = &wk->actionParam[ wk->procPokeIdx ];
    wk->actionAddCount = 0;
    BTL_ACTION_SetNULL( wk->procAction );

    if( is_action_unselectable(wk, wk->procPoke,  wk->procAction) ){
      BTL_N_PrintfEx( PRINT_FLG, DBGSTR_CLIENT_SelectActionSkip, wk->procPokeIdx );
      ClientSubProc_Set( wk, selact_CheckFinish );
    }
    else{
      //@todo �g���[�i�[���b�Z�[�W�\������
#if 1
      if( check_tr_message( wk, &wk->AITrainerMsgID ) == TRUE )
      {
        (*seq) = 5;
      }
      else
      {
        (*seq)++;
      }
#else
      (*seq)++;
#endif
    }
    break;

  case 1:
    // �u�����͂ǂ�����H�v�\��
    if( (wk->prevPokeIdx != wk->procPokeIdx)
    ||  (wk->fStdMsgChanged)
    ){
      BTLV_STRPARAM_Setup( &wk->strParam, BTL_STRTYPE_STD, BTL_STRID_STD_SelectAction );
      BTLV_STRPARAM_AddArg( &wk->strParam, BPP_GetID(wk->procPoke) );
      BTLV_STRPARAM_SetWait( &wk->strParam, 0 );
      BTLV_PrintMsgAtOnce( wk->viewCore, &wk->strParam );
      wk->fStdMsgChanged = FALSE;
      wk->prevPokeIdx = wk->procPokeIdx;
      (*seq)++;
    }
    else{
      (*seq) += 2;
    }
    break;
  case 2:
    if( !BTLV_WaitMsg(wk->viewCore) ){
      return FALSE;
    }
    (*seq)++;
    /* fallthru */

  case 3:
    // �A�N�V�����I���J�n
    BTL_N_Printf( DBGSTR_CLIENT_SelectActionStart, wk->procPokeIdx, BPP_GetID(wk->procPoke), wk->firstPokeIdx );
    BTLV_UI_SelectAction_Start( wk->viewCore, wk->procPoke, (wk->procPokeIdx>wk->firstPokeIdx), wk->procAction );
    (*seq)++;
    break;

  case 4:
    if( CheckSelactForceQuit(wk, selact_ForceQuit) )
    {
      BTL_N_Printf( DBGSTR_CLIENT_ForceQuitByTimeLimit, wk->myID );
      BTLV_UI_SelectAction_ForceQuit( wk->viewCore );
      return FALSE;
    }

    switch( BTLV_UI_SelectAction_Wait(wk->viewCore) ){

    // ����ւ��|�P�����I���̏ꍇ�͂܂��A�N�V�����p�����[�^���s�\��->�|�P�����I����
    case BTL_ACTION_CHANGE:
      BTL_N_Printf( DBGSTR_CLIENT_SelectAction_Pokemon );
      shooterCost_Save( wk, wk->procPokeIdx, 0 );
      ClientSubProc_Set( wk, selact_SelectChangePokemon );
      break;

    // �u���������v��I��
    case BTL_ACTION_FIGHT:
      BTL_N_Printf( DBGSTR_CLIENT_SelectAction_Fight );
      shooterCost_Save( wk, wk->procPokeIdx, 0 );
      ClientSubProc_Set( wk, selact_Fight );
      break;

    // �u�ǂ����v��I��
    case BTL_ACTION_ITEM:
      // �V���[�^�[�g���Ȃ��ݒ�`�F�b�N
      if( wk->bagMode == BBAG_MODE_SHOOTER )
      {
        const SHOOTER_ITEM_BIT_WORK* shooterReg = BTL_MAIN_GetSetupShooterBit( wk->mainModule );
        if( shooterReg->shooter_use == FALSE)
        {
          BTLV_UI_Restart( wk->viewCore );
          (*seq) = 3;
          break;
        }
      }

      ClientSubProc_Set( wk, selact_Item );
      break;

    // �u�ɂ���vor�u���ǂ�v
    case BTL_ACTION_ESCAPE:
      // �擪�̃|�P�Ȃ�u�ɂ���v�Ƃ��ď���
      if( wk->procPokeIdx == wk->firstPokeIdx ){
        shooterCost_Save( wk, wk->procPokeIdx, 0 );
        ClientSubProc_Set( wk, selact_Escape );
      // �Q�̖ڈȍ~�́u���ǂ�v�Ƃ��ď���
      }else{
        BTL_POKEPARAM* bpp;
        while( wk->procPokeIdx )
        {
          wk->procPokeIdx--;
          bpp = BTL_POKECON_GetClientPokeData( wk->pokeCon, wk->myID, wk->procPokeIdx );
          if( !is_action_unselectable(wk, bpp, NULL) )
          {
            wk->shooterEnergy += shooterCost_Get( wk, wk->procPokeIdx );
            // �u���ǂ�v��̃|�P�������A���Ɂu�|�P�����v�Ō����Ώۂ�I��ł����ꍇ�͂��̏���Pop����
            if( BTL_ACTION_GetAction( &wk->actionParam[wk->procPokeIdx] ) == BTL_ACTION_CHANGE ){
              BTL_POKESELECT_RESULT_Pop( &wk->pokeSelResult );
            }
            ClientSubProc_Set( wk, selact_Root );
            return FALSE;
          }
        }
        GF_ASSERT(0);
        break;
      }
      break;
    }
    break;

  //@todo �g���[�i�[���b�Z�[�W�\������
  case 5:
    {
      u8 clientID = BTL_MAIN_GetEnemyClientID( wk->mainModule, 0 );
      u32 trainerID = BTL_MAIN_GetClientTrainerID( wk->mainModule, clientID );
      int trtype = BTL_MAIN_GetClientTrainerType( wk->mainModule, clientID );

      BTLV_EFFECT_SetTrainer( trtype, BTLV_MCSS_POS_TR_BB, 0, 0, 0 );
      BTLV_EFFECT_Add( BTLEFF_TRAINER_IN );
      BTLV_StartMsgTrainer( wk->viewCore, trainerID, wk->AITrainerMsgID );

      // @todo ���ꂾ�ƃs���`BGM���ɂ�SEQ_BGM_BATTLESUPERIOR�Ɉڍs���Ȃ����A����ŗǂ��̂��낤���H
      if( ((wk->AITrainerMsgID==TRMSG_FIGHT_POKE_LAST) || (wk->AITrainerMsgID==TRMSG_FIGHT_POKE_LAST_HP_HALF))
      &&  (BTLV_GAUGE_GetPinchBGMFlag( BTLV_EFFECT_GetGaugeWork() ) == 0)
      ){
        //����A�ȕω��̓W�����[�_�[�Ǝl�V������
        u16 trType = BTL_MAIN_GetClientTrainerType( wk->mainModule, clientID );
        if( (BTL_CALC_IsTrtypeGymLeader(trType) || BTL_CALC_IsTrtypeBig4(trType))
        &&  (wk->fAITrainerBGMChanged == FALSE)
        ){
          //PMSND_PlayBGM( SEQ_BGM_BATTLESUPERIOR );
          //wk->fAITrainerBGMChanged = TRUE;
          PMSND_FadeOutBGM( 8 );
          (*seq) = 6;
          break;
        }
      }
      (*seq) = 7;
    }
    break;
  case 6:
    if( PMSND_CheckFadeOnBGM() == FALSE )
    {
      PMSND_PlayBGM( SEQ_BGM_BATTLESUPERIOR );
      wk->fAITrainerBGMChanged = TRUE;
      (*seq)++;
    }
    break;
  case 7:
    if( !BTLV_EFFECT_CheckExecute()
    &&  BTLV_WaitMsg(wk->viewCore)
    ){
      BTLV_EFFECT_Add( BTLEFF_TRAINER_OUT );
      (*seq)++;
    }
    break;
  case 8:
    if( !BTLV_EFFECT_CheckExecute() )
    {
      BTLV_EFFECT_DelTrainer( BTLV_MCSS_POS_TR_BB );
      (*seq) = 0;
    }
    break;
  }
  return FALSE;
}



//----------------------------------------------------------------------
/**
 *  �u���������v�I����̕���
 */
//----------------------------------------------------------------------
static BOOL selact_Fight( BTL_CLIENT* wk, int* seq )
{
  enum {
    SEQ_START = 0,
    SEQ_SELECT_WAZA_START,
    SEQ_SELECT_ROTATION_WAZA_START,
    SEQ_SELECT_WAZA_WAIT,
    SEQ_CHECK_WAZA_TARGET,
    SEQ_SELECT_TARGET_START,
    SEQ_SELECT_TARGET_WAIT,
    SEQ_WAIT_MSG,
  };

  switch( *seq ){
  case SEQ_START:
    if( is_waza_unselectable( wk, wk->procPoke, wk->procAction ) ){
      ClientSubProc_Set( wk, selact_CheckFinish );
    }
    else
    {
      #ifdef ROTATION_NEW_SYSTEM
      if( BTL_MAIN_GetRule(wk->mainModule) != BTL_RULE_ROTATION ){
        (*seq) = SEQ_SELECT_WAZA_START;
      }else{
        setupRotationParams( wk, &wk->rotWazaSelParam );
        (*seq) = SEQ_SELECT_ROTATION_WAZA_START;
      }
      #endif
    }

    break;

  case SEQ_SELECT_WAZA_START:
    BTLV_UI_SelectWaza_Start( wk->viewCore, wk->procPoke, wk->procAction );
    (*seq) = SEQ_SELECT_WAZA_WAIT;
    break;

  case SEQ_SELECT_ROTATION_WAZA_START:
    BTLV_UI_SelectRotationWaza_Start( wk->viewCore, &wk->rotWazaSelParam, wk->procAction );
    (*seq) = SEQ_SELECT_WAZA_WAIT;
    break;

  case SEQ_SELECT_WAZA_WAIT:
    if( CheckSelactForceQuit(wk, selact_ForceQuit) )
    {
      BTL_N_Printf( DBGSTR_CLIENT_ForceQuitByTimeLimit, wk->myID );
      BTLV_UI_SelectWaza_ForceQuit( wk->viewCore );
      return FALSE;
    }

    if( BTLV_UI_SelectWaza_Wait(wk->viewCore) )
    {
      BtlAction action = BTL_ACTION_GetAction( wk->procAction );

      if( action == BTL_ACTION_NULL ){
        ClientSubProc_Set( wk, selact_Root );
      }
      else if( action == BTL_ACTION_MOVE ){
        ClientSubProc_Set( wk, selact_CheckFinish );
      }
      else
      {
        const BTL_POKEPARAM* procPoke = wk->procPoke;
        BTL_ACTION_PARAM*    procAction = wk->procAction;
        BtlRotateDir dir = BTL_ROTATEDIR_NONE;

        #ifdef ROTATION_NEW_SYSTEM
        if( BTL_MAIN_GetRule(wk->mainModule) == BTL_RULE_ROTATION )
        {
          u8 pokeIdx;
          dir = wk->rotWazaSelParam.actRotation.rotation.dir;
          pokeIdx = BTL_MAINUTIL_GetRotateInPosIdx( dir );
          procPoke = wk->rotWazaSelParam.poke[ pokeIdx ].bpp;
          BTL_N_Printf( DBGSTR_CLIENT_ROT_Determine, dir, pokeIdx, BPP_GetID(procPoke) );
        }
        #endif

        if( is_unselectable_waza(wk, procPoke, procAction->fight.waza, &wk->strParam) )
        {
          selact_startMsg( wk, &wk->strParam );
          (*seq) = SEQ_WAIT_MSG;
        }
        else
        {
          // �A�N�V�����p�����[�^�Ƀ��[�e�������Z�b�g���鉼����
          #ifdef ROTATION_NEW_SYSTEM
          BtlRule rule = BTL_MAIN_GetRule( wk->mainModule );
          if( rule == BTL_RULE_ROTATION )
          {
            if( (dir != BTL_ROTATEDIR_NONE) && (dir != BTL_ROTATEDIR_STAY) )
            {
              if( !BPP_IsDead(procPoke) )
              {
                WazaID waza = BTL_ACTION_GetWazaID( &(wk->rotWazaSelParam.actWaza) );
                BTL_ACTION_SetRotation( wk->procAction++, dir );
                BTL_ACTION_SetFightParam( wk->procAction, waza, BTL_POS_NULL );
                wk->actionAddCount++;
              }
            }
          }
          #endif
          (*seq) = SEQ_CHECK_WAZA_TARGET;
        }
      }
    }
    break;

  case SEQ_CHECK_WAZA_TARGET:
    // �V���O���Ȃ�ΏۑI���Ȃ�
    {
      BtlRule rule = BTL_MAIN_GetRule( wk->mainModule );
      if( BTL_RULE_IsNeedSelectTarget(rule) ){
        (*seq) = SEQ_SELECT_TARGET_START;
      }else{
        ClientSubProc_Set( wk, selact_CheckFinish );
      }
    }
    break;

  case SEQ_SELECT_TARGET_START:
    BTLV_UI_SelectTarget_Start( wk->viewCore, wk->procPoke, wk->procAction );
    (*seq) = SEQ_SELECT_TARGET_WAIT;
    break;

  case SEQ_SELECT_TARGET_WAIT:
    if( CheckSelactForceQuit(wk, selact_ForceQuit) ){
      BTLV_UI_SelectTarget_ForceQuit( wk->viewCore );
      return FALSE;
    }

    {
      BtlvResult result = BTLV_UI_SelectTarget_Wait( wk->viewCore );
      if( result == BTLV_RESULT_DONE ){
        ClientSubProc_Set( wk, selact_CheckFinish );
      }else if( result == BTLV_RESULT_CANCEL ){
        (*seq) = SEQ_SELECT_WAZA_START;
      }
    }
    break;

  case SEQ_WAIT_MSG:
    if( BTLV_WaitMsg(wk->viewCore) ){
      BTLV_UI_Restart( wk->viewCore );
      ClientSubProc_Set( wk, selact_Root );
    }
    break;
  }

  return FALSE;
}
/**
 *  ���[�e�[�V�������U�I��p�p�����[�^������
 */
static void setupRotationParams( BTL_CLIENT* wk, BTLV_ROTATION_WAZASEL_PARAM* param )
{
  const BTL_POKEPARAM* bpp;
  u32 i, j;

  for(i=0; i<BTL_ROTATE_NUM; ++i)
  {
    bpp = BTL_PARTY_GetMemberDataConst( wk->myParty, i );
    param->poke[ i ].bpp = bpp;
    if( !BPP_IsDead(bpp) )
    {
      u32 wazaCnt = BPP_WAZA_GetCount( bpp );

      for(j=0; j<wazaCnt; ++j){
        param->poke[ i ].fWazaUsable[ j ] = !is_unselectable_waza( wk, bpp, BPP_WAZA_GetID(bpp,j), NULL );
      }
      while( j < wazaCnt ){
        param->poke[ i ].fWazaUsable[ j++ ] = FALSE;
      }
    }
    else
    {
      for(j=0; j<PTL_WAZA_MAX; ++j){
        param->poke[ i ].fWazaUsable[ j ] = FALSE;
      }
    }
  }
}

//----------------------------------------------------------------------
/**
 *  �u�|�P�����v�I����̂��ꂩ���|�P�����I��
 */
//----------------------------------------------------------------------
static BOOL selact_SelectChangePokemon( BTL_CLIENT* wk, int* seq )
{
  switch( *seq ){
  case 0:
    {
      u16 tokuseiID;
      u8  code, pokeID, fCantEsc = FALSE;
      code = isForbidEscape( wk, wk->procPoke, &pokeID, &tokuseiID );
      if( (code != BTL_CANTESC_NULL)
      ||  (BPP_GetID(wk->procPoke) == pokeID)
      ){
        fCantEsc = TRUE;
      }
//      setupPokeSelParam( wk, BPL_MODE_NORMAL, wk->numCoverPos, &wk->pokeSelParam, &wk->pokeSelResult  );
      BTLV_StartPokeSelect( wk->viewCore, &wk->pokeSelParam, wk->procPokeIdx, fCantEsc, &wk->pokeSelResult );
      (*seq)++;
    }
    break;

  case 1:
    // �������Ԃɂ�鋭���I���i�|�P�����I����ʂ̏I���ʒm�j
    if( CheckSelactForceQuit(wk, NULL) ){
      BTLV_ForceQuitPokeSelect( wk->viewCore );
      (*seq) = 2;
      return FALSE;
    }

    if( BTLV_WaitPokeSelect(wk->viewCore) )
    {
      if( !BTL_POKESELECT_IsCancel(&wk->pokeSelResult) )
      {
        u8 idx = BTL_POKESELECT_RESULT_GetLast( &wk->pokeSelResult );
        if( idx < BTL_PARTY_MEMBER_MAX ){
          BTL_N_Printf( DBGSTR_CLIENT_SelectChangePoke, idx);
          BTL_ACTION_SetChangeParam( &wk->actionParam[wk->procPokeIdx], wk->procPokeIdx, idx );
          ClientSubProc_Set( wk, selact_CheckFinish );
          break;
        }
      }

      BTL_N_Printf( DBGSTR_CLIENT_SelectChangePokeCancel );
      ClientSubProc_Set( wk, selact_Root );
    }
    break;

  // �������Ԃɂ�鋭���I���i�|�P�����I����ʂ̏I���҂���j
  case 2:
    if( BTLV_WaitPokeSelect(wk->viewCore) ){
      ClientSubProc_Set( wk, selact_ForceQuit );
    }
    break;
  }
  return FALSE;
}
//----------------------------------------------------------------------
/**
 *  �u�ǂ����v�I��
 */
//----------------------------------------------------------------------
static BOOL selact_Item( BTL_CLIENT* wk, int* seq )
{
  switch( *seq ){
  case 0:
    {
      u8 cost_sum = shooterCost_GetSum( wk );
      BTLV_ITEMSELECT_Start( wk->viewCore, wk->bagMode, wk->shooterEnergy, cost_sum );
      (*seq)++;
    }
    break;

  case 1:
    // �������Ԃɂ�鋭���I���ʒm
    if( CheckSelactForceQuit(wk, NULL) ){
      BTLV_ITEMSELECT_ForceQuit( wk->viewCore );
      (*seq) = 2;
      return FALSE;
    }

    if( BTLV_ITEMSELECT_Wait(wk->viewCore) )
    {
      u16 itemID, targetIdx;
      itemID = BTLV_ITEMSELECT_GetItemID( wk->viewCore );
      targetIdx = BTLV_ITEMSELECT_GetTargetIdx( wk->viewCore );
      if( (itemID != ITEM_DUMMY_DATA) && (targetIdx != BPL_SEL_EXIT) )
      {
        u8 cost = BTLV_ITEMSELECT_GetCost( wk->viewCore );
        u16 wazaIdx = BTLV_ITEMSELECT_GetWazaIdx( wk->viewCore );
        if( wk->shooterEnergy >= cost ){
          wk->shooterEnergy -= cost;
        }else{
          GF_ASSERT_MSG(0, "Energy=%d, item=%d, cost=%d\n", wk->shooterEnergy, itemID, cost );
          wk->shooterEnergy = 0;
          cost = wk->shooterEnergy;
        }
        shooterCost_Save( wk, wk->procPokeIdx, cost );
        BTL_ACTION_SetItemParam( wk->procAction, itemID, targetIdx, wazaIdx );
        ClientSubProc_Set( wk, selact_CheckFinish );
      }else{
//      (*seq)=SEQ_SELECT_ACTION;
        ClientSubProc_Set( wk, selact_Root );
      }
    }
    break;

  // �������Ԃɂ�鋭���I���҂���
  case 2:
    if( BTLV_ITEMSELECT_Wait(wk->viewCore) ){
      ClientSubProc_Set( wk, selact_ForceQuit );
    }
    break;

  }
  return FALSE;
}
//----------------------------------------------------------------------
/**
 *  �u�ɂ���v�I��
 */
//----------------------------------------------------------------------
static BOOL selact_Escape( BTL_CLIENT* wk, int* seq )
{
  switch( *seq ){
  case 0:
  {
    // �퓬���[�h���ɂ��֎~�`�F�b�N�i�g���[�i�[��Ȃǁj
    BtlEscapeMode esc = BTL_MAIN_GetEscapeMode( wk->mainModule );
    switch( esc ){
    case BTL_ESCAPE_MODE_OK:
    default:
      {
        BtlCantEscapeCode  code;
        u16 tokuseiID;
        u8 pokeID;
        code = isForbidEscape( wk, wk->procPoke, &pokeID, &tokuseiID );
        // �Ƃ������A���U���ʓ��ɂ��֎~�`�F�b�N
        if( code == BTL_CANTESC_NULL )
        {
          wk->returnDataPtr = &(wk->actionParam[0]);
          wk->returnDataSize = sizeof(wk->actionParam[0]) * wk->numCoverPos;
          ClientSubProc_Set( wk, selact_Finish );
        }
        else
        {
          if( tokuseiID != POKETOKUSEI_NULL ){
            BTLV_STRPARAM_Setup( &wk->strParam, BTL_STRTYPE_SET, BTL_STRID_SET_CantEscTok );
            BTLV_STRPARAM_AddArg( &wk->strParam, pokeID );
            BTLV_STRPARAM_AddArg( &wk->strParam, tokuseiID );
          }else{
            BTLV_STRPARAM_Setup( &wk->strParam, BTL_STRTYPE_STD, BTL_STRID_STD_EscapeProhibit );
          }
          selact_startMsg( wk, &wk->strParam );
          (*seq) = 1;
        }
      }
      break;

    case BTL_ESCAPE_MODE_NG:
      BTLV_STRPARAM_Setup( &wk->strParam, BTL_STRTYPE_STD, BTL_STRID_STD_EscapeCant );
      selact_startMsg( wk, &wk->strParam );
      (*seq) = 1;
      break;
    }
  }
  break;

  case 1:
    if( BTLV_WaitMsg(wk->viewCore) )
    {
      BTLV_STRPARAM_Setup( &wk->strParam, BTL_STRTYPE_STD, BTL_STRID_STD_SelectAction );
      BTLV_STRPARAM_AddArg( &wk->strParam, BPP_GetID(wk->procPoke) );
      BTLV_STRPARAM_SetWait( &wk->strParam, 0 );
      BTLV_PrintMsgAtOnce( wk->viewCore, &wk->strParam );
      (*seq)++;
    }
    break;

  case 2:
    if( BTLV_WaitMsg(wk->viewCore) ){
      ClientSubProc_Set( wk, selact_Root );
    }
    break;
  }
  return FALSE;
}
//----------------------------------------------------------------------
/**
 *  �S�A�N�V�����I���I���`�F�b�N
 */
//----------------------------------------------------------------------
static BOOL selact_CheckFinish( BTL_CLIENT* wk, int* seq )
{
  BTLV_UI_Restart( wk->viewCore );

  wk->procPokeIdx++;

  if( wk->procPokeIdx >= wk->numCoverPos )
  {
    u8 actionCnt = wk->numCoverPos + wk->actionAddCount;
    BTL_N_PrintfEx( PRINT_FLG, DBGSTR_CLIENT_SelectActionDone, wk->numCoverPos);

    wk->actionAddCount = 0;
    wk->returnDataPtr = &(wk->actionParam[0]);
    wk->returnDataSize = sizeof(wk->actionParam[0]) * actionCnt;
    ClientSubProc_Set( wk, selact_Finish );
  }
  else{
    ClientSubProc_Set( wk, selact_Root );
  }

  return FALSE;
}
//----------------------------------------------------------------------
/**
 *  �S�A�N�V�����I���I��->�T�[�o�ԐM��
 */
//----------------------------------------------------------------------
static BOOL selact_Finish( BTL_CLIENT* wk, int* seq )
{
  switch( *seq ){
  case 0:
    if( BTL_MAIN_GetCommMode(wk->mainModule) != BTL_COMM_NONE )
    {
      wk->commWaitInfoOn = TRUE;
      BTLV_StartCommWait( wk->viewCore );
      (*seq)++;
    }
    else
    {
      (*seq)+=2;
    }
    break;

  case 1:
    if( BTLV_WaitCommWait(wk->viewCore) ){
      (*seq)++;
    }
    break;

  case 2:
    if( !BTLV_EFFECT_CheckExecute() ){
      BTL_N_PrintfEx( PRINT_FLG, DBGSTR_CLIENT_ReturnSeqDone );

      CmdLimit_End( wk );
      (*seq)++;
      return TRUE;
    }
    break;
  default:
    return TRUE;
  }
  return FALSE;
}


/*============================================================================================*/
/* �V���[�^�[�R�X�g�v�Z�p���[�N                                                               */
/*============================================================================================*/
// ������
static void shooterCost_Init( BTL_CLIENT* wk )
{
  GFL_STD_MemClear( wk->shooterCost, sizeof(wk->shooterCost) );
}
// �g�p�A�C�e���R�X�g���L�^
static void shooterCost_Save( BTL_CLIENT* wk, u8 procPokeIdx, u8 cost )
{
  wk->shooterCost[ procPokeIdx ] = cost;
}
static u8 shooterCost_Get( BTL_CLIENT* wk, u8 procPokeIdx )
{
  return wk->shooterCost[ procPokeIdx ];
}
// �g�p�\��̍��v�l���擾
static u8 shooterCost_GetSum( BTL_CLIENT* wk )
{
  u32 i, sum = 0;
  for(i=0; i<wk->procPokeIdx; ++i){
    sum += wk->shooterCost[i];
  }
  return sum;
}

//----------------------------------------------------------------------------------
/**
 * �A�N�V�����I��s��Ԃ̃|�P�������ǂ�������
 * �s��Ԃł���΁A�A�N�V�������e��������������
 *
 * @param   wk
 * @param   bpp       ����Ώۃ|�P�����p�����[�^
 * @param   action    [out] �A�N�V�������e������
 *
 * @retval  BOOL      �s��ԂȂ�TRUE
 */
//----------------------------------------------------------------------------------
static BOOL is_action_unselectable( BTL_CLIENT* wk, const BTL_POKEPARAM* bpp, BTL_ACTION_PARAM* action )
{
  // ����ł���NULL�f�[�^��Ԃ�
  if( BPP_IsDead(bpp) )
  {
    if( action ){
      // ����ł��Ԃ̃A�N�V�������e�Z�b�g
      BTL_ACTION_SetNULL( action );
    }
    return TRUE;
  }
  // �A�N�V�����I���ł��Ȃ��i�U���̔����Ȃǁj�ꍇ�̓X�L�b�v
  if( BPP_CONTFLAG_Get(bpp, BPP_CONTFLG_CANT_ACTION) )
  {
    if( action ){
      BTL_ACTION_SetSkip( action );
    }
    return TRUE;
  }
  // ���U���b�N��ԁi�L�^���U�����̂܂܎g���j
  if( BPP_CheckSick(bpp, WAZASICK_WAZALOCK) )
  {
    BPP_SICK_CONT  cont = BPP_GetSickCont( bpp, WAZASICK_WAZALOCK );
    WazaID waza = BPP_SICKCONT_GetParam( cont );
    BtlPokePos pos = BPP_GetPrevTargetPos( bpp );

    GF_ASSERT(waza!=WAZANO_NULL);
    GF_ASSERT(waza<WAZANO_MAX);

    BTL_N_PrintfEx( PRINT_FLG, DBGSTR_CLIENT_WazaLockInfo, wk->myID, waza, pos);

    if( action ){
      BTL_ACTION_SetFightParam( action, waza, pos );
    }
    return TRUE;
  }

  // ���߃��b�N��ԁi�L�^���U�����̂܂܎g���j
  if( BPP_CheckSick(bpp, WAZASICK_TAMELOCK) )
  {
    WazaID waza = BPP_GetPrevWazaID( bpp );
    BtlPokePos pos = BPP_GetPrevTargetPos( bpp );
    u8 waza_idx = BPP_WAZA_SearchIdx( bpp, waza );

    // PP �[���ł����s
    if( action ){
      BTL_ACTION_SetFightParam( action, waza, pos );
    }
    return TRUE;
  }

  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * ���U�I��s��Ԃ̃|�P�������ǂ�������
 * ���U�I��s��Ԃł���΁A�A�N�V�������e��������������
 *
 * @param   wk
 * @param   bpp       ����Ώۃ|�P�����p�����[�^
 * @param   action    [out] �A�N�V�������e������
 *
 * @retval  BOOL      ���U�I��s��ԂȂ�TRUE
 */
//----------------------------------------------------------------------------------
static BOOL is_waza_unselectable( BTL_CLIENT* wk, const BTL_POKEPARAM* bpp, BTL_ACTION_PARAM* action )
{
  u32 wazaCount = BPP_WAZA_GetCount( bpp );
  u32 i;

  // ���傤�͂�ԁi�_���[�W���U�����I�ׂȂ��j�́A�I�ׂ郏�U�������Ă��Ȃ���΁u��邠�����v�Z�b�g�ŃX�L�b�v
  if( BPP_CheckSick(bpp, WAZASICK_TYOUHATSU) )
  {
    for(i=0; i<wazaCount; ++i){
      if( BPP_WAZA_GetPP(bpp, i) && WAZADATA_IsDamage(BPP_WAZA_GetID(bpp, i)) ){
        break;
      }
    }
    if( i == wazaCount ){
      setWaruagakiAction( action, wk, bpp );
      return TRUE;
    }
  }

  // �g���郏�U��PP���S��0�Ȃ�u��邠�����v
  for(i=0; i<wazaCount; ++i)
  {
    if( is_unselectable_waza( wk, bpp, BPP_WAZA_GetID(bpp,i), NULL ) ){
      continue;
    }
    if( BPP_WAZA_GetPP(bpp, i) != 0 ){
      break;
    }
  }
  if( i == wazaCount ){
    setWaruagakiAction( action, wk, bpp );
    return TRUE;
  }


  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * �u��邠�����v�p�A�N�V�����p�����[�^�Z�b�g
 *
 * @param   dst
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
static void setWaruagakiAction( BTL_ACTION_PARAM* dst, BTL_CLIENT* wk, const BTL_POKEPARAM* bpp )
{
  BtlPokePos targetPos = BTL_CALC_DecideWazaTargetAuto( wk->mainModule, wk->pokeCon, bpp, WAZANO_WARUAGAKI );
  BTL_ACTION_SetFightParam( dst, WAZANO_WARUAGAKI, targetPos );
}

//----------------------------------------------------------------------------------
/**
 * �g�p�ł��Ȃ����U���I�΂ꂽ���ǂ�������
 *
 * @param   wk
 * @param   bpp       �Ώۃ|�P����
 * @param   waza      ���U
 * @param   strParam  [out]�x�����b�Z�[�W�p�����[�^�o�͐�
 *
 * @retval  BOOL    �g�p�ł��Ȃ����U�̏ꍇTRUE
 */
//----------------------------------------------------------------------------------
static BOOL is_unselectable_waza( BTL_CLIENT* wk, const BTL_POKEPARAM* bpp, WazaID waza, BTLV_STRPARAM* strParam )
{
  // �������A�C�e�����ʁi�ŏ��Ɏg�������U�����I�ׂȂ��^�������}�W�b�N���[���񔭓����̂݁j
  if( !BTL_CALC_BITFLG_Check(wk->fieldEffectFlag, BTL_FLDEFF_MAGICROOM) )
  {
    if( BPP_CheckSick(bpp, WAZASICK_KODAWARI) )
    {
      BPP_SICK_CONT  cont = BPP_GetSickCont( bpp, WAZASICK_KODAWARI );
      WazaID  kodawariWaza = BPP_SICKCONT_GetParam( cont );

      // �������Ώۂ̃��U���o���Ă��āA����ȊO�̃��U���g�����Ƃ�����_��
      if( (BPP_WAZA_IsUsable(bpp, kodawariWaza))
      &&  (kodawariWaza != waza )
      ){
        GF_ASSERT(kodawariWaza != WAZANO_NULL);
        GF_ASSERT(kodawariWaza < WAZANO_MAX);
        if( strParam != NULL )
        {
          BTLV_STRPARAM_Setup( strParam, BTL_STRTYPE_STD, BTL_STRID_STD_KodawariLock );
          BTLV_STRPARAM_AddArg( strParam, BPP_GetItem(bpp) );
          BTLV_STRPARAM_AddArg( strParam, kodawariWaza );
        }
        return TRUE;
      }
    }
  }

  // �A���R�[�����ʁi�O��Ɠ������U�����o���Ȃ��j
  if( BPP_CheckSick(bpp, WAZASICK_ENCORE) )
  {
    WazaID prevWaza = BPP_GetPrevWazaID( bpp );
    if( waza != prevWaza ){
      if( strParam != NULL )
      {
        BTLV_STRPARAM_Setup( strParam, BTL_STRTYPE_STD, BTL_STRID_STD_WazaLock );
        BTLV_STRPARAM_AddArg( strParam, BPP_GetID(bpp) );
        BTLV_STRPARAM_AddArg( strParam, prevWaza );
      }
      return TRUE;
    }
  }

  // ���傤�͂�ԁi�_���[�W���U�����I�ׂȂ��j
  if( BPP_CheckSick(bpp, WAZASICK_TYOUHATSU) )
  {
    if( !WAZADATA_IsDamage(waza) )
    {
      if( strParam != NULL )
      {
        BTLV_STRPARAM_Setup( strParam, BTL_STRTYPE_SET, BTL_STRID_SET_ChouhatuWarn );
        BTLV_STRPARAM_AddArg( strParam, BPP_GetID(bpp) );
        BTLV_STRPARAM_AddArg( strParam, waza );
      }
      return TRUE;
    }
  }

  // ����������ԁi�Q�^�[�������ē������U��I�ׂȂ��j
  if( BPP_CheckSick(bpp, WAZASICK_ICHAMON) )
  {
    if( BPP_GetPrevOrgWazaID(bpp) == waza )
    {
      if( strParam != NULL )
      {
        BTLV_STRPARAM_Setup( strParam, BTL_STRTYPE_SET, BTL_STRID_SET_IchamonWarn );
        BTLV_STRPARAM_AddArg( strParam, BPP_GetID(bpp) );
        BTLV_STRPARAM_AddArg( strParam, waza );
      }
      return TRUE;
    }
  }

  // ���Ȃ��΂��ԁi���Ȃ��΂蒼�O�ɏo���Ă������U��I�ׂȂ��j
  if( BPP_CheckSick(bpp, WAZASICK_KANASIBARI) )
  {
    u16 kanasibariWaza = BPP_GetSickParam( bpp, WAZASICK_KANASIBARI );
    BTL_Printf("���Ȃ��΂�`�F�b�N waza=%d\n", kanasibariWaza);
    if( (waza == kanasibariWaza) && (waza != WAZANO_WARUAGAKI) )
    {
      if( strParam != NULL )
      {
        BTLV_STRPARAM_Setup( strParam, BTL_STRTYPE_SET, BTL_STRID_SET_KanasibariWarn );
        BTLV_STRPARAM_AddArg( strParam, BPP_GetID(bpp) );
        BTLV_STRPARAM_AddArg( strParam, waza );
      }
      return TRUE;
    }
  }

// �ӂ�����`�F�b�N�i�ӂ�������������|�P�������Ă郏�U���o���Ȃ��j
  if( BTL_FIELD_CheckEffect(BTL_FLDEFF_FUIN) )
  {
    u8 fuinPokeID = BTL_FIELD_GetDependPokeID( BTL_FLDEFF_FUIN );
    u8 myPokeID = BPP_GetID( bpp );
    BTL_Printf("�ӂ�������{��\n");
    if( !BTL_MAINUTIL_IsFriendPokeID(myPokeID, fuinPokeID) )
    {
      const BTL_POKEPARAM* fuinPoke = BTL_POKECON_GetPokeParam( wk->pokeCon, fuinPokeID );
      BTL_Printf("�����͂ӂ�����|�P���Ⴀ��܂���\n");
      if( BPP_WAZA_SearchIdx(fuinPoke, waza) != PTL_WAZA_MAX )
      {
        BTL_Printf("���̃��U(%d)�͂ӂ����񂳂�Ă���\n", waza);
        if( strParam != NULL )
        {
          BTLV_STRPARAM_Setup( strParam, BTL_STRTYPE_SET, BTL_STRID_SET_FuuinWarn );
          BTLV_STRPARAM_AddArg( strParam, myPokeID );
          BTLV_STRPARAM_AddArg( strParam, waza );
        }
        return TRUE;
      }
    }
  }

  // ���イ��傭�`�F�b�N
  if( BTL_FIELD_CheckEffect(BTL_FLDEFF_JURYOKU) )
  {
    if( WAZADATA_GetFlag(waza, WAZAFLAG_Flying) )
    {
      if( strParam != NULL )
      {
        BTLV_STRPARAM_Setup( strParam, BTL_STRTYPE_SET, BTL_STRID_SET_JyuryokuWazaFail );
        BTLV_STRPARAM_AddArg( strParam, BPP_GetID(bpp) );
        BTLV_STRPARAM_AddArg( strParam, waza );
      }
      return TRUE;
    }
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * �I���ł��郏�U��Index��S�ė�
 *
 * @param   wk
 * @param   bpp
 * @param   dst   [out] u8 x 4 �̔z��B�I���ł��郏�U�Ȃ�Ή�Index��1, ����ȊO��0�ɂȂ�
 *
 * @retval  u8    �I���ł���ŏ��̃��UIndex�B�I���ł��郏�U���Ȃ��ꍇ�APTL_WAZA_MAX
 */
//----------------------------------------------------------------------------------
static u8 StoreSelectableWazaFlag( BTL_CLIENT* wk, const BTL_POKEPARAM* bpp, u8* dst )
{
  u8 wazaCount = BPP_WAZA_GetCount( bpp );
  u8 firstIdx = PTL_WAZA_MAX;
  u8 i;

  for(i=0; i<wazaCount; ++i )
  {
    if( BPP_WAZA_GetPP(bpp, i) )
    {
      WazaID  waza = BPP_WAZA_GetID( bpp, i );
      dst[i] = !is_unselectable_waza( wk, bpp, waza, NULL );
      if( (firstIdx==PTL_WAZA_MAX) && (dst[i]) ){
        firstIdx = i;
      }
    }
    else{
      dst[i] = 0;
    }
  }
  for( ; i<PTL_WAZA_MAX; ++i ){
    dst[i] = 0;
  }
  return firstIdx;
}


//----------------------------------------------------------------------------------
/**
 * �����E�����̋֎~�`�F�b�N
 *
 * @param   wk
 * @param   procPoke
 * @param   pokeID      [out] �����E�����ł��Ȃ������ƂȂ�|�PID�i�G�E�����Ƃ����蓾��j
 * @param   tokuseiID   [out] �����E�����ł��Ȃ��������Ƃ������̏ꍇ�A���̂Ƃ�����ID�i����ȊOPOKETOKUSEI_NULL�j
 *
 * @retval  BtlCantEscapeCode
 */
//----------------------------------------------------------------------------------
static BtlCantEscapeCode isForbidEscape( BTL_CLIENT* wk, const BTL_POKEPARAM* procPoke, u8* pokeID, u16* tokuseiID )
{
  BtlExPos    exPos;
  u16 checkTokusei;
  BtlPokePos  myPos;
  u8 procPokeID;
  u8 checkPokeCnt, checkPokeID, i;
  const BTL_POKEPARAM* bpp;

  u8 pokeIDAry[ BTL_POS_MAX ];

  *tokuseiID = POKETOKUSEI_NULL;
  *pokeID = BTL_POKEID_NULL;

  #ifdef PM_DEBUG
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L ){
    return BTL_CANTESC_NULL;
  }
  #endif

  if( BPP_GetItem(procPoke) == ITEM_KIREINANUKEGARA ){
    return BTL_CANTESC_NULL;
  }

  procPokeID = BPP_GetID( procPoke );
  myPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, procPokeID );
  exPos = EXPOS_MAKE( BTL_EXPOS_AREA_ENEMY, myPos );

  checkPokeCnt = BTL_MAIN_ExpandExistPokeID( wk->mainModule, wk->pokeCon, exPos, pokeIDAry );
  for(i=0; i<checkPokeCnt; ++i)
  {
    // ���葤�Ƃ������u�����ӂ݁v�`�F�b�N
    bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, pokeIDAry[i] );
    checkTokusei = BPP_GetValue( bpp, BPP_TOKUSEI_EFFECTIVE );
    checkPokeID = BPP_GetID( bpp );
    if( checkTokusei == POKETOKUSEI_KAGEFUMI )
    {
      BTL_N_Printf( DBGSTR_CLIENT_ForbidEscape_Kagefumi_Chk, checkPokeID );
      if( checkForbitEscapeEffective_Kagefumi(wk, procPoke) ){
        BTL_N_Printf( DBGSTR_CLIENT_ForbidEscape_Kagefumi_Enable );
        *pokeID = BPP_GetID( bpp );
        *tokuseiID = checkTokusei;
        return BTL_CANTESC_KAGEFUMI;
      }
    }
    // ���葤�Ƃ������u���肶�����v�`�F�b�N
    if( checkTokusei == POKETOKUSEI_ARIJIGOKU )
    {
      BTL_N_Printf( DBGSTR_CLIENT_ForbidEscape_Arijigoku_Chk, checkPokeID );
      if( checkForbitEscapeEffective_Arijigoku(wk, procPoke) ){
        BTL_N_Printf( DBGSTR_CLIENT_ForbidEscape_Arijigoku_Enable );
        *pokeID = BPP_GetID( bpp );
        *tokuseiID = checkTokusei;
        return BTL_CANTESC_KAGEFUMI;
      }
    }
    // ���葤�Ƃ������u����傭�v�`�F�b�N
    if( checkTokusei == POKETOKUSEI_JIRYOKU )
    {
      BTL_N_Printf( DBGSTR_CLIENT_ForbidEscape_Jiryoku_Chk, checkPokeID );
      if( checkForbitEscapeEffective_Jiryoku(wk, procPoke) ){
        BTL_N_Printf( DBGSTR_CLIENT_ForbidEscape_Jiryoku_Enable );
        *pokeID = BPP_GetID( bpp );
        *tokuseiID = checkTokusei;
        return BTL_CANTESC_KAGEFUMI;
      }
    }
  }

  // ������̃|�P������Ԉُ�ɂ�铦���E�����֎~�`�F�b�N
  if( BPP_CheckSick( procPoke, WAZASICK_TOOSENBOU )
  ||  BPP_CheckSick( procPoke, WAZASICK_BIND )
  ||  BPP_CheckSick( procPoke, WAZASICK_NEWOHARU )
  ){
     *pokeID = BPP_GetID( procPoke );
     return BTL_CANTESC_TOOSENBOU;
  }

  return BTL_CANTESC_NULL;
}

/**
 *  �����E�����֎~�`�F�b�N�F�����Ɂu�����ӂ݁v��������
 */
static BOOL checkForbitEscapeEffective_Kagefumi( BTL_CLIENT* wk, const BTL_POKEPARAM* procPoke )
{
  if( BPP_GetValue(procPoke, BPP_TOKUSEI_EFFECTIVE) == POKETOKUSEI_KAGEFUMI ){
    return FALSE;
  }
  return TRUE;
}
/**
 *  �����E�����֎~�`�F�b�N�F�����Ɂu���肶�����v��������
 */
static BOOL checkForbitEscapeEffective_Arijigoku( BTL_CLIENT* wk, const BTL_POKEPARAM* procPoke )
{
  if( BTL_FIELD_CheckEffect(BTL_FLDEFF_JURYOKU) ){
    return TRUE;
  }
  if( BPP_CheckSick(procPoke, WAZASICK_FLYING_CANCEL) ){
    return TRUE;
  }
  if( BPP_GetItem(procPoke) == ITEM_KUROITEKKYUU ){
    return TRUE;
  }

  if( BPP_GetValue(procPoke, BPP_TOKUSEI_EFFECTIVE) == POKETOKUSEI_FUYUU ){
    return FALSE;
  }
  if( BPP_IsMatchType(procPoke, POKETYPE_HIKOU) ){
    return FALSE;
  }
  if( BPP_CheckSick(procPoke, WAZASICK_FLYING) ){
    return FALSE;
  }
  if( BPP_GetItem(procPoke) == ITEM_HUUSEN ){
    return TRUE;
  }

  return TRUE;
}

/**
 *  �����E�����֎~�`�F�b�N�F�����Ɂu����傭�v��������
 */
static BOOL checkForbitEscapeEffective_Jiryoku( BTL_CLIENT* wk, const BTL_POKEPARAM* procPoke )
{
  if( BPP_IsMatchType(procPoke, POKETYPE_HAGANE) ){
    return TRUE;
  }
  return FALSE;
}


//----------------------------------------------------------------------------------
/**
 * AI ����ւ�����p���[�N������
 *
 * @param   wk
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static void ChangeAI_InitWork( BTL_CLIENT* wk )
{
  u32 i;
  for(i=0; i<NELEMS(wk->AIChangeIndex); ++i){
    wk->AIChangeIndex[ i ] = -1;
  }
}
static BOOL ChangeAI_CheckReserve( BTL_CLIENT* wk, u8 index )
{
  u32 i;

  for(i=0; i<NELEMS(wk->AIChangeIndex); ++i)
  {
    if( wk->AIChangeIndex[i] == index ){
      return TRUE;
    }
  }

  return FALSE;
}
static void ChangeAI_SetReserve( BTL_CLIENT* wk, u8 outPokeIdx, u8 inPokeIdx )
{
  if( outPokeIdx < NELEMS(wk->AIChangeIndex) )
  {
    wk->AIChangeIndex[ outPokeIdx ] = inPokeIdx;
  }
}

//----------------------------------------------------------------------------------
/**
 * AI ����ւ�����
 *
 * @param   wk
 * @param   procPoke        �s���`�F�b�N���|�P�����p�����[�^
 * @param   procPokeIndex   �s���`�F�b�N���|�P�����̃p�[�e�B��Index
 * @param   targetIndex     [out]����ւ���ꍇ�A�Ώۃ|�P�����̃p�[�e�B��Index
 *
 * @retval  BOOL    ����ւ���ꍇTRUE
 */
//----------------------------------------------------------------------------------
static BOOL ChangeAI_Root( BTL_CLIENT* wk, const BTL_POKEPARAM* procPoke, u8 procPokeIndex, u8* targetIndex )
{
  BOOL fChange = FALSE;
  u8   changeIndex = BTL_PARTY_MEMBER_MAX;
  BTL_POKEPARAM* targetPoke;

  // �����E�����֎~��Ԃ̃`�F�b�N
  {
    u8  prohibit_pokeID;
    u16 prohibit_tokID;

    if( isForbidEscape(wk, procPoke, &prohibit_pokeID, &prohibit_tokID) != BTL_CANTESC_NULL ){
      return FALSE;
    }
  }

  // ���������ł��郁���o�[�����Ȃ�
  if( BTL_PARTY_GetAliveMemberCount(wk->myParty) <= wk->numCoverPos ){
    return FALSE;
  }

  do {

    if( ChangeAI_CheckHorobi(wk, procPoke) ){  // �ق�т̂����`�F�b�N
      BTL_N_Printf( DBGSTR_CLIENT_CHGAI_HOROBI );
      fChange = TRUE;
      break;
    }

    {
      BtlPokePos  basePos = BTL_MAIN_PokeIDtoPokePos(wk->mainModule, wk->pokeCon, BPP_GetID(procPoke) );
      targetPoke = ChangeAI_DecideTarget( wk, basePos );
      if( targetPoke == NULL ){
        break;
      }
    }

    if( ChangeAI_CheckFusigiNaMamori(wk, procPoke, targetPoke) ){  // �ӂ����Ȃ܂���`�F�b�N
      BTL_N_Printf( DBGSTR_CLIENT_CHGAI_FusigiNaMamori );
      fChange = TRUE;
      break;
    }

    if( ChangeAI_CheckNoEffectWaza(wk, procPoke, targetPoke) ){  // �����������U�`�F�b�N
      BTL_N_Printf( DBGSTR_CLIENT_CHGAI_NoEffWaza );
      fChange = TRUE;
      break;
    }

    if( ChangeAI_CheckKodawari(wk, procPoke, targetPoke) ){  // ������胏�U�`�F�b�N
      BTL_N_Printf( DBGSTR_CLIENT_CHGAI_Kodawari );
      fChange = TRUE;
      break;
    }

    if( ChangeAI_CheckUkeTokusei(wk, procPoke, targetPoke, &changeIndex) ){ // �󂯂Ƃ������`�F�b�N
      BTL_N_Printf( DBGSTR_CLIENT_CHGAI_UkeTok );
      fChange = TRUE;
      break;
    }

    if( AI_ChangeProcSub_SizenKaifuku(wk, procPoke, &changeIndex) ){ // �����񂩂��ӂ��`�F�b�N
      BTL_N_Printf( DBGSTR_CLIENT_CHGAI_SizenKaifuku );
      fChange = TRUE;
      break;
    }

    if( AI_ChangeProcSub_WazaAff(wk, procPoke, targetPoke, &changeIndex) ){ // ���U�����`�F�b�N
      BTL_N_Printf( DBGSTR_CLIENT_CHGAI_WazaEff );
      fChange = TRUE;
      break;
    }

    return FALSE;

  }while(0);

  if( !fChange ){ return FALSE; }

  if( changeIndex == BTL_PARTY_MEMBER_MAX )
  {
    u8 puttableList[ BTL_PARTY_MEMBER_MAX ];
    u32 cnt, i;

    cnt = calcPuttablePokemons( wk, puttableList );

    sortPuttablePokemonList( wk, puttableList, cnt, targetPoke );
    for(i=0; i<cnt; ++i)
    {
      if( !ChangeAI_CheckReserve(wk, puttableList[i]) )
      {
        changeIndex = puttableList[ i ];
        break;
      }
    }
  }

  if( changeIndex == BTL_PARTY_MEMBER_MAX ){
    return FALSE;
  }

  ChangeAI_SetReserve( wk, procPokeIndex, changeIndex );
  *targetIndex = changeIndex;
  return TRUE;
}

/**
 *  �����`�F�b�N���ɍl������u����v�|�P�����̌���
 */
static BTL_POKEPARAM* ChangeAI_DecideTarget( BTL_CLIENT* wk, BtlPokePos basePos )
{
  BtlExPos    exPos = EXPOS_MAKE( BTL_EXPOS_AREA_ENEMY, basePos );
  u8 aryPokeID[ BTL_POSIDX_MAX ];
  u8 cnt, i;

  cnt = BTL_MAIN_ExpandExistPokeID( wk->mainModule, wk->pokeCon, exPos, aryPokeID );
  if( cnt )
  {
    u8 idx = GFL_STD_Rand( &wk->AIRandContext, cnt );
    return BTL_POKECON_GetPokeParam( wk->pokeCon, aryPokeID[idx] );
  }

  return NULL;
}
/**
 *  �����`�F�b�N�F�ق�т̂���
 */
static BOOL ChangeAI_CheckHorobi( BTL_CLIENT* wk, const BTL_POKEPARAM* procPoke )
{
  if( BPP_CheckSick(procPoke, WAZASICK_HOROBINOUTA) )
  {
    BPP_SICK_CONT  cont = BPP_GetSickCont( procPoke, WAZASICK_HOROBINOUTA );
    u8 turnMax = BPP_SICCONT_GetTurnMax( cont );
    u8 turnNow = BPP_GetSickTurnCount( procPoke, WAZASICK_HOROBINOUTA );
    if( (turnNow+1) == turnMax ){
      return TRUE;
    }
  }
  return FALSE;
}
/**
 *  �����`�F�b�N�F�ӂ����Ȃ܂���
 */
static BOOL ChangeAI_CheckFusigiNaMamori( BTL_CLIENT* wk, const BTL_POKEPARAM* procPoke, const BTL_POKEPARAM* targetPoke )
{
  if( BTL_MAIN_GetRule(wk->mainModule) != BTL_RULE_SINGLE )
  {
    return FALSE;
  }

  if( BPP_GetValue(targetPoke, BPP_TOKUSEI_EFFECTIVE) == POKETOKUSEI_FUSIGINAMAMORI )
  {
    // �����͌��ʃo�c�O���������ĂȂ����T���͎����Ă���
    if( !AI_ChangeProcSub_CheckWazaAff(wk, procPoke, targetPoke, BTL_TYPEAFF_200) )
    {
      if( AI_ChangeProcSub_HikaeWazaAff(wk, targetPoke, BTL_TYPEAFF_200) )
      {
        if( GFL_STD_Rand(&wk->AIRandContext, 3) < 2 ){
          return TRUE;
        }
      }
    }
  }

  return FALSE;
}
/**
 *  �����`�F�b�N�F���ʂȂ����U���������ĂȂ�
 */
static BOOL ChangeAI_CheckNoEffectWaza( BTL_CLIENT* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender )
{
  u32 i;
  WazaID waza;
  PokeType wazaType;
  PokeTypePair targetPokeType = BPP_GetPokeType( defender );
  u32 waza_cnt = BPP_WAZA_GetCount( attacker );

  u32 dmgWazaCnt = 0;
  u32 noEffCnt = 0;

  for(i=0; i<waza_cnt; ++i)
  {
    waza = BPP_WAZA_GetID( attacker, i );
    if( WAZADATA_IsDamage(waza) )
    {
      // ���ʂ̂���_���[�W���U�������Ă�����FALSE
      wazaType = WAZADATA_GetType( waza );
      if( BTL_CALC_TypeAffPair(wazaType, targetPokeType) != BTL_TYPEAFF_0 ){
        return FALSE;
      }
      ++dmgWazaCnt;
    }
  }

  // ���ʂȂ��_���[�W���U�̂݁A�Q�ȏ㏊�L���Ă�����
  if( dmgWazaCnt >= 2 )
  {
    if( AI_ChangeProcSub_HikaeWazaAff(wk, defender, BTL_TYPEAFF_200) )
    {
      if( GFL_STD_Rand(&wk->AIRandContext, 3) < 2 ){
        return TRUE;
      }
      return FALSE;
    }
    if( AI_ChangeProcSub_HikaeWazaAff(wk, defender, BTL_TYPEAFF_100) )
    {
      if( GFL_STD_Rand(&wk->AIRandContext, 2) < 1 ){
        return TRUE;
      }
      return FALSE;
    }
  }
  return FALSE;
}
/**
 *  �����`�F�b�N�F���������ʔ������Č��ʂȂ�
 */
static BOOL ChangeAI_CheckKodawari( BTL_CLIENT* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender )
{
  if( BPP_CheckSick(attacker, WAZASICK_KODAWARI) )
  {
    BPP_SICK_CONT cont = BPP_GetSickCont( attacker, WAZASICK_KODAWARI );
    WazaID  kodawariWaza = BPP_SICKCONT_GetParam( cont );
    if( kodawariWaza != WAZANO_NULL )
    {
      BOOL fDamageWaza = WAZADATA_IsDamage( kodawariWaza );
      PokeType  wazaType = WAZADATA_GetType( kodawariWaza );
      PokeTypePair targetType = BPP_GetPokeType( defender );

      if( (BTL_CALC_TypeAffPair(wazaType, targetType)  == BTL_TYPEAFF_0)
      &&  (fDamageWaza)
      ){
        if( AI_ChangeProcSub_HikaeWazaAff(wk, defender, BTL_TYPEAFF_200) )
        {
          return ( GFL_STD_Rand(&wk->AIRandContext, 3) < 2 );
        }
        if( AI_ChangeProcSub_HikaeWazaAff(wk, defender, BTL_TYPEAFF_100) )
        {
          return ( GFL_STD_Rand(&wk->AIRandContext, 2) < 1 );
        }
      }
      if( !fDamageWaza )
      {
        return ( GFL_STD_Rand(&wk->AIRandContext, 2) < 1 );
      }
    }
  }
  return FALSE;
}
/**
 *  �����`�F�b�N�F�O�^�[���Ɏ󂯂����U�̎󂯓����������Ă���T���|�P�`�F�b�N
 */
static BOOL ChangeAI_CheckUkeTokusei( BTL_CLIENT* wk, const BTL_POKEPARAM* procPoke, const BTL_POKEPARAM* targetPoke, u8* changeIndex )
{
  enum {
    CHECK_TOK_MAX = 4,  // �`�F�b�N����Ƃ������ő吔
  };

  static const struct {
    u8   wazaType;
    u16  tokusei[ CHECK_TOK_MAX ];
  }CheckTable[] = {
    { POKETYPE_MIZU,  { POKETOKUSEI_TYOSUI,   POKETOKUSEI_YOBIMIZU,   POKETOKUSEI_KANSOUHADA, POKETOKUSEI_NULL } },
    { POKETYPE_DENKI, { POKETOKUSEI_TIKUDEN,  POKETOKUSEI_DENKIENJIN, POKETOKUSEI_HIRAISIN,   POKETOKUSEI_NULL } },
    { POKETYPE_KUSA,  { POKETOKUSEI_SINRYOKU, POKETOKUSEI_NULL,       POKETOKUSEI_NULL,       POKETOKUSEI_NULL } },
    { POKETYPE_HONOO, { POKETOKUSEI_MORAIBI,  POKETOKUSEI_NULL,       POKETOKUSEI_NULL,       POKETOKUSEI_NULL } },
 };

  BPP_WAZADMG_REC rec;
  u32 i;
  u8 idx = 0;

  // ���ʃo�c�O���������Ă�ꍇ�A1/3�̊m���Ń`�F�b�N���s��Ȃ�
  if( AI_ChangeProcSub_CheckWazaAff(wk, procPoke,  targetPoke, BTL_TYPEAFF_200) )
  {
    if( GFL_STD_Rand(&wk->AIRandContext, 3) == 0 ){
      return FALSE;
    }
  }


  while( BPP_WAZADMGREC_Get(procPoke, 1, idx++, &rec) )
  {
    BTL_N_Printf( DBGSTR_CLIENT_CHGAI_UkeTokStart, rec.wazaType );

    for(i=0; i<NELEMS(CheckTable); ++i)
    {
      if( CheckTable[i].wazaType == rec.wazaType )
      {
        u32 t;
        u8  hikae_idx;
        for(t=0; t<CHECK_TOK_MAX; ++t)
        {
          if( CheckTable[i].tokusei[t] == POKETOKUSEI_NULL ){ break; }
          if( AI_ChangeProcSub_CheckTokHikae(wk, CheckTable[i].tokusei[t], &hikae_idx) )
          {
            BTL_N_Printf( DBGSTR_CLIENT_CHGAI_UkeTokFind, hikae_idx, rec.wazaType );

            if( GFL_STD_Rand(&wk->AIRandContext, 2) == 0 )
            {
              BTL_N_Printf( DBGSTR_CLIENT_CHGAI_UkeTokDecide );

              *changeIndex = hikae_idx;
              return TRUE;
            }
          }
        }
      }
    }
  }
  return FALSE;
}
/**
 *  �����`�F�b�N�F�����񂩂��ӂ��`�F�b�N
 */
static BOOL AI_ChangeProcSub_SizenKaifuku( BTL_CLIENT* wk, const BTL_POKEPARAM* procPoke, u8* changeIndex )
{
  if( (BPP_GetValue(procPoke, BPP_TOKUSEI_EFFECTIVE) == POKETOKUSEI_SIZENKAIFUKU)
  &&  (BPP_CheckSick(procPoke, WAZASICK_NEMURI) || BPP_CheckSick(procPoke, WAZASICK_KOORI) )
  &&  (BPP_GetHPRatio(procPoke) >= FX32_CONST(50) )
  ){
    BPP_WAZADMG_REC rec;
    if( BPP_WAZADMGREC_Get(procPoke, 1, 0, &rec) )
    {
      u8 index;
      if( AI_ChangeProcSub_HikaePokeAff(wk, rec.wazaType, BTL_TYPEAFF_50, &index) )
      {
        *changeIndex = index;
        return TRUE;
      }
    }
    else
    {
      return (GFL_STD_Rand(&wk->AIRandContext, 2) == 0);
    }
  }
  return FALSE;
}
/**
 *  �����`�F�b�N�F�ʏ탏�U�����`�F�b�N
 */
static BOOL AI_ChangeProcSub_WazaAff( BTL_CLIENT* wk, const BTL_POKEPARAM* procPoke, const BTL_POKEPARAM* targetPoke, u8* changeIndex )
{
  if( AI_ChangeProcSub_CheckWazaAff(wk, procPoke, targetPoke, BTL_TYPEAFF_200) )
  {
    if( GFL_STD_Rand(&wk->AIRandContext, 10) != 0 ){
      return FALSE;
    }
  }



  {
    u32 i, rankCnt = 0;
    BPP_WAZADMG_REC rec;


    for(i=BPP_RANKVALUE_START; i<=BPP_RANKVALUE_END; ++i)
    {
      rankCnt += BPP_GetValue( procPoke, i );
    }
    if( rankCnt >= 4 ){
      return FALSE;
    }

    if( BPP_WAZADMGREC_Get(procPoke, 1, 0, &rec) )
    {
      u32 i, hikaeStart, memberCnt;

      memberCnt = BTL_PARTY_GetMemberCount( wk->myParty );
      hikaeStart = BTL_RULE_GetNumFrontPos( BTL_MAIN_GetRule(wk->mainModule) );

      for(i=hikaeStart; i<memberCnt; ++i)
      {
        if( !ChangeAI_CheckReserve(wk, i) )
        {
          const BTL_POKEPARAM* bpp = BTL_PARTY_GetMemberDataConst( wk->myParty, i );
          if( AI_ChangeProcSub_CheckWazaAff(wk, bpp, targetPoke, BTL_TYPEAFF_200) )
          {
            BtlTypeAff  aff = BTL_CALC_TypeAffPair( rec.wazaType, BPP_GetPokeType(bpp) );
            if( aff == BTL_TYPEAFF_0 )
            {
              if( GFL_STD_Rand(&wk->AIRandContext, 2) == 0 )
              {
                *changeIndex = i;
                return TRUE;
              }
            }
            else if( aff < BTL_TYPEAFF_100 )
            {
              if( GFL_STD_Rand(&wk->AIRandContext, 3) == 0 )
              {
                *changeIndex = i;
                return TRUE;
              }
            }
          }
        }
      }
    }
  }
  return FALSE;
}


/**
 *  �����`�F�b�N�F�T���|�P�����̒��ɁA�w��Ƃ��������������|�P���������邩�`�F�b�N
 */
static BOOL AI_ChangeProcSub_CheckTokHikae( BTL_CLIENT* wk, u16 tokusei, u8* index )
{
  u32 i, hikaeStart, memberCnt;

  memberCnt = BTL_PARTY_GetMemberCount( wk->myParty );
  hikaeStart = BTL_RULE_HandPokeIndex( BTL_MAIN_GetRule(wk->mainModule), wk->numCoverPos );

  for(i=hikaeStart; i<memberCnt; ++i)
  {
    if( !ChangeAI_CheckReserve(wk, i) )
    {
      const BTL_POKEPARAM* bpp = BTL_PARTY_GetMemberDataConst( wk->myParty, i );
      if( !BPP_IsDead(bpp) && (BPP_GetValue(bpp, BPP_TOKUSEI_EFFECTIVE) == tokusei) )
      {
        *index = i;
        return TRUE;
      }
    }
  }
  return FALSE;
}

/**
 *  �����`�F�b�N�F�T���|�P�����̒��ɁA�Ώۃ|�P�����ɑ΂��đ���affMin�ȏ�̃_���[�W���U�����|�P���������邩
 */
static BOOL AI_ChangeProcSub_HikaeWazaAff( BTL_CLIENT* wk, const BTL_POKEPARAM* target, BtlTypeAff affMin )
{
  u32 i, hikaeStart, memberCnt;

  memberCnt = BTL_PARTY_GetMemberCount( wk->myParty );
  hikaeStart = BTL_RULE_GetNumFrontPos( BTL_MAIN_GetRule(wk->mainModule) );

  for(i=hikaeStart; i<memberCnt; ++i)
  {
    if( !ChangeAI_CheckReserve(wk, i) )
    {
      const BTL_POKEPARAM* bpp = BTL_PARTY_GetMemberDataConst( wk->myParty, i );
      if( AI_ChangeProcSub_CheckWazaAff(wk, bpp, target, affMin) ){
        return TRUE;
      }
    }
  }
  return FALSE;
}

/**
 *  �����`�F�b�N�F�T���|�P�����̒��ɁA�Ώۃ^�C�v�̃��U�ɑ΂���affMax�ȉ��̃^�C�v�ł���|�P���������邩
 */
static BOOL AI_ChangeProcSub_HikaePokeAff( BTL_CLIENT* wk, PokeType wazaType, BtlTypeAff affMax, u8* pokeIndex )
{
  u32 i, hikaeStart, memberCnt;

  memberCnt = BTL_PARTY_GetMemberCount( wk->myParty );
  hikaeStart = BTL_RULE_GetNumFrontPos( BTL_MAIN_GetRule(wk->mainModule) );

  for(i=hikaeStart; i<memberCnt; ++i)
  {
    if( !ChangeAI_CheckReserve(wk, i) )
    {
      const BTL_POKEPARAM* bpp = BTL_PARTY_GetMemberDataConst( wk->myParty, i );
      PokeTypePair  pokeType = BPP_GetPokeType( bpp );
      if( BTL_CALC_TypeAffPair(wazaType, pokeType) <= affMax )
      {
        *pokeIndex = i;
        return TRUE;
      }
    }
  }
  return FALSE;
}

/**
 *  �����`�F�b�N�Fattacker �� defender �ɑ΂����� affMin �ȏ�̃_���[�W���U�������Ă��邩����
 */
static BOOL AI_ChangeProcSub_CheckWazaAff( BTL_CLIENT* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, BtlTypeAff affMin )
{
  if( !BPP_IsDead(attacker) )
  {
    u32 i;
    WazaID waza;
    PokeType wazaType;
    PokeTypePair targetPokeType = BPP_GetPokeType( defender );
    u32 waza_cnt = BPP_WAZA_GetCount( attacker );

    for(i=0; i<waza_cnt; ++i)
    {
      waza = BPP_WAZA_GetID( attacker, i );
      if( BPP_WAZA_GetPP(attacker, i)
      &&  WAZADATA_IsDamage(waza)
      &&  !is_unselectable_waza(wk, attacker, waza, NULL)
      ){
        wazaType = WAZADATA_GetType( waza );
        if( BTL_CALC_TypeAffPair(wazaType, targetPokeType) >= affMin ){
          return TRUE;
        }
      }
    }
  }
  return FALSE;
}

static BOOL SubProc_AI_SelectAction( BTL_CLIENT* wk, int* seq )
{
  enum {
    SEQ_INIT = 0,
    SEQ_ROOT,
    SEQ_WAZA_AI,
    SEQ_INC,
  };

  GF_ASSERT(wk->AIHandle);

  switch( *seq ){
  case SEQ_INIT:
    ChangeAI_InitWork( wk );
    wk->procPokeIdx = 0;
    (*seq) = SEQ_ROOT;
    /* fallthru */
  case SEQ_ROOT:
    if( wk->procPokeIdx < wk->numCoverPos )
    {
      wk->procPoke = BTL_PARTY_GetMemberDataConst( wk->myParty, wk->procPokeIdx );
      wk->procAction = &wk->actionParam[wk->procPokeIdx];

      if( BPP_IsDead(wk->procPoke) )
      {
        BTL_ACTION_SetNULL( wk->procAction );
        (*seq) = SEQ_INC;
        break;
      }
      else
      {
        // �s���I���ł��Ȃ��`�F�b�N
        if( is_action_unselectable(wk, wk->procPoke,  wk->procAction) ){
          (*seq) = SEQ_INC;
          break;
        }

        // �A�C�e���g�p�`�F�b�N
        {
          u16 itemID = AIItem_CheckUse( wk, wk->procPoke, wk->myParty );
          if( itemID != ITEM_DUMMY_DATA ){
            BTL_ACTION_SetItemParam( wk->procAction, itemID, wk->procPokeIdx, 0 );
            (*seq) = SEQ_INC;
            break;
          }
        }

        // ����ւ��`�F�b�N
        if( BTL_MAIN_GetCompetitor(wk->mainModule) != BTL_COMPETITOR_WILD )
        {
          u8 targetIndex;
          if( ChangeAI_Root(wk, wk->procPoke, wk->procPokeIdx, &targetIndex) ){
            BTL_ACTION_SetChangeParam( wk->procAction, wk->procPokeIdx, targetIndex );
            (*seq) = SEQ_INC;
            break;
          }
        }

        // �A���R�[�����
        if( BPP_CheckSick(wk->procPoke, WAZASICK_ENCORE)
        ||  BPP_CheckSick(wk->procPoke, WAZASICK_WAZALOCK)
        ){
          WazaID waza = BPP_GetPrevWazaID( wk->procPoke );
          BtlPokePos pos = BPP_GetPrevTargetPos( wk->procPoke );
          BTL_ACTION_SetFightParam( wk->procAction, waza, pos );
          (*seq) = SEQ_INC;
          break;
        }

        // ���U�I��AI
        {
          u8 wazaCount, wazaIdx, targetPos;
          u8 usableWazaFlag[ PTL_WAZA_MAX ];

          wazaCount = BPP_WAZA_GetCount( wk->procPoke );

          if( StoreSelectableWazaFlag(wk, wk->procPoke, usableWazaFlag) != PTL_WAZA_MAX )
          {
            u8  mypos = BTL_MAIN_GetClientPokePos( wk->mainModule, wk->myID, wk->procPokeIdx );

            TR_AI_Start( wk->AIHandle, usableWazaFlag, mypos );
            (*seq) = SEQ_WAZA_AI;
            break;
          }
          else
          {
            setWaruagakiAction( wk->procAction, wk, wk->procPoke );
            (*seq) = SEQ_INC;
            break;
          }
        }
      }
    }
    break;

  case SEQ_WAZA_AI:
    if( !TR_AI_Main(wk->AIHandle) )
    {
      u8 wazaIdx = TR_AI_GetSelectWazaPos( wk->AIHandle );
      u8 targetPos = TR_AI_GetSelectWazaDir( wk->AIHandle );

      WazaID  waza = BPP_WAZA_GetID( wk->procPoke, wazaIdx );
      BTL_ACTION_SetFightParam( wk->procAction, waza, targetPos );
      (*seq) = SEQ_INC;
    }
    break;

  case SEQ_INC:
    wk->procPokeIdx++;
    if( wk->procPokeIdx >= wk->numCoverPos )
    {
      wk->returnDataPtr = &(wk->actionParam[0]);
      wk->returnDataSize = sizeof(wk->actionParam[0]) * wk->numCoverPos;
      return TRUE;
    }
    break;

  }
  return FALSE;
}
//--------------------------------------------------------------------------
/**
 * ���S���̑I���\�|�P�����i��ɏo�Ă���|�P�����ȊO�ŁA�����Ă���|�P�����j�̐�
 *
 * @param   wk
 * @param   list    [out] �I���\�|�P�����̃p�[�e�B���C���f�b�N�X���i�[����z��
 *
 * @retval  u8    �I���\�|�P�����̐�
 */
//--------------------------------------------------------------------------
static u8 calcPuttablePokemons( BTL_CLIENT* wk, u8* list )
{
  const BTL_POKEPARAM* bpp;
  u8 cnt, numMembers, numFront, i, j;

  numMembers = BTL_PARTY_GetMemberCount( wk->myParty );
  numFront = BTL_RULE_HandPokeIndex( BTL_MAIN_GetRule(wk->mainModule), wk->numCoverPos );

  for(i=numFront, cnt=0; i<numMembers; i++)
  {
    bpp = BTL_PARTY_GetMemberDataConst(wk->myParty, i);
    if( BPP_IsFightEnable(bpp) )
    {
      if( list ){
        list[cnt] = i;
      }
      ++cnt;
    }
  }
  return cnt;
}
//----------------------------------------------------------------------------------
/**
 * calcPuttablePokemons�Ő������� list ���o���ׂ����ɕ��בւ��i����Ƃ̑����ɂ��j
 *
 * @param   wk
 * @param   list
 * @param   numList
 *
 * @retval  u8
 */
//----------------------------------------------------------------------------------
static void sortPuttablePokemonList( BTL_CLIENT* wk, u8* list, u8 numPoke, const BTL_POKEPARAM* target )
{
  u16 affVal[ BTL_PARTY_MEMBER_MAX ];
  const BTL_POKEPARAM* bpp;
  PokeTypePair targetType;
  u8  i, j;

  targetType = BPP_GetPokeType( target );

  for(i=0; i<numPoke; ++i)
  {
    affVal[i] = 0;
    bpp = BTL_PARTY_GetMemberDataConst( wk->myParty, list[i] );
    if( !BPP_IsDead(bpp) )
    {
      u8 wazaCnt = BPP_WAZA_GetCount( bpp );
      for(j=0; j<wazaCnt; ++j)
      {
        if( BPP_WAZA_GetPP(bpp, j) )
        {
          WazaID waza = BPP_WAZA_GetID( bpp, j );
          if( WAZADATA_IsDamage(waza) )
          {
            PokeType wazaType = WAZADATA_GetType( waza );
            u16 pow = WAZADATA_GetPower( waza );
            if( pow < 10 ){ // 10�����͓���ݒ�Ƃ��đ��60���炢�Ɂc
              pow = 60;
            }
            switch( BTL_CALC_TypeAffPair(wazaType, targetType) ){
            case BTL_TYPEAFF_400:  pow *= 4; break;
            case BTL_TYPEAFF_200:  pow *= 2; break;
            case BTL_TYPEAFF_50:   pow /= 2; break;
            case BTL_TYPEAFF_25:   pow /= 4; break;
            case BTL_TYPEAFF_0:    pow = 0; break;
            }

            if( affVal[i] < pow ){
              affVal[ i ] = pow;
            }
          }
        }
      }
    }
  }

  BTL_N_Printf( DBGSTR_CLIENT_CHGAI_PreSortList );
  for(i=0; i<numPoke; ++i){
    BTL_N_PrintfSimple( DBGSTR_CLIENT_CHGAI_SortListVal, list[i], affVal[i]);
  }
  BTL_N_PrintfSimple( DBGSTR_LF );

  for(i=0; i<numPoke; ++i)
  {
    for(j=i+1; j<numPoke; ++j)
    {
      if( affVal[i] < affVal[j] )
      {
        u16 tmp = affVal[i];
        affVal[i] = affVal[j];
        affVal[j] = tmp;

        tmp = list[i];
        list[i] = list[j];
        list[j] = tmp;
      }
    }
  }


  BTL_N_Printf( DBGSTR_CLIENT_CHGAI_AfterSortList );
  for(i=0; i<numPoke; ++i){
    BTL_N_PrintfSimple( DBGSTR_CLIENT_CHGAI_SortListVal, list[i], affVal[i]);
  }
  BTL_N_PrintfSimple( DBGSTR_LF );

}

//--------------------------------------------------------------------------
/**
 * �|�P�����I����ʗp�p�����[�^�Z�b�g
 *
 * @param   wk          �N���C�A���g���W���[���n���h��
 * @param   mode
 * @param   numSelect
 * @param   param       [out] �I����ʗp�p�����[�^���i�[����
 * @param   result      [out] �I����ʌ��ʃf�[�^�i�����������j
 *
 */
//--------------------------------------------------------------------------
static void setupPokeSelParam( BTL_CLIENT* wk, u8 mode, u8 numSelect, BTL_POKESELECT_PARAM* param, BTL_POKESELECT_RESULT* result )
{
  BTL_POKESELECT_PARAM_Init( param, wk->myParty, numSelect, mode );
  BTL_POKESELECT_PARAM_SetProhibitFighting( param, wk->numCoverPos );
  BTL_POKESELECT_RESULT_Init( result, param );
}


// �|�P�����I����ʌ��� -> ����A�N�V�����p�����[�^�ɕϊ�
static void storePokeSelResult( BTL_CLIENT* wk, const BTL_POKESELECT_RESULT* res )
{
  // res->cnt=�I�����ꂽ�|�P������,  wk->myChangePokeCnt=�������S���������ւ����N�G�X�g�|�P������
  GF_ASSERT_MSG(res->cnt <= wk->myChangePokeCnt, "selCnt=%d, changePokeCnt=%d\n", res->cnt, wk->myChangePokeCnt);

  {
    u8 clientID, selIdx, outIdx, i;

    BTL_N_Printf( DBGSTR_CLIENT_PokeSelCnt, res->cnt);

    for(i=0; i<res->cnt; i++)
    {
      selIdx = res->selIdx[i];
      outIdx = res->outPokeIdx[i];
//      BTL_MAIN_BtlPosToClientID_and_PosIdx( wk->mainModule, wk->myChangePokePos[i], &clientID, &posIdx );
      BTL_N_Printf( DBGSTR_CLIENT_PokeChangeIdx, outIdx, selIdx );
      BTL_ACTION_SetChangeParam( &wk->actionParam[i], outIdx, selIdx );
    }
    wk->returnDataPtr = &(wk->actionParam[0]);
    wk->returnDataSize = sizeof(wk->actionParam[0]) * res->cnt;
  }
}

// �����I�����̃|�P�����I���������菈��
static void storePokeSelResult_ForceQuit( BTL_CLIENT* wk )
{
  const BTL_POKEPARAM* bpp;
  u8 clientID, posIdx, selIdx, memberMax, prevMemberIdx, selectCnt=0;
  u32 i, j;

  memberMax = BTL_PARTY_GetMemberCount( wk->myParty );
  prevMemberIdx = wk->numCoverPos;

  for(i=0; i<wk->myChangePokeCnt; ++i)
  {
    BTL_MAIN_BtlPosToClientID_and_PosIdx( wk->mainModule, wk->myChangePokePos[i], &clientID, &posIdx );
    for(j=prevMemberIdx; j<memberMax; ++j)
    {
      bpp = BTL_PARTY_GetMemberDataConst( wk->myParty, j );
      if( !BPP_IsDead(bpp) )
      {
        BTL_N_Printf( DBGSTR_CLIENT_ForcePokeChange, selectCnt+1, wk->myChangePokeCnt, posIdx, j );
        BTL_ACTION_SetChangeParam( &wk->actionParam[selectCnt++], posIdx, j );
        prevMemberIdx = j;
        break;
      }
    }
  }

  GF_ASSERT(selectCnt);

  wk->returnDataPtr = &(wk->actionParam[0]);
  wk->returnDataSize = sizeof(wk->actionParam[0]) * selectCnt;
}

//----------------------------------------------------------------------------------
/**
 * �T�[�o���瑗���Ă�����փ��N�G�X�g�|�P�����ʒu����A�����̒S������ʒu�݂̂𔲂��o���ăo�b�t�@�Ɋi�[
 *
 * @param   wk
 * @param   myCoverPos      [out] changePokePos���A�������S������ʒu���i�[����o�b�t�@
 *
 * @retval  u8    myCoverPos�z��Ɋi�[�����A�������S������ʒu�̐�.
 */
//----------------------------------------------------------------------------------
static u8 storeMyChangePokePos( BTL_CLIENT* wk, BtlPokePos* myCoverPos )
{
  u8 numChangePoke;
  const u8 *changePokePos;
  u32 i, cnt;

  numChangePoke = BTL_ADAPTER_GetRecvData( wk->adapter, (const void*)&changePokePos );
  BTL_N_Printf( DBGSTR_CLIENT_NumChangePokeBegin, numChangePoke);
  for(i=0; i<numChangePoke; ++i){
    BTL_N_PrintfSimple( DBGSTR_csv, changePokePos[i]);
  }
  BTL_N_PrintfSimple( DBGSTR_LF );

  for(i=0, cnt=0; i<numChangePoke; ++i)
  {
    if( BTL_MAIN_BtlPosToClientID(wk->mainModule, changePokePos[i]) == wk->myID ){
      myCoverPos[ cnt++ ] = changePokePos[ i ];
    }
  }
  BTL_N_Printf( DBGSTR_CLIENT_NumChangePokeResult, wk->myID, cnt);

  return cnt;
}

//----------------------------------------------------------------------------------
/**
 * �쐶�E�V���O����Ŏ����̃|�P�������|�ꂽ���́u������v�u����ւ��v�I������
 *
 * @param   wk
 * @param   seq
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL SubProc_UI_SelectChangeOrEscape( BTL_CLIENT* wk, int* seq )
{
  switch( *seq ){
  case 0:
    BTLV_STRPARAM_Setup( &wk->strParam,    BTL_STRTYPE_UI, BTLMSG_UI_SEL_NEXT_POKE );
    BTLV_STRPARAM_Setup( &wk->strParamSub, BTL_STRTYPE_UI, BTLMSG_UI_SEL_ESCAPE );
    BTLV_YESNO_Start( wk->viewCore, &wk->strParam, &wk->strParamSub );
    (*seq)++;
    break;

  case 1:
    {
      BtlYesNo result;
      if( BTLV_YESNO_Wait(wk->viewCore, &result) )
      {
        wk->change_escape_code = (result == BTL_YESNO_YES)? BTL_CLIENTASK_CHANGE : BTL_CLIENTASK_ESCAPE;
        wk->returnDataPtr = &(wk->change_escape_code);
        wk->returnDataSize = sizeof(wk->change_escape_code);
        return TRUE;
      }
    }
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * �y�|�P�����I���z�Ђ񎀁E�����Ԃ�Ŏ��ɏo���|�P������I��
 *
 * @param   wk
 * @param   seq
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL SubProc_UI_SelectPokemonForCover( BTL_CLIENT* wk, int* seq )
{
  return SelectPokemonUI_Core( wk, seq, BPL_MODE_CHG_DEAD );
}
//----------------------------------------------------------------------------------
/**
 * �y�|�P�����I���z�o�g���^�b�`�Ȃǐ����Ă���|�P�������^�[���r���ɓ���ւ���
 *
 * @param   wk
 * @param   seq
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL SubProc_UI_SelectPokemonForChange( BTL_CLIENT* wk, int* seq )
{
  return SelectPokemonUI_Core( wk, seq, BPL_MODE_NO_CANCEL );
}
//----------------------------------------------------------------------------------
/**
 * �y�|�P�����I���zAI
 *
 * @param   wk
 * @param   seq
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL SubProc_AI_SelectPokemon( BTL_CLIENT* wk, int* seq )
{
  wk->myChangePokeCnt = storeMyChangePokePos( wk, wk->myChangePokePos );
  if( wk->myChangePokeCnt )
  {
    u8 puttableList[ BTL_PARTY_MEMBER_MAX ];
    u8 numPuttable;

    numPuttable = calcPuttablePokemons( wk, puttableList );
    if( numPuttable )
    {
      u8 numSelect = wk->myChangePokeCnt;
      u8 posIdx, clientID, i;

      {
        const BTL_POKEPARAM* target = ChangeAI_DecideTarget( wk, wk->myChangePokePos[0] );
        if( target ){
          sortPuttablePokemonList( wk, puttableList, numPuttable, target );
        }
      }

      // �����Ă�|�P���o���Ȃ���΂����Ȃ����ɑ���Ȃ��ꍇ�A��������߂�
      if( numSelect > numPuttable )
      {
        #ifdef ROTATION_NEW_SYSTEM
        if( BTL_MAIN_GetRule(wk->mainModule) == BTL_RULE_ROTATION ){
          // �V���[�e�[�V�����̏ꍇ�A�����I�ɐ����Ă���|�P������O�ɏo����]���s��
          wk->returnDataSize = SetCoverRotateAction( wk, &(wk->actionParam[0]) );
          wk->returnDataPtr = &(wk->actionParam[0]);
          return TRUE;
        }
        #endif

        numSelect = numPuttable;
      }
      BTL_N_Printf( DBGSTR_CLIENT_AI_PutPokeStart, wk->myID, numSelect);
      for(i=0; i<numSelect; i++)
      {
        BTL_MAIN_BtlPosToClientID_and_PosIdx( wk->mainModule, wk->myChangePokePos[i], &clientID, &posIdx );
        BTL_ACTION_SetChangeParam( &wk->actionParam[i], posIdx, puttableList[i] );
        BTL_Printf( DBGSTR_CLIENT_AI_PutPokeDecide, puttableList[i] );
      }
      wk->returnDataPtr = &(wk->actionParam[0]);
      wk->returnDataSize = sizeof(wk->actionParam[0]) * numSelect;
    }
    else
    {
      #ifdef ROTATION_NEW_SYSTEM
      if( BTL_MAIN_GetRule(wk->mainModule) == BTL_RULE_ROTATION )
      {
        // �V���[�e�[�V�����̏ꍇ�A�����I�ɐ����Ă���|�P������O�ɏo����]���s��
        u32 sendDataSize = SetCoverRotateAction( wk, &(wk->actionParam[0]) );
        if( sendDataSize ){
          wk->returnDataSize = sendDataSize;
          wk->returnDataPtr = &(wk->actionParam[0]);
          return TRUE;
        }
      }
      #endif

      BTL_N_Printf( DBGSTR_CLIENT_NoMorePuttablePoke, wk->myID);
      BTL_ACTION_SetChangeDepleteParam( &wk->actionParam[0] );
      wk->returnDataPtr = &(wk->actionParam[0]);
      wk->returnDataSize = sizeof(wk->actionParam[0]);
    }
  }
  else
  {
     BTL_N_Printf( DBGSTR_CLIENT_NotDeadMember, wk->myID);
     BTL_ACTION_SetNULL( &wk->actionParam[0] );
     wk->returnDataPtr = &(wk->actionParam[0]);
     wk->returnDataSize = sizeof(wk->actionParam[0]);
  }
  return TRUE;
}
//----------------------------------------------------------------------------------
/**
 * �y�|�P�����I���z�^��Đ�
 *
 * @param   wk
 * @param   seq
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL SubProc_REC_SelectPokemon( BTL_CLIENT* wk, int* seq )
{
  return SubProc_REC_SelectAction( wk, seq );
}

/**
 *  UI�|�P��������ւ��R�A����
 */
static BOOL SelectPokemonUI_Core( BTL_CLIENT* wk, int* seq, u8 mode )
{
  switch( *seq ){
  case 0:
  {
    wk->myChangePokeCnt = storeMyChangePokePos( wk, wk->myChangePokePos );

    if( wk->myChangePokeCnt )
    {
      u8 numPuttable = calcPuttablePokemons( wk, NULL );
      if( numPuttable )
      {
        u8 numSelect = wk->myChangePokeCnt;
        u8 clientID, outPokeIdx;

        // �����Ă�|�P���o���Ȃ���΂����Ȃ����ɑ���Ȃ��ꍇ�A��������߂�
        if( numSelect > numPuttable )
        {
          numSelect = numPuttable;
        }

        BTL_MAIN_BtlPosToClientID_and_PosIdx( wk->mainModule, wk->myChangePokePos[0], &clientID, &outPokeIdx );

        BTL_N_Printf( DBGSTR_CLIENT_ChangePokeCmdInfo, wk->myID, numSelect, outPokeIdx, mode );
        setupPokeSelParam( wk, mode, numSelect, &wk->pokeSelParam, &wk->pokeSelResult );
        BTLV_StartPokeSelect( wk->viewCore, &wk->pokeSelParam, outPokeIdx, FALSE, &wk->pokeSelResult );

        CmdLimit_Start( wk );
        (*seq)++;
      }
      else
      {
        #ifdef ROTATION_NEW_SYSTEM
        if( BTL_MAIN_GetRule(wk->mainModule) == BTL_RULE_ROTATION )
        {
          // �V���[�e�[�V�����̏ꍇ�A�����I�ɐ����Ă���|�P������O�ɏo����]���s��
          u32 sendDataSize = SetCoverRotateAction( wk, &(wk->actionParam[0]) );
          if( sendDataSize ){
            wk->returnDataSize = sendDataSize;
            wk->returnDataPtr = &(wk->actionParam[0]);
            return TRUE;
          }
        }
        #endif

        BTL_N_Printf( DBGSTR_CLIENT_NoMorePuttablePoke, wk->myID);
        BTL_ACTION_SetChangeDepleteParam( &wk->actionParam[0] );
        wk->returnDataPtr = &(wk->actionParam[0]);
        wk->returnDataSize = sizeof(wk->actionParam[0]);
        return TRUE;
      }
    }
    // �����͋󂫂��o���Ă��Ȃ��̂ŉ����I�ԕK�v���Ȃ�
    else
    {
      BTL_N_Printf( DBGSTR_CLIENT_NotDeadMember, wk->myID);
      BTL_ACTION_SetNULL( &wk->actionParam[0] );
      wk->returnDataPtr = &(wk->actionParam[0]);
      wk->returnDataSize = sizeof(wk->actionParam[0]);
      return TRUE;
    }
  }
  break;

  case 1:
    if( CmdLimit_CheckOver(wk) ){
      BTLV_ForceQuitPokeSelect( wk->viewCore );
      (*seq) = 2;
      break;
    }

    if( BTLV_WaitPokeSelect(wk->viewCore) )
    {
      storePokeSelResult( wk, &wk->pokeSelResult );
      (*seq) = 3;
    }
    break;

  // �R�}���h�������Ԃɂ�鋭���I������
  case 2:
    if( BTLV_WaitPokeSelect(wk->viewCore) )
    {
      storePokeSelResult_ForceQuit( wk );
      (*seq) = 3;
    }
    break;

  case 3:
    CmdLimit_End( wk );
    return TRUE;

  }

  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * ���[�e�[�V�����o�g���ŁA�O�q������ōT�������Ȃ����ɉ�]���s���i��q��O�q�ɏo�����߁j
 *
 * @param   wk
 */
//----------------------------------------------------------------------------------
static u32 SetCoverRotateAction( BTL_CLIENT* wk, BTL_ACTION_PARAM* resultAction )
{
  BtlRotateDir  dir = BTL_ROTATEDIR_R;
  const BTL_POKEPARAM* bpp;
  u32 pokeIdx;

  pokeIdx = BTL_MAINUTIL_GetRotateInPosIdx( dir );
  bpp = BTL_PARTY_GetMemberDataConst( wk->myParty, pokeIdx );
  if( !BPP_IsDead(bpp) )
  {
    BTL_ACTION_SetRotation( resultAction, dir );
    BTL_N_Printf( DBGSTR_CLIENT_CoverRotateDecide, wk->myID, dir );
    return sizeof( BTL_ACTION_PARAM );
  }

  dir = BTL_ROTATEDIR_L;
  pokeIdx = BTL_MAINUTIL_GetRotateInPosIdx( dir );
  bpp = BTL_PARTY_GetMemberDataConst( wk->myParty, pokeIdx );
  if( !BPP_IsDead(bpp) )
  {
    BTL_ACTION_SetRotation( resultAction, dir );
    BTL_N_Printf( DBGSTR_CLIENT_CoverRotateDecide, wk->myID, dir );
    return sizeof( BTL_ACTION_PARAM );
  }

  return 0;
}

//----------------------------------------------------------------------------------
/**
 * �X�^���h�A��������ւ���̓���ւ��m�F����
 *
 * @param   wk
 * @param   seq
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL SubProc_UI_ConfirmIrekae( BTL_CLIENT* wk, int* seq )
{
  enum {
    SEQ_INIT = 0,
    SEQ_START_CONFIRM,
    SEQ_WAIT_CONFIRM,
    SEQ_WAIT_POKESELECT,
    SEQ_DONT_CHANGE,
    SEQ_RETURN,
  };

  switch( *seq ){
  case 0:
    {
      const u8* enemyPutPokeID;
      BTL_ADAPTER_GetRecvData( wk->adapter, (const void**)(&enemyPutPokeID) );
      if( (*enemyPutPokeID) != BTL_POKEID_NULL )
      {
        BTLV_STRPARAM_Setup( &wk->strParam, BTL_STRTYPE_STD, BTL_STRID_STD_IrekaeConfirm );
        BTLV_STRPARAM_AddArg( &wk->strParam, BTL_CLIENTID_SA_ENEMY1 );
        BTLV_STRPARAM_AddArg( &wk->strParam, *enemyPutPokeID );
        BTLV_StartMsg( wk->viewCore, &wk->strParam );
        (*seq) = SEQ_START_CONFIRM;
      }
      else
      {
        (*seq) = SEQ_DONT_CHANGE;
      }
    }
    break;

  case SEQ_START_CONFIRM:
    BTLV_WaitMsg( wk->viewCore );
    if( BTLV_IsJustDoneMsg(wk->viewCore) )
    {
      BTLV_STRPARAM_Setup( &wk->strParam,    BTL_STRTYPE_UI, BTLMSG_UI_SEL_IREKAE_YES );
      BTLV_STRPARAM_Setup( &wk->strParamSub, BTL_STRTYPE_UI, BTLMSG_UI_SEL_IREKAE_NO );
      BTLV_YESNO_Start( wk->viewCore, &wk->strParam, &wk->strParamSub );
      (*seq) = SEQ_WAIT_CONFIRM;
    }
    break;

  case SEQ_WAIT_CONFIRM:
    if( BTLV_WaitMsg(wk->viewCore) )
    {
      BtlYesNo result;
      if( BTLV_YESNO_Wait(wk->viewCore, &result) )
      {
        if( result == BTL_YESNO_YES )
        {
          setupPokeSelParam( wk, BPL_MODE_NORMAL, 1, &wk->pokeSelParam, &wk->pokeSelResult );
          BTLV_StartPokeSelect( wk->viewCore, &wk->pokeSelParam, 0, FALSE, &wk->pokeSelResult );
          (*seq) = SEQ_WAIT_POKESELECT;
        }
        else
        {
          (*seq) = SEQ_DONT_CHANGE;
        }
      }
    }
    break;

  case SEQ_WAIT_POKESELECT:
    if( BTLV_WaitPokeSelect(wk->viewCore) )
    {
      if( !BTL_POKESELECT_IsCancel(&wk->pokeSelResult) )
      {
        u8 idx = BTL_POKESELECT_RESULT_GetLast( &wk->pokeSelResult );
        if( (idx!=0) && (idx < BTL_PARTY_MEMBER_MAX) )
        {
          BTL_ACTION_SetChangeParam( &wk->actionParam[0], 0, idx );
          (*seq) = SEQ_RETURN;
          break;
        }
      }
      (*seq) = SEQ_DONT_CHANGE;
    }
    break;

  case SEQ_DONT_CHANGE:
    BTL_ACTION_SetNULL( &wk->actionParam[0] );
    (*seq) = SEQ_RETURN;
    /* fallthru */
  case SEQ_RETURN:
    wk->returnDataPtr = &(wk->actionParam[0]);
    wk->returnDataSize = sizeof(wk->actionParam[0]);
    return TRUE;

  }
  return FALSE;
}




//---------------------------------------------------
// ����L�^�f�[�^��M->�ۑ�
//---------------------------------------------------
static BOOL SubProc_UI_RecordData( BTL_CLIENT* wk, int* seq )
{
  u32 dataSize;
  const void* dataBuf;

  dataSize = BTL_ADAPTER_GetRecvData( wk->adapter, &dataBuf );

  TAYA_Printf("�^��f�[�^�L�^ %d bytes \n", dataSize);

  if( wk->btlRec )
  {
    TAYA_Printf("�^��f�[�^ %d bytes ��������\n", dataSize);
    BTL_REC_Write( wk->btlRec, dataBuf, dataSize );
  }

  if( wk->cmdCheckServer )
  {
    BTL_SERVER_CMDCHECK_RestoreActionData( wk->cmdCheckServer, dataBuf, dataSize );
    wk->cmdCheckTimingCode = BTL_REC_GetTimingCode( dataBuf );;
    TAYA_Printf( "�������`�F�b�N TimingCode=%d\n", wk->cmdCheckTimingCode);
  }

  return TRUE;
}
//---------------------------------------------------
// �ʐM�ΐ�I��
//---------------------------------------------------
static BOOL SubProc_UI_ExitCommTrainer( BTL_CLIENT* wk, int* seq )
{
  switch( *seq ){
  case 0:
    {
      BtlResult  result;
      u16 strID;
      u8  fMulti;
      u8 clientID = BTL_MAIN_GetEnemyClientID( wk->mainModule, 0 );

      result = checkResult( wk );
      fMulti = BTL_MAIN_IsMultiMode( wk->mainModule );

      switch( result ){
      case BTL_RESULT_WIN:
        strID = (fMulti)? BTL_STRID_STD_WinCommMulti : BTL_STRID_STD_WinComm;
        PMSND_PlayBGM( BTL_MAIN_GetWinBGMNo(wk->mainModule) );
        break;
      case BTL_RESULT_LOSE:
        strID = (fMulti)? BTL_STRID_STD_LoseCommMulti : BTL_STRID_STD_LoseComm;
        break;
      case BTL_RESULT_DRAW:
        strID = (fMulti)? BTL_STRID_STD_DrawCommMulti : BTL_STRID_STD_DrawComm;
        break;
      default:
        return TRUE;
      }

      BTLV_STRPARAM_Setup( &wk->strParam, BTL_STRTYPE_STD, strID );
      BTLV_STRPARAM_AddArg( &wk->strParam, clientID );
      if( fMulti ){
        u8 clientID_2 = BTL_MAIN_GetEnemyClientID( wk->mainModule, 1 );
        BTLV_STRPARAM_AddArg( &wk->strParam, clientID_2);
      }
      BTLV_StartMsg( wk->viewCore, &wk->strParam );
      (*seq)++;
    }
    break;
  case 1:
    if( BTLV_WaitMsg(wk->viewCore) )
    {
      (*seq)++;
    }
    break;
  case 2:
    return TRUE;
  }
  return FALSE;
}

static BtlResult checkResult( BTL_CLIENT* wk )
{
  const BTL_RESULT_CONTEXT* resultContext;

  BTL_ADAPTER_GetRecvData( wk->adapter, (const void*)(&resultContext) );

  switch( resultContext->resultCode ){
  case BTL_RESULT_LOSE:
    if( !BTL_MAINUTIL_IsFriendClientID(resultContext->clientID, wk->myID) ){
      return BTL_RESULT_WIN;
    }
    break;

  case BTL_RESULT_WIN:
    if( !BTL_MAINUTIL_IsFriendClientID(resultContext->clientID, wk->myID) ){
      return BTL_RESULT_LOSE;
    }
    break;

  }
  return resultContext->resultCode;
}


//---------------------------------------------------
// �Q�[�����g���[�i�[����I��
//---------------------------------------------------
static BOOL SubProc_UI_ExitForNPC( BTL_CLIENT* wk, int* seq )
{
  static BtlResult resultCode = 0;

  switch( *seq ){
  case 0:
    {
      resultCode = checkResult( wk );

      if( resultCode == BTL_RESULT_WIN )
      {
        u8 clientID = BTL_MAIN_GetEnemyClientID( wk->mainModule, 0 );

        PMSND_PlayBGM( BTL_MAIN_GetWinBGMNo( wk->mainModule ) );

        BTLV_STRPARAM_Setup( &wk->strParam, BTL_STRTYPE_STD, BTL_STRID_STD_WinTrainer );
        BTLV_STRPARAM_AddArg( &wk->strParam, clientID );
        BTLV_StartMsg( wk->viewCore, &wk->strParam );
        (*seq)++;
      }
      else if( BTL_MAIN_GetSetupStatusFlag(wk->mainModule, BTL_STATUS_FLAG_NO_LOSE) == FALSE )
      {
        u8 clientID = BTL_MAIN_GetPlayerClientID( wk->mainModule );

        BTLV_STRPARAM_Setup( &wk->strParam, BTL_STRTYPE_STD, BTL_STRID_STD_LoseStart );
        BTLV_STRPARAM_AddArg( &wk->strParam, clientID );
        BTLV_STRPARAM_AddArg( &wk->strParam, clientID );
        BTLV_StartMsg( wk->viewCore, &wk->strParam );
        (*seq) = 6;
      }
      else{
        return TRUE;
      }
    }
    break;
  case 1:
    if( BTLV_WaitMsg(wk->viewCore) )
    {
      BOOL fExitMsg = TRUE;
      u16  trmsgID;

      u8 clientID = BTL_MAIN_GetEnemyClientID( wk->mainModule, 0 );
      u32 trainerID = BTL_MAIN_GetClientTrainerID( wk->mainModule, clientID );
      int trtype = BTL_MAIN_GetClientTrainerType( wk->mainModule, clientID );

      if( resultCode == BTL_RESULT_WIN ){
        trmsgID = TRMSG_FIGHT_LOSE;
      }else{
        fExitMsg = FALSE;
      }

      BTLV_EFFECT_SetTrainer( trtype, BTLV_MCSS_POS_TR_BB, 0, 0, 0 );
      BTLV_EFFECT_Add( BTLEFF_TRAINER_IN );

      if( fExitMsg )
      {
        BTLV_StartMsgTrainer( wk->viewCore, trainerID, trmsgID );
      }
      (*seq)++;
    }
    break;
  case 2:
    if( !BTLV_EFFECT_CheckExecute()
    &&  BTLV_WaitMsg(wk->viewCore)
    ){
      if( BTL_MAIN_IsMultiMode( wk->mainModule ) )
      {
        BTLV_EFFECT_Add( BTLEFF_TRAINER_OUT );
        (*seq)++;
      }
      else
      {
        (*seq) = 4;
      }
    }
    break;
  case 3:
    if( !BTLV_EFFECT_CheckExecute() )
    {
      u8 clientID = BTL_MAIN_GetEnemyClientID( wk->mainModule, 1 );
      u32 trainerID = BTL_MAIN_GetClientTrainerID( wk->mainModule, clientID );
      int trtype = BTL_MAIN_GetClientTrainerType( wk->mainModule, clientID );


      BTLV_EFFECT_DelTrainer( BTLV_MCSS_POS_TR_BB );
      BTLV_EFFECT_SetTrainer( trtype, BTLV_MCSS_POS_TR_BB, 0, 0, 0 );
      BTLV_EFFECT_Add( BTLEFF_TRAINER_IN );
      BTLV_StartMsgTrainer( wk->viewCore, trainerID, TRMSG_FIGHT_LOSE );
      (*seq)++;
    }
    break;
  case 4:
    if( BTLV_WaitMsg(wk->viewCore) )
    {
      if( resultCode == BTL_RESULT_WIN )
      {
        u32 getMoney = BTL_MAIN_FixBonusMoney( wk->mainModule );

        if( getMoney )
        {
          u8 clientID = BTL_MAIN_GetPlayerClientID( wk->mainModule );

          BTLV_STRPARAM_Setup( &wk->strParam, BTL_STRTYPE_STD, BTL_STRID_STD_GetMoney );
          BTLV_STRPARAM_AddArg( &wk->strParam, clientID );
          BTLV_STRPARAM_AddArg( &wk->strParam, getMoney );
          BTLV_StartMsg( wk->viewCore, &wk->strParam );
          (*seq)++;
          break;
        }
      }
      return TRUE;
    }
    break;

  case 5:
    if( BTLV_WaitMsg(wk->viewCore) ){
      return TRUE;
    }
    break;

  case 6:
    if( BTLV_WaitMsg(wk->viewCore) )
    {
      u32 loseMoney = BTL_MAIN_FixLoseMoney( wk->mainModule );
      if( loseMoney )
      {
        BTLV_STRPARAM_Setup( &wk->strParam, BTL_STRTYPE_STD, BTL_STRID_STD_LoseMoneyTrainer );
        BTLV_STRPARAM_AddArg( &wk->strParam, wk->myID );
        BTLV_STRPARAM_AddArg( &wk->strParam, loseMoney );
        BTLV_StartMsg( wk->viewCore, &wk->strParam );
      }
      (*seq)++;
    }
    break;
  case 7:
    if( BTLV_WaitMsg(wk->viewCore) )
    {
      BTLV_STRPARAM_Setup( &wk->strParam, BTL_STRTYPE_STD, BTL_STRID_STD_LoseEnd );
      BTLV_STRPARAM_AddArg( &wk->strParam, wk->myID );
      BTLV_StartMsg( wk->viewCore, &wk->strParam );
      (*seq) = 5;
    }
    break;

  default:
    return TRUE;
  }
  return FALSE;
}
//---------------------------------------------------
// �쐶��ŕ�����
//---------------------------------------------------
static BOOL SubProc_UI_LoseWild( BTL_CLIENT* wk, int* seq )
{
  switch( *seq ){
  case 0:
    BTLV_STRPARAM_Setup( &wk->strParam, BTL_STRTYPE_STD, BTL_STRID_STD_LoseStart );
    BTLV_STRPARAM_AddArg( &wk->strParam, wk->myID );
    BTLV_StartMsg( wk->viewCore, &wk->strParam );
    (*seq)++;
    break;

  case 1:
    if( BTLV_WaitMsg(wk->viewCore) )
    {
      u32 loseMoney = BTL_MAIN_FixLoseMoney( wk->mainModule );
      if( loseMoney )
      {
        BTLV_STRPARAM_Setup( &wk->strParam, BTL_STRTYPE_STD, BTL_STRID_STD_LoseMoneyWild );
        BTLV_STRPARAM_AddArg( &wk->strParam, wk->myID );
        BTLV_STRPARAM_AddArg( &wk->strParam, loseMoney );
        BTLV_StartMsg( wk->viewCore, &wk->strParam );
      }
      (*seq)++;
    }
    break;

  case 2:
    if( BTLV_WaitMsg(wk->viewCore) )
    {
      BTLV_STRPARAM_Setup( &wk->strParam, BTL_STRTYPE_STD, BTL_STRID_STD_LoseEnd );
      BTLV_STRPARAM_AddArg( &wk->strParam, wk->myID );
      BTLV_StartMsg( wk->viewCore, &wk->strParam );
      (*seq)++;
    }
    break;

  case 3:
    if( BTLV_WaitMsg(wk->viewCore) ){
      (*seq)++;
    }
    break;

  default:
    return TRUE;
  }

  return FALSE;
}


//---------------------------------------------------
// �������ԏI��
//---------------------------------------------------
static BOOL SubProc_UI_NotifyTimeUp( BTL_CLIENT* wk, int* seq )
{
  switch( *seq ){
  case 0:
    BTLV_STRPARAM_Setup( &wk->strParam, BTL_STRTYPE_STD, BTL_STRID_STD_BattleTimeOver );
    BTLV_StartMsg( wk->viewCore, &wk->strParam );
    (*seq)++;
    break;
  case 1:
    if( BTLV_WaitMsg(wk->viewCore) ){
      (*seq)++;
    }
    break;
  case 2:
    return TRUE;
  }
  return FALSE;
}
//---------------------------------------------------
// �T�[�o�R�}���h����
//---------------------------------------------------
static BOOL SubProc_REC_ServerCmd( BTL_CLIENT* wk, int* seq )
{
  if( wk->viewCore )
  {
    return SubProc_UI_ServerCmd( wk, seq );
  }
  return TRUE;
}
static BOOL SubProc_UI_ServerCmd( BTL_CLIENT* wk, int* seq )
{
  static const struct {
    u32       cmd;
    ServerCmdProc proc;
  }scprocTbl[] = {
    { SC_MSG_STD,               scProc_MSG_Std            },
    { SC_MSG_STD_SE,            scProc_MSG_StdSE          },
    { SC_MSG_SET,               scProc_MSG_Set            },
    { SC_MSG_WAZA,              scProc_MSG_Waza           },
    { SC_ACT_WAZA_EFFECT,       scProc_ACT_WazaEffect     },
    { SC_ACT_WAZA_EFFECT_EX,    scProc_ACT_WazaEffectEx   },
    { SC_ACT_TAMEWAZA_HIDE,     scProc_ACT_TameWazaHide   },
    { SC_ACT_WAZA_DMG,          scProc_ACT_WazaDmg        },
    { SC_ACT_WAZA_DMG_PLURAL,   scProc_ACT_WazaDmg_Plural },
    { SC_ACT_WAZA_ICHIGEKI,     scProc_ACT_WazaIchigeki   },
    { SC_ACT_SICK_ICON,         scProc_ACT_SickIcon       },
    { SC_ACT_CONF_DMG,          scProc_ACT_ConfDamage     },
    { SC_ACT_DEAD,              scProc_ACT_Dead           },
    { SC_ACT_RELIVE,            scProc_ACT_Relive         },
    { SC_ACT_MEMBER_OUT_MSG,    scProc_ACT_MemberOutMsg   },
    { SC_ACT_MEMBER_OUT,        scProc_ACT_MemberOut      },
    { SC_ACT_MEMBER_IN,         scProc_ACT_MemberIn       },
    { SC_ACT_RANKUP,            scProc_ACT_RankUp         },
    { SC_ACT_RANKDOWN,          scProc_ACT_RankDown       },
    { SC_ACT_WEATHER_DMG,       scProc_ACT_WeatherDmg     },
    { SC_ACT_WEATHER_START,     scProc_ACT_WeatherStart   },
    { SC_ACT_WEATHER_END,       scProc_ACT_WeatherEnd     },
    { SC_ACT_SIMPLE_HP,         scProc_ACT_SimpleHP       },
    { SC_ACT_KINOMI,            scProc_ACT_Kinomi         },
    { SC_TOKWIN_IN,             scProc_TOKWIN_In          },
    { SC_TOKWIN_OUT,            scProc_TOKWIN_Out         },
    { SC_OP_HP_MINUS,           scProc_OP_HpMinus         },
    { SC_OP_HP_PLUS,            scProc_OP_HpPlus          },
    { SC_OP_HP_ZERO,            scProc_OP_HpZero          },
    { SC_OP_PP_MINUS,           scProc_OP_PPMinus         },
    { SC_OP_PP_PLUS,            scProc_OP_PPPlus          },
    { SC_OP_RANK_UP,            scProc_OP_RankUp          },
    { SC_OP_RANK_DOWN,          scProc_OP_RankDown        },
    { SC_OP_RANK_SET7,          scProc_OP_RankSet7        },
    { SC_OP_RANK_RECOVER,       scProc_OP_RankRecover     },
    { SC_OP_RANK_RESET,         scProc_OP_RankReset       },
    { SC_OP_ADD_CRITICAL,       scProc_OP_AddCritical     },
    { SC_OP_SICK_SET,           scProc_OP_SickSet         },
    { SC_OP_CURE_POKESICK,      scProc_OP_CurePokeSick    },
    { SC_OP_CURE_WAZASICK,      scProc_OP_CureWazaSick    },
    { SC_OP_MEMBER_IN,          scProc_OP_MemberIn        },
    { SC_OP_SET_STATUS,         scProc_OP_SetStatus       },
    { SC_OP_SET_WEIGHT,         scProc_OP_SetWeight       },
    { SC_OP_CHANGE_POKETYPE,    scProc_OP_ChangePokeType  },
    { SC_OP_WAZASICK_TURNCHECK, scProc_OP_WSTurnCheck     },
    { SC_OP_CONSUME_ITEM,       scProc_OP_ConsumeItem     },
    { SC_OP_UPDATE_USE_WAZA,    scProc_OP_UpdateUseWaza   },
    { SC_OP_SET_CONTFLAG,       scProc_OP_SetContFlag     },
    { SC_OP_RESET_CONTFLAG,     scProc_OP_ResetContFlag   },
    { SC_OP_SET_TURNFLAG,       scProc_OP_SetTurnFlag     },
    { SC_OP_RESET_TURNFLAG,     scProc_OP_ResetTurnFlag   },
    { SC_OP_CHANGE_TOKUSEI,     scProc_OP_ChangeTokusei   },
    { SC_OP_SET_ITEM,           scProc_OP_SetItem         },
    { SC_OP_UPDATE_WAZANUMBER,  scProc_OP_UpdateWazaNumber},
    { SC_OP_HENSIN,             scProc_OP_Hensin          },
    { SC_OP_OUTCLEAR,           scProc_OP_OutClear        },
    { SC_OP_ADD_FLDEFF,         scProc_OP_AddFldEff       },
    { SC_OP_REMOVE_FLDEFF,      scProc_OP_RemoveFldEff    },
    { SC_OP_SET_POKE_COUNTER,   scProc_OP_SetPokeCounter  },
    { SC_OP_BATONTOUCH,         scProc_OP_BatonTouch      },
    { SC_OP_MIGAWARI_CREATE,    scProc_OP_MigawariCreate  },
    { SC_OP_MIGAWARI_DELETE,    scProc_OP_MigawariDelete  },
    { SC_OP_SHOOTER_CHARGE,     scProc_OP_ShooterCharge   },
    { SC_OP_SET_FAKESRC,        scProc_OP_SetFakeSrc      },
    { SC_OP_CLEAR_CONSUMED_ITEM,scProc_OP_ClearConsumedItem },
    { SC_OP_WAZADMG_REC,        scProc_OP_AddWazaDamage   },
    { SC_OP_TURN_CHECK,         scProc_OP_TurnCheck       },
    { SC_ACT_KILL,              scProc_ACT_Kill           },
    { SC_ACT_MOVE,              scProc_ACT_Move           },
    { SC_ACT_EXP,               scProc_ACT_Exp            },
    { SC_ACT_BALL_THROW,        scProc_ACT_BallThrow      },
    { SC_ACT_ROTATION,          scProc_ACT_Rotation       },
    { SC_ACT_CHANGE_TOKUSEI,    scProc_ACT_ChangeTokusei  },
    { SC_ACT_FAKE_DISABLE,      scProc_ACT_FakeDisable    },
    { SC_ACT_EFFECT_BYPOS,      scProc_ACT_EffectByPos    },
    { SC_ACT_EFFECT_BYVECTOR,   scProc_ACT_EffectByVector },
    { SC_ACT_CHANGE_FORM,       scProc_ACT_ChangeForm     },
    { SC_ACT_RESET_MOVE,        scProc_ACT_ResetMove      },
  };

restart:

  switch( *seq ){
  case 0:
    {
      u32 cmdSize;
      const void* cmdBuf;

      cmdSize = BTL_ADAPTER_GetRecvData( wk->adapter, &cmdBuf );

      if( (wk->cmdCheckServer != NULL)
      &&  (wk->cmdCheckTimingCode != BTL_RECTIMING_None)
      ){
        if( BTL_SERVER_CMDCHECK_Make(wk->cmdCheckServer, wk->cmdCheckTimingCode, cmdBuf, cmdSize) ){
          BTL_MAIN_NotifyCmdCheckError( wk->mainModule );
          OS_TPrintf("!!!! recvedCmd=%p, %02x, %02x, %02x, ...\n", cmdBuf,
              ((const u8*)cmdBuf)[0], ((const u8*)cmdBuf)[1], ((const u8*)cmdBuf)[2] );
        }
        wk->cmdCheckTimingCode = BTL_RECTIMING_None;
      }

      SCQUE_Setup( wk->cmdQue, cmdBuf, cmdSize );

      if( wk->commWaitInfoOn )
      {
        wk->commWaitInfoOn = FALSE;
        BTLV_ResetCommWaitInfo( wk->viewCore );
      }
      (*seq)++;
    }
    /* fallthru */
  case 1:
    if( SCQUE_IsFinishRead(wk->cmdQue) )
    {
      BTL_Printf("�T�[�o�R�}���h�ǂݏI���\n");
      (*seq) = 4;
      return TRUE;
    }
    // �^��Đ������`�F�b�N
    else if( RecPlayer_CheckBlackOut(&wk->recPlayer) )
    {
      BTL_N_Printf( DBGSTR_CLIENT_RecPlayerBlackOut );
      return TRUE;
    }
    (*seq)++;
    /* fallthru */
  case 2:
    {
      int i;

      wk->serverCmd = SCQUE_Read( wk->cmdQue, wk->cmdArgs );

      for(i=0; i<NELEMS(scprocTbl); ++i)
      {
        if( scprocTbl[i].cmd == wk->serverCmd ){ break; }
      }

      if( i == NELEMS(scprocTbl) )
      {
        BTL_N_Printf( DBGSTR_CLIENT_UnknownServerCmd, wk->serverCmd);
        (*seq) = 1;
        goto restart;
      }

      BTL_Printf( "serverCmd=%d\n", wk->serverCmd );
      wk->scProc = scprocTbl[i].proc;
      wk->scSeq = 0;
      (*seq)++;
    }
    /* fallthru */
  case 3:
    if( wk->scProc(wk, &wk->scSeq, wk->cmdArgs) )
    {
      (*seq) = 1;
      goto restart;
    }
    break;

  case 4:
    return TRUE;
  }

  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * �����o�[�����グ���b�Z�[�W�\��
 *
 * @param   wk
 * @param   seq
 * @param   args    [0]:ClientID, [1]:pokeID
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL scProc_ACT_MemberOutMsg( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    {
      BOOL fClientArg;
      u16  strID;
      strID = CheckMemberOutStrID( wk, args[0], &fClientArg );

      BTLV_STRPARAM_Setup( &wk->strParam, BTL_STRTYPE_STD, strID );
      if( fClientArg ){
        BTLV_STRPARAM_AddArg( &wk->strParam, args[0] );
      }
      BTLV_STRPARAM_AddArg( &wk->strParam, args[1] );

      BTLV_StartMsg( wk->viewCore, &wk->strParam );
      ++(*seq);
    }
    break;

  case 1:
    if( BTLV_WaitMsg(wk->viewCore) ){
      return TRUE;
    }
    break;
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * �����o�[�����グ���b�Z�[�W�i�����@���ǂ�I�Ȃǁj��ID���擾
 *
 * @param   wk
 * @param   clientID
 * @param   fClientArg    [out] ������̈����Ƃ��ăN���C�A���gID���g���P�[�X��TRUE������
 *
 * @retval  u16   ������ID
 */
//----------------------------------------------------------------------------------
static u16 CheckMemberOutStrID( BTL_CLIENT* wk, u8 clientID, BOOL* fClientArg )
{
  // �������Ώ�
  if( clientID == wk->myID )
  {
    *fClientArg = FALSE;

    // �V���O������ʐM�Ȃ瑊��ɗ^�����_���[�W�ʂɉ����ă��b�Z�[�W��ς���
    if( (BTL_MAIN_GetRule(wk->mainModule) == BTL_RULE_SINGLE)
    &&  (BTL_MAIN_GetCommMode(wk->mainModule) == BTL_COMM_NONE)
    ){
      u32 dmg_per = EnemyPokeHPBase_CheckRatio( wk );
      if( dmg_per >= 75 ){
        return BTL_STRID_STD_BackChange5;
      }
      if( dmg_per > 50 ){
        return BTL_STRID_STD_BackChange4;
      }
      if( dmg_per > 25 ){
        return BTL_STRID_STD_BackChange3;
      }
      if( dmg_per > 0 ){
        return BTL_STRID_STD_BackChange2;
      }
      return BTL_STRID_STD_BackChange1;
    }
    else
    {
      return BTL_STRID_STD_BackChange2;
    }
  }
  // �����ȊO���Ώ�
  else
  {
    *fClientArg = TRUE;

    // �����i�}���`�j
    if( !BTL_MAIN_IsOpponentClientID(wk->mainModule, wk->myID, clientID) )
    {
      return BTL_STRID_STD_BackChange_Player;
    }
    else
    {
      if( BTL_MAIN_GetCompetitor(wk->mainModule) == BTL_COMPETITOR_COMM ){
        return BTL_STRID_STD_BackChange_Player;
      }
      return BTL_STRID_STD_BackChange_NPC;
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * �����o�[�����グ�A�N�V����
 *
 * @param   wk
 * @param   seq
 * @param   args
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL scProc_ACT_MemberOut( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    {
      BtlvMcssPos  vpos = BTL_MAIN_BtlPosToViewPos( wk->mainModule, args[0] );
      BTLV_ACT_MemberOut_Start( wk->viewCore, vpos );
      (*seq)++;
    }
    break;

  case 1:
    if( BTLV_ACT_MemberOut_Wait(wk->viewCore) )
    {
      return TRUE;
    }
    break;
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * �����o�[����A�N�V����
 *
 * @param   wk
 * @param   seq
 * @param   args
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL scProc_ACT_MemberIn( BTL_CLIENT* wk, int* seq, const int* args )
{
  u8 clientID  = args[0];
  u8 posIdx    = args[1];
  u8 memberIdx = args[2];
  u8 fPutMsg   = args[3];

  switch( *seq ){
  case 0:
    if( fPutMsg )
    {
      const BTL_POKEPARAM* bpp = BTL_POKECON_GetClientPokeDataConst( wk->pokeCon, clientID, posIdx );
      u8 pokeID = BPP_GetID( bpp );

      if( !BTL_MAIN_IsOpponentClientID(wk->mainModule, wk->myID, clientID) )
      {
        if( wk->myID == clientID )
        {
          u16 strID = CheckMemberPutStrID( wk );
          BTLV_STRPARAM_Setup( &wk->strParam, BTL_STRTYPE_STD, strID );
          BTLV_STRPARAM_AddArg( &wk->strParam, pokeID );
        }
        else
        {
          BTLV_STRPARAM_Setup( &wk->strParam, BTL_STRTYPE_STD, BTL_STRID_STD_PutSingle_Player );
          BTLV_STRPARAM_AddArg( &wk->strParam, clientID );
          BTLV_STRPARAM_AddArg( &wk->strParam, pokeID );
        }
      }
      else
      {
        // ���肪����ւ�
        if( BTL_MAIN_GetCompetitor(wk->mainModule) == BTL_COMPETITOR_TRAINER )
        {
          BTLV_STRPARAM_Setup( &wk->strParam, BTL_STRTYPE_STD, BTL_STRID_STD_PutSingle_NPC );
          BTLV_STRPARAM_AddArg( &wk->strParam, clientID );
          BTLV_STRPARAM_AddArg( &wk->strParam, pokeID );
        }
        else
        {
          BTLV_STRPARAM_Setup( &wk->strParam, BTL_STRTYPE_STD, BTL_STRID_STD_PutSingle_Player );
          BTLV_STRPARAM_AddArg( &wk->strParam, clientID );
          BTLV_STRPARAM_AddArg( &wk->strParam, pokeID );
        }
      }
      BTLV_StartMsg( wk->viewCore, &wk->strParam );
    }
    (*seq)++;
    break;

  case 1:
    if( BTLV_WaitMsg(wk->viewCore) )
    {
      BtlPokePos  pokePos = BTL_MAIN_GetClientPokePos( wk->mainModule, clientID, posIdx );

      BTL_Printf("�����o�[IN ACT client=%d, posIdx=%d, pos=%d, memberIdx=%d\n",
          clientID, posIdx, pokePos, memberIdx );

      BTLV_StartMemberChangeAct( wk->viewCore, pokePos, clientID, posIdx );
      (*seq)++;
    }
    break;
  case 2:
    if( BTLV_WaitMemberChangeAct(wk->viewCore) )
    {
      EnemyPokeHPBase_Update( wk );
      return TRUE;
    }
    break;
  }
  return FALSE;
}

//----------------------------------------------------------------------------------
/**
 * �����o�[�J��o�����b�Z�[�W�i�䂯���I�����Ȃǁj��ID���擾
 *
 * @param   wk
 *
 * @retval  u16
 */
//----------------------------------------------------------------------------------
static u16 CheckMemberPutStrID( BTL_CLIENT* wk )
{
  if( BTL_MAIN_GetRule(wk->mainModule) == BTL_RULE_SINGLE )
  {
    u8 opponentClientID = BTL_MAIN_GetOpponentClientID( wk->mainModule, wk->myID, 0 );
    const BTL_POKEPARAM* bpp = BTL_POKECON_GetClientPokeDataConst( wk->pokeCon, opponentClientID, 0 );

    if( BPP_IsDead(bpp) ){
      return BTL_STRID_STD_PutSingle;
    }
    else
    {
      fx32 hp_ratio = BPP_GetHPRatio( bpp );

      if( hp_ratio >= FX32_CONST(77.5) ){
        return BTL_STRID_STD_PutSingle;
      }
      if( hp_ratio >= FX32_CONST(55) ){
        return BTL_STRID_STD_PutChange1;
      }
      if( hp_ratio >= FX32_CONST(32.5) ){
        return BTL_STRID_STD_PutChange2;
      }
      if( hp_ratio >= FX32_CONST(10) ){
        return BTL_STRID_STD_PutChange3;
      }
      return BTL_STRID_STD_PutChange4;
    }
  }
  // �V���O����ȊO�͋���
  return BTL_STRID_STD_PutSingle;
}

static BOOL scProc_MSG_Std( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    if( BTL_CLIENT_IsChapterSkipMode(wk) ){
      return TRUE;
    }

    BTLV_StartMsgStd( wk->viewCore, args[0], &args[1] );
    (*seq)++;
    break;
  case 1:
    if( BTLV_WaitMsg(wk->viewCore) )
    {
      return TRUE;
    }
  }
  return FALSE;
}
static BOOL scProc_MSG_StdSE( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    if( BTL_CLIENT_IsChapterSkipMode(wk) ){
      return TRUE;
    }

    BTLV_StartMsgStd( wk->viewCore, args[0], &args[2] );
    (*seq)++;
    break;
  case 1:
    if( BTLV_IsJustDoneMsg(wk->viewCore) ){
      PMSND_PlaySE( args[1] );
    }
    if( BTLV_WaitMsg(wk->viewCore) )
    {
      return TRUE;
    }
  }
  return FALSE;
}


static BOOL scProc_MSG_Set( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    if( BTL_CLIENT_IsChapterSkipMode(wk) ){
      return TRUE;
    }

    BTLV_StartMsgSet( wk->viewCore, args[0], &args[1] );
    (*seq)++;
    break;
  case 1:
    if( BTLV_WaitMsg(wk->viewCore) )
    {
      return TRUE;
    }
  }
  return FALSE;
}

static BOOL scProc_MSG_Waza( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    if( BTL_CLIENT_IsChapterSkipMode(wk) ){
      return TRUE;
    }

    BTLV_StartMsgWaza( wk->viewCore, args[0], args[1] );
    (*seq)++;
    break;
  case 1:
    if( BTLV_WaitMsg(wk->viewCore) )
    {
      return TRUE;
    }
  }
  return FALSE;
}

/**
 *  ���U�G�t�F�N�g
 *  args  [0]:�U���|�P�ʒu  [1]:�h��|�P�ʒu  [2]:���UID
 */
static BOOL scProc_ACT_WazaEffect( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ) {
  case 0:
    if( BTL_CLIENT_IsGameTimeOver(wk) ){
      return TRUE;
    }
    if( BTL_CLIENT_IsChapterSkipMode(wk) ){
      return TRUE;
    }

    if( BTL_MAIN_IsWazaEffectEnable(wk->mainModule) )
    {
      WazaID waza;
      u8 atPokePos, defPokePos;
      const BTL_PARTY* party;
      const BTL_POKEPARAM* poke;

      atPokePos  = args[0];
      defPokePos = args[1];
      waza       = args[2];
      poke = BTL_POKECON_GetFrontPokeDataConst( wk->pokeCon, atPokePos );
      BTLV_ACT_WazaEffect_Start( wk->viewCore, atPokePos, defPokePos, waza, BTLV_WAZAEFF_TURN_DEFAULT, 0 );
      (*seq)++;
    }
    else{
      return TRUE;
    }
    break;

  case 1:
    if( BTLV_ACT_WazaEffect_Wait(wk->viewCore) )
    {
      return TRUE;
    }
    break;
  }

  return FALSE;
}
/**
 *  ���U�G�t�F�N�g�i�g�������t���j
 *  args  [0]:�U���|�P�ʒu  [1]:�h��|�P�ʒu  [2]:���UID  [3]:�g������ (1byte)
 */
static BOOL scProc_ACT_WazaEffectEx( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ) {
  case 0:
    {
      u8 turnType = args[3];
      if( BTL_MAIN_IsWazaEffectEnable(wk->mainModule) )
      {
        WazaID waza;
        u8 atPokePos, defPokePos;
        const BTL_PARTY* party;
        const BTL_POKEPARAM* poke;

        atPokePos  = args[0];
        defPokePos = args[1];
        waza       = args[2];
        poke = BTL_POKECON_GetFrontPokeDataConst( wk->pokeCon, atPokePos );
        BTLV_ACT_WazaEffect_Start( wk->viewCore, atPokePos, defPokePos, waza, turnType, 0 );
        (*seq)++;
      }
      else{
        return TRUE;
      }
    }
    break;

  case 1:
    if( BTLV_ACT_WazaEffect_Wait(wk->viewCore) ){
      return TRUE;
    }
    break;
  }

  return FALSE;
}
/**
 *  ���߃��U�i������Ƃԓ��j�ɂ��A�\���I���^�I�t
 *  args  [0]:�Ώۃ|�PID  [1]:TRUE=��\��, FALSE=�\��
 */
static BOOL scProc_ACT_TameWazaHide( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ) {
  case 0:
    {
      u8 pokeID = args[0];
      BtlvMcssPos vpos = BTL_MAIN_PokeIDtoViewPos( wk->mainModule, wk->pokeCon, pokeID );

      BTLV_MCSS_VANISH_FLAG  flag = (args[1])? BTLV_MCSS_VANISH_ON : BTLV_MCSS_VANISH_OFF;


      BTLV_ACT_TameWazaHide( wk->viewCore, vpos, flag );
      (*seq)++;
    }
    break;

  case 1:
    return TRUE;
  }
  return FALSE;
}



/**
 * �y�A�N�V�����z�P�̃_���[�W���o  [0]:pokeID, [1]:affinity [2]:wazaID
 */
static BOOL scProc_ACT_WazaDmg( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ) {
  case 0:
  {
    WazaID waza;
    u8 atPokePos, defPokePos, affinity;
    u16 damage;
    const BTL_PARTY* party;
    const BTL_POKEPARAM* poke;

    defPokePos  = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, args[0] );
    affinity  = args[1];
    waza      = args[2];

    BTLV_ACT_DamageEffectSingle_Start( wk->viewCore, waza, defPokePos, affinity );
    (*seq)++;
  }
  break;

  case 1:
    if( BTLV_ACT_DamageEffectSingle_Wait(wk->viewCore) )
    {
      return TRUE;
    }
    break;
  }
  return FALSE;
}
/**
 * �y�A�N�V�����z�����̈�ă_���[�W����
 */
static BOOL scProc_ACT_WazaDmg_Plural( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ) {
  case 0:
  {
    u16               poke_cnt  = args[0];
    BtlTypeAffAbout   aff_about = args[1];
    WazaID            waza      = args[2];

    u8 pokeID[ BTL_POS_MAX ];
    u8 i;
    for(i=0; i<poke_cnt; ++i){
      pokeID[i] = SCQUE_READ_ArgOnly( wk->cmdQue );
    }
    BTLV_ACT_DamageEffectPlural_Start( wk->viewCore, poke_cnt, aff_about, pokeID, waza );
    (*seq)++;
  }
  break;

  case 1:
    if( BTLV_ACT_DamageEffectPlural_Wait(wk->viewCore) )
    {
      return TRUE;
    }
    break;
  }
  return FALSE;
}
/**
 * �y�A�N�V�����z�ꌂ�K�E���U����
 */
static BOOL scProc_ACT_WazaIchigeki( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ) {
  case 0:
  {
    BtlPokePos pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, args[0] );

//    BTLV_EFFECT_Damage( BTL_MAIN_BtlPosToViewPos(wk->mainModule, pos), wazaID );

    BTLV_ACT_SimpleHPEffect_Start( wk->viewCore, pos );
    (*seq)++;
  }
  break;

  case 1:
    if( BTLV_ACT_SimpleHPEffect_Wait(wk->viewCore) )
    {
      BTLV_StartMsgStd( wk->viewCore, BTL_STRID_STD_Ichigeki, args );
      (*seq)++;
    }
    break;

  case 2:
    if( BTLV_WaitMsg(wk->viewCore) )
    {
      return TRUE;
    }
    break;
  }
  return FALSE;
}
/**
 * �y�A�N�V�����z��Ԉُ�A�C�R��  args[0]:pokeID  args[1]:��Ԉُ�R�[�h
 */
static BOOL scProc_ACT_SickIcon( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ) {
  case 0:
  {
    BtlPokePos pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, args[0] );
    BtlvMcssPos vpos = BTL_MAIN_BtlPosToViewPos( wk->mainModule, pos );
    BTLV_EFFECT_SetGaugeStatus( args[1], vpos );
    (*seq)++;
  }
  break;

  case 1:
    return TRUE;
  }

  return FALSE;
}
/**
 * �y�A�N�V�����z�����񎩔��_���[�W
 */
static BOOL scProc_ACT_ConfDamage( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ) {
  case 0:
    {
      BtlPokePos pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, args[0] );
      BTLV_ACT_SimpleHPEffect_Start( wk->viewCore, pos );
      (*seq)++;
    }
    break;

  case 1:
    if( BTLV_ACT_SimpleHPEffect_Wait(wk->viewCore) )
    {
      return TRUE;
    }
    break;
  }
  return FALSE;
}
/**
 * �y�A�N�V�����z�|�P�����Ђ�
 */
static BOOL scProc_ACT_Dead( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    {
      BtlPokePos pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, args[0] );
      BTLV_StartDeadAct( wk->viewCore, pos );
      (*seq)++;
    }
    break;
  case 1:
    if( BTLV_WaitDeadAct(wk->viewCore) )
    {
      BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
      BPP_Clear_ForDead( pp );
      return TRUE;
    }
    break;
  }
  return FALSE;
}
/**
 * �y�A�N�V�����z�|�P���������Ԃ�
 */
static BOOL scProc_ACT_Relive( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    {
      BtlPokePos pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, args[0] );
      BTLV_StartReliveAct( wk->viewCore, pos );
      (*seq)++;
    }
    break;
  case 1:
    if( BTLV_WaitReliveAct(wk->viewCore) )
    {
      return TRUE;
    }
    break;
  }
  return FALSE;
}

/**
 * �y�A�N�V�����z�\�̓����N�_�E��
 */
static BOOL scProc_ACT_RankDown( BTL_CLIENT* wk, int* seq, const int* args )
{
  BtlPokePos pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, args[0] );
  u8 vpos = BTL_MAIN_BtlPosToViewPos( wk->mainModule,  pos );

  switch( *seq ){
  case 0:
    if( BTL_CLIENT_IsChapterSkipMode(wk) ){
      return TRUE;
    }

    BTLV_StartRankDownEffect( wk->viewCore, vpos );
    (*seq)++;
    break;
  case 1:
    if( BTLV_WaitRankEffect(wk->viewCore, vpos) ){
      return TRUE;
    }
  }
  return FALSE;
}
/**
 * �y�A�N�V�����z�\�̓����N�A�b�v
 */
static BOOL scProc_ACT_RankUp( BTL_CLIENT* wk, int* seq, const int* args )
{
  BtlPokePos pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, args[0] );
  u8 vpos = BTL_MAIN_BtlPosToViewPos( wk->mainModule, pos );

  switch( *seq ){
  case 0:
    if( BTL_CLIENT_IsChapterSkipMode(wk) ){
      return TRUE;
    }

    BTLV_StartRankUpEffect( wk->viewCore, vpos );
    (*seq)++;
    break;
  case 1:
    if( BTLV_WaitRankEffect(wk->viewCore, vpos) ){
      return TRUE;
    }
  }
  return FALSE;
}
/**
 *  �y�A�N�V�����z�V��ɂ���ă_���[�W����
 */
static BOOL scProc_ACT_WeatherDmg( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    {
      BtlWeather weather = args[0];
      u8 pokeCnt = args[1];
      u16 msgID;
      u8 pokeID, pokePos, i;

      switch( weather ){
      case BTL_WEATHER_SAND:
        BTLV_StartMsgStd( wk->viewCore, BTL_STRID_STD_SandAttack, NULL );
        break;
      case BTL_WEATHER_SNOW:
        BTLV_StartMsgStd( wk->viewCore, BTL_STRID_STD_SnowAttack, NULL );
        break;
      default:
        break;
      }

      for(i=0; i<pokeCnt; ++i)
      {
        pokeID = SCQUE_READ_ArgOnly( wk->cmdQue );
        pokePos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, pokeID );
        BTLV_ACT_SimpleHPEffect_Start( wk->viewCore, pokePos );
      }
      (*seq)++;
    }
    break;
  case 1:
    if( BTLV_WaitMsg(wk->viewCore)
    &&  BTLV_ACT_SimpleHPEffect_Wait(wk->viewCore)
    ){
      return TRUE;
    }
    break;
  }
  return FALSE;
}
//---------------------------------------------------------------------------------------
/**
 *  �V��ω��J�n
 */
//---------------------------------------------------------------------------------------
static BOOL scProc_ACT_WeatherStart( BTL_CLIENT* wk, int* seq, const int* args )
{
  static const struct {
    u16 strID;
    u16 effectID;
  }ParamTbl[] = {
    { BTL_STRID_STD_ShineStart,   BTLEFF_WEATHER_HARE       },
    { BTL_STRID_STD_ShineStart,   BTLEFF_WEATHER_HARE       },
    { BTL_STRID_STD_RainStart,    BTLEFF_WEATHER_AME        },
    { BTL_STRID_STD_SnowStart,    BTLEFF_WEATHER_ARARE      },
    { BTL_STRID_STD_StormStart,   BTLEFF_WEATHER_SUNAARASHI },
  };

  BtlWeather weather = args[0];

  switch( *seq ){
  case 0:
    if( BTL_CLIENT_IsChapterSkipMode(wk) ){
      return TRUE;
    }

    if( weather < NELEMS(ParamTbl) )
    {
      BTLV_EFFECT_Add( ParamTbl[weather].effectID );
    }
    (*seq)++;
    break;

  case 1:
    if( !BTLV_EFFECT_CheckExecute() )
    {
      BTLV_StartMsgStd( wk->viewCore, ParamTbl[weather].strID, NULL );
      (*seq)++;
    }
    break;

  case 2:
    if( BTLV_WaitMsg(wk->viewCore) ){
      return TRUE;
    }
    break;
  }
  return FALSE;
}
//---------------------------------------------------------------------------------------
/**
 *  �^�[���`�F�b�N�ɂ��V��̏I������
 */
//---------------------------------------------------------------------------------------
static BOOL scProc_ACT_WeatherEnd( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    {
      BtlWeather weather = args[0];
      u16  msgID;
      switch( weather ){
      case BTL_WEATHER_SHINE: msgID = BTL_STRID_STD_ShineEnd; break;
      case BTL_WEATHER_RAIN:  msgID = BTL_STRID_STD_RainEnd; break;
      case BTL_WEATHER_SAND:  msgID = BTL_STRID_STD_StormEnd; break;
      case BTL_WEATHER_SNOW:  msgID = BTL_STRID_STD_SnowEnd; break;
//      case BTL_WEATHER_MIST:  msgID = BTL_STRID_STD_MistEnd; break;
      default:
        GF_ASSERT(0); // �������ȓV��ID
        return TRUE;
      }
      BTLV_StartMsgStd( wk->viewCore, msgID, NULL );
      (*seq)++;
    }
    break;
  case 1:
    if( BTLV_WaitMsg(wk->viewCore) )
    {
      return TRUE;
    }
    break;
  }
  return FALSE;
}
//---------------------------------------------------------------------------------------
/**
 *  �V���v����HP�Q�[�W��������
 */
//---------------------------------------------------------------------------------------
static BOOL scProc_ACT_SimpleHP( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    {
      BtlPokePos pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, args[0] );
      BTLV_ACT_SimpleHPEffect_Start( wk->viewCore, pos );
      (*seq)++;
    }
    break;
  case 1:
    if( BTLV_ACT_SimpleHPEffect_Wait(wk->viewCore) )
    {
      return TRUE;
    }
    break;
  }
  return FALSE;
}
//---------------------------------------------------------------------------------------
/**
 *  �Ƃ������u�g���[�X�v�̔�������
 *  args .. [0]:�g���[�X�����̃|�PID  [1]:�R�s�[�Ώۂ̃|�PID  [2]:�R�s�[����Ƃ�����
 */
//---------------------------------------------------------------------------------------
static BOOL scProc_ACT_Kinomi( BTL_CLIENT* wk, int* seq, const int* args )
{
  u8 pokeID = args[0];
  BtlPokePos pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, pokeID );

  switch( *seq ){
  case 0:
    {
      BTLV_KinomiAct_Start( wk->viewCore, pos );
      (*seq)++;
    }
    break;
  case 1:
    if( BTLV_KinomiAct_Wait( wk->viewCore, pos ) )
    {
      return TRUE;
    }
    break;
  }

  return FALSE;
}
//---------------------------------------------------------------------------------------
/**
 *  �����Ђ񂵉��o
 *  args .. [0]:�Ώۃ|�PID  [1]:�G�t�F�N�g�^�C�v
 */
//---------------------------------------------------------------------------------------
static BOOL scProc_ACT_Kill( BTL_CLIENT* wk, int* seq, const int* args )
{
  // @@@ ���͒P����HP���G�t�F�N�g�ő�p
  u8 pokeID = args[0];
  u8 effType = args[1];

  switch( *seq ){
  case 0:
    {
      BtlPokePos pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, pokeID );
      BTLV_ACT_SimpleHPEffect_Start( wk->viewCore, pos );
      (*seq)++;
    }
    break;
  case 1:
    if( BTLV_ACT_SimpleHPEffect_Wait(wk->viewCore) )
    {
      return TRUE;
    }
    break;
  }
  return FALSE;
}
//---------------------------------------------------------------------------------------
/**
 *  ���[�u����
 *  args .. [0]:�ΏۃN���C�A���gID  [1]:�Ώۃ|�P�����ʒu1  [2]:�Ώۃ|�P�����ʒu2
 */
//---------------------------------------------------------------------------------------
static BOOL scProc_ACT_Move( BTL_CLIENT* wk, int* seq, const int* args )
{

  switch( *seq ){
  case 0:
    {
      u8 clientID = args[0];
      u8 pos1 = args[1];
      u8 pos2 = args[2];
      u8 posIdx1 = BTL_MAIN_BtlPosToPosIdx( wk->mainModule, pos1 );
      u8 posIdx2 = BTL_MAIN_BtlPosToPosIdx( wk->mainModule, pos2 );
      u8 vpos1 = BTL_MAIN_BtlPosToViewPos( wk->mainModule, pos1 );
      u8 vpos2 = BTL_MAIN_BtlPosToViewPos( wk->mainModule, pos2 );

      BTL_PARTY* party = BTL_POKECON_GetPartyData( wk->pokeCon, clientID );
      BTL_PARTY_SwapMembers( party, posIdx1, posIdx2 );

      BTL_Printf("���[�u�ʒu: %d <-> %d\n", pos1, pos2 );

      BTLV_ACT_MoveMember_Start( wk->viewCore, clientID, vpos1, vpos2, posIdx1, posIdx2 );
      (*seq)++;
    }
    break;
  case 1:
    if( BTLV_ACT_MoveMember_Wait(wk->viewCore) )
    {
      return TRUE;
    }
    break;
  }
  return FALSE;
}
//---------------------------------------------------------------------------------------
/**
 *  �g���v���o�g���E���Z�b�g���[�u����
 *  args .. [0]:clientID_1, [1]:posIdx_1, [2]:clientID_2, [3]:posIdx_2
 */
//---------------------------------------------------------------------------------------
static BOOL scProc_ACT_ResetMove( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    BTLV_STRPARAM_Setup( &wk->strParam, BTL_STRTYPE_STD, BTL_STRID_STD_ResetMove );
    BTLV_StartMsg( wk->viewCore, &wk->strParam );
    (*seq)++;
    break;
  case 1:
    if( BTLV_WaitMsg(wk->viewCore) )
    {
      u8 clientID = args[0];
      u8 posIdx1 = args[1];
      u8 posIdx2 = 1;
      u8 pos1  = BTL_MAIN_GetClientPokePos( wk->mainModule, clientID, posIdx1 );
      u8 pos2  = BTL_MAIN_GetClientPokePos( wk->mainModule, clientID, posIdx2 );
      u8 vpos1 = BTL_MAIN_BtlPosToViewPos( wk->mainModule, pos1 );
      u8 vpos2 = BTL_MAIN_BtlPosToViewPos( wk->mainModule, pos2 );

      BTL_PARTY* party = BTL_POKECON_GetPartyData( wk->pokeCon, clientID );
      BTL_PARTY_SwapMembers( party, posIdx1, posIdx2 );

      BTLV_ACT_MoveMember_Start( wk->viewCore, clientID, vpos1, vpos2, posIdx1, posIdx2 );
      (*seq)++;
    }
    break;
  case 2:
    if( BTLV_ACT_MoveMember_Wait(wk->viewCore) )
    {
      u8 clientID = args[2];
      u8 posIdx1 = args[3];
      u8 posIdx2 = 1;
      u8 pos1  = BTL_MAIN_GetClientPokePos( wk->mainModule, clientID, posIdx1 );
      u8 pos2  = BTL_MAIN_GetClientPokePos( wk->mainModule, clientID, posIdx2 );
      u8 vpos1 = BTL_MAIN_BtlPosToViewPos( wk->mainModule, pos1 );
      u8 vpos2 = BTL_MAIN_BtlPosToViewPos( wk->mainModule, pos2 );

      BTL_PARTY* party = BTL_POKECON_GetPartyData( wk->pokeCon, clientID );
      BTL_PARTY_SwapMembers( party, posIdx1, posIdx2 );

      BTLV_ACT_MoveMember_Start( wk->viewCore, clientID, vpos1, vpos2, posIdx1, posIdx2 );
      (*seq)++;
    }
    break;
  case 3:
    if( BTLV_ACT_MoveMember_Wait(wk->viewCore) )
    {
      return TRUE;
    }
    break;
  }
  return FALSE;
}

//---------------------------------------------------------------------------------------
/**
 *  �o���l���Z����
 *  args .. [0]:�Ώۃ|�P����ID  [1]:�o���l���Z��
 */
//---------------------------------------------------------------------------------------
static BOOL scProc_ACT_Exp( BTL_CLIENT* wk, int* seq, const int* args )
{
  enum {
    SEQ_INIT = 0,
    SEQ_ADD_ROOT,
    SEQ_GAUGE_NORMAL_WAIT,
    SEQ_LVUP_ROOT,
    SEQ_LVUP_GAUGE_WAIT,
    SEQ_LVUP_EFFECT_WAIT,
    SEQ_LVUP_INFO_START,
    SEQ_LVUP_INFO_MSG_WAIT,
    SEQ_LVUP_INFO_PARAM_START,
    SEQ_LVUP_INFO_PARAM_SEQ1,
    SEQ_LVUP_INFO_PARAM_SEQ2,
    SEQ_LVUP_INFO_PARAM_END,
    SEQ_LVUP_WAZAOBOE_WAIT,
    SEQ_END,
  };

  static BTL_LEVELUP_INFO  lvupInfo;
  static u32  addExp;
  static int  subSeq;

  u8 pokeID = args[0];
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, pokeID );
  BtlvMcssPos vpos = BTL_MAIN_PokeIDtoViewPos( wk->mainModule, wk->pokeCon, pokeID );

  switch( *seq ){
  case SEQ_INIT:
      addExp = args[1];
      subSeq = 0;
      (*seq) = SEQ_ADD_ROOT;
      /* fallthru */
  case SEQ_ADD_ROOT:
    if( addExp )
    {
      u32 expRest = addExp;

      if( BPP_AddExp(bpp, &expRest, &lvupInfo) )
      {
        (*seq) = SEQ_LVUP_ROOT;
      }
      else
      {
        if( BTL_MAIN_CheckFrontPoke(wk->mainModule, wk->pokeCon, pokeID) )
        {
          BTLV_EFFECT_CalcGaugeEXP( vpos, addExp );
          (*seq) = SEQ_GAUGE_NORMAL_WAIT;
        }
        else {
          (*seq) = SEQ_END;
        }
        expRest = 0;
      }
      addExp = expRest;
    }
    else
    {
      (*seq) = SEQ_END;
    }
    break;

  // �ʏ�́i���x���A�b�v���Ȃ��j�Q�[�W���o�̏I���҂�
  case SEQ_GAUGE_NORMAL_WAIT:
    if( !BTLV_EFFECT_CheckExecuteGauge() ){
      (*seq) = SEQ_END;
    }
    break;

  // ���x���A�b�v�������[�g
  case SEQ_LVUP_ROOT:
    BPP_ReflectByPP( bpp );
    BTL_MAIN_ClientPokemonReflectToServer( wk->mainModule, pokeID );
    {
       // ��ɏo�Ă���Ȃ�Q�[�W���o
       if( vpos != BTLV_MCSS_POS_ERROR )
       {
         BTLV_EFFECT_CalcGaugeEXPLevelUp( vpos, bpp );
         (*seq) = SEQ_LVUP_GAUGE_WAIT;
       }
       // �o�Ă��Ȃ��Ȃ�\�͏㏸�\���܂ŃW�����v
       else{
         (*seq) = SEQ_LVUP_INFO_START;
       }
    }
    break;

  case SEQ_LVUP_GAUGE_WAIT:
    if( (!BTLV_EFFECT_CheckExecuteGauge())
    &&  (!PMSND_CheckPlaySE())
    ){
      BTLV_AddEffectByPos( wk->viewCore, vpos, BTLEFF_LVUP );
      (*seq) = SEQ_LVUP_EFFECT_WAIT;
    }
    break;
  case SEQ_LVUP_EFFECT_WAIT:
    if( BTLV_WaitEffectByPos(wk->viewCore, vpos) )
    {
      (*seq) = SEQ_LVUP_INFO_START;
    }
    break;

  case SEQ_LVUP_INFO_START:
    BTLV_STRPARAM_Setup( &wk->strParam, BTL_STRTYPE_STD, BTL_STRID_STD_LevelUp );
    BTLV_STRPARAM_AddArg( &wk->strParam, pokeID );
    BTLV_STRPARAM_AddArg( &wk->strParam, lvupInfo.level );
    BTLV_StartMsg( wk->viewCore, &wk->strParam );
    (*seq) = SEQ_LVUP_INFO_MSG_WAIT;
    break;

  case SEQ_LVUP_INFO_MSG_WAIT:
    if( BTLV_IsJustDoneMsg(wk->viewCore) ){
      PMSND_PauseBGM( TRUE );
      PMSND_PushBGM();
      PMSND_PlayBGM( SEQ_ME_LVUP );
    }
    if( BTLV_WaitMsg(wk->viewCore) && !PMSND_CheckPlayBGM() ){
      PMSND_PopBGM();
      PMSND_PauseBGM( FALSE );
      subSeq = 0;
      (*seq) = SEQ_LVUP_INFO_PARAM_START;
    }
    break;

  case SEQ_LVUP_INFO_PARAM_START:
    ++subSeq;
    if( (GFL_UI_KEY_GetTrg() & (PAD_BUTTON_A|PAD_BUTTON_B))
    ||  (GFL_UI_TP_GetTrg())
    ||  (subSeq > 80)
    ){
      subSeq = 0;
      BTLV_LvupWin_StartDisp( wk->viewCore, bpp, &lvupInfo );
      (*seq) = SEQ_LVUP_INFO_PARAM_SEQ1;
    }
    break;

  case SEQ_LVUP_INFO_PARAM_SEQ1:
    if( BTLV_LvupWin_WaitDisp(wk->viewCore) )
    {
      if( (GFL_UI_KEY_GetTrg() & (PAD_BUTTON_A|PAD_BUTTON_B))
      ||  (GFL_UI_TP_GetTrg())
      ){
        BTLV_LvupWin_StepFwd( wk->viewCore );
        (*seq) = SEQ_LVUP_INFO_PARAM_SEQ2;
      }
    }
    break;

  case SEQ_LVUP_INFO_PARAM_SEQ2:
    if( BTLV_LvupWin_WaitFwd(wk->viewCore) )
    {
      if( (GFL_UI_KEY_GetTrg() & (PAD_BUTTON_A|PAD_BUTTON_B))
      ||  (GFL_UI_TP_GetTrg())
      ){
        BTLV_LvupWin_StartHide( wk->viewCore );
        (*seq) = SEQ_LVUP_INFO_PARAM_END;
      }
    }
    break;

  case SEQ_LVUP_INFO_PARAM_END:
    if( BTLV_LvupWin_WaitHide(wk->viewCore) ){
      subSeq = 0;
      (*seq) = SEQ_LVUP_WAZAOBOE_WAIT;
    }
    break;

  case SEQ_LVUP_WAZAOBOE_WAIT:
    if( wazaOboeSeq(wk, &subSeq, bpp) ){
      (*seq) = SEQ_ADD_ROOT;
    }
    break;

  case SEQ_END:
    return TRUE;

  }
  return FALSE;
}
/**
 * ���U�o������
 *  �i�o���l���Z���� scProc_ACT_Exp �̃T�u�V�[�P���X�j
 */
static BOOL wazaOboeSeq( BTL_CLIENT* wk, int* seq, BTL_POKEPARAM* bpp )
{
  static int wazaoboe_index;
  static WazaID wazaoboe_no;

  u8 pokeID = BPP_GetID( bpp );

  switch( *seq ){
  case 0:
    wazaoboe_index = 0;
    wazaoboe_no = 0;
    (*seq) = 4;
    /* fallthru */

  case 4:
    //�Z�o���`�F�b�N
    {
      const POKEMON_PARAM* pp = BPP_GetSrcData( bpp );
      wazaoboe_no = PP_CheckWazaOboe( ( POKEMON_PARAM* )pp, &wazaoboe_index, wk->heapID );
      if( wazaoboe_no == PTL_WAZAOBOE_NONE )
      {
        return TRUE;
      }
      else if( wazaoboe_no == PTL_WAZASET_SAME )
      {
        break;
      }
      else if( wazaoboe_no & PTL_WAZAOBOE_FULL )
      {
        wazaoboe_no &= ( PTL_WAZAOBOE_FULL ^ 0xffff );
        (*seq) = 7;
      }
      else
      {
        (*seq) = 5;
      }
    }
    break;
  case 5:
    {
      BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, pokeID );
      const POKEMON_PARAM* pp = BPP_GetSrcData( bpp );
      BPP_ReflectByPP( bpp );
      BTLV_STRPARAM_Setup( &wk->strParam, BTL_STRTYPE_WAZAOBOE, msg_waza_oboe_04 );
      BTLV_STRPARAM_AddArg( &wk->strParam, pokeID );
      BTLV_STRPARAM_AddArg( &wk->strParam, wazaoboe_no );
      BTLV_StartMsg( wk->viewCore, &wk->strParam );
      (*seq)++;
    }
    /* fallthru */
  case 6:
    //�Z�o������
    if( BTLV_IsJustDoneMsg(wk->viewCore) ){
      PMSND_PauseBGM( TRUE );
      PMSND_PushBGM();
      PMSND_PlayBGM( SEQ_ME_LVUP );
    }
    if( BTLV_WaitMsg(wk->viewCore) && !PMSND_CheckPlayBGM() ){
      PMSND_PopBGM();
      PMSND_PauseBGM( FALSE );
      (*seq) = 4;
    }
    break;
  case 7:
    //�Z�Y��m�F�����u�����o���邽�߂ɑ��̃��U��Y�ꂳ���܂����H�v
    BTLV_STRPARAM_Setup( &wk->strParam, BTL_STRTYPE_WAZAOBOE, msg_waza_oboe_05 );
    BTLV_STRPARAM_AddArg( &wk->strParam, pokeID );
    BTLV_STRPARAM_AddArg( &wk->strParam, wazaoboe_no );
    BTLV_StartMsg( wk->viewCore, &wk->strParam );
    (*seq)++;
    /* fallthru */
  case 8:
    if( BTLV_IsJustDoneMsg(wk->viewCore) ){
      BTLV_STRPARAM   yesParam;
      BTLV_STRPARAM   noParam;
      BTLV_STRPARAM_Setup( &yesParam, BTL_STRTYPE_YESNO, msgid_yesno_wazawasureru );
      BTLV_STRPARAM_Setup( &noParam, BTL_STRTYPE_YESNO, msgid_yesno_wazawasurenai );
      BTLV_YESNO_Start( wk->viewCore, &yesParam, &noParam );
    }
    if( BTLV_WaitMsg(wk->viewCore) ){
      (*seq)++;
    }
    break;
  case 9:
    {
      BtlYesNo result;

      if( BTLV_YESNO_Wait( wk->viewCore, &result ) )
      {
        if( result == BTL_YESNO_YES )
        {
          const BTL_PARTY* party = BTL_POKECON_GetPartyDataConst( wk->pokeCon, wk->myID );
          int index = BTL_PARTY_FindMemberByPokeID( party, pokeID );
          BTLV_WAZAWASURE_Start( wk->viewCore, index, wazaoboe_no );
          (*seq) = 10;
        }
        else
        {
          BTLV_STRPARAM_Setup( &wk->strParam, BTL_STRTYPE_WAZAOBOE, msg_waza_oboe_08 );
          BTLV_STRPARAM_AddArg( &wk->strParam, pokeID );
          BTLV_STRPARAM_AddArg( &wk->strParam, wazaoboe_no );
          BTLV_StartMsg( wk->viewCore, &wk->strParam );
          (*seq) = 100;
        }
      }
    }
    break;
  case 10:
    //�Z�Y��I������
    {
      u8 result;

      if( BTLV_WAZAWASURE_Wait( wk->viewCore, &result ) )
      {
        if( result == BPL_SEL_WP_CANCEL )
        {
          (*seq) = 6;
        }
        else
        {
          BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, pokeID );
          const POKEMON_PARAM* pp = BPP_GetSrcData( bpp );
          WazaID forget_wazano = PP_Get( pp, ID_PARA_waza1 + result, NULL );
          PP_SetWazaPos( ( POKEMON_PARAM* )pp, wazaoboe_no, result );
          BTLV_STRPARAM_Setup( &wk->strParam, BTL_STRTYPE_WAZAOBOE, msg_waza_oboe_06 );
          BTLV_STRPARAM_AddArg( &wk->strParam, pokeID );
          BTLV_STRPARAM_AddArg( &wk->strParam, forget_wazano );
          BTLV_StartMsg( wk->viewCore, &wk->strParam );
          (*seq)++;
        }
      }
    }
    break;
  case 11:
    //�Z�Y�ꏈ��
    if( BTLV_WaitMsg(wk->viewCore) ){
      (*seq) = 5;
    }
    break;
  case 100:
    //�Z�Y�ꂠ����ߊm�F�����u�ł́������o����̂�������߂܂����H�v
    if( BTLV_IsJustDoneMsg(wk->viewCore) ){
      BTLV_STRPARAM   yesParam;
      BTLV_STRPARAM   noParam;
      BTLV_STRPARAM_Setup( &yesParam, BTL_STRTYPE_YESNO, msgid_yesno_wazaakirameru );
      BTLV_STRPARAM_AddArg( &yesParam, wazaoboe_no );
      BTLV_STRPARAM_Setup( &noParam, BTL_STRTYPE_YESNO, msgid_yesno_wazaakiramenai );
      BTLV_STRPARAM_AddArg( &noParam, wazaoboe_no );
      BTLV_YESNO_Start( wk->viewCore, &yesParam, &noParam );
    }
    if( BTLV_WaitMsg(wk->viewCore) ){
      (*seq)++;
    }
    break;
  case 101:
    {
      BtlYesNo result;

      if( BTLV_YESNO_Wait( wk->viewCore, &result ) )
      {
        // ������߂�
        if( result == BTL_YESNO_YES )
        {
          BTLV_STRPARAM_Setup( &wk->strParam, BTL_STRTYPE_WAZAOBOE, msg_waza_oboe_09 );
          BTLV_STRPARAM_AddArg( &wk->strParam, pokeID );
          BTLV_STRPARAM_AddArg( &wk->strParam, wazaoboe_no );
          BTLV_StartMsg( wk->viewCore, &wk->strParam );
          (*seq) = 102;
        }
        // ������߂Ȃ�
        else
        {
          (*seq) = 7;
        }
      }
    }
    break;
  case 102:
    if( BTLV_WaitMsg(wk->viewCore) ){
      (*seq) = 4;
    }
    break;
  }
  return FALSE;
}
//---------------------------------------------------------------------------------------
/**
 *  �����X�^�[�{�[��������
 *  args .. [0]:�Ώۃ|�P�ʒu  [1]:����  [2]:�ߊl�����t���O [3]: �}�ӓo�^�t���O [4]:�N���e�B�J���t���O
 *           [5]:�{�[���̃A�C�e���i���o�[
 */
//---------------------------------------------------------------------------------------
static BOOL scProc_ACT_BallThrow( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    {
      u8 vpos;

      vpos = BTL_MAIN_BtlPosToViewPos( wk->mainModule, args[0] );
      BTLV_EFFECT_BallThrow( vpos, args[5], args[1], args[2], args[4] );
      (*seq)++;
    }
    break;
  case 1:
    if( !BTLV_EFFECT_CheckExecute() )
    {
      u16 strID;

      // �ߊl�������b�Z�[�W
      if( args[2] )
      {
        const BTL_POKEPARAM* bpp = BTL_POKECON_GetFrontPokeDataConst( wk->pokeCon, args[0] );
        BTLV_STRPARAM_Setup( &wk->strParam, BTL_STRTYPE_STD, BTL_STRID_STD_BallThrowS );
        BTLV_STRPARAM_AddArg( &wk->strParam, BPP_GetID( bpp ) );
      }
      // �ߊl���s���b�Z�[�W
      else
      {
        static const u16 strTbl[] = {
          BTL_STRID_STD_BallThrow0, BTL_STRID_STD_BallThrow1, BTL_STRID_STD_BallThrow2, BTL_STRID_STD_BallThrow3,
        };

        if( args[1] < NELEMS(strTbl) ){
          strID = strTbl[ args[1] ];
        }else{
          strID = strTbl[ 0 ];
        }

        BTLV_STRPARAM_Setup( &wk->strParam, BTL_STRTYPE_STD, strID );

      }
      BTLV_StartMsg( wk->viewCore, &wk->strParam );
      (*seq)++;
    }
    break;
  case 2:
    if( BTLV_WaitMsg(wk->viewCore) )
    {
      if( args[2] ){
        PMSND_PlayBGM( SEQ_ME_POKEGET );
        (*seq)++;
      }else{
        return TRUE;
      }
    }
    break;
  case 3:
    if( !PMSND_CheckPlayBGM() )
    {
      //�퓬�G�t�F�N�g�ꎞ��~������
      BTLV_EFFECT_Restart();
      PMSND_PlayBGM( SEQ_BGM_WIN1 );
      // �}�ӓo�^���b�Z�[�W
      if( args[3] )
      {
        const BTL_POKEPARAM* bpp = BTL_POKECON_GetFrontPokeDataConst( wk->pokeCon, args[0] );
        BTLV_STRPARAM_Setup( &wk->strParam, BTL_STRTYPE_STD, BTL_STRID_STD_ZukanRegister );
        BTLV_STRPARAM_AddArg( &wk->strParam, BPP_GetID( bpp ) );
        BTLV_StartMsg( wk->viewCore, &wk->strParam );
        (*seq)++;
      }
      else{
        return TRUE;
      }
    }
    break;
  case 4:
    if( BTLV_WaitMsg(wk->viewCore) ){
      (*seq)++;
    }
    break;
  default:
      return TRUE;
  }
  return FALSE;
}

//---------------------------------------------------------------------------------------
/**
 *  ���[�e�[�V��������
 *  args .. [0]:�ΏۃN���C�A���gID  [1]:���[�e�[�V�������� (BtlRotateDir)
 */
//---------------------------------------------------------------------------------------
static BOOL scProc_ACT_Rotation( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    {
      u8 clientID = args[0];
      BtlRotateDir dir = args[1];
      BTL_PARTY*  party = BTL_POKECON_GetPartyData( wk->pokeCon, clientID );

      BTL_N_Printf( DBGSTR_CLIENT_StartRotAct, clientID, dir );

      BTL_PARTY_RotateMembers( party, dir, NULL, NULL );
      BTLV_RotationMember_Start( wk->viewCore, clientID, dir );
      (*seq)++;
    }
    break;

  case 1:
    if( BTLV_RotationMember_Wait(wk->viewCore) )
    {
      BtlRotateDir dir = args[1];

      BTL_N_Printf( DBGSTR_CLIENT_EndRotAct, args[0], args[1] );
      if( dir == BTL_ROTATEDIR_STAY ){
        return TRUE;
      }
      else
      {
        u8  clientID = args[0];
        u8  fNPC = BTL_MAIN_IsClientNPC( wk->mainModule, clientID );
        u16 strID;

        if( dir == BTL_ROTATEDIR_L ){
          strID = (fNPC)? BTL_STRID_STD_RotateL_NPC : BTL_STRID_STD_RotateL_Player;
        }else{
          strID = (fNPC)? BTL_STRID_STD_RotateR_NPC : BTL_STRID_STD_RotateR_Player;
        }

        BTLV_StartMsgStd( wk->viewCore, strID, args );
        (*seq)++;
      }
    }
    break;

  case 2:
    if( BTLV_WaitMsg(wk->viewCore) ){
      return TRUE;
    }
    break;
  }
  return FALSE;
}
//---------------------------------------------------------------------------------------
/**
 *  �Ƃ��������������A�N�V����
 *  args .. [0]:�Ώۂ̃|�P����ID [1]:����������̂Ƃ�����
 */
//---------------------------------------------------------------------------------------
static BOOL scProc_ACT_ChangeTokusei( BTL_CLIENT* wk, int* seq, const int* args )
{
  u8 pokeID = args[0];
  BtlPokePos pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, pokeID );

  switch( *seq ){
  case 0:
    {
      BTLV_TokWin_DispStart( wk->viewCore, pos, FALSE );
      (*seq)++;
    }
    break;
  case 1:
    if( BTLV_TokWin_DispWait( wk->viewCore, pos ) )
    {
      BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, pokeID );
      BPP_ChangeTokusei( bpp, args[1] );
      BTLV_TokWin_Renew_Start( wk->viewCore, pos );
      (*seq)++;
    }
    break;
  case 2:
    if( BTLV_TokWin_Renew_Wait( wk->viewCore, pos ) )
    {
      (*seq)++;
    }
    break;
  default:
    return TRUE;
  }
  return FALSE;
}
//---------------------------------------------------------------------------------------
/**
 *  �C�����[�W������Ԃ�����
 *  args .. [0]:�Ώۂ̃|�P����ID
 */
//---------------------------------------------------------------------------------------
static BOOL scProc_ACT_FakeDisable( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    {
      u8 pokeID = args[0];
      BtlPokePos pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, pokeID );
      BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, pokeID );

      BPP_FakeDisable( bpp );
      BTLV_FakeDisable_Start( wk->viewCore, pos );
      (*seq)++;
    }
    break;
  case 1:
    if( BTLV_FakeDisable_Wait(wk->viewCore) )
    {
      (*seq)++;
    }
    break;
  default:
    return TRUE;
  }
  return FALSE;
}
//---------------------------------------------------------------------------------------
/**
 *  �w��ʒu�ɃG�t�F�N�g����
 *  args .. [0]:�ʒu�w��  [1]:�G�t�F�N�g�w��
 */
//---------------------------------------------------------------------------------------
static BOOL scProc_ACT_EffectByPos( BTL_CLIENT* wk, int* seq, const int* args )
{
  BtlvMcssPos vpos = BTL_MAIN_BtlPosToViewPos( wk->mainModule, args[0] );

  switch( *seq ){
  case 0:
    if( BTL_CLIENT_IsChapterSkipMode(wk) ){
      return TRUE;
    }

    BTLV_AddEffectByPos( wk->viewCore, vpos, args[1] );
    (*seq)++;
    break;

  default:
    if( BTLV_WaitEffectByPos( wk->viewCore, vpos ) ){
      return TRUE;
    }
    break;
  }
  return FALSE;
}
//---------------------------------------------------------------------------------------
/**
 *  �w��ʒu -> �w��ʒu �ɃG�t�F�N�g����
 *  args .. [0]:�J�n�ʒu�w��  [1]:�I�[�ʒu�w��  [2]:�G�t�F�N�g�w��
 */
//---------------------------------------------------------------------------------------
static BOOL scProc_ACT_EffectByVector( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    if( BTL_CLIENT_IsChapterSkipMode(wk) ){
      return TRUE;
    }

    {
      BtlvMcssPos vpos_from = BTL_MAIN_BtlPosToViewPos( wk->mainModule, args[0] );
      BtlvMcssPos vpos_to   = BTL_MAIN_BtlPosToViewPos( wk->mainModule, args[1] );

      BTLV_AddEffectByDir( wk->viewCore, vpos_from, vpos_to, args[2] );
      (*seq)++;
    }
    break;

  default:
    if( BTLV_WaitEffectByDir(wk->viewCore) ){
      return TRUE;
    }
    break;
  }
  return FALSE;
}
//---------------------------------------------------------------------------------------
/**
 *  �w��|�P�����̃t�H�����i���o�[�`�F���W
 *  args .. [0]:pokeID  [1]:formNo
 */
//---------------------------------------------------------------------------------------
static BOOL scProc_ACT_ChangeForm( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    {
      BtlPokePos pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, args[0] );
      BtlvMcssPos vpos = BTL_MAIN_BtlPosToViewPos( wk->mainModule, pos );
      BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );

      BPP_ChangeForm( bpp, args[1] );
      BTLV_ChangeForm_Start( wk->viewCore, vpos );
      (*seq)++;
    }
    break;
  case 1:
    if( BTLV_ChangeForm_Wait(wk->viewCore) )
    {
      (*seq)++;
    }
    break;
  default:
    return TRUE;

  }
  return FALSE;
}



//---------------------------------------------------------------------------------------
/**
 *  �Ƃ������E�B���h�E�\���I��
 */
//---------------------------------------------------------------------------------------
static BOOL scProc_TOKWIN_In( BTL_CLIENT* wk, int* seq, const int* args )
{
  BtlPokePos pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, args[0] );
  switch( *seq ){
  case 0:
    if( BTL_CLIENT_IsChapterSkipMode(wk) ){
      return TRUE;
    }

    BTLV_TokWin_DispStart( wk->viewCore, pos, TRUE );
    (*seq)++;
    break;

  case 1:
    if( BTLV_TokWin_DispWait(wk->viewCore, pos) ){
      (*seq)++;
    }
    break;

  default:
    return TRUE;
  }
  return FALSE;
}
//---------------------------------------------------------------------------------------
/**
 *  �Ƃ������E�B���h�E�\���I�t
 */
//---------------------------------------------------------------------------------------
static BOOL scProc_TOKWIN_Out( BTL_CLIENT* wk, int* seq, const int* args )
{
  BtlPokePos pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, args[0] );
  switch( *seq ){
  case 0:
    BTLV_QuitTokWin( wk->viewCore, pos );
    (*seq)++;
    break;
  case 1:
    if( BTLV_QuitTokWinWait(wk->viewCore, pos) ){
      return TRUE;
    }
    break;
  }
  return FALSE;
}

static BOOL scProc_OP_HpMinus( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_HpMinus( pp, args[1] );
  return TRUE;
}


static BOOL scProc_OP_HpPlus( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_HpPlus( pp, args[1] );
  return TRUE;
}
static BOOL scProc_OP_PPMinus( BTL_CLIENT* wk, int* seq, const int* args )
{
  u8 pokeID = args[0];
  u8 wazaIdx = args[1];
  u8 value = args[2];

  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, pokeID );
  BPP_PPMinus( pp, wazaIdx, value );

  return TRUE;
}
static BOOL scProc_OP_HpZero( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_HpZero( pp );
  return TRUE;
}
/**
 *  PP��  args[0]:pokeID  args[1]:WazaIdx  args[2]:��PP�l
 */
static BOOL scProc_OP_PPPlus( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_PPPlus( bpp, args[1], args[2] );
  return TRUE;
}
static BOOL scProc_OP_RankUp( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_RankUp( pp, args[1], args[2] );
  return TRUE;
}
static BOOL scProc_OP_RankDown( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_RankDown( pp, args[1], args[2] );
  return TRUE;
}
static BOOL scProc_OP_RankSet7( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_RankSet( bpp, BPP_ATTACK_RANK,      args[1] );
  BPP_RankSet( bpp, BPP_DEFENCE_RANK,     args[2] );
  BPP_RankSet( bpp, BPP_SP_ATTACK_RANK,   args[3] );
  BPP_RankSet( bpp, BPP_SP_DEFENCE_RANK,  args[4] );
  BPP_RankSet( bpp, BPP_AGILITY_RANK,     args[5] );
  BPP_RankSet( bpp, BPP_HIT_RATIO,        args[6] );
  BPP_RankSet( bpp, BPP_AVOID_RATIO,      args[7] );
  return TRUE;
}
static BOOL scProc_OP_RankRecover( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_RankRecover( bpp );
  return TRUE;
}
static BOOL scProc_OP_RankReset( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_RankReset( bpp );
  return TRUE;
}
static BOOL scProc_OP_AddCritical( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_AddCriticalRank( pp, args[1] );
  return TRUE;
}
static BOOL scProc_OP_SickSet( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_SICK_CONT cont;
  cont.raw = args[2];

  BPP_SetWazaSick( pp, args[1], cont );
  return TRUE;
}
static BOOL scProc_OP_CurePokeSick( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_CurePokeSick( pp );
  return TRUE;
}
static BOOL scProc_OP_CureWazaSick( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_CureWazaSick( pp, args[1] );
  return TRUE;
}
static BOOL scProc_OP_MemberIn( BTL_CLIENT* wk, int* seq, const int* args )
{
  u8 clientID = wk->cmdArgs[0];
  u8 posIdx = wk->cmdArgs[1];
  u8 memberIdx = wk->cmdArgs[2];

  {
    BTL_PARTY* party = BTL_POKECON_GetPartyData( wk->pokeCon, clientID );

    BTL_POKEPARAM* bpp;
    if( posIdx != memberIdx ){
      BTL_PARTY_SwapMembers( party, posIdx, memberIdx );
    }
    BTL_Printf("�����o�[�C�� �ʒu %d <- %d �ɂ����|�P\n", posIdx, memberIdx);
    bpp = BTL_PARTY_GetMemberData( party, posIdx );
    BPP_SetAppearTurn( bpp, args[3] );
    BPP_Clear_ForIn( bpp );
  }
  return TRUE;
}
static BOOL scProc_OP_ChangePokeType( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_ChangePokeType( pp, args[1] );
  return TRUE;
}
static BOOL scProc_OP_WSTurnCheck( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_WazaSick_TurnCheck( bpp, NULL, NULL );
  return TRUE;
}
static BOOL scProc_OP_ConsumeItem( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_ConsumeItem( pp );
  return TRUE;
}
static BOOL scProc_OP_UpdateUseWaza( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );

  {
    BTL_N_Printf( DBGSTR_CLIENT_UpdateWazaProcResult, args[0], args[4], args[1], args[2], args[3] );
  }

  BPP_UpdateWazaProcResult( bpp, args[1], args[2], args[3], args[4] );
  return TRUE;
}
/**
 *  �p���t���O�Z�b�g  args[0]=pokeID, args[1]=flagID
 */
static BOOL scProc_OP_SetContFlag( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_CONTFLAG_Set( pp, args[1] );
  return TRUE;
}
static BOOL scProc_OP_ResetContFlag( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_CONTFLAG_Clear( pp, args[1] );
  return TRUE;
}
static BOOL scProc_OP_SetTurnFlag( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_TURNFLAG_Set( pp, args[1] );
  return TRUE;
}
static BOOL scProc_OP_ResetTurnFlag( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_TURNFLAG_ForceOff( pp, args[1] );
  return TRUE;
}
static BOOL scProc_OP_ChangeTokusei( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_ChangeTokusei( pp, args[1] );
  return TRUE;
}
static BOOL scProc_OP_SetItem( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_SetItem( pp, args[1] );
  return TRUE;
}
static BOOL scProc_OP_UpdateWazaNumber( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  // �T�[�o�R�}���h���M���̓s���ň������w���ȕ��тɂȂ��Ă�c
  BPP_WAZA_UpdateID( pp, args[1], args[4], args[2], args[3] );
  return TRUE;
}
static BOOL scProc_OP_Hensin( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* atkPoke = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BTL_POKEPARAM* tgtPoke = BTL_POKECON_GetPokeParam( wk->pokeCon, args[1] );

  BPP_HENSIN_Set( atkPoke, tgtPoke );
  return TRUE;
}
static BOOL scProc_OP_OutClear( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_Clear_ForOut( pp );
  return TRUE;
}
static BOOL scProc_OP_AddFldEff( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_CALC_BITFLG_Set( wk->fieldEffectFlag, args[0] );
  return TRUE;
}
static BOOL scProc_OP_RemoveFldEff( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_CALC_BITFLG_Off( wk->fieldEffectFlag, args[0] );
  return TRUE;
}
static BOOL scProc_OP_SetPokeCounter( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_COUNTER_Set( bpp, args[1], args[2] );
  return TRUE;
}
static BOOL scProc_OP_BatonTouch( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* user = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BTL_POKEPARAM* target = BTL_POKECON_GetPokeParam( wk->pokeCon, args[1] );
  BPP_BatonTouchParam( target, user );
  return TRUE;
}
static BOOL scProc_OP_MigawariCreate( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_MIGAWARI_Create( bpp, args[1] );
  return TRUE;
}
static BOOL scProc_OP_MigawariDelete( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_MIGAWARI_Delete( bpp );
  return TRUE;
}
static BOOL scProc_OP_ShooterCharge( BTL_CLIENT* wk, int* seq, const int* args )
{
  u8 clientID = args[0];
  u8 increment = args[1];

  if( clientID == wk->myID )
  {
    wk->shooterEnergy += increment;
    if( wk->shooterEnergy > BTL_SHOOTER_ENERGY_MAX ){
      wk->shooterEnergy = BTL_SHOOTER_ENERGY_MAX;
    }
  }
  return TRUE;
}
static BOOL scProc_OP_SetFakeSrc( BTL_CLIENT* wk, int* seq, const int* args )
{
  u8 clientID = args[0];
  u8 memberIdx = args[1];

  BTL_PARTY* party = BTL_POKECON_GetPartyData( wk->pokeCon, clientID );
  BTL_PARTY_SetFakeSrcMember( party, memberIdx );
  return TRUE;
}
static BOOL scProc_OP_ClearConsumedItem( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_ClearConsumedItem( bpp );
  return TRUE;
}
/**
 *  ���U�_���[�W�L�^�ǉ�
 *  args [0]:defPokeID [1]:atkPokeID [2]:pokePos [3]:wazaType [4]:wazaID [5]:damage
 */
static BOOL scProc_OP_AddWazaDamage( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_WAZADMG_REC rec;

  BPP_WAZADMG_REC_Setup( &rec, args[1], args[2], args[4], args[3], args[5] );
  BPP_WAZADMGREC_Add( bpp, &rec );

  return TRUE;
}
/**
 *  �^�[���`�F�b�N����
 *  args [0]:pokeID
 */
static BOOL scProc_OP_TurnCheck( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );

  BPP_TurnCheck( bpp );
  BPP_CombiWaza_ClearParam( bpp );

  return TRUE;
}



/*
 *  �\�͊�{�l��������  [0]:pokeID, [1]:statusID [2]:value
 */
static BOOL scProc_OP_SetStatus( BTL_CLIENT* wk, int* seq, const int* args )
{
  BppValueID statusID = args[1];

  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_SetBaseStatus( bpp, statusID, args[2] );
  return TRUE;
}
/*
 *  �̏d�l��������  [0]:pokeID, [2]:weight
 */
static BOOL scProc_OP_SetWeight( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_SetWeight( bpp, args[1] );
  return TRUE;
}



//------------------------------------------------------------------------------------------------------
// �퓬���|�P�����̏��L�Ƃ������E�^�C�v���`�F�b�N
//------------------------------------------------------------------------------------------------------
static u8 countFrontPokeTokusei( BTL_CLIENT* wk, PokeTokusei tokusei )
{
  const BTL_POKEPARAM* bpp;
  int i, cnt;
  for(i=0, cnt=0; i<wk->numCoverPos; ++i){
    bpp = BTL_PARTY_GetMemberDataConst( wk->myParty, i );
    if( !BPP_IsDead(bpp) )
    {
      if( BPP_GetValue(bpp, BPP_TOKUSEI_EFFECTIVE) == tokusei ){ ++cnt; }
    }
  }
  return cnt;
}
static u8 countFrontPokeType( BTL_CLIENT* wk, PokeType type )
{
  const BTL_POKEPARAM* bpp;
  int i, cnt;
  for(i=0, cnt=0; i<wk->numCoverPos; ++i){
    bpp = BTL_PARTY_GetMemberDataConst( wk->myParty, i );
    if( !BPP_IsDead(bpp) )
    {
      if( BPP_IsMatchType(bpp, type) ){ ++cnt; }
    }
  }
  return cnt;
}


//------------------------------------------------------------------------------------------------------
// �O�����W���[������̏��擾����
//------------------------------------------------------------------------------------------------------
u8 BTL_CLIENT_GetClientID( const BTL_CLIENT* client )
{
  return client->myID;
}

const BTL_PARTY* BTL_CLIENT_GetParty( const BTL_CLIENT* client )
{
  return client->myParty;
}


//=============================================================================================
/**
 * ���݁A�s���I���������̃|�P�����ʒuID���擾
 *
 * @param   client
 *
 * @retval  BtlPokePos
 */
//=============================================================================================
BtlPokePos BTL_CLIENT_GetProcPokePos( const BTL_CLIENT* client )
{
  return client->basePos + client->procPokeIdx*2;
}

//------------------------------------------------------------------------------------------------------
// AI�A�C�e��
//------------------------------------------------------------------------------------------------------

/**
 *  ���[�N������
 */
static void AIItem_Setup( BTL_CLIENT* wk )
{
  u32 i;

  for(i=0; i<NELEMS(wk->AIItem); ++i){
    wk->AIItem[i] = BTL_MAIN_GetClientUseItem( wk->mainModule, wk->myID, i );
  }
}

/**
 *  �g�p�A�C�e���`�F�b�N
 */
static u16 AIItem_CheckUse( BTL_CLIENT* wk, const BTL_POKEPARAM* bpp, const BTL_PARTY* party )
{
  static const u8 borderMembers[] = {
    BTL_PARTY_MEMBER_MAX, 4, 2, 1,
  };
  u32 i, numMembers = BTL_PARTY_GetMemberCount( party );

  for(i=0; i<NELEMS(wk->AIItem); ++i)
  {
    if( borderMembers[i] < numMembers ){
      break;
    }
    if( wk->AIItem[i] != ITEM_DUMMY_DATA )
    {
      BOOL fUse = FALSE;
      // HP�񕜌n
      if( BTL_CALC_ITEM_GetParam(wk->AIItem[i], ITEM_PRM_HP_RCV) )
      {
        u32 hp = BPP_GetValue( bpp, BPP_HP );
        if( hp <= BTL_CALC_QuotMaxHP(bpp, 4) ){
          fUse = TRUE;
        }
        else{
          continue;
        }
      }
      // �\�̓����N�A�b�v�n
      else if( check_status_up_item(wk->AIItem[i], bpp) )
      {
        fUse = TRUE;
      }
      // ��Ԉُ�񕜌n
      else if( check_cure_sick_item(wk->AIItem[i], bpp) )
      {
        fUse = TRUE;
      }

      if( fUse )
      {
        u16 useItem = wk->AIItem[i];
        wk->AIItem[i] = ITEM_DUMMY_DATA;
        return useItem;
      }
    }
  }

  return ITEM_DUMMY_DATA;
}
/**
 *  �\�̓����N�A�b�v�n�A�C�e���ł��違�g���Č��ʂ����锻��
 */
static BOOL check_status_up_item( u16 itemID, const BTL_POKEPARAM* bpp )
{
  static const struct {
    u8 itemParamID;
    u8 rankID;
  }check_tbl[] = {
    { ITEM_PRM_ATTACK_UP,     BPP_ATTACK_RANK     },   // �U���̓A�b�v
    { ITEM_PRM_DEFENCE_UP,    BPP_DEFENCE_RANK    },   // �h��̓A�b�v
    { ITEM_PRM_SP_ATTACK_UP,  BPP_SP_ATTACK_RANK  },   // ���U�A�b�v
    { ITEM_PRM_SP_DEFENCE_UP, BPP_SP_DEFENCE_RANK },   // ���h�A�b�v
    { ITEM_PRM_AGILITY_UP,    BPP_AGILITY_RANK    },   // �f�����A�b�v
    { ITEM_PRM_HIT_UP,        BPP_HIT_RATIO       },   // �������A�b�v
  };
  u32 i;

  for(i=0; i<NELEMS(check_tbl); ++i)
  {
    if( BTL_CALC_ITEM_GetParam(itemID, check_tbl[i].itemParamID) )
    {
      if( BPP_IsRankEffectValid(bpp, check_tbl[i].rankID, 1) ){
        return TRUE;
      }
    }
  }

 // �N���e�B�J�������ʔ���
  if( BTL_CALC_ITEM_GetParam(itemID, ITEM_PRM_CRITICAL_UP) )
  {
    if( BPP_GetCriticalRank(bpp) == 0 ){
      return TRUE;
    }
  }

  return FALSE;
}

/**
 *  ��Ԉُ�񕜌n�A�C�e���ł��違�g���Č��ʂ����锻��
 */
static BOOL check_cure_sick_item( u16 itemID, const BTL_POKEPARAM* bpp )
{
  static const struct {
    u8 itemParamID;
    u8 sickID;
  }check_tbl[] = {
    { ITEM_PRM_SLEEP_RCV,     WAZASICK_NEMURI      },   // �����
    { ITEM_PRM_POISON_RCV,    WAZASICK_DOKU        },   // �ŉ�
    { ITEM_PRM_BURN_RCV,      WAZASICK_YAKEDO      },   // �Ώ���
    { ITEM_PRM_ICE_RCV,       WAZASICK_KOORI       },   // �X��
    { ITEM_PRM_PARALYZE_RCV,  WAZASICK_MAHI        },   // ��჉�
    { ITEM_PRM_PANIC_RCV,     WAZASICK_KONRAN      },   // ������
    { ITEM_PRM_MEROMERO_RCV,  WAZASICK_MEROMERO    },   // ����������
  };
  u32 i;

  for(i=0; i<NELEMS(check_tbl); ++i)
  {
    if( BTL_CALC_ITEM_GetParam(itemID, check_tbl[i].itemParamID) )
    {
      if( BPP_CheckSick(bpp, check_tbl[i].sickID) ){
        return TRUE;
      }
    }
  }

  return FALSE;
}

//------------------------------------------------------------------------------------------------------
// �^��f�[�^�v���C���[
//------------------------------------------------------------------------------------------------------

/**
 *  �̈揉�����i�Đ����łȂ��Ă��A������Ăяo���ď���������K�v������j
 */
static void RecPlayer_Init( RECPLAYER_CONTROL* ctrl )
{
  ctrl->ctrlCode = RECCTRL_NONE;
  ctrl->seq = 0;
  ctrl->fTurnIncrement = FALSE;
  ctrl->fFadeOutDone = FALSE;
  ctrl->fChapterSkip = FALSE;
  ctrl->turnCount = 0;
  ctrl->nextTurnCount = 0;
  ctrl->maxTurnCount = 0;
  ctrl->skipTurnCount = 0;
  ctrl->handlingTimer = 0;
}
/**
 *  �Đ����������i�Đ����ɂ̂݌Ăяo���j
 */
static void RecPlayer_Setup( RECPLAYER_CONTROL* ctrl, u32 turnCnt )
{
  ctrl->maxTurnCount = turnCnt;
}
/**
 *  ��~�܂��̓`���v�^�[����O�̃u���b�N�A�E�g����������������
 */
static BOOL RecPlayer_CheckBlackOut( const RECPLAYER_CONTROL* ctrl )
{
  return ctrl->fFadeOutDone;
}
/**
 *  �^�[���J�E���^�C���N�������g���N�G�X�g
 */
static void RecPlayer_TurnIncReq( RECPLAYER_CONTROL* ctrl )
{
  ctrl->fTurnIncrement = TRUE;
}

/**
 *  ��Ԏ擾
 */
static RecCtrlCode RecPlayer_GetCtrlCode( const RECPLAYER_CONTROL* ctrl )
{
  return ctrl->ctrlCode;
}
/**
 *  �`���v�^�[�X�L�b�v���[�h�֐؂�ւ�
 */
static void RecPlayer_ChapterSkipOn( RECPLAYER_CONTROL* ctrl, u32 nextTurnNum )
{
  ctrl->fChapterSkip = TRUE;
  ctrl->skipTurnCount = 0;
  ctrl->nextTurnCount = nextTurnNum;
  ctrl->fFadeOutDone = FALSE;
}
/**
 *
 */
static void RecPlayer_ChapterSkipOff( RECPLAYER_CONTROL* ctrl )
{
  ctrl->seq = 0;
  ctrl->fChapterSkip = FALSE;
}
/**
 *
 */
static BOOL RecPlayer_CheckChapterSkipEnd( const RECPLAYER_CONTROL* ctrl )
{
  return ( ctrl->skipTurnCount == ctrl->nextTurnCount );
}
/**
 *
 */
static u32 RecPlayer_GetNextTurn( const RECPLAYER_CONTROL* ctrl )
{
  return ctrl->nextTurnCount;
}

/**
 *  ���C���R���g���[���i�L�[�E�^�b�`�p�l���ɔ����A��ԑJ�ڂ���j
 */
static void RecPlayerCtrl_Main( BTL_CLIENT* wk, RECPLAYER_CONTROL* ctrl )
{
  enum {
    CHAPTER_CTRL_FRAMES = 45,
  };

  if( (wk->myType == BTL_CLIENT_TYPE_RECPLAY)
  &&  (ctrl->maxTurnCount)
  ){
    enum {
      SEQ_DEFAULT = 0,
      SEQ_FADEOUT,
      SEQ_STAY,
    };

    BOOL fTurnUpdate = FALSE;

    if( ctrl->fTurnIncrement )
    {
      ctrl->fTurnIncrement = FALSE;

      if( ctrl->fChapterSkip == FALSE )
      {
        if( ctrl->turnCount < ctrl->maxTurnCount ){
          ++(ctrl->turnCount);
          if(ctrl->handlingTimer == 0){
            ctrl->nextTurnCount = ctrl->turnCount;
          }
          fTurnUpdate = TRUE;
        }
      }
      else
      {
        if( ctrl->skipTurnCount < ctrl->nextTurnCount )
        {
          ctrl->skipTurnCount++;
          if( ctrl->skipTurnCount == ctrl->nextTurnCount )
          {
            ctrl->turnCount = ctrl->nextTurnCount;
            if( wk->viewCore ){
              BTLV_UpdateRecPlayerInput( wk->viewCore, ctrl->turnCount, ctrl->nextTurnCount );
            }
          }
        }
        return;
      }
    }

    // �ŏ��̃`���v�^�܂ŉ������Ȃ��i����G�t�F�N�g�҂��j
    if( ctrl->turnCount == 0 ){
//      BTLV_UpdateRecPlayerInput( wk->viewCore, ctrl->turnCount, ctrl->nextTurnCount );
      return;
    }

    // ���������A�`��N���C�A���g�ȊO�͉������Ȃ�
    if( wk->viewCore == NULL) {
      return;
    }

    switch( ctrl->seq ){
    case SEQ_DEFAULT:
      {
        int result = BTLV_CheckRecPlayerInput( wk->viewCore );
        BOOL fCtrlUpdate = FALSE;

        switch( result ){
        case  BTLV_INPUT_BR_SEL_STOP:     //��~
          ctrl->ctrlCode = RECCTRL_QUIT;
          BTLV_RecPlayFadeOut_Start( wk->viewCore );
          ctrl->seq = SEQ_FADEOUT;
          break;

        case BTLV_INPUT_BR_SEL_REW:
          if( ctrl->nextTurnCount > 1 ){
            ctrl->nextTurnCount--;
            fCtrlUpdate = TRUE;
          }
          ctrl->handlingTimer = CHAPTER_CTRL_FRAMES;
          break;

        case BTLV_INPUT_BR_SEL_FF:
          if( ctrl->nextTurnCount < ctrl->maxTurnCount ){
            ctrl->nextTurnCount++;
            fCtrlUpdate = TRUE;
          }
          ctrl->handlingTimer = CHAPTER_CTRL_FRAMES;
          break;
        }

        if( fCtrlUpdate || fTurnUpdate ){
          BTLV_UpdateRecPlayerInput( wk->viewCore, ctrl->turnCount, ctrl->nextTurnCount );
        }

        if( ctrl->handlingTimer )
        {
          if( --(ctrl->handlingTimer) == 0 )
          {
            ctrl->ctrlCode = RECCTRL_CHAPTER;
            BTLV_RecPlayFadeOut_Start( wk->viewCore );
            ctrl->seq = SEQ_FADEOUT;
          }
          break;
        }
      }
      break;

    case SEQ_FADEOUT:
      if( BTLV_RecPlayFadeOut_Wait(wk->viewCore) ){
        ctrl->fFadeOutDone = TRUE;
        ctrl->seq = SEQ_STAY;
      }
      break;

    case SEQ_STAY:
      break;
    }
  }
}
