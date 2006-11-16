//=============================================================================================
/**
 * @file	bmp_win.c
 * @brief	BMP�E�B���h�E�V�X�e��
 * @author	Hiroyuki Nakamura
 * @date	2006/10/18
 *
 *	�|�P����DP��bg_system.c���番��
 *		BGL		->	bg_sys.c
 *		BMP		->	bmp.c
 *		BMPWIN	->	bmp_win.c
 */
//=============================================================================================
#include "gflib.h"
#include "bg_sys.h"
#include "bmp.h"

#define	__BMP_H_GLOBAL__
#include "bmp_win.h"

static void BmpWinOn_Normal( GFL_BMPWIN_DATA * win );
static void BmpWinOnVReq_Normal( GFL_BMPWIN_DATA * win );
static void BmpWinOn_Affine( GFL_BMPWIN_DATA * win );
static void BmpWinOnVReq_Affine( GFL_BMPWIN_DATA * win );
static void BmpWinOff_Normal( GFL_BMPWIN_DATA * win );
static void BmpWinOffVReq_Normal( GFL_BMPWIN_DATA * win );
static void BmpWinOff_Affine( GFL_BMPWIN_DATA * win );
static void BmpWinOffVReq_Affine( GFL_BMPWIN_DATA * win );
static void BmpWinShift16( GFL_BMPWIN_DATA * win, u8 direct, u8 offset, u8 data );
static void BmpWinShift256( GFL_BMPWIN_DATA * win, u8 direct, u8 offset, u8 data );

static void MakeWinScrnText( GFL_BMPWIN_DATA * win );
static void MakeWinScrnAffine( GFL_BMPWIN_DATA * win );
static void ClearWinScrnText( GFL_BMPWIN_DATA * win );
static void ClearWinScrnAffine( GFL_BMPWIN_DATA * win );


//*------------------------------------------*/
/**   �E�B���h�E�f�[�^�쐬���]���֐�         */
//*------------------------------------------*/
static void (* const WinOnFunc[])(GFL_BMPWIN_DATA*) = {
	BmpWinOn_Normal,		// �e�L�X�g
	BmpWinOn_Affine,		// �A�t�B��
	BmpWinOn_Normal,		// �g���A�t�B��
};
static void (* const WinOnVReqFunc[])(GFL_BMPWIN_DATA*) = {
	BmpWinOnVReq_Normal,		// �e�L�X�g
	BmpWinOnVReq_Affine,		// �A�t�B��
	BmpWinOnVReq_Normal,		// �g���A�t�B��
};
//*------------------------------------------*/
/**   �E�B���h�E�\���I�t���]���֐�           */
//*------------------------------------------*/
static void (* const WinOffFunc[])(GFL_BMPWIN_DATA*) = {
	BmpWinOff_Normal,
	BmpWinOff_Affine,
	BmpWinOff_Normal,
};
static void (* const WinOffVReqFunc[])(GFL_BMPWIN_DATA*) = {
	BmpWinOffVReq_Normal,
	BmpWinOffVReq_Affine,
	BmpWinOffVReq_Normal,
};

//*------------------------------------------*/
/**   �E�B���h�E�X�N���[���쐬�֐��e�[�u��   */
//*------------------------------------------*/
static void (* const WinScrnMakeFunc[])(GFL_BMPWIN_DATA*) = {
	MakeWinScrnText,		// �e�L�X�g
	MakeWinScrnAffine,		// �A�t�B��
	MakeWinScrnText,		// �g���A�t�B��
};

//*------------------------------------------*/
/**   �E�B���h�E�X�N���[���N���A�֐��e�[�u�� */
//*------------------------------------------*/
static void (* const WinScrnClearFunc[])(GFL_BMPWIN_DATA*) = {
	ClearWinScrnText,		// �e�L�X�g
	ClearWinScrnAffine,		// �A�t�B��
	ClearWinScrnText,		// �g���A�t�B��
};


static const u8 ScreenXElems[] =  {
	16,		// GFL_BG_SCRSIZ_128x128
	32,		// GFL_BG_SCRSIZ_256x256
	32,		// GFL_BG_SCRSIZ_256x512
	32,		// GFL_BG_SCRSIZ_512x256
	32,		// GFL_BG_SCRSIZ_512x512
	32,		// GFL_BG_SCRSIZ_1024x1024
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
GFL_BMPWIN_DATA * GFL_BMPWIN_AllocGet( u32 heapID, u8 num )
{
	GFL_BMPWIN_DATA * wk;
	u16	i;

	wk = (GFL_BMPWIN_DATA *)sys_AllocMemory( heapID, sizeof(GFL_BMPWIN_DATA) * num );

	for( i=0; i<num; i++ ){
		GFL_BMPWIN_Init( &wk[i], heapID );
	}

	return wk;
}

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
void GFL_BMPWIN_Init( GFL_BMPWIN_DATA * wk, u32 heapID )
{
	wk->heapID = heapID;
	wk->frmnum = GFL_BMPWIN_FRM_NULL;
	wk->posx   = 0;
	wk->posy   = 0;
	wk->sizx   = 0;
	wk->sizy   = 0;
	wk->palnum = 0;
	wk->chrofs = 0;
	wk->chrbuf = NULL;
	wk->bitmode = GFL_BMPWIN_BITMODE_4;		// �O�̂��ߗǂ��g�����ŏ��������Ă���
}

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
u8 GFL_BMPWIN_AddCheck( GFL_BMPWIN_DATA * win )
{
	if( win->frmnum == GFL_BMPWIN_FRM_NULL || win->chrbuf == NULL ){
		return FALSE;
	}
	return TRUE;
}

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
void GFL_BMPWIN_Add(
		GFL_BMPWIN_DATA * win, u8 frmnum,
		u8 posx, u8 posy, u8 sizx, u8 sizy, u8 palnum, u16 chrofs, u32 heapID )
{
	void * chrvbuf;
	u32	chrvsiz;

	// �g�p�t���[���󋵂̔���i�X�N���[���o�b�t�@�m�ۂ���Ă��邩�j
	if( GFL_BG_ScreenAdrsGet( win->bgl, frmnum ) == NULL ){
#ifdef	OSP_ERR_BGL_BMPADD		// BMP�o�^���s
		OS_Printf( "�a�l�o�o�^���s\n" );
#endif	// OSP_ERR_BGL_BMPADD
		return;
	}

	// �L�����N�^�o�b�t�@�̊m��
	chrvsiz = sizx * sizy * GFL_BG_BaseCharSizeGet( win->bgl, frmnum);
	chrvbuf = sys_AllocMemory( heapID, chrvsiz );

	if( chrvbuf == NULL ){
#ifdef	OSP_ERR_BGL_BMPADD		// BMP�o�^���s
		OS_Printf( "�a�l�o�̈�m�ێ��s\n" );
#endif	// OSP_ERR_BGL_BMPADD
		return;
	}

	// �r�b�g�}�b�v�̐ݒ�
	win->heapID = heapID;
	win->frmnum = frmnum;
	win->posx   = posx;
	win->posy   = posy;
	win->sizx   = sizx;
	win->sizy   = sizy;
	win->palnum = palnum;
	win->chrofs = chrofs;
	win->chrbuf = chrvbuf;
	win->bitmode = (GFL_BG_ScreenColorModeGet( win->bgl, frmnum) == GX_BG_COLORMODE_16)? GFL_BMPWIN_BITMODE_4 : GFL_BMPWIN_BITMODE_8;

#ifdef	OSP_BGL_BMP_SIZ		// BMP�T�C�Y
	OS_Printf( "�a�l�o�b�f�w�̈�=%x\n", chrvbuf );
#endif	// OSP_BGL_BMP_SIZ
}

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
void GFL_BMPWIN_AddChar(
		GFL_BMPWIN_DATA * win, u8 sizx, u8 sizy, u16 chrofs, u8 fill_color, u32 heapID )
{
	void * chrvbuf;
	u32	chrvsiz;

	chrvsiz	= (u32)( sizx * sizy * GFL_BG_1CHRDATASIZ );
	chrvbuf = sys_AllocMemory( heapID, chrvsiz );

	fill_color |= fill_color << 4;
	memset( chrvbuf, fill_color, chrvsiz );

	if( chrvbuf == NULL ){
#ifdef	OSP_ERR_BGL_BMPADD		// BMP�o�^���s
		OS_Printf( "BMPOBJ�̈�m�ێ��s\n" );
#endif	// OSP_ERR_BGL_BMPADD
		return;
	}

	// �r�b�g�}�b�v�̐ݒ�
	win->heapID = heapID;
	win->sizx   = sizx;
	win->sizy   = sizy;
	win->chrofs = chrofs;
	win->chrbuf = chrvbuf;
	win->bitmode = GFL_BMPWIN_BITMODE_4;

#ifdef	OSP_BGL_BMP_SIZ		// BMP�T�C�Y
	OS_Printf( "BMPOBJCGX�̈�=%x\n", chrvbuf );
#endif	// OSP_BGL_BMP_SIZ
}

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
void GFL_BMPWIN_AddEx( GFL_BMPWIN_DATA * win, const BMPWIN_SET * dat, u32 heapID )
{
	GFL_BMPWIN_Add(
		win, dat->frm_num, dat->pos_x, dat->pos_y,
		dat->siz_x, dat->siz_y, dat->palnum, dat->chrnum, heapID );
}

//--------------------------------------------------------------------------------------------
/**
 * �r�b�g�}�b�v�폜
 *
 * @param	win		BMPWIN�f�[�^�i�[�ꏊ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void GFL_BMPWIN_Del( GFL_BMPWIN_DATA * win )
{
	// �L�����N�^�o�b�t�@���蓖�ĊJ��
	sys_FreeMemoryEz( win->chrbuf );

	win->frmnum = GFL_BMPWIN_FRM_NULL;
	win->posx   = 0;
	win->posy   = 0;
	win->sizx   = 0;
	win->sizy   = 0;
	win->palnum = 0;
	win->chrofs = 0;
	win->chrbuf = NULL;
	win->heapID = 0;
}

//--------------------------------------------------------------------------------------------
/**
 * �r�b�g�}�b�v�폜�iBMP�f�[�^���J���j
 *
 * @param	win		BMPWIN�f�[�^
 * @param	num		��
 *
 * @return	none
 *
 *	GFL_BMPWIN_AllocGet()�Ŋm�ۂ����ꍇ�Ɏg�p���ĉ�����
 *	
 */
//--------------------------------------------------------------------------------------------
void GFL_BMPWIN_Free( GFL_BMPWIN_DATA * win, u8 num )
{
	u16	i;

	// �L�����N�^�o�b�t�@���蓖�ĊJ��
	for( i=0; i<num; i++ ){
		if( win[i].chrbuf == NULL ){ continue; }
		sys_FreeMemoryEz( win[i].chrbuf );
	}

	// BMP�f�[�^�̈�J��
	sys_FreeMemoryEz( win );
}

//--------------------------------------------------------------------------------------------
/**
 * �r�b�g�}�b�v�\���i�X�N���[���A�L�����]���j
 *
 * @param	win		BMP�f�[�^�i�[�ꏊ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void GFL_BMPWIN_On( GFL_BMPWIN_DATA * win )
{
	u8	mode;
	
	GF_ASSERT(win!=NULL);
	GF_ASSERT(win->frmnum<8);

	mode = GFL_BG_BgModeGet( win->bgl, win->frmnum );

	GF_ASSERT(mode < NELEMS(WinOnFunc));
	WinOnFunc[ mode ]( win );
}
//--------------------------------------------------------------------------------------------
/**
 * �r�b�g�}�b�v�\���i�X�N���[���]�����N�G�X�g�A�L�����]���j
 *
 * @param	win		BMP�f�[�^�i�[�ꏊ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void GFL_BMPWIN_OnVReq( GFL_BMPWIN_DATA * win )
{
	u8	mode;

	GF_ASSERT(win!=NULL);
	GF_ASSERT(win->frmnum<8);

	mode = GFL_BG_BgModeGet( win->bgl, win->frmnum );

	GF_ASSERT(mode < NELEMS(WinOnVReqFunc));
	WinOnVReqFunc[ mode ]( win );
}
//------------------------------------------------------------------
/**
 * Window�̈�X�N���[�����֘ABG�����o�b�t�@�ɍ쐬����
 *
 * @param   win		
 *
 */
//------------------------------------------------------------------
void GFL_BMPWIN_MakeScrn( GFL_BMPWIN_DATA * win )
{
	WinScrnMakeFunc[ GFL_BG_BgModeGet( win->bgl, win->frmnum ) ]( win );
}
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
void GFL_BMPWIN_MakeScrnLimited( GFL_BMPWIN_DATA * win, u32 width, u32 height )
{
	u32  x_save, y_save;

	x_save = win->sizx;
	y_save = win->sizy;
	win->sizx = width;
	win->sizy = height;

	WinScrnMakeFunc[ GFL_BG_BgModeGet( win->bgl, win->frmnum ) ]( win );

	win->sizx = x_save;
	win->sizy = y_save;

}
//------------------------------------------------------------------
/**
 * �֘ABG�����o�b�t�@��Window�̈�X�N���[�����N���A����
 *
 * @param   win		
 *
 */
//------------------------------------------------------------------
void GFL_BMPWIN_ClearScrn( GFL_BMPWIN_DATA * win )
{
	WinScrnClearFunc[ GFL_BG_BgModeGet( win->bgl, win->frmnum) ]( win );
}

//------------------------------------------------------------------
/**
 * �X�N���[���쐬�i�e�L�X�g�E�g���A�t�B���p�j
 *
 * @param   win		
 *
 */
//------------------------------------------------------------------
static void MakeWinScrnText( GFL_BMPWIN_DATA * win )
{
	u16 * scrn = (u16 *)GFL_BG_ScreenAdrsGet( win->bgl, win->frmnum );

	if( scrn != NULL ){
		u32 x, y, x_max, y_max, chr, p, xelems;

//		xelems = ScreenXElems[ GFL_BG_ScreenTypeGet(win->frmnum) ];
		xelems = 32;

		chr = win->chrofs;

		x_max = win->posx + win->sizx;
		y_max = win->posy + win->sizy;
		for(y = win->posy; y < y_max; y++){
			for(x = win->posx; x < x_max; x++){
				p = ((y&0x20)*32) + ((x&0x20)*32) + ((y&0x1f)*xelems) + (x&0x1f);
				scrn[p] = chr | (win->palnum << 12);
				chr++;
			}
		}
	}
}
//------------------------------------------------------------------
/**
 * �X�N���[���쐬�i�A�t�B��BG�p�j
 *
 * @param   win		
 *
 */
//------------------------------------------------------------------
static void MakeWinScrnAffine( GFL_BMPWIN_DATA * win )
{
	u8 * scrn = (u8 *)GFL_BG_ScreenAdrsGet( win->bgl, win->frmnum );

	if( scrn != NULL ){
		int x, y, chr, xelems;

		xelems = ScreenXElems[ GFL_BG_ScreenTypeGet( win->bgl, win->frmnum) ];

		scrn = scrn + win->posy * xelems + win->posx;

		chr = win->chrofs;

		for( y = 0; y < win->sizy; y++ ){
			for( x = 0; x < win->sizx; x++ ){
				scrn[x] = chr++;
			}
			scrn += xelems;
		}
	}
}
//------------------------------------------------------------------
/**
 * �X�N���[���N���A�i�e�L�X�g�E�g���A�t�B���p�j
 *
 * @param   win		
 *
 */
//------------------------------------------------------------------
static void ClearWinScrnText( GFL_BMPWIN_DATA * win )
{
	u16 * scrn = (u16 *)GFL_BG_ScreenAdrsGet( win->bgl, win->frmnum );

	if( scrn != NULL ){
		u32 x, y, x_max, y_max, p, xelems;

		xelems = ScreenXElems[ GFL_BG_ScreenTypeGet( win->bgl, win->frmnum ) ];

		x_max = win->posx + win->sizx;
		y_max = win->posy + win->sizy;
		for(y = win->posy; y < y_max; y++){
			for(x = win->posx; x < x_max; x++){
				p = ((y&0x20)*32) + ((x&0x20)*32) + ((y&0x1f)*xelems) + (x&0x1f);
				scrn[p] = 0x0000;
			}
		}
	}
}
//------------------------------------------------------------------
/**
 * �X�N���[���N���A�i�A�t�B��BG�p�j
 *
 * @param   win		
 *
 */
//------------------------------------------------------------------
static void ClearWinScrnAffine( GFL_BMPWIN_DATA * win )
{
	u8 * scrn = (u8 *)GFL_BG_ScreenAdrsGet( win->bgl, win->frmnum );

	if( scrn != NULL ){
		int x, y, xelems;

		xelems = ScreenXElems[ GFL_BG_ScreenTypeGet( win->bgl, win->frmnum) ];

		scrn = scrn + win->posy * xelems + win->posx;

		for( y = 0; y < win->sizy; y++ ){
			for( x = 0; x < win->sizx; x++ ){
				scrn[x] = 0;
			}
			scrn += xelems;
		}
	}

}
//--------------------------------------------------------------------------------------------
/**
 * �r�b�g�}�b�v�\���i�X�N���[���A�L�����]���i�e�L�X�g�A�A�t�B���g��BG�j�j
 *
 * @param	win		BMP�f�[�^�i�[�ꏊ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BmpWinOn_Normal( GFL_BMPWIN_DATA * win )
{
	MakeWinScrnText( win );

	GFL_BG_LoadScreen(
		win->bgl,
		win->frmnum,
		GFL_BG_ScreenAdrsGet( win->bgl, win->frmnum ),
		GFL_BG_ScreenSizGet( win->bgl, win->frmnum ),
		GFL_BG_ScreenOfsGet( win->bgl, win->frmnum ) );

	GFL_BMPWIN_CgxOn( win );
}
//--------------------------------------------------------------------------------------------
/**
 * �r�b�g�}�b�v�\���i�X�N���[���]�����N�G�X�g�A�L�����]���i�e�L�X�g�A�A�t�B���g��BG�j�j
 *
 * @param	win		BMP�f�[�^�i�[�ꏊ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BmpWinOnVReq_Normal( GFL_BMPWIN_DATA * win )
{
	MakeWinScrnText( win );

	GFL_BG_LoadScreenV_Req( win->bgl, win->frmnum );
	GFL_BMPWIN_CgxOn( win );
}
//--------------------------------------------------------------------------------------------
/**
 * �r�b�g�}�b�v�\���i�X�N���[���A�L�����]���i�g�k�ʁj�j
 *
 * @param	win		BMP�f�[�^�i�[�ꏊ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BmpWinOn_Affine( GFL_BMPWIN_DATA * win )
{
	MakeWinScrnAffine( win );

	GFL_BG_LoadScreen(
		win->bgl,
		win->frmnum,
		GFL_BG_ScreenAdrsGet( win->bgl, win->frmnum ),
		GFL_BG_ScreenSizGet( win->bgl, win->frmnum ),
		GFL_BG_ScreenOfsGet( win->bgl, win->frmnum ) );

	GFL_BG_LoadCharacter(
		win->bgl, win->frmnum, win->chrbuf,
		(u32)( win->sizx * win->sizy * GFL_BG_8BITCHRSIZ ), (u32)win->chrofs );
}
//--------------------------------------------------------------------------------------------
/**
 * �r�b�g�}�b�v�\���i�X�N���[���]�����N�G�X�g�A�L�����]���i�g�k�ʁj�j
 *
 * @param	win		BMP�f�[�^�i�[�ꏊ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BmpWinOnVReq_Affine( GFL_BMPWIN_DATA * win )
{
	MakeWinScrnAffine( win );

	GFL_BG_LoadScreenV_Req( win->bgl, win->frmnum );
	GFL_BG_LoadCharacter(
		win->bgl, win->frmnum, win->chrbuf,
		(u32)( win->sizx * win->sizy * GFL_BG_8BITCHRSIZ ), (u32)win->chrofs );
}

//--------------------------------------------------------------------------------------------
/**
 * �r�b�g�}�b�v�\���i�L�����]���̂݁j
 *
 * @param	win		BMP�f�[�^�i�[�ꏊ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void GFL_BMPWIN_CgxOn( GFL_BMPWIN_DATA * win )
{
	u32 chrsize = win->sizx * win->sizy * GFL_BG_BaseCharSizeGet( win->bgl, win->frmnum );

	GFL_BG_LoadCharacter( win->bgl, win->frmnum, win->chrbuf, chrsize, win->chrofs );
}

//--------------------------------------------------------------------------------------------
/**
 * �r�b�g�}�b�v��\���i�X�N���[���N���A���]���j
 *
 * @param	win		BMP�f�[�^�i�[�ꏊ
 *
 */
//--------------------------------------------------------------------------------------------
void GFL_BMPWIN_Off( GFL_BMPWIN_DATA * win )
{
	WinOffFunc[ GFL_BG_BgModeGet( win->bgl, win->frmnum ) ]( win );
}
//--------------------------------------------------------------------------------------------
/**
 * �r�b�g�}�b�v��\���i�X�N���[���N���A���]�����N�G�X�g�j
 *
 * @param	win		BMP�f�[�^�i�[�ꏊ
 *
 */
//--------------------------------------------------------------------------------------------
void GFL_BMPWIN_OffVReq( GFL_BMPWIN_DATA * win )
{
	WinOffVReqFunc[ GFL_BG_BgModeGet( win->bgl, win->frmnum ) ]( win );
}

//--------------------------------------------------------------------------------------------
/**
 * �r�b�g�}�b�v��\���i�e�L�X�g�A�A�t�B���g��BG�j
 *
 * @param	win
 *
 */
//--------------------------------------------------------------------------------------------
static void BmpWinOff_Normal( GFL_BMPWIN_DATA * win )
{
	ClearWinScrnText( win );
	GFL_BG_LoadScreen(
		win->bgl,
		win->frmnum,
		GFL_BG_ScreenAdrsGet( win->bgl, win->frmnum ),
		GFL_BG_ScreenSizGet( win->bgl, win->frmnum ),
		GFL_BG_ScreenOfsGet( win->bgl, win->frmnum ) );
}

//--------------------------------------------------------------------------------------------
/**
 * �r�b�g�}�b�v��\�����N�G�X�g�i�e�L�X�g�A�A�t�B���g��BG�j
 *
 * @param	win
 *
 */
//--------------------------------------------------------------------------------------------
static void BmpWinOffVReq_Normal( GFL_BMPWIN_DATA * win )
{
	ClearWinScrnText( win );
	GFL_BG_LoadScreenV_Req( win->bgl, win->frmnum );
}

//--------------------------------------------------------------------------------------------
/**
 * �r�b�g�}�b�v��\���i�g�k�ʁj
 *
 * @param	win		BMP�f�[�^�i�[�ꏊ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BmpWinOff_Affine( GFL_BMPWIN_DATA * win )
{
	ClearWinScrnAffine( win );
	GFL_BG_LoadScreen(
		win->bgl, 
		win->frmnum, 
		GFL_BG_ScreenAdrsGet( win->bgl, win->frmnum ),
		GFL_BG_ScreenSizGet( win->bgl, win->frmnum ),
		GFL_BG_ScreenOfsGet( win->bgl, win->frmnum ) );
}

//--------------------------------------------------------------------------------------------
/**
 * �r�b�g�}�b�v��\�����N�G�X�g�i�g�k�ʁj
 *
 * @param	win		BMP�f�[�^�i�[�ꏊ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BmpWinOffVReq_Affine( GFL_BMPWIN_DATA * win )
{
	ClearWinScrnAffine( win );
	GFL_BG_LoadScreenV_Req( win->bgl, win->frmnum );
}

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
void GFL_BMPWIN_DataFill( GFL_BMPWIN_DATA * win, u8 col )
{
	u32	datasize;
	u32 data;
	u32	base_char_siz;

	base_char_siz = GFL_BG_BaseCharSizeGet( win->bgl, win->frmnum );

	if( base_char_siz == GFL_BG_1CHRDATASIZ ){
		col = (col<<4)|col;
	}
	data = (col<<24)|(col<<16)|(col<<8)|col;

	datasize = base_char_siz * win->sizx * win->sizy;

	MI_CpuFillFast( win->chrbuf, data, datasize );
}

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
void GFL_BMPWIN_Print(
			GFL_BMPWIN_DATA * win, void * src,
			u16 src_x, u16 src_y, u16 src_dx, u16 src_dy,
			u16 win_x, u16 win_y, u16 win_dx, u16 win_dy )
{
	GFL_BMPWIN_PrintEx(
		win, src, src_x, src_y, src_dx, src_dy, win_x, win_y, win_dx, win_dy, 0 );
}

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
void GFL_BMPWIN_PrintEx(
		GFL_BMPWIN_DATA * win, void * src,
		u16 src_x, u16 src_y, u16 src_dx, u16 src_dy,
		u16 win_x, u16 win_y, u16 win_dx, u16 win_dy, u16 nuki )
{
	BMPPRT_HEADER	src_data;
	BMPPRT_HEADER	dst_data;

	src_data.adrs	= (u8 *)src;
	src_data.size_x = src_dx;
	src_data.size_y = src_dy;

	dst_data.adrs	= (u8 *)win->chrbuf;
	dst_data.size_x = (u16)(win->sizx * GFL_BG_1CHRDOTSIZ);
	dst_data.size_y = (u16)(win->sizy * GFL_BG_1CHRDOTSIZ);

	if( GFL_BG_ScreenColorModeGet( win->bgl, win->frmnum ) == GX_BG_COLORMODE_16 ){
		GFL_BMP_PrintMain(
			&src_data, &dst_data, src_x, src_y, win_x, win_y, win_dx, win_dy, nuki );
	}else{
		GFL_BMP_PrintMain256(
			&src_data, &dst_data, src_x, src_y, win_x, win_y, win_dx, win_dy, nuki );
	}
}

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
void GFL_BMPWIN_Fill( GFL_BMPWIN_DATA * win, u8 col, u16 px, u16 py, u16 sx, u16 sy )
{
	BMPPRT_HEADER	dst_data;

	dst_data.adrs	= (u8 *)win->chrbuf;
	dst_data.size_x = (u16)(win->sizx * GFL_BG_1CHRDOTSIZ);
	dst_data.size_y = (u16)(win->sizy * GFL_BG_1CHRDOTSIZ);

	if( GFL_BG_ScreenColorModeGet( win->bgl, win->frmnum ) == GX_BG_COLORMODE_16 ){
		GFL_BMP_Fill( (const BMPPRT_HEADER *)&dst_data, px, py, sx, sy, col );
	}else{
		GFL_BMP_Fill256( (const BMPPRT_HEADER *)&dst_data, px, py, sx, sy, col );
	}
}

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
#define	M_NULLPAL_L	(0)
#define	M_NULLPAL_H	(NULLPAL_L << 4)
#define	M_BMPWIN_CHR_DOT	(8)

#define M_SRC_ADRS	(src)
#define M_SRC_POSX	(0)
#define M_SRC_POSY	(0)
#define M_SRC_SIZX	(16)
#define M_SRC_SIZY	(16)
#define	M_SRC_XARG	(((M_SRC_SIZX) + (M_SRC_SIZX & 7))>>3)

#define M_DST_ADRS	(dst)
#define M_DST_POSX	(wx)
#define M_DST_POSY	(wy)
#define M_DST_SIZX	(dsx)
#define M_DST_SIZY	(dsy)
#define	M_DST_XARG	(((M_DST_SIZX) + (M_DST_SIZX & 7))>>3)

#define M_WRT_SIZX	(ssx)
#define M_WRT_SIZY	(ssy)

#define DOT_PUT(xofs,yofs,xsiz,ysiz,dadrs,dpx,dpy,dxarg)				\
	{																	\
		int	sx, dx, sy, dy, shiftval;									\
		u8	src_dat;													\
		u8	*srcadrs, *destadrs;										\
		u32 src_base;													\
																		\
		srcadrs	= M_SRC_ADRS + (yofs / 8 * 0x40) + (xofs / 8 * 0x20);	\
		for(sy=0, dy=dpy+yofs; sy < ysiz; sy++, dy++){					\
			src_base = *(u32*)srcadrs;									\
			for(sx=0, dx=dpx+xofs; sx < xsiz; sx++, dx++){				\
				destadrs = DPPCALC(dadrs, dx, dy, dxarg);				\
																		\
				src_dat = (u8)((src_base >> (sx * 4)) & 0x0f);			\
																		\
				if(src_dat != M_NULLPAL_L){								\
					shiftval = (dx & 1)*4;								\
					*destadrs = (u8)((src_dat << shiftval)|(*destadrs & (0xf0 >> shiftval)));\
				}														\
			}															\
			srcadrs	+= 4;												\
		}																\
	}

#define DOT_PUT_256(xofs,yofs,xsiz,ysiz,dadrs,dpx,dpy,dxarg)			\
	{																	\
		int	sx, dx, sy, dy;												\
		u8	src_dat;													\
		u8	*srcadrs, *destadrs;										\
		u32 src_base;													\
																		\
		srcadrs	= M_SRC_ADRS + (yofs / 8 * 0x40) + (xofs / 8 * 0x40);	\
		for( sy=0, dy=dpy+yofs; sy<ysiz; sy++, dy++ ){					\
			src_base = *(u32*)srcadrs;									\
			for( sx=0, dx=dpx+xofs; sx<xsiz; sx++, dx++ ){				\
				destadrs = DPPCALC_256(dadrs, dx, dy, dxarg);			\
																		\
				src_dat = (u8)((src_base >> (sx * 4)) & 0x0f);			\
																		\
				if( src_dat != M_NULLPAL_L ){							\
					*destadrs = src_dat;								\
				}														\
			}															\
			srcadrs	+= 4;												\
		}																\
	}

void GFL_BMPWIN_PrintMsg( GFL_BMPWIN_DATA * win, u8 * src, u16 ssx, u16 ssy, u16 wx, u16 wy )
{
	u8 * dst; 
	u16	dsx, dsy;
	int	x_max, y_max;
	u8	print_pat;

	dst	= (u8 *)win->chrbuf;
	dsx = (u16)(win->sizx * GFL_BG_1CHRDOTSIZ);
	dsy = (u16)(win->sizy * GFL_BG_1CHRDOTSIZ);

	if((M_DST_SIZX - M_DST_POSX) < M_WRT_SIZX){
		x_max = M_DST_SIZX - M_DST_POSX;
	}else{
		x_max = M_WRT_SIZX;
	}

	if((M_DST_SIZY - M_DST_POSY) < M_WRT_SIZY){
		y_max = M_DST_SIZY - M_DST_POSY;
	}else{
		y_max = M_WRT_SIZY;
	}
	print_pat = 0;

	if( x_max > 8 ){ print_pat |= 1; }
	if( y_max > 8 ){ print_pat |= 2; }

	if( win->bitmode == GFL_BMPWIN_BITMODE_4 ){
		switch( print_pat ){
		case 0:	//�P�L����(8x8)
			DOT_PUT( 0, 0, x_max,	y_max,	 M_DST_ADRS, M_DST_POSX, M_DST_POSY, M_DST_XARG );
			return;
		case 1:	//�Q�L����(16x8)
			DOT_PUT( 0, 0, 8,		y_max,	 M_DST_ADRS, M_DST_POSX, M_DST_POSY, M_DST_XARG );
			DOT_PUT( 8, 0, x_max-8,	y_max,	 M_DST_ADRS, M_DST_POSX, M_DST_POSY, M_DST_XARG );
			return;
		case 2:	//�Q�L����(8x16)
			DOT_PUT( 0, 0, x_max,	8,		 M_DST_ADRS, M_DST_POSX, M_DST_POSY, M_DST_XARG );
			DOT_PUT( 0, 8, x_max,	y_max-8, M_DST_ADRS, M_DST_POSX, M_DST_POSY, M_DST_XARG );
			return;
		case 3:	//�S�L����(16x16)
			DOT_PUT( 0, 0, 8,		8,		 M_DST_ADRS, M_DST_POSX, M_DST_POSY, M_DST_XARG );
			DOT_PUT( 8, 0, x_max-8,	8,		 M_DST_ADRS, M_DST_POSX, M_DST_POSY, M_DST_XARG );
			DOT_PUT( 0, 8, 8,		y_max-8, M_DST_ADRS, M_DST_POSX, M_DST_POSY, M_DST_XARG );
			DOT_PUT( 8, 8, x_max-8,	y_max-8, M_DST_ADRS, M_DST_POSX, M_DST_POSY, M_DST_XARG );
			return;
		}
	}else{
		switch( print_pat ){
		case 0:	//�P�L����(8x8)
			DOT_PUT_256( 0, 0, x_max,	y_max,	 M_DST_ADRS,M_DST_POSX,M_DST_POSY,M_DST_XARG);
			return;
		case 1:	//�Q�L����(16x8)
			DOT_PUT_256( 0, 0, 8,		y_max,	 M_DST_ADRS,M_DST_POSX,M_DST_POSY,M_DST_XARG);
			DOT_PUT_256( 8, 0, x_max-8,	y_max,	 M_DST_ADRS,M_DST_POSX,M_DST_POSY,M_DST_XARG);
			return;
		case 2:	//�Q�L����(8x16)
			DOT_PUT_256( 0, 0, x_max,	8,		 M_DST_ADRS,M_DST_POSX,M_DST_POSY,M_DST_XARG);
			DOT_PUT_256( 0, 8, x_max,	y_max-8, M_DST_ADRS,M_DST_POSX,M_DST_POSY,M_DST_XARG);
			return;
		case 3:	//�S�L����(16x16)
			DOT_PUT_256( 0, 0, 8,		8,		 M_DST_ADRS,M_DST_POSX,M_DST_POSY,M_DST_XARG);
			DOT_PUT_256( 8, 0, x_max-8,	8,		 M_DST_ADRS,M_DST_POSX,M_DST_POSY,M_DST_XARG);
			DOT_PUT_256( 0, 8, 8,		y_max-8, M_DST_ADRS,M_DST_POSX,M_DST_POSY,M_DST_XARG);
			DOT_PUT_256( 8, 8, x_max-8,	y_max-8, M_DST_ADRS,M_DST_POSX,M_DST_POSY,M_DST_XARG);
			return;
		}
	}
}


//=============================================================================================
//=============================================================================================
#define	M_PUT_TBL	( tbl )

#define DOT_PUT_W(xofs,yofs,xsiz,ysiz,dadrs,dpx,dpy,dxarg,ptbl)			\
	{																	\
		int	sx, dx, sy, dy, shiftval;									\
		u8	src_dat;													\
		u8	tbl_flg;													\
		u8	tbl_bit;													\
		u8	*destadrs;													\
		const u8* srcadrs;												\
		u32 src_base;													\
																		\
		srcadrs	= M_SRC_ADRS + (yofs / 8 * 0x40) + (xofs / 8 * 0x20);	\
																		\
		if( yofs == 0 ){												\
			dy = dpy + yofs;											\
			tbl_bit = (u8)( ptbl & 0xff );								\
		}else{															\
			dy = dpy + yofs;											\
			for( sy=0; sy<8; sy++ ){									\
				if( ( (ptbl>>sy) & 1 ) != 0 ){ dy++; }					\
			}															\
			tbl_bit = (u8)( ptbl >> 8 );								\
		}																\
																		\
		for( sy=0; sy<ysiz; sy++ ){										\
			src_base = *(u32*)srcadrs;									\
			tbl_flg = (u8)(( tbl_bit >> sy ) & 1);						\
			for(sx=0, dx=dpx+xofs; sx < xsiz; sx++, dx++){				\
				destadrs = DPPCALC(dadrs, dx, dy, dxarg);				\
																		\
				src_dat = (u8)((src_base >> (sx * 4)) & 0x0f);			\
																		\
				if(src_dat != M_NULLPAL_L){								\
					shiftval = (dx & 1)*4;								\
					src_dat = (u8)((src_dat << shiftval)|(*destadrs & (0xf0 >> shiftval)));\
					*destadrs = src_dat;								\
																		\
					if( tbl_flg != 0 ){									\
						destadrs = DPPCALC(dadrs, dx, (dy+1), dxarg);	\
						*destadrs = src_dat;							\
					}													\
				}														\
			}															\
			if( tbl_flg != 0 ){											\
				dy += 2;												\
			}else{														\
				dy++;													\
			}															\
			srcadrs	+= 4;												\
		}																\
	}
/*
#define DOT_PUT_W_256(xofs,yofs,xsiz,ysiz,dadrs,dpx,dpy,dxarg,ptbl)		\
	{																	\
		int	sx, dx, sy, dy, shiftval;									\
		u8	src_dat;													\
		u8	tbl_flg;													\
		u8	tbl_bit;													\
		u8	*destadrs;													\
		const u8* srcadrs;												\
		u32 src_base;													\
																		\
		srcadrs	= M_SRC_ADRS + (yofs / 8 * 0x40) + (xofs / 8 * 0x20);	\
																		\
		if( yofs == 0 ){												\
			dy = dpy + yofs;											\
			tbl_bit = (u8)( ptbl & 0xff );								\
		}else{															\
			dy = dpy + yofs;											\
			for( sy=0; sy<8; sy++ ){									\
				if( ( (ptbl>>sy) & 1 ) != 0 ){ dy++; }					\
			}															\
			tbl_bit = (u8)( ptbl >> 8 );								\
		}																\
																		\
		for( sy=0; sy<ysiz; sy++ ){										\
			src_base = *(u32*)srcadrs;									\
			tbl_flg = (u8)(( tbl_bit >> sy ) & 1);						\
			for(sx=0, dx=dpx+xofs; sx < xsiz; sx++, dx++){				\
				destadrs = DPPCALC_256(dadrs, dx, dy, dxarg);			\
																		\
				src_dat = (u8)((src_base >> (sx * 4)) & 0x0f);			\
																		\
				if(src_dat != M_NULLPAL_L){								\
					shiftval = (dx & 1)*4;								\
					*destadrs = src_dat;								\
																		\
					if( tbl_flg != 0 ){									\
						destadrs = DPPCALC_256(dadrs,dx,(dy+1),dxarg);	\
						*destadrs = src_dat;							\
					}													\
				}														\
			}															\
			if( tbl_flg != 0 ){											\
				dy += 2;												\
			}else{														\
				dy++;													\
			}															\
			srcadrs	+= 4;												\
		}																\
	}
*/

#define CM_SRC_ADRS	(chg_src)

#define DOT_PUT_W_256(xofs,yofs,xsiz,ysiz,dadrs,dpx,dpy,dxarg,ptbl)		\
	{																	\
		int	sx, dx, sy, dy;												\
		u8	src_dat;													\
		u8	tbl_flg;													\
		u8	tbl_bit;													\
		u8	*destadrs;													\
		const u8* srcadrs;												\
		u8 * src_base;													\
																		\
		srcadrs	= CM_SRC_ADRS + (yofs / 8 * 0x80) + (xofs / 8 * 0x40);	\
																		\
		if( yofs == 0 ){												\
			dy = dpy + yofs;											\
			tbl_bit = (u8)( ptbl & 0xff );								\
		}else{															\
			dy = dpy + yofs;											\
			for( sy=0; sy<8; sy++ ){									\
				if( ( (ptbl>>sy) & 1 ) != 0 ){ dy++; }					\
			}															\
			tbl_bit = (u8)( ptbl >> 8 );								\
		}																\
																		\
		for( sy=0; sy<ysiz; sy++ ){										\
			src_base = (u8 *)srcadrs;									\
			tbl_flg = (u8)(( tbl_bit >> sy ) & 1);						\
			for(sx=0, dx=dpx+xofs; sx < xsiz; sx++, dx++){				\
				destadrs = DPPCALC_256(dadrs, dx, dy, dxarg);			\
																		\
				src_dat = src_base[sx];									\
																		\
				if(src_dat != M_NULLPAL_L){								\
					*destadrs = src_dat;								\
																		\
					if( tbl_flg != 0 ){									\
						destadrs = DPPCALC_256(dadrs,dx,(dy+1),dxarg);	\
						*destadrs = src_dat;							\
					}													\
				}														\
			}															\
			if( tbl_flg != 0 ){											\
				dy += 2;												\
			}else{														\
				dy++;													\
			}															\
			srcadrs	+= 8;												\
		}																\
	}

void GFL_BMPWIN_PrintMsgWide(
		GFL_BMPWIN_DATA * win, const u8 * src, u16 ssx, u16 ssy, u16 wx, u16 wy, u16 tbl )
{
	u8 * dst;
	u16	dsx,dsy;
	int	x_max, y_max;
	u8	print_pat;

	dst	= (u8 *)win->chrbuf;
	dsx = (u16)(win->sizx * GFL_BG_1CHRDOTSIZ);
	dsy = (u16)(win->sizy * GFL_BG_1CHRDOTSIZ);

	if((M_DST_SIZX - M_DST_POSX) < M_WRT_SIZX){
		x_max = M_DST_SIZX - M_DST_POSX;
	}else{
		x_max = M_WRT_SIZX;
	}

	if((M_DST_SIZY - M_DST_POSY) < M_WRT_SIZY){
		y_max = M_DST_SIZY - M_DST_POSY;
	}else{
		y_max = M_WRT_SIZY;
	}

	print_pat = 0;

	if( x_max > 8 ){ print_pat |= 1; }
	if( y_max > 8 ){ print_pat |= 2; }

	if( win->bitmode == GFL_BMPWIN_BITMODE_4 ){
		switch( print_pat ){
		case 0:	//�P�L����(8x8)
			DOT_PUT_W( 0, 0, x_max,	y_max, M_DST_ADRS, M_DST_POSX, M_DST_POSY, M_DST_XARG, M_PUT_TBL );
			return;
		case 1:	//�Q�L����(16x8)
			DOT_PUT_W( 0, 0, 8, y_max, M_DST_ADRS, M_DST_POSX, M_DST_POSY, M_DST_XARG, M_PUT_TBL );
			DOT_PUT_W( 8, 0, x_max-8, y_max, M_DST_ADRS, M_DST_POSX, M_DST_POSY, M_DST_XARG, M_PUT_TBL );
			return;
		case 2:	//�Q�L����(8x16)
			DOT_PUT_W( 0, 0, x_max,	8, M_DST_ADRS, M_DST_POSX, M_DST_POSY, M_DST_XARG, M_PUT_TBL );
			DOT_PUT_W( 0, 8, x_max,	y_max-8, M_DST_ADRS, M_DST_POSX, M_DST_POSY, M_DST_XARG, M_PUT_TBL );
			return;
		case 3:	//�S�L����(16x16)
			DOT_PUT_W( 0, 0, 8, 8, M_DST_ADRS, M_DST_POSX, M_DST_POSY, M_DST_XARG, M_PUT_TBL );
			DOT_PUT_W( 8, 0, x_max-8, 8, M_DST_ADRS, M_DST_POSX, M_DST_POSY, M_DST_XARG, M_PUT_TBL );
			DOT_PUT_W( 0, 8, 8, y_max-8, M_DST_ADRS, M_DST_POSX, M_DST_POSY, M_DST_XARG, M_PUT_TBL );
			DOT_PUT_W( 8, 8, x_max-8, y_max-8, M_DST_ADRS, M_DST_POSX, M_DST_POSY, M_DST_XARG, M_PUT_TBL );
			return;
		}
	}else{
		u8 * chg_src = GFL_BG_4BitCgxChange8Bit( src, ssx*4*ssy*8, win->palnum, win->heapID );

		switch( print_pat ){
		case 0:	//�P�L����(8x8)
			DOT_PUT_W_256( 0, 0, x_max,	y_max, M_DST_ADRS, M_DST_POSX, M_DST_POSY, M_DST_XARG, M_PUT_TBL );
			break;
		case 1:	//�Q�L����(16x8)
			DOT_PUT_W_256( 0, 0, 8, y_max, M_DST_ADRS, M_DST_POSX, M_DST_POSY, M_DST_XARG, M_PUT_TBL );
			DOT_PUT_W_256( 8, 0, x_max-8, y_max, M_DST_ADRS, M_DST_POSX, M_DST_POSY, M_DST_XARG, M_PUT_TBL );
			break;
		case 2:	//�Q�L����(8x16)
			DOT_PUT_W_256( 0, 0, x_max,	8, M_DST_ADRS, M_DST_POSX, M_DST_POSY, M_DST_XARG, M_PUT_TBL );
			DOT_PUT_W_256( 0, 8, x_max,	y_max-8, M_DST_ADRS, M_DST_POSX, M_DST_POSY, M_DST_XARG, M_PUT_TBL );
			break;
		case 3:	//�S�L����(16x16)
			DOT_PUT_W_256( 0, 0, 8, 8, M_DST_ADRS, M_DST_POSX, M_DST_POSY, M_DST_XARG, M_PUT_TBL );
			DOT_PUT_W_256( 8, 0, x_max-8, 8, M_DST_ADRS, M_DST_POSX, M_DST_POSY, M_DST_XARG, M_PUT_TBL );
			DOT_PUT_W_256( 0, 8, 8, y_max-8, M_DST_ADRS, M_DST_POSX, M_DST_POSY, M_DST_XARG, M_PUT_TBL );
			DOT_PUT_W_256( 8, 8, x_max-8, y_max-8, M_DST_ADRS, M_DST_POSX, M_DST_POSY, M_DST_XARG, M_PUT_TBL );
			break;
		}

		sys_FreeMemoryEz( chg_src );
	}
}

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
void GFL_BMPWIN_Shift( GFL_BMPWIN_DATA * win, u8 direct, u8 offset, u8 data )
{
	if( GFL_BG_ScreenColorModeGet( win->bgl, win->frmnum) == GX_BG_COLORMODE_16 ){
		BmpWinShift16( win, direct, offset, data );
	}else{
		BmpWinShift256( win, direct, offset, data );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �r�b�g�}�b�v�E�C���h�E�f�[�^�V�t�g�i�P�U�F�p�j
 *
 * @param	win		BMP�f�[�^�i�[�ꏊ
 * @param	direct	����
 * @param	offset	�V�t�g��
 * @param	data	�󔒂ɂȂ����̈�𖄂߂�f�[�^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BmpWinShift16( GFL_BMPWIN_DATA * win, u8 direct, u8 offset, u8 data )
{
	u8 * cgxram;
	int	chroffs, woffs, roffs;
	int	blanch_chr, over_offs;
	int	xsiz, rline;
	int	i;

	cgxram		= (u8 *)win->chrbuf;
	blanch_chr	= (data<<24) | (data<<16) | (data<<8) | (data<<0);
	over_offs	= win->sizy * win->sizx * GFL_BG_1CHRDATASIZ;
	xsiz		= win->sizx;

	switch( direct ){
	case GFL_BMPWIN_SHIFT_U:
		for( chroffs=0; chroffs<over_offs; chroffs+=GFL_BG_1CHRDATASIZ ){
			rline = offset;

			for( i=0; i<8; i++ ){
				woffs = chroffs + (i << 2);
				roffs = chroffs + (((xsiz * (rline&0xfffffff8))|(rline&0x00000007)) << 2);
				if(roffs < over_offs){
					*(u32*)(cgxram + woffs) = *(u32*)(cgxram + roffs);
				}else{
					*(u32*)(cgxram + woffs) = blanch_chr;
				}
				rline++;
			}
		}
		break;
	case GFL_BMPWIN_SHIFT_D:
		cgxram += over_offs - 4;

		for( chroffs=0; chroffs<over_offs; chroffs+=GFL_BG_1CHRDATASIZ ){
			rline = offset;

			for( i=0; i<8; i++ ){
				woffs = chroffs + (i << 2);
				roffs = chroffs + (((xsiz * (rline&0xfffffff8))|(rline&0x00000007)) << 2);
				if(roffs < over_offs){
					*(u32*)(cgxram - woffs) = *(u32*)(cgxram - roffs);
				}else{
					*(u32*)(cgxram - woffs) = blanch_chr;
				}
				rline++;
			}
		}
		break;
	case GFL_BMPWIN_SHIFT_L:
		break;
	case GFL_BMPWIN_SHIFT_R:
		break;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �r�b�g�}�b�v�E�C���h�E�f�[�^�V�t�g�i�Q�T�U�F�p�j
 *
 * @param	win		BMP�f�[�^�i�[�ꏊ
 * @param	direct	����
 * @param	offset	�V�t�g��
 * @param	data	�󔒂ɂȂ����̈�𖄂߂�f�[�^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BmpWinShift256( GFL_BMPWIN_DATA * win, u8 direct, u8 offset, u8 data )
{
	u8 * cgxram;
	int	chroffs, woffs, roffs;
	int	blanch_chr, over_offs;
	int	xsiz, rline;
	int	i;

	cgxram		= (u8 *)win->chrbuf;
	blanch_chr	= ( data << 24 ) | ( data << 16 ) | ( data << 8 ) | data;
	over_offs	= win->sizy * win->sizx * GFL_BG_8BITCHRSIZ;
	xsiz		= win->sizx;

	switch(direct){
	case GFL_BMPWIN_SHIFT_U:
		for( chroffs=0; chroffs<over_offs; chroffs+=GFL_BG_8BITCHRSIZ ){
			rline = offset;

			for( i=0; i<8; i++ ){
				woffs = chroffs + (i << 3);
				roffs = chroffs + (((xsiz * (rline&0xfffffff8))|(rline&0x00000007)) << 3);
				if( roffs < over_offs ){
					*(u32*)(cgxram + woffs) = *(u32*)(cgxram + roffs);
				}else{
					*(u32*)(cgxram + woffs) = blanch_chr;
				}

				woffs += 4;
				roffs += 4;
				if( roffs < over_offs + 4 ){
					*(u32*)(cgxram + woffs) = *(u32*)(cgxram + roffs);
				}else{
					*(u32*)(cgxram + woffs) = blanch_chr;
				}

				rline++;
			}
		}
		break;
	case GFL_BMPWIN_SHIFT_D:
		cgxram += ( over_offs - 8 );

		for( chroffs=0; chroffs<over_offs; chroffs+=GFL_BG_8BITCHRSIZ ){
			rline = offset;

			for( i=0; i<8; i++ ){
				woffs = chroffs + (i << 3);
				roffs = chroffs + (((xsiz * (rline&0xfffffff8))|(rline&0x00000007)) << 3);
				if( roffs < over_offs ){
					*(u32*)(cgxram - woffs) = *(u32*)(cgxram - roffs);
				}else{
					*(u32*)(cgxram - woffs) = blanch_chr;
				}

				woffs -= 4;
				roffs -= 4;
				if( roffs < over_offs - 4 ){
					*(u32*)(cgxram - woffs) = *(u32*)(cgxram - roffs);
				}else{
					*(u32*)(cgxram - woffs) = blanch_chr;
				}

				rline++;
			}
		}
		break;
	case GFL_BMPWIN_SHIFT_L:
		break;
	case GFL_BMPWIN_SHIFT_R:
		break;
	}
}

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
u8 GFL_BMPWIN_GetFrame( GFL_BMPWIN_DATA * win )
{
	return win->frmnum;
}

//--------------------------------------------------------------------------------------------
/**
 * BMP�f�[�^�擾�FX�T�C�Y
 *
 * @param	win		BMP�f�[�^�i�[�ꏊ
 *
 * @return	win->sizx
 */
//--------------------------------------------------------------------------------------------
u8 GFL_BMPWIN_GetSizeX( GFL_BMPWIN_DATA * win )
{
	return win->sizx;
}

//--------------------------------------------------------------------------------------------
/**
 * BMP�f�[�^�擾�FY�T�C�Y
 *
 * @param	win		BMP�f�[�^�i�[�ꏊ
 *
 * @return	win->sizy
 */
//--------------------------------------------------------------------------------------------
u8 GFL_BMPWIN_GetSizeY( GFL_BMPWIN_DATA * win )
{
	return win->sizy;
}

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
u8 GFL_BMPWIN_GetPosX( GFL_BMPWIN_DATA * win )
{
	return win->posx;
}

//--------------------------------------------------------------------------------------------
/**
 * BMP�f�[�^�擾�FY���W
 *
 * @param	win		BMP�f�[�^�i�[�ꏊ
 *
 * @return	win->posy
 */
//--------------------------------------------------------------------------------------------
u8 GFL_BMPWIN_GetPosY( GFL_BMPWIN_DATA * win )
{
	return win->posy;
}

//--------------------------------------------------------------------------------------------
/**
 * BMP�f�[�^�擾�F�L�����ԍ�
 *
 * @param	win		BMP�f�[�^�i�[�ꏊ
 *
 * @return	win->chrofs
 */
//--------------------------------------------------------------------------------------------
u16 GFL_BMPWIN_GetChrofs( GFL_BMPWIN_DATA * win )
{
	return win->chrofs;
}

//--------------------------------------------------------------------------------------------
/**
 * BMP�f�[�^�ݒ�FX���W
 *
 * @param	win		BMP�f�[�^�i�[�ꏊ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void GFL_BMPWIN_SetPosX( GFL_BMPWIN_DATA * win, u8 px )
{
	win->posx = px;
}

//--------------------------------------------------------------------------------------------
/**
 * BMP�f�[�^�ݒ�FY���W
 *
 * @param	win		BMP�f�[�^�i�[�ꏊ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void GFL_BMPWIN_SetPosY( GFL_BMPWIN_DATA * win, u8 py )
{
	win->posy = py;
}

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
void GFL_BMPWIN_SetPal( GFL_BMPWIN_DATA * win, u8 palnum )
{
	win->palnum = palnum;
}
