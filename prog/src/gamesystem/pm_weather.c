//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		pm_weather.c
 *	@brief  �V�C�Ǘ��@�ړ��|�P�����@�]�[���@�J�����_�[
 *	@author	tomoya takahashi
 *	@date		2010.01.29
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "gflib.h"

#include "gamesystem/pm_weather.h"

#include "field/move_pokemon.h"
#include "field/calender.h"

#include "field/palace_weather.cdat"
#include "field/zonedata.h"

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
static u16 PM_WEATHER_GetPalaceWeather( GAMEDATA* p_data, int zone_id );

//----------------------------------------------------------------------------
/**
 *	@brief  ���݂̎w��ZONE�̓V�C���擾����
 *
 *	@param	p_data �f�[�^
 *	@param	zone_id �]�[��ID
 *
 *	@return �]�[���̓V�C
 */
//-----------------------------------------------------------------------------
u8 PM_WEATHER_GetZoneWeatherNo( GAMEDATA* p_data, int zone_id )
{
  CALENDER* calender = GAMEDATA_GetCalender( p_data );
  u16       weather  = MP_CheckMovePokeWeather( p_data, zone_id );

  // �ړ��|�P�����`�F�b�N
  if( weather != WEATHER_NO_NONE )
  {
    return weather;
  }

  // �p���X�`�F�b�N
  weather = PM_WEATHER_GetPalaceWeather( p_data, zone_id );
  if( weather != WEATHER_NO_NONE )
  {
    return weather;
  }


  // �J�����_�[���Q�Ƃ��ēV�C���擾����.
  // (�J�����_�[�ɓo�^����Ă��Ȃ��]�[���̓V�C��, �]�[���e�[�u���ɏ]��.)
	return CALENDER_GetWeather_today( calender, zone_id );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �]�[���`�F���W�ł̓V�C�A�b�v�f�[�g
 *
 *	@param	p_data �Q�[���f�[�^
 */
//-----------------------------------------------------------------------------
void PM_WEATHER_UpdateZoneChangeWeatherNo( GAMEDATA* p_data, int zone_id )
{
  u16       weather  = PM_WEATHER_GetZoneWeatherNo( p_data, zone_id );
  GAMEDATA_SetWeatherNo( p_data, weather );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �Z�[�u���A���̓V�C�A�b�v�f�[�g
 *
 *	@param	p_data 
 */
//-----------------------------------------------------------------------------
void PM_WEATHER_UpdateSaveLoadWeatherNo( GAMEDATA* p_data, int zone_id )
{
  u16 weather  = MP_CheckMovePokeWeather( p_data, zone_id );

  // �ړ��|�P�����D��
  if( weather != WEATHER_NO_NONE )
  {
    GAMEDATA_SetWeatherNo( p_data, weather );
  }
}






//-----------------------------------------------------------------------------
/**
 *      private�֐�
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  �p���X�̓V�C���擾
 *
 *	@param	p_data      �f�[�^
 *	@param	zone_id     �]�[��ID
 *
 *	@return �V�C
 */
//-----------------------------------------------------------------------------
static u16 PM_WEATHER_GetPalaceWeather( GAMEDATA* p_data, int zone_id )
{
  EVENTWORK * p_evwork = GAMEDATA_GetEventWork( p_data );
  int i;
  
  // �p���X�Ȃ�A�p���X�p
  if( ZONEDATA_IsPalaceField( zone_id ) )
  {
     for( i=0; i<NELEMS(sc_PALACE_WEATHER_DATA); i++ )
     {
       if( sc_PALACE_WEATHER_DATA[i].zone_id == zone_id )
       {
         if( EVENTWORK_CheckEventFlag( p_evwork, sc_PALACE_WEATHER_DATA[i].sys_flag ) == FALSE )
         {
           // ������
           // @TODO ROMVersion���݂ĐU�蕪����K�v������B
           return WEATHER_NO_PALACE_WHITE_MIST;
         }
       }
     }
  }

  return WEATHER_NO_NONE;
}



