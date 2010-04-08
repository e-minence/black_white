//============================================================================================
/**
 * @file  event_demo3d.h
 * @brief Demo3D�A�v���Ăяo���C�x���g
 * @date  2010.04.07
 * @author  iwasawa GAME FREAK inc.
 */
//============================================================================================

#pragma once

#include "gamesystem/gamesystem.h"
#include "demo/demo3d_demoid.h"

//------------------------------------------------------------------
/*
 *  @brief  �f���R�[���C�x���g�N��
 *
 *  @param  gsys  GAMESYS_WORK*
 *  @param  demo_id �f��ID DEMO3D_ID_C_CRUISER��
 *  @param  scene_id  �V�[��ID
 *  @param  use_rtc ���Ԃ�RTC������Ȃ�TRUE,EV-TIME������Ȃ�FALSE
 */
//------------------------------------------------------------------
extern GMEVENT* EVENT_CallDemo3D( GAMESYS_WORK* gsys, u16 demo_id, u16 scene_id, BOOL use_rtc );

