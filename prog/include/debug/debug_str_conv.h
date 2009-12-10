//======================================================================
/**
 * @file	debug_str_conv.h
 * @brief	SJIS��unicode�̑��ݕϊ�
 * @author	ariizumi
 * @data	09/12/01
 *
 * ���W���[�����FDEB_STR_CONV
 */
//======================================================================
#pragma once
//--------------------------------------------------------------
//	char�̕������STRCODE(u16)�ɕϊ�
//
//  @param str1 char�^������
//  @param str2 STRCODE�^������
//  @param str1�̕ϊ����s������(byte�P��
//--------------------------------------------------------------
extern void DEB_STR_CONV_SjisToStrcode( const char* str1 , STRCODE *str2 , const u32 len );

//--------------------------------------------------------------
//	STRCODE(u16)�̕������char�ɕϊ�
//
//  @param str1 STRCODE�^������
//  @param str2 char�^������
//  @param str1�̕ϊ����s������(������
//--------------------------------------------------------------
extern void DEB_STR_CONV_StrcodeToSjis( const STRCODE *str1 , char *str2 , const u32 len );
extern void DEB_STR_CONV_StrBufferToSjis( const STRBUF *str1 , char *str2 , const u32 len );


