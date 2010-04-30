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

#define PROF_WORD_DATA_NUM (7)
//======================================================================
//	enum
//======================================================================
#pragma mark [> enum


//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
struct _PROF_WORD_WORK
{
  void *data[PROF_WORD_DATA_NUM];
  u32  size[PROF_WORD_DATA_NUM];
};

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
static const BOOL PROF_WORD_CheckFile( STRCODE *code , u32 datId , const HEAPID heapId , void *data , u32 size );
static const BOOL PROF_WORD_CheckWord( STRCODE *code , STRCODE *word );

//--------------------------------------------------------------
//  �C���g���p������
//--------------------------------------------------------------
PROF_WORD_WORK* PROF_WORD_AllocWork( const HEAPID heapId )
{
  int i;
  PROF_WORD_WORK* work = GFL_HEAP_AllocClearMemory( heapId , sizeof(PROF_WORD_WORK) );

  GF_ASSERT( GFL_ARC_GetDataFileCnt(ARCID_PROF_WORD) == PROF_WORD_DATA_NUM );
  
  for( i=0;i<PROF_WORD_DATA_NUM;i++ )
  {
    work->data[i] = GFL_ARC_UTIL_LoadEx( ARCID_PROF_WORD , i , FALSE , heapId , &work->size[i] );
    GFL_STD_CODED_Decoded( work->data[i], work->size[i], PROF_WORD_RAND_KEY );
  }
  return work;
}

void PROF_WORD_FreeWork( PROF_WORD_WORK* work )
{
  int i;
  for( i=0;i<PROF_WORD_DATA_NUM;i++ )
  {
    GFL_HEAP_FreeMemory( work->data[i] );
  }
  GFL_HEAP_FreeMemory( work );
}

//--------------------------------------------------------------
//  �s��������`�F�b�N
//  TRUE �s�� FALSE �L��
//--------------------------------------------------------------
const BOOL PROF_WORD_CheckProfanityWord( PROF_WORD_WORK *work , const STRBUF *strBuf , const HEAPID heapId )
{
  return PROF_WORD_CheckProfanityWordCode( work , 
      GFL_STR_GetStringCodePointer(strBuf) ,
      heapId );
}

const BOOL PROF_WORD_CheckProfanityWordCode( PROF_WORD_WORK *work , const STRCODE *strCode , const HEAPID heapId )
{
  int i;
  BOOL ret = FALSE;
  STRCODE code[PROF_WORD_LEN] = {0};
  
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
  
  for( i=0;i<PROF_WORD_DATA_NUM;i++ )
  {
    void *data;
    u32  size;
    if( work == NULL )
    {
      data = (STRCODE*)GFL_ARC_UTIL_LoadEx( ARCID_PROF_WORD , i , FALSE , heapId , &size );
      GFL_STD_CODED_Decoded( data, size , PROF_WORD_RAND_KEY );
    }
    else
    {
      data = work->data[i];
      size = work->size[i];
    }
    
    ret = PROF_WORD_CheckFile( code , i , heapId , data , size );
    if( work == NULL )
    {
      GFL_HEAP_FreeMemory( data );
    }
    if( ret == TRUE )
    {
      break;
    }
  }
  return ret;
}

static const BOOL PROF_WORD_CheckFile( STRCODE *code , u32 datId , const HEAPID heapId , void *data , u32 size )
{
  //�t�@�C���P�ʂŃ`�F�b�N
  BOOL ret;
  int i;
  u16 wordNum;
  u32 fileSize = size;
  STRCODE *buf = (STRCODE*)data;
  
  wordNum = fileSize/(PROF_WORD_LEN*2);
  
  for( i=0;i<wordNum;i++ )
  {
    ret = PROF_WORD_CheckWord( code , &buf[i*PROF_WORD_LEN] );
    if( ret == TRUE )
    {
      break;
    }
  }
  
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

//----------------------------------------------------------------------------
/**
 *	@brief  �s�������`�F�b�N  �����̘A���͂S���܂�
 *
 *	@param	const STRBUF *strBuf  ������
 *	@param	HEAPID heapId         �q�[�vID
 *
 *	@return TRUE�Ȃ�Εs��  FALSE�Ȃ�ΐ���
 */
//-----------------------------------------------------------------------------
const BOOL PROF_WORD_CheckProfanityNumber( const STRBUF *strBuf , const HEAPID heapId )
{ 
  return PROF_WORD_CheckProfanityNumberCode( GFL_STR_GetStringCodePointer(strBuf), 
      GFL_STR_GetBufferLength( strBuf ),
      heapId );
}
const BOOL PROF_WORD_CheckProfanityNumberCode( const STRCODE *strCode , const u16 str_len, const HEAPID heapId )
{ 
  enum
  { 
    PROFANITY_NUMBER  = 4,
  };

  int i;
  BOOL ret = FALSE;
  STRCODE code[PROF_WORD_LEN] = {0};
  int cnt = 0;
  

  GFL_STD_MemCopy( strCode , code , str_len*2 );

  //�`�F�b�N���镶����̕ϊ�
  for( i=0;i<PROF_WORD_LEN;i++ )
  {
    if( (code[i] >= L'0' && code[i] <= L'9')
        || (code[i] >= L'�O' && code[i] <= L'�X') )
    {
      cnt++;
    }
    else
    { 
      cnt = 0;
    }

    if( cnt > PROFANITY_NUMBER )
    { 
      return TRUE;
    }
  }

  return FALSE;
}

