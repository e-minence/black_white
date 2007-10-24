//=============================================================================================
/**
 * @file	bmp.c
 * @brief	BMP�V�X�e��
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

#include "bmp.h"
#include "bg_sys.h"

#define	GFL_BMP_NORMAL		(0)		//GFL_BMP_Create�Ő���
#define	GFL_BMP_WITH_DATA	(1)		//GFL_BMP_CreateWithData�Ő���

struct _GFL_BMP_DATA{
	u8 * adrs;			///<�L�����N�^�f�[�^�̐擪�A�h���X
	u16	size_x;			///<�L�����N�^�f�[�^��X�T�C�Y
	u16	size_y;			///<�L�����N�^�f�[�^��Y�T�C�Y
	u16	col;			///<�J���[���[�h�iGFL_BMP_16_COLOR:16�F�@GFL_BMP_256_COLOR:256�F�j
	u16	create_flag;	///<BMPCreate���@�t���O�iGFL_BMP_NORMAL:GFL_BMP_Create�@GFL_BMP_WITH_DATA:GFL_BMP_CreateWithData�j
};

static	void GFL_BMP_Print16( const GFL_BMP_DATA * src, GFL_BMP_DATA * dest, u16 pos_sx, u16 pos_sy, s16 pos_dx, s16 pos_dy,
			u16 size_x, u16 size_y, u16 nuki_col );

static	void GFL_BMP_Print256( const GFL_BMP_DATA * src, GFL_BMP_DATA * dest, u16 pos_sx, u16 pos_sy, s16 pos_dx, s16 pos_dy,
		u16 size_x, u16 size_y, u16 nuki_col );

static	void GFL_BMP_Fill16(
		GFL_BMP_DATA * dest,
		s16 pos_dx, s16 pos_dy, u16 size_x, u16 size_y, u8 col_code );

static	void GFL_BMP_Fill256(
		GFL_BMP_DATA * dest,
		s16 pos_dx, s16 pos_dy, u16 size_x, u16 size_y, u8 col_code );

//=============================================================================================
//=============================================================================================
//	�r�b�g�}�b�v�֘A
//=============================================================================================
//=============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * BMP�̈搶��
 *
 * @param	sizex	X�T�C�Y�i�L�����P�ʁj
 * @param	sizey	Y�T�C�Y�i�L�����P�ʁj
 * @param	col		�J���[���[�h�iGFL_BMP_16_COLOR:16�F�@GFL_BMP_256_COLOR:256�F�j
 * @param	heapID	�q�[�v�h�c
 *
 * @return	�擾�����������̃A�h���X
 */
//--------------------------------------------------------------------------------------------
GFL_BMP_DATA * GFL_BMP_Create( int sizex, int sizey, int col, HEAPID heapID )
{
	GFL_BMP_DATA * bmp = GFL_HEAP_AllocMemory( heapID, sizeof(GFL_BMP_DATA) );

	// �v�Z�v���Z�X�ȗ��̂��߁A�����ł̓h�b�g�P�ʂŕێ�����
	bmp->size_x			=	sizex * 8;
	bmp->size_y			=	sizey * 8;
	bmp->col			=	col;
	bmp->adrs			=	GFL_HEAP_AllocClearMemory( heapID, sizex * sizey * col );
	bmp->create_flag	=	GFL_BMP_NORMAL;

	return bmp;
}

//--------------------------------------------------------------------------------------------
/**
 * BMP�̈�J��
 *
 * @param	bmp		�V�X�e�����[�N�G���A�ւ̃|�C���^
 */
//--------------------------------------------------------------------------------------------
void	GFL_BMP_Delete( GFL_BMP_DATA *bmp )
{
	if(bmp->create_flag==GFL_BMP_NORMAL){
		GFL_HEAP_FreeMemory( bmp->adrs );
	}
	GFL_HEAP_FreeMemory( bmp );
}

//--------------------------------------------------------------------------------------------
/**
 * BMP�̈搶���iGFL_BMP_DATA�|�C���^�𐶐����āA����ȊO�͈����̂��̂������o�ɑ��
 *
 * @param	adrs	�L�����G���A�ւ̃|�C���^
 * @param	sizex	X�T�C�Y
 * @param	sizey	Y�T�C�Y
 * @param	col		�J���[�R�[�h
 * @param	heapID	�q�[�vID
 *
 * @return	�擾�����������̃A�h���X
 */
//--------------------------------------------------------------------------------------------
GFL_BMP_DATA * GFL_BMP_CreateWithData( u8 *adrs,int sizex, int sizey, int col, HEAPID heapID )
{
	GFL_BMP_DATA * bmp = GFL_HEAP_AllocMemory( heapID, sizeof(GFL_BMP_DATA) );

	bmp->size_x			=	sizex ;
	bmp->size_y			=	sizey ;
	bmp->adrs			=	adrs ;
	bmp->col			=	col ;
	bmp->create_flag	=	GFL_BMP_WITH_DATA;

	return bmp;
}

//--------------------------------------------------------------------------------------------
/*
 * GFL_BMP_DATA�\���̂̃T�C�Y���擾
 *
 * @retval	GFL_BMP_DATA�\���̂̃T�C�Y
 */
//--------------------------------------------------------------------------------------------
int		GFL_BMP_GetGFL_BMP_DATASize( void )
{
	return sizeof(GFL_BMP_DATA);
}


//--------------------------------------------------------------------------------------------
/*
 * �r�b�g�}�b�v�A�h���X���擾
 *
 * @param	bmp		�V�X�e�����[�N�G���A�ւ̃|�C���^
 */
//--------------------------------------------------------------------------------------------
GFL_BMP_DATA	*GFL_BMP_GetBmpAdrs( GFL_BMP_DATA *bmp )
{
	return bmp;
}

//--------------------------------------------------------------------------------------------
/*
 * �r�b�g�}�b�v�L�����G���A�A�h���X���擾
 *
 * @param	bmp		�V�X�e�����[�N�G���A�ւ̃|�C���^
 */
//--------------------------------------------------------------------------------------------
u8	*GFL_BMP_GetCharacterAdrs( GFL_BMP_DATA *bmp )
{
	return bmp->adrs;
}

//--------------------------------------------------------------------------------------------
/*
 * �r�b�g�}�b�v�T�C�YX���擾
 *
 * @param	bmp		�V�X�e�����[�N�G���A�ւ̃|�C���^
 */
//--------------------------------------------------------------------------------------------
u16	GFL_BMP_GetSizeX( GFL_BMP_DATA *bmp )
{
	return bmp->size_x;
}

//--------------------------------------------------------------------------------------------
/*
 * �r�b�g�}�b�v�T�C�YY���擾
 *
 * @param	bmp		�V�X�e�����[�N�G���A�ւ̃|�C���^
 */
//--------------------------------------------------------------------------------------------
u16	GFL_BMP_GetSizeY( GFL_BMP_DATA *bmp )
{
	return bmp->size_y;
}

//--------------------------------------------------------------------------------------------
/**
 * BMP�L�������[�h�iBMP�f�[�^�̌��f�[�^�쐬�j
 *
 * @param	arcID		�L�����̃A�[�J�C�uID
 * @param	datID		�L�����̃A�[�J�C�u���̃f�[�^�C���f�b�N�X
 * @param	compflag	���k�A�񈳏k�t���O
 * @param	heapID		�q�[�v�h�c
 *
 * @return	�擾�����������̃A�h���X
 */
//--------------------------------------------------------------------------------------------
GFL_BMP_DATA * GFL_BMP_LoadCharacter( int arcID, int datID, int compflag, HEAPID heapID )
{
	GFL_BMP_DATA		*bmp = GFL_HEAP_AllocMemory( heapID, sizeof(GFL_BMP_DATA) );
	void				*src;
	NNSG2dCharacterData	*chr;

	src=GFL_ARC_UTIL_Load( arcID, datID, compflag, heapID );
	if( NNS_G2dGetUnpackedBGCharacterData( src, &chr ) == FALSE){
		OS_Panic("GFL_BMP_CharLoad:Faild\n");
		return NULL;
	}

	bmp->size_x			=	chr->W*GFL_BG_1CHRDOTSIZ;
	bmp->size_y			=	chr->H*GFL_BG_1CHRDOTSIZ;
	bmp->adrs			=	GFL_HEAP_AllocMemory( heapID, chr->szByte );
	bmp->create_flag	=	GFL_BMP_NORMAL;
	switch(chr->pixelFmt){ 
	case GX_TEXFMT_PLTT16:
		bmp->col	=	GFL_BMP_16_COLOR;
		break;
	case GX_TEXFMT_PLTT256:
		bmp->col	=	GFL_BMP_256_COLOR;
		break;
	default:
		//G2D�ɂ����ẮA�P�U�F�A�Q�T�U�F�ȊO��Pixel�t�H�[�}�b�g�̓A�T�[�g�ɂ���
		GF_ASSERT(0);
		break;
	}

	GFL_STD_MemCopy32( chr->pRawData, bmp->adrs, chr->szByte );

	GFL_HEAP_FreeMemory( src );

	return bmp;
}

//--------------------------------------------------------------------------------------------
/**
 * �ǂݍ��݂Ə������݂̃A�h���X�Ɣ͈͂��w�肵�ăL������`��(�����F�w�肠��j
 *
 * @param	src			�ǂݍ��݌��L�����f�[�^�w�b�_�[�\���̃|�C���^
 * @param	dest		�������ݐ�L�����f�[�^�w�b�_�[�\���̃|�C���^
 * @param	pos_sx		�ǂݍ��݌��ǂݍ��݊J�nX���W
 * @param	pos_sy		�ǂݍ��݌��ǂݍ��݊J�nY���W
 * @param	pos_dx		�������ݐ揑�����݊J�nX���W
 * @param	pos_dy		�������ݐ揑�����݊J�nY���W
 * @param	size_x		�`��͈�X�T�C�Y
 * @param	size_y		�`��͈�Y�T�C�Y
 * @param	nuki_col	�����F�w��i0�`15 0xff:�����F�w��Ȃ��j
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void GFL_BMP_Print( const GFL_BMP_DATA * src, GFL_BMP_DATA * dest, u16 pos_sx, u16 pos_sy, s16 pos_dx, s16 pos_dy,
			u16 size_x, u16 size_y, u16 nuki_col )
{
	//src��dest�̃J���[���[�h�ɑ��Ⴊ��������A�A�T�[�g�Ŏ~�߂�
	GF_ASSERT(src->col==dest->col);

	if(src->col==GFL_BMP_16_COLOR){
		GFL_BMP_Print16( src, dest, pos_sx, pos_sy, pos_dx, pos_dy, size_x, size_y, nuki_col );
	}
	else{
		GFL_BMP_Print256( src, dest, pos_sx, pos_sy, pos_dx, pos_dy, size_x, size_y, nuki_col );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �͈͂��w�肵�Ďw�肳�ꂽ�F�R�[�h�œh��Ԃ�
 *
 * @param	dest		�������ݐ�L�����f�[�^�w�b�_�[�\���̃|�C���^
 * @param	pos_dx		�������ݐ揑�����݊J�nX���W
 * @param	pos_dy		�������ݐ揑�����݊J�nY���W
 * @param	size_x		�`��͈�X�T�C�Y
 * @param	size_y		�`��͈�Y�T�C�Y
 * @param	col_code	�h��Ԃ��F�R�[�h
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void GFL_BMP_Fill(
		GFL_BMP_DATA * dest,
		s16 pos_dx, s16 pos_dy, u16 size_x, u16 size_y, u8 col_code )
{
	if(dest->col==GFL_BMP_16_COLOR){
		GFL_BMP_Fill16( dest, pos_dx, pos_dy, size_x, size_y, col_code );
	}
	else{
		GFL_BMP_Fill256( dest, pos_dx, pos_dy, size_x, size_y, col_code );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �w�肳�ꂽ�F�R�[�h�œh��Ԃ�
 *
 * @param	dest		�������ݐ�L�����f�[�^�w�b�_�[�\���̃|�C���^
 * @param	col_code	�h��Ԃ��F�R�[�h
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void GFL_BMP_Clear( GFL_BMP_DATA * dest, u8 col_code )
{
	u8	col;

	col=((col_code&0x0f)<<4)|(col_code&0x0f);

	GFL_STD_MemFill( dest->adrs, col, ( dest->size_x / 8 ) * ( dest->size_y / 8 ) * dest->col );
}


//--------------------------------------------------------------------------------------------
/**
 * �ǂݍ��݂Ə������݂̃A�h���X�Ɣ͈͂��w�肵�ăL������`��(�����F�w�肠��j
 *
 * @param	src			�ǂݍ��݌��L�����f�[�^�w�b�_�[�\���̃|�C���^
 * @param	dest		�������ݐ�L�����f�[�^�w�b�_�[�\���̃|�C���^
 * @param	pos_sx		�ǂݍ��݌��ǂݍ��݊J�nX���W
 * @param	pos_sy		�ǂݍ��݌��ǂݍ��݊J�nY���W
 * @param	pos_dx		�������ݐ揑�����݊J�nX���W
 * @param	pos_dy		�������ݐ揑�����݊J�nY���W
 * @param	size_x		�`��͈�X�T�C�Y
 * @param	size_y		�`��͈�Y�T�C�Y
 * @param	nuki_col	�����F�w��i0�`15 0xff:�����F�w��Ȃ��j
 *
 * @return	none
 *
 * @li	�P�U�F�p
 */
//--------------------------------------------------------------------------------------------
#define	NULLPAL_L	(nuki_col)

#define SRC_ADRS	(src->adrs)
#define SRC_POSX	(pos_sx)
#define SRC_POSY	(pos_sy)
#define SRC_SIZX	(src->size_x)
#define SRC_SIZY	(src->size_y)
#define	SRC_XARG	(((SRC_SIZX) + (SRC_SIZX & 7))>>3)

#define DST_ADRS	(dest->adrs)
#define DST_POSX	(pos_dx)
#define DST_POSY	(pos_dy)
#define DST_SIZX	(dest->size_x)
#define DST_SIZY	(dest->size_y)
#define	DST_XARG	(((DST_SIZX) + (DST_SIZX & 7))>>3)

#define WRT_SIZX	(size_x)
#define WRT_SIZY	(size_y)

#define DPPCALC(adrs, pos_x, pos_y, size_x)		\
	(u8*)((adrs) +								\
	((pos_x >> 1) & 0x00000003) +				\
	((pos_x << 2) & 0x00003fe0) +				\
	(((pos_y << 2) & 0x00003fe0) * size_x) +	\
	((u32)((pos_y << 2)&0x0000001c))			\
	)

#define DPPCALC_256(adrs, pos_x, pos_y, size_x)	\
	(u8*)((adrs) +								\
	(pos_x & 0x00000007) +						\
	((pos_x << 3) & 0x00007fc0) +				\
	(((pos_y << 3) & 0x00007fc0) * size_x) +	\
	((u32)((pos_y << 3)&0x00000038))			\
	)

static	void GFL_BMP_Print16( const GFL_BMP_DATA * src, GFL_BMP_DATA * dest, u16 pos_sx, u16 pos_sy, s16 pos_dx, s16 pos_dy,
			u16 size_x, u16 size_y, u16 nuki_col )
{
	int	sx, dx, sy, dy, src_dat, shiftval, x_max, y_max, srcxarg, dstxarg;
	u8	*srcadrs, *dstadrs;

	if((DST_SIZX - DST_POSX) < WRT_SIZX){
		x_max = DST_SIZX - DST_POSX + SRC_POSX;
	}else{
		x_max = WRT_SIZX + SRC_POSX;
	}

	if((DST_SIZY - DST_POSY) < WRT_SIZY){
		y_max = DST_SIZY - DST_POSY + SRC_POSY;
	}else{
		y_max = WRT_SIZY + SRC_POSY;
	}

	srcxarg = SRC_XARG;
	dstxarg = DST_XARG;

	if(nuki_col==GF_BMPPRT_NOTNUKI){	//�����F�w��Ȃ�
		for(sy=SRC_POSY, dy=DST_POSY; sy < y_max; sy++, dy++){
			for(sx=SRC_POSX, dx=DST_POSX; sx < x_max; sx++, dx++){
				if( ( dx >= 0 ) && ( dy >= 0)){

					//�T�C�Y�I�[�o�[����������A�T�[�g�Ŏ~�߂�
					GF_ASSERT( sx < src->size_x);
					GF_ASSERT( sy < src->size_y);
					GF_ASSERT( dx < dest->size_x);
					GF_ASSERT( dy < dest->size_y);

					srcadrs	= DPPCALC(SRC_ADRS, sx, sy, srcxarg);
					dstadrs = DPPCALC(DST_ADRS, dx, dy, dstxarg);
	
					src_dat = (*srcadrs >> ((sx & 1)*4)) & 0x0f;
					shiftval = (dx & 1)*4;
					*dstadrs = (u8)((src_dat << shiftval)|(*dstadrs & (0xf0 >> shiftval)));
				}
			}
		}
	}else{				//�����F�w�肠��
		for(sy=SRC_POSY, dy=DST_POSY; sy < y_max; sy++, dy++){
			for(sx=SRC_POSX, dx=DST_POSX; sx < x_max; sx++, dx++){
				if( ( dx >= 0 ) && ( dy >= 0)){

					//�T�C�Y�I�[�o�[����������A�T�[�g�Ŏ~�߂�
					GF_ASSERT( sx < src->size_x);
					GF_ASSERT( sy < src->size_y);
					GF_ASSERT( dx < dest->size_x);
					GF_ASSERT( dy < dest->size_y);

					srcadrs	= DPPCALC(SRC_ADRS, sx, sy, srcxarg);
					dstadrs = DPPCALC(DST_ADRS, dx, dy, dstxarg);
	
					src_dat = (*srcadrs >> ((sx & 1)*4)) & 0x0f;
	
//					if(src_dat != NULLPAL_L){
					if(src_dat){
						shiftval = (dx & 1)*4;
						*dstadrs = (u8)((src_dat << shiftval)|(*dstadrs & (0xf0 >> shiftval)));
					}
//					}
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �ǂݍ��݂Ə������݂̃A�h���X�Ɣ͈͂��w�肵�ăL������`��(�����F�w�肠��j
 *
 * @param	src			�ǂݍ��݌��L�����f�[�^�w�b�_�[�\���̃|�C���^
 * @param	dest		�������ݐ�L�����f�[�^�w�b�_�[�\���̃|�C���^
 * @param	pos_sx		�ǂݍ��݌��ǂݍ��݊J�nX���W
 * @param	pos_sy		�ǂݍ��݌��ǂݍ��݊J�nY���W
 * @param	pos_dx		�������ݐ揑�����݊J�nX���W
 * @param	pos_dy		�������ݐ揑�����݊J�nY���W
 * @param	size_x		�`��͈�X�T�C�Y
 * @param	size_y		�`��͈�Y�T�C�Y
 * @param	nuki_col	�����F�w��i0�`15 0xff:�����F�w��Ȃ��j
 *
 * @return	none
 *
 * @li	�Q�T�U�F�p
 */
//--------------------------------------------------------------------------------------------
static	void GFL_BMP_Print256( const GFL_BMP_DATA * src, GFL_BMP_DATA * dest, u16 pos_sx, u16 pos_sy, s16 pos_dx, s16 pos_dy,
		u16 size_x, u16 size_y, u16 nuki_col )
{
	int	sx, dx, sy, dy, x_max, y_max, srcxarg, dstxarg;
	u8 * srcadrs;
	u8 * dstadrs;

	if( (DST_SIZX - DST_POSX) < WRT_SIZX ){
		x_max = DST_SIZX - DST_POSX + SRC_POSX;
	}else{
		x_max = WRT_SIZX + SRC_POSX;
	}

	if( (DST_SIZY - DST_POSY) < WRT_SIZY ){
		y_max = DST_SIZY - DST_POSY + SRC_POSY;
	}else{
		y_max = WRT_SIZY + SRC_POSY;
	}
	srcxarg = SRC_XARG;
	dstxarg = DST_XARG;

	if( nuki_col == GF_BMPPRT_NOTNUKI ){	//�����F�w��Ȃ�
		for( sy=SRC_POSY, dy=DST_POSY; sy<y_max; sy++, dy++ ){
			for( sx=SRC_POSX, dx=DST_POSX; sx<x_max; sx++, dx++ ){
				if( ( dx >= 0 ) && ( dy >= 0 ) ){

					//�T�C�Y�I�[�o�[����������A�T�[�g�Ŏ~�߂�
					GF_ASSERT( sx < src->size_x);
					GF_ASSERT( sy < src->size_y);
					GF_ASSERT( dx < dest->size_x);
					GF_ASSERT( dy < dest->size_y);

					srcadrs	= DPPCALC_256( SRC_ADRS, sx, sy, srcxarg );
					dstadrs = DPPCALC_256( DST_ADRS, dx, dy, dstxarg );
					*dstadrs = *srcadrs;
				}
			}
		}
	}else{				//�����F�w�肠��
		for( sy=SRC_POSY, dy=DST_POSY; sy<y_max; sy++, dy++ ){
			for( sx=SRC_POSX, dx=DST_POSX; sx<x_max; sx++, dx++ ){
				if( ( dx >= 0 ) && ( dy >= 0 ) ){

					//�T�C�Y�I�[�o�[����������A�T�[�g�Ŏ~�߂�
					GF_ASSERT( sx < src->size_x);
					GF_ASSERT( sy < src->size_y);
					GF_ASSERT( dx < dest->size_x);
					GF_ASSERT( dy < dest->size_y);

					srcadrs	= DPPCALC_256( SRC_ADRS, sx, sy, srcxarg );
					dstadrs = DPPCALC_256( DST_ADRS, dx, dy, dstxarg );
					if( *srcadrs != NULLPAL_L ){
						*dstadrs = *srcadrs;
					}
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �͈͂��w�肵�Ďw�肳�ꂽ�F�R�[�h�œh��Ԃ�
 *
 * @param	dest		�������ݐ�L�����f�[�^�w�b�_�[�\���̃|�C���^
 * @param	pos_dx		�������ݐ揑�����݊J�nX���W
 * @param	pos_dy		�������ݐ揑�����݊J�nY���W
 * @param	size_x		�`��͈�X�T�C�Y
 * @param	size_y		�`��͈�Y�T�C�Y
 * @param	col_code	�h��Ԃ��F�R�[�h
 *
 * @return	none
 *
 * @li	�P�U�F�p
 */
//--------------------------------------------------------------------------------------------
static	void GFL_BMP_Fill16( GFL_BMP_DATA * dest, s16 pos_dx, s16 pos_dy, u16 size_x, u16 size_y, u8 col_code )
{
	int	x,y,x_max,y_max,xarg;
	u8	*destadrs;

	x_max = pos_dx + size_x;
	if(x_max > dest->size_x){
		x_max = dest->size_x;
	}

	y_max = pos_dy + size_y;
	if(y_max > dest->size_y){
		y_max = dest->size_y;
	}

	xarg = (((dest->size_x) + (dest->size_x&7))>>3);

	for(y = pos_dy; y < y_max; y++){
		for(x = pos_dx; x < x_max; x++){
			if( ( x >= 0 ) && ( y >= 0 ) ){

				//�T�C�Y�I�[�o�[����������A�T�[�g�Ŏ~�߂�
				GF_ASSERT( x < dest->size_x);
				GF_ASSERT( y < dest->size_y);

				destadrs=DPPCALC(dest->adrs, x, y, xarg);
				if(x&1){
					*destadrs&=0x0f;
					*destadrs|=(col_code<<4);
				}
				else{
					*destadrs&=0xf0;
					*destadrs|=col_code;
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �͈͂��w�肵�Ďw�肳�ꂽ�F�R�[�h�œh��Ԃ�
 *
 * @param	dest		�������ݐ�L�����f�[�^�w�b�_�[�\���̃|�C���^
 * @param	pos_dx		�������ݐ揑�����݊J�nX���W
 * @param	pos_dy		�������ݐ揑�����݊J�nY���W
 * @param	size_x		�`��͈�X�T�C�Y
 * @param	size_y		�`��͈�Y�T�C�Y
 * @param	col_code	�h��Ԃ��F�R�[�h
 *
 * @return	none
 *
 * @li	�Q�T�U�F�p
 */
//--------------------------------------------------------------------------------------------
static	void GFL_BMP_Fill256( GFL_BMP_DATA * dest, s16 pos_dx, s16 pos_dy, u16 size_x, u16 size_y, u8 col_code )
{
	int	x,y,x_max,y_max,xarg;
	u8	*destadrs;

	x_max = pos_dx + size_x;
	if( x_max > dest->size_x ){
		x_max = dest->size_x;
	}

	y_max = pos_dy + size_y;
	if( y_max > dest->size_y ){
		y_max = dest->size_y;
	}

	xarg = ( dest->size_x + (dest->size_x & 7) ) >> 3;

	for( y=pos_dy; y<y_max; y++ ){
		for( x=pos_dx; x<x_max; x++ ){
			if( ( x >= 0 ) && ( y >= 0 ) ){

				//�T�C�Y�I�[�o�[����������A�T�[�g�Ŏ~�߂�
				GF_ASSERT( x < dest->size_x);
				GF_ASSERT( y < dest->size_y);

				destadrs = DPPCALC_256( dest->adrs, x, y, xarg );
				*destadrs = col_code;
			}
		}
	}
}

