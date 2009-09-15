/*---------------------------------------------------------------------------*
  Project:  TwlSDK - SSP - include
  File:     jpegdec.h

  Copyright 2007-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-04#$
  $Rev: 10698 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#ifndef TWL_SSP_JPEGDEC_H_
#define TWL_SSP_JPEGDEC_H_

#include <twl/ssp/common/ssp_jpeg_type.h>
#include <twl/ssp/ARM9/exifdec.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <twl/types.h>

/*---------------------------------------------------------------------------*
  Name:         SSP_CheckJpegDecoderSign

  Description:  JPEG�t�@�C���̏����`�F�b�N�݂̂��s��

  Arguments:    data   : JPEG�t�@�C��
                size   : JPEG�t�@�C���̃T�C�Y

  Returns:      �����Ȃ��TRUE��Ԃ�
 *---------------------------------------------------------------------------*/
BOOL SSP_CheckJpegDecoderSign(u8* data, u32 size);

/*---------------------------------------------------------------------------*
  Name:         SSP_StartJpegDecoder

  Description:  JPEG�t�@�C�����f�R�[�h����

  Arguments:    data   : JPEG�t�@�C��
                size   : JPEG�t�@�C���̃T�C�Y
                dst    : �f�R�[�h��o�b�t�@ (����RGB555�̂�)
                width  : �����Ƃ��ĉ摜���T�C�Y�̍ő�l��^���܂��B
                         �f�R�[�h�������ɂ̓f�R�[�h�����摜�̉��T�C�Y���Ԃ�܂��B
                height : �����Ƃ��ĉ摜�c�T�C�Y�̍ő�l��^���܂��B
                         �f�R�[�h�������ɂ̓f�R�[�h�����摜�̏c�T�C�Y���Ԃ�܂��B
                option : �I�v�V����(�r�b�g�_���a)
                         SSP_JPEG_RGB555: RGB555�`���Ńf�R�[�h�B
                                ���͎w�肵�Ȃ��Ă�RGB555�`���Ńf�R�[�h���܂��B
                         SSP_JPEG_THUMBNAIL: �T���l�C���̃f�R�[�h�B���w�莞�͎�摜�̃f�R�[�h�B
                         SSP_JPEG_EXIF: EXIF���̂ݎ擾����

  Returns:      �����Ȃ��TRUE��Ԃ�
 *---------------------------------------------------------------------------*/
BOOL SSP_StartJpegDecoder(u8* data, u32 size, void* dst, s16* width, s16* height, u32 option);

/*---------------------------------------------------------------------------*
  Name:         SSP_StartJpegDecoderEx

  Description:  �����@�\�̗L�����w�肵��JPEG�t�@�C�����f�R�[�h����

  Arguments:    data   : JPEG�t�@�C��
                size   : JPEG�t�@�C���̃T�C�Y
                dst    : �f�R�[�h��o�b�t�@ (����RGB555�̂�)
                width  : �����Ƃ��ĉ摜���T�C�Y�̍ő�l��^���܂��B
                         �f�R�[�h�������ɂ̓f�R�[�h�����摜�̉��T�C�Y���Ԃ�܂��B
                height : �����Ƃ��ĉ摜�c�T�C�Y�̍ő�l��^���܂��B
                         �f�R�[�h�������ɂ̓f�R�[�h�����摜�̏c�T�C�Y���Ԃ�܂��B
                option : �I�v�V����(�r�b�g�_���a)
                         SSP_JPEG_RGB555: RGB555�`���Ńf�R�[�h�B
                                ���͎w�肵�Ȃ��Ă�RGB555�`���Ńf�R�[�h���܂��B
                         SSP_JPEG_THUMBNAIL: �T���l�C���̃f�R�[�h�B���w�莞�͎�摜�̃f�R�[�h�B
                         SSP_JPEG_EXIF: EXIF���̂ݎ擾����
                sign   : �����`�F�b�N���s���ꍇ�� TRUE ���w�肷��B

  Returns:      �����Ȃ��TRUE��Ԃ�
 *---------------------------------------------------------------------------*/
static inline BOOL SSP_StartJpegDecoderEx(u8* data, u32 size, void* dst, s16* width, s16* height, u32 option, BOOL sign)
{
    BOOL result;
    
    (void)SSP_SetJpegDecoderSignMode(sign);
    result = SSP_StartJpegDecoder(data, size, dst, width, height, option);
    (void)SSP_SetJpegDecoderSignMode(FALSE);
    
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         SSP_ExtractJpegDecoderExif

  Description:  JPEG�t�@�C������Exif��񂾂��𒊏o����
                ���ۂɒ��o���ꂽ�l�͊e�� Get �֐��Ŏ擾���邱��

  Arguments:    data   : JPEG�t�@�C��
                size   : JPEG�t�@�C���̃T�C�Y
                
  Returns:      �����Ȃ��TRUE��Ԃ�
 *---------------------------------------------------------------------------*/
static inline BOOL SSP_ExtractJpegDecoderExif(u8* data, u32 size)
{
    return SSP_StartJpegDecoder(data, size, 0, 0, 0, SSP_JPEG_EXIF);
}

#ifdef __cplusplus
} /* extern "C" */
#endif

/* TWL_SSP_JPEGDEC_H_ */
#endif
