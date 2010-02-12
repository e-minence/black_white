//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_season_time.c
 *	@brief  �G�߂ɂ��@���ԑѕω�
 *	@author	tomoya takahashi
 *	@date		2010.01.28
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "field_season_time.h"

#include "system/rtc_tool.h"

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
//-------------------------------------
///	�G�߂ɂ�鎞�ԑѕω��Ǘ����[�N
//=====================================
struct _FLD_SEASON_TIME 
{
  u32 dummy;  // @TODO ��͋G�߂��Ƃ̎��ԑѐ؂�ւ�����
};

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------


//----------------------------------------------------------------------------
/**
 *	@brief  �G�߂̎��ԑъǗ����[�N�@����
 *
 *	@param	season_id   �G��ID
 *	@param	heapID      �q�[�vID
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
FLD_SEASON_TIME* FLD_SEASON_TIME_Create( u8 season_id, HEAPID heapID )
{
  FLD_SEASON_TIME* p_sys = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLD_SEASON_TIME) );
  return p_sys;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �G�߂̎��ԑъǗ����[�N�@�j��
 *
 *	@param	p_sys   �V�X�e��
 */
//-----------------------------------------------------------------------------
void FLD_SEASON_TIME_Delete( FLD_SEASON_TIME* p_sys )
{
  GFL_HEAP_FreeMemory( p_sys );
}



//----------------------------------------------------------------------------
/**
 *	@brief  �G�߂��Ƃ̎��ԑю擾
 *
 *	@param	cp_sys  �V�X�e��
 *
 *	@return ���ԑ�
 */
//-----------------------------------------------------------------------------
TIMEZONE FLD_SEASON_TIME_GetTimeZone( const FLD_SEASON_TIME* cp_sys )
{
  GF_ASSERT( cp_sys );
  return GFL_RTC_GetTimeZone();
}

// ���̎��Ԃ���^�C���]�[���؂�ւ����Ԃ܂ł̍�
//----------------------------------------------------------------------------
/**
 *	@brief  ���̎��Ԃ���^�C���]�[���؂�ւ��܂ł̍�
 *
 *	@param	cp_sys    �V�X�e��
 *	@param	timezone  �^�C���]�[��
 *
 *	@return�@�b��(-12���ԁ`11����)
 */
//-----------------------------------------------------------------------------
int FLD_SEASON_TIME_GetTimeZoneChangeTime( const FLD_SEASON_TIME* cp_sys, TIMEZONE timezone )
{
  static const u8 sc_TIMEZONE_CHANGE_TIME[ 5 ] = 
  {
    4,
    10,
    17,
    20,
    0
  };
  int time_second;
  int sub_second;
  int change_time;
  RTCTime time;

  
  GF_ASSERT( cp_sys );
  GF_ASSERT( timezone < TIMEZONE_MAX );
  
  GFL_RTC_GetTime( &time );

  change_time = sc_TIMEZONE_CHANGE_TIME[timezone];
  time_second = (time.hour * 60 * 60) + (time.minute * 60) + time.second;
  sub_second = (change_time*60*60) - time_second;
  if( sub_second > (12*60*60) )
  {
    sub_second = (24*60*60) - sub_second;
  }
  return sub_second;
}

