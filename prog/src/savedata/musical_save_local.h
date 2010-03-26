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
	
  MUSICAL_FAN_STATE fanState[MUS_SAVE_FAN_NUM];
  
  //�z�M���ږ�
  STRCODE distTitle[MUSICAL_PROGRAM_NAME_MAX];  //37
  
  //�Q����
  u16 entryNum; 

  //�g�b�v��
  u16 topNum;
  //�݌v���_
  u16 sumPoint;

  //�A�C�e������bit
  u8 itemBit[MUS_SAVE_ITEM_BIT_MAX];
  //New�A�C�e��bit
  u8 itemNewBit[MUS_SAVE_ITEM_BIT_MAX];
  
  //�O��R���f�B�V����
  u8 befCondition[MCT_MAX];
  //�O��]���_
  u8 befPoint;

  //�I�����Ă��鉉�ڔԍ�
  u8 programNumber;
  //�z�M�f�[�^�̃`�F�b�N
  u8 enableDistData;
  //�f�����������H
  u8 isLookDemo;
  u8 padding[1];
};

#endif //MUSICAL_SAVE_LOCAL_H__