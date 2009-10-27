//==============================================================================
/**
 * @file    intrude_mission.h
 * @brief   �~�b�V���������̃w�b�_
 * @author  matsuda
 * @date    2009.10.26(��)
 */
//==============================================================================
#pragma once


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern void MISSION_Init(MISSION_DATA *mission);
extern BOOL MISSION_SetEntry(INTRUDE_COMM_SYS_PTR intcomm, MISSION_DATA *mission, const MISSION_REQ *req, int accept_netid);
extern void MISSION_SetMissionData(MISSION_DATA *mission, const MISSION_DATA *src);
extern void MISSION_SendUpdate(INTRUDE_COMM_SYS_PTR intcomm, MISSION_DATA *mission);
extern BOOL MISSION_RecvCheck(const MISSION_DATA *mission);
extern u16 MISSION_GetMissionMsgID(const MISSION_DATA *mission);

