//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		msgsearch.c
 *	@brief	MSG���當������������郂�W���[��
 *	@author	Toru=Nagihashi
 *	@data		2009.10.30
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include <gflib.h>
#include "ui/msgsearch.h"
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
#define MSGSEARCH_BUFFER_SIZE	(128)

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	MSG�f�[�^���當������������郏�[�N
//=====================================
struct _MSGSEARCH_WORK
{	
	STRBUF	*p_word;
	u32 msg_max;
	GFL_MSGDATA *p_msg_tbl[0];
};

//=============================================================================
/**
 *					PRIVATE
*/
//=============================================================================
static BOOL sub_strncmp( const STRBUF* str1, const STRBUF* str2, u32 len );

//=============================================================================
/**
 *					�O�����J
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	MSG����	������
 *
 *	@param	GFL_MSGDATA *p_msg_tbl	�������郁�b�Z�[�W�f�[�^�̃e�[�u��
 *	@param	msg_max									�������郁�b�Z�[�W�f�[�^�̐�
 *	@param	heapID									�q�[�vID 
 *
 *	@return	���[�N
 */
//-----------------------------------------------------------------------------
MSGSEARCH_WORK *MSGSEARCH_Init( GFL_MSGDATA *p_msg_tbl[], u32 msg_max, HEAPID heapID )
{	
	u32 size;
	MSGSEARCH_WORK	*p_wk;

	size	= sizeof(MSGSEARCH_WORK) + msg_max * sizeof(GFL_MSGDATA *);
	p_wk	= GFL_HEAP_AllocMemory( heapID, size );
	GFL_STD_MemClear( p_wk, size );

	p_wk->msg_max	= msg_max;
	{	
		int i;
		for( i = 0; i < msg_max; i++ )
		{	
			p_wk->p_msg_tbl[i]	= p_msg_tbl[i];
		}
	}

	p_wk->p_word		= GFL_STR_CreateBuffer( MSGSEARCH_BUFFER_SIZE, heapID );

	return	p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief	MSG����	�j��
 *
 *	@param	MSGSEARCH_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void MSGSEARCH_Exit( MSGSEARCH_WORK *p_wk )
{	
	GFL_STR_DeleteBuffer( p_wk->p_word );

	GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	MSG����	���b�Z�[�W�P�̂݌���
 *
 *	@param	MSGSEARCH_WORK *p_wk
 *	@param	�������郁�b�Z�[�W�̃C���f�b�N�X�iINIT�ł̓o�^�e�[�u���̃C���f�b�N�X�j
 *	@param	�����J�n����C���f�b�N�X				�i0�Ȃ�ΑS�������j
 *	@param	STRBUF *cp_buf		����������
 *	@param	*p_result_tbl			�������ʃe�[�u��
 *	@param	tbl_max						�������ʃe�[�u���̐�
 *
 *	@return	��������	�O�Ȃ�Ό�����Ȃ������B�@���Ȃ�΂����������ʂ��e�[�u���ɓ��ꂽ��
 */
//-----------------------------------------------------------------------------
u32 MSGSEARCH_Search( const MSGSEARCH_WORK *cp_wk, u16 msg_idx, u16 start_str_idx, const STRBUF *cp_buf, MSGSEARCH_RESULT *p_result_tbl, u32 tbl_max )
{	
	int match_cnt	= 0;
	u32 word_len	= GFL_STR_GetBufferLength( cp_buf );

	GFL_STD_MemFill32( p_result_tbl, -1, sizeof(MSGSEARCH_RESULT)*tbl_max );

	if( word_len )
	{	
		int search_idx;
		int result_idx;
		u32	str_max;
		GFL_MSGDATA	*p_msg;

		str_max = GFL_MSG_GetStrCount( cp_wk->p_msg_tbl[ msg_idx ] );
		p_msg		= cp_wk->p_msg_tbl[ msg_idx ];

		search_idx	= 0;
		result_idx	= 0;
		while( 1 )
		{	
			//�I������
			if( search_idx >= str_max )
			{	
				break;
			}
			if( result_idx >= tbl_max )
			{	
				break;
			}

			GFL_MSG_GetString( p_msg, search_idx, cp_wk->p_word );

      if( sub_strncmp( cp_buf, cp_wk->p_word, GFL_STR_GetBufferLength(cp_buf) ) )
			{
				if( result_idx < tbl_max )
				{	
					p_result_tbl[ result_idx ].msg_idx	= msg_idx;
					p_result_tbl[ result_idx ].str_idx	= search_idx;
					result_idx++;
				}
				match_cnt++;
      }
			search_idx++;
		}
	}

	return match_cnt;
}
//----------------------------------------------------------------------------
/**
 *	@brief	MSG����	���ׂẴ��b�Z�[�W����������
 *
 *	@param	MSGSEARCH_WORK *p_wk
 *	@param	STRBUF *cp_buf		����������
 *	@param	*p_result_tbl			�������ʃe�[�u��
 *	@param	tbl_max						�������ʃe�[�u���̐�
 *
 *	@return	��������	�O�Ȃ�Ό�����Ȃ������B�@���Ȃ�΂����������ʂ��e�[�u���ɓ��ꂽ��
 */
//-----------------------------------------------------------------------------
u32 MSGSEARCH_SearchAll( const MSGSEARCH_WORK *cp_wk, const STRBUF *cp_buf, MSGSEARCH_RESULT *p_result_tbl, u32 tbl_max )
{	
	MSGSEARCH_RESULT *p_result;
	u32 tbl_use;
	s32 tbl_rest;
	u32 match_cnt;
	int i;

	match_cnt	= 0;
	tbl_use		= 0;
	tbl_rest	= tbl_max;

	GFL_STD_MemFill32( p_result_tbl, -1, sizeof(MSGSEARCH_RESULT)*tbl_max );

	for( i = 0; i < cp_wk->msg_max; i++ )
	{	
		//�e�[�u���g�p�ʌv�Z
		tbl_rest	-= tbl_use;

		//�����c�ʂ�0�ȉ��Ȃ�ΏI��
		if( tbl_rest <= 0 )
		{	
			break;
		}
		//�g�p�e�[�u���擪�|�C���^
		p_result	= &p_result_tbl[ match_cnt ];

		//�T�[�`
		tbl_use		= MSGSEARCH_Search( cp_wk, i, 0, cp_buf, p_result, tbl_rest );

		//�T�[�`�����ʂ��������ɒǉ�
		match_cnt	+= tbl_use;
	}

  return match_cnt;	
}
//----------------------------------------------------------------------------
/**
 *	@brief	MSG����	�����݂��邩����
 *							��LSearch SearchAll�֐��̖߂�l�Ƃ̈Ⴂ�́A
 *							������͑S���������Đ���Ԃ��̂ɑ΂��A
 *							��L�֐��́A�e�[�u�������ꂽ�炷���Ɍ����I�����Ă��܂��_
 *
 *	@param	MSGSEARCH_WORK *p_wk
 *	@param	STRBUF *cp_buf		����������
 *
 *	@return	��������	����������
 */
//-----------------------------------------------------------------------------
u32 MSGSEARCH_SearchNum( const MSGSEARCH_WORK *cp_wk, const STRBUF *cp_buf )
{	
	int i, j;
	u32 match_cnt;

	match_cnt	= 0;
	for( i = 0; i < cp_wk->msg_max; i++ )
	{	
		u32 str_max;
		GFL_MSGDATA	*p_msg;

		p_msg	= cp_wk->p_msg_tbl[ i ];
		str_max	= GFL_MSG_GetStrCount( p_msg );
		for( j = 0; j < str_max; j++ )
		{	
			GFL_MSG_GetString( p_msg, j, cp_wk->p_word );
			if( sub_strncmp( cp_buf, cp_wk->p_word, GFL_STR_GetBufferLength(cp_buf) ) )
			{	
				match_cnt++;
			}
		}
	}

	return match_cnt;
}
//----------------------------------------------------------------------------
/**
 *	@brief	MSG����	�������ʂ��當������쐬����
 *
 *	@param	const MSGSEARCH_WORK *cp_wk				���[�N
 *	@param	MSGSEARCH_RESULT *cp_result				��������
 *
 *	@return	STRBUF
 */
//-----------------------------------------------------------------------------
STRBUF *MSGSEARCH_CreateString( const MSGSEARCH_WORK *cp_wk, const MSGSEARCH_RESULT *cp_result )
{	
	GF_ASSERT_MSG( 0 <= cp_result_tbl->msg_idx && cp_result_tbl->msg_idx < cp_wk->msg_max, "�o�^���ꂽ���b�Z�[�W�e�[�u�����I�[�o�[���Ă��܂��@%d < %d", cp_result_tbl->msg_idx, cp_wk->msg_max );
	return	GFL_MSG_CreateString( cp_wk->p_msg_tbl[ cp_result_tbl->msg_idx ], cp_result_tbl->str_idx );
}

//----------------------------------------------------------------------------
/**
 *	@brief	MSG����	�������ʂ��當������󂯎��
 *
 *	@param	const MSGSEARCH_WORK *cp_wk				���[�N
 *	@param	MSGSEARCH_RESULT *cp_result				��������
 *	@param	*p_str														������󂯎��o�b�t�@
 */
//-----------------------------------------------------------------------------
void MSGSEARCH_GetString( const MSGSEARCH_WORK *cp_wk, const MSGSEARCH_RESULT *cp_result, STRBUF *p_str )
{	
	GF_ASSERT_MSG( 0 <= cp_result_tbl->msg_idx && cp_result_tbl->msg_idx < cp_wk->msg_max, "�o�^���ꂽ���b�Z�[�W�e�[�u�����I�[�o�[���Ă��܂��@%d < %d", cp_result_tbl->msg_idx, cp_wk->msg_max );
	GFL_MSG_GetString( cp_wk->p_msg_tbl[ cp_result_tbl->msg_idx ], cp_result_tbl->str_idx, p_str );
}
//=============================================================================
/**
 *			PRIVATE
 */
//=============================================================================

//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	const STRBUF* str1	����������
 *	@param	STRBUF* str2				������������
 *	@param	len									������
 *
 *	@return	TRUE�Ȃ�Έ�v			�����Ȃ��Εs��v
 */
//-----------------------------------------------------------------------------
static BOOL sub_strncmp( const STRBUF* str1, const STRBUF* str2, u32 len )
{
	if( GFL_STR_GetBufferLength(str1) < len ){
		return FALSE;
	}
	if( GFL_STR_GetBufferLength(str2) < len ){
		return FALSE;
	}

	{
		const STRCODE *p1 = GFL_STR_GetStringCodePointer( str1 );
		const STRCODE *p2 = GFL_STR_GetStringCodePointer( str2 );
		u32 i;
		for(i=0; i<len; ++i){
			if( *p1++ != *p2++ ){ return FALSE; }
		}
		return TRUE;
	}
	return FALSE;
}

