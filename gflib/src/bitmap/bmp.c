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

#define	__BMP_H_GLOBAL__
#include "bmp.h"
#include "bg_sys.h"


//=============================================================================================
//=============================================================================================
//	�r�b�g�}�b�v�֘A
//=============================================================================================
//=============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * BMP������
 *
 * @param	sizex	X�T�C�Y
 * @param	sizey	Y�T�C�Y
 * @param	col		�J���[���[�h�iGFL_BMP_16_COLOR:16�F�@GFL_BMP_256_COLOR:256�F�j
 * @param	heapID	�q�[�v�h�c
 *
 * @return	�擾�����������̃A�h���X
 */
//--------------------------------------------------------------------------------------------
GFL_BMP_DATA * GFL_BMP_sysInit( int sizex, int sizey, int col, HEAPID heapID )
{
	GFL_BMP_DATA * bmp = GFL_HEAP_AllocMemory( heapID, sizeof(GFL_BMP_DATA) );

	bmp->size_x	=	sizex;
	bmp->size_y	=	sizey;
	bmp->adrs	=	GFL_HEAP_AllocMemoryClear( heapID, sizex * sizey * col );

	return bmp;
}

//--------------------------------------------------------------------------------------------
/**
 * �V�X�e�����[�N�G���A�J��
 *
 * @param	bgl		�V�X�e�����[�N�G���A�ւ̃|�C���^
 */
//--------------------------------------------------------------------------------------------
void	GFL_BMP_sysExit( GFL_BMP_DATA *bmp )
{
	GFL_HEAP_FreeMemory( bmp->adrs );
	GFL_HEAP_FreeMemory( bmp );
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
GFL_BMP_DATA * GFL_BMP_CharLoad( int arcID, int datID, int compflag, HEAPID heapID )
{
	GFL_BMP_DATA		*bmp = GFL_HEAP_AllocMemory( heapID, sizeof(GFL_BMP_DATA) );
	void				*src;
	NNSG2dCharacterData	*chr;

	src=GFL_ARC_UtilLoad( arcID, datID, compflag, heapID );
	if( NNS_G2dGetUnpackedBGCharacterData( src, &chr ) == FALSE){
		OS_Panic("GFL_BMP_CharLoad:Faild\n");
		return NULL;
	}

	bmp->size_x	=	chr->W*GFL_BG_1CHRDOTSIZ;
	bmp->size_y	=	chr->H*GFL_BG_1CHRDOTSIZ;
	bmp->adrs	=	GFL_HEAP_AllocMemory( heapID, chr->szByte );
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

#if 0
#define DPPCALC(adrs, pos_x, pos_y, size_x)		\
	(u8*)((adrs) +								\
	(((pos_x)>>1) & 3) +						\
	(((pos_x)>>3) << 5) +						\
	((((pos_y)>>3) * (size_x)) << 5) +			\
	((u32)((pos_y)<<29)>>27)					\
	)

#define DPPCALC_256(adrs, pos_x, pos_y, size_x)	\
	(u8*)((adrs) +								\
	(pos_x & 7) +								\
	((pos_x>>3) << 6) +							\
	(((pos_y>>3) * size_x) << 6) +				\
	((u32)((pos_y)<<29)>>26)					\
	)
#else
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
#endif


void GFL_BMP_PrintMain(
			const GFL_BMP_DATA * src, const GFL_BMP_DATA * dest,
			u16 pos_sx, u16 pos_sy, u16 pos_dx, u16 pos_dy,
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
				srcadrs	= DPPCALC(SRC_ADRS, sx, sy, srcxarg);
				dstadrs = DPPCALC(DST_ADRS, dx, dy, dstxarg);
	
				src_dat = (*srcadrs >> ((sx & 1)*4)) & 0x0f;
				shiftval = (dx & 1)*4;
				*dstadrs = (u8)((src_dat << shiftval)|(*dstadrs & (0xf0 >> shiftval)));
			}
		}
	}else{				//�����F�w�肠��
		for(sy=SRC_POSY, dy=DST_POSY; sy < y_max; sy++, dy++){
			for(sx=SRC_POSX, dx=DST_POSX; sx < x_max; sx++, dx++){
				srcadrs	= DPPCALC(SRC_ADRS, sx, sy, srcxarg);
				dstadrs = DPPCALC(DST_ADRS, dx, dy, dstxarg);
	
				src_dat = (*srcadrs >> ((sx & 1)*4)) & 0x0f;
	
//				if(src_dat != NULLPAL_L){
				if(src_dat){
					shiftval = (dx & 1)*4;
					*dstadrs = (u8)((src_dat << shiftval)|(*dstadrs & (0xf0 >> shiftval)));
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
void GFL_BMP_PrintMain256(
		const GFL_BMP_DATA * src, const GFL_BMP_DATA * dest,
		u16 pos_sx, u16 pos_sy, u16 pos_dx, u16 pos_dy,
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
				srcadrs	= DPPCALC_256( SRC_ADRS, sx, sy, srcxarg );
				dstadrs = DPPCALC_256( DST_ADRS, dx, dy, dstxarg );
				*dstadrs = *srcadrs;
			}
		}
	}else{				//�����F�w�肠��
		for( sy=SRC_POSY, dy=DST_POSY; sy<y_max; sy++, dy++ ){
			for( sx=SRC_POSX, dx=DST_POSX; sx<x_max; sx++, dx++ ){
				srcadrs	= DPPCALC_256( SRC_ADRS, sx, sy, srcxarg );
				dstadrs = DPPCALC_256( DST_ADRS, dx, dy, dstxarg );
				if( *srcadrs != NULLPAL_L ){
					*dstadrs = *srcadrs;
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
void GFL_BMP_Fill(
		const GFL_BMP_DATA * dest,
		u16 pos_dx, u16 pos_dy, u16 size_x, u16 size_y, u8 col_code )
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
void GFL_BMP_Fill256(
		const GFL_BMP_DATA * dest,
		u16 pos_dx, u16 pos_dy, u16 size_x, u16 size_y, u8 col_code )
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
			destadrs = DPPCALC_256( dest->adrs, x, y, xarg );
			*destadrs = col_code;
		}
	}
}
