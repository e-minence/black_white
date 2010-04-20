//==============================================================================
/**
 * @file    event_intrude.c
 * @brief   �N���F�C�x���g
 * @author  matsuda
 * @date    2009.12.15(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "arc_def.h"

#include "message.naix"

#include "script.h"
#include "event_fieldtalk.h"

#include "print/wordset.h"
#include "event_comm_talk.h"
#include "field/field_comm/intrude_main.h"
#include "field/field_comm/intrude_work.h"
#include "field/field_comm/intrude_comm_command.h"
#include "field/field_comm/bingo_system.h"
#include "field/field_comm/intrude_mission.h"
#include "field/field_comm/intrude_message.h"
#include "msg/msg_invasion.h"
#include "msg/msg_mission_monolith.h"
#include "msg/msg_mission_msg.h"
#include "field/field_comm/intrude_battle.h"
#include "field/event_fieldmap_control.h" //EVENT_FieldSubProc
#include "item/itemsym.h"
#include "event_intrude.h"
#include "field_comm\intrude_field.h"
#include "sound/pm_sndsys.h"
#include "event_mapchange.h"
#include "sound/pm_sndsys.h"
#include "fieldmap/zone_id.h"

#include "../../../resource/fldmapdata/script/common_scr_def.h"

#include "poke_tool/status_rcv.h"

#include "palace_gimmick.h"


//==============================================================================
//  �萔��`
//==============================================================================
enum{
  DISGUISE_ANM_WAIT_MIN = 1,
  DISGUISE_ANM_WAIT_MAX = 15,

  DISGUISE_ANM_WAIT_BARRIER_MAX = 15,
};


//======================================================================
//	typedef struct
//======================================================================
//--------------------------------------------------------------
///	DISGUISE_EVENT_WORK
//--------------------------------------------------------------
typedef struct
{
	HEAPID heapID;
	FIELDMAP_WORK *fieldWork;
  
	INTRUDE_EVENT_MSGWORK iem;
	
	MISSION_DATA mdata;
	INTRUDE_EVENT_DISGUISE_WORK iedw;
	
	u8 mission_view;
	u8 padding[3];
}DISGUISE_EVENT_WORK;


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static GMEVENT_RESULT DisguiseEvent( GMEVENT *event, int *seq, void *wk );


//======================================================================
//	�C�x���g�F�v���C���[�ϐg�C�x���g
//======================================================================
//==================================================================
/**
 * �v���C���[�ϐg�C�x���g�N��
 *
 * @param   gsys		
 * @param   fieldWork		
 * @param   intcomm		      �N���V�X�e�����[�N�ւ̃|�C���^
 * @param   fmmdl_player		���@���샂�f��
 * @param   heap_id		      �q�[�vID
 *
 * @retval  GMEVENT *		
 */
//==================================================================
GMEVENT * EVENT_Disguise(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork,
  INTRUDE_COMM_SYS_PTR intcomm, HEAPID heap_id)
{
	DISGUISE_EVENT_WORK *dis_wk;
	GMEVENT *event = NULL;
  const MISSION_DATA *mdata = MISSION_GetRecvData(&intcomm->mission);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gsys);
	
	event = GMEVENT_Create( gsys, NULL, DisguiseEvent, sizeof(DISGUISE_EVENT_WORK) );
  
	dis_wk = GMEVENT_GetEventWork( event );
	GFL_STD_MemClear( dis_wk, sizeof(DISGUISE_EVENT_WORK) );
	
	dis_wk->heapID = heap_id;
	dis_wk->fieldWork = fieldWork;
	dis_wk->mdata = *mdata;
#if 0
	if(mdata->accept_netid != GAMEDATA_GetIntrudeMyID(gamedata)){
    //�󒍂����̂������Ŗ����Ȃ�A�܂��~�b�V�������e��\��
    dis_wk->mission_view = TRUE;
  }
#else //�N�ł��\���ɕύX�@2010.02.01(��)
  dis_wk->mission_view = TRUE;
#endif

  IntrudeEventPrint_SetupFieldMsg(&dis_wk->iem, gsys);

	//�N���V�X�e���̃A�N�V�����X�e�[�^�X���X�V
	Intrude_SetActionStatus(intcomm, INTRUDE_ACTION_EFFECT);

	return( event );
}

//--------------------------------------------------------------
/**
 * �v���C���[�ϐg�C�x���g
 * @param	event	GMEVENT
 * @param	seq		�V�[�P���X
 * @param	wk		event dis_wk
 */
//--------------------------------------------------------------
static GMEVENT_RESULT DisguiseEvent( GMEVENT *event, int *seq, void *wk )
{
	DISGUISE_EVENT_WORK *dis_wk = wk;
	GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
	INTRUDE_COMM_SYS_PTR intcomm;
  enum{
  	SEQ_MISSION_VIEW,
  	SEQ_ME_START,
  	SEQ_ME_WAIT,
    SEQ_MISSION_VIEW_WAIT,
    SEQ_DISGUISE_START,
    SEQ_DISGUISE_MAIN,
  	SEQ_FINISH,
  };
	
  intcomm = Intrude_Check_CommConnect(game_comm);
  if(intcomm == NULL){
    if((*seq) == SEQ_MISSION_VIEW){ //�N���e�B�J���ȃ^�C�~���O������FINISH�֔�΂�
      *seq = SEQ_FINISH;
    }
  }

	switch( *seq ){
	case SEQ_MISSION_VIEW:
	  if(dis_wk->mission_view == FALSE){
      *seq = SEQ_DISGUISE_START;
      break;
    }
    
    IntrudeEventPrint_SetupExtraMsgWin(&dis_wk->iem, gsys, 1, 1, 32-2, 16);
    
    MISSIONDATA_Wordset(&dis_wk->mdata.cdata, &dis_wk->mdata.target_info, 
      dis_wk->iem.wordset, dis_wk->heapID);
    {
      u16 explain_msgid, title_msgid;
      
      title_msgid = msg_mistype_000 + dis_wk->mdata.cdata.type;
      explain_msgid = dis_wk->mdata.cdata.msg_id_contents_monolith;
      
      IntrudeEventPrint_PrintExtraMsgWin_MissionMono(&dis_wk->iem, title_msgid, 8, 0);
      IntrudeEventPrint_PrintExtraMsgWin_MissionMono(&dis_wk->iem, explain_msgid, 8, 16);
    }
    (*seq)++;
    break;
  
  case SEQ_ME_START:
    GMEVENT_CallEvent(event, EVENT_FSND_PushPlayJingleBGM(gsys, SEQ_ME_MISSION_START ));
    (*seq)++;
    break;
  case SEQ_ME_WAIT:
    if( PMSND_CheckPlayBGM() == FALSE ){
      GMEVENT_CallEvent(event, EVENT_FSND_PopBGM(gsys, FSND_FADE_NONE, FSND_FADE_FAST));
      (*seq)++;
    }
    break;
    
  case SEQ_MISSION_VIEW_WAIT:
    IntrudeEventPrint_ExitExtraMsgWin(&dis_wk->iem);
    (*seq)++;
    break;
  
  case SEQ_DISGUISE_START:
    {
      GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gsys);
      NetID my_net_id = GAMEDATA_GetIntrudeMyID(gamedata);
      IntrudeEvent_Sub_DisguiseEffectSetup(&dis_wk->iedw, gsys, dis_wk->fieldWork, 
        dis_wk->mdata.cdata.obj_id[my_net_id], dis_wk->mdata.cdata.talk_type[my_net_id],
        dis_wk->mdata.cdata.obj_sex[my_net_id]);
      (*seq)++;
    }
    break;
  case SEQ_DISGUISE_MAIN:
    if(IntrudeEvent_Sub_DisguiseEffectMain(&dis_wk->iedw, intcomm) == TRUE){
      (*seq)++;
    }
    break;
	  
	case SEQ_FINISH:   //�I������
    IntrudeEventPrint_ExitFieldMsg(&dis_wk->iem);

  	//�N���V�X�e���̃A�N�V�����X�e�[�^�X���X�V
  	if(intcomm != NULL){
    	Intrude_SetActionStatus(intcomm, INTRUDE_ACTION_FIELD);
    }
		return( GMEVENT_RES_FINISH );
	}
	
	return( GMEVENT_RES_CONTINUE );
}

//==================================================================
/**
 * �ϑ����o�F�Z�b�g�A�b�v
 *
 * @param   iedw		
 * @param   fieldWork		
 * @param   disguise_code		�ϑ�����OBJCODE
 *                  (DISGUISE_NO_NULL�̏ꍇ�͒ʏ�̎p�ADISGUISE_NO_NORMAL�̏ꍇ�̓p���X���̕W���p)
 * @param   disguise_type   �ϑ��^�C�v(TALK_TYPE_xxx)
 * @param   disguise_sex    �ϑ���̐���
 */
//==================================================================
void IntrudeEvent_Sub_DisguiseEffectSetup(INTRUDE_EVENT_DISGUISE_WORK *iedw, GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, u16 disguise_code, u8 disguise_type, u8 disguise_sex)
{
  GFL_STD_MemClear(iedw, sizeof(INTRUDE_EVENT_DISGUISE_WORK));
  iedw->gsys = gsys;
  iedw->fieldWork = fieldWork;
  iedw->disguise_code =disguise_code;
  iedw->disguise_type =disguise_type;
  iedw->disguise_sex =disguise_sex;
  iedw->wait_max = DISGUISE_ANM_WAIT_MAX;
  iedw->wait = iedw->wait_max;
}

//==================================================================
/**
 * �ϑ����o�F���C���V�[�P���X
 *
 * @param   iedw		
 * @param   intcomm ������NULL�`�F�b�N�͂���ׁANULL�ł����Ă����̊֐����Ăё����Ă�������
 *
 * @retval  BOOL		TRUE:�V�[�P���X�I���@FALSE:�V�[�P���X�p����
 */
//==================================================================
BOOL IntrudeEvent_Sub_DisguiseEffectMain(INTRUDE_EVENT_DISGUISE_WORK *iedw, INTRUDE_COMM_SYS_PTR intcomm)
{
  switch(iedw->seq){
  case 0:
    iedw->wait_max = DISGUISE_ANM_WAIT_MAX;
    iedw->wait = iedw->wait_max;
    PMSND_PlaySE( SEQ_SE_FLD_103 );
    iedw->seq++;
    break;
  case 1:
    {
      FIELD_PLAYER * player = FIELDMAP_GetFieldPlayer(iedw->fieldWork);
      MMDL *player_mmdl = FIELD_PLAYER_GetMMdl(player);
      static const u8 player_anm_tbl[] = {
        AC_DIR_U, AC_DIR_L, AC_DIR_D, AC_DIR_R,
      };
      
      if(MMDL_CheckPossibleAcmd(player_mmdl) == TRUE){
        if(iedw->wait_max >= DISGUISE_ANM_WAIT_MAX 
            && iedw->loop > 0
            && player_anm_tbl[iedw->anm_no] == AC_DIR_R){
          iedw->seq++;
          break;
        }
        iedw->anm_no = (iedw->anm_no + 1) % NELEMS(player_anm_tbl);
        iedw->wait = iedw->wait_max;
        if(iedw->loop == 0){
          iedw->wait_max--;
          if(iedw->wait_max <= DISGUISE_ANM_WAIT_MIN){
            GAMEDATA *gamedata = GAMESYSTEM_GetGameData(iedw->gsys);
            IntrudeField_PlayerDisguise(intcomm, iedw->gsys, 
              iedw->disguise_code, iedw->disguise_type, iedw->disguise_sex);
            iedw->loop++;
          }
        }
        else{
          if(iedw->wait_max < DISGUISE_ANM_WAIT_MAX){
            iedw->wait_max++;
          }
        }
        MMDL_SetAcmd(player_mmdl, player_anm_tbl[iedw->anm_no]);
      }
    }
    break;
  default:
    return TRUE;
  }
  
  return FALSE;
}



//----------------------------------------------------------------------------
/**
 *	@brief  Barrier�w���V�� �Z�b�g�A�b�v
 *
 * @param   iedw		
 * @param   fieldWork		
 * @param   disguise_code		�ϑ�����OBJCODE
 *                  (DISGUISE_NO_NULL�̏ꍇ�͒ʏ�̎p�ADISGUISE_NO_NORMAL�̏ꍇ�̓p���X���̕W���p)
 * @param   disguise_type   �ϑ��^�C�v(TALK_TYPE_xxx)
 * @param   disguise_sex    �ϑ���̐���
 * @param   move_dir        �ړ�����
 * @param   change          �ϐg�̗L��
 */
//-----------------------------------------------------------------------------
void IntrudeEvent_Sub_BarrierDisguiseEffectSetup(INTRUDE_EVENT_DISGUISE_WORK *iedw, GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, INTRUDE_COMM_SYS_PTR intcomm, u16 disguise_code, u8 disguise_type, u8 disguise_sex, u16 move_dir, BOOL change )
{
  static MMDL_HEADER TMP_MDL_HEADER = {
    0,
    0,
    MV_DMY,
    0,
    0,
    0,
    DIR_DOWN,
    0,0,0,
    0,0,
    MMDL_HEADER_POSTYPE_GRID,
  };
  
  GFL_STD_MemClear(iedw, sizeof(INTRUDE_EVENT_DISGUISE_WORK));
  iedw->gsys = gsys;
  iedw->fieldWork = fieldWork;
  iedw->disguise_code =disguise_code;
  iedw->disguise_type =disguise_type;
  iedw->disguise_sex =disguise_sex;
  iedw->wait_max = DISGUISE_ANM_WAIT_BARRIER_MAX;
  iedw->wait = iedw->wait_max;
  iedw->dir = move_dir;

  // �_�~�[���f������
  if( change ){
    GAMEDATA* gamedata = GAMESYSTEM_GetGameData(gsys);
    MMDLSYS* mmdlsys = GAMEDATA_GetMMdlSys( gamedata );
    FIELD_PLAYER* player = FIELDMAP_GetFieldPlayer( fieldWork );
    MMDL* fldmmdl = FIELD_PLAYER_GetMMdl( player );
    u16 zone_id = FIELDMAP_GetZoneID( fieldWork );
    
    // �����ڂƈʒu��ݒ�
    TMP_MDL_HEADER.obj_code = MMDL_GetOBJCode( fldmmdl );
    TMP_MDL_HEADER.dir      = move_dir;
    MMDLHEADER_SetGridPos( &TMP_MDL_HEADER, 
        MMDL_GetGridPosX(fldmmdl), MMDL_GetGridPosZ(fldmmdl), MMDL_GetVectorPosY(fldmmdl) );
    iedw->tmp_mdl = MMDLSYS_AddMMdl( mmdlsys, &TMP_MDL_HEADER, zone_id );

    // ���@�̊G�ύX
    IntrudeField_PlayerDisguise(intcomm, iedw->gsys, 
      iedw->disguise_code, iedw->disguise_type, iedw->disguise_sex);

    // ��l���ƃX�C�b�`�B
    MMDL_SetStatusBitVanish( fldmmdl, TRUE );
    MMDL_SetStatusBitVanish( iedw->tmp_mdl, FALSE );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  Barrier�w���V������
 *
 * @param   iedw		
 * @param   intcomm ������NULL�`�F�b�N�͂���ׁANULL�ł����Ă����̊֐����Ăё����Ă�������
 *
 * @retval  BOOL		TRUE:�V�[�P���X�I���@FALSE:�V�[�P���X�p����
 */
//-----------------------------------------------------------------------------
BOOL IntrudeEvent_Sub_BarrierDisguiseEffectMain(INTRUDE_EVENT_DISGUISE_WORK *iedw, INTRUDE_COMM_SYS_PTR intcomm)
{
  enum{
    _SEQ_INIT,
    _SEQ_CHANGE,
    _SEQ_WAIT,
    _SEQ_FINISH,
  };
  FIELD_PLAYER* player = FIELDMAP_GetFieldPlayer( iedw->fieldWork );
  MMDL* fldmmdl = FIELD_PLAYER_GetMMdl( player );
  
  
  switch(iedw->seq){
  case _SEQ_INIT:
    {
      u16 code = (iedw->dir == DIR_RIGHT) ? AC_WALK_R_32F : AC_WALK_L_32F;
      VecFx32 pos;
      
      // �ړ��J�n
      MMDL_SetAcmd(fldmmdl, code);
      if(iedw->tmp_mdl){
        MMDL_SetAcmd(iedw->tmp_mdl, code);
      }

      // BARRIER���oON
      FIELD_PLAYER_GetPos( player, &pos );
      if( iedw->dir == DIR_RIGHT ){
        pos.x += FIELD_CONST_GRID_HALF_FX32_SIZE;
      }else{
        pos.x -= FIELD_CONST_GRID_HALF_FX32_SIZE;
      }
      PALACE_GMK_StartBarrierEffect( iedw->fieldWork, &pos );
    }

    // �T�E���h�͂���H
    PMSND_PlaySE( SEQ_SE_FLD_132 );
    
    iedw->seq++;
    break;
  case _SEQ_CHANGE:
    iedw->wait_max --;
    if( iedw->wait_max <= 0 ){

      TOMOYA_Printf( "change \n" );

      // �w���V��
      if( iedw->tmp_mdl ){
        MMDL_SetStatusBitVanish( fldmmdl, FALSE );
        MMDL_SetStatusBitVanish( iedw->tmp_mdl, TRUE );
      }
      iedw->seq++;
    }
    break;

  case _SEQ_WAIT:

    // BARRIER���o�ƃ��f���ړ��̏I���҂�
    if( PALACE_GMK_IsBarrierEffect( iedw->fieldWork ) ){
      break;
    }
    
    if(MMDL_CheckEndAcmd( fldmmdl ) == TRUE){

      MMDL_EndAcmd( fldmmdl );
      if( iedw->tmp_mdl ){
        MMDL_EndAcmd( iedw->tmp_mdl );
      }
      iedw->seq++;
    }
    break;

  case _SEQ_FINISH:
    if( iedw->tmp_mdl ){
      
      // �I�u�W�F�j��
      MMDL_Delete( iedw->tmp_mdl );
      iedw->tmp_mdl = NULL;
    }
    return TRUE;
  }
  
  return FALSE;
}



//==============================================================================
//
//  
//
//==============================================================================
//--------------------------------------------------------------
/**
 * ��b���n�߂鎞�̍ŏ��̑���̃��A�N�V�����҂��Ȃǂ̏������ꊇ���čs��
 *
 * @param   iem		
 * @param   seq		
 *
 * @retval  FIRST_TALK_RET		
 */
//--------------------------------------------------------------
FIRST_TALK_RET EVENT_INTRUDE_FirstTalkSeq(INTRUDE_COMM_SYS_PTR intcomm, COMMTALK_COMMON_EVENT_WORK *ccew, u8 *seq)
{
  enum{
    SEQ_FIRST_TALK,
    SEQ_TALK_SEND,
    SEQ_TALK_WAIT,
    SEQ_TALK_RECV_WAIT,
    SEQ_TALK_NG,
    SEQ_TALK_NG_MSGWAIT,
    SEQ_TALK_NG_LAST,
    SEQ_END_OK,
    SEQ_END_NG,
    SEQ_END_CANCEL,
  };
  
  switch(*seq){
	case SEQ_FIRST_TALK:
    IntrudeEventPrint_StartStream(&ccew->iem, msg_intrude_000);
		(*seq)++;
		break;
	case SEQ_TALK_SEND:
	  if(IntrudeSend_Talk(intcomm, intcomm->talk.talk_netid, &ccew->mdata, ccew->intrude_talk_type) == TRUE){
      (*seq)++;
    }
    break;
	case SEQ_TALK_WAIT:
    if(IntrudeEventPrint_WaitStream(&ccew->iem) == TRUE){
			(*seq)++;
		}
		break;
	case SEQ_TALK_RECV_WAIT:
	  {
      INTRUDE_TALK_STATUS talk_status = Intrude_GetTalkAnswer(intcomm);
      switch(talk_status){
      case INTRUDE_TALK_STATUS_OK:
        *seq = SEQ_END_OK;
        break;
      case INTRUDE_TALK_STATUS_NG:
      case INTRUDE_TALK_STATUS_CANCEL:
        *seq = SEQ_TALK_NG;
        break;
      default:  //�܂��Ԏ������Ă��Ȃ�
        if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL){
          if(IntrudeSend_TalkCancel(intcomm->talk.talk_netid) == TRUE){
            *seq = SEQ_END_CANCEL;
          }
        }
        break;
      }
    }
		break;

  case SEQ_TALK_NG:   //�f��ꂽ
    IntrudeEventPrint_StartStream(&ccew->iem, msg_intrude_002);
		(*seq)++;
    break;
  case SEQ_TALK_NG_MSGWAIT:
    if(IntrudeEventPrint_WaitStream(&ccew->iem) == TRUE){
			(*seq)++;
		}
		break;
	case SEQ_TALK_NG_LAST:
    if(IntrudeEventPrint_LastKeyWait() == TRUE){
			(*seq) = SEQ_END_NG;
		}
	  break;
	
	case SEQ_END_OK:
	  return FIRST_TALK_RET_OK;
	case SEQ_END_NG:
	  return FIRST_TALK_RET_NG;
	case SEQ_END_CANCEL:
	  return FIRST_TALK_RET_CANCEL;
  default:
    GF_ASSERT(0);
    break;
  }
  
  return FIRST_TALK_RET_NULL;
}


//==================================================================
/**
 * ���ݎ���������ROM�̃p���X�փ��[�v����
 *
 * @param   gsys		
 *
 * @retval  GMEVENT *		
 *
 * �G���[���̖߂�Ƃ��Ă��g�p�o����悤��intcomm�Ɉˑ����Ă��Ȃ�
 * �G���[���͎����̃p���X�փ��[�v����
 */
//==================================================================
GMEVENT * EVENT_IntrudeWarpPalace(GAMESYS_WORK *gsys)
{
  GMEVENT * event;
  VecFx32 pos;
  
  IntrudeField_GetPalaceTownZoneID(ZONE_ID_PALACE01, &pos);
  event = EVENT_ChangeMapPalace_to_Palace( gsys, ZONE_ID_PALACE01, &pos );
  return event;
}

//==================================================================
/**
 * �����̃p���X�փ��[�v����
 *
 * @param   gsys		
 *
 * @retval  GMEVENT *		
 *
 * �G���[���̖߂�Ƃ��Ă��g�p�o����悤��intcomm�Ɉˑ����Ă��Ȃ�
 * �G���[���͎����̃p���X�փ��[�v����
 */
//==================================================================
GMEVENT * EVENT_IntrudeWarpPalace_Mine(GAMESYS_WORK *gsys)
{
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gsys);
  INTRUDE_COMM_SYS_PTR intcomm;
  
  intcomm = Intrude_Check_CommConnect(game_comm);
  if(intcomm != NULL){
    intcomm->intrude_status_mine.palace_area = GAMEDATA_GetIntrudeMyID(gamedata);
  }
  return EVENT_IntrudeWarpPalace(gsys);
}

//==================================================================
/**
 * �w�肵��palace_area�̃p���X�փ��[�v����
 *
 * @param   gsys		
 *
 * @retval  GMEVENT *		
 *
 * �G���[���̖߂�Ƃ��Ă��g�p�o����悤��intcomm�Ɉˑ����Ă��Ȃ�
 * �G���[���͎����̃p���X�փ��[�v����
 */
//==================================================================
GMEVENT * EVENT_IntrudeWarpPalace_NetID(GAMESYS_WORK *gsys, int net_id)
{
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
  INTRUDE_COMM_SYS_PTR intcomm;
  
  intcomm = Intrude_Check_CommConnect(game_comm);
  if(intcomm != NULL){
    intcomm->intrude_status_mine.palace_area = net_id;
  }
  return EVENT_IntrudeWarpPalace(gsys);
}

#if 0

  u16 target_talk_battle[TALK_TYPE_MAX];    ///<�������^�[�Q�b�g�Ő퓬��Ԃ̑���ɘb��������
  u16 target_not_talk[TALK_TYPE_MAX];       ///<�������^�[�Q�b�g�ő��肪�b���������Ȃ����
  u16 mission_talk_battle[TALK_TYPE_MAX];   ///<�������~�b�V�������s�҂Ő퓬��Ԃ̑���ɘb��������
  u16 mission_talk_challenger[TALK_TYPE_MAX];  ///<�������~�b�V�������s�҂Řb����������������s��
  u16 mission_not_talk[TALK_TYPE_MAX];      ///<�������~�b�V�������s�҂ő��肪�b���������Ȃ����
  { //�������^�[�Q�b�g�Ő퓬��Ԃ̑���ɘb��������
    mis_btl_01_t1,
    mis_btl_01_t2,
    mis_btl_01_t3,
    mis_btl_01_t4,
    mis_btl_01_t5,
  },
  { //�������^�[�Q�b�g�ő��肪�b���������Ȃ����
    mis_std_01_t1,
    mis_std_01_t2,
    mis_std_01_t3,
    mis_std_01_t4,
    mis_std_01_t5,
  },
  { //�������~�b�V�������s�҂Ő퓬��Ԃ̑���ɘb��������
    mis_btl_01_m1,
    mis_btl_01_m2,
    mis_btl_01_m3,
    mis_btl_01_m4,
    mis_btl_01_m5,
  },
  { //�������~�b�V�������s�҂Řb����������������s��
    mis_m01_04_m1,
    mis_m01_04_m2,
    mis_m01_04_m3,
    mis_m01_04_m4,
    mis_m01_04_m5,
  },
  { //�������~�b�V�������s�҂ő��肪�b���������Ȃ����
    mis_std_01_m1,
    mis_std_01_m2,
    mis_std_01_m3,
    mis_std_01_m4,
    mis_std_01_m5,
  },
#endif
