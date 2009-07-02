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
#include "system/vm.h"
#include "eventwork.h"

#include "script_def.h"

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
	ID_EVSCR_COMMON_SCR_FLAG,		//���ʃX�N���v�g�؂�ւ��t���O
	ID_EVSCR_WIN_OPEN_FLAG,			//��b�E�B���h�E���J�������t���O
	ID_EVSCR_VMHANDLE_COUNT,		//�ǉ��������z�}�V���̐�

	ID_EVSCR_SCRIPT_ID,				//���C���̃X�N���v�gID

	ID_EVSCR_PLAYER_DIR,			//�C�x���g�N�����̎�l���̌���
	ID_EVSCR_TARGET_OBJ,			//�b�������Ώۂ�OBJ�̃|�C���^
	ID_EVSCR_DUMMY_OBJ,				//�����_�~�[��OBJ�̃|�C���^
	ID_EVSCR_RETURN_SCRIPT_WK,		//�C�x���g���ʂ������郏�[�N�̃|�C���^

	ID_EVSCR_VM_MAIN,				//VM�̃|�C���^
	ID_EVSCR_VM_SUB1,				//VM�̃|�C���^

	ID_EVSCR_WORDSET,				//�P��Z�b�g
	ID_EVSCR_MSGBUF,				//���b�Z�[�W�o�b�t�@�̃|�C���^
	ID_EVSCR_TMPBUF,				//�e���|�����o�b�t�@�̃|�C���^
	ID_EVSCR_WAITICON,				///<�ҋ@�A�C�R���̃|�C���^

	ID_EVSCR_SUBPROC_WORK,			//�T�u�v���Z�X�̃p�����[�^
	ID_EVSCR_PWORK,					//���[�N�ւ̔ėp�|�C���^
	ID_EVSCR_EOA,					//�t�B�[���h�G�t�F�N�g�ւ̃|�C���^
	ID_EVSCR_PLAYER_TCB,			//���@�`��TCB�̃|�C���^

	ID_EVSCR_WIN_FLAG,

	ID_EVSCR_TR0_RANGE,				//����
	ID_EVSCR_TR0_DIR,
	ID_EVSCR_TR0_SCR_ID,
	ID_EVSCR_TR0_ID,
	ID_EVSCR_TR0_TYPE,
	ID_EVSCR_TR0_FLDOBJ,
	ID_EVSCR_TR0_TCB,

	ID_EVSCR_TR1_RANGE,
	ID_EVSCR_TR1_DIR,
	ID_EVSCR_TR1_SCR_ID,
	ID_EVSCR_TR1_ID,
	ID_EVSCR_TR1_TYPE,
	ID_EVSCR_TR1_FLDOBJ,
	ID_EVSCR_TR1_TCB,

	//�R�C���E�C���h�E
	ID_EVSCR_COINWINDAT,

	//�����E�C���h�E
	ID_EVSCR_GOLDWINDAT,

	ID_EVSCR_REPORTWIN,		///<���|�[�g���E�B���h�E

	//�X�N���v�g�ƃv���O�����̃f�[�^�����p
	ID_EVSCR_WK_START,				//���[�N�̊J�nID
	ID_EVSCR_WK_PARAM0 = ID_EVSCR_WK_START,				
	ID_EVSCR_WK_PARAM1,
	ID_EVSCR_WK_PARAM2,
	ID_EVSCR_WK_PARAM3,

	//�X�N���v�g�ł̃e���|����
	ID_EVSCR_WK_TEMP0,					
	ID_EVSCR_WK_TEMP1,
	ID_EVSCR_WK_TEMP2,
	ID_EVSCR_WK_TEMP3,

	//�X�N���v�g�����ł̏����p
	ID_EVSCR_WK_REG0,					
	ID_EVSCR_WK_REG1,
	ID_EVSCR_WK_REG2,
	ID_EVSCR_WK_REG3,

	//�X�N���v�g�ɓ�����Ԃ��ėp���[�N
	ID_EVSCR_WK_ANSWER,

	//�b�������Ώۂ�OBJID(�ύX�s�I)
	ID_EVSCR_WK_TARGET_OBJID,

	//�����̓��[�N�̐��ɉe������̂Œ��ӁI

	/*
	//�莝���������ς��Ńp�\�R���ɓ]�����ꂽ��
	u16 BoxNoWork = 0;			//���Ԃ�BOX��
	u16 PasoNoWork = 0;			//����BOX�̉��Ԗڂ�

	//���j�I�����[���ł̃C�x���g��Ԑ���p���[�N
	u16 UnionRoomWork = 0;
	*/
	
	/*WB*/
	//GAMESYS_WORK
	ID_EVSCR_WK_GAMESYS_WORK,
	//HEAPID
	ID_EVSCR_WK_HEAPID,
	//SCRIPT_FLDPARAM
	ID_EVSCR_WK_FLDPARAM,
	
	ID_EVSCR_WK_END,				//���[�N�̏I��ID
};

#define EVSCR_WORK_MAX	(ID_EVSCR_WK_END-ID_EVSCR_WK_START)	//���[�N�̐�

//���z�}�V���̃i���o�[
enum{
	VMHANDLE_MAIN = 0,		//���C��
	VMHANDLE_SUB1,			//�T�u
	VMHANDLE_MAX,			//�ő吔
};

#define HIDE_LIST_SX				(7)		//�����͈�
#define HIDE_LIST_SZ				(7)		//�����͈�(���g�p)
#define HIDE_LIST_SZ_2				(6)		//�����͈�(���g�p)
#define HIDE_LIST_TOP				(7)		//�����͈�(��l�������ʏ�)
#define HIDE_LIST_BOTTOM			(6)		//�����͈�(��l�������ʉ�)
#define HIDE_LIST_RESPONSE_NONE		(0xff)	//�I���R�[�h

//========================================================================
//	struct
//========================================================================
//--------------------------------------------------------------
//	FLDCOMMON_WORK
//	FIELDSYS_WORK�𗘗p���Ă����ӏ��ւ̉��Ώ��p�\���́B
//	���ł��B����������܂��B
//--------------------------------------------------------------
typedef void FLDCOMMON_WORK;

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
 * @param	fsys		FLDCOMMON_WORK�^�̃|�C���^
 * @param	scr_id		�X�N���v�gID
 * @param	obj			�b�������Ώ�OBJ�̃|�C���^(�Ȃ�����NULL)
 * @retval	none
 */
//--------------------------------------------------------------
extern GMEVENT * SCRIPT_SetEventScript(
		GAMESYS_WORK *gsys, u16 scr_id, MMDL *obj,
		HEAPID heapID, const SCRIPT_FLDPARAM *fparam );

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

//--------------------------------------------------------------
/**
 * @brief	���z�}�V���ǉ�
 * @param	fsys		FLDCOMMON_WORK�^�̃|�C���^
 * @param	scr_id		�X�N���v�gID
 * @param	start		���߃e�[�u���J�n�A�h���X
 * @param	end			���߃e�[�u���I���A�h���X
 * @retval	none
 */
//--------------------------------------------------------------
extern VMHANDLE * SCRIPT_AddVMachine(
	GAMESYS_WORK *gsys, SCRIPT_WORK *sc, HEAPID heapID, u16 scr_id );

//======================================================================
//	�X�N���v�g���䃏�[�N�̃����o�[�A�N�Z�X
//======================================================================
//--------------------------------------------------------------
/**
 * @brief	�v���O��������X�N���v�g�փp�����[�^��n��
 * @param	fsys	FLDCOMMON_WORK�^�̃|�C���^
 * @param	prm0	�p�����[�^�O�iSCWK_PARAM0�j
 * @param	prm1	�p�����[�^�P�iSCWK_PARAM1�j
 * @param	prm2	�p�����[�^�Q�iSCWK_PARAM2�j
 * @param	prm3	�p�����[�^�R�iSCWK_PARAM3�j
 *
 * SCRIPT_GetSubMemberWork���g�p���Ă��悢���A�����\����
 * �����o���ɂȂ��Ă���̂łS�̃p�����[�^�̂ݐ�p�֐���p�ӂ���
 */
//--------------------------------------------------------------
extern void SCRIPT_SetParam(
	SCRIPT_WORK *fsys, u16 prm0, u16 prm1, u16 prm2, u16 prm3 );

//--------------------------------------------------------------
/**
 * �X�N���v�g���䃏�[�N�̃����o�[�A�h���X�擾
 * @param	sc		SCRIPT�^�̃|�C���^
 * @param	id		�擾���郁���oID(script.h�Q��)
 * @return	"�A�h���X"
 */
//--------------------------------------------------------------
extern void * SCRIPT_GetSubMemberWork( SCRIPT_WORK * sc, u32 id );

//--------------------------------------------------------------
/**
 * �X�N���v�g���䃏�[�N�̃����o�[�A�h���X�擾
 * @param	fsys	FLDCOMMON_WORK�^�̃|�C���^
 * @param	id		�擾���郁���oID(script.h�Q��)
 * @return	"�A�h���X"
 */
//--------------------------------------------------------------
extern void * SCRIPT_GetMemberWork( SCRIPT_WORK *sc, u32 id );

//--------------------------------------------------------------
/**
 * �X�N���v�g���䃏�[�N��"next_func"�Ƀ��j���[�Ăяo�����Z�b�g
 * @param	fsys	FLDCOMMON_WORK�^�̃|�C���^
 * @return	none
 * �t�B�[���h���j���[�Ăяo������I
 * ���ꂪ�c�������炭�Ȃ�̂ŁA�ėp�I�Ɏg���Ȃ��悤�ɂ��Ă���I
 */
//--------------------------------------------------------------
#ifndef SCRIPT_PL_NULL
extern void SCRIPT_SetNextScript( FLDCOMMON_WORK* fsys );
#endif

//======================================================================
//	
//======================================================================
//------------------------------------------------------------------
/**
 * @brief	�C�x���g���[�N�A�h���X���擾
 *
 * @param	fsys		FLDCOMMON_WORK�ւ̃|�C���^
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
 * @param	fsys		FLDCOMMON_WORK�ւ̃|�C���^
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
 * @param	fsys		FLDCOMMON_WORK�ւ̃|�C���^
 * @param	work_no		���[�N�i���o�[
 * @param	value		�Z�b�g����l
 *
 * @return	"TRUE=�Z�b�g�o�����AFALSE=�Z�b�g�o���Ȃ�����"
 */
//------------------------------------------------------------------
extern BOOL SCRIPT_SetEventWorkValue(
		FLDCOMMON_WORK* fsys, u16 work_no, u16 value );

//------------------------------------------------------------------
/**
 * @brief	�X�N���v�g����w�肷��OBJ�R�[�h���擾
 *
 * @param	fsys		FLDCOMMON_WORK�ւ̃|�C���^
 * @param	no			0-15
 *
 * @return	"OBJ�L�����R�[�h"
 */
//------------------------------------------------------------------
extern u16 GetEvDefineObjCode( FLDCOMMON_WORK* fsys, u16 no );

//------------------------------------------------------------------
/**
 * @brief	�X�N���v�g����w�肷��OBJ�R�[�h���Z�b�g
 *
 * @param	fsys		FLDCOMMON_WORK�ւ̃|�C���^
 * @param	no			0-15
 * @param	obj_code	OBJ�R�[�h
 *
 * @return	"TRUE=�Z�b�g�o�����AFALSE=�Z�b�g�o���Ȃ�����"
 */
//------------------------------------------------------------------
extern BOOL SetEvDefineObjCode( FLDCOMMON_WORK* fsys, u16 no, u16 obj_code );


//======================================================================
//
//	�t���O�֘A
//
//======================================================================

//------------------------------------------------------------------
/**
 * @brief	�C�x���g�t���O���`�F�b�N����
 *
 * @param	fsys		FLDCOMMON_WORK�̃|�C���^
 * @param	flag_no		�t���O�i���o�[
 *
 * @retval	"1 = �t���OON"
 * @retval	"0 = �t���OOFF"
 */
//------------------------------------------------------------------
extern BOOL CheckEventFlag( FLDCOMMON_WORK* fsys, u16 flag_no);

//------------------------------------------------------------------
/**
 * @brief	�C�x���g�t���O���Z�b�g����
 *
 * @param	fsys		FLDCOMMON_WORK�̃|�C���^
 * @param	flag_no		�t���O�i���o�[
 *
 * @return	none
 */
//------------------------------------------------------------------
extern void SetEventFlag( FLDCOMMON_WORK* fsys, u16 flag_no);

//------------------------------------------------------------------
/**
 * @brief	�C�x���g�t���O�����Z�b�g����
 *
 * @param	fsys		FLDCOMMON_WORK�̃|�C���^
 * @param	flag_no		�t���O�i���o�[
 *
 * @return	none
 */
//------------------------------------------------------------------
extern void ResetEventFlag( FLDCOMMON_WORK* fsys, u16 flag_no);

//------------------------------------------------------------------
/**
 * @brief	�}�b�v������̃Z�[�u�t���O���N���A����
 *
 * @param	fsys		FLDCOMMON_WORK�̃|�C���^
 *
 * @return	none
 */
//------------------------------------------------------------------
extern void LocalEventFlagClear( FLDCOMMON_WORK* fsys );

//------------------------------------------------------------------
/**
 * @brief	1���o�߂��ƂɃN���A�����t���O���N���A����
 *
 * @param	fsys		FLDCOMMON_WORK�̃|�C���^
 *
 * @return	none
 */
//------------------------------------------------------------------
extern void TimeEventFlagClear( FLDCOMMON_WORK* fsys );


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
#ifndef SCRIPT_PL_NULL
extern u16 GetTrainerIdByScriptId( u16 scr_id );
#else
extern u16 SCRIPT_GetTrainerID_ByScriptID( u16 scr_id );
#endif

//--------------------------------------------------------------
/**
 * �X�N���v�gID����A���E�ǂ���̃g���[�i�[���擾
 *
 * @param   scr_id		�X�N���v�gID
 *
 * @retval  "0=���A1=�E"
 */
//--------------------------------------------------------------
#ifndef SCRIPT_PL_NULL
extern BOOL GetTrainerLRByScriptId( u16 scr_id );
#else
extern BOOL SCRIPT_GetTrainerLR_ByScriptID( u16 scr_id );
#endif

//--------------------------------------------------------------
/**
 * �g���[�i�[ID����A�_�u���o�g���^�C�v���擾
 *
 * @param   tr_id		�g���[�i�[ID
 *
 * @retval  "0=�V���O���o�g���A1=�_�u���o�g��"
 */
//--------------------------------------------------------------
#ifndef SCRIPT_PL_NULL
extern BOOL CheckTrainer2vs2Type( u16 tr_id );
#else
extern BOOL SCRIPT_CheckTrainer2vs2Type( u16 tr_id );
#endif

//------------------------------------------------------------------
/**
 * @brief	�g���[�i�[�C�x���g�t���O���`�F�b�N����
 *
 * @param	fsys		FLDCOMMON_WORK�̃|�C���^
 * @param	tr_id		�g���[�i�[ID
 *
 * @retval	"1 = �t���OON"
 * @retval	"0 = �t���OOFF"
 */
//------------------------------------------------------------------
#ifndef SCRIPT_PL_NULL
extern BOOL CheckEventFlagTrainer( FLDCOMMON_WORK* fsys, u16 tr_id );
#else
extern BOOL SCRIPT_CheckEventFlagTrainer( EVENTWORK *ev, u16 tr_id );
#endif

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
extern void SetEventFlagTrainer( FLDCOMMON_WORK* fsys, u16 tr_id );

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
extern void ResetEventFlagTrainer( FLDCOMMON_WORK* fsys, u16 tr_id );


//======================================================================
//
//	�B���A�C�e���֘A
//
//	�E�X�N���v�gID����A�B���A�C�e���t���O���擾���āA�t���O�`�F�b�N
//	BOOL CheckEventFlag( fsys, GetHideItemFlagNoByScriptId(scr_id) );
//
//======================================================================

//--------------------------------------------------------------
/**
 * �X�N���v�gID����A�B���A�C�e���t���O�i���o�[���擾
 *
 * @param   scr_id		�X�N���v�gID
 *
 * @retval  "�t���O�i���o�["
 */
//--------------------------------------------------------------
//extern u16 GetHideItemFlagNoByScriptId( u16 scr_id );
extern u16 SCRIPT_GetHideItemFlagNoByScriptID( u16 scr_id );

//--------------------------------------------------------------
/**
 * �X�N���v�gID����A�B���A�C�e���t���O�C���f�b�N�X���擾
 *
 * @param   scr_id		�X�N���v�gID
 *
 * @retval  "�t���O�C���f�b�N�X"
 */
//--------------------------------------------------------------
extern u16 GetHideItemFlagIndexByScriptId( u16 scr_id );

//--------------------------------------------------------------
/**
 * �X�N���v�gID����A�B���A�C�e���̔������擾
 *
 * @param   scr_id		�X�N���v�gID
 *
 * @retval  "����"
 *
 * �B���A�C�e���̃X�N���v�g����������ĂԂ悤�ɂ���I
 */
//--------------------------------------------------------------
extern u8 GetHideItemResponseByScriptId( u16 scr_id );

//--------------------------------------------------------------
/**
 * ��ʓ��ɂ���B���A�C�e�����������Ċm�ۂ������X�g�ɓo�^
 *
 * @param   fsys		FLDCOMMON_WORK�^�̃|�C���^
 * @param   heapid		�q�[�vID
 *
 * @retval  "���X�g�̃A�h���X"
 *
 * ���������Y�ꂸ�ɁI
 */
//--------------------------------------------------------------
#ifndef SCRIPT_PL_NULL
extern HIDE_ITEM_LIST * HideItem_CreateList( FLDCOMMON_WORK * fsys, int heapid );
#endif

//--------------------------------------------------------------
/**
 * 0���ŉB���t���O����������
 *
 * @param   fsys	FLDCOMMON_WORK�^�̃|�C���^
 *
 * @retval  none
 */
//--------------------------------------------------------------
extern void HideItemFlagOneDayClear( FLDCOMMON_WORK* fsys );

//------------------------------------------------------------------
/**
 * @brief	�Q�[���J�n �X�N���v�g�����ݒ�̎��s
 *
 * @param	fsys		FLDCOMMON_WORK�^�̃|�C���^
 *
 * @return	none
 */
//------------------------------------------------------------------
extern void GameStartScriptInit( FLDCOMMON_WORK* fsys );

//------------------------------------------------------------------
/**
 * @brief	EVENT�ł͂Ȃ�����X�N���v�g���s
 *
 * @param	fsys		FLDCOMMON_WORK�^�̃|�C���^
 * @param	scr_id		�X�N���v�gID
 *
 * @return	none
 *
 * ���ӁI
 * EV_SCRIPT_WORK���m�ۂ��Ă��Ȃ��̂ŁA
 * SCWK_ANSWER�Ȃǂ̃��[�N�͎g�p���邱�Ƃ��o���Ȃ��I
 * LOCALWORK0�Ȃǂ��g�p����悤�ɂ���I
 *
 * ���ʃX�N���v�g�ɂ��Ă͌���g�p�s�I
 * �Ή��\�肾���A�]�[�����܂��������ɏ����������邩���H
 * ���܂����Ȃ������H
 *
 * �t���O�`�F���W���x���ŋ��ʃX�N���v�g���g�������Ƃ��́A
 * ��t���O�`�F���W���x�����������ƂɂȂ邩���H
 * �Е��́A�]�[���ł��Ƃ������́A�Е��́A���ʃX�N���v�g��ID���w��H�B�B�B
 */
//------------------------------------------------------------------
extern void SpScriptStart( FLDCOMMON_WORK* fsys, u16 scr_id );

//------------------------------------------------------------------
/**
 * @brief	����X�N���v�g�������Ď��s
 *
 * @param	fsys		FLDCOMMON_WORK�^�̃|�C���^
 * @param	key			����X�N���v�gID
 *
 * @return	"TRUE=����X�N���v�g���s�AFALSE=�������Ȃ�"
 */
//------------------------------------------------------------------
extern BOOL SpScriptSearch( FLDCOMMON_WORK* fsys, u8 key );



/***************/
/***************/
/***************/
/***************/
/***************/
/***************/
/***************/


extern void InitControlScriptCheck( void * p );

extern u16 * GetEventWorkAdrs( FLDCOMMON_WORK* fsys, u16 work_no );
extern u16 GetEventWorkValue( FLDCOMMON_WORK* fsys, u16 work_no );


#endif	/* SCRIPT_H */


