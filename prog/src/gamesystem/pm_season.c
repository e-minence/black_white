//============================================================================================
/**
 * @file	pm_season.h
 * @brief	�G�߂̒�`
 * @author	tamada GAME FREAK Inc.
 * @date	09.03.17
 *
 */
//============================================================================================

#include <gflib.h>
#include "gamesystem/pm_season.h"
#include "gamesystem/game_data.h"
#include "gamesystem/pm_season_local.h"
#include "field/field_comm/intrude_work.h"



//----------------------------------------------------------------------------
/**
 *	@brief  �ʐM�i�i���j�ɑΉ������G�߂��擾
 *
 *	@param	gamedata  �Q�[���f�[�^
 *
 *	@return �ʐM�i�i���j�ɑΉ������G�߂��擾
 *
 *	���g�p�ӏ�
 *	  �G���A�f�[�^�|�|�|�|�|�|�|�n�`�ɂ��������
 *	  �t�B�[���h�G�t�F�N�g�|�|�|�n�ʂɂ�����鉉�o
 */
//-----------------------------------------------------------------------------
u8 PMSEASON_GetConsiderCommSeason( GAMESYS_WORK* gamesys )
{
  GAME_COMM_SYS_PTR gamecomm = GAMESYSTEM_GetGameCommSysPtr( gamesys );
  return Intrude_GetSeasonID( gamecomm );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �G�߁A���̋G�߂̎擾
 *
 *	@param	gamedata     �Q�[���f�[�^
 *	@param  prevSeason  ���̋G�߁@�i�[��
 *	@param  nextSeason  ���̋G�߁@�i�[��
 *
 *	@retval TRUE    �ύX����
 *	@retval FALSE   �ύX�Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL PMSEASON_GetNextSeason( const GAMEDATA* gamedata, u16* prevSeason, u16* nextSeason )
{
  // �i�����́A�G�߂̍X�V���s��Ȃ��B
  if( GAMEDATA_GetIntrudeReverseArea( gamedata ) ){
    *prevSeason = GAMEDATA_GetSeasonID( gamedata );
    *nextSeason = *prevSeason;
    return FALSE;
  }
  
  // ���̑��̏ꍇ�́A�G�ߕύX
  *prevSeason = GAMEDATA_GetSeasonID( gamedata );
  *nextSeason = PMSEASON_GetRealTimeSeasonID();


  if( (*prevSeason) == (*nextSeason) ){
    return FALSE;
  }
  return TRUE;
}



//--------------------------------------------------------------------------------------------
/**
 * @brief ���ݎ����̋G�߂��v�Z����
 *
 * @return ���ݎ����̋G��( PMSEASON_SPRING �Ȃ� )
 */
//--------------------------------------------------------------------------------------------
u8 PMSEASON_GetRealTimeSeasonID()
{
  RTCDate date;
  u8 season;

  // ���ݎ������擾
  GFL_RTC_GetDate( &date );

  // �G�߂��v�Z
  season = (date.month - 1) % PMSEASON_TOTAL;
  return season;
} 

//--------------------------------------------------------------------------------------------
/**
 * @brief ���̋G�߂����߂�
 *
 * @param now ���݂̋G��( PMSEASON_SPRING �Ȃ� )
 *
 * @return ���̋G��( PMSEASON_SPRING �Ȃ� )
 */
//--------------------------------------------------------------------------------------------
u8 PMSEASON_GetNextSeasonID( u8 now )
{
  u8 next;
  next = (now + 1) % PMSEASON_TOTAL;
  return next;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �O�̋G�߂����߂�
 *
 * @param now ���݂̋G��( PMSEASON_SPRING �Ȃ� )
 *
 * @return �O�̋G��( PMSEASON_SPRING �Ȃ� )
 */
//--------------------------------------------------------------------------------------------
u8 PMSEASON_GetPrevSeasonID( u8 now )
{
  u8 prev;
  prev = (now + PMSEASON_TOTAL - 1) % PMSEASON_TOTAL;
  return prev;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �G�߂�ݒ�
 *
 *	@param	gamedata    �Q�[���f�[�^
 *	@param  season      ���̋G��
 */
//-----------------------------------------------------------------------------
void PMSEASON_SetSeasonID( GAMEDATA* gamedata, u16 season )
{
  GAMEDATA_SetSeasonID( gamedata, season );
}

