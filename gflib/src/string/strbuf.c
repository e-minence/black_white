//============================================================================================
/**
 * @file	strbuf.c
 * @brief	�ėp������o�b�t�@�^�I�u�W�F�N�g
 */
//============================================================================================
#include "gflib.h"

//--------------------------------------------------------------
/**
 * �萔                                                      
 */
//--------------------------------------------------------------
#define STRBUF_MAGIC_NUMBER		(0xb6f8d2ec)
#define STRBUF_MAX_SIZE			(1024)

//--------------------------------------------------------------
/**
 * ������o�b�t�@�I�u�W�F�N�g�^��`
 */
//--------------------------------------------------------------
typedef struct _STRBUF {
	u16  size;			///< �z��T�C�Y
	u16  strlen;		///< �����񒷁iEOM���܂܂Ȃ��j
	u32  magicNumber;	///< ����ɏ������ς݂ł��邱�Ƃ��`�F�b�N���邽�߂̃i���o�[

	STRCODE  buffer[1];	///< �z��{��
}STRBUF;

/** �w�b�_�T�C�Y */
#define STRBUF_HEADER_SIZE	(sizeof(STRBUF) - sizeof(STRCODE))

//==============================================================================================
//------------------------------------------------------------------
/**
 * �w�肳�ꂽ�|�C���^��STRBUF�Ƃ��ėL���Ȃ��̂ł��邩�`�F�b�N
 *
 * @param   ptr		�|�C���^
 *
 * @retval  BOOL	TRUE�ŗL���ł���
 */
//------------------------------------------------------------------
BOOL
	GFL_STR_CheckBufferValid
		(const void* ptr)
{
	if( (ptr != NULL) && (((const STRBUF*)ptr)->magicNumber == STRBUF_MAGIC_NUMBER) ){
		return TRUE;
	} else {
		return FALSE;
	}
}


//------------------------------------------------------------------
/**
 * ������o�b�t�@�I�u�W�F�N�g�쐬
 *
 * @param   size		�i�[�ł��镶�����iEOM���܂ށj
 * @param   heapID		�쐬��q�[�vID
 *
 * @retval  STRBUF*		�I�u�W�F�N�g�ւ̃|�C���^
 */
//------------------------------------------------------------------
STRBUF*
	GFL_STR_CreateBuffer
		( u32 size, HEAPID heapID )
{
	STRBUF* strbuf;

	strbuf = GFL_HEAP_AllocMemory( heapID, STRBUF_HEADER_SIZE + sizeof(STRCODE)*size );

	strbuf->magicNumber = STRBUF_MAGIC_NUMBER;
	strbuf->size = size;
	strbuf->strlen = 0;
	strbuf->buffer[0] = EOM_;

	return strbuf;
}


//------------------------------------------------------------------
/**
 * ������o�b�t�@�I�u�W�F�N�g�폜
 *
 * @param   strbuf		�I�u�W�F�N�g�ւ̃|�C���^
 *
 */
//------------------------------------------------------------------
void
	GFL_STR_DeleteBuffer
		( STRBUF* strbuf )
{
	GF_ASSERT( GFL_STR_CheckBufferValid( strbuf ) == TRUE );

	// Free ����ɓ����A�h���X�ŌĂяo������MagicNumber�����̂܂܎c���Ă�̂�
	strbuf->magicNumber = 0;

	GFL_HEAP_FreeMemory( strbuf );
}


//------------------------------------------------------------------
/**
 * ���e���N���A���ċ󕶎���ɂ���
 * �iCreate ����̓N���A����Ă���̂ŌĂԕK�v�͖����j
 *
 * @param   strbuf		�I�u�W�F�N�g�ւ̃|�C���^
 *
 */
//------------------------------------------------------------------
void
	GFL_STR_ClearBuffer
		( STRBUF* strbuf )
{
	GF_ASSERT( GFL_STR_CheckBufferValid( strbuf ) == TRUE );

	strbuf->strlen = 0;
	strbuf->buffer[0] = EOM_;
}


//------------------------------------------------------------------
/**
 * �o�b�t�@�Ԃ̕�����R�s�[
 *
 * @param   dst		�R�s�[��o�b�t�@�I�u�W�F�N�g
 * @param   src		�R�s�[���o�b�t�@�I�u�W�F�N�g
 *
 */
//------------------------------------------------------------------
void
	GFL_STR_CopyBuffer
		( STRBUF* dst, const STRBUF* src )
{
	GF_ASSERT( GFL_STR_CheckBufferValid( dst ) == TRUE );
	GF_ASSERT( GFL_STR_CheckBufferValid( src ) == TRUE );

	if( dst->size > src->strlen )
	{
		GFL_STD_MemCopy( ( void* )src->buffer, dst->buffer,
						( src->strlen + EOM_CODESIZE ) * sizeof(STRCODE) );
		dst->strlen = src->strlen;
		return;
	}
	GF_ASSERT_MSG(0, "STRBUF overflow : dstsize=%d,  srclen=%d", dst->size, src->strlen);
}


//------------------------------------------------------------------
/**
 * �������e�̃o�b�t�@�𕡐�����
 *
 * @param   strbuf		�������̃o�b�t�@�I�u�W�F�N�g
 *
 * @retval  STRBUF*		�������ꂽ�o�b�t�@�I�u�W�F�N�g
 */
//------------------------------------------------------------------
STRBUF*
	GFL_STR_CreateCopyBuffer
		( const STRBUF* origin, HEAPID heapID )
{
	STRBUF* copy;

	GF_ASSERT( GFL_STR_CheckBufferValid( origin ) == TRUE );

	copy = GFL_STR_CreateBuffer( origin->strlen + EOM_CODESIZE, heapID );
	GFL_STR_CopyBuffer( copy, origin );

	return copy;
}


//------------------------------------------------------------------
/**
 * ��������r����
 *
 * @param   str1		������P
 * @param   str2		������Q
 *
 * @retval  BOOL		TRUE�ň�v
 */
//------------------------------------------------------------------
BOOL
	GFL_STR_CompareBuffer
		( const STRBUF* str1, const STRBUF* str2 )
{
	int i;

	GF_ASSERT( GFL_STR_CheckBufferValid( str1 ) == TRUE );
	GF_ASSERT( GFL_STR_CheckBufferValid( str2 ) == TRUE );

	for(i=0; str1->buffer[i] == str2->buffer[i]; i++)
	{
		if( str1->buffer[i] == EOM_ )
		{
			return TRUE;
		}
	}
	return FALSE;
}


//------------------------------------------------------------------
/**
 * �i�[����Ă��镶������Ԃ�
 *
 * @param   strbuf	������o�b�t�@�I�u�W�F�N�g
 *
 * @retval  u32		������
 */
//------------------------------------------------------------------
u32
	GFL_STR_GetBufferLength
		( const STRBUF* str )
{
	GF_ASSERT( GFL_STR_CheckBufferValid( str ) == TRUE );

	return str->strlen;
}


//==============================================================================================
// ���̕����z�����舵���֐��Q
// �����𗘗p�ł��郂�W���[���͏o���������肷��
//==============================================================================================
//------------------------------------------------------------------
/**
 * ���̕�����iEOM�I�[�j���o�b�t�@�I�u�W�F�N�g�ɃZ�b�g����
 *
 * @param   strbuf		[out] �o�b�t�@�I�u�W�F�N�g�ւ̃|�C���^
 * @param   sz			[in]  EOM �ŏI��镶���z��
 *
 */
//------------------------------------------------------------------
void
	GFL_STR_SetStringCode
		( STRBUF* strbuf, const STRCODE* sz )
{
	GF_ASSERT( GFL_STR_CheckBufferValid( strbuf ) == TRUE );

	strbuf->strlen = 0;
	while( *sz != EOM_ )
	{
		if( strbuf->strlen >= (strbuf->size-1) )
		{
			GF_ASSERT_MSG( 0, "STRBUF overflow: busize=%d", strbuf->size);
			break;
		}
		strbuf->buffer[ strbuf->strlen++ ] = *sz++;
	}
	strbuf->buffer[strbuf->strlen] = EOM_;
}


//------------------------------------------------------------------
/**
 * ���̕�����i�����w��j���o�b�t�@�I�u�W�F�N�g�ɃZ�b�g����
 *
 * @param   strbuf		[out] �o�b�t�@�I�u�W�F�N�g�ւ̃|�C���^
 * @param   str			[in]  �����z��̐擪�|�C���^
 * @param   len			[in]  �Z�b�g���镶�����iEOM���܂ށj
 *
 */
//------------------------------------------------------------------
void
	GFL_STR_SetStringCodeOrderLength
		( STRBUF* strbuf, const STRCODE* str, u32 len )
{
	GF_ASSERT( GFL_STR_CheckBufferValid( strbuf ) == TRUE );

	if( len <= strbuf->size )
	{
		u32 i;

		GFL_STD_MemCopy( ( void* )str, strbuf->buffer, len*sizeof(STRCODE) );

		for(i=0; i<len; i++)
		{
			if(strbuf->buffer[i] == EOM_)
			{
				break;
			}
		}
		strbuf->strlen = i;

		// EOM�t������Ȃ������񂾂�����t���Ă���
		if( i==len )
		{
			strbuf->buffer[len-1] = EOM_;
		}
		return;
	}
	GF_ASSERT_MSG(0, "STRBUF overflow: bufsize=%d, strlen=%d", strbuf->size, len);
}


//------------------------------------------------------------------
/**
 * �o�b�t�@���琶�̕����z����R�s�[����
 *
 * @param   strbuf		[in]  �o�b�t�@�I�u�W�F�N�g�ւ̃|�C���^
 * @param   ary			[out] �R�s�[��z��
 * @param   arysize		[in]  �R�s�[��z��̗v�f��
 *
 */
//------------------------------------------------------------------
void
	GFL_STR_GetStringCode
		( const STRBUF* strbuf, STRCODE* ary, u32 arysize )
{
	GF_ASSERT( GFL_STR_CheckBufferValid( strbuf ) == TRUE );

	if( ( strbuf->strlen + EOM_CODESIZE ) <= arysize )
	{
		GFL_STD_MemCopy( ( void* )strbuf->buffer, ary,
						( strbuf->strlen + EOM_CODESIZE ) * sizeof( STRCODE ) );
		return;
	}

	GF_ASSERT_MSG(0, "STRBUF overflow: strlen=%d, arysize=%d", strbuf->strlen, arysize);
}


//------------------------------------------------------------------
/**
 * �o�b�t�@�I�u�W�F�N�g������镶���z��̃A�h���X��Ԃ�
 * �����̊֐����Ăяo���ӏ��͒�����B���Ԃ񕶎��o�͌n�̂݁B
 *
 * @param   strbuf				[in] �o�b�t�@�I�u�W�F�N�g�ւ̃|�C���^
 *
 * @retval  const STRCODE*		�����z��̃A�h���X
 */
//------------------------------------------------------------------
const STRCODE* 
	GFL_STR_GetStringCodePointer
		( const STRBUF* strbuf )
{
	GF_ASSERT( GFL_STR_CheckBufferValid( strbuf ) == TRUE );

	return strbuf->buffer;
}


//==============================================================================================
// ������̘A��������s���֐��B�y�y �����g�p�֎~ �z�z
// �g���ėǂ��̂̓��[�J���C�Y���l�����������񏈗����W���[���̂݁B
//==============================================================================================
//------------------------------------------------------------------
/**
 * ������A��
 *
 * @param   dst		�A�������o�b�t�@�I�u�W�F�N�g
 * @param   src		�A�����镶��������o�b�t�@�I�u�W�F�N�g
 *
 */
//------------------------------------------------------------------
void
	GFL_STR_AddString
		( STRBUF* dst, const STRBUF* src )
{
	GF_ASSERT( GFL_STR_CheckBufferValid( dst ) == TRUE );
	GF_ASSERT( GFL_STR_CheckBufferValid( src ) == TRUE );

	if( ( dst->strlen + src->strlen + EOM_CODESIZE ) <= dst->size )
		
	{
		GFL_STD_MemCopy( ( void* )src->buffer, &( dst->buffer[ dst->strlen ] ),
							( src->strlen + EOM_CODESIZE ) * sizeof( STRCODE ) );
		dst->strlen += src->strlen;
		return;
	}
	GF_ASSERT_MSG(0, "STRBUF overflow: bufsize=%d, src_len=%d, dst_len=%d",
					dst->size, src->strlen, dst->strlen);
}


//------------------------------------------------------------------
/**
 * �P�����A��
 *
 * @param   dst			�A�������o�b�t�@�I�u�W�F�N�g
 * @param   code		�A�����镶���R�[�h
 *
 */
//------------------------------------------------------------------
void
	GFL_STR_AddCode
		( STRBUF* dst, STRCODE code )
{
	GF_ASSERT( GFL_STR_CheckBufferValid( dst ) == TRUE );

	if( (dst->strlen + EOM_CODESIZE) < dst->size )
	{
		dst->buffer[dst->strlen++] = code;
		dst->buffer[dst->strlen] = EOM_;
		return;
	}
	GF_ASSERT_MSG(0, "STRBUF overflow: busize=%d", dst->size);
}


