//======================================================================
/**
 * @file    scrcmd_proc_fld.c
 * @brief   �X�N���v�g�R�}���h�p�֐��@�e��A�v���Ăяo���n(�t�B�[���h�풓)
 * @author  Miyuki Iwasawa 
 * @date    09.10.23
 *
 * �t�F�[�h��t�B�[���h�v���Z�X�R���g���[�������O�ōs���v���Z�X�Ăяo���@�����
 * �v���Z�X�Ăяo���R�}���h���̂��풓���Ă���K�v������Ƃ���
 * scrcmd_proc.c�ɃR�}���h��u���Ă�������
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
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
#include "scrcmd_proc_fld.h"
#include "../../../resource/fldmapdata/script/usescript.h"

#include "app/box2.h"

// �{�b�N�X�v���Z�X�f�[�^�ƃR�[���o�b�N�֐�
static void callback_BoxProc( void* work );

//--------------------------------------------------------------
/**
 * @brief �{�b�N�X�v���Z�X�I����ɌĂ΂��R�[���o�b�N�֐�
 */
//--------------------------------------------------------------
typedef struct
{
  BOX2_GFL_PROC_PARAM* box_param;
} BOX_CALLBACK_WORK;

static void callback_BoxProc( void* work )
{
  BOX_CALLBACK_WORK* cw = (BOX_CALLBACK_WORK*)work;
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
  BOX_CALLBACK_WORK* cw = NULL;
  GMEVENT* event = NULL;
  
  // �{�b�N�X�̃v���Z�X�p�����[�^���쐬
  box_param            = GFL_HEAP_AllocMemory( HEAPID_PROC, sizeof(BOX2_GFL_PROC_PARAM) );
  box_param->gamedata  = gdata;
  box_param->sv_box    = GAMEDATA_GetBoxManager( gdata );
  box_param->pokeparty = GAMEDATA_GetMyPokemon( gdata );
  box_param->myitem    = GAMEDATA_GetMyItem( gdata );
  box_param->mystatus  = GAMEDATA_GetMyStatus( gdata );
  box_param->cfg       = SaveData_GetConfig( sv );
  box_param->zknMode   = 0;
  box_param->callMode  = BOX_MODE_SEIRI;

  // �R�[���o�b�N�̃p�����[�^���쐬
  cw = GFL_HEAP_AllocMemory( HEAPID_PROC, sizeof(BOX_CALLBACK_WORK) );
  cw->box_param = box_param;

  // �C�x���g���Ăяo��
  event = EVENT_FieldSubProc_Callback(
      gsys, fieldmap, 
      FS_OVERLAY_ID(box), &BOX2_ProcData, box_param, // �Ăяo���v���Z�X���w��
      callback_BoxProc, cw );  // �R�[���o�b�N�֐���, ���̈������w��
  SCRIPT_CallEvent( scw, event );
  return VMCMD_RESULT_SUSPEND;
}
