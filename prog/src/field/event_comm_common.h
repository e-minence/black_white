//==============================================================================
/**
 * @file    event_comm_common.h
 * @brief   �~�b�V�����F�b�������A�b���������C�x���g���ʏ���
 * @author  matsuda
 * @date    2010.02.20(�y)
 */
//==============================================================================
#pragma once

#include "field\field_comm\intrude_message.h"


//==============================================================================
//  �\���̒�`
//==============================================================================
///���ʃC�x���g���[�N
typedef struct{
	HEAPID heapID;
	MMDL *fmmdl_player;             ///<����
	MMDL *fmmdl_talk;               ///<�b����������
	FIELDMAP_WORK *fieldWork;
	
	INTRUDE_EVENT_MSGWORK iem;

	u8 talk_netid;                 ///<�b�����NetID
	u8 intrude_talk_type;          ///<INTRUDE_TALK_TYPE_xxx
	u8 padding[2];
	
	MISSION_DATA mdata;            ///<���{���̃~�b�V�����f�[�^(�b���������u�Ԃ̃f�[�^��ێ�)
}COMMTALK_COMMON_EVENT_WORK;


//==============================================================================
//  �^��`
//==============================================================================
typedef void (*EVENT_COMM_FUNC)(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew);


//==============================================================================
//  �O���֐��錾
//==============================================================================
//--------------------------------------------------------------
//  ���ʃC�x���g�N��
//--------------------------------------------------------------
extern GMEVENT * EVENT_CommCommon_Talk(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, INTRUDE_COMM_SYS_PTR intcomm, MMDL *fmmdl_player, u32 talk_net_id, HEAPID heap_id);
extern GMEVENT * EVENT_CommCommon_Talked(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork,
  INTRUDE_COMM_SYS_PTR intcomm, MMDL *fmmdl_player, u32 talk_net_id, HEAPID heap_id);

//--------------------------------------------------------------
//  �e�C�x���g�֎}�����ꂵ����̋��ʏ���
//--------------------------------------------------------------
extern void EVENT_CommCommon_Finish(INTRUDE_COMM_SYS_PTR intcomm, COMMTALK_COMMON_EVENT_WORK *ccew);
