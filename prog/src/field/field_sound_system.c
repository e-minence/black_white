///////////////////////////////////////////////////////////////////////////////////
/**
 * @brief �t�B�[���h�̃T�E���h����
 * @file  field_sound_system.c
 * @author obata
 * @date   2009.12.25
 */
///////////////////////////////////////////////////////////////////////////////////
#include "field_sound_system.h"
#include "sound/pm_sndsys.h"


//================================================================================= 
// ���萔
//================================================================================= 
#define PRINT_NO            (1)  // printf�o�͐�R���\�[���ԍ�
#define BGM_NONE            (0)  // BGM�ԍ��̖����l
#define REQUEST_QUEUE_SIZE (10)  // �o�b�t�@�ł��郊�N�G�X�g�̐�

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

//---------------------------------------------------------------------------------
// �e������Ԃɑ΂���, ���N�G�X�g�̉s�e�[�u��
//
// RequestAcceptTable[ �V�X�e����� ][ ���N�G�X�g ] �� TRUE �̎�,
// �V�X�e���� ���̃��N�G�X�g���������邱�Ƃ��o����.
//
static BOOL RequestAcceptTable[ FSND_STATE_NUM ][ FSND_BGM_REQUEST_NUM ] = 
{
                        /* NONE    F_IN    F_OUT   PUSH    POP     CHANGE  S_BY    F_PLAY */
  /* STOP             */{  FALSE,  FALSE,  FALSE,  FALSE,   TRUE,   TRUE,   TRUE,   TRUE,  },
  /* PLAY             */{  FALSE,   TRUE,   TRUE,   TRUE,   TRUE,   TRUE,   TRUE,   TRUE,  },
  /* WAIT             */{  FALSE,   TRUE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,   TRUE,  },
  /* FADE_OUT         */{  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  },
  /* FALDE_IN         */{  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  },
  /* PUSH             */{  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  },
  /* POP_out          */{  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  },
  /* POP_in           */{  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  },
  /* CHANGE_out       */{  FALSE,  FALSE,  FALSE,   TRUE,  FALSE,   TRUE,  FALSE,  FALSE,  },
  /* CHANGE_load      */{  FALSE,  FALSE,  FALSE,   TRUE,  FALSE,   TRUE,  FALSE,  FALSE,  },
  /* CHANGE_in        */{  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,   TRUE,  FALSE,  FALSE,  },
  /* CHANGE_PUSH_out  */{  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  },
  /* CHANGE_PUSH_load */{  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  },
  /* STAND_BY_out     */{  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  },
  /* STAND_BY_load    */{  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  },
};

//---------------------------------------------------------------------------------
//  �V�K���N�G�X�g�ɂ��f�b�h���b�N �e�[�u��
//
//  DeadRockTable[ �ŏI���N�G�X�g ][ �V�K���N�G�X�g ] �� FALSE �̎�, 
//  �V�K���N�G�X�g���󗝂��邱�Ƃ��o������, �f�b�h���b�N��ԂƂȂ�.
//
static BOOL DeadRockTable[ FSND_BGM_REQUEST_NUM ][ FSND_BGM_REQUEST_NUM ] = 
{
              /* NONE    F_IN    F_OUT   PUSH    POP     CHANGE  S_BY    F_PLAY */
  /* NONE   */{  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,  }, 
  /* F_IN   */{  FALSE,   TRUE,   TRUE,   TRUE,   TRUE,   TRUE,   TRUE,   TRUE,  },
  /* F_OUT  */{  FALSE,   TRUE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,   TRUE,  },
  /* PUSH   */{  FALSE,  FALSE,  FALSE,  FALSE,   TRUE,   TRUE,   TRUE,   TRUE,  },
  /* POP    */{  FALSE,   TRUE,   TRUE,   TRUE,   TRUE,   TRUE,   TRUE,   TRUE,  },
  /* CHANGE */{  FALSE,   TRUE,   TRUE,   TRUE,   TRUE,   TRUE,   TRUE,   TRUE,  },
  /* S_BY   */{  FALSE,   TRUE,  FALSE,  FALSE,  FALSE,  FALSE,  FALSE,   TRUE,  },
  /* F_PLAY */{  FALSE,   TRUE,   TRUE,   TRUE,   TRUE,   TRUE,   TRUE,   TRUE,  },
};


//=================================================================================
// �����N�G�X�g�̏ڍ׃f�[�^
//=================================================================================
typedef struct 
{
  FSND_BGM_REQUEST request;  // ���N�G�X�g�̎��
  u32             soundIdx;  // BGM No.
  u16         fadeOutFrame;  // �Đ�����BGM�̃t�F�[�h�A�E�g �t���[����
  u16          fadeInFrame;  // �Đ�����BGM�̃t�F�[�h�C�� �t���[����

} FSND_REQUEST_DATA;


//================================================================================= 
// ���t�B�[���h�T�E���h�Ǘ����[�N
//================================================================================= 
struct _FIELD_SOUND
{
  // �������
  FSND_STATE state;  // ���݂̏��

  // �������
  u32  currentBGM;    // ���ݍĐ�����BGM
  u32  requestBGM;    // ���N�G�X�g���ꂽBGM
  u32  loadBGM;       // �������[�h����BGM
  u32  loadSeq;       // �������[�h�̃V�[�P���X
  u16  fadeInFrame;   // �t�F�[�h�C�� �t���[���� 
  u16  fadeOutFrame;  // �t�F�[�h�A�E�g �t���[����
  FSND_BGM_REQUEST request;  // �������̃��N�G�X�g

  // BGM �ޔ�
  FSND_PUSHCOUNT pushCount;                    // �ޔ�
  u32            pushBGM[FSND_PUSHCOUNT_MAX];  // �ޔ𒆂�BGN No.

  // ���N�G�X�g�L���[
  FSND_REQUEST_DATA requestData[ REQUEST_QUEUE_SIZE ];  // ���N�G�X�g�z��
  u8                requestHeadPos;  // �L���[�̐擪�ʒu( �擪���N�G�X�g�̈ʒu )
  u8                requestTailPos;  // �L���[�̖����ʒu( �V�K���N�G�X�g�̒ǉ��ʒu )
};


//================================================================================= 
// ������J�֐�
//================================================================================= 
// �V�X�e���̏�����
static void InitFieldSoundSystem( FIELD_SOUND* fieldSound );
static void ResetFieldSoundSystem( FIELD_SOUND* fieldSound );

// ���N�G�X�g�L���[�̑���
static void RequestQueueCheck( FIELD_SOUND* fieldSound );
static const FSND_REQUEST_DATA* GetHeadRequest( const FIELD_SOUND* fieldSound );
static const FSND_REQUEST_DATA* GetNextRequest( const FIELD_SOUND* fieldSound );
static const FSND_REQUEST_DATA* GetTailRequest( const FIELD_SOUND* fieldSound );
static BOOL QueueHaveRequest( const FIELD_SOUND* fieldSound );
static void RegisterNewRequest( FIELD_SOUND* fieldSound, const FSND_REQUEST_DATA* requestData );
static void RemoveHeadRequest( FIELD_SOUND* fieldSound );
// �L���[�Ƀ��N�G�X�g��o�^�ł��邩�ǂ����̔���
static BOOL CanRegisterRequest( const FIELD_SOUND* fieldSound, FSND_BGM_REQUEST request );

// ���N�G�X�g�̔��s
static void ThrowRequest( FIELD_SOUND* fieldSound, const FSND_REQUEST_DATA* requestData );
static void ThrowRequest_FADE_IN( FIELD_SOUND* fieldSound, u16 fadeInFrame );
static void ThrowRequest_FADE_OUT( FIELD_SOUND* fieldSound, u16 fadeOutFrame );
static void ThrowRequest_PUSH( FIELD_SOUND* fieldSound, u16 fadeOutFrame );
static void ThrowRequest_POP( FIELD_SOUND* fieldSound, u16 fadeOutFrame, u16 fadeInFrame );
static void ThrowRequest_CHANGE( FIELD_SOUND* fieldSound, u32 soundIdx, 
                                 u16 fadeOutFrame, u16 fadeInFrame );
static void ThrowRequest_STAND_BY( FIELD_SOUND* fieldSound, u32 soundIdx, u16 fadeOutFrame );
static void ThrowRequest_FORCE_PLAY( FIELD_SOUND* fieldSound, u16 soundIdx );
// �V�X�e�������N�G�X�g���󗝂ł��邩�ǂ����̔���
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
// ���N�G�X�g�󗝏�Ԃ̍X�V
static void ResetRequest( FIELD_SOUND* fieldSound );
static void CancelRequest( FIELD_SOUND* fieldSound );
static void FinishRequest( FIELD_SOUND* fieldSound );

// BGM�̑���
static void FadeInBGM ( FIELD_SOUND* fieldSound );
static void FadeOutBGM( FIELD_SOUND* fieldSound );
static void PushBGM( FIELD_SOUND* fieldSound );
static void PopBGM ( FIELD_SOUND* fieldSound );
static void DivLoadBGM_start( FIELD_SOUND* fieldSound );
static BOOL DivLoadBGM_load ( FIELD_SOUND* fieldSound );
static void ForcePlayBGM( FIELD_SOUND* fieldSound );

// �f�o�b�O
static void DebugPrint_RequestQueue( const FIELD_SOUND* fieldSound );


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
  FIELD_SOUND* fieldSound;

  // �쐬
  fieldSound = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_SOUND) );

  // ������
  InitFieldSoundSystem( fieldSound );

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
// ���擾
//=================================================================================

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

//---------------------------------------------------------------------------------
/**
 * @brief ���N�G�X�g�ێ�����
 *
 * @param fieldSound
 *
 * @return �������̃��N�G�X�g�������Ă���ꍇ TRUE
 *         �����łȂ���΁i�S�Ẵ��N�G�X�g�ɑ΂��鏈�����������Ă���ꍇ�jFALSE
 */
//---------------------------------------------------------------------------------
BOOL FIELD_SOUND_HaveRequest( const FIELD_SOUND* fieldSound )
{
  // �L���[�Ƀ��N�G�X�g���Ȃ� and ���N�G�X�g�������łȂ�
  if( (QueueHaveRequest(fieldSound) == FALSE) &&
      (fieldSound->request == FSND_BGM_REQUEST_NONE) )
  {
    return FALSE;
  }
  return TRUE;
}


//=================================================================================
// �����N�G�X�g
//=================================================================================

//---------------------------------------------------------------------------------
/**
 * @brief ���N�G�X�g�����N�G�X�g�L���[�ɓo�^����
 *
 * @param fieldSound
 * @param request      �o�^���郊�N�G�X�g
 * @param soundIdx     �Đ�����BGM
 * @param fadeOutFrame �Đ�����BGM�̃t�F�[�h�A�E�g �t���[����
 * @param fadeInFrame  �ύX���BGM�̃t�F�[�h�C�� �t���[����
 */
//---------------------------------------------------------------------------------
void FIELD_SOUND_RegisterRequest( FIELD_SOUND* fieldSound, 
                                  FSND_BGM_REQUEST request,
                                  u32 soundIdx, 
                                  u16 fadeOutFrame, 
                                  u16 fadeInFrame )
{ 
  FSND_REQUEST_DATA requestData;

  // ���N�G�X�g�f�[�^�쐬
  requestData.request = request;
  requestData.soundIdx = soundIdx;
  requestData.fadeOutFrame = fadeOutFrame;
  requestData.fadeInFrame = fadeInFrame;

  // ���N�G�X�g�L���[�ɓo�^
  RegisterNewRequest( fieldSound, &requestData ); 
}


//=================================================================================
// ���V�X�e������
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
  // ���N�G�X�g�L���[���烊�N�G�X�g�����o��
  RequestQueueCheck( fieldSound );

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

//---------------------------------------------------------------------------------
/**
 * @brief �V�X�e�������Z�b�g����
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
void FIELD_SOUND_Reset( FIELD_SOUND* fieldSound )
{
  ResetFieldSoundSystem( fieldSound );
}


//================================================================================= 
// ������J�֐�
//================================================================================= 

//---------------------------------------------------------------------------------
/**
 * @brief �V�X�e���̏�����
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void InitFieldSoundSystem( FIELD_SOUND* fieldSound )
{
  int i;

  fieldSound->state        = FSND_STATE_STOP;
  fieldSound->request      = FSND_BGM_REQUEST_NONE;
  fieldSound->currentBGM   = BGM_NONE;
  fieldSound->requestBGM   = BGM_NONE;
  fieldSound->loadBGM      = BGM_NONE;
  fieldSound->fadeInFrame  = 0;
  fieldSound->fadeOutFrame = 0;
  fieldSound->pushCount    = FSND_PUSHCOUNT_NONE;

  for( i=0; i<FSND_PUSHCOUNT_MAX; i++ )
  {
    fieldSound->pushBGM[i] = BGM_NONE; 
  }

  for( i=0; i<REQUEST_QUEUE_SIZE; i++ )
  {
    fieldSound->requestData[i].request = FSND_BGM_REQUEST_NONE;
  }

  fieldSound->requestHeadPos = 0;
  fieldSound->requestTailPos = 0;
}

//---------------------------------------------------------------------------------
/**
 * @brief �V�X�e���̃��Z�b�g
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void ResetFieldSoundSystem( FIELD_SOUND* fieldSound )
{
  while( fieldSound->pushCount != FSND_PUSHCOUNT_NONE )
  {
    PopBGM( fieldSound );
  }
  InitFieldSoundSystem( fieldSound );
}

//---------------------------------------------------------------------------------
/**
 * @brief ���N�G�X�g�L���[�̐擪���N�G�X�g�����o��
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void RequestQueueCheck( FIELD_SOUND* fieldSound )
{
  const FSND_REQUEST_DATA* headRequestData;

  // �擪���N�G�X�g�擾
  headRequestData = GetHeadRequest( fieldSound );

  // �擪���N�G�X�g���s
  if( CanAcceptRequest( fieldSound, headRequestData->request ) )
  {
    ThrowRequest( fieldSound, headRequestData );
    RemoveHeadRequest( fieldSound );
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief �擪���N�G�X�g���擾����
 *
 * @param fieldSound
 *
 * @return �擪���N�G�X�g
 */
//---------------------------------------------------------------------------------
const FSND_REQUEST_DATA* GetHeadRequest( const FIELD_SOUND* fieldSound )
{
  int pos;

  pos = fieldSound->requestHeadPos;

  return &fieldSound->requestData[ pos ];
}

//---------------------------------------------------------------------------------
/**
 * @brief ���̃��N�G�X�g���擾����
 *
 * @param fieldSound
 *
 * @return ���̃��N�G�X�g
 */
//---------------------------------------------------------------------------------
const FSND_REQUEST_DATA* GetNextRequest( const FIELD_SOUND* fieldSound )
{
  int pos;

  pos = (fieldSound->requestHeadPos + 1) % REQUEST_QUEUE_SIZE;

  return &fieldSound->requestData[ pos ];
}

//---------------------------------------------------------------------------------
/**
 * @brief �������N�G�X�g���擾����
 *
 * @param fieldSound
 *
 * @return �������N�G�X�g
 */
//---------------------------------------------------------------------------------
const FSND_REQUEST_DATA* GetTailRequest( const FIELD_SOUND* fieldSound )
{
  int pos;

  if( QueueHaveRequest( fieldSound ) == FALSE )
  { // ���N�G�X�g���Ȃ�
    pos = fieldSound->requestHeadPos;
  }
  else
  { // ���N�G�X�g������
    pos = ( fieldSound->requestTailPos - 1 + REQUEST_QUEUE_SIZE ) % REQUEST_QUEUE_SIZE;
  }

  return &fieldSound->requestData[ pos ];
}

//---------------------------------------------------------------------------------
/**
 * @brief �L���[�Ƀ��N�G�X�g�����邩�ǂ����𔻒肷��
 *
 * @param fieldSound
 * 
 * @return �L���[�Ƀ��N�G�X�g������ꍇ TRUE
 *         �L���[�Ƀ��N�G�X�g���Ȃ��ꍇ FALSE
 */
//---------------------------------------------------------------------------------
static BOOL QueueHaveRequest( const FIELD_SOUND* fieldSound )
{
  const FSND_REQUEST_DATA* headRequest;

  headRequest = GetHeadRequest( fieldSound );

  return ( headRequest->request != FSND_BGM_REQUEST_NONE );
}

//---------------------------------------------------------------------------------
/**
 * @brief �V�K���N�G�X�g�����N�G�X�g�L���[�ɓo�^����
 *
 * @param fieldSound
 * @param requestData �ǉ����郊�N�G�X�g�f�[�^
 */
//---------------------------------------------------------------------------------
void RegisterNewRequest( FIELD_SOUND* fieldSound, const FSND_REQUEST_DATA* requestData )
{
  int addPos;

  addPos = fieldSound->requestTailPos;

  // �f�b�h���b�N
  if( CanRegisterRequest(fieldSound, requestData->request) == FALSE )
  {
    OS_TFPrintf( PRINT_NO, 
                 "FIELD-SOUND-QUEUE: Can't register request(%d)\n", requestData->request );
    GF_ASSERT(0);
    return;
  }

  // ���N�G�X�g�L���[ �I�[�o�[�t���[
  if( fieldSound->requestData[ addPos ].request != FSND_BGM_REQUEST_NONE )
  {
    OS_Printf( "FIELD-SOUND: Request queue overflow\n" );
    GF_ASSERT(0);
    return;
  }

  // �o�^
  fieldSound->requestData[ addPos ] = *requestData;
  fieldSound->requestTailPos = (addPos + 1) % REQUEST_QUEUE_SIZE;

  // DEBUG:
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND-QUEUE: Regist new request\n" );

  // DEBUG: �L���[�̏�Ԃ��o��
  DebugPrint_RequestQueue( fieldSound );
}

//---------------------------------------------------------------------------------
/**
 * @brief ���N�G�X�g�L���[�Ƀ��N�G�X�g��o�^�ł��邩�ǂ����𔻒肷��
 *
 * @param fieldSound
 * @param request    ���肷�郊�N�G�X�g
 *
 * @param �o�^�ł���ꍇ TRUE
 *        �����łȂ���΁@FALSE
 */
//---------------------------------------------------------------------------------
static BOOL CanRegisterRequest( const FIELD_SOUND* fieldSound, FSND_BGM_REQUEST request )
{
  const FSND_REQUEST_DATA* tailRequest;

  // �ŏI���N�G�X�g���擾
  tailRequest = GetTailRequest( fieldSound );

  // �L���[�Ƀ��N�G�X�g�����݂���
  if( tailRequest->request != FSND_BGM_REQUEST_NONE )
  {
    return DeadRockTable[ tailRequest->request ][ request ];
  }

  // �Ō�̃��N�G�X�g��������
  if( fieldSound->request != FSND_BGM_REQUEST_NONE )
  {
    return DeadRockTable[ fieldSound->request ][ request ];
  }

  // ���N�G�X�g�����݂��Ȃ����, ��ԂɈˑ�
  return CanAcceptRequest( fieldSound, request );
}

//---------------------------------------------------------------------------------
/**
 * @brief �擪���N�G�X�g����菜��
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
void RemoveHeadRequest( FIELD_SOUND* fieldSound )
{
  int delPos;

  delPos = fieldSound->requestHeadPos;

  // ���N�G�X�g�������Ă��Ȃ�
  if( fieldSound->requestData[ delPos ].request == FSND_BGM_REQUEST_NONE )
  {
    OS_Printf( "FIELD-SOUND: Can't remove head request\n" );
    GF_ASSERT(0);
    return;
  }

  // �폜
  fieldSound->requestData[ delPos ].request = FSND_BGM_REQUEST_NONE;
  fieldSound->requestHeadPos = (delPos + 1) % REQUEST_QUEUE_SIZE;

  // DEBUG:
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND-QUEUE: Remove head request\n" );

  // DEBUG: �L���[�̏�Ԃ��o��
  DebugPrint_RequestQueue( fieldSound );
} 

//---------------------------------------------------------------------------------
/**
 * @brief ���N�G�X�g�̔��s
 *
 * @param fieldSound
 * @param requestData ���s���郊�N�G�X�g �f�[�^
 */
//---------------------------------------------------------------------------------
static void ThrowRequest( FIELD_SOUND* fieldSound, const FSND_REQUEST_DATA* requestData )
{
  switch( requestData->request )
  {
  case FSND_BGM_REQUEST_NONE:  
    break;
  case FSND_BGM_REQUEST_FADE_IN:  
    ThrowRequest_FADE_IN( fieldSound, requestData->fadeInFrame );
    break;
  case FSND_BGM_REQUEST_FADE_OUT:
    ThrowRequest_FADE_OUT( fieldSound, requestData->fadeOutFrame );
    break;
  case FSND_BGM_REQUEST_PUSH:
    ThrowRequest_PUSH( fieldSound, requestData->fadeOutFrame );
    break;
  case FSND_BGM_REQUEST_POP:
    ThrowRequest_POP( fieldSound, requestData->fadeOutFrame, requestData->fadeInFrame );
    break;
  case FSND_BGM_REQUEST_CHANGE:
    ThrowRequest_CHANGE( fieldSound, requestData->soundIdx, 
                         requestData->fadeOutFrame, requestData->fadeInFrame );
    break;
  case FSND_BGM_REQUEST_STAND_BY:
    ThrowRequest_STAND_BY( fieldSound, requestData->soundIdx, requestData->fadeOutFrame );
    break;
  case FSND_BGM_REQUEST_FORCE_PLAY:
    ThrowRequest_FORCE_PLAY( fieldSound, requestData->soundIdx );
    break;
  default:
    GF_ASSERT(0);
  }
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
  if( CanAcceptRequest(fieldSound, FSND_BGM_REQUEST_FADE_IN) == FALSE )
  {
    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: Can't Accept BGM fade in request\n" );
    GF_ASSERT(0);
    return;
  }

  // ���N�G�X�g����
  fieldSound->request = FSND_BGM_REQUEST_FADE_IN;
  fieldSound->fadeInFrame = fadeInFrame;

#ifdef DEBUG_ONLY_FOR_obata_toshihiro
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: accept request(FADE_IN)\n" );
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
  if( CanAcceptRequest(fieldSound, FSND_BGM_REQUEST_FADE_OUT) == FALSE )
  {
    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: Can't Accept BGM fade out request\n" );
    GF_ASSERT(0);
    return;
  }

  // ���N�G�X�g����
  fieldSound->request = FSND_BGM_REQUEST_FADE_OUT;
  fieldSound->fadeOutFrame = fadeOutFrame;

#ifdef DEBUG_ONLY_FOR_obata_toshihiro
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: accept request(FADE_OUT)\n" );
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
  if( CanAcceptRequest(fieldSound, FSND_BGM_REQUEST_PUSH) == FALSE )
  {
    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: Can't Accept BGM push request\n" );
    GF_ASSERT(0);
    return;
  }

  // ���N�G�X�g����
  fieldSound->request = FSND_BGM_REQUEST_PUSH;
  fieldSound->fadeOutFrame = fadeOutFrame;

#ifdef DEBUG_ONLY_FOR_obata_toshihiro
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: accept request(PUSH)\n" );
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
  if( CanAcceptRequest(fieldSound, FSND_BGM_REQUEST_POP) == FALSE )
  {
    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: Can't Accept BGM pop request\n" );
    GF_ASSERT(0);
    return;
  }

  // ���N�G�X�g����
  fieldSound->request = FSND_BGM_REQUEST_POP;
  fieldSound->fadeInFrame = fadeInFrame;
  fieldSound->fadeOutFrame = fadeOutFrame;

#ifdef DEBUG_ONLY_FOR_obata_toshihiro
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: accept request(POP)\n" );
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
  if( CanAcceptRequest(fieldSound, FSND_BGM_REQUEST_CHANGE) == FALSE )
  {
    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: Can't Accept BGM change request\n" );
    GF_ASSERT(0);
    return;
  } 

  // ���N�G�X�g����
  fieldSound->request = FSND_BGM_REQUEST_CHANGE;
  fieldSound->requestBGM = soundIdx;
  fieldSound->fadeInFrame = fadeInFrame;
  fieldSound->fadeOutFrame = fadeOutFrame;

#ifdef DEBUG_ONLY_FOR_obata_toshihiro
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: accept request(CHANGE)\n" );
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
  if( CanAcceptRequest(fieldSound, FSND_BGM_REQUEST_STAND_BY) == FALSE )
  {
    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: Can't Accept BGM stand-by request\n" );
    GF_ASSERT(0);
    return;
  }

  // ���N�G�X�g����
  fieldSound->request = FSND_BGM_REQUEST_STAND_BY;
  fieldSound->requestBGM = soundIdx;
  fieldSound->fadeOutFrame = fadeOutFrame;
  
#ifdef DEBUG_ONLY_FOR_obata_toshihiro
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: accept request(STAND_BY)\n" );
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
  if( CanAcceptRequest(fieldSound, FSND_BGM_REQUEST_FORCE_PLAY) == FALSE )
  {
    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: Can't Accept BGM force play request\n" );
    GF_ASSERT(0);
    return;
  }

  // ���N�G�X�g����
  fieldSound->request = FSND_BGM_REQUEST_FORCE_PLAY;
  fieldSound->requestBGM = soundIdx;

#ifdef DEBUG_ONLY_FOR_obata_toshihiro
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: accept request(FORCE_PLAY)\n" );
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
    FadeInBGM( fieldSound );
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
    FinishRequest( fieldSound );
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
  case FSND_BGM_REQUEST_FADE_IN:
    // �Đ����̃��N�G�X�g�͖�������
    CancelRequest( fieldSound );
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
    // �Đ����̋Ȃ̃��N�G�X�g�͖�������
    if( fieldSound->requestBGM == fieldSound->currentBGM )
    {
      CancelRequest( fieldSound );
    }
    else
    {
      FadeOutBGM( fieldSound );
      ChangeState( fieldSound, FSND_STATE_CHANGE_out );
    }
    break;
  case FSND_BGM_REQUEST_STAND_BY: 
    // �Đ����̋Ȃ̃��N�G�X�g�͖�������
    if( fieldSound->requestBGM == fieldSound->currentBGM )
    {
      CancelRequest( fieldSound );
    }
    else
    {
      FadeOutBGM( fieldSound );
      ChangeState( fieldSound, FSND_STATE_STAND_BY_out );
    }
    break;
  case FSND_BGM_REQUEST_FORCE_PLAY:
    ForcePlayBGM( fieldSound );
    ChangeState( fieldSound, FSND_STATE_PLAY );
    FinishRequest( fieldSound );
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
    FinishRequest( fieldSound );
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
    FinishRequest( fieldSound );
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
    FinishRequest( fieldSound );
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
    FinishRequest( fieldSound );
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
    FinishRequest( fieldSound );
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
    if( (fieldSound->requestBGM != BGM_NONE) &&
        (fieldSound->requestBGM != fieldSound->currentBGM) )
    { 
      // �ă��[�h�J�n
      DivLoadBGM_start( fieldSound );
      ChangeState( fieldSound, FSND_STATE_CHANGE_load );
    }
    else
    { 
      // �t�F�[�h�C���J�n
      FadeInBGM( fieldSound );
      ChangeState( fieldSound, FSND_STATE_CHANGE_in );
    }
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
    if( (fieldSound->requestBGM != BGM_NONE) &&
        (fieldSound->requestBGM != fieldSound->currentBGM) )
    {
      // �t�F�[�h�A�E�g�J�n
      FadeOutBGM( fieldSound );
      ChangeState( fieldSound, FSND_STATE_CHANGE_out );
    }
    else
    {
      ChangeState( fieldSound, FSND_STATE_PLAY );
      FinishRequest( fieldSound );
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
    FinishRequest( fieldSound );
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
    FinishRequest( fieldSound );
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
  case FSND_STATE_STOP:              OS_TFPrintf( PRINT_NO, "FIELD-SOUND: change state (STOP ==> " );  break;
  case FSND_STATE_PLAY:              OS_TFPrintf( PRINT_NO, "FIELD-SOUND: change state (PLAY ==> " );  break;
  case FSND_STATE_WAIT:              OS_TFPrintf( PRINT_NO, "FIELD-SOUND: change state (WAIT ==> " );  break;
  case FSND_STATE_FADE_OUT:          OS_TFPrintf( PRINT_NO, "FIELD-SOUND: change state (FADE OUT ==> " );  break;
  case FSND_STATE_FADE_IN:           OS_TFPrintf( PRINT_NO, "FIELD-SOUND: change state (FADE IN ==> " );  break;
  case FSND_STATE_PUSH:              OS_TFPrintf( PRINT_NO, "FIELD-SOUND: change state (PUSH ==> " );  break;
  case FSND_STATE_POP_out:           OS_TFPrintf( PRINT_NO, "FIELD-SOUND: change state (POP out ==> " );  break;
  case FSND_STATE_POP_in:            OS_TFPrintf( PRINT_NO, "FIELD-SOUND: change state (POP in ==> " );  break;
  case FSND_STATE_CHANGE_out:        OS_TFPrintf( PRINT_NO, "FIELD-SOUND: change state (CHANGE out ==> " );  break;
  case FSND_STATE_CHANGE_load:       OS_TFPrintf( PRINT_NO, "FIELD-SOUND: change state (CHANGE load ==> " );  break;
  case FSND_STATE_CHANGE_in:         OS_TFPrintf( PRINT_NO, "FIELD-SOUND: change state (CHANGE in ==> " );  break;
  case FSND_STATE_CHANGE_PUSH_out:   OS_TFPrintf( PRINT_NO, "FIELD-SOUND: change state (CHANGE PUSH out ==> " );  break;
  case FSND_STATE_CHANGE_PUSH_load:  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: change state (CHANGE PUSH load ==> " );  break;
  case FSND_STATE_STAND_BY_out:      OS_TFPrintf( PRINT_NO, "FIELD-SOUND: change state (STAND BY out ==> " );  break;
  case FSND_STATE_STAND_BY_load:     OS_TFPrintf( PRINT_NO, "FIELD-SOUND: change state (STAND BY load ==> " );  break;
  default: GF_ASSERT(0);
  }
  switch( nextState )
  {
  case FSND_STATE_STOP:              OS_TFPrintf( PRINT_NO, "STOP)\n" );  break;
  case FSND_STATE_PLAY:              OS_TFPrintf( PRINT_NO, "PLAY)\n" );  break;
  case FSND_STATE_WAIT:              OS_TFPrintf( PRINT_NO, "WAIT)\n" );  break;
  case FSND_STATE_FADE_OUT:          OS_TFPrintf( PRINT_NO, "FADE OUT)\n" );  break;
  case FSND_STATE_FADE_IN:           OS_TFPrintf( PRINT_NO, "FADE IN)\n" );  break;
  case FSND_STATE_PUSH:              OS_TFPrintf( PRINT_NO, "PUSH)\n" );  break;
  case FSND_STATE_POP_out:           OS_TFPrintf( PRINT_NO, "POP out)\n" );  break;
  case FSND_STATE_POP_in:            OS_TFPrintf( PRINT_NO, "POP in)\n" );  break;
  case FSND_STATE_CHANGE_out:        OS_TFPrintf( PRINT_NO, "CHANGE out)\n" );  break;
  case FSND_STATE_CHANGE_load:       OS_TFPrintf( PRINT_NO, "CHANGE load)\n" );  break;
  case FSND_STATE_CHANGE_in:         OS_TFPrintf( PRINT_NO, "CHANGE in)\n" );  break;
  case FSND_STATE_CHANGE_PUSH_out:   OS_TFPrintf( PRINT_NO, "CHANGE PUSH out)\n" );  break;
  case FSND_STATE_CHANGE_PUSH_load:  OS_TFPrintf( PRINT_NO, "CHANGE PUSH load)\n" );  break;
  case FSND_STATE_STAND_BY_out:      OS_TFPrintf( PRINT_NO, "STAND BY out)\n" );  break;
  case FSND_STATE_STAND_BY_load:     OS_TFPrintf( PRINT_NO, "STAND BY load)\n" );  break;
  default: GF_ASSERT(0);
  }
#endif

  // �X�V
  fieldSound->state = nextState; 
}

//---------------------------------------------------------------------------------
/**
 * @brief ���N�G�X�g�󗝏�Ԃ����Z�b�g����
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void ResetRequest( FIELD_SOUND* fieldSound )
{
  fieldSound->request = FSND_BGM_REQUEST_NONE;
  fieldSound->requestBGM = BGM_NONE;
  fieldSound->fadeInFrame = 0;
  fieldSound->fadeOutFrame = 0;
}

//---------------------------------------------------------------------------------
/**
 * @brief �󂯎�������N�G�X�g���L�����Z������
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void CancelRequest( FIELD_SOUND* fieldSound )
{
  // ���N�G�X�g�������Ă��Ȃ�
  if( fieldSound->request == FSND_BGM_REQUEST_NONE )
  {
    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: Don't have BGM request\n" );
    GF_ASSERT(0);
    return;
  }

  // ���N�G�X�g�󗝏�Ԃ����Z�b�g
  ResetRequest( fieldSound );

  // DEBUG:
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: Cancel request\n" );
}

//---------------------------------------------------------------------------------
/**
 * @brief ���N�G�X�g�ɑ΂��铮��̊�������
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void FinishRequest( FIELD_SOUND* fieldSound )
{
  // ���N�G�X�g�������Ă��Ȃ�
  if( fieldSound->request == FSND_BGM_REQUEST_NONE )
  {
    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: Don't have BGM request\n" );
    GF_ASSERT(0);
    return;
  }

#ifdef DEBUG_ONLY_FOR_obata_toshihiro
  switch( fieldSound->request )
  {
  case FSND_BGM_REQUEST_FADE_IN:     OS_TFPrintf( PRINT_NO, "FIELD-SOUND: request(FADE IN) finished!!\n" );  break;
  case FSND_BGM_REQUEST_FADE_OUT:    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: request(FADE OUT) finished!!\n" );  break;
  case FSND_BGM_REQUEST_PUSH:        OS_TFPrintf( PRINT_NO, "FIELD-SOUND: request(PUSH) finished!!\n" );  break;
  case FSND_BGM_REQUEST_POP:         OS_TFPrintf( PRINT_NO, "FIELD-SOUND: request(POP) finished!!\n" );  break;
  case FSND_BGM_REQUEST_CHANGE:      OS_TFPrintf( PRINT_NO, "FIELD-SOUND: request(CHANGE) finished!!\n" );  break;
  case FSND_BGM_REQUEST_STAND_BY:    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: request(STAND BY) finished!!\n" );  break;
  case FSND_BGM_REQUEST_FORCE_PLAY:  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: request(FORCE_PLAY) finished!!\n" );  break;
  }
#endif

  // ���N�G�X�g�󗝏�Ԃ����Z�b�g
  ResetRequest( fieldSound );
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
  if( fieldSound->currentBGM == BGM_NONE )
  {
    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: Can't fade in BGM (Don't playing BGM)\n" );
    GF_ASSERT(0);
    return;
  } 

  PMSND_FadeInBGM( fieldSound->fadeInFrame );
  PMSND_PauseBGM( FALSE );

#ifdef DEBUG_ONLY_FOR_obata_toshihiro
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: BGM fade in start\n" );
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
  if( fieldSound->currentBGM == BGM_NONE )
  {
    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: Can't fade out BGM (Don't playing BGM)\n" );
    GF_ASSERT(0);
    return;
  } 

  PMSND_FadeOutBGM( fieldSound->fadeOutFrame );

#ifdef DEBUG_ONLY_FOR_obata_toshihiro
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: BGM fade out start\n" );
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
  if( fieldSound->currentBGM == BGM_NONE )
  {
    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: Can't push BGM (Don't playing BGM)\n" );
    GF_ASSERT(0);
    return;
  } 
  // �ޔ𐔃I�[�o�[
  if( FSND_PUSHCOUNT_MAX <= fieldSound->pushCount )
  {
    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: Can't push BGM (Push count over)\n" );
    GF_ASSERT(0);
    return;
  }

  // �ޔ�
  PMSND_PushBGM();

  // ���������X�V
  fieldSound->pushBGM[ fieldSound->pushCount ] = fieldSound->currentBGM;
  fieldSound->pushCount++;
  fieldSound->currentBGM = BGM_NONE; 

#ifdef DEBUG_ONLY_FOR_obata_toshihiro
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: push BGM(%d)\n", fieldSound->pushBGM[ fieldSound->pushCount-1 ] );
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
    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: Can't pop BGM (There is no pushed BGM)\n" );
    GF_ASSERT(0);
    return;
  }

  // ���A
  PMSND_PopBGM();

  // ���������X�V
  fieldSound->pushCount--;
  fieldSound->currentBGM = fieldSound->pushBGM[ fieldSound->pushCount ];

#ifdef DEBUG_ONLY_FOR_obata_toshihiro
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: pop BGM(%d)\n", fieldSound->currentBGM );
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
  if( fieldSound->requestBGM == BGM_NONE )
  {
    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: Can't start load BGM (Don't know target BGM)\n" );
    GF_ASSERT(0);
    return;
  }

  // �������[�h�J�n
  PMSND_PlayBGMdiv( fieldSound->requestBGM, &(fieldSound->loadSeq), TRUE );

  // ���������X�V
  fieldSound->loadBGM = fieldSound->requestBGM;
  fieldSound->requestBGM = BGM_NONE;
  fieldSound->currentBGM = BGM_NONE;

#ifdef DEBUG_ONLY_FOR_obata_toshihiro
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: div load BGM(%d) start\n", fieldSound->loadBGM );
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
  if( fieldSound->loadBGM == BGM_NONE )
  {
    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: Can't load BGM (Don't know target BGM)\n" );
    GF_ASSERT(0); 
  }

  // �������[�h���s
  loadFinished = PMSND_PlayBGMdiv( fieldSound->loadBGM, &(fieldSound->loadSeq), FALSE );

  // �������[�h����(�Đ��J�n)
  if( loadFinished )
  {
    // ���������X�V
    fieldSound->currentBGM = fieldSound->loadBGM;
    fieldSound->loadBGM = BGM_NONE;

#ifdef DEBUG_ONLY_FOR_obata_toshihiro
    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: div load BGM(%d) finish\n", fieldSound->currentBGM );
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
  if( fieldSound->requestBGM == BGM_NONE )
  {
    OS_TFPrintf( PRINT_NO, "FIELD-SOUND: Can't force play BGM (Don't know target BGM)\n" );
    GF_ASSERT(0);
    return;
  }

  // �����Đ�
  PMSND_PlayBGM( fieldSound->requestBGM );

  // ���������X�V
  fieldSound->currentBGM = fieldSound->requestBGM;
  fieldSound->requestBGM = BGM_NONE;
  fieldSound->loadBGM = BGM_NONE;

#ifdef DEBUG_ONLY_FOR_obata_toshihiro
  OS_TFPrintf( PRINT_NO, "FIELD-SOUND: force play BGM(%d) finish\n", fieldSound->currentBGM );
#endif
} 

//---------------------------------------------------------------------------------
/**
 * @brief ���N�G�X�g�L���[��\������
 *
 * @param fieldSound
 */
//---------------------------------------------------------------------------------
static void DebugPrint_RequestQueue( const FIELD_SOUND* fieldSound )
{
  int pos;
  const FSND_REQUEST_DATA* requestData;

  OS_TFPrintf( PRINT_NO, "FIELD-SOUND-QUEUE: " );

  // �擪���N�G�X�g�擾
  pos = fieldSound->requestHeadPos;
  requestData = &( fieldSound->requestData[ pos ] );

  while( requestData->request != FSND_BGM_REQUEST_NONE )
  {
    switch( requestData->request )
    {
    case FSND_BGM_REQUEST_FADE_IN:     OS_TFPrintf( PRINT_NO, "FADE IN" );  break;
    case FSND_BGM_REQUEST_FADE_OUT:    OS_TFPrintf( PRINT_NO, "FADE OUT" );  break;
    case FSND_BGM_REQUEST_PUSH:        OS_TFPrintf( PRINT_NO, "PUSH" );  break;
    case FSND_BGM_REQUEST_POP:         OS_TFPrintf( PRINT_NO, "POP" );  break;
    case FSND_BGM_REQUEST_CHANGE:      OS_TFPrintf( PRINT_NO, "CHANGE" );  break;
    case FSND_BGM_REQUEST_STAND_BY:    OS_TFPrintf( PRINT_NO, "STAND BY" );  break;
    case FSND_BGM_REQUEST_FORCE_PLAY:  OS_TFPrintf( PRINT_NO, "FORCE_PLAY" );  break;
    }
    OS_TFPrintf( PRINT_NO, " | " );

    // ���̃��N�G�X�g��
    pos = (pos + 1) % REQUEST_QUEUE_SIZE;
    requestData = &( fieldSound->requestData[ pos ] );

    // �������
    if( pos == fieldSound->requestHeadPos ){ break; }
  }

  OS_TFPrintf( PRINT_NO, "\n" );
}
