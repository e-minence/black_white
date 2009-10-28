//======================================================================
/**
 * @file scr_musical.c
 * @brief �X�N���v�g�R�}���h�@�~���[�W�J���֘A
 * @author	Satoshi Nohara
 * @date	06.06.26
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "system/vm_cmd.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "fieldmap.h"

#include "script.h"
#include "script_local.h"
#include "script_def.h"
#include "scrcmd.h"
#include "scrcmd_work.h"

#include "musical/musical_system.h"
#include "musical/musical_event.h"

#include "poke_tool/poke_tool.h"  //�h���X�A�b�v���f�[�^�p
#include "poke_tool/monsno_def.h" //�h���X�A�b�v���f�[�^�p
#include "event_fieldmap_control.h"

#include "field_sound.h"

#include "scrcmd_musical.h"

#include "../../../resource/fldmapdata/script/musical_scr_local.h"

//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
/// EVENT_MUSICAL_WORK
//--------------------------------------------------------------
typedef struct
{
  MUSICAL_INIT_WORK init;
  GMEVENT *event;
}EVENT_MUSICAL_WORK;

//======================================================================
//  proto
//======================================================================
static GMEVENT_RESULT event_Musical( GMEVENT *event, int *seq, void *work );

//======================================================================
//  �X�N���v�g�R�}���h�@�~���[�W�J��
//======================================================================
//--------------------------------------------------------------
/**
 * �~���[�W�J���F�~���[�W�J���Ăяo��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdMusicalCall( VMHANDLE *core, void *wk )
{
  GMEVENT *call_event;
  MUSICAL_INIT_WORK *init;
  EVENT_MUSICAL_WORK *ev_musical_work;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  
  u8  mode = VMGetU8(core);
  u16 pokeIdx = SCRCMD_GetVMWorkValue( core, work );

  call_event = GMEVENT_Create(
      gsys, NULL, event_Musical, sizeof(EVENT_MUSICAL_WORK) );
  ev_musical_work = GMEVENT_GetEventWork( call_event );
	MI_CpuClear8( ev_musical_work, sizeof(EVENT_MUSICAL_WORK) );
  
  init = &ev_musical_work->init;
  init->saveCtrl = GAMEDATA_GetSaveControlWork( gdata );
  init->pokePara = PP_Create(
      MONSNO_PIKUSII , 20, PTL_SETUP_POW_AUTO , HEAPID_PROC );
  init->isComm = FALSE;
  init->isDebug = FALSE;
  init->gameComm = GAMESYSTEM_GetGameCommSysPtr( gsys );
  
//  ev_musical_work->event = EVENT_FieldSubProc( gsys, fparam->fieldMap,
//    NO_OVERLAY_ID, &Musical_ProcData, init );
  ev_musical_work->event = MUSICAL_CreateEvent( gsys, gdata , FALSE );
   
  //�X�N���v�g�I����A�w�肵���C�x���g�ɑJ�ڂ���
  SCRIPT_EntryNextEvent( sc, call_event );
  
  {
    FIELD_SOUND *fsnd = GAMEDATA_GetFieldSound( gdata );
    PMSND_StopBGM();
  //  FIELD_SOUND_PushBGM( fsnd );
  //  PMSND_FadeOutBGM( 30 );
  }
  
  VM_End( core );
  //return VMCMD_RESULT_CONTINUE;;
  return VMCMD_RESULT_SUSPEND;;
}

//--------------------------------------------------------------
/**
 * �~���[�W�J���F�~���[�W�J���n���[�h�Z�b�g
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdMusicalFittingCall( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;

  u16 pokeIdx = SCRCMD_GetVMWorkValue( core, work );

  return VMCMD_RESULT_CONTINUE;
}

VMCMD_RESULT EvCmdGetMusicalValue( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;

  u8   type = VMGetU8(core);
  u16  val = SCRCMD_GetVMWorkValue( core, work );
  u16* ret_wk = SCRCMD_GetVMWork( core, work );

  return VMCMD_RESULT_CONTINUE;
}

VMCMD_RESULT EvCmdGetMusicalFanValue( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;

  u16  pos  = SCRCMD_GetVMWorkValue( core, work );
  u8   type = VMGetU8(core);
  u16* ret_wk = SCRCMD_GetVMWork( core, work );

  return VMCMD_RESULT_CONTINUE;
}

VMCMD_RESULT EvCmdGetMusicalWaitRoomValue( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );

  u8   type = VMGetU8(core);
  u16  val   = SCRCMD_GetVMWorkValue( core, work );
  u16* ret_wk = SCRCMD_GetVMWork( core, work );

  MUSICAL_EVENT_WORK *musWork = SCRIPT_GetMemberWork( sc , ID_EVSCR_MUSICAL_EVENT_WORK );

  switch( type )
  {
  case MUSICAL_VALUE_WR_SELF_IDX:  //�����̎Q���ԍ�
    *ret_wk = MUSICAL_EVENT_GetSelfIndex( musWork );
    break;
  case MUSICAL_VALUE_WR_MAX_POINT: //�ō��]���_
    break;
  case MUSICAL_VALUE_WR_MIN_POINT: //�ō����_
    break;
  case MUSICAL_VALUE_WR_GRADE_MSG: //�]�����b�Z�[�W�̎擾
    break;
  default:
    GF_ASSERT_MSG( 0 , "�^�C�v�w�肪�Ԉ���Ă�[%d]\n",type );
    break;
  }

  return VMCMD_RESULT_CONTINUE;
}

VMCMD_RESULT EvCmdAddMusicalGoods( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  u16  val   = SCRCMD_GetVMWorkValue( core, work );

  return VMCMD_RESULT_CONTINUE;
}

VMCMD_RESULT EvCmdMusicalWord( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;

  u8   type = VMGetU8(core);
  u8   idx  = VMGetU8(core);
  u16  val  = SCRCMD_GetVMWorkValue( core, work );

  return VMCMD_RESULT_CONTINUE;
}


//======================================================================
//  �~���[�W�J���@�C�x���g����
//======================================================================
//--------------------------------------------------------------
/**
 * @param
 * @retval
 */
//--------------------------------------------------------------
static GMEVENT_RESULT event_Musical(
    GMEVENT *event, int *seq, void *work )
{
  EVENT_MUSICAL_WORK *ev_musical_work = work;
  
  switch( (*seq) ){
  case 0:
    GMEVENT_CallEvent( event, ev_musical_work->event );
    (*seq)++;
    break;
  case 1:
    {
      MUSICAL_INIT_WORK *init = &ev_musical_work->init;
      GFL_HEAP_FreeMemory( init->pokePara );
    }

    {
      GAMESYS_WORK *gsys =  GMEVENT_GetGameSysWork( event );
      GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
      FIELD_SOUND *fsnd = GAMEDATA_GetFieldSound( gdata );
      //FIELD_SOUND_PopBGM( fsnd );
      PMSND_FadeInBGM( 30 );
    }
    (*seq)++;
    return( GMEVENT_RES_FINISH );
  }
  return( GMEVENT_RES_CONTINUE );
}
