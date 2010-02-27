//======================================================================
/**
 * @file	  delivery_beacon.h
 * @brief	  �r�[�R���z�M
 * @author	k.ohno
 * @data	  10/02/27
 */
//======================================================================

#pragma once
#include "net/network_define.h"

#define DELIVERY_BEACON_ONCE_NUM  (88)  //���M�T�C�Y
#define DELIVERY_BEACON_MAX_NUM  (9)    //DELIVERY_BEACON_MAX_NUM*DELIVERY_BEACON_ONCE_NUM�o�C�g�̑��M���\



typedef struct {
  int NetDevID;   // //�ʐM���
  int datasize;   //�f�[�^�S�̃T�C�Y
  u8* pData;     // �f�[�^
  u16 ConfusionID;   //�������Ȃ����߂�ID ���M���ŕK�v
  HEAPID heapID;    //HEAP
} DELIVERY_BEACON_INIT;


typedef struct _DELIVERY_BEACON_LOCALWORK DELIVERY_BEACON_WORK;


//--------------------------------------------------------------
/**
 * @brief   �r�[�R���z��������
 * @param   DELIVERY_BEACON_INIT �������\���� ���[�J���ϐ��ł����v
 * @retval  DELIVERY_BEACON_WORK �Ǘ����[�N
 */
//--------------------------------------------------------------
extern DELIVERY_BEACON_WORK* DELIVERY_BEACON_Init(DELIVERY_BEACON_INIT* pInit);

//--------------------------------------------------------------
/**
 * @brief   �r�[�R���z�M�J�n
 * @param   DELIVERY_BEACON_WORK �Ǘ����[�N
 */
//--------------------------------------------------------------
extern BOOL DELIVERY_BEACON_SendStart(DELIVERY_BEACON_WORK* pWork);

//--------------------------------------------------------------
/**
 * @brief   �r�[�R����M�J�n
 * @param   DELIVERY_BEACON_WORK �Ǘ����[�N
 * @retval  �J�n�ł�����TRUE
 */
//--------------------------------------------------------------
extern BOOL DELIVERY_BEACON_RecvStart(DELIVERY_BEACON_WORK* pWork);

//--------------------------------------------------------------
/**
 * @brief   �r�[�R����M�������ǂ���
 * @param   DELIVERY_BEACON_WORK �Ǘ����[�N
 * @retval  �󂯎�ꂽ�ꍇTRUE
 */
//--------------------------------------------------------------
extern BOOL DELIVERY_BEACON_RecvCheck(DELIVERY_BEACON_WORK* pWork);

//--------------------------------------------------------------
/**
 * @brief   �r�[�R�����P�ł����邩�ǂ���
 * @param   DELIVERY_BEACON_WORK �Ǘ����[�N
 * @retval  ����ꍇTRUE
 */
//--------------------------------------------------------------
extern BOOL DELIVERY_BEACON_RecvSingleCheck(DELIVERY_BEACON_WORK* pWork);

//--------------------------------------------------------------
/**
 * @brief   �r�[�R���z�����C��
 * @param   DELIVERY_BEACON_WORK �Ǘ����[�N
 */
//--------------------------------------------------------------

extern void DELIVERY_BEACON_Main(DELIVERY_BEACON_WORK*  pWork);

//--------------------------------------------------------------
/**
 * @brief   �r�[�R���z���I��
 * @param   DELIVERY_BEACON_WORK �Ǘ����[�N
 */
//--------------------------------------------------------------
extern void DELIVERY_BEACON_End(DELIVERY_BEACON_WORK*  pWork);
