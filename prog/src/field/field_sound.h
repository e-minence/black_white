///////////////////////////////////////////////////////////////////////////////////
/**
 * @brief �t�B�[���h�̃T�E���h���� ( WB�����@�\ )
 * @file   field_sound.h
 * @author obata
 * @date   2010.01.15
 */
///////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "gflib.h"
#include "sound/pm_sndsys.h"          // for PMSND_xxxx
#include "gamesystem/gamesystem.h"    // for GAMESYS_WORK
#include "gamesystem/gamedata_def.h"  // for GAMEDATA
#include "gamesystem/game_event.h"    // for GMEVENT
#include "gamesystem/iss_sys.h"       // for ISS_SYS
#include "field_sound_proc.h"         // for FIELD_SOUND
#include "field_sound_system.h"       // for FIELD_SOUND_xxxx


//=================================================================================
// ���萔
//================================================================================= 
#define MAX_VOLUME       (127)         // �ő�{�����[��
#define APP_HOLD_VOLUME  (64)          // �A�v�����̃{�����[��
#define APP_FADE_FRAME   (6)           // �A�v�����̃t�F�[�h �t���[����

// �t�F�[�h �t���[����
#define FSND_FADE_NONE   (0)                 // ��
#define FSND_FADE_FAST   (PMSND_FADE_FAST)   // �Z
#define FSND_FADE_SHORT  (PMSND_FADE_SHORT)  // ���Z
#define FSND_FADE_NORMAL (PMSND_FADE_NORMAL) // ��
#define FSND_FADE_LONG   (PMSND_FADE_LONG)   // ��


//=================================================================================
// ��BGM�̑���
//=================================================================================
extern GMEVENT* EVENT_FSND_ForcePlayBGM( GAMESYS_WORK* gameSystem, u16 soundIdx );
extern GMEVENT* EVENT_FSND_FadeInBGM( GAMESYS_WORK* gameSystem, u16 fadeInFrame );
extern GMEVENT* EVENT_FSND_FadeOutBGM( GAMESYS_WORK* gameSystem, u16 fadeOutFrame );
extern GMEVENT* EVENT_FSND_PushBGM( GAMESYS_WORK* gameSystem, u16 fadeOutFrame );
extern GMEVENT* EVENT_FSND_PopBGM( GAMESYS_WORK* gameSystem, u16 fadeOutFrame, u16 fadeInFrame );
extern GMEVENT* EVENT_FSND_AllPopBGM( GAMESYS_WORK* gameSystem, u16 fadeInFrame );
extern GMEVENT* EVENT_FSND_PushPlayNextBGM( GAMESYS_WORK* gameSystem, u32 soundIdx, u16 fadeOutFrame, u16 fadeInFrame );
extern GMEVENT* EVENT_FSND_PushPlayJingleBGM( GAMESYS_WORK* gameSystem, u32 soundIdx );
extern GMEVENT* EVENT_FSND_ChangeBGM( GAMESYS_WORK* gameSystem, u32 soundIdx, u16 fadeOutFrame, u16 fadeInFrame );
extern GMEVENT* EVENT_FSND_ResetBGM( GAMESYS_WORK* gameSystem, u16 fadeOutFrame );

// �x�[�XBGM��ޔ���, �퓬�Ȃ��Đ�����
extern GMEVENT* EVENT_FSND_PushPlayBattleBGM( GAMESYS_WORK* gameSystem, u32 soundIdx ); 
// �퓬�Ȃ���x�[�XBGM�ɕ��A����
extern GMEVENT* EVENT_FSND_PopPlayBGM_fromBattle( GAMESYS_WORK* gameSystem );

// �t�F�[�h����̊����҂�
extern GMEVENT* EVENT_FSND_WaitBGMFade( GAMESYS_WORK* gameSystem ); 
// BGM���A�҂�
extern GMEVENT* EVENT_FSND_WaitBGMPop( GAMESYS_WORK* gameSystem );

// �C�x���g�ȁE�����ȗp �����Đ��C�x���g
extern GMEVENT* EVENT_FSND_PlayEventBGM( GAMESYS_WORK* gameSystem, u32 soundIdx );
extern GMEVENT* EVENT_FSND_PlayEventBGMEx( GAMESYS_WORK* gameSystem, u32 soundIdx, u32 fadeOutFrame );
extern GMEVENT* EVENT_FSND_PushPlayEventBGM( GAMESYS_WORK* gameSystem, u32 soundIdx );
extern GMEVENT* EVENT_FSND_PlayTrainerEyeBGM( GAMESYS_WORK* gameSystem, u32 soundIdx );


//=================================================================================
// ���v���C���[�̑���ɂ�蔭������BGM�̕ύX
//=================================================================================
// �]�[�� �`�F���W
extern void FSND_ChangeBGM_byZoneChange( FIELD_SOUND* fieldSound, GAMEDATA* gameData, u16 nextZoneID ); 

// �}�b�v �`�F���W
extern void FSND_StandByNextMapBGM( FIELD_SOUND* fieldSound, GAMEDATA* gameData, u16 prevZoneID, u16 nextZoneID );
extern void FSND_PlayStartBGM( FIELD_SOUND* fieldSound, GAMEDATA* gameData, u16 zoneID );

// ���@�ړ��t�H�[���ύX
extern void FSND_ChangeBGM_byPlayerFormChange( FIELD_SOUND* fieldSound, GAMEDATA* gameData, u16 zoneID ); 


//=================================================================================
// ��BGM�{�����[������
//=================================================================================
// �t�B�[���h �� �A�v�� �J�ڎ�
// ��ISS�ƃv���C���[�{�����[���𑀍삵�܂��B
extern void FSND_HoldBGMVolume_forApp( FIELD_SOUND* fieldSound, ISS_SYS* iss );
extern void FSND_ReleaseBGMVolume_fromApp( FIELD_SOUND* fieldSound, ISS_SYS* iss );

// �A�v�����ł̑���
// ���v���C���[�{�����[���݂̂𑀍삵�܂��B
inline void FSND_HoldBGMVolume_inApp( FIELD_SOUND* fieldSound )
{
  FIELD_SOUND_ChangePlayerVolume( fieldSound, APP_HOLD_VOLUME, 0 );  
}
inline void FSND_ReleaseBGMVolume_inApp( FIELD_SOUND* fieldSound )
{
  FIELD_SOUND_ChangePlayerVolume( fieldSound, MAX_VOLUME, 0 );  
}


//=================================================================================
// ��BGM No.�̎擾
//=================================================================================
// �w�肵���t�B�[���h��ōĐ����ׂ� BGM No.
extern u32 FSND_GetFieldBGM( GAMEDATA* gdata, u32 zoneID ); 

// �g���[�i�[���� BGM No.
extern u32 FSND_GetTrainerEyeBGM( u32 trType ); 


//=================================================================================
// ������
//=================================================================================
// �����̍Đ�
inline void FSND_PlayEnvSE( FIELD_SOUND* fieldSound, u32 soundIdx )
{
  FIELD_SOUND_PlayEnvSE( fieldSound, soundIdx );
}
inline void FSND_PlayEnvSEVol( FIELD_SOUND* fieldSound, u32 soundIdx, u32 vol )
{
  FIELD_SOUND_PlayEnvSEVol( fieldSound, soundIdx, vol );
}
// �����̃{�����[������
inline void FSND_SetEnvSEVol( FIELD_SOUND* fieldSound, u32 soundIdx, u32 vol )
{
  FIELD_SOUND_SetEnvSEVol( fieldSound, soundIdx, vol );
}
// �����̒�~
inline void FSND_StopEnvSE( FIELD_SOUND* fieldSound, u32 soundIdx )
{
  FIELD_SOUND_StopEnvSE( fieldSound, soundIdx );
}
// �����̈ꎞ��~
inline void FSND_PauseEnvSE( FIELD_SOUND* fieldSound )
{
  FIELD_SOUND_PauseEnvSE( fieldSound );
}
// �����̍Ďn��
inline void FSND_RePlayEnvSE( FIELD_SOUND* fieldSound )
{
  FIELD_SOUND_RePlayEnvSE( fieldSound );
}

//=================================================================================
// ��TV�g�����V�[�o�[���M��
//=================================================================================
inline void FSND_RequestTVTRingTone( FIELD_SOUND* fieldSound )
{
  FIELD_SOUND_PlayTVTRingTone( fieldSound );
}
inline void FSND_StopTVTRingTone( FIELD_SOUND* fieldSound )
{
  FIELD_SOUND_StopTVTRingTone( fieldSound );
} 
