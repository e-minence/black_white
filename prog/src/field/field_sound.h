//======================================================================
/**
 * @file	field_sound.c
 * @brief	�t�B�[���h�̃T�E���h�֘A
 * @author	kagaya
 * @date	05.07.13
 */
//======================================================================
#pragma once
#include <gflib.h>
#include "system/gfl_use.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_data.h"

#include "sound/pm_sndsys.h"

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
extern void FIELD_SOUND_PlayBGM( u32 bgmNo );
extern void FIELD_SOUND_PlayNextBGM( u32 bgmNo );
extern void FIELD_SOUND_ChangePlayZoneBGM(
    GAMEDATA *gdata, PLAYER_MOVE_FORM form, u32 zone_id );
extern void FIELD_SOUND_PushPlayEventBGM( FIELD_SOUND *fsnd, u32 bgmno );
extern void FIELD_SOUND_PushPlayJingleBGM( FIELD_SOUND *fsnd, u32 bgmno );

//�t�B�[���hBGM �t�F�[�h
extern void FIELD_SOUND_FadeInBGM( u16 frames );
extern void FIELD_SOUND_FadeOutBGM( u16 frames );
extern BOOL FIELD_SOUND_IsBGMFade();

//�t�B�[���hBGM �g���b�N�֘A
extern void FIELD_SOUND_ChangeBGMTrackAction( void );
extern void FIELD_SOUND_ChangeBGMTrackStill( void );
extern void FIELD_SOUND_ChangeBGMVolume( int vol );
extern void FIELD_SOUND_ChangeBGMActionVolume( int vol );

//�t�B�[���hBGM BGM�i���o�[
extern u32 FIELD_SOUND_GetFieldBGMNo(
    GAMEDATA *gdata, PLAYER_MOVE_FORM form, u32 zone_id );

//�g���[�i�[����BGM�i���o�[
extern u32 FIELD_SOUND_GetTrainerEyeBgmNo( u32 trtype );

//�t�B�[���hBGM BGM�ޔ��A���A
extern void FIELD_SOUND_PushBGM( FIELD_SOUND *fsnd );
extern void FIELD_SOUND_PopBGM( FIELD_SOUND *fsnd );
extern void FIELD_SOUND_ForcePopBGM( FIELD_SOUND *fsnd );
