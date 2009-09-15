/*---------------------------------------------------------------------------*
  Project:  TwlSDK - SSP - include
  File:     exifenc.h

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

#ifndef TWL_SSP_EXIFENC_H_
#define TWL_SSP_EXIFENC_H_

#include <twl/ssp/common/ssp_jpeg_type.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <twl/types.h>

/*---------------------------------------------------------------------------*
  Name:         SSP_SetJpegEncoderMakerNoteEx

  Description:  JPEG�̃G���R�[�h���Ƀ��[�J�[�m�[�g�ɒǉ�����f�[�^

  Arguments:    tag    : ���ߍ��ރf�[�^�̎��
                buffer : �f�[�^�̃o�b�t�@
                size   : �f�[�^�̃T�C�Y

  Returns:      None.
 *---------------------------------------------------------------------------*/
void SSP_SetJpegEncoderMakerNoteEx(SSPJpegMakernote tag, const u8* buffer, u32 size);

/*---------------------------------------------------------------------------*
  Name:         SSP_SetJpegEncoderMakerNote

  Description:  JPEG�̃G���R�[�h���Ƀ��[�J�[�m�[�g�ɒǉ�����DSi�J�����p�̃f�[�^

  Arguments:    buffer : DSi�J�����p�̃f�[�^�̃o�b�t�@
                size   : �f�[�^�̃T�C�Y

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void SSP_SetJpegEncoderMakerNote(const u8* buffer, u32 size)
{
    SSP_SetJpegEncoderMakerNoteEx(SSP_MAKERNOTE_PHOTO, buffer, size);
}

/*---------------------------------------------------------------------------*
  Name:         SSP_SetJpegEncoderDateTime

  Description:  JPEG�̃G���R�[�h���� Exif IFD �� DateTimeOriginal ��
                DateTimeDigitized �^�O�ɐݒ肷��������w�肷��

  Arguments:    buffer : YYYY:MM:DD HH:MM:SS �`���̓���������������

  Returns:      None.
 *---------------------------------------------------------------------------*/
void SSP_SetJpegEncoderDateTime(const u8* buffer);

/*---------------------------------------------------------------------------*
  Name:         SSP_GetDateTime

  Description:  ������ RTC_GetDateTime �֐����Ăяo�������̊֐�

  Arguments:    None.

  Returns:      ���������Ȃ�� TRUE ��Ԃ�.
 *---------------------------------------------------------------------------*/
BOOL SSP_GetDateTime( RTCDate* date , RTCTime* time );

/*---------------------------------------------------------------------------*
  Name:         SSP_SetJpegEncoderDateTimeNow

  Description:  JPEG�̃G���R�[�h���� Exif IFD �� DateTimeOriginal ��
                DateTimeDigitized �^�O�Ɍ��݂̎�����ݒ肷��

  Arguments:    None.

  Returns:      ���ݎ����̐ݒ�ɐ��������� TRUE.
 *---------------------------------------------------------------------------*/
BOOL SSP_SetJpegEncoderDateTimeNow(void);

/*---------------------------------------------------------------------------*
  Name:         SSP_SetJpegEncoderSignMode

  Description:  JPEG�̃G���R�[�h���ɏ�����t������@�\�̗L��/������؂�ւ���

  Arguments:    TRUE �Ȃ� ON �ɁAFALSE �Ȃ� OFF �ɂ���

  Returns:      �ȑO�ɐݒ肳��Ă������[�h��Ԃ�
 *---------------------------------------------------------------------------*/
BOOL SSP_SetJpegEncoderSignMode(BOOL mode);

u32 SSP_ExifEncode(u8* l_dst, u32 width, u32 height, const u8* thumb_src, u32 thumb_size);

#ifdef __cplusplus
} /* extern "C" */
#endif

/* TWL_SSP_EXIFENC_H_ */
#endif
