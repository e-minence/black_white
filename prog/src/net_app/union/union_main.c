//==============================================================================
/**
 * @file    union_main.c
 * @brief   ���j�I�����[���F�풓��
 * @author  matsuda
 * @date    2009.07.02(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "gamesystem/game_comm.h"
#include "union_types.h"
#include "savedata/mystatus.h"
#include "net_app/union/union_main.h"
#include "union_local.h"
#include "union_receive.h"
#include "union_chara.h"
#include "union_oneself.h"
#include "union_subproc.h"


void Union_Main(GAME_COMM_SYS_PTR game_comm, FIELD_MAIN_WORK *fieldmap)
{
  UNION_SYSTEM_PTR unisys;
  
  if(GameCommSys_BootCheck(game_comm) != GAME_COMM_NO_UNION){
    return;
  }
  
  unisys = GameCommSys_GetAppWork(game_comm);
  GF_ASSERT(unisys != NULL);
  
  switch(unisys->my_situation.play_category){
  case UNION_PLAY_CATEGORY_UNION:
    if(UnionSubProc_IsExits(unisys) == FALSE){  //�T�uPROC���������̂ݎ��s
      //�f�[�^��M�ɂ��C�x���g�N��
      UnionReceive_BeaconInterpret(unisys);
      
      //�L�[����ɂ��C�x���g�N��
      UnionOneself_Update(unisys, fieldmap);
      
      //OBJ���f
      UNION_CHAR_Update(unisys, unisys->uniparent->game_data);
      
      //����ʔ��f
    }
    break;
  case UNION_PLAY_CATEGORY_COLOSSEUM:
    break;
  }
}

