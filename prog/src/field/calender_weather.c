#include "calender_weather.h"
#include "arc/arc_def.h"
#include "arc/calender.naix"
#include "field/weather_no.h"


//////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief 1�̃]�[���Ɋւ���, 1�N�Ԃ̓V�C�f�[�^
 */
//////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
  u8  zoneID;   // �]�[��ID
  u16 dateNum;  // �o�^���t��
  u8* month;    // ���z��
  u8* day;      // ���z��
  u8* weather;  // �V�C�ԍ��z��
}
WEATHER_DATA;


//========================================================================================
/**
 * @breif ����J�֐��̃v���g�^�C�v�錾
 */
//========================================================================================

// �w�肵�����t�̓V�C���擾����
static u8 GetWeather( WEATHER_DATA* p_data, const RTCDate* p_date );


//========================================================================================
/**
 * @breif ����J�֐��̒�`
 */
//========================================================================================

//----------------------------------------------------------------------------------------
/**
 * @brief �w�肵�����t�̓V�C���擾����
 *
 * @param p_data �V�C�f�[�^
 * @param p_date �w���
 *
 * @return �w����̓V�C
 */
//----------------------------------------------------------------------------------------
static u8 GetWeather( WEATHER_DATA* p_data, const RTCDate* p_date )
{
  int i;
  int index = 0;

  // �w������܂܂��ꏊ������
  for( i=1; i<p_data->dateNum; i++ )
  {
    // �w����𔭌�
    if( ( p_data->month[i] == p_date->month ) && ( p_data->day[i] == p_date->day ) )
    {
      index = i;
      break;
    }
    // �w�������̓��t�𔭌�
    if( ( p_date->month < p_data->month[i] ) ||
        ( ( p_date->month == p_data->month[i] ) && ( p_date->day < p_data->day[i] ) ) )
    {
      index = i - 1;
      break;
    }
  }

  // �w����̓V�C��Ԃ�
  return p_data->weather[ index ];
}


//////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief �V�X�e�����[�N
 */
//////////////////////////////////////////////////////////////////////////////////////////
struct _CALENDER_WEATHER_INFO
{
  u16           zoneNum;  // �o�^�]�[����
  WEATHER_DATA* data;     // �e�]�[���̓V�C�f�[�^
};


//========================================================================================
/**
 * @brief ����J�֐��̃v���g�^�C�v�錾
 */
//========================================================================================

// �w�肵��2�o�C�g�����g���G���f�B�A���Ƃ��ĉ��߂�, �l���擾����
static u16 GetU16( u8* data, int pos );

// �f�[�^��ǂݍ���
static void LoadData( CALENDER_WEATHER_INFO* p_info, HEAPID heap_id );


//========================================================================================
/**
 * @brief ���J�֐��̒�`
 */
//========================================================================================

//----------------------------------------------------------------------------------------
/**
 * @brief �J�����_�[�V�C�f�[�^���쐬����
 *
 * @param heap_id �g�p����q�[�vID
 *
 * @return �쐬�����J�����_�[�V�C�f�[�^
 */
//----------------------------------------------------------------------------------------
CALENDER_WEATHER_INFO* CALENDER_WEATHER_INFO_Create( HEAPID heap_id )
{
  CALENDER_WEATHER_INFO* p_info;

  // ���[�N���쐬
  p_info = (CALENDER_WEATHER_INFO*)GFL_HEAP_AllocMemory( heap_id, sizeof( CALENDER_WEATHER_INFO ) );

  // �f�[�^��ǂݍ���
  LoadData( p_info, heap_id );

  // �쐬�����f�[�^��Ԃ�
  return p_info;
}

//----------------------------------------------------------------------------------------
/**
 * @brief �w����E�w��]�[���̓V�C���擾����
 *
 * @param p_info  �J�����_�[�V�C�f�[�^
 * @param p_date  ���t
 * @param zone_id �]�[��ID
 *
 * @return �w����̎w��]�[���̓V�C
 */
//----------------------------------------------------------------------------------------
u16 CALENDER_WEATHER_INFO_GetWeather( CALENDER_WEATHER_INFO* p_info, const RTCDate* p_date, u16 zone_id )
{
  int i;
  u16 weather_no = WEATHER_NO_NONE;

  // �w��]�[��������
  for( i=0; i<p_info->zoneNum; i++ )
  { 
    // ����
    if( p_info->data[i].zoneID == zone_id )
    {
      weather_no = (u16)GetWeather( &p_info->data[i], p_date );
      break;
    }
  }

  // DEBUG:
  OBATA_Printf( "%2d/%2d : zone_id = %d : weather no = %d\n", p_date->month, p_date->day, zone_id, weather_no );

  // �w��]�[�����o�^����Ă��Ȃ����, �����l��Ԃ�
  return weather_no;
}

//----------------------------------------------------------------------------------------
/**
 * @brief �J�����_�[�V�C�f�[�^��j������
 *
 * @param p_info �j������f�[�^
 */ 
//----------------------------------------------------------------------------------------
void CALENDER_WEATHER_INFO_Delete( CALENDER_WEATHER_INFO* p_info )
{
  int i;

  // �S�]�[���̓V�C�f�[�^��j��
  for( i=0; i<p_info->zoneNum; i++ )
  {
    GFL_HEAP_FreeMemory( p_info->data[i].month ); 
    GFL_HEAP_FreeMemory( p_info->data[i].day );
    GFL_HEAP_FreeMemory( p_info->data[i].weather );
  } 

  // �{�̂�j��
  GFL_HEAP_FreeMemory( p_info->data );
  GFL_HEAP_FreeMemory( p_info );
}


//==================================================================================
/**
 * @brief ����J�֐��̒�`
 */
//==================================================================================

//---------------------------------------------------------------------------- 
/**
 * @brief data + pos �̈ʒu����n�܂�2�o�C�g��, 
 *        ���g���G���f�B�A���ŕ���u16�̃f�[�^�Ƃ��ĉ��߂��l���擾����
 *
 * @param data �f�[�^�J�n�ʒu
 * @param pos  �J�n�ʒu�I�t�Z�b�g
 *
 * @return u16
 */
//---------------------------------------------------------------------------- 
static u16 GetU16( u8* data, int pos )
{
	u16 lower = (u16)( data[ pos ] );
	u16 upper = (u16)( data[ pos + 1 ] );
	u16 value = ( upper << 8 ) | lower;

	return value;
}

//---------------------------------------------------------------------------------
/**
 * @brief �f�[�^��ǂݍ���
 *
 * @param p_info  �ǂݍ��񂾃f�[�^�̊i�[��
 * @param heap_id �g�p����q�[�vID
 */
//---------------------------------------------------------------------------------
static void LoadData( CALENDER_WEATHER_INFO* p_info, HEAPID heap_id )
{
  int i, j;
  u8* src;
  int pos = 0; 

  // �f�[�^�ǂݍ���
  src = (u8*)GFL_ARC_LoadDataAlloc( ARCID_CALENDER, NARC_calender_weather_bin, heap_id ); 

  // �o�^�]�[�������擾
  p_info->zoneNum = GetU16( src, pos );

  // �o�b�t�@�m��
  p_info->data = (WEATHER_DATA*)GFL_HEAP_AllocMemory( heap_id, sizeof( WEATHER_DATA ) * p_info->zoneNum );

  // �e�]�[���̓V�C�f�[�^���擾
  for( i=0; i<p_info->zoneNum; i++ )
  {
    // �]�[��ID���擾
    pos = 2 + 4 * i;
    p_info->data[i].zoneID = GetU16( src, pos );
    pos += 2;

    // �f�[�^�擪�ʒu���擾
    pos = GetU16( src, pos );

    // �o�^���t�����擾
    p_info->data[i].dateNum = GetU16( src, pos );
    pos += 2;

    // �o�b�t�@���m��
    p_info->data[i].month   = (u8*)GFL_HEAP_AllocMemory( heap_id, sizeof( u8 ) * p_info->data[i].dateNum );
    p_info->data[i].day     = (u8*)GFL_HEAP_AllocMemory( heap_id, sizeof( u8 ) * p_info->data[i].dateNum );
    p_info->data[i].weather = (u8*)GFL_HEAP_AllocMemory( heap_id, sizeof( u8 ) * p_info->data[i].dateNum );

    // �o�^����Ă���S���t��, ���̓V�C���擾
    for( j=0; j<p_info->data[i].dateNum; j++ )
    {
      p_info->data[i].month[j]   = src[ pos++ ];
      p_info->data[i].day[j]     = src[ pos++ ];
      p_info->data[i].weather[j] = src[ pos++ ];
    }
  }

  // ��n��
  GFL_HEAP_FreeMemory( src ); 


  // DEBUG:
  OBATA_Printf( "zoneNum = %d\n", p_info->zoneNum );
  for( i=0; i<p_info->zoneNum; i++ )
  {
    OBATA_Printf( "---------------------------\n" );
    OBATA_Printf( "zoneID = %d, dateNum = %d\n", p_info->data[i].zoneID, p_info->data[i].dateNum );
    OBATA_Printf( "---------------------------\n" );
    for( j=0; j<p_info->data[i].dateNum; j++ )
    {
      OBATA_Printf( "%2d/%2d = %d\n", p_info->data[i].month[j], p_info->data[i].day[j], p_info->data[i].weather[j] );
    }
  }
}
