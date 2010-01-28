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
#include "gamesystem/gamesystem.h"    // for GAMESYS_WORK
#include "gamesystem/gamedata_def.h"  // for GAMEDATA
#include "gamesystem/game_event.h"    // for GMEVENT
#include "gamesystem/iss_sys.h"       // for ISS_SYS
#include "field_sound_proc.h"         // for FIELD_SOUND


//=================================================================================
// ���萔
//================================================================================= 
// �t�F�[�h �t���[����
#define FSND_FADE_NONE    (0)  // ��
#define FSND_FADE_SHORT  (18)  // �Z 
#define FSND_FADE_NORMAL (60)  // ��
#define FSND_FADE_LONG   (90)  // ��


//=================================================================================
// ��BGM�̑���
//=================================================================================
extern GMEVENT* EVENT_FSND_ForcePlayBGM( GAMESYS_WORK* gameSystem, u16 soundIdx );
extern GMEVENT* EVENT_FSND_FadeInBGM( GAMESYS_WORK* gameSystem, u16 fadeInFrame );
extern GMEVENT* EVENT_FSND_FadeOutBGM( GAMESYS_WORK* gameSystem, u16 fadeOutFrame );
extern GMEVENT* EVENT_FSND_PushBGM( GAMESYS_WORK* gameSystem, u16 fadeOutFrame );
extern GMEVENT* EVENT_FSND_PopBGM( GAMESYS_WORK* gameSystem, 
                                   u16 fadeOutFrame, u16 fadeInFrame );
extern GMEVENT* EVENT_FSND_AllPopBGM( GAMESYS_WORK* gameSystem, u16 fadeInFrame );
extern GMEVENT* EVENT_FSND_PushPlayNextBGM( GAMESYS_WORK* gameSystem, u32 soundIdx, 
                                            u16 fadeOutFrame, u16 fadeInFrame );
extern GMEVENT* EVENT_FSND_PushPlayJingleBGM( GAMESYS_WORK* gameSystem, u32 soundIdx );
extern GMEVENT* EVENT_FSND_ChangeBGM( GAMESYS_WORK* gameSystem, u32 soundIdx, 
                                      u16 fadeOutFrame, u16 fadeInFrame );

// �x�[�XBGM��ޔ���, �퓬�Ȃ��Đ�����
extern GMEVENT* EVENT_FSND_PushPlayBattleBGM( GAMESYS_WORK* gameSystem, u32 soundIdx );

// �t�F�[�h����̊����҂�
extern GMEVENT* EVENT_FSND_WaitBGMFade( GAMESYS_WORK* gameSystem );


//=================================================================================
// ���v���C���[�̑���ɂ�蔭������BGM�̕ύX
//=================================================================================
// �]�[�� �`�F���W
extern void FSND_ChangeBGM_byZoneChange( FIELD_SOUND* fieldSound, GAMEDATA* gameData, 
                                         u16 prevZoneID, u16 nextZoneID ); 

// �}�b�v �`�F���W
extern void FSND_StandByNextMapBGM( FIELD_SOUND* fieldSound, GAMEDATA* gameData, 
                                    u16 prevZoneID, u16 nextZoneID );

extern void FSND_PlayStartBGM( FIELD_SOUND* fieldSound );

// ���@�ړ��t�H�[���ύX
extern void FSND_ChangeBGM_byPlayerFormChange( FIELD_SOUND* fieldSound, 
                                               GAMEDATA* gameData, u16 zoneID ); 


//=================================================================================
// ��BGM�{�����[������
//=================================================================================
// �t�B�[���h �� �A�v�� �J�ڎ�
// ��ISS�ƃv���C���[�{�����[���𑀍삵�܂��B
extern void FSND_HoldBGMVolume_forApp   ( FIELD_SOUND* fieldSound, ISS_SYS* iss );
extern void FSND_ReleaseBGMVolume_forApp( FIELD_SOUND* fieldSound, ISS_SYS* iss );

// �A�v�����ł̑���
// ���v���C���[�{�����[���݂̂𑀍삵�܂��B
extern void FSND_HoldBGMVolume_inApp   ( FIELD_SOUND* fieldSound );
extern void FSND_ReleaseBGMVolume_inApp( FIELD_SOUND* fieldSound );


//=================================================================================
// ��BGM No.�̎擾
//=================================================================================
// �w�肵���t�B�[���h��ōĐ����ׂ� BGM No.
extern u32 FSND_GetFieldBGM( GAMEDATA* gdata, u32 zoneID ); 

// �g���[�i�[���� BGM No.
extern u32 FSND_GetTrainerEyeBGM( u32 trType ); 
