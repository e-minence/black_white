//======================================================================
/**
 * @file	  delivery_irc.h
 * @brief	  IRC�z�M
 * @author	k.ohno
 * @data	  10/02/27
 */
//======================================================================

#pragma once
#include "net/delivery_base.h"


#define DELIVERY_IRC_FUNC (0)   //���쒆
#define DELIVERY_IRC_SUCCESS (1)  //����
#define DELIVERY_IRC_FAILED (2)   //���s
#define DELIVERY_IRC_NOTDATA (3)  //��M���F�Ή����郊�N�G�X�g�����M���ɂȂ��������߂��炦�Ȃ�����
                                  //���M���F�Ή����Ă��Ȃ����N�G�X�g�����ꂽ

typedef struct _DELIVERY_IRC_LOCALWORK DELIVERY_IRC_WORK;


//--------------------------------------------------------------
/**
 * @brief   IRC�z��������
 * @param   DELIVERY_IRC_INIT �������\���� ���[�J���ϐ��ł����v
 * @retval  DELIVERY_IRC_WORK �Ǘ����[�N
 */
//--------------------------------------------------------------
extern DELIVERY_IRC_WORK* DELIVERY_IRC_Init(DELIVERY_IRC_INIT* pInit);

//--------------------------------------------------------------
/**
 * @brief   IRC�z�M�J�n
 * @param   DELIVERY_IRC_WORK �Ǘ����[�N
 */
//--------------------------------------------------------------
extern BOOL DELIVERY_IRC_SendStart(DELIVERY_IRC_WORK* pWork);

//--------------------------------------------------------------
/**
 * @brief   IRC��M�J�n
 * @param   DELIVERY_IRC_WORK �Ǘ����[�N
 * @retval  �J�n�ł�����TRUE
 */
//--------------------------------------------------------------
extern BOOL DELIVERY_IRC_RecvStart(DELIVERY_IRC_WORK* pWork);

//--------------------------------------------------------------
/**
 * @brief   IRC��M�������ǂ���
 * @param   DELIVERY_IRC_WORK �Ǘ����[�N
 * @retval  �󂯎�ꂽ�ꍇDELIVERY_IRC_SUCCESS ���sDELIVERY_IRC_FAILED
 */
//--------------------------------------------------------------
extern int DELIVERY_IRC_RecvCheck(DELIVERY_IRC_WORK* pWork);

//--------------------------------------------------------------
/**
 * @brief   IRC�z�����C��
 * @param   DELIVERY_IRC_WORK �Ǘ����[�N
 */
//--------------------------------------------------------------

extern void DELIVERY_IRC_Main(DELIVERY_IRC_WORK*  pWork);

//--------------------------------------------------------------
/**
 * @brief   IRC�z���I��
 * @param   DELIVERY_IRC_WORK �Ǘ����[�N
 */
//--------------------------------------------------------------
extern void DELIVERY_IRC_End(DELIVERY_IRC_WORK*  pWork);
