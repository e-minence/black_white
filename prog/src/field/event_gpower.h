//============================================================================================
/**
 * @file    event_gpower.h
 * @brief	  �C�x���g�FG�p���[�֘A�C�x���g
 * @author  iwasawa	
 * @date	  2009.02.17
 */
//============================================================================================

#pragma once

#define SE_POWER_USE  (SEQ_SE_W295_01)

typedef struct _GPOWER_EFFECT_PARAM{
  GPOWER_ID g_power;
  BOOL      mine_f;
}GPOWER_EFFECT_PARAM;

FS_EXTERN_OVERLAY( event_gpower );

/*
 *  @brief  G�p���[�����C�x���g
 *
 *  @param  g_power ��������G�p���[ID
 *  @param  mine    ������G�p���[���ǂ����H
 */
extern GMEVENT* EVENT_GPowerEffectStart(GAMESYS_WORK * gsys, void* work );

/*
 *  @brief  G�p���[���ʏI���C�x���g
 */
extern GMEVENT* EVENT_GPowerEffectEnd( GAMESYS_WORK * gsys, void* work );

/*
 *  @brief  G�p���[���ʔ����G�t�F�N�g
 */
extern GMEVENT* EVENT_GPowerUseEffect( GAMESYS_WORK * gsys );

/*
 *  @brief  ��������G�p���[�m�F�C�x���g
 */
extern GMEVENT* EVENT_GPowerEnableListCheck( GAMESYS_WORK * gsys, void* work );


