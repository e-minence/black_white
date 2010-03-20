//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		dwc_tool.h
 *	@brief  WIFI�֌W�̋��ʃ��W���[���w�b�_
 *	@author	Toru=Nagihashi
 *	@date		2010.03.20
 *
 *  �I�[�o�[���C�𖳎����邽�߁Astatic inline��`�ł�
 *
 *  ����̃c�[���͈ȉ��ł�
 *  �E�s�������`�F�b�N    [DWC_TOOL_BADWORD]
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
//	���C�u����
#include <gflib.h>
#include <dwc.h>

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_
//
//    �s�������`�F�b�N  ���ʏ���
//
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_
//=============================================================================
/**
 *					�\����
*/
//=============================================================================
#define DWC_TOOL_BADWORD_STRL_MAX  (10) //�b��łP�O�����܂łł�
//-------------------------------------
///	�s���������[�N
//=====================================
typedef struct 
{
  STRCODE   badword_str[ DWC_TOOL_BADWORD_STRL_MAX ];
  u16       *p_badword_arry[1];
  char      badword_result[1];
  int       badword_num;
} DWC_TOOL_BADWORD_WORK;


//=============================================================================
/**
 *					�֐�
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �s�������`�F�b�N���X�^�[�g
 *
 *	@param	DWC_TOOL_BADWORD_WORK *p_wk  ���[�N
 *	@param	STRCODE *cp_str                 �`�F�b�N����STRCODE
 *	@param	str_len                         STRCODE�̒���
 */
//-----------------------------------------------------------------------------
static inline void DWC_TOOL_BADWORD_Start( DWC_TOOL_BADWORD_WORK *p_wk, const STRCODE *cp_str, u32 str_len )
{ 
  BOOL ret;
/*
  # �����R�[�h��Unicode�i���g���G���f�B�A��UTF16�j���g�p���Ă��������B
  ����ȊO�̕����R�[�h���g�p���Ă���ꍇ�́AUnicode�ɕϊ����Ă��������B
  # �X�N���[���l�[����Unicode�y��IPL�t�H���g�ɂȂ��Ǝ��̕������g�p���Ă���ꍇ�́A�X�y�[�X�ɒu�������Ă��������B
  # �I�[��"\0\0"�iu16��0x0000�j�ł���K�v������܂��B
  # �z����̑S�Ă̕�����̍��v��501�����܂Łi�e������̏I�[���܂ށj�ɂ���K�v������܂��B
  # �z����̕������NULL���w�肷�邱�Ƃ͂ł��܂���B 
  */
  { 
    int i;
    GF_ASSERT( str_len < DWC_TOOL_BADWORD_STRL_MAX );
    for( i = 0; i < str_len; i++ )
    { 
      if( GFL_STR_GetEOMCode() == cp_str[i] )
      {
        p_wk->badword_str[i]  = 0x0000;
      }
      else
      { 
        p_wk->badword_str[i]  = cp_str[i];
      }
    }
    p_wk->p_badword_arry[0] = p_wk->badword_str;
  }
  p_wk->badword_num = 0;


  ret = DWC_CheckProfanityExAsync( (const u16 **)p_wk->p_badword_arry,
                             1,
                             NULL,
                             0,
                             p_wk->badword_result,
                             &p_wk->badword_num,
                             DWC_PROF_REGION_ALL );
  GF_ASSERT( ret );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �s�������`�F�b�N���C��
 *
 *	@param	DWC_TOOL_BADWORD_WORK *p_wk    ���[�N
 *	@param	*p_is_bad_word                    TRUE�Ȃ�Εs������  FALSE�Ȃ�ΐ��핶��
 *
 *	@return TRUE�����I��  FALSE������
 */
//-----------------------------------------------------------------------------
static inline BOOL DWC_TOOL_BADWORD_Wait( DWC_TOOL_BADWORD_WORK *p_wk, BOOL *p_is_bad_word )
{ 
  BOOL ret;
  ret = DWC_CheckProfanityProcess() == DWC_PROF_STATE_SUCCESS;

  if( ret == TRUE )
  { 
    *p_is_bad_word  = p_wk->badword_num;
  }
  return ret;
}
