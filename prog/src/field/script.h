//======================================================================
/**
 * @file	script.h
 * @bfief	�X�N���v�g���䃁�C������
 * @author	Satoshi Nohara
 * @date	05.08.04
 *
 * 01.03.28	Sousuke Tamada
 * 03.04.15	Satoshi Nohara
 * 05.04.25 Hiroyuki Nakamura
 */
//======================================================================
#ifndef SCRIPT_H
#define SCRIPT_H

#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "eventwork.h"


#include "field/fieldmap_proc.h"
#include "field/field_msgbg.h"

#if 0
#include "field_common.h"
#include "field_event.h"
#include "vm.h"					//VM_CMD
#include "script_def.h"			//����X�N���v�g��`
#endif

//======================================================================
//	define
//======================================================================
//--------------------------------------------------------------
//	debug
//--------------------------------------------------------------
#define SCRIPT_PL_NULL	//PL�ӏ�������
#define DEBUG_SCRIPT		//��`�Ńf�o�b�O�@�\�L��

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
	ID_EVSCR_WK_GAMESYS_WORK,
	//HEAPID
	ID_EVSCR_WK_HEAPID,
	//SCRIPT_FLDPARAM
	ID_EVSCR_WK_FLDPARAM,
	//GMEVENT
  ID_EVSCR_WK_GMEVENT,
  //TEMP HEAPID
  ID_EVSCR_WK_TEMP_HEAPID,


	ID_EVSCR_WK_END,				//���[�N�̏I��ID

	//���������������̓��[�N�̐��ɉe������̂Œ��ӁI����������
};


//���z�}�V���̃i���o�[
typedef enum{
	VMHANDLE_MAIN = 0,		//���C��
	VMHANDLE_SUB1,			//�T�u
	VMHANDLE_MAX,			//�ő吔
}VMHANDLE_ID;

//========================================================================
//	struct
//========================================================================
//--------------------------------------------------------------
//	�X�N���v�g���䃏�[�N�ւ̕s���S�^�|�C���^
//--------------------------------------------------------------
typedef	struct _TAG_SCRIPT_WORK SCRIPT_WORK;

//--------------------------------------------------------------
///	SCRIPT_FLDPARAM �X�N���v�g���䃏�[�N�@�t�B�[���h�p�����[�^
//--------------------------------------------------------------
typedef struct
{
	FLDMSGBG *msgBG;
  FIELDMAP_WORK *fieldMap;
}SCRIPT_FLDPARAM;

//======================================================================
//	proto
//======================================================================
//--------------------------------------------------------------
/**
 * �X�N���v�g�C�x���g�Z�b�g
 * @param	scr_id		�X�N���v�gID
 * @param	obj			�b�������Ώ�OBJ�̃|�C���^(�Ȃ�����NULL)
 * @retval	none
 */
//--------------------------------------------------------------
extern GMEVENT * SCRIPT_SetEventScript(
		GAMESYS_WORK *gsys, u16 scr_id, MMDL *obj,
		HEAPID temp_heapID, const SCRIPT_FLDPARAM *fparam );

extern void SCRIPT_SetTrainerEyeData( GMEVENT *event, MMDL *mmdl,
    s16 range, u16 dir, u16 scr_id, u16 tr_id, int tr_type, int tr_no );

//--------------------------------------------------------------
/**
 * �X�N���v�g�C�x���g�R�[��
 * @param	event		GMEVENT_CONTROL�^�̃|�C���^
 * @param	scr_id		�X�N���v�gID
 * @param	obj			�b�������Ώ�OBJ�̃|�C���^(�Ȃ�����NULL)
 * @param	ret_script_wk	�X�N���v�g���ʂ������郏�[�N�̃|�C���^
 * @retval	none
 */
//--------------------------------------------------------------
extern void SCRIPT_CallScript( GMEVENT *event,
	u16 scr_id, MMDL *obj, void *ret_script_wk, HEAPID heapID );

//--------------------------------------------------------------
/**
 * ����X�N���v�g�Ăяo��
 * @param gsys
 * @param heapID
 * @param script_id
 */
//--------------------------------------------------------------
extern void SCRIPT_CallSpecialScript( GAMESYS_WORK *gsys, HEAPID heapID, u16 script_id );

//--------------------------------------------------------------
/**
 * �X�N���v�g�C�x���g�ؑ�
 * @param	event		GMEVENT_CONTROL�^�̃|�C���^
 * @param	scr_id		�X�N���v�gID
 * @param	obj			�b�������Ώ�OBJ�̃|�C���^(�Ȃ�����NULL)
 * @retval	none
 * ���̃C�x���g����X�N���v�g�ւ̐ؑւ��s��
 */
//--------------------------------------------------------------
extern void SCRIPT_ChangeScript( GMEVENT *event,
		u16 scr_id, MMDL *obj, HEAPID heapID );

extern void SCRIPT_AddVMachine( SCRIPT_WORK *sc, u16 zone_id, u16 scr_id, VMHANDLE_ID vm_id );
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

//--------------------------------------------------------------
/**
 * �X�N���v�g���䃏�[�N��"next_func"�Ƀ��j���[�Ăяo�����Z�b�g
 * @return	none
 * �t�B�[���h���j���[�Ăяo������I
 * ���ꂪ�c�������炭�Ȃ�̂ŁA�ėp�I�Ɏg���Ȃ��悤�ɂ��Ă���I
 */
//--------------------------------------------------------------
#ifndef SCRIPT_PL_NULL
extern void SCRIPT_SetNextScript( FLDCOMMON_WORK* fsys );
#endif

extern GMEVENT * SCRIPT_GetEvent( SCRIPT_WORK * sc );
extern GAMESYS_WORK * SCRIPT_GetGameSysWork( SCRIPT_WORK * sc );

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

//------------------------------------------------------------------
/**
 * @brief	�X�N���v�g����w�肷��OBJ�R�[�h���擾
 *
 * @param	no			0-15
 *
 * @return	"OBJ�L�����R�[�h"
 */
//------------------------------------------------------------------
extern u16 GetEvDefineObjCode( SCRIPT_WORK * sc, u16 no );

//------------------------------------------------------------------
/**
 * @brief	�X�N���v�g����w�肷��OBJ�R�[�h���Z�b�g
 *
 * @param	no			0-15
 * @param	obj_code	OBJ�R�[�h
 *
 * @return	"TRUE=�Z�b�g�o�����AFALSE=�Z�b�g�o���Ȃ�����"
 */
//------------------------------------------------------------------
extern BOOL SetEvDefineObjCode( SCRIPT_WORK * sc, u16 no, u16 obj_code );


//======================================================================
//
//	�t���O�֘A
//
//======================================================================
//------------------------------------------------------------------
/**
 * @brief	�}�b�v������̃Z�[�u�t���O���N���A����
 *
 *
 * @return	none
 */
//------------------------------------------------------------------
extern void LocalEventFlagClear( EVENTWORK * eventwork );

//------------------------------------------------------------------
/**
 * @brief	1���o�߂��ƂɃN���A�����t���O���N���A����
 *
 *
 * @return	none
 */
//------------------------------------------------------------------
extern void TimeEventFlagClear( EVENTWORK * eventwork );


//======================================================================
//
//	�g���[�i�[�t���O�֘A
//
//	�E�X�N���v�gID����A�g���[�i�[ID���擾���āA�t���O�`�F�b�N
//	BOOL CheckEventFlagTrainer( fsys, GetTrainerIdByScriptId(scr_id) );
//
//======================================================================

//--------------------------------------------------------------
/**
 * �X�N���v�gID����A�g���[�i�[ID���擾
 *
 * @param   scr_id		�X�N���v�gID
 *
 * @retval  "�g���[�i�[ID = �t���O�C���f�b�N�X"
 */
//--------------------------------------------------------------
extern u16 SCRIPT_GetTrainerID_ByScriptID( u16 scr_id );

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
 *
 * @param   tr_id		�g���[�i�[ID
 *
 * @retval  "0=�V���O���o�g���A1=�_�u���o�g��"
 */
//--------------------------------------------------------------
extern BOOL SCRIPT_CheckTrainer2vs2Type( u16 tr_id );

//------------------------------------------------------------------
/**
 * @brief	�g���[�i�[�C�x���g�t���O���`�F�b�N����
 *
 * @param	tr_id		�g���[�i�[ID
 *
 * @retval	"1 = �t���OON"
 * @retval	"0 = �t���OOFF"
 */
//------------------------------------------------------------------
extern BOOL SCRIPT_CheckEventFlagTrainer( EVENTWORK *ev, u16 tr_id );

//------------------------------------------------------------------
/**
 * @brief	�g���[�i�[�C�x���g�t���O���Z�b�g����
 *
 * @param	tr_id		�g���[�i�[ID
 *
 * @return	none
 */
//------------------------------------------------------------------
extern void SCRIPT_SetEventFlagTrainer( EVENTWORK *ev, u16 tr_id );

//------------------------------------------------------------------
/**
 * @brief	�g���[�i�[�C�x���g�t���O�����Z�b�g����
 *
 * @param	tr_id		�g���[�i�[ID
 *
 * @return	none
 */
//------------------------------------------------------------------
extern void SCRIPT_ResetEventFlagTrainer( EVENTWORK *ev, u16 tr_id );

//------------------------------------------------------------------
/**
 * @brief	�Q�[���J�n �X�N���v�g�����ݒ�̎��s
 */
//------------------------------------------------------------------
extern void SCRIPT_CallGameStartInitScript( GAMESYS_WORK *gsys, HEAPID heapID );

//------------------------------------------------------------------
/**
 * @brief	����X�N���v�g�������Ď��s
 *
 * @param	key			����X�N���v�gID
 *
 * @return	"TRUE=����X�N���v�g���s�AFALSE=�������Ȃ�"
 */
//------------------------------------------------------------------
extern GMEVENT * SCRIPT_SearchSceneScript( GAMESYS_WORK * gsys, HEAPID heapID);
extern BOOL SCRIPT_CallFieldInitScript( GAMESYS_WORK * gsys, HEAPID heapID );
extern BOOL SCRIPT_CallFieldRecoverScript( GAMESYS_WORK * gsys, HEAPID heapID );




#endif	/* SCRIPT_H */


