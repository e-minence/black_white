//============================================================================================
/**
 * @file	wifi_p2pmatch.h
 * @bfief	WIFI P2P�ڑ��̃}�b�`���O
 * @author	k.ohno
 * @date	06.04.07
 */
//============================================================================================
#pragma once

#include <gflib.h>
#include "gamesystem/game_event.h"
#include "wifi_p2pmatch_def.h"

//============================================================================================
//	��`
//============================================================================================

//------------------------------------------------------
/**
 * WIFI�Ƃ��������X�gGFL_PROC�p�����[�^
 */
//------------------------------------------------------
typedef struct {
  void* pMatch;
	SAVE_CONTROL_WORK*  pSaveData;
    int seq;                // �ǂ��ɕ��򂵂Ăق������������Ă���
    int targetID;   //�ΐ� ��������l���N�Ȃ̂��������Ă���
}WIFIP2PMATCH_PROC_PARAM;

extern const GFL_PROC_DATA WifiP2PMatchProcData;

// �|�C���^�Q�Ƃ����ł��郌�R�[�h�R�[�i�[���[�N�\����
typedef struct _WIFIP2PMATCH_WORK WIFIP2PMATCH_WORK;	

// �Ăяo�����߂̃C�x���g
extern void EventCmd_P2PMatchProc(GMEVENT * event);
// ���E�����O�ɌĂяo�����߂̃C�x���g
extern void EventCmd_DPWInitProc(GMEVENT * event);
//
extern void EventCmd_DPWInitProc2(GMEVENT * event, u16* ret);


//------------------------------------------------------------------
/*
   @title WIFI�N���u�C�x���g���������m��
 */
//------------------------------------------------------------------
extern void* WIFICLUB_CreateWork(void);



