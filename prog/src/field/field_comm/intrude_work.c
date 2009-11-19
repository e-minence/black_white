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


//--------------------------------------------------------------
/**
 * �N���V�X�e�����[�N���擾����
 *
 * @param   game_comm		
 *
 * @retval  INTRUDE_COMM_SYS_PTR		�N���V�X�e�����[�N�ւ̃|�C���^
 * @retval  INTRUDE_COMM_SYS_PTR    �N���V�X�e�����N�����Ă��Ȃ��ꍇ��NULL
 */
//--------------------------------------------------------------
static INTRUDE_COMM_SYS_PTR _GetIntrudeCommSys(GAME_COMM_SYS_PTR game_comm)
{
  INTRUDE_COMM_SYS_PTR intcomm = GameCommSys_GetAppWork(game_comm);
  
  if(intcomm == NULL || GameCommSys_BootCheck(game_comm) != GAME_COMM_NO_INVASION){
    GF_ASSERT(0);
    return NULL;
  }
  return intcomm;
}

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
  INTRUDE_COMM_SYS_PTR intcomm = _GetIntrudeCommSys(game_comm);
  
  if(intcomm == NULL){
    return;
  }
  
  Intrude_SetActionStatus(intcomm, action);
  intcomm->send_status = TRUE;
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
  INTRUDE_COMM_SYS_PTR intcomm = _GetIntrudeCommSys(game_comm);
  
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
  INTRUDE_COMM_SYS_PTR intcomm = _GetIntrudeCommSys(game_comm);
  
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
