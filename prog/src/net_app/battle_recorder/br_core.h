//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_core.h
 *	@brief	�o�g�����R�[�_�[�{��
 *	@author	Toru=Nagihashi
 *	@date		2009.11.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include "net_app/battle_recorder.h"
#include "br_net.h"
#include "br_inner.h"
#include "br_data.h"
//=============================================================================
/**
 *					�萔
*/
//=============================================================================
//-------------------------------------
///	�v���Z�XID
//=====================================
typedef enum 
{
	BR_PROCID_START,				//�N�����
	BR_PROCID_MENU,					//���j���[���
	BR_PROCID_RECORD,				//�^��L�^���
	BR_PROCID_BTLSUBWAY,		//�o�g���T�u�E�F�C���щ��
	BR_PROCID_RNDMATCH,			//�����_���}�b�`���щ��
	BR_PROCID_BV_RANK,			//�o�g���r�f�I30����ʁi�ŐV�A�ʐM�ΐ�A�T�u�E�F�C�j
	BR_PROCID_BV_SEARCH,		//�ڂ����T�����
	BR_PROCID_CODEIN,				//�o�g���r�f�I�i���o�[���͉��
	BR_PROCID_BV_SEND,			//�o�g���r�f�I���M���
	BR_PROCID_BV_DELETE,	  //�o�g���r�f�I�������
  BR_PROCID_BV_SAVE,      //�o�g���r�f�I�ۑ����
	BR_PROCID_MUSICAL_LOOK,	//�~���[�W�J���V���b�g	�ʐ^��������
	BR_PROCID_MUSICAL_SEND,	//�~���[�W�J���V���b�g	�ʐ^�𑗂���

	BR_PROCID_MAX
} BR_PROCID;

//-------------------------------------
///	�o�g�����R�[�_�[�R�A�̋N�����[�h
//=====================================
typedef enum
{
  BR_CORE_MODE_INIT,    //����
  BR_CORE_MODE_RETURN,  //�o�g�����畜�A
} BR_CORE_MODE;

//-------------------------------------
///	�o�g�����R�[�_�[�R�A�̏I�����[�h
//=====================================
typedef enum
{
  BR_CORE_RETURN_FINISH,  //�I��
  BR_CORE_RETURN_BTLREC,  //�o�g����
} BR_CORE_RETURN;

//=============================================================================
/**
 *					�\����
*/
//=============================================================================
//-------------------------------------
///	�o�g�����R�[�_�[�R�A�p����
//=====================================
typedef struct 
{
  BR_CORE_RETURN        ret;      //[out]�I�����[�h
  BR_CORE_MODE          mode;     //[in ]�N�����[�h
  BR_DATA               *p_data;  //[in ]�풓�f�[�^
	BATTLERECORDER_PARAM	*p_param; //[in ]�{�̂̈���
} BR_CORE_PARAM;


//=============================================================================
/**
 *					�v���Z�X�f�[�^
*/
//=============================================================================
extern const GFL_PROC_DATA BR_CORE_ProcData;
