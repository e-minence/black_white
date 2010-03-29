//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_bvrank_proc.h
 *	@brief	�o�g���r�f�I�����N�v���Z�X
 *	@author	Toru=Nagihashi
 *	@date		2009.11.13
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
#include "br_proc_sys.h"
#include "br_res.h"
#include "br_fade.h"
#include "br_net.h"
#include "br_data.h"
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�����L���O���[�h
//=====================================
typedef enum
{
  BR_BVRANK_MODE_NEW,       //�ŐV�R�O��
  BR_BVRANK_MODE_FAVORITE,  //�ʐM�ΐ탉���L���O
  BR_BVRANK_MODE_SUBWAY,    //�T�u�E�F�C
  BR_BVRANK_MODE_SEARCH,    //��������

  BR_BVRANK_MODE_RETURN,    //���R�[�h��ʂ���̖߂�
} BR_BVRANK_MODE;



//=============================================================================
/**
 *					�\����
*/
//=============================================================================
//-------------------------------------
///	�o�g���r�f�I�����N�v���Z�X����
//=====================================
typedef struct 
{
  BR_BVRANK_MODE  mode;         //[in ]���[�h
  BR_FADE_WORK    *p_fade;      //[in ]�t�F�[�h
	GFL_CLUNIT			*p_unit;			//[in ]���j�b�g
	BR_RES_WORK			*p_res;				//[in ]���\�[�X�Ǘ�
	BR_PROC_SYS			*p_procsys;		//[in ]�v���Z�X�Ǘ�
  BR_NET_WORK     *p_net;       //[in ]�l�b�g�Ǘ�
  BR_OUTLINE_DATA *p_outline;   //[in ]�풓�f�[�^
  BR_NET_VIDEO_SEARCH_DATA  search_data;  //[in]�������ځimode��BR_BVRANK_MODE_SEARCH�̂Ƃ������j
} BR_BVRANK_PROC_PARAM;


//=============================================================================
/**
 *					�O�����J�֐�
*/
//=============================================================================
//-------------------------------------
///	�o�g���r�f�I�����N�v���Z�X�f�[�^
//=====================================
extern const GFL_PROC_DATA	BR_BVRANK_ProcData;
