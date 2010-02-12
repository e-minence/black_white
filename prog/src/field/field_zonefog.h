//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_zonefog.h
 *	@brief	�]�[���p�t�H�O�E���C�g���
 *	@author	tomoya takahashi
 *	@date		2009.07.09
 *
 *	���W���[�����FFIELD_ZONEFOGLIGHT
 *
 *	���̃��W���[���Ƃ̊֌W
 *			FIELD_WEATHER
 *			WEATHER��SUNNY�֐����ŁAFOG���𔽉f���Ă��܂��B
 *			(weather_sunny.c)
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#include "arc_def.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
#define FIELD_ZONEFOGLIGHT_DATA_NONE	(0xfffffff)
#define FIELD_ZONEFOGLIGHT_ARC_LIGHT	( ARCID_ZONELIGHT_TABLE )

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	FIELD_ZONEFOGLIGHT�V�X�e��
//=====================================
typedef struct _FIELD_ZONEFOGLIGHT	FIELD_ZONEFOGLIGHT;


//-------------------------------------
///	�Q��FOG�f�[�^
//=====================================
typedef struct {
  u16 zone_id;
  u16 data_id;
} ZONE_FOG_DATA;

//-------------------------------------
///	�Q��LIGHT�f�[�^
//=====================================
typedef struct {
  u16 zone_id;
  u16 data_id;
} ZONE_LIGHT_DATA;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

extern FIELD_ZONEFOGLIGHT* FIELD_ZONEFOGLIGHT_Create( HEAPID heapID );
extern void FIELD_ZONEFOGLIGHT_Delete( FIELD_ZONEFOGLIGHT* p_sys );

extern void FIELD_ZONEFOGLIGHT_Load( FIELD_ZONEFOGLIGHT* p_sys, u32 fogno, u32 lightno, HEAPID heapID );
extern void FIELD_ZONEFOGLIGHT_Clear( FIELD_ZONEFOGLIGHT* p_sys );

extern BOOL FIELD_ZONEFOGLIGHT_IsFogData( const FIELD_ZONEFOGLIGHT* cp_sys );
extern BOOL FIELD_ZONEFOGLIGHT_IsLightData( const FIELD_ZONEFOGLIGHT* cp_sys );
extern s32 FIELD_ZONEFOGLIGHT_GetOffset( const FIELD_ZONEFOGLIGHT* cp_sys );
extern u32 FIELD_ZONEFOGLIGHT_GetSlope( const FIELD_ZONEFOGLIGHT* cp_sys );
extern u32 FIELD_ZONEFOGLIGHT_GetLightIndex( const FIELD_ZONEFOGLIGHT* cp_sys );

#ifdef _cplusplus
}	// extern "C"{
#endif



