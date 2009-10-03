//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		weather.h
 *	@brief		�V�C�V�X�e��
 *	@author		tomoya takahashi
 *	@date		2009.03.17
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifdef _cplusplus
extern "C"{
#endif

#ifndef __WEATHER_H__
#define __WEATHER_H__

#include "field/weather_no.h"

#include "field_light.h"
#include "field_fog.h"
#include "field_zonefog.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�V�C���Ȃ�
//=====================================
#define FIELD_WEATHER_NO_NONE	( 0xffff )

//-------------------------------------
///	�V�C�Z�����j�b�g�D�揇��
//=====================================
#define FIELD_WEATHER_CLUNIT_PRI	( 8 )

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�V�C�V�X�e�����[�N
//=====================================
typedef struct _FIELD_WEATHER FIELD_WEATHER;



//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	�V�C�V�X�e��	�������j��
//=====================================
extern FIELD_WEATHER* FIELD_WEATHER_Init( const FIELD_CAMERA* cp_camera, FIELD_LIGHT* p_light, FIELD_FOG_WORK* p_fog, const FIELD_ZONEFOGLIGHT* cp_zonefog, u32 heapID );
extern void FIELD_WEATHER_Exit( FIELD_WEATHER* p_sys );
extern void FIELD_WEATHER_Main( FIELD_WEATHER* p_sys, u32 heapID );
extern void FIELD_WEATHER_3DWrite( FIELD_WEATHER* p_sys );

//-------------------------------------
///	�V�C�V�X�e��	�Ǘ�
//=====================================
extern void FIELD_WEATHER_Set( FIELD_WEATHER* p_sys, u32 weather_no, u32 heapID );
extern void FIELD_WEATHER_Change( FIELD_WEATHER* p_sys, u32 weather_no );
extern u32 FIELD_WEATHER_GetWeatherNo( const FIELD_WEATHER* cp_sys );
extern u32 FIELD_WEATHER_GetNextWeatherNo( const FIELD_WEATHER* cp_sys );


#endif		// __WEATHER_H__

#ifdef _cplusplus
}
#endif

