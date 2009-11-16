////////////////////////////////////////////////////////////////////////////////////////////
/**
 * 
 * @brief ISS�]�[���V�X�e��
 * @file iss_zone_sys.c
 * @author obata
 * @date 2009.11.06
 *
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
  u16 zoneID;         // �]�[��ID
  u16 padding;
  u8 initVolume[16];  // �e�g���b�N�̏�������
} ZONE_DATA;


//==========================================================================================
// ���V�X�e�����[�N
//==========================================================================================
struct _ISS_ZONE_SYS
{
  BOOL     active;  // �N�����Ă��邩�ǂ���
  BOOL   initFlag;  // ���ʏ��������s�t���O
  u16  initZoneID;  // ���������s�Ώۂ̃]�[��ID

  GAMEDATA*     gdata;  // �Ď��Ώۂ̃Q�[���f�[�^
  PLAYER_WORK* player;  // �Ď��Ώۂ̎��@

  u8         zoneDataNum;  // �ێ��f�[�^��
  ZONE_DATA*    zoneData;  // �f�[�^�z��
};


//==========================================================================================
// ������J�֐��̃v���g�^�C�v�錾
//==========================================================================================
static void InitISSZoneSystem(
    ISS_ZONE_SYS* sys, GAMEDATA* gdata, PLAYER_WORK* player, HEAPID heap_id );
static void LoadZoneData( ISS_ZONE_SYS* sys, HEAPID heap_id );
static void InitBGMVolume( ISS_ZONE_SYS* sys );
static void CheckSystemDown( ISS_ZONE_SYS* sys );
static void SystemOn( ISS_ZONE_SYS* sys );
static void SystemOff( ISS_ZONE_SYS* sys );


//==========================================================================================
// ���쐬�E�j��
//==========================================================================================

//------------------------------------------------------------------------------------------
/**
 * @brief ISS�]�[���V�X�e�����쐬����
 * 
 * @param gdata   �Ď��Ώۂ̃Q�[���f�[�^
 * @param player  �Ď��Ώۂ̎��@
 * @param heap_id �g�p����q�[�vID
 *
 * @return �쐬�����V�X�e��
 */
//------------------------------------------------------------------------------------------
ISS_ZONE_SYS* ISS_ZONE_SYS_Create( GAMEDATA* gdata, PLAYER_WORK* player, HEAPID heap_id )
{
  ISS_ZONE_SYS* sys;

  // ����
  sys = GFL_HEAP_AllocMemory( heap_id, sizeof(ISS_ZONE_SYS) );

  // ������
  InitISSZoneSystem( sys, gdata, player, heap_id );
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
 * @brief ���C������
 *
 * @param sys �������V�X�e��
 */
//------------------------------------------------------------------------------------------
void ISS_ZONE_SYS_Update( ISS_ZONE_SYS* sys )
{ 
  // �N�����ĂȂ�
  if( sys->active != TRUE ) return;

  // BGM�{�����[���̏�����
  InitBGMVolume( sys );

  // �V�X�e����~�`�F�b�N
  CheckSystemDown( sys );
}

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
  // ��������o�^
  sys->initFlag   = TRUE;
  sys->initZoneID = next_zone_id;

  // �V�X�e���N��
  SystemOn( sys );
} 


//==========================================================================================
// ������J�֐�
//==========================================================================================

//------------------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����[�N������������
 *
 * @param sys     ����������V�X�e�����[�N
 * @param gdata   �Ď��Ώۂ̃Q�[���f�[�^
 * @param player  �Ď��Ώۂ̎��@
 * @param heap_id �g�p����q�[�vID
 */
//------------------------------------------------------------------------------------------
static void InitISSZoneSystem( 
    ISS_ZONE_SYS* sys, GAMEDATA* gdata, PLAYER_WORK* player, HEAPID heap_id )
{
  sys->active      = FALSE;
  sys->initFlag    = FALSE;
  sys->initZoneID  = 0;
  sys->gdata       = gdata;
  sys->player      = player;
  sys->zoneDataNum = 0;
  sys->zoneData    = NULL;

  // �]�[���f�[�^��ǂݍ���
  LoadZoneData( sys, heap_id );
}

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
  int i;
  int num;
  ARCHANDLE* handle = GFL_ARC_OpenDataHandle( ARCID_ISS_ZONE, heap_id );

  // �f�[�^�����擾
  num = GFL_ARC_GetDataFileCntByHandle( handle );
  sys->zoneDataNum = num;

  // �o�b�t�@���m��
  sys->zoneData = GFL_HEAP_AllocMemory( heap_id, sizeof(ZONE_DATA) * num );

  // �f�[�^��ǂݍ���
  for( i=0; i<num; i++ )
  {
    GFL_ARC_LoadDataOfsByHandle( handle, i, 0, sizeof(ZONE_DATA), &sys->zoneData[i] );
  }
  GFL_ARC_CloseDataHandle( handle );

  // DEBUG:
  OBATA_Printf( "ISS-Z: load zone data\n" );
  for( i=0; i<num; i++ )
  {
    int j;
    OBATA_Printf( "- zoneData[%d]\n", i );
    OBATA_Printf( "-- zoneID = %d\n", sys->zoneData[i].zoneID ); 
    for( j=0; j<16; j++ )
    {
      OBATA_Printf( "-- track[%d].volume = %d\n", j, sys->zoneData[i].initVolume[j] );
    }
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief BGM�̃{�����[��������������
 *        ���]�[���f�[�^�̓�, ���ʂ�0�̃g���b�N�݂̂𔽉f������B
 *        @todo �K�v�ɉ����ĕύX�B
 *
 * @param �����������s����V�X�e��
 */
//------------------------------------------------------------------------------------------ 
static void InitBGMVolume( ISS_ZONE_SYS* sys )
{
  int i;
  u16 zone_id = PLAYERWORK_getZoneID( sys->player );

  // ���������o�^����Ă��Ȃ�
  if( sys->initFlag != TRUE )
  {
    return;
  }
  // ���@���w��]�[���ɂ��Ȃ�
  if( sys->initZoneID != zone_id )
  {
    return;
  }
  // �w��]�[����BGM���Đ�����Ă��Ȃ�
  {
    u8 season = GAMEDATA_GetSeasonID( sys->gdata );
    u16 def = ZONEDATA_GetBGMID( sys->initZoneID, season );
    u16 now = PMSND_GetBGMsoundNo();
    if( now != def )
    {
      return;
    }
  }

  // �����f�[�^��������
  for( i=0; i<sys->zoneDataNum; i++ )
  {
    // �w��]�[���̃f�[�^�𔭌�
    if( sys->zoneData[i].zoneID == zone_id )
    {
      int track;
      u16 track_bit = 0;
      // ����0�̃g���b�N�����o
      for( track=0; track<16; track++ )
      {
        if( sys->zoneData[track].initVolume == 0 )
        {
          track_bit |= (1 << track);
        }
      }
      PMSND_ChangeBGMVolume( track_bit, 0 );  // ���ʂ�ύX
      // DEBUG:
      OBATA_Printf( "ISS-Z: init BGM volume\n" );
      break;
    }
  }
  sys->initFlag = FALSE;
}

//------------------------------------------------------------------------------------------ 
/**
 * @brief �V�X�e����~�`�F�b�N
 *
 * @param sys �`�F�b�N���s���V�X�e��
 */
//------------------------------------------------------------------------------------------ 
static void CheckSystemDown( ISS_ZONE_SYS* sys )
{
  // ���ׂĂ̎d�����Ȃ��Ȃ������~
  if( sys->initFlag != TRUE )
  {
    SystemOff( sys );
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����N������
 *
 * @param sys �N������V�X�e��
 */
//------------------------------------------------------------------------------------------
static void SystemOn( ISS_ZONE_SYS* sys )
{
  // ���łɋN���ς�
  if( sys->active ) return;

  // �N��
  sys->active = TRUE;

  // DEBUG:
  OBATA_Printf( "ISS-Z: On\n" );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����~������
 *
 * @param sys ��~������V�X�e��
 */
//------------------------------------------------------------------------------------------
static void SystemOff( ISS_ZONE_SYS* sys )
{
  // ���łɒ�~�ς�
  if( sys->active != TRUE ) return;

  // ��~
  sys->active = FALSE;

  // DEBUG:
  OBATA_Printf( "ISS-Z: Off\n" );
}
