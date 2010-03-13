//======================================================================
/**
 * @file	  delivery_base.h
 * @brief	  �z�M�̏������\���̂Ɗ�{���
 * @author	k.ohno
 * @data	  10/02/28
 */
//======================================================================

#pragma once

#include "net/network_define.h"

#define DELIVERY_BEACON_ONCE_NUM  (88)  //�r�[�R�����̑��M�T�C�Y
#define DELIVERY_BEACON_MAX_NUM  (17)    //DELIVERY_BEACON_MAX_NUM*DELIVERY_BEACON_ONCE_NUM�o�C�g�̑��M���\

//IRC���M�T�C�Y
//�ԊO���͓����ŕ������M
#define DELIVERY_IRC_MAX_NUM  (DELIVERY_BEACON_ONCE_NUM*DELIVERY_BEACON_MAX_NUM)


typedef struct {
  int NetDevID;   // //�ʐM��� network_define.h�Q��
  int datasize;   //�f�[�^�S�̃T�C�Y
  u8* pData;     // �f�[�^
  u16 ConfusionID;   //�������Ȃ����߂�ID ���M���ŕK�v
  HEAPID heapID;    //HEAP
} DELIVERY_BEACON_INIT;

typedef struct {
  int NetDevID;   // //�ʐM���  network_define.h�Q��
  int datasize;   //�f�[�^�S�̃T�C�Y
  u8* pData;     // �f�[�^  ��M�o�b�t�@ ���M�o�b�t�@
  u16 ConfusionID;   //�������Ȃ����߂�ID ���M���ŕK�v
  HEAPID heapID;    //HEAP
  u32 LangCode;    //����R�[�h
} DELIVERY_IRC_INIT;



