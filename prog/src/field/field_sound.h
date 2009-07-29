//======================================================================
/**
 * @file	field_sound.c
 * @brief	�t�B�[���h�̃T�E���h�֘A
 * @author	kagaya
 * @data	05.07.13
 */
//======================================================================
#pragma once
#include <gflib.h>
#include "system/gfl_use.h"

#include "sound/pm_sndsys.h"
#include "sound/wb_sound_data.sadl"

#include "field/fieldmap_proc.h"

#include "field_sound_proc.h"

//======================================================================
//  define
//======================================================================

//======================================================================
//  struct  
//======================================================================

//======================================================================
//  extern
//======================================================================

//�t�B�[���hBGM �Đ�
extern void FIELD_SOUND_PlayNextBGM( u32 bgmNo );

//�t�B�[���hBGM �g���b�N�֘A
extern void FIELD_SOUND_ChgangeBGMTrackAction( void );
extern void FIELD_SOUND_ChangeBGMTrackStill( void );
extern void FIELD_SOUND_ChangeBGMVolume( int vol );
extern void FIELD_SOUND_ChangeBGMActionVolume( int vol );

//�t�B�[���hBGM BGM�i���o�[
extern u32 FIELD_SOUND_GetFieldBGMNo(
    FIELDMAP_WORK *fieldMap, u32 zone_id );

//�t�B�[���hBGM BGM�ޔ��A���A
extern void FIELD_SOUND_PushBGM( FIELD_SOUND *fsnd );
extern void FIELD_SOUND_PopBGM( FIELD_SOUND *fsnd );
