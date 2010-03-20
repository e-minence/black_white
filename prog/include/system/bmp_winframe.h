//======================================================================
/**
 * @file	window.h
 * @brief	�E�B���h�E�\��
 * @author	Hiroyuki Nakamura
 * @date	2005.10.13
 */
//======================================================================
#ifndef WINDOW_H
#define WINDOW_H

#include <gflib.h>

//======================================================================
//	�萔��`
//======================================================================
#define	MENU_WIN_CGX_SIZ	( 9 )		// ���j���[�E�B���h�E�̃L������
#define	TALK_WIN_CGX_SIZ	( 18+12 )		// ��b�E�B���h�E�̃L������
#define	BOARD_WIN_CGX_SIZ	( 18+12 )		// �ŔE�B���h�E�̃L������
#define	BOARD_MAP_CGX_SIZ	( 24 )		// �Ŕ̃}�b�v�̃L������

// ���ŔA���Ŕ̑S�L������
#define	BOARD_DATA_CGX_SIZ	( BOARD_WIN_CGX_SIZ + BOARD_MAP_CGX_SIZ )

enum {
	WINDOW_TRANS_ON = 0,
	WINDOW_TRANS_ON_V,	//V�u�����N�œ]��
	WINDOW_TRANS_OFF
};

// ���j���[�E�B���h�E�̃^�C�v
typedef enum {
	MENU_TYPE_SYSTEM = 0,	// �I���E�B���h�E�i�V�X�e���E�B���h�E�j
	MENU_TYPE_TALK,		// �t�B�[���h�̃v���[���E�B���h�E(�Ɠ���������)
	MENU_TYPE_MAX,
}MENU_TYPE;

// �Ŕ�
#define BOARD_TYPE_TOWN	(0)	// �^�E���}�b�v
#define	BOARD_TYPE_ROAD	(1)	// �W��
#define	BOARD_TYPE_POST	(2)	// �\�D
#define	BOARD_TYPE_INFO	(3)	// �f����

typedef struct{
  u32 pos;
  u32 size;
} BMPWINFRAME_AREAMANAGER_POS;


//======================================================================
//	�v���g�^�C�v�錾
//======================================================================
//--------------------------------------------------------------
/**
 * �E�B���h�E�L�����N�^�f�[�^�̃A�[�J�C�u�C���f�b�N�X�擾
 *
 * @param	none
 *
 * @return	�A�[�J�C�u�C���f�b�N�X
 */
//--------------------------------------------------------------
extern u32 BmpWinFrame_CgxArcGet( MENU_TYPE win_type );

//--------------------------------------------------------------
/**
 * �E�B���h�E�p���b�g�̃A�[�J�C�u�C���f�b�N�X�擾
 *
 * @param	none
 *
 * @return	�A�[�J�C�u�C���f�b�N�X
 */
//--------------------------------------------------------------
extern u32 BmpWinFrame_WinPalArcGet( MENU_TYPE win_type );

//--------------------------------------------------------------
/**
 * ���j���[�E�B���h�E�̃L�������Z�b�g
 *
 * @param	frmnum		BG�t���[��
 * @param	cgx			  �L�����]���ʒu
 * @param	win_type	�E�B���h�E�^�C�v�� MENU_TYPE�^
 * @param	heap		  �q�[�vID
 *
 * @return	none
 */
//--------------------------------------------------------------
extern void BmpWinFrame_CgxSet( u8 frmnum, u16 cgx, MENU_TYPE win_type, HEAPID heap );

//--------------------------------------------------------------
/**
 * ���j���[�E�B���h�E�̃p���b�g���Z�b�g
 *
 * @param	frmnum		BG�t���[��
 * @param	pal			  �p���b�g�]���ʒu(�p���b�g�P��)
 * @param	win_type		�E�B���h�E�^�C�v�� MENU_TYPE�^
 * @param	heap		�q�[�vID
 *
 * @return	none
 */
//--------------------------------------------------------------
extern void BmpWinFrame_PalSet( u8 frmnum, u8 pal, MENU_TYPE win_type, HEAPID heap );

//--------------------------------------------------------------
/**
 * �E�B���h�E�t���[���̃O���t�B�b�N���Z�b�g
 *
 * @param	ini			  BGL�f�[�^
 * @param	frmnum		BG�t���[��
 * @param	cgx			  �L�����]���ʒu
 * @param	pal			  �p���b�g�ԍ�
 * @param	win_type	�E�B���h�E�ԍ� MENU_TYPE�^
 * @param	heap		�q�[�vID
 *
 * @return	none
 */
//--------------------------------------------------------------
extern void BmpWinFrame_GraphicSet(
	u8 frmnum, u16 cgx, u8 pal, MENU_TYPE win_type, HEAPID heap );

//--------------------------------------------------------------
/**
 * �G���A�}�l�[�W���[���g���ăE�B���h�E�̃O���t�B�b�N���Z�b�g
 *
 * @param	ini			  BGL�f�[�^
 * @param	frmnum		BG�t���[��
 * @param	pal			  �p���b�g�ԍ�
 * @param	win_type	�E�B���h�E�ԍ� MENU_TYPE�^
 * @param	heap		  �q�[�vID
 *
 * @return	GFL_ARCUTIL_TRANSINFO
 */
//--------------------------------------------------------------
extern GFL_ARCUTIL_TRANSINFO BmpWinFrame_GraphicSetAreaMan( u8 frmnum, u8 pal, MENU_TYPE win_type, HEAPID heap );

//--------------------------------------------------------------
/**
 * �G���A�}�l�[�W���[���g���ăZ�b�g�����E�B���h�E�̃L�����N�^�̈���J��
 *
 * @param	frmnum		BG�t���[��
 * @param tInfo     GFL_ARCUTIL_TRANSINFO
 */
//--------------------------------------------------------------
extern void BmpWinFrame_GraphicFreeAreaMan( u8 frmnum, GFL_ARCUTIL_TRANSINFO tInfo );

//--------------------------------------------------------------
/**
 * ���j���[�E�B���h�E��`��
 *
 * @param	win			BMP�E�B���h�E�f�[�^
 * @param	trans_sw	�]���X�C�b�`
 * @param	win_cgx		�E�B���h�E�L�����ʒu
 * @param	pal			�p���b�g
 *
 * @return	none
 */
//--------------------------------------------------------------
extern void BmpWinFrame_Write(
		GFL_BMPWIN *win, u8 trans_sw, u16 win_cgx, u8 pal );

//--------------------------------------------------------------
/**
 * �E�B���h�E��`��(AreaMan���g�������p�̃��[�e�B���e�B)
 *
 * @param	win			  BMP�E�B���h�E�f�[�^
 * @param	trans_sw	�]���X�C�b�`
 * @param	tInfo		  GFL_ARCUTIL_TRANSINFO
 * @param	pal			  �p���b�g
 *
 * @return	none
 */
//--------------------------------------------------------------
extern void BmpWinFrame_WriteAreaMan( GFL_BMPWIN *win, u8 trans_sw, GFL_ARCUTIL_TRANSINFO tInfo, u8 pal );

//--------------------------------------------------------------
/**
 *	���j���[�E�B���h�E���N���A
 *
 * @param	win			BMP�E�B���h�E�f�[�^
 * @param	trans_sw	�]���X�C�b�`
 *
 * @return	none
 */
//--------------------------------------------------------------
extern void BmpWinFrame_Clear( GFL_BMPWIN *win, u8 trans_sw );

//--------------------------------------------------------------
/**
 * @brief   BMPWIN�X�N���[���]��
 *
 * @param	win				BMP�E�B���h�E�f�[�^
 * @param   trans_sw		�]���X�C�b�`
 */
//--------------------------------------------------------------
extern void BmpWinFrame_TransScreen(GFL_BMPWIN *win, u8 trans_sw);

#endif	// WINDOW_H
