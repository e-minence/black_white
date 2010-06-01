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
//  �萔��`
//==============================================================================
typedef enum{
  GRAYSCALE_TYPE_NULL,    ///<�O���[�X�P�[���ɂ���K�v����
  GRAYSCALE_TYPE_WHITE,   ///<���o�[�W�����̃O���[�X�P�[��
  GRAYSCALE_TYPE_BLACK,   ///<���o�[�W�����̃O���[�X�P�[��
  GRAYSCALE_TYPE_NEXT,    ///<����o�[�W�����̃O���[�X�P�[��
}GRAYSCALE_TYPE;


///���C�u�ʐM��ʗp�F�N�����
typedef enum{
  LIVE_COMM_NULL,             ///<�N������Ă��Ȃ�
  LIVE_COMM_INTRUDE,          ///<�N������Ă���
  LIVE_COMM_INTRUDE_PARTNER,  ///<���͎҂ɂȂ���
  LIVE_COMM_INTRUDE_OUT,      ///<���͎҂ɂȂ�����A���C�u�ʐM��ʂ���x�����Ă���
}LIVE_COMM;


///�N���̐ڑ���
typedef enum{
  INTRUDE_CONNECT_NULL,              ///<�ڑ�����Ă��Ȃ�
  INTRUDE_CONNECT_INTRUDE,           ///<�n�C�����N�ڑ���(�~�b�V�������s���Ă��Ȃ�)
  INTRUDE_CONNECT_MISSION_TARGET,    ///<�n�C�����N�ڑ���(�~�b�V�����̃^�[�Q�b�g�ɂȂ��Ă���)
  INTRUDE_CONNECT_MISSION_PARTNER,   ///<�n�C�����N�ڑ���(���͎҂ł���)
}INTRUDE_CONNECT;


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern BOOL Intrude_Check_AlwaysBoot(GAMESYS_WORK *gsys);
extern INTRUDE_COMM_SYS_PTR Intrude_Check_CommConnect(GAME_COMM_SYS_PTR game_comm);
extern void IntrudeWork_SetActionStatus(GAME_COMM_SYS_PTR game_comm, INTRUDE_ACTION action);
extern void Intrude_SetWarpPlayerNetID(GAME_COMM_SYS_PTR game_comm, int net_id);
extern int Intrude_GetWarpPlayerNetID(GAME_COMM_SYS_PTR game_comm);
extern void Intrude_SetWarpTown(GAME_COMM_SYS_PTR game_comm, int town_tblno);
extern int Intrude_GetWarpTown(GAME_COMM_SYS_PTR game_comm);
extern u8 Intrude_GetIntrudeRomVersion(u8 pm_version, u32 trainer_id);
extern BOOL Intrude_GetTargetTimingNo(INTRUDE_COMM_SYS_PTR intcomm, u8 timing_no, u8 target_netid);
extern OCCUPY_INFO * Intrude_GetOccupyInfo(GAMEDATA *gamedata, int palace_area);
extern MYSTATUS * Intrude_GetMyStatus(GAMEDATA *gamedata, int net_id);
extern MISSION_CHOICE_LIST * Intrude_GetChoiceList(INTRUDE_COMM_SYS_PTR intcomm, int palace_area);
extern u8 Intrude_GetPalaceArea(const GAMEDATA *gamedata);
extern BOOL Intrude_GetRecvWfbc(INTRUDE_COMM_SYS_PTR intcomm);
extern WFBC_COMM_DATA * Intrude_GetWfbcCommData(INTRUDE_COMM_SYS_PTR intcomm);
extern u8 Intrude_GetRomVersion(GAME_COMM_SYS_PTR game_comm);
extern u8 Intrude_GetSeasonID(GAME_COMM_SYS_PTR game_comm);
extern BOOL Intrude_CheckNextPalaceAreaMine(GAME_COMM_SYS_PTR game_comm, const GAMEDATA *gamedata, u16 dir);
extern GRAYSCALE_TYPE Intrude_CheckGrayScaleMap(GAME_COMM_SYS_PTR game_comm, GAMEDATA *gamedata);
extern BOOL Intrude_CheckPalaceConnect(GAME_COMM_SYS_PTR game_comm);
extern BOOL Intrude_CheckZonePalaceConnect(u16 zone_id);
extern BOOL Intrude_CheckTutorialComplete(GAMEDATA *gamedata);
extern BOOL Intrude_CheckStartTimeoutMsgON(INTRUDE_COMM_SYS_PTR intcomm);
extern LIVE_COMM Intrude_CheckLiveCommStatus(GAME_COMM_SYS_PTR game_comm);
extern void Intrude_SetLiveCommStatus_IntrudeOut(GAME_COMM_SYS_PTR game_comm);
extern INTRUDE_CONNECT Intrude_GetIntrudeStatus(GAME_COMM_SYS_PTR game_comm);
extern int Intrude_GetMissionEntryNum(GAME_COMM_SYS_PTR game_comm);
extern BOOL Intrude_GetTargetName(GAME_COMM_SYS_PTR game_comm, STRBUF *dest_buf);

