//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		namein_data.h
 *	@brief	�Q��ނ̃f�[�^�ǂ݂���
 *
 *					�P�D���O���͗p�f�[�^
 *					�����Ă���f�[�^�́A
 *					�L�[�{�[�h�œ��͂���Ƃ��̕����ł��B
 *		
 *					���ȂȂ�Έȉ�
 *
 *					�����c
 *					�����c
 *					�����c
 *					�����c
 *					�����c
 *
 *					���オ���_�ŁA�E���Ɂ{�ł��B
 *					��L��ŁAGetStr( wk, 1, 3 )�����ꍇ�A�u���v���擾���܂�
 *
 *					
 *					�Q�D�ϊ��p�f�[�^
 *					�ȉ��̃f�[�^�������Ă���
 *					�E�ϊ�����镶����
 *					�E�ϊ���̕�����
 *					�ESHIFT���������Ƃ��̕ϊ���̕�����
 *					�E�ϊ���A�ϊ��o�b�t�@�Ɏc�镶����
 *
 *
 *	@author	Toru=Nagihashi
 *	@date		2009.10.08
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

//=============================================================================
/**
 *					�L�[�z��f�[�^�̕ϐ�
*/
//=============================================================================
//-------------------------------------
///		�ǂݍ��ރ^�C�v
//=====================================
typedef enum
{
	NAMEIN_KEYMAPTYPE_KANA,
	NAMEIN_KEYMAPTYPE_KATA,
	NAMEIN_KEYMAPTYPE_ABC,
	NAMEIN_KEYMAPTYPE_KIGOU,
	NAMEIN_KEYMAPTYPE_QWERTY,
	NAMEIN_KEYMAPTYPE_MAX,
} NAMEIN_KEYMAPTYPE;

//=============================================================================
/**
 *					�L�[�z��f�[�^�̍\���̐錾
*/
//=============================================================================
//-------------------------------------
///	���O���̓f�[�^���[�N
//=====================================
typedef struct _NAMEIN_KEYMAP NAMEIN_KEYMAP;

//-------------------------------------
///	���O���̓f�[�^���[�N�n���h��
//=====================================
typedef struct _NAMEIN_KEYMAP_HANDLE NAMEIN_KEYMAP_HANDLE;

//=============================================================================
/**
 *					�L�[�z��f�[�^�̊O�����J
*/
//=============================================================================
//�f�[�^�ǂ݂���
extern NAMEIN_KEYMAP *NAMEIN_KEYMAP_Alloc( NAMEIN_KEYMAPTYPE type, HEAPID heapID );
extern void NAMEIN_KEYMAP_Free( NAMEIN_KEYMAP *p_wk );

//�������ɑS�f�[�^�������āA�ǂݍ��ޔ�
extern NAMEIN_KEYMAP_HANDLE * NAMEIN_KEYMAP_HANDLE_Alloc( HEAPID heapID );
extern void NAMEIN_KEYMAP_HANDLE_Free( NAMEIN_KEYMAP_HANDLE *p_handle );
extern NAMEIN_KEYMAP *NAMEIN_KEYMAP_HANDLE_GetData( NAMEIN_KEYMAP_HANDLE *p_handle, NAMEIN_KEYMAPTYPE type );

//�f�[�^�擾
extern u16 NAMEIN_KEYMAP_GetWidth( const NAMEIN_KEYMAP *cp_wk );
extern u16 NAMEIN_KEYMAP_GetHeight( const NAMEIN_KEYMAP *cp_wk );
extern STRCODE NAMEIN_KEYMAP_GetStr( const NAMEIN_KEYMAP *cp_wk, u16 x, u16 y );


//=============================================================================
/**
 *					�ϊ��f�[�^�̕ϐ�
*/
//=============================================================================
//-------------------------------------
///		�ǂݍ��ރ^�C�v
//=====================================
typedef enum
{
	NAMEIN_STRCHANGETYPE_DAKUTEN,
	NAMEIN_STRCHANGETYPE_HANDAKUTEN,
	NAMEIN_STRCHANGETYPE_QWERTY,
	NAMEIN_STRCHANGETYPE_MAX,
} NAMEIN_STRCHANGETYPE;

//EOM���̂��������ő吔�i�����̒�`�́Anamein_change_conv.pl���Ɠ����j
#define NAMEIN_STRCHANGE_CODE_LEN	(3)

//�Ȃ��΂����̕Ԃ�l
#define NAMEIN_STRCHANGE_NONE			(0xFFFF)

//=============================================================================
/**
 *					�ϊ��̍\���̐錾
*/
//=============================================================================
//-------------------------------------
///	���O���̓f�[�^���[�N
//=====================================
typedef struct _NAMEIN_STRCHANGE NAMEIN_STRCHANGE;

//=============================================================================
/**
 *					�ϊ��̊O�����J
*/
//=============================================================================
//�f�[�^�ǂ݂���
extern NAMEIN_STRCHANGE *NAMEIN_STRCHANGE_Alloc( NAMEIN_STRCHANGETYPE type, HEAPID heapID );
extern void NAMEIN_STRCHANGE_Free( NAMEIN_STRCHANGE *p_wk );

//�f�[�^�擾
extern u16 NAMEIN_STRCHANGE_GetNum( const NAMEIN_STRCHANGE *cp_wk );
extern void NAMEIN_STRCHANGE_GetInputStr( const NAMEIN_STRCHANGE *cp_wk, u16 idx, STRCODE *p_code, u8 code_len, u8 *p_len );
extern BOOL NAMEIN_STRCHANGE_GetChangeStr( const NAMEIN_STRCHANGE *cp_wk, u16 idx, STRCODE *p_code, u8 code_len, u8 *p_len );
extern BOOL NAMEIN_STRCHANGE_GetChangeShiftStr( const NAMEIN_STRCHANGE *cp_wk, u16 idx, STRCODE *p_code, u8 code_le, u8 *p_len );
extern BOOL NAMEIN_STRCHANGE_GetRestStr( const NAMEIN_STRCHANGE *cp_wk, u16 idx, STRCODE *p_code, u8 code_le, u8 *p_len );
extern u16 NAMEIN_STRCHANGE_GetIndexByInputStr( const NAMEIN_STRCHANGE *cp_wk, const STRCODE *cp_code );
