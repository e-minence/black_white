//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		namein_data.c
 *	@brief	���O���͗p�f�[�^
 *	@author	Toru=Nagihashi
 *	@data		2009.10.08
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>

//�V�X�e��
#include "print/str_tool.h"

//�A�[�J�C�u
#include "arc_def.h"
#include "namein_data.naix"

//�O��
#include "namein_data.h"
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�ϊ��p�f�[�^
//=====================================
//���݂��Ȃ��ꍇ�����Ă���l�inamein_change_conv.pl���ŃX�y�[�X������Ă���j
#define NAMEIN_STRCHANGE_EMPTY	(L'�@')

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	���O���̓f�[�^
//=====================================
struct _NAMEIN_KEYMAP
{
	u16			w;
	u16			h;
	STRCODE	str[0];
} ;

//-------------------------------------
///	�ϊ��p�f�[�^
//=====================================
typedef struct
{	
	STRCODE	input[NAMEIN_STRCHANGE_CODE_LEN];
	STRCODE	chage[NAMEIN_STRCHANGE_CODE_LEN];
	STRCODE	shift[NAMEIN_STRCHANGE_CODE_LEN];
	STRCODE	rest[NAMEIN_STRCHANGE_CODE_LEN];
}CHANGECODE;
struct _NAMEIN_STRCHANGE
{
	u16					num;
	CHANGECODE	word[0];
} ;

//-------------------------------------
///	�n���h��
//=====================================
struct _NAMEIN_KEYMAP_HANDLE
{ 
  NAMEIN_KEYMAP *p_data[ NAMEIN_KEYMAPTYPE_MAX ];
};


//=============================================================================
/**
 *	�v���g�^�C�v�錾
 */
//=============================================================================
static BOOL STRCODE_Search( const STRCODE* data, const STRCODE* code );

//=============================================================================
/**
 *				�L�[�z��
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�f�[�^�ǂ݂���
 *
 *	@param	NAMEIN_KEYMAP type	�^�C�v
 *	@param	heapID						�q�[�vID
 *
 *	@return	�f�[�^�n���h��
 */
//-----------------------------------------------------------------------------
NAMEIN_KEYMAP *NAMEIN_KEYMAP_Alloc( NAMEIN_KEYMAPTYPE type, HEAPID heapID )
{	
	NAMEIN_KEYMAP	*p_wk;

	GF_ASSERT( type < NAMEIN_KEYMAPTYPE_MAX );

	//narc�̃C���f�b�N�X���^�C�v���ɂȂ��Ă���̂ŁA
	//���̂܂ܓǂݍ���
	p_wk	= GFL_ARC_UTIL_Load( ARCID_NAMEIN_DATA, NARC_namein_data_keymap_kana_dat + type, FALSE, heapID );

	return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�f�[�^�j��
 *
 *	@param	NAMEIN_KEYMAP *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void NAMEIN_KEYMAP_Free( NAMEIN_KEYMAP *p_wk )
{	
	GFL_HEAP_FreeMemory( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��������ɑS�f�[�^���������߂̃n���h���쐬
 *
 *	@param	HEAPID heapID   �q�[�vID
 *
 *	@return KEYMAP�����ׂă�������ɂ������߂̃��[�N
 */
//-----------------------------------------------------------------------------
NAMEIN_KEYMAP_HANDLE * NAMEIN_KEYMAP_HANDLE_Alloc( HEAPID heapID )
{ 
  NAMEIN_KEYMAP_HANDLE *p_wk;
  p_wk= GFL_HEAP_AllocMemory( heapID, sizeof(NAMEIN_KEYMAP_HANDLE) );
  GFL_STD_MemClear( p_wk, sizeof(NAMEIN_KEYMAP_HANDLE) );

  {
    int i;
    for( i = 0; i < NAMEIN_KEYMAPTYPE_MAX; i++ )
    { 
      p_wk->p_data[i] = NAMEIN_KEYMAP_Alloc( i, heapID );
    }
  }

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �n���h���j��
 *
 *	@param	NAMEIN_KEYMAP_HANDLE *p_handle�n���h��
 */
//-----------------------------------------------------------------------------
void NAMEIN_KEYMAP_HANDLE_Free( NAMEIN_KEYMAP_HANDLE *p_handle )
{ 
  {
    int i;
    for( i = 0; i < NAMEIN_KEYMAPTYPE_MAX; i++ )
    { 
      NAMEIN_KEYMAP_Free( p_handle->p_data[i] );
    }
  }

  GFL_HEAP_FreeMemory( p_handle );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �n���h������f�[�^���擾
 *
 *	@param	NAMEIN_KEYMAP_HANDLE *p_handle �n���h��
 */
//-----------------------------------------------------------------------------
NAMEIN_KEYMAP *NAMEIN_KEYMAP_HANDLE_GetData( NAMEIN_KEYMAP_HANDLE *p_handle, NAMEIN_KEYMAPTYPE type )
{ 
  return p_handle->p_data[ type ];
}


//----------------------------------------------------------------------------
/**
 *	@brief	�f�[�^�����擾
 *
 *	@param	const NAMEIN_KEYMAP *cp_wk	���[�N
 *
 *	@return	��
 */
//-----------------------------------------------------------------------------
u16 NAMEIN_KEYMAP_GetWidth( const NAMEIN_KEYMAP *cp_wk )
{	
	return cp_wk->w;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�f�[�^�������擾
 *
 *	@param	const NAMEIN_KEYMAP *cp_wk	���[�N
 *
 *	@return	����
 */
//-----------------------------------------------------------------------------
u16 NAMEIN_KEYMAP_GetHeight( const NAMEIN_KEYMAP *cp_wk )
{	
	return cp_wk->h;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�w��ʒu�̕������擾
 *
 *	@param	const NAMEIN_KEYMAP *cp_wk	���[�N
 *	@param	x	X�ʒu
 *	@param	y	Y�ʒu
 *
 *	@return	�������擾
 */
//-----------------------------------------------------------------------------
STRCODE NAMEIN_KEYMAP_GetStr( const NAMEIN_KEYMAP *cp_wk, u16 x, u16 y )
{	
	GF_ASSERT( x < NAMEIN_KEYMAP_GetWidth(cp_wk) );
	GF_ASSERT( y < NAMEIN_KEYMAP_GetHeight(cp_wk) );

	return cp_wk->str[  y * NAMEIN_KEYMAP_GetWidth( cp_wk ) + x ];
}
//=============================================================================
/**
 *				�ϊ�
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�f�[�^�ǂ݂���
 *
 *	@param	NAMEIN_STRCHANGE type	�^�C�v
 *	@param	heapID						�q�[�vID
 *
 *	@return	�f�[�^�n���h��
 */
//-----------------------------------------------------------------------------
NAMEIN_STRCHANGE *NAMEIN_STRCHANGE_Alloc( NAMEIN_STRCHANGETYPE type, HEAPID heapID )
{	
	NAMEIN_STRCHANGE	*p_wk;

	GF_ASSERT( type < NAMEIN_STRCHANGETYPE_MAX );

	//narc�̃C���f�b�N�X���^�C�v���ɂȂ��Ă���̂ŁA
	//���̂܂ܓǂݍ���
	p_wk	= GFL_ARC_UTIL_Load( ARCID_NAMEIN_DATA, NARC_namein_data_change_dakuten_dat + type, FALSE, heapID );

	return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�f�[�^�j��
 *
 *	@param	NAMEIN_STRCHANGE *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void NAMEIN_STRCHANGE_Free( NAMEIN_STRCHANGE *p_wk )
{	
	GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�f�[�^�����擾
 *
 *	@param	const NAMEIN_STRCHANGE *cp_wk	���[�N
 *
 *	@return	��
 */
//-----------------------------------------------------------------------------
u16 NAMEIN_STRCHANGE_GetNum( const NAMEIN_STRCHANGE *cp_wk )
{	
	return cp_wk->num;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ϊ��O������擾
 *
 *	@param	const NAMEIN_STRCHANGE *cp_wk	���[�N
 *	@param	idx				�C���f�b�N�X
 *	@param	*p_code		�󂯎��z��
 *	@param	code_len	�󂯎��z��v�f��
 *	@param	*p_len		���������邩
 */
//-----------------------------------------------------------------------------
void NAMEIN_STRCHANGE_GetInputStr( const NAMEIN_STRCHANGE *cp_wk, u16 idx, STRCODE *p_code, u8 code_len, u8 *p_len )
{	
	GF_ASSERT( idx < NAMEIN_STRCHANGE_GetNum(cp_wk) );
	STRTOOL_Copy( cp_wk->word[ idx ].input, p_code, code_len );	
	if( p_len )
	{	
		for( *p_len = 0; *p_len < NAMEIN_STRCHANGE_CODE_LEN && p_code[*p_len] != NAMEIN_STRCHANGE_EMPTY ; (*p_len)++ )
		{	};
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ϊ��㕶����擾
 *
 *	@param	const NAMEIN_STRCHANGE *cp_wk	���[�N
 *	@param	idx				�C���f�b�N�X
 *	@param	*p_code		�󂯎��z��
 *	@param	code_len	�󂯎��z��v�f��
 *	@param	*p_len		���������邩
 *	@return	TRUE�Ȃ�Α��݁AFALSE�Ȃ�Α��݂��Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL NAMEIN_STRCHANGE_GetChangeStr( const NAMEIN_STRCHANGE *cp_wk, u16 idx, STRCODE *p_code, u8 code_len, u8 *p_len )
{	
	u8	len;

	GF_ASSERT( idx < NAMEIN_STRCHANGE_GetNum(cp_wk) );
	STRTOOL_Copy( cp_wk->word[ idx ].chage, p_code, code_len );	
	for( len = 0; len < NAMEIN_STRCHANGE_CODE_LEN && p_code[len] != NAMEIN_STRCHANGE_EMPTY ; (len)++ )
	{	};
	if( p_len )
	{	
		*p_len = len;
	}
	return len != 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief	SHIFT�ϊ��㕶����擾
 *
 *	@param	const NAMEIN_STRCHANGE *cp_wk	���[�N
 *	@param	idx				�C���f�b�N�X
 *	@param	*p_code		�󂯎��z��
 *	@param	code_len	�󂯎��z��v�f��
 *	@param	*p_len		���������邩
 *	@return	TRUE�Ȃ�Α��݁AFALSE�Ȃ�Α��݂��Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL NAMEIN_STRCHANGE_GetChangeShiftStr( const NAMEIN_STRCHANGE *cp_wk, u16 idx, STRCODE *p_code, u8 code_len, u8 *p_len )
{	
	u8	len;

	GF_ASSERT( idx < NAMEIN_STRCHANGE_GetNum(cp_wk) );
	STRTOOL_Copy( cp_wk->word[ idx ].shift, p_code, code_len );	
	for( len = 0; len < NAMEIN_STRCHANGE_CODE_LEN && p_code[len] != NAMEIN_STRCHANGE_EMPTY ; (len)++ )
	{	};
	if( p_len )
	{	
		*p_len = len;
	}
	return len != 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ϊ���Ɏc�镶����擾
 *
 *	@param	const NAMEIN_STRCHANGE *cp_wk	���[�N
 *	@param	idx				�C���f�b�N�X
 *	@param	*p_code		�󂯎��z��
 *	@param	code_len	�󂯎��z��v�f��
 *	@param	*p_len		���������邩
 *	@return	TRUE�Ȃ�Α��݁AFALSE�Ȃ�Α��݂��Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL NAMEIN_STRCHANGE_GetRestStr( const NAMEIN_STRCHANGE *cp_wk, u16 idx, STRCODE *p_code, u8 code_len, u8 *p_len )
{	
	u8	len;

	GF_ASSERT( idx < NAMEIN_STRCHANGE_GetNum(cp_wk) );
	STRTOOL_Copy( cp_wk->word[ idx ].rest, p_code, code_len );
	for( len = 0; len < NAMEIN_STRCHANGE_CODE_LEN && p_code[len] != NAMEIN_STRCHANGE_EMPTY ; (len)++ )
	{	};
	if( p_len )
	{	
		*p_len = len;
	}
	return len != 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ϊ��O�����񂩂�A�C���f�b�N�X�𓾂�
 *
 *	@param	const NAMEIN_STRCHANGE *cp_wk	���[�N
 *	@param	*p_code												�ϊ��O������
 *
 *	@return	�C���f�b�N�X	�Ȃ������ꍇ��NAMEIN_STRCHANGE_NONE
 */
//-----------------------------------------------------------------------------
u16 NAMEIN_STRCHANGE_GetIndexByInputStr( const NAMEIN_STRCHANGE *cp_wk, const STRCODE *cp_code )
{	
	int i;
	u16 max;

	max	= NAMEIN_STRCHANGE_GetNum(cp_wk);
	for( i = 0; i < max; i++ )
	{	
		if( STRCODE_Search( cp_wk->word[ i ].input, cp_code) )
		{	
			return i;
		}
	}

	return NAMEIN_STRCHANGE_NONE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	������������T�[�`
 *
 *	@param	const STRCODE* data	�f�[�^
 *	@param	STRCODE* code				����������
 *
 *	@return	TRUE�Ő���
 */
//-----------------------------------------------------------------------------
static BOOL STRCODE_Search( const STRCODE* data, const STRCODE* code )
{
	STRCODE EOMCODE = GFL_STR_GetEOMCode();
	int i;

	for( i = 0; i < NAMEIN_STRCHANGE_CODE_LEN; i++ )
	{
		if( *data == NAMEIN_STRCHANGE_EMPTY
				&& *code == EOMCODE )
		{	
			return TRUE;
		}
		if( *data != *code )
		{ 
			return FALSE; 
		}
		if( *code == EOMCODE )
		{	
			return FALSE;
		}
		data++;
		code++;
	}

	//�����ɂ��������
	//3����������

	return TRUE;
}
