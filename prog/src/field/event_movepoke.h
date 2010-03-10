//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		event_movepoke.h
 *	@brief  �ړ��|�P�����@�C�x���g
 *	@author	tomoya takahashi
 *	@date		2010.03.09
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#include "gflib.h"
#include "gamesystem/gamesystem.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�|�P��������A�j���[�V�����^�C�v
//  src/field/script_def.h�̒�`�ƈ�v���Ă���K�v������B
//=====================================
typedef enum {
  EV_MOVEPOKE_ANIME_RAI_KAZA_INSIDE,
  EV_MOVEPOKE_ANIME_RAI_KAZA_OUTSIDE,

  EV_MOVEPOKE_ANIME_TUCHI_INSIDE,
  EV_MOVEPOKE_ANIME_TUCHI_OUTSIDE,

  EV_MOVEPOKE_ANIME_MAX,
    
} EV_MOVEPOKE_ANIME_TYPE;

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�ړ��|�P�����@���샏�[�N
//=====================================
typedef struct _EV_MOVEPOKE_WORK EV_MOVEPOKE_WORK;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
// �e�|�P��������C�x���g
extern EV_MOVEPOKE_WORK* EVENT_MOVEPOKE_Create( FIELDMAP_WORK* p_fieldmap, u16 id, const VecFx32* cp_pos, HEAPID heapID );
extern void EVENT_MOVEPOKE_Delete( EV_MOVEPOKE_WORK* p_wk );

extern void EVENT_MOVEPOKE_StartAnime( EV_MOVEPOKE_WORK* p_wk, EV_MOVEPOKE_ANIME_TYPE anime_no );
extern BOOL EVENT_MOVEPOKE_IsAnimeEnd( const EV_MOVEPOKE_WORK* cp_wk );


#ifdef _cplusplus
}	// extern "C"{
#endif



