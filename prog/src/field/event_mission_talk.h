//==============================================================================
/**
 * @file    event_mission_talk.h
 * @brief   �~�b�V�����F�b��������
 * @author  matsuda
 * @date    2010.02.21(��)
 */
//==============================================================================
#pragma once


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern void EVENTCHANGE_CommMissionTalk_MtoT_Talk(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew);
extern void EVENTCHANGE_CommMissionTalk_TtoM_Talk(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew);
extern void EVENTCHANGE_CommMissionTalk_MtoT_Talked(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew);
extern void EVENTCHANGE_CommMissionTalk_TtoM_Talked(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew);

