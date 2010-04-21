///////////////////////////////////////////////////////////////////////////////////
/**
 * @brief �t�B�[���h�̃T�E���h���� ( WB�����@�\ )
 * @file   field_sound.c
 * @author obata
 * @date   2010.01.15
 */
///////////////////////////////////////////////////////////////////////////////////
#include "field_sound_system.h"
#include "field_sound.h"
#include "sound/wb_sound_data.sadl"
#include "gamesystem/playerwork.h"
#include "gamesystem/game_data.h"
#include "field/zonedata.h"
#include "tr_tool/trtype_def.h"
#include "../../../resource/trtype_bgm/trtype_bgm.cdat" 
#include "../../../resource/fldmapdata/special_bgm/special_bgm.cdat"


//================================================================================= 
// ���萔
//================================================================================= 
#define SPECIAL_BGM_NONE (0xffffffff)  // ����BGM�Ȃ�
#define MAX_VOLUME       (127)         // �ő�{�����[��
#define APP_HOLD_VOLUME  (64)          // �A�v�����̃{�����[��
#define APP_FADE_FRAME   (6)           // �A�v�����̃t�F�[�h �t���[����


//================================================================================= 
// ������J�֐�
//================================================================================= 
// ���@�t�H�[���̎擾
static PLAYER_MOVE_FORM GetPlayerMoveForm( GAMEDATA* gameData );
// BGM �ԍ��擾
static u32 GetZoneBGM   ( GAMEDATA* gameData, u32 zoneID );
static u32 GetSpecialBGM( GAMEDATA* gameData, u32 zoneID );
static u32 GetFieldBGM  ( GAMEDATA* gameData, u32 zoneID );


//=================================================================================
// ��BGM ����
//=================================================================================

//---------------------------------------------------------------------------------
/**
 * @brief �C�x���g���[�N
 */
//---------------------------------------------------------------------------------
typedef struct
{
  GAMESYS_WORK* gameSystem;
  FIELD_SOUND*  fieldSound;
  u32             soundIdx;  // �Đ�����BGM
  u16          fadeInFrame;  // �Đ�����BGM�̃t�F�[�h�C�� �t���[����
  u16         fadeOutFrame;  // �Đ�����BGM�̃t�F�[�h�A�E�g �t���[����

} FSND_EVWORK;


//---------------------------------------------------------------------------------
/**
 * @brief BGM �����Đ��C�x���g�����֐�
 */
//---------------------------------------------------------------------------------
static GMEVENT_RESULT ForcePlayBGMEvent( GMEVENT* event, int* seq, void* wk )
{
  FSND_EVWORK* work;
  FIELD_SOUND* fieldSound;

  work = (FSND_EVWORK*)wk;
  fieldSound = work->fieldSound;

  switch( *seq )
  {
  case 0:  // ���N�G�X�g���s
    FIELD_SOUND_RegisterRequest_FORCE_PLAY( fieldSound, work->soundIdx );
    (*seq)++;
    break;
  case 1:
    return GMEVENT_RES_FINISH; 
  }
  return GMEVENT_RES_CONTINUE;
} 
//---------------------------------------------------------------------------------
/**
 * @brief BGM �����Đ��C�x���g �擾
 *
 * @param gameSystem
 * @param soundIdx �Đ����� BGM
 * 
 * @return BGM �����Đ��C�x���g
 */
//---------------------------------------------------------------------------------
GMEVENT* EVENT_FSND_ForcePlayBGM( GAMESYS_WORK* gameSystem, u16 soundIdx )
{
  GMEVENT* event;
  FSND_EVWORK* work;
  GAMEDATA* gdata;

  gdata = GAMESYSTEM_GetGameData( gameSystem );

  event = GMEVENT_Create( gameSystem, NULL, ForcePlayBGMEvent, sizeof(FSND_EVWORK) );
  work = GMEVENT_GetEventWork( event );
  work->fieldSound = GAMEDATA_GetFieldSound( gdata );
  work->soundIdx = soundIdx;
  return event;
}

//---------------------------------------------------------------------------------
/**
 * @brief BGM �t�F�[�h�C�� �C�x���g�����֐�
 */
//---------------------------------------------------------------------------------
static GMEVENT_RESULT FadeInBGMEvent( GMEVENT* event, int* seq, void* wk )
{
  FSND_EVWORK* work;
  FIELD_SOUND* fieldSound;

  work = (FSND_EVWORK*)wk;
  fieldSound = work->fieldSound;

  switch( *seq )
  {
  case 0:  // ���N�G�X�g���s
    FIELD_SOUND_RegisterRequest_FADE_IN( fieldSound, work->fadeInFrame );
    (*seq)++;
    break;
  case 1:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
} 
//---------------------------------------------------------------------------------
/**
 * @brief BGM �t�F�[�h�C�� �C�x���g �擾
 *
 * @param gameSystem
 * @param fadeInFrame �t�F�[�h�C������
 * 
 * @return BGM �t�F�[�h�C�� �C�x���g
 */
//---------------------------------------------------------------------------------
GMEVENT* EVENT_FSND_FadeInBGM( GAMESYS_WORK* gameSystem, u16 fadeInFrame )
{
  GMEVENT* event;
  FSND_EVWORK* work;
  GAMEDATA* gdata;

  gdata = GAMESYSTEM_GetGameData( gameSystem );

  event = GMEVENT_Create( gameSystem, NULL, FadeInBGMEvent, sizeof(FSND_EVWORK) );
  work = GMEVENT_GetEventWork( event );
  work->fieldSound = GAMEDATA_GetFieldSound( gdata );
  work->fadeInFrame = fadeInFrame;
  return event;
}

//---------------------------------------------------------------------------------
/**
 * @brief BGM �t�F�[�h�A�E�g�����֐�
 */
//---------------------------------------------------------------------------------
static GMEVENT_RESULT FadeOutBGMEvent( GMEVENT* event, int* seq, void* wk )
{
  FSND_EVWORK* work;
  FIELD_SOUND* fieldSound;

  work = (FSND_EVWORK*)wk;
  fieldSound = work->fieldSound;

  switch( *seq )
  {
  case 0:  // ���N�G�X�g���s
    FIELD_SOUND_RegisterRequest_FADE_OUT( fieldSound, work->fadeOutFrame );
    (*seq)++;
    break;
  case 1:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
} 
//---------------------------------------------------------------------------------
/**
 * @brief BGM �t�F�[�h�A�E�g �C�x���g �擾
 *
 * @param gameSystem
 * @param fadeOutFrame �t�F�[�h�A�E�g����
 * 
 * @return BGM �t�F�[�h�A�E�g �C�x���g
 */
//---------------------------------------------------------------------------------
GMEVENT* EVENT_FSND_FadeOutBGM( GAMESYS_WORK* gameSystem, u16 fadeOutFrame )
{
  GMEVENT* event;
  FSND_EVWORK* work;
  GAMEDATA* gdata;

  gdata = GAMESYSTEM_GetGameData( gameSystem );

  event = GMEVENT_Create( gameSystem, NULL, FadeOutBGMEvent, sizeof(FSND_EVWORK) );
  work = GMEVENT_GetEventWork( event );
  work->fieldSound = GAMEDATA_GetFieldSound( gdata );
  work->fadeOutFrame = fadeOutFrame;
  return event;
}

//---------------------------------------------------------------------------------
/**
 * @brief BGM �ޔ��C�x���g�����֐�
 */
//---------------------------------------------------------------------------------
static GMEVENT_RESULT PushBGMEvent( GMEVENT* event, int* seq, void* wk )
{
  FSND_EVWORK* work;
  FIELD_SOUND* fieldSound;

  work = (FSND_EVWORK*)wk;
  fieldSound = work->fieldSound;

  switch( *seq )
  {
  case 0:  // ���N�G�X�g���s
    FIELD_SOUND_RegisterRequest_PUSH( fieldSound, work->fadeOutFrame );
    (*seq)++;
    break;
  case 1:  // �ޔ������҂�
    if( FIELD_SOUND_HaveRequest(fieldSound) == FALSE ){ (*seq)++; }
    break;
  case 2:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
} 
//---------------------------------------------------------------------------------
/**
 * @brief BGM �ޔ��C�x���g �擾
 *
 * @param gameSystem
 * @param fadeOutFrame �t�F�[�h�A�E�g����
 * 
 * @return BGM �ޔ��C�x���g
 */
//---------------------------------------------------------------------------------
GMEVENT* EVENT_FSND_PushBGM( GAMESYS_WORK* gameSystem, u16 fadeOutFrame )
{
  GMEVENT* event;
  FSND_EVWORK* work;
  GAMEDATA* gdata;

  gdata = GAMESYSTEM_GetGameData( gameSystem );

  event = GMEVENT_Create( gameSystem, NULL, PushBGMEvent, sizeof(FSND_EVWORK) );
  work = GMEVENT_GetEventWork( event );
  work->fieldSound = GAMEDATA_GetFieldSound( gdata );
  work->fadeOutFrame = fadeOutFrame;
  return event;
} 

//---------------------------------------------------------------------------------
/**
 * @brief BGM���A�C�x���g�����֐�
 */
//---------------------------------------------------------------------------------
static GMEVENT_RESULT PopBGMEvent( GMEVENT* event, int* seq, void* wk )
{
  FSND_EVWORK* work;
  FIELD_SOUND* fieldSound;

  work = (FSND_EVWORK*)wk;
  fieldSound = work->fieldSound;

  switch( *seq )
  {
  case 0:  // ���N�G�X�g���s
    FIELD_SOUND_RegisterRequest_POP( fieldSound, work->fadeOutFrame, work->fadeInFrame );
    (*seq)++;
    break;
  case 1:  // BGM���A�����҂�
    if( FIELD_SOUND_HaveRequest(fieldSound) == FALSE ){ (*seq)++; }
    break;
  case 2: 
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
} 
//---------------------------------------------------------------------------------
/**
 * @brief BGM ���A�C�x���g �擾
 *
 * @param gameSystem
 * @param fadeOutFrame �Đ�����BGM�̃t�F�[�h�A�E�g �t���[����
 * @param fadeInFrame  ���A����BGM�̃t�F�[�h�C�� �t���[����
 * 
 * @return BGM ���A�C�x���g
 */
//---------------------------------------------------------------------------------
GMEVENT* EVENT_FSND_PopBGM( GAMESYS_WORK* gameSystem, u16 fadeOutFrame, u16 fadeInFrame )
{
  GMEVENT* event;
  FSND_EVWORK* work;
  GAMEDATA* gdata;

  gdata = GAMESYSTEM_GetGameData( gameSystem );

  event = GMEVENT_Create( gameSystem, NULL, PopBGMEvent, sizeof(FSND_EVWORK) );
  work = GMEVENT_GetEventWork( event );
  work->fieldSound = GAMEDATA_GetFieldSound( gdata );
  work->fadeInFrame = fadeInFrame;
  work->fadeOutFrame = fadeOutFrame;
  return event;
}

//---------------------------------------------------------------------------------
/**
 * @brief BGM �S���A�C�x���g�����֐�
 */
//---------------------------------------------------------------------------------
static GMEVENT_RESULT AllPopBGMEvent( GMEVENT* event, int* seq, void* wk )
{
  FSND_EVWORK* work = (FSND_EVWORK*)wk;
  FIELD_SOUND* fieldSound = work->fieldSound;
  FSND_PUSHCOUNT pushCount;
  int i; 

  switch( *seq )
  {
  // �S�|�b�v
  case 0:
    // �|�b�v�����\��̂Ȃ�BGM�̐����擾
    pushCount = FIELD_SOUND_GetBGMPushCount_atAllRequestFinished( fieldSound );

    // �SBGM���|�b�v
    if( FSND_PUSHCOUNT_NONE < pushCount )
    {
      for( i=0; i<pushCount-1; i++ )
      { // �t�F�[�h�Ȃ��Ń|�b�v
        FIELD_SOUND_RegisterRequest_POP( fieldSound, FSND_FADE_NONE, FSND_FADE_NONE );
      }
      // �t�F�[�h����Ń|�b�v
      FIELD_SOUND_RegisterRequest_POP( fieldSound, FSND_FADE_NONE, work->fadeInFrame );
    }
    (*seq)++;
    break;

  // �C�x���g�I��
  case 1:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE; 
} 
//---------------------------------------------------------------------------------
/**
 * @brief BGM �S���A�C�x���g �擾
 *
 * @param gameSystem
 * @param fadeInFrame �t�F�[�h�C������
 * 
 * @return BGM �S���A�C�x���g
 */
//---------------------------------------------------------------------------------
GMEVENT* EVENT_FSND_AllPopBGM( GAMESYS_WORK* gameSystem, u16 fadeInFrame )
{
  GMEVENT* event;
  FSND_EVWORK* work;
  GAMEDATA* gdata;

  gdata = GAMESYSTEM_GetGameData( gameSystem );

  event = GMEVENT_Create( gameSystem, NULL, AllPopBGMEvent, sizeof(FSND_EVWORK) );
  work = GMEVENT_GetEventWork( event );
  work->fieldSound = GAMEDATA_GetFieldSound( gdata );
  work->fadeInFrame = fadeInFrame;
  return event;
} 

//---------------------------------------------------------------------------------
/**
 * @brief BGM �ޔ��E�Đ��C�x���g�����֐�
 */
//---------------------------------------------------------------------------------
static GMEVENT_RESULT PushPlayNextBGMEvent( GMEVENT* event, int* seq, void* wk )
{
  FSND_EVWORK* work;
  FIELD_SOUND* fieldSound;

  work = (FSND_EVWORK*)wk;
  fieldSound = work->fieldSound;

  switch( *seq )
  {
  case 0:  // ���N�G�X�g���s
    FIELD_SOUND_RegisterRequest_PUSH( fieldSound, work->fadeOutFrame );
    FIELD_SOUND_RegisterRequest_CHANGE( fieldSound, work->soundIdx, 
                                        FSND_FADE_NONE, work->fadeInFrame );
    (*seq)++;
    break;
  case 1:
    return GMEVENT_RES_FINISH;
  } 
  return GMEVENT_RES_CONTINUE;
}
//---------------------------------------------------------------------------------
/**
 * @brief BGM�ޔ��E�Đ��C�x���g�擾
 *
 * @param gameSystem
 * @param soundIdx     �Đ�����BGM
 * @param fadeOutFrame �Đ�����BGM�̃t�F�[�h�A�E�g �t���[����
 * @param fadeInFrame  ���A����BGM�̃t�F�[�h�C�� �t���[����
 * 
 * @return �ޔ��E�Đ��C�x���g
 */
//--------------------------------------------------------------------------------- 
GMEVENT* EVENT_FSND_PushPlayNextBGM( GAMESYS_WORK* gameSystem, u32 soundIdx,
                                     u16 fadeOutFrame, u16 fadeInFrame )
{
  GMEVENT* event;
  FSND_EVWORK* work;
  GAMEDATA* gdata;

  gdata = GAMESYSTEM_GetGameData( gameSystem );

  event = GMEVENT_Create( gameSystem, NULL, PushPlayNextBGMEvent, sizeof(FSND_EVWORK) );
  work = GMEVENT_GetEventWork( event );
  work->fieldSound = GAMEDATA_GetFieldSound( gdata );
  work->soundIdx = soundIdx;
  work->fadeInFrame = fadeInFrame;
  work->fadeOutFrame = fadeOutFrame;
  return event;
}

//---------------------------------------------------------------------------------
/**
 * @brief ME �Đ��C�x���g�����֐�
 */
//---------------------------------------------------------------------------------
static GMEVENT_RESULT PushPlayJingleBGMEvent( GMEVENT* event, int* seq, void* wk )
{
  FSND_EVWORK* work;
  FIELD_SOUND* fieldSound;

  work = (FSND_EVWORK*)wk;
  fieldSound = work->fieldSound;

  switch( *seq )
  {
  case 0:  // ���N�G�X�g���s
    FIELD_SOUND_RegisterRequest_PUSH( fieldSound, FSND_FADE_FAST );
    FIELD_SOUND_RegisterRequest_FORCE_PLAY( fieldSound, work->soundIdx );
    (*seq)++;
    break;
  case 1:
    return GMEVENT_RES_FINISH;
  } 
  return GMEVENT_RES_CONTINUE;
} 
//---------------------------------------------------------------------------------
/**
 * @brief ME �Đ��C�x���g �擾
 *
 * @param gameSystem
 * @param soundIdx �Đ����� ME
 * 
 * @return ME �Đ��C�x���g
 */
//---------------------------------------------------------------------------------
GMEVENT* EVENT_FSND_PushPlayJingleBGM( GAMESYS_WORK* gameSystem, u32 soundIdx )
{
  GMEVENT* event;
  FSND_EVWORK* work;
  GAMEDATA* gdata;

  gdata = GAMESYSTEM_GetGameData( gameSystem );

  event = GMEVENT_Create( gameSystem, NULL, PushPlayJingleBGMEvent, sizeof(FSND_EVWORK) );
  work = GMEVENT_GetEventWork( event );
  work->fieldSound = GAMEDATA_GetFieldSound( gdata );
  work->soundIdx = soundIdx;
  work->fadeOutFrame = FSND_FADE_NONE;
  return event;
} 

//---------------------------------------------------------------------------------
/**
 * @brief BGM �ύX�C�x���g�����֐�
 */
//---------------------------------------------------------------------------------
static GMEVENT_RESULT ChangeBGMEvent( GMEVENT* event, int* seq, void* wk )
{
  FSND_EVWORK* work;
  FIELD_SOUND* fieldSound;

  work = (FSND_EVWORK*)wk;
  fieldSound = work->fieldSound;

  switch( *seq )
  {
  case 0:  // ���N�G�X�g���s
    FIELD_SOUND_RegisterRequest_CHANGE( fieldSound, work->soundIdx, 
                                        work->fadeOutFrame, work->fadeInFrame );
    (*seq)++;
    break;
  case 1:
    return GMEVENT_RES_FINISH;
  } 
  return GMEVENT_RES_CONTINUE;
}
//---------------------------------------------------------------------------------
/**
 * @brief BGM �ύX�C�x���g�擾
 *
 * @param gameSystem
 * @param soundIdx �Đ����� ME
 * @param fadeOutFrame �Đ�����BGM�̃t�F�[�h�A�E�g �t���[����
 * @param fadeInFrame  ���A����BGM�̃t�F�[�h�C�� �t���[����
 * 
 * @return �ޔ��E�Đ��C�x���g
 */
//--------------------------------------------------------------------------------- 
GMEVENT* EVENT_FSND_ChangeBGM( GAMESYS_WORK* gameSystem, u32 soundIdx,
                               u16 fadeOutFrame, u16 fadeInFrame )
{
  GMEVENT* event;
  FSND_EVWORK* work;
  GAMEDATA* gdata;

  gdata = GAMESYSTEM_GetGameData( gameSystem );

  event = GMEVENT_Create( gameSystem, NULL, ChangeBGMEvent, sizeof(FSND_EVWORK) );
  work = GMEVENT_GetEventWork( event );
  work->fieldSound = GAMEDATA_GetFieldSound( gdata );
  work->soundIdx = soundIdx;
  work->fadeInFrame = fadeInFrame;
  work->fadeOutFrame = fadeOutFrame;
  return event;
}

//---------------------------------------------------------------------------------
/**
 * @brief BGM ���Z�b�g �C�x���g�����֐�
 */
//---------------------------------------------------------------------------------
static GMEVENT_RESULT ResetBGMEvent( GMEVENT* event, int* seq, void* wk )
{
  FSND_EVWORK* work       = (FSND_EVWORK*)wk;
  GAMEDATA*    gameData   = GAMESYSTEM_GetGameData( work->gameSystem );
  FIELD_SOUND* fieldSound = work->fieldSound;

  switch( *seq ) {
  // �t�F�[�h�A�E�g���N�G�X�g���s
  case 0:
    FIELD_SOUND_RegisterRequest_FADE_OUT( fieldSound, work->fadeOutFrame );
    (*seq)++;
    break;

  // �S���N�G�X�g�̏����҂�
  case 1:
    if( FIELD_SOUND_HaveRequest( fieldSound ) == FALSE ) { (*seq)++; }
    break;

  // FIELD_SOUND�����Z�b�g
  case 2:
    FIELD_SOUND_Reset( fieldSound, gameData );
    (*seq)++;
    break;

  // �C�x���g�I��
  case 3:
    return GMEVENT_RES_FINISH;
  } 
  return GMEVENT_RES_CONTINUE;
}

//---------------------------------------------------------------------------------
/**
 * @brief �C�x���gBGM�Đ��C�x���g�����֐�
 */
//---------------------------------------------------------------------------------
static GMEVENT_RESULT PlayEventBGMEvent( GMEVENT* event, int* seq, void* wk )
{
  FSND_EVWORK* work;
  FIELD_SOUND* fieldSound;

  work = (FSND_EVWORK*)wk;
  fieldSound = work->fieldSound;

  switch( *seq ) {
  case 0:  // ���N�G�X�g���s
    FIELD_SOUND_RegisterRequest_FADE_OUT( fieldSound, FSND_FADE_FAST );
    FIELD_SOUND_RegisterRequest_FORCE_PLAY( fieldSound, work->soundIdx );
    (*seq)++;
    break;
  case 1:
    // �����Đ��̊����҂�
    if( FIELD_SOUND_HaveRequest( fieldSound ) == FALSE ) {
      return GMEVENT_RES_FINISH;
    }
  } 
  return GMEVENT_RES_CONTINUE;
} 
//--------------------------------------------------------------------------------- 
/**
 * @brief �C�x���gBGM�̍Đ��C�x���g�𐶐�����
 *
 * @param gameSystem
 * @param soundIdx   �Đ�����BGM
 *
 * @return ���������C�x���g
 */
//--------------------------------------------------------------------------------- 
GMEVENT* EVENT_FSND_PlayEventBGM( GAMESYS_WORK* gameSystem, u32 soundIdx )
{
  GMEVENT* event;
  FSND_EVWORK* work;
  GAMEDATA* gdata;

  gdata = GAMESYSTEM_GetGameData( gameSystem );

  // �C�x���g�𐶐�
  event = GMEVENT_Create( gameSystem, NULL, PlayEventBGMEvent, sizeof(FSND_EVWORK) );

  // �C�x���g���[�N��������
  work = GMEVENT_GetEventWork( event );
  work->fieldSound = GAMEDATA_GetFieldSound( gdata );
  work->soundIdx = soundIdx;

  return event;
}

//---------------------------------------------------------------------------------
/**
 * @brief �C�x���gBGM�Đ��C�x���g�����֐� ( �t�B�[���h BGM �ޔ� ver. )
 */
//---------------------------------------------------------------------------------
static GMEVENT_RESULT PushPlayEventBGM( GMEVENT* event, int* seq, void* wk )
{
  FSND_EVWORK* work;
  FIELD_SOUND* fieldSound;

  work = (FSND_EVWORK*)wk;
  fieldSound = work->fieldSound;

  switch( *seq ) {
  case 0:  // ���N�G�X�g���s
    FIELD_SOUND_RegisterRequest_PUSH( fieldSound, FSND_FADE_FAST );
    FIELD_SOUND_RegisterRequest_FORCE_PLAY( fieldSound, work->soundIdx );
    (*seq)++;
    break;
  case 1:
    return GMEVENT_RES_FINISH;
  } 
  return GMEVENT_RES_CONTINUE;
} 
//--------------------------------------------------------------------------------- 
/**
 * @brief �C�x���gBGM�̍Đ��C�x���g�𐶐����� ( �t�B�[���h BGM �ޔ� ver. )
 *
 * @param gameSystem
 * @param soundIdx   �Đ�����BGM
 *
 * @return ���������C�x���g
 */
//--------------------------------------------------------------------------------- 
GMEVENT* EVENT_FSND_PushPlayEventBGM( GAMESYS_WORK* gameSystem, u32 soundIdx )
{
  GMEVENT* event;
  FSND_EVWORK* work;
  GAMEDATA* gdata;

  gdata = GAMESYSTEM_GetGameData( gameSystem );

  // �C�x���g�𐶐�
  event = GMEVENT_Create( gameSystem, NULL, PushPlayEventBGM, sizeof(FSND_EVWORK) );

  // �C�x���g���[�N��������
  work = GMEVENT_GetEventWork( event );
  work->fieldSound = GAMEDATA_GetFieldSound( gdata );
  work->soundIdx = soundIdx;

  return event;
}

//---------------------------------------------------------------------------------
/**
 * @brief �����ȍĐ��C�x���g�����֐�
 */
//---------------------------------------------------------------------------------
static GMEVENT_RESULT PlayTrainerEyeBGM( GMEVENT* event, int* seq, void* wk )
{
  FSND_EVWORK* work;
  FIELD_SOUND* fieldSound;

  work = (FSND_EVWORK*)wk;
  fieldSound = work->fieldSound;

  switch( *seq ) {
  case 0:  // ���N�G�X�g���s
    FIELD_SOUND_RegisterRequest_FADE_OUT( fieldSound, FSND_FADE_FAST );
    FIELD_SOUND_RegisterRequest_FORCE_PLAY( fieldSound, work->soundIdx );
    (*seq)++;
    break;
  case 1:
    return GMEVENT_RES_FINISH;
  } 
  return GMEVENT_RES_CONTINUE;
} 
//--------------------------------------------------------------------------------- 
/**
 * @brief �����ȍĐ��C�x���g�𐶐�����
 *
 * @param gameSystem
 * @param soundIdx   �Đ�����BGM
 *
 * @return ���������C�x���g
 */
//--------------------------------------------------------------------------------- 
GMEVENT* EVENT_FSND_PlayTrainerEyeBGM( GAMESYS_WORK* gameSystem, u32 soundIdx )
{
  GMEVENT* event;
  FSND_EVWORK* work;
  GAMEDATA* gdata;

  gdata = GAMESYSTEM_GetGameData( gameSystem );

  // �C�x���g�𐶐�
  event = GMEVENT_Create( gameSystem, NULL, PlayTrainerEyeBGM, sizeof(FSND_EVWORK) );

  // �C�x���g���[�N��������
  work = GMEVENT_GetEventWork( event );
  work->fieldSound = GAMEDATA_GetFieldSound( gdata );
  work->soundIdx = soundIdx;

  return event;
}

//--------------------------------------------------------------------------------- 
/**
 * @brief BGM���~��, �ޔ𒆂̑S�Ă�BGM��j������
 *
 * @param gameSystem
 * @param fadeOutFrame �Đ�����BGM�̃t�F�[�h�A�E�g �t���[����
 *
 * @return BGM���Z�b�g�C�x���g
 */
//--------------------------------------------------------------------------------- 
GMEVENT* EVENT_FSND_ResetBGM( GAMESYS_WORK* gameSystem, u16 fadeOutFrame )
{
  GMEVENT* event;
  FSND_EVWORK* work;
  GAMEDATA* gdata;

  gdata = GAMESYSTEM_GetGameData( gameSystem );

  // �C�x���g�𐶐�
  event = GMEVENT_Create( gameSystem, NULL, ResetBGMEvent, sizeof(FSND_EVWORK) );

  // �C�x���g���[�N��������
  work = GMEVENT_GetEventWork( event );
  work->gameSystem   = gameSystem;
  work->fieldSound   = GAMEDATA_GetFieldSound( gdata );
  work->fadeOutFrame = fadeOutFrame;

  return event;
}

//---------------------------------------------------------------------------------
/**
 * @brief �x�[�XBGM �ޔ��C�x���g�����֐�
 */
//---------------------------------------------------------------------------------
static GMEVENT_RESULT PushPlayBattleBGMEvent( GMEVENT* event, int* seq, void* wk )
{
  FSND_EVWORK* work;
  FIELD_SOUND* fieldSound;

  work = (FSND_EVWORK*)wk;
  fieldSound = work->fieldSound;

  switch( *seq ) {
  // ���N�G�X�g���s
  case 0:  
    if( FIELD_SOUND_GetBGMPushCount_atAllRequestFinished(fieldSound) == FSND_PUSHCOUNT_NONE ) { 
      // �x�[�XBGM�Đ���
      FIELD_SOUND_RegisterRequest_PUSH( fieldSound, FSND_FADE_FAST );
    }
    else { 
      // �����ȍĐ��� ( �x�[�XBGM�͂��łɑޔ��ς� )
      FIELD_SOUND_RegisterRequest_FADE_OUT( fieldSound, FSND_FADE_FAST );
    }
    (*seq)++;
    break;

  // BGM�ޔ� or �t�F�[�h�A�E�g�����҂�
  case 1:  
    if( FIELD_SOUND_HaveRequest(fieldSound) == FALSE ){ (*seq)++; }
    break;

  // �o�g��BGM�Đ�
  case 2:  
    FIELD_SOUND_RegisterRequest_FORCE_PLAY( fieldSound, work->soundIdx );
    (*seq)++;
    break;

  // �C�x���g�I��
  case 3:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
} 
//---------------------------------------------------------------------------------
/**
 * @brief �x�[�XBGM�̑ޔ�, �퓬BGM�Đ��C�x���g�擾
 *
 * @param gameSystem
 * @param soundIdx   �Đ�����퓬BGM
 * 
 * @return BGM �ޔ��C�x���g
 */
//---------------------------------------------------------------------------------
GMEVENT* EVENT_FSND_PushPlayBattleBGM( GAMESYS_WORK* gameSystem, u32 soundIdx )
{
  GMEVENT* event;
  FSND_EVWORK* work;
  GAMEDATA* gdata;

  gdata = GAMESYSTEM_GetGameData( gameSystem );

  event = GMEVENT_Create( gameSystem, NULL, PushPlayBattleBGMEvent, sizeof(FSND_EVWORK) );
  work = GMEVENT_GetEventWork( event );
  work->fieldSound = GAMEDATA_GetFieldSound( gdata );
  work->soundIdx = soundIdx;
  return event;
} 
//---------------------------------------------------------------------------------
/**
 * @brief �퓬�Ȃ����BGM���A�C�x���g�����֐�
 */
//---------------------------------------------------------------------------------
static GMEVENT_RESULT PopPlayBGM_fromBattleEvent( GMEVENT* event, int* seq, void* wk )
{
  FSND_EVWORK* work;
  FIELD_SOUND* fieldSound;

  work = (FSND_EVWORK*)wk;
  fieldSound = work->fieldSound;

  switch( *seq ) {
  case 0:  // ���N�G�X�g���s
    FIELD_SOUND_RegisterRequest_POP( fieldSound, work->fadeOutFrame, work->fadeInFrame );
    (*seq)++;
    break;
  case 1: 
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
} 
//---------------------------------------------------------------------------------
/**
 * @brief �퓬�Ȃ���x�[�XBGM�ɕ��A����C�x���g�𐶐�����
 *
 * @param gameSystem
 *
 * @return �퓬�Ȃ����BGM���A�C�x���g
 */
//---------------------------------------------------------------------------------
GMEVENT* EVENT_FSND_PopPlayBGM_fromBattle( GAMESYS_WORK* gameSystem )
{
  FSND_EVWORK* work;
  GAMEDATA*    gameData;
  GMEVENT*     event;

  gameData = GAMESYSTEM_GetGameData( gameSystem );

  // �C�x���g�𐶐�
  event = GMEVENT_Create( gameSystem, NULL, PopPlayBGM_fromBattleEvent, sizeof(FSND_EVWORK) );

  // �C�x���g���[�N��������
  work = GMEVENT_GetEventWork( event );
  work->fieldSound = GAMEDATA_GetFieldSound( gameData );
  work->fadeInFrame = FSND_FADE_NORMAL;
  work->fadeOutFrame = FSND_FADE_SHORT;
  return event;
} 
//---------------------------------------------------------------------------------
/**
 * @brief BGM �t�F�[�h�����҂��C�x���g
 */
//---------------------------------------------------------------------------------
static GMEVENT_RESULT WaitBGMFadeEvent( GMEVENT* event, int* seq, void* wk )
{
  FSND_EVWORK* work;
  FIELD_SOUND* fieldSound;

  work = (FSND_EVWORK*)wk;
  fieldSound = work->fieldSound;

  switch( *seq ) {
  case 0:  // �t�F�[�h�����҂�
    if( FIELD_SOUND_IsBGMFade(fieldSound) == FALSE ){ (*seq)++; }
    break;
  case 1:
    return GMEVENT_RES_FINISH;
  } 
  return GMEVENT_RES_CONTINUE;
}
//--------------------------------------------------------------------------------- 
/**
 * @brief BGM�t�F�[�h�����҂��C�x���g�擾
 *
 * @param gameSystem
 *
 * @return BGM�t�F�[�h�����҂��C�x���g
 */
//--------------------------------------------------------------------------------- 
GMEVENT* EVENT_FSND_WaitBGMFade( GAMESYS_WORK* gameSystem )
{
  GMEVENT* event;
  FSND_EVWORK* work;
  GAMEDATA* gdata;

  gdata = GAMESYSTEM_GetGameData( gameSystem );

  event = GMEVENT_Create( gameSystem, NULL, WaitBGMFadeEvent, sizeof(FSND_EVWORK) );
  work = GMEVENT_GetEventWork( event );
  work->fieldSound = GAMEDATA_GetFieldSound( gdata );
  return event;
} 

//---------------------------------------------------------------------------------
/**
 * @brief BGM���A�҂��C�x���g
 */
//---------------------------------------------------------------------------------
static GMEVENT_RESULT WaitBGMPopEvent( GMEVENT* event, int* seq, void* wk )
{
  FSND_EVWORK* work;
  FIELD_SOUND* fieldSound;

  work = (FSND_EVWORK*)wk;
  fieldSound = work->fieldSound;

  switch( *seq ) {
  // BGM��POP�����҂�
  case 0:
    if( FIELD_SOUND_CheckBGMPopFinished(fieldSound) == TRUE ) { (*seq)++; }
    break;

  // �C�x���g�I��
  case 1:
    return GMEVENT_RES_FINISH;
  } 
  return GMEVENT_RES_CONTINUE;
}
//--------------------------------------------------------------------------------- 
/**
 * @brief BGM���A�҂��C�x���g�𐶐�����
 *
 * @param gameSystem
 */
//--------------------------------------------------------------------------------- 
GMEVENT* EVENT_FSND_WaitBGMPop( GAMESYS_WORK* gameSystem )
{
  GMEVENT* event;
  FSND_EVWORK* work;
  GAMEDATA* gdata;

  gdata = GAMESYSTEM_GetGameData( gameSystem );

  event = GMEVENT_Create( gameSystem, NULL, WaitBGMPopEvent, sizeof(FSND_EVWORK) );
  work = GMEVENT_GetEventWork( event );
  work->fieldSound = GAMEDATA_GetFieldSound( gdata );
  return event;
}

//=================================================================================
// ���v���C���[�̑���ɂ�蔭������BGM�̕ύX
//=================================================================================

//---------------------------------------------------------------------------------
/**
 * @brief �]�[�� �`�F���W�ɂ��BGM�̕ύX
 *
 * @param fieldSound
 * @param gameData
 * @param nextZoneID �`�F���W��̃]�[��ID
 */
//---------------------------------------------------------------------------------
void FSND_ChangeBGM_byZoneChange( FIELD_SOUND* fieldSound, GAMEDATA* gameData, 
                                  u16 nextZoneID )
{
  u32 soundIdx;
  u16 fadeOutFrame, fadeInFrame;


  // �`�F���W��̃]�[����BGM���擾
  soundIdx = GetSpecialBGM( gameData, nextZoneID );
  if( soundIdx == SPECIAL_BGM_NONE ) {
    soundIdx = GetZoneBGM( gameData, nextZoneID ); 
  }

  // �t�F�[�h �t���[����������
  if( GetPlayerMoveForm(gameData) == PLAYER_MOVE_FORM_CYCLE )
  { // ���]��
    fadeOutFrame = FSND_FADE_NORMAL;
    fadeInFrame  = FSND_FADE_NORMAL;
  }
  else
  { // ����
    fadeOutFrame = FSND_FADE_NORMAL;
    fadeInFrame  = FSND_FADE_NONE;
  }

  // ���N�G�X�g�o�^
  FIELD_SOUND_RegisterRequest_CHANGE( fieldSound, soundIdx, fadeOutFrame, fadeInFrame );
}

//---------------------------------------------------------------------------------
/**
 * @brief �}�b�v �`�F���W����BGM�Đ�����
 *
 * @param fieldSound
 * @param gameData
 * @param prevZoneID �`�F���W�O�̃]�[��ID
 * @param nextZoneID �`�F���W��̃]�[��ID
 */
//---------------------------------------------------------------------------------
void FSND_StandByNextMapBGM( FIELD_SOUND* fieldSound, GAMEDATA* gameData, u16 nextZoneID )
{
  u32 soundIdx;

  // �O��̃]�[����BGM���擾
  soundIdx = GetSpecialBGM( gameData, nextZoneID );
  if( soundIdx == SPECIAL_BGM_NONE ) {
    soundIdx = GetZoneBGM( gameData, nextZoneID ); 
  }

  // ���N�G�X�g�o�^
  FIELD_SOUND_RegisterRequest_STAND_BY( fieldSound, soundIdx, FSND_FADE_NORMAL );
}

//---------------------------------------------------------------------------------
/**
 * @brief �}�b�v �`�F���W����BGM�Đ��J�n
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
void FSND_PlayStartBGM( FIELD_SOUND* fieldSound )
{
  // ���N�G�X�g�o�^
  FIELD_SOUND_RegisterRequest_FADE_IN( fieldSound, FSND_FADE_NONE );
}

//---------------------------------------------------------------------------------
/**
 * @brief ���@�ړ��t�H�[���̕ύX�ɂ��BGM�ύX
 *
 * @param fieldSound
 * @param gameData
 * @param zoneID      ���݂̃]�[��ID
 */
//---------------------------------------------------------------------------------
void FSND_ChangeBGM_byPlayerFormChange( FIELD_SOUND* fieldSound, 
                                        GAMEDATA* gameData, u16 zoneID )
{
  u32 soundIdx;

  soundIdx = GetFieldBGM( gameData, zoneID );

  // ���N�G�X�g�o�^
  FIELD_SOUND_RegisterRequest_CHANGE( fieldSound, soundIdx, FSND_FADE_SHORT, FSND_FADE_NONE );
}


//=================================================================================
// ��BGM�{�����[������
//=================================================================================

//---------------------------------------------------------------------------------
/**
 * @brief �t�B�[���h �� �A�v�� �J�ڎ��� BGM�{�����[���}������
 *
 * @param fieldSound
 * @parma iss
 */
//---------------------------------------------------------------------------------
void FSND_HoldBGMVolume_forApp( FIELD_SOUND* fieldSound, ISS_SYS* iss )
{
  ISS_3DS_SYS* iss3DSSystem;

  iss3DSSystem = ISS_SYS_GetIss3DSSystem( iss );

  // ��ISS�𖳌���
  ISS_3DS_SYS_SetMasterVolume( iss3DSSystem, 0 );

  // �v���C���[�{�����[�����t�F�[�h�A�E�g
  FIELD_SOUND_ChangePlayerVolume( fieldSound, APP_HOLD_VOLUME, APP_FADE_FRAME );  
}

//---------------------------------------------------------------------------------
/**
 * @brief �t�B�[���h �� �A�v�� �J�ڎ��� BGM�{�����[���������
 *
 * @param fieldSound
 * @parma iss
 */
//---------------------------------------------------------------------------------
void FSND_ReleaseBGMVolume_fromApp( FIELD_SOUND* fieldSound, ISS_SYS* iss )
{
  ISS_3DS_SYS* iss3DSSystem;

  iss3DSSystem = ISS_SYS_GetIss3DSSystem( iss );

  // ��ISS�𖳌���
  ISS_3DS_SYS_SetMasterVolume( iss3DSSystem, MAX_VOLUME );

  // �v���C���[�{�����[�����t�F�[�h�C��
  FIELD_SOUND_ChangePlayerVolume( fieldSound, MAX_VOLUME, APP_FADE_FRAME );
}

//---------------------------------------------------------------------------------
/**
 * @brief �A�v�����ɂ����� BGM�{�����[������ ( ������ꍇ )
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
void FSND_HoldBGMVolume_inApp( FIELD_SOUND* fieldSound )
{
  // �v���C���[�{�����[���𑦎��ݒ�
  FIELD_SOUND_ChangePlayerVolume( fieldSound, APP_HOLD_VOLUME, 0 );  
}

//---------------------------------------------------------------------------------
/**
 * @brief �A�v�����ɂ����� BGM�{�����[������ ( �グ��ꍇ )
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
void FSND_ReleaseBGMVolume_inApp( FIELD_SOUND* fieldSound )
{
  // �v���C���[�{�����[���𑦎��ݒ�
  FIELD_SOUND_ChangePlayerVolume( fieldSound, MAX_VOLUME, 0 );  
}


//=================================================================================
// ��BGM No.�擾
//=================================================================================

//---------------------------------------------------------------------------------
/**
 * @brief �w�肵���]�[���ł̍Đ����ׂ�BGM���擾����
 *
 * @param gameData
 * @param zoneID   �]�[��ID
 *
 * @return �w�肵���]�[���ōĐ����ׂ�BGM No.
 */
//---------------------------------------------------------------------------------
u32 FSND_GetFieldBGM( GAMEDATA* gameData, u32 zoneID )
{
  return GetFieldBGM( gameData, zoneID );
}

//---------------------------------------------------------------------------------
/**
 * @brief �g���[�i�[������ �擾
 *
 * @param trType �g���[�i�[�^�C�v
 *
 * @return �w�肵���g���[�i�[�^�C�v�̎�����
 */
//---------------------------------------------------------------------------------
u32 FSND_GetTrainerEyeBGM( u32 trType )
{
  int i = 0;
  while( DATA_TrainerTypeToEyeBGMNo[i][0] != TRTYPE_MAX )
  {
    if( DATA_TrainerTypeToEyeBGMNo[i][0] == trType )
    {
      return( DATA_TrainerTypeToEyeBGMNo[i][1]  );
    }
    i++;
  }
  
  GF_ASSERT(0 && "FIELD-SOUND: error unknown trainer type\n");
  return SEQ_BGM_EYE_01;
} 


//=================================================================================
// ������J�֐�
//=================================================================================

//---------------------------------------------------------------------------------
/**
 * @brief ���@�̃t�H�[�����擾����
 *
 * @param gameData
 *
 * @return ���@�̃t�H�[��
 */
//---------------------------------------------------------------------------------
static PLAYER_MOVE_FORM GetPlayerMoveForm( GAMEDATA* gameData )
{
  PLAYER_WORK* player;
  PLAYER_MOVE_FORM playerForm;

  player = GAMEDATA_GetMyPlayerWork( gameData );
  playerForm = PLAYERWORK_GetMoveForm( player );

  return playerForm;
}

//---------------------------------------------------------------------------------
/**
 * @brief �w��]�[����BGM���擾
 *
 * @param gameData
 * @param zoneID �]�[��ID
 *
 * @return �w��]�[����BGM
 */
//---------------------------------------------------------------------------------
static u32 GetZoneBGM( GAMEDATA* gameData, u32 zoneID )
{
  u32 soundIdx;
  u8 seasonID;

  seasonID = GAMEDATA_GetSeasonID( gameData );
  soundIdx = ZONEDATA_GetBGMID( zoneID, seasonID );

  return soundIdx;
}

//---------------------------------------------------------------------------------
/**
 * @brief ����BGM���擾����
 *
 * @param gameData
 * @param zoneID   ����BGM����������]�[��
 *
 * @return ��������v�������BGM�����݂���ꍇ, ����BGM No.
 *         ����BGM�����݂��Ȃ��ꍇ, SPECIAL_BGM_NONE
 */
//---------------------------------------------------------------------------------
static u32 GetSpecialBGM( GAMEDATA* gameData, u32 zoneID )
{
  int i;
  EVENTWORK* eventWork;

  eventWork = GAMEDATA_GetEventWork( gameData );

  // ����
  for( i=0; i<NELEMS(specialBGMTable); i++)
  {
    // ����
    if( (specialBGMTable[i].zoneID == zoneID) &&
        (EVENTWORK_CheckEventFlag(eventWork, specialBGMTable[i].flagNo) == TRUE) )
    {
      return specialBGMTable[i].soundIdx;
    }
  }

  // ��v�Ȃ�
  return SPECIAL_BGM_NONE;
}

//---------------------------------------------------------------------------------
/**
 * @brief �w��]�[���ɂ�����, �Đ����ׂ�BGM���擾����
 *
 * @param gameData
 * @param zoneID
 *
 * @return �w�肵���]�[���ōĐ����ׂ�BGM
 */
//---------------------------------------------------------------------------------
static u32 GetFieldBGM( GAMEDATA* gameData, u32 zoneID )
{
  u32 soundIdx;
  PLAYER_MOVE_FORM playerForm;
  FIELD_SOUND* fieldSound;

  playerForm = GetPlayerMoveForm( gameData );
  fieldSound = GAMEDATA_GetFieldSound( gameData );

  // ����BGM���擾
  soundIdx = GetSpecialBGM( gameData, zoneID );

  // ����BGM������
  if( soundIdx == SPECIAL_BGM_NONE )
  {
    if( (playerForm == PLAYER_MOVE_FORM_SWIM) && ZONEDATA_BicycleBGMEnable( zoneID ) )
    { // �Ȃ݂̂�
      soundIdx = SEQ_BGM_NAMINORI;
    }      
    else if( (playerForm == PLAYER_MOVE_FORM_CYCLE) && ZONEDATA_BicycleBGMEnable( zoneID ) )
    { // ���]��
      soundIdx = SEQ_BGM_BICYCLE; 
    } 
    else
    { // ����
      soundIdx = GetZoneBGM( gameData, zoneID ); 
    }                             
  } 

  return soundIdx;
}




//=================================================================================
// ������
//=================================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �����̍Đ�
 *
 *	@param	fieldSound    �t�B�[���h�T�E���h
 *	@param	soundIdx      �T�E���h�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
void FSND_PlayEnvSE( FIELD_SOUND* fieldSound, u32 soundIdx )
{
  FIELD_SOUND_PlayEnvSE( fieldSound, soundIdx );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����̍Đ�  �i�P���̂ݑΉ��j
 *
 *	@param	fieldSound    �t�B�[���h�T�E���h
 *	@param	soundIdx      �T�E���h�C���f�b�N�X
 *	@param	vol           �w��{�����[��(�L���l 0-127 �f�t�H���g��127)
 */
//-----------------------------------------------------------------------------
void FSND_PlayEnvSEVol( FIELD_SOUND* fieldSound, u32 soundIdx, u32 vol )
{
  FIELD_SOUND_PlayEnvSEVol( fieldSound, soundIdx, vol );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����{�����[���̐ݒ�
 *
 *	@param	fieldSound  �t�B�[���h�T�E���h
 *	@param	soundIdx    �T�E���h�C���f�b�N�X
 *	@param	vol           �w��{�����[��(�L���l 0-127 �f�t�H���g��127)
 */
//-----------------------------------------------------------------------------
void FSND_SetEnvSEVol( FIELD_SOUND* fieldSound, u32 soundIdx, u32 vol )
{
  FIELD_SOUND_SetEnvSEVol( fieldSound, soundIdx, vol );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �����̒�~
 *
 *	@param	fieldSound  �t�B�[���h�T�E���h
 *	@param	soundIdx    �T�E���h�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
void FSND_StopEnvSE( FIELD_SOUND* fieldSound, u32 soundIdx )
{
  FIELD_SOUND_StopEnvSE( fieldSound, soundIdx );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����̈ꎞ��~
 *
 *	@param	fieldSound  �t�B�[���h�T�E���h
 */
//-----------------------------------------------------------------------------
void FSND_PauseEnvSE( FIELD_SOUND* fieldSound )
{
  FIELD_SOUND_PauseEnvSE( fieldSound );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����̍Ďn��
 *
 *	@param	fieldSound  �t�B�[���h�T�E���h
 */
//-----------------------------------------------------------------------------
void FSND_RePlayEnvSE( FIELD_SOUND* fieldSound )
{
  FIELD_SOUND_RePlayEnvSE( fieldSound );
}

//=================================================================================
// ��TV�g�����V�[�o�[���M��
//=================================================================================
//-----------------------------------------------------------------------------
/**
 * @brief TV�g�����V�[�o�[���M�����N�G�X�g
 *
 *	@param	fieldSound  �t�B�[���h�T�E���h
 */
//-----------------------------------------------------------------------------
void FSND_RequestTVTRingTone( FIELD_SOUND* fieldSound )
{
  FIELD_SOUND_PlayTVTRingTone( fieldSound );
}

//-----------------------------------------------------------------------------
/**
 * @brief TV�g�����V�[�o�[���M���̒�~
 *
 *	@param	fieldSound  �t�B�[���h�T�E���h
 */
//-----------------------------------------------------------------------------
void FSND_StopTVTRingTone( FIELD_SOUND* fieldSound )
{
  FIELD_SOUND_StopTVTRingTone( fieldSound );
}

