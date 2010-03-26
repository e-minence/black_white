//============================================================================================
/**
 * @file	script_trainer.c
 * @brief	�X�N���v�g�F�g���[�i�[�֘A
 * @date  2010.03.15
 * @author  tamada
 */
//============================================================================================
#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "script.h"
#include "script_trainer.h"
//#include "script_local.h"

#include "scrcmd_work.h"
#include "eventwork_def.h"

#include "field/zonedata.h"

#include "print/wordset.h"    //WORDSET

#include "trainer_eye_data.h"   //EV_TRAINER_EYE_HITDATA
#include "field/eventdata_system.h" //EVENTDATA_GetSpecialScriptData

#include "fieldmap.h"   //FIELDMAP_GetFldMsgBG
#include "battle/battle.h"




//============================================================================================
//
//	�g���[�i�[�t���O�֘A
//	�E�X�N���v�gID����A�g���[�i�[ID���擾���āA�t���O�`�F�b�N
//
//============================================================================================
//ID_TRAINER_2VS2_OFFSET, ID_TRAINER_OFFSET
#include "../../../resource/fldmapdata/script/scrid_offset/scr_offset_id.h"

#define GET_TRAINER_FLAG(id)	( TR_FLAG_START+(id) )

//--------------------------------------------------------------
/**
 * �g���[�i�[���������i�[ ���O��SCRIPT_SetEventScript()���N�����Ă�����
 * @param	event SCRIPT_SetEventScript()�߂�l�B
 * @param	mmdl �������q�b�g����FIELD_OBJ_PTR
 * @param	range		�O���b�h�P�ʂ̎�������
 * @param	dir			�ړ�����
 * @param	scr_id		�����q�b�g�����X�N���v�gID
 * @param	tr_id		�����q�b�g�����g���[�i�[ID
 * @param	tr_type		�g���[�i�[�^�C�v�@�V���O���A�_�u���A�^�b�O����
 * @param	tr_no		���ԖڂɃq�b�g�����g���[�i�[�Ȃ̂�
 */
//--------------------------------------------------------------
void SCRIPT_SetTrainerEyeData( GMEVENT *event, MMDL *mmdl,
    s16 range, u16 dir, u16 scr_id, u16 tr_id, int tr_type, int tr_no )
{
  SCRIPT_WORK *sc =  SCRIPT_GetScriptWorkFromEvent( event );
	EV_TRAINER_EYE_HITDATA *eye = SCRIPT_GetTrainerEyeData( sc, tr_no );
  TRAINER_EYE_HITDATA_Set(eye, mmdl, range, dir, scr_id, tr_id, tr_type, tr_no );
}

//--------------------------------------------------------------
/**
 * �X�N���v�gID����A�g���[�i�[ID���擾
 *
 * @param   scr_id		�X�N���v�gID
 *
 * @retval  "�g���[�i�[ID = �t���O�C���f�b�N�X"
 */
//--------------------------------------------------------------
u16 SCRIPT_GetTrainerID_ByScriptID( u16 scr_id )
{
#if 0 //pl
	if( scr_id < ID_TRAINER_2VS2_OFFSET ){
		return (scr_id - ID_TRAINER_OFFSET + 1);		//1�I���W��
	}else{
		return (scr_id - ID_TRAINER_2VS2_OFFSET + 1);		//1�I���W��
	}
#else
	if( scr_id < ID_TRAINER_2VS2_OFFSET ){
		return (scr_id - ID_TRAINER_OFFSET);		//0�I���W��
	}else{
		return (scr_id - ID_TRAINER_2VS2_OFFSET);		//0�I���W��
	}
#endif
}

//--------------------------------------------------------------
/**
 * �g���[�i�[ID����A�X�N���v�gID���擾
 *
 * @param   tr_id		�g���[�i�[ID
 *
 * @retval  "�X�N���v�gID"
 */
//--------------------------------------------------------------
u16 SCRIPT_GetTrainerScriptID_ByTrainerID( u16 tr_id )
{
  return (tr_id + ID_TRAINER_OFFSET);		//0�I���W��
}

//--------------------------------------------------------------
/**
 * �g���[�i�[ID����A�_�u���o�g���p�̃X�N���v�gID���擾
 *
 * @param   tr_id		�g���[�i�[ID
 *
 * @retval  "�X�N���v�gID"
 */
//--------------------------------------------------------------
u16 SCRIPT_GetTrainerScriptID_By2vs2TrainerID( u16 tr_id )
{
  return (tr_id + ID_TRAINER_2VS2_OFFSET);		//0�I���W��
}

//--------------------------------------------------------------
/**
 * �X�N���v�gID����A���E�ǂ���̃g���[�i�[���擾
 * @param   scr_id		�X�N���v�gID
 * @retval  "0=���A1=�E"
 */
//--------------------------------------------------------------
BOOL SCRIPT_GetTrainerLR_ByScriptID( u16 scr_id )
{
	if( scr_id < ID_TRAINER_2VS2_OFFSET ){
		return 0;
	}else{
		return 1;
	}
}

//--------------------------------------------------------------
/**
 * �g���[�i�[ID����A�_�u���o�g���^�C�v���擾
 * @param   tr_id		�g���[�i�[ID
 * @retval  "0=�V���O���o�g���A1=�_�u���o�g��"
 */
//--------------------------------------------------------------
BOOL SCRIPT_CheckTrainer2vs2Type( u16 tr_id )
{
  u32 rule = TT_TrainerDataParaGet( tr_id, ID_TD_fight_type );
  
  //OS_Printf( "check 2v2 TRID=%d, type = %d\n", tr_id, rule );
  return (rule == BTL_RULE_DOUBLE );
}

//--------------------------------------------------------------
/**
 * �g���[�i�[ID����A�g���v��/���[�e�o�g���^�C�v���擾
 *
 * @param   tr_id		�g���[�i�[ID
 *
 * @retval  0 2vs2�ȉ�
 * @retval  1 3vs3
 */
//--------------------------------------------------------------
BOOL SCRIPT_CheckTrainer3vs3Type( u16 tr_id )
{
  u32 rule = TT_TrainerDataParaGet( tr_id, ID_TD_fight_type );
  
  switch( rule ){
  case BTL_RULE_TRIPLE:
  case BTL_RULE_ROTATION:
    return( 1 );
  }
  
  return( 0 );
}

//--------------------------------------------------------------
/**
 * �g���[�i�[ID����o�g�����[�����擾
 * @param   tr_id		�g���[�i�[ID
 * @retval  BtlRule�^ BTL_RULE_SINGLE�� 
 */
//--------------------------------------------------------------
u8 SCRIPT_GetTrainerBtlRule( u16 tr_id )
{
  return TT_TrainerDataParaGet( tr_id, ID_TD_fight_type );
}

//------------------------------------------------------------------
/**
 * @brief	�g���[�i�[�C�x���g�t���O���`�F�b�N����
 * @param	fsys		FLDCOMMON_WORK�̃|�C���^
 * @param	tr_id		�g���[�i�[ID
 * @retval	"1 = �t���OON"
 * @retval	"0 = �t���OOFF"
 */
//------------------------------------------------------------------
BOOL SCRIPT_CheckEventFlagTrainer( EVENTWORK *ev, u16 tr_id )
{
  return EVENTWORK_CheckEventFlag(ev,GET_TRAINER_FLAG(tr_id) );
}

//------------------------------------------------------------------
/**
 * @brief	�g���[�i�[�C�x���g�t���O���Z�b�g����
 *
 * @param	fsys		FLDCOMMON_WORK�̃|�C���^
 * @param	tr_id		�g���[�i�[ID
 *
 * @return	none
 */
//------------------------------------------------------------------
void SCRIPT_SetEventFlagTrainer( EVENTWORK *ev, u16 tr_id )
{
  EVENTWORK_SetEventFlag( ev, GET_TRAINER_FLAG(tr_id) );
}

//------------------------------------------------------------------
/**
 * @brief	�g���[�i�[�C�x���g�t���O�����Z�b�g����
 *
 * @param	fsys		FLDCOMMON_WORK�̃|�C���^
 * @param	tr_id		�g���[�i�[ID
 *
 * @return	none
 */
//------------------------------------------------------------------
void SCRIPT_ResetEventFlagTrainer( EVENTWORK *ev, u16 tr_id )
{
	EVENTWORK_ResetEventFlag( ev, GET_TRAINER_FLAG(tr_id) );
}


