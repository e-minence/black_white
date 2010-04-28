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
#include <gflib.h>

#include "print/str_tool.h"
#include "system/prof_word.h"
#include "arc_def.h"
#include "prof_word.naix"

//======================================================================
//	define
//======================================================================
#pragma mark [> define

#define PROF_WORD_LEN (32)
#define PROF_WORD_RAND_KEY (0x72012891)

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum


//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
static const BOOL PROF_WORD_CheckFile( STRCODE *code , u32 datId , const HEAPID heapId );
static const BOOL PROF_WORD_CheckWord( STRCODE *code , STRCODE *word );

//--------------------------------------------------------------
//  �s��������`�F�b�N
//  TRUE �s�� FALSE �L��
//--------------------------------------------------------------
const BOOL PROF_WORD_CheckProfanityWord( const STRBUF *strBuf , const HEAPID heapId )
{
  return PROF_WORD_CheckProfanityWordCode( GFL_STR_GetStringCodePointer(strBuf) , heapId );
}

const BOOL PROF_WORD_CheckProfanityWordCode( const STRCODE *strCode , const HEAPID heapId )
{
  int i;
  BOOL ret = FALSE;
  STRCODE code[PROF_WORD_LEN];
  
  GFL_STD_MemCopy( strCode , code , PROF_WORD_LEN*2 );

  //�`�F�b�N���镶����̕ϊ�
  for( i=0;i<PROF_WORD_LEN;i++ )
  {
    if( code[i] >= L'�@' && code[i] <= L'��' )
    {
      code[i] = code[i] - L'�@' + L'��';
    }
    else
    if( code[i] >= L'a' && code[i] <= L'z' )
    {
      code[i] = code[i] - L'a' + L'A';
    }
    else
    if( code[i] >= L'��' && code[i] <= L'z' )
    {
      code[i] = code[i] - L'��' + L'A';
    }
    else
    if( code[i] >= L'�`' && code[i] <= L'z' )
    {
      code[i] = code[i] - L'�`' + L'A';
    }
    else  //���[���b�p�n�̓��ꕶ��
    if( code[i] >= 0xe0 && code[i] <= 0xfe )
    {
      code[i] = code[i] - 0xe0 + 0xc0;
    }
    
    //����ȕ���(�A���t�@�x�b�g26�����ȊO)�͑啶���ϊ����Ă܂���I
  }
  
  for( i=0;i<GFL_ARC_GetDataFileCnt(ARCID_PROF_WORD);i++ )
  {
    ret = PROF_WORD_CheckFile( code , i , heapId );
    if( ret == TRUE )
    {
      break;
    }
  }
  return ret;
}

static const BOOL PROF_WORD_CheckFile( STRCODE *code , u32 datId , const HEAPID heapId )
{
  //�t�@�C���P�ʂŃ`�F�b�N
  BOOL ret;
  int i;
  u16 wordNum;
  u32 fileSize;
  STRCODE *buf = (STRCODE*)GFL_ARC_UTIL_LoadEx( ARCID_PROF_WORD , datId , FALSE , heapId , &fileSize );
  
  GFL_STD_CODED_Decoded( buf , fileSize , PROF_WORD_RAND_KEY );
  wordNum = fileSize/(PROF_WORD_LEN*2);
  
  for( i=0;i<wordNum;i++ )
  {
    ret = PROF_WORD_CheckWord( code , &buf[i*PROF_WORD_LEN] );
    if( ret == TRUE )
    {
      break;
    }
  }
  
  GFL_HEAP_FreeMemory( buf );
  
  return ret;
}

static const BOOL PROF_WORD_CheckWord( STRCODE *code , STRCODE *word )
{
  int i;
  //���S��v�`�F�b�N�̂��ߓƎ����[�`��
  for( i=0;i<PROF_WORD_LEN;i++ )
  {
    if( code[i] != word[i] )
    {
      return FALSE;
    }
    else
    if( code[i] == GFL_STR_GetEOMCode() &&
        word[i] == GFL_STR_GetEOMCode() )
    {
      return TRUE;
    }
  }
  return FALSE;
}
