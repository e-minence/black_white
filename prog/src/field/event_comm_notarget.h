//==============================================================================
/**
 * @file    event_comm_notarget.h
 * @brief   �~�b�V�����F���̑���b
 * @author  matsuda
 * @date    2010.04.19(��)
 */
//==============================================================================
#pragma once


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern void EVENTCHANGE_CommMissionEtc_MtoT_Battle_Talk(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew);
extern void EVENTCHANGE_CommMissionEtc_MtoT_NotTalk_Talk(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew);
extern void EVENTCHANGE_CommMissionEtc_MtoM_Talk(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew);
extern void EVENTCHANGE_CommMissionEtc_MtoM_Talked(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew);
extern void EVENTCHANGE_CommMissionEtc_MtoN_M(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew);
extern void EVENTCHANGE_CommMissionEtc_MtoN_N(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew);
