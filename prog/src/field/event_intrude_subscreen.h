//==============================================================================
/**
 * @file    event_intrude_subscreen.h
 * @brief   �ȒP�Ȑ���������
 * @author  matsuda
 * @date    2009.10.18(��)
 */
//==============================================================================
#pragma once


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern GMEVENT * EVENT_ChangeIntrudeSubScreen(
  GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, FIELD_SUBSCREEN_MODE change_mode);
extern GMEVENT * EVENT_IntrudeTownWarp(
  GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, int town_tblno);
extern GMEVENT * EVENT_IntrudeMissionPut(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, HEAPID heap_id);

