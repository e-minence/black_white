//======================================================================
/**
 * @file	scrcmd_trainer.c
 * @brief	�X�N���v�g�R�}���h�F�g���[�i�[�֘A
 * @author	Satoshi Nohara
 * @date	06.06.26
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/vm_cmd.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "fieldmap.h"

#include "script.h"
#include "script_local.h"
#include "script_trainer.h"
#include "script_def.h"
#include "scrcmd.h"
#include "scrcmd_work.h"

#include "scrcmd_trainer.h"

#include "event_trainer_eye.h"

#include "tr_tool/tr_tool.h"		//TT_TrainerMessageGet
#include "tr_tool/trno_def.h"
#include "arc/fieldmap/fldmmdl_objcode.h"

#include "event_battle.h"

#include "field_sound.h"

#include "trainer_eye_data.h"

#include "field_event_check.h"

#include "event_field_fade.h" //EVENT_FieldFadeIn_Black
#if 0
#include "battle/battle_common.h"	//���C���N���[�h�ɕK�v
#include "ev_trainer.h"				//EvTrainer
#include "field_encount.h"			//DebugFieldEncount
#include "field_battle.h"			//BattleParam_IsWinResult
#include "ev_pokemon.h"				//EvPoke_Add
#endif

//======================================================================
//	�v�����g�^�C�v�錾
//======================================================================

//======================================================================
//	�R�}���h
//======================================================================
//--------------------------------------------------------------
/**
 * �����F�g���[�i�[�ړ��Ăяo��
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @return	"0"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdEyeTrainerMoveSet( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
	u16 pos = SCRCMD_GetVMWorkValue( core, work ); //�����f�[�^��0,1���H
  //GMEVENT **ev_eye_move;
  EV_TRAINER_EYE_HITDATA * eye;

  if( pos == 0 ){ //����0
    eye = SCRIPT_GetTrainerEyeData( sc, TRAINER_EYE_HIT0 );
  }else{
    eye = SCRIPT_GetTrainerEyeData( sc, TRAINER_EYE_HIT1 );
  }
  
  eye->ev_eye_move = EVENT_SetTrainerEyeMove( fparam->fieldMap,
      eye->mmdl, FIELDMAP_GetFieldPlayer(fparam->fieldMap),
      eye->dir, eye->range, 0, eye->tr_type, pos );
  
	return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �g���[�i�[�ړ��I���҂��@0
 * @param
 * @retval
 */
//--------------------------------------------------------------
static BOOL EvWaitTrainer0Move( VMHANDLE *core, void *wk )
{
  GMEVENT_RESULT res;
  GMEVENT **ev_eye_move;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  EV_TRAINER_EYE_HITDATA * eye = SCRIPT_GetTrainerEyeData( sc, TRAINER_EYE_HIT0 );

  ev_eye_move = &eye->ev_eye_move;
  res = GMEVENT_Run( *ev_eye_move );
  
  if( res == GMEVENT_RES_FINISH ){
    GMEVENT_Delete( *ev_eye_move );
    *ev_eye_move = NULL;
    return( TRUE );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �g���[�i�[�ړ��I���҂��@1
 * @param
 * @retval
 */
//--------------------------------------------------------------
static BOOL EvWaitTrainer1Move( VMHANDLE *core, void *wk )
{
  GMEVENT_RESULT res;
  GMEVENT **ev_eye_move;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  EV_TRAINER_EYE_HITDATA * eye = SCRIPT_GetTrainerEyeData( sc, TRAINER_EYE_HIT1 );

  ev_eye_move = &eye->ev_eye_move;
  res = GMEVENT_Run( *ev_eye_move );
  
  if( res == GMEVENT_RES_FINISH ){
    GMEVENT_Delete( *ev_eye_move );
    *ev_eye_move = NULL;
    return( TRUE );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �g���[�i�[�ړ��I���҂��@0&1
 * @param
 * @retval
 */
//--------------------------------------------------------------
static BOOL EvWaitTrainer01Move( VMHANDLE *core, void *wk )
{
  GMEVENT **ev_eye_move0;
  GMEVENT **ev_eye_move1;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GMEVENT_RESULT res0,res1;
  EV_TRAINER_EYE_HITDATA * eye0 = 
    SCRIPT_GetTrainerEyeData( sc, TRAINER_EYE_HIT0 );
  EV_TRAINER_EYE_HITDATA * eye1 = 
    SCRIPT_GetTrainerEyeData( sc, TRAINER_EYE_HIT1 );

  ev_eye_move0 = &eye0->ev_eye_move;
  
  if( *ev_eye_move0 != NULL ){
    res0 = GMEVENT_Run( *ev_eye_move0 );
  }else{
    res0 = GMEVENT_RES_FINISH;
  }
  
  ev_eye_move1 = &eye1->ev_eye_move;
  
  if( *ev_eye_move1 != NULL ){
    res1 = GMEVENT_Run( *ev_eye_move1 );
  }else{
    res1 = GMEVENT_RES_FINISH;
  }
  
  if( res0 == GMEVENT_RES_FINISH && res1 == GMEVENT_RES_FINISH ){
    if( *ev_eye_move0 != NULL ){
      GMEVENT_Delete( *ev_eye_move0 );
    }
    if( *ev_eye_move1 != NULL ){
      GMEVENT_Delete( *ev_eye_move1 );
    }
    *ev_eye_move0 = NULL;
    *ev_eye_move1 = NULL;
    return( TRUE );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �����F�g���[�i�[�ړ��@�P��
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @return	"1"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdEyeTrainerMoveSingle( VMHANDLE *core, void *wk )
{
  GMEVENT **ev_eye_move;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
	u16 pos = SCRCMD_GetVMWorkValue( core, work ); //�����f�[�^��0,1���H
  
	if( pos == SCR_EYE_TR_0 ){
    EV_TRAINER_EYE_HITDATA * eye0 =
      SCRIPT_GetTrainerEyeData( sc, TRAINER_EYE_HIT0 );
    ev_eye_move = &eye0->ev_eye_move;
	}else{
    EV_TRAINER_EYE_HITDATA * eye1 =
      SCRIPT_GetTrainerEyeData( sc, TRAINER_EYE_HIT1 );
    ev_eye_move = &eye1->ev_eye_move;
	}
  
	//�o�^����Ă��Ȃ���
  if( *ev_eye_move == NULL ){
    return 0;
  }
  
  if( pos == SCR_EYE_TR_0 ){
    VMCMD_SetWait( core, EvWaitTrainer0Move );
  }else{
    VMCMD_SetWait( core, EvWaitTrainer1Move );
  }
   
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * �����F�g���[�i�[�ړ��@�_�u��
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @return	"1"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdEyeTrainerMoveDouble( VMHANDLE *core, void *wk )
{
  GMEVENT **ev_eye_move0;
  GMEVENT **ev_eye_move1;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  EV_TRAINER_EYE_HITDATA * eye0 =
    SCRIPT_GetTrainerEyeData( sc, TRAINER_EYE_HIT0 );
  EV_TRAINER_EYE_HITDATA * eye1 =
    SCRIPT_GetTrainerEyeData( sc, TRAINER_EYE_HIT1 );
  
  ev_eye_move0 = &eye0->ev_eye_move;
  ev_eye_move1 = &eye1->ev_eye_move;
  
	//�o�^����Ă��Ȃ���
  if( *ev_eye_move0 == NULL && *ev_eye_move1 == NULL ){
    return 0;
  }
  
  if( *ev_eye_move0 == NULL || *ev_eye_move1 == NULL ){
    OS_Printf( "�����f�[�^�ُ�\n" );
  }

  VMCMD_SetWait( core, EvWaitTrainer01Move );
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * �����F�g���[�i�[�^�C�v�擾
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @return	"0"
 * TR0_TYPE�ɌŒ�I
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdEyeTrainerTypeGet( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  EV_TRAINER_EYE_HITDATA * eye0 = SCRIPT_GetTrainerEyeData( sc, TRAINER_EYE_HIT0 );
	u16 *ret_wk	= SCRCMD_GetVMWork( core, work );
  *ret_wk = eye0->tr_type;
	return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �����F�g���[�i�[ID�擾
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @return	"0"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdEyeTrainerIdGet( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  EV_TRAINER_EYE_HITDATA * eye0 = SCRIPT_GetTrainerEyeData( sc, TRAINER_EYE_HIT0 );
  EV_TRAINER_EYE_HITDATA * eye1 = SCRIPT_GetTrainerEyeData( sc, TRAINER_EYE_HIT1 );
	u16 pos = SCRCMD_GetVMWorkValue( core, work ); //�����f�[�^��0,1���H
	u16 *ret_wk		= SCRCMD_GetVMWork( core, work );
  *ret_wk = (pos == SCR_EYE_TR_0) ? (eye0->tr_id) : (eye1->tr_id);
  KAGAYA_Printf( "�����g���[�i�[ID�擾 %d\n", *ret_wk );
	return VMCMD_RESULT_CONTINUE;
}

//======================================================================
//	�퓬�֘A(������)
//======================================================================
//--------------------------------------------------------------
/**
 * �g���[�i�[ID�擾
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @return	"0"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTrainerIdGet( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  u16 script_id   = SCRIPT_GetStartScriptID( sc );
	u16 *ret_wk = SCRCMD_GetVMWork( core, work );
  
	//�X�N���v�gID����A�g���[�i�[ID���擾
	*ret_wk = SCRIPT_GetTrainerID_ByScriptID( script_id );
	return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �g���[�i�[�퓬�Ăяo��
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @return	"1"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTrainerBattleSet( VMHANDLE *core, void *wk )
{
  u32 fight_type;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  u16 script_id   = SCRIPT_GetStartScriptID( sc );
	u16 tr_id_0 = SCRCMD_GetVMWorkValue( core, work );
	u16 tr_id_1 = SCRCMD_GetVMWorkValue( core, work );
  u16 flags = SCRCMD_GetVMWorkValue( core, work );
  u8 rule;

  rule = SCRIPT_GetTrainerBtlRule( tr_id_0 );

  //�@�b����������_�u���o�g���ւ̑Ή�
  // tr_id_0 == tr_id_1�̂Ƃ��A�_�u���o�g������������
  if (tr_id_1 == 0 && SCRIPT_CheckTrainer2vs2Type( tr_id_0 ) == 1 )
  {
    tr_id_1 = tr_id_0;
  }
  {
    GAMESYS_WORK *gsys = SCRIPT_GetGameSysWork( sc );
    SCRIPT_FLDPARAM * fparam = SCRIPT_GetFieldParam( sc );
    GMEVENT *ev_battle =
      EVENT_TrainerBattle( gsys, fparam->fieldMap, rule, TRID_NULL, tr_id_0, tr_id_1, flags );
    SCRIPT_CallEvent( sc, ev_battle );
  }
	return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * �g���[�i�[�ΐ�Ăяo���iAI�}���`�o�g���j
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTrainerMultiBattleSet( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  u16 script_id   = SCRIPT_GetStartScriptID( sc );
	u16 partner_id = SCRCMD_GetVMWorkValue( core, work );
	u16 tr_id_0 = SCRCMD_GetVMWorkValue( core, work );
	u16 tr_id_1 = SCRCMD_GetVMWorkValue( core, work );
  u16 flags = SCRCMD_GetVMWorkValue( core, work );
  
  {
    GAMESYS_WORK *gsys = SCRIPT_GetGameSysWork( sc );
    SCRIPT_FLDPARAM * fparam = SCRIPT_GetFieldParam( sc );
    GMEVENT *ev_battle =
      EVENT_TrainerBattle( gsys, fparam->fieldMap, BTL_RULE_DOUBLE, partner_id, tr_id_0, tr_id_1, flags );
    SCRIPT_CallEvent( sc, ev_battle );
  }
	return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * �g���[�i�[��b�̎�ގ擾
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @return	"0"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTrainerTalkTypeGet( VMHANDLE *core, void *wk )
{
	u16 btl_type,lr,start_type,after_type,one_type;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  u16 script_id   = SCRIPT_GetStartScriptID( sc );
	u16 *wk1				= SCRCMD_GetVMWork( core, work );
	u16 *wk2				= SCRCMD_GetVMWork( core, work );
	u16 *wk3				= SCRCMD_GetVMWork( core, work );
  
	//�X�N���v�gID����A�g���[�i�[ID���擾�A�_�u���o�g���^�C�v���擾
	btl_type = SCRIPT_CheckTrainer2vs2Type(
      SCRIPT_GetTrainerID_ByScriptID(script_id) );
  
	//�V���O�����_�u�����`�F�b�N
	if( btl_type == 0 ){
		//�V���O��
		start_type = TRMSG_FIGHT_START;
		after_type = TRMSG_FIGHT_AFTER;
		one_type   = 0;
	}else{
		//�_�u��
		lr = SCRIPT_GetTrainerLR_ByScriptID( script_id );
    
		if( lr == 0 ){
			//��
			start_type = TRMSG_FIGHT_START_1;
			after_type = TRMSG_FIGHT_AFTER_1;
			one_type = TRMSG_POKE_ONE_1;
		}else{
			//�E
			start_type = TRMSG_FIGHT_START_2;
			after_type = TRMSG_FIGHT_AFTER_2;
			one_type = TRMSG_POKE_ONE_2;
		}
	}

	*wk1 = start_type;
	*wk2 = after_type;
	*wk3 = one_type;
	OS_Printf( "start_type = %d\n", *wk1 );
	OS_Printf( "after_type = %d\n", *wk2 );
	OS_Printf( "one_type = %d\n", *wk3 );
	return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �Đ�g���[�i�[��b�̎�ގ擾
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @return	"0"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdRevengeTrainerTalkTypeGet( VMHANDLE *core, void *wk )
{
	u16 btl_type,lr,start_type,after_type,one_type;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  u16 script_id   = SCRIPT_GetStartScriptID( sc );
	u16 *wk1				= SCRCMD_GetVMWork( core, work );
	u16 *wk2				= SCRCMD_GetVMWork( core, work );
	u16 *wk3				= SCRCMD_GetVMWork( core, work );
  
	//�X�N���v�gID����A�g���[�i�[ID���擾�A�_�u���o�g���^�C�v���擾
	btl_type = SCRIPT_CheckTrainer2vs2Type(
      SCRIPT_GetTrainerID_ByScriptID(script_id) );
  
	//�V���O�����_�u�����`�F�b�N
	if( btl_type == 0 ){
		//�V���O��
		start_type = TRMSG_REVENGE_FIGHT_START;
		after_type = 0;
		one_type   = 0;
	}else{
		//�_�u��
		lr = SCRIPT_GetTrainerLR_ByScriptID( script_id );

		if( lr == 0 ){
			//��
			start_type = TRMSG_REVENGE_FIGHT_START_1;
			after_type = 0;
			one_type   = TRMSG_POKE_ONE_1;
		}else{
			//�E
			start_type = TRMSG_REVENGE_FIGHT_START_2;
			after_type = 0;
			one_type   = TRMSG_POKE_ONE_2;
		}
	}

	*wk1 = start_type;
	*wk2 = after_type;
	*wk3 = one_type;
  OS_Printf( "start_type = %d\n", *wk1 );
	OS_Printf( "after_type = %d\n", *wk2 );
	OS_Printf( "one_type = %d\n", *wk3 );
	return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �g���[�i�[�^�C�v�擾
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @return	"0"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTrainerTypeGet( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  u16 script_id   = SCRIPT_GetStartScriptID( sc );
	u16 *ret_wk		= SCRCMD_GetVMWork( core, work );

	//�X�N���v�gID����A�g���[�i�[ID���擾�A�_�u���o�g���^�C�v���擾
	*ret_wk = SCRIPT_CheckTrainer2vs2Type(
      SCRIPT_GetTrainerID_ByScriptID(script_id) );
	return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �g���[�i�[BGM�Đ�
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @return	"1"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTrainerBgmSet( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*  work = wk;
  GAMESYS_WORK* gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA*    gdata = SCRCMD_WORK_GetGameData( work );
  SCRIPT_WORK*    sc = SCRCMD_WORK_GetScriptWork( work );
  u16          tr_id = SCRCMD_GetVMWorkValue(core,wk);
  u32           type = TT_TrainerDataParaGet( tr_id, ID_TD_tr_type );
  u32            seq = FSND_GetTrainerEyeBGM( type );
  GMEVENT*     event = EVENT_FSND_PushPlayNextBGM( 
                         gsys, seq, FSND_FADE_FAST, FSND_FADE_NONE );

  SCRIPT_CallEvent( sc, event );
	return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * �g���[�i�[����
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @return	VMCMD_RESULT_SUSPEND
 *
 * @todo
 * ����͂��ꂪ�Ă΂�Ȃ��Ă����퓮�삵�Ă���B
 * �������ɃX�N���v�g����OBJ����Ȃǂ��s���A���̌�t�F�[�h�C������B
 * ���̍ۂ̃t�F�[�h�C�������Ȃǂ����������\��B
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTrainerWin( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK * gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );

  GMEVENT* fade_event;
  fade_event = EVENT_FieldFadeIn_Black(gsys, fieldmap, FIELD_FADE_WAIT);
  SCRIPT_CallEvent( sc, fade_event );
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * �莝���`�F�b�N 2vs2���\���擾
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @return	"0"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmd2vs2BattleCheck( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys =  SCRCMD_WORK_GetGameSysWork( work );
	u16* ret_wk	= SCRCMD_GetVMWork( core, work );
  
  *ret_wk = FIELD_EVENT_Check2vs2Battle( gsys );
	return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �f�o�b�N�퓬�Ăяo��(������)
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @return	"1"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdDebugBattleSet( VMHANDLE *core, void *wk )
{
#if 0
	FIELDSYS_WORK * fsys = core->fsys;
	VMCMD_RESULT * win_flag			= SCRIPT_GetMemberWork( sc, ID_EVSCR_WIN_FLAG );

	//�C�x���g���؂�ւ���Ă��܂��̂ŉ��I�I�I
	//DebugFieldEncount( fsys );
	
	//�C�x���g�R�[���Ȃ̂ŁA�X�N���v�g�ɕ��A���܂��B
	//EventCmd_TrainerBattle( core->event_work, 0 );
	//EventCmd_TrainerBattle( core->event_work, 0, FIGHT_TYPE_TRAINER, HEAPID_WORLD ,win_flag );
	EventCmd_TrainerBattle( core->event_work, 1, 0, 0, HEAPID_WORLD ,win_flag );

	return 1;
#else //wb kari
  return VMCMD_RESULT_SUSPEND;
#endif
}


//--------------------------------------------------------------
/**
 * �퓬���ʂ��擾
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @return	"0"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBattleResultGet( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
	u16* ret_wk		= SCRCMD_GetVMWork( core, work );
  
	*ret_wk = 0;
	OS_Printf( "*ret_wk = %d\n", *ret_wk );
	return VMCMD_RESULT_CONTINUE;
}


//--------------------------------------------------------------
/**
 * �ϗ��ԃg���[�i�[��OBJID��Ԃ�
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @return	"0"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetWheelTrainerObjID( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
	u16  id	= SCRCMD_GetVMWorkValue( core, work );
	u16* ret_wk		= SCRCMD_GetVMWork( core, work );
 
  static const u16 obj_tbl[] = {
    GIRL2, DANCER,
    MOUNTMAN, TRAINERM,
    OL, WAITRESS,
    BABYGIRL2, BOY4,
  };
	*ret_wk = obj_tbl[id];
	return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �ϗ��ԃg���[�i�[�̃g���[�i�[ID��Ԃ�
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @return	"0"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetWheelTrainerTrID( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
	u16  id	= SCRCMD_GetVMWorkValue( core, work );
	u16* ret_wk		= SCRCMD_GetVMWork( core, work );
  
  static const u16 trid_tbl[] = {
    TRID_MINI_06, TRID_DANCER_03,
    TRID_MOUNT_13, TRID_ELITEM_14,
    TRID_OL_04, TRID_WAITRESS_04,
    TRID_KINDERGARTENW_05,TRID_PRINCE_05,
  };

	*ret_wk = trid_tbl[id];
	return VMCMD_RESULT_CONTINUE;
}
