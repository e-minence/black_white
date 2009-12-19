//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_record_proc.h
 *	@brief	���R�[�h�v���Z�X
 *	@author	Toru=Nagihashi
 *	@date		2009.11.13
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
#include "br_proc_sys.h"
#include "br_res.h"
#include "br_fade.h"
#include "savedata/gds_profile.h"
#include "savedata/battle_rec.h"
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	���R�[�h���[�h
//=====================================
typedef enum
{
	BR_RECODE_PROC_MY,
	BR_RECODE_PROC_OTHER_00,
	BR_RECODE_PROC_OTHER_01,
	BR_RECODE_PROC_OTHER_02,
} BR_RECODE_PROC_MODE;

//-------------------------------------
///	�I�����[�h
//=====================================
typedef enum
{
  BR_RECORD_RETURN_FINISH,
  BR_RECORD_RETURN_BTLREC,
} BR_RECORD_RETURN;

//=============================================================================
/**
 *					�\����
*/
//=============================================================================
//-------------------------------------
///	���R�[�h�v���Z�X����
//=====================================
typedef struct 
{
  BR_RECORD_RETURN    ret;          //[out]�I�����[�h
	BR_RECODE_PROC_MODE	mode;					//[in ]�N�����[�h
  BR_FADE_WORK        *p_fade;      //[in ]�t�F�[�h
	BR_RES_WORK					*p_res;				//[in ]���\�[�X�Ǘ�
	BR_PROC_SYS					*p_procsys;		//[in ]�v���Z�X�Ǘ�
	GFL_CLUNIT					*p_unit;			//[in ]���j�b�g
  GDS_PROFILE_PTR       p_profile;  //[in ]�v���t�B�[��
  BATTLE_REC_HEADER_PTR p_header;   //[in ]�o�g���w�b�_
} BR_RECORD_PROC_PARAM;


//=============================================================================
/**
 *					�O�����J�֐�
*/
//=============================================================================
//-------------------------------------
///	���R�[�h�v���Z�X�f�[�^
//=====================================
extern const GFL_PROC_DATA	BR_RECORD_ProcData;
