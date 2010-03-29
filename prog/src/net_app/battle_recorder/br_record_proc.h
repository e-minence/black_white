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
#include "br_inner.h"
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
	BR_RECODE_PROC_DOWNLOAD_RANK,
	BR_RECODE_PROC_DOWNLOAD_NUMBER,
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
  u64                 video_number; //[in ]�r�f�I�i���o�[�imode��BR_RECODE_PROC_DOWNLOAD_NUMBER�̂Ƃ��̂ݎ󂯎��j
  BR_NET_WORK         *p_net;       //[in ]�ʐM�Ǘ�
  GAMEDATA            *p_gamedata;  //[in ]�Q�[���f�[�^
  BR_RECORD_DATA      *p_record;    //[in ]�^��풓�f�[�^
  BR_OUTLINE_DATA     *p_outline;   //[in ]�A�E�g���C���풓�f�[�^
  BOOL                is_btl_return;//[in ]�퓬����̖߂�
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
