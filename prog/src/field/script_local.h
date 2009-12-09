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
// SCRIPT�̃����o�[ID��`
//--------------------------------------------------------------
enum{
	ID_EVSCR_EVWIN = 0,				//�C�x���g�E�B���h�E���[�N�̃|�C���^

	ID_EVSCR_MSGWINDAT,				//��b�E�B���h�E�r�b�g�}�b�v�f�[�^
	ID_EVSCR_MENUWORK,				//�r�b�g�}�b�v���j���[���[�N�̃|�C���^

	ID_EVSCR_MSGINDEX,				//��b�E�B���h�E���b�Z�[�W�C���f�b�N�X
	ID_EVSCR_ANMCOUNT,				//�A�j���[�V�����̐�
	ID_EVSCR_WIN_OPEN_FLAG,			//��b�E�B���h�E���J�������t���O

	ID_EVSCR_SCRIPT_ID,				//���C���̃X�N���v�gID

	//ID_EVSCR_PLAYER_DIR,			//�C�x���g�N�����̎�l���̌���
	ID_EVSCR_TARGET_OBJ,			//�b�������Ώۂ�OBJ�̃|�C���^
	ID_EVSCR_DUMMY_OBJ,				//�����_�~�[��OBJ�̃|�C���^
	ID_EVSCR_RETURN_SCRIPT_WK,		//�C�x���g���ʂ������郏�[�N�̃|�C���^

	ID_EVSCR_WORDSET,				//�P��Z�b�g
	ID_EVSCR_MSGBUF,				//���b�Z�[�W�o�b�t�@�̃|�C���^
	ID_EVSCR_TMPBUF,				//�e���|�����o�b�t�@�̃|�C���^
	//ID_EVSCR_WAITICON,				///<�ҋ@�A�C�R���̃|�C���^

	ID_EVSCR_SUBPROC_WORK,			//�T�u�v���Z�X�̃p�����[�^
	ID_EVSCR_PWORK,					//���[�N�ւ̔ėp�|�C���^
	//ID_EVSCR_EOA,					//�t�B�[���h�G�t�F�N�g�ւ̃|�C���^
	//ID_EVSCR_PLAYER_TCB,			//���@�`��TCB�̃|�C���^

	ID_EVSCR_WIN_FLAG,

  ID_EVSCR_TRAINER0,

  ID_EVSCR_TRAINER1,

	//�R�C���E�C���h�E
	//ID_EVSCR_COINWINDAT,

	//�����E�C���h�E
	//ID_EVSCR_GOLDWINDAT,

	//ID_EVSCR_REPORTWIN,		///<���|�[�g���E�B���h�E

	/*WB*/
	//GAMESYS_WORK
	//ID_EVSCR_WK_GAMESYS_WORK,
	//HEAPID
	ID_EVSCR_WK_HEAPID,
	//SCRIPT_FLDPARAM
	//ID_EVSCR_WK_FLDPARAM,
	//GMEVENT
  //ID_EVSCR_WK_GMEVENT,
  //TEMP HEAPID
  ID_EVSCR_WK_TEMP_HEAPID,

  ID_EVSCR_MUSICAL_EVENT_WORK,  //�~���[�W�J�����[�N(�T�����̂ݗL��

	ID_EVSCR_WK_END,				//���[�N�̏I��ID

	//���������������̓��[�N�̐��ɉe������̂Œ��ӁI����������
};


//���z�}�V���̃i���o�[
typedef enum{
	VMHANDLE_MAIN = 0,		//���C��
	VMHANDLE_SUB1,			//�T�u
	VMHANDLE_MAX,			//�ő吔
}VMHANDLE_ID;

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
//--------------------------------------------------------------
/**
 * �X�N���v�g���䃏�[�N�̃����o�[�A�h���X�擾
 * @param	id		�擾���郁���oID(script.h�Q��)
 * @return	"�A�h���X"
 */
//--------------------------------------------------------------
extern void * SCRIPT_GetMemberWork( SCRIPT_WORK *sc, u32 id );


extern GMEVENT * SCRIPT_GetEvent( SCRIPT_WORK * sc );
extern GAMESYS_WORK * SCRIPT_GetGameSysWork( SCRIPT_WORK * sc );
extern SCRIPT_FLDPARAM * SCRIPT_GetFieldParam( SCRIPT_WORK * sc );
extern void * SCRIPT_GetMsgWinPointer( SCRIPT_WORK *sc );
extern void SCRIPT_SetMsgWinPointer( SCRIPT_WORK *sc, void* msgWin );

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
 * �X�N���v�g���䃏�[�N�̃����o�[�A�h���X�ݒ�(�~���[�W�J��
 * @param	sc	  SCRIPT_WORK�^�̃|�C���^
 */
//--------------------------------------------------------------
extern void SCRIPT_SetMemberWork_Musical( SCRIPT_WORK *sc, void *musEveWork );
