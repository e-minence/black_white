//=============================================================================================
/**
 * @file	bmp.h
 * @brief	BG�`��V�X�e���v���O����
 * @author	Hiroyuki Nakamura
 * @date	2006/10/18
 */
//=============================================================================================
#ifndef _BMP_H_
#define _BMP_H_

#undef GLOBAL
#ifdef __BMP_H_GLOBAL__
#define GLOBAL /*	*/
#else
#define GLOBAL extern
#endif


//--------------------------------------------------------------------
//�r�b�g�}�b�v�`��f�[�^�\����
typedef struct{
	const u8 * adrs;	///<�L�����N�^�f�[�^�̐擪�A�h���X
	u16	size_x;			///<�L�����N�^�f�[�^��X�T�C�Y
	u16	size_y;			///<�L�����N�^�f�[�^��Y�T�C�Y
}BMPPRT_HEADER;


//�r�b�g�}�b�v�������ݗp��`
#define	GF_BMPPRT_NOTNUKI	( 0xffff )	// �����F�w��Ȃ�

// �����r�b�g�}�b�v�C���f�b�N�X�l
#define	GF_BITMAP_NULL		(0xff)

// BMP�������݃}�N��
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
GLOBAL void GF_BMP_PrintMain(
			const BMPPRT_HEADER * src, const BMPPRT_HEADER * dest,
			u16 pos_sx, u16 pos_sy, u16 pos_dx, u16 pos_dy,
			u16 size_x, u16 size_y, u16 nuki_col );

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
GLOBAL void GF_BMP_PrintMain256(
		const BMPPRT_HEADER * src, const BMPPRT_HEADER * dest,
		u16 pos_sx, u16 pos_sy, u16 pos_dx, u16 pos_dy,
		u16 size_x, u16 size_y, u16 nuki_col );

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
GLOBAL void GF_BGL_BmpFill(
		const BMPPRT_HEADER * dest,
		u16 pos_dx, u16 pos_dy, u16 size_x, u16 size_y, u8 col_code );

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
GLOBAL void GF_BGL_BmpFill256(
		const BMPPRT_HEADER * dest,
		u16 pos_dx, u16 pos_dy, u16 size_x, u16 size_y, u8 col_code );


#undef GLOBAL
#endif
