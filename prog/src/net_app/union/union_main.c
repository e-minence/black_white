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
#include "gamesystem/game_beacon.h"
#include "field/zonedata.h"


//--------------------------------------------------------------
//  �I�[�o�[���C��`
//--------------------------------------------------------------
//FS_EXTERN_OVERLAY(union_room);



//==================================================================
/**
 * ���j�I���ʐM���N������
 *
 * @param   gsys		
 */
//==================================================================
void UNION_CommBoot(GAMESYS_WORK *gsys)
{
  GAME_COMM_SYS_PTR game_comm;
  GAMEDATA *gdata;
  
  game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
  gdata = GAMESYSTEM_GetGameData( gsys );
  
  GF_ASSERT(GameCommSys_BootCheck(game_comm) == GAME_COMM_NO_NULL);

  {
    UNION_PARENT_WORK *upw;
    
    OS_TPrintf("union_system heap size = 0x%x\n", sizeof(UNION_PARENT_WORK));
    upw = GFL_HEAP_AllocClearMemory(HEAPID_APP_CONTROL, sizeof(UNION_PARENT_WORK));
    upw->mystatus = GAMEDATA_GetMyStatus(gdata);
    upw->game_comm = game_comm;
    upw->game_data = gdata;
    upw->gsys = gsys;
    GameCommSys_Boot(game_comm, GAME_COMM_NO_UNION, upw);
	}

  GAMEBEACON_Set_UnionIn();
}

//==================================================================
/**
 * ���j�I���X�V����
 *
 * @param   game_comm		
 * @param   fieldmap		
 */
//==================================================================
void Union_Main(GAME_COMM_SYS_PTR game_comm, FIELDMAP_WORK *fieldmap)
{
  UNION_SYSTEM_PTR unisys;
  
  if(GameCommSys_BootCheck(game_comm) != GAME_COMM_NO_UNION){
    return;
  }
  
  unisys = GameCommSys_GetAppWork(game_comm);
  GF_ASSERT(unisys != NULL);

  if(GAMESYSTEM_GetFieldCommErrorReq(FIELDMAP_GetGameSysWork(fieldmap)) == TRUE){
    return; //�t�B�[���h�ɑ΂��ĒʐM�G���[���N�G�X�g���������Ă���ꍇ�͂����ŏI��
  }

  if(GAMESYSTEM_EVENT_IsExists(unisys->uniparent->gsys) == TRUE){
    return; //�C�x���g�N�����͂����ŏI��
  }
  
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
          UNION_CHAR_Update(unisys, unisys->uniparent->game_data, fieldmap);
          
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
  if(unisys->overlay_load == TRUE){
    if(GAMESYSTEM_CheckFieldMapWork(unisys->uniparent->gsys) == TRUE){
      if(FIELDMAP_IsReady(GAMESYSTEM_GetFieldMapWork(unisys->uniparent->gsys)) == TRUE){
        return TRUE;
      }
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
  
#if 0
  if(unisys->overlay_load == FALSE){
    GFL_OVERLAY_Load( FS_OVERLAY_ID( union_room ) );
    unisys->overlay_load = TRUE;
    OS_TPrintf("�I�[�o�[���CLoad : union_room\n");
  }
#else
  unisys->overlay_load = TRUE;
#endif
  
  if(unisys->player_pause == TRUE){
    FIELDMAP_CTRL_GRID_SetPlayerPause( fieldWork, TRUE );
  }
  
  if(ZONEDATA_IsUnionRoom( FIELDMAP_GetZoneID( fieldWork ) ) == TRUE){//���j�I�����[�����ł̂ݎ��s
    UNION_CHAR_MarkRecover(unisys, GAMESYSTEM_GetGameData(unisys->uniparent->gsys), fieldWork);
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
      Colosseum_ExitSystem(unisys->colosseum_sys, unisys->uniparent->gsys);
      unisys->colosseum_sys = NULL;
    }
    else{
      if(clsys->comm_ready == TRUE){
        CommPlayer_Push(clsys->cps);
      }
    }
  }

#if 0
  if(unisys->overlay_load == TRUE){
    GFL_OVERLAY_Unload( FS_OVERLAY_ID( union_room ) );
    unisys->overlay_load = FALSE;
    OS_TPrintf("�I�[�o�[���CUnload : union_room\n");
  }
#else
  unisys->overlay_load = FALSE;
#endif
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

//==================================================================
/**
 * �A�s�[���ԍ����Z�b�g
 *
 * @param   unisys		
 * @param   appeal_no		�A�s�[���ԍ�
 */
//==================================================================
UNION_APPEAL UnionMain_GetAppealNo(UNION_SYSTEM_PTR unisys)
{
  return unisys->my_situation.appeal_no;
}

//==================================================================
/**
 * ���@���O���C�x���g�𔭓��o����t���[�ȏ�Ԃ����ׂ�
 * @param   gsys		
 * @retval  BOOL		TRUE:���S�Ƀt���[�@FALSE:�t���[�ł͂Ȃ�
 */
//==================================================================
BOOL UnionMain_CheckPlayerFreeMode(GAMESYS_WORK *gsys)
{
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
  UNION_SYSTEM_PTR unisys = GameCommSys_GetAppWork(game_comm);
  
  if(unisys == NULL){
    return FALSE;
  }
  if((unisys->my_situation.union_status == UNION_STATUS_NORMAL
      || unisys->my_situation.union_status == UNION_STATUS_COLOSSEUM_NORMAL)
      && unisys->player_pause == FALSE){
    return TRUE;
  }
  return FALSE;
}
