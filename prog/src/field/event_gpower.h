//============================================================================================
/**
 * @file    event_gpower.h
 * @brief	  �C�x���g�FG�p���[�֘A�C�x���g
 * @author  iwasawa	
 * @date	  2009.02.17
 */
//============================================================================================

#pragma once

/*
 *  @brief  G�p���[�����C�x���g
 *
 *  @param  g_power ��������G�p���[ID
 *  @param  mine    ������G�p���[���ǂ����H
 */
extern GMEVENT* EVENT_GPowerEffectStart(GAMESYS_WORK * gsys, GPOWER_ID g_power, BOOL mine );

/*
 *  @brief  G�p���[���ʏI���C�x���g
 */
extern GMEVENT* EVENT_GPowerEffectEnd( GAMESYS_WORK * gsys );

/*
 *  @brief  G�p���[���ʔ����G�t�F�N�g
 */
extern GMEVENT* EVENT_GPowerUseEffect( GAMESYS_WORK * gsys );



