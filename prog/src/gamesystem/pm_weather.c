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

#include "system/ds_system.h"

#include "gamesystem/pm_weather.h"

#include "field/move_pokemon.h"
#include "field/calender.h"

#include "field/palace_weather.cdat"
#include "field/zonedata.h"



#include "field/eventwork.h"
#include "../../../resource/fldmapdata/flagwork/flag_define.h"


#include "field/field_comm/intrude_work.h"

#ifdef PM_DEBUG

#include "debug/debug_flg.h"

#endif // PM_DEBUG

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
static u16 PM_WEATHER_GetPalaceWeather( const GAMESYS_WORK* cp_gamesystem, const GAMEDATA* cp_data, int zone_id );
static u16 PM_WEATHER_GetEventWeather( GAMESYS_WORK* p_gamesystem, GAMEDATA* p_data, int zone_id );
static u16 PM_WEATHER_GetBirthDayWeather( GAMESYS_WORK* p_gamesystem, GAMEDATA* p_data, int zone_id );

//----------------------------------------------------------------------------
/**
 *	@brief  ���݂̎w��ZONE�̓V�C���擾����
 *
 *	@param	p_gamesystem �f�[�^
 *	@param	zone_id �]�[��ID
 *
 *	@return �]�[���̓V�C
 */
//-----------------------------------------------------------------------------
u8 PM_WEATHER_GetZoneWeatherNo( GAMESYS_WORK* p_gamesystem, int zone_id )
{
  GAMEDATA* p_data  = GAMESYSTEM_GetGameData( p_gamesystem );
  CALENDER* calender = GAMEDATA_GetCalender( p_data );
  u16       weather  = MP_CheckMovePokeWeather( p_data, zone_id );

  // �ړ��|�P�����`�F�b�N
  if( weather != WEATHER_NO_NONE )
  {
    return weather;
  }

  // �p���X�`�F�b�N
  weather = PM_WEATHER_GetPalaceWeather( p_gamesystem, p_data, zone_id );
  if( weather != WEATHER_NO_NONE )
  {
    return weather;
  }

  // �C�x���g�ɂ��A�V�C��������
  weather = PM_WEATHER_GetEventWeather( p_gamesystem, p_data, zone_id );
  if( weather != WEATHER_NO_NONE )
  {
    return weather;
  }

  // �a�����ɂ��V�C��������
  weather = PM_WEATHER_GetBirthDayWeather( p_gamesystem, p_data, zone_id );
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
 *	@param	p_gamesystem �Q�[���f�[�^
 */
//-----------------------------------------------------------------------------
void PM_WEATHER_UpdateZoneChangeWeatherNo( GAMESYS_WORK* p_gamesystem, int zone_id )
{
  GAMEDATA* p_data  = GAMESYSTEM_GetGameData( p_gamesystem );
  u16       weather  = PM_WEATHER_GetZoneWeatherNo( p_gamesystem, zone_id );
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
static u16 PM_WEATHER_GetPalaceWeather( const GAMESYS_WORK* cp_gamesystem, const GAMEDATA* cp_data, int zone_id )
{
  EVENTWORK * p_evwork = GAMEDATA_GetEventWork( (GAMEDATA*)cp_data );
  GAME_COMM_SYS_PTR p_gamecomm = GAMESYSTEM_GetGameCommSysPtr( (GAMESYS_WORK*)cp_gamesystem );
  int i;
  
  // �p���X�Ȃ�A�p���X�p
  if( ZONEDATA_IsPalaceField( zone_id ) )
  {
    if( p_gamecomm ){
      for( i=0; i<NELEMS(sc_PALACE_WEATHER_DATA); i++ )
      {
        if( sc_PALACE_WEATHER_DATA[i].zone_id == zone_id )
        {
          if( EVENTWORK_CheckEventFlag( p_evwork, sc_PALACE_WEATHER_DATA[i].sys_flag ) == FALSE )
          {
            if( Intrude_GetRomVersion( p_gamecomm ) == VERSION_WHITE ){
              return WEATHER_NO_PALACE_WHITE_MIST_HIGH;
            }else{
              return WEATHER_NO_PALACE_BLACK_MIST_HIGH;
            }
          }
          else
          {
            if( Intrude_GetRomVersion( p_gamecomm ) == VERSION_WHITE ){
              return WEATHER_NO_PALACE_WHITE_MIST;
            }else{
              return WEATHER_NO_PALACE_BLACK_MIST;
            }
          }
        }
      }
    }
  }

#ifdef PM_DEBUG
  //�p���X�N���e�N�X�`��ON
  if( DEBUG_FLG_GetFlg(DEBUG_FLG_PalaceTexture) == TRUE )
  {
    //�p���X����ON
    if( DEBUG_FLG_GetFlg(DEBUG_FLG_PalaceArrive) == TRUE )
    {
      if( GET_VERSION() == VERSION_WHITE ){
        return WEATHER_NO_PALACE_WHITE_MIST;
      }else{
        return WEATHER_NO_PALACE_BLACK_MIST;
      }
    }
    else
    {
      if( GET_VERSION() == VERSION_WHITE ){
        return WEATHER_NO_PALACE_WHITE_MIST_HIGH;
      }else{
        return WEATHER_NO_PALACE_BLACK_MIST_HIGH;
      }
    }
  }
#endif // PM_DEBUG

  return WEATHER_NO_NONE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �C�x���g�̓V�C���擾
 *
 *	@param	p_gamesystem    �Q�[���V�X�e��
 *	@param	p_data          �f�[�^
 *	@param	zone_id         �]�[��ID
 *
 *	@return �C�x���g�N���F�V�C
 */
//-----------------------------------------------------------------------------
static u16 PM_WEATHER_GetEventWeather( GAMESYS_WORK* p_gamesystem, GAMEDATA* p_data, int zone_id )
{
	EVENTWORK * p_ev;
	p_ev = GAMEDATA_GetEventWork( p_data );

  // R07  �ړ��|�P����
  if( zone_id == ZONE_ID_R07 )
  {
    if( EVENTWORK_CheckEventFlag( p_ev, SYS_FLAG_WEATHER_R07 ) )
    {
      // BLACK = �J�U�J�~
      // WHITE = ���C�J�~
      if( GetVersion() == VERSION_BLACK ){
        return WEATHER_NO_KAZAKAMI;
      }
      return WEATHER_NO_RAIKAMI;
    }
  }

  return WEATHER_NO_NONE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �a�����̓V�C���擾
 *
 *	@param	p_gamesystem  �Q�[���V�X�e��
 *	@param	p_data        �Q�[���f�[�^
 *	@param	zone_id       �]�[��ID
 *
 *	@return �a�����F�V�C
 */
//-----------------------------------------------------------------------------
static u16 PM_WEATHER_GetBirthDayWeather( GAMESYS_WORK* p_gamesystem, GAMEDATA* p_data, int zone_id )
{
  RTCDate data;
  u8 month;
  u8 day;
  
  if( ZONEDATA_IsBirthDayWeatherChange( zone_id ) ){
    // �����̓��ɂ����擾
    GFL_RTC_GetDate( &data );

    // �a�������擾
    DS_SYSTEM_GetBirthDay( &month, &day );

    // ���ɂ��ƒa��������v������A����ɂ���
    if( (data.month == month) && (data.day == day) )
    {
      return WEATHER_NO_SUNNY;
    }
  }
  return WEATHER_NO_NONE;
}
