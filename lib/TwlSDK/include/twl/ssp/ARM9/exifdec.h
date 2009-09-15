/*---------------------------------------------------------------------------*
  Project:  TwlSDK - SSP - include
  File:     exifdec.h

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

#ifndef TWL_SSP_EXIFDEC_H_
#define TWL_SSP_EXIFDEC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <twl/types.h>

/*---------------------------------------------------------------------------*
  Name:         SSP_GetJpegDecoderDateTime

  Description:  JPEG�̃f�R�[�h���̓��������擾����

  Arguments:    buffer : ������������o�b�t�@

  Returns:      �T�C�Y
 *---------------------------------------------------------------------------*/
u16 SSP_GetJpegDecoderDateTime(u8* buffer);

/*---------------------------------------------------------------------------*
  Name:         SSP_GetJpegDecoderSoftware

  Description:  JPEG�� Software �^�O�ɓo�^����Ă���^�C�g��ID�̉���4�o�C�g���擾����

  Arguments:    buffer : Software ��������o�b�t�@

  Returns:      �T�C�Y
 *---------------------------------------------------------------------------*/
u16 SSP_GetJpegDecoderSoftware(char* buffer);

/*---------------------------------------------------------------------------*
  Name:         SSP_GetJpegDecoderMakerNoteAddrEx

  Description:  JPEG�̃f�R�[�h���̃��[�J�[�m�[�g�̐擪�A�h���X���擾����

  Arguments:    tag : �擾����f�[�^�̎��.

  Returns:      �A�h���X
 *---------------------------------------------------------------------------*/
u8* SSP_GetJpegDecoderMakerNoteAddrEx(SSPJpegMakernote tag);

/*---------------------------------------------------------------------------*
  Name:         SSP_GetJpegDecoderMakerNoteSizeEx

  Description:  JPEG�̃f�R�[�h���̃��[�J�[�m�[�g�̃T�C�Y���擾����

  Arguments:    tag : �擾����f�[�^�̎��.

  Returns:      �T�C�Y
 *---------------------------------------------------------------------------*/
u16 SSP_GetJpegDecoderMakerNoteSizeEx(SSPJpegMakernote tag);

/*---------------------------------------------------------------------------*
  Name:         SSP_GetJpegDecoderMakerNoteAddr

  Description:  JPEG�̃f�R�[�h���̃��[�J�[�m�[�g��DSi�J�����p�f�[�^�̐擪�A�h���X���擾����

  Arguments:    None.

  Returns:      �A�h���X
 *---------------------------------------------------------------------------*/
static inline u8* SSP_GetJpegDecoderMakerNoteAddr(void)
{
    return SSP_GetJpegDecoderMakerNoteAddrEx(SSP_MAKERNOTE_PHOTO);
}

/*---------------------------------------------------------------------------*
  Name:         SSP_GetJpegDecoderMakerNoteSize

  Description:  JPEG�̃f�R�[�h���̃��[�J�[�m�[�g��DSi�J�����p�f�[�^�̃T�C�Y���擾����

  Arguments:    None.

  Returns:      �T�C�Y
 *---------------------------------------------------------------------------*/
static inline u16 SSP_GetJpegDecoderMakerNoteSize(void)
{
    return SSP_GetJpegDecoderMakerNoteSizeEx(SSP_MAKERNOTE_PHOTO);
}

/*---------------------------------------------------------------------------*
  Name:         SSP_SetJpegDecoderSignMode

  Description:  JPEG�̃f�R�[�h���ɏ������`�F�b�N����@�\�̗L��/������؂�ւ���

  Arguments:    TRUE �Ȃ� ON �ɁAFALSE �Ȃ� OFF �ɂ���

  Returns:      �ȑO�ɐݒ肳��Ă������[�h��Ԃ�
 *---------------------------------------------------------------------------*/
BOOL SSP_SetJpegDecoderSignMode(BOOL mode);

s32 SSP_ExifDecode(u8* src, u32 src_size, int* cur, int option);

#ifdef __cplusplus
} /* extern "C" */
#endif

/* TWL_SSP_EXIFDEC_H_ */
#endif
