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
#include "net_app/union/comm_player.h"
#include "net_app/union/colosseum_types.h"


//==============================================================================
//  �萔��`
//==============================================================================
///��x�Ɏ�M�ł���ő�r�[�R����
#define UNION_RECEIVE_BEACON_MAX      (10)

///�L�����N�^�Ǘ���(��x�ɏo����ő�L�����N�^��)
#define UNION_CHARACTER_MAX           (UNION_RECEIVE_BEACON_MAX * UNION_CONNECT_PLAYER_NUM)

///����f�[�^��M�o�b�t�@�T�C�Y
#define UNION_HUGE_RECEIVE_BUF_SIZE   (0x800)

///���X�^�[�g���[�h
typedef enum{
  UNION_RESTART_SHUTDOWN_RESTARTS,    ///<�ؒf���ĊJ
  UNION_RESTART_SHUTDOWN,             ///<�ؒf�̂�
  UNION_RESTART_RESTARTS,             ///<�ĊJ�̂�
}UNION_RESTART;


//==============================================================================
//  �\���̒�`
//==============================================================================
typedef struct _UNION_SYSTEM{
  UNION_PARENT_WORK *uniparent;
  u8 huge_receive_buf[UNION_CONNECT_PLAYER_NUM][UNION_HUGE_RECEIVE_BUF_SIZE];
  
  GFL_MSGDATA *msgdata;
  FLDMSGWIN_STREAM *fld_msgwin_stream;
  FLDMENUFUNC *fldmenu_func;
  FLDMENUFUNC *fldmenu_yesno_func;
  
  UNION_MY_SITUATION my_situation;    ///<�����̏�
  UNION_BEACON my_beacon;             ///<�����̑��M�r�[�R��
  UNION_BEACON_PC receive_beacon[UNION_RECEIVE_BEACON_MAX];  ///<��M�r�[�R��
  UNION_CHARACTER character[UNION_CHARACTER_MAX];   ///<�L�����N�^�Ǘ��o�b�t�@
  
  UNION_SUB_PROC subproc;             ///<�T�uPROC�Ăяo������
  void *parent_work;                  ///<�T�uPROC�p�Ɏg�p����ꎞ�I�ȃ��[�N�Ǘ��p
  
  COLOSSEUM_SYSTEM *colosseum_sys;    ///<�R���V�A���V�X�e�����[�N
  
  u8 comm_status;
  u8 restart_seq;                     ///<�u�ؒf���ĊJ�v�����V�[�P���XNo
  u8 restart_mode;                    ///< UNION_RESTART_???
  u8 player_pause;                    ///<���@�̃|�[�Y��
}UNION_SYSTEM;


