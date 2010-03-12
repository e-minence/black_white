/*
 *  @file   demo3d_param.c(�ǂ�����Ă΂�邩�s��Ȃ̂ŏ풓)
 *  @brief  3D�f���Ăяo���p�����[�^�Z�b�g
 *
 *  @author Miyuki Iwasawa
 *  @date   10.02.25
 */
//�K���K�v�ȃC���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"
#include "gamesystem/game_data.h"
#include "field/ev_time.h"
#include "gamesystem/pm_season.h"

#include "demo/demo3d.h"

/*
 *  @brief  demo3d�Ăяo���p�����[�^�Z�b�g
 *
 *  @param  pp        DEMO3D_PARAM�^�\���̂ւ̃|�C���^
 *  @param  gdata   GAMEDATA�̎Q�ƃ|�C���^
 *  @param  demo_id   �Ăяo���f��ID DEMO3D_ID_NULL�` include/demo/demo3d_demoid.h 
 *  @param  scene_id  �V�[��ID
 *  @param  s_frame   �f���̃X�^�[�g�t���[���w��
 *  @param  t_hour    �f���̃��C�g�ݒ�Ɏg�����Ԏw��(��)
 *  @param  t_min     �f���̃��C�g�ݒ�Ɏg�����Ԏw��(��)
 *  @param  season    �f���̃��C�g�ݒ�Ɏg���G�� PMSEASON_SPRING�` include/gamesystem/pm_season.h
 */
void DEMO3D_PARAM_Set( DEMO3D_PARAM* pp, const GAMEDATA* gdata, u8 demo_id, u8 scene_id, u32 s_frame, u8 t_hour, u8 t_min, u8 season )
{
  MI_CpuClear8( pp, sizeof(DEMO3D_PARAM));
  

  pp->demo_id = demo_id;
  pp->scene_id = scene_id;
  pp->start_frame = s_frame;
  pp->hour = t_hour;
  pp->min = t_min;
  pp->season = season;

  if( demo_id != DEMO3D_ID_INTRO_TOWN ){
    MYSTATUS* my = GAMEDATA_GetMyStatus( (GAMEDATA*)gdata );
    pp->player_sex = MyStatus_GetMySex(my);
  }
}

/*
 *  @brief  demo3d�Ăяo���p�����[�^�Z�b�g(RTC�ˑ�)
 *
 *  @param  pp      DEMO3D_PARAM�^�\���̂ւ̃|�C���^
 *  @param  gdata   GAMEDATA�̎Q�ƃ|�C���^
 *  @param  demo_id �Ăяo���f��ID DEMO3D_ID_NULL�` include/demo/demo3d_demoid.h 
 *
 *  �f���̃��C�g�ݒ�Ɏg�����ԁE�G�߂����A���^�C���N���b�N����Z�o���܂�
 */
void DEMO3D_PARAM_SetFromRTC( DEMO3D_PARAM* pp,const GAMEDATA* gdata, u8 demo_id, u8 scene_id )
{
  RTCTime time;
  GFL_RTC_GetTime(&time);

  DEMO3D_PARAM_Set( pp, gdata, demo_id, scene_id, 0, time.hour, time.minute, PMSEASON_GetRealTimeSeasonID() );
}

/*
 *  @brief  demo3d�Ăяo���p�����[�^�Z�b�g(EV-TIME�ˑ�)
 *
 *  @param  pp      DEMO3D_PARAM�^�\���̂ւ̃|�C���^
 *  @param  gdata   GAMEDATA�̎Q�ƃ|�C���^
 *  @param  demo_id �Ăяo���f��ID DEMO3D_ID_NULL�` include/demo/demo3d_demoid.h 
 *
 *  �f���̃��C�g�ݒ�Ɏg�����ԁE�G�߂�EV-TIME����Z�o���܂�
 */
void DEMO3D_PARAM_SetFromEvTime( DEMO3D_PARAM* pp,const GAMEDATA* gdata, u8 demo_id, u8 scene_id )
{
  DEMO3D_PARAM_Set( pp, gdata, demo_id, scene_id, 0,
      EVTIME_GetHour(gdata), EVTIME_GetMinute( gdata ), GAMEDATA_GetSeasonID(gdata) );
}

