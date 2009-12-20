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
// ���t�F�[�h�p�����[�^
//============================================================================================

//------------------------------------------------------------------
/**
 * @brief	�C�x���g�ł̃t�F�[�h��ގw��
 *
 * ���̂Ƃ���_�~�[�Őݒ肵�Ă���B���x�Ȃǂ��w�肷�邩���H
 */
//------------------------------------------------------------------
typedef enum {
	FIELD_FADE_BLACK,  // �P�x�t�F�[�h(�u���b�N)
	FIELD_FADE_WHITE,  // �P�x�t�F�[�h(�z���C�g)
  FIELD_FADE_CROSS,  // �N���X�t�F�[�h
} FIELD_FADE_TYPE;

//------------------------------------------------------------------
/**
 * @brief	�C�x���g�ł̋G�ߕ\���̗L���w��t���O
 */
//------------------------------------------------------------------
typedef enum{
  FIELD_FADE_SEASON_ON,   // �G�ߕ\����������
  FIELD_FADE_SEASON_OFF,  // �G�ߕ\���������Ȃ�
} FIELD_FADE_SEASON_FLAG;

//------------------------------------------------------------------
/**
 * @brief	�P�x�t�F�[�h�̊����҂��t���O
 */
//------------------------------------------------------------------
typedef enum{
  FIELD_FADE_WAIT,    // �t�F�[�h������҂�
  FIELD_FADE_NO_WAIT, // �t�F�[�h������҂��Ȃ�
} FIELD_FADE_WAIT_TYPE;


//============================================================================================
// ����{�t�F�[�h�֐�
//============================================================================================

//------------------------------------------------------------------
/**
 * @brief	�t�F�[�h�A�E�g�C�x���g����
 * @param	gsys		  GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap	�t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @param	type		  �t�F�[�h�̎�ގw��
 * @param wait      �t�F�[�h������҂��ǂ���(�P�x�t�F�[�h���̂ݗL��)
 * @return	GMEVENT	���������C�x���g�ւ̃|�C���^*/
//------------------------------------------------------------------
extern GMEVENT * EVENT_FieldFadeOut( GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldmap, 
                                     FIELD_FADE_TYPE type, 
                                     FIELD_FADE_WAIT_TYPE wait );
// �N���X�t�F�[�h
#define EVENT_FieldFadeOut_Cross( gsys, fieldmap ) \
        EVENT_FieldFadeOut( gsys, fieldmap, FIELD_FADE_CROSS, 0 )
// �P�x�t�F�[�h(�u���b�N)
#define EVENT_FieldFadeOut_Black( gsys, fieldmap, wait ) \
        EVENT_FieldFadeOut( gsys, fieldmap, FIELD_FADE_BLACK, wait )
// �P�x�t�F�[�h(�z���C�g)
#define EVENT_FieldFadeOut_White( gsys, fieldmap, wait ) \
        EVENT_FieldFadeOut( gsys, fieldmap, FIELD_FADE_WHITE, wait )

//------------------------------------------------------------------
/**
 * @brief	�t�F�[�h�C���C�x���g����
 * @param	gsys		  GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap  �t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @param	type		  �t�F�[�h�̎�ގw��
 * @param season    �G�ߕ\���������邩�ǂ���(�P�x�t�F�[�h���̂ݗL��)
 * @param wait      �t�F�[�h������҂��ǂ���(�P�x�t�F�[�h���̂ݗL��)
 * @param bg_init_flag  BG������������ł����Ȃ����ǂ���(�P�x�t�F�[�h���̂ݗL��)
 * @return	GMEVENT	���������C�x���g�ւ̃|�C���^
 */
//------------------------------------------------------------------
extern GMEVENT * EVENT_FieldFadeIn( GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldmap, 
                                    FIELD_FADE_TYPE type, 
                                    FIELD_FADE_SEASON_FLAG season,
                                    FIELD_FADE_WAIT_TYPE wait,
                                    BOOL bg_init_flag );
// �N���X�t�F�[�h
#define EVENT_FieldFadeIn_Cross( gsys, fieldmap ) \
        EVENT_FieldFadeIn( gsys, fieldmap, FIELD_FADE_CROSS, 0, 0, TRUE )
// �P�x�t�F�[�h(�u���b�N)
#define EVENT_FieldFadeIn_Black( gsys, fieldmap, wait ) \
        EVENT_FieldFadeIn( gsys, fieldmap, FIELD_FADE_BLACK, FIELD_FADE_SEASON_OFF, wait, TRUE )
// �P�x�t�F�[�h(�z���C�g)
#define EVENT_FieldFadeIn_White( gsys, fieldmap, wait ) \
        EVENT_FieldFadeIn( gsys, fieldmap, FIELD_FADE_WHITE, FIELD_FADE_SEASON_OFF, wait, TRUE )
// �G�߃t�F�[�h
#define EVENT_FieldFadeIn_Season( gsys, fieldmap ) \
        EVENT_FieldFadeIn( gsys, fieldmap, FIELD_FADE_BLACK, FIELD_FADE_SEASON_ON, 0, TRUE )
// ���j���[��ԃt�F�[�h�iY�{�^�����j���[���o�����܂܂Ȃǁj
#define EVENT_FieldFadeIn_Menu( gsys, fieldmap, wait ) \
        EVENT_FieldFadeIn( gsys, fieldmap, FIELD_FADE_BLACK, FIELD_FADE_SEASON_OFF, wait, FALSE )


//============================================================================================
// ����O�I�ȃt�F�[�h�֐�
//============================================================================================

//------------------------------------------------------------------
/**
 * @brief	����ԃu���C�g�l�X�A�E�g�C�x���g����
 * @param	gsys		  GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap	�t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @param	type		  �t�F�[�h�̎�ގw��
 * @param wait      �t�F�[�h������҂��ǂ���
 * @return	GMEVENT	���������C�x���g�ւ̃|�C���^
 */
//------------------------------------------------------------------
extern GMEVENT * EVENT_FlySkyBrightOut( GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldmap, 
                                       FIELD_FADE_TYPE type, FIELD_FADE_WAIT_TYPE wait );

//------------------------------------------------------------------
/**
 * @brief	����ԃu���C�g�l�X�C���C�x���g����
 * @param	gsys		  GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap  �t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @param	type		  �t�F�[�h�̎�ގw��
 * @param wait      �t�F�[�h������҂��ǂ���
 * @return	GMEVENT	���������C�x���g�ւ̃|�C���^
 */
//------------------------------------------------------------------
extern GMEVENT * EVENT_FlySkyBrightIn( GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldmap, 
                                    FIELD_FADE_TYPE type, FIELD_FADE_WAIT_TYPE wait );

//------------------------------------------------------------------
/**
 * @brief	�f�o�b�O�p �����A�E�g
 * @param	gsys		  GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap	�t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @return	GMEVENT	���������C�x���g�ւ̃|�C���^
 */
//------------------------------------------------------------------
extern GMEVENT* DEBUG_EVENT_QuickFadeOut( GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldmap); 

//------------------------------------------------------------------
/**
 * @brief	�f�o�b�O�p �����C��
 * @param	gsys		  GAMESYS_WORK�ւ̃|�C���^
 * @param	fieldmap	�t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 * @return	GMEVENT	���������C�x���g�ւ̃|�C���^
 */
//------------------------------------------------------------------
extern GMEVENT* DEBUG_EVENT_QuickFadeIn( GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldmap); 


//============================================================================================
// ���t�B�[���h�}�b�v�̊J�n/�I��
//============================================================================================

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
