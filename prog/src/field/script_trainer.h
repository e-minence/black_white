//======================================================================
/**
 * @file	script_trainer.h
 * @brief	�X�N���v�g�E�g���[�i�[�֘A
 * @author	tamada GAMEFREAK inc.
 * @date	2010.03.15
 *
 */
//======================================================================
#pragma once

#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "field/eventwork.h"

#include "trainer_eye_data.h"

//--------------------------------------------------------------
/**
 * @brief �X�N���v�gID --> �g���[�i�[ID
 * @param   scr_id		�X�N���v�gID
 * @retval  "�g���[�i�[ID = �t���O�C���f�b�N�X"
 */
//--------------------------------------------------------------
extern u16 SCRIPT_GetTrainerID_ByScriptID( u16 scr_id );

//--------------------------------------------------------------
/**
 * @brief �g���[�i�[ID --> �X�N���v�gID
 * @param   tr_id		�g���[�i�[ID
 * @retval  "�X�N���v�gID"
 */
//--------------------------------------------------------------
extern u16 SCRIPT_GetTrainerScriptID_ByTrainerID( u16 tr_id );

//--------------------------------------------------------------
/**
 * @brief �g���[�i�[ID --> �_�u���o�g���i�T�u�j�̃X�N���v�gID
 * @param   tr_id		�g���[�i�[ID
 * @retval  "�X�N���v�gID"
 */
//--------------------------------------------------------------
extern u16 SCRIPT_GetTrainerScriptID_By2vs2TrainerID( u16 tr_id );

//--------------------------------------------------------------
/**
 * �X�N���v�gID����A���E�ǂ���̃g���[�i�[���擾
 *
 * @param   scr_id		�X�N���v�gID
 *
 * @retval  "0=���A1=�E"
 */
//--------------------------------------------------------------
extern BOOL SCRIPT_GetTrainerLR_ByScriptID( u16 scr_id );

//--------------------------------------------------------------
/**
 * �g���[�i�[ID����A�_�u���o�g���^�C�v���擾
 * @param   tr_id		�g���[�i�[ID
 * @retval  "0=�V���O���o�g���A1=�_�u���o�g��"
 */
//--------------------------------------------------------------
extern BOOL SCRIPT_CheckTrainer2vs2Type( u16 tr_id );

//--------------------------------------------------------------
/**
 * �g���[�i�[ID����A�g���v��/���[�e�o�g���^�C�v���擾
 * @param   tr_id		�g���[�i�[ID
 * @retval  0 2vs2�ȉ�
 * @retval  1 3vs3
 */
//--------------------------------------------------------------
extern BOOL SCRIPT_CheckTrainer3vs3Type( u16 tr_id );

//--------------------------------------------------------------
/**
 * �g���[�i�[ID����o�g�����[�����擾
 * @param   tr_id		�g���[�i�[ID
 * @retval  BtlRule�^ BTL_RULE_SINGLE�� 
 */
//--------------------------------------------------------------
extern u8 SCRIPT_GetTrainerBtlRule( u16 tr_id );

//------------------------------------------------------------------
/**
 * @brief	�g���[�i�[�C�x���g�t���O���`�F�b�N����
 * @param	tr_id		�g���[�i�[ID
 * @retval	"1 = �t���OON"
 * @retval	"0 = �t���OOFF"
 */
//------------------------------------------------------------------
extern BOOL SCRIPT_CheckEventFlagTrainer( EVENTWORK *ev, u16 tr_id );

//------------------------------------------------------------------
/**
 * @brief	�g���[�i�[�C�x���g�t���O���Z�b�g����
 * @param	tr_id		�g���[�i�[ID
 * @return	none
 */
//------------------------------------------------------------------
extern void SCRIPT_SetEventFlagTrainer( EVENTWORK *ev, u16 tr_id );

//------------------------------------------------------------------
/**
 * @brief	�g���[�i�[�C�x���g�t���O�����Z�b�g����
 * @param	tr_id		�g���[�i�[ID
 * @return	none
 */
//------------------------------------------------------------------
extern void SCRIPT_ResetEventFlagTrainer( EVENTWORK *ev, u16 tr_id );


//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
extern void SCRIPT_TRAINER_SetHitData(
    GMEVENT * event, u32 tr_no, const TRAINER_HITDATA * hitdata );

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
extern SCR_TRAINER_HITDATA * SCRIPT_GetTrainerHitData( SCRIPT_WORK * sc, u32 tr_no );


