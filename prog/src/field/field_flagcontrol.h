//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_flagcontrol.h
 *	@brief  �t�B�[���h�@�t���O�Ǘ�
 *	@author	tomoya takahashi
 *	@date		2009.11.19
 *
 *	���W���[�����FFIELD_FLAGCONT
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#include "field/fieldmap.h"
#include "gamesystem/game_data.h"

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

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	�e��t���O�̏������^�C�~���O
//=====================================
extern void FIELD_FLAGCONT_INIT_WalkStepOver(GAMEDATA * gdata, FIELDMAP_WORK* fieldWork);   // fieldmap.c
extern void FIELD_FLAGCONT_INIT_MapJump(GAMEDATA * gdata, u16 zone_id);        // event_mapchange.c
extern void FIELD_FLAGCONT_INIT_FlySky(GAMEDATA * gdata, u16 zone_id);        // event_mapchange.c
extern void FIELD_FLAGCONT_INIT_Teleport(GAMEDATA * gdata, u16 zone_id);      // event_mapchange.c
extern void FIELD_FLAGCONT_INIT_Escape(GAMEDATA * gdata, u16 zone_id);        // event_mapchange.c
extern void FIELD_FLAGCONT_INIT_GameOver(GAMEDATA * gdata, u16 zone_id);       // event_mapchange.c
extern void FIELD_FLAGCONT_INIT_Continue(GAMEDATA * gdata, u16 zone_id);       // event_mapchange.c
extern void FIELD_FLAGCONT_INIT_FieldIn(GAMEDATA * gdata, u16 zone_id);


#ifdef _cplusplus
}	// extern "C"{
#endif



