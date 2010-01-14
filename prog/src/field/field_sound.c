///////////////////////////////////////////////////////////////////////////////////
/**
 * @brief �t�B�[���h�̃T�E���h����
 * @file  field_sound.c
 * @author obata
 * @date   2009.12.25
 */
///////////////////////////////////////////////////////////////////////////////////
#include "field_sound.h"
#include "sound/wb_sound_data.sadl"
#include "sound/pm_sndsys.h"
#include "gamesystem/playerwork.h"
#include "gamesystem/game_data.h"
#include "field/zonedata.h"


//================================================================================= 
// ���萔
//================================================================================= 
#define FSND_PRINT_NO (1)  // printf�o�͐�R���\�[���ԍ�
#define FSND_BGM_NULL (0)  // BGM�ԍ��̖����l

// �t�F�[�h �t���[����
static const u16 FadeInFrame [FSND_FADEIN_SPEED_NUM]  = {0, 9, 30, 45};
static const u16 FadeOutFrame[FSND_FADEOUT_SPEED_NUM] = {0, 9, 30, 45};

// �������
typedef enum{
  FSND_STATE_STOP,              // ��~��
  FSND_STATE_PLAY,              // �Đ���
  FSND_STATE_WAIT,              // �ҋ@��
  FSND_STATE_FADE_OUT,          // BGM�t�F�[�h�A�E�g��
  FSND_STATE_FADE_IN,           // BGM�t�F�[�h�C����
  FSND_STATE_PUSH,              // BGM�ޔ�(�t�F�[�h�A�E�g�����҂�)
  FSND_STATE_POP_out,           // BGM���A��(�t�F�[�h�A�E�g�����҂�)
  FSND_STATE_POP_in,            // BGM���A��(pop���s�҂�)
  FSND_STATE_CHANGE_out,        // BGM�ύX��(�t�F�[�h�A�E�g�����҂�)
  FSND_STATE_CHANGE_load,       // BGM�ύX��(�������[�h�����҂�)
  FSND_STATE_CHANGE_in,         // BGM�ύX��(�t�F�[�h�C�������҂�)
  FSND_STATE_CHANGE_PUSH_out,   // BGM�ύX���ޔ�(�t�F�[�h�A�E�g�����҂�)
  FSND_STATE_CHANGE_PUSH_load,  // BGM�ύX���ޔ�(�������[�h�����҂�)
  FSND_STATE_STAND_BY_out,      // BGM�Đ�������(�t�F�[�h�A�E�g�����҂�)
  FSND_STATE_STAND_BY_load,     // BGM�Đ�������(�������[�h�����҂�)
  FSND_STATE_NUM
} FSND_STATE;

// ���N�G�X�g
typedef enum{
  FSND_BGM_REQUEST_NONE,        // ���N�G�X�g�Ȃ�
  FSND_BGM_REQUEST_FADE_IN,     // BGM �t�F�[�h�C��
  FSND_BGM_REQUEST_FADE_OUT,    // BGM �t�F�[�h�A�E�g
  FSND_BGM_REQUEST_PUSH,        // BGM �ޔ�
  FSND_BGM_REQUEST_POP,         // BGM ���A
  FSND_BGM_REQUEST_CHANGE,      // BGM �ύX
  FSND_BGM_REQUEST_STAND_BY,    // BGM �Đ�����
  FSND_BGM_REQUEST_FORCE_PLAY,  // �����Đ�
  FSND_BGM_REQUEST_NUM
} FSND_BGM_REQUEST;

// �e������Ԃɑ΂���, ���N�G�X�g�̉s�e�[�u��
static BOOL RequestAcceptTable[ FSND_STATE_NUM ][ FSND_BGM_REQUEST_NUM ] = 
{
                        /* NONE    F_IN     F_OUT    PUSH     POP      CHANGE   S_BY     F_PLAY */
  /* STOP             */{  TRUE,   FALSE,   FALSE,   FALSE,    TRUE,    TRUE,    TRUE,    TRUE,  },
  /* PLAY             */{  TRUE,   FALSE,    TRUE,    TRUE,    TRUE,    TRUE,    TRUE,    TRUE,  },
  /* WAIT             */{  TRUE,    TRUE,   FALSE,   FALSE,   FALSE,   FALSE,   FALSE,    TRUE,  },
  /* FADE_OUT         */{  TRUE,   FALSE,   FALSE,   FALSE,   FALSE,   FALSE,   FALSE,   FALSE,  },
  /* FALDE_IN         */{  TRUE,   FALSE,   FALSE,   FALSE,   FALSE,   FALSE,   FALSE,   FALSE,  },
  /* PUSH             */{  TRUE,   FALSE,   FALSE,   FALSE,   FALSE,   FALSE,   FALSE,   FALSE,  },
  /* POP_out          */{  TRUE,   FALSE,   FALSE,   FALSE,   FALSE,   FALSE,   FALSE,   FALSE,  },
  /* POP_in           */{  TRUE,   FALSE,   FALSE,   FALSE,   FALSE,   FALSE,   FALSE,   FALSE,  },
  /* CHANGE_out       */{  TRUE,   FALSE,   FALSE,    TRUE,   FALSE,    TRUE,   FALSE,   FALSE,  },
  /* CHANGE_load      */{  TRUE,   FALSE,   FALSE,    TRUE,   FALSE,    TRUE,   FALSE,   FALSE,  },
  /* CHANGE_in        */{  TRUE,   FALSE,   FALSE,   FALSE,   FALSE,    TRUE,   FALSE,   FALSE,  },
  /* CHANGE_PUSH_out  */{  TRUE,   FALSE,   FALSE,   FALSE,   FALSE,   FALSE,   FALSE,   FALSE,  },
  /* CHANGE_PUSH_load */{  TRUE,   FALSE,   FALSE,   FALSE,   FALSE,   FALSE,   FALSE,   FALSE,  },
  /* STAND_BY_out     */{  TRUE,   FALSE,   FALSE,   FALSE,   FALSE,   FALSE,    TRUE,   FALSE,  },
  /* STAND_BY_load    */{  TRUE,   FALSE,   FALSE,   FALSE,   FALSE,   FALSE,    TRUE,   FALSE,  },
};


//================================================================================= 
// ���t�B�[���h�T�E���h�Ǘ����[�N
//================================================================================= 
struct _FIELD_SOUND
{
  FSND_STATE         state;  // ���݂̏��
  FSND_BGM_REQUEST request;  // ���N�G�X�g
  u32           currentBGM;  // ���ݍĐ�����BGM
  u32           requestBGM;  // ���N�G�X�gBGM
  u32              loadBGM;  // �������[�h�Ώ�BGM
  u32              loadSeq;  // �������[�h�̃V�[�P���X
  u16          fadeInFrame;  // �t�F�[�h�C�� �t���[���� 
  u16         fadeOutFrame;  // �t�F�[�h�A�E�g �t���[����
  FSND_PUSHCOUNT pushCount;  // BGM�ޔ�
  u32 pushBGM[FSND_PUSHCOUNT_MAX];  // �ޔ𒆂�BGN
};


//================================================================================= 
// ������J�֐�
//================================================================================= 
// BGM �ԍ��擾
static u32 GetZoneBGM( GAMEDATA* gameData, u32 zoneID );
// �t�F�[�h �t���[�����̎擾
static u16 GetFadeInFrame( FSND_FADEIN_SPEED fadeInSpeed );
static u16 GetFadeOutFrame( FSND_FADEOUT_SPEED fadeOutSpeed );
// ���N�G�X�g�̔��s
static void ThrowRequest_FADE_IN( FIELD_SOUND* fieldSound, u16 fadeInFrame );
static void ThrowRequest_FADE_OUT( FIELD_SOUND* fieldSound, u16 fadeOutFrame );
static void ThrowRequest_PUSH( FIELD_SOUND* fieldSound, u16 fadeOutFrame );
static void ThrowRequest_POP( FIELD_SOUND* fieldSound, u16 fadeOutFrame, u16 fadeInFrame );
static void ThrowRequest_CHANGE( FIELD_SOUND* fieldSound, u32 soundIdx, 
                                 u16 fadeOutFrame, u16 fadeInFrame );
static void ThrowRequest_STAND_BY( FIELD_SOUND* fieldSound, u32 soundIdx, u16 fadeOutFrame );
static void ThrowRequest_FORCE_PLAY( FIELD_SOUND* fieldSound, u16 soundIdx );
// ���N�G�X�g���󗝂ł��邩�ǂ����̔���
static BOOL CanAcceptRequest( const FIELD_SOUND* fieldSound, FSND_BGM_REQUEST request );
// ���N�G�X�g�ɑ΂��铮��
static void RequestCheck_STOP            ( FIELD_SOUND* fieldSound );
static void RequestCheck_PLAY            ( FIELD_SOUND* fieldSound );
static void RequestCheck_WAIT            ( FIELD_SOUND* fieldSound );
static void RequestCheck_FADE_OUT        ( FIELD_SOUND* fieldSound );
static void RequestCheck_FADE_IN         ( FIELD_SOUND* fieldSound );
static void RequestCheck_PUSH            ( FIELD_SOUND* fieldSound );
static void RequestCheck_POP_out         ( FIELD_SOUND* fieldSound );
static void RequestCheck_POP_in          ( FIELD_SOUND* fieldSound );
static void RequestCheck_CHANGE_out      ( FIELD_SOUND* fieldSound );
static void RequestCheck_CHANGE_load     ( FIELD_SOUND* fieldSound );
static void RequestCheck_CHANGE_in       ( FIELD_SOUND* fieldSound );
static void RequestCheck_CHANGE_PUSH_out ( FIELD_SOUND* fieldSound );
static void RequestCheck_CHANGE_PUSH_load( FIELD_SOUND* fieldSound );
static void RequestCheck_STAND_BY_out    ( FIELD_SOUND* fieldSound );
static void RequestCheck_STAND_BY_load   ( FIELD_SOUND* fieldSound );
// ��������
static void Main_STOP            ( FIELD_SOUND* fieldSound );
static void Main_PLAY            ( FIELD_SOUND* fieldSound );
static void Main_WAIT            ( FIELD_SOUND* fieldSound );
static void Main_FADE_OUT        ( FIELD_SOUND* fieldSound );
static void Main_FADE_IN         ( FIELD_SOUND* fieldSound );
static void Main_PUSH            ( FIELD_SOUND* fieldSound );
static void Main_POP_out         ( FIELD_SOUND* fieldSound );
static void Main_POP_in          ( FIELD_SOUND* fieldSound );
static void Main_CHANGE_out      ( FIELD_SOUND* fieldSound );
static void Main_CHANGE_load     ( FIELD_SOUND* fieldSound );
static void Main_CHANGE_in       ( FIELD_SOUND* fieldSound );
static void Main_CHANGE_PUSH_out ( FIELD_SOUND* fieldSound );
static void Main_CHANGE_PUSH_load( FIELD_SOUND* fieldSound );
static void Main_STAND_BY_out    ( FIELD_SOUND* fieldSound );
static void Main_STAND_BY_load   ( FIELD_SOUND* fieldSound );
// ������Ԃ̕ύX
static void ChangeState( FIELD_SOUND* fieldSound, FSND_STATE nextState );
// ���N�G�X�g�������̏���
static void RequestFinished( FIELD_SOUND* fieldSound );
// BGM����
static void FadeInBGM ( FIELD_SOUND* fieldSound );
static void FadeOutBGM( FIELD_SOUND* fieldSound );
static void PushBGM( FIELD_SOUND* fieldSound );
static void PopBGM ( FIELD_SOUND* fieldSound );
static void DivLoadBGM_start( FIELD_SOUND* fieldSound );
static BOOL DivLoadBGM_load ( FIELD_SOUND* fieldSound );
static void ForcePlayBGM( FIELD_SOUND* fieldSound );


//================================================================================= 
// ���V�X�e���쐬/�j��
//================================================================================= 

//---------------------------------------------------------------------------------
/**
 * @brief �t�B�[���h�T�E���h�쐬
 *
 * @param heapID �q�[�vID
 *
 * @return �쐬�����t�B�[���h�T�E���h
 */
//---------------------------------------------------------------------------------
FIELD_SOUND* FIELD_SOUND_Create( HEAPID heapID )
{
  int i;
  FIELD_SOUND* fieldSound;
  fieldSound               = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_SOUND) );
  fieldSound->state        = FSND_STATE_STOP;
  fieldSound->request      = FSND_BGM_REQUEST_NONE;
  fieldSound->currentBGM   = FSND_BGM_NULL;
  fieldSound->requestBGM   = FSND_BGM_NULL;
  fieldSound->loadBGM      = FSND_BGM_NULL;
  fieldSound->fadeInFrame  = 0;
  fieldSound->fadeOutFrame = 0;
  fieldSound->pushCount    = FSND_PUSHCOUNT_NONE;
  for( i=0; i<FSND_PUSHCOUNT_MAX; i++ ){ fieldSound->pushBGM[i] = FSND_BGM_NULL; }
  return fieldSound;
}

//---------------------------------------------------------------------------------
/**
 * @brief �t�B�[���h�T�E���h�j��
 *
 * @param fieldSound �t�B�[���h�T�E���h
 */
//---------------------------------------------------------------------------------
void FIELD_SOUND_Delete( FIELD_SOUND* fieldSound )
{
  GFL_HEAP_FreeMemory( fieldSound );
}


//=================================================================================
// ��BGM ����C�x���g
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
  u32             soundIdx;
  u16          fadeInFrame;
  u16         fadeOutFrame;

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
  case 0:  // �����Đ����N�G�X�g���s
    if( CanAcceptRequest( fieldSound, FSND_BGM_REQUEST_FORCE_PLAY ) )
    {
      ThrowRequest_FORCE_PLAY( fieldSound, work->soundIdx );
      (*seq)++;
    }
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
GMEVENT* EVENT_FieldSound_ForcePlayBGM( GAMESYS_WORK* gameSystem, u32 soundIdx )
{
  GMEVENT* event;
  FSND_EVWORK* work;
  GAMEDATA* gameData;

  gameData = GAMESYSTEM_GetGameData( gameSystem );

  event = GMEVENT_Create( gameSystem, NULL, ForcePlayBGMEvent, sizeof(FSND_EVWORK) );
  work = GMEVENT_GetEventWork( event );
  work->gameSystem = gameSystem;
  work->fieldSound = GAMEDATA_GetFieldSound( gameData );
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
  case 0:  // �t�F�[�h�C�� ���N�G�X�g���s
    if( CanAcceptRequest( fieldSound, FSND_BGM_REQUEST_FADE_IN ) )
    {
      ThrowRequest_FADE_IN( fieldSound, work->fadeInFrame );
      (*seq)++;
    }
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
 * @param fadeInSpeed �t�F�[�h�C�����x
 * 
 * @return BGM �t�F�[�h�C�� �C�x���g
 */
//---------------------------------------------------------------------------------
GMEVENT* EVENT_FieldSound_FadeInBGM( GAMESYS_WORK* gameSystem, FSND_FADEIN_SPEED fadeInSpeed )
{
  GMEVENT* event;
  FSND_EVWORK* work;
  GAMEDATA* gameData;

  gameData = GAMESYSTEM_GetGameData( gameSystem );

  event = GMEVENT_Create( gameSystem, NULL, FadeInBGMEvent, sizeof(FSND_EVWORK) );
  work = GMEVENT_GetEventWork( event );
  work->gameSystem = gameSystem;
  work->fieldSound = GAMEDATA_GetFieldSound( gameData );
  work->fadeInFrame = GetFadeInFrame( fadeInSpeed );
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
  case 0:  // �t�F�[�h�A�E�g ���N�G�X�g���s
    if( CanAcceptRequest( fieldSound, FSND_BGM_REQUEST_FADE_OUT ) )
    {
      ThrowRequest_FADE_OUT( fieldSound, work->fadeOutFrame );
      (*seq)++;
    }
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
 * @param fadeOutSpeed �t�F�[�h�A�E�g���x
 * 
 * @return BGM �t�F�[�h�A�E�g �C�x���g
 */
//---------------------------------------------------------------------------------
GMEVENT* EVENT_FieldSound_FadeOutBGM( GAMESYS_WORK* gameSystem, FSND_FADEOUT_SPEED fadeOutSpeed )
{
  GMEVENT* event;
  FSND_EVWORK* work;
  GAMEDATA* gameData;

  gameData = GAMESYSTEM_GetGameData( gameSystem );

  event = GMEVENT_Create( gameSystem, NULL, FadeOutBGMEvent, sizeof(FSND_EVWORK) );
  work = GMEVENT_GetEventWork( event );
  work->gameSystem = gameSystem;
  work->fieldSound = GAMEDATA_GetFieldSound( gameData );
  work->fadeOutFrame = GetFadeOutFrame( fadeOutSpeed );
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
  case 0:  // BGM�ޔ����N�G�X�g���s
    if( CanAcceptRequest( fieldSound, FSND_BGM_REQUEST_PUSH ) )
    {
      ThrowRequest_PUSH( fieldSound, work->fadeOutFrame );
      (*seq)++;
    }
    break;
  case 1:  // �ޔ������҂�
    if( fieldSound->request == FSND_BGM_REQUEST_NONE )
    {
      (*seq)++;
    }
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
 * @param fadeOutSpeed �t�F�[�h�A�E�g���x
 * 
 * @return BGM �ޔ��C�x���g
 */
//---------------------------------------------------------------------------------
GMEVENT* EVENT_FieldSound_PushBGM( GAMESYS_WORK* gameSystem, FSND_FADEOUT_SPEED fadeOutSpeed )
{
  GMEVENT* event;
  FSND_EVWORK* work;
  GAMEDATA* gameData;

  gameData = GAMESYSTEM_GetGameData( gameSystem );

  event = GMEVENT_Create( gameSystem, NULL, PushBGMEvent, sizeof(FSND_EVWORK) );
  work = GMEVENT_GetEventWork( event );
  work->gameSystem = gameSystem;
  work->fieldSound = GAMEDATA_GetFieldSound( gameData );
  work->fadeOutFrame = GetFadeOutFrame( fadeOutSpeed );
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
  case 0:  // BGM���A���N�G�X�g���s
    if( CanAcceptRequest( fieldSound, FSND_BGM_REQUEST_POP ) )
    {
      ThrowRequest_POP( fieldSound, work->fadeOutFrame, work->fadeInFrame );
      (*seq)++;
    }
    break;
  case 1:  // ���ABGM�̃t�F�[�h�J�n�҂�
    if( (fieldSound->state == FSND_STATE_POP_in) ||
        (fieldSound->state == FSND_STATE_PLAY) )
    {
      (*seq)++;
    }
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
 * @param fadeOutSpeed �Đ�����BGM�̃t�F�[�h�A�E�g���x
 * @param fadeInSpeed  ���A����BGM�̃t�F�[�h�C�����x
 * 
 * @return BGM ���A�C�x���g
 */
//---------------------------------------------------------------------------------
GMEVENT* EVENT_FieldSound_PopBGM( GAMESYS_WORK* gameSystem, 
                                  FSND_FADEOUT_SPEED fadeOutSpeed, 
                                  FSND_FADEIN_SPEED fadeInSpeed )
{
  GMEVENT* event;
  FSND_EVWORK* work;
  GAMEDATA* gameData;

  gameData = GAMESYSTEM_GetGameData( gameSystem );

  event = GMEVENT_Create( gameSystem, NULL, PopBGMEvent, sizeof(FSND_EVWORK) );
  work = GMEVENT_GetEventWork( event );
  work->gameSystem = gameSystem;
  work->fieldSound = GAMEDATA_GetFieldSound( gameData );
  work->fadeInFrame = GetFadeInFrame( fadeInSpeed );
  work->fadeOutFrame = GetFadeOutFrame( fadeOutSpeed );
  return event;
}

//---------------------------------------------------------------------------------
/**
 * @brief BGM �S���A�C�x���g�����֐�
 */
//---------------------------------------------------------------------------------
static GMEVENT_RESULT AllPopBGMEvent( GMEVENT* event, int* seq, void* wk )
{
  FSND_EVWORK* work; 
  FSND_PUSHCOUNT pushCount;
  u16 fadeInFrame;

  work = (FSND_EVWORK*)wk;
  pushCount = FIELD_SOUND_GetBGMPushCount( work->fieldSound );

  switch( *seq )
  {
  case 0:
    if( FSND_PUSHCOUNT_NONE < pushCount )
    {
      if( pushCount == FSND_PUSHCOUNT_BASE ){ fadeInFrame = work->fadeInFrame; }
      else{ fadeInFrame = GetFadeInFrame(FSND_FADEIN_NONE); }

      {
        u16 fadeOutFrame;
        GMEVENT* popEvent;
        
        fadeOutFrame = GetFadeOutFrame(FSND_FADEOUT_NONE);
        popEvent = EVENT_FieldSound_PopBGM( work->gameSystem, fadeOutFrame, fadeInFrame );
        GMEVENT_CallEvent( event, popEvent ); 
      }
    }
    else
    {
      (*seq)++;
    }
    break;
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
 * @param fadeInSpeed �t�F�[�h�C�����x
 * 
 * @return BGM �S���A�C�x���g
 */
//---------------------------------------------------------------------------------
GMEVENT* EVENT_FieldSound_AllPopBGM( GAMESYS_WORK* gameSystem, FSND_FADEIN_SPEED fadeInSpeed )
{
  GMEVENT* event;
  FSND_EVWORK* work;
  GAMEDATA* gameData;

  gameData = GAMESYSTEM_GetGameData( gameSystem );

  event = GMEVENT_Create( gameSystem, NULL, AllPopBGMEvent, sizeof(FSND_EVWORK) );
  work = GMEVENT_GetEventWork( event );
  work->gameSystem = gameSystem;
  work->fieldSound = GAMEDATA_GetFieldSound( gameData );
  work->fadeInFrame = GetFadeInFrame( fadeInSpeed );
  return event;
} 

//---------------------------------------------------------------------------------
/**
 * @brief �C�x���gBGM �Đ��C�x���g�����֐�
 */
//---------------------------------------------------------------------------------
static GMEVENT_RESULT PushPlayEventBGMEvent( GMEVENT* event, int* seq, void* wk )
{
  FSND_EVWORK* work;
  FIELD_SOUND* fieldSound;

  work = (FSND_EVWORK*)wk;
  fieldSound = work->fieldSound;

  switch( *seq )
  {
  case 0:  // BGM�ޔ����N�G�X�g���s
    if( CanAcceptRequest( fieldSound, FSND_BGM_REQUEST_PUSH ) )
    {
      ThrowRequest_PUSH( fieldSound, work->fadeOutFrame );
      (*seq)++;
    }
    break;
  case 1:  // BGM�����Đ����N�G�X�g���s
    if( CanAcceptRequest( fieldSound, FSND_BGM_REQUEST_FORCE_PLAY ) )
    {
      // BGM�ޔ𐔃I�[�o�[
      if( FSND_PUSHCOUNT_BASE < fieldSound->pushCount )
      {
        OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: Can't play event-BGM. (push count over!)\n" );
        GF_ASSERT(0);
      }

      ThrowRequest_FORCE_PLAY( fieldSound, work->soundIdx );
      (*seq)++;
    }
    break;
  case 2:
    return GMEVENT_RES_FINISH;
  } 
  return GMEVENT_RES_CONTINUE;
}
//---------------------------------------------------------------------------------
/**
 * @brief �C�x���gBGM �Đ��C�x���g �擾
 *
 * @param gameSystem
 * @param soundIdx   �Đ�����BGM
 * 
 * @return ME �Đ��C�x���g
 */
//--------------------------------------------------------------------------------- 
GMEVENT* EVENT_FieldSound_PushPlayEventBGM( GAMESYS_WORK* gameSystem, u32 soundIdx )
{
  GMEVENT* event;
  FSND_EVWORK* work;
  GAMEDATA* gameData;

  gameData = GAMESYSTEM_GetGameData( gameSystem );

  event = GMEVENT_Create( gameSystem, NULL, PushPlayEventBGMEvent, sizeof(FSND_EVWORK) );
  work = GMEVENT_GetEventWork( event );
  work->gameSystem = gameSystem;
  work->fieldSound = GAMEDATA_GetFieldSound( gameData );
  work->soundIdx = soundIdx;
  work->fadeOutFrame = GetFadeOutFrame(FSND_FADEOUT_FAST);
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
  case 0:  // BGM�ޔ����N�G�X�g���s
    if( CanAcceptRequest( fieldSound, FSND_BGM_REQUEST_PUSH ) )
    {
      ThrowRequest_PUSH( fieldSound, work->fadeOutFrame );
      (*seq)++;
    }
    break;
  case 1:  // BGM�����Đ����N�G�X�g���s
    if( CanAcceptRequest( fieldSound, FSND_BGM_REQUEST_FORCE_PLAY ) )
    {
      // BGM�ޔ𐔃I�[�o�[
      if( FSND_PUSHCOUNT_EVENT < fieldSound->pushCount )
      {
        OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: Can't play jingle. (push count over!)\n" );
        GF_ASSERT(0);
      }

      ThrowRequest_FORCE_PLAY( fieldSound, work->soundIdx );
      (*seq)++;
    }
    break;
  case 2:
    return GMEVENT_RES_FINISH;
  } 
  return GMEVENT_RES_CONTINUE;
} 
//---------------------------------------------------------------------------------
/**
 * @brief ME �Đ��C�x���g �擾
 *
 * @param gameSystem
 * @param soundIdx   �Đ�����ME
 * 
 * @return ME �Đ��C�x���g
 */
//---------------------------------------------------------------------------------
GMEVENT* EVENT_FieldSound_PushPlayJingleBGM( GAMESYS_WORK* gameSystem, u32 soundIdx )
{
  GMEVENT* event;
  FSND_EVWORK* work;
  GAMEDATA* gameData;

  gameData = GAMESYSTEM_GetGameData( gameSystem );

  event = GMEVENT_Create( gameSystem, NULL, PushPlayJingleBGMEvent, sizeof(FSND_EVWORK) );
  work = GMEVENT_GetEventWork( event );
  work->gameSystem = gameSystem;
  work->fieldSound = GAMEDATA_GetFieldSound( gameData );
  work->soundIdx = soundIdx;
  work->fadeOutFrame = GetFadeOutFrame(FSND_FADEOUT_NONE);
  return event;
} 

//---------------------------------------------------------------------------------
/**
 * @brief BGM�ύX�C�x���g�����֐�
 */
//---------------------------------------------------------------------------------
static GMEVENT_RESULT ChangeFieldBGMEvent( GMEVENT* event, int* seq, void* wk )
{
  FSND_EVWORK* work;
  FIELD_SOUND* fieldSound;

  work = (FSND_EVWORK*)wk;
  fieldSound = work->fieldSound;

  switch( *seq )
  {
  case 0:  // BGM�ύX���N�G�X�g���s
    if( CanAcceptRequest( fieldSound, FSND_BGM_REQUEST_CHANGE ) )
    {
      ThrowRequest_CHANGE( fieldSound, work->soundIdx, work->fadeOutFrame, work->fadeInFrame );
      (*seq)++;
    }
    break;
  case 1:
    return GMEVENT_RES_FINISH;
  } 
  return GMEVENT_RES_CONTINUE;
} 
//---------------------------------------------------------------------------------
/**
 * @brief BGM�ύX�C�x���g �擾
 *
 * @param gameSystem
 * @param gameData   
 * @param zoneID     BGM ���Đ�����ꏊ
 * 
 * @return BGM�ύX�C�x���g
 */
//---------------------------------------------------------------------------------
GMEVENT* EVENT_FieldSound_ChangeFieldBGM( GAMESYS_WORK* gameSystem, u16 zoneID )
{
  GMEVENT* event;
  FSND_EVWORK* work;
  GAMEDATA* gameData;

  gameData = GAMESYSTEM_GetGameData( gameSystem );

  event = GMEVENT_Create( gameSystem, NULL, ChangeFieldBGMEvent, sizeof(FSND_EVWORK) );
  work = GMEVENT_GetEventWork( event );
  work->gameSystem = gameSystem;
  work->fieldSound = GAMEDATA_GetFieldSound( gameData );
  work->soundIdx = FIELD_SOUND_GetFieldBGM( gameData, zoneID );
  work->fadeOutFrame = GetFadeOutFrame(FSND_FADEOUT_NORMAL);
  work->fadeInFrame = GetFadeInFrame(FSND_FADEIN_NONE);
  return event;
}

//---------------------------------------------------------------------------------
/**
 * @brief BGM�Đ������C�x���g�����֐�
 */
//---------------------------------------------------------------------------------
static GMEVENT_RESULT StandByFieldBGMEvent( GMEVENT* event, int* seq, void* wk )
{
  FSND_EVWORK* work;
  FIELD_SOUND* fieldSound;

  work = (FSND_EVWORK*)wk;
  fieldSound = work->fieldSound;

  switch( *seq )
  {
  case 0:  // BGM�Đ��������N�G�X�g���s
    if( CanAcceptRequest( fieldSound, FSND_BGM_REQUEST_STAND_BY ) )
    {
      ThrowRequest_STAND_BY( fieldSound, work->soundIdx, work->fadeOutFrame );
      (*seq)++;
    }
    break;
  case 1:
    return GMEVENT_RES_FINISH;
  } 
  return GMEVENT_RES_CONTINUE;
} 
//---------------------------------------------------------------------------------
/**
 * @brief BGM�Đ������C�x���g �擾
 *
 * @param gameSystem
 * @param gameData   
 * @param zoneID     BGM ���Đ�����ꏊ
 * 
 * @return BGM�Đ������C�x���g
 */
//---------------------------------------------------------------------------------
GMEVENT* EVENT_FieldSound_StandByFieldBGM( GAMESYS_WORK* gameSystem, u16 zoneID )
{
  GMEVENT* event;
  FSND_EVWORK* work;
  GAMEDATA* gameData;

  gameData = GAMESYSTEM_GetGameData( gameSystem );

  event = GMEVENT_Create( gameSystem, NULL, StandByFieldBGMEvent, sizeof(FSND_EVWORK) );
  work = GMEVENT_GetEventWork( event );
  work->gameSystem = gameSystem;
  work->fieldSound = GAMEDATA_GetFieldSound( gameData );
  work->soundIdx = FIELD_SOUND_GetFieldBGM( gameData, zoneID );
  work->fadeOutFrame = GetFadeOutFrame(FSND_FADEOUT_NORMAL);
  return event;
}

//---------------------------------------------------------------------------------
/**
 * @brief BGM�Đ��J�n�C�x���g�����֐�
 */
//---------------------------------------------------------------------------------
static GMEVENT_RESULT PlayStartFieldBGMEvent( GMEVENT* event, int* seq, void* wk )
{
  FSND_EVWORK* work;
  FIELD_SOUND* fieldSound;

  work = (FSND_EVWORK*)wk;
  fieldSound = work->fieldSound;

  switch( *seq )
  {
  case 0:  // ���łɍĐ����Ȃ�, �C�x���g�I��
    if( fieldSound->state == FSND_STATE_PLAY )
    {
      return GMEVENT_RES_FINISH;
    }
    (*seq)++;
    break;
  case 1:  // BGM�Đ��J�n���N�G�X�g���s
    if( CanAcceptRequest( fieldSound, FSND_BGM_REQUEST_FADE_IN ) )
    {
      ThrowRequest_FADE_IN( fieldSound, work->fadeInFrame );
      (*seq)++;
    }
    break;
  case 2:
    return GMEVENT_RES_FINISH;
  } 
  return GMEVENT_RES_CONTINUE;
} 
//---------------------------------------------------------------------------------
/**
 * @brief BGM�Đ��J�n�C�x���g �擾
 *
 * @param gameSystem
 * 
 * @return BGM�Đ��J�n�C�x���g
 */
//---------------------------------------------------------------------------------
GMEVENT* EVENT_FieldSound_PlayStartFieldBGM( GAMESYS_WORK* gameSystem )
{
  GMEVENT* event;
  FSND_EVWORK* work;
  GAMEDATA* gameData;

  gameData = GAMESYSTEM_GetGameData( gameSystem );

  event = GMEVENT_Create( gameSystem, NULL, PlayStartFieldBGMEvent, sizeof(FSND_EVWORK) );
  work = GMEVENT_GetEventWork( event );
  work->gameSystem = gameSystem;
  work->fieldSound = GAMEDATA_GetFieldSound( gameData );
  work->fadeInFrame = GetFadeInFrame(FSND_FADEIN_NONE);
  return event;
}


//=================================================================================
// ��BGM �ύX���N�G�X�g
//=================================================================================

//---------------------------------------------------------------------------------
/**
 * @brief BGM �ύX���N�G�X�g���s (BGM ���ڎw��)
 *
 * @param fieldSound   
 * @param soundIdx     �Đ����� BGM
 * @param fadeOutSpeed �t�F�[�h�A�E�g���x
 * @param fadeInSpeed  �t�F�[�h�C�����x
 *
 * @return ���N�G�X�g���󗝂����ꍇ TRUE
 *         �����łȂ����           FALSE
 */
//---------------------------------------------------------------------------------
BOOL FIELD_SOUND_BGMChangeRequest( FIELD_SOUND* fieldSound, 
                                   u32 soundIdx, 
                                   FSND_FADEOUT_SPEED fadeOutSpeed, 
                                   FSND_FADEIN_SPEED fadeInSpeed )
{
  u16 fadeOutFrame, fadeInFrame;

  // �t�F�[�h �t���[�������擾
  fadeOutFrame = GetFadeOutFrame( fadeOutSpeed );
  fadeInFrame = GetFadeInFrame( fadeInSpeed );

  // ���N�G�X�g���s
  if( CanAcceptRequest( fieldSound, FSND_BGM_REQUEST_CHANGE ) )
  { 
    ThrowRequest_CHANGE( fieldSound, soundIdx, fadeOutFrame, fadeInFrame );
    return TRUE;
  }
  return FALSE;
}

//---------------------------------------------------------------------------------
/**
 * @brief BGM �ύX���N�G�X�g���s (BGM �����I��)
 *
 * @param fieldSound 
 * @param gameData   
 * @param zoneID     BGM ���Đ�����ꏊ
 *
 * @return ���N�G�X�g���󗝂����ꍇ TRUE
 *         �����łȂ����           FALSE
 */
//---------------------------------------------------------------------------------
BOOL FIELD_SOUND_FieldBGMChangeRequest( FIELD_SOUND* fieldSound, 
                                        GAMEDATA* gameData, u16 zoneID )
{
  u32 soundIdx;
  
  // BGM������
  soundIdx = FIELD_SOUND_GetFieldBGM( gameData, zoneID );
  
  return FIELD_SOUND_BGMChangeRequest( fieldSound, soundIdx, 
                                       FSND_FADEOUT_NORMAL, FSND_FADEIN_NONE );
}


//=================================================================================
// ��BGM �Ǘ�
//=================================================================================

//---------------------------------------------------------------------------------
/**
 * @brief BGM�̏�Ԃ��Ǘ���, BGM�̕ύX���s��
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
void FIELD_SOUND_Main( FIELD_SOUND* fieldSound )
{
  // ���N�G�X�g�ɑ΂��锽��
  switch( fieldSound->state )
  {
  case FSND_STATE_STOP:              RequestCheck_STOP(fieldSound);              break;
  case FSND_STATE_PLAY:              RequestCheck_PLAY(fieldSound);              break;
  case FSND_STATE_WAIT:              RequestCheck_WAIT(fieldSound);              break;
  case FSND_STATE_FADE_OUT:          RequestCheck_FADE_OUT(fieldSound);          break;
  case FSND_STATE_FADE_IN:           RequestCheck_FADE_IN(fieldSound);           break;
  case FSND_STATE_PUSH:              RequestCheck_PUSH(fieldSound);              break;
  case FSND_STATE_POP_out:           RequestCheck_POP_out(fieldSound);           break;
  case FSND_STATE_POP_in:            RequestCheck_POP_in(fieldSound);            break;
  case FSND_STATE_CHANGE_out:        RequestCheck_CHANGE_out(fieldSound);        break;
  case FSND_STATE_CHANGE_load:       RequestCheck_CHANGE_load(fieldSound);       break;
  case FSND_STATE_CHANGE_in:         RequestCheck_CHANGE_in(fieldSound);         break;
  case FSND_STATE_CHANGE_PUSH_out:   RequestCheck_CHANGE_PUSH_out(fieldSound);   break;
  case FSND_STATE_CHANGE_PUSH_load:  RequestCheck_CHANGE_PUSH_load(fieldSound);  break;
  case FSND_STATE_STAND_BY_out:      RequestCheck_STAND_BY_out(fieldSound);      break;
  case FSND_STATE_STAND_BY_load:     RequestCheck_STAND_BY_load(fieldSound);     break;
  default: GF_ASSERT(0);
  }

  // ��������
  switch( fieldSound->state )
  {
  case FSND_STATE_STOP:              Main_STOP(fieldSound);              break;
  case FSND_STATE_PLAY:              Main_PLAY(fieldSound);              break;
  case FSND_STATE_WAIT:              Main_WAIT(fieldSound);              break;
  case FSND_STATE_FADE_OUT:          Main_FADE_OUT(fieldSound);          break;
  case FSND_STATE_FADE_IN:           Main_FADE_IN(fieldSound);           break;
  case FSND_STATE_PUSH:              Main_PUSH(fieldSound);              break;
  case FSND_STATE_POP_out:           Main_POP_out(fieldSound);           break;
  case FSND_STATE_POP_in:            Main_POP_in(fieldSound);            break;
  case FSND_STATE_CHANGE_out:        Main_CHANGE_out(fieldSound);        break;
  case FSND_STATE_CHANGE_load:       Main_CHANGE_load(fieldSound);       break;
  case FSND_STATE_CHANGE_in:         Main_CHANGE_in(fieldSound);         break;
  case FSND_STATE_CHANGE_PUSH_out:   Main_CHANGE_PUSH_out(fieldSound);   break;
  case FSND_STATE_CHANGE_PUSH_load:  Main_CHANGE_PUSH_load(fieldSound);  break;
  case FSND_STATE_STAND_BY_out:      Main_STAND_BY_out(fieldSound);      break;
  case FSND_STATE_STAND_BY_load:     Main_STAND_BY_load(fieldSound);     break;
  default: GF_ASSERT(0);
  }
}


//=================================================================================
// ���擾
//=================================================================================

//---------------------------------------------------------------------------------
/**
 * @brief �t�B�[���h BGM �Đ�����BGM�i���o�[�擾
 *
 * @param gameData  �Q�[���f�[�^
 * @param zoneID    �]�[��ID
 *
 * @return �t�B�[���hBGM�i���o�[
 */
//---------------------------------------------------------------------------------
u32 FIELD_SOUND_GetFieldBGM( GAMEDATA* gameData, u32 zoneID )
{
  PLAYER_WORK* player;
  PLAYER_MOVE_FORM form;

  // ���@�t�H�[���擾
  player = GAMEDATA_GetMyPlayerWork( gameData );
  form   = PLAYERWORK_GetMoveForm( player );

  // �Ȃ݂̂�
  if( form == PLAYER_MOVE_FORM_SWIM ){ return( SEQ_BGM_NAMINORI ); } 
  // ���]��
  if( form == PLAYER_MOVE_FORM_CYCLE ){ return( SEQ_BGM_BICYCLE ); }
  // ����ȊO�Ȃ�, �]�[����BGM
  return GetZoneBGM( gameData, zoneID );
}

#include "tr_tool/trtype_def.h"
#include "../../../resource/trtype_bgm/trtype_bgm.cdat" 
//---------------------------------------------------------------------------------
/**
 * @brief �g���[�i�[������ �擾
 *
 * @param trType �g���[�i�[�^�C�v
 *
 * @return �w�肵���g���[�i�[�^�C�v�̎�����
 */
//---------------------------------------------------------------------------------
u32 FIELD_SOUND_GetTrainerEyeBGM( u32 trType )
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
  
  OS_TFPrintf( FSND_PRINT_NO, "ERROR:UNKNOW TRAINER TYPE BGM\n" );
  GF_ASSERT( 0 );
  return SEQ_BGM_EYE_01;
}

//---------------------------------------------------------------------------------
/**
 * @brief BGM�v�b�V���񐔂��擾
 *
 * @param fieldSound
 *
 * @return �ς܂�Ă���BGM�̐�
 */
//---------------------------------------------------------------------------------
FSND_PUSHCOUNT FIELD_SOUND_GetBGMPushCount( const FIELD_SOUND* fieldSound )
{
  return fieldSound->pushCount;
}

//---------------------------------------------------------------------------------
/**
 * @brief BGM�t�F�[�h���o
 *
 * @param fieldSound
 * 
 * @return �t�F�[�h���Ȃ� TRUE
 *         �����łȂ���� FALSE
 */
//---------------------------------------------------------------------------------
BOOL FIELD_SOUND_IsBGMFade( const FIELD_SOUND* fieldSound )
{
  if( (fieldSound->state == FSND_STATE_FADE_IN) ||
      (fieldSound->state == FSND_STATE_FADE_OUT) ||
      (fieldSound->state == FSND_STATE_POP_out) ||
      (fieldSound->state == FSND_STATE_POP_in) ||
      (fieldSound->state == FSND_STATE_PUSH) ||
      (fieldSound->state == FSND_STATE_CHANGE_out) ||
      (fieldSound->state == FSND_STATE_CHANGE_in) ||
      (fieldSound->state == FSND_STATE_CHANGE_PUSH_out) ||
      (fieldSound->state == FSND_STATE_STAND_BY_out) )
  {
    return TRUE;
  } 
  return FALSE;
}


//================================================================================= 
// ������J�֐�
//================================================================================= 

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
  u16 sound_idx;
  u8 season_id;

  season_id = GAMEDATA_GetSeasonID( gameData );
  sound_idx = ZONEDATA_GetBGMID( zoneID, season_id );
  return( sound_idx );
}

//---------------------------------------------------------------------------------
/**
 * @brief �t�F�[�h�C�� �t���[�������擾����
 *
 * @param fadeInSpeed �t�F�[�h�C�����x
 *
 * @return �w�肵�����x�̃t�F�[�h�C�� �t���[����
 */
//---------------------------------------------------------------------------------
static u16 GetFadeInFrame( FSND_FADEIN_SPEED fadeInSpeed )
{
  return FadeInFrame[ fadeInSpeed ];
}

//---------------------------------------------------------------------------------
/**
 * @brief �t�F�[�h�A�E�g �t���[�������擾����
 *
 * @param fadeInSpeed �t�F�[�h�A�E�g���x
 *
 * @return �w�肵�����x�̃t�F�[�h�A�E�g �t���[����
 */
//---------------------------------------------------------------------------------
static u16 GetFadeOutFrame( FSND_FADEOUT_SPEED fadeOutSpeed )
{
  return FadeOutFrame[ fadeOutSpeed ];
}

//---------------------------------------------------------------------------------
/**
 * @brief ���N�G�X�g�̔��s ( FSND_BGM_REQUEST_FADE_IN )
 *
 * @param fieldSound
 * @param fadeInFrame �t�F�[�h�C�� �t���[����
 */
//---------------------------------------------------------------------------------
static void ThrowRequest_FADE_IN( FIELD_SOUND* fieldSound, u16 fadeInFrame )
{
  // ���N�G�X�g���󗝂ł��Ȃ�
  if( CanAcceptRequest == FALSE )
  {
    OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: Can't Accept BGM fade in request\n" );
    GF_ASSERT(0);
    return;
  }

  // ���N�G�X�g����
  fieldSound->request = FSND_BGM_REQUEST_FADE_IN;
  fieldSound->fadeInFrame = fadeInFrame;

#ifdef DEBUG_ONLY_FOR_obata_toshihiro
  OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: accept request(FADE_IN)\n" );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief ���N�G�X�g�̔��s ( FSND_BGM_REQUEST_FADE_OUT )
 *
 * @param fieldSound
 * @param fadeOutFrame �t�F�[�h�A�E�g �t���[����
 */
//---------------------------------------------------------------------------------
static void ThrowRequest_FADE_OUT( FIELD_SOUND* fieldSound, u16 fadeOutFrame )
{
  // ���N�G�X�g���󗝂ł��Ȃ�
  if( CanAcceptRequest == FALSE )
  {
    OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: Can't Accept BGM fade out request\n" );
    GF_ASSERT(0);
    return;
  }

  // ���N�G�X�g����
  fieldSound->request = FSND_BGM_REQUEST_FADE_OUT;
  fieldSound->fadeOutFrame = fadeOutFrame;

#ifdef DEBUG_ONLY_FOR_obata_toshihiro
  OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: accept request(FADE_OUT)\n" );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief ���N�G�X�g�̔��s ( FSND_BGM_REQUEST_PUSH )
 *
 * @param fieldSound
 * @param fadeOutFrame �t�F�[�h�A�E�g �t���[����
 */
//---------------------------------------------------------------------------------
static void ThrowRequest_PUSH( FIELD_SOUND* fieldSound, u16 fadeOutFrame )
{
  // ���N�G�X�g���󗝂ł��Ȃ�
  if( CanAcceptRequest == FALSE )
  {
    OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: Can't Accept BGM push request\n" );
    GF_ASSERT(0);
    return;
  }

  // ���N�G�X�g����
  fieldSound->request = FSND_BGM_REQUEST_PUSH;
  fieldSound->fadeOutFrame = fadeOutFrame;

#ifdef DEBUG_ONLY_FOR_obata_toshihiro
  OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: accept request(PUSH)\n" );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief ���N�G�X�g�̔��s ( FSND_BGM_REQUEST_POP )
 *
 * @param fieldSound
 * @param fadeOutFrame �Đ�����BGM�̃t�F�[�h�A�E�g �t���[����
 * @param fadeInFrame  ���A����BGM�̃t�F�[�h�C�� �t���[����
 */
//---------------------------------------------------------------------------------
static void ThrowRequest_POP( FIELD_SOUND* fieldSound,
                              u16 fadeOutFrame, u16 fadeInFrame )
{
  // ���N�G�X�g���󗝂ł��Ȃ�
  if( CanAcceptRequest == FALSE )
  {
    OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: Can't Accept BGM pop request\n" );
    GF_ASSERT(0);
    return;
  }

  // ���N�G�X�g����
  fieldSound->request = FSND_BGM_REQUEST_POP;
  fieldSound->fadeInFrame = fadeInFrame;
  fieldSound->fadeOutFrame = fadeOutFrame;

#ifdef DEBUG_ONLY_FOR_obata_toshihiro
  OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: accept request(POP)\n" );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief ���N�G�X�g�̔��s ( FSND_BGM_REQUEST_CHANGE )
 *
 * @param fieldSound
 * @param soundIdx     �Đ�����BGM
 * @param fadeOutFrame �Đ�����BGM�̃t�F�[�h�A�E�g �t���[����
 * @param fadeInFrame  �Đ�����BGM�̃t�F�[�h�C�� �t���[����
 */
//---------------------------------------------------------------------------------
static void ThrowRequest_CHANGE( FIELD_SOUND* fieldSound, u32 soundIdx, 
                                 u16 fadeOutFrame, u16 fadeInFrame )
{
  // ���N�G�X�g���󗝂ł��Ȃ�
  if( CanAcceptRequest == FALSE )
  {
    OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: Can't Accept BGM change request\n" );
    GF_ASSERT(0);
    return;
  } 
  // �Đ�����BGM���w��
  if( fieldSound->currentBGM == soundIdx ){ return; } 

  // ���N�G�X�g����
  fieldSound->request = FSND_BGM_REQUEST_CHANGE;
  fieldSound->requestBGM = soundIdx;
  fieldSound->fadeInFrame = fadeInFrame;
  fieldSound->fadeOutFrame = fadeOutFrame;

#ifdef DEBUG_ONLY_FOR_obata_toshihiro
  OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: accept request(CHANGE)\n" );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief ���N�G�X�g�̔��s ( FSND_BGM_REQUEST_STAND_BY )
 *
 * @param fieldSound
 * @param soundIdx     �Đ��̏��������邷��BGM
 * @param fadeOutFrame �Đ�����BGM�̃t�F�[�h�A�E�g �t���[����
 */
//---------------------------------------------------------------------------------
static void ThrowRequest_STAND_BY( FIELD_SOUND* fieldSound, 
                                   u32 soundIdx, u16 fadeOutFrame )
{
  // ���N�G�X�g���󗝂ł��Ȃ�
  if( CanAcceptRequest == FALSE )
  {
    OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: Can't Accept BGM stand-by request\n" );
    GF_ASSERT(0);
    return;
  }
  // �Đ�����BGM���w��
  if( fieldSound->currentBGM == soundIdx ){ return; } 

  // ���N�G�X�g����
  fieldSound->request = FSND_BGM_REQUEST_STAND_BY;
  fieldSound->requestBGM = soundIdx;
  fieldSound->fadeOutFrame = fadeOutFrame;
  
#ifdef DEBUG_ONLY_FOR_obata_toshihiro
  OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: accept request(STAND_BY)\n" );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief ���N�G�X�g�̔��s ( FSND_BGM_REQUEST_FORCE_PLAY )
 *
 * @param fieldSound
 * @param soundIdx   �Đ�����BGM
 */
//---------------------------------------------------------------------------------
static void ThrowRequest_FORCE_PLAY( FIELD_SOUND* fieldSound, u16 soundIdx )
{
  // ���N�G�X�g���󗝂ł��Ȃ�
  if( CanAcceptRequest == FALSE )
  {
    OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: Can't Accept BGM force play request\n" );
    GF_ASSERT(0);
    return;
  }

  // ���N�G�X�g����
  fieldSound->request = FSND_BGM_REQUEST_FORCE_PLAY;
  fieldSound->requestBGM = soundIdx;

#ifdef DEBUG_ONLY_FOR_obata_toshihiro
  OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: accept request(FORCE_PLAY)\n" );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief ���N�G�X�g���󗝂ł��邩�ǂ����𔻒肷��
 *
 * @param fieldSound
 * @param request    �`�F�b�N���郊�N�G�X�g
 *
 * @return �w�肵�����N�G�X�g���󗝂ł����ԂȂ� TRUE
 *         �����łȂ����                         FALSE
 */
//---------------------------------------------------------------------------------
static BOOL CanAcceptRequest( const FIELD_SOUND* fieldSound, FSND_BGM_REQUEST request )
{
  return RequestAcceptTable[ fieldSound->state ][ request ];
}

//---------------------------------------------------------------------------------
/**
 * @brief ������� STOP ����, �e�탊�N�G�X�g�ɑ΂��铮��
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void RequestCheck_STOP( FIELD_SOUND* fieldSound )
{
  switch( fieldSound->request )
  {
  case FSND_BGM_REQUEST_NONE:  
    break;
  case FSND_BGM_REQUEST_POP:
    PopBGM( fieldSound );
    ChangeState( fieldSound, FSND_STATE_POP_in );
    break;
  case FSND_BGM_REQUEST_CHANGE:  
    DivLoadBGM_start( fieldSound );
    ChangeState( fieldSound, FSND_STATE_CHANGE_load );
    break;
  case FSND_BGM_REQUEST_STAND_BY:  
    ChangeState( fieldSound, FSND_STATE_STAND_BY_load );
    break;
  case FSND_BGM_REQUEST_FORCE_PLAY:
    ForcePlayBGM( fieldSound );
    ChangeState( fieldSound, FSND_STATE_PLAY );
    RequestFinished( fieldSound );
    break;
  default:  
    GF_ASSERT(0);
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief ������� PLAY ����, �e�탊�N�G�X�g�ɑ΂��铮��
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void RequestCheck_PLAY( FIELD_SOUND* fieldSound )
{
  switch( fieldSound->request )
  {
  case FSND_BGM_REQUEST_NONE: 
    break;
  case FSND_BGM_REQUEST_FADE_OUT: 
    FadeOutBGM( fieldSound );
    ChangeState( fieldSound, FSND_STATE_FADE_OUT );
    break;
  case FSND_BGM_REQUEST_PUSH: 
    ChangeState( fieldSound, FSND_STATE_PUSH );
    break;
  case FSND_BGM_REQUEST_POP: 
    FadeOutBGM( fieldSound );
    ChangeState( fieldSound, FSND_STATE_POP_out );
    break;
  case FSND_BGM_REQUEST_CHANGE: 
    FadeOutBGM( fieldSound );
    ChangeState( fieldSound, FSND_STATE_CHANGE_out );
    break;
  case FSND_BGM_REQUEST_STAND_BY: 
    FadeOutBGM( fieldSound );
    ChangeState( fieldSound, FSND_STATE_STAND_BY_out );
    break;
  case FSND_BGM_REQUEST_FORCE_PLAY:
    ForcePlayBGM( fieldSound );
    ChangeState( fieldSound, FSND_STATE_PLAY );
    RequestFinished( fieldSound );
    break;
  default:
    GF_ASSERT(0);
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief ������� WAIT ����, �e�탊�N�G�X�g�ɑ΂��铮��
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void RequestCheck_WAIT( FIELD_SOUND* fieldSound )
{
  switch( fieldSound->request )
  {
  case FSND_BGM_REQUEST_NONE: 
    break;
  case FSND_BGM_REQUEST_FADE_IN: 
    FadeInBGM( fieldSound );
    ChangeState( fieldSound, FSND_STATE_FADE_IN );
    break;
  case FSND_BGM_REQUEST_FORCE_PLAY: 
    ForcePlayBGM( fieldSound );
    ChangeState( fieldSound, FSND_STATE_PLAY );
    RequestFinished( fieldSound );
    break;
  default:
    GF_ASSERT(0);
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief ������� FADE_OUT ����, �e�탊�N�G�X�g�ɑ΂��铮��
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void RequestCheck_FADE_OUT( FIELD_SOUND* fieldSound )
{
  switch( fieldSound->request )
  {
  case FSND_BGM_REQUEST_NONE: 
    break;
  case FSND_BGM_REQUEST_FADE_OUT: 
    break;
  default:
    GF_ASSERT(0);
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief ������� FADE_IN ����, �e�탊�N�G�X�g�ɑ΂��铮��
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void RequestCheck_FADE_IN( FIELD_SOUND* fieldSound )
{
  switch( fieldSound->request )
  {
  case FSND_BGM_REQUEST_NONE: 
    break;
  case FSND_BGM_REQUEST_FADE_IN: 
    break;
  default:
    GF_ASSERT(0);
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief ������� PUSH ����, �e�탊�N�G�X�g�ɑ΂��铮��
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void RequestCheck_PUSH( FIELD_SOUND* fieldSound )
{
  switch( fieldSound->request )
  {
  case FSND_BGM_REQUEST_NONE: 
    break;
  case FSND_BGM_REQUEST_PUSH:
    break;
  default:
    GF_ASSERT(0);
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief ������� POP_out ����, �e�탊�N�G�X�g�ɑ΂��铮��
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void RequestCheck_POP_out( FIELD_SOUND* fieldSound )
{
  switch( fieldSound->request )
  {
  case FSND_BGM_REQUEST_NONE: 
    break;
  case FSND_BGM_REQUEST_POP:
    break;
  default:
    GF_ASSERT(0);
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief ������� POP_in ����, �e�탊�N�G�X�g�ɑ΂��铮��
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void RequestCheck_POP_in( FIELD_SOUND* fieldSound )
{
  switch( fieldSound->request )
  {
  case FSND_BGM_REQUEST_NONE: 
    break;
  case FSND_BGM_REQUEST_POP:
    break;
  default:
    GF_ASSERT(0);
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief ������� CHANGE_out ����, �e�탊�N�G�X�g�ɑ΂��铮��
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void RequestCheck_CHANGE_out( FIELD_SOUND* fieldSound )
{
  switch( fieldSound->request )
  {
  case FSND_BGM_REQUEST_NONE: 
    break;
  case FSND_BGM_REQUEST_PUSH: 
    ChangeState( fieldSound, FSND_STATE_CHANGE_PUSH_out);
    break;
  case FSND_BGM_REQUEST_CHANGE:
    break;
  default:
    GF_ASSERT(0);
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief ������� CHANGE_load ����, �e�탊�N�G�X�g�ɑ΂��铮��
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void RequestCheck_CHANGE_load( FIELD_SOUND* fieldSound )
{
  switch( fieldSound->request )
  {
  case FSND_BGM_REQUEST_NONE: 
    break;
  case FSND_BGM_REQUEST_PUSH: 
    ChangeState( fieldSound, FSND_STATE_CHANGE_PUSH_load );
    break;
  case FSND_BGM_REQUEST_CHANGE:
    break;
  default:
    GF_ASSERT(0);
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief ������� CHANGE_in ����, �e�탊�N�G�X�g�ɑ΂��铮��
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void RequestCheck_CHANGE_in( FIELD_SOUND* fieldSound )
{
  switch( fieldSound->request )
  {
  case FSND_BGM_REQUEST_NONE: 
    break;
  case FSND_BGM_REQUEST_CHANGE:
    break;
  default:
    GF_ASSERT(0);
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief ������� CHANGE_PUSH_out ����, �e�탊�N�G�X�g�ɑ΂��铮��
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void RequestCheck_CHANGE_PUSH_out ( FIELD_SOUND* fieldSound )
{
  switch( fieldSound->request )
  {
  case FSND_BGM_REQUEST_NONE: 
    break;
  case FSND_BGM_REQUEST_PUSH:
    break;
  default:
    GF_ASSERT(0);
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief ������� CHANGE_PUSH_load ����, �e�탊�N�G�X�g�ɑ΂��铮��
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void RequestCheck_CHANGE_PUSH_load( FIELD_SOUND* fieldSound )
{
  switch( fieldSound->request )
  {
  case FSND_BGM_REQUEST_NONE: 
    break;
  case FSND_BGM_REQUEST_PUSH:
    break;
  default:
    GF_ASSERT(0);
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief ������� STAND_BY_out ����, �e�탊�N�G�X�g�ɑ΂��铮��
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void RequestCheck_STAND_BY_out( FIELD_SOUND* fieldSound )
{
  switch( fieldSound->request )
  {
  case FSND_BGM_REQUEST_NONE: 
    break;
  case FSND_BGM_REQUEST_STAND_BY:
    break;
  default:
    GF_ASSERT(0);
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief ������� STAND_BY_load ����, �e�탊�N�G�X�g�ɑ΂��铮��
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void RequestCheck_STAND_BY_load( FIELD_SOUND* fieldSound )
{
  switch( fieldSound->request )
  {
  case FSND_BGM_REQUEST_NONE: 
    break;
  case FSND_BGM_REQUEST_STAND_BY:
    break;
  default:
    GF_ASSERT(0);
  }
}


//---------------------------------------------------------------------------------
/**
 * @brief ������� STOP ���̓���
 */
//---------------------------------------------------------------------------------
static void Main_STOP( FIELD_SOUND* fieldSound )
{ 
}

//---------------------------------------------------------------------------------
/**
 * @brief ������� PLAY ���̓���
 */
//---------------------------------------------------------------------------------
static void Main_PLAY( FIELD_SOUND* fieldSound )
{
}

//---------------------------------------------------------------------------------
/**
 * @brief ������� WAIT ���̓���
 */
//---------------------------------------------------------------------------------
static void Main_WAIT( FIELD_SOUND* fieldSound )
{
}

//---------------------------------------------------------------------------------
/**
 * @brief ������� FADE_OUT ���̓���
 */
//---------------------------------------------------------------------------------
static void Main_FADE_OUT( FIELD_SOUND* fieldSound )
{
  if( PMSND_CheckFadeOnBGM() == FALSE )
  {
    ChangeState( fieldSound, FSND_STATE_WAIT );
    RequestFinished( fieldSound );
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief ������� FADE_IN ���̓���
 */
//---------------------------------------------------------------------------------
static void Main_FADE_IN( FIELD_SOUND* fieldSound )
{
  if( PMSND_CheckFadeOnBGM() == FALSE )
  {
    ChangeState( fieldSound, FSND_STATE_PLAY );
    RequestFinished( fieldSound );
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief ������� PUSH ���̓���
 */
//---------------------------------------------------------------------------------
static void Main_PUSH( FIELD_SOUND* fieldSound )
{
  if( PMSND_CheckFadeOnBGM() == FALSE )
  {
    PushBGM( fieldSound );
    ChangeState( fieldSound, FSND_STATE_STOP );
    RequestFinished( fieldSound );
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief ������� POP_out ���̓���
 */
//---------------------------------------------------------------------------------
static void Main_POP_out( FIELD_SOUND* fieldSound )
{
  if( PMSND_CheckFadeOnBGM() == FALSE )
  {
    PopBGM( fieldSound );
    FadeInBGM( fieldSound );
    ChangeState( fieldSound, FSND_STATE_POP_in );
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief ������� POP_in ���̓���
 */
//---------------------------------------------------------------------------------
static void Main_POP_in( FIELD_SOUND* fieldSound )
{
  if( PMSND_CheckFadeOnBGM() == FALSE )
  {
    ChangeState( fieldSound, FSND_STATE_PLAY );
    RequestFinished( fieldSound );
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief ������� CHANGE_out ���̓���
 */
//---------------------------------------------------------------------------------
static void Main_CHANGE_out( FIELD_SOUND* fieldSound )
{
  if( PMSND_CheckFadeOnBGM() == FALSE )
  {
    DivLoadBGM_start( fieldSound );
    ChangeState( fieldSound, FSND_STATE_CHANGE_load );
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief ������� CHANGE_load ���̓���
 */
//---------------------------------------------------------------------------------
static void Main_CHANGE_load( FIELD_SOUND* fieldSound )
{
  BOOL loadFinished;

  // �������[�h���s
  loadFinished = DivLoadBGM_load( fieldSound );

  // �������[�h����(�Đ��J�n)
  if( loadFinished )
  { 
    // �t�F�[�h�C���J�n
    FadeInBGM( fieldSound );
    ChangeState( fieldSound, FSND_STATE_CHANGE_in );
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief ������� CHANGE_in ���̓���
 */
//---------------------------------------------------------------------------------
static void Main_CHANGE_in( FIELD_SOUND* fieldSound )
{
  // �t�F�[�h�C������
  if( PMSND_CheckFadeOnBGM() == FALSE )
  {
    // �Đ��J�n�ȍ~�� BGM�ύX���N�G�X�g���󂯂��ꍇ
    if( fieldSound->requestBGM != FSND_BGM_NULL )
    {
      // �t�F�[�h�A�E�g�J�n
      FadeOutBGM( fieldSound );
      ChangeState( fieldSound, FSND_STATE_CHANGE_out );
    }
    else
    {
      ChangeState( fieldSound, FSND_STATE_PLAY );
      RequestFinished( fieldSound );
    }
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief ������� CHANGE_PUSH_out ���̓���
 */
//---------------------------------------------------------------------------------
static void Main_CHANGE_PUSH_out( FIELD_SOUND* fieldSound )
{
  if( PMSND_CheckFadeOnBGM() == FALSE )
  {
    DivLoadBGM_start( fieldSound );
    ChangeState( fieldSound, FSND_STATE_CHANGE_PUSH_load );
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief ������� CHANGE_PUSH_load ���̓���
 */
//---------------------------------------------------------------------------------
static void Main_CHANGE_PUSH_load( FIELD_SOUND* fieldSound )
{
  BOOL loadFinished;

  // �������[�h���s
  loadFinished = DivLoadBGM_load( fieldSound );

  // �������[�h����(�Đ��J�n)
  if( loadFinished )
  { 
    // BGM �ޔ�
    PushBGM( fieldSound );
    ChangeState( fieldSound, FSND_STATE_STOP );
    RequestFinished( fieldSound );
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief ������� STAND_BY_out ���̓���
 */
//---------------------------------------------------------------------------------
static void Main_STAND_BY_out( FIELD_SOUND* fieldSound )
{
  if( PMSND_CheckFadeOnBGM() == FALSE )
  {
    DivLoadBGM_start( fieldSound );
    ChangeState( fieldSound, FSND_STATE_STAND_BY_load );
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief ������� STAND_BY_load ���̓���
 */
//---------------------------------------------------------------------------------
static void Main_STAND_BY_load( FIELD_SOUND* fieldSound )
{
  BOOL loadFinished;

  // �������[�h���s
  loadFinished = DivLoadBGM_load( fieldSound );

  // �������[�h����(�Đ��J�n)
  if( loadFinished )
  { 
    // BGM�ꎞ��~
    PMSND_PauseBGM( TRUE );
    ChangeState( fieldSound, FSND_STATE_WAIT );
    RequestFinished( fieldSound );
  }
} 

//---------------------------------------------------------------------------------
/**
 * @brief ������Ԃ�ύX����
 *
 * @param fieldSound 
 * @param nextState  �J�ڐ�̏��
 */
//---------------------------------------------------------------------------------
static void ChangeState( FIELD_SOUND* fieldSound, FSND_STATE nextState )
{
#ifdef DEBUG_ONLY_FOR_obata_toshihiro
  switch( fieldSound->state )
  {
  case FSND_STATE_STOP:              OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: change state (STOP ==> " );  break;
  case FSND_STATE_PLAY:              OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: change state (PLAY ==> " );  break;
  case FSND_STATE_WAIT:              OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: change state (WAIT ==> " );  break;
  case FSND_STATE_FADE_OUT:          OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: change state (FADE OUT ==> " );  break;
  case FSND_STATE_FADE_IN:           OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: change state (FADE IN ==> " );  break;
  case FSND_STATE_PUSH:              OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: change state (PUSH ==> " );  break;
  case FSND_STATE_POP_out:           OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: change state (POP out ==> " );  break;
  case FSND_STATE_POP_in:            OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: change state (POP in ==> " );  break;
  case FSND_STATE_CHANGE_out:        OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: change state (CHANGE out ==> " );  break;
  case FSND_STATE_CHANGE_load:       OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: change state (CHANGE load ==> " );  break;
  case FSND_STATE_CHANGE_in:         OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: change state (CHANGE in ==> " );  break;
  case FSND_STATE_CHANGE_PUSH_out:   OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: change state (CHANGE PUSH out ==> " );  break;
  case FSND_STATE_CHANGE_PUSH_load:  OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: change state (CHANGE PUSH load ==> " );  break;
  case FSND_STATE_STAND_BY_out:      OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: change state (STAND BY out ==> " );  break;
  case FSND_STATE_STAND_BY_load:     OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: change state (STAND BY load ==> " );  break;
  default: GF_ASSERT(0);
  }
  switch( nextState )
  {
  case FSND_STATE_STOP:              OS_TFPrintf( FSND_PRINT_NO, "STOP)\n" );  break;
  case FSND_STATE_PLAY:              OS_TFPrintf( FSND_PRINT_NO, "PLAY)\n" );  break;
  case FSND_STATE_WAIT:              OS_TFPrintf( FSND_PRINT_NO, "WAIT)\n" );  break;
  case FSND_STATE_FADE_OUT:          OS_TFPrintf( FSND_PRINT_NO, "FADE OUT)\n" );  break;
  case FSND_STATE_FADE_IN:           OS_TFPrintf( FSND_PRINT_NO, "FADE IN)\n" );  break;
  case FSND_STATE_PUSH:              OS_TFPrintf( FSND_PRINT_NO, "PUSH)\n" );  break;
  case FSND_STATE_POP_out:           OS_TFPrintf( FSND_PRINT_NO, "POP out)\n" );  break;
  case FSND_STATE_POP_in:            OS_TFPrintf( FSND_PRINT_NO, "POP in)\n" );  break;
  case FSND_STATE_CHANGE_out:        OS_TFPrintf( FSND_PRINT_NO, "CHANGE out)\n" );  break;
  case FSND_STATE_CHANGE_load:       OS_TFPrintf( FSND_PRINT_NO, "CHANGE load)\n" );  break;
  case FSND_STATE_CHANGE_in:         OS_TFPrintf( FSND_PRINT_NO, "CHANGE in)\n" );  break;
  case FSND_STATE_CHANGE_PUSH_out:   OS_TFPrintf( FSND_PRINT_NO, "CHANGE PUSH out)\n" );  break;
  case FSND_STATE_CHANGE_PUSH_load:  OS_TFPrintf( FSND_PRINT_NO, "CHANGE PUSH load)\n" );  break;
  case FSND_STATE_STAND_BY_out:      OS_TFPrintf( FSND_PRINT_NO, "STAND BY out)\n" );  break;
  case FSND_STATE_STAND_BY_load:     OS_TFPrintf( FSND_PRINT_NO, "STAND BY load)\n" );  break;
  default: GF_ASSERT(0);
  }
#endif

  // �X�V
  fieldSound->state = nextState; 
}

//---------------------------------------------------------------------------------
/**
 * @brief ���N�G�X�g�ɑ΂��铮��̊�������
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void RequestFinished( FIELD_SOUND* fieldSound )
{
  // ���N�G�X�g�������Ă��Ȃ�
  if( fieldSound->request == FSND_BGM_REQUEST_NONE )
  {
    OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: Don't have BGM request\n" );
    GF_ASSERT(0);
    return;
  }

#ifdef DEBUG_ONLY_FOR_obata_toshihiro
  switch( fieldSound->request )
  {
  case FSND_BGM_REQUEST_FADE_IN:     OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: request(FADE IN) finished!!\n" );  break;
  case FSND_BGM_REQUEST_FADE_OUT:    OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: request(FADE OUT) finished!!\n" );  break;
  case FSND_BGM_REQUEST_PUSH:        OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: request(PUSH) finished!!\n" );  break;
  case FSND_BGM_REQUEST_POP:         OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: request(POP) finished!!\n" );  break;
  case FSND_BGM_REQUEST_CHANGE:      OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: request(CHANGE) finished!!\n" );  break;
  case FSND_BGM_REQUEST_STAND_BY:    OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: request(STAND BY) finished!!\n" );  break;
  case FSND_BGM_REQUEST_FORCE_PLAY:  OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: request(FORCE_PLAY) finished!!\n" );  break;
  }
#endif

  // ���������X�V
  fieldSound->request = FSND_BGM_REQUEST_NONE; 
}

//---------------------------------------------------------------------------------
/**
 * @brief BGM�̃t�F�[�h�C�����J�n����
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void FadeInBGM( FIELD_SOUND* fieldSound )
{
  // BGM�����Ă��Ȃ�
  if( fieldSound->currentBGM == FSND_BGM_NULL )
  {
    OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: Can't fade in BGM (Don't playing BGM)\n" );
    GF_ASSERT(0);
    return;
  } 

  PMSND_FadeInBGM( fieldSound->fadeInFrame );
  PMSND_PauseBGM( FALSE );

#ifdef DEBUG_ONLY_FOR_obata_toshihiro
  OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: BGM fade in start\n" );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief BGM�̃t�F�[�h�A�E�g���J�n����
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void FadeOutBGM( FIELD_SOUND* fieldSound )
{
  // BGM�����Ă��Ȃ�
  if( fieldSound->currentBGM == FSND_BGM_NULL )
  {
    OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: Can't fade out BGM (Don't playing BGM)\n" );
    GF_ASSERT(0);
    return;
  } 

  PMSND_FadeOutBGM( fieldSound->fadeOutFrame );

#ifdef DEBUG_ONLY_FOR_obata_toshihiro
  OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: BGM fade out start\n" );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief BGM��ޔ�����
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void PushBGM( FIELD_SOUND* fieldSound )
{
  // BGM�����Ă��Ȃ�
  if( fieldSound->currentBGM == FSND_BGM_NULL )
  {
    OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: Can't push BGM (Don't playing BGM)\n" );
    GF_ASSERT(0);
    return;
  } 
  // �ޔ𐔃I�[�o�[
  if( FSND_PUSHCOUNT_MAX <= fieldSound->pushCount )
  {
    OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: Can't push BGM (Push count over)\n" );
    GF_ASSERT(0);
    return;
  }

  // �ޔ�
  PMSND_PushBGM();

  // ���������X�V
  fieldSound->pushBGM[ fieldSound->pushCount ] = fieldSound->currentBGM;
  fieldSound->pushCount++;
  fieldSound->currentBGM = FSND_BGM_NULL; 

#ifdef DEBUG_ONLY_FOR_obata_toshihiro
  OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: push BGM(%d)\n", fieldSound->pushBGM[ fieldSound->pushCount-1 ] );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief BGM�𕜋A����
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void PopBGM( FIELD_SOUND* fieldSound )
{
  // �ޔ�����Ă��Ȃ�
  if( fieldSound->pushCount <= FSND_PUSHCOUNT_NONE )
  {
    OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: Can't pop BGM (There is no pushed BGM)\n" );
    GF_ASSERT(0);
    return;
  }

  // ���A
  PMSND_PopBGM();

  // ���������X�V
  fieldSound->pushCount--;
  fieldSound->currentBGM = fieldSound->pushBGM[ fieldSound->pushCount ];

#ifdef DEBUG_ONLY_FOR_obata_toshihiro
  OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: pop BGM(%d)\n", fieldSound->currentBGM );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief BGM�̕������[�h���J�n����
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void DivLoadBGM_start( FIELD_SOUND* fieldSound )
{
  // �ǂݍ���BGM���\�񂳂�Ă��Ȃ�
  if( fieldSound->requestBGM == FSND_BGM_NULL )
  {
    OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: Can't start load BGM (Don't know target BGM)\n" );
    GF_ASSERT(0);
    return;
  }

  // �������[�h�J�n
  PMSND_PlayBGMdiv( fieldSound->requestBGM, &(fieldSound->loadSeq), TRUE );

  // ���������X�V
  fieldSound->loadBGM = fieldSound->requestBGM;
  fieldSound->requestBGM = FSND_BGM_NULL;
  fieldSound->currentBGM = FSND_BGM_NULL;

#ifdef DEBUG_ONLY_FOR_obata_toshihiro
  OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: div load BGM(%d) start\n", fieldSound->loadBGM );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief BGM�̕������[�h��i�߂�
 *
 * @param fieldSound
 *
 * @return �������[�h���I����, �Đ����J�n���ꂽ�ꍇ TRUE
 *         �������[�h�������Ȃ�@FALSE
 */
//---------------------------------------------------------------------------------
static BOOL DivLoadBGM_load( FIELD_SOUND* fieldSound )
{
  BOOL loadFinished;

  // ���[�h���J�n����Ă��Ȃ�
  if( fieldSound->loadBGM == FSND_BGM_NULL )
  {
    OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: Can't load BGM (Don't know target BGM)\n" );
    GF_ASSERT(0); 
  }

  // �������[�h���s
  loadFinished = PMSND_PlayBGMdiv( fieldSound->loadBGM, &(fieldSound->loadSeq), FALSE );

  // �������[�h����(�Đ��J�n)
  if( loadFinished )
  {
    // ���������X�V
    fieldSound->currentBGM = fieldSound->loadBGM;
    fieldSound->loadBGM = FSND_BGM_NULL;

#ifdef DEBUG_ONLY_FOR_obata_toshihiro
    OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: div load BGM(%d) finish\n", fieldSound->currentBGM );
#endif
  } 
  return loadFinished;
}

//---------------------------------------------------------------------------------
/**
 * @brief BGM�̑����Đ����s��
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void ForcePlayBGM( FIELD_SOUND* fieldSound )
{
  // �Đ�����BGM���w�肳��Ă��Ȃ�
  if( fieldSound->requestBGM == FSND_BGM_NULL )
  {
    OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: Can't force play BGM (Don't know target BGM)\n" );
    GF_ASSERT(0);
    return;
  }

  // �����Đ�
  PMSND_PlayBGM( fieldSound->requestBGM );

  // ���������X�V
  fieldSound->currentBGM = fieldSound->requestBGM;
  fieldSound->requestBGM = FSND_BGM_NULL;
  fieldSound->loadBGM = FSND_BGM_NULL;

#ifdef DEBUG_ONLY_FOR_obata_toshihiro
  OS_TFPrintf( FSND_PRINT_NO, "FIELD-SOUND: force play BGM(%d) finish\n", fieldSound->currentBGM );
#endif
} 
