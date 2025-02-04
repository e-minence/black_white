//==============================================================================
/**
 * @file    intrude_mission.h
 * @brief   ミッション処理のヘッダ
 * @author  matsuda
 * @date    2009.10.26(月)
 */
//==============================================================================
#pragma once

#include "print/wordset.h"


//==============================================================================
//  外部関数宣言
//==============================================================================
extern void MISSION_Init(MISSION_SYSTEM *mission);
extern void MISSION_Init_List(MISSION_SYSTEM *mission);
extern void MISSION_Update(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission);
extern void MISSION_Set_ListSendReq(MISSION_SYSTEM *mission, int palace_area);
extern void MISSION_Set_DataSendReq(MISSION_SYSTEM *mission);
extern void MISSION_Set_ResultSendReq(MISSION_SYSTEM *mission);
extern BOOL MISSION_SetMissionData(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission, const MISSION_DATA *src);
extern void MISSION_RecvMissionStart(MISSION_SYSTEM *mission);
extern void MISSION_RecvMissionStartClient(MISSION_SYSTEM *mission, NetID net_id);
extern void MISSION_RecvMissionStartClientAnswer(MISSION_SYSTEM *mission, s32 now_timer);
extern BOOL MISSION_RecvCheck(const MISSION_SYSTEM *mission);
extern BOOL MISSION_CheckMissionStartOK(MISSION_SYSTEM *mission);
extern MISSION_DATA * MISSION_GetRecvData(MISSION_SYSTEM *mission);
extern void MISSION_SetMissionEntry(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission);
extern BOOL MISSION_GetMissionEntry(const MISSION_SYSTEM *mission);
extern void MISSION_SetMissionComplete(MISSION_SYSTEM *mission);
extern BOOL MISSION_GetMissionComplete(const MISSION_SYSTEM *mission);
extern MISSION_RESULT * MISSION_GetResultData(MISSION_SYSTEM *mission);
extern BOOL MISSION_GetMissionPlay(MISSION_SYSTEM *mission);
extern void MISSION_Recv_CheckAchieveUser(MISSION_SYSTEM *mission, int net_id);
extern MISSION_ACHIEVE_USER MISSION_GetAnswerAchieveUser(MISSION_SYSTEM *mission);
extern void MISSION_SetRecvBufAnswerAchieveUserBuf(MISSION_SYSTEM *mission, MISSION_ACHIEVE_USER data);
extern BOOL MISSION_EntryAchieve(MISSION_SYSTEM *mission, const MISSION_DATA *mdata, int achieve_netid);
extern MISSION_ACHIEVE MISSION_GetAchieveAnswer(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission);
extern void MISSION_ClearAchieveAnswer(MISSION_SYSTEM *mission);
extern void MISSION_SetParentAchieve(MISSION_SYSTEM *mission, MISSION_ACHIEVE achieve);
extern void MISSION_SetResult(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission, const MISSION_RESULT *cp_result);
extern BOOL MISSION_CheckRecvResult(const MISSION_SYSTEM *mission);
extern BOOL MISSION_Talk_CheckAchieve(const MISSION_SYSTEM *mission, int talk_netid);
extern MISSION_TYPE MISSION_GetMissionType(const MISSION_SYSTEM *mission);
extern s32 MISSION_GetResultPoint(INTRUDE_COMM_SYS_PTR intcomm, const MISSION_SYSTEM *mission);
extern void MISSION_SetMissionClear(GAMEDATA *gamedata, INTRUDE_COMM_SYS_PTR intcomm, const MISSION_RESULT *result);
extern BOOL MISSION_CheckResultMissionMine(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission);
extern BOOL MISSION_CheckResultTimeout(MISSION_SYSTEM *mission);
extern BOOL MISSION_CheckSuccessTimeEnd(MISSION_SYSTEM *mission);
extern void MISSION_MissionList_Create(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission, int accept_netid, int palace_area);
extern BOOL MISSION_MissionList_CheckOcc(const MISSION_CHOICE_LIST *list);
extern BOOL MISSION_SetEntryNew(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission, const MISSION_ENTRY_REQ *entry_req, int net_id);
extern void MISSION_SetRecvEntryAnswer(MISSION_SYSTEM *mission, const MISSION_ENTRY_ANSWER *entry_answer);
extern MISSION_ENTRY_RESULT MISSION_GetRecvEntryAnswer(MISSION_SYSTEM *mission);
extern void MISSION_ClearRecvEntryAnswer(MISSION_SYSTEM *mission);

extern void MISSION_SetMissionList(MISSION_SYSTEM *mission, const MISSION_CHOICE_LIST *list, NetID net_id);
extern BOOL MISSION_CheckMissionTargetNetID(MISSION_SYSTEM *mission, int net_id);
extern NetID MISSION_GetMissionTargetNetID(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission);
extern BOOL MISSION_CheckTargetIsMine(INTRUDE_COMM_SYS_PTR intcomm);
extern void MISSIONDATA_WordsetTargetName(WORDSET *wordset, u32 bufID, const MISSION_TARGET_INFO *target, HEAPID temp_heap_id);
extern void MISSIONDATA_Wordset(const MISSION_CONV_DATA *cdata, const MISSION_TARGET_INFO *target, WORDSET *wordset, HEAPID temp_heap_id);
extern s32 MISSION_GetMissionTimer(MISSION_SYSTEM *mission);
extern void MISSION_LIST_Create(OCCUPY_INFO *occupy);
extern void MISSION_LIST_Create_Type(OCCUPY_INFO *occupy, MISSION_TYPE mission_type);
extern BOOL MISSION_LIST_Create_Complete(OCCUPY_INFO *occupy, u8 *white_num, u8 *black_num);

#ifdef PM_DEBUG
extern int DEBUG_MISSION_TypeSearch(MISSION_TYPE mission_type, int *no);
extern void DEBUG_MISSION_TypeNoSet(OCCUPY_INFO *occupy, MISSION_TYPE mission_type, int no);
extern int DEBUG_MISSION_TypeNoGet(const OCCUPY_INFO *occupy, MISSION_TYPE mission_type);
#endif  //PM_DEBUG
