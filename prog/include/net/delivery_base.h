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

#define DELIVERY_IRC_SEND_DATA_MAX  (7)   //���A�p�A�t�����X�A�C�^���A�A�h�C�c�A�X�y�C���A�؍��̂V��

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
  int datasize;   //�f�[�^�S�̃T�C�Y
  u8* pData;     // �f�[�^  ��M�o�b�t�@ ���M�o�b�t�@
  u32 LangCode;    //����R�[�h
} DELIVERY_IRC_DATA;

typedef struct {
  int NetDevID;   // //�ʐM���  network_define.h�Q��
  u16 ConfusionID;   //�������Ȃ����߂�ID ���M���ŕK�v
  HEAPID heapID;    //HEAPD
  DELIVERY_IRC_DATA  data[DELIVERY_IRC_SEND_DATA_MAX];
  u32               dataNum;   //���M���͕����A��M���͂P�����ݒ肷�邱��
} DELIVERY_IRC_INIT;



