
#pragma once

//----------------------------------------------------------
/**
 * @brief  �\���̒�`
 */
//----------------------------------------------------------

struct _WIFI_NEGOTIATION_SAVEDATA{
  MYSTATUS aMyStatus[WIFI_NEGOTIATION_DATAMAX];
  u16 dummy_date[WIFI_NEGOTIATION_DATAMAX];         //��������  �g��Ȃ�����
  PMS_DATA message;   //�ȈՕ� ���ȏЉ
  u16 dummy_count;   //�f�[�^�z�ׂ̈̃J�E���^  �g��Ȃ�����
  u16 num;  //�����̌����|�P������
};

