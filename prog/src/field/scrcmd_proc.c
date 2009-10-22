//======================================================================
/**
 * @file    scrcmd_proc.c
 * @brief   �X�N���v�g�R�}���h�p�֐��@�e��A�v���Ăяo���n(�풓)
 * @author  Miyuki Iwasawa 
 * @date    09.10.22
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/vm_cmd.h"

#include "script.h"
#include "script_local.h"
#include "script_def.h"

#include "scrcmd.h"
#include "scrcmd_work.h"

#include "msgdata.h"
#include "print/wordset.h"

#include "sound/pm_sndsys.h"
//#include "sound/wb_sound_data.sadl"

#include "fieldmap.h"
#include "field_sound.h"
#include "field_player.h"
#include "event_fieldmap_control.h"

#include "scrcmd_proc.h"
#include "app/bag.h"
#include "../../../resource/fldmapdata/script/usescript.h"

#include "app/box2.h"
#include "event_fieldmap_control.h"   // for EVENT_FieldSubProc_Callback


// �{�b�N�X�v���Z�X�f�[�^�ƃR�[���o�b�N�֐�
FS_EXTERN_OVERLAY(box);
extern const GFL_PROC_DATA BOX2_ProcData;
static void callback_BoxProc( void* work );


//--------------------------------------------------------------
/**
 * @brief �t�B�[���h�}�b�v�v���Z�X����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdFieldOpen( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );

  SCRIPT_CallEvent( sc, EVENT_FieldOpen( gsys ));
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * @brief �t�B�[���h�}�b�v�v���Z�X�j��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdFieldClose( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );

  SCRIPT_CallEvent( sc, EVENT_FieldClose(gsys, GAMESYSTEM_GetFieldMapWork(gsys)));
  
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * @brief �T�u�v���Z�X�̏I����҂��܂�(���[�N��������L��)
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  TRUE    �I��
 * @retval  FALSE   �҂�
 */
//--------------------------------------------------------------
BOOL SCMD_WaitSubProcEnd( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );

	if (GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL){
    return FALSE;
  }
  SCRIPT_FreeSubProcWorkPointer(sc);

  return TRUE;
}
//--------------------------------------------------------------
/**
 * @brief �T�u�v���Z�X�̏I����҂��܂�(���[�N�����������)
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  TRUE    �I��
 * @retval  FALSE   �҂�
 *
 * ��������ő҂����ꍇ�A�����I�� EvCmdFreeSubProcWork( VMHANDLE *core, void *wk ) ���g���ĉ�����邱��
 */
//--------------------------------------------------------------
BOOL SCMD_WaitSubProcEndNonFree( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );

	if (GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL){
    return FALSE;
  }
  return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief �ėp�T�u�v���Z�X���[�N����R�}���h
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdFreeSubProcWork( VMHANDLE *core, void *wk )
{
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( (SCRCMD_WORK*)wk );
  SCRIPT_FreeSubProcWorkPointer(sc);
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * @brief   �o�b�O�v���Z�X���Ăяo���܂�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 *
 * ���T�u�v���Z�X���[�N�̉���������s��Ȃ��̂ŁA�K�������I�� EvCmdBagProcWorkFree()���Ăяo������
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCallBagProc( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  void** sub_wk = SCRIPT_SetSubProcWorkPointerAdrs( sc );

  u16 mode = SCRCMD_GetVMWorkValue( core, work );

  if( mode == SCR_BAG_MODE_SELL ){
    mode = BAG_MODE_SELL;
  }else{
    mode = BAG_MODE_FIELD;
  }
  *sub_wk = (void*)BAG_CreateParam( SCRCMD_WORK_GetGameData( work ), NULL, mode );
  
  GAMESYSTEM_CallProc( gsys, FS_OVERLAY_ID(bag), &ItemMenuProcData, *sub_wk ); 
 
  VMCMD_SetWait( core, SCMD_WaitSubProcEndNonFree );

  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * @brief   �o�b�O�v���Z�X�̃��^�[���R�[�h���擾���܂�
 * @param   core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 *
 * ��EvCmdFreeSubProcWork() �̑O�ɌĂяo������
 *
 * �o�b�O�v���Z�X�̏I�����[�h�ƑI���A�C�e��No��Ԃ��܂�
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetBagProcResult( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  BAG_PARAM* bp = (BAG_PARAM*)(SCRIPT_SetSubProcWorkPointer( sc ));

  u16* ret_mode = SCRCMD_GetVMWork( core, work );
  u16* ret_item = SCRCMD_GetVMWork( core, work );

  if(bp->next_proc == BAG_NEXTPROC_EXIT ){
    *ret_mode = SCR_PROC_RETMODE_EXIT;
  }else{
    *ret_mode = SCR_PROC_RETMODE_NORMAL;
  }
  *ret_item = bp->ret_item;
  return VMCMD_RESULT_SUSPEND;
}


//--------------------------------------------------------------
/**
 * @brief �{�b�N�X�v���Z�X�I����ɌĂ΂��R�[���o�b�N�֐�
 */
//--------------------------------------------------------------
typedef struct
{
  BOX2_GFL_PROC_PARAM* box_param;
} CALLBACK_WORK;
static void callback_BoxProc( void* work )
{
  CALLBACK_WORK* cw = (CALLBACK_WORK*)work;
  GFL_HEAP_FreeMemory( cw->box_param );
}

//--------------------------------------------------------------
/**
 * @brief   �{�b�N�X�v���Z�X���Ăяo���܂�
 * @param   core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCallBoxProc( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  SCRIPT_WORK*        scw = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA*         gdata = GAMESYSTEM_GetGameData( gsys );
  SAVE_CONTROL_WORK*   sv = GAMEDATA_GetSaveControlWork( gdata );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  BOX2_GFL_PROC_PARAM* box_param = NULL;
  CALLBACK_WORK* cw = NULL;
  GMEVENT* event = NULL;
  
  // �{�b�N�X�̃v���Z�X�p�����[�^���쐬
  box_param            = GFL_HEAP_AllocMemory( HEAPID_PROC, sizeof(BOX2_GFL_PROC_PARAM) );
  box_param->sv_box    = GAMEDATA_GetBoxManager( gdata );
  box_param->pokeparty = GAMEDATA_GetMyPokemon( gdata );
  box_param->myitem    = GAMEDATA_GetMyItem( gdata );
  box_param->mystatus  = GAMEDATA_GetMyStatus( gdata );
  box_param->cfg       = SaveData_GetConfig( sv );
  box_param->zknMode   = 0;
  box_param->callMode  = BOX_MODE_SEIRI;

  // �R�[���o�b�N�̃p�����[�^���쐬
  cw = GFL_HEAP_AllocMemory( HEAPID_PROC, sizeof(CALLBACK_WORK) );
  cw->box_param = box_param;

  // �C�x���g���Ăяo��
  event = EVENT_FieldSubProc_Callback(
      gsys, fieldmap, 
      FS_OVERLAY_ID(box), &BOX2_ProcData, box_param, // �Ăяo���v���Z�X���w��
      callback_BoxProc, cw );  // �R�[���o�b�N�֐���, ���̈������w��
  SCRIPT_CallEvent( scw, event );
  return VMCMD_RESULT_SUSPEND;
}
