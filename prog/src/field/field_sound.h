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
//--------------------------------------------------------------
/// �t�B�[���hBGM�ޔ���
//
// �t�B�[���hBGM�K�w
//                    ____ME(jingle
//          ____EVENT(event,trainer...
//  ____BASE(field
//--------------------------------------------------------------
typedef enum
{
  FSND_PUSHCOUNT_NONE = 0, ///<�ޔ��Ȃ�
  FSND_PUSHCOUNT_BASEBGM = 1, ///<�x�[�XBGM�ޔ�
  FSND_PUSHCOUNT_EVENTBGM, ///<�C�x���gBGM�ޔ�
  FSND_PUSHCOUNT_OVER, ///<�ޔ𐔃I�[�o�[
}FSND_PUSHCOUNT;

#define PLAY_NEXTBGM_FADEOUT_FRAME (60)
#define PLAY_NEXTBGM_FADEIN_FRAME (0)

//======================================================================
//  struct  
//======================================================================

//======================================================================
//  extern
//======================================================================
//�t�B�[���hBGM �Đ�
extern void FIELD_SOUND_PlayBGM( u32 bgmNo );
extern void FIELD_SOUND_PlayNextBGM_Ex( 
    FIELD_SOUND* fsnd, u32 bgmNo, u16 fadeOutFrame, u16 fadeInFrame );
#define FIELD_SOUND_PlayNextBGM( fsnd, bgmNo ) \
        FIELD_SOUND_PlayNextBGM_Ex( fsnd, bgmNo, \
            PLAY_NEXTBGM_FADEOUT_FRAME, PLAY_NEXTBGM_FADEIN_FRAME ) // ��L�ȈՔ�
extern void FIELD_SOUND_ChangePlayZoneBGM(
    FIELD_SOUND* fsnd, GAMEDATA* gdata, PLAYER_MOVE_FORM form, u32 zone_id );
extern void FIELD_SOUND_PushPlayEventBGM( FIELD_SOUND *fsnd, u32 bgmno );
extern void FIELD_SOUND_PushPlayJingleBGM( FIELD_SOUND *fsnd, u32 bgmno );
extern BOOL FIELD_SOUND_CanPlayBGM( const FIELD_SOUND *fsnd );

// �t�B�[���hBGM �t�F�[�h�Ǘ�
extern void FIELD_SOUND_UpdateBGM( FIELD_SOUND* fsnd );

//�t�B�[���hBGM �t�F�[�h
extern void FIELD_SOUND_FadeInBGM( FIELD_SOUND* fsnd, u32 bgmNo, u16 frames );
extern void FIELD_SOUND_FadeInPopBGM( FIELD_SOUND* fsnd, u16 frames );
extern void FIELD_SOUND_FadeOutBGM( FIELD_SOUND* fsnd, u16 frames );
extern void FIELD_SOUND_FadeOutPushBGM( FIELD_SOUND* fsnd, u16 frames );
extern BOOL FIELD_SOUND_IsBGMFade( FIELD_SOUND* fsnd );

#if 0
//�t�B�[���hBGM �g���b�N�֘A
extern void FIELD_SOUND_ChangeBGMTrackAction( void );
extern void FIELD_SOUND_ChangeBGMTrackStill( void );
extern void FIELD_SOUND_ChangeBGMVolume( int vol );
extern void FIELD_SOUND_ChangeBGMActionVolume( int vol );
#endif

//�t�B�[���hBGM BGM�i���o�[
extern u32 FIELD_SOUND_GetFieldBGMNo(
    GAMEDATA *gdata, PLAYER_MOVE_FORM form, u32 zone_id );

//�g���[�i�[����BGM�i���o�[
extern u32 FIELD_SOUND_GetTrainerEyeBgmNo( u32 trtype );

//�t�B�[���hBGM BGM�ޔ��A���A
extern FSND_PUSHCOUNT FIELD_SOUND_GetBGMPushCount( const FIELD_SOUND *fsnd );
extern void FIELD_SOUND_PushBGM( FIELD_SOUND *fsnd );
extern void FIELD_SOUND_PopBGM( FIELD_SOUND *fsnd );
extern void FIELD_SOUND_ForcePopBGM( FIELD_SOUND *fsnd );
