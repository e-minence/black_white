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
#include "savedata/save_control.h"
#include "savedata/musical_save.h"
#include "savedata/musical_dist_save.h"

#include "poke_tool/poke_tool.h"  //�h���X�A�b�v���f�[�^�p
#include "poke_tool/monsno_def.h" //�h���X�A�b�v���f�[�^�p
#include "event_fieldmap_control.h"

#include "field_sound.h"
#include "message.naix"
#include "msg/script/msg_musical_scr.h"
#include "msg/msg_musical_common.h"
#include "msg/msg_mus_item_name.h"
#include "scrcmd_musical.h"

#include "../../../resource/fldmapdata/script/musical_scr_local.h"
#include "fieldmap/fldmmdl_objcode.h"

#include "test/ariizumi/ari_debug.h"

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
  //  PMSND_StopBGM();
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

//--------------------------------------------------------------
/**
 * �~���[�W�J���F�~���[�W�J���ėp���l�擾
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetMusicalValue( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;

  u8   type = VMGetU8(core);
  u16  val = SCRCMD_GetVMWorkValue( core, work );
  u16* ret_wk = SCRCMD_GetVMWork( core, work );

  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  SAVE_CONTROL_WORK *svWork = GAMEDATA_GetSaveControlWork( gdata );
  MUSICAL_SAVE* musSave = MUSICAL_SAVE_GetMusicalSave( svWork );

  switch( type )
  {
  case MUSICAL_VALUE_JOINNUM:       //�Q����
    *ret_wk = MUSICAL_SAVE_GetEntryNum( musSave );
    break;
  case MUSICAL_VALUE_TOPNUM:        //�g�b�v��
    *ret_wk = MUSICAL_SAVE_GetTopNum( musSave );
    break;
  case MUSICAL_VALUE_LAST_POINT:    //�ŏI�]���_
    *ret_wk = MUSICAL_SAVE_GetBefPoint( musSave );
    break;
  case MUSICAL_VALUE_LAST_CONDITION://�ŏI�R���f�B�V����
    break;
  }

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �~���[�W�J���F�~���[�W�J���t�@���p���l�擾
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetMusicalFanValue( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;

  u16  pos  = SCRCMD_GetVMWorkValue( core, work );
  u8   type = VMGetU8(core);
  u16* ret_wk = SCRCMD_GetVMWork( core, work );

  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  SAVE_CONTROL_WORK *svWork = GAMEDATA_GetSaveControlWork( gdata );
  MUSICAL_SAVE* musSave = MUSICAL_SAVE_GetMusicalSave( svWork );
  MUSICAL_FAN_STATE *fanState = MUSICAL_SAVE_GetFanState( musSave , pos );
  
  switch( type )
  {
  case MUSICAL_VALUE_FAN_TYPE:        //������
    switch( fanState->type )
    {
    case MCT_COOL:
      *ret_wk = MIDDLEMAN1;
      break;
    case MCT_CUTE:
      *ret_wk = GIRL2;
      break;
    case MCT_ELEGANT:
      *ret_wk = LADY;
      break;
    case MCT_UNIQUE:
      *ret_wk = BOY3;
      break;
    case MCT_MAX:
      *ret_wk = MUSICAL_FAL_TYPE_NONE; // ���Ȃ��̂�0
      break;
    }
    break;
  case MUSICAL_VALUE_FAN_CHEER_MSG:   //�������b�Z�[�W
    *ret_wk = msg_musical_fan_cheer_01 + fanState->type;
    break;
  case MUSICAL_VALUE_FAN_GIFT_MSG:    //�v���[���g���b�Z�[�W
    *ret_wk = msg_musical_fan_gift_01 + fanState->type;
    break;
  case MUSICAL_VALUE_FAN_GIFT_TYPE:   //�v���[���g���
    *ret_wk = fanState->giftType;
    break;
  case MUSICAL_VALUE_FAN_GIFT_NUMBER: //�v���[���g�ԍ�
    *ret_wk = fanState->giftValue;
    break;
  }

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �~���[�W�J���F�~���[�W�J���T�����p���l�擾
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetMusicalWaitRoomValue( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );

  u8   type = VMGetU8(core);
  u16  val   = SCRCMD_GetVMWorkValue( core, work );
  u16* ret_wk = SCRCMD_GetVMWork( core, work );

  MUSICAL_EVENT_WORK *evWork = SCRIPT_GetMemberWork( sc , ID_EVSCR_MUSICAL_EVENT_WORK );

  switch( type )
  {
  case MUSICAL_VALUE_WR_SELF_IDX:  //�����̎Q���ԍ�
    *ret_wk = MUSICAL_EVENT_GetSelfIndex( evWork );
    ARI_TPrintf("MusValWR:SelfIdx[%d]\n",*ret_wk);
    break;
  case MUSICAL_VALUE_WR_MAX_POINT: //�ō��]���_
    *ret_wk = MUSICAL_EVENT_GetMaxPoint( evWork );
    ARI_TPrintf("MusValWR:MaxPoint[%d]\n",*ret_wk);
    break;
  case MUSICAL_VALUE_WR_MIN_POINT: //�Œ�]���_
    *ret_wk = MUSICAL_EVENT_GetMinPoint( evWork );
    ARI_TPrintf("MusValWR:MinPoint[%d]\n",*ret_wk);
    break;
  case MUSICAL_VALUE_WR_POINT:     //�l���_
    *ret_wk = MUSICAL_EVENT_GetPoint( evWork , val );
    ARI_TPrintf("MusValWR:Point[%d:%d]\n",val,*ret_wk);
    break;
  case MUSICAL_VALUE_WR_GRADE_MSG: //�]�����b�Z�[�W�̎擾
    break;
  case MUSICAL_VALUE_WR_POS_TO_IDX: //�����ʒu�ɑΉ������Q���ԍ�
    *ret_wk = MUSICAL_EVENT_GetPosIndex( evWork , val );
    ARI_TPrintf("MusValWR:PosIdx[%d:%d]\n",val,*ret_wk);
    break;
  case MUSICAL_VALUE_WR_MAX_CONDITION: //���ڂ̍����R���f�B�V����
    *ret_wk = MUSICAL_EVENT_GetMaxCondition( evWork  );
    ARI_TPrintf("MusValWR:MaxCondition[%d]\n",*ret_wk);
    break;
  case MUSICAL_VALUE_WR_POS_TO_RANK://���ʂɑΉ������Q���ԍ�
    *ret_wk = MUSICAL_EVENT_GetPosToRank( evWork , val );
    ARI_TPrintf("MusValWR:PosRank[%d:%d]\n",val,*ret_wk);
    break;
  case MUSICAL_VALUE_WR_POKE_MAX_POINT_CON: //�ł��_�����������R���f�B�V����
    *ret_wk = MUSICAL_EVENT_GetMaxPointCondition( evWork , val );
    ARI_TPrintf("MusValWR:MaxPointCon[%d:%d]\n",val,*ret_wk);
    break;
  default:
    GF_ASSERT_MSG( 0 , "�^�C�v�w�肪�Ԉ���Ă�[%d]\n",type );
    break;
  }

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �~���[�W�J���F�~���[�W�J���O�b�Y�ǉ�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdAddMusicalGoods( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;

  u16  val   = SCRCMD_GetVMWorkValue( core, work );

  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  SAVE_CONTROL_WORK *svWork = GAMEDATA_GetSaveControlWork( gdata );
  MUSICAL_SAVE* musSave = MUSICAL_SAVE_GetMusicalSave( svWork );

  MUSICAL_SAVE_AddItem( musSave , val );

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �~���[�W�J���F�~���[�W�J���n���[�h�Z�b�g
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdMusicalWord( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  HEAPID heapId = SCRCMD_WORK_GetHeapID( work );
  WORDSET **wordset    = SCRIPT_GetMemberWork( sc, ID_EVSCR_WORDSET );

  u8   type = VMGetU8(core);
  u8   idx  = VMGetU8(core);
  u16  val  = SCRCMD_GetVMWorkValue( core, work );

  switch( type )
  {
  case MUSICAL_WORD_TITLE:        //�Z�[�u�ɂ��鉉��
    {
      GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
      SAVE_CONTROL_WORK *svWork = GAMEDATA_GetSaveControlWork( gdata );
      MUSICAL_DIST_SAVE* musDistSave = MUSICAL_DIST_SAVE_LoadData( svWork , heapId );
      void *msgData = MUSICAL_DIST_SAVE_GetMessageData( musDistSave , heapId , NULL );

      STRBUF * tmpBuf;
      GFL_MSGDATA *msgHandle = GFL_MSG_Construct( msgData , heapId );

      tmpBuf = GFL_MSG_CreateString( msgHandle , 0 );
      WORDSET_RegisterWord( *wordset, idx, tmpBuf, 0, TRUE, PM_LANG );

      GFL_STR_DeleteBuffer( tmpBuf );
      GFL_MSG_Delete( msgHandle );
      GFL_HEAP_FreeMemory( msgData );
    }
    break;
  case MUSICAL_WORD_GOODS:        //�O�b�Y��
    {
      STRBUF * tmpBuf;
      GFL_MSGDATA *msgHandle = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL , ARCID_MESSAGE , NARC_message_mus_item_name_dat , heapId );

      tmpBuf = GFL_MSG_CreateString( msgHandle , ITEM_NAME_000+val );
      WORDSET_RegisterWord( *wordset, idx, tmpBuf, 0, TRUE, PM_LANG );

      GFL_STR_DeleteBuffer( tmpBuf );
      GFL_MSG_Delete( msgHandle );
    }
    break;
  case MUSICAL_WORD_TITLE_LOCAL:  //�����݉���
    {
      MUSICAL_EVENT_WORK *evWork = SCRIPT_GetMemberWork( sc , ID_EVSCR_MUSICAL_EVENT_WORK );
      STRBUF * tmpBuf = MUSICAL_EVENT_CreateStr_ProgramTitle( evWork , heapId );
      WORDSET_RegisterWord( *wordset, idx, tmpBuf, 0, TRUE, PM_LANG );
      GFL_STR_DeleteBuffer( tmpBuf );
    }
    break;
  case MUSICAL_WORD_AUDI_TYPE:    //�l�C�q�w
    {
      STRBUF * tmpBuf;
      GFL_MSGDATA *msgHandle = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL , ARCID_MESSAGE , NARC_message_musical_common_dat , heapId );

      tmpBuf = GFL_MSG_CreateString( msgHandle , MUSICAL_AUDIENCE_TYPE_01+val );
      WORDSET_RegisterWord( *wordset, idx, tmpBuf, 0, TRUE, PM_LANG );

      GFL_STR_DeleteBuffer( tmpBuf );
      GFL_MSG_Delete( msgHandle );
    }
    break;
  case MUSICAL_WORD_CONDITION:  //�R���f�B�V������
    {
      STRBUF * tmpBuf;
      GFL_MSGDATA *msgHandle = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL , ARCID_MESSAGE , NARC_message_musical_common_dat , heapId );

      tmpBuf = GFL_MSG_CreateString( msgHandle , MUSICAL_AUDIENCE_CON_01+val );
      WORDSET_RegisterWord( *wordset, idx, tmpBuf, 0, TRUE, PM_LANG );

      GFL_STR_DeleteBuffer( tmpBuf );
      GFL_MSG_Delete( msgHandle );
    }
    break;
  }

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �~���[�W�J���F�~���[�W�J���n�t�@�����蕨�t���O���Ƃ�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdResetMusicalFanGiftFlg( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;

  u16  pos  = SCRCMD_GetVMWorkValue( core, work );

  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  SAVE_CONTROL_WORK *svWork = GAMEDATA_GetSaveControlWork( gdata );
  MUSICAL_SAVE* musSave = MUSICAL_SAVE_GetMusicalSave( svWork );
  MUSICAL_FAN_STATE *fanState = MUSICAL_SAVE_GetFanState( musSave , pos );
  
  fanState->giftType = MUSICAL_GIFT_TYPE_NONE;
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
