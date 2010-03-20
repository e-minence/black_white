//======================================================================
/**
 * @file	script_local.h
 * @brief	�X�N���v�g����F
 * @date  2009.09.13
 * @author  tamada GAMEFREAK inc.
 *
 * script.h����A�X�N���v�g�V�X�e����R�}���h�ȊO�Ɍ��J����
 * �K�R�����Ȃ����̂��ړ�����
 */
//======================================================================
#pragma once

#include "field/fieldmap_proc.h"
#include "field/field_msgbg.h"


#include "scrend_check_bit.h"
#include "scrcmd_work.h"      //for SCRCMD_WORK

#include "field_saveanime.h"      //for FIELD_SAVEANIME
#include "event_movepoke.h"      //for EV_MOVEPOKE_WORK

//--------------------------------------------------------------
///	SCRIPT_FLDPARAM �X�N���v�g���䃏�[�N�@�t�B�[���h�p�����[�^
//--------------------------------------------------------------
typedef struct
{
	FLDMSGBG *msgBG;
  FIELDMAP_WORK *fieldMap;
}SCRIPT_FLDPARAM;

typedef struct
{
  GAMESYS_WORK *gsys;
  SCRCMD_WORK   *ScrCmdWk;
  SCRIPT_WORK *ScrWk;
}SCREND_CHECK;

//--------------------------------------------------------------
// �g���[�i�[�f�[�^�Ǘ��p
//--------------------------------------------------------------
enum{
  TRAINER_EYE_HIT0 = 0,
  TRAINER_EYE_HIT1,

  TRAINER_EYE_HITMAX,  ///<�g���[�i�[�����f�[�^�ő吔
};


//���z�}�V���̃i���o�[
typedef enum{
	VMHANDLE_MAIN = 0,		//���C��
	VMHANDLE_SUB1,			//�T�u
	VMHANDLE_MAX,			//�ő吔
}VMHANDLE_ID;

//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * @brief �X�N���v�g�p�O���[�o�����[�N�̐���
 * @param main_heapID
 * @param gsys
 * @param event
 * @param	scr_id		�X�N���v�gID
 * @param	ret_wk		�X�N���v�g���ʂ������郏�[�N�̃|�C���^
 * @return	SCRIPT_WORK			SCRIPT�^�̃|�C���^
 *
 * �X�N���v�g�R�}���h�S�ʂ���O���[�o���A�N�Z�X�\�ȃf�[�^��ێ�����
 */
//--------------------------------------------------------------
extern SCRIPT_WORK * SCRIPTWORK_Create( HEAPID main_heapID,
    GAMESYS_WORK * gsys, GMEVENT * event, u16 scr_id, void* ret_wk);
extern void SCRIPTWORK_Delete( SCRIPT_WORK * sc );

//======================================================================
//
//
//  �X�N���v�g��������E�R�}���h����Ă΂��֐�
//
//
//======================================================================
//--------------------------------------------------------------
/**
 * @brief �X�N���v�g�V�X�e���F���̃C�x���g�Ăяo��
 * @param sc
 * @param call_event  �Ăяo���C�x���g�ւ̃|�C���^
 *
 * �X�N���v�g�R�}���h����g�p����B
 * �w�肵���C�x���g���I��������́A���݂̃X�N���v�g�ɕ��A�����s�����B
 *
 */
//--------------------------------------------------------------
extern void SCRIPT_CallEvent( SCRIPT_WORK *sc, GMEVENT * call_event );

//--------------------------------------------------------------
/**
 * @brief �X�N���v�g�V�X�e���F�X�N���v�g�I�������̃C�x���g�֐؂�ւ�
 * @param sc
 * @param next_event  �X�N���v�g�I����ɉғ�����C�x���g�ւ̃|�C���^
 *
 * �X�N���v�g�R�}���h����g�p����B
 * �X�N���v�g���I��������A�w�肵���C�x���g�ɐ؂�ւ��B
 */
//--------------------------------------------------------------
extern void SCRIPT_EntryNextEvent( SCRIPT_WORK *sc, GMEVENT * next_event );

//--------------------------------------------------------------
/**
 * @brief �X�N���v�g�V�X�e���F���z�}�V���̒ǉ�
 * @param sc
 * @param zone_id �N������]�[���w��ID
 * @param scr_id  �N������X�N���v�g�w��ID
 * @param vm_id   ���z�}�V���w��ID
 *
 *  ��ɃX�N���v�g�R�}���h����g�p����B
 *  �X�N���v�g�����s���鉼�z�}�V���𐶐�����B
 */
//--------------------------------------------------------------
extern void SCRIPT_AddVMachine( SCRIPT_WORK *sc, u16 zone_id, u16 scr_id, VMHANDLE_ID vm_id );

//--------------------------------------------------------------
/**
 * @brief �X�N���v�g�V�X�e���F���z�}�V���̑��݃`�F�b�N
 * @param sc
 * @param vm_id   ���z�}�V���w��ID
 * @retval  TRUE    �w�肵��ID�̉��z�}�V�������݂���
 * @retval  FALSE   �w�肵��ID�̉��z�}�V���͑��݂��Ȃ�
 */
//--------------------------------------------------------------
extern BOOL SCRIPT_GetVMExists( SCRIPT_WORK *sc, VMHANDLE_ID vm_id );

//======================================================================
//	�X�N���v�g���䃏�[�N�̃����o�[�A�N�Z�X
//======================================================================
extern GMEVENT * SCRIPT_GetEvent( SCRIPT_WORK * sc );
extern GAMESYS_WORK * SCRIPT_GetGameSysWork( SCRIPT_WORK * sc );
extern SCRIPT_FLDPARAM * SCRIPT_GetFieldParam( SCRIPT_WORK * sc );
extern void * SCRIPT_GetMsgWinPointer( SCRIPT_WORK *sc );
extern void SCRIPT_SetMsgWinPointer( SCRIPT_WORK *sc, void* msgWin );
extern void * SCRIPT_GetTimeIconPointer( SCRIPT_WORK *sc );
extern void SCRIPT_SetTimeIconPointer( SCRIPT_WORK *sc, void* timeIcon );
extern WORDSET * SCRIPT_GetWordSet( SCRIPT_WORK * sc );
extern STRBUF * SCRIPT_GetMsgBuffer( SCRIPT_WORK * sc );
extern STRBUF * SCRIPT_GetMsgTempBuffer( SCRIPT_WORK * sc );

extern u8 * SCRIPT_GetSoundSeFlag( SCRIPT_WORK * sc );
extern u8 * SCRIPT_GetAnimeCount( SCRIPT_WORK * sc );
extern void SCRIPT_SetFLDMENUFUNC( SCRIPT_WORK * sc, void * mw );
extern void * SCRIPT_GetFLDMENUFUNC( SCRIPT_WORK * sc );
extern MMDL * SCRIPT_GetTargetObj( SCRIPT_WORK * sc );
extern void SCRIPT_SetTargetObj( SCRIPT_WORK * sc, MMDL * obj );

//======================================================================
//	
//======================================================================
//------------------------------------------------------------------
/**
 * @brief	�C�x���g���[�N�A�h���X���擾
 *
 * @param	work_no		���[�N�i���o�[
 *
 * @return	"���[�N�̃A�h���X"
 *
 * @li	work_no < 0x8000	�ʏ�̃Z�[�u���[�N
 * @li	work_no >= 0x8000	�X�N���v�g���䃏�[�N�̒��Ɋm�ۂ��Ă��郏�[�N
 */
//------------------------------------------------------------------
extern u16 * SCRIPT_GetEventWork( SCRIPT_WORK *sc, GAMEDATA *gdata, u16 work_no );

//------------------------------------------------------------------
/**
 * @brief	�C�x���g���[�N�̒l���擾
 *
 * @param	work_no		���[�N�i���o�[
 *
 * @return	"���[�N�̒l"
 */
//------------------------------------------------------------------
extern u16 SCRIPT_GetEventWorkValue( SCRIPT_WORK *sc, GAMEDATA *gdata, u16 work_no );

//------------------------------------------------------------------
/**
 * @brief	�C�x���g���[�N�̒l���Z�b�g
 *
 * @param	work_no		���[�N�i���o�[
 * @param	value		�Z�b�g����l
 *
 * @return	"TRUE=�Z�b�g�o�����AFALSE=�Z�b�g�o���Ȃ�����"
 */
//------------------------------------------------------------------
extern BOOL SCRIPT_SetEventWorkValue(
		SCRIPT_WORK* sc, u16 work_no, u16 value );

//--------------------------------------------------------------
/**
 * @brief ���z�}�V�����[�J���ȕϐ��̈�̃o�b�N�A�b�v
 */
//--------------------------------------------------------------
extern void * SCRIPT_BackupUserWork( SCRIPT_WORK * sc );

//--------------------------------------------------------------
/**
 * @brief ���z�}�V�����[�J���ȕϐ��̈�̕��A
 */
//--------------------------------------------------------------
extern void SCRIPT_RestoreUserWork( SCRIPT_WORK * sc, void *backup_work );

//--------------------------------------------------------------
/**
 * @brief   �J�n���̃X�N���v�gID�擾
 * @param	sc		    SCRIPT_WORK�̃|�C���^
 * @return  u16   �X�N���v�gID
 */
//--------------------------------------------------------------
extern u16 SCRIPT_GetStartScriptID( const SCRIPT_WORK * sc );



//----------------------------------------------------------------------------
/**
 *	@brief  �Z�[�uBG�A�j���[�V�������[�N�̊Ǘ�
 */
//-----------------------------------------------------------------------------
extern void SCRIPT_SetSaveAnimeWork( SCRIPT_WORK * sc, FIELD_SAVEANIME * wk );
extern FIELD_SAVEANIME * SCRIPT_GetSaveAnimeWork( SCRIPT_WORK * sc ); 

//----------------------------------------------------------------------------
/**
 *	@brief  �ړ��|�P�����A�j���[�V�������[�N�̊Ǘ�
 */
//-----------------------------------------------------------------------------
extern void SCRIPT_SetMovePokeWork( SCRIPT_WORK * sc, EV_MOVEPOKE_WORK * wk );
extern EV_MOVEPOKE_WORK * SCRIPT_GetMovePokeWork( SCRIPT_WORK * sc ); 


//----------------------------------------------------------------------------
/**
 *	@brief  �Đ�g���[�i�[���[�N�̊Ǘ�
 */
//-----------------------------------------------------------------------------
extern void SCRIPT_SetReBattleTrainerData( SCRIPT_WORK * sc, void * wk );
extern void * SCRIPT_GetReBattleTrainerData( SCRIPT_WORK * sc ); 


