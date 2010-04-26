//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_light_status.c
 *  @brief  �o�g���Ɉ����p���t�B�[���h���C�g�J���[
 *  @author tomoya takahashi
 *	@date		2010.02.02
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "arc_def.h"
#include "arc/field_light.naix"
#include "arc/field_weather_light.naix"

#include "gamesystem/pm_season.h"

#include "system/rtc_tool.h"

#include "field/field_light_status.h"
#include "field/weather_no.h"
#include "field/zonedata.h"
#include "field/areadata.h"

#include "field_light.h"
#include "field_zonefog.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
static LIGHT_DATA* LIGHT_STATUS_LoadData( u32 arcID, u32 dataIdx, u32* p_num, HEAPID heapID );
static void LIGHT_STATUS_GetTimeColor( LIGHT_DATA* p_wk, u32 num, int time, int season, FIELD_LIGHT_STATUS* p_color );


//----------------------------------------------------------------------------
/**
 *	@brief  ���C�g���̎擾
 *
 *	@param	zone_id       �]�[��ID
 *	@param	time          ����
 *	@param	weather_id    �V�CID
 *	@param	season        �G��
 *	@param	p_status      ��ԃp�����[�^
 */
//-----------------------------------------------------------------------------
void FIELD_LIGHT_STATUS_Get( u32 zone_id, int hour, int minute, int weather_id, int season, FIELD_LIGHT_STATUS* p_status, HEAPID heapID )
{
  u32 zone_light_idx;
  LIGHT_DATA* p_data = NULL;
  u32 data_num;
  int time;
  
  GF_ASSERT( weather_id < WEATHER_NO_NUM );
  GF_ASSERT( season < PMSEASON_TOTAL );

  // �b�ɕϊ� 
  time = (hour * 60 * 60) + (minute * 60);


  // �V�C
  if( weather_id != WEATHER_NO_SUNNY )
  {
    // �V�C���C�g���擾
    p_data = LIGHT_STATUS_LoadData( ARCID_FIELD_WEATHER_LIGHT, FIELD_LIGHT_STATUS_GetWeatherLightDatIdx(weather_id), &data_num, GFL_HEAP_LOWID(heapID) );
  }

  // �t�H�OZONE
  if( p_data == NULL )
  {
    zone_light_idx = ZONEDATA_GetLight( zone_id );
    if( zone_light_idx != FIELD_ZONEFOGLIGHT_DATA_NONE )
    {
      // �]�[���p���C�g���擾
      p_data = LIGHT_STATUS_LoadData( ARCID_ZONELIGHT_TABLE, zone_light_idx, &data_num, GFL_HEAP_LOWID(heapID) );
    }
  }

  //�@�ʏ�e�[�u��
  if( p_data == NULL )
  {
    AREADATA* p_area = AREADATA_Create( GFL_HEAP_LOWID(heapID), ZONEDATA_GetAreaID(zone_id), season );
      
    p_data = LIGHT_STATUS_LoadData( ARCID_FIELD_LIGHT, AREADATA_GetLightType(p_area), &data_num, GFL_HEAP_LOWID(heapID) );

    AREADATA_Delete( p_area );
  }

  // ���Ԃ̐F���擾
  LIGHT_STATUS_GetTimeColor( p_data, data_num, time, season, p_status );

  GFL_HEAP_FreeMemory( p_data );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �V�CID�̃��C�g�f�[�^�C���f�b�N�X���擾
 *
 *	@param	weather_id  �V�C
 *  
 *	@retval �V�CID�̃��C�g�f�[�^�C���f�b�N�X
 *	@retval FIELD_LIGHT_STATUS_WEATHER_LIGHT_DAT_NONE
 */
//-----------------------------------------------------------------------------
u16 FIELD_LIGHT_STATUS_GetWeatherLightDatIdx( int weather_id )
{
  static const u32 sc_WEATHER_LIGHT[ WEATHER_NO_NUM ] = 
  {
    FIELD_LIGHT_STATUS_WEATHER_LIGHT_DAT_NONE,
    NARC_field_weather_light_light_show_dat,
    NARC_field_weather_light_light_rain_dat,
    NARC_field_weather_light_light_storm_dat,
    NARC_field_weather_light_light_show_dat,
    NARC_field_weather_light_light_show_dat,
    NARC_field_weather_light_light_raikami_dat,
    NARC_field_weather_light_light_kazakami_dat,
    NARC_field_weather_light_light_show_dat,
    NARC_field_weather_light_light_mist_dat,
    NARC_field_weather_light_light_palace_white_mist_dat,
    NARC_field_weather_light_light_palace_black_mist_dat,
    NARC_field_weather_light_light_storm_dat,
  };
  GF_ASSERT( weather_id < WEATHER_NO_NUM );
  return sc_WEATHER_LIGHT[ weather_id ];
}



//----------------------------------------------------------------------------
/**
 *	@brief  ���C�g�f�[�^�ǂݍ���
 *
 *	@param	arcID       �A�[�J�C�u�f�[�^
 *	@param	dataIdx     �f�[�^�C���f�b�N�X
 *	@param  p_num       �f�[�^���i�[��
 *	@param	heapID      �q�[�vID
 *
 *	@retval ���C�g�f�[�^
 */
//-----------------------------------------------------------------------------
static LIGHT_DATA* LIGHT_STATUS_LoadData( u32 arcID, u32 dataIdx, u32* p_num, HEAPID heapID )
{
  u32 size;
  int i, j;
  u32 data_num;
  LIGHT_DATA* p_buff;

  
  size = GFL_ARC_GetDataSize( arcID, dataIdx );
  data_num = size / sizeof(LIGHT_DATA);
  *p_num = data_num;

  p_buff = GFL_ARC_LoadDataAlloc( arcID, dataIdx, heapID );


  // �����x�N�g���̒l���ꉞ�P�ʃx�N�g���ɂ���B
  for( i=0; i<data_num; i++ ){

    for( j=0; j<4; j++ ){
      VEC_Fx16Normalize( &p_buff[i].light_vec[j], &p_buff[i].light_vec[j] );
    }
  }

  return p_buff;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���Ԃ̐F���擾
 *e
 *	@param	p_wk    �J���[�f�[�^
 *	@param	num     �f�[�^��
 *	@param	time    ���̎���
 *	@param  season  �G��
 *	@param	p_color �F�i�[��
 */
//-----------------------------------------------------------------------------
static void LIGHT_STATUS_GetTimeColor( LIGHT_DATA* p_wk, u32 num, int time, int season, FIELD_LIGHT_STATUS* p_color )
{
  int i;
  int start_time;
  int end_time;
  
  GF_ASSERT( num > 0 );

  
  start_time = 0;

  for( i=0; i<num; i++ )
  {
    end_time = PM_RTC_GetTimeZoneChangeHour( season, p_wk[i].timezone ) * 3600;
    end_time += p_wk[i].change_minutes * 60;
    
    if( (start_time <= time) && (end_time > time) )
    {
      break;
    }

    start_time = end_time;
  }

  i %= num; // �Ō�܂ōs������O�ɉ�荞�ށB


  // ���̐F��ݒ�
  p_color->light  = p_wk[i].light_color[0];
  p_color->light1 = p_wk[i].light_color[1];
  p_color->light2 = p_wk[i].light_color[2];
  p_color->light3 = p_wk[i].light_color[3];
  p_color->diffuse = p_wk[i].diffuse;
  p_color->ambient = p_wk[i].ambient;
  p_color->specular = p_wk[i].specular;
  p_color->emission = p_wk[i].emission;
}



