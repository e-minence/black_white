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
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�C�x���g�ł̃t�F�[�h��ގw��
 *
 * ���̂Ƃ���_�~�[�Őݒ肵�Ă���B���x�Ȃǂ��w�肷�邩���H
 */
//------------------------------------------------------------------
typedef enum {
	FIELD_FADE_BLACK = 0,
	FIELD_FADE_WHITE,
}FIELD_FADE_TYPE;

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�t�F�[�h�A�E�g�C�x���g����
 * @param	gsys		GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap	�t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @param	type		�t�F�[�h�̎�ގw��
 * @return	GMEVENT		���������C�x���g�ւ̃|�C���^
 */
//------------------------------------------------------------------
extern GMEVENT * EVENT_FieldFadeOut(GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldmap, FIELD_FADE_TYPE type);

//------------------------------------------------------------------
/**
 * @brief	�t�F�[�h�C���C�x���g����
 * @param	gsys		GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap	�t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @param	type		�t�F�[�h�̎�ގw��
 * @return	GMEVENT		���������C�x���g�ւ̃|�C���^
 */
//------------------------------------------------------------------
extern GMEVENT * EVENT_FieldFadeIn(GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldmap, FIELD_FADE_TYPE type);

//------------------------------------------------------------------
/**
 * @brief	�t�B�[���h�}�b�v�J�n�C�x���g�̐���
 * @param	gsys		GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap	�t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @return	GMEVENT		���������C�x���g�ւ̃|�C���^
 */
//------------------------------------------------------------------
extern GMEVENT * EVENT_FieldOpen(GAMESYS_WORK *gsys);

//------------------------------------------------------------------
/**
 * @brief	�t�B�[���h�}�b�v�I���C�x���g����
 * @param	gsys		GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap	�t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @return	GMEVENT		���������C�x���g�ւ̃|�C���^
 */
//------------------------------------------------------------------
extern GMEVENT * EVENT_FieldClose(GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldmap);

//------------------------------------------------------------------
/**
 * @brief	�T�u�v���Z�X�Ăяo���C�x���g����
 * @param	gsys		GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap	�t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @param	ov_id		�J�ڂ���T�u�v���Z�X�̃I�[�o�[���C�w��
 * @param	proc_data	�J�ڂ���T�u�v���Z�X�̃v���Z�X�f�[�^�ւ̃|�C���^
 * @param	proc_work	�J�ڂ���T�u�v���Z�X�Ŏg�p���郏�[�N�ւ̃|�C���^
 * @return	GMEVENT		���������C�x���g�ւ̃|�C���^
 *
 * �t�F�[�h�A�E�g���t�B�[���h�}�b�v�I�����T�u�v���Z�X�Ăяo��
 * ���t�B�[���h�}�b�v�ĊJ���t�F�[�h�C������������
 */
//------------------------------------------------------------------
extern GMEVENT * EVENT_FieldSubProc(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap,
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
