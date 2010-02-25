//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   field_light_status.h
 *  @brief  �o�g���Ɉ����p���t�B�[���h���C�g�J���[
 *  @author tomoya takahashi
 *  @date   2010.01.29
 *
 *  ���W���[�����FFIELD_LIGHT_STATUS
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#include <gflib.h>

//-----------------------------------------------------------------------------
/**
 *          �萔�錾
*/
//-----------------------------------------------------------------------------
#define FIELD_LIGHT_STATUS_WEATHER_LIGHT_DAT_NONE ( 0xffff )

//-----------------------------------------------------------------------------
/**
 *          �\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
/// FIELD_LIGHT_STATUS
//=====================================
typedef struct {
  GXRgb   light;
  GXRgb   light1;
  GXRgb   light2;
  GXRgb   light3;
  GXRgb   diffuse;
  GXRgb   ambient;
  GXRgb   specular;
  GXRgb   emission;
  GXRgb   pad;      // padding
} FIELD_LIGHT_STATUS;

//-----------------------------------------------------------------------------
/**
 *          �v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

// ���C�g�J���[�̎擾
extern void FIELD_LIGHT_STATUS_Get( u32 zone_id, int hour, int minute, int weather_id, int season, FIELD_LIGHT_STATUS* p_status, HEAPID heapID );
// �V�CNO���烉�C�g�f�[�^�̃A�[�J�C�u�C���f�b�N�X�擾
extern u16 FIELD_LIGHT_STATUS_GetWeatherLightDatIdx( int weather_id );

#ifdef _cplusplus
} // extern "C"{
#endif



