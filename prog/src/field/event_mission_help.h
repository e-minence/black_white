//==============================================================================
/**
 * @file    event_mission_help.h
 * @brief   �~�b�V�����F������
 * @author  matsuda
 * @date    2010.02.21(��)
 */
//==============================================================================
#pragma once


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern void EVENTCHANGE_CommMissionHelp_MtoT_Talk(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew);
extern void EVENTCHANGE_CommMissionHelp_TtoM_Talk(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew);
extern void EVENTCHANGE_CommMissionHelp_MtoT_Talked(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew);
extern void EVENTCHANGE_CommMissionHelp_TtoM_Talked(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew);
extern void EVENTCHANGE_CommMissionHelp_TtoM_Battle(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew);

