//======================================================================
/**
 * @file	prof_word.h
 * @brief	�s�������񌟍�
 * @author	ariizumi
 * @data	100428
 *
 * ���W���[�����FPROF_WORD
 */
//======================================================================
#pragma once

typedef struct _PROF_WORD_WORK PROF_WORD_WORK;
//�C���g���p�A������
extern PROF_WORD_WORK* PROF_WORD_AllocWork( const HEAPID heapId );
extern void PROF_WORD_FreeWork( PROF_WORD_WORK* work );

//  �s��������`�F�b�N
//  TRUE �s�� FALSE �L��
//  work��NULL�ł������܂�(���Ńt�@�C����Alloc���܂�
extern const BOOL PROF_WORD_CheckProfanityWord( PROF_WORD_WORK *work , const STRBUF *strBuf , const HEAPID heapId );
extern const BOOL PROF_WORD_CheckProfanityWordCode( PROF_WORD_WORK *work , const STRCODE *strCode, const HEAPID heapId );

//�@�s�������`�F�b�N
//  TRUE �s�� FALSE �L��
extern const BOOL PROF_WORD_CheckProfanityNumber( const STRBUF *strBuf , const HEAPID heapId );
extern const BOOL PROF_WORD_CheckProfanityNumberCode( const STRCODE *strCode , const u16 str_len, const HEAPID heapId );

