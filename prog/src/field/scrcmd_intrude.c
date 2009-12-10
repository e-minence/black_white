//==============================================================================
/**
 * @file    scrcmd_intrude.c
 * @brief   �X�N���v�g�R�}���h�F�N��
 * @author  matsuda
 * @date    2009.10.26(��)
 */
//==============================================================================
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

#include "scrcmd_intrude.h"

#include "field_comm\intrude_types.h"
#include "field_comm\intrude_main.h"
#include "field_comm\intrude_comm_command.h"
#include "field/field_comm/intrude_minimono.h"
#include "field/field_comm/intrude_monolith.h"
#include "field/field_comm/intrude_mission.h"
#include "field/field_comm/intrude_message.h"
#include "field/field_comm/intrude_field.h"

#include "message.naix"
#include "msg/msg_invasion.h"


//==============================================================================
//  �\���̒�`
//==============================================================================
///�C�x���g���䃏�[�N�F�~�b�V�����J�n
typedef struct{
  INTRUDE_EVENT_MSGWORK iem;  ///<�v�����g����
  BOOL error;                 ///<TRUE:�G���[����
  u16 zone_id;                ///<�]�[��ID
  u8 monolith_type;           ///<���m���X�^�C�v
  u8 padding;
}EVENT_MISSION_START;


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static GMEVENT * EVENT_Intrude_MissionStart( GAMESYS_WORK * gsys, u16 monolith_type, ZONEID zone_id );
static GMEVENT_RESULT _event_IntrudeMissionStart( GMEVENT * event, int * seq, void * work );




//==================================================================
/**
 * �~�j���m���X�Z�b�e�B���O
 *
 * @param   core		���z�}�V������\���̂ւ̃|�C���^
 * @param   wk		
 *
 * @retval  VMCMD_RESULT		
 */
//==================================================================
VMCMD_RESULT EvCmdIntrudeMinimonoSetting( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
  
  MINIMONO_AddPosRand(game_comm, fieldWork);
  
  return VMCMD_RESULT_CONTINUE;
}

//==================================================================
/**
 * �~�b�V�����J�n
 *
 * @param   core		
 * @param   wk		
 *
 * @retval  VMCMD_RESULT		
 *
 * �ȉ��̏������s���܂��B
 * �P�D�e�Ƀ~�b�V�����ԍ����N�G�X�g
 * �Q�D�~�b�V�����ԍ���M�҂�
 * �R�D�~�b�V�������b�Z�[�W�\��
 */
//==================================================================
VMCMD_RESULT EvCmdIntrudeMissionStart( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*  work = (SCRCMD_WORK*)wk;
  SCRIPT_WORK*   scw = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK* gsys = SCRCMD_WORK_GetGameSysWork( work );
  MMDL **mmdl = SCRIPT_GetMemberWork( scw, ID_EVSCR_TARGET_OBJ );  //�b���������̂ݗL��
  u16 obj_code = MMDL_GetOBJCode( *mmdl );
  ZONEID zone_id = SCRCMD_WORK_GetZoneID( work );
  u16 monolith_type;
  
  monolith_type = (obj_code == BLACKMONOLITH) ? MONOLITH_TYPE_BLACK : MONOLITH_TYPE_WHITE;
  SCRIPT_CallEvent( scw, EVENT_Intrude_MissionStart(gsys, monolith_type, zone_id) );
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * �C�x���g�F�~�b�V�����J�n
 *
 * @param   gsys		
 * @param   ret_wk		
 * @param   party_index		
 *
 * @retval  GMEVENT *		
 */
//--------------------------------------------------------------
static GMEVENT * EVENT_Intrude_MissionStart(GAMESYS_WORK * gsys, u16 monolith_type, ZONEID zone_id)
{
  GMEVENT *event;
  EVENT_MISSION_START *ems;
  
  event = GMEVENT_Create( gsys, NULL, _event_IntrudeMissionStart, sizeof(EVENT_MISSION_START) );
  ems = GMEVENT_GetEventWork( event );
  ems->monolith_type = monolith_type;
  ems->zone_id = zone_id;
  return event;
}

//--------------------------------------------------------------
/**
 * �C�x���g�����֐��F�~�b�V�����J�n   ��check�@�폜���
 *
 * @param   event		
 * @param   seq		
 * @param   work		
 *
 * @retval  GMEVENT_RESULT		
 * 
 * �ȉ��̏������s���܂��B
 * �P�D�e�Ƀ~�b�V�����ԍ����N�G�X�g
 * �Q�D�~�b�V�����ԍ���M�҂�
 * �R�D�~�b�V�������b�Z�[�W�\��
 */
//--------------------------------------------------------------
static GMEVENT_RESULT _event_IntrudeMissionStart( GMEVENT * event, int * seq, void * work )
{
  EVENT_MISSION_START *ems = work;
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  GAME_COMM_SYS_PTR game_comm= GAMESYSTEM_GetGameCommSysPtr(gsys);
  INTRUDE_COMM_SYS_PTR intcomm;
  enum{
    SEQ_INIT,
    SEQ_MISSION_REQ,
    SEQ_RECV_WAIT,
    SEQ_MSG_INIT,
    SEQ_MSG_WAIT,
    SEQ_MSG_END_BUTTON_WAIT,
    SEQ_END,
  };
  
  intcomm = Intrude_Check_CommConnect(game_comm);
  if(intcomm == NULL){
    if((*seq) > SEQ_INIT && (*seq) < SEQ_MSG_WAIT){
      IntrudeEventPrint_StartStream(&ems->iem, msg_invasion_mission_sys000);
      *seq = SEQ_MSG_WAIT;
      ems->error = TRUE;
    }
  }
  
  switch( *seq ){
  case SEQ_INIT:
    IntrudeEventPrint_SetupFieldMsg(&ems->iem, gsys);
    (*seq)++;
    break;
  case SEQ_MISSION_REQ:
    if(MISSION_RecvCheck(&intcomm->mission) == TRUE || 
        IntrudeSend_MissionReq(intcomm, ems->monolith_type, ems->zone_id) == TRUE){
      *seq = SEQ_RECV_WAIT;
    }
    break;
  case SEQ_RECV_WAIT:
    if(MISSION_RecvCheck(&intcomm->mission) == TRUE){
      *seq = SEQ_MSG_INIT;
    }
    break;
  case SEQ_MSG_INIT:
    {
      GAMEDATA *gdata = GAMESYSTEM_GetGameData(gsys);
      MYSTATUS *target_myst = GAMEDATA_GetMyStatusPlayer(gdata,intcomm->mission.data.target_netid);
      u32 msg_id;

      msg_id = MISSION_GetMissionMsgID(&intcomm->mission);
      if(ems->error == FALSE && intcomm->mission.data.accept_netid != INTRUDE_NETID_NULL){
        WORDSET_RegisterPlayerName( ems->iem.wordset, 0, target_myst );
      }

      IntrudeEventPrint_StartStream(&ems->iem, msg_id);
    }
    *seq = SEQ_MSG_WAIT;
    break;
  case SEQ_MSG_WAIT:
    if(IntrudeEventPrint_WaitStream(&ems->iem) == TRUE){
      *seq = SEQ_MSG_END_BUTTON_WAIT;
    }
    break;
  case SEQ_MSG_END_BUTTON_WAIT:
    if(IntrudeEventPrint_LastKeyWait() == TRUE){
      *seq = SEQ_END;
    }
    break;
  case SEQ_END:
    if(intcomm != NULL){
      IntrudeField_PlayerDisguise(intcomm, gsys);
    }
    IntrudeEventPrint_ExitFieldMsg(&ems->iem);
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//==================================================================
/**
 * �p���X���̏����}�b�v�A���Z�b�e�B���O
 *
 * @param   core		���z�}�V������\���̂ւ̃|�C���^
 * @param   wk		
 *
 * @retval  VMCMD_RESULT		
 */
//==================================================================
VMCMD_RESULT EvCmdIntrudeConnectMapSetting( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
  INTRUDE_COMM_SYS_PTR intcomm = NULL;
  
  if(GameCommSys_BootCheck(game_comm) == GAME_COMM_NO_INVASION){
    intcomm = GameCommSys_GetAppWork(game_comm);
  }

  if(intcomm != NULL){
    IntrudeField_ConnectMapInit(fieldWork, gsys, intcomm);
    IntrudeField_ConnectMap(fieldWork, gsys, intcomm);
  }
  
  MONOLITH_AddConnectAllMap(fieldWork);

  return VMCMD_RESULT_CONTINUE;
}

