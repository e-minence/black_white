//==============================================================================
/**
 * @file    union_local.h
 * @brief   ���j�I�����[���֘A�̃V�X�e�������������\�[�X�݂̂Ŏg�p������`��
 * @author  matsuda
 * @date    2009.07.04(�y)
 */
//==============================================================================
#include "net_app/union/union_main.h"
#include "union_types.h"

#pragma once


//==============================================================================
//  �萔��`
//==============================================================================
///��x�Ɏ�M�ł���ő�r�[�R����
#define UNION_RECEIVE_BEACON_MAX      (SCAN_PARENT_COUNT_MAX)


//==============================================================================
//  �\���̒�`
//==============================================================================
typedef struct _UNION_SYSTEM{
  UNION_PARENT_WORK *uniparent;
  
  UNION_MY_SITUATION my_situation;    ///<�����̏�
  UNION_BEACON my_beacon;             ///<�����̑��M�r�[�R��
  UNION_BEACON_PC receive_beacon[UNION_RECEIVE_BEACON_MAX];  ///<��M�r�[�R��
  
  u8 comm_status;
}UNION_SYSTEM;


