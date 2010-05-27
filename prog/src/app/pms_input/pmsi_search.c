//=============================================================================
/**
 *
 *	@file		pmsi_search.c
 *	@brief  �ȈՉ�b���� �������W���[��
 *	@author	hosaka genya
 *	@data		2009.11.14
 *
 */
//=============================================================================
#include <gflib.h>

#include "arc_def.h"
#include "system\main.h"
#include "system\pms_word.h"
#include "system\pms_data.h"

#include "ui/msgsearch.h"

#include "pmsi_initial_data.h"
#include "pms_input_prv.h"

#include "pmsi_search.h"

#include "pms_abc_gmm_def.h"


#include "../../../../resource/quiz/pms_answer.h"


//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================

enum
{ 
  WORD_CODE_MAX = PMS_INPUTWORD_MAX,  ///< ���͂ł���ő啶����
  WORD_SEARCH_MAX = PMS_ABC_GMMROW_MAX,
};  

//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================

//--------------------------------------------------------------
///	���C�����[�N
//==============================================================
struct _PMS_INPUT_SEARCH {
  // [IN]
  const PMS_INPUT_WORK* mwk;
  const PMS_INPUT_DATA* dwk;
  HEAPID heap_id;
  // [PRIVATE]
  GFL_MSGDATA* msg_tbl[ PMS_ABC_GMMTBL_MAX ];
  MSGSEARCH_WORK* search_wk;
  MSGSEARCH_RESULT search_result[ WORD_SEARCH_MAX ];
  PMS_WORD result_word[ WORD_SEARCH_MAX+1 ];
  STRBUF* str_search;
  u16 word_code[ WORD_CODE_MAX ];
  u16 padding_u16;
  u8 word_code_pos;
  u8 search_hitnum;
  u8 padding[2];
};

//=============================================================================
/**
 *							�v���g�^�C�v�錾
 */
//=============================================================================

#ifdef PM_DEBUG
#define DEBUG_PRINT
static void debug_word_code_print( PMS_INPUT_SEARCH* wk )
{ 
  int i;

  for( i=0; i<WORD_CODE_MAX; i++ )
  {
    HOSAKA_Printf("word_code:");
    HOSAKA_Printf("%d ", wk->word_code[i] );
    HOSAKA_Printf("\n");
  }
}
#endif



//=============================================================================
/**
 *								�O�����J�֐�
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  �������W���[�� ����
 *
 *	@param	const PMS_INPUT_WORK* mwk
 *	@param	const PMS_INPUT_DATA* dwk
 *	@param	heap_id 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
PMS_INPUT_SEARCH* PMSI_SEARCH_Create( const PMS_INPUT_WORK* mwk, const PMS_INPUT_DATA* dwk, HEAPID heap_id )
{
  int i;
  PMS_INPUT_SEARCH* wk;
  
  wk = GFL_HEAP_AllocClearMemory( heap_id, sizeof( PMS_INPUT_SEARCH ) );

  wk->mwk = mwk;
  wk->dwk = dwk;
  wk->heap_id =  heap_id;

  PMSI_SEARCH_ClearWord( wk );

  // GMM����
  for( i=0; i<PMS_ABC_GMMTBL_MAX; i++ )
  {
    wk->msg_tbl[i] = GFL_MSG_Create( GFL_MSG_LOAD_FAST, ARCID_MESSAGE, pms_abc_gmmtbl[i], heap_id );
  }

  wk->search_wk = MSGSEARCH_Init( wk->msg_tbl, PMS_ABC_GMMTBL_MAX, wk->heap_id );
  
  wk->str_search = GFL_STR_CreateBuffer( WORD_CODE_MAX*2+EOM_CODESIZE, heap_id );

  return wk;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �������W���[�� �폜
 *
 *	@param	PMS_INPUT_SEARCH* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void PMSI_SEARCH_Delete( PMS_INPUT_SEARCH* wk )
{ 
  int i;
  
  GFL_STR_DeleteBuffer( wk->str_search );
  
  MSGSEARCH_Exit( wk->search_wk );
  
  for( i=0; i<PMS_ABC_GMMTBL_MAX; i++ )
  {
    GFL_MSG_Delete( wk->msg_tbl[i] );
  }

  GFL_HEAP_FreeMemory( wk );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �ꕶ���o�^
 *
 *	@param	PMS_INPUT_SEARCH* wk
 *	@param	word_code 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void PMSI_SEARCH_AddWord( PMS_INPUT_SEARCH* wk, u16 word_code )
{
  GF_ASSERT(wk);
  GF_ASSERT( wk->word_code_pos < WORD_CODE_MAX );

  wk->word_code[ wk->word_code_pos ] = word_code;

  wk->word_code_pos = MATH_CLAMP( wk->word_code_pos+1, 0, WORD_CODE_MAX-1 );

#ifdef DEBUG_PRINT
  debug_word_code_print( wk );
#endif
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �ꕶ���폜
 *
 *	@param	PMS_INPUT_SEARCH* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
BOOL PMSI_SEARCH_DelWord( PMS_INPUT_SEARCH* wk )
{
  GF_ASSERT(wk);
  GF_ASSERT( wk->word_code_pos < WORD_CODE_MAX );

  // ���͂���Ă��Ȃ���Ή��������ɔ�����
  if( wk->word_code_pos == 0 )
  {
    return FALSE;
  }

  // ��������Ȃ�1�����O��
  if( wk->word_code[ WORD_CODE_MAX-1 ] == INI_DIS )
  {
    wk->word_code_pos--;
  }
  
  wk->word_code[ wk->word_code_pos ] = INI_DIS;

#ifdef DEBUG_PRINT
  debug_word_code_print( wk );
#endif
  
  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ���͂���Ă��镶�����𓾂�
 *
 *	@param	const PMS_INPUT_SEARCH* wk 
 *
 *	@retval ���͂���Ă��镶����
 */
//-----------------------------------------------------------------------------
u8 PMSI_SEARCH_GetCharNum( const PMS_INPUT_SEARCH* wk )
{
  return wk->word_code_pos;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ���͂��ꂽ���������N���A
 *
 *	@param	PMS_INPUT_SEARCH* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void PMSI_SEARCH_ClearWord( PMS_INPUT_SEARCH* wk )
{
  int i;

  // ���͏�񏉊���
  for( i=0; i<WORD_CODE_MAX; i++ )
  {
    wk->word_code[i] = INI_DIS;
  }

  wk->word_code_pos = 0;
  
  // �������ʂ�0�ɂ��Ă��� 
  wk->search_hitnum = 0;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ���͂��ꂽ�������out_buf�ɃZ�b�g
 *
 *	@param	PMSI_INPUT_SEARCH* wk
 *	@param	out_buf 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void PMSI_SEARCH_GetInputWord( PMS_INPUT_SEARCH* wk, STRBUF* out_buf )
{
  int i;
  STRCODE code[ WORD_CODE_MAX + 1 ];

  for( i=0; i<WORD_CODE_MAX; i++ )
  {
    if( wk->word_code[i] == INI_DIS )
    {
      // �I�������ǉ�
      code[i] = GFL_STR_GetEOMCode();
      break;
    }

    code[i] = PMSI_INITIAL_DAT_GetStrCode( wk->word_code[i] );
  }
  
  // �Ō�ɏI�������ǉ�
  code[ WORD_CODE_MAX ] = GFL_STR_GetEOMCode();
  
  // ������Z�b�g
  GFL_STR_SetStringCode( out_buf, code );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �������������ɉ�
 *
 *	@param	void* elem1
 *	@param	elem2 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static s32 qsort_result( void* elem1, void* elem2 )
{
  MSGSEARCH_RESULT* p1 = (MSGSEARCH_RESULT*) elem1;
  MSGSEARCH_RESULT* p2 = (MSGSEARCH_RESULT*) elem2;
  
  // ������
  if( p1->str_idx  == p2->str_idx )
  {
    return 0;
  }
  // �~��
  else if( p1->str_idx > p2->str_idx )
  {
    return 1;
  }
  else
  {
    return -1;
  }


  GF_ASSERT(0);
    
  return 0;

}


//-----------------------------------------------------------------------------
/**
 *	@brief  �����J�n
 *
 *	@param	PMSI_INPUT_SEARCH*wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
BOOL PMSI_SEARCH_Start( PMS_INPUT_SEARCH*wk )
{
  int i;
  u16 search_idx;
  
  // 1�����ڂŌ����Ώۂ�GMM��ύX
  search_idx = wk->word_code[0];
  HOSAKA_Printf("search idx =%d\n", search_idx );
 
  // ���������Ȃ猟�����Ȃ�
  if( search_idx >= INI_SEARCH_MAX )
  {
    wk->search_hitnum = 0; // ���ʂ�0��
    return FALSE;
  }

  // ��������
  GFL_STR_ClearBuffer( wk->str_search );
  PMSI_SEARCH_GetInputWord( wk, wk->str_search );

#if 0
  {
    const STRCODE *p1 = GFL_STR_GetStringCodePointer( wk->str_search );
    
    // �������ꂽ������\��
    while( *p1 != GFL_STR_GetEOMCode() )
    {
      HOSAKA_Printf("---->0x%x \n", *p1 );
      p1++;
    }
  }
#endif

  // 1�����ڂŉp�����̌������ʏ�̌��������򂷂�
  // �p�����̌���
  if( search_idx == INI_OTHER )
  {
    static const PMS_WORD pic_word[] =
    {
      pic01,
      pic02,
      pic03,
      pic04,
      pic05,
      pic06,
      pic07,
      pic08,
      pic09,
      pic10,
    };

    u16          msg_idx    = search_idx;
    GFL_MSGDATA* p_msg      = wk->msg_tbl[ msg_idx ];
    u32          str_max    = GFL_MSG_GetStrCount( p_msg );
    int          str_idx;
    u32          hitnum                        = 0;
    PMS_WORD     rslt_tbl[ WORD_SEARCH_MAX+1 ] = {0};

    for( str_idx=0; str_idx<str_max; str_idx++ )
    {
      PMS_WORD word = PMSI_DATA_GetWordToOriginalPos( wk->dwk, search_idx, str_idx );
      int j;
      BOOL is_pic_word = FALSE;
      for( j=0; j<sizeof(pic_word)/sizeof(pic_word[0]); j++ )
      {
        if( word == pic_word[j] )
        {
          is_pic_word = TRUE;
          break;
        }
      }
      if( !is_pic_word )
      {
        wk->search_result[hitnum].msg_idx = search_idx;
        wk->search_result[hitnum].str_idx = str_idx;
        rslt_tbl[hitnum] = word;
        hitnum++;
      }
    }

    // �e�[�u�������ɖ����f�[�^
    rslt_tbl[hitnum] = PMSI_DATA_GetWordTableEndData( wk->dwk );
 
    // �����f�[�^��r��
    wk->search_hitnum = PMSI_DATA_GetInitialEnableWordTable( wk->dwk, rslt_tbl, wk->result_word );
    
    return ( wk->search_hitnum > 0 );
  }
  // �ʏ�̌���
  else
  {
    int i;
    u32 hitnum;
    PMS_WORD rslt_tbl[ WORD_SEARCH_MAX+1 ] = {0};

    hitnum = MSGSEARCH_Search( wk->search_wk, search_idx, 0, wk->str_search, wk->search_result ,WORD_SEARCH_MAX );
    
    HOSAKA_Printf("search hitnum = %d \n\n", hitnum);

    // �������ʂ���PMS_WORD�𐶐�
    for( i=0; i<hitnum; i++ )
    {
      rslt_tbl[i] = PMSI_DATA_GetWordToOriginalPos( wk->dwk, search_idx, wk->search_result[i].str_idx );
      HOSAKA_Printf("[%d] word=%d str_idx=%d \n",i, rslt_tbl[i], wk->search_result[i].str_idx );
    }

    // �e�[�u�������ɖ����f�[�^
    rslt_tbl[hitnum] = PMSI_DATA_GetWordTableEndData( wk->dwk );

    // �����f�[�^��r��
    wk->search_hitnum = PMSI_DATA_GetInitialEnableWordTable( wk->dwk, rslt_tbl, wk->result_word );
    
    HOSAKA_Printf("search enable hitnum = %d \n", wk->search_hitnum );
  
    return wk->search_hitnum > 0;
  }

  return FALSE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �����q�b�g����Ԃ�
 *
 *	@param	PMS_INPUT_SEARCH* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
u16 PMSI_SEARCH_GetResultCount( PMS_INPUT_SEARCH* wk )
{
  GF_ASSERT( wk );

  return wk->search_hitnum;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �������ʂ��當����𐶐�
 *
 *	@param	PMS_INPUT_SEARCH* wk
 *	@param	result_idx  
 *	@param  dst_buf
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void PMSI_SEARCH_GetResultString( PMS_INPUT_SEARCH* wk, u8 result_idx, STRBUF* dst_buf )
{
  u32 word_idx;

  GF_ASSERT( wk );
  GF_ASSERT( result_idx <  wk->search_hitnum );
  GF_ASSERT( wk->word_code[0] != INI_DIS );

  word_idx = wk->result_word[ result_idx ];

  HOSAKA_Printf("getresult result_idx=%d word_idx=%d \n", result_idx, word_idx );

  PMSI_DATA_GetWordString( wk->dwk, word_idx, dst_buf );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �������ʂ̃C���f�b�N�X���� PMS_WORD ���擾
 *
 *	@param	PMS_INPUT_SEARCH* wk
 *	@param	cur_pos
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
PMS_WORD PMSI_SEARCH_GetWordCode( PMS_INPUT_SEARCH* wk, u32 cur_pos )
{
  GF_ASSERT( wk );
  GF_ASSERT( cur_pos <  wk->search_hitnum );
  GF_ASSERT( wk->word_code[0] != INI_DIS );

  HOSAKA_Printf("getwordcode cur_pos=%d word=%d \n", cur_pos, wk->result_word[cur_pos] );

  return wk->result_word[ cur_pos ];
}

//=============================================================================
/**
 *								static�֐�
 */
//=============================================================================


