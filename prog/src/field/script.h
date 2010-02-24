//======================================================================
/**
 * @file	script.h
 * @brief	�X�N���v�g���䃁�C������
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

#include "fldmmdl.h"    //MMDL

//======================================================================
//	define
//======================================================================
//--------------------------------------------------------------
//	debug
//--------------------------------------------------------------
#define SCRIPT_PL_NULL	//PL�ӏ�������
#define DEBUG_SCRIPT		//��`�Ńf�o�b�O�@�\�L��

//--------------------------------------------------------------
//--------------------------------------------------------------
enum {
  SCRID_NULL = 0
};
//========================================================================
//	struct
//========================================================================
//--------------------------------------------------------------
//	�X�N���v�g���䃏�[�N�ւ̕s���S�^�|�C���^
//--------------------------------------------------------------
typedef struct _SCRIPTSYS SCRIPTSYS;

typedef	struct _TAG_SCRIPT_WORK SCRIPT_WORK;

//--------------------------------------------------------------
//	�X�N���v�g�C�x���g�p���[�N
//--------------------------------------------------------------
typedef struct
{
	SCRIPT_WORK *sc; //�X�N���v�g�p���[�N
  SCRIPTSYS * scrsys; //���䃏�[�N
}EVENT_SCRIPT_WORK;

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
		HEAPID temp_heapID );

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
extern SCRIPT_WORK * SCRIPT_CallScript( GMEVENT *event,
	u16 scr_id, MMDL *obj, void *ret_script_wk, HEAPID temp_heapID );

//--------------------------------------------------------------
/**
 * �X�N���v�g�C�x���g�ؑ�
 * @param	event		GMEVENT_CONTROL�^�̃|�C���^
 * @param	scr_id		�X�N���v�gID
 * @param	obj			�b�������Ώ�OBJ�̃|�C���^(�Ȃ�����NULL)
 * @retval	SCRIPT_WORK
 * ���̃C�x���g����X�N���v�g�ւ̐ؑւ��s��
 */
//--------------------------------------------------------------
extern SCRIPT_WORK * SCRIPT_ChangeScript( GMEVENT *event,
		u16 scr_id, MMDL *obj, HEAPID temp_heapID );

//--------------------------------------------------------------
/**
 * �X�N���v�g�C�x���g����X�N���v�g���[�N���擾
 * @param	GMEVENT *event
 * @retval  SCRIPT_WORK*
 */
//--------------------------------------------------------------
extern SCRIPT_WORK* SCRIPT_GetScriptWorkFromEvent( GMEVENT *event );

//--------------------------------------------------------------
/// �X�N���v�g�C�x���g����X�N���v�g���䃏�[�N���擾
//--------------------------------------------------------------
extern SCRIPTSYS * SCRIPT_GetScriptSysFromEvent( GMEVENT *event );

//--------------------------------------------------------------
/// �X�N���v�g���[�N����X�N���v�g���䃏�[�N���擾
//--------------------------------------------------------------
extern SCRIPTSYS * SCRIPT_GetScriptSys( SCRIPT_WORK * sc );


//------------------------------------------------------------------
/**
 * @brief �X�N���v�gID�̃`�F�b�N
 * @param script_id
 * @return  BOOL  FALSE�̂Ƃ��A���s�\�ȃX�N���v�g
 */
//------------------------------------------------------------------
extern BOOL SCRIPT_IsValidScriptID( u16 script_id );

//======================================================================
//
//
//    ����X�N���v�g�֘A
//
//
//======================================================================
//--------------------------------------------------------------
/**
 * ����X�N���v�g�Ăяo��
 * @param gsys
 * @param sc
 * @param heapID
 * @param script_id
 */
//--------------------------------------------------------------
extern void SCRIPT_CallSpecialScript(
    GAMESYS_WORK *gsys, SCRIPT_WORK * sc, HEAPID heapID, u16 script_id );

//------------------------------------------------------------------
/**
 * @brief	�Q�[���J�n �X�N���v�g�����ݒ�̎��s
 */
//------------------------------------------------------------------
extern void SCRIPT_CallGameStartInitScript( GAMESYS_WORK *gsys, HEAPID heapID );

#ifdef  PM_DEBUG
//------------------------------------------------------------------
/**
 * @brief	�Q�[���J�n �X�N���v�g�����ݒ�̎��s�i�f�o�b�O�X�^�[�g�j
 */
//------------------------------------------------------------------
extern void SCRIPT_CallDebugGameStartInitScript( GAMESYS_WORK *gsys, HEAPID heapID );
#endif  //PM_DEBUG

//------------------------------------------------------------------
/**
 * @brief	�Q�[���N���A�������̃X�N���v�g���s
 */
//------------------------------------------------------------------
extern void SCRIPT_CallGameClearScript( GAMESYS_WORK *gsys, HEAPID heapID );

//------------------------------------------------------------------
/**
 * @brief ����X�N���v�g�Ăяo���F�t�B�[���h���A�i����̂݁j
 * @param gsys
 * @param heapID
 * @retval  TRUE    ����X�N���v�g�����s����
 * @retval  FALSE   ����X�N���v�g�͑��݂��Ȃ�
 */
//------------------------------------------------------------------
extern BOOL SCRIPT_CallFieldInitScript( GAMESYS_WORK * gsys, HEAPID heapID );

//------------------------------------------------------------------
/**
 * @brief ����X�N���v�g�Ăяo���F�]�[���؂�ւ�
 * @param gsys
 * @param heapID
 * @retval  TRUE    ����X�N���v�g�����s����
 * @retval  FALSE   ����X�N���v�g�͑��݂��Ȃ�
 */
//------------------------------------------------------------------
extern BOOL SCRIPT_CallZoneChangeScript( GAMESYS_WORK * gsys, HEAPID heapID);

//------------------------------------------------------------------
/**
 * @brief ����X�N���v�g�Ăяo���F�t�B�[���h���A
 * @param gsys
 * @param heapID
 * @retval  TRUE    ����X�N���v�g�����s����
 * @retval  FALSE   ����X�N���v�g�͑��݂��Ȃ�
 */
//------------------------------------------------------------------
extern BOOL SCRIPT_CallFieldRecoverScript( GAMESYS_WORK * gsys, HEAPID heapID );

//------------------------------------------------------------------
/**
 * @brief	�V�[���N���X�N���v�g�̌���
 * @param gsys
 * @param heapID
 * @retval  NULL      ���݂̓V�[���N���X�N���v�g�����݂��Ȃ�
 * @retval  GMEVENT   �N������ׂ��X�N���v�g���L�[�ɐ������ꂽ�C�x���g�ւ̃|�C���^
 */
//------------------------------------------------------------------
extern GMEVENT * SCRIPT_SearchSceneScript( GAMESYS_WORK * gsys, HEAPID heapID);


//--------------------------------------------------------------
/**
 * �v���O��������X�N���v�g�ւ̈����ƂȂ�p�����[�^���Z�b�g
 * @param	sc SCRIPT_WORK
 * @param	prm0	�p�����[�^�O�iSCWK_PARAM0�j
 * @param	prm1	�p�����[�^�P�iSCWK_PARAM1�j
 * @param	prm2	�p�����[�^�Q�iSCWK_PARAM2�j
 * @param	prm3	�p�����[�^�R�iSCWK_PARAM3�j
 * @retval none
 */
//--------------------------------------------------------------
extern void SCRIPT_SetScriptWorkParam( SCRIPT_WORK *sc, u16 prm0, u16 prm1, u16 prm2, u16 prm3 );

//--------------------------------------------------------------
/**
 * �T�u�v���Z�X�p���[�N�̃|�C���^�[�A�h���X���擾
 * @param	sc SCRIPT_WORK
 * @retval none
 *
 * �g���I�������K��NULL�N���A���邱�ƁI
 */
//--------------------------------------------------------------
extern void** SCRIPT_SetSubProcWorkPointerAdrs( SCRIPT_WORK *sc );

//--------------------------------------------------------------
/**
 * �T�u�v���Z�X�p���[�N�̃|�C���^�[���擾
 * @param	sc SCRIPT_WORK
 * @retval none
 *
 * �g���I�������K��NULL�N���A���邱�ƁI
 */
//--------------------------------------------------------------
extern void* SCRIPT_SetSubProcWorkPointer( SCRIPT_WORK *sc );

//--------------------------------------------------------------
/**
 * �T�u�v���Z�X�p���[�N�̈�̉��(�|�C���^��NULL�łȂ����Free)
 * @param	sc SCRIPT_WORK
 * @retval none
 */
//--------------------------------------------------------------
extern void SCRIPT_FreeSubProcWorkPointer( SCRIPT_WORK *sc );


//======================================================================
//
//	�g���[�i�[�֘A
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


extern void SCRIPT_SetTrainerEyeData( GMEVENT *event, MMDL *mmdl,
    s16 range, u16 dir, u16 scr_id, u16 tr_id, int tr_type, int tr_no );

extern void * SCRIPT_GetTrainerEyeData( SCRIPT_WORK * sc, u32 tr_no );


#endif	/* SCRIPT_H */
