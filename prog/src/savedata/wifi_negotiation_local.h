
#pragma once

//----------------------------------------------------------
/**
 * @brief  �\���̒�`
 */
//----------------------------------------------------------

struct _WIFI_NEGOTIATION_SAVEDATA{
  MYSTATUS aMyStatus[WIFI_NEGOTIATION_DATAMAX];
  u16 dummy_date[WIFI_NEGOTIATION_DATAMAX];         //��������  �������Ă��Ȃ�
  PMS_DATA message;   //�ȈՕ� ���ȏЉ  �������Ă��Ȃ�
  u32 num;  //�����̌����|�P������
};

