//============================================================================================
/**
 * @file	event_fieldmap_control.h
 * @brief	�C�x���g�F�t�B�[���h�}�b�v����c�[��
 * @author	tamada GAMEFREAK inc.
 * @date	2008.12.10
 */
//============================================================================================
#pragma once

#include <gflib.h>

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "app/pokelist.h" // PLIST_DATA
#include "app/p_status.h" // PSTATUS_DATA

//============================================================================================
// ���t�F�[�h����
//============================================================================================
//  2009.12.22  event_field_fade�Ƃ��ēƗ�������
#include "event_field_fade.h"

//============================================================================================
// ���t�B�[���h�}�b�v�̊J�n/�I��
//============================================================================================

//------------------------------------------------------------------
/**
 * @brief	�t�B�[���h�}�b�v�J�n�C�x���g�̐���
 * @param	gsys		 GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap �t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @return	GMEVENT		���������C�x���g�ւ̃|�C���^
 */
//------------------------------------------------------------------
extern GMEVENT * EVENT_FieldOpen(GAMESYS_WORK *gsys);

//------------------------------------------------------------------
/**
 * @brief	�t�B�[���h�}�b�v�I���C�x���g���� ( FIELD_SOUND�̏����҂��L )
 * @param	gsys		GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap	�t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @return	GMEVENT		���������C�x���g�ւ̃|�C���^
 */
//------------------------------------------------------------------
extern GMEVENT * EVENT_FieldClose(GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldmap);


//============================================================================================
// ���T�u�v���Z�X�Ăяo��
//============================================================================================

//------------------------------------------------------------------
/**
 * @brief	�T�u�v���Z�X�Ăяo���C�x���g����
 * @param	gsys		  GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap	�t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @param	ov_id		  �J�ڂ���T�u�v���Z�X�̃I�[�o�[���C�w��
 * @param	proc_data	�J�ڂ���T�u�v���Z�X�̃v���Z�X�f�[�^�ւ̃|�C���^
 * @param	proc_work	�J�ڂ���T�u�v���Z�X�Ŏg�p���郏�[�N�ւ̃|�C���^
 * @return ���������C�x���g�ւ̃|�C���^
 *
 * �t�F�[�h�A�E�g���t�B�[���h�}�b�v�I�����T�u�v���Z�X�Ăяo��
 * ���t�B�[���h�}�b�v�ĊJ���t�F�[�h�C������������
 */
//------------------------------------------------------------------
extern GMEVENT * EVENT_FieldSubProc( GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap,
		FSOverlayID ov_id, const GFL_PROC_DATA * proc_data, void * proc_work);

//------------------------------------------------------------------
/**
 * @brief	�T�u�v���Z�X�Ăяo���C�x���g����(�t�F�[�h�Ȃ�)
 * @param	gsys		  GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap	�t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @param	ov_id		  �J�ڂ���T�u�v���Z�X�̃I�[�o�[���C�w��
 * @param	proc_data	�J�ڂ���T�u�v���Z�X�̃v���Z�X�f�[�^�ւ̃|�C���^
 * @param	proc_work	�J�ڂ���T�u�v���Z�X�Ŏg�p���郏�[�N�ւ̃|�C���^
 * @return ���������C�x���g�ւ̃|�C���^
 *
 * �t�B�[���h�}�b�v�I�� �� �T�u�v���Z�X�Ăяo�� �� �t�B�[���h�}�b�v�ĊJ
 */
//------------------------------------------------------------------
extern GMEVENT * EVENT_FieldSubProcNoFade( GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap,
		FSOverlayID ov_id, const GFL_PROC_DATA * proc_data, void * proc_work); 

//------------------------------------------------------------------
/**
 * @brief	�T�u�v���Z�X�Ăяo���C�x���g����(�R�[���o�b�N�֐��t��)
 * @param	gsys		      GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap	    �t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @param	ov_id		      �J�ڂ���T�u�v���Z�X�̃I�[�o�[���C�w��
 * @param	proc_data	    �J�ڂ���T�u�v���Z�X�̃v���Z�X�f�[�^�ւ̃|�C���^
 * @param	proc_work	    �J�ڂ���T�u�v���Z�X�Ŏg�p���郏�[�N�ւ̃|�C���^
 * @param callback      �R�[���o�b�N�֐�
 * @param callback_work �R�[���o�b�N�֐��ɂ킽���|�C���^
 * @return	GMEVENT		���������C�x���g�ւ̃|�C���^
 *
 * �t�F�[�h�A�E�g���t�B�[���h�}�b�v�I�����T�u�v���Z�X�Ăяo��
 * ���t�B�[���h�}�b�v�ĊJ���t�F�[�h�C������������
 */
//------------------------------------------------------------------
extern GMEVENT * EVENT_FieldSubProc_Callback(
    GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap,
		FSOverlayID ov_id, const GFL_PROC_DATA * proc_data, void * proc_work,
    void (*callback)(void*), void* callback_work );

//------------------------------------------------------------------
/**
 * @brief	�T�u�v���Z�X�Ăяo���C�x���g����(�t�F�[�h�Ȃ�, �R�[���o�b�N�֐��t��)
 * @param	gsys		      GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap	    �t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @param	ov_id		      �J�ڂ���T�u�v���Z�X�̃I�[�o�[���C�w��
 * @param	proc_data	    �J�ڂ���T�u�v���Z�X�̃v���Z�X�f�[�^�ւ̃|�C���^
 * @param	proc_work	    �J�ڂ���T�u�v���Z�X�Ŏg�p���郏�[�N�ւ̃|�C���^
 * @param callback      �R�[���o�b�N�֐�
 * @param callback_work �R�[���o�b�N�֐��ɂ킽���|�C���^
 * @return	GMEVENT		���������C�x���g�ւ̃|�C���^
 *
 * �t�F�[�h�A�E�g���t�B�[���h�}�b�v�I�����T�u�v���Z�X�Ăяo��
 * ���t�B�[���h�}�b�v�ĊJ���t�F�[�h�C������������
 */
//------------------------------------------------------------------
extern GMEVENT * EVENT_FieldSubProcNoFade_Callback(
    GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap,
		FSOverlayID ov_id, const GFL_PROC_DATA * proc_data, void * proc_work,
    void (*callback)(void*), void* callback_work );


//============================================================================================
// ���莝���|�P�����I��
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�|�P�����I���C�x���g
 * @param	gsys		    GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap	  �t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @param list_data   �|�P�������X�g���    
 * @param status_data �|�P�����X�e�[�^�X���
 * @return	GMEVENT		���������C�x���g�ւ̃|�C���^
 *
 * �t�F�[�h�A�E�g���t�B�[���h�}�b�v�I�����T�u�v���Z�X�Ăяo��
 * ���t�B�[���h�}�b�v�ĊJ���t�F�[�h�C������������
 */
//------------------------------------------------------------------
extern GMEVENT * EVENT_PokeSelect( 
    GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap, 
    PLIST_DATA* list_data, PSTATUS_DATA* status_data );
