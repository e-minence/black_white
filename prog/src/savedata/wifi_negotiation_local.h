
#pragma once

//----------------------------------------------------------
/**
 * @brief  �\���̒�`
 */
//----------------------------------------------------------

struct _WIFI_NEGOTIATION_SAVEDATA{
  MYSTATUS aMyStatus[WIFI_NEGOTIATION_DATAMAX];
  u16 date[WIFI_NEGOTIATION_DATAMAX];         //��������
  PMS_DATA message;   //�ȈՕ� ���ȏЉ
  u16 count;   //�f�[�^�z�ׂ̈̃J�E���^
  u16 num;  //�����̌����|�P������
};

