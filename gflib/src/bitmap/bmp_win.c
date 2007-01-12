//=============================================================================================
/**
 *
 * @file	bmp_win.c
 * @brief	�r�b�g�}�b�v�E�B���h�E�V�X�e��
 *
 */
//=============================================================================================
#include "gflib.h"

//=============================================================================================
//	�^�錾
//=============================================================================================
#define	GFL_BMPWIN_MAGICNUM		(15676)

///�̈�Ǘ��\����
typedef struct {
	GFL_AREAMAN*	man;
	u32				start;
	u32				size;

}GFL_BMPWIN_AREA;

///BMPWIN�f�[�^�\����
typedef struct {
	GFL_BG_INI*		bgl;
	GFL_BMPWIN_AREA	area[ GFL_BG_FRAME_MAX ];
	u16				heapID;

}GFL_BMPWIN_SYS;

///BMPWIN�f�[�^�\����
struct _GFL_BMPWIN {
	u16				magicnum:15;
	u16				bitmode:1;	
	u8				frmnum;
	u8				posx;
	u8				posy;
	u8				sizx;
	u8				sizy;
	u8				width;
	u8				height;
	u8				palnum;
	u16				chrnum;
	GFL_BMP_DATA	bmp;
};

static GFL_BMPWIN_SYS* bmpwin_sys = NULL;
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
/**
 *
 * �r�b�g�}�b�v�Ǘ��V�X�e���̊J�n�A�I��
 *
 * ��Ƀv���Z�X�N���A�I�����ɌĂ΂��B
 * �r�b�g�}�b�v�E�C���h�E���g�p����a�f�t���[���i�����w��j�A�g�p�b�f�w�̈�̐ݒ���s���B
 * ����Ȍ�A�e�r�b�g�}�b�v�쐬�̍ۂ́A�����Őݒ肵���X�e�[�^�X���p������B
 */
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
/**
 * �V�X�e��������
 *
 * @param	bgl		BG�V�X�e���\���̃|�C���^
 * @param	set		�g�p�̈�ݒ�\����
 * @param	heapID	�g�p�q�[�v�̈�
 */
//--------------------------------------------------------------------------------------------
void
	GFL_BMPWIN_sysInit
		( GFL_BG_INI* bgl, GFL_BMPWIN_SET* set, u16 heapID )
{
	int	i;

	bmpwin_sys = (GFL_BMPWIN_SYS*)GFL_HEAP_AllocMemory( heapID, sizeof(GFL_BMPWIN_SYS) );
	bmpwin_sys->bgl = bgl;
	bmpwin_sys->heapID = heapID;

	//�̈�Ǘ��}�l�[�W���̍쐬
	for( i=0; i<GFL_BG_FRAME_MAX;i++ ){
		u32 blocksize = ( set->limit[i].end - set->limit[i].start ) / 0x20;

		if( blocksize ){
			//�r�b�g�}�b�v�E�C���h�E���g�p����t���[��(enable)
			bmpwin_sys->area[i].man = GFL_AREAMAN_Create( blocksize, heapID );
			bmpwin_sys->area[i].start = 0;
			bmpwin_sys->area[i].size = blocksize;
			//�����L�����N�^�[�p�Ƀf�t�H���g�m��
			GFL_AREAMAN_ReserveAssignArea( bmpwin_sys->area[i].man, 0, blocksize, 1 );
		} else {
			//�r�b�g�}�b�v�E�C���h�E���g�p���Ȃ��t���[��(disable)
			bmpwin_sys->area[i].man = NULL;
			bmpwin_sys->area[i].start = 0;
			bmpwin_sys->area[i].size = 0;
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �V�X�e���I��
 */
//--------------------------------------------------------------------------------------------
void
	GFL_BMPWIN_sysExit
		( void )
{
	int	i;

	//�̈�Ǘ��}�l�[�W���̉��
	for( i=0; i<GFL_BG_FRAME_MAX;i++ ){
		if( bmpwin_sys->area[i].man != NULL ){
			GFL_AREAMAN_Delete( bmpwin_sys->area[i].man );
		}
	}
	GFL_HEAP_FreeMemory( bmpwin_sys );
	bmpwin_sys = NULL;
}


//--------------------------------------------------------------------------------------------
/**
 *
 * �r�b�g�}�b�v�E�C���h�E�̍쐬�A�j��
 *
 * �r�b�g�}�b�v�Ƃ��Ďg�p�������a�f�t���[���A�`��ʒu�A�g�p�b�f�w�T�C�Y�̐ݒ�Ȃǂ��w�肵
 * �r�b�g�}�b�v�E�C���h�E�Ǘ��n���h�����쐬����B
 * �����łb�f�w�o�b�t�@�̈�̃q�[�v�m�ۂ��s���B
 * �쐬��̃r�b�g�}�b�v�E�C���h�E����́A�n���h���|�C���^�ɂ���čs����B
 */
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
static inline void CHK_BMPWIN( GFL_BMPWIN* bmpwin )
{
	if( bmpwin->magicnum != GFL_BMPWIN_MAGICNUM ){
		OS_Panic( "�w�肳�ꂽ�n���h����GFL_BMPWIN�n���h���ł͂���܂���(bmp_win.c)\n" );
	}
}
//--------------------------------------------------------------------------------------------
/**
 *
 * �r�b�g�}�b�v�E�C���h�E�̍쐬
 *
 * @param	frmnum	GFL_BGL�g�p�t���[��
 * @param	posx	�w���W�i�L�����N�^�[�P�ʁj
 * @param	posy	�x���W�i�L�����N�^�[�P�ʁj
 * @param	sizx	�w�T�C�Y�i�L�����N�^�[�P�ʁj
 * @param	sizy	�x�T�C�Y�i�L�����N�^�[�P�ʁj
 * @param	panum	�g�p�p���b�g�i���o�[
 * @param	dir		�m�ۂu�q�`�l����
 *
 * @return	�r�b�g�}�b�v�E�C���h�E�n���h���|�C���^
 */
//--------------------------------------------------------------------------------------------
GFL_BMPWIN* 
	GFL_BMPWIN_Create
		( u8 frmnum, u8 posx, u8 posy, u8 sizx, u8 sizy, u8 palnum, u8 dir )
{
	GFL_BMPWIN* bmpwin;
	u32 areasiz, areapos;
	u16 heapID = bmpwin_sys->heapID;

	bmpwin = (GFL_BMPWIN*)GFL_HEAP_AllocMemory( heapID, sizeof(GFL_BMPWIN) );

	bmpwin->magicnum	= GFL_BMPWIN_MAGICNUM;
	bmpwin->frmnum		= frmnum;
	bmpwin->posx		= posx;
	bmpwin->posy		= posy;
	bmpwin->sizx		= sizx;
	bmpwin->sizy		= sizy;
	bmpwin->width		= sizx;	//�X�N���[���`��T�C�Y�i�f�t�H���g�̓T�C�Y�Ɠ������j
	bmpwin->height		= sizy;	//�X�N���[���`��T�C�Y�i�f�t�H���g�̓T�C�Y�Ɠ������j
	bmpwin->palnum		= palnum;

	//�L�����N�^���[�h����
	if( GFL_BG_ScreenColorModeGet( bmpwin_sys->bgl, frmnum) == GX_BG_COLORMODE_16){
		bmpwin->bitmode = GFL_BMPWIN_BITMODE_4;	//�P�U�F���[�h
		areasiz = sizx * sizy;
	} else {
		bmpwin->bitmode = GFL_BMPWIN_BITMODE_8;	//�Q�T�U�F���[�h
		areasiz = sizx * sizy * 2;	//�g�p����L�����̈�͂Q�{(0x40)
	}
	if( bmpwin_sys->area[frmnum].man == NULL ){
		OS_Panic( "�r�b�g�}�b�v�����\�ȃt���[���ł͂���܂���\n" );
	}
	//�L�����N�^�[�̈�̊m��
	areapos = GFL_AREAMAN_ReserveAssignArea(	bmpwin_sys->area[frmnum].man,
												bmpwin_sys->area[frmnum].start,
												bmpwin_sys->area[frmnum].size,
												areasiz );
	if( areapos == AREAMAN_POS_NOTFOUND ){
		OS_Panic( "�r�b�g�}�b�v�����ɕK�v�ȃL�����u�q�`�l�̈悪����Ȃ�\n" );
	} else {
		bmpwin->chrnum = areapos;
		bmpwin->bmp.adrs = GFL_HEAP_AllocMemory( heapID, areasiz * 0x20 );
		bmpwin->bmp.size_x = sizx * 8;
		bmpwin->bmp.size_y = sizy * 8;
	}
	return bmpwin;
}


//--------------------------------------------------------------------------------------------
/**
 *
 * �r�b�g�}�b�v�E�C���h�E�̔j��
 *
 * @param	bmpwin	�r�b�g�}�b�v�E�C���h�E�|�C���^
 *
 */
//--------------------------------------------------------------------------------------------
void
	GFL_BMPWIN_Delete
		( GFL_BMPWIN* bmpwin )
{
	u32 areasiz;

	CHK_BMPWIN( bmpwin );

	if(	bmpwin->bitmode == GFL_BMPWIN_BITMODE_4 ){
		areasiz = bmpwin->sizx * bmpwin->sizy;
	} else {
		areasiz = bmpwin->sizx * bmpwin->sizy * 2;	//�g�p����L�����̈�͂Q�{(0x40)
	}
	//�L�����N�^�[�̈�̉��
	GFL_AREAMAN_Release( bmpwin_sys->area[bmpwin->frmnum].man, bmpwin->chrnum, areasiz );

	GFL_HEAP_FreeMemory( bmpwin->bmp.adrs );
	GFL_HEAP_FreeMemory( bmpwin );
}


//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
/**
 *
 *
 * �`�搧��
 *
 *
 */
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//---------------------------------------------------------
/**
 * �r�b�g�}�b�v�p�L�����N�^�[������o�b�t�@����u�q�`�l�֓]��
 *
 * @param	bmpwin	�r�b�g�}�b�v�E�C���h�E�|�C���^
 */
//---------------------------------------------------------
void
	GFL_BMPWIN_UploadChar
		( GFL_BMPWIN * bmpwin )
{
	u32	bmpsize;

	CHK_BMPWIN( bmpwin );

	bmpsize = (u32)
		((bmpwin->sizx*bmpwin->sizy * GFL_BG_BaseCharSizeGet(bmpwin_sys->bgl,bmpwin->frmnum)));

	GFL_BG_LoadCharacter
		( bmpwin_sys->bgl, bmpwin->frmnum, bmpwin->bmp.adrs, bmpsize, bmpwin->chrnum );
}

//---------------------------------------------------------
/**
 * �r�b�g�}�b�v�p�X�N���[�����֘A�a�f�X�N���[���o�b�t�@�ɍ쐬����
 *
 * @param	bmpwin	�r�b�g�}�b�v�E�C���h�E�|�C���^
 */
//---------------------------------------------------------
void
	GFL_BMPWIN_MakeScrn
		( GFL_BMPWIN * bmpwin )
{
	u16*	scrnbuf;
	u16		scrnsiz,scrnchr,scrnpal;
	int		i;

	CHK_BMPWIN( bmpwin );

	scrnsiz = bmpwin->sizx * bmpwin->sizy;
	scrnbuf = (u16*)GFL_HEAP_AllocMemoryLow( bmpwin_sys->heapID, scrnsiz*2 );
	scrnchr = bmpwin->chrnum;
	scrnpal = (bmpwin->palnum << 12);
				
	for(i=0;i<scrnsiz;i++){
		scrnbuf[i] = (scrnchr|scrnpal);
		scrnchr++;
	}
	GFL_BG_ScrWrite( bmpwin_sys->bgl, bmpwin->frmnum, scrnbuf, 
						bmpwin->posx, bmpwin->posy, bmpwin->sizx, bmpwin->sizy );

	GFL_HEAP_FreeMemory( scrnbuf );
}

//---------------------------------------------------------
/**
 * �֘A�a�f�X�N���[���o�b�t�@�̃r�b�g�}�b�v�p�X�N���[�����N���A����
 *
 * @param	bmpwin	�r�b�g�}�b�v�E�C���h�E�|�C���^
 */
//---------------------------------------------------------
void
	GFL_BMPWIN_ClearScrn
		( GFL_BMPWIN * bmpwin )
{
	CHK_BMPWIN( bmpwin );

	CHK_BMPWIN( bmpwin );

	GFL_BG_ScrFill( bmpwin_sys->bgl, bmpwin->frmnum, 0,
					bmpwin->posx, bmpwin->posy, bmpwin->sizx, bmpwin->sizy, 0 );
}


//--------------------------------------------------------------------------------------------
/**
 * �r�b�g�}�b�v�E�C���h�E�X�e�[�^�X�擾
 */
//--------------------------------------------------------------------------------------------
//--------------------------------
/**
 * �t���[���i���o�[�̎擾
 */
//--------------------------------
u8
	GFL_BMPWIN_GetFrame
		( GFL_BMPWIN * bmpwin )
{
	CHK_BMPWIN( bmpwin );
	return bmpwin->frmnum;
}

//--------------------------------
/**
 * �L�����N�^�[�w�T�C�Y�̎擾
 */
//--------------------------------
u8
	GFL_BMPWIN_GetSizeX
		( GFL_BMPWIN * bmpwin )
{
	CHK_BMPWIN( bmpwin );
	return bmpwin->sizx;
}

//--------------------------------
/**
 * �L�����N�^�[�x�T�C�Y�̎擾
 */
//--------------------------------
u8
	GFL_BMPWIN_GetSizeY
		( GFL_BMPWIN * bmpwin )
{
	CHK_BMPWIN( bmpwin );
	return bmpwin->sizy;
}

//--------------------------------
/**
 * �`��w���W�̎擾
 */
//--------------------------------
u8
	GFL_BMPWIN_GetPosX
		( GFL_BMPWIN * bmpwin )
{
	CHK_BMPWIN( bmpwin );
	return bmpwin->posx;
}

//--------------------------------
/**
 * �`��x���W�̎擾
 */
//--------------------------------
u8
	GFL_BMPWIN_GetPosY
		( GFL_BMPWIN * bmpwin )
{
	CHK_BMPWIN( bmpwin );
	return bmpwin->posy;
}

//--------------------------------
/**
 * �L�����N�^�[�i���o�[�̎擾
 */
//--------------------------------
u16
	GFL_BMPWIN_GetChrNum
		( GFL_BMPWIN * bmpwin )
{
	CHK_BMPWIN( bmpwin );
	return bmpwin->chrnum;
}

//--------------------------------
/**
 * �r�b�g�}�b�v�|�C���^�̎擾
 */
//--------------------------------
GFL_BMP_DATA*
	GFL_BMPWIN_GetBmp
		( GFL_BMPWIN * bmpwin )
{
	CHK_BMPWIN( bmpwin );
	return &bmpwin->bmp;
}


//--------------------------------------------------------------------------------------------
/**
 * �r�b�g�}�b�v�E�C���h�E�X�e�[�^�X�ύX
 */
//--------------------------------------------------------------------------------------------
//--------------------------------
/**
 * �`��w���W�̕ύX
 */
//--------------------------------
void
	GFL_BMPWIN_SetPosX
		( GFL_BMPWIN * bmpwin, u8 px )
{
	CHK_BMPWIN( bmpwin );
	bmpwin->posx = px;
}
//--------------------------------
/**
 * �`��x���W�̕ύX
 */
//--------------------------------
void
	GFL_BMPWIN_SetPosY
		( GFL_BMPWIN * bmpwin, u8 py )
{
	CHK_BMPWIN( bmpwin );
	bmpwin->posy = py;
}
//--------------------------------
/**
 * �X�N���[���`��T�C�Y�w�̕ύX
 */
//--------------------------------
void
	GFL_BMPWIN_SetScrnSizX
		( GFL_BMPWIN * bmpwin, u8 sx )
{
	CHK_BMPWIN( bmpwin );
	bmpwin->width = sx;
}
//--------------------------------
/**
 * �X�N���[���`��T�C�Y�x�̕ύX
 */
//--------------------------------
void
	GFL_BMPWIN_SetScrnSizY
		( GFL_BMPWIN * bmpwin, u8 sy )
{
	CHK_BMPWIN( bmpwin );
	bmpwin->height = sy;
}
//--------------------------------
/**
 * �p���b�g�̕ύX
 */
//--------------------------------
void
	GFL_BMPWIN_SetPal
		( GFL_BMPWIN * bmpwin, u8 palnum )
{
	CHK_BMPWIN( bmpwin );
	bmpwin->palnum = palnum;
}


