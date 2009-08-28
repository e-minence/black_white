//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		fieldmap_ctrl_nogrid_work.h
 *	@brief  �t�B�[���h�}�b�v�R���g���[���@�m�[�O���b�h�@���ʃ��[�N
 *	@author	tomoya takahashi
 *	@date		2009.08.28
 *
 *	���W���[�����FFIELDMAP_CTRL_NOGRID_WORK
 *
 *	�m�[�O���b�h�}�b�v�́@�}�b�v�R���g���[���ł́A���̃��[�N���m�ۂ���悤�ɂ��Ă��������B
 *
 *	
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#include "field_player_nogrid.h"


//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�t�B�[���h�}�b�v�R���g���[���@�m�[�O���b�h�}�b�v�@���ʃ��[�N
//=====================================
typedef struct _FIELDMAP_CTRL_NOGRID_WORK FIELDMAP_CTRL_NOGRID_WORK;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

// �������m�ہE�j��
extern FIELDMAP_CTRL_NOGRID_WORK* FIELDMAP_CTRL_NOGRID_WORK_Create( u32 heapID );
extern void FIELDMAP_CTRL_NOGRID_WORK_Delete( FIELDMAP_CTRL_NOGRID_WORK* p_wk );

// ���[�J�����[�N�̊m�ہE�j���E�m�ۃ������̎擾
extern void* FIELDMAP_CTRL_NOGRID_WORK_AllocLocalWork( FIELDMAP_CTRL_NOGRID_WORK* p_wk, u32 heapID, u32 size );
extern void FIELDMAP_CTRL_NOGRID_WORK_FreeLocalWork( FIELDMAP_CTRL_NOGRID_WORK* p_wk );
extern void* FIELDMAP_CTRL_NOGRID_WORK_GetLocalWork( const FIELDMAP_CTRL_NOGRID_WORK* cp_wk );

// ���擾
extern FIELD_PLAYER_NOGRID_WORK* FIELDMAP_CTRL_NOGRID_WORK_GetNogridPlayerWork( const FIELDMAP_CTRL_NOGRID_WORK* cp_wk );

#ifdef _cplusplus
}	// extern "C"{
#endif



