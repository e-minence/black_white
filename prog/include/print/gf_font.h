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
#include <strbuf.h>

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
 * �V�X�e���������i�v���O�����N����ɂP��̂݁j
 */
//=============================================================================================
extern void GFL_FONTSYS_Init( void );

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


extern void GFL_FONTSYS_SetDefaultColor( void );
extern void GFL_FONTSYS_SetColor( u8 letterColor, u8 shadowColor, u8 backColor );
extern void GFL_FONTSYS_GetColor( u8* letter, u8* shadow, u8* back );
extern BOOL GFL_FONTSYS_IsDifferentColor( u8 letter, u8 shadow, u8 back );


//=========================================================================
//-----------------------------------
/**
 *	�����h�b�g�T�C�Y���
 */
//-----------------------------------
typedef struct {
	u8 left_width;		///< �����]�������̕�
	u8 glyph_width;		///< ���������̕�
	u8 width;			///< �]���A���������̍��v��
	u8 height;			///< ����
}GFL_FONT_SIZE;

extern void GFL_FONT_GetBitMap( const GFL_FONT* wk, STRCODE code, void* dst, GFL_FONT_SIZE* size );
extern u16 GFL_FONT_GetWidth( const GFL_FONT* wk, STRCODE code );
extern u16 GFL_FONT_GetLineHeight( const GFL_FONT* wk );


#endif
