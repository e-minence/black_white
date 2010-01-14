///////////////////////////////////////////////////////////////////////////////////
/**
 * @brief �t�B�[���h�̃T�E���h����
 * @file  field_sound.h
 * @author obata
 * @date   2009.12.25
 */
///////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "gflib.h"
#include "gamesystem/gamedata_def.h"  // for GAMEDATA
#include "gamesystem/game_event.h"
#include "field_sound_proc.h"


//=================================================================================
// ���萔
//=================================================================================
// BGM �ޔ���
typedef enum {
  FSND_PUSHCOUNT_NONE,   // �ޔ��Ȃ�
  FSND_PUSHCOUNT_BASE,   // �x�[�X BGM �ޔ�
  FSND_PUSHCOUNT_EVENT,  // �C�x���g BGM �ޔ�
  FSND_PUSHCOUNT_OVER,   // �ޔ𐔃I�[�o�[
  FSND_PUSHCOUNT_MAX = FSND_PUSHCOUNT_OVER-1  // �ő�ޔ�
} FSND_PUSHCOUNT;

// �t�F�[�h�C�����x
typedef enum{
  FSND_FADEIN_NONE,    // �t�F�[�h�Ȃ�
  FSND_FADEIN_FAST,    // �Z
  FSND_FADEIN_NORMAL,  // ��
  FSND_FADEIN_SLOW,    // ��
  FSND_FADEIN_SPEED_NUM
} FSND_FADEIN_SPEED;

// �t�F�[�h�A�E�g���x
typedef enum{
  FSND_FADEOUT_NONE,    // �t�F�[�h�Ȃ�
  FSND_FADEOUT_FAST,    // �Z
  FSND_FADEOUT_NORMAL,  // ��
  FSND_FADEOUT_SLOW,    // ��
  FSND_FADEOUT_SPEED_NUM
} FSND_FADEOUT_SPEED;
  

//=================================================================================
// ��BGM ����C�x���g
//=================================================================================
// �����Đ�
extern GMEVENT* EVENT_FieldSound_ForcePlayBGM( GAMESYS_WORK* gameSystem, u32 soundIdx );

// �t�F�[�h�C�� / �t�F�[�h�A�E�g
extern GMEVENT* EVENT_FieldSound_FadeInBGM( GAMESYS_WORK* gameSystem, 
                                            FSND_FADEIN_SPEED fadeInSpeed );
extern GMEVENT* EVENT_FieldSound_FadeOutBGM( GAMESYS_WORK* gameSystem, 
                                             FSND_FADEOUT_SPEED fadeOutSpeed );

// �ޔ� / ���A
extern GMEVENT* EVENT_FieldSound_PushBGM( GAMESYS_WORK* gameSystem, 
                                          FSND_FADEOUT_SPEED fadeOutSpeed );
extern GMEVENT* EVENT_FieldSound_PopBGM( GAMESYS_WORK* gameSystem, 
                                         FSND_FADEOUT_SPEED fadeOutSpeed, 
                                         FSND_FADEIN_SPEED fadeInSpeed );
extern GMEVENT* EVENT_FieldSound_AllPopBGM( GAMESYS_WORK* gameSystem, 
                                            FSND_FADEIN_SPEED fadeInSpeed );

// �Đ�����BGM��ޔ���, ����BGM��炷
extern GMEVENT* EVENT_FieldSound_PushPlayEventBGM( GAMESYS_WORK* gameSystem, u32 soundIdx );

// ME �Đ�
extern GMEVENT* EVENT_FieldSound_PushPlayJingleBGM( GAMESYS_WORK* gameSystem, u32 soundIdx ); 

// �C�x���gBGM / �t�B�[���hBGM �Đ�
extern GMEVENT* EVENT_FieldSound_PlayEventBGM( GAMESYS_WORK* gameSystem, u32 soundIdx );
extern GMEVENT* EVENT_FieldSound_PlayFieldBGM( GAMESYS_WORK* gameSystem, u16 zoneID );

// BGM �ύX
extern GMEVENT* EVENT_FieldSound_ChangeFieldBGM( GAMESYS_WORK* gameSystem, u16 zoneID );
extern GMEVENT* EVENT_FieldSound_StandByFieldBGM( GAMESYS_WORK* gameSystem, u16 zoneID );
extern GMEVENT* EVENT_FieldSound_PlayStartFieldBGM( GAMESYS_WORK* gameSystem );


//=================================================================================
// ��BGM �ύX���N�G�X�g
//=================================================================================
extern BOOL FIELD_SOUND_BGMChangeRequest( FIELD_SOUND* fieldSound, 
                                          u32 soundIdx, 
                                          FSND_FADEOUT_SPEED fadeOutSpeed, 
                                          FSND_FADEIN_SPEED fadeInSpeed );

extern BOOL FIELD_SOUND_FieldBGMChangeRequest( FIELD_SOUND* fieldSound,
                                               GAMEDATA* gameData, u16 zoneID );


//=================================================================================
// ��BGM �Ǘ�
//=================================================================================
extern void FIELD_SOUND_Main( FIELD_SOUND* fieldSound );


//=================================================================================
// ���擾
//=================================================================================
// �t�B�[���h BGM No.
extern u32 FIELD_SOUND_GetFieldBGM( GAMEDATA* gdata, u32 zoneID ); 

// �g���[�i�[���� BGM No.
extern u32 FIELD_SOUND_GetTrainerEyeBGM( u32 trType );

// BGM�ޔ𐔂̎擾
extern FSND_PUSHCOUNT FIELD_SOUND_GetBGMPushCount( const FIELD_SOUND* fieldSound );

// �t�F�[�h�����ǂ���
extern BOOL FIELD_SOUND_IsBGMFade( const FIELD_SOUND* fieldSound );
