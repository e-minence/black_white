//======================================================================
/**
 * @file  trainer_eye_data.h
 * @date  2009.09.09
 * @author  GAMEFREAK inc.
 *
 * 2009.09.09 script.c���番������
 */
//======================================================================
#pragma once
//--------------------------------------------------------------
/**
 * �g���[�i�[�����f�[�^�\����
 */
//--------------------------------------------------------------
typedef struct {
	int range;				//��������
	int dir;					//�ړ�����
	int scr_id;				//�X�N���v�gID
	int tr_id;				//�g���[�i�[ID
	int tr_type;			//�g���[�i�[�^�C�v
	MMDL *mmdl;
  GMEVENT *ev_eye_move;
}EV_TRAINER_EYE_HITDATA;

//--------------------------------------------------------------
//--------------------------------------------------------------
static inline void TRAINER_EYE_HITDATA_Set(
    EV_TRAINER_EYE_HITDATA * eye,
    MMDL *mmdl,
    s16 range, u16 dir, u16 scr_id, u16 tr_id, int tr_type, int tr_no )
{
  eye->range = range;
	eye->dir = dir;
	eye->scr_id = scr_id;
	eye->tr_id = tr_id;
	eye->tr_type = tr_type;
	eye->mmdl = mmdl;
  eye->ev_eye_move = NULL;
}

