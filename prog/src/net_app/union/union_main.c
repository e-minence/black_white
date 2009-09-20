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
#include "field/fieldmap_ctrl_grid.h"
#include "field/fieldmap_call.h"  //FIELDMAP_IsReady
#include "colosseum.h"


void Union_Main(GAME_COMM_SYS_PTR game_comm, FIELDMAP_WORK *fieldmap)
{
  UNION_SYSTEM_PTR unisys;
  
  if(GameCommSys_BootCheck(game_comm) != GAME_COMM_NO_UNION){
    return;
  }
  
  unisys = GameCommSys_GetAppWork(game_comm);
  GF_ASSERT(unisys != NULL);
  
  //PROC���t�B�[���h�̎��̂ݎ��s���鏈��
  if(Union_FieldCheck(unisys) == TRUE){
    if(unisys->my_situation.play_category >= UNION_PLAY_CATEGORY_COLOSSEUM_START
        && unisys->my_situation.play_category <= UNION_PLAY_CATEGORY_COLOSSEUM_END){
      if(UnionSubProc_IsExits(unisys) == FALSE){  //�T�uPROC���������̂ݎ��s
        //�L�[����ɂ��C�x���g�N��
        UnionOneself_Update(unisys, fieldmap);
      }
    }
    else{
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
      }
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
 * �t�B�[���h�쐬���ɌĂ΂��R�[���o�b�N
 *
 * @param   unisys		
 */
//==================================================================
void UnionMain_Callback_FieldCreate(void *pwk, void *app_work, FIELDMAP_WORK *fieldWork)
{
  UNION_PARENT_WORK *uniparent = pwk;
  UNION_SYSTEM_PTR unisys = app_work;
  COLOSSEUM_SYSTEM_PTR clsys = unisys->colosseum_sys;
  
  if(unisys->player_pause == TRUE){
    FIELDMAP_CTRL_GRID_SetPlayerPause( fieldWork, TRUE );
  }
  
  if(clsys != NULL && clsys->comm_ready == TRUE){
    CommPlayer_Pop(clsys->cps);
  }
}

//==================================================================
/**
 * �t�B�[���h�폜���ɌĂ΂��R�[���o�b�N
 *
 * @param   unisys		
 */
//==================================================================
void UnionMain_Callback_FieldDelete(void *pwk, void *app_work, FIELDMAP_WORK *fieldWork)
{
  UNION_PARENT_WORK *uniparent = pwk;
  UNION_SYSTEM_PTR unisys = app_work;
  COLOSSEUM_SYSTEM_PTR clsys = unisys->colosseum_sys;

  if(clsys != NULL){
    if(clsys->colosseum_leave == TRUE){ //�R���V�A���ޏo����
      Colosseum_ExitSystem(unisys->colosseum_sys);
      unisys->colosseum_sys = NULL;
    }
    else{
      if(clsys->comm_ready == TRUE){
        CommPlayer_Push(clsys->cps);
      }
    }
  }
}

//==================================================================
/**
 * ���j�I���I�����N�G�X�g�t���O�̎擾
 *
 * @param   unisys		
 *
 * @retval  BOOL		
 */
//==================================================================
BOOL UnionMain_GetFinishReq(UNION_SYSTEM_PTR unisys)
{
  return unisys->finish;
}

//==================================================================
/**
 * ���j�I���I���������s���t���O�̎擾
 *
 * @param   unisys		
 *
 * @retval  BOOL		
 */
//==================================================================
BOOL UnionMain_GetFinishExe(UNION_SYSTEM_PTR unisys)
{
  return unisys->finish_exe;
}

//==================================================================
/**
 * �A�s�[���ԍ����Z�b�g
 *
 * @param   unisys		
 * @param   appeal_no		�A�s�[���ԍ�
 */
//==================================================================
void UnionMain_SetAppealNo(UNION_SYSTEM_PTR unisys, UNION_APPEAL appeal_no)
{
  unisys->my_situation.appeal_no = appeal_no;
}
