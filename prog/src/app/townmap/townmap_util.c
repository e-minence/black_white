//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		townmap_util.h
 *	@brief  �^�E���}�b�v�������ʃ��[�`��
 *	@author	Toru=Nagihashi
 *	@date		2010.02.06
 *	@note   �^�E���}�b�v�ōs���Ă����l���̈ʒu�T�[�`�Ȃǂ�
 *	        ���̏ꏊ�ł��g���̂ŊO����
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//lib
#include <gflib.h>
#include "app/townmap_util.h"
#include "field/zonedata.h"
#include "src/field/evt_lock.h"
#include "app/townmap_data.h"
#include "savedata/wifihistory.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �^�E���}�b�v�Ŏg�p���邽�߁A���̃]�[��ID���t�B�[���h���
 *	        �ǂ��̃]�[��ID�Ȃ̂��A�ǂ����炫���]�[��ID�Ȃ̂��Ԃ��B
 *	        �i�Ⴆ�΁AZONEID_C01GYM0101��C01�̃W���Ȃ̂�
 *	        ZONEID_C01���Ԃ��Ă��܂��j
 *	        -> �^�E���}�b�v�ł��]�[���O���[�v���g���悤�ɂȂ����̂�
 *	        �]�[���O���[�v���Ԃ��Ă��܂��B100407
 *
 *	@param	const GAMEDATA *cp_gamedata �Q�[���f�[�^  (�G�X�P�[�v���P�[�V�����擾�p)
 *	@param  now_zoneID
 *
 *	@return ���ɂȂ�ZONEID
 */
//-----------------------------------------------------------------------------
u16 TOWNMAP_UTIL_GetRootZoneID( const GAMEDATA* cp_gamedata, u16 now_zoneID )
{
  PLAYER_WORK *p_player;

  return ZONEDATA_GetGroupID( now_zoneID );

#if 0 //old
  if( ZONEDATA_IsFieldMatrixID( now_zoneID ) )
  { 
    //�t�B�[���h�ɂ�����
    return now_zoneID;
  }
  else
  { 
    //�t�B�[���h�ȊO�ɂ�����
    return GAMEDATA_GetEscapeLocation( cp_gamedata )->zone_id;
  }
#endif
}
//----------------------------------------------------------------------------
/**
 *	@brief  �^�E���}�b�v�f�[�^�̃t���O���`�F�b�N
 *
 *	@param	const GAMEDATA* cp_gamedata
 *	@param	flag   TOWNMAP_DATA_PARAM_ARRIVE_FLAG��TOWNMAP_DATA_PARAM_HIDE_FLAG�Ŏ擾�����l
 *
 *	@return TRUE�Ńt���O�������Ă���  FALSE�Ńt���O�������Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL TOWNMAP_UTIL_CheckFlag( GAMEDATA* p_gamedata, u16 flag )
{ 
  EVENTWORK	* p_ev		    = GAMEDATA_GetEventWork( p_gamedata );
  MISC      * p_misc      = GAMEDATA_GetMiscWork( p_gamedata );
  MYSTATUS  * p_mystatus  = GAMEDATA_GetMyStatus( p_gamedata );
  WIFI_HISTORY *p_wifi    = SaveData_GetWifiHistory( GAMEDATA_GetSaveControlWork(p_gamedata) );

#ifdef PM_DEBUG
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L )
  { 
    return TRUE;
  }
#endif //PM_DEBUG

  switch( flag )
  { 
  case TOWNMAP_USER_FLAG_LIBERTY_GARDEN_TICKET:
    //�r�N�e�B�C�x���g�t���O�@���o�e�B�K�[�f�����ւ̃`�P�b�g�������Ă��邩�H
    return EVTLOCK_CheckEvtLock( p_misc, EVT_LOCK_NO_VICTYTICKET, p_mystatus );

  case TOWNMAP_USER_FLAG_OVERSEAS_TRADE:
    //���A�ւ͐��E����������n�}�ア����
    return WIFIHISTORY_GetMyCountryCountEx( p_wifi, p_mystatus, FALSE );

  default:
    //�����Ȃ��΃C�x���g�t���O
    return EVENTWORK_CheckEventFlag( p_ev, flag );
  }
}
