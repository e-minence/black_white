//============================================================================================
/**
 * @file	musical_save_local.h
 * @brief	�~���[�W�J���p�Z�[�u�f�[�^
 * @author	Nobuhiko Ariizumi
 * @date	2009/04/08
 */
//============================================================================================

#ifndef MUSICAL_SAVE_LOCAL_H__
#define MUSICAL_SAVE_LOCAL_H__

#include "savedata/musical_save.h"

struct _MUSICAL_SAVE {
  //�~���[�W�J���V���b�g
  MUSICAL_SHOT_DATA   musicalShotData;
	//�O�񑕔�
	MUSICAL_EQUIP_SAVE	befEquip;
	
	//�A�C�e������bit
	u8 itemBit[MUS_SAVE_ITEM_BIT_MAX];
  //New�A�C�e��bit
	u8 itemNewBit[MUS_SAVE_ITEM_BIT_MAX];

  MUSICAL_FAN_STATE fanState[MUS_SAVE_FAN_NUM];

  //�Q����
  u16 entryNum; 
  //�g�b�v��
  u16 topNum;
  
  //�O��R���f�B�V����
  u8 befCondition[MCT_MAX];
  //�O��]���_
  u8 befPoint;
  
  //�I�����Ă��鉉�ڔԍ�
  u8 programNumber;
  
  u8 padding[2];
};

#endif //MUSICAL_SAVE_LOCAL_H__