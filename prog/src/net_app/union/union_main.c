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
  
  //PROC���t�B�[���h�̎��̂ݎ��s���鏈��
  if(Union_FieldCheck(unisys) == TRUE){
    switch(unisys->my_situation.play_category){
    case UNION_PLAY_CATEGORY_UNION:
    case UNION_PLAY_CATEGORY_TALK:
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
      if(UnionSubProc_IsExits(unisys) == FALSE){  //�T�uPROC���������̂ݎ��s
        //�L�[����ɂ��C�x���g�N��
        UnionOneself_Update(unisys, fieldmap);
      }
      break;
    }
  }
}

//==================================================================
/**
 * PROC���t�B�[���h�̏�Ԃ����ׂ�
 *
 * @param   unisys		
 *
 * @retval  BOOL		TRUE:�t�B�[���h�ł���
 */
//==================================================================
BOOL Union_FieldCheck(UNION_SYSTEM_PTR unisys)
{
  if(GAMESYSTEM_CheckFieldMapWork(unisys->uniparent->gsys) == TRUE){
    if(FIELDMAP_IsReady(GAMESYSTEM_GetFieldMapWork(unisys->uniparent->gsys)) == TRUE){
      return TRUE;
    }
  }
  return FALSE;
}

//==================================================================
/**
 * �t�B�[���h�폜���ɍs������
 *
 * @param   unisys		
 */
//==================================================================
void Union_FieldDelete(UNION_SYSTEM_PTR unisys)
{
}

//==================================================================
/**
 * ���A�v������t�B�[���h�֕��A���Ă������̏���
 *
 * @param   unisys		
 */
//==================================================================
void Union_FieldComeback(UNION_SYSTEM_PTR unisys)
{
}
