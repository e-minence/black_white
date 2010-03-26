/////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  ISS �X�C�b�`
 * @file   iss_switch.c
 * @author obata
 * @date   2010.03.23
 */
///////////////////////////////////////////////////////////////////////////////// 
#include <gflib.h>
#include "iss_switch.h"

#include "sound/pm_sndsys.h" // for PMSND_xxxx


//===============================================================================
// ���f�o�b�O���
//===============================================================================
//#define DEBUG_MODE       // �f�o�b�O�X�C�b�`
#define PRINT_TARGET (1) // �f�o�b�O���̏o�͐�

#ifdef DEBUG_MODE
static u8 Debug_switchNum = 0;
#endif


//===============================================================================
// ���萔
//===============================================================================
#define MIN_VOLUME   (0) // �ŏ��{�����[��
#define MAX_VOLUME (127) // �ő�{�����[��

// �X�C�b�`�̏��
typedef enum {
  SWITCH_STATE_ON,      // ON
  SWITCH_STATE_OFF,     // OFF
  SWITCH_STATE_FADEIN,  // �t�F�[�h �C��
  SWITCH_STATE_FADEOUT, // �t�F�[�h �A�E�g
} SWITCH_STATE; 


//===============================================================================
// ��ISS����X�C�b�`
//===============================================================================
struct _ISS_SWITCH
{ 
  SWITCH_STATE state;     // ���
  u16          fadeCount; // �t�F�[�h�J�E���^
  u16          fadeFrame; // �t�F�[�h�t���[����
  u16          trackBit;  // ����Ώۃg���b�N�r�b�g

#ifdef DEBUG_MODE
  u8 debugID; // ���ʎq
#endif
};


//===============================================================================
// ���֐��C���f�b�N�X
//===============================================================================
static ISS_SWITCH* CreateSwitch( HEAPID heapID ); // �X�C�b�`�𐶐�����
static void DeleteSwitch( ISS_SWITCH* sw ); // �X�C�b�`��j������
static void InitSwitch( ISS_SWITCH* sw ); // �X�C�b�`������������
static void SetSwitchParam( ISS_SWITCH* sw, u16 fadeFrame, u16 trackBit ); // �X�C�b�`�̃p�����[�^��ݒ肷��
static SWITCH_STATE GetSwitchState( const ISS_SWITCH* sw ); // �X�C�b�`�̏�Ԃ��擾����
static void SetSwitchState( ISS_SWITCH* sw, SWITCH_STATE state ); // �X�C�b�`�̏�Ԃ�ύX����
static int GetVolume( const ISS_SWITCH* sw ); // �X�C�b�`�̏�Ԃ���BGM�̃g���b�N�{�����[�����v�Z����
static void SetBGMVolume( const ISS_SWITCH* sw, int priorVolume ); // �X�C�b�`�̏�Ԃ�BGM�{�����[���ɔ��f������
static BOOL CheckSwitchFade( const ISS_SWITCH* sw ); // �t�F�[�h�����ǂ����𔻒肷��
static BOOL CheckSwitchOn( const ISS_SWITCH* sw ); // �X�C�b�`��������Ă��邩�ǂ����𔻒肷��
static void IncFadeCount( ISS_SWITCH* sw ); // �t�F�[�h�J�E���^���C���N�������g����
static BOOL CheckFadeFinish( const ISS_SWITCH* sw ); // �t�F�[�h�������������ǂ������`�F�b�N����
static void SwitchMain( ISS_SWITCH* sw ); // �X�C�b�`�̃��C������
static void SwitchMain_ON( ISS_SWITCH* sw ); // �X�C�b�`�̃��C������ ( SWITCH_STATE_ON )
static void SwitchMain_OFF( ISS_SWITCH* sw ); // �X�C�b�`�̃��C������ ( SWITCH_STATE_OFF )
static void SwitchMain_FADEIN( ISS_SWITCH* sw ); // �X�C�b�`�̃��C������ ( SWITCH_STATE_FADEIN )
static void SwitchMain_FADEOUT( ISS_SWITCH* sw ); // �X�C�b�`�̃��C������ ( SWITCH_STATE_FADEOUT )
static void SwitchOn( ISS_SWITCH* sw ); // �X�C�b�`������
static void SwitchOff( ISS_SWITCH* sw ); // �X�C�b�`�𗣂�
static void SwitchOnDirect( ISS_SWITCH* sw ); // �X�C�b�`�𑦎���ON�̏�Ԃɂ���
static void SwitchOffDirect( ISS_SWITCH* sw ); // �X�C�b�`�𑦎���OFF�̏�Ԃɂ���

#ifdef DEBUG_MODE
static void Debug_PrintSwitch( const ISS_SWITCH* sw ); // �X�C�b�`�̏�Ԃ��o�͂���
#endif


//===============================================================================
// ��public method
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�𐶐�����
 *
 * @param heapID �g�p����q�[�vID
 *
 * @return ���������X�C�b�`
 */
//-------------------------------------------------------------------------------
ISS_SWITCH* ISS_SWITCH_Create( HEAPID heapID )
{
  ISS_SWITCH* sw;

  sw = CreateSwitch( heapID );
  InitSwitch( sw ); 

  return sw;
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`��j������
 *
 * @param sw
 */
//-------------------------------------------------------------------------------
void ISS_SWITCH_Delete( ISS_SWITCH* sw )
{
  GF_ASSERT( sw );

  DeleteSwitch( sw );
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�̃p�����[�^��ݒ肷��
 *
 * @param sw
 * @param fadeFrame �t�F�[�h�t���[����
 * @param trackBit  ����Ώۃg���b�N��\���r�b�g�}�X�N
 */
//-------------------------------------------------------------------------------
void ISS_SWITCH_SetSwitchParam( ISS_SWITCH* sw, u16 fadeFrame, u16 trackBit )
{
  GF_ASSERT( sw );

  SetSwitchParam( sw, fadeFrame, trackBit );
}

//-------------------------------------------------------------------------------
/**
 * @brief �t�F�[�h�����ǂ����𔻒肷��
 *
 * @param sw
 *
 * @return �t�F�[�h���Ȃ� TRUE, �����łȂ���� FALSE
 */
//-------------------------------------------------------------------------------
BOOL ISS_SWITCH_IsFade( const ISS_SWITCH* sw )
{
  GF_ASSERT( sw );

  return CheckSwitchFade( sw );
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`��������Ă��邩�ǂ����𔻒肷��
 *
 * @param sw
 *
 * @return �X�C�b�`��������Ă���ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//-------------------------------------------------------------------------------
BOOL ISS_SWITCH_IsOn( const ISS_SWITCH* sw )
{
  GF_ASSERT( sw );

  return CheckSwitchOn( sw );
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`������
 *
 * @param sw
 */
//-------------------------------------------------------------------------------
void ISS_SWITCH_On( ISS_SWITCH* sw )
{
  GF_ASSERT( sw );

  SwitchOn( sw );
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�𗣂�
 *
 * @param sw
 */
//-------------------------------------------------------------------------------
void ISS_SWITCH_Off( ISS_SWITCH* sw )
{
  GF_ASSERT( sw );

  SwitchOff( sw );
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�𑦎���ON�̏�Ԃɂ���
 *
 * @param sw
 */
//-------------------------------------------------------------------------------
void ISS_SWITCH_ForceOn( ISS_SWITCH* sw )
{
  GF_ASSERT( sw );

  SwitchOnDirect( sw );
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�𑦎���OFF�̏�Ԃɂ���
 *
 * @param sw
 */
//-------------------------------------------------------------------------------
void ISS_SWITCH_ForceOff( ISS_SWITCH* sw )
{
  GF_ASSERT( sw );

  SwitchOffDirect( sw );
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�̃��C������
 *
 * @param sw
 */
//-------------------------------------------------------------------------------
void ISS_SWITCH_Main( ISS_SWITCH* sw )
{
  GF_ASSERT( sw );

  SwitchMain( sw );
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�̏�Ԃ�BGM�̃{�����[���ɔ��f������
 *
 * @param sw
 * @param priorVolume
 */
//-------------------------------------------------------------------------------
void ISS_SWITCH_SetBGMVolume( const ISS_SWITCH* sw, int priorVolume )
{
  GF_ASSERT( sw );

  SetBGMVolume( sw, priorVolume );
}


//===============================================================================
// ��private method
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�𐶐�����
 *
 * @param heapID
 *
 * @return ���������X�C�b�`
 */
//-------------------------------------------------------------------------------
static ISS_SWITCH* CreateSwitch( HEAPID heapID )
{
  ISS_SWITCH* sw;

  sw = GFL_HEAP_AllocMemory( heapID, sizeof(ISS_SWITCH) );

#ifdef DEBUG_MODE
  sw->debugID = Debug_switchNum++;
#endif

#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, "ISS-SWITCH[%d]: create switch\n", sw->debugID );
#endif

  return sw;
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`��j������
 *
 * @param sw
 */
//-------------------------------------------------------------------------------
static void DeleteSwitch( ISS_SWITCH* sw )
{
#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, "ISS-SWITCH[%d]: delete switch\n", sw->debugID );
#endif

  GFL_HEAP_FreeMemory( sw );
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`������������
 *
 * @param sw
 */
//-------------------------------------------------------------------------------
static void InitSwitch( ISS_SWITCH* sw )
{
  sw->state     = SWITCH_STATE_OFF;
  sw->fadeCount = 0;
  sw->fadeFrame = 0;
  sw->trackBit  = 0;

#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, "ISS-SWITCH[%d]: init switch\n", sw->debugID );
#endif
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�̃p�����[�^��ݒ肷��
 *
 * @param sw
 * @param fadeFrame �t�F�[�h�t���[����
 * @param trackBit  ����Ώۃg���b�N��\���r�b�g�}�X�N
 */
//-------------------------------------------------------------------------------
static void SetSwitchParam( ISS_SWITCH* sw, u16 fadeFrame, u16 trackBit )
{
  sw->fadeFrame = fadeFrame;
  sw->trackBit = trackBit;

#ifdef DEBUG_MODE
  Debug_PrintSwitch( sw );
#endif
}

//-------------------------------------------------------------------------------
/**
 * @brief �t�F�[�h�J�E���^���C���N�������g����
 *
 * @param sw
 */
//-------------------------------------------------------------------------------
static void IncFadeCount( ISS_SWITCH* sw )
{
  sw->fadeCount++;

  // �ő�l�␳
  if( sw->fadeFrame < sw->fadeCount ) {
    sw->fadeCount = sw->fadeFrame;
  }
} 

//-------------------------------------------------------------------------------
/**
 * @brief �t�F�[�h�������������ǂ������`�F�b�N����
 *
 * @param sw
 *
 * @return �t�F�[�h�����������ꍇ TRUE
 *         �����łȂ��ꍇ FALSE
 */
//-------------------------------------------------------------------------------
static BOOL CheckFadeFinish( const ISS_SWITCH* sw )
{
  if( sw->fadeFrame <= sw->fadeCount ) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�̃��C������
 *
 * @param sw
 */
//-------------------------------------------------------------------------------
static void SwitchMain( ISS_SWITCH* sw )
{ 
  switch( GetSwitchState(sw) ) {
  case SWITCH_STATE_ON:      SwitchMain_ON( sw );      break;
  case SWITCH_STATE_OFF:     SwitchMain_OFF( sw );     break;
  case SWITCH_STATE_FADEIN:  SwitchMain_FADEIN( sw );  break;
  case SWITCH_STATE_FADEOUT: SwitchMain_FADEOUT( sw ); break;
  default: GF_ASSERT(0);
  }
} 

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�̃��C������ ( SWITCH_STATE_ON )
 *
 * @param sw
 */
//-------------------------------------------------------------------------------
static void SwitchMain_ON( ISS_SWITCH* sw )
{
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�̃��C������ ( SWITCH_STATE_OFF )
 *
 * @param sw
 */
//-------------------------------------------------------------------------------
static void SwitchMain_OFF( ISS_SWITCH* sw )
{
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�̃��C������ ( SWITCH_STATE_FADEIN )
 *
 * @param sw
 */
//-------------------------------------------------------------------------------
static void SwitchMain_FADEIN( ISS_SWITCH* sw )
{
  IncFadeCount( sw );

  // �t�F�[�h�C���I��
  if( CheckFadeFinish( sw ) == TRUE ) {
    SetSwitchState( sw, SWITCH_STATE_ON );
  }
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�̃��C������ ( SWITCH_STATE_FADEOUT )
 *
 * @param sw
 */
//-------------------------------------------------------------------------------
static void SwitchMain_FADEOUT( ISS_SWITCH* sw )
{
  IncFadeCount( sw );

  // �t�F�[�h�A�E�g�I��
  if( CheckFadeFinish( sw ) == TRUE ) {
    SetSwitchState( sw, SWITCH_STATE_OFF );
  }
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`������
 *
 * @param sw
 */
//-------------------------------------------------------------------------------
static void SwitchOn( ISS_SWITCH* sw )
{
  SetSwitchState( sw, SWITCH_STATE_FADEIN ); 

#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, "ISS-SWITCH[%d]: on\n", sw->debugID );
#endif
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�𗣂�
 *
 * @param sw
 */
//-------------------------------------------------------------------------------
static void SwitchOff( ISS_SWITCH* sw )
{
  SetSwitchState( sw, SWITCH_STATE_FADEOUT );

#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, "ISS-SWITCH[%d]: off\n", sw->debugID );
#endif
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�𑦎���ON�̏�Ԃɂ���
 *
 * @param sw
 */
//-------------------------------------------------------------------------------
static void SwitchOnDirect( ISS_SWITCH* sw )
{
  SetSwitchState( sw, SWITCH_STATE_ON );

#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, "ISS-SWITCH[%d]: direct on\n", sw->debugID );
#endif
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�𑦎���OFF�̏�Ԃɂ���
 *
 * @param sw
 */
//-------------------------------------------------------------------------------
static void SwitchOffDirect( ISS_SWITCH* sw )
{
  SetSwitchState( sw, SWITCH_STATE_OFF );

#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, "ISS-SWITCH[%d]: direct off\n", sw->debugID );
#endif
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�̏�Ԃ��擾����
 *
 * @param sw
 *
 * @return �X�C�b�`�̏��
 */
//-------------------------------------------------------------------------------
static SWITCH_STATE GetSwitchState( const ISS_SWITCH* sw )
{
  return sw->state;
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�̏�Ԃ�ύX����
 *
 * @param sw
 * @param state �ݒ肷����
 */
//-------------------------------------------------------------------------------
static void SetSwitchState( ISS_SWITCH* sw, SWITCH_STATE state )
{
  sw->state = state;
  sw->fadeCount = 0;

#ifdef DEBUG_MODE
  OS_TFPrintf( PRINT_TARGET, "ISS-SWITCH[%d]: set switch state ==> ", sw->debugID );
  switch( state ) {
  case SWITCH_STATE_ON:      OS_TFPrintf( PRINT_TARGET, "ON" );      break;
  case SWITCH_STATE_OFF:     OS_TFPrintf( PRINT_TARGET, "OFF" );     break;
  case SWITCH_STATE_FADEIN:  OS_TFPrintf( PRINT_TARGET, "FADEIN" );  break;
  case SWITCH_STATE_FADEOUT: OS_TFPrintf( PRINT_TARGET, "FADEOUT" ); break;
  }
  OS_TFPrintf( PRINT_TARGET, "\n" );
#endif
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�̏�Ԃ���BGM�̃g���b�N�{�����[�����v�Z����
 *
 * @param sw
 *
 * @return �X�C�b�`�̏�ԂɑΉ�����g���b�N�{�����[��
 */
//-------------------------------------------------------------------------------
static int GetVolume( const ISS_SWITCH* sw )
{
  SWITCH_STATE state;
  int volume;

  switch( GetSwitchState( sw ) ) {
  case SWITCH_STATE_ON:  return MAX_VOLUME;
  case SWITCH_STATE_OFF: return MIN_VOLUME;
  case SWITCH_STATE_FADEIN:
    volume = MAX_VOLUME * sw->fadeCount / sw->fadeFrame;
    break;
  case SWITCH_STATE_FADEOUT:
    volume = MAX_VOLUME - MAX_VOLUME * sw->fadeCount / sw->fadeFrame;
    break;
  default: 
    GF_ASSERT(0);
  }

  GF_ASSERT( MIN_VOLUME <= volume );
  GF_ASSERT( volume <= MAX_VOLUME );
  return volume;
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�̏�Ԃ�BGM�̃{�����[���ɔ��f������
 *
 * @param sw
 * @param priorVolume ��ʃ{�����[���l
 */
//-------------------------------------------------------------------------------
static void SetBGMVolume( const ISS_SWITCH* sw, int priorVolume )
{
  int volume;

  // ��ʃ{�����[�����l�������l���Z�o
  volume = GetVolume(sw) * priorVolume / MAX_VOLUME;

  GF_ASSERT( MIN_VOLUME <= volume );
  GF_ASSERT( volume <= MAX_VOLUME );

  // BGM�ɔ��f
  PMSND_ChangeBGMVolume( sw->trackBit, volume );
}

//-------------------------------------------------------------------------------
/**
 * @brief �t�F�[�h�����ǂ����𔻒肷��
 *
 * @param sw
 *
 * @return �t�F�[�h���Ȃ� TRUE, �����łȂ���� FALSE
 */
//-------------------------------------------------------------------------------
static BOOL CheckSwitchFade( const ISS_SWITCH* sw )
{
  switch( GetSwitchState( sw ) ) {
  case SWITCH_STATE_ON:      return FALSE;
  case SWITCH_STATE_OFF:     return FALSE;
  case SWITCH_STATE_FADEIN:  return TRUE;
  case SWITCH_STATE_FADEOUT: return TRUE;
  default: GF_ASSERT(0);
  }

  GF_ASSERT(0);
  return FALSE;
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`��������Ă��邩�ǂ����𔻒肷��
 *
 * @param sw
 *
 * @return �X�C�b�`��������Ă���ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//-------------------------------------------------------------------------------
static BOOL CheckSwitchOn( const ISS_SWITCH* sw )
{
  switch( GetSwitchState( sw ) ) {
  case SWITCH_STATE_ON:      return TRUE;
  case SWITCH_STATE_OFF:     return FALSE;
  case SWITCH_STATE_FADEIN:  return FALSE;
  case SWITCH_STATE_FADEOUT: return FALSE;
  default: GF_ASSERT(0);
  }

  GF_ASSERT(0);
  return FALSE;
}

#ifdef DEBUG_MODE
//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�̏�Ԃ��o�͂���
 *
 * @param sw
 */
//-------------------------------------------------------------------------------
static void Debug_PrintSwitch( const ISS_SWITCH* sw )
{
  OS_TFPrintf( PRINT_TARGET, "ISS-SWITCH[%d]: ", sw->debugID );

  // �t�F�[�h����
  OS_TFPrintf( PRINT_TARGET, "fadeFrame = %d, ", sw->fadeFrame ); 
  // �g���b�N�r�b�g
  OS_TFPrintf( PRINT_TARGET, "trackBit = 0x%x, ", sw->trackBit );

  OS_TFPrintf( PRINT_TARGET, "\n" );
} 
#endif
