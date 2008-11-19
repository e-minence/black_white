//=============================================================================================
/**
 * @file	gf_font.h
 * @brief	�t�H���g�f�[�^�}�l�[�W��
 * @author	taya
 * @date	2005.09.14
 *
 */
//=============================================================================================
#ifndef __GF_FONT_H__
#define __GF_FONT_H__


#include <heapsys.h>
#include <arc_tool.h>

//--------------------------------------------------------------
/**
 * �t�H���g�f�[�^�n���h���^��`
 */
//--------------------------------------------------------------
typedef struct _GFL_FONT  GFL_FONT;


//--------------------------------------------------------------
/**
 * �t�H���g�f�[�^�̓ǂݏo������
 */
//--------------------------------------------------------------
typedef enum {
	GFL_FONT_LOADTYPE_FILE,		///< �K�v�ȕ����̂݁A�����t�@�C������ǂݏo��
	GFL_FONT_LOADTYPE_MEMORY,	///< �S�������������ɏ풓�����Ă����A��������ǂݏo��
}GFL_FONT_LOADTYPE;


//=============================================================================================
/**
 * �t�H���g�f�[�^�n���h���쐬
 *
 * @param   arcID			�t�H���g�f�[�^���܂܂��A�[�J�C�uID
 * @param   datID			�t�H���g�f�[�^�̃A�[�J�C�u��ID
 * @param   loadType		�t�H���g�f�[�^�ǂݏo������
 * @param   fixedFontFlag	�����t�H���g�Ƃ��Ĉ������߂̃t���O�iTRUE�Ȃ瓙���j
 * @param   heapID			�n���h�������p�q�[�vID
 *
 * @retval  GFL_FONT*		�t�H���g�f�[�^�n���h��
 */
//=============================================================================================
extern GFL_FONT* GFL_FONT_Create( u32 arcID, u32 datID, GFL_FONT_LOADTYPE loadType, BOOL fixedFontFlag, HEAPID heapID );

//=============================================================================================
/**
 * �t�H���g�f�[�^�n���h���폜
 *
 * @param   wk			�t�H���g�f�[�^�n���h��
 */
//=============================================================================================
extern void GFL_FONT_Delete( GFL_FONT* wk );


//==============================================================================================
/**
 * �t�H���g�f�[�^�}�l�[�W���쐬
 *
 * @param   arcHandle		�t�H���g�f�[�^���܂ރA�[�J�C�u�n���h��
 * @param   datID			�t�H���g�f�[�^���i�[����Ă���A�[�J�C�u���t�@�C��ID
 * @param   loadType		�t�H���g�f�[�^�̓ǂݏo������
 * @param   fixedFontFlag	TRUE�Ȃ瓙���t�H���g�Ƃ��Ĉ���
 * @param   heapID			�}�l�[�W���쐬��q�[�vID
 *
 * @retval  GFL_FONT_HANDLE_MAN*	�t�H���g�f�[�^�}�l�[�W���|�C���^
 *
 * @li  loadType �� FONTDATA_LOADTYPE_ON_MEMORY �̏ꍇ�A�}�l�[�W���Ɠ����q�[�v�̈�Ƀt�H���g�f�[�^��ǂݍ���
 */
//==============================================================================================
extern GFL_FONT* GFL_FONT_CreateHandle( ARCHANDLE* arcHandle, u32 datID, GFL_FONT_LOADTYPE loadType, BOOL fixedFontFlag, HEAPID heapID );


//==============================================================================================
/**
 * �t�H���g�f�[�^�}�l�[�W���폜
 *
 * @param   wk		�t�H���g�f�[�^�}�l�[�W���̃|�C���^
 *
 */
//==============================================================================================
extern void FontDataMan_Delete( GFL_FONT* wk );


//==============================================================================================
/**
 * �t�H���g�r�b�g�f�[�^�̓ǂݍ��݃^�C�v��ύX����
 *
 * @param   wk			�t�H���g�f�[�^�}�l�[�W���|�C���^
 * @param   loadType	�ύX��̓ǂݍ��݃^�C�v
 * @param   heapID		�풓�^�C�v�ɕύX�̏ꍇ�A�t�H���g�f�[�^��ǂݍ��ރq�[�v�Ƃ��Ďg�p����B
 *
 */
//==============================================================================================
extern void FontDataMan_ChangeLoadType( GFL_FONT* wk, GFL_FONT_LOADTYPE loadType, HEAPID heapID );


//------------------------------------------------------------------
/*
 *	�����r�b�g�}�b�v�f�[�^�擾
 *
 * @param	wk			�t�H���g�f�[�^�}�l�[�W��
 * @param	index		�����R�[�h
 * @param	dst			�r�b�g�}�b�v�擾���[�N
 *
 */
//------------------------------------------------------------------
extern void FontDataMan_GetBitmap( const GFL_FONT* wk, u32 index, void* dst );


extern void GFL_FONT_GetBitMap( const GFL_FONT* wk, u32 index, void* dst, u16* sizeX, u16* sizeY );
extern u16 GFL_FONT_GetWidth( const GFL_FONT* wk, u32 index );
extern u16 GFL_FONT_GetHeight( const GFL_FONT* wk, u32 index );


extern void GFL_FONTSYS_Init( void );
extern void GFL_FONTSYS_SetDefaultColor( void );
extern void GFL_FONTSYS_SetColor( u8 letterColor, u8 shadowColor, u8 backColor );
extern void GFL_FONTSYS_GetColor( u8* letter, u8* shadow, u8* back );


#if 0
//------------------------------------------------------------------
/**
 * ��������r�b�g�}�b�v���������̒����i�h�b�g�j���v�Z���ĕԂ�
 *
 * @param   wk		�t�H���g�f�[�^�}�l�[�W��
 * @param   str		������
 * @param   margin	���ԁi�h�b�g�j
 *
 * @retval  u32		����
 */
//------------------------------------------------------------------
extern u32 FontDataMan_GetStrWidth( const GFL_FONT* wk, const STRCODE* str, u32 margin );
#endif

#endif
