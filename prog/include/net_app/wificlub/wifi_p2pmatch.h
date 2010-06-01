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
#include "gamesystem/game_data.h"
#include "wifi_p2pmatch_def.h"

#include "savedata/wifilist.h"
#include "savedata/regulation.h"

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
  GAMEDATA* pGameData;
  SAVE_CONTROL_WORK*  pSaveData;
  int seq;        // �ǂ��ɕ��򂵂Ăق������������Ă���
  int targetID;   // �ΐ� ��������l���N�Ȃ̂��������Ă���
  POKEPARTY* pPokeParty[2];   //���݂���Party����M
  REGULATION* pRegulation;    //ROM����ǂݍ���
  u8 matchno[WIFILIST_FRIEND_MAX];   //�O��}�b�`���O��������no
  u8 VCTOn[2];   ///<CNM_WFP2PMF_STATUS  
  u8 vchatMain;  // VCHAT��ONOFF���郁�C���t���O �����͑���ɂ���ĕω������
  u8 friendNo;  //�ΐ킵�Ă���l�̔ԍ�
  u8 shooter;         //�키�悤�̃V���[�^�[
  u8 shooterSelect;   //�����őI�������V���[�^�[
  u8 dummy3[2]; 
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



