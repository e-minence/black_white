//////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  �X�N���v�g�R�}���h�F�V���b�v�֘A
 * @file   scrcmd_shop.c
 * @author iwasawa
 * @date   2009.10.20
 *
 */
//////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "system/vm_cmd.h"
#include "scrcmd.h"
#include "app/pokelist.h"
#include "system/main.h"

#include "script_local.h"
//#include "script.h"     // SCRIPT_SetSubProcWorkPointerAdrs
#include "app/p_status.h"
#include "event_fieldmap_control.h"
#include "scrcmd_shop.h"
#include "fieldmap.h"
#include "script_def.h"
#include "field_camera.h"

FS_EXTERN_OVERLAY(pokelist);

typedef struct _SHOP_EVENT_WORK{
  void* bag_param;
  u16*  ret_work;
}SHOP_EVENT_WORK;

//====================================================================
// ���v���g�^�C�v�錾
//====================================================================

static BOOL callproc_WaitShopBuyEnd( VMHANDLE *core, void *wk );


typedef struct {
  int seq;
  int wait;
} SHOP_BUY_CALL_WORK;

//--------------------------------------------------------------------
/**
 * @brief �V���b�v�w���C�x���g�Ăяo�� 
 *
 * @param	core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk   SCRCMD_WORK�ւ̃|�C���^
 *
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------------
VMCMD_RESULT EvCmdCallShopProcBuy( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*      work = (SCRCMD_WORK*)wk;
  u16               shop_id = SCRCMD_GetVMWorkValue( core, work );
  u16*              ret_work = SCRCMD_GetVMWork( core, work );
  GAMESYS_WORK*     gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  void** scr_subproc_work = SCRIPT_SetSubProcWorkPointerAdrs( sc );

  SHOP_BUY_CALL_WORK* sbw = GFL_HEAP_AllocClearMemory(HEAPID_PROC,sizeof(SHOP_BUY_CALL_WORK));
  *scr_subproc_work = sbw;


  if(shop_id == SCR_SHOPID_NULL){
    //�ϓ��V���b�v�Ăяo�� 
     VMCMD_SetWait( core, callproc_WaitShopBuyEnd );
  }else{
    //�Œ�V���b�v�Ăяo��
     VMCMD_SetWait( core, callproc_WaitShopBuyEnd );
  }

  /*
    �C�x���g�I������ ret_work �ɏI�����[�h��Ԃ��Ă�������
    SCR_PROC_RETMODE_EXIT �ꔭ�I��
    SCR_PROC_RETMODE_NORMAL �p��
  */
  return VMCMD_RESULT_SUSPEND;
}


//----------------------------------------------------------------------------------
/**
 * @brief	�V���b�v�����T�u�V�[�P���X
 *
 * @param   core  SCRCMD_WORK
 * @param   wk    SHOP_BUY_CALL_WORK
 *
 * @retval  BOOL  �I��:TRUE   �҂�:FALSE
 */
//----------------------------------------------------------------------------------
static BOOL callproc_WaitShopBuyEnd( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  void **scr_subproc_work = SCRIPT_SetSubProcWorkPointerAdrs( sc );
  SHOP_BUY_CALL_WORK *sbw = *scr_subproc_work;
  GAMESYS_WORK*     gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  FIELD_CAMERA *fieldcamera = FIELDMAP_GetFieldCamera( fieldmap );


  // 
  switch(sbw->seq)
  {
    case 0:
      OS_Printf("�V���b�v�N��\n");
      FIELD_CAMERA_SetRecvCamParam( fieldcamera );
      {
        FLD_CAM_MV_PARAM param;
        param.Core.Shift.x = FX32_ONE*16*8;
        param.Core.Shift.y = 0;
        param.Core.Shift.z = 0;
  
        param.Chk.Shift    = TRUE;
        param.Chk.Pitch    = FALSE;
        param.Chk.Yaw   = FALSE;
        param.Chk.Dist   = FALSE;
        param.Chk.Fovy = FALSE;
        param.Chk.Pos   = FALSE;
        FIELD_CAMERA_SetLinerParam( fieldcamera, &param, 40 );
      }
      sbw->seq++;
      break;
    case 1:
      if(!FIELD_CAMERA_CheckMvFunc(fieldcamera)){
        sbw->seq++;
      }
      break;
    case 2:
      if(++sbw->wait>30){
        sbw->seq++;
      }
      break;
    case 3:
      {
        FLD_CAM_MV_PARAM_CHK chk = {TRUE, FALSE,FALSE,FALSE,FALSE,};
        FIELD_CAMERA_RecvLinerParam(fieldcamera, &chk, 40);
      }
      sbw->seq++;
      sbw->wait=0;
      break;
    case 4:
      if(!FIELD_CAMERA_CheckMvFunc(fieldcamera)){
        FIELD_CAMERA_ClearRecvCamParam(fieldcamera);
        sbw->seq++;
      }
      break;
    case 5:
      return TRUE;
      break;
  }
  
  return FALSE;
}