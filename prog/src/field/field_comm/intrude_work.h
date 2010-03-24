//==============================================================================
/**
 * @file    intrude_work.h
 * @brief   �N���V�X�e�����[�N�ɊO������G��ׂ̊֐��Q
 * @author  matsuda
 * @date    2009.10.10(�y)
 */
//==============================================================================
#pragma once

#include "gamesystem/game_comm.h"
#include "field/field_comm/intrude_types.h"
#include "field\field_wfbc_data.h"


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern void IntrudeWork_SetActionStatus(GAME_COMM_SYS_PTR game_comm, INTRUDE_ACTION action);
extern void Intrude_SetWarpPlayerNetID(GAME_COMM_SYS_PTR game_comm, int net_id);
extern int Intrude_GetWarpPlayerNetID(GAME_COMM_SYS_PTR game_comm);
extern void Intrude_SetWarpTown(GAME_COMM_SYS_PTR game_comm, int town_tblno);
extern int Intrude_GetWarpTown(GAME_COMM_SYS_PTR game_comm);
extern BOOL Intrude_GetTargetTimingNo(INTRUDE_COMM_SYS_PTR intcomm, u8 timing_no, u8 target_netid);
extern OCCUPY_INFO * Intrude_GetOccupyInfo(INTRUDE_COMM_SYS_PTR intcomm, int palace_area);
extern MYSTATUS * Intrude_GetMyStatus(INTRUDE_COMM_SYS_PTR intcomm, int net_id);
extern MISSION_CHOICE_LIST * Intrude_GetChoiceList(INTRUDE_COMM_SYS_PTR intcomm, int palace_area);
extern u8 Intrude_GetPalaceArea(INTRUDE_COMM_SYS_PTR intcomm);
extern BOOL Intrude_GetRecvWfbc(INTRUDE_COMM_SYS_PTR intcomm);
extern BOOL Intrude_GetNewMissionRecvFlag(INTRUDE_COMM_SYS_PTR intcomm);
extern WFBC_COMM_DATA * Intrude_GetWfbcCommData(INTRUDE_COMM_SYS_PTR intcomm);
extern u8 Intrude_GetRomVersion(GAME_COMM_SYS_PTR game_comm);
extern u8 Intrude_GetSeasonID(GAME_COMM_SYS_PTR game_comm);
extern BOOL Intrude_CheckNextPalaceAreaMine(GAME_COMM_SYS_PTR game_comm, const GAMEDATA *gamedata, u16 dir);

