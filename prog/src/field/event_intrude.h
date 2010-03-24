//==============================================================================
/**
 * @file    event_intrude.h
 * @brief   �N���F�C�x���g
 * @author  matsuda
 * @date    2009.12.15(��)
 */
//==============================================================================
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "field/field_msgbg.h"
#include "fieldmap.h"
#include "fldmmdl.h"

#include "field_comm\intrude_types.h"
#include "field_comm\intrude_message.h"
#include "field/event_comm_common.h"

//==============================================================================
//  �萔��`
//==============================================================================
typedef enum{
  FIRST_TALK_RET_NULL,      ///<�V�[�P���X���s��
  FIRST_TALK_RET_OK,        ///<OK�ŏI��
  FIRST_TALK_RET_NG,        ///<NG�ŏI��
  FIRST_TALK_RET_CANCEL,    ///<�L�����Z������
}FIRST_TALK_RET;


//==============================================================================
//  �\���̒�`
//==============================================================================
///�ϑ����o�Ǘ����[�N
typedef struct{
	GAMESYS_WORK *gsys;
	FIELDMAP_WORK *fieldWork;
	s16 wait;
	s16 wait_max;
	u16 disguise_code;
	u8 disguise_type;     ///<TALK_TYPE_xxx
	u8 disguise_sex;
  u8 seq;
	u8 anm_no;
	u8 loop;

  // �o���A���o��p
	u8 dir;
  MMDL* tmp_mdl;
}INTRUDE_EVENT_DISGUISE_WORK;


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern GMEVENT * EVENT_Disguise(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork,
  INTRUDE_COMM_SYS_PTR intcomm, HEAPID heap_id);

//--------------------------------------------------------------
//  �ϑ����o�V�[�P���X
//--------------------------------------------------------------
// ���邭��
extern void IntrudeEvent_Sub_DisguiseEffectSetup(INTRUDE_EVENT_DISGUISE_WORK *iedw, GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, u16 disguise_code, u8 disguise_type, u8 disguise_sex);
extern BOOL IntrudeEvent_Sub_DisguiseEffectMain(INTRUDE_EVENT_DISGUISE_WORK *iedw, INTRUDE_COMM_SYS_PTR intcomm);
// Barrier�w���V��
extern void IntrudeEvent_Sub_BarrierDisguiseEffectSetup(INTRUDE_EVENT_DISGUISE_WORK *iedw, GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, INTRUDE_COMM_SYS_PTR intcomm, u16 disguise_code, u8 disguise_type, u8 disguise_sex, u16 move_dir, BOOL change );
extern BOOL IntrudeEvent_Sub_BarrierDisguiseEffectMain(INTRUDE_EVENT_DISGUISE_WORK *iedw, INTRUDE_COMM_SYS_PTR intcomm);



//--------------------------------------------------------------
//  
//--------------------------------------------------------------
extern FIRST_TALK_RET EVENT_INTRUDE_FirstTalkSeq(INTRUDE_COMM_SYS_PTR intcomm, COMMTALK_COMMON_EVENT_WORK *ccew, u8 *seq);

//--------------------------------------------------------------
//  
//--------------------------------------------------------------
extern GMEVENT * EVENT_IntrudeWarpMyPalace(GAMESYS_WORK *gsys);
