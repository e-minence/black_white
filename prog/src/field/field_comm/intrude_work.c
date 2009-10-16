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
#include "palace_sys.h"
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

void IntrudeWork_SetActionStatus(GAME_COMM_SYS_PTR game_comm, INTRUDE_ACTION action)
{
  INTRUDE_COMM_SYS_PTR intcomm = _GetIntrudeCommSys(game_comm);
  
  if(intcomm == NULL){
    return;
  }
  
  Intrude_SetActionStatus(intcomm, action);
  intcomm->send_status = TRUE;
}
