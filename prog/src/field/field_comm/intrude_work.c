//==============================================================================
/**
 * @file    intrude_work.c
 * @brief   �N���V�X�e�����[�N�ɊO������G��ׂ̊֐��Q
 * @author  matsuda
 * @date    2009.10.10(�y)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "gamesystem/game_comm.h"
#include "savedata/mystatus.h"
#include "system/net_err.h"
#include "net/network_define.h"
#include "field/intrude_comm.h"
#include "intrude_comm_command.h"
#include "net_app/union/comm_player.h"
#include "intrude_types.h"
#include "intrude_comm_command.h"
#include "intrude_main.h"
#include "bingo_types.h"
#include "intrude_work.h"


//==================================================================
/**
 * �A�N�V�����X�e�[�^�X��ݒ�
 *
 * @param   game_comm		
 * @param   action		
 */
//==================================================================
void IntrudeWork_SetActionStatus(GAME_COMM_SYS_PTR game_comm, INTRUDE_ACTION action)
{
  INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
  
  if(intcomm == NULL){
    return;
  }
  
  Intrude_SetActionStatus(intcomm, action);
  intcomm->send_status = TRUE;
}

//==================================================================
/**
 * ���[�v����v���C���[�����ݒ�
 *
 * @param   game_comm		
 * @param   town_tblno		
 */
//==================================================================
void Intrude_SetWarpPlayerNetID(GAME_COMM_SYS_PTR game_comm, int net_id)
{
  INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
  
  if(intcomm == NULL){
    return;
  }
  
  intcomm->warp_player_netid = net_id;
}

//==================================================================
/**
 * ���[�v��̃v���C���[������擾
 *
 * @param   game_comm		
 *
 * @retval  int		�X�ԍ�(PALACE_TOWN_DATA_NULL�̏ꍇ�͖���)
 */
//==================================================================
int Intrude_GetWarpPlayerNetID(GAME_COMM_SYS_PTR game_comm)
{
  INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
  
  if(intcomm == NULL){
    return GAMEDATA_GetIntrudeMyID( GameCommSys_GetGameData(game_comm) );
  }
  return intcomm->warp_player_netid;
}

//==================================================================
/**
 * ���[�v����X�ԍ���ݒ�
 *
 * @param   game_comm		
 * @param   town_tblno		
 */
//==================================================================
void Intrude_SetWarpTown(GAME_COMM_SYS_PTR game_comm, int town_tblno)
{
  INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
  
  if(intcomm == NULL){
    return;
  }
  
  intcomm->warp_town_tblno = town_tblno;
}

//==================================================================
/**
 * ���[�v��̊X�ԍ����擾
 *
 * @param   game_comm		
 *
 * @retval  int		�X�ԍ�(PALACE_TOWN_DATA_NULL�̏ꍇ�͖���)
 */
//==================================================================
int Intrude_GetWarpTown(GAME_COMM_SYS_PTR game_comm)
{
  INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
  
  if(intcomm == NULL){
    return PALACE_TOWN_DATA_NULL;
  }
  return intcomm->warp_town_tblno;
}

//==================================================================
/**
 * ����w��̃^�C�~���O�R�}���h����M�ł��Ă��邩�m�F���܂�
 *
 * @param   intcomm		
 * @param   timing_no		    �����ԍ�
 * @param   target_netid		���������NetID
 *
 * @retval  BOOL		TRUE:������芮���B�@FALSE:�����҂�
 */
//==================================================================
BOOL Intrude_GetTargetTimingNo(INTRUDE_COMM_SYS_PTR intcomm, u8 timing_no, u8 target_netid)
{
  u8 recv_timing_no = intcomm->recv_target_timing_no;
  
  if(timing_no == recv_timing_no && target_netid == intcomm->recv_target_timing_netid){
    intcomm->recv_target_timing_no = 0;
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * �w�肵���p���X�G���A�̐苒�����擾����
 *
 * @param   intcomm		
 * @param   palace_area		  
 *
 * @retval  OCCUPY_INFO *		
 */
//==================================================================
OCCUPY_INFO * Intrude_GetOccupyInfo(INTRUDE_COMM_SYS_PTR intcomm, int palace_area)
{
  GAMEDATA *gamedata = GameCommSys_GetGameData(intcomm->game_comm);
  OCCUPY_INFO *occupy;
  
  if(palace_area == GAMEDATA_GetIntrudeMyID(gamedata)){
    occupy = GAMEDATA_GetMyOccupyInfo(gamedata);
  }
  else{
    occupy = GAMEDATA_GetOccupyInfo(gamedata, palace_area);
  }
  
  return occupy;
}

//==================================================================
/**
 * �w�肵��NetID��MYSTATUS���擾����
 *
 * @param   intcomm		
 * @param   net_id	  
 *
 * @retval  MYSTATUS *		
 */
//==================================================================
MYSTATUS * Intrude_GetMyStatus(INTRUDE_COMM_SYS_PTR intcomm, int net_id)
{
  GAMEDATA *gamedata = GameCommSys_GetGameData(intcomm->game_comm);
  MYSTATUS *myst;
  
  if(net_id == GAMEDATA_GetIntrudeMyID(gamedata)){
    myst = GAMEDATA_GetMyStatus(gamedata);
  }
  else{
    myst = GAMEDATA_GetMyStatusPlayer(gamedata, net_id);
  }
  
  return myst;
}

//==================================================================
/**
 * �w��p���X�G���A�̃~�b�V�����I����⃊�X�g�̃|�C���^���擾
 *
 * @param   intcomm		
 * @param   palace_area		�p���X�G���A
 *
 * @retval  MISSION_CHOICE_LIST *		�~�b�V�����I����⃊�X�g�ւ̃|�C���^
 */
//==================================================================
MISSION_CHOICE_LIST * Intrude_GetChoiceList(INTRUDE_COMM_SYS_PTR intcomm, int palace_area)
{
  return &intcomm->mission.list[palace_area];
}

//==================================================================
/**
 * ������������p���X�G���ANo���擾����
 *
 * @param   intcomm		
 *
 * @retval  u8		�p���X�G���ANo
 */
//==================================================================
u8 Intrude_GetPalaceArea(INTRUDE_COMM_SYS_PTR intcomm)
{
  return intcomm->intrude_status_mine.palace_area;
}

//==================================================================
/**
 * WFBC�p�����[�^����M���������擾
 *
 * @param   intcomm		
 *
 * @retval  BOOL		TRUE:��M�����@FALSE:��M���Ă��Ȃ�
 */
//==================================================================
BOOL Intrude_GetRecvWfbc(INTRUDE_COMM_SYS_PTR intcomm)
{
  return intcomm->wfbc_recv;
}

//==================================================================
/**
 * �V�K�~�b�V������M�t���O���擾   ��check ����g�p���Ă��Ȃ��̂ō폜��� 2010.02.02(��) ���t���[���Ď����Ă���g�����łȂ��Ƌ@�\���Ȃ��̂ōēx�g���ꍇ�͒���
 *
 * @param   intcomm		
 *
 * @retval  BOOL		TRUE:��M�����@FALSE:��M���Ă��Ȃ�
 */
//==================================================================
BOOL Intrude_GetNewMissionRecvFlag(INTRUDE_COMM_SYS_PTR intcomm)
{
  BOOL new_mission = intcomm->new_mission_recv;
  intcomm->new_mission_recv = FALSE;
  return new_mission;
}

//==================================================================
/**
 * WFBC�ʐM���[�N���擾
 *
 * @param   intcomm		
 *
 * @retval  WFBC_COMM_DATA *		WFBC�ʐM���[�N�ւ̃|�C���^
 */
//==================================================================
WFBC_COMM_DATA * Intrude_GetWfbcCommData(INTRUDE_COMM_SYS_PTR intcomm)
{
  return &intcomm->wfbc_comm_data;
}

//==================================================================
/**
 * ���������݂���N�����PM_VERSION���擾
 *
 * @param   game_comm		
 *
 * @retval  int		����������ROM��PM_VERSION
 *                (�ʐM���Ă��Ȃ��A�N�����Ă��Ȃ��A�ʐM�G���[���͎�����PM_VERSION)
 */
//==================================================================
u8 Intrude_GetRomVersion(GAME_COMM_SYS_PTR game_comm)
{
  INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
  const MYSTATUS *myst;
  u8 palace_area;
  
  if(intcomm == NULL){
    return GET_VERSION();
  }
  
  palace_area = Intrude_GetPalaceArea(intcomm);
  myst = Intrude_GetMyStatus(intcomm, palace_area);
  return MyStatus_GetRomCode( myst );
}

//==================================================================
/**
 * ���������݂���N����̋G�߂��擾
 *
 * @param   game_comm		
 *
 * @retval  u8		����������ROM�̋G��ID
 *                (�ʐM���Ă��Ȃ��A�N�����Ă��Ȃ��A�ʐM�G���[���͎�����ROM�̋G��)
 */
//==================================================================
u8 Intrude_GetSeasonID(GAME_COMM_SYS_PTR game_comm)
{
  INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
  GAMEDATA *gamedata = GameCommSys_GetGameData(game_comm);
  u8 palace_area;
  
  if(intcomm == NULL){
    return GAMEDATA_GetSeasonID(gamedata);
  }
  
  palace_area = Intrude_GetPalaceArea(intcomm);
#if 0
  //��check �܂����쐬
  return intcomm->intrude_status[palace_area].season_id;
#else
  return GAMEDATA_GetSeasonID(gamedata);
#endif
}

//==================================================================
/**
 * �ׂ̃p���X�������̃p���X���𒲂ׂ�
 *
 * @param   game_comm		
 * @param   gamedata		
 * @param   dir		      ���ׂ�ׂ̌���(DIR_LEFT or DIR_RIGHT)
 *
 * @retval  BOOL		    TRUE�F�����̃p���X�@FALSE:�����ł͂Ȃ��l�̃p���X
 */
//==================================================================
BOOL Intrude_CheckNextPalaceAreaMine(GAME_COMM_SYS_PTR game_comm, const GAMEDATA *gamedata, u16 dir)
{
  INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
  int palace_area;
  
  if(intcomm == NULL){
    return TRUE;
  }
  
  palace_area = intcomm->intrude_status_mine.palace_area;
  if(dir == DIR_LEFT){
    palace_area--;
    if(palace_area < 0){
      palace_area = intcomm->member_num - 1;
    }
  }
  else{
    palace_area++;
    if(palace_area >= intcomm->member_num){
      palace_area = 0;
    }
  }
  
  if(palace_area == GAMEDATA_GetIntrudeMyID(gamedata)){
    return TRUE;
  }
  return FALSE;
}
