//==============================================================================
/**
 * @file    event_mission_talked.h
 * @brief   �~�b�V�����F�b����������
 * @author  matsuda
 * @date    2010.02.20(�y)
 */
//==============================================================================
#pragma once


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern void EVENTCHANGE_CommMissionTalked_MtoT_Talked(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew);
extern void EVENTCHANGE_CommMissionTalked_TtoM_Talked(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew);
extern void EVENTCHANGE_CommMissionTalked_MtoT_Talk(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew);
extern void EVENTCHANGE_CommMissionTalked_TtoM_Talk(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew);
