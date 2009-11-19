////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief ISS�X�C�b�`�V�X�e��
 * @file iss_switch_sys.c
 * @author obata
 * @date 2009.11.17
 */
////////////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "iss_switch_sys.h"
#include "sound/pm_sndsys.h"
#include "arc/arc_def.h"  // for ARCID_ISS_SWITCH


//==========================================================================================
// ���萔
//==========================================================================================
#define TRACK_NUM    (16)      // �g���b�N��
#define TRACKBIT_ALL (0xffff)  // �S�g���b�N�w��
#define MAX_VOLUME   (127)     // �{�����[���ő�l
#define INVALID_DATA_INDEX (0xff)  // �Q�ƃf�[�^�̖����C���f�b�N�X

// �X�C�b�`�̏��
typedef enum{
  SWITCH_STATE_ON,       // ON
  SWITCH_STATE_OFF,      // OFF
  SWITCH_STATE_FADE_IN,  // �t�F�[�h�E�C��
  SWITCH_STATE_FADE_OUT, // �t�F�[�h�E�A�E�g
} SWITCH_STATE; 


//==========================================================================================
// ���X�C�b�`�f�[�^
//========================================================================================== 
typedef struct
{ 
  u16 seqNo;                // �V�[�P���X�ԍ�
  u16 padding1;             // padding
  u16 trackBit[SWITCH_NUM]; // �e�X�C�b�`�̑���Ώۃg���b�N�r�b�g
  u16 padding2;             // padding
  u16 fadeFrame;            // �t�F�[�h����

} SWITCH_DATA;


//========================================================================================== 
// ��ISS-S�V�X�e��
//========================================================================================== 
struct _ISS_SWITCH_SYS
{ 
  u8        switchDataNum;  // �X�C�b�`�f�[�^�̐�
  u8        switchDataIdx;  // �Q�ƃf�[�^�̃C���f�b�N�X
  SWITCH_DATA* switchData;  // �X�C�b�`�f�[�^�z��

  BOOL         boot;                    // �N�����Ă��邩�ǂ���
  SWITCH_STATE switchState[SWITCH_NUM]; // �e�X�C�b�`�̏��
  u16          fadeCount[SWITCH_NUM];   // �t�F�[�h�J�E���^
};


//========================================================================================== 
// ������J�֐��̃v���g�^�C�v�錾
//========================================================================================== 
static void LoadSwitchData( ISS_SWITCH_SYS* sys, HEAPID heap_id );
static void InitSwitch( ISS_SWITCH_SYS* sys );
static void UpdateSwitchDataIdx( ISS_SWITCH_SYS* sys );
static void BootSystem( ISS_SWITCH_SYS* sys );
static void StopSystem( ISS_SWITCH_SYS* sys );
static void SwitchOn( ISS_SWITCH_SYS* sys, SWITCH_INDEX idx );
static void SwitchOff( ISS_SWITCH_SYS* sys, SWITCH_INDEX idx );
static void SwitchUpdate( ISS_SWITCH_SYS* sys, SWITCH_INDEX idx );
static void SwitchUpdate_FADE_IN( ISS_SWITCH_SYS* sys, SWITCH_INDEX idx );
static void SwitchUpdate_FADE_OUT( ISS_SWITCH_SYS* sys, SWITCH_INDEX idx );
static void AllSwitchUpdate( ISS_SWITCH_SYS* sys );
static void CheckSystemDown( ISS_SWITCH_SYS* sys );


//==========================================================================================
// ���쐬�E�j��
//==========================================================================================

//------------------------------------------------------------------------------------------
/**
 * @brief ISS�X�C�b�`�V�X�e�����쐬����
 * 
 * @param heap_id �g�p����q�[�vID
 *
 * @return �쐬�����V�X�e��
 */
//------------------------------------------------------------------------------------------
ISS_SWITCH_SYS* ISS_SWITCH_SYS_Create( HEAPID heap_id )
{
  ISS_SWITCH_SYS* sys;

  // ����
  sys = GFL_HEAP_AllocMemory( heap_id, sizeof(ISS_SWITCH_SYS) );
  sys->switchDataNum = 0;
  sys->switchDataIdx = INVALID_DATA_INDEX;
  sys->switchData    = NULL;
  sys->boot          = FALSE;
  // ������
  InitSwitch( sys );
  LoadSwitchData( sys, heap_id );

  return sys;
}

//------------------------------------------------------------------------------------------
/**
 * @brief ISS�X�C�b�`�V�X�e����j������
 *
 * @param sys �j������V�X�e��
 */
//------------------------------------------------------------------------------------------
void ISS_SWITCH_SYS_Delete( ISS_SWITCH_SYS* sys )
{ 
  GFL_HEAP_FreeMemory( sys->switchData ); // �X�C�b�`�f�[�^
  GFL_HEAP_FreeMemory( sys );             // �{��
}


//==========================================================================================
// ������
//==========================================================================================

//------------------------------------------------------------------------------------------
/**
 * @brief �V�X�e���E���C������
 *
 * @param sys �������V�X�e��
 */
//------------------------------------------------------------------------------------------
void ISS_SWITCH_SYS_Update( ISS_SWITCH_SYS* sys )
{
  // �S�ẴX�C�b�`��Ԃ��X�V����
  AllSwitchUpdate( sys );
}


//==========================================================================================
// ���V�X�e������
//==========================================================================================

//------------------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����N������
 *
 * @param sys �N������V�X�e��
 */
//------------------------------------------------------------------------------------------
void ISS_SWITCH_SYS_On( ISS_SWITCH_SYS* sys )
{
  BootSystem( sys );           // �N��
}

//------------------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����~����
 *
 * @param sys ��~����V�X�e��
 */
//------------------------------------------------------------------------------------------
void ISS_SWITCH_SYS_Off( ISS_SWITCH_SYS* sys )
{
  StopSystem( sys );
}


//==========================================================================================
// ���X�C�b�`����
//==========================================================================================

//------------------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�� ON �ɂ���
 *
 * @param sys �X�C�b�`�������V�X�e��
 * @param idx �����X�C�b�`�̃C���f�b�N�X
 */
//------------------------------------------------------------------------------------------
void ISS_SWITCH_SYS_SwitchOn( ISS_SWITCH_SYS* sys, SWITCH_INDEX idx )
{
  SwitchOn( sys, idx );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�� OFF �ɂ���
 *
 * @param sys �X�C�b�`�������V�X�e��
 * @param idx �����X�C�b�`�̃C���f�b�N�X
 */
//------------------------------------------------------------------------------------------
void ISS_SWITCH_SYS_SwitchOff( ISS_SWITCH_SYS* sys, SWITCH_INDEX idx )
{
  SwitchOff( sys, idx );
}


//========================================================================================== 
// ������J�֐�
//========================================================================================== 

//------------------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�f�[�^��ǂݍ���
 *
 * @param sys     �ǂݍ��񂾃f�[�^��ێ�����V�X�e��
 * @param heap_id �g�p����q�[�vID
 */
//------------------------------------------------------------------------------------------
static void LoadSwitchData( ISS_SWITCH_SYS* sys, HEAPID heap_id )
{
  int data_idx;
  u8 num;
  ARCHANDLE* handle;

  // �A�[�J�C�u�n���h���I�[�v��
  handle = GFL_ARC_OpenDataHandle( ARCID_ISS_SWITCH, heap_id );

  // �o�b�t�@���m��
  num = GFL_ARC_GetDataFileCntByHandle( handle );
  sys->switchDataNum = num;
  sys->switchData = GFL_HEAP_AllocMemory( heap_id, sizeof(SWITCH_DATA) * num );

  // �S�Ẵf�[�^���擾
  for( data_idx=0; data_idx<num; data_idx++ )
  {
    GFL_ARC_LoadDataOfsByHandle( 
        handle, data_idx, 0, sizeof(SWITCH_DATA), &sys->switchData[data_idx] );
  }

  // �A�[�J�C�u�n���h���N���[�Y
  GFL_ARC_CloseDataHandle( handle );

  // DEBUG:
  OBATA_Printf( "ISS-S: load switch data\n" );
  for( data_idx=0; data_idx<num; data_idx++ )
  {
    int bit_idx;
    int swt_idx;
    SWITCH_DATA* data = &sys->switchData[data_idx];
    OBATA_Printf( "- switchData[%d].seqNo = %d\n", data_idx, data->seqNo );
    for( swt_idx=0; swt_idx<SWITCH_NUM; swt_idx++ )
    {
      OBATA_Printf( "- switchData[%d].trackBit[%d] = ", data_idx, swt_idx );
      for( bit_idx=0; bit_idx<TRACK_NUM; bit_idx++ )
      {
        if( data->trackBit[swt_idx] & (1<<(TRACK_NUM-1-bit_idx)) ) OBATA_Printf( "��" );
        else                                                       OBATA_Printf( "��" );
      }
      OBATA_Printf( "\n" );
    }
    OBATA_Printf( "- switchData[%d].fadeFrame = %d\n", data_idx, data->fadeFrame );
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`������������
 *
 * @param sys �������ΏۃV�X�e��
 */
//------------------------------------------------------------------------------------------
static void InitSwitch( ISS_SWITCH_SYS* sys )
{
  int i;
  for( i=0; i<SWITCH_NUM; i++ )
  {
    sys->switchState[i] = SWITCH_STATE_OFF;
    sys->fadeCount[i]   = 0;
  }
  // DEBUG:
  OBATA_Printf( "ISS-S: init switch\n" );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �Q�Ƃ���X�C�b�`�f�[�^���X�V����
 *
 * @param sys �X�V����V�X�e��
 */
//------------------------------------------------------------------------------------------
static void UpdateSwitchDataIdx( ISS_SWITCH_SYS* sys )
{
  u32 bgm;
  int data_idx;

  // ���݂�BGM�ɑ΂���X�C�b�`�f�[�^������
  bgm = PMSND_GetBGMsoundNo();
  for( data_idx=0; data_idx<sys->switchDataNum; data_idx++ )
  {
    // ����
    if( sys->switchData[data_idx].seqNo == bgm )
    {
      sys->switchDataIdx = data_idx;
      // DEBUG:
      OBATA_Printf( "ISS-S: update switch data index --> %d\n", data_idx );
      return;
    }
  }

  // �Y������f�[�^�������Ă��Ȃ�
  sys->switchDataIdx = INVALID_DATA_INDEX;
  OBATA_Printf( "ISS-S: don't have switch data of bgm seq %d\n", bgm );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����N������
 *
 * @param sys �N��������V�X�e��
 */
//------------------------------------------------------------------------------------------
static void BootSystem( ISS_SWITCH_SYS* sys )
{
  // ���łɋN�����Ă���
  if( sys->boot ) return;

  // �N��
  sys->boot = TRUE;

  // �X�C�b�`0�݂̂�������Ă����Ԃɏ�����
  InitSwitch( sys );
  UpdateSwitchDataIdx( sys );
  PMSND_ChangeBGMVolume( TRACKBIT_ALL, 0 );
  PMSND_ChangeBGMVolume( sys->switchData[sys->switchDataIdx].trackBit[0], MAX_VOLUME );
  sys->switchState[0] = SWITCH_STATE_ON;

  // DEBUG:
  OBATA_Printf( "ISS-S: boot\n" );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����~����
 *
 * @param sys ��~������V�X�e��
 */
//------------------------------------------------------------------------------------------
static void StopSystem( ISS_SWITCH_SYS* sys )
{
  // ���łɒ�~���Ă���
  if( !sys->boot ) return;

  // ��~
  sys->boot = FALSE;

  // DEBUG:
  OBATA_Printf( "ISS-S: stop\n" );
}

//------------------------------------------------------------------------------------------
/**
 * @beirf �X�C�b�`������
 *
 * @param sys �X�C�b�`�������V�X�e��
 * @param idx �����X�C�b�`�̃C���f�b�N�X
 */
//------------------------------------------------------------------------------------------
static void SwitchOn( ISS_SWITCH_SYS* sys, SWITCH_INDEX idx )
{
  // �N�����Ă��Ȃ�
  if( !sys->boot ) return; 
  // �X�C�b�`���������Ԃ���Ȃ�
  if( sys->switchState[idx] != SWITCH_STATE_OFF ) return;

  // �X�C�b�`ON(�t�F�[�h�C���J�n)
  sys->switchState[idx] = SWITCH_STATE_FADE_IN;
  sys->fadeCount[idx]   = 0;

  // DEBUG:
  OBATA_Printf( "ISS-S: switch %d on\n", idx );
}

//------------------------------------------------------------------------------------------
/**
 * @beirf �X�C�b�`�𗣂�
 *
 * @param sys �X�C�b�`�������V�X�e��
 * @param idx �����X�C�b�`�̃C���f�b�N�X
 */
//------------------------------------------------------------------------------------------
static void SwitchOff( ISS_SWITCH_SYS* sys, SWITCH_INDEX idx )
{
  // �N�����Ă��Ȃ�
  if( !sys->boot ) return; 
  // �X�C�b�`���������Ԃ���Ȃ�
  if( sys->switchState[idx] != SWITCH_STATE_ON ) return;

  // �X�C�b�`OFF(�t�F�[�h�A�E�g�J�n)
  sys->switchState[idx] = SWITCH_STATE_FADE_OUT;
  sys->fadeCount[idx]   = 0;

  // DEBUG:
  OBATA_Printf( "ISS-S: switch %d off\n", idx );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�̏�Ԃ��X�V����
 *
 * @param sys �X�V����V�X�e��
 * @param idx �X�V����X�C�b�`���w��
 */
//------------------------------------------------------------------------------------------
static void SwitchUpdate( ISS_SWITCH_SYS* sys, SWITCH_INDEX idx )
{
  switch( sys->switchState[idx] )
  {
  case SWITCH_STATE_FADE_IN:   SwitchUpdate_FADE_IN( sys, idx );   break;
  case SWITCH_STATE_FADE_OUT:  SwitchUpdate_FADE_OUT( sys, idx );  break;
  case SWITCH_STATE_ON:
  case SWITCH_STATE_OFF:
    // �ω����Ȃ�
    break;
  default:
    // �G���[
    OBATA_Printf( "==================\n" );
    OBATA_Printf( "ISS-S: state error\n" ); 
    OBATA_Printf( "==================\n" );
    break;
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�̏�Ԃ��X�V����(FADE_IN)
 *
 * @param sys �X�V����V�X�e��
 * @param idx �X�V����X�C�b�`���w��
 */
//------------------------------------------------------------------------------------------
static void SwitchUpdate_FADE_IN( ISS_SWITCH_SYS* sys, SWITCH_INDEX idx )
{
  SWITCH_DATA* data;

  // �Q�ƃf�[�^���擾
  data = &sys->switchData[sys->switchDataIdx];  

  // �X�V
  sys->fadeCount[idx]++;
  {
    float now = (float)sys->fadeCount[idx];
    float end = (float)data->fadeFrame;
    int   vol = MAX_VOLUME * (now / end);
    PMSND_ChangeBGMVolume( data->trackBit[idx], vol );
    // DEBUG:
    OBATA_Printf( "ISS-S: fade in volume switch[%d] ==> %d\n", idx, vol );
  }
  // �t�F�[�h�C�����I�������� ON ��Ԃ�
  if( data->fadeFrame <= sys->fadeCount[idx] )
  {
    sys->switchState[idx] = SWITCH_STATE_ON;
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�̏�Ԃ��X�V����(FADE_OUT)
 *
 * @param sys �X�V����V�X�e��
 * @param idx �X�V����X�C�b�`���w��
 */
//------------------------------------------------------------------------------------------
static void SwitchUpdate_FADE_OUT( ISS_SWITCH_SYS* sys, SWITCH_INDEX idx )
{
  SWITCH_DATA* data;

  // �Q�ƃf�[�^���擾
  data = &sys->switchData[sys->switchDataIdx];  

  // �X�V
  sys->fadeCount[idx]++;
  {
    float now = (float)sys->fadeCount[idx];
    float end = (float)data->fadeFrame;
    int   vol = MAX_VOLUME * ( 1.0f - (now / end) );
    PMSND_ChangeBGMVolume( data->trackBit[idx], vol );
    // DEBUG:
    OBATA_Printf( "ISS-S: fade out volume switch[%d] ==> %d\n", idx, vol );
  }
  // �t�F�[�h�A�E�g���I�������� OFF ��Ԃ�
  if( data->fadeFrame <= sys->fadeCount[idx] )
  {
    sys->switchState[idx] = SWITCH_STATE_OFF;
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief �S�ẴX�C�b�`��Ԃ��X�V����
 *
 * @param sys �X�V����X�C�b�`
 */
//------------------------------------------------------------------------------------------
static void AllSwitchUpdate( ISS_SWITCH_SYS* sys )
{
  int swt_idx;

  // �Q�ƃf�[�^�����݂��Ȃ�
  if( sys->switchDataNum <= sys->switchDataIdx ) return;

  // �S�ẴX�C�b�`��Ԃ��X�V
  for( swt_idx=0; swt_idx<SWITCH_NUM; swt_idx++ )
  {
    SwitchUpdate( sys, swt_idx );
  }
}
