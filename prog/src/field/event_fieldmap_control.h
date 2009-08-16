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
extern GMEVENT * EVENT_FieldFadeOut(GAMESYS_WORK *gsys, FIELD_MAIN_WORK * fieldmap, FIELD_FADE_TYPE type);

//------------------------------------------------------------------
/**
 * @brief	�t�F�[�h�C���C�x���g����
 * @param	gsys		GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap	�t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @param	type		�t�F�[�h�̎�ގw��
 * @return	GMEVENT		���������C�x���g�ւ̃|�C���^
 */
//------------------------------------------------------------------
extern GMEVENT * EVENT_FieldFadeIn(GAMESYS_WORK *gsys, FIELD_MAIN_WORK * fieldmap, FIELD_FADE_TYPE type);

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
extern GMEVENT * EVENT_FieldClose(GAMESYS_WORK *gsys, FIELD_MAIN_WORK * fieldmap);

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
extern GMEVENT * EVENT_FieldSubProc(GAMESYS_WORK * gsys, FIELD_MAIN_WORK * fieldmap,
		FSOverlayID ov_id, const GFL_PROC_DATA * proc_data, void * proc_work);


