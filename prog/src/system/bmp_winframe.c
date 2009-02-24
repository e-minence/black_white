//======================================================================
/**
 * @file	bmp_winframe.c
 * @brief	�E�B���h�E�t���[���\��
 * @author	Hiroyuki Nakamura
 * @date	2005.10.13
 *	�E2008.09 DP����WB�ֈڐA
 */
//======================================================================
#include <gflib.h>

#include "include/system/gfl_use.h"
#include "arc/arc_def.h"
#include "system/bmp_winframe.h"

#include "arc/test_graphic/winframe.naix"	//test
#include "arc/test_graphic/menu_gra.naix"	//test

//======================================================================
//	�萔��`
//======================================================================
#define	MAKE_CURSOR_CGX_SIZ	( 0x20*4*3 )	// ����J�[�\���T�C�Y

// �ŔE�B���h�E�}�b�v�J�n�A�[�J�C�uID
#define	BOARD_TOWNMAP_START	( NARC_field_board_board_town_dmy_NCGR )	// �X
#define	BOARD_ROADMAP_START	( NARC_field_board_board_road000_NCGR )		// ��

//======================================================================
//	�v���g�^�C�v�錾
//======================================================================

//======================================================================
//	���j���[�E�B���h�E
//======================================================================
//--------------------------------------------------------------
/**
 * ���j���[�E�B���h�E�̃L�������Z�b�g
 *
 * @param	ini			BGL�f�[�^
 * @param	frmnum		BG�t���[��
 * @param	cgx			�L�����]���ʒu
 * @param	win_num		�E�B���h�E�ԍ�
 * @param	heap		�q�[�vID
 *
 * @return	none
 */
//--------------------------------------------------------------
void BmpWinFrame_CgxSet( u8 frmnum, u16 cgx, u8 win_num, u32 heap )
{
	if( win_num == MENU_TYPE_SYSTEM ){
		GFL_ARC_UTIL_TransVramBgCharacter(
			ARCID_FLDMAP_WINFRAME, NARC_winframe_system_ncgr,
			frmnum, cgx, 0, 0, heap );
	}else{
		GFL_ARC_UTIL_TransVramBgCharacter(
			ARCID_FLDMAP_WINFRAME, NARC_winframe_fmenu_ncgr,
			frmnum, cgx, 0, 0, heap );
	}
}

//--------------------------------------------------------------
/**
 * ���j���[�E�B���h�E�p���b�g�̃A�[�J�C�u�C���f�b�N�X�擾
 *
 * @param	none
 *
 * @return	�A�[�J�C�u�C���f�b�N�X
 */
//--------------------------------------------------------------
u32 BmpWinFrame_WinPalArcGet(void)
{
	return ( NARC_winframe_system_nclr );
}

//--------------------------------------------------------------
/**
 * ���j���[�E�B���h�E�̃O���t�B�b�N���Z�b�g
 *
 * @param	ini			BGL�f�[�^
 * @param	frmnum		BG�t���[��
 * @param	cgx			�L�����]���ʒu
 * @param	pal			�p���b�g�ԍ�
 * @param	win_num		�E�B���h�E�ԍ�
 * @param	heap		�q�[�vID
 *
 * @return	none
 */
//--------------------------------------------------------------
void BmpWinFrame_GraphicSet(
	u8 frmnum, u16 cgx, u8 pal, u8 win_num, u32 heap )
{
	u32	arc;
	
	// �L����
	if( win_num == MENU_TYPE_SYSTEM ){
		arc = NARC_winframe_system_ncgr;
	}else{
		arc = NARC_winframe_fmenu_ncgr;
	}

	GFL_ARC_UTIL_TransVramBgCharacter(
		ARCID_FLDMAP_WINFRAME, arc, frmnum, cgx, 0, 0, heap );
	
	// �p���b�g
	if( win_num == MENU_TYPE_UG ){
		arc = NARC_winframe_ugmenu_win_nclr;
	}else{
		arc = NARC_winframe_system_nclr;
	}

	if( frmnum < GFL_BG_FRAME0_S ){
		GFL_ARC_UTIL_TransVramPalette( ARCID_FLDMAP_WINFRAME,
			arc, PALTYPE_MAIN_BG, pal*0x20, 0x20, heap );
	}else{
		GFL_ARC_UTIL_TransVramPalette( ARCID_FLDMAP_WINFRAME,
			arc, PALTYPE_SUB_BG, pal*0x20, 0x20, heap );
	}
}


//--------------------------------------------------------------
/**
 * �G���A�}�l�[�W���[���g���ă��j���[�E�B���h�E�̃O���t�B�b�N���Z�b�g
 *
 * @param	ini			BGL�f�[�^
 * @param	frmnum		BG�t���[��
 * @param	pal			�p���b�g�ԍ�
 * @param	win_num		�E�B���h�E�ԍ�
 * @param	heap		�q�[�vID
 *
 * @return	GFL_ARCUTIL_TRANSINFO
 */
//--------------------------------------------------------------
GFL_ARCUTIL_TRANSINFO BmpWinFrame_GraphicSetAreaMan( u8 frmnum, u8 pal, u8 win_num, u32 heap )
{
	u32	arc,pos;
    GFL_ARCUTIL_TRANSINFO bgchar;
	
	// �L����
	if( win_num == MENU_TYPE_SYSTEM ){
		arc = NARC_winframe_system_ncgr;
	}else{
		arc = NARC_winframe_fmenu_ncgr;
	}

	bgchar = GFL_ARC_UTIL_TransVramBgCharacterAreaMan(
        ARCID_FLDMAP_WINFRAME, arc, frmnum, 0, 0, heap );
	
	// �p���b�g
	if( win_num == MENU_TYPE_UG ){
		arc = NARC_winframe_ugmenu_win_nclr;
	}else{
		arc = NARC_winframe_system_nclr;
	}

	if( frmnum < GFL_BG_FRAME0_S ){
		GFL_ARC_UTIL_TransVramPalette( ARCID_FLDMAP_WINFRAME,
			arc, PALTYPE_MAIN_BG, pal*0x20, 0x20, heap );
	}else{
		GFL_ARC_UTIL_TransVramPalette( ARCID_FLDMAP_WINFRAME,
			arc, PALTYPE_SUB_BG, pal*0x20, 0x20, heap );
	}
    return bgchar;
}

//--------------------------------------------------------------
/**
 *	���j���[�E�C���h�E�`�惁�C��
 *
 * @param	frm			BG�t���[���i���o�[(bg_sys)
 * @param	px			�w���W
 * @param	py			�x���W
 * @param	sx			�w�T�C�Y
 * @param	sy			�T�C�Y
 * @param	pal			�g�p�p���b�g
 *
 * @return	none
 */
//--------------------------------------------------------------
static void BmpMenuWinWriteMain(
	u8 frm, u8 px, u8 py, u8 sx, u8 sy, u8 pal, u16 cgx )
{
	GFL_BG_FillScreen( frm, cgx,   px-1,  py-1, 1,  1, pal );
	GFL_BG_FillScreen( frm, cgx+1, px,    py-1, sx, 1, pal );
	GFL_BG_FillScreen( frm, cgx+2, px+sx, py-1, 1,  1, pal );
	
	GFL_BG_FillScreen( frm, cgx+3, px-1,  py, 1, sy, pal );
	GFL_BG_FillScreen( frm, cgx+5, px+sx, py, 1, sy, pal );

	GFL_BG_FillScreen( frm, cgx+6, px-1,  py+sy, 1,  1, pal );
	GFL_BG_FillScreen( frm, cgx+7, px,    py+sy, sx, 1, pal );
	GFL_BG_FillScreen( frm, cgx+8, px+sx, py+sy, 1,  1, pal );
}

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
void BmpWinFrame_Write( GFL_BMPWIN *win, u8 trans_sw, u16 win_cgx, u8 pal )
{
	u8	frm = GFL_BMPWIN_GetFrame( win );
	
	BmpMenuWinWriteMain(
		frm, 
		GFL_BMPWIN_GetPosX( win ),
		GFL_BMPWIN_GetPosY( win ),
		GFL_BMPWIN_GetSizeX( win ),
		GFL_BMPWIN_GetSizeY( win ),
		pal, win_cgx );
	
	if( trans_sw == WINDOW_TRANS_ON ){
		GFL_BG_LoadScreenReq( frm );
	}
}

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
void BmpWinFrame_Clear( GFL_BMPWIN *win, u8 trans_sw )
{
	u8	frm = GFL_BMPWIN_GetFrame( win );
	
	GFL_BG_FillScreen(
		frm, 0,
		GFL_BMPWIN_GetPosX( win ) - 1,
		GFL_BMPWIN_GetPosY( win ) - 1,
		GFL_BMPWIN_GetSizeX( win ) + 2,
		GFL_BMPWIN_GetSizeY( win ) + 2,
		0 );

	if( trans_sw == WINDOW_TRANS_ON ){
//		GFL_BG_ClearScreen( frm );
		GFL_BG_LoadScreenReq( frm );
	}
}
