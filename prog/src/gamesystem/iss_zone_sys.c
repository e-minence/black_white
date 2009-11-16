////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief ISS�]�[���V�X�e��
 * @file iss_zone_sys.c
 * @author obata
 * @date 2009.11.06
 */
////////////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "gamesystem/playerwork.h"
#include "iss_zone_sys.h"
#include "sound/pm_sndsys.h"    // for PMSND_ChangeBGMVolume
#include "gamesystem/game_data.h"
#include "field/zonedata.h"
#include "arc/arc_def.h"  // for ARCID_ISS_ZONE


//==========================================================================================
// ���]�[�����Ƃ̃f�[�^
//==========================================================================================
typedef struct
{
  u16 zoneID;        // �]�[��ID
  u16 openTrackBit;  // �J���g���b�N�r�b�g
  u16 closeTrackBit; // ���g���b�N�r�b�g

} ZONE_DATA;


//==========================================================================================
// ���V�X�e�����[�N
//==========================================================================================
struct _ISS_ZONE_SYS
{
  u8      zoneDataNum;  // �ێ��f�[�^��
  ZONE_DATA* zoneData;  // �f�[�^�z��
  u8   currentDataIdx;  // �Q�ƃf�[�^�̃C���f�b�N�X
};


//==========================================================================================
// ������J�֐��̃v���g�^�C�v�錾
//==========================================================================================
static void LoadZoneData( ISS_ZONE_SYS* sys, HEAPID heap_id );
static u8 SearchZoneData( const ISS_ZONE_SYS* sys, u16 zone_id );


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
  sys->zoneDataNum = 0;
  sys->zoneData    = NULL;

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
 * @brief �]�[���؂�ւ���ʒm����
 *
 * @param sys          �ʒm�Ώۂ̃V�X�e��
 * @param next_zone_id �V�����]�[��ID
 */
//------------------------------------------------------------------------------------------
void ISS_ZONE_SYS_ZoneChange( ISS_ZONE_SYS* sys, u16 next_zone_id )
{
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
      if( sys->zoneData[data_idx].openTrackBit & (1 << (15-j)) ) OBATA_Printf( "1" );
      else                                                       OBATA_Printf( "0" );
    }
  }
} 
