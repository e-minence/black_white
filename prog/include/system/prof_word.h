//======================================================================
/**
 * @file	prof_word.c
 * @brief	�s�������񌟍�
 * @author	ariizumi
 * @data	100428
 *
 * ���W���[�����FPROF_WORD
 */
//======================================================================
#pragma once

//  �s��������`�F�b�N
//  TRUE �s�� FALSE �L��
extern const BOOL PROF_WORD_CheckProfanityWord( const STRBUF *strBuf , const HEAPID heapId );
extern const BOOL PROF_WORD_CheckProfanityWordCode( const STRCODE *code , const HEAPID heapId );

//�@�s�������`�F�b�N
//  TRUE �s�� FALSE �L��
extern const BOOL PROF_WORD_CheckProfanityNumber( const STRBUF *strBuf , const HEAPID heapId );
extern const BOOL PROF_WORD_CheckProfanityNumberCode( const STRCODE *strCode , const u16 str_len, const HEAPID heapId );
