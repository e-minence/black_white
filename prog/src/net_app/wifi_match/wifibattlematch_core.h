//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifibattlematch_core.h
 *	@brief	WIFI�̃o�g���}�b�`�R�A���
 *	@author	Toru=Nagihashi
 *	@date		2009.11.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include "net_app/wifibattlematch.h"
#include "battle/battle.h"
#include "wifibattlematch_data.h"
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�I���R�[�h
//=====================================
typedef enum 
{
  WIFIBATTLEMATCH_CORE_RESULT_FINISH,       //�A�v���I��
  WIFIBATTLEMATCH_CORE_RESULT_NEXT_BATTLE,  //�o�g����
} WIFIBATTLEMATCH_CORE_RESULT;

//-------------------------------------
///	�N�����[�h
//=====================================
typedef enum 
{
  WIFIBATTLEMATCH_CORE_MODE_START,      //�J�n
  WIFIBATTLEMATCH_CORE_MODE_ENDBATTLE,  //�o�g���I��
} WIFIBATTLEMATCH_CORE_MODE;


//=============================================================================
/**
 *					�\����
*/
//=============================================================================
//-------------------------------------
///	�R�A�v���Z�X�̈���
//=====================================
typedef struct
{
  WIFIBATTLEMATCH_CORE_RESULT result;   //[out]�I���R�[�h
  WIFIBATTLEMATCH_PARAM       *p_param; //[in ]�O������
  WIFIBATTLEMATCH_CORE_MODE   mode;     //[in ]�N�����[�h
  BtlResult                   btl_result;   //[in ]�o�g������
  WIFIBATTLEMATCH_DATA_WORK   *p_data;      //[in ]�풓�f�[�^�u����
} WIFIBATTLEMATCH_CORE_PARAM;

//=============================================================================
/**
 *					�O���Q��
*/
//=============================================================================
FS_EXTERN_OVERLAY( wifibattlematch_core );
extern const GFL_PROC_DATA	WifiBattleMaptchCore_ProcData;
