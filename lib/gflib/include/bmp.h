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

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------------------------------------
//�r�b�g�}�b�v�`��f�[�^�\����
typedef struct	_GFL_BMP_DATA	GFL_BMP_DATA;


#define	GFL_BMP_16_COLOR	(0x20)
#define	GFL_BMP_256_COLOR	(0x40)

// VRAM�m�ە�����`
#define GFL_BMP_CHRAREA_GET_F	(GFL_BG_CHRAREA_GET_F)		//VRAM�O���m��
#define GFL_BMP_CHRAREA_GET_B	(GFL_BG_CHRAREA_GET_B)		//VRAM����m��

//�r�b�g�}�b�v�������ݗp��`
#define	GF_BMPPRT_NOTNUKI	( 0xffff )	// �����F�w��Ȃ�

//--------------------------------------------------------------------------------------------
/**
 * Bitmap�����i�ʏ�ŁF�q�[�v��ɕK�v�ȗ̈���m�ۂ���j
 *
 * @param	sizex	X�T�C�Y�i�L�����P�ʁj
 * @param	sizey	Y�T�C�Y�i�L�����P�ʁj
 * @param	col		�J���[���[�h
 * @param	heapID	�q�[�v�h�c
 *
 * @return	�������ꂽBitmap�n���h��
 */
//--------------------------------------------------------------------------------------------
extern	GFL_BMP_DATA * GFL_BMP_Create( int sizex, int sizey, int col, HEAPID heapID );

//--------------------------------------------------------------------------------------------
/**
 * Bitmap�����i���Ɋm�ۂ���Ă���L�����N�^�f�[�^�̈��Bitmap�Ƃ��ė��p����j
 *
 * @param	adrs	���p����L�����N�^�f�[�^�̈�A�h���X
 * @param	sizex	X�T�C�Y�i�L�����P�ʁj
 * @param	sizey	Y�T�C�Y�i�L�����P�ʁj
 * @param	col		�J���[�R�[�h
 * @param	heapID	�q�[�vID
 *
 * @return	�������ꂽBitmap�n���h��
 */
//--------------------------------------------------------------------------------------------
extern	GFL_BMP_DATA * GFL_BMP_CreateWithData( u8 *adrs,int sizex, int sizey, int col, HEAPID heapID );

//--------------------------------------------------------------------------------------------
/**
 * Bitmap�����iVRAM�̈�𒼐�,Bitmap�Ƃ��ė��p����j
 *
 * @param	adrs	���p����VRAM�A�h���X
 * @param	sizex	X�T�C�Y�i�L�����P�ʁj
 * @param	sizey	Y�T�C�Y�i�L�����P�ʁj
 * @param	col		�J���[�R�[�h
 * @param	heapID	�q�[�vID
 *
 * @return	�������ꂽBitmap�n���h��
 */
//--------------------------------------------------------------------------------------------
extern	GFL_BMP_DATA * GFL_BMP_CreateInVRAM( u8 *adrs,int sizex, int sizey, int col, HEAPID heapID );

//--------------------------------------------------------------------------------------------
/**
 * BMP�̈�J��
 *
 * @param	bmp		BMP�̈�ւ̃|�C���^
 */
//--------------------------------------------------------------------------------------------
extern	void	GFL_BMP_Delete( GFL_BMP_DATA *bmp );

//--------------------------------------------------------------------------------------------
/*
 * GFL_BMP_DATA�\���̂̃T�C�Y���擾
 *
 * @retval	GFL_BMP_DATA�\���̂̃T�C�Y
 */
//--------------------------------------------------------------------------------------------
extern	int		GFL_BMP_GetGFL_BMP_DATASize( void );

//--------------------------------------------------------------------------------------------
/*
 * �r�b�g�}�b�v�L�����G���A�A�h���X���擾
 *
 * @param	bmp		BMP�̈�ւ̃|�C���^
 */
//--------------------------------------------------------------------------------------------
extern	u8	*GFL_BMP_GetCharacterAdrs( GFL_BMP_DATA *bmp );

//--------------------------------------------------------------------------------------------
/*
 * �r�b�g�}�b�v�T�C�YX�i�h�b�g�P�ʁj���擾
 *
 * @param	bmp		BMP�̈�ւ̃|�C���^
 */
//--------------------------------------------------------------------------------------------
extern	u16	GFL_BMP_GetSizeX( GFL_BMP_DATA *bmp );

//--------------------------------------------------------------------------------------------
/*
 * �r�b�g�}�b�v�T�C�YY�i�h�b�g�P�ʁj���擾
 *
 * @param	bmp		BMP�̈�ւ̃|�C���^
 */
//--------------------------------------------------------------------------------------------
extern	u16	GFL_BMP_GetSizeY( GFL_BMP_DATA *bmp );

//--------------------------------------------------------------------------------------------
/*
 * �r�b�g�}�b�v���f�[�^�T�C�Y���擾
 *
 * @param	bmp		�V�X�e�����[�N�G���A�ւ̃|�C���^
 */
//--------------------------------------------------------------------------------------------
extern u32	GFL_BMP_GetBmpDataSize( const GFL_BMP_DATA *bmp );

//--------------------------------------------------------------------------------------------
/*
 * �r�b�g�}�b�v�f�[�^�̃R�s�[
 *
 * @param	src		�R�s�[��
 * @param	dst		�R�s�[��
 */
//--------------------------------------------------------------------------------------------
extern void GFL_BMP_Copy( const GFL_BMP_DATA *src, GFL_BMP_DATA *dst );

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
extern	GFL_BMP_DATA * GFL_BMP_LoadCharacter( int arcID, int datID, int compflag, HEAPID heapID );

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
 * @param	nuki_col	�����F�w��i0�`15 GF_BMPPRT_NOTNUKI:�����F�w��Ȃ��j
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void GFL_BMP_Print( const GFL_BMP_DATA * src, GFL_BMP_DATA * dest, u16 pos_sx, u16 pos_sy, s16 pos_dx, s16 pos_dy,
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
 */
//--------------------------------------------------------------------------------------------
extern void GFL_BMP_Fill( GFL_BMP_DATA * dest, s16 pos_dx, s16 pos_dy, u16 size_x, u16 size_y, u8 col_code );

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
extern	void GFL_BMP_Clear( GFL_BMP_DATA * dest, u8 col_code );

//--------------------------------------------------------------------------------------------
/**
 * �f�[�^�R���o�[�g�iChr�f�[�^�t�H�[�}�b�g��BMP�f�[�^�t�H�[�}�b�g�j
 *
 * @param	src			�ǂݍ��݌��L�����f�[�^�w�b�_�[�\���̃|�C���^
 * @param	mode		�V�K�Ƀt�@�C���p���������A���P�[�g���邩�ǂ���
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern GFL_BMP_DATA * GFL_BMP_DataConv_to_BMPformat( GFL_BMP_DATA * src, BOOL mode, HEAPID heapID );

#ifdef __cplusplus
}/* extern "C" */
#endif

#endif
