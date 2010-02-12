//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_season_time.h
 *	@brief  �G�߂ɂ��@���ԑѕω�
 *	@author	tomoya takahashi
 *	@date		2010.01.28
 *
 *	���W���[�����FFLD_SEASON_TIME
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#include <gflib.h>

#include "system/timezone.h"

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
///	�G�߂ɂ�鎞�ԑѕω��Ǘ����[�N
//=====================================
typedef struct _FLD_SEASON_TIME FLD_SEASON_TIME;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

// �����E�j��
extern FLD_SEASON_TIME* FLD_SEASON_TIME_Create( u8 season_id, HEAPID heapID );
extern void FLD_SEASON_TIME_Delete( FLD_SEASON_TIME* p_sys );


// �^�C���]�[���擾
extern TIMEZONE FLD_SEASON_TIME_GetTimeZone( const FLD_SEASON_TIME* cp_sys );

// ���̎��Ԃ���^�C���]�[���؂�ւ����Ԃ܂ł̍�(�b��)
extern int FLD_SEASON_TIME_GetTimeZoneChangeTime( const FLD_SEASON_TIME* cp_sys, TIMEZONE timezone );



#ifdef _cplusplus
}	// extern "C"{
#endif



