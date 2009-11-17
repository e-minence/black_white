////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief ISS�]�[���V�X�e��
 * @file iss_zone_sys.c
 * @author obata
 * @date 2009.11.06
 */
////////////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "iss_zone_sys.h"
#include "sound/pm_sndsys.h"  // for PMSND_ChangeBGMVolume
#include "arc/arc_def.h"      // for ARCID_ISS_ZONE


//==========================================================================================
// ���萔
//==========================================================================================
#define TRACK_NUM          (16)             // �g���b�N��
#define MAX_TRACK_NO       (TRACK_NUM - 1)  // �g���b�N�ԍ��̍ő�l
#define INVALID_DATA_INDEX (0xff)           // �Q�ƃf�[�^�Ȃ�
#define MAX_VOLUME         (127)            // �{�����[���ő�l


//==========================================================================================
// ���]�[�����Ƃ̃f�[�^
//==========================================================================================
typedef struct
{
  u16 zoneID;        // �]�[��ID
  u16 openTrackBit;  // �J���g���b�N�r�b�g
  u16 closeTrackBit; // ���g���b�N�r�b�g
  u16 fadeFrame;     // �t�F�[�h����

} ZONE_DATA;


//==========================================================================================
// ���V�X�e�����[�N
//==========================================================================================
struct _ISS_ZONE_SYS
{
  BOOL boot;  // �N�����Ă��邩�ǂ���

  u8      zoneDataNum;  // �ێ��f�[�^��
  ZONE_DATA* zoneData;  // �f�[�^�z��
  u8   currentDataIdx;  // �Q�ƃf�[�^�̃C���f�b�N�X

  u16 fadeInTrackBit;   // �t�F�[�h�C��������g���b�N�r�b�g
  u16 fadeOutTrackBit;  // �t�F�[�h�A�E�g������g���b�N�r�b�g
  u16 fadeFrame;        // �t�F�[�h����
  u16 fadeCount;        // �t�F�[�h�J�E���^
};


//==========================================================================================
// ������J�֐��̃v���g�^�C�v�錾
//==========================================================================================
static void LoadZoneData( ISS_ZONE_SYS* sys, HEAPID heap_id );
static void BootSystem( ISS_ZONE_SYS* sys, u16 zone_id );
static void StopSystem( ISS_ZONE_SYS* sys );
static void ChangeZoneData( ISS_ZONE_SYS* sys, u16 zone_id );
static u8 SearchZoneData( const ISS_ZONE_SYS* sys, u16 zone_id );
static void UpdateFade( ISS_ZONE_SYS* sys );


//==========================================================================================
// ���쐬�E�j��
//==========================================================================================

//------------------------------------------------------------------------------------------
/**
 * @brief ISS�]�[���V�X�e�����쐬����
 * 
 * @param heap_id �g�p����q�[�vID
 *
 * @return �쐬�����V�X�e��
 */
//------------------------------------------------------------------------------------------
ISS_ZONE_SYS* ISS_ZONE_SYS_Create( HEAPID heap_id )
{
  ISS_ZONE_SYS* sys;

  // ����
  sys = GFL_HEAP_AllocMemory( heap_id, sizeof(ISS_ZONE_SYS) );
  sys->boot            = FALSE;
  sys->zoneDataNum     = 0;
  sys->zoneData        = NULL;
  sys->currentDataIdx  = INVALID_DATA_INDEX;
  sys->fadeInTrackBit  = 0;
  sys->fadeOutTrackBit = 0;
  sys->fadeFrame       = 0;
  sys->fadeCount       = 0;

  // �f�[�^�ǂݍ���
  LoadZoneData( sys, heap_id );
  return sys;
}

//------------------------------------------------------------------------------------------
/**
 * @brief ISS�]�[���V�X�e����j������
 *
 * @param sys �j������V�X�e��
 */
//------------------------------------------------------------------------------------------
void ISS_ZONE_SYS_Delete( ISS_ZONE_SYS* sys )
{ 
  // �]�[���f�[�^
  if( sys->zoneData )
  {
    GFL_HEAP_FreeMemory( sys->zoneData );
  }
  // �V�X�e���{��
  GFL_HEAP_FreeMemory( sys );
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
void ISS_ZONE_SYS_Update( ISS_ZONE_SYS* sys )
{
  // �N�����Ă��Ȃ�
  if( !sys->boot ) return;

  // �t�F�[�h��Ԃ��X�V
  UpdateFade( sys );
}


//==========================================================================================
// ������
//==========================================================================================

//------------------------------------------------------------------------------------------
/**
 * @brief �]�[���؂�ւ���ʒm����
 *
 * @param sys          �ʒm�Ώۂ̃V�X�e��
 * @param next_zone_id �V�����]�[��ID
 */
//------------------------------------------------------------------------------------------
void ISS_ZONE_SYS_ZoneChange( ISS_ZONE_SYS* sys, u16 next_zone_id )
{
  // �N�����Ă��Ȃ� ==> �]�[���f�[�^�����݂���ꍇ�͋N��
  if( !sys->boot )
  {
    BootSystem( sys, next_zone_id );
  }

  // �N������
  if( !sys->boot ) return; 

  // �Q�Ƃ���f�[�^��ύX
  ChangeZoneData( sys, next_zone_id );
} 

//------------------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����N������
 *
 * @param sys     �N������V�X�e��
 * @param zone_id �N�����̃]�[��ID
 */
//------------------------------------------------------------------------------------------
void ISS_ZONE_SYS_On( ISS_ZONE_SYS* sys, u16 zone_id )
{
  BootSystem( sys, zone_id );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����~����
 *
 * @param sys     ��~����V�X�e��
 */
//------------------------------------------------------------------------------------------
void ISS_ZONE_SYS_Off( ISS_ZONE_SYS* sys )
{
  StopSystem( sys );
}


//==========================================================================================
// ������J�֐�
//==========================================================================================

//------------------------------------------------------------------------------------------
/**
 * @brief �]�[���f�[�^��ǂݍ���
 *
 * @param sys     �ǂݍ��񂾃f�[�^��ێ�����V�X�e��
 * @param heap_id �g�p����q�[�vID
 */
//------------------------------------------------------------------------------------------
static void LoadZoneData( ISS_ZONE_SYS* sys, HEAPID heap_id )
{
  int data_idx;
  int num;
  ARCHANDLE* handle = GFL_ARC_OpenDataHandle( ARCID_ISS_ZONE, heap_id );

  // �f�[�^�����擾
  num = GFL_ARC_GetDataFileCntByHandle( handle );
  sys->zoneDataNum = num;

  // �o�b�t�@���m��
  sys->zoneData = GFL_HEAP_AllocMemory( heap_id, sizeof(ZONE_DATA) * num );

  // �f�[�^��ǂݍ���
  for( data_idx=0; data_idx<num; data_idx++ )
  {
    GFL_ARC_LoadDataOfsByHandle( 
        handle, data_idx, 0, sizeof(ZONE_DATA), &sys->zoneData[data_idx] );
  }
  GFL_ARC_CloseDataHandle( handle );

  // DEBUG:
  OBATA_Printf( "ISS-Z: load zone data\n" );
  for( data_idx=0; data_idx<num; data_idx++ )
  {
    int j;
    OBATA_Printf( "- zoneData[%d]\n", data_idx );
    OBATA_Printf( "-- zoneID = %d\n", sys->zoneData[data_idx].zoneID ); 
    OBATA_Printf( "-- openTrack = " );
    for( j=0; j<16; j++ )
    {
      if( sys->zoneData[data_idx].openTrackBit & (1 << (MAX_TRACK_NO-j)) ) 
        OBATA_Printf( "1" );
      else                                                       
        OBATA_Printf( "0" );
    }
    OBATA_Printf( "\n" );
    OBATA_Printf( "-- closeTrack = " );
    for( j=0; j<16; j++ )
    {
      if( sys->zoneData[data_idx].closeTrackBit & (1 << (MAX_TRACK_NO-j)) ) 
        OBATA_Printf( "1" );
      else                                                       
        OBATA_Printf( "0" );
    }
    OBATA_Printf( "\n" );
    OBATA_Printf( "-- fadeFrame = %d\n", sys->zoneData[data_idx].fadeFrame ); 
  }
} 

//------------------------------------------------------------------------------------------
/**
 * @brief �V�X�e���N������
 *
 * @param sys     �N������V�X�e��
 * @param zone_id �����]�[��ID
 */
//------------------------------------------------------------------------------------------
static void BootSystem( ISS_ZONE_SYS* sys, u16 zone_id )
{
  u8 idx;

  // ���łɋN�����Ă���
  if( sys->boot ) return;

  // �]�[���f�[�^������
  idx = SearchZoneData( sys, zone_id );

  // �w��]�[���̃f�[�^�������Ă��Ȃ�
  if( idx == INVALID_DATA_INDEX ) return;

  // ������Ԃ�ݒ�
  sys->boot            = TRUE;
  sys->currentDataIdx  = idx;
  sys->fadeInTrackBit  = sys->zoneData[idx].openTrackBit;
  sys->fadeOutTrackBit = sys->zoneData[idx].closeTrackBit;
  sys->fadeFrame       = 1;   // �N�����̓t�F�[�h�����Ŕ��f������
  sys->fadeCount       = 0;

  // DEBUG:
  OBATA_Printf( "ISS-Z: boot\n" );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �V�X�e����~����
 *
 * @param sys ��~����V�X�e��
 */
//------------------------------------------------------------------------------------------
static void StopSystem( ISS_ZONE_SYS* sys )
{
  // ���łɒ�~���Ă���
  if( !sys->boot ) return;

  // ��~
  sys->boot = FALSE;

  // DEBUG:
  OBATA_Printf( "ISS-Z: stop\n" );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �Q�ƃf�[�^��ύX����
 *
 * @param sys     �ύX����V�X�e��
 * @param zone_id �ύX��f�[�^�̃]�[��ID
 */
//------------------------------------------------------------------------------------------
static void ChangeZoneData( ISS_ZONE_SYS* sys, u16 zone_id )
{
  u8 now;
  u8 next;
  ZONE_DATA* data;

  // �Q�Ƃ���]�[���f�[�^������
  now  = sys->currentDataIdx;
  next = SearchZoneData( sys, zone_id ); 
  data = sys->zoneData;

  // �V���ɎQ�Ƃ���f�[�^�����݂��Ȃ�
  if( next == INVALID_DATA_INDEX ) return;

  // �t�F�[�h��Ԃ��X�V
  sys->fadeInTrackBit  = data[now].closeTrackBit & data[next].openTrackBit;
  sys->fadeOutTrackBit = data[now].openTrackBit  & data[next].closeTrackBit;
  sys->fadeFrame       = data[next].fadeFrame;
  sys->fadeCount       = 0;

  // �Q�Ɛ�f�[�^�̃C���f�b�N�X���X�V
  sys->currentDataIdx = next;

  // DEBUG:
  OBATA_Printf( "ISS-Z: change zone data\n" );
  OBATA_Printf( "- now = %d\n", now );
  OBATA_Printf( "- next = %d\n", next );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �]�[��ID���L�[�ɂ���, �f�[�^�̃C���f�b�N�X����������
 *
 * @param sys     �����ΏۃV�X�e��
 * @param zone_id �����f�[�^�̃]�[��ID
 *
 * @return �w�肵���]�[��ID�����f�[�^�̃C���f�b�N�X
 *         ���݂��Ȃ��ꍇ�� INVALID_DATA_INDEX ��Ԃ�
 */
//------------------------------------------------------------------------------------------
static u8 SearchZoneData( const ISS_ZONE_SYS* sys, u16 zone_id )
{
  u8 idx;

  for( idx=0; idx<sys->zoneDataNum; idx++ )
  {
    // ����
    if( sys->zoneData[idx].zoneID == zone_id )
    {
      return idx;
    }
  }

  // ���݂��Ȃ�
  OBATA_Printf( "====================================\n" );
  OBATA_Printf( "ISS-Z: don't have data (zoneID = %d)\n", zone_id );
  OBATA_Printf( "====================================\n" );
  return INVALID_DATA_INDEX;  
}

//------------------------------------------------------------------------------------------
/**
 * @brief �t�F�[�h��Ԃ��X�V����
 *
 * @param sys �X�V����V�X�e��
 */
//------------------------------------------------------------------------------------------
static void UpdateFade( ISS_ZONE_SYS* sys )
{
  int vol;

  // ���łɊ������Ă���
  if( sys->fadeFrame <= sys->fadeCount ) return;

  // �J�E���^�X�V
  sys->fadeCount++;

  // �t�F�[�h�C��
  if( sys->fadeInTrackBit )
  {
    vol = MAX_VOLUME * ((float)sys->fadeCount / (float)sys->fadeFrame);
    PMSND_ChangeBGMVolume( sys->fadeInTrackBit, vol );
    // DEBUG:
    OBATA_Printf( "ISS-Z: update fade in ==> %d\n", vol );
  }
  // �t�F�[�h�A�E�g
  if( sys->fadeOutTrackBit )
  {
    vol = MAX_VOLUME * ( 1.0f - ((float)sys->fadeCount / (float)sys->fadeFrame) );
    PMSND_ChangeBGMVolume( sys->fadeOutTrackBit, vol );
    // DEBUG:
    OBATA_Printf( "ISS-Z: update fade out ==> %d\n", vol );
  } 
}
