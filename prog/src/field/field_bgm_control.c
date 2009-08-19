/////////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @file field_bgm_control.c
 * @brief �t�B�[���h�}�b�v�ɂ�����BGM�t�F�[�h�̑���֐��Q
 * @author obata
 * @date   2009.08.19
 *
 */
/////////////////////////////////////////////////////////////////////////////////////////

#include "sound/pm_sndsys.h"
#include "field_sound.h"
#include "field_bgm_control.h"


//---------------------------------------------------------------------------------------
/**
 * @breif �w��]�[���ōĐ����ׂ�BGM�̃t�F�[�h�C�����J�n����
 *
 * @param p_gdata     �Q�[���f�[�^
 * @param zone_id     BGM���J�n����]�[��
 * @param fade_frames �t�F�[�h�C���ɗv����t���[����
 */
//---------------------------------------------------------------------------------------
void FIELD_BGM_CONTROL_FadeIn( GAMEDATA* p_gdata, u16 zone_id, u16 fade_frames )
{
  PLAYER_WORK*     p_player = GAMEDATA_GetPlayerWork( p_gdata, 0 );
  PLAYER_MOVE_FORM form     = PLAYERWORK_GetMoveForm( p_player );
  u32              bgm_no   = FIELD_SOUND_GetFieldBGMNo( p_gdata, form, zone_id );
  u32              now      = PMSND_GetBGMsoundNo();

  if( bgm_no != now )
  {
    FIELD_SOUND_PlayBGM( bgm_no );
    FIELD_SOUND_FadeInBGM( fade_frames );
  }
}

//---------------------------------------------------------------------------------------
/**
 * @breif �w��]�[����BGM���Đ�����BGM�ƈقȂ�ꍇ, �Đ�����BGM�̃t�F�[�h�A�E�g���J�n����
 *
 * @param p_gdata     �Q�[���f�[�^
 * @param zone_id     BGM���J�n����]�[��
 * @param fade_frames �t�F�[�h�C���ɗv����t���[����
 */
//---------------------------------------------------------------------------------------
void FIELD_BGM_CONTROL_FadeOut( GAMEDATA* p_gdata, u16 zone_id, u16 fade_frames )
{
  PLAYER_WORK*     p_player = GAMEDATA_GetPlayerWork( p_gdata, 0 );
  PLAYER_MOVE_FORM form     = PLAYERWORK_GetMoveForm( p_player );
  u32              bgm_no   = FIELD_SOUND_GetFieldBGMNo( p_gdata, form, zone_id );
  u32              now      = PMSND_GetBGMsoundNo();

  if( bgm_no != now )
  {
    FIELD_SOUND_FadeOutBGM( fade_frames );
  }
} 

//---------------------------------------------------------------------------------------
/**
 * @brief �t�F�[�h�����ǂ����𔻒肷��
 *
 * @return �t�F�[�h���Ȃ�TRUE
 */
//---------------------------------------------------------------------------------------
BOOL FIELD_BGM_CONTROL_IsFade()
{
  return FIELD_SOUND_IsBGMFade();
}
