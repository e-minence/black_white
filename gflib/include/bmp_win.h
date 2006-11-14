//=============================================================================================
/**
 * @file	bmp_win.h
 * @brief	BG�`��V�X�e���v���O����
 * @author	Hiroyuki Nakamura
 * @date	2006/10/18
 */
//=============================================================================================
#ifndef _BMP_WIN_H_
#define _BMP_WIN_H_

#undef GLOBAL
#ifdef __BMP_WIN_H_GLOBAL__
#define GLOBAL /*	*/
#else
#define GLOBAL extern
#endif

// �����r�b�g�}�b�v�C���f�b�N�X�l
#define	GF_BMPWIN_FRM_NULL		(0xff)

///BMPWIN�f�[�^�\����
typedef struct {
	GF_BGL_INI	*bgl;
	u32			heapID;
	u8			frmnum;
	u8			posx;
	u8			posy;
	u8			sizx;
	u8			sizy;
	u8			palnum;
	u16			chrofs:15;
	u16			bitmode:1;	
	void		*chrbuf;
}GF_BMPWIN_DATA;

///BMPWIN�ݒ�f�[�^�\����
typedef struct{
	u8			frm_num;	///<�E�C���h�E�g�p�t���[��
	u8			pos_x;		///<�E�C���h�E�̈�̍����X���W�i�L�����P�ʂŎw��j
	u8			pos_y;		///<�E�C���h�E�̈�̍����Y���W�i�L�����P�ʂŎw��j
	u8			siz_x;		///<�E�C���h�E�̈��X�T�C�Y�i�L�����P�ʂŎw��j
	u8			siz_y;		///<�E�C���h�E�̈��Y�T�C�Y�i�L�����P�ʂŎw��j
	u8			palnum;		///<�E�C���h�E�̈�̃p���b�g�i���o�[
	u16			chrnum;		///<�E�C���h�E�L�����̈�̊J�n�L�����N�^�i���o�[
}BMPWIN_SET;


//�r�b�g�}�b�v�V�t�g�p��`
#define	GF_BMPWIN_SHIFT_U		( 0 )
#define	GF_BMPWIN_SHIFT_D		( 1 )
#define	GF_BMPWIN_SHIFT_L		( 2 )		//������
#define	GF_BMPWIN_SHIFT_R		( 3 )		//������

// BitmapWindow�̃r�b�g���[�h
enum {
	GF_BMPWIN_BITMODE_4 = 0,
	GF_BMPWIN_BITMODE_8 = 1
};




//--------------------------------------------------------------------------------------------
/**
 * BMPWIN�f�[�^�m��
 *
 * @param	heapID	�q�[�vID
 * @param	num		�m�ۂ���BMP�E�B���h�E�̐�
 *
 * @return	�m�ۂ����f�[�^�̃A�h���X
 */
//--------------------------------------------------------------------------------------------
GLOBAL GF_BMPWIN_DATA * GF_BMPWIN_AllocGet( u32 heapID, u8 num );

//--------------------------------------------------------------------------------------------
/**
 * BMPWIN�f�[�^������
 *
 * @param	wk		BMPWIN�f�[�^
 * @param	heapID	�q�[�vID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GF_BMPWIN_Init( GF_BMPWIN_DATA * wk, u32 heapID );

//--------------------------------------------------------------------------------------------
/**
 * BMPWIN�o�^�`�F�b�N
 *
 * @param	win		BMPWIN�f�[�^
 *
 * @retval	"TRUE = �o�^�ς�"
 * @retval	"FALSE = ���o�^"
 */
//--------------------------------------------------------------------------------------------
GLOBAL u8 GF_BMPWIN_AddCheck( GF_BMPWIN_DATA * win );

//--------------------------------------------------------------------------------------------
/**
 * BMPWIN�ǉ�
 *
 * @param	win		BMPWIN�f�[�^�i�[�ꏊ
 * @param	frmnum	BG�t���[���ԍ�
 * @param	posx	X���W�i�L�����P�ʁj
 * @param	posy	Y���W�i�L�����P�ʁj
 * @param	sizx	X�T�C�Y�i�L�����P�ʁj
 * @param	sizy	X�T�C�Y�i�L�����P�ʁj
 * @param	palnum	�g�p�p���b�g�ԍ�
 * @param	chrofs	�g�pCGX�I�t�Z�b�g
 * @param	heapID	�q�[�vID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GF_BMPWIN_Add(
		GF_BMPWIN_DATA * win, u8 frmnum,
		u8 posx, u8 posy, u8 sizx, u8 sizy, u8 palnum, u16 chrofs, u32 heapID );

//--------------------------------------------------------------------------------------------
/**
 * BMPWIN�ǉ��iOBJ�p�ɃL�����̈悾�����m�ہB4bit��p�B�j
 *
 * @param	win			BMPWIN�f�[�^�i�[�ꏊ
 * @param	sizx		X�T�C�Y�i�L�����P�ʁj
 * @param	sizy		X�T�C�Y�i�L�����P�ʁj
 * @param	chrofs		�g�pCGX�I�t�Z�b�g
 * @param   fill_color	���ߐs�����J���[�ԍ�(0�`0xf)
 * @param	heapID		�q�[�vID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GF_BMPWIN_AddChar(
		GF_BMPWIN_DATA * win, u8 sizx, u8 sizy, u16 chrofs, u8 fill_color, u32 heapID );

//--------------------------------------------------------------------------------------------
/**
 * BMPWIN�ǉ��i�f�[�^�w��j
 *
 * @param	win			BMPWIN�f�[�^�i�[�ꏊ
 * @param	dat			BMPWIN_SET�\���̌^�f�[�^�ւ̃|�C���^
 * @param	heapID		�q�[�vID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GF_BMPWIN_AddEx( GF_BMPWIN_DATA * win, const BMPWIN_SET * dat, u32 heapID );

//--------------------------------------------------------------------------------------------
/**
 * �r�b�g�}�b�v�폜
 *
 * @param	win		BMPWIN�f�[�^�i�[�ꏊ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GF_BMPWIN_Del( GF_BMPWIN_DATA * win );

//--------------------------------------------------------------------------------------------
/**
 * �r�b�g�}�b�v�폜�iBMP�f�[�^���J���j
 *
 * @param	win		BMPWIN�f�[�^
 * @param	num		��
 *
 * @return	none
 *
 *	GF_BMPWIN_AllocGet()�Ŋm�ۂ����ꍇ�Ɏg�p���ĉ�����
 *	
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GF_BMPWIN_Free( GF_BMPWIN_DATA * win, u8 num );

//--------------------------------------------------------------------------------------------
/**
 * �r�b�g�}�b�v�\���i�X�N���[���A�L�����]���j
 *
 * @param	win		BMP�f�[�^�i�[�ꏊ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GF_BMPWIN_On( GF_BMPWIN_DATA * win );

//--------------------------------------------------------------------------------------------
/**
 * �r�b�g�}�b�v�\���i�X�N���[���]�����N�G�X�g�A�L�����]���j
 *
 * @param	win		BMP�f�[�^�i�[�ꏊ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GF_BMPWIN_OnVReq( GF_BMPWIN_DATA * win );

//------------------------------------------------------------------
/**
 * Window�̈�X�N���[�����֘ABG�����o�b�t�@�ɍ쐬����
 *
 * @param   win		
 *
 */
//------------------------------------------------------------------
GLOBAL void GF_BMPWIN_MakeScrn( GF_BMPWIN_DATA * win );

//------------------------------------------------------------------
/**
 * Window�̈�X�N���[�����֘ABG�����o�b�t�@�ɍ쐬����
 * �i�T�C�Y�w��BBitmapWindow�쐬�������������̈�̃X�N���[���쐬���\�B�傫���̂͂m�f�B�j
 *
 * @param   win			BitmapWindow �ւ̃|�C���^
 * @param   width		�X�N���[���쐬�̈�̕��i�L�����P�ʁj
 * @param   height		�X�N���[���쐬�̈�̍����i�L�����P�ʁj
 *
 */
//------------------------------------------------------------------
GLOBAL void GF_BMPWIN_MakeScrnLimited( GF_BMPWIN_DATA * win, u32 width, u32 height );

//------------------------------------------------------------------
/**
 * �֘ABG�����o�b�t�@��Window�̈�X�N���[�����N���A����
 *
 * @param   win		
 *
 */
//------------------------------------------------------------------
GLOBAL void GF_BMPWIN_ClearScrn( GF_BMPWIN_DATA * win );

//--------------------------------------------------------------------------------------------
/**
 * �r�b�g�}�b�v�\���i�L�����]���̂݁j
 *
 * @param	win		BMP�f�[�^�i�[�ꏊ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GF_BMPWIN_CgxOn( GF_BMPWIN_DATA * win );

//--------------------------------------------------------------------------------------------
/**
 * �r�b�g�}�b�v��\���i�X�N���[���N���A���]���j
 *
 * @param	win		BMP�f�[�^�i�[�ꏊ
 *
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GF_BMPWIN_Off( GF_BMPWIN_DATA * win );

//--------------------------------------------------------------------------------------------
/**
 * �r�b�g�}�b�v��\���i�X�N���[���N���A���]�����N�G�X�g�j
 *
 * @param	win		BMP�f�[�^�i�[�ꏊ
 *
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GF_BMPWIN_OffVReq( GF_BMPWIN_DATA * win );


//--------------------------------------------------------------------------------------------
/**
 * �w��J���[�őS�̂�h��Ԃ�
 *
 * @param	win		BMP�f�[�^�i�[�ꏊ
 * @param	col		�J���[�i���o�[
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GF_BMPWIN_DataFill( GF_BMPWIN_DATA * win, u8 col );

//--------------------------------------------------------------------------------------------
/**
 * �r�b�g�}�b�v�`��
 *
 * @param	win		BMP�f�[�^�i�[�ꏊ
 * @param	src		�r�b�g�}�b�v�\���f�[�^�|�C���^
 * @param	src_x	�r�b�g�}�b�v�\���J�n�ʒu�w
 * @param	src_y	�r�b�g�}�b�v�\���J�n�ʒu�x
 * @param	src_dx	�r�b�g�}�b�v�f�[�^�T�C�Y�w
 * @param	src_dy	�r�b�g�}�b�v�f�[�^�T�C�Y�w
 * @param	win_x	�E�C���h�E�\���J�n�ʒu�w
 * @param	win_y	�E�C���h�E�\���J�n�ʒu�x
 * @param	win_dx	�E�C���h�E�f�[�^�T�C�Y�w
 * @param	win_dy	�E�C���h�E�f�[�^�T�C�Y�x
 *
 * @retval	�Ȃ�
 *
 * �r�b�g�}�b�v�f�[�^�̏������݂����s
 * �r�b�g�}�b�v�f�[�^�̐؂�����\�ɂ�������
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GF_BMPWIN_Print(
			GF_BMPWIN_DATA * win, void * src,
			u16 src_x, u16 src_y, u16 src_dx, u16 src_dy,
			u16 win_x, u16 win_y, u16 win_dx, u16 win_dy );

//--------------------------------------------------------------------------------------------
/**
 *	�r�b�g�}�b�v�`��i�g���Łj
 *
 * @param	win		BMP�f�[�^�i�[�ꏊ
 * @param	src		�r�b�g�}�b�v�\���f�[�^�|�C���^
 * @param	src_x	�r�b�g�}�b�v�\���J�n�ʒu�w
 * @param	src_y	�r�b�g�}�b�v�\���J�n�ʒu�x
 * @param	src_dx	�r�b�g�}�b�v�f�[�^�T�C�Y�w
 * @param	src_dy	�r�b�g�}�b�v�f�[�^�T�C�Y�w
 * @param	win_x	�E�C���h�E�\���J�n�ʒu�w
 * @param	win_y	�E�C���h�E�\���J�n�ʒu�x
 * @param	win_dx	�E�C���h�E�f�[�^�T�C�Y�w
 * @param	win_dy	�E�C���h�E�f�[�^�T�C�Y�x
 * @param	nuki	�����F�w��i0�`15 0xff:�����F�w��Ȃ��j
 *
 * @retval	�Ȃ�
 *
 * �r�b�g�}�b�v�f�[�^�̏������݂����s
 * �r�b�g�}�b�v�f�[�^�̐؂�����\�ɂ�������
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GF_BMPWIN_PrintEx(
		GF_BMPWIN_DATA * win, void * src,
		u16 src_x, u16 src_y, u16 src_dx, u16 src_dy,
		u16 win_x, u16 win_y, u16 win_dx, u16 win_dy, u16 nuki );

//--------------------------------------------------------------------------------------------
/**
 * �w��͈͂�h��Ԃ�
 *
 * @param	win			BMP�f�[�^
 * @param	col			�h��Ԃ��J���[
 * @param	px			�J�nX���W
 * @param	py			�J�nY���W
 * @param	sx			X�T�C�Y
 * @param	sy			Y�T�C�Y
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GF_BMPWIN_Fill( GF_BMPWIN_DATA * win, u8 col, u16 px, u16 py, u16 sx, u16 sy );

//---------------------------------------------------------------------------------------------
/**
 * �����\����p���������A�b�v��
 *
 * @param	win		BMP�f�[�^�i�[�ꏊ
 * @param	src		�L�����f�[�^
 * @param	ssx		�t�H���gX�T�C�Y
 * @param	ssy		�t�H���gY�T�C�Y
 * @param	wx		��������X���W
 * @param	wy		��������Y���W
 *
 * @return	none
 */
//---------------------------------------------------------------------------------------------
GLOBAL void GF_BMPWIN_PrintMsg( GF_BMPWIN_DATA * win, u8 * src, u16 ssx, u16 ssy, u16 wx, u16 wy );

GLOBAL void GF_BMPWIN_PrintMsgWide(
		GF_BMPWIN_DATA * win, const u8 * src, u16 ssx, u16 ssy, u16 wx, u16 wy, u16 tbl );

//--------------------------------------------------------------------------------------------
/**
 * �r�b�g�}�b�v�E�C���h�E�f�[�^�V�t�g
 *
 * @param	win		BMP�f�[�^�i�[�ꏊ
 * @param	direct	����
 * @param	offset	�V�t�g��
 * @param	data	�󔒂ɂȂ����̈�𖄂߂�f�[�^
 *
 * @return	none
 *
 * �r�b�g�}�b�v�f�[�^(�S�̈�)�̃V�t�g�����s
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GF_BMPWIN_Shift( GF_BMPWIN_DATA * win, u8 direct, u8 offset, u8 data );


//=============================================================================================
//=============================================================================================
//	BMP�f�[�^����
//=============================================================================================
//=============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * BMP�f�[�^�擾�FBG�t���[���ԍ�
 *
 * @param	win		BMP�f�[�^�i�[�ꏊ
 *
 * @return	win->frmnum
 */
//--------------------------------------------------------------------------------------------
GLOBAL u8 GF_BMPWIN_GetFrame( GF_BMPWIN_DATA * win );

//--------------------------------------------------------------------------------------------
/**
 * BMP�f�[�^�擾�FX�T�C�Y
 *
 * @param	win		BMP�f�[�^�i�[�ꏊ
 *
 * @return	win->sizx
 */
//--------------------------------------------------------------------------------------------
GLOBAL u8 GF_BMPWIN_GetSizeX( GF_BMPWIN_DATA * win );

//--------------------------------------------------------------------------------------------
/**
 * BMP�f�[�^�擾�FY�T�C�Y
 *
 * @param	win		BMP�f�[�^�i�[�ꏊ
 *
 * @return	win->sizy
 */
//--------------------------------------------------------------------------------------------
GLOBAL u8 GF_BMPWIN_GetSizeY( GF_BMPWIN_DATA * win );

//--------------------------------------------------------------------------------------------
/**
 * BMP�f�[�^�擾�FX���W
 *
 * @param	win		BMP�f�[�^�i�[�ꏊ
 * @param	index	�r�b�g�}�b�v�g�p�ݒ�w�b�_INDEX
 *
 * @return	win->posx
 */
//--------------------------------------------------------------------------------------------
GLOBAL u8 GF_BMPWIN_GetPosX( GF_BMPWIN_DATA * win );

//--------------------------------------------------------------------------------------------
/**
 * BMP�f�[�^�擾�FY���W
 *
 * @param	win		BMP�f�[�^�i�[�ꏊ
 *
 * @return	win->posy
 */
//--------------------------------------------------------------------------------------------
GLOBAL u8 GF_BMPWIN_GetPosY( GF_BMPWIN_DATA * win );

//--------------------------------------------------------------------------------------------
/**
 * BMP�f�[�^�擾�F�L�����ԍ�
 *
 * @param	win		BMP�f�[�^�i�[�ꏊ
 *
 * @return	win->chrofs
 */
//--------------------------------------------------------------------------------------------
GLOBAL u16 GF_BMPWIN_GetChrofs( GF_BMPWIN_DATA * win );

//--------------------------------------------------------------------------------------------
/**
 * BMP�f�[�^�ݒ�FX���W
 *
 * @param	win		BMP�f�[�^�i�[�ꏊ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GF_BMPWIN_SetPosX( GF_BMPWIN_DATA * win, u8 px );

//--------------------------------------------------------------------------------------------
/**
 * BMP�f�[�^�ݒ�FY���W
 *
 * @param	win		BMP�f�[�^�i�[�ꏊ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GF_BMPWIN_SetPosY( GF_BMPWIN_DATA * win, u8 py );

//--------------------------------------------------------------------------------------------
/**
 * BMP�f�[�^�ݒ�F�p���b�g�ԍ�
 *
 * @param	win		BMP�f�[�^�i�[�ꏊ
 * @param   palnum	�p���b�g�ԍ�
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GF_BMPWIN_SetPal( GF_BMPWIN_DATA * win, u8 palnum );



#undef GLOBAL
#endif
