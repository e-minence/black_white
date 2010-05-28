//=============================================================================================
/**
 * @file  btl_debug_print.h
 * @brief ポケモンWB バトルシステム デバッグPrint機能
 * @author  taya
 *
 * @date  2010.01.07  作成
 */
//=============================================================================================
#pragma once


typedef enum {
  BTL_PRINTTYPE_UNKNOWN,    ///< タイプ未確定
  BTL_PRINTTYPE_SERVER,     ///< タイプサーバー
  BTL_PRINTTYPE_CLIENT,     ///< タイプサーバー以外
  BTL_PRINTTYPE_STANDALONE, ///< タイプスタンドアロン実行
}BtlPrintType;

/**
 *  文字列リテラルID
 */
typedef enum {
  DBGSTR_SETUP_DONE = 0,
  DBGSTR_DEBUGFLAG_BIT,
  DBGSTR_ADAPTER_SEND_START,
  DBGSTR_ADAPTER_RECV_DONE,
  DBGSTR_ADAPTER_Create,
  DBGSTR_ADAPTER_RECVED_A_CLIENT,
  DBGSTR_MAIN_PokeConGetByPos,
  DBGSTR_MAIN_CheckHPByLvup,
  DBGSTR_MAIN_PerappVoiceAdded,
  DBGSTR_MAIN_PerappVoiceComplete,
  DBGSTR_MAIN_MultiAITrainer_SeqStart,
  DBGSTR_MAIN_MultiAITrainer_SendDone,

  DBGSTR_CLIENT_RETURN_CMD_START,
  DBGSTR_CLIENT_RETURN_CMD_DONE,
  DBGSTR_CLIENT_SelActPokeDead,
  DBGSTR_CLIENT_SelectActionSkip,
  DBGSTR_CLIENT_SelectActionRoot,
  DBGSTR_CLIENT_SelectActionStart,
  DBGSTR_CLIENT_SelectActionDone,
  DBGSTR_CLIENT_SelectActionBacktoRoot,
  DBGSTR_CLIENT_SelectAction_Pokemon,
  DBGSTR_CLIENT_SelectAction_Fight,
  DBGSTR_CLIENT_SelectChangePoke,
  DBGSTR_CLIENT_SelectChangePokeCancel,
  DBGSTR_CLIENT_ReturnSeqDone,
  DBGSTR_CLIENT_WazaLockInfo,
  DBGSTR_CLIENT_NoMorePuttablePoke,
  DBGSTR_CLIENT_NotDeadMember,
  DBGSTR_CLIENT_ChangePokeCmdInfo,
  DBGSTR_CLIENT_NumChangePokeBegin,
  DBGSTR_CLIENT_NumChangePokeResult,
  DBGSTR_CLIENT_PokeSelCnt,
  DBGSTR_CLIENT_PokeChangeIdx,
  DBGSTR_CLIENT_ForbidEscape_Kagefumi_Chk,
  DBGSTR_CLIENT_ForbidEscape_Kagefumi_Enable,
  DBGSTR_CLIENT_ForbidEscape_Arijigoku_Chk,
  DBGSTR_CLIENT_ForbidEscape_Arijigoku_Enable,
  DBGSTR_CLIENT_ForbidEscape_Jiryoku_Chk,
  DBGSTR_CLIENT_ForbidEscape_Jiryoku_Enable,
  DBGSTR_CLIENT_UpdateWazaProcResult,
  DBGSTR_CLIENT_CmdLimitTimeEnable,
  DBGSTR_CLIENT_CmdLimitTimeOver,
  DBGSTR_CLIENT_ForcePokeChange,
  DBGSTR_CLIENT_UpdateEnemyBaseHP,
  DBGSTR_CLIENT_ForceQuitByTimeLimit,
  DBGSTR_CLIENT_StartCmd,
  DBGSTR_CLIENT_RecvedQuitCmd,
  DBGSTR_CLIENT_ReplyToQuitCmd,
  DBGSTR_CLIENT_StartRotAct,
  DBGSTR_CLIENT_EndRotAct,
  DBGSTR_CLIENT_ReadRecAct,
  DBGSTR_CLIENT_ReadRecAct_Fight,
  DBGSTR_CLIENT_ReadRecAct_Change,
  DBGSTR_CLIENT_ReadRecAct_Move,
  DBGSTR_CLIENT_UnknownServerCmd,
  DBGSTR_CLIENT_RecPlayerBlackOut,
  DBGSTR_CLIENT_RecPlay_ChapterSkipped,
  DBGSTR_CLIENT_CHGAI_UkeTokStart,
  DBGSTR_CLIENT_CHGAI_UkeTokFind,
  DBGSTR_CLIENT_CHGAI_UkeTokDecide,
  DBGSTR_CLIENT_CHGAI_PreSortList,
  DBGSTR_CLIENT_CHGAI_AfterSortList,
  DBGSTR_CLIENT_CHGAI_SortListVal,
  DBGSTR_CLIENT_CHGAI_HOROBI,
  DBGSTR_CLIENT_CHGAI_FusigiNaMamori,
  DBGSTR_CLIENT_CHGAI_NoEffWaza,
  DBGSTR_CLIENT_CHGAI_Kodawari,
  DBGSTR_CLIENT_CHGAI_UkeTok,
  DBGSTR_CLIENT_CHGAI_SizenKaifuku,
  DBGSTR_CLIENT_CHGAI_WazaEff,
  DBGSTR_CLIENT_ROT_R,
  DBGSTR_CLIENT_ROT_L,
  DBGSTR_CLIENT_ROT_Determine,
  DBGSTR_CLIENT_AI_PutPokeStart,
  DBGSTR_CLIENT_AI_PutPokeDecide,
  DBGSTR_CLIENT_CoverRotateDecide,
  DBGSTR_CLIENT_HPCheckByLvup,
  DBGSTR_CLIENT_OP_HPPlus,
  DBGSTR_CLIENT_OP_MemberIn,

  DBGSTR_BPP_NemuriWakeCheck,
  DBGSTR_BPP_WazaLinkOff,
  DBGSTR_BPP_WazaRecord,

  DBGSTR_MAIN_SwapPartyMember,
  DBGSTR_MAIN_PartyDataNotifyComplete,
  DBGSTR_MAIN_AIPartyDataSendComplete,
  DBGSTR_MAIN_SendAIPartyStart,
  DBGSTR_MAIN_SendAIPartyParam,
  DBGSTR_MAIN_RecvedMultiAITrainer,
  DBGSTR_MAIN_GetFrontPokeData,
  DBGSTR_MAIN_GetFrontPokeDataResult,
  DBGSTR_MAIN_CommError,
  DBGSTR_MAIN_Illusion1st,
  DBGSTR_MAIN_Illusion2nd,

  DBGSTR_NET_Startup,
  DBGSTR_NET_DecideClientID,
  DBGSTR_NET_SendSucceed,
  DBGSTR_NET_RecvedServerParam,
  DBGSTR_NET_RecvedPartyData,
  DBGSTR_NET_PartyDataComplete,
  DBGSTR_NET_PerappVoiceComplete,
  DBGSTR_NET_SendCmdDone,
  DBGSTR_NET_RecvedAllClientsData,
  DBGSTR_NET_ReturnToServerTrying,
  DBGSTR_NET_RecvedClientData,
  DBGSTR_NET_RecvedServerVersion,
  DBGSTR_NET_ServerDetermine,
  DBGSTR_NET_SendAITrainerData,
  DBGSTR_NET_RecvAITrainerData,
  DBGSTR_NET_AIPartyInfo,
  DBGSTR_NET_CreateAIPartyRecvBuffer,
  DBGSTR_NET_RecvedAIPartyData,
  DBGSTR_NET_PrappVoiceGetBufDisable,
  DBGSTR_NET_PrappVoiceGetBufEnable,
  DBGSTR_NET_PerappVoiceCheckRaw,
  DBGSTR_NET_PerappVoiceRecvedEnable,
  DBGSTR_NET_PerappVoiceRecvedDisable,

  DBGSTR_CALCDMG_BaseDamage,
  DBGSTR_CALCDMG_WazaParam,
  DBGSTR_CALCDMG_RangeHosei,
  DBGSTR_CALCDMG_WeatherHosei,
  DBGSTR_CALCDMG_RAW_DAMAGE,
  DBGSTR_CALCDMG_Critical,
  DBGSTR_CALCDMG_RandomHosei,
  DBGSTR_CALCDMG_TypeMatchHosei,
  DBGSTR_CALCDMG_TypeAffInfo,
  DBGSTR_CALCDMG_DamageResult,
  DBGSTR_CALCDMG_DamageMarume,
  DBGSTR_STR_StdStrID,
  DBGSTR_STR_SetStrID,
  DBGSTR_STR_StdStrInfo,
  DBGSTR_STR_Arg,
  DBGSTR_STR_SetPoke1,
  DBGSTR_STR_SetPoke2,
  DBGSTR_PRINT_BufIdx,
  DBGSTR_STR_TagSetNumber,
  DBGSTR_STR_SetPokeNickname,
  DBGSTR_STR_SetTagWazaName,
  DBGSTR_STR_SetItemName,
  DBGSTR_SVFL_AtkPower,
  DBGSTR_SVFL_DefGuard,
  DBGSTR_SVFL_WazaPower,
  DBGSTR_SVFL_UncategoryWazaInfo,
  DBGSTR_SVFL_QueWritePtr,
  DBGSTR_SVFL_ActOrderStart,
  DBGSTR_SVFL_ActOrderEnd,
  DBGSTR_SVFL_ActOrder_Fight,
  DBGSTR_SVFL_ActOrder_Item,
  DBGSTR_SVFL_ActOrder_Change,
  DBGSTR_SVFL_ActOrder_Escape,
  DBGSTR_SVFL_ActOrder_Dead,
  DBGSTR_SVFL_ActOrder_Rotation,
  DBGSTR_SVFL_CorrectTarget_Info,
  DBGSTR_SVFL_CorrectResult,
  DBGSTR_SVFL_CorrectHitFarOn,
  DBGSTR_SVFL_CorrectHitFarOff,
  DBGSTR_SVFL_CorrectTargetNum,
  DBGSTR_SVFL_CorrectTargetDiff,
  DBGSTR_SVFL_CorrectTargetHP,
  DBGSTR_SVFL_ClientPokeStillAlive,
  DBGSTR_SVFL_ClientPokeDeadAll,
  DBGSTR_SVFL_PosEffAdd,
  DBGSTR_SVFL_PosEffDupFail,
  DBGSTR_SVFL_HitCheckInfo1,
  DBGSTR_SVFL_HitCheckInfo2,
  DBGSTR_SVFL_HitCheckInfo3,
  DBGSTR_SVFL_HitCheckInfo4,
  DBGSTR_SVFL_StartAfterPokeChange,
  DBGSTR_SVFL_PokeChangeRootInfo,
  DBGSTR_SVFL_UseItemCall,
  DBGSTR_SVFL_ChangePokeReqInfo,
  DBGSTR_SVFL_TurnStart_Result,
  DBGSTR_SVFL_ActOrderMainStart,
  DBGSTR_SVFL_ActOrderMainDropOut,
  DBGSTR_SVFL_ExpCalc_Base,
  DBGSTR_SVFL_ExpCalc_MetInfo,
  DBGSTR_SVFL_ExpCalc_DivideInfo,
  DBGSTR_SVFL_ExpCalc_Result,
  DBGSTR_SVFL_ExpAdjustCalc,
  DBGSTR_SVFL_RecDataSendComped,
  DBGSTR_SVFL_SendServerCmd,
  DBGSTR_SVFL_AllClientCmdPlayComplete,
  DBGSTR_SVFL_GotoQuit,
  DBGSTR_SVFL_ReqWazaCallActOrder,
  DBGSTR_SVFL_WazaExeFail_1,
  DBGSTR_SVFL_WazaExeFail_2,
  DBGSTR_SVFL_HandEx_AddSick,
  DBGSTR_SVFL_CombiWazaCheck,
  DBGSTR_SVFL_CombiWazaFound,
  DBGSTR_SVFL_CombiDecide,
  DBGSTR_SVFL_StartAfterPokeIn,
  DBGSTR_SVFL_AfterPokeIn_Alive,
  DBGSTR_SVFL_AfterPokeIn_Dead,
  DBGSTR_SVFL_PokeDead,
  DBGSTR_SVFL_AddSickFailCode,
  DBGSTR_SVFL_AddSickFixed,
  DBGSTR_SVFL_WazaExeStart,
  DBGSTR_SVFL_ActionSortInfo,
  DBGSTR_SVFL_ActionSortRotation,
  DBGSTR_SVFL_ActionSortGen,
  DBGSTR_SVFL_TrainerItemTarget,
  DBGSTR_SVFL_DoubleTargetIntr,
  DBGSTR_SVFL_DoubleTargetRegister,
  DBGSTR_SVFL_DeadAlready,
  DBGSTR_SVFL_DeadDiffLevelCheck,
  DBGSTR_SVFL_HandExRelive,
  DBGSTR_SVFL_PutWazaEffect,
  DBGSTR_SVFL_WazaDmgCmd,
  DBGSTR_SVFL_ActIntr,
  DBGSTR_SVFL_RelivePokeStart,
  DBGSTR_SVFL_ReliveEndIn,
  DBGSTR_SVFL_ReliveEndIgnore,
  DBGSTR_SVFL_RankEffCore,
  DBGSTR_SVFL_IntrTargetPoke,
  DBGSTR_SVFL_CheckItemReaction,
  DBGSTR_SVFL_MagicCoatStart,
  DBGSTR_SVFL_MagicCoatReflect,

  DBGSTR_SVFS_RegTargetDouble,

  DBGSTR_SC_PutCmd,
  DBGSTR_SC_ReservedPos,
  DBGSTR_SC_WriteReservedPos,
  DBGSTR_SC_PutMsgParam,
  DBGSTR_SC_ReadMsgParam,
  DBGSTR_SC_PutMsg_SE,
  DBGSTR_SC_ReadMsg_SE,
  DBGSTR_SC_ArgsEqual,
  DBGSTR_SC_ReadCmd,
  DBGSTR_SC_ReserveCmd,

  DBGSTR_SERVER_SendShooterChargeCmd,
  DBGSTR_SERVER_ShooterChargeCmdDoneAll,
  DBGSTR_SERVER_SendActionSelectCmd,
  DBGSTR_SERVER_ActionSelectDoneAll,
  DBGSTR_SERVER_FlowResult,
  DBGSTR_SV_ChangePokeOnTheTurn,
  DBGSTR_SV_PokeInReqForEmptyPos,
  DBGSTR_SV_StartChangePokeInfo,
  DBGSTR_SV_SendQuitACmad,
  DBGSTR_SV_ReplyQuitACmad,
  DBGSTR_SV_RestoreAction,
  DBGSTR_SV_CmdCheckOK,
  DBGSTR_SV_CmdCheckNG,
  DBGSTR_SV_SendActRecord,
  DBGSTR_SV_SendActRecordFailed,
  DBGSTR_SV_ExitBattle,

  DBGSTR_EVENT_AddFactorInfo,
  DBGSTR_EV_AddFactor,
  DBGSTR_EV_DelFactor,
  DBGSTR_EV_LinkInfoHeader,
  DBGSTR_EV_LinkInfoFooder,
  DBGSTR_EV_LinkPtr,
  DBGSTR_EV_LinkEmpty,
  DBGSTR_EVAR_Push,
  DBGSTR_EVAR_Pop,

  DBGSTR_SIDE_AddFirst,
  DBGSTR_SIDE_NoMoreAdd,
  DBGSTR_PSET_Copy,
  DBGSTR_PSET_Add,
  DBGSTR_PSET_Remove,

  DBGSTR_FIDLD_FuinCheck,
  DBGSTR_FIELD_FuinHit,

  DBGSTR_SICK_ContProc,

  DBGSTR_POKESET_RemoveDeadPoke,

  DBGSTR_VCORE_PokeListStart,
  DBGSTR_VCORE_SelPokeEnd,
  DBGSTR_VCORE_SelPokeEnd_Sel,
  DBGSTR_VCORE_SelPokeEnd_Unsel,
  DBGSTR_VCORE_PluralDamageInfo,
  DBGSTR_VCORE_PluralDamagePoke,
  DBGSTR_SCU_RelivePokeAct,

  DBGSTR_POSPOKE_Out,
  DBGSTR_POSPOKE_In,
  DBGSTR_DEADREC_Add,

  DBGSTR_Item_PinchReactItem,
  DBGSTR_Item_PinchReactOn,
  DBGSTR_HANDWAZA_CombiWazaExe,
  DBGSTR_HANDWAZA_AlreadyRegistered,

  DBGSTR_REC_ReadActStart,
  DBGSTR_REC_ReadActSkip,
  DBGSTR_REC_SeekClient,
  DBGSTR_REC_ReadActParam,
  DBGSTR_REC_CheckMatchClient,
  DBGSTR_REC_ReadTimeOverCmd,

  DBGSTR_csv,
  DBGSTR_done,
  DBGSTR_LF,

}BtlDebugStrID;


extern void BTL_DEBUGPRINT_SetType( BtlPrintType type );
extern void BTL_DEBUGPRINT_PrintHeader( const char* fileName, u32 line );
extern void BTL_DEBUGPRINT_PrintDump( const char* caption, const void* data, u32 size );
extern const char* BTL_DEBUGPRINT_GetFormatStr( BtlDebugStrID strID );

extern const char* BTL_DEBUGPRINT_GetServerCmdName( int cmd );


