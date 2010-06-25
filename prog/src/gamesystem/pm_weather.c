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
#include "gamesystem/pm_season.h"

#include "field/move_pokemon.h"
#include "field/calender.h"

#include "field/palace_weather.cdat"
#include "field/zonedata.h"



#include "field/eventwork.h"
#include "../../../resource/fldmapdata/flagwork/flag_define.h"


#include "field/field_comm/intrude_work.h"

#include "field/areadata.h"
#include "system/main.h"

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
static BOOL CheckGameDataHaveRTCSeasonID( const GAMEDATA* p_gamedata );
static u16 PM_WEATHER_GetPalaceWeather( const GAMESYS_WORK* cp_gamesystem, const GAMEDATA* cp_data, int zone_id );
static u16 PM_WEATHER_GetEventWeather( GAMESYS_WORK* p_gamesystem, GAMEDATA* p_data, int zone_id );
static u16 PM_WEATHER_GetBirthDayWeather( GAMESYS_WORK* p_gamesystem, GAMEDATA* p_data, int zone_id );
static inline BOOL PM_WEATHER_IsMovePokeWeather( u32 weather_no );

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
#if 0
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

  if( CheckGameDataHaveRTCSeasonID( p_data ) ) {
    // �J�����_�[���Q�Ƃ��ēV�C���擾����.
    // (�J�����_�[�ɓo�^����Ă��Ȃ��]�[���̓V�C��, �]�[���e�[�u���ɏ]��.)
    return CALENDER_GetWeather_today( calender, zone_id );
  }
  else {
    // �Q�[���f�[�^�̋G�߂�RTC�̋G�߂ɐ��������Ȃ��ꍇ, �]�[���e�[�u���ɏ]��
    return ZONEDATA_GetWeatherID( zone_id );
  } 
#endif

  // �̌��łł�, �G�߂��ƂɓV�C���Œ肷��
  {
    GAMEDATA* p_gamedata = GAMESYSTEM_GetGameData( p_gamesystem );
    u8 season_id  = GAMEDATA_GetSeasonID( p_gamedata );
    u8 weather_no;
    BOOL indoor;

    // ���� or ���O?
    {
      u16       area_id   = ZONEDATA_GetAreaID( zone_id );
      AREADATA* area_data = AREADATA_Create( HEAPID_PROC, area_id, season_id ); 
      indoor = ( AREADATA_GetInnerOuterSwitch(area_data) == 0 );
      AREADATA_Delete( area_data );
    }

    if( indoor ) {
      weather_no = WEATHER_NO_SUNNY; // �����Ȃ琰��
    }
    else { 
      switch( season_id ) {
      default: GF_ASSERT(0);
      case PMSEASON_SPRING: weather_no = WEATHER_NO_SUNNY; break;
      case PMSEASON_SUMMER: weather_no = WEATHER_NO_RAIN; break;
      case PMSEASON_AUTUMN: weather_no = WEATHER_NO_SUNNY; break;
      case PMSEASON_WINTER: weather_no = WEATHER_NO_SNOW; break;
      }
    } 

    return weather_no;
  }
}

//----------------------------------------------------------------------------
/**
 * @brief �Q�[�����G�߂�RTC����Z�o�����G�߂����������ǂ����H
 *
 * @param p_gamesystem
 */
//----------------------------------------------------------------------------
static BOOL CheckGameDataHaveRTCSeasonID( const GAMEDATA* p_gamedata )
{
  u8 gamedata_season = GAMEDATA_GetSeasonID( p_gamedata );
  u8 rtc_season = PMSEASON_GetRealTimeSeasonID();

  if( gamedata_season == rtc_season ) {
    return TRUE;
  }
  else {
    return FALSE;
  }
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
  u16 last_weather = GAMEDATA_GetWeatherNo( p_data );

  // �ړ��|�P�����D��
  if( weather != WEATHER_NO_NONE )
  {
    GAMEDATA_SetWeatherNo( p_data, weather );
  }else{

    // �ړ��|�P�����̓����ŁA���C�J�~�J�U�J�~�V�C�̏ꍇ�ɂ́AZoneTable�̓V�C�ɂ���B
    // �ړ��|�P�����̓���O�̏ꍇ�ɂ́A�C�x���g�Ń��C�J�~�J�U�J�~�ɂ��Ă��邱�Ƃ�����̂�
    // �N���A���邱�Ƃ͂��Ȃ��B
    //
    // �ړ��|�P�����@�����Ƀ��C�J�~�A�J�U�J�~�V�C�ɂ��Ă���ꍇ����������ƁA
    // ���̃N���A����������������B�@���ӁI�I
    if( PM_WEATHER_IsMovePokeWeather( last_weather ) ){
      if( MP_CheckMovePokeValid( p_data ) == TRUE ){

        // �V�C�N���A   �f�t�H���g�̓V�C���g�p����B
        GAMEDATA_SetWeatherNo( p_data, ZONEDATA_GetWeatherID( zone_id ) );
      }
    }
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



//----------------------------------------------------------------------------
/**
 *	@brief  �ړ��|�P�����V�C���`�F�b�N����
 *
 *	@param	weather_no  �V�C�i���o�[
 *
 *	@retval TRUE    �ړ��|�P�����V�C
 *	@retval FALSE   �ʏ�V�C
 */
//-----------------------------------------------------------------------------
static inline BOOL PM_WEATHER_IsMovePokeWeather( u32 weather_no )
{
  if( (weather_no == WEATHER_NO_RAIKAMI) || (weather_no == WEATHER_NO_KAZAKAMI) ){
    return TRUE;
  }
  return FALSE;
}

