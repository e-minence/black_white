//==============================================================================
/**
 * @file    intrude_mission.h
 * @brief   �~�b�V���������̃w�b�_
 * @author  matsuda
 * @date    2009.10.26(��)
 */
//==============================================================================
#pragma once

#include "print/wordset.h"


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern void MISSION_Init(MISSION_SYSTEM *mission);
extern void MISSION_Init_List(MISSION_SYSTEM *mission);
extern void MISSION_Update(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission);
extern void MISSION_Set_ListSendReq(MISSION_SYSTEM *mission, int palace_area);
extern void MISSION_Set_DataSendReq(MISSION_SYSTEM *mission);
extern BOOL MISSION_SetEntry(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission, const MISSION_REQ *req, int accept_netid);
extern void MISSION_SetMissionData(MISSION_SYSTEM *mission, const MISSION_DATA *src);
extern BOOL MISSION_RecvCheck(const MISSION_SYSTEM *mission);
extern MISSION_DATA * MISSION_GetRecvData(MISSION_SYSTEM *mission);
extern u16 MISSION_GetMissionMsgID(const MISSION_SYSTEM *mission);
extern u16 MISSION_GetAchieveMsgID(const MISSION_SYSTEM *mission, int my_netid);
extern BOOL MISSION_EntryAchieve(MISSION_SYSTEM *mission, const MISSION_DATA *mdata, int achieve_netid);
extern BOOL MISSION_EntryAchieve(MISSION_SYSTEM *mission, const MISSION_DATA *mdata, int achieve_netid);
extern void MISSION_SetResult(MISSION_SYSTEM *mission, const MISSION_RESULT *cp_result);
extern BOOL MISSION_RecvAchieve(const MISSION_SYSTEM *mission);
extern BOOL MISSION_Talk_CheckAchieve(const MISSION_SYSTEM *mission, int talk_netid);
extern s32 MISSION_GetPoint(INTRUDE_COMM_SYS_PTR intcomm, const MISSION_RESULT *result);
extern BOOL MISSION_AddPoint(INTRUDE_COMM_SYS_PTR intcomm, const MISSION_SYSTEM *mission);
extern void MISSION_MissionList_Create(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission, int accept_netid, int palace_area);
extern BOOL MISSION_MissionList_CheckOcc(const MISSION_CHOICE_LIST *list);
extern BOOL MISSION_MissionList_CheckAgree(const MISSION_SYSTEM *mission, const MISSION_DATA *mdata);
extern BOOL MISSION_SetEntryNew(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission, const MISSION_DATA *mdata, int net_id);
extern void MISSION_SetRecvEntryAnswer(MISSION_SYSTEM *mission, const MISSION_ENTRY_ANSWER *entry_answer);
extern MISSION_ENTRY_RESULT MISSION_GetRecvEntryAnswer(MISSION_SYSTEM *mission);
extern void MISSION_ClearRecvEntryAnswer(MISSION_SYSTEM *mission);

extern void MISSION_SetMissionList(MISSION_SYSTEM *mission, const MISSION_CHOICE_LIST *list);
extern void MISSIONDATA_Wordset(INTRUDE_COMM_SYS_PTR intcomm, const MISSION_DATA *mdata, WORDSET *wordset, HEAPID temp_heap_id);

