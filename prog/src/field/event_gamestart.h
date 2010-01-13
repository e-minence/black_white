//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		event_gamestart.h
 *	@brief  �C�x���g�@�Q�[���J�n
 *	@author	tomoya takahashi
 *	@date		2010.01.12
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_init.h"
#include "field/fieldmap.h"


//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

//------------------------------------------------------------------
/**
 * @brief �Q�[���J�n�C�x���g
 */
//------------------------------------------------------------------
extern GMEVENT * EVENT_GameStart(GAMESYS_WORK * gsys, GAME_INIT_WORK * game_init_work);

//------------------------------------------------------------------
/**
 * @brief �f�o�b�O�p�F�Q�[���I��
 */
//------------------------------------------------------------------
extern GMEVENT * DEBUG_EVENT_GameEnd( GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap);

//--------------------------------------------------------------
/**
 * @brief �S�Ŏ��̃}�b�v�J�ڏ����i�t�B�[���h�񐶐����j
 */
//--------------------------------------------------------------
extern GMEVENT * EVENT_GameOver( GAMESYS_WORK * gsys );


//--------------------------------------------------------------
/**
 * @brief FIELD_INIT�I�[�o�[���C�̓ǂݍ��݁E�j��
 */
//--------------------------------------------------------------
extern void GAMESTART_OVERLAY_FIELD_INIT_Load( void );
extern void GAMESTART_OVERLAY_FIELD_INIT_UnLoad( void );


#ifdef _cplusplus
}	// extern "C"{
#endif



