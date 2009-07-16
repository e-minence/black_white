//==============================================================================
/**
 * @file    union_local.h
 * @brief   ���j�I�����[���֘A�̃V�X�e�������������\�[�X�݂̂Ŏg�p������`��
 * @author  matsuda
 * @date    2009.07.04(�y)
 */
//==============================================================================
#pragma once

#include "net_app/union/union_main.h"
#include "union_types.h"
#include "field/field_msgbg.h"



//==============================================================================
//  �萔��`
//==============================================================================
///��x�ɐڑ��ł���ő�l��
#define UNION_CONNECT_PLAYER_NUM      (5) //���R�[�h�R�[�i�[���ő�5�l�Ȃ̂�

///��x�Ɏ�M�ł���ő�r�[�R����
#define UNION_RECEIVE_BEACON_MAX      (SCAN_PARENT_COUNT_MAX)

///����f�[�^��M�o�b�t�@�T�C�Y
#define UNION_HUGE_RECEIVE_BUF_SIZE   (0x200)

//==============================================================================
//  �\���̒�`
//==============================================================================
typedef struct _UNION_SYSTEM{
  UNION_PARENT_WORK *uniparent;
  u8 huge_receive_buf[UNION_CONNECT_PLAYER_NUM][UNION_HUGE_RECEIVE_BUF_SIZE];
  
  GFL_MSGDATA *msgdata;
  FLDMSGWIN_STREAM *fld_msgwin_stream;
  FLDMENUFUNC_LISTDATA *fldmenu_listdata;
  FLDMENUFUNC *fldmenu_func;
  FLDMENUFUNC *fldmenu_yesno_func;
  
  UNION_MY_SITUATION my_situation;    ///<�����̏�
  UNION_BEACON my_beacon;             ///<�����̑��M�r�[�R��
  UNION_BEACON_PC receive_beacon[UNION_RECEIVE_BEACON_MAX];  ///<��M�r�[�R��
  
  UNION_SUB_PROC subproc;             ///<�T�uPROC�Ăяo������
  
  u8 comm_status;
}UNION_SYSTEM;


