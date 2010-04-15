////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �J�����_�[
 * @file   calender.h
 * @author obata
 * @date   2010.01.29
 */
////////////////////////////////////////////////////////////////////////////////
#include "calender.h"
#include "ev_time.h"
#include "field/zonedata.h"
#include "arc/arc_def.h"
#include "arc/calender.naix"


//==============================================================================
// ���萔
//==============================================================================
#define WEATHER_DATA_NONE (0xffff)  // �f�[�^���o�^����Ă��Ȃ�


//==============================================================================
// ���C���f�b�N�X �f�[�^ �t�H�[�}�b�g
//==============================================================================
typedef struct
{
  u16 zoneID;     // �]�[��ID
  u16 dataIndex;  // �擪�f�[�^�̃C���f�b�N�X

} INDEX_DATA;


//==============================================================================
// ���J�����_�[���[�N
//==============================================================================
struct _CALENDER
{
  HEAPID     heapID;
  GAMEDATA*  gameData;
  ARCHANDLE* arcHandle;  // �A�[�J�C�u�n���h��
};


//==============================================================================
// ������J�֐�
//============================================================================== 
// �J�����_�[���[�N
void InitCalenderWork( CALENDER* calender );
void OpenArcHandle   ( CALENDER* calender );
void CloseArcHandle  ( CALENDER* calender ); 
// �V�C�f�[�^
WEATHER_NO GetWeatherNo_today( const CALENDER* calender, u16 zoneID );
WEATHER_NO GetWeatherNo      ( const CALENDER* calender, u16 zoneID, u8 month, u8 day ); 
// �����f�[�^
u16 GetAnnualWeatherDataIndex ( const CALENDER* calender, u16 zoneID );
u16 GetDateIndex              ( u8 month, u8 day );


//==============================================================================
// �����J�֐�
//============================================================================== 

//------------------------------------------------------------------------------
/**
 * @brief �J�����_�[���쐬����
 *
 * @praram gameData
 * @param  heapID
 */
//------------------------------------------------------------------------------
CALENDER* CALENDER_Create( GAMEDATA* gameData, HEAPID heapID )
{
  CALENDER* calender;

  // ����
  calender = GFL_HEAP_AllocMemory( heapID, sizeof(CALENDER) );

  // ������
  InitCalenderWork( calender );
  calender->heapID   = heapID;
  calender->gameData = gameData;
  OpenArcHandle( calender );

  return calender;
}

//------------------------------------------------------------------------------
/**
 * @brief �J�����_�[��j������
 *
 * @param calender
 */
//------------------------------------------------------------------------------
void CALENDER_Delete( CALENDER* calender )
{
  CloseArcHandle( calender );
  GFL_HEAP_FreeMemory( calender );
}

//------------------------------------------------------------------------------
/**
 * @brief �V�C���擾����
 *
 * @param calender
 * @parma zoneID  �]�[��ID
 * @param month   ��[1, 12]
 * @param day     ��[1, 31]
 *
 * @return �w�肵�� �ꏊ-���t �̓V�C
 */
//------------------------------------------------------------------------------
WEATHER_NO CALENDER_GetWeather( const CALENDER* calender, u16 zoneID, u8 month, u8 day )
{
  return GetWeatherNo( calender, zoneID, month, day );
}

//------------------------------------------------------------------------------
/**
 * @brief �����̓V�C���擾����
 *
 * @param calender
 * @parma zoneID  �]�[��ID
 *
 * @return �w�肵���ꏊ�̍����̓V�C
 */
//------------------------------------------------------------------------------
WEATHER_NO CALENDER_GetWeather_today( const CALENDER* calender, u16 zoneID )
{
  return GetWeatherNo_today( calender, zoneID );
}

//==============================================================================
// ���J�����_�[���[�N
//==============================================================================

//------------------------------------------------------------------------------
/**
 * @brief �J�����_�[���[�N������������
 *
 * @param calender
 */
//------------------------------------------------------------------------------
void InitCalenderWork( CALENDER* calender )
{
  calender->heapID    = 0;
  calender->gameData  = NULL;
  calender->arcHandle = NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief �J�����_�[�f�[�^�̃t�@�C���n���h�����J��
 *
 * @param calender
 */
//------------------------------------------------------------------------------
void OpenArcHandle( CALENDER* calender )
{
  GF_ASSERT( calender );
  GF_ASSERT( calender->heapID );
  GF_ASSERT( calender->arcHandle == NULL );

  calender->arcHandle = GFL_ARC_OpenDataHandle( ARCID_CALENDER, calender->heapID );

  GF_ASSERT( calender->arcHandle );
}

//------------------------------------------------------------------------------
/**
 * @brief �J�����_�[�f�[�^�̃t�@�C���n���h�������
 *
 * @param calender
 */
//------------------------------------------------------------------------------
void CloseArcHandle( CALENDER* calender )
{
  GF_ASSERT( calender );
  GF_ASSERT( calender->arcHandle );

  GFL_ARC_CloseDataHandle( calender->arcHandle );
  calender->arcHandle = NULL;
}


//==============================================================================
// ���V�C�f�[�^
//==============================================================================

//------------------------------------------------------------------------------
/**
 * @brief �����̓V�C���擾����
 *
 * @param calender
 * @param zoneID   �V�C���擾����ꏊ
 *
 * @return �w�肵���ꏊ�̍����̓V�C ( WEATHER_NO_xxxx )
 */
//------------------------------------------------------------------------------
WEATHER_NO GetWeatherNo_today( const CALENDER* calender, u16 zoneID )
{
  int month, day;

  // ���t���擾
  month = EVTIME_GetMonth( calender->gameData );
  day   = EVTIME_GetDay( calender->gameData );

  // �����̓V�C��Ԃ�
  return GetWeatherNo( calender, zoneID, month, day );
}

//------------------------------------------------------------------------------
/**
 * @brief �V�C���擾����
 *
 * @param calender
 * @param zoneID   �V�C���擾����ꏊ
 * @param month    �V�C���擾���錎[1, 12]
 * @param day      �V�C���擾�����[1, 31]
 *
 * @return �w�肵���ꏊ/���t�̓V�C ( WEATHER_NO_xxxx )
 */
//------------------------------------------------------------------------------
WEATHER_NO GetWeatherNo( const CALENDER* calender, u16 zoneID, u8 month, u8 day )
{
  u8 weather;
  u16 annualIndex, dateIndex;

  // �N�ԃf�[�^�̃C���f�b�N�X�擾
  annualIndex = GetAnnualWeatherDataIndex( calender, zoneID );

  // �o�^����Ă��Ȃ�
  if( annualIndex == WEATHER_DATA_NONE ) {
    return ZONEDATA_GetWeatherID( zoneID ); // �]�[���e�[�u�����Q��
  }

  // �w����̓V�C�f�[�^�̃C���f�b�N�X
  dateIndex = annualIndex + GetDateIndex( month, day );

  // �V�C���擾
  GFL_ARC_LoadDataOfsByHandle( calender->arcHandle, NARC_calender_weather_data_bin, 
                               dateIndex, sizeof(weather), &weather );
  return weather;
}


//==============================================================================
// �������f�[�^
//==============================================================================

//------------------------------------------------------------------------------
/**
 * @brief �N�ԃf�[�^�̃C���f�b�N�X���擾����
 *
 * @param calender
 * @param zoneID   �]�[��ID
 *
 * @return �w�肵���]�[���̔N�ԓV�C�f�[�^�̃C���f�b�N�X
 *         ( �V�C�f�[�^����, �w��]�[��1/1�̓V�C�f�[�^�̏ꏊ )
 *
 *         �w�肵���]�[�����e�[�u���ɓo�^����Ă��Ȃ��ꍇ WEATHER_DATA_NONE
 */
//------------------------------------------------------------------------------
u16 GetAnnualWeatherDataIndex( const CALENDER* calender, u16 zoneID )
{
  int i;
  u16 index;
  u16 tableSize;
  INDEX_DATA* table;

  // �e�[�u���T�C�Y�擾
  GFL_ARC_LoadDataOfsByHandle( calender->arcHandle, NARC_calender_weather_index_bin, 
                               0, sizeof(tableSize), &tableSize );
  
  // �e�[�u���쐬
  table = GFL_HEAP_AllocMemory( calender->heapID, sizeof(INDEX_DATA) * tableSize );
  GFL_ARC_LoadDataOfsByHandle( calender->arcHandle, NARC_calender_weather_index_bin,
                               sizeof(tableSize), sizeof(INDEX_DATA) * tableSize, table );

  // ����
  index = WEATHER_DATA_NONE;
  for( i=0; i < tableSize; i++ )
  {
    // ����
    if( table[i].zoneID == zoneID )
    {
      index = table[i].dataIndex; 
      break;
    }
  }

  // �e�[�u���j��
  GFL_HEAP_FreeMemory( table );

  return index;
}

//------------------------------------------------------------------------------
/**
 * @brief ���ԃf�[�^�̃C���f�b�N�X���擾����
 *
 * @param month    ��[1, 12]
 * @param day      ��[1, 31]
 *
 * @return �w�肵�����t�̔N�ԃC���f�b�N�X ( �����ڂ� )
 */
//------------------------------------------------------------------------------
u16 GetDateIndex( u8 month, u8 day )
{
  int i;
  u16 index;

  // �����Ƃ̓���
  const u8 dayOfMonth[13] = {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  // �����`�F�b�N
  GF_ASSERT( (month != 0) && (month <= 12) );
  GF_ASSERT( (day   != 0) && (day   <= dayOfMonth[month]) );

  // �����ڂ�
  index = 0;
  for( i=0; i<month; i++ ){ index += dayOfMonth[i]; }  // �O���܂ł̓���
  index += (day - 1);  // �w���

  return index;
}
