//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifibattlematch_utilproc.h
 *	@brief  wifibattlematch_sys�ŐF�X�ȏ����������Ȃ����߂ɁAPROC����������
 *	@author	Toru=Nagihashi
 *	@date		2010.02.16
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
#include "net_app/wifibattlematch.h"
#include "wifibattlematch_data.h"
#include "wifibattlematch_net.h"
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					���X�g��v���Z�X
 *					    �^�C�~���O���Ƃ��ăp�[�e�B�𑗂邾��
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	���X�g��v���Z�X�̖߂�l
//=====================================
typedef enum 
{
  WIFIBATTLEMATCH_LISTAFTER_RESULT_SUCCESS,           //����ɏI��
  WIFIBATTLEMATCH_LISTAFTER_RESULT_ERROR_NEXT_LOGIN,  //�G���[���m(WIFI�̂�)
  WIFIBATTLEMATCH_LISTAFTER_RESULT_ERROR_RETURN_LIVE, //�G���[���m(LIVE�̂�)
  WIFIBATTLEMATCH_LISTAFTER_RESULT_ERROR_DISCONNECT_WIFI, //�ؒf���ꂽ�iWIFI�̂݁j
} WIFIBATTLEMATCH_LISTAFTER_RESULT;

//-------------------------------------
///	�ʐM�^�C�v
//=====================================
typedef enum
{
  WIFIBATTLEMATCH_LISTAFTER_NETTYPE_WIFI,
  WIFIBATTLEMATCH_LISTAFTER_NETTYPE_IRC,
} WIFIBATTLEMATCH_LISTAFTER_NETTYPE;


//-------------------------------------
///	���X�g��v���Z�X�̈���
//=====================================
typedef struct
{
  WIFIBATTLEMATCH_PARAM             *p_param;     //[in ]�O������
  WIFIBATTLEMATCH_LISTAFTER_NETTYPE type;         //[in ]�ʐM�^�C�v
  WIFIBATTLEMATCH_LISTAFTER_RESULT  result;       //[out]�߂�l

  //�ȉ��A[in ]�풓�f�[�^�u����
  POKEPARTY                   *p_player_btl_party; //�����Ō��߂��p�[�e�B
  POKEPARTY                   *p_enemy_btl_party; //����̌��߂��p�[�e�B
  WIFIBATTLEMATCH_NET_DATA    *p_net_data;
  WIFIBATTLEMATCH_RECV_DATA   *p_recv_data;
} WIFIBATTLEMATCH_LISTAFTER_PARAM;

//-------------------------------------
///	���X�g��v���Z�X�̊O���Q��
//=====================================
//FS_EXTERN_OVERLAY( wifibattlematch_core );  //core.h�ŌĂ΂�Ă���
//���X�g��̏���
extern const GFL_PROC_DATA	WifiBattleMatch_ListAfter_ProcData;

