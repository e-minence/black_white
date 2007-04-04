//==============================================================================
/**
 *
 *@file		sjisconv_inter.h
 *@brief	������ϊ�
 *
 */
//==============================================================================
#ifndef __SJISCONV_H__
#define __SJISCONV_H__

#include "strbuf.h"

//------------------------------------------------------------------
/**
 * ������R���o�[�g�i Sjis > SystemFontCode �j
 *
 * @param   src				�ϊ��OSjis������|�C���^
 * @param   dst				�ϊ���STRCODE������|�C���^
 * @param   dstcodesize		STRCODE������ő�T�C�Y�i�I�[�o�[�t���[���p�j
 *
 */
//------------------------------------------------------------------
extern void GFL_STR_CONV_Sjis2Systemfontcode( const char* src, STRCODE* dst, int dstcodesCount );

#endif


